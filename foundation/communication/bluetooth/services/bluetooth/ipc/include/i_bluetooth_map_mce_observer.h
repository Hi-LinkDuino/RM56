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
#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_INTERFACE_H

#include "../parcel/bluetooth_map_mce_parameter.h"
#include "../parcel/bluetooth_raw_address.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothMapMceObserver : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothMapMceObserver");

    enum Code {
        MCE_ON_ACTION_COMPLETED = 0,
        MCE_ON_CONNECTION_STATE_CHANGED,
        MCE_ON_EVENT_REPORTED,
        MCE_ON_BMESSAGE_COMPLETED,
        MCE_ON_MESSAGE_LIST_COMPLETED,
        MCE_ON_CONVERSATION_LIST_COMPLETED,
    };

    virtual void OnMapActionCompleted(const BluetoothRawAddress &device, const BluetoothIProfileMapAction &action,
        int32_t status) = 0;
    virtual void OnConnectionStateChanged(const BluetoothRawAddress &device, int32_t status) = 0;
    virtual void OnMapEventReported(const BluetoothRawAddress &device,
        const BluetoothIProfileMapEventReport &report) = 0;
    virtual void OnBmessageCompleted(const BluetoothRawAddress &device, const BluetoothIProfileBMessage &bmsg,
        int32_t status) = 0;
    virtual void OnMessagesListingCompleted(const BluetoothRawAddress &device,
        const BluetoothIProfileMessagesListing &listing, int32_t status) = 0;
    virtual void OnConversationListingCompleted(const BluetoothRawAddress &device,
        const BluetoothIProfileConversationListing &listing, int32_t status) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_INTERFACE_H
