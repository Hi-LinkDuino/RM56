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

#include "bluetooth_avrcp_tg_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothAvrcpTgObserverProxy::OnConnectionStateChanged(const BluetoothRawAddress &device, int32_t state)
{
    HILOGI("BluetoothAvrcpTgObserverProxy::OnConnectionStateChanged Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothAvrcpTgObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothAvrcpTgObserverProxy::OnConnectionStateChanged WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothAvrcpTgObserverProxy::OnConnectionStateChanged device error");
        return;
    }
    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothAvrcpTgObserverProxy::OnConnectionStateChanged transport error");
        return;
    }

    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    int error = Remote()->SendRequest(
        IBluetoothAvrcpTgObserver::Code::BT_AVRCP_TG_OBSERVER_CONNECTION_STATE_CHANGED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothAvrcpTgObserverProxy::OnConnectionStateChanged done fail, error: %{public}d", error);
        return;
    }
}
}  // namespace Bluetooth
}  // namespace OHOS