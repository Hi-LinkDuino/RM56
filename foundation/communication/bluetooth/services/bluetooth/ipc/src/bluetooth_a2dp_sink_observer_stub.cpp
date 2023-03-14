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

#include "bluetooth_a2dp_sink_observer_stub.h"
#include "bluetooth_log.h"
#include "raw_address.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
BluetoothA2dpSinkObserverStub::BluetoothA2dpSinkObserverStub()
{
    HILOGD("%{public}s start.", __func__);
}

BluetoothA2dpSinkObserverStub::~BluetoothA2dpSinkObserverStub()
{
    HILOGD("%{public}s start.", __func__);
}

int BluetoothA2dpSinkObserverStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothA2dpSinkObserverStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());

    ErrCode errCode = NO_ERROR;
    BluetoothA2dpSinkObserverStub::Code msgCode = static_cast<BluetoothA2dpSinkObserverStub::Code>(code);
    switch (msgCode) {
        case BluetoothA2dpSinkObserverStub::Code::BT_A2DP_SINK_OBSERVER_CONNECTION_STATE_CHANGED:
            errCode = OnConnectionStateChangedInner(data, reply);
            break;
        default:
            HILOGW("BluetoothA2dpSinkObserverStub::OnRemoteRequest, default case, need check.");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return errCode;
}

ErrCode BluetoothA2dpSinkObserverStub::OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    if (BluetoothA2dpSinkObserverStub::GetDescriptor() != data.ReadInterfaceToken()) {
        HILOGE("BluetoothA2dpSinkObserverStub::check interface token failed in: %{public}s.", __func__);
        return ERR_INVALID_STATE;
    }
    std::string addr = data.ReadString();
    int state = data.ReadInt32();

    OnConnectionStateChanged(RawAddress(addr), state);

    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS