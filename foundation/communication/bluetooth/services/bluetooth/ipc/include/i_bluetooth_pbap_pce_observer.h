
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
#ifndef OHOS_BLUETOOTH_STANDARD_PBAP_PCE_OBSERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_PBAP_PCE_OBSERVER_INTERFACE_H

#include "../parcel/bluetooth_pbap_pce_parameter.h"
#include "../parcel/bluetooth_raw_address.h"
#include "iremote_broker.h"
#include "pbap_pce_parameter.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothPbapPceObserver : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothPbapPceObserver");

    enum Code {
        PBAP_PCE_ON_SERVICE_CONNECTION_STATE_CHANGED = 0,
        PBAP_PCE_ON_SERVICE_PASSWORD_REQUIRED,
        PBAP_PCE_ON_ACTION_COMPLETED,

    };
    virtual void OnServiceConnectionStateChanged(const BluetoothRawAddress &device, int state) = 0;
    virtual void OnServicePasswordRequired(
        const BluetoothRawAddress &device, const ::std::vector<uint8_t> &description, int8_t charset, bool fullAccess) = 0;
    virtual void OnActionCompleted(
        const BluetoothRawAddress &device, int respCode, int actionType, const BluetoothIPbapPhoneBookData &result) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PCE_OBSERVER_INTERFACE_H
