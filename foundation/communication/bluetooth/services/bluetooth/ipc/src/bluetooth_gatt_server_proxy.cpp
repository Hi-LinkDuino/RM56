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

#include "bluetooth_gatt_server_proxy.h"
#include "bluetooth_log.h"
#include "bluetooth_errorcode.h"

namespace OHOS {
namespace Bluetooth {
int BluetoothGattServerProxy::AddService(int32_t appId, BluetoothGattService *services)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::AddService WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattServerProxy::AddService error");
        return ERROR;
    }

    if (!data.WriteParcelable(services)) {
        HILOGE("BluetoothGattServerProxy::AddService error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_ADD_SERVICE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::AddService done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
void BluetoothGattServerProxy::ClearServices(int appId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::ClearServices WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattServerProxy::ClearServices error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_CLEAR_SERVICES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::ClearServices done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerProxy::CancelConnection(const BluetoothGattDevice &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::CancelConnection WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerProxy::CancelConnection error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_CANCEL_CONNECTION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::CancelConnection done fail, error: %d", error);
        return;
    }
    return;
}
int BluetoothGattServerProxy::RegisterApplication(const sptr<IBluetoothGattServerCallback> &callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::RegisterApplication WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOGE("BluetoothGattServerProxy::RegisterApplication error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_REGISTER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::RegisterApplication done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
int BluetoothGattServerProxy::DeregisterApplication(int appId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::WriteInterfaceToken WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattServerProxy::DeregisterApplication error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_DEREGISTER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::DeregisterApplication done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
int BluetoothGattServerProxy::NotifyClient(
    const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, bool needConfirm)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::WriteInterfaceToken WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerProxy::NotifyClient error");
        return ERROR;
    }
    if (!data.WriteParcelable(characteristic)) {
        HILOGE("BluetoothGattServerProxy::NotifyClient error");
        return ERROR;
    }
    if (!data.WriteBool(needConfirm)) {
        HILOGE("BluetoothGattServerProxy::NotifyClient error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_NOTIFY_CLIENT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::NotifyClient done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
int BluetoothGattServerProxy::RemoveService(int32_t appId, const BluetoothGattService &services)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::RemoveService WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattServerProxy::RemoveService error");
        return ERROR;
    }
    if (!data.WriteParcelable(&services)) {
        HILOGE("BluetoothGattServerProxy::RemoveService error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_REMOVE_SERVICE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::RemoveService done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
int BluetoothGattServerProxy::RespondCharacteristicRead(
    const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, int32_t ret)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicRead WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicRead error");
        return ERROR;
    }
    if (!data.WriteParcelable(characteristic)) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicRead error");
        return ERROR;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicRead error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_RESPOND_CHARACTERISTIC_READ, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicRead done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
int BluetoothGattServerProxy::RespondCharacteristicWrite(
    const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic, int32_t ret)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicWrite WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicWrite error");
        return ERROR;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicWrite error");
        return ERROR;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicWrite error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattServer::Code::GATT_SERVER_RESPOND_CHARACTERISTIC_WRITE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::RespondCharacteristicWrite done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
int BluetoothGattServerProxy::RespondDescriptorRead(
    const BluetoothGattDevice &device, BluetoothGattDescriptor *descriptor, int32_t ret)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorRead WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorRead error");
        return ERROR;
    }
    if (!data.WriteParcelable(descriptor)) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorRead error");
        return ERROR;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorRead error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_RESPOND_DESCRIPTOR_READ, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorRead done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
int BluetoothGattServerProxy::RespondDescriptorWrite(
    const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor, int32_t ret)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorWrite WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorWrite error");
        return ERROR;
    }
    if (!data.WriteParcelable(&descriptor)) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorWrite error");
        return ERROR;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorWrite error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattServer::Code::GATT_SERVER_RESPOND_DESCRIPTOR_WRITE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerProxy::RespondDescriptorWrite done fail, error: %d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}
}  // namespace Bluetooth
}  // namespace OHOS