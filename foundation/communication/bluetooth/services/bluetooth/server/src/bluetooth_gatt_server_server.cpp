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

#include "bluetooth_gatt_server_server.h"
#include "bluetooth_log.h"
#include "bluetooth_utils_server.h"
#include "gatt_data.h"
#include "hisysevent.h"
#include "i_bluetooth_gatt_server.h"
#include "interface_adapter_manager.h"
#include "interface_profile_gatt_server.h"
#include "interface_profile_manager.h"
#include "ipc_skeleton.h"
#include "permission_utils.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
struct BluetoothGattServerServer::impl {
    class GattServerCallbackImpl;
    class SystemStateObserver;

    bluetooth::IProfileGattServer *serverService_;
    std::unique_ptr<SystemStateObserver> systemStateObserver_;
    std::list<std::shared_ptr<GattServerCallbackImpl>> callbacks_;
    std::mutex registerMutex_;

    impl();
    ~impl();

    bluetooth::IProfileGattServer *GetServicePtr()
    {
        if (bluetooth::IProfileManager::GetInstance() == nullptr) {
            return nullptr;
        }
        return static_cast<bluetooth::IProfileGattServer *>(
            bluetooth::IProfileManager::GetInstance()->GetProfileService(bluetooth::PROFILE_NAME_GATT_SERVER));
    }
};
class BluetoothGattServerServer::impl::SystemStateObserver : public bluetooth::ISystemStateObserver {
public:
    SystemStateObserver(BluetoothGattServerServer::impl *impl) : impl_(impl){};
    ~SystemStateObserver() override = default;

    void OnSystemStateChange(const bluetooth::BTSystemState state) override
    {
        std::lock_guard<std::mutex> lck(impl_->registerMutex_);
        switch (state) {
            case bluetooth::BTSystemState::ON:
                impl_->serverService_ = impl_->GetServicePtr();
                break;
            case bluetooth::BTSystemState::OFF:
                impl_->serverService_ = nullptr;
                break;
            default:
                break;
        }
    }

private:
    BluetoothGattServerServer::impl *impl_;
};

