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

#include "bluetooth_host_observer_proxy.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothHostObserverProxy::OnStateChanged(int32_t transport, int32_t status)
{
    MessageParcel data;

    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnStateChanged WriteInterfaceToken error");
        return;
    }

    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostObserverProxy::OnStateChanged transport error");
        return;
    }

    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothHostObserverProxy::OnStateChanged status error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_STATE_CHANGE, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnStateChanged done fail, error: %{public}d", result);
        return;
    }
}

// ON_DIS_STA_CHANGE_CODE
void BluetoothHostObserverProxy::OnDiscoveryStateChanged(int32_t status)
{
    MessageParcel data;

    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnDiscoveryStateChanged WriteInterfaceToken error");
        return;
    }

    if (!data.WriteInt32(status)) {
        HILOGE("BluetoothHostObserverProxy::OnDiscoveryStateChanged status error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_DISCOVERY_STATE_CHANGE, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnDiscoveryStateChanged done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothHostObserverProxy::OnDiscoveryResult(const BluetoothRawAddress &device)
{
    MessageParcel data;

    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnDiscoveryResult WriteInterfaceToken error");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHostObserverProxy::OnDiscoveryResult device error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_DISCOVERY_RESULT, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnDiscoveryResult done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothHostObserverProxy::OnPairRequested(const int32_t transport, const BluetoothRawAddress &device)
{
    MessageParcel data;

    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnPairRequested WriteInterfaceToken error");
        return;
    }

    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostObserverProxy::OnPairRequested transport error");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHostObserverProxy::OnPairRequested device error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_PAIR_REQUESTED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnPairRequested done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothHostObserverProxy::OnPairConfirmed(
    const int32_t transport, const BluetoothRawAddress &device, int reqType, int number)
{
    MessageParcel data;

    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnPairConfirmed WriteInterfaceToken error");
        return;
    }

    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostObserverProxy::OnPairConfirmed transport error");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothHostObserverProxy::OnPairConfirmed device error");
        return;
    }

    if (!data.WriteInt32(reqType)) {
        HILOGE("BluetoothHostObserverProxy::OnPairConfirmed transport error");
        return;
    }

    if (!data.WriteInt32(number)) {
        HILOGE("BluetoothHostObserverProxy::OnPairConfirmed transport error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_PAIR_CONFIRMED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnPairConfirmed done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothHostObserverProxy::OnScanModeChanged(int32_t mode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnScanModeChanged WriteInterfaceToken error");
        return;
    }

    if (!data.WriteInt32(mode)) {
        HILOGE("BluetoothHostObserverProxy::OnScanModeChanged status error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_SCAN_MODE_CHANGED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnScanModeChanged done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothHostObserverProxy::OnDeviceNameChanged(const std::string &deviceName)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnDeviceNameChanged WriteInterfaceToken error");
        return;
    }

    if (!data.WriteString(deviceName)) {
        HILOGE("BluetoothHostObserverProxy::OnDeviceNameChanged status error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_DEVICE_NAME_CHANGED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnDeviceNameChanged done fail, error: %{public}d", result);
        return;
    }
}

void BluetoothHostObserverProxy::OnDeviceAddrChanged(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostObserverProxy::GetDescriptor())) {
        HILOGE("BluetoothHostObserverProxy::OnDeviceAddrChanged WriteInterfaceToken error");
        return;
    }

    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostObserverProxy::OnDeviceAddrChanged status error");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};

    ErrCode result = InnerTransact(BT_HOST_OBSERVER_DEVICE_ADDR_CHANGED, option, data, reply);
    if (result != NO_ERROR) {
        HILOGE("BluetoothHostProxy::OnDeviceAddrChanged done fail, error: %{public}d", result);
        return;
    }
}

ErrCode BluetoothHostObserverProxy::InnerTransact(
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
