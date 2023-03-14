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

#include "bluetooth_gatt_client_callback_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothGattClientCallbackProxy::OnConnectionStateChanged(int32_t state, int32_t newState)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnConnectionStateChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionStateChanged transport error");
        return;
    }
    if (!data.WriteInt32(newState)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionStateChanged transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CONNECT_STATE_CHANGE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionStateChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnCharacteristicChanged(const BluetoothGattCharacteristic &characteristic)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnCharacteristicChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicChanged transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CHARACTER_CHANGE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnCharacteristicRead(
    int32_t ret, const BluetoothGattCharacteristic &characteristic)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnCharacteristicRead Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicRead WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicRead transport error");
        return;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicRead transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CHARACTER_READ, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicRead done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnCharacteristicWrite(
    int32_t ret, const BluetoothGattCharacteristic &characteristic)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnCharacteristicWrite Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicWrite WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicWrite transport error");
        return;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicWrite transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CHARACTER_WRITE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnCharacteristicWrite done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnDescriptorRead(int32_t ret, const BluetoothGattDescriptor &descriptor)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnDescriptorRead Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorRead WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorRead transport error");
        return;
    }
    if (!data.WriteParcelable(&descriptor)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorRead transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_DESCRIPTOR_READ, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorRead done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnDescriptorWrite(int32_t ret, const BluetoothGattDescriptor &descriptor)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnDescriptorWrite Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorWrite WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorWrite transport error");
        return;
    }
    if (!data.WriteParcelable(&descriptor)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorWrite transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_DESCRIPTOR_WRITE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnDescriptorWrite done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnMtuChanged(int32_t state, int32_t mtu)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnMtuChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnMtuChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnMtuChanged transport error");
        return;
    }
    if (!data.WriteInt32(mtu)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnMtuChanged transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_MTU_UPDATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnMtuChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnServicesDiscovered(int32_t status)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnServicesDiscovered Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnServicesDiscovered WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnServicesDiscovered transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_SERVICES_DISCOVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnServicesDiscovered done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnConnectionParameterChanged(
    int32_t interval, int32_t latency, int32_t timeout, int32_t status)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnConnectionParameterChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionParameterChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(interval)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionParameterChanged transport error");
        return;
    }
    if (!data.WriteInt32(latency)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionParameterChanged transport error");
        return;
    }
    if (!data.WriteInt32(timeout)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionParameterChanged transport error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionParameterChanged transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CONNECTION_PARA_CHANGE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnConnectionParameterChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothGattClientCallbackProxy::OnServicesChanged(std::vector<BluetoothGattService> &service)
{
    HILOGI("BluetoothGattClientCallbackProxy::OnServicesChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattClientCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattClientCallbackProxy::OnServicesChanged WriteInterfaceToken error");
        return;
    }
    int num = service.size();
    if (!data.WriteInt32(num)) {
        HILOGE("BluetoothGattClientCallbackProxy::OnServicesChanged transport error");
        return;
    }
    for (int i = 0; i < num; i++) {
        if (!data.WriteParcelable(&service[i])) {
            HILOGE("BluetoothGattClientCallbackProxy::OnServicesChanged transport error");
            return;
        }
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_SERVICES_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattClientCallbackProxy::OnServicesChanged done fail, error: %{public}d", error);
        return;
    }
}
}  // namespace Bluetooth
}  // namespace OHOS