class BluetoothGattServerServer::impl::GattServerCallbackImpl : public bluetooth::IGattServerCallback {
public:
    void OnCharacteristicReadRequest(
        const bluetooth::GattDevice &device, const bluetooth::Characteristic &characteristic) override
    {
        HILOGI("addr: %{public}s", GET_ENCRYPT_GATT_ADDR(device));
        if (PermissionUtils::VerifyUseBluetoothPermission(tokenId_) == PERMISSION_DENIED) {
            HILOGE("check permission failed, tokenId_: %{public}u", tokenId_);
            return;
        }
        callback_->OnCharacteristicReadRequest(
            (BluetoothGattDevice)device, (BluetoothGattCharacteristic)characteristic);
    }
    void OnCharacteristicReadByUuidRequest(
        const bluetooth::GattDevice &device, const bluetooth::Characteristic &characteristic) override
    {
        HILOGI("addr: %{public}s", GET_ENCRYPT_GATT_ADDR(device));
    }
    void OnCharacteristicWriteRequest(const bluetooth::GattDevice &device,
        const bluetooth::Characteristic &characteristic, bool needRespones) override
    {
        HILOGI("addr: %{public}s, needRespones: %{public}d", GET_ENCRYPT_GATT_ADDR(device), needRespones);
        if (PermissionUtils::VerifyUseBluetoothPermission(tokenId_) == PERMISSION_DENIED) {
            HILOGE("check permission failed, tokenId_: %{public}u", tokenId_);
            return;
        }
        callback_->OnCharacteristicWriteRequest(
            (BluetoothGattDevice)device, (BluetoothGattCharacteristic)characteristic, needRespones);
    }
    void OnDescriptorReadRequest(const bluetooth::GattDevice &device, const bluetooth::Descriptor &descriptor) override
    {
        HILOGI("addr: %{public}s", GET_ENCRYPT_GATT_ADDR(device));
        if (PermissionUtils::VerifyUseBluetoothPermission(tokenId_) == PERMISSION_DENIED) {
            HILOGE("check permission failed, tokenId_: %{public}u", tokenId_);
            return;
        }
        callback_->OnDescriptorReadRequest((BluetoothGattDevice)device, (BluetoothGattDescriptor)descriptor);
    }
    void OnDescriptorWriteRequest(const bluetooth::GattDevice &device, const bluetooth::Descriptor &descriptor) override
    {
        HILOGI("addr: %{public}s", GET_ENCRYPT_GATT_ADDR(device));
        if (PermissionUtils::VerifyUseBluetoothPermission(tokenId_) == PERMISSION_DENIED) {
            HILOGE("check permission failed, tokenId_: %{public}u", tokenId_);
            return;
        }
        callback_->OnDescriptorWriteRequest((BluetoothGattDevice)device, (BluetoothGattDescriptor)descriptor);
    }
    void OnNotifyConfirm(
        const bluetooth::GattDevice &device, const bluetooth::Characteristic &characteristic, int result) override
    {
        HILOGI("addr: %{public}s, result: %{public}d", GET_ENCRYPT_GATT_ADDR(device), result);
        callback_->OnNotifyConfirm((BluetoothGattDevice)device, (BluetoothGattCharacteristic)characteristic, result);
    }
    void OnConnectionStateChanged(const bluetooth::GattDevice &device, int ret, int state) override
    {
        HILOGI("addr: %{public}s, ret: %{public}d, state: %{public}d", GET_ENCRYPT_GATT_ADDR(device), ret, state);
        if (PermissionUtils::VerifyUseBluetoothPermission(tokenId_) == PERMISSION_DENIED) {
            HILOGE("check permission failed, tokenId_: %{public}u", tokenId_);
            return;
        }
        int32_t pid = IPCSkeleton::GetCallingPid();
        int32_t uid = IPCSkeleton::GetCallingUid();
        if (state == static_cast<int>(BTConnectState::CONNECTED) ||
            state == static_cast<int>(BTConnectState::DISCONNECTED)) {
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "GATT_SERVER_CONN_STATE",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "PID", pid, "UID", uid, "STATE", state);
        }
        callback_->OnConnectionStateChanged((BluetoothGattDevice)device, ret, state);
    }
    void OnMtuChanged(const bluetooth::GattDevice &device, int mtu) override
    {
        HILOGI("addr: %{public}s, mtu: %{public}d", GET_ENCRYPT_GATT_ADDR(device), mtu);
        callback_->OnMtuChanged((BluetoothGattDevice)device, mtu);
    }
    void OnAddService(int ret, const bluetooth::Service &services) override
    {
        HILOGI("ret: %{public}d", ret);
        callback_->OnAddService(ret, (BluetoothGattService)services);
    }
    void OnServiceChanged(const bluetooth::Service &services) override
    {
        HILOGI("enter");
    }
    void OnConnectionParameterChanged(
        const bluetooth::GattDevice &device, int interval, int latency, int timeout, int status) override
    {
        HILOGI("addr: %{public}s, interval: %{public}d, latency: %{public}d, timeout: %{public}d, status: %{public}d",
            GET_ENCRYPT_GATT_ADDR(device), interval, latency, timeout, status);
        callback_->OnConnectionParameterChanged((BluetoothGattDevice)device, interval, latency, timeout, status);
    }

    sptr<IBluetoothGattServerCallback> GetCallback()
    {
        return callback_;
    }

    void SetAppId(int32_t appId)
    {
        HILOGI("SetAppId = %{public}d", appId);
        appId_ = appId;
    }

    int32_t GetAppId()
    {
        return appId_;
    }

    GattServerCallbackImpl(const sptr<IBluetoothGattServerCallback> &callback, BluetoothGattServerServer &owner);
    ~GattServerCallbackImpl()
    {
        if (!callback_->AsObject()->RemoveDeathRecipient(deathRecipient_)) {
            HILOGE("Failed to unlink death recipient to callback");
        }
        callback_ = nullptr;
        deathRecipient_ = nullptr;
    };

private:
    class GattServerCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        GattServerCallbackDeathRecipient(
            const sptr<IBluetoothGattServerCallback> &callback, BluetoothGattServerServer &owner);

        sptr<IBluetoothGattServerCallback> GetCallback() const
        {
            return callback_;
        };

        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

    private:
        sptr<IBluetoothGattServerCallback> callback_;
        BluetoothGattServerServer &owner_;
    };

    sptr<IBluetoothGattServerCallback> callback_;
    sptr<GattServerCallbackDeathRecipient> deathRecipient_;
    uint32_t tokenId_;
    int32_t appId_;
};

