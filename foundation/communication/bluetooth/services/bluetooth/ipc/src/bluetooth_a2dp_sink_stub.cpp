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

#include "bluetooth_a2dp_sink_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "parcel_bt_uuid.h"
#include "raw_address.h"

namespace OHOS {
namespace Bluetooth {
using namespace OHOS::bluetooth;
BluetoothA2dpSinkStub::BluetoothA2dpSinkStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_CONNECT)] =
        &BluetoothA2dpSinkStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_DISCONNECT)] =
        &BluetoothA2dpSinkStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_REGISTER_OBSERVER)] =
        &BluetoothA2dpSinkStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_DEREGISTER_OBSERVER)] =
        &BluetoothA2dpSinkStub::DeregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_DEVICE_BY_STATES)] =
        &BluetoothA2dpSinkStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_DEVICE_STATE)] =
        &BluetoothA2dpSinkStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_PLAYING_STATE)] =
        &BluetoothA2dpSinkStub::GetPlayingStateInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_SET_CONNECT_STRATEGY)] =
        &BluetoothA2dpSinkStub::SetConnectStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_GET_CONNECT_STRATEGY)] =
        &BluetoothA2dpSinkStub::GetConnectStrategyInner;
    memberFuncMap_[static_cast<uint32_t>(IBluetoothA2dpSink::Code::BT_A2DP_SINK_SEND_DELAY)] =
        &BluetoothA2dpSinkStub::SendDelayInner;
}

BluetoothA2dpSinkStub::~BluetoothA2dpSinkStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothA2dpSinkStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothA2dpSinkStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothA2dpSinkStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGI("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGW("BluetoothA2dpSinkStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothA2dpSinkStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = Connect(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSinkStub: ConnectInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = Disconnect(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSinkStub: DisconnectInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothA2dpSinkObserver> observer = OHOS::iface_cast<IBluetoothA2dpSinkObserver>(remote);
    RegisterObserver(observer);

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothA2dpSinkObserver> observer = OHOS::iface_cast<IBluetoothA2dpSinkObserver>(remote);
    DeregisterObserver(observer);

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int32_t> states = {};
    int32_t stateSize = data.ReadInt32();

    for (int i = 0; i < stateSize; i++) {
        int32_t state = data.ReadInt32();
        states.push_back(state);
    }

    std::vector<RawAddress> rawAdds = GetDevicesByStates(states);
    reply.WriteInt32(rawAdds.size());

    for (auto rawAdd : rawAdds) {
        if (!reply.WriteString(rawAdd.GetAddress())) {
            HILOGE("GetConnectedDevicesInner: write WriteString failed");
            return ERR_INVALID_STATE;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = GetDeviceState(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSinkStub: GetDeviceStateInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::GetPlayingStateInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();

    int result = GetPlayingState(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSinkStub: GetPlayingStateInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::SetConnectStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int strategy = data.ReadInt32();

    int result = SetConnectStrategy(RawAddress(addr), strategy);

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSinkStub: SetConnectStrategyInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::GetConnectStrategyInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int result = GetConnectStrategy(RawAddress(addr));

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSinkStub: GetConnectStrategyInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothA2dpSinkStub::SendDelayInner(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    int delayValue = data.ReadInt32();

    int result = SendDelay(RawAddress(addr), delayValue);

    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothA2dpSinkStub: SendDelayInner reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS