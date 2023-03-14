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

#include "bluetooth_a2dp_sink_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothA2dpSinkObserverProxy::OnConnectionStateChanged(const RawAddress &device, int state)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkObserverProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkObserverProxy::OnConnectionStateChanged write device error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothA2dpSinkObserverProxy::OnConnectionStateChanged state error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    int ret = Remote()->SendRequest(
        IBluetoothA2dpSinkObserver::Code::BT_A2DP_SINK_OBSERVER_CONNECTION_STATE_CHANGED, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::OnPlayingStateChanged done fail");
        return;
    }
}

}  // namespace Bluetooth
}  // namespace OHOS