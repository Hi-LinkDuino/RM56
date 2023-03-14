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

#include "bluetooth_avrcp_tg_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"

namespace OHOS {
namespace Bluetooth {
BluetoothAvrcpTgStub::BluetoothAvrcpTgStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_REGISTER_OBSERVER)] =
        &BluetoothAvrcpTgStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_UNREGISTER_OBSERVER)] =
        &BluetoothAvrcpTgStub::UnregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_SET_ACTIVE_DEVICE)] =
        &BluetoothAvrcpTgStub::SetActiveDeviceInner; 
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_CONNECT)] =
        &BluetoothAvrcpTgStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_DISCONNECT)] =
        &BluetoothAvrcpTgStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_GET_CONNECTED_DEVICES)] =
        &BluetoothAvrcpTgStub::GetConnectedDevicesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_GET_DEVICES_BY_STATES)] =
        &BluetoothAvrcpTgStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_GET_DEVICE_STATE)] =
        &BluetoothAvrcpTgStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_PLAYBACK_STATUS_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyPlaybackStatusChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_TRACK_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyTrackChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_TRACK_REACHED_END)] =
        &BluetoothAvrcpTgStub::NotifyTrackReachedEndInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_TRACK_REACHED_START)] =
        &BluetoothAvrcpTgStub::NotifyTrackReachedStartInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_PLAYBACK_POS_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyPlaybackPosChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_PLAYER_APP_SETTING_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyPlayerAppSettingChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_NOWPLAYING_CONTENT_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyNowPlayingContentChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_AVAILABLE_PLAYERS_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyAvailablePlayersChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_ADDRESSED_PLAYER_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyAddressedPlayerChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_UID_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyUidChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpTg::Code::BT_AVRCP_TG_NOTIFY_VOLUME_CHANGED)] =
        &BluetoothAvrcpTgStub::NotifyVolumeChangedInner;
}

BluetoothAvrcpTgStub::~BluetoothAvrcpTgStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothAvrcpTgStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothAvrcpTgStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothAvrcpTgStub::GetDescriptor();
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
    HILOGW("BluetoothAvrcpTgStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothAvrcpTgStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::RegisterObserverInner starts");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothAvrcpTgObserver> observer = OHOS::iface_cast<IBluetoothAvrcpTgObserver>(remote);

    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::UnregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::UnregisterObserverInner starts");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothAvrcpTgObserver> observer = OHOS::iface_cast<IBluetoothAvrcpTgObserver>(remote);

    UnregisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::SetActiveDeviceInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::SetActiveDeviceInner starts");
    std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    SetActiveDevice(*addr);
    return NO_ERROR;
}

  
ErrCode BluetoothAvrcpTgStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::ConnectInner starts");
    std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }

    int result = Connect(*addr);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothAvrcpTgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::DisconnectInner starts");
    std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }

    int result = Disconnect(*addr);

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothAvrcpTgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::GetConnectedDevicesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::GetConnectedDevicesInner starts");
    std::vector<BluetoothRawAddress> result = GetConnectedDevices();
    if (!reply.WriteInt32(result.size())) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    } else {
        for (auto device : result) {
            reply.WriteParcelable(&device);
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t statesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(statesSize)) {
        HILOGE("BluetoothAvrcpTgStub::GetDevicesByStatesInner Invalid MessageParcel device states");
        return ERR_INVALID_VALUE;
    }
    std::vector<int32_t> states;
    for (int i = 0; i < statesSize; ++i) {
        int32_t state = data.ReadInt32();
        states.push_back(state);
    }

    std::vector<BluetoothRawAddress> result = GetDevicesByStates(states);
    if (!reply.WriteInt32(result.size())) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    } else {
        for (auto device : result) {
            reply.WriteParcelable(&device);
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::GetDeviceStateInner starts");
    std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }

    int result = GetDeviceState(*addr);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothAvrcpTgStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyPlaybackStatusChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyPlaybackStatusChangedInner starts");
    int32_t playStatus = data.ReadInt32();
    int32_t playbackPos = data.ReadInt32();

    NotifyPlaybackStatusChanged(playStatus, playbackPos);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyTrackChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyTrackChangedInner starts");
    long uid = data.ReadInt64();
    int32_t playbackPos = data.ReadInt32();

    NotifyTrackChanged(uid, playbackPos);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyTrackReachedEndInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyTrackReachedEndInner starts");
    int32_t playbackPos = data.ReadInt32();

    NotifyTrackReachedEnd(playbackPos);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyTrackReachedStartInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyTrackReachedStartInner starts");
    int32_t playbackPos = data.ReadInt32();

    NotifyTrackReachedStart(playbackPos);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyPlaybackPosChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyPlaybackPosChangedInner starts");
    int32_t playbackPos = data.ReadInt32();

    NotifyPlaybackPosChanged(playbackPos);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyPlayerAppSettingChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyPlayerAppSettingChangedInner starts");
    int32_t attributesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(attributesSize)) {
        HILOGE("BluetoothAvrcpTgStub::NotifyPlayerAppSettingChangedInner Invalid MessageParcel attributes");
        return ERR_INVALID_VALUE;
    }
    std::vector<int32_t> attributes;
    for (int i = 0; i < attributesSize; ++i) {
        int32_t attribute = data.ReadInt32();
        attributes.push_back(attribute);
    }

    int32_t valuesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(valuesSize)) {
        HILOGE("BluetoothAvrcpTgStub::NotifyPlayerAppSettingChangedInner Invalid MessageParcel values");
        return ERR_INVALID_VALUE;
    }
    std::vector<int32_t> values;
    for (int i = 0; i < valuesSize; ++i) {
        int32_t value = data.ReadInt32();
        values.push_back(value);
    }

    NotifyPlayerAppSettingChanged(attributes, values);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyNowPlayingContentChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyNowPlayingContentChangedInner starts");

    NotifyNowPlayingContentChanged();
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyAvailablePlayersChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyAvailablePlayersChangedInner starts");

    NotifyAvailablePlayersChanged();
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyAddressedPlayerChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyAddressedPlayerChangedInner starts");
    int32_t playerId = data.ReadInt32();
    int32_t uidCounter = data.ReadInt32();

    NotifyAddressedPlayerChanged(playerId, uidCounter);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyUidChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyUidChangedInner starts");
    int32_t uidCounter = data.ReadInt32();

    NotifyUidChanged(uidCounter);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpTgStub::NotifyVolumeChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothAvrcpTgStub::NotifyVolumeChangedInner starts");
    int32_t volume = data.ReadInt32();

    NotifyVolumeChanged(volume);
    return NO_ERROR;
}

bool BluetoothAvrcpTgStub::IsInvalidAttributesSize(int32_t attributesSize)
{
    bool ret = false;
    const int32_t COUNT_OF_AVRC_PLAYER_ATTRIBUTE = 255;
    if (attributesSize > COUNT_OF_AVRC_PLAYER_ATTRIBUTE) {
        ret =  true;
    }
    return ret;
}

bool BluetoothAvrcpTgStub::IsInvalidDeviceStatesSize(int32_t statesSize)
{
    bool ret = false;
    const int32_t COUNT_OF_DEVICE_STATE = 4;
    if (statesSize > COUNT_OF_DEVICE_STATE) {
        ret =  true;
    }
    return ret;
}
}  // namespace Bluetooth
}  // namespace OHOS