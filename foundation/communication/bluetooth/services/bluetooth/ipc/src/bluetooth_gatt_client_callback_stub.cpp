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

#include "bluetooth_gatt_client_callback_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothGattClientCallbackStub::BluetoothGattClientCallbackStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CONNECT_STATE_CHANGE)] =
        &BluetoothGattClientCallbackStub::OnConnectionStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CHARACTER_CHANGE)] =
        &BluetoothGattClientCallbackStub::OnCharacteristicChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CHARACTER_READ)] =
        &BluetoothGattClientCallbackStub::OnCharacteristicReadInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CHARACTER_WRITE)] =
        &BluetoothGattClientCallbackStub::OnCharacteristicWriteInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_DESCRIPTOR_READ)] =
        &BluetoothGattClientCallbackStub::OnDescriptorReadInner;
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_DESCRIPTOR_WRITE)] =
        &BluetoothGattClientCallbackStub::OnDescriptorWriteInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_MTU_UPDATE)] =
        &BluetoothGattClientCallbackStub::OnMtuChangedInner;
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_SERVICES_DISCOVER)] =
        &BluetoothGattClientCallbackStub::OnServicesDiscoveredInner;
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_CONNECTION_PARA_CHANGE)] =
        &BluetoothGattClientCallbackStub::OnConnectionParameterChangedInner;
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothGattClientCallback::Code::BT_GATT_CLIENT_CALLBACK_SERVICES_CHANGED)] =
        &BluetoothGattClientCallbackStub::OnServicesChangedInner;
}

BluetoothGattClientCallbackStub::~BluetoothGattClientCallbackStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothGattClientCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothGattClientCallbackStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
    std::u16string descriptor = BluetoothGattClientCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGI("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGW("BluetoothGattClientCallbackStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothGattClientCallbackStub::OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnConnectionStateChangedInner Triggered!");
    int32_t state = data.ReadInt32();
    int32_t newState = data.ReadInt32();
    OnConnectionStateChanged(state, newState);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnCharacteristicChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnCharacteristicChangedInner Triggered!");
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    OnCharacteristicChanged(*characteristic);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnCharacteristicReadInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnCharacteristicReadInner Triggered!");
    int32_t ret = data.ReadInt32();
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    OnCharacteristicRead(ret, *characteristic);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnCharacteristicWriteInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnCharacteristicWriteInner Triggered!");
    int32_t ret = data.ReadInt32();
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    OnCharacteristicWrite(ret, *characteristic);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnDescriptorReadInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnDescriptorReadInner Triggered!");
    int32_t ret = data.ReadInt32();
    std::shared_ptr<BluetoothGattDescriptor> descriptor(data.ReadParcelable<BluetoothGattDescriptor>());
    if (!descriptor) {
        return TRANSACTION_ERR;
    }
    OnDescriptorRead(ret, *descriptor);

    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnDescriptorWriteInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnDescriptorWriteInner Triggered!");
    int32_t ret = data.ReadInt32();
    std::shared_ptr<BluetoothGattDescriptor> descriptor(data.ReadParcelable<BluetoothGattDescriptor>());
    if (!descriptor) {
        return TRANSACTION_ERR;
    }
    OnDescriptorWrite(ret, *descriptor);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnMtuChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnMtuChangedInner Triggered!");
    int32_t state = data.ReadInt32();
    int32_t mtu = data.ReadInt32();
    OnMtuChanged(state, mtu);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnServicesDiscoveredInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnServicesDiscoveredInner Triggered!");
    int32_t status = data.ReadInt32();
    OnServicesDiscovered(status);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnConnectionParameterChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnConnectionParameterChangedInner Triggered!");
    int32_t interval = data.ReadInt32();
    int32_t latency = data.ReadInt32();
    int32_t timeout = data.ReadInt32();
    int32_t status = data.ReadInt32();
    OnConnectionParameterChanged(interval, latency, timeout, status);
    return NO_ERROR;
}

ErrCode BluetoothGattClientCallbackStub::OnServicesChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientCallbackStub::OnServicesChangedInner Triggered!");
    int32_t num = data.ReadInt32();
    std::vector<BluetoothGattService> service;
    for (int i = num; i > 0; i--) {
        std::shared_ptr<BluetoothGattService> dev(data.ReadParcelable<BluetoothGattService>());
        if (!dev) {
            return TRANSACTION_ERR;
        }
        service.push_back(*dev);
    }
    OnServicesChanged(service);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS