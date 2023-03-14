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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SRC_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SRC_INTERFACE_H

#include <string>

#include "iremote_broker.h"
#include "i_bluetooth_a2dp_src_observer.h"
#include "../parcel/bluetooth_raw_address.h"
#include "../parcel/bluetooth_a2dp_a2dpCodecStatus.h"
#include "../parcel/bluetooth_a2dp_a2dpCodecInfo.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
class IBluetoothA2dpSrc : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothA2dpSrc");

    enum Code {
        BT_A2DP_SRC_CONNECT = 0,
        BT_A2DP_SRC_DISCONNECT,
        BT_A2DP_SRC_REGISTER_OBSERVER,
        BT_A2DP_SRC_DEREGISTER_OBSERVER,
        BT_A2DP_SRC_GET_DEVICE_BY_STATES,
        BT_A2DP_SRC_GET_DEVICE_STATE,
        BT_A2DP_SRC_GET_PLAYING_STATE,
        BT_A2DP_SRC_SET_CONNECT_STRATEGY,
        BT_A2DP_SRC_GET_CONNECT_STRATEGY,
        BT_A2DP_SRC_SET_ACTIVE_SINK_DEVICE,
        BT_A2DP_SRC_GET_ACTIVE_SINK_DEVICE,
        BT_A2DP_SRC_GET_CODEC_STATUS,
        BT_A2DP_SRC_SET_CODEC_PREFERENCE,
        BT_A2DP_SRC_SWITCH_OPTIONAL_CODECS,
        BT_A2DP_SRC_GET_OPTIONAL_CODECS_SUPPORT_STATE,
        BT_A2DP_SRC_START_PLAYING,
        BT_A2DP_SRC_SUSPEND_PLAYING,
        BT_A2DP_SRC_STOP_PLAYING,
        BT_A2DP_SRC_SET_AUDIO_CONFIGURE,
        BT_A2DP_SRC_WRITE_FRAME,
        BT_A2DP_SRC_GET_RENDER_POSITION,
    };

    virtual int Connect(const RawAddress &device) = 0;
    virtual int Disconnect(const RawAddress &device) = 0;
    virtual void RegisterObserver(const sptr<IBluetoothA2dpSourceObserver> &observer) = 0;
    virtual void DeregisterObserver(const sptr<IBluetoothA2dpSourceObserver> &observer) = 0;
    virtual std::vector<RawAddress> GetDevicesByStates(const std::vector<int32_t> &states) = 0;
    virtual int GetDeviceState(const RawAddress &device) = 0;
    virtual int GetPlayingState(const RawAddress &device) = 0;
    virtual int SetConnectStrategy(const RawAddress &device, int32_t strategy) = 0;
    virtual int GetConnectStrategy(const RawAddress &device) = 0;
    virtual int SetActiveSinkDevice(const RawAddress &device) = 0;
    virtual RawAddress GetActiveSinkDevice() = 0;
    virtual BluetoothA2dpCodecStatus GetCodecStatus(const RawAddress &device) = 0;
    virtual int SetCodecPreference(const RawAddress &device, const BluetoothA2dpCodecInfo &info) = 0;
    virtual void SwitchOptionalCodecs(const RawAddress &device, bool isEnable) = 0;
    virtual int GetOptionalCodecsSupportState(const RawAddress &device) = 0;
    virtual int StartPlaying(const RawAddress &device) = 0;
    virtual int SuspendPlaying(const RawAddress &device) = 0;
    virtual int StopPlaying(const RawAddress &device) = 0;
    virtual int WriteFrame(const uint8_t *data, uint32_t size) = 0;
    virtual void GetRenderPosition(uint16_t &delayValue, uint16_t &sendDataSize, uint32_t &timeStamp) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_INTERFACE_H