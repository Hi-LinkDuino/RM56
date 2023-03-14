/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "bluetooth_avrcp_tg_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothAvrcpTgProxy::RegisterObserver(const sptr<IBluetoothAvrcpTgObserver> &observer)
{
    HILOGI("BluetoothAvrcpTgProxy::RegisterObserver start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothAvrcpTgProxy::RegisterObserver observer error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_REGISTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::UnregisterObserver(const sptr<IBluetoothAvrcpTgObserver> &observer)
{
    HILOGI("BluetoothAvrcpTgProxy::UnregisterObserver start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::UnregisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothAvrcpTgProxy::UnregisterObserver observer error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_UNREGISTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::SetActiveDevice(const BluetoothRawAddress &addr)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("[SetActiveDevice] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(&addr)) {
        HILOGE("[SetActiveDevice] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpTg::Code::BT_AVRCP_TG_SET_ACTIVE_DEVICE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::SetActiveDevice done fail, error: %{public}d", error);
        return;
    }
}

int32_t BluetoothAvrcpTgProxy::Connect(const BluetoothRawAddress &addr)
{
    HILOGI("BluetoothAvrcpTgProxy::Connect start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::Connect WriteInterfaceToken error");
        return -1;
    }
    if (!data.WriteParcelable(&addr)) {
        HILOGE("BluetoothAvrcpTgProxy::Connect transport error");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_CONNECT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::Connect done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpTgProxy::Disconnect(const BluetoothRawAddress &addr)
{
    HILOGI("BluetoothAvrcpTgProxy::Disconnect start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::Disconnect WriteInterfaceToken error");
        return -1;
    }
    if (!data.WriteParcelable(&addr)) {
        HILOGE("BluetoothAvrcpTgProxy::Disconnect transport error");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_DISCONNECT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::Disconnect done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

std::vector<BluetoothRawAddress> BluetoothAvrcpTgProxy::GetConnectedDevices()
{
    std::vector<BluetoothRawAddress> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::GetConnectedDevices WriteInterfaceToken error");
        return vec;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_GET_CONNECTED_DEVICES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::GetConnectedDevices done fail, error: %{public}d", error);
        return vec;
    }
    int32_t size = reply.ReadInt32();
    for (int32_t i = 0; i < size; i++) {
        std::shared_ptr<BluetoothRawAddress> rawAddress(reply.ReadParcelable<BluetoothRawAddress>());
        if (!rawAddress) {
            return vec;
        }
        vec.push_back(*rawAddress);
    }
    return vec;
}

std::vector<BluetoothRawAddress> BluetoothAvrcpTgProxy::GetDevicesByStates(const std::vector<int32_t> &states)
{
    std::vector<BluetoothRawAddress> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::GetConnectedDevices WriteInterfaceToken error");
        return vec;
    }
    int num = states.size();
    if (!data.WriteInt32(num)) {
        HILOGE("BluetoothAvrcpTgProxy::GetDevicesByStates num error");
        return vec;
    }
    for (int i = 0; i < num; i++) {
        if (!data.WriteInt32(states[i])) {
            HILOGE("BluetoothAvrcpTgProxy::GetDevicesByStates states error");
            return vec;
        }
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_GET_DEVICES_BY_STATES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::GetConnectedDevices done fail, error: %{public}d", error);
        return vec;
    }
    int32_t size = reply.ReadInt32();
    for (int32_t i = 0; i < size; i++) {
        std::shared_ptr<BluetoothRawAddress> rawAddress(reply.ReadParcelable<BluetoothRawAddress>());
        vec.push_back(*rawAddress);
    }
    return vec;
}

int32_t BluetoothAvrcpTgProxy::GetDeviceState(const BluetoothRawAddress &addr)
{
    HILOGI("BluetoothAvrcpTgProxy::GetDeviceState start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::GetDeviceState WriteInterfaceToken error");
        return -1;
    }
    if (!data.WriteParcelable(&addr)) {
        HILOGE("BluetoothAvrcpTgProxy::GetDeviceState transport error");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_GET_DEVICE_STATE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::GetDeviceState done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

void BluetoothAvrcpTgProxy::NotifyPlaybackStatusChanged(int32_t playStatus, int32_t playbackPos)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyPlaybackStatusChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackStatusChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(playStatus)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackStatusChanged playStatus error");
        return;
    }
    if (!data.WriteInt32(playbackPos)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackStatusChanged playbackPos error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_PLAYBACK_STATUS_CHANGED,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackStatusChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyTrackChanged(int64_t uid, int32_t playbackPos)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyTrackChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt64(uid)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackChanged uid error");
        return;
    }
    if (!data.WriteInt32(playbackPos)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackChanged playbackPos error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_TRACK_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackStatusChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyTrackReachedEnd(int32_t playbackPos)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyTrackReachedEnd start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackReachedEnd WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(playbackPos)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackReachedEnd playbackPos error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_TRACK_REACHED_END,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackReachedEnd done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyTrackReachedStart(int32_t playbackPos)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyTrackReachedStart start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackReachedStart WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(playbackPos)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackReachedStart playbackPos error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_TRACK_REACHED_START,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyTrackReachedStart done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyPlaybackPosChanged(int32_t playbackPos)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyPlaybackPosChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackPosChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(playbackPos)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackPosChanged playbackPos error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_PLAYBACK_POS_CHANGED,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlaybackPosChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyPlayerAppSettingChanged(const std::vector<int32_t> &attributes,
    const std::vector<int32_t> &values)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlayerAppSettingChanged WriteInterfaceToken error");
        return;
    }
    int attributesNum = attributes.size();
    if (!data.WriteInt32(attributesNum)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlayerAppSettingChanged attributesNum error");
        return;
    }
    for (int i = 0; i < attributesNum; i++) {
        if (!data.WriteInt32(attributes[i])) {
            HILOGE("BluetoothAvrcpTgProxy::NotifyPlayerAppSettingChanged attributes error");
            return;
        }
    }

    int valuesNum = values.size();
    if (!data.WriteInt32(valuesNum)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlayerAppSettingChanged valuesNum error");
        return;
    }
    for (int i = 0; i < valuesNum; i++) {
        if (!data.WriteInt32(values[i])) {
            HILOGE("BluetoothAvrcpTgProxy::NotifyPlayerAppSettingChanged values error");
            return;
        }
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_PLAYER_APP_SETTING_CHANGED,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyPlayerAppSettingChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyNowPlayingContentChanged()
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyNowPlayingContentChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyNowPlayingContentChanged WriteInterfaceToken error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_NOWPLAYING_CONTENT_CHANGED,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyNowPlayingContentChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyAvailablePlayersChanged()
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyAvailablePlayersChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyAvailablePlayersChanged WriteInterfaceToken error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };

    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_AVAILABLE_PLAYERS_CHANGED,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyAvailablePlayersChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyAddressedPlayerChanged(int32_t playerId, int32_t uidCounter)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyAddressedPlayerChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyAddressedPlayerChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(playerId)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyAddressedPlayerChanged playStatus error");
        return;
    }
    if (!data.WriteInt32(uidCounter)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyAddressedPlayerChanged playbackPos error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_ADDRESSED_PLAYER_CHANGED,
                              option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyAddressedPlayerChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyUidChanged(int32_t uidCounter)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyUidChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyUidChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(uidCounter)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyUidChanged playStatus error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_UID_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyUidChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothAvrcpTgProxy::NotifyVolumeChanged(int32_t volume)
{
    HILOGI("BluetoothAvrcpTgProxy::NotifyVolumeChanged start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyVolumeChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32(volume)) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyVolumeChanged playStatus error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = InnerTransact(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_VOLUME_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgProxy::NotifyVolumeChanged done fail, error: %{public}d", error);
        return;
    }
    return;
}

ErrCode BluetoothAvrcpTgProxy::InnerTransact(uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOGW("[InnerTransact] fail: get Remote fail code %{public}d", code);
        return OBJECT_NULL;
    }
    int err = remote->SendRequest(code, data, reply, flags);
    switch (err) {
        case NO_ERROR: {
            return NO_ERROR;
        }
        case DEAD_OBJECT: {
            HILOGW("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return DEAD_OBJECT;
        }
        default: {
            HILOGW("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return TRANSACTION_ERR;
        }
    }
}
}  // namespace Bluetooth
}  // namespace OHOS