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

#include "bluetooth_def.h"
#include "bluetooth_hitrace.h"
#include "bluetooth_log.h"
#include "bluetooth_utils_server.h"
#include "hisysevent.h"
#include "interface_profile_manager.h"
#include "interface_profile_a2dp_src.h"
#include "remote_observer_list.h"
#include "interface_adapter_manager.h"
#include "permission_utils.h"
#include "bluetooth_a2dp_source_server.h"

namespace OHOS {
namespace Bluetooth {
class A2dpSourceObserver : public IA2dpObserver {
public:
    A2dpSourceObserver() = default;
    ~A2dpSourceObserver() override = default;

    void OnConnectionStateChanged(const RawAddress &device, int state) override
    {
        HILOGI("addr: %{public}s, state: %{public}d", GET_ENCRYPT_ADDR(device), state);
        if (state == static_cast<int>(BTConnectState::CONNECTED) ||
            state == static_cast<int>(BTConnectState::DISCONNECTED)) {
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::BLUETOOTH, "A2DP_CONNECTED_STATE",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "STATE", state);
        }
        observers_->ForEach([device, state](sptr<IBluetoothA2dpSourceObserver> observer) {
            observer->OnConnectionStateChanged(device, state);
        });
    }

    void OnPlayingStatusChaned(const RawAddress &device, int playingState, int error) override
    {
        HILOGI("addr: %{public}s, state: %{public}d, error: %{public}d",
            GET_ENCRYPT_ADDR(device), playingState, error);
        observers_->ForEach([device, playingState, error](sptr<IBluetoothA2dpSourceObserver> observer) {
            observer->OnPlayingStatusChanged(device, playingState, error);
        });
    }

    void OnConfigurationChanged(const RawAddress &device, const A2dpSrcCodecInfo &info, int error) override
    {
        HILOGI("addr: %{public}s, error: %{public}d", GET_ENCRYPT_ADDR(device), error);
        observers_->ForEach([device, info, error](sptr<IBluetoothA2dpSourceObserver> observer) {
            BluetoothA2dpCodecInfo  tmpInfo {};
            tmpInfo.bitsPerSample = info.bitsPerSample;
            tmpInfo.channelMode = info.channelMode;
            tmpInfo.codecPriority = info.codecPriority;
            tmpInfo.codecType = info.codecType;
            tmpInfo.sampleRate = info.sampleRate;
            tmpInfo.codecSpecific1 = info.codecSpecific1;
            tmpInfo.codecSpecific2 = info.codecSpecific2;
            tmpInfo.codecSpecific3 = info.codecSpecific3;
            tmpInfo.codecSpecific4 = info.codecSpecific4;

            observer->OnConfigurationChanged(device, tmpInfo, error);
        });
    }

    void SetObserver(RemoteObserverList<IBluetoothA2dpSourceObserver> *observers)
    {
        observers_ = observers;
    }

private:
    RemoteObserverList<IBluetoothA2dpSourceObserver> *observers_;
};

struct BluetoothA2dpSourceServer::impl {
    impl();
    ~impl();

    /// sys state observer
    class SystemStateObserver;
    std::unique_ptr<SystemStateObserver> systemStateObserver_ = nullptr;

    RemoteObserverList<IBluetoothA2dpSourceObserver> observers_;
    std::unique_ptr<A2dpSourceObserver> observerImp_{std::make_unique<A2dpSourceObserver>()};
    IProfileA2dpSrc *a2dpSrcService_ = nullptr;
};

class BluetoothA2dpSourceServer::impl::SystemStateObserver : public ISystemStateObserver {
public:
    SystemStateObserver(BluetoothA2dpSourceServer::impl *pimpl) : pimpl_(pimpl) {};
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
                    pimpl_->a2dpSrcService_ =
                        (IProfileA2dpSrc *)serviceMgr->GetProfileService(PROFILE_NAME_A2DP_SRC);
                    if (pimpl_->a2dpSrcService_ != nullptr) {
                        pimpl_->a2dpSrcService_->RegisterObserver(pimpl_->observerImp_.get());
                    }
                }
                break;
            case BTSystemState::OFF:
                pimpl_->a2dpSrcService_ = nullptr;
                break;
            default:
                break;
        }
    }

private:
    BluetoothA2dpSourceServer::impl *pimpl_ = nullptr;
};

BluetoothA2dpSourceServer::impl::impl()
{
    HILOGI("starts");
}

BluetoothA2dpSourceServer::impl::~impl()
{
    HILOGI("starts");
}

BluetoothA2dpSourceServer::BluetoothA2dpSourceServer()
{
    HILOGI("starts");
    pimpl = std::make_unique<impl>();
    pimpl->observerImp_->SetObserver(&(pimpl->observers_));
    pimpl->systemStateObserver_ = std::make_unique<impl::SystemStateObserver>(pimpl.get());
    IAdapterManager::GetInstance()->RegisterSystemStateObserver(*(pimpl->systemStateObserver_));

    IProfileManager *serviceMgr = IProfileManager::GetInstance();
    if (serviceMgr != nullptr) {
        pimpl->a2dpSrcService_ = (IProfileA2dpSrc *)serviceMgr->GetProfileService(PROFILE_NAME_A2DP_SRC);
        if (pimpl->a2dpSrcService_ != nullptr) {
            pimpl->a2dpSrcService_->RegisterObserver(pimpl->observerImp_.get());
        }
    }
}

