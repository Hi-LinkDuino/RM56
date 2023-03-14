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

#ifndef OHOS_BLUETOOTH_STANDARD_BLE_CENTRAL_MANAGER_STUB_H
#define OHOS_BLUETOOTH_STANDARD_BLE_CENTRAL_MANAGER_STUB_H

#include <map>

#include "i_bluetooth_ble_central_manager.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothBleCentralManagerStub : public IRemoteStub<IBluetoothBleCentralManager> {
public:
    BluetoothBleCentralManagerStub();
    ~BluetoothBleCentralManagerStub() override;
    DISALLOW_COPY_AND_MOVE(BluetoothBleCentralManagerStub);

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    static const std::map<uint32_t,
        std::function<ErrCode(BluetoothBleCentralManagerStub *, MessageParcel &, MessageParcel &)>>
        interfaces_;

    ErrCode RegisterBleCentralManagerCallbackInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterBleCentralManagerCallbackInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StartScanInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StartScanWithSettingsInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StopScanInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ConfigScanFilterInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RemoveScanFilterInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ProxyUidInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ResetAllProxyInner(MessageParcel &data, MessageParcel &reply);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif