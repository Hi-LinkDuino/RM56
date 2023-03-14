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

#include "bluetooth_a2dp_src_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothA2dpSrcObserverProxy::OnConnectionStateChanged(const RawAddress &device, int state)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSrcObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConnectionStateChanged write device error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConnectionStateChanged state error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    ErrCode result = InnerTransact(BT_A2DP_SRC_OBSERVER_CONNECTION_STATE_CHANGED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConnectionStateChanged done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothA2dpSrcObserverProxy::OnPlayingStatusChanged(const RawAddress &device, int playingState, int error)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSrcObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnPlayingStatusChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnPlayingStatusChanged write device error");
        return;
    }
    if (!data.WriteInt32(playingState)) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnPlayingStatusChanged playingState error");
        return;
    }

    if (!data.WriteInt32(error)) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnPlayingStatusChanged error error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    ErrCode result = InnerTransact(BT_A2DP_SRC_OBSERVER_PLAYING_STATUS_CHANGED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnPlayingStatusChanged done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothA2dpSrcObserverProxy::OnConfigurationChanged(const RawAddress &device, const BluetoothA2dpCodecInfo &info, int error)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSrcObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConfigurationChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConfigurationChanged write device error");
        return;
    }
    if (!data.WriteParcelable(&info)) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConfigurationChanged transport error");
        return;
    }
    if (!data.WriteInt32(error)) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConfigurationChanged error error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    ErrCode result = InnerTransact(BT_A2DP_SRC_OBSERVER_CONFIGURATION_CHANGED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothA2dpSrcObserverProxy::OnConfigurationChanged done fail, error: %{public}d", result);
        return;
    }
}

ErrCode BluetoothA2dpSrcObserverProxy::InnerTransact(
    uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOGE("[InnerTransact] fail: get Remote fail code %{public}d", code);
        return ERR_DEAD_OBJECT;
    }
    int err = remote->SendRequest(code, data, reply, flags);
    switch (err) {
        case NO_ERROR: {
            return ERR_OK;
        }
        case DEAD_OBJECT: {
            HILOGE("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return ERR_DEAD_OBJECT;
        }
        default: {
            HILOGE("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return TRANSACTION_ERR;
        }
    }
}
}  // namespace Bluetooth
}  // namespace OHOS