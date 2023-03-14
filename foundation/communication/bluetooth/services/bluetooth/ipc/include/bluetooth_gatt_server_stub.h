/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_SERVER_STUB_H
#define OHOS_BLUETOOTH_STANDARD_GATT_SERVER_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_gatt_server.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothGattServerStub : public IRemoteStub<IBluetoothGattServer> {
public:
    BluetoothGattServerStub();
    virtual ~BluetoothGattServerStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode AddServiceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ClearServicesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode CancelConnectionInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RegisterApplicationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterApplicationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode NotifyClientInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RemoveServiceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RespondCharacteristicReadInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RespondCharacteristicWriteInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RespondDescriptorReadInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RespondDescriptorWriteInner(MessageParcel &data, MessageParcel &reply);
    using BluetoothGattServerFunc = ErrCode (BluetoothGattServerStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothGattServerFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothGattServerStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_SERVER_STUB_H