BluetoothGattServerServer::impl::GattServerCallbackImpl::GattServerCallbackImpl(
    const sptr<IBluetoothGattServerCallback> &callback, BluetoothGattServerServer &owner)
    : callback_(callback), deathRecipient_(new GattServerCallbackDeathRecipient(callback, owner))
{
    if (!callback_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        HILOGE("Failed to link death recipient to callback");
    }
    tokenId_ = IPCSkeleton::GetCallingTokenID();
    appId_ = -1;
}

BluetoothGattServerServer::impl::GattServerCallbackImpl::GattServerCallbackDeathRecipient::
    GattServerCallbackDeathRecipient(
    const sptr<IBluetoothGattServerCallback> &callback, BluetoothGattServerServer &owner)
    : callback_(callback), owner_(owner)
{}

void BluetoothGattServerServer::impl::GattServerCallbackImpl::GattServerCallbackDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    HILOGI("GattServerCallbackDeathRecipient OnRemoteDied start, list size = %{public}lu",
        (unsigned long)owner_.pimpl->callbacks_.size());
    std::lock_guard<std::mutex> lck(owner_.pimpl->registerMutex_);
    for (auto it = owner_.pimpl->callbacks_.begin(); it != owner_.pimpl->callbacks_.end(); ++it) {
        if ((*it) != nullptr && (*it)->GetCallback() != nullptr && (*it)->GetCallback()->AsObject() == remote) {
            int appId = (*it)->GetAppId();
            HILOGI("callback is erased from callbacks, appId: %{public}d", appId);
            sptr<GattServerCallbackDeathRecipient> dr = (*it)->deathRecipient_;
            if (!dr->GetCallback()->AsObject()->RemoveDeathRecipient(dr)) {
                HILOGE("Failed to unlink death recipient from callback");
            }
            if (owner_.pimpl->serverService_ != nullptr) {
                int ret = owner_.pimpl->serverService_->DeregisterApplication(appId);
                HILOGI("DeregisterApplication result:%{public}d, appId:%{public}d", ret, appId);
            }
            owner_.pimpl->callbacks_.erase(it);
            return;
        }
    }
}
BluetoothGattServerServer::impl::impl() : serverService_(nullptr), systemStateObserver_(new SystemStateObserver(this))
{
    bluetooth::IAdapterManager::GetInstance()->RegisterSystemStateObserver(*systemStateObserver_);
}

BluetoothGattServerServer::impl::~impl()
{
    bluetooth::IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*systemStateObserver_);
}

int BluetoothGattServerServer::AddService(int32_t appId, BluetoothGattService *services)
{
    HILOGI("enter, appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    bluetooth::Service svc = (bluetooth::Service)*services;

    return pimpl->serverService_->AddService(appId, svc);
}

void BluetoothGattServerServer::ClearServices(int appId)
{
    HILOGI("enter, appId: %{public}d", appId);
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return;
    }
    pimpl->serverService_->ClearServices(appId);
}

void BluetoothGattServerServer::CancelConnection(const BluetoothGattDevice &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_GATT_ADDR(device));
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return;
    }
    pimpl->serverService_->CancelConnection((bluetooth::GattDevice)device);
}

int BluetoothGattServerServer::NotifyClient(
    const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, bool needConfirm)
{
    HILOGI("addr: %{public}s, needConfirm: %{public}d", GET_ENCRYPT_GATT_ADDR(device), needConfirm);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    bluetooth::Characteristic character(characteristic->handle_);
    character.length_ = characteristic->length_;
    character.value_ = std::move(characteristic->value_);
    characteristic->length_ = 0;

    return pimpl->serverService_->NotifyClient((bluetooth::GattDevice)device, character, needConfirm);
}

int BluetoothGattServerServer::RemoveService(int32_t appId, const BluetoothGattService &services)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }

    return pimpl->serverService_->RemoveService(appId, (bluetooth::Service)services);
}

