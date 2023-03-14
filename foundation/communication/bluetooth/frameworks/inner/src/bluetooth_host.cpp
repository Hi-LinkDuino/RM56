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

#include "bluetooth_host.h"
#include <memory>
#include <mutex>
#include <unistd.h>
#include "bluetooth_ble_peripheral_observer_stub.h"
#include "bluetooth_host_observer_stub.h"
#include "bluetooth_host_proxy.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "bluetooth_observer_list.h"
#include "bluetooth_remote_device_observer_stub.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
struct BluetoothHost::impl {
    impl();
    ~impl();

    // host observer
    class BluetoothHostObserverImp;
    sptr<BluetoothHostObserverImp> observerImp_ = nullptr;
    sptr<BluetoothHostObserverImp> bleObserverImp_ = nullptr;

    // remote device observer
    class BluetoothRemoteDeviceObserverImp;
    sptr<BluetoothRemoteDeviceObserverImp> remoteObserverImp_ = nullptr;

    // remote device observer
    class BluetoothBlePeripheralCallbackImp;
    sptr<BluetoothBlePeripheralCallbackImp> bleRemoteObserverImp_ = nullptr;

    // user regist observers
    BluetoothObserverList<BluetoothHostObserver> observers_;

    // user regist remote observers
    BluetoothObserverList<BluetoothRemoteDeviceObserver> remoteObservers_;

    sptr<IBluetoothHost> proxy_ = nullptr;

private:
    void GetHostProxy();
    class BluetoothHostDeathRecipient;
    sptr<BluetoothHostDeathRecipient> deathRecipient_ = nullptr;
    std::mutex proxyMutex_;
};

class BluetoothHost::impl::BluetoothHostObserverImp : public BluetoothHostObserverStub {
public:
    BluetoothHostObserverImp(BluetoothHost::impl &host) : host_(host){};
    ~BluetoothHostObserverImp() override{};

    void Register(std::shared_ptr<BluetoothHostObserver> &observer)
    {
        host_.observers_.Register(observer);
    }

    void Deregister(std::shared_ptr<BluetoothHostObserver> &observer)
    {
        host_.observers_.Deregister(observer);
    }

    void OnStateChanged(int32_t transport, int32_t status) override
    {
        HILOGI("bluetooth state, transport: %{public}s, status: %{public}s",
            GetBtTransportName(transport).c_str(), GetBtStateName(status).c_str());
        host_.observers_.ForEach([transport, status](std::shared_ptr<BluetoothHostObserver> observer) {
            observer->OnStateChanged(transport, status);
        });
    }

    void OnDiscoveryStateChanged(int32_t status) override
    {
        HILOGI("enter, status: %{public}d", status);
        host_.observers_.ForEach(
            [status](std::shared_ptr<BluetoothHostObserver> observer) { observer->OnDiscoveryStateChanged(status); });
    }

