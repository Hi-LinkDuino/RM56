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
#include "bluetooth_pan_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothPanStub::BluetoothPanStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(COMMAND_DISCONNECT)] =
        &BluetoothPanStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_DEVICE_STATE)] =
        &BluetoothPanStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_DEVICES_BY_STATES)] =
        &BluetoothPanStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_REGISTER_OBSERVER)] =
        &BluetoothPanStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_DEREGISTER_OBSERVER)] =
        &BluetoothPanStub::DeregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_SET_TETHERING)] =
        &BluetoothPanStub::SetTetheringInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_IS_TETHERING_ON)] =
        &BluetoothPanStub::IsTetheringOnInner;
    HILOGD("%{public}s ends.", __func__);
}

BluetoothPanStub::~BluetoothPanStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothPanStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothPanStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothPanStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGE("local descriptor is not equal to remote");
        return IPC_INVOKER_TRANSLATE_ERR;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGW("BluetoothHfpHfStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothPanStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    const BluetoothRawAddress *device = data.ReadParcelable<BluetoothRawAddress>();
    bool result;
    HILOGD("BluetoothPanStub::DisconnectInner");
    ErrCode ec = Disconnect(*device, result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(NO_ERROR);
    }
    return ERR_NONE;
}

ErrCode BluetoothPanStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    const BluetoothRawAddress *device = data.ReadParcelable<BluetoothRawAddress>();
    int result;
    HILOGD("BluetoothPanStub::GetDeviceStateInner");
    ErrCode ec = GetDeviceState(*device, result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}

ErrCode BluetoothPanStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int32_t> states = {};
    int32_t stateSize = data.ReadInt32();
    HILOGD("BluetoothPanStub::GetDevicesByStatesInner");
    for (int i = 0; i < stateSize; i++) {
        int32_t state = data.ReadInt32();
        states.push_back(state);
    }
    std::vector<BluetoothRawAddress> rawAdds;
    ErrCode ec = GetDevicesByStates(states, rawAdds);
    if (ec != ERR_OK) {
        return ec;
    }
    reply.WriteInt32(rawAdds.size());
    for (auto rawAdd : rawAdds) {
        if (!reply.WriteParcelable(&rawAdd)) {
            return ERR_INVALID_STATE;
        }
    }
    return ERR_NONE;
}

ErrCode BluetoothPanStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothPanStub::RegisterObserverInner");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothPanObserver> observer = OHOS::iface_cast<IBluetoothPanObserver>(remote);
    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothPanStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothPanStub::DeregisterObserverInner");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothPanObserver> observer = OHOS::iface_cast<IBluetoothPanObserver>(remote);
    DeregisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothPanStub::SetTetheringInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothPanStub::SetTetheringInner");
    const bool value = data.ReadBool();
    bool result;
    ErrCode ec = SetTethering(value, result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}

ErrCode BluetoothPanStub::IsTetheringOnInner(MessageParcel &data, MessageParcel &reply)
{
    bool result;
    HILOGD("BluetoothPanStub::IsTetheringOnInner");
    ErrCode ec = IsTetheringOn(result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}
}  // namespace Bluetooth
}  // namespace OHOS