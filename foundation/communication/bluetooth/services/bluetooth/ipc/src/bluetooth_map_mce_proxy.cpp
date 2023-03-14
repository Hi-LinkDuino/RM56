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

#include "bluetooth_map_mce_proxy.h"
#include "bluetooth_log.h"
#include "bluetooth_errorcode.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothMapMceProxy::RegisterObserver(
    const sptr<IBluetoothMapMceObserver> &observer)
{
    HILOGI("BluetoothMapMceProxy::RegisterObserver Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPbapPceProxy::RegisterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMceProxy::DeregisterObserver(
    const sptr<IBluetoothMapMceObserver> &observer)
{
    HILOGI("BluetoothMapMceProxy::DeregisterObserver Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::DeregisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothPbapPceProxy::DeregisterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_DE_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::DeregisterObserver done fail, error: %{public}d", error);
        return;
    }
}

int BluetoothMapMceProxy::Connect(
    const BluetoothRawAddress &device)
{
    HILOGI("BluetoothMapMceProxy::Connect Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::Connect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::Connect error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_CONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::Connect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::Disconnect(
    const BluetoothRawAddress &device)
{
    HILOGI("BluetoothMapMceProxy::Disconnect Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::Disconnect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::Disconnect error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::Disconnect done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::IsConnected(
    const BluetoothRawAddress &device)
{
    HILOGI("BluetoothMapMceProxy::IsConnected Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::IsConnected WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::IsConnected error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_ISCONNECTED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::IsConnected done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

void BluetoothMapMceProxy::GetConnectDevices(
    std::vector<BluetoothRawAddress> &devices)
{
    HILOGI("BluetoothMapMceProxy::GetConnectDevices Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetConnectDevices WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_CONNECT_DEVICES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetConnectDevices done fail, error: %{public}d", error);
        return;
    }
    int devNum = reply.ReadInt32();
    for (int i = devNum; i > 0; i--) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return;
        }
        devices.push_back(*dev);
    }
}

void BluetoothMapMceProxy::GetDevicesByStates(
    const std::vector<int> &statusList, std::vector<BluetoothRawAddress> &devices)
{
    HILOGI("BluetoothMapMceProxy::GetDevicesByStates Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetDevicesByStates WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32Vector(statusList)) {
        HILOGE("BluetoothPbapPceProxy::GetDevicesByStates transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_DEVICES_BY_STATES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return;
    }
    int devNum = reply.ReadInt32();
    for (int i = devNum; i > 0; i--) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return;
        }
        devices.push_back(*dev);
    }
}

int BluetoothMapMceProxy::GetConnectionState(
    const BluetoothRawAddress &device)
{
    HILOGI("BluetoothMapMceProxy::GetConnectionState Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetConnectionState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::IsConnected error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_CONNECTION_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetConnectionState done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::SetConnectionStrategy(
    const BluetoothRawAddress &device, int strategy)
{
    HILOGI("BluetoothMapMceProxy::SetConnectionStrategy Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::SetConnectionStrategy WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::SetConnectionStrategy error");
        return ERROR;
    }
    if (!data.WriteInt32(strategy)) {
        HILOGE("BluetoothPbapPceProxy::SetConnectionStrategy transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_SET_CONNECTION_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::GetConnectionStrategy(
    const BluetoothRawAddress &device)
{
    HILOGI("BluetoothMapMceProxy::GetConnectionStrategy Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetConnectionStrategy WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetConnectionStrategy error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_CONNECTION_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetConnectionStrategy done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::GetUnreadMessages(
    const BluetoothRawAddress &device, int32_t msgType, int32_t max)
{
    HILOGI("BluetoothMapMceProxy::GetUnreadMessages Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetUnreadMessages WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetUnreadMessages error");
        return ERROR;
    }
    if (!data.WriteInt32(msgType)) {
        HILOGE("BluetoothPbapPceProxy::GetUnreadMessages transport error");
        return ERROR;
    }
    if (!data.WriteInt32(max)) {
        HILOGE("BluetoothPbapPceProxy::GetUnreadMessages transport error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_UNREAD_MESSAGES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetUnreadMessages done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::GetSupportedFeatures(
    const BluetoothRawAddress &device)
{
    HILOGI("BluetoothMapMceProxy::GetSupportedFeatures Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetSupportedFeatures WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetSupportedFeatures error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_SUPPORTED_FEATURES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetSupportedFeatures done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::SendMessage(
    const BluetoothRawAddress &device, const BluetoothIProfileSendMessageParameters &msg)
{
    HILOGI("BluetoothMapMceProxy::SendMessage Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::SendMessage WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::SendMessage error");
        return ERROR;
    }
    if (!data.WriteParcelable(&msg)) {
        HILOGE("BluetoothMapMceProxy::SendMessage error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_SEND_MESSAGE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::SendMessage done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::SetNotificationFilter(
    const BluetoothRawAddress &device, int32_t mask)
{
    HILOGI("BluetoothMapMceProxy::SetNotificationFilter Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::SetNotificationFilter WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::SetNotificationFilter error");
        return ERROR;
    }
    if (!data.WriteInt32(mask)) {
        HILOGE("BluetoothMapMceProxy::SetNotificationFilter error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_SET_NOTIFICATION_FILTER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::SetNotificationFilter done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::GetMessagesListing(
    const BluetoothRawAddress &device, const BluetoothIProfileGetMessagesListingParameters &para)
{
    HILOGI("BluetoothMapMceProxy::GetMessagesListing Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetMessagesListing WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetMessagesListing error");
        return ERROR;
    }
    if (!data.WriteParcelable(&para)) {
        HILOGE("BluetoothMapMceProxy::GetMessagesListing error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_MESSAGES_LISTING, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetMessagesListing done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::GetMessage(
    const BluetoothRawAddress &device, int32_t msgType,
    const std::u16string &msgHandle, const BluetoothIProfileGetMessageParameters &para)
{
    HILOGI("BluetoothMapMceProxy::GetMessage Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetMessage WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetMessage error");
        return ERROR;
    }
    if (!data.WriteInt32(msgType)) {
        HILOGE("BluetoothMapMceProxy::GetMessage error");
        return ERROR;
    }
    if (!data.WriteString16(msgHandle)) {
        HILOGE("BluetoothMapMceProxy::GetMessage error");
        return ERROR;
    }
    if (!data.WriteParcelable(&para)) {
        HILOGE("BluetoothMapMceProxy::GetMessage error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_MESSAGE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetMessage done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::UpdateInbox(
    const BluetoothRawAddress &device, int32_t msgType)
{
    HILOGI("BluetoothMapMceProxy::UpdateInbox Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::UpdateInbox WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::UpdateInbox error");
        return ERROR;
    }
    if (!data.WriteInt32(msgType)) {
        HILOGE("BluetoothMapMceProxy::UpdateInbox error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_UPDATE_INBOX, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::UpdateInbox done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::GetConversationListing(
    const BluetoothRawAddress &device, const BluetoothIProfileGetConversationListingParameters &para)
{
    HILOGI("BluetoothMapMceProxy::GetConversationListing Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetConversationListing WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetConversationListing error");
        return ERROR;
    }
    if (!data.WriteParcelable(&para)) {
        HILOGE("BluetoothMapMceProxy::GetConversationListing error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_CONVERSATION_LISTING, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetConversationListing done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::SetMessageStatus(
    const BluetoothRawAddress &device, int32_t msgType, const std::u16string &msgHandle,
    int32_t statusIndicator, int32_t statusValue, const std::string &extendedData)
{
    HILOGI("BluetoothMapMceProxy::SetMessageStatus Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus error");
        return ERROR;
    }
    if (!data.WriteInt32(msgType)) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus error");
        return ERROR;
    }
    if (!data.WriteString16(msgHandle)) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus error");
        return ERROR;
    }
    if (!data.WriteInt32(statusIndicator)) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus error");
        return ERROR;
    }
    if (!data.WriteInt32(statusValue)) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus error");
        return ERROR;
    }
    if (!data.WriteString(extendedData)) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_SET_MESSAGE_STATUS, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::SetMessageStatus done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::SetOwnerStatus(
    const BluetoothRawAddress &device, const BluetoothIProfileSetOwnerStatusParameters &para)
{
    HILOGI("BluetoothMapMceProxy::SetOwnerStatus Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::SetOwnerStatus WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::SetOwnerStatus error");
        return ERROR;
    }
    if (!data.WriteParcelable(&para)) {
        HILOGE("BluetoothMapMceProxy::SetOwnerStatus error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_SET_OWNER_STATUS, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::SetOwnerStatus done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

int BluetoothMapMceProxy::GetOwnerStatus(
    const BluetoothRawAddress &device, const std::string &conversationId)
{
    HILOGI("BluetoothMapMceProxy::GetOwnerStatus Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetOwnerStatus WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetOwnerStatus error");
        return ERROR;
    }
    if (!data.WriteString(conversationId)) {
        HILOGE("BluetoothMapMceProxy::GetOwnerStatus error");
        return ERROR;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_OWNER_STATUS, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetOwnerStatus done fail, error: %{public}d", error);
        return ERROR;
    }
    return reply.ReadInt32();
}

BluetoothIProfileMasInstanceInfoList BluetoothMapMceProxy::GetMasInstanceInfo(
    const BluetoothRawAddress &device)
{
    HILOGI("BluetoothMapMceProxy::GetMasInstanceInfo Triggered!");
    BluetoothIProfileMasInstanceInfoList list;
    list.isValid = false;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceProxy::GetMasInstanceInfo WriteInterfaceToken error");
        return list;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceProxy::GetMasInstanceInfo error");
        return list;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMce::Code::MCE_GET_MAS_INSTANCE_INFO, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceProxy::GetMasInstanceInfo done fail, error: %{public}d", error);
        return list;
    }
    std::shared_ptr<BluetoothIProfileMasInstanceInfoList> listPtr(
        reply.ReadParcelable<BluetoothIProfileMasInstanceInfoList>());
    if (!listPtr) {
        return list;
    }
    return *listPtr;
}
}  // namespace Bluetooth
}  // namespace OHOS
