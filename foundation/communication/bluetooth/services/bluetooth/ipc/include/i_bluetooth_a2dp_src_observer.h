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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SRC_OBSERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SRC_OBSERVER_INTERFACE_H

#include "iremote_broker.h"
#include "raw_address.h"
#include "../parcel/bluetooth_a2dp_a2dpCodecInfo.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
class IBluetoothA2dpSourceObserver : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothA2dpSourceObserver");

    enum Code {
        BT_A2DP_SRC_OBSERVER_CONNECTION_STATE_CHANGED = 0,
        BT_A2DP_SRC_OBSERVER_PLAYING_STATUS_CHANGED,
        BT_A2DP_SRC_OBSERVER_CONFIGURATION_CHANGED,
    };

    virtual void OnConnectionStateChanged(const RawAddress &device, int state) = 0;
    virtual void OnPlayingStatusChanged(const RawAddress &device, int playingState, int error) = 0;
    virtual void OnConfigurationChanged(const RawAddress &device, const BluetoothA2dpCodecInfo &info, int error) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_SRC_OBSERVER_INTERFACE_H