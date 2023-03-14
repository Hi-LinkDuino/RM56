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
#include <memory>
#include <mutex>

#include "bluetooth_def.h"
#include "bluetooth_avrcp_tg_proxy.h"
#include "bluetooth_avrcp_tg_observer_stub.h"
#include "bluetooth_host.h"
#include "bluetooth_host_proxy.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "bluetooth_observer_list.h"
#include "iservice_registry.h"
#include "raw_address.h"
#include "system_ability_definition.h"
#include "bluetooth_avrcp_tg.h"

namespace OHOS {
namespace Bluetooth {
struct AvrcpTarget::impl {
public:
    class ObserverImpl : public BluetoothAvrcpTgObserverStub {
    public:
        explicit ObserverImpl(AvrcpTarget::impl *impl) : impl_(impl)
        {}
        ~ObserverImpl() override = default;

        void OnConnectionStateChanged(const BluetoothRawAddress &addr, int32_t state) override
        {
            HILOGI("enter, address: %{public}s, state: %{public}d", GetEncryptAddr(addr.GetAddress()).c_str(), state);
            BluetoothRemoteDevice device(addr.GetAddress(), BTTransport::ADAPTER_BREDR);
            impl_->OnConnectionStateChanged(device, static_cast<int>(state));

            return;
        }

    private:
        AvrcpTarget::impl *impl_;
    };

    class AvrcpTgDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AvrcpTgDeathRecipient(AvrcpTarget::impl &avrcpTgServer) : avrcpTgServer_(avrcpTgServer) {};
        ~AvrcpTgDeathRecipient() final = default;
        BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(AvrcpTgDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final
        {
            HILOGI("starts");
            avrcpTgServer_.proxy_->AsObject()->RemoveDeathRecipient(avrcpTgServer_.deathRecipient_);
            avrcpTgServer_.proxy_ = nullptr;
        }

    private:
        AvrcpTarget::impl &avrcpTgServer_;
    };

    impl()
    {
        HILOGI("enter");

        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> hostRemote = samgr->GetSystemAbility(BLUETOOTH_HOST_SYS_ABILITY_ID);
        if (!hostRemote) {
            HILOGI("failed: no hostRemote");
            return;
        }

        sptr<IBluetoothHost> hostProxy = iface_cast<IBluetoothHost>(hostRemote);
        sptr<IRemoteObject> remote = hostProxy->GetProfile(PROFILE_AVRCP_TG);
        if (!remote) {
            HILOGI("failed: no remote");
            return;
        }
        HILOGI("remote obtained");
        proxy_ = iface_cast<IBluetoothAvrcpTg>(remote);

        deathRecipient_ = new AvrcpTgDeathRecipient(*this);
        if (!proxy_) {
            HILOGI("proxy_ is null when used!");
        } else {
            proxy_->AsObject()->AddDeathRecipient(deathRecipient_);
        }

        observer_ = new (std::nothrow) ObserverImpl(this);
        proxy_->RegisterObserver(observer_);
    }

    ~impl()
    {
        HILOGI("enter");
        proxy_->UnregisterObserver(observer_);
    }

    bool IsEnabled(void)
    {
        HILOGI("enter");

        return (proxy_ != nullptr && !BluetoothHost::GetDefaultHost().IsBtDiscovering());
    }

    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
    {
        HILOGI("enter, device: %{public}s, state: %{public}d", GET_ENCRYPT_ADDR(device), state);
        std::lock_guard<std::mutex> lock(observerMutex_);
        observers_.ForEach([device, state](std::shared_ptr<IObserver> observer) {
            observer->OnConnectionStateChanged(device, state);
        });
    }

    std::mutex observerMutex_;
    sptr<IBluetoothAvrcpTg> proxy_;
    BluetoothObserverList<AvrcpTarget::IObserver> observers_;
    sptr<ObserverImpl> observer_;
    sptr<AvrcpTgDeathRecipient> deathRecipient_;
};

AvrcpTarget *AvrcpTarget::GetProfile(void)
{
    HILOGI("enter");

    static AvrcpTarget instance;

    return &instance;
}

/******************************************************************
 * REGISTER / UNREGISTER OBSERVER                                 *
 ******************************************************************/

void AvrcpTarget::RegisterObserver(AvrcpTarget::IObserver *observer)
{
    HILOGI("enter");

    std::lock_guard<std::mutex> lock(pimpl->observerMutex_);
    if (pimpl->IsEnabled()) {
        std::shared_ptr<IObserver> observerPtr(observer, [](IObserver *) {});
        pimpl->observers_.Register(observerPtr);
    }
}

void AvrcpTarget::UnregisterObserver(AvrcpTarget::IObserver *observer)
{
    HILOGI("enter");

    std::lock_guard<std::mutex> lock(pimpl->observerMutex_);

    std::shared_ptr<IObserver> observerPtr(observer, [](IObserver *) {});
    if (pimpl->IsEnabled()) {
        pimpl->observers_.Deregister(observerPtr);
    }
}

/******************************************************************
 * CONNECTION                                                     *
 ******************************************************************/

void AvrcpTarget::SetActiveDevice(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));

    if (pimpl->IsEnabled()) {
        BluetoothRawAddress rawAddr(device.GetDeviceAddr());
        pimpl->proxy_->SetActiveDevice(rawAddr);
    }
}

std::vector<BluetoothRemoteDevice> AvrcpTarget::GetConnectedDevices(void)
{
    HILOGI("enter");

    std::vector<BluetoothRemoteDevice> devices;

    if (pimpl->IsEnabled()) {
        std::vector<BluetoothRawAddress> rawAddrs = pimpl->proxy_->GetConnectedDevices();

        for (auto rawAddr : rawAddrs) {
            BluetoothRemoteDevice device(rawAddr.GetAddress(), BTTransport::ADAPTER_BREDR);
            devices.push_back(device);
        }
    }

    return devices;
}

std::vector<BluetoothRemoteDevice> AvrcpTarget::GetDevicesByStates(std::vector<int> states)
{
    HILOGI("enter");

    std::vector<BluetoothRemoteDevice> devices;

    if (pimpl->IsEnabled()) {
        std::vector<int32_t> convertStates;
        for (auto state : states) {
            convertStates.push_back(static_cast<int32_t>(state));
        }
        std::vector<BluetoothRawAddress> rawAddrs = pimpl->proxy_->GetDevicesByStates(convertStates);

        for (auto rawAddr : rawAddrs) {
            BluetoothRemoteDevice device(rawAddr.GetAddress(), BTTransport::ADAPTER_BREDR);
            devices.push_back(device);
        }
    }

    return devices;
}

int AvrcpTarget::GetDeviceState(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));

    int32_t result = static_cast<int32_t>(BTConnectState::DISCONNECTED);

    if (pimpl->IsEnabled()) {
        BluetoothRawAddress rawAddr(device.GetDeviceAddr());
        result = pimpl->proxy_->GetDeviceState(rawAddr);
    }

    return static_cast<int>(result);
}

bool AvrcpTarget::Connect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));

    int result = RET_BAD_STATUS;

    if (pimpl->IsEnabled()) {
        BluetoothRawAddress rawAddr(device.GetDeviceAddr());
        result = pimpl->proxy_->Connect(rawAddr);
    }

    return result == RET_NO_ERROR ? true : false;
}

bool AvrcpTarget::Disconnect(const BluetoothRemoteDevice &device)
{
    HILOGI("enter, device: %{public}s", GET_ENCRYPT_ADDR(device));

    int result = RET_BAD_STATUS;

    if (pimpl->IsEnabled()) {
        BluetoothRawAddress rawAddr(device.GetDeviceAddr());
        result = pimpl->proxy_->Disconnect(rawAddr);
    }

    return result == RET_NO_ERROR ? true : false;
}

/******************************************************************
 * NOTIFICATION                                                   *
 ******************************************************************/

void AvrcpTarget::NotifyPlaybackStatusChanged(uint8_t playStatus, uint32_t playbackPos)
{
    HILOGI("enter, playStatus: %{public}d, playbackPos: %{public}d", playStatus, playbackPos);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyPlaybackStatusChanged(
            static_cast<int32_t>(playStatus), static_cast<int32_t>(playbackPos));
    }
}

void AvrcpTarget::NotifyTrackChanged(uint64_t uid, uint32_t playbackPos)
{
    HILOGI("enter, playbackPos: %{public}d", playbackPos);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyTrackChanged(static_cast<int64_t>(uid), static_cast<int32_t>(playbackPos));
    }
}

void AvrcpTarget::NotifyTrackReachedEnd(uint32_t playbackPos)
{
    HILOGI("enter, playbackPos: %{public}d", playbackPos);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyTrackReachedEnd(static_cast<int32_t>(playbackPos));
    }
}

void AvrcpTarget::NotifyTrackReachedStart(uint32_t playbackPos)
{
    HILOGI("enter, playbackPos: %{public}d", playbackPos);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyTrackReachedStart(static_cast<int32_t>(playbackPos));
    }
}

void AvrcpTarget::NotifyPlaybackPosChanged(uint32_t playbackPos)
{
    HILOGI("enter, playbackPos: %{public}d", playbackPos);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyPlaybackPosChanged(static_cast<int32_t>(playbackPos));
    }
}

void AvrcpTarget::NotifyPlayerAppSettingChanged(
    const std::vector<uint8_t> &attributes, const std::vector<uint8_t> &values)
{
    HILOGI("enter");

    std::vector<int32_t> attrs;
    std::vector<int32_t> vals;

    for (auto attribute : attributes) {
        attrs.push_back(attribute);
    }
    for (auto value : values) {
        vals.push_back(value);
    }

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyPlayerAppSettingChanged(attrs, vals);
    }
}

void AvrcpTarget::NotifyNowPlayingContentChanged(void)
{
    HILOGI("enter");

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyNowPlayingContentChanged();
    }
}

void AvrcpTarget::NotifyAvailablePlayersChanged(void)
{
    HILOGI("enter");

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyAvailablePlayersChanged();
    }
}

void AvrcpTarget::NotifyAddressedPlayerChanged(uint16_t playerId, uint16_t uidCounter)
{
    HILOGI("enter, playerId: %{public}d, uidCounter: %{public}d", playerId, uidCounter);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyAddressedPlayerChanged(
            static_cast<int32_t>(playerId), static_cast<int32_t>(uidCounter));
    }
}

void AvrcpTarget::NotifyUidChanged(uint16_t uidCounter)
{
    HILOGI("enter, uidCounter: %{public}d", uidCounter);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyUidChanged(static_cast<int32_t>(uidCounter));
    }
}

void AvrcpTarget::NotifyVolumeChanged(uint8_t volume)
{
    HILOGI("enter, volume: %{public}d", volume);

    if (pimpl->IsEnabled()) {
        pimpl->proxy_->NotifyVolumeChanged(static_cast<int32_t>(volume));
    }
}

AvrcpTarget::AvrcpTarget(void)
{
    HILOGI("enter");

    pimpl = std::make_unique<impl>();
}

AvrcpTarget::~AvrcpTarget(void)
{
    HILOGI("enter");

    pimpl->proxy_->AsObject()->RemoveDeathRecipient(pimpl->deathRecipient_);
    pimpl = nullptr;
}
} // namespace Bluetooth
} // namespace OHOS
