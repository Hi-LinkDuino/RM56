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
#include "bluetooth_opp_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothOppStub::BluetoothOppStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(COMMAND_SEND_FILE)] =
        &BluetoothOppStub::SendFileInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_SET_INCOMING_FILE_CONFIRMATION)] =
        &BluetoothOppStub::SetIncomingFileConfirmationInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_CURRENT_TRANSFER_INFORMATION)] =
        &BluetoothOppStub::GetCurrentTransferInformationInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_CANCEL_TRANSFER)] =
        &BluetoothOppStub::CancelTransferInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_REGISTER_OBSERVER)] =
        &BluetoothOppStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_DEREGISTER_OBSERVER)] =
        &BluetoothOppStub::DeregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_DEVICE_STATE)] =
        &BluetoothOppStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_DEVICES_BY_STATES)] =
        &BluetoothOppStub::GetDevicesByStatesInner;
    HILOGI("ends.");
}

BluetoothOppStub::~BluetoothOppStub()
{
    HILOGI("start.");
    memberFuncMap_.clear();
}

int BluetoothOppStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGI("BluetoothOppStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothOppStub::GetDescriptor();
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
    HILOGI("BluetoothHfpHfStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothOppStub::SendFileInner(MessageParcel &data, MessageParcel &reply)
{
    std::string device = data.ReadString();
    bool result;
    HILOGI("BluetoothOppStub::SendFileInner");
    std::vector<std::string> filePaths = {};
    std::vector<std::string> mimeTypes = {};
    int32_t fileSize = data.ReadInt32();
    HILOGI("BluetoothOppStub::GetDevicesByStatesInner");
    for (int32_t i = 0; i < fileSize; i++) {
        std::string filePath = data.ReadString();
        filePaths.push_back(filePath);
    }
    int32_t mimeSize = data.ReadInt32();
    for (int32_t i = 0; i < mimeSize; i++) {
        std::string mimeType = data.ReadString();
        mimeTypes.push_back(mimeType);
    }

    ErrCode ec = SendFile(device, filePaths, mimeTypes, result);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}

ErrCode BluetoothOppStub::SetIncomingFileConfirmationInner(MessageParcel &data, MessageParcel &reply)
{
    bool accept = data.ReadBool();
    bool result;
    HILOGI("BluetoothOppStub::SetIncomingFileConfirmationInner");
    ErrCode ec = SetIncomingFileConfirmation(accept, result);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}

ErrCode BluetoothOppStub::GetCurrentTransferInformationInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothOppStub::GetCurrentTransferInformationInner");
    BluetoothIOppTransferInformation oppInformation;
    ErrCode ec = GetCurrentTransferInformation(oppInformation);
    if (ec != ERR_OK) {
        HILOGE("BluetoothOppStub::fail");
        return ec;
    }
    reply.WriteParcelable(&oppInformation);
    return ERR_NONE;
}

ErrCode BluetoothOppStub::CancelTransferInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothOppStub::CancelTransfer");
    bool result;
    ErrCode ec = CancelTransfer(result);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}

ErrCode BluetoothOppStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothOppStub::RegisterObserverInner");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothOppObserver> observer = OHOS::iface_cast<IBluetoothOppObserver>(remote);
    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothOppStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothOppStub::DeregisterObserverInner");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothOppObserver> observer = OHOS::iface_cast<IBluetoothOppObserver>(remote);
    DeregisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothOppStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    const BluetoothRawAddress *device = data.ReadParcelable<BluetoothRawAddress>();
    int result;
    HILOGI("BluetoothOppStub::GetDeviceStateInner");
    ErrCode ec = GetDeviceState(*device, result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}

ErrCode BluetoothOppStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int32_t> states = {};
    int32_t stateSize = data.ReadInt32();
    HILOGI("BluetoothOppStub::GetDevicesByStatesInner");
    for (int i = 0; i < stateSize; i++) {
        int32_t state = data.ReadInt32();
        states.push_back(state);
    }
    std::vector<BluetoothRawAddress> rawAdds;
    ErrCode ec = GetDevicesByStates(states, rawAdds);
    if (ec != ERR_OK) {
        HILOGE("BluetoothOppStub::fail");
        return ec;
    }
    reply.WriteInt32(rawAdds.size());
    for (auto rawAdd : rawAdds) {
        if (!reply.WriteParcelable(&rawAdd)) {
            HILOGE("BluetoothOppStub:: WriteParcelable fail");
            return ERR_INVALID_STATE;
        }
    }
    return ERR_NONE;
}
}  // namespace Bluetooth
}  // namespace OHOS