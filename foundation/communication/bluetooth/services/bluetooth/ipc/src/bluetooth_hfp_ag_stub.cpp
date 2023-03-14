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

#include "bluetooth_hfp_ag_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothHfpAgStub::BluetoothHfpAgStub() {
    HILOGI("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_GET_CONNECT_DEVICES)] =
        &BluetoothHfpAgStub::GetConnectDevicesInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_GET_DEVICES_BY_STATES)] =
        &BluetoothHfpAgStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_GET_DEVICE_STATE)] =
        &BluetoothHfpAgStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_CONNECT)] =
        &BluetoothHfpAgStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_DISCONNECT)] =
        &BluetoothHfpAgStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_GET_SCO_STATE)] =
        &BluetoothHfpAgStub::GetScoStateInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_CONNECT_SCO)] =
        &BluetoothHfpAgStub::ConnectScoInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_DISCONNECT_SCO)] =
        &BluetoothHfpAgStub::DisconnectScoInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_PHONE_STATE_CHANGED)] =
        &BluetoothHfpAgStub::PhoneStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_CLCC_RESPONSE)] =
        &BluetoothHfpAgStub::ClccResponseInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_OPEN_VOICE_RECOGNITION)] =
        &BluetoothHfpAgStub::OpenVoiceRecognitionInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_CLOSE_VOICE_RECOGNITION)] =
        &BluetoothHfpAgStub::CloseVoiceRecognitionInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_SET_ACTIVE_DEVICE)] =
        &BluetoothHfpAgStub::SetActiveDeviceInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_GET_ACTIVE_DEVICE)] =
        &BluetoothHfpAgStub::GetActiveDeviceInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_INTO_MOCK)] =
        &BluetoothHfpAgStub::IntoMockInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_SEND_NO_CARRIER)] =
        &BluetoothHfpAgStub::SendNoCarrierInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_REGISTER_OBSERVER)] =
        &BluetoothHfpAgStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(
        BluetoothHfpAgStub::Code::BT_HFP_AG_DEREGISTER_OBSERVER)] =
        &BluetoothHfpAgStub::DeregisterObserverInner;

    HILOGI("%{public}s ends.", __func__);
}

BluetoothHfpAgStub::~BluetoothHfpAgStub() {
    HILOGI("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothHfpAgStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) {
    HILOGI("BluetoothHfpAgStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothHfpAgStub::GetDescriptor();
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
    HILOGW("BluetoothHfpAgStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);   
}

ErrCode BluetoothHfpAgStub::GetConnectDevicesInner(MessageParcel &data, MessageParcel &reply) {
    std::vector<BluetoothRawAddress> devices;
    GetConnectDevices(devices);
    uint32_t DevNum = devices.size();
    if (!reply.WriteUint32(DevNum)) {
        HILOGE("BluetoothHfpAgStub:WriteInt32 failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    for (uint32_t i = 0; i < DevNum; i++) {
        if (!reply.WriteParcelable(&devices[i])) {
            HILOGE("BluetoothHfpAgStub:WriteParcelable failed in: %{public}s.", __func__);
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply) {
    std::vector<BluetoothRawAddress> devices;
    std::vector<int> states;
    data.ReadInt32Vector(&states);
    GetDevicesByStates(states, devices);
    uint32_t DevNum = devices.size();
    if (!reply.WriteUint32(DevNum)) {
        HILOGE("BluetoothHfpAgStub:WriteInt32 failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    for (uint32_t i = 0; i < DevNum; i++) {
        if (!reply.WriteParcelable(&devices[i])) {
            HILOGE("BluetoothHfpAgStub:WriteParcelable failed in: %{public}s.", __func__);
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetDeviceState(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::ConnectInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Connect(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::DisconnectInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Disconnect(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::GetScoStateInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetScoState(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::ConnectScoInner(MessageParcel &data, MessageParcel &reply) {
    bool result = ConnectSco();
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::DisconnectScoInner(MessageParcel &data, MessageParcel &reply) {
    bool result = DisconnectSco();
    if (!reply.WriteBool(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::PhoneStateChangedInner(MessageParcel &data, MessageParcel &reply) {
    int numActive = data.ReadInt32();
    int numHeld = data.ReadInt32();
    int callState = data.ReadInt32();
    std::string number = data.ReadString();
    int type = data.ReadInt32();
    std::string name = data.ReadString();
    PhoneStateChanged(numActive, numHeld, callState, number, type, name);
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::ClccResponseInner(MessageParcel &data, MessageParcel &reply) {
    int index = data.ReadInt32();
    int direction = data.ReadInt32();
    int status = data.ReadInt32();
    int mode = data.ReadInt32();
    bool mpty = data.ReadBool();
    std::string number = data.ReadString();
    int type = data.ReadInt32();
    ClccResponse(index, direction, status, mode, mpty, number, type);
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::OpenVoiceRecognitionInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = OpenVoiceRecognition(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::CloseVoiceRecognitionInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = CloseVoiceRecognition(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::SetActiveDeviceInner(MessageParcel &data, MessageParcel &reply) {
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = SetActiveDevice(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::IntoMockInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int state = data.ReadInt32();
    int result = IntoMock(*device, state);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::SendNoCarrierInner(MessageParcel &data, MessageParcel &reply)
{
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = SendNoCarrier(*device);
    if (!reply.WriteInt32(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}



ErrCode BluetoothHfpAgStub::GetActiveDeviceInner(MessageParcel &data, MessageParcel &reply) {
    std::string result = GetActiveDevice();
    if (!reply.WriteString(result)) {
        HILOGE("BluetoothHfpAgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply) {
    sptr<IRemoteObject> tempObject = data.ReadRemoteObject();
    sptr<IBluetoothHfpAgObserver> observer = iface_cast<IBluetoothHfpAgObserver>(tempObject);
    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHfpAgStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply) {
    sptr<IRemoteObject> tempObject = data.ReadRemoteObject();
    sptr<IBluetoothHfpAgObserver> observer = iface_cast<IBluetoothHfpAgObserver>(tempObject);
    DeregisterObserver(observer);
    return NO_ERROR;
}

}  // namespace Bluetooth
}  // namespace OHOS
