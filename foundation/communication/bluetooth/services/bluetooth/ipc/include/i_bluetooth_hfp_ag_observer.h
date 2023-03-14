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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_AG_OBSERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_HFP_AG_OBSERVER_INTERFACE_H

#include "../parcel/bluetooth_raw_address.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothHfpAgObserver : public OHOS::IRemoteBroker {
public:

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothHfpAgObserver");

    enum Code {
        BT_HFP_AG_OBSERVER_CONNECTION_STATE_CHANGED = 0,
        BT_HFP_AG_OBSERVER_SCO_STATE_CHANGED,
        BT_HFP_AG_OBSERVER_ACTIVE_DEVICE_CHANGED,
        BT_HFP_AG_OBSERVER_HF_ENHANCED_DRIVER_SAFETY_CHANGED,
    };

    virtual void OnConnectionStateChanged(const BluetoothRawAddress &device, int state) = 0;
    virtual void OnScoStateChanged(const BluetoothRawAddress &device, int state) = 0;
    virtual void OnActiveDeviceChanged(const BluetoothRawAddress &device) = 0;
    virtual void OnHfEnhancedDriverSafetyChanged(const BluetoothRawAddress &device, int indValue) = 0;
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_AG_OBSERVER_INTERFACE_H