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

#include "bluetooth_hfp_ag_proxy.h"
#include "bluetooth_errorcode.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
int BluetoothHfpAgProxy::GetConnectDevices(std::vector<BluetoothRawAddress> &devices) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::GetConnectDevices WriteInterfaceToken error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_GET_CONNECT_DEVICES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::GetConnectDevices done fail, error: %{public}d", error);
        return ERROR;
    }
    uint32_t DevNum = reply.ReadUint32();
    for (uint32_t i = 0; i < DevNum; i++) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return TRANSACTION_ERR;
        }
        devices.push_back(*dev);
    }
    return NO_ERROR;
}

int BluetoothHfpAgProxy::GetDevicesByStates(const std::vector<int> &states, std::vector<BluetoothRawAddress> &devices) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::GetDevicesByStates WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteInt32Vector(states)) {
        HILOGE("BluetoothHfpAgProxy::GetDevicesByStates WriteInt32Vector error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_GET_DEVICES_BY_STATES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return ERROR;
    }
    uint32_t DevNum = reply.ReadUint32();
    for (uint32_t i = 0; i < DevNum; i++) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return TRANSACTION_ERR;
        }
        devices.push_back(*dev);
    }
    return NO_ERROR;
}
int BluetoothHfpAgProxy::GetDeviceState(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::GetDeviceState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::GetDeviceState WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_GET_DEVICE_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::GetDeviceState done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothHfpAgProxy::Connect(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::Connect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::Connect WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_CONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::Connect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothHfpAgProxy::Disconnect(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::Disconnect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::Disconnect WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::Disconnect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothHfpAgProxy::GetScoState(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::GetScoState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::GetScoState WriteParcelable error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_GET_SCO_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::GetScoState done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();   
}

bool BluetoothHfpAgProxy::ConnectSco() {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::ConnectSco WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_CONNECT_SCO, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::ConnectSco done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();   
}

bool BluetoothHfpAgProxy::DisconnectSco() {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::DisconnectSco WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_DISCONNECT_SCO, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::DisconnectSco done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();       
}

void BluetoothHfpAgProxy::PhoneStateChanged(int numActive, int numHeld, int callState, const std::string &number, 
    int type, const std::string &name) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(numActive)) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged WriteInt32 error");
        return;
    }
    if (!data.WriteInt32(numHeld)) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged WriteInt32 error");
        return;
    }
    if (!data.WriteInt32(callState)) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged WriteInt32 error");
        return;
    }
    if (!data.WriteString(number)) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged WriteString error");
        return;
    }
    if (!data.WriteInt32(type)) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged WriteInt32 error");
        return;
    }
    if (!data.WriteString(name)) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged WriteString error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_PHONE_STATE_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::PhoneStateChanged done fail, error: %{public}d", error);
        return;
    }    
}

void BluetoothHfpAgProxy::ClccResponse(int index, int direction, int status, int mode, bool mpty, 
    const std::string &number, int type) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(index)) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteInt32 error");
        return;
    }
    if (!data.WriteInt32(direction)) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteInt32 error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteInt32 error");
        return;
    }
    if (!data.WriteInt32(mode)) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteInt32 error");
        return;
    }
    if (!data.WriteBool(mpty)) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteBool error");
        return;
    }
    if (!data.WriteString(number)) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteString error");
        return;
    }
    if (!data.WriteInt32(type)) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_CLCC_RESPONSE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::ClccResponse done fail, error: %{public}d", error);
        return;
    } 
}

bool BluetoothHfpAgProxy::OpenVoiceRecognition(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::OpenVoiceRecognition WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::OpenVoiceRecognition WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_OPEN_VOICE_RECOGNITION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::OpenVoiceRecognition done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpAgProxy::CloseVoiceRecognition(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::CloseVoiceRecognition WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::CloseVoiceRecognition WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_CLOSE_VOICE_RECOGNITION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::CloseVoiceRecognition done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpAgProxy::SetActiveDevice(const BluetoothRawAddress &device) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::SetActiveDevice WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::SetActiveDevice WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_SET_ACTIVE_DEVICE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::SetActiveDevice done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpAgProxy::IntoMock(const BluetoothRawAddress &device, int state)
{
        MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::IntoMock WriteInterfaceToken error");
        return false;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::IntoMock WriteParcelable error");
        return false;
    }

    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothHfpAgProxy::IntoMock WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_INTO_MOCK, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::IntoMock done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHfpAgProxy::SendNoCarrier(const BluetoothRawAddress &device)
{
        MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::SendNoCarrier WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpAgProxy::SendNoCarrier WriteParcelable error");
        return false;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_SEND_NO_CARRIER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::SendNoCarrier done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

std::string BluetoothHfpAgProxy::GetActiveDevice() {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::GetActiveDevice WriteInterfaceToken error");
        return "";
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_GET_ACTIVE_DEVICE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::GetActiveDevice done fail, error: %{public}d", error);
        return "";
    }
    return reply.ReadString();
}

void BluetoothHfpAgProxy::RegisterObserver(const sptr<IBluetoothHfpAgObserver> &observer) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHfpAgProxy::RegisterObserver WriteRemoteObject error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpAgProxy::DeregisterObserver(const sptr<IBluetoothHfpAgObserver> &observer) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpAgProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpAgProxy::DeregisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHfpAgProxy::DeregisterObserver WriteRemoteObject error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpAg::Code::BT_HFP_AG_DEREGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpAgProxy::DeregisterObserver done fail, error: %{public}d", error);
        return;
    }
}

}  // namespace Bluetooth
}  // namespace OHOS
