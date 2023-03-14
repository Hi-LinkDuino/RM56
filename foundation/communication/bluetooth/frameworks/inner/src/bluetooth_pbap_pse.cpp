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
#include <string>
#include "bluetooth_pbap_pse_observer_stub.h"
#include "bluetooth_pbap_pse_proxy.h"
#include "bluetooth_pbap_server.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_host.h"
#include "bluetooth_utils.h"
#include "bluetooth_observer_list.h"
#include "iservice_registry.h"
#include "raw_address.h"
#include "system_ability_definition.h"
#include "bluetooth_host_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothPbapPseObserverImp : public BluetoothPbapPseObserverStub {
public:
    BluetoothPbapPseObserverImp() = default;
    ~BluetoothPbapPseObserverImp() override = default;
    void SetObserver(BluetoothObserverList<PbapObserver> *observers)
    {
        observers_ = observers;
    }

    void OnServiceConnectionStateChanged(const BluetoothRawAddress &device, int state) override
    {
        HILOGI("enter, device: %{public}s, state: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), state);
        observers_->ForEach([device, state](std::shared_ptr<PbapObserver> observer) {
            BluetoothRemoteDevice dev(device.GetAddress(), 0);
            observer->OnServiceConnectionStateChanged(dev, state);
        });
    }

    void OnServicePermission(const BluetoothRawAddress &device) override
    {
        HILOGI("enter, device: %{public}s", GetEncryptAddr((device).GetAddress()).c_str());
        observers_->ForEach([device](std::shared_ptr<PbapObserver> observer) {
            BluetoothRemoteDevice dev(device.GetAddress(), 0);
            observer->OnServicePermission(dev);
        });
    }
    void OnServicePasswordRequired(const BluetoothRawAddress &device,
        const ::std::vector<uint8_t> &description, int8_t charset, bool fullAccess) override
    {
        HILOGI("enter, device: %{public}s, charset: %{public}d, fullAccess: %{public}d",
            GetEncryptAddr((device).GetAddress()).c_str(), charset, fullAccess);
        observers_->ForEach([device, description, charset, fullAccess](std::shared_ptr<PbapObserver> observer) {
            BluetoothRemoteDevice dev(device.GetAddress(), 0);
            observer->OnServicePasswordRequired(dev, description, charset, fullAccess);
        });
    }

private:
    BluetoothObserverList<PbapObserver> *observers_;
};

struct PbapServer::impl {
    sptr<IBluetoothPbapPse> proxy_;
    class BluetoothPbapPseDeathRecipient;
    sptr<BluetoothPbapPseDeathRecipient> deathRecipient_ = nullptr;
    impl();

    ~impl()
    {
        if (proxy_ != nullptr) {
            proxy_->DeregisterObserver(serviceObserver_);
            proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        }
    }

    void RegisterObserver(std::shared_ptr<PbapObserver> &observer)
    {
        HILOGI("enter");
        if (observer) {
            observers_.Register(observer);
        }
    }

    void DeregisterObserver(std::shared_ptr<PbapObserver> &observer)
    {
        HILOGI("enter");
        if (observer) {
            observers_.Deregister(observer);
        }
    }

    int GetDeviceState(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (!device.IsValidBluetoothRemoteDevice()) {
            return static_cast<int>(BTConnectState::DISCONNECTED);
        }
        int32_t state = static_cast<int>(BTConnectState::DISCONNECTED);
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            state = proxy_->GetDeviceState(bluetooth::RawAddress(device.GetDeviceAddr()));
        }
        return (int)state;
    }

    std::vector<BluetoothRemoteDevice> GetDevicesByStates(const std::vector<int> &states)
    {
        HILOGI("enter");
        std::vector<BluetoothRemoteDevice> remoteDevices;
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            std::vector<int32_t> tmpStates;
            for (int state : states) {
                tmpStates.push_back((int32_t)state);
            }
            std::vector<BluetoothRawAddress> rawDevices;
            proxy_->GetDevicesByStates(tmpStates, rawDevices);
            for (BluetoothRawAddress rawDevice : rawDevices) {
                BluetoothRemoteDevice remoteDevice(rawDevice.GetAddress(), 0);
                remoteDevices.push_back(remoteDevice);
            }
        }
        return remoteDevices;
    }

    std::vector<BluetoothRemoteDevice> GetConnectedDevices()
    {
        HILOGI("enter");
        std::vector<int> states {static_cast<int>(BTConnectState::CONNECTED)};
        return GetDevicesByStates(states);
    }

    bool Disconnect(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (!device.IsValidBluetoothRemoteDevice()) {
            return false;
        }
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            int32_t ret = proxy_->Disconnect(bluetooth::RawAddress(device.GetDeviceAddr()));
            return ret == RET_NO_ERROR;
        }
        return false;
    }

    bool SetConnectionStrategy(const BluetoothRemoteDevice &device, int strategy)
    {
        HILOGI("enter, device: %{public}s, strategy: %{public}d", GET_ENCRYPT_ADDR(device), strategy);
        if (!device.IsValidBluetoothRemoteDevice()) {
            return false;
        }
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            int32_t ret = proxy_->SetConnectionStrategy(bluetooth::RawAddress(device.GetDeviceAddr()), strategy);
            return ret == RET_NO_ERROR;
        }
        return false;
    }

    int GetConnectionStrategy(const BluetoothRemoteDevice &device) const
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (!device.IsValidBluetoothRemoteDevice()) {
            return static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN);
        }
        int32_t ret = static_cast<int>(BTStrategyType::CONNECTION_FORBIDDEN);
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            ret = proxy_->GetConnectionStrategy(bluetooth::RawAddress(device.GetDeviceAddr()));
        }
        return (int)ret;
    }

    void GrantPermission(const BluetoothRemoteDevice &device, bool allow, bool save)
    {
        HILOGI("enter, device: %{public}s, allow: %{public}d, save: %{public}d", GET_ENCRYPT_ADDR(device), allow, save);
        if (!device.IsValidBluetoothRemoteDevice()) {
            return;
        }
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            proxy_->GrantPermission(bluetooth::RawAddress(device.GetDeviceAddr()), allow, save);
        }
    }
    int SetDevicePassword(const BluetoothRemoteDevice &device, const std::string &password, std::string userId)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (!device.IsValidBluetoothRemoteDevice()) {
            return RET_BAD_PARAM;
        }
        int32_t ret = RET_NO_SUPPORT;
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            ret = proxy_->SetDevicePassword(bluetooth::RawAddress(device.GetDeviceAddr()), password, userId);
        }
        return (int)ret;
    }

private:
    std::mutex mutex_;
    BluetoothObserverList<PbapObserver> observers_;
    sptr<BluetoothPbapPseObserverImp> serviceObserver_;
};

class PbapServer::impl::BluetoothPbapPseDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    BluetoothPbapPseDeathRecipient(PbapServer::impl &pbapPse) : pbapPse_(pbapPse) {};
    ~BluetoothPbapPseDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothPbapPseDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("starts");
        pbapPse_.proxy_->AsObject()->RemoveDeathRecipient(pbapPse_.deathRecipient_);
        pbapPse_.proxy_ = nullptr;
    }

private:
    PbapServer::impl &pbapPse_;
};

PbapServer::impl::impl()
{
    serviceObserver_ = new BluetoothPbapPseObserverImp();
    serviceObserver_->SetObserver(&observers_);
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGI("failed: no hostRemote");
        return;
    }
    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_PBAP_PSE);

    if (!remote) {
        HILOGE("failed: no remote");
        return;
    }
    HILOGI("remote obtained");

    proxy_ = iface_cast<IBluetoothPbapPse>(remote);
    if (proxy_ == nullptr) {
        return;
    }
    deathRecipient_ = new BluetoothPbapPseDeathRecipient(*this);
    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
    proxy_->RegisterObserver(serviceObserver_);
}

PbapServer *PbapServer::GetProfile()
{
    static PbapServer instance;
    return &instance;
}

PbapServer::PbapServer()
{
    pimpl = std::make_unique<impl>();
}

PbapServer::~PbapServer()
{}

void PbapServer::RegisterObserver(PbapObserver *observer)
{
    HILOGI("enter");
    std::shared_ptr<PbapObserver> pointer(observer, [](PbapObserver *) {});
    return pimpl->RegisterObserver(pointer);
}

void PbapServer::DeregisterObserver(PbapObserver *observer)
{
    HILOGI("enter");
    std::shared_ptr<PbapObserver> pointer(observer, [](PbapObserver *) {});
    return pimpl->DeregisterObserver(pointer);
}

int PbapServer::GetDeviceState(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->GetDeviceState(device);
}

std::vector<BluetoothRemoteDevice> PbapServer::GetDevicesByStates(const std::vector<int> &states)
{
    HILOGI("enter");
    return pimpl->GetDevicesByStates(states);
}

std::vector<BluetoothRemoteDevice> PbapServer::GetConnectedDevices()
{
    HILOGI("enter");
    return pimpl->GetConnectedDevices();
}

bool PbapServer::Disconnect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->Disconnect(device);
}

bool PbapServer::SetConnectionStrategy(const BluetoothRemoteDevice &device, int strategy)
{
    HILOGI("enter, device: %{public}s, strategy: %{public}d", GET_ENCRYPT_ADDR(device), strategy);
    return pimpl->SetConnectionStrategy(device, strategy);
}

int PbapServer::GetConnectionStrategy(const BluetoothRemoteDevice &device) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->GetConnectionStrategy(device);
}

void PbapServer::GrantPermission(const BluetoothRemoteDevice &device, bool allow, bool save)
{
    HILOGI("enter, device: %{public}s, allow: %{public}d, save: %{public}d", GET_ENCRYPT_ADDR(device), allow, save);
    pimpl->GrantPermission(device, allow, save);
}

int PbapServer::SetDevicePassword(const BluetoothRemoteDevice &device, const std::string &password, std::string userId)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->SetDevicePassword(device, password, userId);
}
}  // namespace Bluetooth
}  // namespace OHOS