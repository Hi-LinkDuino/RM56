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
#ifndef OHOS_BLUETOOTH_IBLUETOOTHHIDHOSTOBSERVER_H
#define OHOS_BLUETOOTH_IBLUETOOTHHIDHOSTOBSERVER_H

#include <string_ex.h>
#include <iremote_broker.h>
#include "../parcel/bluetooth_raw_address.h"
#include "ipc_types.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;

class IBluetoothHidHostObserver : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Bluetooth.IBluetoothHidHostObserver");

    virtual ErrCode OnConnectionStateChanged(const BluetoothRawAddress &device, int state) = 0;

protected:
    static constexpr int COMMAND_ON_CONNECTION_STATE_CHANGED = MIN_TRANSACTION_ID + 0;
};
} // Bluetooth
} // OHOS

#endif // OHOS_BLUETOOTH_IBLUETOOTHHIDHOSTOBSERVER_H

