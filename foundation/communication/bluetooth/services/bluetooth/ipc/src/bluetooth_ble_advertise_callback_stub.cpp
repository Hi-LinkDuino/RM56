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

#include "bluetooth_ble_advertise_callback_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"

namespace OHOS {
namespace Bluetooth {
const std::map<uint32_t, std::function<ErrCode(BluetoothBleAdvertiseCallbackStub *, MessageParcel &, MessageParcel &)>>
    BluetoothBleAdvertiseCallbackStub::memberFuncMap_ = {
        {IBluetoothBleAdvertiseCallback::Code::BT_BLE_ADVERTISE_CALLBACK_AUTO_STOP_EVENT,
            std::bind(&BluetoothBleAdvertiseCallbackStub::OnAutoStopAdvEventInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {IBluetoothBleAdvertiseCallback::Code::BT_BLE_ADVERTISE_CALLBACK_RESULT_EVENT,
            std::bind(&BluetoothBleAdvertiseCallbackStub::OnStartResultEventInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
};

BluetoothBleAdvertiseCallbackStub::BluetoothBleAdvertiseCallbackStub()
{
    HILOGD("%{public}s start.", __func__);
}

BluetoothBleAdvertiseCallbackStub::~BluetoothBleAdvertiseCallbackStub()
{
    HILOGD("%{public}s start.", __func__);
}

int BluetoothBleAdvertiseCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothBleAdvertiseCallbackStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
    std::u16string descriptor = BluetoothBleAdvertiseCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGI("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return memberFunc(this, data, reply);
        }
    }

    HILOGW("BluetoothBleAdvertiseCallbackStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothBleAdvertiseCallbackStub::OnStartResultEventInner(MessageParcel &data, MessageParcel &reply)
{
    const int32_t result = static_cast<int32_t>(data.ReadInt32());
    const int32_t advHandle = static_cast<int32_t>(data.ReadInt32());
    const int32_t opcode = static_cast<int32_t>(data.ReadInt32());

    OnStartResultEvent(result, advHandle, opcode);
    return NO_ERROR;
}

ErrCode BluetoothBleAdvertiseCallbackStub::OnAutoStopAdvEventInner(MessageParcel &data, MessageParcel &reply)
{
    const int32_t advHandle = static_cast<int32_t>(data.ReadInt32());
    OnAutoStopAdvEvent(advHandle);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