    void OnDiscoveryResult(const BluetoothRawAddress &device) override
    {
        HILOGI("enter, device: %{public}s", GetEncryptAddr((device).GetAddress()).c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), BTTransport::ADAPTER_BREDR);
        host_.observers_.ForEach([remoteDevice](std::shared_ptr<BluetoothHostObserver> observer) {
            observer->OnDiscoveryResult(remoteDevice);
        });
    }

    void OnPairRequested(const int32_t transport, const BluetoothRawAddress &device) override
    {
        HILOGI("enter, transport: %{public}d, device: %{public}s",
            transport, GetEncryptAddr((device).GetAddress()).c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), transport);
        host_.observers_.ForEach([remoteDevice](std::shared_ptr<BluetoothHostObserver> observer) {
            observer->OnPairRequested(remoteDevice);
        });
    }

    void OnPairConfirmed(const int32_t transport, const BluetoothRawAddress &device, int reqType, int number) override
    {
        HILOGI("enter, transport: %{public}d, device: %{public}s, reqType: %{public}d, number: %{public}d",
            transport, GetEncryptAddr((device).GetAddress()).c_str(), reqType, number);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), transport);
        host_.observers_.ForEach([remoteDevice, reqType, number](std::shared_ptr<BluetoothHostObserver> observer) {
            observer->OnPairConfirmed(remoteDevice, reqType, number);
        });
    }

    void OnScanModeChanged(int mode) override
    {
        HILOGI("enter, mode: %{public}d", mode);
        host_.observers_.ForEach(
            [mode](std::shared_ptr<BluetoothHostObserver> observer) { observer->OnScanModeChanged(mode); });
    }

    void OnDeviceNameChanged(const std::string &deviceName) override
    {
        HILOGI("enter, deviceName: %{public}s", deviceName.c_str());
        host_.observers_.ForEach([deviceName](std::shared_ptr<BluetoothHostObserver> observer) {
            observer->OnDeviceNameChanged(deviceName);
        });
    }

    void OnDeviceAddrChanged(const std::string &address) override
    {
        HILOGI("enter");
        host_.observers_.ForEach(
            [address](std::shared_ptr<BluetoothHostObserver> observer) { observer->OnDeviceAddrChanged(address); });
    }

private:
    BluetoothHost::impl &host_;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothHostObserverImp);
};

class BluetoothHost::impl::BluetoothRemoteDeviceObserverImp : public BluetoothRemoteDeviceObserverstub {
public:
    BluetoothRemoteDeviceObserverImp(BluetoothHost::impl &host) : host_(host){};
    ~BluetoothRemoteDeviceObserverImp() override = default;

    void OnPairStatusChanged(const int32_t transport, const BluetoothRawAddress &device, int32_t status) override
    {
        HILOGI("enter, transport: %{public}d, device: %{public}s, status: %{public}d",
            transport, GetEncryptAddr((device).GetAddress()).c_str(), status);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), transport);

        host_.remoteObservers_.ForEach([remoteDevice, status](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
            observer->OnPairStatusChanged(remoteDevice, status);
        });
    }

    void OnRemoteUuidChanged(const BluetoothRawAddress &device, const std::vector<bluetooth::Uuid> uuids) override
    {
        HILOGI("enter, device: %{public}s", GetEncryptAddr((device).GetAddress()).c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), BTTransport::ADAPTER_BREDR);
        host_.remoteObservers_.ForEach([remoteDevice, uuids](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
            std::vector<ParcelUuid> parcelUuids;
            for (auto &uuid : uuids) {
                ParcelUuid parcelUuid = UUID::ConvertFrom128Bits(uuid.ConvertTo128Bits());
                parcelUuids.push_back(parcelUuid);
                observer->OnRemoteUuidChanged(remoteDevice, parcelUuids);
            }
        });
    }

    void OnRemoteNameChanged(const BluetoothRawAddress &device, const std::string deviceName) override
    {
        HILOGI("enter, device: %{public}s, deviceName: %{public}s",
            GetEncryptAddr((device).GetAddress()).c_str(), deviceName.c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), BTTransport::ADAPTER_BREDR);
        host_.remoteObservers_.ForEach(
            [remoteDevice, deviceName](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
                observer->OnRemoteNameChanged(remoteDevice, deviceName);
            });
    }

    void OnRemoteAliasChanged(const BluetoothRawAddress &device, const std::string alias) override
    {
        HILOGI("enter, device: %{public}s, alias: %{public}s",
            GetEncryptAddr((device).GetAddress()).c_str(), alias.c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), BTTransport::ADAPTER_BREDR);
        host_.remoteObservers_.ForEach([remoteDevice, alias](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
            observer->OnRemoteAliasChanged(remoteDevice, alias);
        });
    }

    void OnRemoteCodChanged(const BluetoothRawAddress &device, int32_t cod) override
    {
        HILOGI("enter, device: %{public}s, cod: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), cod);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), BTTransport::ADAPTER_BREDR);
        BluetoothDeviceClass deviceClass(cod);
        host_.remoteObservers_.ForEach(
            [remoteDevice, deviceClass](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
                observer->OnRemoteCodChanged(remoteDevice, deviceClass);
            });
    }

    void OnRemoteBatteryLevelChanged(const BluetoothRawAddress &device, int32_t batteryLevel) override
    {
        HILOGI("enter, device: %{public}s, batteryLevel: %{public}d",
            GetEncryptAddr((device).GetAddress()).c_str(), batteryLevel);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), BTTransport::ADAPTER_BREDR);
        host_.remoteObservers_.ForEach(
            [remoteDevice, batteryLevel](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
                observer->OnRemoteBatteryLevelChanged(remoteDevice, batteryLevel);
            });
    }

private:
    BluetoothHost::impl &host_;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothRemoteDeviceObserverImp);
};

class BluetoothHost::impl::BluetoothBlePeripheralCallbackImp : public BluetoothBlePeripheralObserverStub {
public:
    BluetoothBlePeripheralCallbackImp(BluetoothHost::impl &host) : host_(host){};
    ~BluetoothBlePeripheralCallbackImp() override = default;

    void OnPairStatusChanged(const int32_t transport, const BluetoothRawAddress &device, int status) override
    {
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), transport);
        host_.remoteObservers_.ForEach([remoteDevice, status](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
            observer->OnPairStatusChanged(remoteDevice, status);
        });
    }

    void OnReadRemoteRssiEvent(const BluetoothRawAddress &device, int rssi, int status) override
    {
        HILOGI("enter, device: %{public}s, rssi: %{public}d, status: %{public}d",
            GetEncryptAddr((device).GetAddress()).c_str(), rssi, status);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), BTTransport::ADAPTER_BLE);
        host_.remoteObservers_.ForEach(
            [remoteDevice, rssi, status](std::shared_ptr<BluetoothRemoteDeviceObserver> observer) {
                observer->OnReadRemoteRssiEvent(remoteDevice, rssi, status);
            });
    }

private:
    BluetoothHost::impl &host_;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothBlePeripheralCallbackImp);
};

class BluetoothHost::impl::BluetoothHostDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    BluetoothHostDeathRecipient(BluetoothHost::impl &impl) : impl_(impl)
    {};
    ~BluetoothHostDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothHostDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("bluetooth_servi died and then re-registered");
        std::lock_guard<std::mutex> lock(impl_.proxyMutex_);
        impl_.proxy_->AsObject()->RemoveDeathRecipient(impl_.deathRecipient_);
        impl_.proxy_ = nullptr;

        // Notify the upper layer that bluetooth is disabled.
        if (impl_.observerImp_ != nullptr && impl_.bleObserverImp_ != nullptr) {
            HILOGI("bluetooth_servi died and send state off to app");
            impl_.observerImp_->OnStateChanged(BTTransport::ADAPTER_BLE, BTStateID::STATE_TURN_OFF);
            impl_.bleObserverImp_->OnStateChanged(BTTransport::ADAPTER_BREDR, BTStateID::STATE_TURN_OFF);
        }

        // Re-obtain the proxy and register the observer.
        if (!ResetHostProxy()) {
            HILOGE("proxy reset failed");
            return;
        }
        HILOGE("proxy reset succeded");
    }

private:
    bool ResetHostProxy()
    {
        if (impl_.proxy_ != nullptr) {
            return true;
        }
        HILOGI("proxy is null, try to get proxy.");
        impl_.GetHostProxy();

        if (impl_.proxy_ == nullptr) {
            HILOGI("try to get proxy failed.");
            return false;
        }
        impl_.proxy_->RegisterObserver(impl_.observerImp_);
        impl_.proxy_->RegisterBleAdapterObserver(impl_.bleObserverImp_);
        impl_.proxy_->RegisterRemoteDeviceObserver(impl_.remoteObserverImp_);
        impl_.proxy_->RegisterBlePeripheralCallback(impl_.bleRemoteObserverImp_);
        impl_.proxy_->AsObject()->AddDeathRecipient(impl_.deathRecipient_);
        return true;
    }
    BluetoothHost::impl &impl_;
};

BluetoothHost::impl::impl()
{
    HILOGI("starts");
    observerImp_ = new (std::nothrow) BluetoothHostObserverImp(*this);
    if (observerImp_ == nullptr) {
        HILOGE("observerImp_ is null");
        return;
    }

    remoteObserverImp_ = new (std::nothrow) BluetoothRemoteDeviceObserverImp(*this);
    if (remoteObserverImp_ == nullptr) {
        HILOGE("remoteObserverImp_ is null");
        return;
    }

    bleRemoteObserverImp_ = new (std::nothrow) BluetoothBlePeripheralCallbackImp(*this);
    if (bleRemoteObserverImp_ == nullptr) {
        HILOGE("bleRemoteObserverImp_ is null");
        return;
    }

    bleObserverImp_ = new (std::nothrow) BluetoothHostObserverImp(*this);
    if (bleObserverImp_ == nullptr) {
        HILOGE("bleObserverImp_ is null");
        return;
    }

    GetHostProxy();

    if (proxy_ == nullptr) {
        HILOGE("proxy_ is null");
        return;
    }

    proxy_->RegisterObserver(observerImp_);
    proxy_->RegisterBleAdapterObserver(bleObserverImp_);
    proxy_->RegisterRemoteDeviceObserver(remoteObserverImp_);
    proxy_->RegisterBlePeripheralCallback(bleRemoteObserverImp_);

    deathRecipient_ = new BluetoothHostDeathRecipient(*this);
    if (deathRecipient_ == nullptr) {
        HILOGE("deathRecipient_ is null");
        return;
    }
    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
}

BluetoothHost::impl::~impl()
{
    HILOGI("starts");
    if (proxy_ == nullptr) {
        HILOGE("proxy_ is null");
        return;
    }
    proxy_->DeregisterObserver(observerImp_);
    proxy_->DeregisterBleAdapterObserver(bleObserverImp_);
    proxy_->DeregisterRemoteDeviceObserver(remoteObserverImp_);
    proxy_->DeregisterBlePeripheralCallback(bleRemoteObserverImp_);
    proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
}

void BluetoothHost::impl::GetHostProxy()
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOGE("error: no samgr");
        return;
    }

    sptr<IRemoteObject> remote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);
    if (!remote) {
        int try_time = 5;
        while ((!remote) && try_time > 0) {
            sleep(GET_PROXY_SLEEP_SEC);
            try_time--;
            remote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);
        }
    }

    if (!remote) {
        HILOGE("error:no remote");
        return;
    }

    proxy_ = iface_cast<IBluetoothHost>(remote);
    if (!proxy_) {
        HILOGE("error:no proxy");
        return;
    }
}

BluetoothHost BluetoothHost::hostAdapter_;

BluetoothHost::BluetoothHost()
{
    pimpl = std::make_unique<impl>();
    if (!pimpl) {
        HILOGE("fails: no pimpl");
    }
}

BluetoothHost::~BluetoothHost()
{}

BluetoothHost &BluetoothHost::GetDefaultHost()
{
    return hostAdapter_;
}

void BluetoothHost::RegisterObserver(BluetoothHostObserver &observer)
{
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return;
    }
    std::shared_ptr<BluetoothHostObserver> pointer(&observer, [](BluetoothHostObserver *) {});
    pimpl->observers_.Register(pointer);
}

void BluetoothHost::DeregisterObserver(BluetoothHostObserver &observer)
{
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return;
    }
    std::shared_ptr<BluetoothHostObserver> pointer(&observer, [](BluetoothHostObserver *) {});
    pimpl->observers_.Deregister(pointer);
}

bool BluetoothHost::EnableBt()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }

    return pimpl->proxy_->EnableBt();
}

bool BluetoothHost::DisableBt()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->DisableBt();
}

int BluetoothHost::GetBtState() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("failed: no pimpl");
        return INVALID_VALUE;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("failed: no proxy");
        return INVALID_VALUE;
    }
    int state = pimpl->proxy_->GetBtState();
    HILOGI("state: %{public}d", state);
    return state;
}

bool BluetoothHost::BluetoothFactoryReset()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->BluetoothFactoryReset();
}

bool BluetoothHost::IsValidBluetoothAddr(const std::string &addr)
{
    if (addr.empty() || addr.length() != ADDRESS_LENGTH) {
        return false;
    }
    for (int i = 0; i < ADDRESS_LENGTH; i++) {
        char c = addr[i];
        switch (i % ADDRESS_SEPARATOR_UNIT) {
            case 0:
            case 1:
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
                    break;
                }
                return false;
            case ADDRESS_COLON_INDEX:
            default:
                if (c == ':') {
                    break;
                }
                return false;
        }
    }
    return true;
}

BluetoothRemoteDevice BluetoothHost::GetRemoteDevice(const std::string &addr, int transport) const
{
    BluetoothRemoteDevice remoteDevice(addr, transport);
    return remoteDevice;
}

bool BluetoothHost::EnableBle()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->EnableBle();
}

bool BluetoothHost::DisableBle()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->DisableBle();
}

bool BluetoothHost::IsBleEnabled() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->IsBleEnabled();
}

std::string BluetoothHost::GetLocalAddress() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return std::string();
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return std::string();
    }
    return pimpl->proxy_->GetLocalAddress();
}

std::vector<uint32_t> BluetoothHost::GetProfileList() const
{
    HILOGI("enter");
    std::vector<uint32_t> profileList;
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return profileList;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return profileList;
    }
    profileList = pimpl->proxy_->GetProfileList();
    return profileList;
}

int BluetoothHost::GetMaxNumConnectedAudioDevices() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return INVALID_VALUE;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return INVALID_VALUE;
    }
    return pimpl->proxy_->GetMaxNumConnectedAudioDevices();
}

int BluetoothHost::GetBtConnectionState() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return INVALID_VALUE;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return INVALID_VALUE;
    }
    return pimpl->proxy_->GetBtConnectionState();
}

int BluetoothHost::GetBtProfileConnState(uint32_t profileId) const
{
    HILOGI("enter, profileId: %{public}d", profileId);
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return INVALID_VALUE;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return INVALID_VALUE;
    }
    return pimpl->proxy_->GetBtProfileConnState(profileId);
}

void BluetoothHost::GetLocalSupportedUuids(std::vector<ParcelUuid> &uuids)
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return;
    }
    std::vector<std::string> stringUuids;
    pimpl->proxy_->GetLocalSupportedUuids(stringUuids);
    for (auto uuid : stringUuids) {
        uuids.push_back(UUID::FromString(uuid));
    }
}

bool BluetoothHost::Start()
{
    /// This function only used for passthrough, so this is empty.
    return true;
}

void BluetoothHost::Stop()
{
    /// This function only used for passthrough, so this is empty.
}

BluetoothDeviceClass BluetoothHost::GetLocalDeviceClass() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return BluetoothDeviceClass(0);
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return BluetoothDeviceClass(0);
    }
    int LocalDeviceClass = pimpl->proxy_->GetLocalDeviceClass();
    return BluetoothDeviceClass(LocalDeviceClass);
}

bool BluetoothHost::SetLocalDeviceClass(const BluetoothDeviceClass &deviceClass)
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    int cod = deviceClass.GetClassOfDevice();
    return pimpl->proxy_->SetLocalDeviceClass(cod);
}

std::string BluetoothHost::GetLocalName() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return std::string();
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return std::string();
    }
    return pimpl->proxy_->GetLocalName();
}

