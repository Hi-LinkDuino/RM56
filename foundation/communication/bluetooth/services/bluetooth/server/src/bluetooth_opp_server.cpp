/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "bluetooth_def.h"
#include "bluetooth_log.h"
#include "bluetooth_utils_server.h"
#include "interface_profile.h"
#include "interface_profile_opp.h"
#include "i_bluetooth_host_observer.h"
#include "permission_utils.h"
#include "remote_observer_list.h"
#include "hilog/log.h"
#include "bluetooth_opp_server.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothOppCallback : public bluetooth::IOppObserver {
public:
    BluetoothOppCallback() = default;
    ~BluetoothOppCallback() override = default;

    void OnReceiveIncomingFile(const IOppTransferInformation &transferInformation) override
    {
        HILOGI("start.");
        observers_->ForEach([transferInformation](sptr<IBluetoothOppObserver> observer) {
            observer->OnReceiveIncomingFileChanged(BluetoothIOppTransferInformation(transferInformation));
        });
    }
    void OnTransferStateChange(const IOppTransferInformation &transferInformation) override
    {
        HILOGI("start.");
        observers_->ForEach([transferInformation](sptr<IBluetoothOppObserver> observer) {
            observer->OnTransferStateChanged(BluetoothIOppTransferInformation(transferInformation));
        });
    }

    void SetObserver(RemoteObserverList<IBluetoothOppObserver> *observers)
    {
        observers_ = observers;
    }

private:
    RemoteObserverList<IBluetoothOppObserver> *observers_;
};

struct BluetoothOppServer::impl {
    impl();
    ~impl();

    class SystemStateObserver;
    std::unique_ptr<SystemStateObserver> systemStateObserver_ = nullptr;

    RemoteObserverList<IBluetoothOppObserver> observers_;
    std::unique_ptr<BluetoothOppCallback> observerImp_ = std::make_unique<BluetoothOppCallback>();
    IProfileOpp *oppService_ = nullptr;
    std::vector<sptr<IBluetoothOppObserver>> advCallBack_;

    IProfileOpp *GetServicePtr()
    {
        if (IProfileManager::GetInstance() == nullptr) {
            HILOGI("IProfileManager::GetInstance() is null");
            return nullptr;
        }
        return static_cast<IProfileOpp *>(
            IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_OPP));
    }
};

class BluetoothOppServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    explicit SystemStateObserver(BluetoothOppServer::impl *pimpl) : pimpl_(pimpl) {}
    void OnSystemStateChange(const BTSystemState state) override
    {
        HILOGI("BTSystemState:%{public}d", state);
        switch (state) {
            case BTSystemState::ON:
                pimpl_->oppService_ = pimpl_->GetServicePtr();
                if (pimpl_->oppService_ != nullptr) {
                    pimpl_->oppService_->RegisterObserver(*pimpl_->observerImp_.get());
                }
                break;
            case BTSystemState::OFF:
                pimpl_->oppService_ = nullptr;
                break;
            default:
                break;
        }
    };

private:
    BluetoothOppServer::impl *pimpl_ = nullptr;
};

BluetoothOppServer::impl::impl()
{
    HILOGI("enter");
}

BluetoothOppServer::impl::~impl()
{
    HILOGI("enter");
}

BluetoothOppServer::BluetoothOppServer()
{
    HILOGI("start");
    pimpl = std::make_unique<impl>();
    pimpl->observerImp_->SetObserver(&(pimpl->observers_));
    pimpl->systemStateObserver_ = std::make_unique<impl::SystemStateObserver>(pimpl.get());
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*(pimpl->systemStateObserver_));

    pimpl->oppService_ = pimpl->GetServicePtr();
    if (pimpl->oppService_ != nullptr) {
        pimpl->oppService_->RegisterObserver(*pimpl->observerImp_.get());
    }
}

