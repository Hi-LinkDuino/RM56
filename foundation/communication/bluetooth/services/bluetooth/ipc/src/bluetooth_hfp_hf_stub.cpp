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

#include "bluetooth_hfp_hf_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothHfpHfStub::BluetoothHfpHfStub() {
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_CONNECT_SCO)] =
        &BluetoothHfpHfStub::ConnectScoInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_DISCONNECT_SCO)] =
        &BluetoothHfpHfStub::DisconnectScoInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_GET_DEVICES_BY_STATES)] =
        &BluetoothHfpHfStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_GET_DEVICE_STATE)] =
        &BluetoothHfpHfStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_GET_SCO_STATE)] =
        &BluetoothHfpHfStub::GetScoStateInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_SEND_DTMF_TONE)] =
        &BluetoothHfpHfStub::SendDTMFToneInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_CONNECT)] =
        &BluetoothHfpHfStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_DISCONNECT)] =
        &BluetoothHfpHfStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_OPEN_VOICE_RECOGNITION)] =
        &BluetoothHfpHfStub::OpenVoiceRecognitionInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_CLOSE_VOICE_RECOGNITION)] =
        &BluetoothHfpHfStub::CloseVoiceRecognitionInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_GET_CURRENT_CALL_LIST)] =
        &BluetoothHfpHfStub::GetCurrentCallListInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_ACCEPT_INCOMING_CALL)] =
        &BluetoothHfpHfStub::AcceptIncomingCallInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_HOLD_ACTIVE_CALL)] =
        &BluetoothHfpHfStub::HoldActiveCallInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_REJECT_INCOMING_CALL)] =
        &BluetoothHfpHfStub::RejectIncomingCallInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_SEND_KEY_PRESSED)] =
        &BluetoothHfpHfStub::SendKeyPressedInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_HANDLE_INCOMING_CALL)] =
        &BluetoothHfpHfStub::HandleIncomingCallInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_HANDLE_MULLTI_CALL)] =
        &BluetoothHfpHfStub::HandleMultiCallInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_DIAL_LAST_NUMBER)] =
        &BluetoothHfpHfStub::DialLastNumberInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_DIAL_MEMORY)] =
        &BluetoothHfpHfStub::DialMemoryInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_SEND_VOICE_TAG)] =
        &BluetoothHfpHfStub::SendVoiceTagInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_FINISH_ATIVE_CALL)] =
        &BluetoothHfpHfStub::FinishActiveCallInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_START_DIAL)] =
        &BluetoothHfpHfStub::StartDialInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_REGISTER_OBSERVER)] =
        &BluetoothHfpHfStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpHfStub::Code::BT_HFP_HF_DEREGISTER_OBSERVER)] =
        &BluetoothHfpHfStub::DeregisterObserverInner;

    HILOGD("%{public}s ends.", __func__);
}

BluetoothHfpHfStub::~BluetoothHfpHfStub() {
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothHfpHfStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) {
    HILOGD("BluetoothHfpHfStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothHfpHfStub::GetDescriptor();
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
    HILOGW("BluetoothHfpHfStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);   
}

ErrCode BluetoothHfpHfStub::ConnectScoInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = ConnectSco(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::DisconnectScoInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = DisconnectSco(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply) {
    std::vector<BluetoothRawAddress> devices;
    std::vector<int> states;
    data.ReadInt32Vector(&states);
    GetDevicesByStates(states, devices);
    uint32_t devNum = devices.size();
    if (!reply.WriteUint32(devNum)) {
        HILOGE("BluetoothHfpHfStub:WriteInt32 failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    for (uint32_t i = 0; i < devNum; i++) {
        if (!reply.WriteParcelable(&devices[i])) {
            HILOGE("BluetoothHfpHfStub:WriteParcelable failed in: %{public}s.", __func__);
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetDeviceState(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::GetScoStateInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetScoState(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::SendDTMFToneInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    uint8_t code = data.ReadUint8();
    int result = SendDTMFTone(*device, code);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::ConnectInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Connect(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::DisconnectInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Disconnect(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::OpenVoiceRecognitionInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = OpenVoiceRecognition(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::CloseVoiceRecognitionInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = CloseVoiceRecognition(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::GetCurrentCallListInner(MessageParcel &data, MessageParcel &reply) {
    std::vector<BluetoothHfpHfCall> calls;
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    GetCurrentCallList(*device, calls);
    uint32_t callNum = calls.size();
    if (!reply.WriteUint32(callNum)) {
        HILOGE("BluetoothHfpHfStub:WriteInt32 failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    for (uint32_t i = 0; i < callNum; i++) {
        if (!reply.WriteParcelable(&calls[i])) {
            HILOGE("BluetoothHfpHfStub:WriteParcelable failed in: %{public}s.", __func__);
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::AcceptIncomingCallInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int flag = data.ReadInt32();
    int result = AcceptIncomingCall(*device, flag);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::HoldActiveCallInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = HoldActiveCall(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::SendKeyPressedInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = SendKeyPressed(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::RejectIncomingCallInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = RejectIncomingCall(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::HandleIncomingCallInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        HILOGE("BluetoothHfpHfStub: device is not exist.");
        return TRANSACTION_ERR;
    }
    int flag = data.ReadInt32();
    int result = HandleIncomingCall(*device, flag);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed.");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::HandleMultiCallInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        HILOGE("BluetoothHfpHfStub: device is not exist.");
        return TRANSACTION_ERR;
    }
    int flag = data.ReadInt32();
    int index = data.ReadInt32();
    int result = HandleMultiCall(*device, flag, index);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed.");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::DialLastNumberInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        HILOGE("BluetoothHfpHfStub: device is not exist.");
        return TRANSACTION_ERR;
    }
    int result = DialLastNumber(*device);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed.");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::DialMemoryInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        HILOGE("BluetoothHfpHfStub: device is not exist.");
        return TRANSACTION_ERR;
    }
    int index = data.ReadInt32();
    HILOGE("BluetoothHfpHfStub: reply writing failed. index = %{publilc}d", index);
    int result = DialMemory(*device, index);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed.");
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::SendVoiceTagInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int index = data.ReadInt32();
    HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s. index = %{publilc}d", __func__, index);
    int result = SendVoiceTag(*device, index);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}


ErrCode BluetoothHfpHfStub::FinishActiveCallInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothHfpHfCall> call(data.ReadParcelable<BluetoothHfpHfCall>());
    if (!call) {
        return TRANSACTION_ERR;
    }
    int result = FinishActiveCall(*device, *call);
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::StartDialInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::string number = data.ReadString();
    std::shared_ptr<BluetoothHfpHfCall> call(data.ReadParcelable<BluetoothHfpHfCall>());
    if (!call) {
        return TRANSACTION_ERR;
    }
    int result = StartDial(*device, number, *call);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpHfStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply) {
    sptr<IRemoteObject> tempObject = data.ReadRemoteObject();
    sptr<IBluetoothHfpHfObserver> observer = iface_cast<IBluetoothHfpHfObserver>(tempObject);
    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHfpHfStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply) {
    sptr<IRemoteObject> tempObject = data.ReadRemoteObject();
    sptr<IBluetoothHfpHfObserver> observer = iface_cast<IBluetoothHfpHfObserver>(tempObject);
    DeregisterObserver(observer);
    return NO_ERROR;
}

}  // namespace Bluetooth
}  // namespace OHOS
