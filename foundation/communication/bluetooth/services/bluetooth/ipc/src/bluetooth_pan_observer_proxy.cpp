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
#include "bluetooth_pan_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
ErrCode BluetoothPanObserverProxy::OnConnectionStateChanged(const BluetoothRawAddress &device, int state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(BluetoothPanObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothPanObserverProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    data.WriteParcelable(&device);
    data.WriteInt16(state);
    int32_t st = Remote()->SendRequest(COMMAND_ON_CONNECTION_STATE_CHANGED, data, reply, option);
    if (st != ERR_NONE) {
        HILOGE("OnConnectionStateChanged failed, error code is %{public}d", st);
        return st;
    }

    ErrCode ec = reply.ReadInt32();
    if (FAILED(ec)) {
        return ec;
    }

    return ERR_OK;
}
}  // namespace Bluetooth
}  // namespace OHOS