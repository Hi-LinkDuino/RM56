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
#include "bluetooth_opp_proxy.h"
#include "bluetooth_log.h"
#include "refbase.h"

namespace OHOS {
namespace Bluetooth {
ErrCode BluetoothOppProxy::SendFile(std::string &device,
    std::vector<std::string> &filePaths, std::vector<std::string> &mimeTypes, bool& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::SendFile WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteString(device)) {
        HILOGE("BluetoothOppProxy::SendFile write device error");
        return INVALID_DATA;
    }
    if (!WriteParcelableStringVector(filePaths, data)) {
        HILOGE("[SendFile] fail: write result failed");
        return INVALID_DATA;
    }
    if (!WriteParcelableStringVector(mimeTypes, data)) {
        HILOGE("[SendFile] fail: write result failed");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_SEND_FILE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothOppProxy::SendFile done fail, error: %{public}d", error);
        return error;
    }

    result = reply.ReadInt32() == NO_ERROR ? true : false;
    return result;
}

ErrCode BluetoothOppProxy::SetIncomingFileConfirmation(bool &accept, bool &result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::SetIncomingFileConfirmation WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteBool(accept)) {
        HILOGE("BluetoothOppProxy::SetIncomingFileConfirmation write bool error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_SET_INCOMING_FILE_CONFIRMATION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothOppProxy::GetDeviceState done fail, error: %{public}d", error);
        return error;
    }

    result = reply.ReadInt32() == NO_ERROR ? true : false;
    return result;
}

ErrCode BluetoothOppProxy::GetCurrentTransferInformation(BluetoothIOppTransferInformation &oppInformation)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::GetCurrentTransferInformation WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = Remote()->SendRequest(COMMAND_GET_CURRENT_TRANSFER_INFORMATION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothOppProxy::GetCurrentTransferInformation done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    std::unique_ptr<BluetoothIOppTransferInformation>
        oppInformation_(reply.ReadParcelable<BluetoothIOppTransferInformation>());
    if (oppInformation_ == nullptr) {
        return OBJECT_NULL;
    }
    oppInformation = *oppInformation_;
    return ERR_OK;
}

ErrCode BluetoothOppProxy::CancelTransfer(bool &result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::CancelTransfer WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_CANCEL_TRANSFER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothOppProxy::CancelTransfer done fail, error: %{public}d", error);
        return INVALID_DATA;
    }

    result = reply.ReadInt32() == NO_ERROR ? true : false;
    return error;
}

ErrCode BluetoothOppProxy::RegisterObserver(
    const sptr<IBluetoothOppObserver> observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::RegisterObserver WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothOppProxy::RegisterObserver error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    int error = Remote()->SendRequest(COMMAND_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothOppProxy::RegisterObserver done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    return error;
}

ErrCode BluetoothOppProxy::DeregisterObserver(
    const sptr<IBluetoothOppObserver> observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::DeregisterObserver WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothOppProxy::DeregisterObserver error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    int error = Remote()->SendRequest(COMMAND_DEREGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothOppProxy::DeregisterObserver done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    return error;
}

ErrCode BluetoothOppProxy::GetDeviceState(const BluetoothRawAddress &device, int& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::GetDeviceState WriteInterfaceToken error");
        return IPC_PROXY_TRANSACTION_ERR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothOppProxy::GetDeviceState write device error");
        return INVALID_DATA;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(COMMAND_GET_DEVICE_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothOppProxy::GetDeviceState done fail, error: %{public}d", error);
        return error;
    }

    ErrCode ec = reply.ReadInt32();
    if (FAILED(ec)) {
        return ec;
    }

    result = reply.ReadInt32();
    return ERR_OK;
}

ErrCode BluetoothOppProxy::GetDevicesByStates(const std::vector<int32_t> &states,
    std::vector<BluetoothRawAddress>& result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothOppProxy::GetDescriptor())) {
        HILOGE("BluetoothOppProxy::GetDevicesByStates WriteInterfaceToken error");
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
        HILOGE("BluetoothOppProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return INVALID_DATA;
    }
    int32_t rawAddsSize = reply.ReadInt32();
    for (int i = 0; i < rawAddsSize; i++) {
        std::unique_ptr<BluetoothRawAddress> address(reply.ReadParcelable<BluetoothRawAddress>());
        result.push_back(*address);
    }
    return ERR_OK;
}

bool BluetoothOppProxy::WriteParcelableStringVector(
    const std::vector<std::string> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOGE("write ParcelableVector failed");
        return false;
    }

    for (auto parcelable : parcelableVector) {
        if (!reply.WriteString(parcelable)) {
            HILOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

bool BluetoothOppProxy::WriteParcelableInt32Vector(
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