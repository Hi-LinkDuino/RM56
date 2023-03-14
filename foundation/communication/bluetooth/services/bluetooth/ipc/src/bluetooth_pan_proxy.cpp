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
#include "bluetooth_pan_proxy.h"
#include "bluetooth_log.h"
#include "refbase.h"

namespace OHOS {
namespace Bluetooth {
ErrCode BluetoothPanProxy::Disconnect(const BluetoothRawAddress &device, bool& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPanProxy::GetDescriptor())) {
        HILOGE("BluetoothPanProxy::Disconnect WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPanProxy::Disconnect write device error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPanProxy::Disconnect done fail, error: %{public}d", error);
        return error;
    }

    result = reply.ReadInt32() == NO_ERROR ? true : false;
    return ERR_OK;
}

ErrCode BluetoothPanProxy::GetDeviceState(const BluetoothRawAddress &device, int& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPanProxy::GetDescriptor())) {
        HILOGE("BluetoothPanProxy::GetDeviceState WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPanProxy::GetDeviceState write device error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_GET_DEVICE_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPanProxy::GetDeviceState done fail, error: %{public}d", error);
        return error;
    }

    ErrCode ec = reply.ReadInt32();
    if (FAILED(ec)) {
        return ec;
    }

    result = reply.ReadInt32();
    return ERR_OK;
}

ErrCode BluetoothPanProxy::GetDevicesByStates(const std::vector<int32_t> &states,
    std::vector<BluetoothRawAddress>& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPanProxy::GetDescriptor())) {
        HILOGE("BluetoothPanProxy::GetDevicesByStates WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!WriteParcelableInt32Vector(states, data)) {
        HILOGE("[GetDevicesByStates] fail: write result failed");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = Remote()->SendRequest(COMMAND_GET_DEVICES_BY_STATES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPanProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    int32_t rawAddsSize = reply.ReadInt32();
    for (int i = 0; i < rawAddsSize; i++) {
        std::unique_ptr<BluetoothRawAddress> address(reply.ReadParcelable<BluetoothRawAddress>());
        result.push_back(*address);
    }
    return ERR_OK;
}

ErrCode BluetoothPanProxy::RegisterObserver(
    const sptr<IBluetoothPanObserver> observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPanProxy::GetDescriptor())) {
        HILOGE("BluetoothPanProxy::RegisterObserver WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPanProxy::RegisterObserver error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    int error = Remote()->SendRequest(COMMAND_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPanProxy::RegisterObserver done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    return error;
}

ErrCode BluetoothPanProxy::DeregisterObserver(
    const sptr<IBluetoothPanObserver> observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPanProxy::GetDescriptor())) {
        HILOGE("BluetoothPanProxy::DeregisterObserver WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPanProxy::DeregisterObserver error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    int error = Remote()->SendRequest(COMMAND_DEREGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPanProxy::DeregisterObserver done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    return error;
}

ErrCode BluetoothPanProxy::SetTethering(
    const bool value, bool& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPanProxy::GetDescriptor())) {
        HILOGE("BluetoothPanProxy::SetTethering WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteBool(value)) {
        HILOGE("BluetoothPanProxy::SetTethering error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_SET_TETHERING, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPanProxy::SetTethering done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    ErrCode ec = reply.ReadInt32();
    if (FAILED(ec)) {
        return ec;
    }

    result = reply.ReadInt32();
    return error;
}

ErrCode BluetoothPanProxy::IsTetheringOn(
    bool& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPanProxy::GetDescriptor())) {
        HILOGE("BluetoothPanProxy::IsTetheringOn WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_IS_TETHERING_ON, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPanProxy::IsTetheringOn done fail, error: %{public}d", error);
        return error;
    }

    ErrCode ec = reply.ReadInt32();
    if (FAILED(ec)) {
        return ec;
    }

    result = reply.ReadInt32();
    return ERR_OK;
}

bool BluetoothPanProxy::WriteParcelableInt32Vector(
    const std::vector<int32_t> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOGE("write ParcelableVector failed");
        return false;
    }

    for (auto parcelable : parcelableVector) {
        if (!reply.WriteInt32(parcelable)) {
            HILOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS