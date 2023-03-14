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

#include "bluetooth_avrcp_ct_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothAvrcpCtProxy::RegisterObserver(const sptr<IBluetoothAvrcpCtObserver> &observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[RegisterObserver] fail: write interface token failed.");
        return;
    }

    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("[RegisterObserver] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_REGISTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothAvrcpCtProxy::UnregisterObserver(const sptr<IBluetoothAvrcpCtObserver> &observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[UnregisterObserver] fail: write interface token failed.");
        return;
    }

    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("[UnregisterObserver] fail: write result failed");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_UNREGISTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::UnregisterObserver done fail, error: %{public}d", error);
        return;
    }
}

std::vector<RawAddress> BluetoothAvrcpCtProxy::GetConnectedDevices()
{
    MessageParcel data;
    std::vector<RawAddress> rawAdds = {};
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetConnectedDevices] fail: write interface token failed.");
        return rawAdds;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_CONNECTED_DEVICES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetConnectedDevices done fail, error: %{public}d", error);
        return rawAdds;
    }
    int32_t rawAddsSize = reply.ReadInt32();
    for (int i = 0; i < rawAddsSize; i++) {
        rawAdds.push_back(RawAddress(reply.ReadString()));
    }
    return rawAdds;
}

std::vector<RawAddress> BluetoothAvrcpCtProxy::GetDevicesByStates(const std::vector<int32_t> &states)
{
    MessageParcel data;
    std::vector<RawAddress> rawAdds = {};
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetDevicesByStates] fail: write interface token failed.");
        return rawAdds;
    }

    if (!WriteParcelableInt32Vector(states, data)) {
        HILOGE("[GetDevicesByStates] fail: write result failed");
        return rawAdds;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_DEVICES_BY_STATES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return rawAdds;
    }
    int32_t rawAddsSize = reply.ReadInt32();
    for (int i = 0; i < rawAddsSize; i++) {
        rawAdds.push_back(RawAddress(reply.ReadString()));
    }
    return rawAdds;
}

int32_t BluetoothAvrcpCtProxy::GetDeviceState(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetDeviceState] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetDeviceState] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_DEVICE_STATE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetDeviceState done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::Connect(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[Connect] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[Connect] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_CONNECT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::Connect done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::Disconnect(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[Disconnect] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[Disconnect] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_DISCONNECT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::Disconnect done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::PressButton(const RawAddress &device, int32_t button)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[PressButton] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[PressButton] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(button)) {
        HILOGE("[PressButton] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_PRESS_BUTTON, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::PressButton done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::ReleaseButton(const RawAddress &device, int32_t button)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[ReleaseButton] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[ReleaseButton] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(button)) {
        HILOGE("[ReleaseButton] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_RELEASE_BUTTON, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::ReleaseButton done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetUnitInfo(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetUnitInfo] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetUnitInfo] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_UNIT_INFO, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetUnitInfo done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetSubUnitInfo(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetSubUnitInfo] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetSubUnitInfo] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_SUB_UNIT_INFO, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetSubUnitInfo done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetSupportedCompanies(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetSupportedCompanies] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetSupportedCompanies] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_SUPPORTED_COMPANIES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetSupportedCompanies done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetSupportedEvents(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetSupportedEvents] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetSupportedEvents] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_SUPPORTED_EVENTS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetSupportedEvents done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetPlayerAppSettingAttributes(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetPlayerAppSettingAttributes] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetPlayerAppSettingAttributes] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetPlayerAppSettingAttributes done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetPlayerAppSettingValues(
    const RawAddress &device, int32_t attribute)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetPlayerAppSettingValues] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetPlayerAppSettingValues] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(attribute)) {
        HILOGE("[GetPlayerAppSettingValues] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_VALUES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetPlayerAppSettingValues done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetPlayerAppSettingCurrentValue(const RawAddress &device,
    const std::vector<int32_t> &attributes)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetPlayerAppSettingCurrentValue] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetPlayerAppSettingCurrentValue] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(attributes, data)) {
        HILOGE("[GetPlayerAppSettingCurrentValue] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_CURRENT_VALUE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetPlayerAppSettingCurrentValue done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::SetPlayerAppSettingCurrentValue(const RawAddress &device,
    const std::vector<int32_t> &attributes, const std::vector<int32_t> &values)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[SetPlayerAppSettingCurrentValue] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[SetPlayerAppSettingCurrentValue] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(attributes, data)) {
        HILOGE("[SetPlayerAppSettingCurrentValue] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(values, data)) {
        HILOGE("[SetPlayerAppSettingCurrentValue] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_SET_PLAYER_APP_SETTING_CURRENT_VALUE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::SetPlayerAppSettingCurrentValue done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetPlayerAppSettingAttributeText(const RawAddress &device,
    const std::vector<int32_t> &attributes)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetPlayerAppSettingAttributeText] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetPlayerAppSettingAttributeText] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(attributes, data)) {
        HILOGE("[GetPlayerAppSettingAttributeText] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_ATTRIBUTE_TEXT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetPlayerAppSettingAttributeText done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetPlayerAppSettingValueText(const RawAddress &device, int32_t attributes,
    const std::vector<int32_t> &values)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetPlayerAppSettingValueText] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetPlayerAppSettingValueText] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(attributes)) {
        HILOGE("[GetPlayerAppSettingValueText] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(values, data)) {
        HILOGE("[GetPlayerAppSettingValueText] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_APP_SETTING_VALUES_TEXT, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetPlayerAppSettingValueText done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetElementAttributes(const RawAddress &device,
    const std::vector<int32_t> &attributes)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetElementAttributes] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetElementAttributes] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(attributes, data)) {
        HILOGE("[GetElementAttributes] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_ELEMENT_ATTRIBUTES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetElementAttributes done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetPlayStatus(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetPlayStatus] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetPlayStatus] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_PLAYER_STATUS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetPlayStatus done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::PlayItem(const RawAddress &device, int32_t scope, int64_t uid,
    int32_t uidCounter)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[PlayItem] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[PlayItem] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(scope)) {
        HILOGE("[PlayItem] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt64(uid)) {
        HILOGE("[PlayItem] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[PlayItem] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_PLAY_ITEM, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::PlayItem done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetFolderItems(const RawAddress &device, int32_t startItem, int32_t endItem,
    const std::vector<int32_t> &attributes)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetFolderItems] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetFolderItems] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(startItem)) {
        HILOGE("[GetFolderItems] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(endItem)) {
        HILOGE("[GetFolderItems] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(attributes, data)) {
        HILOGE("[GetFolderItems] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_FOLDER_ITEMS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetFolderItems done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetTotalNumberOfItems(const RawAddress &device, int32_t scope)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetTotalNumberOfItems] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetTotalNumberOfItems] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(scope)) {
        HILOGE("[GetTotalNumberOfItems] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_TOTAL_NUMBER_OF_ITEMS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetTotalNumberOfItems done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::SetAbsoluteVolume(const RawAddress &device, int32_t volume)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[SetAbsoluteVolume] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[SetAbsoluteVolume] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(volume)) {
        HILOGE("[SetAbsoluteVolume] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_SET_ABSOLUTE_VOLUME, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::SetAbsoluteVolume done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::EnableNotification(const RawAddress &device,
    const std::vector<int32_t> &events, int32_t interval)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[EnableNotification] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[EnableNotification] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(events, data)) {
        HILOGE("[EnableNotification] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(interval)) {
        HILOGE("[EnableNotification] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_ENABLE_NOTIFICATION, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::EnableNotification done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::DisableNotification(const RawAddress &device,
    const std::vector<int32_t> &events)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[DisableNotification] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[DisableNotification] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(events, data)) {
        HILOGE("[DisableNotification] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_DISABLE_NOTIFICATION, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::DisableNotification done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetItemAttributes(const RawAddress &device, int64_t uid, int32_t uidCounter,
    const std::vector<int32_t> &attributes)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[DisableNotification] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[DisableNotification] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt64(uid)) {
        HILOGE("[DisableNotification] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(uidCounter)) {
        HILOGE("[DisableNotification] fail: write result failed");
        return -1;
    }

    if (!WriteParcelableInt32Vector(attributes, data)) {
        HILOGE("[DisableNotification] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_GET_ITEM_ATTRIBUTES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::DisableNotification done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::SetBrowsedPlayer(const RawAddress &device, int32_t playerId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[SetBrowsedPlayer] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[SetBrowsedPlayer] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(playerId)) {
        HILOGE("[SetBrowsedPlayer] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_SET_BROWSERED_PLAYER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::SetBrowsedPlayer done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

int32_t BluetoothAvrcpCtProxy::GetMeidaPlayerList(
    const RawAddress &device, int32_t startItem, int32_t endItem)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpCtProxy::GetDescriptor())) {
        HILOGE("[GetMeidaPlayerList] fail: write interface token failed.");
        return -1;
    }

    if (!data.WriteString(device.GetAddress())) {
        HILOGE("[GetMeidaPlayerList] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(startItem)) {
        HILOGE("[GetMeidaPlayerList] fail: write result failed");
        return -1;
    }

    if (!data.WriteInt32(endItem)) {
        HILOGE("[GetMeidaPlayerList] fail: write result failed");
        return -1;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = InnerTransact(
        IBluetoothAvrcpCt::Code::AVRCP_CT_MEDIA_PLAYER_LIST, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpCtProxy::GetMeidaPlayerList done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

bool BluetoothAvrcpCtProxy::WriteParcelableInt32Vector(
    const std::vector<int32_t> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOGE("write ParcelableVector failed");
        return false;
    }

    for (auto parcelable : parcelableVector) {
        if (!reply.WriteInt32(parcelable)) {
            HILOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

ErrCode BluetoothAvrcpCtProxy::InnerTransact(
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
