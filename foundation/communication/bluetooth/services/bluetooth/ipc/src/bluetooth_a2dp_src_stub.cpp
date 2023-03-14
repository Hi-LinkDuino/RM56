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

#include "bluetooth_a2dp_src_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "parcel_bt_uuid.h"
#include "raw_address.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
BluetoothA2dpSrcStub::BluetoothA2dpSrcStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_CONNECT)] =
        &BluetoothA2dpSrcStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_DISCONNECT)] =
        &BluetoothA2dpSrcStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_REGISTER_OBSERVER)] =
        &BluetoothA2dpSrcStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_DEREGISTER_OBSERVER)] =
        &BluetoothA2dpSrcStub::DeregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_DEVICE_BY_STATES)] =
        &BluetoothA2dpSrcStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_DEVICE_STATE)] =
        &BluetoothA2dpSrcStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_PLAYING_STATE)] =
        &BluetoothA2dpSrcStub::GetPlayingStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_SET_CONNECT_STRATEGY)] =
        &BluetoothA2dpSrcStub::SetConnectStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_CONNECT_STRATEGY)] =
        &BluetoothA2dpSrcStub::GetConnectStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_SET_ACTIVE_SINK_DEVICE)] =
        &BluetoothA2dpSrcStub::SetActiveSinkDeviceInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_ACTIVE_SINK_DEVICE)] =
        &BluetoothA2dpSrcStub::GetActiveSinkDeviceInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_CODEC_STATUS)] =
        &BluetoothA2dpSrcStub::GetCodecStatusInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_SET_CODEC_PREFERENCE)] =
        &BluetoothA2dpSrcStub::SetCodecPreferenceInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_SWITCH_OPTIONAL_CODECS)] =
        &BluetoothA2dpSrcStub::SwitchOptionalCodecsInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_OPTIONAL_CODECS_SUPPORT_STATE)] =
        &BluetoothA2dpSrcStub::GetOptionalCodecsSupportStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_START_PLAYING)] =
        &BluetoothA2dpSrcStub::StartPlayingInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_SUSPEND_PLAYING)] =
        &BluetoothA2dpSrcStub::SuspendPlayingInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_STOP_PLAYING)] =
        &BluetoothA2dpSrcStub::StopPlayingInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_WRITE_FRAME)] =
        &BluetoothA2dpSrcStub::WriteFrameInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSrc::Code::BT_A2DP_SRC_GET_RENDER_POSITION)] =
        &BluetoothA2dpSrcStub::GetRenderPositionInner;
}

BluetoothA2dpSrcStub::~BluetoothA2dpSrcStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothA2dpSrcStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothA2dpSrcStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothA2dpSrcStub::GetDescriptor();
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
    HILOGW("BluetoothA2dpSrcStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothA2dpSrcStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = Connect(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: ConnectInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = Disconnect(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: DisconnectInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothA2dpSourceObserver> observer = OHOS::iface_cast<IBluetoothA2dpSourceObserver>(remote);
    RegisterObserver(observer);

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothA2dpSourceObserver> observer = OHOS::iface_cast<IBluetoothA2dpSourceObserver>(remote);
    DeregisterObserver(observer);

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int32_t> states = {};
    int32_t stateSize = data.ReadInt32();

    for (int i = 0; i < stateSize; i++) {
        int32_t state = data.ReadInt32();
        states.push_back(state);
    }

    std::vector<RawAddress> rawAdds = GetDevicesByStates(states);
    reply.WriteInt32(rawAdds.size());

    for (auto rawAdd : rawAdds) {
        if (!reply.WriteString(rawAdd.GetAddress())) {
            HILOGE("GetConnectedDevicesInner: write WriteString failed");
            return ERR_INVALID_STATE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = GetDeviceState(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: GetDeviceStateInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetPlayingStateInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = GetPlayingState(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: GetPlayingStateInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::SetConnectStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int strategy = data.ReadInt32();

    int result = SetConnectStrategy(RawAddress(addr), strategy);

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: SetConnectStrategyInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetConnectStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int result = GetConnectStrategy(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: GetConnectStrategyInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::SetActiveSinkDeviceInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int result = SetActiveSinkDevice(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: SetActiveSinkDeviceInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetActiveSinkDeviceInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress adds = GetActiveSinkDevice();

    if (!reply.WriteString(adds.GetAddress())) {
        HILOGE("GetActiveSinkDeviceInner: write WriteString failed");
        return ERR_INVALID_STATE;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetCodecStatusInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    BluetoothA2dpCodecStatus result = GetCodecStatus(RawAddress(addr));

    bool ret = reply.WriteParcelable(&result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: GetCodecStatusInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::SetCodecPreferenceInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    std::shared_ptr<BluetoothA2dpCodecInfo> info(data.ReadParcelable<BluetoothA2dpCodecInfo>());
    if (!info) {
        return TRANSACTION_ERR;
    }
    int result = SetCodecPreference(RawAddress(addr), *info);

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: SetCodecPreferenceInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::SwitchOptionalCodecsInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    bool isEnable = data.ReadBool();
    SwitchOptionalCodecs(RawAddress(addr), isEnable);
    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetOptionalCodecsSupportStateInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int result = GetOptionalCodecsSupportState(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: GetOptionalCodecsSupportStateInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::StartPlayingInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int result = StartPlaying(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: StartPlayingInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::SuspendPlayingInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int result = SuspendPlaying(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: SuspendPlayingInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::StopPlayingInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int result = StopPlaying(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSrcStub: StopPlayingInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::WriteFrameInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<uint8_t> dataVector;
    if (!data.ReadUInt8Vector(&dataVector)) {
        HILOGE("BluetoothA2dpSrcStub: WriteFrameInner data reading failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    int ret = WriteFrame(dataVector.data(), dataVector.size());
    if (!reply.WriteInt32(ret)) {
        HILOGE("BluetoothA2dpSrcStub: WriteFrameInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSrcStub::GetRenderPositionInner(MessageParcel &data, MessageParcel &reply)
{
    uint16_t delayValue;
    uint16_t sendDataSize;
    uint32_t timeStamp;
    GetRenderPosition(delayValue, sendDataSize, timeStamp);
    if (!reply.WriteUint16(delayValue)) {
        HILOGE("BluetoothA2dpSrcStub: GetRenderPositionInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    if (!reply.WriteUint16(sendDataSize)) {
        HILOGE("BluetoothA2dpSrcStub: GetRenderPositionInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    if (!reply.WriteUint32(timeStamp)) {
        HILOGE("BluetoothA2dpSrcStub: GetRenderPositionInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

}  // namespace Bluetooth
}  // namespace OHOS