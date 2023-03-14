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

#include "bluetooth_map_mce_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothMapMceStub::BluetoothMapMceStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_REGISTER_OBSERVER)] =
        &BluetoothMapMceStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_DE_REGISTER_OBSERVER)] =
        &BluetoothMapMceStub::DeregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_CONNECT)] =
        &BluetoothMapMceStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_DISCONNECT)] =
        &BluetoothMapMceStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_ISCONNECTED)] =
        &BluetoothMapMceStub::IsConnectedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_CONNECT_DEVICES)] =
        &BluetoothMapMceStub::GetConnectDevicesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_DEVICES_BY_STATES)] =
        &BluetoothMapMceStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_CONNECTION_STATE)] =
        &BluetoothMapMceStub::GetConnectionStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_SET_CONNECTION_STRATEGY)] =
        &BluetoothMapMceStub::SetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_CONNECTION_STRATEGY)] =
        &BluetoothMapMceStub::GetConnectionStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_UNREAD_MESSAGES)] =
        &BluetoothMapMceStub::GetUnreadMessagesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_SUPPORTED_FEATURES)] =
        &BluetoothMapMceStub::GetSupportedFeaturesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_SEND_MESSAGE)] =
        &BluetoothMapMceStub::SendMessageInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_SET_NOTIFICATION_FILTER)] =
        &BluetoothMapMceStub::SetNotificationFilterInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_MESSAGES_LISTING)] =
        &BluetoothMapMceStub::GetMessagesListingInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_MESSAGE)] =
        &BluetoothMapMceStub::GetMessageInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_UPDATE_INBOX)] =
        &BluetoothMapMceStub::UpdateInboxInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_CONVERSATION_LISTING)] =
        &BluetoothMapMceStub::GetConversationListingInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_SET_MESSAGE_STATUS)] =
        &BluetoothMapMceStub::SetMessageStatusInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_SET_OWNER_STATUS)] =
        &BluetoothMapMceStub::SetOwnerStatusInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_OWNER_STATUS)] =
        &BluetoothMapMceStub::GetOwnerStatusInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMce::Code::MCE_GET_MAS_INSTANCE_INFO)] =
        &BluetoothMapMceStub::GetMasInstanceInfoInner;
}

BluetoothMapMceStub::~BluetoothMapMceStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothMapMceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothMapMceStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
    std::u16string descriptor = BluetoothMapMceStub::GetDescriptor();
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
    HILOGW("BluetoothMapMceStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothMapMceStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::RegisterObserverInner Triggered!");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothMapMceObserver> callback = OHOS::iface_cast<IBluetoothMapMceObserver>(remote);
    RegisterObserver(callback);
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::DeregisterObserverInner Triggered!");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothMapMceObserver> callback = OHOS::iface_cast<IBluetoothMapMceObserver>(remote);
    DeregisterObserver(callback);
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::ConnectInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Connect(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::DisconnectInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = Disconnect(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::IsConnectedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::IsConnectedInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = IsConnected(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetConnectDevicesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetConnectDevicesInner Triggered!");
    std::vector<BluetoothRawAddress> devices;
    GetConnectDevices(devices);
    reply.WriteInt32(devices.size());
    int num = devices.size();
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&devices[i]);
        if (!ret) {
            HILOGE("WriteParcelable<GetConnectDevicesInner> failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetDevicesByStates Triggered!");
    std::vector<int32_t> states;
    if (!data.ReadInt32Vector(&states)) {
        HILOGW("BluetoothMapMceStub::GetDevicesByStatesInner: get tmpState failed.");
        return INVALID_DATA;
    }
    std::vector<BluetoothRawAddress> rawDevices;
    GetDevicesByStates(states, rawDevices);
    reply.WriteInt32(rawDevices.size());
    int num = rawDevices.size();
    for (int i = 0; i < num; i++) {
        bool ret = reply.WriteParcelable(&rawDevices[i]);
        if (!ret) {
            HILOGE("WriteParcelable<GetDevicesByStatesInner> failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetConnectionStateInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetConnectionStateInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetConnectionState(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::SetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::SetConnectionStrategyInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t strategy = data.ReadInt32();
    int result = SetConnectionStrategy(*device, strategy);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetConnectionStrategyInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetConnectionStrategy(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetUnreadMessagesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetUnreadMessagesInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t msgType = data.ReadInt32();
    int32_t max = data.ReadInt32();
    int result = GetUnreadMessages(*device, msgType, max);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetSupportedFeaturesInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetSupportedFeaturesInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int result = GetSupportedFeatures(*device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::SendMessageInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::SendMessageInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileSendMessageParameters> msg(
        data.ReadParcelable<BluetoothIProfileSendMessageParameters>());
    int result = SendMessage(*device, *msg);

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::SetNotificationFilterInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::SetNotificationFilterInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t mask = data.ReadInt32();
    int result = SetNotificationFilter(*device, mask);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetMessagesListingInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetMessagesListingInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileGetMessagesListingParameters> msg(
        data.ReadParcelable<BluetoothIProfileGetMessagesListingParameters>());
    int result = GetMessagesListing(*device, *msg);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetMessageInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetMessagesListingInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t msgType = data.ReadInt32();
    const std::u16string msgHandle = data.ReadString16();
    std::shared_ptr<BluetoothIProfileGetMessageParameters> msg(
        data.ReadParcelable<BluetoothIProfileGetMessageParameters>());
    int result = GetMessage(*device, msgType, msgHandle, *msg);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::UpdateInboxInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::UpdateInboxInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t msgType = data.ReadInt32();
    int result = UpdateInbox(*device, msgType);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetConversationListingInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetConversationListingInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileGetConversationListingParameters> msg(
        data.ReadParcelable<BluetoothIProfileGetConversationListingParameters>());
    int result = GetConversationListing(*device, *msg);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::SetMessageStatusInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::SetMessageStatusInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    int32_t msgType = data.ReadInt32();
    const std::u16string msgHandle = data.ReadString16();
    int32_t statusIndicator = data.ReadInt32();
    int32_t statusValue = data.ReadInt32();
    const std::string extendedData = data.ReadString();
    int result = SetMessageStatus(*device, msgType, msgHandle, statusIndicator, statusValue, extendedData);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::SetOwnerStatusInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::SetOwnerStatusInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileSetOwnerStatusParameters> msg (
        data.ReadParcelable<BluetoothIProfileSetOwnerStatusParameters>());
    int result = SetOwnerStatus(*device, *msg);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetOwnerStatusInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::SetOwnerStatusInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    const std::string conversationId = data.ReadString();
    int result = GetOwnerStatus(*device, conversationId);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothMapMceStub::GetMasInstanceInfoInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceStub::GetMasInstanceInfoInner Triggered!");
    std::shared_ptr<BluetoothRawAddress> device(data.ReadParcelable<BluetoothRawAddress>());
    if (!device) {
        return TRANSACTION_ERR;
    }
    BluetoothIProfileMasInstanceInfoList result = GetMasInstanceInfo(*device);
    bool ret = reply.WriteParcelable(&result);
    if (!ret) {
        HILOGE("BluetoothMapMceStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
