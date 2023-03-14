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

#include "bluetooth_avrcp_ct_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "string_ex.h"

namespace OHOS {
namespace Bluetooth {
BluetoothAvrcpCtStub::BluetoothAvrcpCtStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_REGISTER_OBSERVER)] =
        &BluetoothAvrcpCtStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_UNREGISTER_OBSERVER)] =
        &BluetoothAvrcpCtStub::UnregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_CONNECTED_DEVICES)] =
        &BluetoothAvrcpCtStub::GetConnectedDevicesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_DEVICES_BY_STATES)] =
        &BluetoothAvrcpCtStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_DEVICE_STATE)] =
        &BluetoothAvrcpCtStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_CONNECT)] =
        &BluetoothAvrcpCtStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_DISCONNECT)] =
        &BluetoothAvrcpCtStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_PRESS_BUTTON)] =
        &BluetoothAvrcpCtStub::PressButtonnner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_RELEASE_BUTTON)] =
        &BluetoothAvrcpCtStub::ReleaseButtonInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_UNIT_INFO)] =
        &BluetoothAvrcpCtStub::GetUnitInfoInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_SUB_UNIT_INFO)] =
        &BluetoothAvrcpCtStub::GetSubUnitInfoInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_SUPPORTED_COMPANIES)] =
        &BluetoothAvrcpCtStub::GetSupportedCompaniesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_SUPPORTED_EVENTS)] =
        &BluetoothAvrcpCtStub::GetSupportedEventsInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTES)] =
        &BluetoothAvrcpCtStub::GetPlayerAppSettingAttributesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_VALUES)] =
        &BluetoothAvrcpCtStub::GetPlayerAppSettingValuesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_CURRENT_VALUE)] =
        &BluetoothAvrcpCtStub::GetPlayerAppSettingCurrentValueInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_SET_PLAYER_APP_SETTING_CURRENT_VALUE)] =
        &BluetoothAvrcpCtStub::SetPlayerAppSettingCurrentValueInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTE_TEXT)] =
        &BluetoothAvrcpCtStub::GetPlayerAppSettingAttributeTextInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_VALUES_TEXT)] =
        &BluetoothAvrcpCtStub::GetPlayerAppSettingValueTextInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_ELEMENT_ATTRIBUTES)] =
        &BluetoothAvrcpCtStub::GetElementAttributesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_STATUS)] =
        &BluetoothAvrcpCtStub::GetPlayStatusInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_PLAY_ITEM)] =
        &BluetoothAvrcpCtStub::PlayItemInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_FOLDER_ITEMS)] =
        &BluetoothAvrcpCtStub::GetFolderItemsInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS)] =
        &BluetoothAvrcpCtStub::GetTotalNumberOfItemsInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_SET_ABSOLUTE_VOLUME)] =
        &BluetoothAvrcpCtStub::SetAbsoluteVolumeInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_ENABLE_NOTIFICATION)] =
        &BluetoothAvrcpCtStub::EnableNotificationInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_DISABLE_NOTIFICATION)] =
        &BluetoothAvrcpCtStub::DisableNotificationInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_GET_ITEM_ATTRIBUTES)] =
        &BluetoothAvrcpCtStub::GetItemAttributesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_SET_BROWSERED_PLAYER)] =
        &BluetoothAvrcpCtStub::SetBrowsedPlayerInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothAvrcpCt::Code::AVRCP_CT_MEDIA_PLAYER_LIST)] =
        &BluetoothAvrcpCtStub::GetMeidaPlayerListInner;
}

BluetoothAvrcpCtStub::~BluetoothAvrcpCtStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothAvrcpCtStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothAvrcpCtStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
    std::u16string descriptor = BluetoothAvrcpCtStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGI("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data,reply);
        }
    }

    HILOGW("BluetoothAvrcpCtStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothAvrcpCtStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject>  tempObject = data.ReadRemoteObject();    
    sptr<IBluetoothAvrcpCtObserver> observer;
    observer = iface_cast<IBluetoothAvrcpCtObserver>(tempObject);
    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::UnregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject>  tempObject = data.ReadRemoteObject();
    sptr<IBluetoothAvrcpCtObserver> observer;
    observer = iface_cast<IBluetoothAvrcpCtObserver>(tempObject);
    UnregisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetConnectedDevicesInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<RawAddress> rawAdds = GetConnectedDevices();
    reply.WriteInt32(rawAdds.size());
    for (auto rawAdd : rawAdds) {
        if (!reply.WriteString(rawAdd.GetAddress())) {
            HILOGE("GetConnectedDevicesInner: write WriteString failed");
            return ERR_INVALID_STATE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int32_t> states = {};
    int32_t stateSize = data.ReadInt32();
    if (IsInvalidDeviceStatesSize(stateSize)) {
        HILOGE("BluetoothAvrcpCtStub::GetDevicesByStatesInner Invalid MessageParcel device states");
        return ERR_INVALID_VALUE;
    }
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

ErrCode BluetoothAvrcpCtStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t state = GetDeviceState(RawAddress(data.ReadString()));
    reply.WriteInt32(state);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = Connect(RawAddress(data.ReadString()));
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = Disconnect(RawAddress(data.ReadString()));
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::PressButtonnner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = PressButton(RawAddress(data.ReadString()), data.ReadInt32());
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::ReleaseButtonInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = ReleaseButton(RawAddress(data.ReadString()), data.ReadInt32());
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetUnitInfoInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetUnitInfo(RawAddress(data.ReadString()));
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetSubUnitInfoInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetSubUnitInfo(RawAddress(data.ReadString()));
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetSupportedCompaniesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetSupportedCompanies(RawAddress(data.ReadString()));
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetSupportedEventsInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetSupportedEvents(RawAddress(data.ReadString()));
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetPlayerAppSettingAttributesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetPlayerAppSettingAttributes(RawAddress(data.ReadString()));
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetPlayerAppSettingValuesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetPlayerAppSettingValues(RawAddress(data.ReadString()), data.ReadInt32());
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetPlayerAppSettingCurrentValueInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    std::vector<int32_t> attributes = {};
    int32_t attributesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(attributesSize)) {
        HILOGE("BluetoothAvrcpCtStub::GetPlayerAppSettingCurrentValueInner Invalid MessageParcel attributes");
        return ERR_INVALID_VALUE;
    }
    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        attributes.push_back(attribute);
    }
    int32_t result = GetPlayerAppSettingCurrentValue(raw, attributes);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::SetPlayerAppSettingCurrentValueInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    std::vector<int32_t> attributes = {};
    int32_t attributesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(attributesSize)) {
        HILOGE("BluetoothAvrcpCtStub::SetPlayerAppSettingCurrentValueInner Invalid MessageParcel attributes");
        return ERR_INVALID_VALUE;
    }
    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        attributes.push_back(attribute);
    }

    std::vector<int32_t> values = {};
    int32_t valuesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(valuesSize)) {
        HILOGE("BluetoothAvrcpCtStub::SetPlayerAppSettingCurrentValueInner Invalid MessageParcel values");
        return ERR_INVALID_VALUE;
    }
    for (int i = 0; i < valuesSize; i++) {
        int32_t value = data.ReadInt32();
        values.push_back(value);
    }

    int32_t result = SetPlayerAppSettingCurrentValue(raw, attributes, values);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetPlayerAppSettingAttributeTextInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    std::vector<int32_t> attributes = {};
    int32_t attributesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(attributesSize)) {
        HILOGE("BluetoothAvrcpCtStub::GetPlayerAppSettingAttributeTextInner Invalid MessageParcel attributes");
        return ERR_INVALID_VALUE;
    }
    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        attributes.push_back(attribute);
    }
    int32_t result = GetPlayerAppSettingAttributeText(raw, attributes);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetPlayerAppSettingValueTextInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t attributes = data.ReadInt32();
    std::vector<int32_t> values = {};
    int32_t valuesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(valuesSize)) {
        HILOGE("BluetoothAvrcpCtStub::GetPlayerAppSettingValueTextInner Invalid MessageParcel values");
        return ERR_INVALID_VALUE;
    }
    for (int i = 0; i < valuesSize; i++) {
        int32_t value = data.ReadInt32();
        values.push_back(value);
    }

    int32_t result = GetPlayerAppSettingValueText(raw, attributes, values);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetElementAttributesInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    std::vector<int32_t> attributes = {};
    int32_t attributesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(attributesSize)) {
        HILOGE("BluetoothAvrcpCtStub::GetElementAttributesInner Invalid MessageParcel attributes");
        return ERR_INVALID_VALUE;
    }
    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        attributes.push_back(attribute);
    }
    int32_t result = GetElementAttributes(raw, attributes);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetPlayStatusInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t result = GetPlayStatus(raw);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::PlayItemInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t result = PlayItem(raw, data.ReadInt32(), data.ReadInt64(), data.ReadInt32());
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetFolderItemsInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t startItem = data.ReadInt32();
    int32_t endItem = data.ReadInt32();
    std::vector<int32_t> attributes = {};
    int32_t attributesSize = data.ReadInt32();
    if (IsInvalidAttributesSize(attributesSize)) {
        HILOGE("BluetoothAvrcpCtStub::GetFolderItemsInner Invalid MessageParcel attributes");
        return ERR_INVALID_VALUE;
    }
    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        attributes.push_back(attribute);
    }

    int32_t result = GetFolderItems(raw, startItem, endItem, attributes);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetTotalNumberOfItemsInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t scope = data.ReadInt32();
    int32_t result = GetTotalNumberOfItems(raw, scope);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::SetAbsoluteVolumeInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t volume = data.ReadInt32();
    int32_t result = SetAbsoluteVolume(raw, volume);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::EnableNotificationInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    std::vector<int32_t> events = {};
    int32_t attributesSize = data.ReadInt32();

    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        events.push_back(attribute);
    }
    int32_t interval = data.ReadInt32();
    int32_t result = EnableNotification(raw, events, interval);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::DisableNotificationInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    std::vector<int32_t> events = {};
    int32_t attributesSize = data.ReadInt32();

    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        events.push_back(attribute);
    }
    int32_t result = DisableNotification(raw, events);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::GetItemAttributesInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int64_t uid = data.ReadInt64();
    int32_t uidCounter = data.ReadInt32();
    std::vector<int32_t> events = {};
    int32_t attributesSize = data.ReadInt32();

    for (int i = 0; i < attributesSize; i++) {
        int32_t attribute = data.ReadInt32();
        events.push_back(attribute);
    }
    int32_t result = GetItemAttributes(raw, uid, uidCounter, events);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode BluetoothAvrcpCtStub::SetBrowsedPlayerInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t playerId = data.ReadInt32();

    int32_t result = SetBrowsedPlayer(raw, playerId);
    reply.WriteInt32(result);

    return NO_ERROR;

}

ErrCode BluetoothAvrcpCtStub::GetMeidaPlayerListInner(MessageParcel &data, MessageParcel &reply)
{
    RawAddress raw = RawAddress(data.ReadString());
    int32_t startItem = data.ReadInt32();
    int32_t endItem = data.ReadInt32();

    int32_t result = GetMeidaPlayerList(raw, startItem, endItem);
    reply.WriteInt32(result);

    return NO_ERROR;
}

bool BluetoothAvrcpCtStub::IsInvalidAttributesSize(int32_t attributesSize)
{
    bool ret = false;
    const int32_t COUNT_OF_AVRC_PLAYER_ATTRIBUTE = 255;
    if (attributesSize > COUNT_OF_AVRC_PLAYER_ATTRIBUTE) {
        ret =  true;
    }
    return ret;
}

bool BluetoothAvrcpCtStub::IsInvalidDeviceStatesSize(int32_t statesSize)
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
