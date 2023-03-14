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

#include "bluetooth_ble_central_manager_proxy.h"
#include "bluetooth_log.h"
#include "parcel_bt_uuid.h"

namespace OHOS {
namespace Bluetooth {
BluetoothBleCentralManagerProxy::BluetoothBleCentralManagerProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IBluetoothBleCentralManager>(impl)
{}

BluetoothBleCentralManagerProxy::~BluetoothBleCentralManagerProxy()
{}

void BluetoothBleCentralManagerProxy::RegisterBleCentralManagerCallback(
    const sptr<IBluetoothBleCentralManagerCallback> &callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[RegisterBleCentralManagerCallback] fail: write interface token failed.");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOGW("[RegisterBleCentralManagerCallback] fail: write callback failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_REGISTER_BLE_CENTRAL_MANAGER_CALLBACK, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[RegisterBleCentralManagerCallback] fail: transact ErrCode=%{public}d", result);
    }
}

void BluetoothBleCentralManagerProxy::DeregisterBleCentralManagerCallback(
    const sptr<IBluetoothBleCentralManagerCallback> &callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[DeregisterBleCentralManagerCallback] fail: write interface token failed.");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOGW("[DeregisterBleCentralManagerCallback] fail: write callback failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_DE_REGISTER_BLE_CENTRAL_MANAGER_CALLBACK, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[DeregisterBleCentralManagerCallback] fail: transact ErrCode=%{public}d", result);
    }
}

void BluetoothBleCentralManagerProxy::StartScan()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[StartScan] fail: write interface token failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_START_SCAN, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[StartScan] fail: transact ErrCode=%{public}d", result);
    }
}

void BluetoothBleCentralManagerProxy::StartScan(const BluetoothBleScanSettings &settings)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[StartScan] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(&settings)) {
        HILOGW("[StartScan] fail:write settings failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_START_SCAN_WITH_SETTINGS, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[StartScan] fail: transact ErrCode=%{public}d", result);
    }
}

void BluetoothBleCentralManagerProxy::StopScan()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[StopScan] fail: write interface token failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_STOP_SCAN, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[StopScan] fail: transact ErrCode=%{public}d", result);
    }
}

int BluetoothBleCentralManagerProxy::ConfigScanFilter(
    const int clientId, const std::vector<BluetoothBleScanFilter> &filters)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[ConfigScanFilter] fail: write interface token failed.");
        return 0;
    }

    if (!data.WriteInt32(clientId)) {
        HILOGE("[ConfigScanFilter] fail: write clientId failed");
        return 0;
    }

    if (!data.WriteInt32(filters.size())) {
        HILOGE("[ConfigScanFilter] fail: write vector size failed");
        return 0;
    }
    for (uint32_t i = 0; i < filters.size(); i++) {
        if (!data.WriteParcelable(&filters[i])) {
            HILOGE("[ConfigScanFilter] fail: write filter failed");
            return 0;
        }
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_CONFIG_SCAN_FILTER, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[ConfigScanFilter] fail: transact ErrCode=%{public}d", result);
        return 0;
    }
    return reply.ReadInt32();
}

void BluetoothBleCentralManagerProxy::RemoveScanFilter(const int clientId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[RemoveScanFilter] fail: write interface token failed.");
        return;
    }

    if (!data.WriteInt32(clientId)) {
        HILOGE("[RemoveScanFilter] fail: write clientId failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_REMOVE_SCAN_FILTER, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[RemoveScanFilter] fail: transact ErrCode=%{public}d", result);
    }
}

bool BluetoothBleCentralManagerProxy::ProxyUid(int32_t uid, bool isProxy)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[ProxyUid] fail: write interface token failed.");
        return false;
    }

    if (!data.WriteInt32(uid) || !data.WriteBool(isProxy)) {
        HILOGE("[ProxyUid] fail: write data failed");
        return false;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(BLE_PROXY_UID, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[ProxyUid] fail: transact ErrCode=%{public}d", result);
        return false;
    }
    return true;
}

bool BluetoothBleCentralManagerProxy::ResetAllProxy()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleCentralManagerProxy::GetDescriptor())) {
        HILOGW("[ResetAllProxy] fail: write interface token failed.");
        return false;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    ErrCode result = InnerTransact(BLE_RESET_ALL_PROXY, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[ResetAllProxy] fail: transact ErrCode=%{public}d", result);
        return false;
    }
    return true;
}

ErrCode BluetoothBleCentralManagerProxy::InnerTransact(
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