BluetoothA2dpSourceServer::~BluetoothA2dpSourceServer()
{
    HILOGI("starts");
    IAdapterManager::GetInstance()->DeregisterSystemStateObserver(*(pimpl->systemStateObserver_));
    if (pimpl->a2dpSrcService_ != nullptr) {
        pimpl->a2dpSrcService_->DeregisterObserver(pimpl->observerImp_.get());
    }
}

void BluetoothA2dpSourceServer::RegisterObserver(const sptr<IBluetoothA2dpSourceObserver> &observer)
{
    HILOGI("starts");
    if (observer == nullptr) {
        HILOGI("observer is null");
        return;
    }
    pimpl->observers_.Register(observer);
    if (pimpl->a2dpSrcService_ == nullptr) {
        return;
    }
    // During A2DP HDF Registration, check the current status and callback
    RawAddress device = GetActiveSinkDevice();
    int state = GetDeviceState((const RawAddress &)device);
    if (state == static_cast<int>(BTConnectState::CONNECTED)) {
        HILOGI("onConnectionStateChanged");
        observer->OnConnectionStateChanged(device, state);
    }
}

void BluetoothA2dpSourceServer::DeregisterObserver(const sptr<IBluetoothA2dpSourceObserver> &observer)
{
    HILOGI("starts");
    if (observer == nullptr) {
        HILOGE("observer is null");
        return;
    }

    if (pimpl != nullptr) {
        pimpl->observers_.Deregister(observer);
    }
}

int BluetoothA2dpSourceServer::Connect(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("Connect error, check permission failed");
        return BT_FAILURE;
    }
    OHOS::Bluetooth::BluetoothHiTrace::BluetoothStartAsyncTrace("A2DP_SRC_CONNECT", 1);
    int result = pimpl->a2dpSrcService_->Connect(device);
    OHOS::Bluetooth::BluetoothHiTrace::BluetoothFinishAsyncTrace("A2DP_SRC_CONNECT", 1);
    return result;
}

int BluetoothA2dpSourceServer::Disconnect(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyDiscoverBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("Disconnect error, check permission failed");
        return BT_FAILURE;
    }
    return pimpl->a2dpSrcService_->Disconnect(device);
}

int BluetoothA2dpSourceServer::GetDeviceState(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return BT_FAILURE;
    }
    return pimpl->a2dpSrcService_->GetDeviceState(device);
}

std::vector<RawAddress> BluetoothA2dpSourceServer::GetDevicesByStates(const std::vector<int32_t> &states)
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

    rawDevices = pimpl->a2dpSrcService_->GetDevicesByStates(tmpStates);
    return rawDevices;
}

int BluetoothA2dpSourceServer::GetPlayingState(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    if (PermissionUtils::VerifyUseBluetoothPermission() == PERMISSION_DENIED) {
        HILOGE("false, check permission failed");
        return BT_FAILURE;
    }
    return pimpl->a2dpSrcService_->GetPlayingState(device);
}

int BluetoothA2dpSourceServer::SetConnectStrategy(const RawAddress &device, int strategy)
{
    HILOGI("addr: %{public}s, strategy: %{public}d", GET_ENCRYPT_ADDR(device), strategy);
    return pimpl->a2dpSrcService_->SetConnectStrategy(device, strategy);
}

int BluetoothA2dpSourceServer::GetConnectStrategy(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSrcService_->GetConnectStrategy(device);
}

int BluetoothA2dpSourceServer::SetActiveSinkDevice(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSrcService_->SetActiveSinkDevice(device);
}

RawAddress BluetoothA2dpSourceServer::GetActiveSinkDevice()
{
    HILOGI("starts");
    return pimpl->a2dpSrcService_->GetActiveSinkDevice();
}

