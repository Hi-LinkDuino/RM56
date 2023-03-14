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

#include <list>
#include <mutex>
#include <thread>

#include "bluetooth_gatt_client_server.h"
#include "bluetooth_hitrace.h"
#include "bluetooth_log.h"
#include "hisysevent.h"
#include "interface_adapter_ble.h"
#include "interface_adapter_classic.h"
#include "interface_adapter_manager.h"
#include "interface_profile_gatt_client.h"
#include "interface_profile_manager.h"
#include "bluetooth_utils_server.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "permission_utils.h"

using namespace OHOS::bluetooth;
namespace OHOS {
namespace Bluetooth {
struct BluetoothGattClientServer::impl {
    class GattClientCallbackImpl;
    class SystemStateObserver;

    IProfileGattClient *clientService_;
    std::unique_ptr<SystemStateObserver> systemStateObserver_;
    std::list<std::unique_ptr<GattClientCallbackImpl>> callbacks_;
    std::mutex registerMutex_;

    impl();
    ~impl();

    IProfileGattClient *GetServicePtr()
    {
        if (IProfileManager::GetInstance() == nullptr) {
            return nullptr;
        }
        return static_cast<IProfileGattClient *>(
            IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_GATT_CLIENT));
    }
};

class BluetoothGattClientServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    SystemStateObserver(BluetoothGattClientServer::impl *impl) : impl_(impl){};
    ~SystemStateObserver() override = default;

    void OnSystemStateChange(const BTSystemState state) override
    {
        std::lock_guard<std::mutex> lck(impl_->registerMutex_);
        switch (state) {
            case BTSystemState::ON:
                impl_->clientService_ = impl_->GetServicePtr();
                break;
            case BTSystemState::OFF:
                impl_->clientService_ = nullptr;
                break;
            default:
                break;
        }
    }

private:
    BluetoothGattClientServer::impl *impl_;
};

class BluetoothGattClientServer::impl::GattClientCallbackImpl : public IGattClientCallback {
public:
    void OnConnectionStateChanged(int state, int newState) override
    {
        HILOGI("curState(%{public}d) -> newState(%{public}d)", state, newState);
        if (PermissionUtils::VerifyUseBluetoothPermission(tokenId_) == PERMISSION_DENIED) {
            HILOGE("check permission failed, tokenId: %{public}u", tokenId_);
            return;
        }
        int32_t pid = IPCSkeleton::GetCallingPid();
        int32_t uid = IPCSkeleton::GetCallingUid();
        if (state == static_cast<int>(BTConnectState::CONNECTED) ||
            state == static_cast<int>(BTConnectState::DISCONNECTED)) {
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "GATT_CLIENT_CONN_STATE",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", state);
        }
        callback_->OnConnectionStateChanged(state, newState);
    }

    void OnCharacteristicChanged(const Characteristic &characteristic) override
    {
        HILOGI("enter");
        if (PermissionUtils::VerifyUseBluetoothPermission(tokenId_) == PERMISSION_DENIED) {
            HILOGE("check permission failed, tokenId: %{public}u", tokenId_);
            return;
        }
        callback_->OnCharacteristicChanged((BluetoothGattCharacteristic)characteristic);
    }

    void OnCharacteristicRead(int ret, const Characteristic &characteristic) override
    {
        HILOGI("ret: %{public}d", ret);
        callback_->OnCharacteristicRead(ret, (BluetoothGattCharacteristic)characteristic);
    }

    void OnCharacteristicWrite(int ret, const Characteristic &characteristic) override
    {
        HILOGI("ret: %{public}d", ret);
        callback_->OnCharacteristicWrite(ret, (BluetoothGattCharacteristic)characteristic);
    }

    void OnDescriptorRead(int ret, const Descriptor &descriptor) override
    {
        HILOGI("ret: %{public}d", ret);
        callback_->OnDescriptorRead(ret, (BluetoothGattDescriptor)descriptor);
    }

    void OnDescriptorWrite(int ret, const Descriptor &descriptor) override
    {
        HILOGI("ret: %{public}d", ret);
        callback_->OnDescriptorWrite(ret, (BluetoothGattDescriptor)descriptor);
    }

    void OnMtuChanged(int state, int mtu) override
    {
        HILOGI("state: %{public}d, mtu: %{public}d", state, mtu);
        callback_->OnMtuChanged(state, mtu);
    }

    void OnServicesDiscovered(int status) override
    {
        HILOGI("status: %{public}d", status);
        callback_->OnServicesDiscovered(status);
    }

    void OnConnectionParameterChanged(int interval, int latency, int timeout, int status) override
    {
        HILOGI("interval: %{public}d, latency: %{public}d, timeout: %{public}d, status: %{public}d",
            interval, latency, timeout, status);
        callback_->OnConnectionParameterChanged(interval, latency, timeout, status);
    }

