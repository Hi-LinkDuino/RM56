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

#include "bluetooth_hfp_hf.h"
#include "bluetooth_host.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "bluetooth_observer_list.h"
#include "i_bluetooth_hfp_hf.h"
#include "bluetooth_hfp_hf_observer_stub.h"
#include "i_bluetooth_host.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Bluetooth {
class HfServiceObserver : public BluetoothHfpHfObserverStub {
public:
    explicit HfServiceObserver(BluetoothObserverList<HandsFreeUnitObserver> &observers) : observers_(observers)
    {
        HILOGI("enter");
    }
    ~HfServiceObserver() override
    {
        HILOGI("enter");
    }

    void OnConnectionStateChanged(const BluetoothRawAddress &device, int32_t state) override
    {
        HILOGI("enter, device: %{public}s, state: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), state);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, state](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnConnectionStateChanged(remoteDevice, state);
        });
    }

    void OnScoStateChanged(const BluetoothRawAddress &device, int32_t state) override
    {
        HILOGI("enter, device: %{public}s, state: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), state);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, state](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnScoStateChanged(remoteDevice, state);
        });
    }

    void OnCallChanged(const BluetoothRawAddress &device,
        const BluetoothHfpHfCall &call) override
    {
        HILOGI("enter, device: %{public}s", GetEncryptAddr((device).GetAddress()).c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        UUID uuid = UUID::ConvertFrom128Bits(call.GetUuid().ConvertTo128Bits());
        HandsFreeUnitCall tmpCall(call.GetRemoteDevice(),
            call.GetId(),
            call.GetState(),
            call.GetNumber(),
            uuid,
            call.IsMultiParty(),
            call.IsOutgoing(),
            call.IsInBandRing(),
            call.GetCreationTime());
        observers_.ForEach([remoteDevice, tmpCall](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnCallChanged(remoteDevice, tmpCall);
        });
    }

    void OnSignalStrengthChanged(const BluetoothRawAddress &device, int32_t signal) override
    {
        HILOGI("enter, device: %{public}s, signal: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), signal);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, signal](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnSignalStrengthChanged(remoteDevice, signal);
        });
    }

    void OnRegistrationStatusChanged(const BluetoothRawAddress &device, int32_t status) override
    {
        HILOGI("enter, device: %{public}s, status: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), status);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, status](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnRegistrationStatusChanged(remoteDevice, status);
        });
    }

    void OnRoamingStatusChanged(const BluetoothRawAddress &device, int32_t status) override
    {
        HILOGI("enter, device: %{public}s, status: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), status);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, status](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnRoamingStatusChanged(remoteDevice, status);
        });
    }

    void OnOperatorSelectionChanged(
        const BluetoothRawAddress &device, const std::string &name) override
    {
        HILOGI("enter, device: %{public}s", GetEncryptAddr((device).GetAddress()).c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, name](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnOperatorSelectionChanged(remoteDevice, name);
        });
    }

    void OnSubscriberNumberChanged(
        const BluetoothRawAddress &device, const std::string &number) override
    {
        HILOGI("enter, device: %{public}s, number: %{public}s",
            GetEncryptAddr((device).GetAddress()).c_str(), number.c_str());
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, number](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnSubscriberNumberChanged(remoteDevice, number);
        });
    }

    void OnVoiceRecognitionStatusChanged(
        const BluetoothRawAddress &device, int32_t status) override
    {
        HILOGI("enter, device: %{public}s, status: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), status);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, status](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnVoiceRecognitionStatusChanged(remoteDevice, status);
        });
    }

    void OnInBandRingToneChanged(const BluetoothRawAddress &device, int32_t status) override
    {
        HILOGI("enter, device: %{public}s, status: %{public}d", GetEncryptAddr((device).GetAddress()).c_str(), status);
        BluetoothRemoteDevice remoteDevice(device.GetAddress(), 0);
        observers_.ForEach([remoteDevice, status](std::shared_ptr<HandsFreeUnitObserver> observer) {
            observer->OnInBandRingToneChanged(remoteDevice, status);
        });
    }

private:
    BluetoothObserverList<HandsFreeUnitObserver> &observers_;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(HfServiceObserver);
};

std::string HfpHfServiceName = "bluetooth-hfp-hf-server";

struct HandsFreeUnit::impl {
    impl();
    ~impl();

