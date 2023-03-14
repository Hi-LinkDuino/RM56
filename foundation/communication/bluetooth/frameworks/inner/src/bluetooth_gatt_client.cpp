/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <condition_variable>
#include <memory>
#include <set>

#include "bluetooth_def.h"
#include "bluetooth_gatt_client.h"
#include "bluetooth_gatt_client_proxy.h"
#include "bluetooth_gatt_client_callback_stub.h"
#include "bluetooth_host.h"
#include "bluetooth_host_proxy.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "gatt_data.h"
#include "hilog/log.h"
#include "i_bluetooth_gatt_client.h"
#include "iservice_registry.h"
#include "raw_address.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
constexpr uint8_t REQUEST_TYPE_CHARACTERISTICS_READ = 0x00;
constexpr uint8_t REQUEST_TYPE_CHARACTERISTICS_WRITE = 0x01;
constexpr uint8_t REQUEST_TYPE_DESCRIPTOR_READ = 0x02;
constexpr uint8_t REQUEST_TYPE_DESCRIPTOR_WRITE = 0x03;
constexpr uint8_t REQUEST_TYPE_SET_NOTIFY_CHARACTERISTICS = 0x04;

struct DiscoverInfomation {
    struct Characteristics {
        bool isDiscoverDescCompleted_;
        Characteristics() : isDiscoverDescCompleted_(false)
        {}
    };

    struct Service {
        bool isDiscoverCompleted_;
        bool isDiscoverCharacteristicCompleted_;
        bool isDiscoverIncludeSvcCompleted_;
        uint16_t endHandle_;
        std::map<uint16_t, Characteristics> characteristics_;
        Service(uint16_t endHandle)
            : isDiscoverCompleted_(false),
              isDiscoverCharacteristicCompleted_(false),
              isDiscoverIncludeSvcCompleted_(false),
              endHandle_(endHandle)
        {}
    };
    bool isDiscovering_;
    bool needNotify_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::map<uint16_t, Service> service_;
    DiscoverInfomation() : isDiscovering_(false), needNotify_(false)
    {}
};

struct RequestInformation {
    bool doing_;
    uint8_t type_;
    union {
        GattCharacteristic *characteristic_;
        GattDescriptor *descriptor_;
    } context_;
    std::mutex mutex_;
    RequestInformation() : doing_(false)
    {}
};

struct GattClient::impl {
    class BluetoothGattClientCallbackStubImpl;

    bool isGetServiceYet_;
    bool isRegisterSucceeded_;
    GattClientCallback *callback_;
    int applicationId_;
    int connectionState_;
    BluetoothRemoteDevice device_;
    sptr<IBluetoothGattClient> proxy_;
    sptr<BluetoothGattClientCallbackStubImpl> clientCallback_;
    std::vector<GattService> gattServices_;
    std::mutex connStateMutex_;
    RequestInformation requestInformation_;
    DiscoverInfomation discoverInformation_;

    impl(GattClient &client, const BluetoothRemoteDevice &device);
    ~impl();

    int DiscoverStart();
    void DiscoverComplete(int state);
    void BuildServiceList(const std::vector<BluetoothGattService> &src);
    GattService *FindService(uint16_t handle);
    void GetServices();
    int GetTransport(int &transport);
    void CleanConnectionInfo();

    class GattClientDeathRecipient;
    sptr<GattClientDeathRecipient> deathRecipient_;
};

class GattClient::impl::GattClientDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    GattClientDeathRecipient(GattClient::impl &client) : client_(client){};
    ~GattClientDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(GattClientDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("enter");
        client_.proxy_->AsObject()->RemoveDeathRecipient(client_.deathRecipient_);
        client_.proxy_ = nullptr;
    }

private:
    GattClient::impl &client_;
};

class GattClient::impl::BluetoothGattClientCallbackStubImpl : public BluetoothGattClientCallbackStub {
public:
    void OnServicesChanged(std::vector<BluetoothGattService> &service) override
    {
        HILOGI("enter");
        client_.pimpl->DiscoverStart();
    }

    void OnConnectionStateChanged(int32_t state, int32_t newState) override
    {
        HILOGI("gattClient conn state, status: %{public}d, newState: %{public}s",
            state, GetProfileConnStateName(newState).c_str());
        if (newState == static_cast<int>(BTConnectState::DISCONNECTED)) {
            client_.pimpl->CleanConnectionInfo();
        }

        {
            std::lock_guard<std::mutex> lck(client_.pimpl->connStateMutex_);
            client_.pimpl->connectionState_ = newState;
        }

        client_.pimpl->callback_->OnConnectionStateChanged(newState, state);
    }

    void OnCharacteristicChanged(const BluetoothGattCharacteristic &characteristic) override
    {
        HILOGI("enter");
        for (auto &svc : client_.pimpl->gattServices_) {
            for (auto &character : svc.GetCharacteristics()) {
                if (character.GetHandle() == characteristic.handle_) {
                    character.SetValue(characteristic.value_.get(), characteristic.length_);
                    client_.pimpl->callback_->OnCharacteristicChanged(character);
                }
            }
        }
    }

    void OnCharacteristicRead(int32_t ret, const BluetoothGattCharacteristic &characteristic) override
    {
        HILOGI("enter, ret: %{public}d", ret);
        std::lock_guard<std::mutex> lock(client_.pimpl->requestInformation_.mutex_);
        client_.pimpl->requestInformation_.doing_ = false;
        auto ptr = client_.pimpl->requestInformation_.context_.characteristic_;
        if (client_.pimpl->requestInformation_.type_ != REQUEST_TYPE_CHARACTERISTICS_READ) {
            HILOGE("Unexpected call!");
        }
        if (GattStatus::GATT_SUCCESS == ret) {
            ptr->SetValue(characteristic.value_.get(), characteristic.length_);
        }
        client_.pimpl->callback_->OnCharacteristicReadResult(*ptr, ret);
    }

    void OnCharacteristicWrite(int32_t ret, const BluetoothGattCharacteristic &characteristic) override
    {
        HILOGI("enter, ret: %{public}d", ret);
        std::lock_guard<std::mutex> lock(client_.pimpl->requestInformation_.mutex_);
        client_.pimpl->requestInformation_.doing_ = false;
        auto ptr = client_.pimpl->requestInformation_.context_.characteristic_;
        if (client_.pimpl->requestInformation_.type_ != REQUEST_TYPE_CHARACTERISTICS_WRITE) {
            HILOGE("Unexpected call!");
        }
        client_.pimpl->callback_->OnCharacteristicWriteResult(*ptr, ret);
    }

    void OnDescriptorRead(int32_t ret, const BluetoothGattDescriptor &descriptor) override
    {
        HILOGI("enter, ret: %{public}d", ret);
        std::lock_guard<std::mutex> lock(client_.pimpl->requestInformation_.mutex_);
        client_.pimpl->requestInformation_.doing_ = false;
        auto ptr = client_.pimpl->requestInformation_.context_.descriptor_;
        if (client_.pimpl->requestInformation_.type_ != REQUEST_TYPE_DESCRIPTOR_READ) {
            HILOGE("Unexpected call!");
        }
        if (GattStatus::GATT_SUCCESS == ret) {
            ptr->SetValue(descriptor.value_.get(), descriptor.length_);
        }
        client_.pimpl->callback_->OnDescriptorReadResult(*ptr, ret);
    }

    void OnDescriptorWrite(int32_t ret, const BluetoothGattDescriptor &descriptor) override
    {
        HILOGI("enter, ret: %{public}d", ret);
        std::lock_guard<std::mutex> lock(client_.pimpl->requestInformation_.mutex_);
        client_.pimpl->requestInformation_.doing_ = false;
        auto ptr = client_.pimpl->requestInformation_.context_.descriptor_;
        if (client_.pimpl->requestInformation_.type_ == REQUEST_TYPE_DESCRIPTOR_WRITE) {
            client_.pimpl->callback_->OnDescriptorWriteResult(*ptr, ret);
        } else if (client_.pimpl->requestInformation_.type_ == REQUEST_TYPE_SET_NOTIFY_CHARACTERISTICS) {
            client_.pimpl->callback_->OnSetNotifyCharacteristic(ret);
        } else {
            HILOGE("Unexpected call!");
        }
    }

