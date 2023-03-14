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

#include "bluetooth_gatt_client_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"

namespace OHOS {
namespace Bluetooth {
BluetoothGattClientStub::BluetoothGattClientStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_REGISTER_APP)] =
        &BluetoothGattClientStub::RegisterApplicationInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_DEREGISTER_APP)] =
        &BluetoothGattClientStub::DeregisterApplicationInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_CONNECT)] =
        &BluetoothGattClientStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_DIS_CONNECT)] =
        &BluetoothGattClientStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_DISCOVERY_SERVICES)] =
        &BluetoothGattClientStub::DiscoveryServicesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_READ_CHARACTERISTIC)] =
        &BluetoothGattClientStub::ReadCharacteristicInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_WRITE_CHARACTERISTIC)] =
        &BluetoothGattClientStub::WriteCharacteristicInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_SIGNED_WRITE_CHARACTERISTIC)] =
        &BluetoothGattClientStub::SignedWriteCharacteristicInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_READ_DESCRIPTOR)] =
        &BluetoothGattClientStub::ReadDescriptorInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_WRITE_DESCRIPTOR)] =
        &BluetoothGattClientStub::WriteDescriptorInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_REQUEST_EXCHANGE_MTU)] =
        &BluetoothGattClientStub::RequestExchangeMtuInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_GET_ALL_DEVICE)] =
        &BluetoothGattClientStub::GetAllDeviceInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_REQUEST_CONNECTION_PRIORITY)] =
        &BluetoothGattClientStub::RequestConnectionPriorityInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothGattClient::Code::BT_GATT_CLIENT_GET_SERVICES)] =
        &BluetoothGattClientStub::GetServicesInner;
}

BluetoothGattClientStub::~BluetoothGattClientStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothGattClientStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothGattClientStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothGattClientStub::GetDescriptor();
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
    HILOGW("BluetoothGattClientStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothGattClientStub::RegisterApplicationInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::RegisterApplicationInner starts");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothGattClientCallback> callback = OHOS::iface_cast<IBluetoothGattClientCallback>(remote);
    std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    int32_t transport = data.ReadInt32();
    int result = RegisterApplication(callback, *addr, transport);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::DeregisterApplicationInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::DeregisterApplicationInner starts");
    int32_t appId = data.ReadInt32();
    int result = DeregisterApplication(appId);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::ConnectInner starts");
    int32_t appId = data.ReadInt32();
    bool autoConnect = data.ReadBool();
    int result = Connect(appId, autoConnect);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::DisconnectInner starts");
    int32_t appId = data.ReadInt32();
    int result = Disconnect(appId);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::DiscoveryServicesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::DiscoveryServicesInner starts");
    int32_t appId = data.ReadInt32();
    int result = DiscoveryServices(appId);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::ReadCharacteristicInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::ReadCharacteristicInner starts");
    int32_t appId = data.ReadInt32();
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    int result = ReadCharacteristic(appId, *characteristic);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::WriteCharacteristicInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::WriteCharacteristicInner starts");
    int32_t appId = data.ReadInt32();
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    bool withoutRespond = data.ReadBool();
    int result = WriteCharacteristic(appId, characteristic.get(), withoutRespond);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::SignedWriteCharacteristicInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::SignedWriteCharacteristicInner starts");
    int32_t appId = data.ReadInt32();
    std::shared_ptr<BluetoothGattCharacteristic> characteristic(data.ReadParcelable<BluetoothGattCharacteristic>());
    if (!characteristic) {
        return TRANSACTION_ERR;
    }
    int result = SignedWriteCharacteristic(appId, characteristic.get());
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::ReadDescriptorInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::ReadDescriptorInner starts");
    int32_t appId = data.ReadInt32();
    std::shared_ptr<BluetoothGattDescriptor> descriptor(data.ReadParcelable<BluetoothGattDescriptor>());
    if (!descriptor) {
        return TRANSACTION_ERR;
    }
    int result = ReadDescriptor(appId, *descriptor);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::WriteDescriptorInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::WriteDescriptorInner starts");
    int32_t appId = data.ReadInt32();
    std::shared_ptr<BluetoothGattDescriptor> descriptor(data.ReadParcelable<BluetoothGattDescriptor>());
    if (!descriptor) {
        return TRANSACTION_ERR;
    }
    int result = WriteDescriptor(appId, descriptor.get());
    HILOGI("appId=%{public}d, result=%{public}d", appId, result);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::RequestExchangeMtuInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::RequestExchangeMtuInner starts");
    int32_t appId = data.ReadInt32();
    int32_t mtu = data.ReadInt32();
    int result = RequestExchangeMtu(appId, mtu);
    HILOGI("appId=%{public}d, mtu=%{public}d, result=%{public}d", appId, mtu, result);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::GetAllDeviceInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::GetAllDeviceInner starts");
    std::vector<BluetoothGattDevice> device;
    GetAllDevice(device);
    reply.WriteInt32(device.size());
    int num = device.size();
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&device[i]);
        if (!ret) {
            HILOGE("WriteParcelable<GetAllDeviceInner> failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::RequestConnectionPriorityInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::RequestConnectionPriorityInner starts");
    int32_t appId = data.ReadInt32();
    int32_t connPriority = data.ReadInt32();
    int result = RequestConnectionPriority(appId, connPriority);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothGattClientStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothGattClientStub::GetServicesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothGattClientStub::GetServicesInner starts");
    int32_t appId = data.ReadInt32();
    std::vector<BluetoothGattService> service;
    GetServices(appId, service);
    reply.WriteInt32(service.size());
    int num = service.size();
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&service[i]);
        if (!ret) {
            HILOGE("WriteParcelable<GetServicesInner> failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS