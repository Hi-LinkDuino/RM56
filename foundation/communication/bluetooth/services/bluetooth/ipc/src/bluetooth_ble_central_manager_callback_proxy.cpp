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

#include "bluetooth_ble_central_manager_callback_proxy.h"

#include "bluetooth_log.h"
#include "ipc_types.h"

namespace OHOS {
namespace Bluetooth {
BluetoothBleCentralManagerCallBackProxy::BluetoothBleCentralManagerCallBackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IBluetoothBleCentralManagerCallback>(impl)
{}
BluetoothBleCentralManagerCallBackProxy::~BluetoothBleCentralManagerCallBackProxy()
{}

void BluetoothBleCentralManagerCallBackProxy::OnScanCallback(const BluetoothBleScanResult &result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerCallBackProxy::GetDescriptor())) {
        HILOGE("[OnStartResultEvent] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(&result)) {
        HILOGE("[OnStartResultEvent] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error =
        InnerTransact(IBluetoothBleCentralManagerCallback::Code::BT_BLE_CENTRAL_MANAGER_CALLBACK, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothBleCentralManagerCallBackProxy::OnScanCallback done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothBleCentralManagerCallBackProxy::OnBleBatchScanResultsEvent(std::vector<BluetoothBleScanResult> &results)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerCallBackProxy::GetDescriptor())) {
        HILOGE("[OnBleBatchScanResultsEvent] fail: write interface token failed.");
        return;
    }

    int32_t size = results.size();
    if (!data.WriteInt32(size)) {
        HILOGE("[OnBleBatchScanResultsEvent] fail: write size token failed.");
        return;
    }

    for (auto &result : results) {
        if (!data.WriteParcelable(&result)) {
            HILOGE("write ParcelableVector failed");
            return;
        }
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    int error = InnerTransact(
        IBluetoothBleCentralManagerCallback::Code::BT_BLE_CENTRAL_MANAGER_BLE_BATCH_CALLBACK, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE(
            "BluetoothBleCentralManagerCallBackProxy::OnBleBatchScanResultsEvent done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothBleCentralManagerCallBackProxy::OnStartOrStopScanEvent(int resultCode, bool isStartScan)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerCallBackProxy::GetDescriptor())) {
        HILOGE("write interface token failed.");
        return;
    }

    if (!data.WriteInt32(resultCode)) {
        HILOGE("write resultCode failed");
        return;
    }
    if (!data.WriteBool(isStartScan)) {
        HILOGE("write isStartScan failed");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        IBluetoothBleCentralManagerCallback::Code::BT_BLE_CENTRAL_MANAGER_CALLBACK_SCAN_FAILED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("InnerTransact error: %{public}d", error);
        return;
    }
}

ErrCode BluetoothBleCentralManagerCallBackProxy::InnerTransact(
    uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOGW("[InnerTransact] fail: get Remote fail code %{public}d", code);
        return OBJECT_NULL;
    }
    int err = remote->SendRequest(code, data, reply, flags);
    switch (err) {
        case NO_ERROR: {
            return NO_ERROR;
        }
        case DEAD_OBJECT: {
            HILOGW("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return DEAD_OBJECT;
        }
        default: {
            HILOGW("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return TRANSACTION_ERR;
        }
    }
}
}  // namespace Bluetooth
}  // namespace OHOS