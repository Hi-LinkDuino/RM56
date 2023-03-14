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
#include "bluetooth_opp_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
ErrCode BluetoothOppObserverProxy::OnReceiveIncomingFileChanged(const BluetoothIOppTransferInformation &oppInformation)
{
    HILOGI("BluetoothOppObserverProxy::OnReceiveIncomingFileChanged start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(BluetoothOppObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothOppObserverProxy::OnReceiveIncomingFileChanged WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    data.WriteParcelable(&oppInformation);
    int32_t st = Remote()->SendRequest(COMMAND_ON_RECEIVE_INCOMING_FILE_CHANGED, data, reply, option);
    if (st != ERR_NONE) {
        HILOGE("OnReceiveIncomingFileChanged failed, error code is %{public}d", st);
        return st;
    }

    ErrCode ec = reply.ReadInt32();
    if (FAILED(ec)) {
        return ec;
    }

    return ERR_OK;
}

ErrCode BluetoothOppObserverProxy::OnTransferStateChanged(const BluetoothIOppTransferInformation &oppInformation)
{
    HILOGI("BluetoothOppObserverProxy::OnTransferStateChanged start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(BluetoothOppObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothOppObserverProxy::OnTransferStateChanged WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    data.WriteParcelable(&oppInformation);
    int32_t st = Remote()->SendRequest(COMMAND_ON_TRANSFER_STATE_CHANGED, data, reply, option);
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