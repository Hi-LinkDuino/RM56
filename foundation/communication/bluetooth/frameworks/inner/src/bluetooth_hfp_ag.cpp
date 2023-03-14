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

#include "bluetooth_hfp_ag.h"
#include <unistd.h>
#include "bluetooth_device.h"
#include "bluetooth_host.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "bluetooth_observer_list.h"
#include "i_bluetooth_hfp_ag.h"
#include "bluetooth_hfp_ag_observer_stub.h"
#include "i_bluetooth_host.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
class AgServiceObserver : public BluetoothHfpAgObserverStub {
public:
    explicit AgServiceObserver(BluetoothObserverList<HandsFreeAudioGatewayObserver> &observers) : observers_(observers)
    {
        HILOGI("enter");
    }
    ~AgServiceObserver() override
    {
        HILOGI("enter");
    };

    void OnConnectionStateChanged(const BluetoothRawAddress &device, int32_t state) override
    {
        HILOGI("hfpAg conn state, device: %{public}s, state: %{public}s",
            GetEncryptAddr((device).GetAddress()).c_str(), GetProfileConnStateName(state).c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, state](std::shared_ptr<HandsFreeAudioGatewayObserver> observer) {
            observer->OnConnectionStateChanged(remoteDevice, state);
        });
    }

    void OnScoStateChanged(const BluetoothRawAddress &device, int32_t state) override
    {
        HILOGI("enter, device: %{public}s, state: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), state);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, state](std::shared_ptr<HandsFreeAudioGatewayObserver> observer) {
            observer->OnScoStateChanged(remoteDevice, state);
        });
    }

    void OnActiveDeviceChanged(const BluetoothRawAddress &device) override
    {
        HILOGI("enter, device: %{public}s", GetEncryptAddr((device).GetAddress()).c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice](std::shared_ptr<HandsFreeAudioGatewayObserver> observer) {
            observer->OnActiveDeviceChanged(remoteDevice);
        });
    }

    void OnHfEnhancedDriverSafetyChanged(
        const BluetoothRawAddress &device, int32_t indValue) override
    {
        HILOGI("enter, device: %{public}s, indValue: %{public}d",
            GetEncryptAddr((device).GetAddress()).c_str(), indValue);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, indValue](std::shared_ptr<HandsFreeAudioGatewayObserver> observer) {
            observer->OnHfEnhancedDriverSafetyChanged(remoteDevice, indValue);
        });
    }

private:
    BluetoothObserverList<HandsFreeAudioGatewayObserver> &observers_;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(AgServiceObserver);
};

std::string HfpAgServiceName = "bluetooth-hfp-ag-server";

struct HandsFreeAudioGateway::impl {
    impl();
    ~impl();
    std::vector<BluetoothRemoteDevice> GetConnectedDevices()
    {
        HILOGI("enter");
        std::vector<BluetoothRemoteDevice> devices;
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            std::vector<BluetoothRawAddress> ori;
            proxy_->GetConnectDevices(ori);
            for (auto it = ori.begin(); it != ori.end(); it++) {
                devices.push_back(BluetoothRemoteDevice(it->GetAddress(), 0));
            }
        }
        return devices;
    }

    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states)
    {
        HILOGI("enter");
        std::vector<BluetoothRemoteDevice> remoteDevices;
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            std::vector<BluetoothRawAddress> rawDevices;
            std::vector<int32_t> tmpstates;
            for (int state : states) {
                int32_t value = (int32_t)state;
                tmpstates.push_back(value);
            }
            proxy_->GetDevicesByStates(tmpstates, rawDevices);
            for (BluetoothRawAddress rawDevice : rawDevices) {
                BluetoothRemoteDevice remoteDevice(rawDevice.GetAddress(), 0);
                remoteDevices.push_back(remoteDevice);
            }
        }
        return remoteDevices;
    }

    int GetDeviceState(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));

        if (proxy_ == nullptr) {
            HILOGE("proxy_ is nullptr");
            return HFP_AG_SLC_STATE_DISCONNECTED;
        }

        if (!IS_BT_ENABLED()) {
            HILOGE("Not IS_BT_ENABLED");
            return HFP_AG_SLC_STATE_DISCONNECTED;
        }

        if (device.IsValidBluetoothRemoteDevice()) {
            return proxy_->GetDeviceState(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return HFP_AG_SLC_STATE_DISCONNECTED;
    }

    bool Connect(const BluetoothRemoteDevice &device)
    {
        HILOGI("hfp connect remote device: %{public}s", GET_ENCRYPT_ADDR(device));

        if (proxy_ == nullptr) {
            HILOGE("proxy_ is nullptr");
            return false;
        }

        if (!IS_BT_ENABLED()) {
            HILOGE("Not IS_BT_ENABLED");
            return false;
        }

        if (!device.IsValidBluetoothRemoteDevice()) {
            HILOGE("Addr is invalid");
            return false;
        }

        if (!device.GetDeviceClass().IsProfileSupported(BluetoothDevice::PROFILE_HEADSET)) {
            HILOGE("hfp connect failed. The remote device does not support HFP service.");
            return false;
        }

        if (proxy_->Connect(BluetoothRawAddress(device.GetDeviceAddr())) == NO_ERROR) {
            return true;
        }

        return false;
    }

    bool Disconnect(const BluetoothRemoteDevice &device)
    {
        HILOGI("hfp disconnect remote device: %{public}s", GET_ENCRYPT_ADDR(device));

        if (proxy_ == nullptr) {
            HILOGE("proxy_ == nullptr");
            return false;
        }

        if (!IS_BT_ENABLED())
        {
            HILOGE("Not IS_BT_ENABLED");
            return false;
        }

        if (!device.IsValidBluetoothRemoteDevice()) {
            HILOGE("Addr is invalid");
            return false;
        }

        if (proxy_->Disconnect(BluetoothRawAddress(device.GetDeviceAddr())) == NO_ERROR) {
            return true;
        }
        return false;
    }

    int GetScoState(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->GetScoState(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return HFP_AG_SCO_STATE_DISCONNECTED;
    }

    bool ConnectSco()
    {
        HILOGI("enter");
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            return proxy_->ConnectSco();
        }
        return false;
    }

    bool DisconnectSco()
    {
        HILOGI("enter");
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            return proxy_->DisconnectSco();
        }
        return false;
    }

    void PhoneStateChanged(
        int numActive, int numHeld, int callState, const std::string &number, int type, const std::string &name)
    {
        HILOGI("numActive: %{public}d, numHeld: %{public}d, callState: %{public}d, type: %{public}d",
            numActive, numHeld, callState, type);
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            proxy_->PhoneStateChanged(numActive, numHeld, callState, number, type, name);
        }
    }

    void ClccResponse(int index, int direction, int status, int mode, bool mpty, std::string number, int type)
    {
        HILOGI("enter, index: %{public}d, direction: %{public}d, status: %{public}d, mode: %{public}d, mpty: "
            "%{public}d, type: %{public}d", index, direction, status, mode, mpty, type);
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            proxy_->ClccResponse(index, direction, status, mode, mpty, number, type);
        }
    }

    bool OpenVoiceRecognition(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->OpenVoiceRecognition(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool CloseVoiceRecognition(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->CloseVoiceRecognition(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool SetActiveDevice(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->SetActiveDevice(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool IntoMock(const BluetoothRemoteDevice &device, int state)
    {
        HILOGI("enter");
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            return proxy_->IntoMock(device.GetDeviceAddr(), state);
        }
        return false;
    }

    bool SendNoCarrier(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter");
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->SendNoCarrier(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    BluetoothRemoteDevice GetActiveDevice()
    {
        HILOGI("enter");
        BluetoothRemoteDevice device;
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            std::string address = proxy_->GetActiveDevice();
            BluetoothRemoteDevice remoteDevice(address, 0);
            device = remoteDevice;
        }
        return device;
    }

    void RegisterObserver(std::shared_ptr<HandsFreeAudioGatewayObserver> observer)
    {
        HILOGI("enter");
        observers_.Register(observer);
    }

    void DeregisterObserver(std::shared_ptr<HandsFreeAudioGatewayObserver> observer)
    {
        HILOGI("enter");
        observers_.Deregister(observer);
        HILOGI("end");
    }
    
private:
    const static int HFP_AG_SLC_STATE_DISCONNECTED = (int)BTConnectState::DISCONNECTED;
    const static int HFP_AG_SCO_STATE_DISCONNECTED = 3;

    BluetoothObserverList<HandsFreeAudioGatewayObserver> observers_;
    sptr<AgServiceObserver> serviceObserver_;
    sptr<IBluetoothHfpAg> proxy_;
    class HandsFreeAudioGatewayDeathRecipient;
    sptr<HandsFreeAudioGatewayDeathRecipient> deathRecipient_;

    void GetHfpAgProxy()
    {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            HILOGE("error: no samgr");
            return;
        }

        sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);
        if (hostRemote == nullptr) {
            HILOGE("failed: no hostRemote");
            return;
        }

        sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
        if (hostProxy == nullptr) {
            HILOGE("error: host no proxy");
            return;
        }

        sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_HFP_AG);
        if (remote == nullptr) {
            HILOGE("failed: no remote");
            return;
        }

        proxy_ = iface_cast<IBluetoothHfpAg>(remote);
        if (proxy_ == nullptr) {
            HILOGE("error: no proxy");
            return;
        }
    }
};

class HandsFreeAudioGateway::impl::HandsFreeAudioGatewayDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    HandsFreeAudioGatewayDeathRecipient(HandsFreeAudioGateway::impl &impl) : impl_(impl)
    {};
    ~HandsFreeAudioGatewayDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(HandsFreeAudioGatewayDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("starts");
        impl_.proxy_->AsObject()->RemoveDeathRecipient(impl_.deathRecipient_);
        impl_.proxy_ = nullptr;

        // Re-obtain the proxy and register the observer.
        sleep(GET_PROXY_SLEEP_SEC);
        impl_.GetHfpAgProxy();
        if (impl_.proxy_ == nullptr) {
            HILOGE("proxy reset failed");
            return;
        }
        if (impl_.serviceObserver_ == nullptr || impl_.deathRecipient_ == nullptr) {
            HILOGE("serviceObserver_ or deathRecipient_ is null");
            return;
        }
        impl_.proxy_->RegisterObserver(impl_.serviceObserver_);
        impl_.proxy_->AsObject()->AddDeathRecipient(impl_.deathRecipient_);
    }

private:
    HandsFreeAudioGateway::impl &impl_;
};

HandsFreeAudioGateway::impl::impl()
{
    HILOGI("start");
    GetHfpAgProxy();
    if (proxy_ == nullptr) {
        HILOGE("get proxy_ failed");
        return;
    }
    serviceObserver_ = new AgServiceObserver(observers_);
    if (serviceObserver_ == nullptr) {
        HILOGE("serviceObserver_ is null");
        return;
    }
    proxy_->RegisterObserver(serviceObserver_);

    deathRecipient_ = new HandsFreeAudioGatewayDeathRecipient(*this);
    if (deathRecipient_ == nullptr) {
        HILOGE("deathRecipient_ is null");
        return;
    }
    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
}

HandsFreeAudioGateway::impl::~impl()
{
    HILOGI("enter");
    if (proxy_ == nullptr) {
        return;
    }
    proxy_->DeregisterObserver(serviceObserver_);
    proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
}

HandsFreeAudioGateway *HandsFreeAudioGateway::GetProfile()
{
    HILOGI("enter");
    static HandsFreeAudioGateway instance;
    return &instance;
}

HandsFreeAudioGateway::HandsFreeAudioGateway()
{
    HILOGI("enter");
    pimpl = std::make_unique<impl>();
}

HandsFreeAudioGateway::~HandsFreeAudioGateway()
{
    HILOGI("enter");
}

std::vector<BluetoothRemoteDevice> HandsFreeAudioGateway::GetConnectedDevices() const
{
    HILOGI("enter");
    return pimpl->GetConnectedDevices();
}

std::vector<BluetoothRemoteDevice> HandsFreeAudioGateway::GetDevicesByStates(std::vector<int> states)
{
    HILOGI("enter");
    return pimpl->GetDevicesByStates(states);
}

int HandsFreeAudioGateway::GetDeviceState(const BluetoothRemoteDevice &device) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->GetDeviceState(device);
}