int BluetoothGattServerServer::RespondCharacteristicRead(
    const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, int32_t ret)
{
    HILOGI("addr: %{public}s, ret: %{public}d", GET_ENCRYPT_GATT_ADDR(device), ret);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    bluetooth::Characteristic character(characteristic->handle_);
    character.length_ = characteristic->length_;
    character.value_ = std::move(characteristic->value_);
    characteristic->length_ = 0;

    return pimpl->serverService_->RespondCharacteristicRead((bluetooth::GattDevice)device, character, ret);
}

int BluetoothGattServerServer::RespondCharacteristicWrite(
    const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic, int32_t ret)
{
    HILOGI("addr: %{public}s, ret: %{public}d", GET_ENCRYPT_GATT_ADDR(device), ret);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }

    return pimpl->serverService_->RespondCharacteristicWrite(
        (bluetooth::GattDevice)device, (bluetooth::Characteristic)characteristic, ret);
}

int BluetoothGattServerServer::RespondDescriptorRead(
    const BluetoothGattDevice &device, BluetoothGattDescriptor *descriptor, int32_t ret)
{
    HILOGI("addr: %{public}s, ret: %{public}d", GET_ENCRYPT_GATT_ADDR(device), ret);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    bluetooth::Descriptor desc(descriptor->handle_);
    desc.length_ = descriptor->length_;
    desc.value_ = std::move(descriptor->value_);
    descriptor->length_ = 0;

    return pimpl->serverService_->RespondDescriptorRead((bluetooth::GattDevice)device, desc, ret);
}

int BluetoothGattServerServer::RespondDescriptorWrite(
    const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor, int32_t ret)
{
    HILOGI("addr: %{public}s, ret: %{public}d", GET_ENCRYPT_GATT_ADDR(device), ret);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }

    return pimpl->serverService_->RespondDescriptorWrite(
        (bluetooth::GattDevice)device, (bluetooth::Descriptor)descriptor, ret);
}

int BluetoothGattServerServer::RegisterApplication(const sptr<IBluetoothGattServerCallback> &callback)
{
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    pimpl->serverService_ = pimpl->GetServicePtr();
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }

    auto it = pimpl->callbacks_.emplace(
        pimpl->callbacks_.begin(), std::make_shared<impl::GattServerCallbackImpl>(callback, *this));

    int ret = pimpl->serverService_->RegisterApplication(*it);
    if (ret >= 0) {
        HILOGI("appId, %{public}d", ret);
        (*it)->SetAppId(ret);
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "GATT_APP_REGISTER",
            OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,  "ACTION", "register",
            "SIDE", "server", "ADDRESS", "empty", "PID", OHOS::IPCSkeleton::GetCallingPid(),
            "UID", OHOS::IPCSkeleton::GetCallingUid(), "APPID", ret);
    } else {
        HILOGE("RegisterApplication failed, ret: %{public}d", ret);
        pimpl->callbacks_.erase(it);
    }
    return ret;
}

int BluetoothGattServerServer::DeregisterApplication(int32_t appId)
{
    HILOGI("appId: %{public}d", appId);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return REQUEST_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lck(pimpl->registerMutex_);
    if (!pimpl->serverService_) {
        HILOGE("serverService_ is null");
        return bluetooth::GattStatus::REQUEST_NOT_SUPPORT;
    }
    int ret = pimpl->serverService_->DeregisterApplication(appId);
    HILOGI("list size: %{public}lu", (unsigned long)pimpl->callbacks_.size());
    for (auto it = pimpl->callbacks_.begin(); it != pimpl->callbacks_.end(); ++it) {
        if ((*it) != nullptr && (*it)->GetAppId() == appId) {
            HILOGI("erase appId: %{public}d", appId);
            pimpl->callbacks_.erase(it);
            break;
        }
    }
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "GATT_APP_REGISTER",
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,  "ACTION", "deregister",
        "SIDE", "server", "ADDRESS", "empty", "PID", OHOS::IPCSkeleton::GetCallingPid(),
        "UID", OHOS::IPCSkeleton::GetCallingUid(), "APPID", appId);
    return ret;
}
BluetoothGattServerServer::BluetoothGattServerServer() : pimpl(new impl())
{
    HILOGI("Bluetooth Gatt Server Server Created!");
}

BluetoothGattServerServer::~BluetoothGattServerServer()
{}
}  // namespace Bluetooth
}  // namespace OHOS