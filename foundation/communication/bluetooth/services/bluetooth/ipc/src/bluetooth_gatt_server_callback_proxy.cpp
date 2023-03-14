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

#include "bluetooth_gatt_server_callback_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothGattServerCallbackProxy::OnCharacteristicReadRequest(
    const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicReadRequest WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicReadRequest error");
        return;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicReadRequest error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_CHARACTERISTIC_READREQUEST, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicReadRequest done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnConnectionStateChanged(
    const BluetoothGattDevice &device, int32_t ret, int32_t state)
{
    HILOGI("BluetoothGattServerCallbackProxy::OnConnectionStateChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionStateChanged error");
        return;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionStateChanged error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionStateChanged error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_CONNECTIONSTATE_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionStateChanged done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnAddService(int32_t ret, const BluetoothGattService &service)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnAddService WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(ret)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnAddService error");
        return;
    }
    if (!data.WriteParcelable(&service)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnAddService error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_ADD_SERVICE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnAddService done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnCharacteristicWriteRequest(
    const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic, bool needRespones)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicWriteRequest WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicWriteRequest error");
        return;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicWriteRequest error");
        return;
    }
    if (!data.WriteBool(needRespones)) {
        HILOGE("BluetoothGattServerProxy::OnCharacteristicWriteRequest error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_CHARACTERISTIC_WRITE_REQUEST, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnCharacteristicWriteRequest done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnDescriptorReadRequest(
    const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorReadRequest WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorReadRequest error");
        return;
    }
    if (!data.WriteParcelable(&descriptor)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorReadRequest error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_DESCRIPTOR_READ_REQUEST, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorReadRequest done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnDescriptorWriteRequest(
    const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorWriteRequest WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorWriteRequest error");
        return;
    }
    if (!data.WriteParcelable(&descriptor)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorWriteRequest error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_DESCRIPTOR_WRITE_REQUEST, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnDescriptorWriteRequest done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnMtuChanged(const BluetoothGattDevice &device, int32_t mtu)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnMtuUpdateOnMtuChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnMtuUpdateOnMtuChanged error");
        return;
    }
    if (!data.WriteInt32(mtu)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnMtuUpdateOnMtuChanged error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_MTU_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnMtuUpdateOnMtuChanged done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnNotifyConfirm(
    const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnNotifyConfirm WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnNotifyConfirm error");
        return;
    }
    if (!data.WriteParcelable(&characteristic)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnNotifyConfirm error");
        return;
    }
    if (!data.WriteInt32(result)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnNotifyConfirm error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error =
        Remote()->SendRequest(IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_NOTIFY_CONFIRM, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnNotifyConfirm done fail, error: %d", error);
        return;
    }
    return;
}
void BluetoothGattServerCallbackProxy::OnConnectionParameterChanged(
    const BluetoothGattDevice &device, int32_t interval, int32_t latency, int32_t timeout, int32_t status)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothGattServerCallbackProxy::GetDescriptor())) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionParameterChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionParameterChanged error");
        return;
    }
    if (!data.WriteInt32(interval)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionParameterChanged error");
        return;
    }
    if (!data.WriteInt32(latency)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionParameterChanged error");
        return;
    }
    if (!data.WriteInt32(timeout)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionParameterChanged error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionParameterChanged error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothGattServerCallback::Code::GATT_SERVER_CALLBACK_CONNECTION_PARAMETER_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothGattServerCallbackProxy::OnConnectionParameterChanged done fail, error: %d", error);
        return;
    }
    return;
}
}  // namespace Bluetooth
}  // namespace OHOS