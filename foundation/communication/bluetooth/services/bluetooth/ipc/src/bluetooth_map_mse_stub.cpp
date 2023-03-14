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

#include "bluetooth_map_mse_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothMapMseStub::BluetoothMapMseStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_REGISTER_OBSERVER)] =
        &BluetoothMapMseStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_DE_REGISTER_OBSERVER)] =
        &BluetoothMapMseStub::DeregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_GET_STATE)] =
        &BluetoothMapMseStub::GetStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_DISCONNECT)] =
        &BluetoothMapMseStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_ISCONNECTED)] =
        &BluetoothMapMseStub::IsConnectedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_GET_CONNECTED_DEVICES)] =
        &BluetoothMapMseStub::GetConnectedDevicesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_GET_DEVICES_BY_STATES)] =
        &BluetoothMapMseStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_GET_CONNECTION_STATE)] =
        &BluetoothMapMseStub::GetConnectionStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_SET_CONNECTION_STRATEGY)] =
        &BluetoothMapMseStub::SetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_GET_CONNECTION_STRATEGY)] =
        &BluetoothMapMseStub::GetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMse::Code::MSE_GRANT_PERMISSION)] =
        &BluetoothMapMseStub::GrantPermissionInner;
}

BluetoothMapMseStub::~BluetoothMapMseStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothMapMseStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothMapMseStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
    std::u16string descriptor = BluetoothMapMseStub::GetDescriptor();
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
    HILOGW("BluetoothMapMseStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothMapMseStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::RegisterObserverInner Triggered!");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothMapMseObserver> callback = OHOS::iface_cast<IBluetoothMapMseObserver>(remote);
    RegisterObserver(callback);
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::DeregisterObserverInner Triggered!");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothMapMseObserver> callback = OHOS::iface_cast<IBluetoothMapMseObserver>(remote);
    DeregisterObserver(callback);
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::GetStateInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::GetStateInner Triggered!");
    int32_t result;
    GetState(result);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::DisconnectInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t result;
    Disconnect(*device, result);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::IsConnectedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::IsConnectedInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    bool result;
    IsConnected(*device, result);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothMapMseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::GetConnectedDevicesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::GetConnectDevicesInner Triggered!");
    std::vector<BluetoothRawAddress> devices;
    GetConnectedDevices(devices);
    reply.WriteInt32(devices.size());
    int num = devices.size();
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&devices[i]);
        if (!ret) {
            HILOGE("WriteParcelable<GetConnectDevicesInner> failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::GetDevicesByStates Triggered!");
    std::vector<int32_t> states;
    if (!data.ReadInt32Vector(&states)) {
        HILOGW("BluetoothMapMseStub::GetDevicesByStatesInner: get tmpState failed.");
        return INVALID_DATA;
    }
    std::vector<BluetoothRawAddress> rawDevices;
    GetDevicesByStates(states, rawDevices);
    reply.WriteInt32(rawDevices.size());
    int num = rawDevices.size();
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&rawDevices[i]);
        if (!ret) {
            HILOGE("WriteParcelable<GetDevicesByStatesInner> failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::GetConnectionStateInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::GetConnectionStateInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t result;
    GetConnectionState(*device, result);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::SetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::SetConnectionStrategyInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t strategy = data.ReadInt32();
    bool result;
    SetConnectionStrategy(*device, strategy, result);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothMapMseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::GetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::GetConnectionStrategyInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t result;
    GetConnectionStrategy(*device, result);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMseStub::GrantPermissionInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMseStub::GrantPermissionInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    bool allow = data.ReadBool();
    bool save = data.ReadBool();
    GrantPermission(*device, allow, save);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
