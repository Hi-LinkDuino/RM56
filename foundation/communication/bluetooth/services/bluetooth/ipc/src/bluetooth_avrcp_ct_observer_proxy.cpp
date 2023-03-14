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

#include "bluetooth_avrcp_ct_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothAvrcpCtObserverProxy::OnConnectionStateChanged(const RawAddress &rawAddr, int state)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[GetDeviceState] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[GetDeviceState] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(state)) {
        HILOGE("[GetDeviceState] fail: write result failed");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_CONNECTION_STATE_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::GetDeviceState done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnPressButton(const RawAddress &rawAddr, uint8_t button, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnPressButton] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnPressButton] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(button)) {
        HILOGE("[OnPressButton] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnPressButton] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_PRESS_BUTTON, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnPressButton done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnReleaseButton(const RawAddress &rawAddr, uint8_t button,  int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnReleaseButton] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnReleaseButton] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(button)) {
        HILOGE("[OnReleaseButton] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnReleaseButton] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_RELEASE_BUTTON, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnReleaseButton done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothAvrcpCtObserverProxy::OnSetBrowsedPlayer(const RawAddress &rawAddr, uint16_t uidCounter,
    uint32_t numberOfItems, const std::vector<std::string> &folderNames, int result, int detail)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnSetBrowsedPlayer] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnSetBrowsedPlayer] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[OnSetBrowsedPlayer] fail: write result failed");
        return;
    }

    if (!data.WriteUint32(numberOfItems)) {
        HILOGE("[OnSetBrowsedPlayer] fail: write result failed");
        return;
    }    

    if (!WriteParcelableStringVector(folderNames, data)) {
        HILOGE("[OnSetBrowsedPlayer] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnSetBrowsedPlayer] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(detail)) {
        HILOGE("[OnSetBrowsedPlayer] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_SET_BROWSED_PLAYER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnSetBrowsedPlayer done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothAvrcpCtObserverProxy::OnGetCapabilities(const RawAddress &rawAddr, const std::vector<uint32_t> &companies,
    const std::vector<uint8_t> &events, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetCapabilities] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetCapabilities] fail: write result failed");
        return;
    }

    if (!WriteParcelableUint32Vector(companies, data)) {
        HILOGE("[OnGetCapabilities] fail: write result failed");
        return;
    }

    std::vector<int32_t> eventsTmp = {};
    for (auto event : events) {
        eventsTmp.push_back(event);
    }

    if (!WriteParcelableInt32Vector(eventsTmp, data)) {
        HILOGE("[OnGetCapabilities] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetCapabilities] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_CAPABILITIES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetCapabilities done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingAttributes(
    const RawAddress &rawAddr, std::vector<uint8_t> attributes, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetPlayerAppSettingAttributes] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetPlayerAppSettingAttributes] fail: write result failed");
        return;
    }

    std::vector<int32_t> attributesTmp = {};
    for (auto event : attributes) {
        attributesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(attributesTmp, data)) {
        HILOGE("[OnGetPlayerAppSettingAttributes] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetPlayerAppSettingAttributes] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingAttributes done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingValues(const RawAddress &rawAddr,
    int attribute, const std::vector<uint8_t> &values, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetPlayerAppSettingValues] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetPlayerAppSettingValues] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(attribute)) {
        HILOGE("[OnGetPlayerAppSettingValues] fail: write result failed");
        return;
    }

    std::vector<int32_t> valuesTmp {};
    for (auto event : values) {
        valuesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(valuesTmp, data)) {
        HILOGE("[OnGetPlayerAppSettingValues] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetPlayerAppSettingValues] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_VALUES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingValues done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingCurrentValue(
    const RawAddress &rawAddr, const std::vector<uint8_t> attributes, const std::vector<uint8_t> &values, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    std::vector<int32_t> attributesTmp {};
    for (auto event : attributes) {
        attributesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(attributesTmp, data)) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    std::vector<int32_t> valuesTmp {};
    for (auto event : values) {
        valuesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(valuesTmp, data)) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_CURRENT_VALUE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingCurrentValue done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnSetPlayerAppSettingCurrentValue(const RawAddress &rawAddr, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnSetPlayerAppSettingCurrentValue] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnSetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnSetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_SET_PLAYER_APP_SETTING_CURRENT_VALUE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnSetPlayerAppSettingCurrentValue done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingAttributeText(const RawAddress &rawAddr,
    const std::vector<uint8_t> attribtues, const std::vector<std::string> &attributeName, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    std::vector<int32_t> attributesTmp {};
    for (auto event : attribtues) {
        attributesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(attributesTmp, data)) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    if (!WriteParcelableStringVector(attributeName, data)) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetPlayerAppSettingCurrentValue] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTE_TEXT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingCurrentValue done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingValueText(const RawAddress &rawAddr,
    const std::vector<uint8_t> &values, const std::vector<std::string> &valueName, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetPlayerAppSettingValueText] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetPlayerAppSettingValueText] fail: write result failed");
        return;
    }

    std::vector<int32_t> valuesTmp{};
    for (auto event : values) {
        valuesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(valuesTmp, data)) {
        HILOGE("[OnGetPlayerAppSettingValueText] fail: write result failed");
        return;
    }

    if (!WriteParcelableStringVector(valueName, data)) {
        HILOGE("[OnGetPlayerAppSettingValueText] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetPlayerAppSettingValueText] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_VALUE_TEXT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetPlayerAppSettingValueText done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetElementAttributes(const RawAddress &rawAddr,
    const std::vector<uint32_t> &attribtues, const std::vector<std::string> &valueName, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetElementAttributes] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetElementAttributes] fail: write result failed");
        return;
    }

    if (!WriteParcelableUint32Vector(attribtues, data)) {
        HILOGE("[OnGetElementAttributes] fail: write result failed");
        return;
    }

    if (!WriteParcelableStringVector(valueName, data)) {
        HILOGE("[OnGetElementAttributes] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetElementAttributes] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_ELEMENT_ATTRIBUTRES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetElementAttributes done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetPlayStatus(const RawAddress &rawAddr,
    uint32_t songLength, uint32_t songPosition, uint8_t playStatus, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetPlayStatus] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetPlayStatus] fail: write result failed");
        return;
    }

    if (!data.WriteUint32(songLength)) {
        HILOGE("[OnGetPlayStatus] fail: write result failed");
        return;
    }

    if (!data.WriteUint32(songPosition)) {
        HILOGE("[OnGetPlayStatus] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(playStatus)) {
        HILOGE("[OnGetPlayStatus] fail: write result failed");
        return;
    }


    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetPlayStatus] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_PLAY_STATUS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetPlayStatus done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnPlayItem(const RawAddress &rawAddr, int status, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnPlayItem] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnPlayItem] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(status)) {
        HILOGE("[OnPlayItem] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnPlayItem] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_PLAY_ITEM, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnPlayItem done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetTotalNumberOfItems(const RawAddress &rawAddr,
    uint16_t uidCounter, uint32_t numOfItems, int result, int detail)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetTotalNumberOfItems] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetTotalNumberOfItems] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[OnGetTotalNumberOfItems] fail: write result failed");
        return;
    }

    if (!data.WriteUint32(numOfItems)) {
        HILOGE("[OnGetTotalNumberOfItems] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[OnGetTotalNumberOfItems] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(detail)) {
        HILOGE("[OnGetTotalNumberOfItems] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetTotalNumberOfItems done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetItemAttributes(const RawAddress &rawAddr,
    const std::vector<uint32_t> &attribtues, const std::vector<std::string> &values, int result, int detail)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetItemAttributes] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetItemAttributes] fail: write result failed");
        return;
    }

    if (!WriteParcelableUint32Vector(attribtues, data)) {
        HILOGE("[OnGetItemAttributes] fail: write result failed");
        return;
    }

    if (!WriteParcelableStringVector(values, data)) {
        HILOGE("[OnGetItemAttributes] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetItemAttributes] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(detail)) {
        HILOGE("[OnGetItemAttributes] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_ITEM_ATTRIBUTES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetItemAttributes done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnSetAbsoluteVolume(const RawAddress &rawAddr, uint8_t volume, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnSetAbsoluteVolume] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnSetAbsoluteVolume] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(volume)) {
        HILOGE("[OnSetAbsoluteVolume] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnSetAbsoluteVolume] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_SET_ABSOLUTE_VOLUME, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnSetAbsoluteVolume done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnPlaybackStatusChanged(const RawAddress &rawAddr, uint8_t playStatus, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnPlaybackStatusChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnPlaybackStatusChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(playStatus)) {
        HILOGE("[OnPlaybackStatusChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnPlaybackStatusChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_PLAYBACK_STATUS_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnPlaybackStatusChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnTrackChanged(const RawAddress &rawAddr, uint64_t uid, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnTrackChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnTrackChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt64(uid)) {
        HILOGE("[OnTrackChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnTrackChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_TRACK_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnTrackChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnTrackReachedEnd(const RawAddress &rawAddr, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnTrackReachedEnd] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnTrackReachedEnd] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnTrackReachedEnd] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_TRACK_REACHED_END, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnTrackReachedEnd done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnTrackReachedStart(const RawAddress &rawAddr, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnTrackReachedStart] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnTrackReachedStart] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnTrackReachedStart] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_TRACK_REACHED_START, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnTrackReachedStart done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnPlaybackPosChanged(const RawAddress &rawAddr, uint32_t playbackPos, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnPlaybackPosChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnPlaybackPosChanged] fail: write result failed");
        return;
    }

    if (!data.WriteUint32(playbackPos)) {
        HILOGE("[OnPlaybackPosChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnPlaybackPosChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_PLAYBACK_POS_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnPlaybackPosChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnPlayerAppSettingChanged(const RawAddress &rawAddr, const std::vector<uint8_t> &attributes,
    const std::vector<uint8_t> &values, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnPlayerAppSettingChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnPlayerAppSettingChanged] fail: write result failed");
        return;
    }

    std::vector<int32_t> attributesTmp {};
    for (auto event : attributes) {
        attributesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(attributesTmp, data)) {
        HILOGE("[OnPlayerAppSettingChanged] fail: write result failed");
        return;
    }

    std::vector<int32_t> valuesTmp {};
    for (auto event : values) {
        valuesTmp.push_back(event);
    }
    if (!WriteParcelableInt32Vector(valuesTmp, data)) {
        HILOGE("[OnPlayerAppSettingChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnPlayerAppSettingChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_PLAY_APP_SETTING_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnPlayerAppSettingChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnNowPlayingContentChanged(const RawAddress &rawAddr, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnNowPlayingContentChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnNowPlayingContentChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnNowPlayingContentChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_NOW_PLAYING_CONTENT_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnNowPlayingContentChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnAvailablePlayersChanged(const RawAddress &rawAddr, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnAvailablePlayersChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnAvailablePlayersChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnAvailablePlayersChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_AVAILABLE_PLAYER_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnAvailablePlayersChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnAddressedPlayerChanged(
    const RawAddress &rawAddr, uint16_t playerId, uint16_t uidCounter, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnAddressedPlayerChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnAddressedPlayerChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(playerId)) {
        HILOGE("[OnAddressedPlayerChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[OnAddressedPlayerChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnAddressedPlayerChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_ADDRESSED_PLAYER_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnAddressedPlayerChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnUidChanged(const RawAddress &rawAddr, uint16_t uidCounter, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnUidChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnUidChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[OnUidChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnUidChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_UID_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnUidChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnVolumeChanged(const RawAddress &rawAddr, uint8_t volume, int result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnVolumeChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnVolumeChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(volume)) {
        HILOGE("[OnVolumeChanged] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(result)) {
        HILOGE("[OnVolumeChanged] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_VOLUME_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnVolumeChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetMediaPlayers(const RawAddress &rawAddr, uint16_t uidCounter,
    std::vector<BluetoothAvrcpMpItem> &items, int result, int detail)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetMediaPlayers] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetMediaPlayers] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[OnGetMediaPlayers] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(items.size())) {
        HILOGE("[OnGetMediaPlayers] fail: write result failed");
        return;
    }
    for (uint32_t i = 0; i < items.size(); i++) {
        if (!data.WriteParcelable(&items[i])) {
            HILOGE("[OnGetMediaPlayers] fail: write AvrcMpItem result failed");
            return;
        }
    }    

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetMediaPlayers] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(detail)) {
        HILOGE("[OnGetMediaPlayers] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_MEDIA_PLAYERS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetMediaPlayers done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothAvrcpCtObserverProxy::OnGetFolderItems(const RawAddress &rawAddr, uint16_t uidCounter, std::vector<BluetoothAvrcpMeItem> &items,
    int result, int detail)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtObserverProxy::GetDescriptor())) {
        HILOGE("[OnGetFolderItems] fail: write interface token failed.");
        return;
    }

    if (!data.WriteString(rawAddr.GetAddress())) {
        HILOGE("[OnGetFolderItems] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[OnGetFolderItems] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(items.size())) {
        HILOGE("[OnGetFolderItems] fail: write result failed");
        return;
    }
    for (uint32_t i = 0; i < items.size(); i++) {
        if (!data.WriteParcelable(&items[i])) {
            HILOGE("[OnGetFolderItems] fail: write AvrcMeItem result failed");
            return;
        }
    }    

    if (!data.WriteInt32(result)) {
        HILOGE("[OnGetFolderItems] fail: write result failed");
        return;
    }

    if (!data.WriteInt32(detail)) {
        HILOGE("[OnGetFolderItems] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        BluetoothAvrcpCtObserverProxy::Code::AVRCP_CT_GET_FOLDER_ITEMS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtObserverProxy::OnGetFolderItems done fail, error: %{public}d", error);
        return;
    }
}

bool BluetoothAvrcpCtObserverProxy::WriteParcelableStringVector(
    const std::vector<std::string> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOGE("write WriteParcelableStringVector failed");
        return false;
    }

    for (auto parcelable : parcelableVector) {
        if (!reply.WriteString(parcelable)) {
            HILOGE("write WriteParcelableStringVector failed");
            return false;
        }
    }
    return true;
}

bool BluetoothAvrcpCtObserverProxy::WriteParcelableInt32Vector(
    const std::vector<std::int32_t> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOGE("write WriteParcelableInt32Vector failed");
        return false;
    }

    for (auto parcelable : parcelableVector) {
        if (!reply.WriteInt32(parcelable)) {
            HILOGE("write WriteParcelableInt32Vector failed");
            return false;
        }
    }
    return true;
}

bool BluetoothAvrcpCtObserverProxy::WriteParcelableUint32Vector(
    const std::vector<std::uint32_t> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOGE("write WriteParcelableUint32Vector failed");
        return false;
    }

    for (auto parcelable : parcelableVector) {
        if (!reply.WriteUint32(parcelable)) {
            HILOGE("write WriteParcelableUint32Vector failed");
            return false;
        }
    }
    return true;
}

ErrCode BluetoothAvrcpCtObserverProxy::InnerTransact(
    uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
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
