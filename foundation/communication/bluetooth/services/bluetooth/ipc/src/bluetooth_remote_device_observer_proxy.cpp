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

#include "bluetooth_remote_device_observer_proxy.h"
#include "bluetooth_bt_uuid.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothRemoteDeviceObserverproxy::BluetoothRemoteDeviceObserverproxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IBluetoothRemoteDeviceObserver>(impl)
{}
BluetoothRemoteDeviceObserverproxy::~BluetoothRemoteDeviceObserverproxy()
{}

void BluetoothRemoteDeviceObserverproxy::OnPairStatusChanged(
    const int32_t transport, const BluetoothRawAddress &device, int32_t status)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothRemoteDeviceObserverproxy::GetDescriptor())) {
        HILOGE("[OnPairStatusChanged] fail: write interface token failed.");
        return;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("[OnPairStatusChanged] fail: write transport failed.");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("[OnPairStatusChanged] fail: write device failed");
        return;
    }
    if (!data.WriteInt32(status)) {
        HILOGE("[OnPairStatusChanged] fail: write status failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t error =
        InnerTransact(IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_PSIR_STATUS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothRemoteDeviceObserverproxy::OnPairStatusChanged done fail, error: %{public}d", error);
        return;
    }
}
void BluetoothRemoteDeviceObserverproxy::OnRemoteUuidChanged(
    const BluetoothRawAddress &device, const std::vector<bluetooth::Uuid> uuids)
{
    if (uuids.empty()) {
        HILOGE("[OnRemoteUuidChanged] fail: uuids is empty.");
        return;
    }

    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothRemoteDeviceObserverproxy::GetDescriptor())) {
        HILOGE("[OnRemoteUuidChanged] fail: write interface token failed.");
        return;
    }

    if (!data.WriteParcelable(&device)) {
        HILOGE("[OnRemoteUuidChanged] fail: write device failed");
        return;
    }

    size_t uuidSize = uuids.size();
    if (uuidSize > UINT16_MAX) {
        HILOGE("[OnRemoteUuidChanged], uuidSize = %{public}zu exceeds the maximum number.", uuidSize);
        return;
    }
    if (!data.WriteUint16(uuidSize)) {
        HILOGE("[OnRemoteUuidChanged] fail: write uuids size failed");
        return;
    }
    for (auto uuid : uuids) {
        BluetoothUuid btUuid = BluetoothUuid(uuid);
        if (!data.WriteParcelable(&btUuid)) {
            HILOGE("[OnRemoteUuidChanged] faild: write uuid error");
            return;
        }
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t error =
        InnerTransact(IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_UUID, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothRemoteDeviceObserverproxy::OnRemoteUuidChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothRemoteDeviceObserverproxy::OnRemoteNameChanged(
    const BluetoothRawAddress &device, const std::string deviceName)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothRemoteDeviceObserverproxy::GetDescriptor())) {
        HILOGE("[OnRemoteNameChanged] fail: write interface token failed.");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("[OnRemoteNameChanged] fail: write device failed");
        return;
    }
    if (!data.WriteString(deviceName)) {
        HILOGE("[OnRemoteNameChanged] fail: write deviceName failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t error =
        InnerTransact(IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_NAME, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothRemoteDeviceObserverproxy::OnRemoteNameChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothRemoteDeviceObserverproxy::OnRemoteAliasChanged(
    const BluetoothRawAddress &device, const std::string alias)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothRemoteDeviceObserverproxy::GetDescriptor())) {
        HILOGE("[OnRemoteAliasChanged] fail: write interface token failed.");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("[OnRemoteAliasChanged] fail: write device failed");
        return;
    }
    if (!data.WriteString(alias)) {
        HILOGE("[OnRemoteAliasChanged] fail: write alias failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t error = InnerTransact(
        IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_ALIAS_CHANGED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothRemoteDeviceObserverproxy::OnRemoteAliasChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothRemoteDeviceObserverproxy::OnRemoteCodChanged(const BluetoothRawAddress &device, int32_t cod)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothRemoteDeviceObserverproxy::GetDescriptor())) {
        HILOGE("[OnRemoteCodChanged] fail: write interface token failed.");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("[OnRemoteCodChanged] fail: write device failed");
        return;
    }
    if (!data.WriteInt32(cod)) {
        HILOGE("[OnRemoteCodChanged] fail: write cod failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t error =
        InnerTransact(IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_COD, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothRemoteDeviceObserverproxy::OnRemoteCodChanged done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothRemoteDeviceObserverproxy::OnRemoteBatteryLevelChanged(
    const BluetoothRawAddress &device, int32_t batteryLevel)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothRemoteDeviceObserverproxy::GetDescriptor())) {
        HILOGE("[OnRemoteBatteryLevelChanged] fail: write interface token failed.");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("[OnRemoteBatteryLevelChanged] fail: write device failed");
        return;
    }
    if (!data.WriteInt32(batteryLevel)) {
        HILOGE("[OnRemoteBatteryLevelChanged] fail: write status failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t error = InnerTransact(
        IBluetoothRemoteDeviceObserver::Code::BT_REMOTE_DEVICE_OBSERVER_REMOTE_BATTERY_LEVEL, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothRemoteDeviceObserverproxy::OnRemoteBatteryLevelChanged done fail, error: %{public}d", error);
        return;
    }
}

ErrCode BluetoothRemoteDeviceObserverproxy::InnerTransact(
    uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOGW("[InnerTransact] fail: get Remote fail code %{public}d", code);
        return OBJECT_NULL;
    }
    int err = remote->SendRequest(code, data, reply, flags);
    switch (err) {
        case NO_ERROR: {
            return NO_ERROR;
        }
        case DEAD_OBJECT: {
            HILOGW("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return DEAD_OBJECT;
        }
        default: {
            HILOGW("[InnerTransact] fail: ipcErr=%{public}d code %{public}d", err, code);
            return TRANSACTION_ERR;
        }
    }
}
}  // namespace Bluetooth
}  // namespace OHOS