BluetoothA2dpCodecStatus BluetoothA2dpSourceServer::GetCodecStatus(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    bluetooth::RawAddress addr(device.GetAddress());
    bluetooth::A2dpSrcCodecStatus ret;
    BluetoothA2dpCodecStatus codeStatus;
    BluetoothA2dpCodecInfo serviceInfo;
    ret = pimpl->a2dpSrcService_->GetCodecStatus(addr);

    codeStatus.codecInfo.codecPriority = ret.codecInfo.codecPriority;
    codeStatus.codecInfo.codecType = ret.codecInfo.codecType;
    codeStatus.codecInfo.sampleRate = ret.codecInfo.sampleRate;
    codeStatus.codecInfo.bitsPerSample = ret.codecInfo.bitsPerSample;
    codeStatus.codecInfo.channelMode = ret.codecInfo.channelMode;
    codeStatus.codecInfo.codecSpecific1 = ret.codecInfo.codecSpecific1;
    codeStatus.codecInfo.codecSpecific2 = ret.codecInfo.codecSpecific2;
    codeStatus.codecInfo.codecSpecific3 = ret.codecInfo.codecSpecific3;
    codeStatus.codecInfo.codecSpecific4 = ret.codecInfo.codecSpecific4;

    for (auto it = ret.codecInfoConfirmedCap.begin(); it != ret.codecInfoConfirmedCap.end(); it++) {
        serviceInfo.codecPriority = it->codecPriority;
        serviceInfo.codecType = it->codecType;
        serviceInfo.sampleRate = it->sampleRate;
        serviceInfo.bitsPerSample = it->bitsPerSample;
        serviceInfo.channelMode = it->channelMode;
        serviceInfo.codecSpecific1 = it->codecSpecific1;
        serviceInfo.codecSpecific2 = it->codecSpecific2;
        serviceInfo.codecSpecific3 = it->codecSpecific3;
        serviceInfo.codecSpecific4 = it->codecSpecific4;
        codeStatus.codecInfoConfirmCap.push_back(serviceInfo);
    }

    for (auto it = ret.codecInfoLocalCap.begin(); it != ret.codecInfoLocalCap.end(); it++) {
        serviceInfo.codecPriority = it->codecPriority;
        serviceInfo.codecType = it->codecType;
        serviceInfo.sampleRate = it->sampleRate;
        serviceInfo.bitsPerSample = it->bitsPerSample;
        serviceInfo.channelMode = it->channelMode;
        serviceInfo.codecSpecific1 = it->codecSpecific1;
        serviceInfo.codecSpecific2 = it->codecSpecific2;
        serviceInfo.codecSpecific3 = it->codecSpecific3;
        serviceInfo.codecSpecific4 = it->codecSpecific4;
        codeStatus.codecInfoLocalCap.push_back(serviceInfo);
    }

    return codeStatus;
}

int BluetoothA2dpSourceServer::SetCodecPreference(const RawAddress &device, const BluetoothA2dpCodecInfo &info)
{
    HILOGI("BluetoothA2dpSourceServer::SetCodecPreference starts, codecPriority = %{public}u,"
           "codecPriority = %{public}u, sampleRate = %{public}u, bitsPerSample = %{public}d, "
           "channelMode = %{public}d, codecSpecific1 = %{public}llu, codecSpecific2 = %{public}llu, "
           "codecSpecific3 = %{public}llu, codecSpecific4 = %{public}llu",
           info.codecPriority, info.codecType, info.sampleRate, info.bitsPerSample, info.channelMode,
           (unsigned long long)info.codecSpecific1, (unsigned long long)info.codecSpecific2,
           (unsigned long long)info.codecSpecific3, (unsigned long long)info.codecSpecific4);
    bluetooth::A2dpSrcCodecInfo setInfo;

    setInfo.bitsPerSample = info.bitsPerSample;
    setInfo.channelMode = info.channelMode;
    setInfo.codecPriority = info.codecPriority;
    setInfo.codecType = info.codecType;
    setInfo.sampleRate = info.sampleRate;
    setInfo.codecSpecific1 = info.codecSpecific1;
    setInfo.codecSpecific2 = info.codecSpecific2;
    setInfo.codecSpecific3 = info.codecSpecific3;
    setInfo.codecSpecific4 = info.codecSpecific4;

    return pimpl->a2dpSrcService_->SetCodecPreference(device, setInfo);
}

void BluetoothA2dpSourceServer::SwitchOptionalCodecs(const RawAddress &device, bool isEnable)
{
    HILOGI("addr: %{public}s, isEnable = %{public}d", GET_ENCRYPT_ADDR(device), isEnable);
    pimpl->a2dpSrcService_->SwitchOptionalCodecs(device, isEnable);
}

int BluetoothA2dpSourceServer::GetOptionalCodecsSupportState(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSrcService_->GetOptionalCodecsSupportState(device);
}

int BluetoothA2dpSourceServer::StartPlaying(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSrcService_->StartPlaying(device);
}

int BluetoothA2dpSourceServer::SuspendPlaying(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSrcService_->SuspendPlaying(device);
}

int BluetoothA2dpSourceServer::StopPlaying(const RawAddress &device)
{
    HILOGI("addr: %{public}s", GET_ENCRYPT_ADDR(device));
    return pimpl->a2dpSrcService_->StopPlaying(device);
}

int BluetoothA2dpSourceServer::WriteFrame(const uint8_t *data, uint32_t size)
{
    HILOGI("size = %{public}u", size);
    return pimpl->a2dpSrcService_->WriteFrame(data, size);
}

void BluetoothA2dpSourceServer::GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp)
{
    HILOGI("starts");
    pimpl->a2dpSrcService_->GetRenderPosition(delayValue, sendDataSize, timeStamp);
    HILOGI("delayValue = %{public}hu, sendDataSize = %{public}hu, timeStamp = %{public}u", delayValue, sendDataSize, 
        timeStamp);
}
    
}  // namespace Bluetooth
}  // namespace OHOS
