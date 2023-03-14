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

#include "bluetooth_map_mse_proxy.h"
#include "bluetooth_log.h"
#include "bluetooth_errorcode.h"

namespace OHOS {
namespace Bluetooth {
void BluetoothMapMseProxy::RegisterObserver(
    const sptr<IBluetoothMapMseObserver> &observer)
{
    HILOGI("BluetoothMapMseProxy::RegisterObserver Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothMapMseProxy::RegisterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMseProxy::DeregisterObserver(
    const sptr<IBluetoothMapMseObserver> &observer)
{
    HILOGI("BluetoothMapMseProxy::DeregisterObserver Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::DeregisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothMapMseProxy::DeregisterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_DE_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::DeregisterObserver done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothMapMseProxy::GetState(
    int32_t &ret)
{
    HILOGI("BluetoothMapMseProxy::Connect Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::Connect WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_GET_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::Connect done fail, error: %{public}d", error);
        return;
    }
    ret = reply.ReadInt32();
}

void BluetoothMapMseProxy::Disconnect(
    const BluetoothRawAddress &device, int32_t &ret)
{
    HILOGI("BluetoothMapMseProxy::Disconnect Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::Disconnect WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMseProxy::Disconnect error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::Disconnect done fail, error: %{public}d", error);
        return;
    }
    ret = reply.ReadInt32();
}

void BluetoothMapMseProxy::IsConnected(
    const BluetoothRawAddress &device, bool &ret)
{
    HILOGI("BluetoothMapMseProxy::IsConnected Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::IsConnected WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMseProxy::IsConnected error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_ISCONNECTED, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::IsConnected done fail, error: %{public}d", error);
        return;
    }
    ret = reply.ReadBool();
}

void BluetoothMapMseProxy::GetConnectedDevices(
    std::vector<BluetoothRawAddress> &devices)
{
    HILOGI("BluetoothMapMseProxy::GetConnectDevices Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::GetConnectDevices WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_GET_CONNECTED_DEVICES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::GetConnectDevices done fail, error: %{public}d", error);
        return;
    }
    int DevNum = reply.ReadInt32();
    for (int i = DevNum; i > 0; i--) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return;
        }
        devices.push_back(*dev);
    }
}

void BluetoothMapMseProxy::GetDevicesByStates(
    const std::vector<int> &statusList, std::vector<BluetoothRawAddress> &devices)
{
    HILOGI("BluetoothMapMseProxy::GetDevicesByStates Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::GetDevicesByStates WriteInterfaceToken error");
        return;
    }
    if (!data.WriteInt32Vector(statusList)) {
        HILOGE("BluetoothMapMseProxy::GetDevicesByStates transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_GET_DEVICES_BY_STATES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return;
    }
    int DevNum = reply.ReadInt32();
    for (int i = DevNum; i > 0; i--) {
        std::shared_ptr<BluetoothRawAddress> dev(reply.ReadParcelable<BluetoothRawAddress>());
        if (!dev) {
            return;
        }
        devices.push_back(*dev);
    }
}

void BluetoothMapMseProxy::GetConnectionState(
    const BluetoothRawAddress &device, int32_t &ret)
{
    HILOGI("BluetoothMapMseProxy::GetConnectionState Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::GetConnectionState WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMseProxy::IsConnected error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_GET_CONNECTION_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::GetConnectionState done fail, error: %{public}d", error);
        return;
    }
    ret = reply.ReadInt32();
}

void BluetoothMapMseProxy::SetConnectionStrategy(
    const BluetoothRawAddress &device, int strategy, bool &ret)
{
    HILOGI("BluetoothMapMseProxy::SetConnectionStrategy Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::SetConnectionStrategy WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMseProxy::SetConnectionStrategy error");
        return;
    }
    if (!data.WriteInt32(strategy)) {
        HILOGE("BluetoothMapMseProxy::SetConnectionStrategy transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_SET_CONNECTION_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return;
    }
    ret = reply.ReadBool();
}

void BluetoothMapMseProxy::GetConnectionStrategy(
    const BluetoothRawAddress &device, int32_t &ret)
{
    HILOGI("BluetoothMapMseProxy::GetConnectionStrategy Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::GetConnectionStrategy WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMseProxy::GetConnectionStrategy error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_GET_CONNECTION_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::GetConnectionStrategy done fail, error: %{public}d", error);
        return;
    }
    ret = reply.ReadInt32();
}

void BluetoothMapMseProxy::GrantPermission(
    const BluetoothRawAddress &device, bool allow, bool save)
{
    HILOGI("BluetoothMapMseProxy::GrantPermission Triggered!");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothMapMseProxy::GetDescriptor())) {
        HILOGE("BluetoothMapMseProxy::GrantPermission WriteInterfaceToken error");
        return;
    }
    if (!data.WriteParcelable(&device)) {
        HILOGE("BluetoothMapMseProxy::GrantPermission error");
        return;
    }
    if (!data.WriteBool(allow)) {
        HILOGE("BluetoothMapMseProxy::GrantPermission transport error");
        return;
    }
    if (!data.WriteBool(save)) {
        HILOGE("BluetoothMapMseProxy::GrantPermission transport error");
        return;
    }
    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };
    int error = Remote()->SendRequest(
        IBluetoothMapMse::Code::MSE_GRANT_PERMISSION, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothMapMseProxy::GrantPermission done fail, error: %{public}d", error);
        return;
    }
}
}  // namespace Bluetooth
}  // namespace OHOS
