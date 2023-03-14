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

#include "bluetooth_ble_advertiser_proxy.h"
#include "bluetooth_log.h"
#include "parcel_bt_uuid.h"

namespace OHOS {
namespace Bluetooth {
namespace {
const int32_t BLE_INVALID_ADVERTISING_HANDLE = 0xFF;
}

BluetoothBleAdvertiserProxy::BluetoothBleAdvertiserProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IBluetoothBleAdvertiser>(impl)
{}

BluetoothBleAdvertiserProxy::~BluetoothBleAdvertiserProxy()
{}

void BluetoothBleAdvertiserProxy::RegisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleAdvertiserProxy::GetDescriptor())) {
        HILOGW("[RegisterBleAdvertiserCallback] fail: write interface token failed.");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOGW("[RegisterBleAdvertiserCallback] fail: write callback failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_REGISTER_BLE_ADVERTISER_CALLBACK, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[RegisterBleAdvertiserCallback] fail: transact ErrCode=%{public}d", result);
    }
}
void BluetoothBleAdvertiserProxy::DeregisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleAdvertiserProxy::GetDescriptor())) {
        HILOGW("[DeregisterBleAdvertiserCallback] fail: write interface token failed.");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOGW("[DeregisterBleAdvertiserCallback] fail: write callback failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_DE_REGISTER_BLE_ADVERTISER_CALLBACK, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[DeregisterBleAdvertiserCallback] fail: transact ErrCode=%{public}d", result);
    }
}

void BluetoothBleAdvertiserProxy::StartAdvertising(const BluetoothBleAdvertiserSettings &settings,
    const BluetoothBleAdvertiserData &advData, const BluetoothBleAdvertiserData &scanResponse, int32_t advHandle,
    bool isRawData)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleAdvertiserProxy::GetDescriptor())) {
        HILOGW("[StartAdvertising] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(&settings)) {
        HILOGW("[StartAdvertising] fail:write settings failed");
        return;
    }

    if (!data.WriteParcelable(&advData)) {
        HILOGW("[StartAdvertising] fail:write advData failed");
        return;
    }

    if (!data.WriteParcelable(&scanResponse)) {
        HILOGW("[StartAdvertising] fail:write scanResponse failed");
        return;
    }

    if (!data.WriteInt32(advHandle)) {
        HILOGW("[StartAdvertising] fail: write advHandle failed.");
        return;
    }

    if (!data.WriteBool(isRawData)) {
        HILOGW("[StartAdvertising] fail: write isRawData failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_START_ADVERTISING, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[StartAdvertising] fail: transact ErrCode=%{public}d", result);
    }
}

void BluetoothBleAdvertiserProxy::StopAdvertising(int32_t advHandle)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleAdvertiserProxy::GetDescriptor())) {
        HILOGW("[StopAdvertising] fail: write interface token failed.");
        return;
    }

    if (!data.WriteInt32(advHandle)) {
        HILOGW("[StopAdvertising] fail: write advHandle failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_STOP_ADVERTISING, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[StopAdvertising] fail: transact ErrCode=%{public}d", result);
    }
}

void BluetoothBleAdvertiserProxy::Close(int32_t advHandle)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleAdvertiserProxy::GetDescriptor())) {
        HILOGW("[Close] fail: write interface token failed.");
        return;
    }

    if (!data.WriteInt32(advHandle)) {
        HILOGW("[Close] fail: write advHandle failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_CLOSE, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[Close] fail: transact ErrCode=%{public}d", result);
    }
}

int32_t BluetoothBleAdvertiserProxy::GetAdvertiserHandle()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothBleAdvertiserProxy::GetDescriptor())) {
        HILOGW("[GetAdvertiserHandle] fail: write interface token failed.");
        return BLE_INVALID_ADVERTISING_HANDLE;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ErrCode result = InnerTransact(BLE_GET_ADVERTISER_HANDLE, option, data, reply);
    if (result != NO_ERROR) {
        HILOGW("[GetAdvertiserHandle] fail: transact ErrCode=%{public}d", result);
        return BLE_INVALID_ADVERTISING_HANDLE;
    }
    return reply.ReadInt32();
}

ErrCode BluetoothBleAdvertiserProxy::InnerTransact(
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