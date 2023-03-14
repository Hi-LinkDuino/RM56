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

#include "bluetooth_map_mce_observer_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothMapMceObserverStub::BluetoothMapMceObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothMapMceObserver::Code::MCE_ON_ACTION_COMPLETED)] =
        &BluetoothMapMceObserverStub::OnMapActionCompletedInner;
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothMapMceObserver::Code::MCE_ON_CONNECTION_STATE_CHANGED)] =
        &BluetoothMapMceObserverStub::OnConnectionStateChangedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMceObserver::Code::MCE_ON_EVENT_REPORTED)] =
        &BluetoothMapMceObserverStub::OnMapEventReportedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMceObserver::Code::MCE_ON_BMESSAGE_COMPLETED)] =
        &BluetoothMapMceObserverStub::OnBmessageCompletedInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothMapMceObserver::Code::MCE_ON_MESSAGE_LIST_COMPLETED)] =
        &BluetoothMapMceObserverStub::OnMessagesListingCompletedInner;
    memberFuncMap_[static_cast<uint32_t>(
        IBluetoothMapMceObserver::Code::MCE_ON_CONVERSATION_LIST_COMPLETED)] =
        &BluetoothMapMceObserverStub::OnConversationListingCompletedInner;
}

BluetoothMapMceObserverStub::~BluetoothMapMceObserverStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothMapMceObserverStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothMapMceObserverStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code,
        option.GetFlags());
    std::u16string descriptor = BluetoothMapMceObserverStub::GetDescriptor();
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
    HILOGW("BluetoothMapMceObserverStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothMapMceObserverStub::OnMapActionCompletedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceObserverStub::OnMapActionCompletedInner Triggered!");
        std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileMapAction> action(data.ReadParcelable<BluetoothIProfileMapAction>());
    if (!action) {
        return TRANSACTION_ERR;
    }
    int32_t status = data.ReadInt32();
    OnMapActionCompleted(*addr, *action, status);
    return NO_ERROR;
}

ErrCode BluetoothMapMceObserverStub::OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceObserverStub::OnConnectionStateChangedInner Triggered!");
        std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    int32_t status = data.ReadInt32();
    OnConnectionStateChanged(*addr, status);
    return NO_ERROR;
}

ErrCode BluetoothMapMceObserverStub::OnMapEventReportedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceObserverStub::OnMapEventReportedInner Triggered!");
        std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileMapEventReport> event(data.ReadParcelable<BluetoothIProfileMapEventReport>());
    if (!event) {
        return TRANSACTION_ERR;
    }
    OnMapEventReported(*addr, *event);
    return NO_ERROR;
}

ErrCode BluetoothMapMceObserverStub::OnBmessageCompletedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceObserverStub::OnBmessageCompletedInner Triggered!");
        std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileBMessage> action(data.ReadParcelable<BluetoothIProfileBMessage>());
    int32_t status = data.ReadInt32();
    OnBmessageCompleted(*addr, *action, status);
    return NO_ERROR;
}

ErrCode BluetoothMapMceObserverStub::OnMessagesListingCompletedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceObserverStub::OnMessagesListingCompletedInner Triggered!");
        std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileMessagesListing> action(data.ReadParcelable<BluetoothIProfileMessagesListing>());
    int32_t status = data.ReadInt32();
    OnMessagesListingCompleted(*addr, *action, status);
    return NO_ERROR;
}

ErrCode BluetoothMapMceObserverStub::OnConversationListingCompletedInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothMapMceObserverStub::OnConversationListingCompletedInner Triggered!");
        std::shared_ptr<BluetoothRawAddress> addr(data.ReadParcelable<BluetoothRawAddress>());
    if (!addr) {
        return TRANSACTION_ERR;
    }
    std::shared_ptr<BluetoothIProfileConversationListing> action(
        data.ReadParcelable<BluetoothIProfileConversationListing>());
    int32_t status = data.ReadInt32();
    OnConversationListingCompleted(*addr, *action, status);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