    void OnMtuChanged(int32_t state, int32_t mtu) override
    {
        HILOGI("state: %{public}d, mtu: %{public}d", state, mtu);
        client_.pimpl->callback_->OnMtuUpdate(mtu, state);
    }

    void OnServicesDiscovered(int32_t status) override
    {
        HILOGI("status: %{public}d", status);
        client_.pimpl->DiscoverComplete(status);
    }

    void OnConnectionParameterChanged(int32_t interval, int32_t latency, int32_t timeout, int32_t status) override
    {
        HILOGI("interval: %{public}d, latency: %{public}d, timeout: %{public}d, status: %{public}d",
            interval, latency, timeout, status);
        client_.pimpl->callback_->OnConnectionParameterChanged(interval, latency, timeout, status);
    }

    BluetoothGattClientCallbackStubImpl(GattClient &client) : client_(client)
    {}
    ~BluetoothGattClientCallbackStubImpl()
    {}

private:
    GattClient &client_;
};

GattClient::impl::impl(GattClient &client, const BluetoothRemoteDevice &device)
    : isGetServiceYet_(false),
      isRegisterSucceeded_(false),
      callback_(nullptr),
      applicationId_(0),
      connectionState_(static_cast<int>(BTConnectState::DISCONNECTED)),
      device_(device)
{
    HILOGI("enter");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGE("hostRemote is null");
        return;
    }

    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_GATT_CLIENT);

    if (!remote) {
        HILOGE("remote is null");
        return;
    }
    HILOGI("remote obtained");
    proxy_ = iface_cast<IBluetoothGattClient>(remote);
    clientCallback_ = new BluetoothGattClientCallbackStubImpl(client);

    deathRecipient_ = new GattClientDeathRecipient(*this);
    if (!proxy_) {
        HILOGE("proxy_ is null");
    } else {
        proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
    }
}

GattClient::impl::~impl()
{
    HILOGI("GattClient ~impl");
    if (!proxy_) {
        HILOGE("proxy_ is null");
    } else {
        proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }
}

int GattClient::impl::DiscoverStart()
{
    {
        std::unique_lock<std::mutex> lock(discoverInformation_.mutex_);
        while (discoverInformation_.isDiscovering_) {
            discoverInformation_.condition_.wait(lock);
        }
        discoverInformation_.isDiscovering_ = true;
    }

    if (!isRegisterSucceeded_) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    int result = GattStatus::GATT_FAILURE;
    if (!proxy_) {
        HILOGE("proxy_ is null");
    } else {
        result = proxy_->DiscoveryServices(applicationId_);
        if (GattStatus::GATT_SUCCESS != result) {
            DiscoverComplete(GattStatus::GATT_FAILURE);
        }
    }
    return result;
}

void GattClient::impl::DiscoverComplete(int state)
{
    bool ret = false;
    {
        std::unique_lock<std::mutex> lock(discoverInformation_.mutex_);
        if (discoverInformation_.isDiscovering_) {
            discoverInformation_.isDiscovering_ = false;
            isGetServiceYet_ = false;
            discoverInformation_.condition_.notify_all();
            ret = true;
        }
    }
    if (ret) {
        callback_->OnServicesDiscovered(state);
    }
}

void GattClient::impl::BuildServiceList(const std::vector<BluetoothGattService> &src)
{
    HILOGI("enter");
    for (auto &svc : src) {
        GattService svcTmp(UUID::ConvertFrom128Bits(svc.uuid_.ConvertTo128Bits()),
            svc.handle_,
            svc.endHandle_,
            svc.isPrimary_ ? GattServiceType::PRIMARY : GattServiceType::SECONDARY);
        for (auto &character : svc.characteristics_) {
            GattCharacteristic characterTmp(UUID::ConvertFrom128Bits(character.uuid_.ConvertTo128Bits()),
                character.handle_,
                character.permissions_,
                character.properties_);
            for (auto &desc : character.descriptors_) {
                characterTmp.AddDescriptor(GattDescriptor(
                    UUID::ConvertFrom128Bits(desc.uuid_.ConvertTo128Bits()), desc.handle_, desc.permissions_));
            }
            svcTmp.AddCharacteristic(std::move(characterTmp));
        }
        gattServices_.emplace_back(std::move(svcTmp));
    }
    for (auto &svc : src) {
        GattService *ptr = FindService(svc.handle_);
        if (ptr == NULL) {
            return;
        }
        for (auto &isvc : svc.includeServices_) {
            GattService *iptr = FindService(isvc.startHandle_);
            ptr->AddService(*iptr);
        }
    }
}

