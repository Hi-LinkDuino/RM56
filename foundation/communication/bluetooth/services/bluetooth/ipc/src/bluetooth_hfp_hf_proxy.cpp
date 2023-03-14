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
#include "bluetooth_hfp_hf_proxy.h"
#include "bluetooth_errorcode.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothHfpHfProxy::ConnectSco(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::ConnectSco WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::ConnectSco WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_CONNECT_SCO, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::ConnectSco done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::DisconnectSco(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::DisconnectSco WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::DisconnectSco WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_DISCONNECT_SCO, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::DisconnectSco done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int BluetoothHfpHfProxy::GetDevicesByStates(const std::vector<int> &states, std::vector<BluetoothRawAddress> &devices) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::GetDevicesByStates WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32Vector(states)) {
        HILOGE("BluetoothHfpHfProxy::GetDevicesByStates WriteInt32Vector error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_GET_DEVICES_BY_STATES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return ERROR;
    }
    uint32_t devNum = reply.ReadUint32();
    for (uint32_t i = 0; i < devNum; i++) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return TRANSACTION_ERR;
        }
        devices.push_back(*dev);
    }
    return NO_ERROR;
}

int BluetoothHfpHfProxy::GetDeviceState(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::GetDeviceState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::GetDeviceState WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_GET_DEVICE_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::GetDeviceState done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothHfpHfProxy::GetScoState(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::GetScoState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::GetScoState WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_GET_SCO_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::GetScoState done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

bool BluetoothHfpHfProxy::SendDTMFTone(const BluetoothRawAddress &device, uint8_t code) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::SendDTMFTone WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::SendDTMFTone WriteParcelable error");
        return false;
    }
    if (!data.WriteUint8(code)) {
        HILOGE("BluetoothHfpHfProxy::SendDTMFTone WriteUint8 error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_SEND_DTMF_TONE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::SendDTMFTone done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int BluetoothHfpHfProxy::Connect(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::Connect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::Connect WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_CONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::Connect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothHfpHfProxy::Disconnect(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::Disconnect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::Disconnect WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::Disconnect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

bool BluetoothHfpHfProxy::OpenVoiceRecognition(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::OpenVoiceRecognition WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::OpenVoiceRecognition WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_OPEN_VOICE_RECOGNITION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::OpenVoiceRecognition done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::CloseVoiceRecognition(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::CloseVoiceRecognition WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::CloseVoiceRecognition WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_CLOSE_VOICE_RECOGNITION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::CloseVoiceRecognition done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int BluetoothHfpHfProxy::GetCurrentCallList(const BluetoothRawAddress &device, std::vector<BluetoothHfpHfCall> &calls) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::GetCurrentCallList WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::GetCurrentCallList WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_GET_CURRENT_CALL_LIST, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::GetCurrentCallList done fail, error: %{public}d", error);
        return ERROR;
    }
    uint32_t callNum = reply.ReadUint32();
    for (uint32_t i = 0; i < callNum; i++) {
        std::shared_ptr<BluetoothHfpHfCall> call(reply.ReadParcelable<BluetoothHfpHfCall>());
        if (!call) {
            return TRANSACTION_ERR;
        }
        calls.push_back(*call);
    }
    return NO_ERROR;
}

bool BluetoothHfpHfProxy::AcceptIncomingCall(const BluetoothRawAddress &device, int flag) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::AcceptIncomingCall WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::AcceptIncomingCall WriteParcelable error");
        return false;
    }
    if (!data.WriteInt32(flag)) {
        HILOGE("BluetoothHfpHfProxy::AcceptIncomingCall WriteInt32 error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_ACCEPT_INCOMING_CALL, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::AcceptIncomingCall done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::HoldActiveCall(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::HoldActiveCall WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::HoldActiveCall WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_HOLD_ACTIVE_CALL, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::HoldActiveCall done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::RejectIncomingCall(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::RejectIncomingCall WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::RejectIncomingCall WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_REJECT_INCOMING_CALL, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::RejectIncomingCall done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::HandleIncomingCall(const BluetoothRawAddress &device, int flag)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::HandleIncomingCall WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::HandleIncomingCall WriteParcelable error");
        return false;
    }
    if (!data.WriteInt32(flag)) {
        HILOGE("BluetoothHfpHfProxy::HandleIncomingCall WriteInt32 error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_HANDLE_INCOMING_CALL, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::HandleIncomingCall done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::HandleMultiCall(const BluetoothRawAddress &device, int flag, int index)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::HandleMultiCall WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::HandleMultiCall WriteParcelable error");
        return false;
    }
    if (!data.WriteInt32(flag)) {
        HILOGE("BluetoothHfpHfProxy::HandleMultiCall WriteInt32 flag error");
        return false;
    }
    if (!data.WriteInt32(index)) {
        HILOGE("BluetoothHfpHfProxy::HandleMultiCall WriteInt32 index error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_HANDLE_MULLTI_CALL, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::HandleMultiCall done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::DialLastNumber(const BluetoothRawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::DialLastNumber WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::DialLastNumber WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_DIAL_LAST_NUMBER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::DialLastNumber done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::DialMemory(const BluetoothRawAddress &device, int index)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::DialMemory WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::DialMemory WriteParcelable error");
        return false;
    }

    if (!data.WriteInt32(index)) {
        HILOGE("BluetoothHfpHfProxy::DialMemory WriteInt32 error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_DIAL_MEMORY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::DialMemory done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::SendVoiceTag(const BluetoothRawAddress &device, int index) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::SendVoiceTag WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::SendVoiceTag WriteParcelable error");
        return false;
    }

    if (!data.WriteInt32(index)) {
        HILOGE("BluetoothHfpHfProxy::SendVoiceTag WriteInt32 error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_SEND_VOICE_TAG, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::SendVoiceTag done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::SendKeyPressed(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::SendVoiceTag WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::SendVoiceTag WriteParcelable error");
        return false;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_SEND_KEY_PRESSED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::SendKeyPressed done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpHfProxy::FinishActiveCall(const BluetoothRawAddress &device, const BluetoothHfpHfCall &call) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::FinishActiveCall WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::FinishActiveCall WriteParcelable error");
        return false;
    }
    if (!data.WriteParcelable(&call)) {
        HILOGE("BluetoothHfpHfProxy::FinishActiveCall WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_FINISH_ATIVE_CALL, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::FinishActiveCall done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int BluetoothHfpHfProxy::StartDial(const BluetoothRawAddress &device, const std::string &number, 
    BluetoothHfpHfCall &call) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::StartDial WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfProxy::StartDial WriteParcelable error");
        return ERROR;
    }
    if (!data.WriteString(number)) {
        HILOGE("BluetoothHfpHfProxy::StartDial WriteString error");
        return ERROR;
    }
    if (!data.WriteParcelable(&call)) {
        HILOGE("BluetoothHfpHfProxy::StartDial WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_START_DIAL, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::StartDial done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothHfpHfProxy::RegisterObserver(const sptr<IBluetoothHfpHfObserver> &observer) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHfpHfProxy::RegisterObserver WriteRemoteObject error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfProxy::DeregisterObserver(const sptr<IBluetoothHfpHfObserver> &observer) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfProxy::DeregisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHfpHfProxy::DeregisterObserver WriteRemoteObject error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHf::Code::BT_HFP_HF_DEREGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfProxy::DeregisterObserver done fail, error: %{public}d", error);
        return;
    }
}

}  // namespace Bluetooth
}  // namespace OHOS