bool HandsFreeAudioGateway::Connect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->Connect(device);
}

bool HandsFreeAudioGateway::Disconnect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->Disconnect(device);
}

int HandsFreeAudioGateway::GetScoState(const BluetoothRemoteDevice &device) const
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->GetScoState(device);
}

bool HandsFreeAudioGateway::ConnectSco()
{
    HILOGI("enter");
    return pimpl->ConnectSco();
}

bool HandsFreeAudioGateway::DisconnectSco()
{
    HILOGI("enter");
    return pimpl->DisconnectSco();
}

void HandsFreeAudioGateway::PhoneStateChanged(
    int numActive, int numHeld, int callState, const std::string &number, int type, const std::string &name)
{
    pimpl->PhoneStateChanged(numActive, numHeld, callState, number, type, name);
}

void HandsFreeAudioGateway::ClccResponse(
    int index, int direction, int status, int mode, bool mpty, const std::string &number, int type)
{
    pimpl->ClccResponse(index, direction, status, mode, mpty, number, type);
}

bool HandsFreeAudioGateway::OpenVoiceRecognition(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->OpenVoiceRecognition(device);
}

bool HandsFreeAudioGateway::CloseVoiceRecognition(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->CloseVoiceRecognition(device);
}

bool HandsFreeAudioGateway::SetActiveDevice(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->SetActiveDevice(device);
}

bool HandsFreeAudioGateway::IntoMock(const BluetoothRemoteDevice &device, int state)
{
    return pimpl->IntoMock(device, state);
}

bool HandsFreeAudioGateway::SendNoCarrier(const BluetoothRemoteDevice &device)
{
    return pimpl->SendNoCarrier(device);
}

BluetoothRemoteDevice HandsFreeAudioGateway::GetActiveDevice() const
{
    return pimpl->GetActiveDevice();
}

void HandsFreeAudioGateway::RegisterObserver(HandsFreeAudioGatewayObserver *observer)
{
    
    HILOGI("enter");
    std::shared_ptr<HandsFreeAudioGatewayObserver> observerPtr(observer, [](HandsFreeAudioGatewayObserver *) {});
    pimpl->RegisterObserver(observerPtr);
}

void HandsFreeAudioGateway::DeregisterObserver(HandsFreeAudioGatewayObserver *observer)
{
    HILOGI("enter");
    std::shared_ptr<HandsFreeAudioGatewayObserver> observerPtr(observer, [](HandsFreeAudioGatewayObserver *) {});
    if (pimpl == nullptr) {
        HILOGI("pimpl is nullptr!");
        return;
    }
    pimpl->DeregisterObserver(observerPtr);
    HILOGI("end");
}
}  // namespace Bluetooth
}  // namespace OHOS