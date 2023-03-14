/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "interface_profile_hid_host.h"
#include "i_bluetooth_host_observer.h"
#include "permission_utils.h"
#include "remote_observer_list.h"
#include "hilog/log.h"
#include "bluetooth_hid_host_server.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHidHostCallback : public bluetooth::IHidHostObserver {
public:
    BluetoothHidHostCallback() = default;
    ~BluetoothHidHostCallback() override = default;

    void OnConnectionStateChanged(const RawAddress &device, int state) override
    {
        // Reference "BTConnectState"
        HILOGI("addr:%{public}s, state:%{public}d", GET_ENCRYPT_ADDR(device), state);
        observers_->ForEach([device, state](sptr<IBluetoothHidHostObserver> observer) {
            observer->OnConnectionStateChanged(device, state);
        });
    }

    void SetObserver(RemoteObserverList<IBluetoothHidHostObserver> *observers)
    {
        observers_ = observers;
    }

private:
    RemoteObserverList<IBluetoothHidHostObserver> *observers_;
};

struct BluetoothHidHostServer::impl {
    impl();
    ~impl();

    class SystemStateObserver;
    std::unique_ptr<SystemStateObserver> systemStateObserver_ = nullptr;

    RemoteObserverList<IBluetoothHidHostObserver> observers_;
    std::unique_ptr<BluetoothHidHostCallback> observerImp_ = std::make_unique<BluetoothHidHostCallback>();
    IProfileHidHost *hidHostService_ = nullptr;
    std::vector<sptr<IBluetoothHidHostObserver>> advCallBack_;

    IProfileHidHost *GetServicePtr()
    {
        if (IProfileManager::GetInstance() == nullptr) {
            return nullptr;
        }
        return static_cast<IProfileHidHost *>(
            IProfileManager::GetInstance()->GetProfileService(PROFILE_NAME_HID_HOST));
    }
};

class BluetoothHidHostServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    explicit SystemStateObserver(BluetoothHidHostServer::impl *pimpl) : pimpl_(pimpl) {};
    void OnSystemStateChange(const BTSystemState state) override
    {
        HILOGI("start, BTSystemState:%{public}d", state);
        switch (state) {
            case BTSystemState::ON:
                pimpl_->hidHostService_ = pimpl_->GetServicePtr();
                if (pimpl_->hidHostService_ != nullptr) {
                    pimpl_->hidHostService_->RegisterObserver(*pimpl_->observerImp_.get());
                }
                break;
            case BTSystemState::OFF:
                pimpl_->hidHostService_ = nullptr;
                break;
            default:
                break;
        }
    };

private:
    BluetoothHidHostServer::impl *pimpl_ = nullptr;
};

BluetoothHidHostServer::impl::impl()
{
    HILOGI("enter");
}

BluetoothHidHostServer::impl::~impl()
{
    HILOGI("enter");
}

BluetoothHidHostServer::BluetoothHidHostServer()
{
    HILOGI("start");
    pimpl = std::make_unique<impl>();
    pimpl->observerImp_->SetObserver(&(pimpl->observers_));
    pimpl->systemStateObserver_ = std::make_unique<impl::SystemStateObserver>(pimpl.get());
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*(pimpl->systemStateObserver_));

    pimpl->hidHostService_ = pimpl->GetServicePtr();
    if (pimpl->hidHostService_ != nullptr) {
        pimpl->hidHostService_->RegisterObserver(*pimpl->observerImp_.get());
    }
}

BluetoothHidHostServer::~BluetoothHidHostServer()
{
    HILOGI("start");
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*(pimpl->systemStateObserver_));
    if (pimpl->hidHostService_ != nullptr) {
        pimpl->hidHostService_->DeregisterObserver(*pimpl->observerImp_.get());
    }
}

ErrCode BluetoothHidHostServer::RegisterObserver(const sptr<IBluetoothHidHostObserver> observer)
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

ErrCode BluetoothHidHostServer::DeregisterObserver(const sptr<IBluetoothHidHostObserver> observer)
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
    pimpl->hidHostService_->DeregisterObserver(*pimpl->observerImp_.get());
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::GetDevicesByStates(
    const std::vector<int32_t> &states, std::vector<BluetoothRawAddress>& result)
{
    HILOGI("start");
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return false;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGI("hidHostService_ is null");
        return ERR_NO_INIT;
    }

    std::vector<bluetooth::RawAddress> serviceDeviceList = pimpl->hidHostService_->GetDevicesByStates(states);
    for (auto &device : serviceDeviceList) {
        BluetoothRawAddress bluetoothDevice(device.GetAddress());
        result.push_back(bluetoothDevice);
        HILOGI("%{public}s", GET_ENCRYPT_ADDR(bluetoothDevice));
    }
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::GetDeviceState(const BluetoothRawAddress &device, int& result)
{
    HILOGI("start, addr:%{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return BT_FAILURE;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGI("hidHostService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->hidHostService_->GetDeviceState(device);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::Connect(const BluetoothRawAddress &device, bool& result)
{
    HILOGI("start, addr:%{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGE("hidHostService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->hidHostService_->Connect(device);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::Disconnect(const BluetoothRawAddress &device, bool& result)
{
    HILOGI("start, addr:%{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGI("hidHostService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->hidHostService_->Disconnect(device);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::HidHostVCUnplug(std::string &device,
    uint8_t &id, uint16_t &size, uint8_t &type, int& result)
{
    HILOGI("start");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGI("hidHostService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->hidHostService_->HidHostVCUnplug(device, id, size, type);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::HidHostSendData(std::string &device,
    uint8_t &id, uint16_t &size, uint8_t &type, int& result)
{
    HILOGI("start");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGI("hidHostService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->hidHostService_->HidHostSendData(device, id, size, type);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::HidHostSetReport(std::string &device,
    uint8_t &type, uint16_t &size, uint8_t &report, int& result)
{
    HILOGI("start");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGI("hidHostService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->hidHostService_->HidHostSetReport(device, type, size, &report);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}

ErrCode BluetoothHidHostServer::HidHostGetReport(std::string &device,
    uint8_t &id, uint16_t &size, uint8_t &type, int& result)
{
    HILOGI("start");
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("check permission failed");
        return ERR_INVALID_VALUE;
    }
    if (pimpl == nullptr || pimpl->hidHostService_ == nullptr) {
        HILOGI("hidHostService_ is null");
        return ERR_NO_INIT;
    }
    result = pimpl->hidHostService_->HidHostGetReport(device, id, size, type);
    HILOGI("end, result:%{public}d", result);
    return ERR_OK;
}
}  // namespace Bluetooth
}  // namespace OHOS
