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

#include "bluetooth_map_mce_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothMapMceObserverProxy::OnMapActionCompleted(
    const BluetoothRawAddress &device, const BluetoothIProfileMapAction &action, const int status)
{
    HILOGI("BluetoothMapMceObserverProxy::OnMapActionCompleted Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceObserverProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapActionCompleted error");
        return;
    }
    if (!data.WriteParcelable(&action)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapActionCompleted error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapActionCompleted transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMceObserver::Code::MCE_ON_ACTION_COMPLETED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapActionCompleted done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMceObserverProxy::OnConnectionStateChanged(
    const BluetoothRawAddress &device, int status)
{
    HILOGI("BluetoothMapMceObserverProxy::OnConnectionStateChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceObserverProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceObserverProxy::OnConnectionStateChanged error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothMapMceObserverProxy::OnConnectionStateChanged transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMceObserver::Code::MCE_ON_CONNECTION_STATE_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceObserverProxy::OnConnectionStateChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMceObserverProxy::OnMapEventReported(
    const BluetoothRawAddress &device, const BluetoothIProfileMapEventReport &report)
{
    HILOGI("BluetoothMapMceObserverProxy::OnMapEventReported Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapEventReported WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapActionCompleted error");
        return;
    }
    if (!data.WriteParcelable(&report)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapActionCompleted error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMceObserver::Code::MCE_ON_EVENT_REPORTED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceObserverProxy::OnMapEventReported done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMceObserverProxy::OnBmessageCompleted(
    const BluetoothRawAddress &device, const BluetoothIProfileBMessage &bmsg, const int status)
{
    HILOGI("BluetoothMapMceObserverProxy::OnBmessageCompleted Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceObserverProxy::OnBmessageCompleted WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceObserverProxy::OnBmessageCompleted error");
        return;
    }
    if (!data.WriteParcelable(&bmsg)) {
        HILOGE("BluetoothMapMceObserverProxy::OnBmessageCompleted error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothMapMceObserverProxy::OnBmessageCompleted transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMceObserver::Code::MCE_ON_BMESSAGE_COMPLETED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceObserverProxy::OnBmessageCompleted done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMceObserverProxy::OnMessagesListingCompleted(
    const BluetoothRawAddress &device, const BluetoothIProfileMessagesListing &listing, const int status)
{
    HILOGI("BluetoothMapMceObserverProxy::OnMessagesListingCompleted Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceObserverProxy::OnMessagesListingCompleted WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMessagesListingCompleted error");
        return;
    }
    if (!data.WriteParcelable(&listing)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMessagesListingCompleted error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothMapMceObserverProxy::OnMessagesListingCompleted transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMceObserver::Code::MCE_ON_MESSAGE_LIST_COMPLETED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceObserverProxy::OnMessagesListingCompleted done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMceObserverProxy::OnConversationListingCompleted(
    const BluetoothRawAddress &device, const BluetoothIProfileConversationListing &listing, const int status)
{
    HILOGI("BluetoothMapMceObserverProxy::OnConversationListingCompleted Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMceObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMceObserverProxy::OnConversationListingCompleted WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMceObserverProxy::OnConversationListingCompleted error");
        return;
    }
    if (!data.WriteParcelable(&listing)) {
        HILOGE("BluetoothMapMceObserverProxy::OnConversationListingCompleted error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothMapMceObserverProxy::OnConversationListingCompleted transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMceObserver::Code::MCE_ON_CONVERSATION_LIST_COMPLETED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMceObserverProxy::OnConversationListingCompleted done fail, error: %{public}d", error);
        return;
    }
}
}  // namespace Bluetooth
}  // namespace OHOS