    void OnServicesChanged(const std::vector<Service> &services) override
    {
        HILOGI("enter");
        std::vector<BluetoothGattService> result;
        int num = services.size();
        for (int i = 0; i < num; i++) {
            result.push_back((BluetoothGattService)services[i]);
        }
        callback_->OnServicesChanged(result);
    }

    sptr<IBluetoothGattClientCallback> GetCallback()
    {
        return callback_;
    }

    void SetAppId(int appId)
    {
        applicationId_ = appId;
    }

    int GetAppId()
    {
        return applicationId_;
    }

    GattClientCallbackImpl(const sptr<IBluetoothGattClientCallback> &callback, BluetoothGattClientServer &owner);
    ~GattClientCallbackImpl()
    {
        if (!callback_->AsObject()->RemoveDeathRecipient(deathRecipient_)) {
            HILOGE("Failed to unlink death recipient to callback");
        }
        callback_ = nullptr;
        deathRecipient_ = nullptr;
    };

private:
    class CallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        CallbackDeathRecipient(const sptr<IBluetoothGattClientCallback> &callback, BluetoothGattClientServer &owner);

        sptr<IBluetoothGattClientCallback> GetCallback() const
        {
            return callback_;
        };

        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

    private:
        sptr<IBluetoothGattClientCallback> callback_;
        BluetoothGattClientServer &owner_;
    };

    sptr<IBluetoothGattClientCallback> callback_;
    sptr<CallbackDeathRecipient> deathRecipient_;
    int applicationId_;
    uint32_t tokenId_;
};

BluetoothGattClientServer::impl::GattClientCallbackImpl::GattClientCallbackImpl(
    const sptr<IBluetoothGattClientCallback> &callback, BluetoothGattClientServer &owner)
    : callback_(callback), deathRecipient_(new CallbackDeathRecipient(callback, owner))
{
    if (!callback_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        HILOGE("Failed to link death recipient to callback");
    }
    tokenId_ = IPCSkeleton::GetCallingTokenID();
}

BluetoothGattClientServer::impl::GattClientCallbackImpl::CallbackDeathRecipient::CallbackDeathRecipient(
    const sptr<IBluetoothGattClientCallback> &callback, BluetoothGattClientServer &owner)
    : callback_(callback), owner_(owner)
{}

void BluetoothGattClientServer::impl::GattClientCallbackImpl::CallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    HILOGI("enter");
    if (owner_.pimpl == nullptr || owner_.pimpl->clientService_ == nullptr) {
        HILOGE("gattClientServer clientService_ is not support.");
        return;
    }
    std::lock_guard<std::mutex> lck(owner_.pimpl->registerMutex_);
    for (auto it = owner_.pimpl->callbacks_.begin(); it != owner_.pimpl->callbacks_.end(); ++it) {
        if ((*it)->GetCallback()->AsObject() == remote) {
            HILOGI("callback is found from callbacks");
            sptr<CallbackDeathRecipient> dr = (*it)->deathRecipient_;
            if (!dr->GetCallback()->AsObject()->RemoveDeathRecipient(dr)) {
                HILOGE("Failed to unlink death recipient from callback");
            }
            HILOGI("App id is %{public}d", (*it)->GetAppId());
            owner_.pimpl->clientService_->Disconnect((*it)->GetAppId());
            owner_.pimpl->clientService_->DeregisterApplication((*it)->GetAppId());
            owner_.pimpl->callbacks_.erase(it);
            *it = nullptr;
            return;
        }
    }
    HILOGE("No callback erased from callbacks");
}

BluetoothGattClientServer::impl::impl() : clientService_(nullptr), systemStateObserver_(new SystemStateObserver(this))
{
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*systemStateObserver_);
}

BluetoothGattClientServer::impl::~impl()
{
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*systemStateObserver_);
}

BluetoothGattClientServer::BluetoothGattClientServer() : pimpl(new impl())
{
    HILOGI("enter");
}

BluetoothGattClientServer::~BluetoothGattClientServer()
{}

int BluetoothGattClientServer::RegisterApplication(
    const sptr<IBluetoothGattClientCallback> &callback, const BluetoothRawAddress &addr, int32_t transport)
{
    HILOGI("address: %{public}s, transport: %{public}d", GetEncryptAddr(addr.GetAddress()).c_str(), transport);
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    pimpl->clientService_ = pimpl->GetServicePtr();
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    auto it = pimpl->callbacks_.emplace(
        pimpl->callbacks_.begin(), std::make_unique<impl::GattClientCallbackImpl>(callback, *this));
    int appId = pimpl->clientService_->RegisterSharedApplication(*it->get(), (RawAddress)addr, transport);
    (*it)->SetAppId(appId);
    HILOGI("appId: %{public}d", appId);
    if (appId >= 0) {
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "GATT_APP_REGISTER",
            OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,  "ACTION", "register",
            "SIDE", "client", "ADDRESS", addr.GetAddress(), "PID", OHOS::IPCSkeleton::GetCallingPid(),
            "UID", OHOS::IPCSkeleton::GetCallingUid(), "APPID", appId);
    }
    return appId;
}

