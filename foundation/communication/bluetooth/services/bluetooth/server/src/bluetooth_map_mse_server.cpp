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
#include "bluetooth_map_mse_server.h"
#include "interface_adapter_manager.h"
#include "interface_profile_manager.h"
#include "interface_profile_map_mse.h"
#include "bluetooth_log.h"
#include "bluetooth_utils_server.h"
#include "remote_observer_list.h"

using namespace OHOS::bluetooth;

namespace OHOS {
namespace Bluetooth {
class BluetoothMapMseObserverImpl : public IMapMseObserver {
public:
    BluetoothMapMseObserverImpl(RemoteObserverList<IBluetoothMapMseObserver> *observers) : observers_(observers)
    {
        HILOGI("enter");
    }

    ~BluetoothMapMseObserverImpl() override = default;

    void OnConnectionStateChanged(const RawAddress &remoteAddr, int state) override
    {
        HILOGI("addr: %{public}s, state: %{public}d", GetEncryptAddr(remoteAddr.GetAddress()).c_str(), state);
        observers_->ForEach([remoteAddr, state](IBluetoothMapMseObserver *observer) {
            observer->OnConnectionStateChanged(remoteAddr, state);
        });
    }

    void OnPermission(const RawAddress &remoteAddr) override
    {
        HILOGI("addr: %{public}s", GetEncryptAddr(remoteAddr.GetAddress()).c_str());
        observers_->ForEach([remoteAddr](IBluetoothMapMseObserver *observer) { observer->OnPermission(remoteAddr); });
    }

private:
    RemoteObserverList<IBluetoothMapMseObserver> *observers_;
};

struct BluetoothMapMseServer::impl {
    impl();
    RemoteObserverList<IBluetoothMapMseObserver> observers_;
    std::unique_ptr<BluetoothMapMseObserverImpl> observerImp_ = nullptr;
    IProfileMapMse *mapMseService_ = nullptr;
    class MapMseSystemStateObserver;
    std::unique_ptr<MapMseSystemStateObserver> systemStateObserver_ = nullptr;
};

class BluetoothMapMseServer::impl::MapMseSystemStateObserver : public ISystemStateObserver {
public:
    MapMseSystemStateObserver(BluetoothMapMseServer::impl *pimpl) : pimpl_(pimpl) {};
    ~MapMseSystemStateObserver() override = default;
    void OnSystemStateChange(const BTSystemState state) override
    {
        HILOGI("enter");
        switch (state) {
            case BTSystemState::ON: {
                IProfileManager *serviceMgr = IProfileManager::GetInstance();
                if (serviceMgr != nullptr) {
                    pimpl_->mapMseService_ = (IProfileMapMse *)serviceMgr->GetProfileService(PROFILE_NAME_MAP_MSE);
                    if (pimpl_->mapMseService_ != nullptr) {
                        pimpl_->mapMseService_->RegisterObserver(*(pimpl_->observerImp_));
                    }
                }
            } break;
            case BTSystemState::OFF:
                pimpl_->mapMseService_ = nullptr;
                break;
            default:
                break;
        }
    }

private:
    BluetoothMapMseServer::impl *pimpl_;
};

BluetoothMapMseServer::impl::impl()
{
    systemStateObserver_ = std::make_unique<impl::MapMseSystemStateObserver>(this);
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*systemStateObserver_);
}

BluetoothMapMseServer::BluetoothMapMseServer()
{
    HILOGI("enter");
    pimpl = std::make_unique<impl>();

    IProfileManager *serviceMgr = IProfileManager::GetInstance();
    if (serviceMgr != nullptr) {
        pimpl->mapMseService_ = (IProfileMapMse *)serviceMgr->GetProfileService(PROFILE_NAME_MAP_MSE);
        if (pimpl->mapMseService_ != nullptr) {
            pimpl->observerImp_ = std::make_unique<BluetoothMapMseObserverImpl>(&(pimpl->observers_));
            pimpl->mapMseService_->RegisterObserver(*(pimpl->observerImp_));
        }
    }
}
BluetoothMapMseServer::~BluetoothMapMseServer()
{}

// IBluetoothMapMse overrides:
void BluetoothMapMseServer::RegisterObserver(
    const sptr<IBluetoothMapMseObserver> &observer)
{
    HILOGI("enter");
    if (!observer) {
        HILOGE("RegisterObserver called with NULL . Ignoring.");
    }
    pimpl->observers_.Register(observer);
}

void BluetoothMapMseServer::DeregisterObserver(
    const sptr<IBluetoothMapMseObserver> &observer)
{
    HILOGI("enter");
    if (!observer) {
        HILOGE("UnregisterObserver called with NULL . Ignoring.");
    }
    pimpl->observers_.Deregister(observer);
}

void BluetoothMapMseServer::GetState(int32_t &ret)
{
    HILOGI("ret: %{public}d", ret);
    ret = -1;
    if (pimpl->mapMseService_ != nullptr) {
        ret = pimpl->mapMseService_->GetState();
    }
}

void BluetoothMapMseServer::Disconnect(
    const BluetoothRawAddress &device, int32_t &ret)
{
    HILOGI("device: %{public}s, ret: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), ret);
    ret = -1;
    std::string addString = device.GetAddress();
    bluetooth::RawAddress addr(addString);
    if (pimpl->mapMseService_ != nullptr) {
        ret = pimpl->mapMseService_->Disconnect(addr);
    }
}

void BluetoothMapMseServer::IsConnected(
    const BluetoothRawAddress &device, bool &ret)
{
    HILOGI("device: %{public}s, ret: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), ret);
    ret = false;
    std::string addString = device.GetAddress();
    bluetooth::RawAddress addr(addString);
    if (pimpl->mapMseService_ != nullptr) {
        ret = pimpl->mapMseService_->IsConnected(addr);
    }
}

void BluetoothMapMseServer::GetConnectedDevices(
    std::vector<BluetoothRawAddress> &devices)
{
    HILOGI("enter");
    if (pimpl->mapMseService_ != nullptr) {
        for (auto &device : pimpl->mapMseService_->GetConnectDevices()) {
            devices.push_back(device);
        }
    }
}

void BluetoothMapMseServer::GetDevicesByStates(
    const std::vector<int32_t> &states, std::vector<BluetoothRawAddress> &devices)
{
    HILOGI("enter");
    if (pimpl->mapMseService_ != nullptr) {
        for (auto &device : pimpl->mapMseService_->GetDevicesByStates(states)) {
            devices.push_back(device);
        }
    }
}

void BluetoothMapMseServer::GetConnectionState(
    const BluetoothRawAddress &device, int32_t &ret)
{
    HILOGI("device: %{public}s, ret: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), ret);
    ret = -1;
    std::string addString = device.GetAddress();
    bluetooth::RawAddress addr(addString);
    if (pimpl->mapMseService_ != nullptr) {
        ret = pimpl->mapMseService_->GetConnectionState(addr);
    }
}

void BluetoothMapMseServer::SetConnectionStrategy(
    const BluetoothRawAddress &device, int32_t strategy, bool &ret)
{
    HILOGI("device: %{public}s, strategy: %{public}d, ret: %{public}d",
        GetEncryptAddr(device.GetAddress()).c_str(), strategy, ret);
    ret = false;
    std::string addString = device.GetAddress();
    bluetooth::RawAddress addr(addString);
    if (pimpl->mapMseService_ != nullptr) {
        ret = pimpl->mapMseService_->SetConnectionStrategy(addr, strategy);
    }
}

void BluetoothMapMseServer::GetConnectionStrategy(
    const BluetoothRawAddress &device, int32_t &ret)
{
    HILOGI("device: %{public}s, ret: %{public}d", GetEncryptAddr(device.GetAddress()).c_str(), ret);
    ret = -1;
    std::string addString = device.GetAddress();
    bluetooth::RawAddress addr(addString);
    if (pimpl->mapMseService_ != nullptr) {
        ret = pimpl->mapMseService_->GetConnectionStrategy(addr);
    }
}

void BluetoothMapMseServer::GrantPermission(
    const BluetoothRawAddress &device, bool allow, bool save)
{
    HILOGI("device: %{public}s allow: %{public}d save: %{public}d",
        GetEncryptAddr(device.GetAddress()).c_str(), allow, save);
    std::string addString = device.GetAddress();
    bluetooth::RawAddress addr(addString);
    if (pimpl->mapMseService_ != nullptr) {
        pimpl->mapMseService_->GrantPermission(addr, allow, save);
    }
}
}  // namespace Bluetooth
}  // namespace OHOS
