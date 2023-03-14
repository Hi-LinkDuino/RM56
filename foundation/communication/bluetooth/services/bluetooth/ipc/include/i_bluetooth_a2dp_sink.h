/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SINK_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SINK_INTERFACE_H

#include <string>

#include "iremote_broker.h"
#include "i_bluetooth_a2dp_sink_observer.h"
#include "raw_address.h"
#include "../parcel/bluetooth_raw_address.h"
#include "../parcel/bluetooth_a2dp_a2dpCodecStatus.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
class IBluetoothA2dpSink : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothA2dpSink");

    enum Code {
        BT_A2DP_SINK_CONNECT = 0,
        BT_A2DP_SINK_DISCONNECT,
        BT_A2DP_SINK_REGISTER_OBSERVER,
        BT_A2DP_SINK_DEREGISTER_OBSERVER,
        BT_A2DP_SINK_GET_DEVICE_BY_STATES,
        BT_A2DP_SINK_GET_DEVICE_STATE,
        BT_A2DP_SINK_GET_PLAYING_STATE,
        BT_A2DP_SINK_SET_CONNECT_STRATEGY,
        BT_A2DP_SINK_GET_CONNECT_STRATEGY,
        BT_A2DP_SINK_SEND_DELAY,
    };

    virtual int Connect(const RawAddress &device) = 0;
    virtual int Disconnect(const RawAddress &device) = 0;
    virtual void RegisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer) = 0;
    virtual void DeregisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer) = 0;
    virtual std::vector<RawAddress> GetDevicesByStates(const std::vector<int32_t> &states) = 0;
    virtual int GetDeviceState(const RawAddress &device) = 0;
    virtual int GetPlayingState(const RawAddress &device) = 0;
    virtual int SetConnectStrategy(const RawAddress &device, int32_t strategy) = 0;
    virtual int GetConnectStrategy(const RawAddress &device) = 0;
    virtual int SendDelay(const RawAddress &device, int32_t delayValue) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_INTERFACE_H