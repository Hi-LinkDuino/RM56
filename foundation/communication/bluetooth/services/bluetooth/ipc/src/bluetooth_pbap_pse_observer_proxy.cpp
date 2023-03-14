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

#include "bluetooth_pbap_pse_observer_proxy.h"
#include "bluetooth_log.h"
#include "i_bluetooth_pbap_pse_observer.h"
#include "bluetooth_errorcode.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged(const BluetoothRawAddress &device, int state)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged WriteInterfaceToken error");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged error");
        return;
    }

    if (!data.WriteInt32(state)) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPseObserver::Code::PBAP_PSE_ON_SERVICE_CONNECTION_STATE_CHANGE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged done fail, error: %d", error);
        return;
    }
    return;
}

void BluetoothPbapPseObserverProxy::OnServicePermission(const BluetoothRawAddress &device) 
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged WriteInterfaceToken error");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPseObserver::Code::PBAP_PSE_ON_SERVICE_PERMISSION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServiceConnectionStateChanged done fail, error: %d", error);
        return;
    }
    return;
}

void BluetoothPbapPseObserverProxy::OnServicePasswordRequired(
        const BluetoothRawAddress &device, const ::std::vector<uint8_t> &description, int8_t charset, bool fullAccess)
{
    HILOGD("[%{public}s]: %{public}s(): Enter!", __FILE__, __FUNCTION__);
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothPbapPseObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServicePasswordRequired WriteInterfaceToken error");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServicePasswordRequired error");
        return;
    }

    if (!data.WriteUInt8Vector(description)) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServicePasswordRequired error");
        return;
    }

    if (!data.WriteInt8(charset)) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServicePasswordRequired error");
        return;
    }

    if (!data.WriteBool(fullAccess)) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServicePasswordRequired error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothPbapPseObserver::Code::PBAP_PSE_ON_SERVICE_PASSWORD_REQUIRED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothPbapPseObserverProxy::OnServicePasswordRequired done fail, error: %d", error);
        return;
    }
    return;
}
}  // namespace Bluetooth
}  // namespace OHOS