BluetoothOppServer::~BluetoothOppServer()
{
    HILOGI("start");
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*(pimpl->systemStateObserver_));
    if (pimpl->oppService_ != nullptr) {
        pimpl->oppService_->DeregisterObserver(*pimpl->observerImp_.get());
    }
}

ErrCode BluetoothOppServer::RegisterObserver(const sptr<IBluetoothOppObserver> observer)
{
    HILOGI("start");

    if (observer == nullptr) {
        HILOGE("observer is null");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr) {
        HILOGE("pimpl is null");
        return ERR_NO_INIT;
    }

    pimpl->observers_.Register(observer);
    pimpl->advCallBack_.push_back(observer);
    return ERR_OK;
}

ErrCode BluetoothOppServer::DeregisterObserver(const sptr<IBluetoothOppObserver> observer)
{
    HILOGI("start");
    if (observer == nullptr) {
        HILOGE("observer is null");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr) {
        HILOGE("pimpl is null");
        return ERR_NO_INIT;
    }
    for (auto iter = pimpl->advCallBack_.begin(); iter != pimpl->advCallBack_.end(); ++iter) {
        if ((*iter)->AsObject() == observer->AsObject()) {
            if (pimpl != nullptr) {
                pimpl->observers_.Deregister(*iter);
                pimpl->advCallBack_.erase(iter);
                break;
            }
        }
    }
    pimpl->oppService_->DeregisterObserver(*pimpl->observerImp_.get());
    return ERR_OK;
}

ErrCode BluetoothOppServer::GetDevicesByStates(
    const std::vector<int32_t> &states, std::vector<BluetoothRawAddress>& result)
{
    HILOGI("start");
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return false;
    }
    if (pimpl == nullptr || pimpl->oppService_ == nullptr) {
        HILOGE("oppService_ is null");
        return ERR_NO_INIT;
    }

    std::vector<bluetooth::RawAddress> serviceDeviceList = pimpl->oppService_->GetDevicesByStates(states);
    for (auto &device : serviceDeviceList) {
        BluetoothRawAddress bluetoothDevice(device.GetAddress());
        result.push_back(bluetoothDevice);
        HILOGI("%{public}s", GET_ENCRYPT_ADDR(bluetoothDevice));
    }
    return ERR_OK;
}

ErrCode BluetoothOppServer::GetDeviceState(const BluetoothRawAddress &device, int& result)
{
    HILOGI("addr:%{public}s, res:%{public}d", GET_ENCRYPT_ADDR(device), result);
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return BT_FAILURE;
    }
    if (pimpl == nullptr || pimpl->oppService_ == nullptr) {
        HILOGE("oppService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->oppService_->GetDeviceState(device);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothOppServer::SendFile(std::string &device, std::vector<std::string> &filePaths,
    std::vector<std::string> &mimeTypes, bool& result)
{
    HILOGI("addr:%{public}s, res:%{public}d", GetEncryptAddr(device).c_str(), result);
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->oppService_ == nullptr) {
        HILOGE("oppService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->oppService_->SendFile(device, filePaths, mimeTypes);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothOppServer::SetIncomingFileConfirmation(bool &accept, bool &result)
{
    HILOGI("accept:%{public}d, res:%{public}d", accept, result);
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->oppService_ == nullptr) {
        HILOGE("oppService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->oppService_->SetIncomingFileConfirmation(accept);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothOppServer::GetCurrentTransferInformation(BluetoothIOppTransferInformation &transferInformation)
{
    HILOGI("start");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->oppService_ == nullptr) {
        HILOGE("oppService_ is null");
        return ERR_NO_INIT;
    }
    transferInformation = BluetoothIOppTransferInformation(pimpl->oppService_->GetCurrentTransferInformation());
    HILOGI("end");
    return ERR_OK;
}

ErrCode BluetoothOppServer::CancelTransfer(bool &result)
{
    HILOGI("res:%{public}d", result);
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->oppService_ == nullptr) {
        HILOGE("oppService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->oppService_->CancelTransfer();
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}
}  // namespace Bluetooth
}  // namespace OHOS