    bool ConnectSco(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->ConnectSco(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool DisconnectSco(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->DisconnectSco(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states)
    {
        HILOGI("enter");
        std::vector<BluetoothRemoteDevice> remoteDevices;
        if (proxy_ != nullptr && IS_BT_ENABLED()) {
            std::vector<BluetoothRawAddress> rawDevices;
            std::vector<int32_t> tmpStates;
            for (int state : states) {
                tmpStates.push_back((int32_t)state);
            }

            proxy_->GetDevicesByStates(tmpStates, rawDevices);
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
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->GetDeviceState(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return HFP_HF_SLC_STATE_DISCONNECTED;
    }

    int GetScoState(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->GetScoState(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return HFP_HF_SCO_STATE_DISCONNECTED;
    }

    bool SendDTMFTone(const BluetoothRemoteDevice &device, uint8_t code)
    {
        HILOGI("enter, device: %{public}s, code: %{public}d", GET_ENCRYPT_ADDR(device), code);
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->SendDTMFTone(BluetoothRawAddress(device.GetDeviceAddr()), code);
        }
        return false;
    }

    bool Connect(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && !BluetoothHost::GetDefaultHost().IsBtDiscovering() &&
            device.IsValidBluetoothRemoteDevice()) {
                return proxy_->Connect(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        HILOGE("fw return false!");
        return false;
    }

    bool Disconnect(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->Disconnect(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        HILOGE("fw return false!");
        return false;
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

    std::vector<HandsFreeUnitCall> GetExistingCalls(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        std::vector<HandsFreeUnitCall> calls;
        std::vector<BluetoothHfpHfCall> callsList;
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            proxy_->GetCurrentCallList(BluetoothRawAddress(device.GetDeviceAddr()), callsList);
            for (BluetoothHfpHfCall call : callsList) {
                UUID uuid = UUID::ConvertFrom128Bits(call.GetUuid().ConvertTo128Bits());
                HandsFreeUnitCall tmpCall(call.GetRemoteDevice(),
                    call.GetId(),
                    call.GetState(),
                    call.GetNumber(),
                    uuid,
                    call.IsMultiParty(),
                    call.IsOutgoing(),
                    call.IsInBandRing(),
                    call.GetCreationTime());
                calls.push_back(tmpCall);
            }
        }
        return calls;
    }

    bool AcceptIncomingCall(const BluetoothRemoteDevice &device, int flag)
    {
        HILOGI("enter, device: %{public}s, flag: %{public}d", GET_ENCRYPT_ADDR(device), flag);
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->AcceptIncomingCall(BluetoothRawAddress(device.GetDeviceAddr()), (int32_t)flag);
        }
        return false;
    }

    bool HoldActiveCall(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->HoldActiveCall(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool RejectIncomingCall(const BluetoothRemoteDevice &device)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->RejectIncomingCall(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool SendKeyPressed(const BluetoothRemoteDevice &device)
    {
        HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->SendKeyPressed(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool HandleIncomingCall(const BluetoothRemoteDevice &device, int flag)
    {
        HILOGI("Enter!");
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->HandleIncomingCall(BluetoothRawAddress(device.GetDeviceAddr()), (int32_t)flag);
        }
        return false;
    }

    bool HandleMultiCall(const BluetoothRemoteDevice &device, int flag, int index)
    {
        HILOGI("Enter!");
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->HandleMultiCall(BluetoothRawAddress(device.GetDeviceAddr()), (int32_t)flag, (int32_t)index);
        }
        return false;
    }

    bool DialLastNumber(const BluetoothRemoteDevice &device)
    {
        HILOGI("Enter!");
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->DialLastNumber(BluetoothRawAddress(device.GetDeviceAddr()));
        }
        return false;
    }

    bool DialMemory(const BluetoothRemoteDevice &device, int index)
    {
        HILOGD("Enter! index = %{public}d", index);
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->DialMemory(BluetoothRawAddress(device.GetDeviceAddr()), index);
        }
        return false;
    }

    bool SendVoiceTag(const BluetoothRemoteDevice &device, int index)
    {
        HILOGD("[%{public}s]: %{public}s(): Enter! index = %{public}d", __FILE__, __FUNCTION__, index);
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            return proxy_->SendVoiceTag(BluetoothRawAddress(device.GetDeviceAddr()), index);
        }
        return false;
    }

    bool FinishActiveCall(const BluetoothRemoteDevice &device, const HandsFreeUnitCall &call)
    {
        HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            bluetooth::Uuid uuid = bluetooth::Uuid::ConvertFrom128Bits(call.GetUuid().ConvertTo128Bits());
            bluetooth::HandsFreeUnitCalls calls(call.GetRemoteDevice(),
                call.GetId(),
                call.GetState(),
                call.GetNumber(),
                uuid,
                call.IsMultiParty(),
                call.IsOutgoing(),
                call.IsInBandRing(),
                call.GetCreationTime());
            return proxy_->FinishActiveCall(BluetoothRawAddress(device.GetDeviceAddr()), calls);
        }
        return false;
    }

    std::optional<HandsFreeUnitCall> StartDial(const BluetoothRemoteDevice &device, const std::string &number)
    {
        HILOGI("enter, device: %{public}s, number: %{public}s", GET_ENCRYPT_ADDR(device), number.c_str());
        if (proxy_ != nullptr && IS_BT_ENABLED() && device.IsValidBluetoothRemoteDevice()) {
            BluetoothHfpHfCall calls;
            proxy_->StartDial(BluetoothRawAddress(device.GetDeviceAddr()), number, calls);
            UUID uuid = UUID::ConvertFrom128Bits(calls.GetUuid().ConvertTo128Bits());
            HandsFreeUnitCall call(calls.GetRemoteDevice(),
                calls.GetId(),
                calls.GetState(),
                calls.GetNumber(),
                uuid,
                calls.IsMultiParty(),
                calls.IsOutgoing(),
                calls.IsInBandRing(),
                calls.GetCreationTime());
            return call;
        }
        return std::nullopt;
    }

    void RegisterObserver(std::shared_ptr<HandsFreeUnitObserver> observer)
    {
        HILOGI("enter");
        observers_.Register(observer);
    }

    void DeregisterObserver(std::shared_ptr<HandsFreeUnitObserver> observer)
    {
        HILOGI("enter");
        observers_.Deregister(observer);
    }

private:
    const static int HFP_HF_SLC_STATE_DISCONNECTED = (int)BTConnectState::DISCONNECTED;
    const static int HFP_HF_SCO_STATE_DISCONNECTED = 3;

    BluetoothObserverList<HandsFreeUnitObserver> observers_;
    HfServiceObserver serviceObserver_ {HfServiceObserver(observers_)};
    sptr<IBluetoothHfpHf> proxy_;
    class HandsFreeUnitDeathRecipient;
    sptr<HandsFreeUnitDeathRecipient> deathRecipient_;
};

class HandsFreeUnit::impl::HandsFreeUnitDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    HandsFreeUnitDeathRecipient(HandsFreeUnit::impl &impl) : impl_(impl)
    {};
    ~HandsFreeUnitDeathRecipient() final = default;
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(HandsFreeUnitDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final
    {
        HILOGI("starts");
        impl_.proxy_->AsObject()->RemoveDeathRecipient(impl_.deathRecipient_);
        impl_.proxy_ = nullptr;
    }

private:
    HandsFreeUnit::impl &impl_;
};

HandsFreeUnit::impl::impl()
{
    HILOGI("enter");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);

    if (!hostRemote) {
        HILOGE("failed: no hostRemote");
        return;
    }
    sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
    sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_HFP_HF);

    if (!remote) {
        HILOGE("failed: no remote");
        return;
    }
    HILOGI("remote obtained");

    proxy_ = iface_cast<IBluetoothHfpHf>(remote);
    proxy_->RegisterObserver(&serviceObserver_);
    deathRecipient_ = new HandsFreeUnitDeathRecipient(*this);
    proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
}

HandsFreeUnit::impl::~impl()
{
    HILOGI("enter");
    if (proxy_ == nullptr) {
        return;
    }
    proxy_->DeregisterObserver(&serviceObserver_);
    proxy_->AsObject()->RemoveDeathRecipient(deathRecipient_);
}

HandsFreeUnit::HandsFreeUnit()
{
    pimpl = std::make_unique<impl>();
}

HandsFreeUnit::~HandsFreeUnit()
{}

HandsFreeUnit *HandsFreeUnit::GetProfile()
{
    static HandsFreeUnit instance;
    return &instance;
}

bool HandsFreeUnit::ConnectSco(const BluetoothRemoteDevice &device)
{
    return pimpl->ConnectSco(device);
}

bool HandsFreeUnit::DisconnectSco(const BluetoothRemoteDevice &device)
{
    return pimpl->DisconnectSco(device);
}

std::vector<BluetoothRemoteDevice> HandsFreeUnit::GetDevicesByStates(std::vector<int> states) const
{
    return pimpl->GetDevicesByStates(states);
}

int HandsFreeUnit::GetDeviceState(const BluetoothRemoteDevice &device) const
{
    return pimpl->GetDeviceState(device);
}

int HandsFreeUnit::GetScoState(const BluetoothRemoteDevice &device) const
{
    return pimpl->GetScoState(device);
}

bool HandsFreeUnit::SendDTMFTone(const BluetoothRemoteDevice &device, uint8_t code)
{
    return pimpl->SendDTMFTone(device, code);
}

bool HandsFreeUnit::Connect(const BluetoothRemoteDevice &device)
{
    return pimpl->Connect(device);
}

bool HandsFreeUnit::Disconnect(const BluetoothRemoteDevice &device)
{
    return pimpl->Disconnect(device);
}

bool HandsFreeUnit::OpenVoiceRecognition(const BluetoothRemoteDevice &device)
{
    return pimpl->OpenVoiceRecognition(device);
}

bool HandsFreeUnit::CloseVoiceRecognition(const BluetoothRemoteDevice &device)
{
    return pimpl->CloseVoiceRecognition(device);
}

std::vector<HandsFreeUnitCall> HandsFreeUnit::GetExistingCalls(const BluetoothRemoteDevice &device)
{
    return pimpl->GetExistingCalls(device);
}

bool HandsFreeUnit::AcceptIncomingCall(const BluetoothRemoteDevice &device, int flag)
{
    return pimpl->AcceptIncomingCall(device, flag);
}

bool HandsFreeUnit::HoldActiveCall(const BluetoothRemoteDevice &device)
{
    return pimpl->HoldActiveCall(device);
}

bool HandsFreeUnit::RejectIncomingCall(const BluetoothRemoteDevice &device)
{
    return pimpl->RejectIncomingCall(device);
}

bool HandsFreeUnit::SendKeyPressed(const BluetoothRemoteDevice &device)
{
    return pimpl->SendKeyPressed(device);
}

bool HandsFreeUnit::HandleIncomingCall(const BluetoothRemoteDevice &device, int flag)
{
    return pimpl->HandleIncomingCall(device, flag);
}

bool HandsFreeUnit::HandleMultiCall(const BluetoothRemoteDevice &device, int flag, int index)
{
    return pimpl->HandleMultiCall(device, flag, index);
}

bool HandsFreeUnit::DialLastNumber(const BluetoothRemoteDevice &device)
{
    return pimpl->DialLastNumber(device);
}

bool HandsFreeUnit::DialMemory(const BluetoothRemoteDevice &device, int index)
{
    return pimpl->DialMemory(device, index);
}

bool HandsFreeUnit::SendVoiceTag(const BluetoothRemoteDevice &device, int index)
{
    return pimpl->SendVoiceTag(device, index);
}

bool HandsFreeUnit::FinishActiveCall(const BluetoothRemoteDevice &device, const HandsFreeUnitCall &call)
{
    return pimpl->FinishActiveCall(device, call);
}

std::optional<HandsFreeUnitCall> HandsFreeUnit::StartDial(
    const BluetoothRemoteDevice &device, const std::string &number)
{
    return pimpl->StartDial(device, number);
}

void HandsFreeUnit::RegisterObserver(HandsFreeUnitObserver *observer)
{
    std::shared_ptr<HandsFreeUnitObserver> observerPtr(observer, [](HandsFreeUnitObserver *) {});
    return pimpl->RegisterObserver(observerPtr);
}

void HandsFreeUnit::DeregisterObserver(HandsFreeUnitObserver *observer)
{
    std::shared_ptr<HandsFreeUnitObserver> observerPtr(observer, [](HandsFreeUnitObserver *) {});
    return pimpl->DeregisterObserver(observerPtr);
}
}  // namespace Bluetooth
}  // namespace OHOS