int BluetoothGattClientServer::DeregisterApplication(int32_t appId)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "GATT_APP_REGISTER",
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,  "ACTION", "deregister",
        "SIDE", "client", "ADDRESS", "empty", "PID", OHOS::IPCSkeleton::GetCallingPid(),
        "UID", OHOS::IPCSkeleton::GetCallingUid(), "APPID", appId);
    return pimpl->clientService_->DeregisterApplication(appId);
}

int BluetoothGattClientServer::Connect(int32_t appId, bool autoConnect)
{
    HILOGI("appId: %{public}d, autoConnect: %{public}d", appId, autoConnect);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    OHOS::Bluetooth::BluetoothHiTrace::BluetoothStartAsyncTrace("GATT_CLIENT_CONNECT", 1);
    int result = pimpl->clientService_->Connect(appId, autoConnect);
    OHOS::Bluetooth::BluetoothHiTrace::BluetoothFinishAsyncTrace("GATT_CLIENT_CONNECT", 1);
    return result;
}

int BluetoothGattClientServer::Disconnect(int32_t appId)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->Disconnect(appId);
}

int BluetoothGattClientServer::DiscoveryServices(int32_t appId)
{
    HILOGI("appId: %{public}d", appId);
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->DiscoveryServices(appId);
}

int BluetoothGattClientServer::ReadCharacteristic(int32_t appId, const BluetoothGattCharacteristic &characteristic)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->ReadCharacteristic(appId, (Characteristic)characteristic);
}

int BluetoothGattClientServer::WriteCharacteristic(
    int32_t appId, BluetoothGattCharacteristic *characteristic, bool withoutRespond)
{
    HILOGI("appId: %{public}d, withoutRespond: %{public}d", appId, withoutRespond);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    Characteristic character(characteristic->handle_);
    character.length_ = characteristic->length_;
    character.value_ = std::move(characteristic->value_);
    characteristic->length_ = 0;
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->WriteCharacteristic(appId, character, withoutRespond);
}
int BluetoothGattClientServer::SignedWriteCharacteristic(int32_t appId, BluetoothGattCharacteristic *characteristic)
{
    HILOGI("appId: %{public}d", appId);
    Characteristic character(characteristic->handle_);
    character.length_ = characteristic->length_;
    character.value_ = std::move(characteristic->value_);
    characteristic->length_ = 0;
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->SignedWriteCharacteristic(appId, character);
}

int BluetoothGattClientServer::ReadDescriptor(int32_t appId, const BluetoothGattDescriptor &descriptor)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->ReadDescriptor(appId, (Descriptor)descriptor);
}

int BluetoothGattClientServer::WriteDescriptor(int32_t appId, BluetoothGattDescriptor *descriptor)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    Descriptor desc(descriptor->handle_);
    desc.length_ = descriptor->length_;
    desc.value_ = std::move(descriptor->value_);
    descriptor->length_ = 0;
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->WriteDescriptor(appId, desc);
}

int BluetoothGattClientServer::RequestExchangeMtu(int32_t appId, int32_t mtu)
{
    HILOGI("appId: %{public}d, mtu: %{public}d", appId, mtu);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    return pimpl->clientService_->RequestExchangeMtu(appId, mtu);
}

void BluetoothGattClientServer::GetAllDevice(::std::vector<BluetoothGattDevice> &device)
{
    HILOGI("enter");
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return;
    }
    for (auto &dev : pimpl->clientService_->GetAllDevice()) {
        device.push_back(dev);
    }
}

int BluetoothGattClientServer::RequestConnectionPriority(int32_t appId, int32_t connPriority)
{
    HILOGI("appId: %{public}d, connPriority: %{public}d", appId, connPriority);
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return 0;
    }
    return pimpl->clientService_->RequestConnectionPriority(appId, connPriority);
}

void BluetoothGattClientServer::GetServices(int32_t appId, ::std::vector<BluetoothGattService> &service)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (pimpl->clientService_ == nullptr) {
        HILOGE("request not support.");
        return;
    }
    for (auto &svc : pimpl->clientService_->GetServices(appId)) {
        service.push_back(svc);
    }
}
}  // namespace Bluetooth
}  // namespace OHOS
