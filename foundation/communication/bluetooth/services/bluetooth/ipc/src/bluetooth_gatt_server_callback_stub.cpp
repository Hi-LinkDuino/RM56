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

#include "bluetooth_gatt_server_callback_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"

namespace OHOS {
namespace Bluetooth {
BluetoothGattServerCallbackStub::BluetoothGattServerCallbackStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_CHARACTERISTIC_READREQUEST)] =
        &BluetoothGattServerCallbackStub::OnCharacteristicReadRequestInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_CONNECTIONSTATE_CHANGED)] =
        &BluetoothGattServerCallbackStub::OnConnectionStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_ADD_SERVICE)] =
        &BluetoothGattServerCallbackStub::OnAddServiceInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_CHARACTERISTIC_WRITE_REQUEST)] =
        &BluetoothGattServerCallbackStub::OnCharacteristicWriteRequestInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_DESCRIPTOR_READ_REQUEST)] =
        &BluetoothGattServerCallbackStub::OnDescriptorReadRequestInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_DESCRIPTOR_WRITE_REQUEST)] =
        &BluetoothGattServerCallbackStub::OnDescriptorWriteRequestInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_MTU_CHANGED)] =
        &BluetoothGattServerCallbackStub::OnMtuChangedInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_NOTIFY_CONFIRM)] =
        &BluetoothGattServerCallbackStub::OnNotifyConfirmInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothGattServerCallbackStub::Code::GATT_SERVER_CALLBACK_CONNECTION_PARAMETER_CHANGED)] =
        &BluetoothGattServerCallbackStub::OnConnectionParameterChangedInner;
    HILOGD("%{public}s ends.", __func__);
}

BluetoothGattServerCallbackStub::~BluetoothGattServerCallbackStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}
int BluetoothGattServerCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothGattServerCallbackStub::OnRemoteRequest, cmd = %{public}u, flags= %{public}d",
        code, option.GetFlags());
    std::u16string descriptor = BluetoothGattServerCallbackStub::GetDescriptor();
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
    HILOGW("BluetoothGattServerCallbackStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
};
ErrCode BluetoothGattServerCallbackStub::OnCharacteristicReadRequestInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }

    OnCharacteristicReadRequest(*device, *characteristic);

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattServerCallbackStub::OnConnectionStateChangedInner Triggered!");
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        HILOGE("BluetoothGattServerCallbackStub::OnConnectionStateChangedInner device is null");
        return TRANSACTION_ERR;
    }
    
    int32_t ret = data.ReadInt32();
    int32_t state = data.ReadInt32();
    OnConnectionStateChanged(*device, ret, state);

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnAddServiceInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattServerCallbackStub::OnAddServiceInner Triggered!");
    int32_t ret = data.ReadInt32();
    std::shared_ptr<BluetoothGattService> service(data.ReadParcelable<BluetoothGattService>());
    if (!service) {
        return TRANSACTION_ERR;
    }

    OnAddService(ret, *service);

    service = nullptr;

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnCharacteristicWriteRequestInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    bool needRespones = data.ReadBool();

    OnCharacteristicWriteRequest(*device, *characteristic, needRespones);

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnDescriptorReadRequestInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothGattDescriptor> descriptor(data.ReadParcelable<BluetoothGattDescriptor>());
    if (!descriptor) {
        return TRANSACTION_ERR;
    }

    OnDescriptorReadRequest(*device, *descriptor);

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnDescriptorWriteRequestInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothGattDescriptor> descriptor(data.ReadParcelable<BluetoothGattDescriptor>());
    if (!descriptor) {
        return TRANSACTION_ERR;
    }

    OnDescriptorWriteRequest(*device, *descriptor);

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnMtuChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t mtu = data.ReadInt32();

    OnMtuChanged(*device, mtu);

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnNotifyConfirmInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    int32_t result = data.ReadInt32();

    OnNotifyConfirm(*device, *characteristic, result);

    return NO_ERROR;
}
ErrCode BluetoothGattServerCallbackStub::OnConnectionParameterChangedInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothGattDevice> device(data.ReadParcelable<BluetoothGattDevice>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t interval = data.ReadInt32();
    int32_t latency = data.ReadInt32();
    int32_t timeout = data.ReadInt32();
    int32_t status = data.ReadInt32();

    OnConnectionParameterChanged(*device, interval, latency, timeout, status);

    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS