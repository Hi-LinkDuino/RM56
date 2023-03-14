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
#include <mutex>
#include "bluetooth_def.h"
#include "bluetooth_avrcp_tg_server.h"
#include "bluetooth_log.h"
#include "bluetooth_utils.h"
#include "interface_adapter_manager.h"
#include "interface_profile.h"
#include "interface_profile_avrcp_tg.h"
#include "interface_profile_manager.h"
#include "remote_observer_list.h"
#include "permission_utils.h"


namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;

struct BluetoothAvrcpTgServer::impl {
public:
    class SysStsObserverImpl : public ISystemStateObserver {
    public:
        explicit SysStsObserverImpl(BluetoothAvrcpTgServer::impl *impl) : impl_(impl)
        {}
        virtual ~SysStsObserverImpl()
        {}

        void OnSystemStateChange(const BTSystemState state) override
        {
            impl_->OnSystemStateChange(state);
        }

    private:
        BluetoothAvrcpTgServer::impl *impl_;
    };
    class ObserverImpl : public IProfileAvrcpTg::IObserver {
    public:
        explicit ObserverImpl(BluetoothAvrcpTgServer::impl *impl) : impl_(impl)
        {}
        virtual ~ObserverImpl()
        {}

        void OnConnectionStateChanged(const RawAddress &rawAddr, int state) override
        {
            impl_->OnConnectionStateChanged(rawAddr, state);
        }

    private:
        BluetoothAvrcpTgServer::impl *impl_;
    };

    impl()
    {
        HILOGI("start.");

        auto svManager = IProfileManager::GetInstance();
        service_ = static_cast<IProfileAvrcpTg *>(svManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
        if (service_ != nullptr) {
            observer_ = std::make_unique<ObserverImpl>(this);
            service_->RegisterObserver(observer_.get());
        }

        sysObserver_ = std::make_unique<SysStsObserverImpl>(this);
        IAdapterManager::GetInstance()->RegisterSystemStateObserver(*sysObserver_);
    }

    ~impl()
    {
        HILOGI("start.");

        auto svManager = IProfileManager::GetInstance();
        service_ = static_cast<IProfileAvrcpTg *>(svManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
        if (service_ != nullptr) {
            service_->UnregisterObserver();
            observer_ = nullptr;
            service_ = nullptr;
        }

        IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*sysObserver_);
        sysObserver_ = nullptr;
    }

    bool IsEnabled()
    {
        HILOGI("start.");

        auto servManager = IProfileManager::GetInstance();
        service_ = static_cast<IProfileAvrcpTg *>(servManager->GetProfileService(PROFILE_NAME_AVRCP_TG));

        return (service_ != nullptr && service_->IsEnabled());
    }

    void OnSystemStateChange(const BTSystemState state)
    {
        HILOGI("start.");

        std::lock_guard<std::mutex> lock(serviceMutex_);

        switch (state) {
            case BTSystemState::ON: {
                auto svManager = IProfileManager::GetInstance();
                service_ = static_cast<IProfileAvrcpTg *>(svManager->GetProfileService(PROFILE_NAME_AVRCP_TG));
                if (service_ != nullptr) {
                    observer_ = std::make_unique<ObserverImpl>(this);
                    service_->RegisterObserver(observer_.get());
                }
                break;
            }
            case BTSystemState::OFF:
                /// FALL THROUGH
            default:
                if (service_ != nullptr) {
                    service_->UnregisterObserver();
                    observer_ = nullptr;
                    service_ = nullptr;
                }
                break;
        }
    }

    void OnConnectionStateChanged(const RawAddress &rawAddr, int state)
    {
        HILOGI("res: %{public}s, state: %{public}d.", GET_ENCRYPT_AVRCP_ADDR(rawAddr), state);
        std::lock_guard<std::mutex> lock(observerMutex_);

        observers_.ForEach([rawAddr, state](IBluetoothAvrcpTgObserver *observer) {
            observer->OnConnectionStateChanged(rawAddr, static_cast<int32_t>(state));
        });
    }

    std::mutex serviceMutex_;
    IProfileAvrcpTg *service_;

    std::mutex observerMutex_;
    RemoteObserverList<IBluetoothAvrcpTgObserver> observers_;

    std::unique_ptr<ObserverImpl> observer_;
    std::unique_ptr<SysStsObserverImpl> sysObserver_;
};

BluetoothAvrcpTgServer::BluetoothAvrcpTgServer()
{
    HILOGI("start.");

    pimpl = std::make_unique<impl>();
}

BluetoothAvrcpTgServer::~BluetoothAvrcpTgServer()
{
    pimpl = nullptr;
}

void BluetoothAvrcpTgServer::RegisterObserver(const sptr<IBluetoothAvrcpTgObserver> &observer)
{
    HILOGI("start.");

    std::lock_guard<std::mutex> lock(pimpl->observerMutex_);

    if (observer == nullptr) {
        HILOGI("observer is NULL.");
        return ;
    }
    pimpl->observers_.Register(observer);
    HILOGI("end.");

    return ;
}

void BluetoothAvrcpTgServer::UnregisterObserver(const sptr<IBluetoothAvrcpTgObserver> &observer)
{
    HILOGI("start.");

    std::lock_guard<std::mutex> lock(pimpl->observerMutex_);

    if (observer == nullptr) {
        HILOGI("observer is NULL.");
        return;
    }
    pimpl->observers_.Deregister(observer);
    HILOGI("end.");

    return;
}

void BluetoothAvrcpTgServer::SetActiveDevice(const BluetoothRawAddress &addr)
{
    HILOGI("address: %{public}s", GetEncryptAddr(addr.GetAddress()).c_str());

    if (pimpl->IsEnabled()) {
        pimpl->service_->SetActiveDevice(addr);
    } else {
        HILOGE("service is null or disable ");
    }
    HILOGI("end.");
}

int32_t BluetoothAvrcpTgServer::Connect(const BluetoothRawAddress &addr)
{
    HILOGI("address: %{public}s", GetEncryptAddr(addr.GetAddress()).c_str());

    int32_t result = 0;

    if (pimpl->IsEnabled()) {
        result = pimpl->service_->Connect(addr);
    } else {
        HILOGE("service is null or disable ");
    }
    HILOGI("end.");

    return result;
}

int32_t BluetoothAvrcpTgServer::Disconnect(const BluetoothRawAddress &addr)
{
    HILOGI("address: %{public}s", GetEncryptAddr(addr.GetAddress()).c_str());

    int32_t result = 0;

    if (pimpl->IsEnabled()) {
        result = pimpl->service_->Disconnect(addr);
    } else {
        HILOGE("service is null or disable ");
    }
    HILOGI("end.");

    return result;
}

std::vector<BluetoothRawAddress> BluetoothAvrcpTgServer::GetConnectedDevices()
{
    HILOGI("start.");

    std::vector<BluetoothRawAddress> results;

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return results;
    }