GattService *GattClient::impl::FindService(uint16_t handle)
{
    for (auto &item : gattServices_) {
        if (item.GetHandle() == handle) {
            return &item;
        }
    }
    return nullptr;
}

void GattClient::impl::GetServices()
{
    HILOGI("enter");
    std::unique_lock<std::mutex> lock(discoverInformation_.mutex_);
    while (discoverInformation_.isDiscovering_) {
        discoverInformation_.condition_.wait(lock);
    }
    if (isGetServiceYet_) {
        HILOGE("isGetServiceYet_ is true");
        return;
    }
    if (!isRegisterSucceeded_) {
        HILOGE("isRegisterSucceeded_ is false");
        return;
    }
    gattServices_.clear();
    std::vector<BluetoothGattService> result;
    if (!proxy_) {
        HILOGE("proxy_ is null");
    } else {
        proxy_->GetServices(applicationId_, result);
        BuildServiceList(result);
        isGetServiceYet_ = true;
    }
}

int GattClient::impl::GetTransport(int &transport)
{
    auto deviceType = device_.GetDeviceType();
    switch (transport) {
        case GATT_TRANSPORT_TYPE_LE: {
            if (deviceType == DEVICE_TYPE_BREDR) {
                return GattStatus::REQUEST_NOT_SUPPORT;
            }
            break;
        }
        case GATT_TRANSPORT_TYPE_CLASSIC: {
            if (deviceType == DEVICE_TYPE_LE) {
                return GattStatus::REQUEST_NOT_SUPPORT;
            }
            break;
        }
        case GATT_TRANSPORT_TYPE_AUTO: {
            if (deviceType == DEVICE_TYPE_LE || deviceType == DEVICE_TYPE_DUAL_MONO ||
                deviceType == DEVICE_TYPE_UNKNOWN) {
                transport = GATT_TRANSPORT_TYPE_LE;
            } else {
                transport = GATT_TRANSPORT_TYPE_CLASSIC;
            }
            break;
        }
        default:
            return GattStatus::INVALID_PARAMETER;
            break;
    }
    return GattStatus::GATT_SUCCESS;
}

void GattClient::impl::CleanConnectionInfo()
{
    DiscoverComplete(GattStatus::GATT_FAILURE);
    std::lock_guard<std::mutex> lock(requestInformation_.mutex_);
    requestInformation_.doing_ = false;
}

GattClient::GattClient(const BluetoothRemoteDevice &device) : pimpl(new GattClient::impl(*this, device))
{
    HILOGI("enter");
}

GattClient::~GattClient()
{
    HILOGI("~GattClient");
    if (pimpl->isRegisterSucceeded_) {
        if (!pimpl->proxy_) {
            HILOGE("proxy_ is null");
        } else {
            pimpl->proxy_->DeregisterApplication(pimpl->applicationId_);
        }
    }
}

