/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "bluetooth_a2dp_audio_src_observer_stub.h"
#include "bluetooth_log.h"
#include "raw_address.h"

namespace OHOS {
namespace Bluetooth {
using namespace bluetooth;
BluetoothA2dpAudioSrcObserverStub::BluetoothA2dpAudioSrcObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    funcMap_[static_cast<uint32_t>(IBluetoothA2dpSourceObserver::Code::BT_A2DP_SRC_OBSERVER_CONNECTION_STATE_CHANGED)] =
        &BluetoothA2dpAudioSrcObserverStub::OnConnectionStateChangedInner;
    funcMap_[static_cast<uint32_t>(IBluetoothA2dpSourceObserver::Code::BT_A2DP_SRC_OBSERVER_PLAYING_STATUS_CHANGED)] =
        &BluetoothA2dpAudioSrcObserverStub::OnPlayingStatusChangedInner;
    funcMap_[static_cast<uint32_t>(IBluetoothA2dpSourceObserver::Code::BT_A2DP_SRC_OBSERVER_CONFIGURATION_CHANGED)] =
        &BluetoothA2dpAudioSrcObserverStub::OnConfigurationChangedInner;
}

BluetoothA2dpAudioSrcObserverStub::~BluetoothA2dpAudioSrcObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    funcMap_.clear();
}

int BluetoothA2dpAudioSrcObserverStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothA2dpAudioSrcObserverStub::OnRemoteRequest, cmd = %{public}d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = BluetoothA2dpAudioSrcObserverStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGI("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    auto itFunc = funcMap_.find(code);
    if (itFunc != funcMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGW("BluetoothA2dpAudioSrcObserverStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothA2dpAudioSrcObserverStub::OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int state = data.ReadInt32();
    HILOGD("BluetoothA2dpAudioSrcObserverStub::OnConnectionStateChangedInner");
    OnConnectionStateChanged(RawAddress(addr), state);

    return NO_ERROR;
}

ErrCode BluetoothA2dpAudioSrcObserverStub::OnPlayingStatusChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int playingState = data.ReadInt32();
    int error = data.ReadInt32();
    HILOGD("BluetoothA2dpAudioSrcObserverStub::OnPlayingStatusChangedInner");
    OnPlayingStatusChanged(RawAddress(addr), playingState, error);

    return NO_ERROR;
}

ErrCode BluetoothA2dpAudioSrcObserverStub::OnConfigurationChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    BluetoothA2dpCodecInfo info = *data.ReadParcelable<BluetoothA2dpCodecInfo>();
    int error = data.ReadInt32();
    HILOGD("BluetoothA2dpAudioSrcObserverStub::OnConfigurationChangedInner");
    OnConfigurationChanged(RawAddress(addr), info, error);

    return NO_ERROR;
}
}
}