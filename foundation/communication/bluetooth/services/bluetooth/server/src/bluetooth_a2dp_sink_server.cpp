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
#include "interface_profile_manager.h"
#include "interface_profile_a2dp_snk.h"
#include "remote_observer_list.h"
#include "interface_adapter_manager.h"
#include "permission_utils.h"
#include "bluetooth_a2dp_sink_server.h"

namespace OHOS {
namespace Bluetooth {
class A2dpSinkObserver : public IA2dpObserver {
public:
    A2dpSinkObserver() = default;
    ~A2dpSinkObserver() override = default;

    void OnConnectionStateChanged(const RawAddress &device, int state) override
    {
        observers_->ForEach([device, state](sptr<IBluetoothA2dpSinkObserver> observer) {
            observer->OnConnectionStateChanged(device, state);
        });
    }

    void SetObserver(RemoteObserverList<IBluetoothA2dpSinkObserver> *observers)
    {
        observers_ = observers;
    }

private:
    RemoteObserverList<IBluetoothA2dpSinkObserver> *observers_;
};

struct BluetoothA2dpSinkServer::impl {
    impl();
    ~impl();

    /// sys state observer
    class SystemStateObserver;
    std::unique_ptr<SystemStateObserver> systemStateObserver_ = nullptr;

    RemoteObserverList<IBluetoothA2dpSinkObserver> observers_;
    std::unique_ptr<A2dpSinkObserver> observerImp_{std::make_unique<A2dpSinkObserver>()};
    IProfileA2dpSnk *a2dpSnkService_ = nullptr;
};

class BluetoothA2dpSinkServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    SystemStateObserver(BluetoothA2dpSinkServer::impl *pimpl) : pimpl_(pimpl) {};
    ~SystemStateObserver() override = default;

    void OnSystemStateChange(const BTSystemState state) override
    {
        IProfileManager *serviceMgr = IProfileManager::GetInstance();
        if (!pimpl_) {
            HILOGI("failed: pimpl_ is null");
            return;
        }

        switch (state) {
            case BTSystemState::ON:
                if (serviceMgr != nullptr) {
                    pimpl_->a2dpSnkService_ =
                        (IProfileA2dpSnk *)serviceMgr->GetProfileService(PROFILE_NAME_A2DP_SINK);
                    if (pimpl_->a2dpSnkService_ != nullptr) {
                        pimpl_->a2dpSnkService_->RegisterObserver(pimpl_->observerImp_.get());
                    }
                }
                break;
            case BTSystemState::OFF:
                pimpl_->a2dpSnkService_ = nullptr;
                break;
            default:
                break;
        }
    }

private:
    BluetoothA2dpSinkServer::impl *pimpl_ = nullptr;
};

BluetoothA2dpSinkServer::impl::impl()
{
    HILOGI("starts");
}

BluetoothA2dpSinkServer::impl::~impl()
{
    HILOGI("starts");
}

BluetoothA2dpSinkServer::BluetoothA2dpSinkServer()
{
    pimpl = std::make_unique<impl>();
    pimpl->observerImp_->SetObserver(&(pimpl->observers_));
    pimpl->systemStateObserver_ = std::make_unique<impl::SystemStateObserver>(pimpl.get());
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*(pimpl->systemStateObserver_));

    IProfileManager *serviceMgr = IProfileManager::GetInstance();
    if (serviceMgr != nullptr) {
        pimpl->a2dpSnkService_ = (IProfileA2dpSnk *)serviceMgr->GetProfileService(PROFILE_NAME_A2DP_SINK);
        if (pimpl->a2dpSnkService_ != nullptr) {
            pimpl->a2dpSnkService_->RegisterObserver(pimpl->observerImp_.get());
        }
    }
}

BluetoothA2dpSinkServer::~BluetoothA2dpSinkServer()
{
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*(pimpl->systemStateObserver_));
    if (pimpl->a2dpSnkService_ != nullptr) {
        pimpl->a2dpSnkService_->DeregisterObserver(pimpl->observerImp_.get());
    }
}

void BluetoothA2dpSinkServer::RegisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer)
{
    HILOGI("starts");
    if (observer == nullptr) {
        HILOGI("observer is null");
        return;
    }
    pimpl->observers_.Register(observer);
}

void BluetoothA2dpSinkServer::DeregisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer)
{
    HILOGI("starts");
    if (observer == nullptr) {
        HILOGI("observer is null");
        return;
    }

    if (pimpl != nullptr) {
        pimpl->observers_.Deregister(observer);
    }
}

int BluetoothA2dpSinkServer::Connect(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSnkService_->Connect(device);
}

int BluetoothA2dpSinkServer::Disconnect(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSnkService_->Disconnect(device);
}

int BluetoothA2dpSinkServer::GetDeviceState(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return BT_FAILURE;
    }
    return pimpl->a2dpSnkService_->GetDeviceState(device);
}

std::vector<RawAddress> BluetoothA2dpSinkServer::GetDevicesByStates(const std::vector<int32_t> &states)
{
    HILOGI("starts");
    std::vector<RawAddress> rawDevices;
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return rawDevices;
    }
    std::vector<int> tmpStates;
    for (int32_t state : states) {
        HILOGI("state = %{public}d", state);
        tmpStates.push_back((int)state);
    }

    rawDevices = pimpl->a2dpSnkService_->GetDevicesByStates(tmpStates);
    return rawDevices;
}

int BluetoothA2dpSinkServer::GetPlayingState(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return BT_FAILURE;
    }
    return pimpl->a2dpSnkService_->GetPlayingState(device);
}

int BluetoothA2dpSinkServer::SetConnectStrategy(const RawAddress &device, int strategy)
{
    HILOGI("addr: %{public}s, strategy: %{public}d", GET_ENCRYPT_ADDR(device), strategy);
    return pimpl->a2dpSnkService_->SetConnectStrategy(device, strategy);
}

int BluetoothA2dpSinkServer::GetConnectStrategy(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSnkService_->GetConnectStrategy(device);
}

int BluetoothA2dpSinkServer::SendDelay(const RawAddress &device, int32_t delayValue)
{
    HILOGI("addr: %{public}s, delayValue: %{public}d", GET_ENCRYPT_ADDR(device), delayValue);
    if (pimpl->a2dpSnkService_ == nullptr) {
        HILOGE("SendDelay but a2dpSnkService_ is nullptr return");
        return BT_FAILURE;
    }
    return pimpl->a2dpSnkService_->SendDelay(device, (uint16_t)delayValue);
}
}  // namespace Bluetooth
}  // namespace OHOS