    std::vector<RawAddress> devices;
    devices = pimpl->service_->GetConnectedDevices();
    for (auto device : devices) {
        BluetoothRawAddress rawAddr = BluetoothRawAddress(device);
        results.emplace_back(rawAddr);

    }
    HILOGI("end.");

    return results;
}

std::vector<BluetoothRawAddress> BluetoothAvrcpTgServer::GetDevicesByStates(const std::vector<int32_t> &states)
{
    HILOGI("start.");

    std::vector<BluetoothRawAddress> results;

    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return results;
    }
    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return results;
    }

    std::vector<RawAddress> devices;
    std::vector<int> convertStates;
    for (auto state : states) {
        HILOGI("state = %{public}d", state);
        convertStates.push_back(static_cast<int>(state));
    }

    devices = pimpl->service_->GetDevicesByStates(convertStates);
    for (auto device : devices) {
        BluetoothRawAddress rawAddr = BluetoothRawAddress(device);
        results.emplace_back(rawAddr);
    }
    HILOGI("end.");

    return results;
}

int32_t BluetoothAvrcpTgServer::GetDeviceState(const BluetoothRawAddress &addr)
{
    HILOGI("address: %{public}s", GetEncryptAddr(addr.GetAddress()).c_str());
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return BT_FAILURE;
    }
    int32_t result = 0;

    if (pimpl->IsEnabled()) {
        result = pimpl->service_->GetDeviceState(addr);
    } else {
        HILOGE("service is null or disable ");
    }
    HILOGI("end.");

    return result;
}

void BluetoothAvrcpTgServer::NotifyPlaybackStatusChanged(int32_t playStatus, int32_t playbackPos)
{
    HILOGI("playStatus: %{public}d, playbackPos: %{public}d", playStatus, playbackPos);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyPlaybackStatusChanged(static_cast<uint8_t>(playStatus),
                                                 static_cast<uint32_t>(playbackPos));
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyTrackChanged(int64_t uid, int32_t playbackPos)
{
    HILOGI("uid: %{public}jd, playbackPos: %{public}d", uid, playbackPos);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyTrackChanged(static_cast<uint64_t>(uid), static_cast<uint32_t>(playbackPos));
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyTrackReachedEnd(int32_t playbackPos)
{
    HILOGI("playbackPos: %{public}d", playbackPos);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyTrackReachedEnd(static_cast<uint32_t>(playbackPos));
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyTrackReachedStart(int32_t playbackPos)
{
    HILOGI("playbackPos: %{public}d", playbackPos);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyTrackReachedStart(static_cast<uint32_t>(playbackPos));
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyPlaybackPosChanged(int32_t playbackPos)
{
    HILOGI("playbackPos: %{public}d", playbackPos);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyPlaybackPosChanged(static_cast<uint32_t>(playbackPos));
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyPlayerAppSettingChanged(const std::vector<int32_t> &attributes,
    const std::vector<int32_t> &values)
{
    HILOGI("start.");

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    std::deque<uint8_t> attrs;
    std::deque<uint8_t> vals;

    for (auto attribute : attributes) {
        HILOGI("attributes = %{public}d", attribute);
        attrs.push_back(attribute);
    }
    for (auto value : values) {
        HILOGI("values = %{public}d", value);
        vals.push_back(value);
    }

    pimpl->service_->NotifyPlayerAppSettingChanged(attrs, vals);
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyNowPlayingContentChanged()
{
    HILOGI("start.");

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyNowPlayingContentChanged();
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyAvailablePlayersChanged()
{
    HILOGI("start.");

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyAvailablePlayersChanged();
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyAddressedPlayerChanged(int32_t playerId, int32_t uidCounter)
{
    HILOGI("playerId: %{public}d, uidCounter: %{public}d", playerId, uidCounter);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyAddressedPlayerChanged(static_cast<uint32_t>(playerId), static_cast<uint32_t>(uidCounter));
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyUidChanged(int32_t uidCounter)
{
    HILOGI("uidCounter: %{public}d", uidCounter);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyUidChanged(static_cast<uint32_t>(uidCounter));
    HILOGI("end.");
}

void BluetoothAvrcpTgServer::NotifyVolumeChanged(int32_t volume)
{
    HILOGI("volume: %{public}d", volume);

    if (!pimpl->IsEnabled()) {
        HILOGE("service is null or disable ");
        return;
    }

    pimpl->service_->NotifyVolumeChanged(static_cast<uint8_t>(volume));
    HILOGI("end.");
}

}  // namespace Bluetooth

}  // namespace OHOS