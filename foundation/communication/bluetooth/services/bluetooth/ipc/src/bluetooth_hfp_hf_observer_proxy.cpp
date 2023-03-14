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

#include "bluetooth_hfp_hf_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothHfpHfObserverProxy::OnConnectionStateChanged(const BluetoothRawAddress &device, int state) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnConnectionStateChanged WriteParcelable error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnConnectionStateChanged WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_CONNECTION_STATE_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnConnectionStateChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnScoStateChanged(const BluetoothRawAddress &device,  int state) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnScoStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnScoStateChanged WriteParcelable error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnScoStateChanged WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_SCO_STATE_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnScoStateChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnCallChanged(const BluetoothRawAddress &device, const BluetoothHfpHfCall &call) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnCallChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnCallChanged WriteParcelable error");
        return;
    }
    if (!data.WriteParcelable(&call)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnCallChanged WriteParcelable error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_CALL_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnCallChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnSignalStrengthChanged(const BluetoothRawAddress &device,  int signal) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSignalStrengthChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSignalStrengthChanged WriteParcelable error");
        return;
    }
    if (!data.WriteInt32(signal)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSignalStrengthChanged WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_SIGNAL_STRENGTH_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSignalStrengthChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnRegistrationStatusChanged(const BluetoothRawAddress &device,  int status) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRegistrationStatusChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRegistrationStatusChanged WriteParcelable error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRegistrationStatusChanged WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_REGISTRATION_STATUS_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRegistrationStatusChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnRoamingStatusChanged(const BluetoothRawAddress &device,  int status) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRoamingStatusChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRoamingStatusChanged WriteParcelable error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRoamingStatusChanged WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_ROAMING_STATUS_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnRoamingStatusChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnOperatorSelectionChanged(const BluetoothRawAddress &device, 
    const std::string &name) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnOperatorSelectionChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnOperatorSelectionChanged WriteParcelable error");
        return;
    }
    if (!data.WriteString(name)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnOperatorSelectionChanged WriteString error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_OPERATOR_SELECTION_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnOperatorSelectionChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnSubscriberNumberChanged(const BluetoothRawAddress &device, 
    const std::string &number) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSubscriberNumberChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSubscriberNumberChanged WriteParcelable error");
        return;
    }
    if (!data.WriteString(number)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSubscriberNumberChanged WriteString error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_SUBSCRIBER_NUMBER_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnSubscriberNumberChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnVoiceRecognitionStatusChanged(const BluetoothRawAddress &device, int status) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnVoiceRecognitionStatusChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnVoiceRecognitionStatusChanged WriteParcelable error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnVoiceRecognitionStatusChanged WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_VOICE_RECOGNITION_STATUS_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnVoiceRecognitionStatusChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHfpHfObserverProxy::OnInBandRingToneChanged(const BluetoothRawAddress &device, int status) {
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHfpHfObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHfpHfObserverProxy::OnInBandRingToneChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnInBandRingToneChanged WriteParcelable error");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothHfpHfObserverProxy::OnInBandRingToneChanged WriteInt32 error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothHfpHfObserver::Code::BT_HFP_HF_OBSERVER_IN_BAND_RING_TONE_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHfpHfObserverProxy::OnInBandRingToneChanged done fail, error: %{public}d", error);
        return;
    }
}

}  // namespace Bluetooth
}  // namespace OHOS