int GattClient::Connect(GattClientCallback &callback, bool isAutoConnect, int transport)
{
    HILOGI("enter, isAutoConnect: %{public}d, transport: %{public}d", isAutoConnect, transport);
    std::lock_guard<std::mutex> lock(pimpl->connStateMutex_);
    if (pimpl->connectionState_ == static_cast<int>(BTConnectState::CONNECTED)) {
        HILOGE("Already connected");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    HILOGI("isRegisterSucceeded: %{public}d", pimpl->isRegisterSucceeded_);
    if (pimpl->isRegisterSucceeded_) {
        if (!pimpl->proxy_) {
            HILOGE("proxy_ is null");
            return GattStatus::GATT_FAILURE;
        }
        return pimpl->proxy_->Connect(pimpl->applicationId_, isAutoConnect);
    }
    pimpl->callback_ = &callback;
    int32_t result = pimpl->GetTransport(transport);
    if (GattStatus::GATT_SUCCESS != result) {
        HILOGE("Transport error, status: %{public}d", result);
        return result;
    }
    if ((transport == GATT_TRANSPORT_TYPE_LE && !IS_BLE_ENABLED()) ||
        (transport == GATT_TRANSPORT_TYPE_CLASSIC && !IS_BT_ENABLED())) {
        HILOGE("Unsupported mode");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    if (transport == GATT_TRANSPORT_TYPE_CLASSIC && isAutoConnect) {
        HILOGE("Unsupported mode");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    if (!pimpl->device_.IsValidBluetoothRemoteDevice()) {
        HILOGE("Invalid remote device");
        return GattStatus::INVALID_REMOTE_DEVICE;
    }
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    result = pimpl->proxy_->RegisterApplication(
        pimpl->clientCallback_, bluetooth::RawAddress(pimpl->device_.GetDeviceAddr()), transport);
    HILOGI("Proxy register application result: %{public}d", result);
    if (result > 0) {
        pimpl->applicationId_ = result;
        pimpl->isRegisterSucceeded_ = true;
        if (!pimpl->proxy_) {
            HILOGE("proxy_ is null");
            return GattStatus::GATT_FAILURE;
        }
        result = pimpl->proxy_->Connect(pimpl->applicationId_, isAutoConnect);
    }
    return result;
}

int GattClient::Disconnect()
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lock(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED) || !pimpl->isRegisterSucceeded_) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    int result = GattStatus::GATT_FAILURE;
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    result = pimpl->proxy_->Disconnect(pimpl->applicationId_);
    return result;
}

int GattClient::Close()
{
    HILOGI("enter");
    if (pimpl->isRegisterSucceeded_) {
        if (!pimpl->proxy_) {
            HILOGE("proxy_ is null");
            return GattStatus::REQUEST_NOT_SUPPORT;
        } else {
            int32_t result = pimpl->proxy_->DeregisterApplication(pimpl->applicationId_);
            HILOGI("result: %{public}d", result);
            if (result == RET_NO_ERROR) {
                pimpl->isRegisterSucceeded_ = false;
            }
            return result;
        }
    }
    HILOGI("isRegisterSucceeded_ is false");
    return GattStatus::GATT_SUCCESS;
}

int GattClient::DiscoverServices()
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lck(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED)) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->DiscoverStart();
}

std::optional<std::reference_wrapper<GattService>> GattClient::GetService(const UUID &uuid)
{
    HILOGI("enter");
    pimpl->GetServices();
    for (auto &svc : pimpl->gattServices_) {
        if (svc.GetUuid().Equals(uuid)) {
            HILOGI("successful");
            return svc;
        }
    }
    HILOGE("failed");
    return std::nullopt;
}

std::vector<GattService> &GattClient::GetService()
{
    HILOGI("enter");
    pimpl->GetServices();
    return pimpl->gattServices_;
}

int GattClient::ReadCharacteristic(GattCharacteristic &characteristic)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lock(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED) || !pimpl->isRegisterSucceeded_) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->requestInformation_.mutex_);
    if (pimpl->requestInformation_.doing_) {
        HILOGE("Remote device busy");
        return GattStatus::REMOTE_DEVICE_BUSY;
    }
    int result = GattStatus::GATT_FAILURE;
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    HILOGI("applicationId: %{public}d, handle: 0x%{public}04X", pimpl->applicationId_, characteristic.GetHandle());
    result = pimpl->proxy_->ReadCharacteristic(
        pimpl->applicationId_, (BluetoothGattCharacteristic)bluetooth::Characteristic(characteristic.GetHandle()));
    HILOGI("result: %{public}d", result);
    if (GattStatus::GATT_SUCCESS == result) {
        pimpl->requestInformation_.doing_ = true;
        pimpl->requestInformation_.type_ = REQUEST_TYPE_CHARACTERISTICS_READ;
        pimpl->requestInformation_.context_.characteristic_ = &characteristic;
    }
    return result;
}

