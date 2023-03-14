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

#include "bluetooth_a2dp_sink_proxy.h"
#include "bluetooth_log.h"
#include "parcel_bt_uuid.h"
#include "bluetooth_errorcode.h"

namespace OHOS {
namespace Bluetooth {
int BluetoothA2dpSinkProxy::Connect(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::Connect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkProxy::Connect write device error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_CONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::Connect done fail, error: %{public}d", error);
        return ERROR;
    }

    return reply.ReadInt32();
}

int BluetoothA2dpSinkProxy::Disconnect(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::Disconnect WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkProxy::Disconnect write device error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_DISCONNECT, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::Disconnect done fail, error: %{public}d", error);
        return ERROR;
    }

    return reply.ReadInt32();
}

void BluetoothA2dpSinkProxy::RegisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothA2dpSinkProxy::RegisterObserver error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_REGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothA2dpSinkProxy::DeregisterObserver(const sptr<IBluetoothA2dpSinkObserver> &observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::DeregisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothA2dpSinkProxy::DeregisterObserver error");
        return;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_ASYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_DEREGISTER_OBSERVER, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::DeregisterObserver done fail, error: %{public}d", error);
        return;
    }
}

std::vector<RawAddress> BluetoothA2dpSinkProxy::GetDevicesByStates(const std::vector<int32_t> &states)
{
    MessageParcel data;
    std::vector<RawAddress> rawAdds = {};
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("[GetDevicesByStates] fail: write interface token failed.");
        return rawAdds;
    }

    if (!WriteParcelableInt32Vector(states, data)) {
        HILOGE("[GetDevicesByStates] fail: write result failed");
        return rawAdds;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_DEVICE_BY_STATES, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::GetDevicesByStates done fail, error: %{public}d", error);
        return rawAdds;
    }
    int32_t rawAddsSize = reply.ReadInt32();
    for (int i = 0; i < rawAddsSize; i++) {
        rawAdds.push_back(RawAddress(reply.ReadString()));
    }
    return rawAdds;
}

int BluetoothA2dpSinkProxy::GetDeviceState(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::GetDeviceState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkProxy::GetDeviceState write device error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_DEVICE_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::GetDeviceState done fail, error: %{public}d", error);
        return ERROR;
    }

    return reply.ReadInt32();
}

int BluetoothA2dpSinkProxy::GetPlayingState(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::GetPlayingState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkProxy::GetPlayingState write device error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_PLAYING_STATE, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::GetPlayingState done fail, error: %{public}d", error);
        return ERROR;
    }

    return reply.ReadInt32();
}

int BluetoothA2dpSinkProxy::SetConnectStrategy(const RawAddress &device, int32_t strategy)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::GetPlayingState WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkProxy::GetPlayingState write device error");
        return ERROR;
    }
    if (!data.WriteInt32(strategy)) {
        HILOGE("BluetoothA2dpSinkProxy::GetPlayingState write strategy error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_SET_CONNECT_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::GetPlayingState done fail, error: %{public}d", error);
        return ERROR;
    }

    return reply.ReadInt32();
}

int BluetoothA2dpSinkProxy::GetConnectStrategy(const RawAddress &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::GetConnectStrategy WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkProxy::GetConnectStrategy write device error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_CONNECT_STRATEGY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::GetConnectStrategy done fail, error: %{public}d", error);
        return ERROR;
    }

    return reply.ReadInt32();
}

int BluetoothA2dpSinkProxy::SendDelay(const RawAddress &device, int32_t delayValue)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothA2dpSinkProxy::GetDescriptor())) {
        HILOGE("BluetoothA2dpSinkProxy::SendDelay WriteInterfaceToken error");
        return ERROR;
    }
    if (!data.WriteString(device.GetAddress())) {
        HILOGE("BluetoothA2dpSinkProxy::SendDelay write device error");
        return ERROR;
    }
    if (!data.WriteInt32(delayValue)) {
        HILOGE("BluetoothA2dpSinkProxy::SendDelay write delayValue error");
        return ERROR;
    }

    MessageParcel reply;
    MessageOption option {
        MessageOption::TF_SYNC
    };

    int error = Remote()->SendRequest(IBluetoothA2dpSink::Code::BT_A2DP_SINK_SEND_DELAY, data, reply, option);
    if (error != NO_ERROR) {
        HILOGE("BluetoothA2dpSinkProxy::SendDelay done fail, error: %{public}d", error);
        return ERROR;
    }

    return reply.ReadInt32();
}

bool BluetoothA2dpSinkProxy::WriteParcelableInt32Vector(
    const std::vector<int32_t> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOGE("write ParcelableVector failed");
        return false;
    }

    for (auto parcelable : parcelableVector) {
        if (!reply.WriteInt32(parcelable)) {
            HILOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

}  // namespace Bluetooth
}  // namespace OHOS