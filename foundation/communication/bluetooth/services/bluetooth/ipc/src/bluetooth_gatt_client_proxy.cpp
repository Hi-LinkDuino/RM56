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

#include "bluetooth_gatt_client_proxy.h"
#include "bluetooth_errorcode.h"
#include "bluetooth_log.h"
#include "parcel_bt_uuid.h"

namespace OHOS {
namespace Bluetooth {
int BluetoothGattClientProxy::RegisterApplication(
    const sptr<IBluetoothGattClientCallback> &callback, const BluetoothRawAddress &addr, int32_t transport)
{
    HILOGI("BluetoothGattClientProxy::RegisterApplication start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::RegisterApplication WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOGE("BluetoothGattClientProxy::RegisterApplication transport error");
        return ERROR;
    }
    if (!data.WriteParcelable(&addr)) {
        HILOGE("BluetoothGattClientProxy::RegisterApplication transport error");
        return ERROR;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothGattClientProxy::RegisterApplication transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_REGISTER_APP, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::RegisterApplication done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::DeregisterApplication(int32_t appId)
{
    HILOGI("BluetoothGattClientProxy::DeregisterApplication start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::DeregisterApplication WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::DeregisterApplication transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    HILOGE("mobaiye2 BluetoothGattClientProxy::DeregisterApplication appId : %{public}d", appId);
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_DEREGISTER_APP, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::DeregisterApplication done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::Connect(int32_t appId, bool autoConnect)
{
    HILOGI("BluetoothGattClientProxy::Connect start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::Connect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::Connect transport error");
        return ERROR;
    }
    if (!data.WriteBool(autoConnect)) {
        HILOGE("BluetoothGattClientProxy::Connect transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_CONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::Connect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::Disconnect(int32_t appId)
{
    HILOGI("BluetoothGattClientProxy::Disconnect start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::Disconnect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::Disconnect transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_DIS_CONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::Disconnect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::DiscoveryServices(int32_t appId)
{
    HILOGI("BluetoothGattClientProxy::DiscoveryServices start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::DiscoveryServices WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::DiscoveryServices transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_DISCOVERY_SERVICES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::DiscoveryServices done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::ReadCharacteristic(int32_t appId, const BluetoothGattCharacteristic &characteristic)
{
    HILOGI("BluetoothGattClientProxy::ReadCharacteristic start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::ReadCharacteristic WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::ReadCharacteristic transport error");
        return ERROR;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattClientProxy::ReadCharacteristic transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_READ_CHARACTERISTIC, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::ReadCharacteristic done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::WriteCharacteristic(
    int32_t appId, BluetoothGattCharacteristic *characteristic, bool withoutRespond)
{
    HILOGI("BluetoothGattClientProxy::WriteCharacteristic start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::WriteCharacteristic WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::WriteCharacteristic transport error");
        return ERROR;
    }
    if (!data.WriteParcelable(characteristic)) {
        HILOGE("BluetoothGattClientProxy::WriteCharacteristic transport error");
        return ERROR;
    }
    if (!data.WriteBool(withoutRespond)) {
        HILOGE("BluetoothGattClientProxy::WriteCharacteristic transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_WRITE_CHARACTERISTIC, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::WriteCharacteristic done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::SignedWriteCharacteristic(int32_t appId, BluetoothGattCharacteristic *characteristic)
{
    HILOGI("BluetoothGattClientProxy::SignedWriteCharacteristic start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::SignedWriteCharacteristic WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::SignedWriteCharacteristic transport error");
        return ERROR;
    }
    if (!data.WriteParcelable(characteristic)) {
        HILOGE("BluetoothGattClientProxy::SignedWriteCharacteristic transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClient::Code::BT_GATT_CLIENT_SIGNED_WRITE_CHARACTERISTIC, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::SignedWriteCharacteristic done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::ReadDescriptor(int32_t appId, const BluetoothGattDescriptor &descriptor)
{
    HILOGI("BluetoothGattClientProxy::ReadDescriptor start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::ReadDescriptor WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::ReadDescriptor transport error");
        return ERROR;
    }
    if (!data.WriteParcelable(&descriptor)) {
        HILOGE("BluetoothGattClientProxy::ReadDescriptor transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_READ_DESCRIPTOR, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::ReadDescriptor done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::WriteDescriptor(int32_t appId, BluetoothGattDescriptor *descriptor)
{
    HILOGI("BluetoothGattClientProxy::WriteDescriptor start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::WriteDescriptor WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::WriteDescriptor transport error");
        return ERROR;
    }
    if (!data.WriteParcelable(descriptor)) {
        HILOGE("BluetoothGattClientProxy::WriteDescriptor transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_WRITE_DESCRIPTOR, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::WriteDescriptor done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothGattClientProxy::RequestExchangeMtu(int32_t appId, int32_t mtu)
{
    HILOGI("BluetoothGattClientProxy::RequestExchangeMtu start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::RequestExchangeMtu WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::RequestExchangeMtu transport error");
        return ERROR;
    }
    if (!data.WriteInt32(mtu)) {
        HILOGE("BluetoothGattClientProxy::RequestExchangeMtu transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_REQUEST_EXCHANGE_MTU, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::RequestExchangeMtu done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothGattClientProxy::GetAllDevice(std::vector<BluetoothGattDevice> &device)
{
    HILOGI("BluetoothGattClientProxy::GetAllDevice start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::GetAllDevice WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_GET_ALL_DEVICE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::GetAllDevice done fail, error: %d", error);
    }
    int DevNum = reply.ReadInt32();
    for (int i = DevNum; i > 0; i--) {
        std::shared_ptr<BluetoothGattDevice> dev(reply.ReadParcelable<BluetoothGattDevice>());
        if (!dev) {
            return;
        }
        device.push_back(*dev);
    }
}

int BluetoothGattClientProxy::RequestConnectionPriority(int32_t appId, int32_t connPriority)
{
    HILOGI("BluetoothGattClientProxy::RequestConnectionPriority start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::RequestConnectionPriority WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::RequestConnectionPriority transport error");
        return ERROR;
    }
    if (!data.WriteInt32(connPriority)) {
        HILOGE("BluetoothGattClientProxy::RequestConnectionPriority transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClient::Code::BT_GATT_CLIENT_REQUEST_CONNECTION_PRIORITY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::RequestConnectionPriority done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothGattClientProxy::GetServices(int32_t appId, std::vector<BluetoothGattService> &service)
{
    HILOGI("BluetoothGattClientProxy::GetServices start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientProxy::GetServices WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(appId)) {
        HILOGE("BluetoothGattClientProxy::GetServices transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(IBluetoothGattClient::Code::BT_GATT_CLIENT_GET_SERVICES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientProxy::GetServices done fail, error: %d", error);
    }
    int DevNum = reply.ReadInt32();
    for (int i = DevNum; i > 0; i--) {
        std::shared_ptr<BluetoothGattService> dev(reply.ReadParcelable<BluetoothGattService>());
        if (!dev) {
            return;
        }
        service.push_back(*dev);
    }
}
}  // namespace Bluetooth
}  // namespace OHOS