int GattClient::ReadDescriptor(GattDescriptor &descriptor)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lck(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED) || !pimpl->isRegisterSucceeded_) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lock(pimpl->requestInformation_.mutex_);
    if (pimpl->requestInformation_.doing_) {
        HILOGE("Remote device busy");
        return GattStatus::REMOTE_DEVICE_BUSY;
    }
    int result = GattStatus::GATT_FAILURE;
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    HILOGI("applicationId: %{public}d, handle: 0x%{public}04X", pimpl->applicationId_, descriptor.GetHandle());
    result = pimpl->proxy_->ReadDescriptor(
        pimpl->applicationId_, (BluetoothGattDescriptor)bluetooth::Descriptor(descriptor.GetHandle()));
    HILOGI("result: %{public}d", result);
    if (GattStatus::GATT_SUCCESS == result) {
        pimpl->requestInformation_.doing_ = true;
        pimpl->requestInformation_.type_ = REQUEST_TYPE_DESCRIPTOR_READ;
        pimpl->requestInformation_.context_.descriptor_ = &descriptor;
    }
    return result;
}

int GattClient::RequestBleMtuSize(int mtu)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lck(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED) || !pimpl->isRegisterSucceeded_) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    int result = GattStatus::GATT_FAILURE;
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    HILOGI("applicationId: %{public}d, mtu: %{public}d", pimpl->applicationId_, mtu);
    result = pimpl->proxy_->RequestExchangeMtu(pimpl->applicationId_, mtu);
    HILOGI("result: %{public}d", result);
    return result;
}

int GattClient::SetNotifyCharacteristic(GattCharacteristic &characteristic, bool enable)
{
    HILOGI("handle: 0x%{public}04X, enable: %{public}d", characteristic.GetHandle(), enable);
    static const uint8_t NOTIFICATION[2] = {1, 0};
    static const uint8_t DEFAULT_VALUE[2] = {0};
    static const size_t CLIENT_CHARACTERISTIC_CONFIGURATION_VALUE_LENGTH = 0x02;
    std::lock_guard<std::mutex> lockConn(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED)) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    auto descriptor = characteristic.GetDescriptor(UUID::FromString("00002902-0000-1000-8000-00805F9B34FB"));
    if (descriptor == nullptr) {
        HILOGE("Invalid parameters");
        return GattStatus::INVALID_PARAMETER;
    }
    std::lock_guard<std::mutex> lock(pimpl->requestInformation_.mutex_);
    if (pimpl->requestInformation_.doing_) {
        HILOGI("Remote device busy");
        return GattStatus::REMOTE_DEVICE_BUSY;
    }
    BluetoothGattDescriptor desc(bluetooth::Descriptor(descriptor->GetHandle(),
        (enable ? NOTIFICATION : DEFAULT_VALUE),
        CLIENT_CHARACTERISTIC_CONFIGURATION_VALUE_LENGTH));
    int result = GattStatus::GATT_FAILURE;
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    HILOGI("applicationId: %{public}d", pimpl->applicationId_);
    result = pimpl->proxy_->WriteDescriptor(pimpl->applicationId_, &desc);
    HILOGI("result: %{public}d", result);
    if (GattStatus::GATT_SUCCESS == result) {
        pimpl->requestInformation_.type_ = REQUEST_TYPE_SET_NOTIFY_CHARACTERISTICS;
        pimpl->requestInformation_.context_.descriptor_ = descriptor;
        pimpl->requestInformation_.doing_ = true;
    }
    return result;
}

int GattClient::WriteCharacteristic(GattCharacteristic &characteristic)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lockConn(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED)) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    size_t length = 0;
    auto &characterValue = characteristic.GetValue(&length);
    if (characterValue == nullptr || length == 0) {
        HILOGE("Invalid parameters");
        return GattStatus::INVALID_PARAMETER;
    }
    std::lock_guard<std::mutex> lock(pimpl->requestInformation_.mutex_);
    if (pimpl->requestInformation_.doing_) {
        HILOGE("Remote device busy");
        return GattStatus::REMOTE_DEVICE_BUSY;
    }
    BluetoothGattCharacteristic character(
        bluetooth::Characteristic(characteristic.GetHandle(), characterValue.get(), length));
    int result = GattStatus::GATT_FAILURE;
    bool withoutRespond = true;
    if (characteristic.GetWriteType() == (int)GattCharacteristic::WriteType::SIGNED) {
        if (!pimpl->proxy_) {
            HILOGE("proxy_ is null");
            return GattStatus::GATT_FAILURE;
        }
        HILOGI("Signed write");
        result = pimpl->proxy_->SignedWriteCharacteristic(pimpl->applicationId_, &character);
    } else {
        withoutRespond =
            ((characteristic.GetWriteType() == (int)GattCharacteristic::WriteType::DEFAULT) ? false : true);
        if (!pimpl->proxy_) {
            HILOGE("proxy_ is null");
            return GattStatus::GATT_FAILURE;
        }
        HILOGI("Write without response");
        result = pimpl->proxy_->WriteCharacteristic(pimpl->applicationId_, &character, withoutRespond);
    }
    if (GattStatus::GATT_SUCCESS == result && !withoutRespond) {
        HILOGI("successful");
        pimpl->requestInformation_.type_ = REQUEST_TYPE_CHARACTERISTICS_WRITE;
        pimpl->requestInformation_.context_.characteristic_ = &characteristic;
        pimpl->requestInformation_.doing_ = true;
    } else {
        HILOGI("result: %{public}d", result);
    }
    return result;
}

int GattClient::WriteDescriptor(GattDescriptor &descriptor)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lck(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED) || !pimpl->isRegisterSucceeded_) {
        HILOGE("Request not supported");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    size_t length = 0;
    auto &characterValue = descriptor.GetValue(&length);
    if (characterValue == nullptr || length == 0) {
        HILOGE("Invalid parameters");
        return GattStatus::INVALID_PARAMETER;
    }
    std::lock_guard<std::mutex> lock(pimpl->requestInformation_.mutex_);
    if (pimpl->requestInformation_.doing_) {
        HILOGE("Remote device busy");
        return GattStatus::REMOTE_DEVICE_BUSY;
    }
    int result = GattStatus::GATT_FAILURE;
    BluetoothGattDescriptor desc(bluetooth::Descriptor(descriptor.GetHandle(), characterValue.get(), length));
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    result = pimpl->proxy_->WriteDescriptor(pimpl->applicationId_, &desc);
    HILOGI("result: %{public}d", result);
    if (GattStatus::GATT_SUCCESS == result) {
        pimpl->requestInformation_.doing_ = true;
        pimpl->requestInformation_.type_ = REQUEST_TYPE_DESCRIPTOR_WRITE;
        pimpl->requestInformation_.context_.descriptor_ = &descriptor;
    }
    return result;
}

int GattClient::RequestConnectionPriority(int connPriority)
{
    std::lock_guard<std::mutex> lockConn(pimpl->connStateMutex_);
    if (pimpl->connectionState_ != static_cast<int>(BTConnectState::CONNECTED)) {
        HILOGE("Not connected");
        return GattStatus::REQUEST_NOT_SUPPORT;
    }
    if (connPriority != static_cast<int>(GattConnectionPriority::BALANCED) &&
        connPriority != static_cast<int>(GattConnectionPriority::HIGH) &&
        connPriority != static_cast<int>(GattConnectionPriority::LOW_POWER)) {
        HILOGE("Invalid parameters");
        return GattStatus::INVALID_PARAMETER;
    }
    int result = GattStatus::GATT_FAILURE;
    if (!pimpl->proxy_) {
        HILOGE("proxy_ is null");
        return GattStatus::GATT_FAILURE;
    }
    result = pimpl->proxy_->RequestConnectionPriority(pimpl->applicationId_, connPriority);
    HILOGI("result: %{public}d", result);
    return result;
}
}  // namespace Bluetooth
}  // namespace OHOS