bool BluetoothHost::SetLocalName(const std::string &name)
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->SetLocalName(name);
}

int BluetoothHost::GetBtScanMode() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return INVALID_VALUE;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return INVALID_VALUE;
    }
    return pimpl->proxy_->GetBtScanMode();
}

bool BluetoothHost::SetBtScanMode(int mode, int duration)
{
    HILOGI("enter, mode: %{public}d, duration: %{public}d", mode, duration);
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->SetBtScanMode(mode, duration);
}

int BluetoothHost::GetBondableMode(int transport) const
{
    HILOGI("enter, transport: %{public}d", transport);
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return INVALID_VALUE;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return INVALID_VALUE;
    }
    return pimpl->proxy_->GetBondableMode(transport);
}

bool BluetoothHost::SetBondableMode(int transport, int mode)
{
    HILOGI("enter, transport: %{public}d, mode: %{public}d", transport, mode);
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->SetBondableMode(transport, mode);
}

bool BluetoothHost::StartBtDiscovery()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->StartBtDiscovery();
}

bool BluetoothHost::CancelBtDiscovery()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->CancelBtDiscovery();
}

bool BluetoothHost::IsBtDiscovering(int transport) const
{
    HILOGI("enter, transport: %{public}d", transport);
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->IsBtDiscovering(transport);
}

long BluetoothHost::GetBtDiscoveryEndMillis() const
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return 0;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return 0;
    }
    return pimpl->proxy_->GetBtDiscoveryEndMillis();
}

std::vector<BluetoothRemoteDevice> BluetoothHost::GetPairedDevices(int transport) const
{
    HILOGI("enter, transport: %{public}d", transport);
    std::vector<BluetoothRemoteDevice> pairedDevices;
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return pairedDevices;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return pairedDevices;
    }
    std::vector<BluetoothRawAddress> pairedAddr = pimpl->proxy_->GetPairedDevices(transport);

    for (auto it = pairedAddr.begin(); it != pairedAddr.end(); it++) {
        BluetoothRemoteDevice device((*it).GetAddress(), transport);
        pairedDevices.emplace_back(device);
    }
    return pairedDevices;
}

bool BluetoothHost::RemovePair(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    if (!device.IsValidBluetoothRemoteDevice()) {
        HILOGE("Invalid remote device.");
        return false;
    }
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    sptr<BluetoothRawAddress> rawAddrSptr = new BluetoothRawAddress(device.GetDeviceAddr());
    return pimpl->proxy_->RemovePair(device.GetTransportType(), rawAddrSptr);
}

bool BluetoothHost::RemoveAllPairs()
{
    HILOGI("enter");
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return false;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return false;
    }
    return pimpl->proxy_->RemoveAllPairs();
}

void BluetoothHost::RegisterRemoteDeviceObserver(BluetoothRemoteDeviceObserver &observer)
{
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return;
    }
    std::shared_ptr<BluetoothRemoteDeviceObserver> pointer(&observer, [](BluetoothRemoteDeviceObserver *) {});
    pimpl->remoteObservers_.Register(pointer);
}

void BluetoothHost::DeregisterRemoteDeviceObserver(BluetoothRemoteDeviceObserver &observer)
{
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return;
    }
    std::shared_ptr<BluetoothRemoteDeviceObserver> pointer(&observer, [](BluetoothRemoteDeviceObserver *) {});
    pimpl->remoteObservers_.Deregister(pointer);
}

int BluetoothHost::GetBleMaxAdvertisingDataLength() const
{
    if (!pimpl) {
        HILOGE("fails: no pimpl");
        return INVALID_VALUE;
    }
    if (pimpl->proxy_ == nullptr) {
        HILOGE("fails: no proxy");
        return INVALID_VALUE;
    }
    return pimpl->proxy_->GetBleMaxAdvertisingDataLength();
}
}  // namespace Bluetooth
}  // namespace OHOS
