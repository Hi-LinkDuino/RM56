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

#include "bluetooth_pbap_pse_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"
namespace OHOS {
namespace Bluetooth {
BluetoothPbapPseStub::BluetoothPbapPseStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_GET_DEVICE_STATE)] =
        &BluetoothPbapPseStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_GET_DEVICES_BY_STATES)] =
        &BluetoothPbapPseStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_DISCONNECT)] =
        &BluetoothPbapPseStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_SET_CONNECTION_STRATEGY)] =
        &BluetoothPbapPseStub::SetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_GET_CONNECTION_STRATEGY)] =
        &BluetoothPbapPseStub::GetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_GRANT_PERMISSION)] =
        &BluetoothPbapPseStub::GrantPermissionInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_SET_DEVICE_PASSWORD)] =
        &BluetoothPbapPseStub::SetDevicePasswordInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_REGISTER_OBSERVER)] =
        &BluetoothPbapPseStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(BluetoothPbapPseStub::Code::PBAP_PSE_DEREGISTER_OBSERVER)] =
        &BluetoothPbapPseStub::DeregisterObserverInner;
}

BluetoothPbapPseStub::~BluetoothPbapPseStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}
int BluetoothPbapPseStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothPbapPseStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothPbapPseStub::GetDescriptor();
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
    HILOGW("BluetoothPbapPseStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
};

ErrCode BluetoothPbapPseStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetDeviceState(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPseStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothPbapPseStub::GetDevicesByStatesInner starts");
    std::vector<int32_t> state;
    if (!data.ReadInt32Vector(&state)) {
        HILOGW("BluetoothPbapPseStub::GetDevicesByStatesInner: get tmpState failed.");
        return INVALID_DATA;
    }
    std::vector<BluetoothRawAddress> rawDevices;
    GetDevicesByStates(state, rawDevices);
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

ErrCode BluetoothPbapPseStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Disconnect(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPseStub::SetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int strategy = data.ReadInt32();
    int result = SetConnectionStrategy(*device, strategy);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPseStub::GetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetConnectionStrategy(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPseStub::GrantPermissionInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    bool allow = data.ReadBool();
    bool save = data.ReadBool();
    GrantPermission(*device, allow, save);

    return NO_ERROR;
}

ErrCode BluetoothPbapPseStub::SetDevicePasswordInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    const std::string password = data.ReadString();
    const std::string userId = data.ReadString();
    int result = SetDevicePassword(*device, password, userId);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothPbapPseStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothPbapPseStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothPbapPseObserver> observer = OHOS::iface_cast<IBluetoothPbapPseObserver>(remote);
    RegisterObserver(observer);

    return NO_ERROR;
}

ErrCode BluetoothPbapPseStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothPbapPseObserver> observer = OHOS::iface_cast<IBluetoothPbapPseObserver>(remote);
    DeregisterObserver(observer);
    
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS