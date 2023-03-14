/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "bluetooth_hid_host_stub.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothHidHostStub::BluetoothHidHostStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(COMMAND_CONNECT)] =
        &BluetoothHidHostStub::ConnectInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_DISCONNECT)] =
        &BluetoothHidHostStub::DisconnectInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_DEVICE_STATE)] =
        &BluetoothHidHostStub::GetDeviceStateInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_DEVICES_BY_STATES)] =
        &BluetoothHidHostStub::GetDevicesByStatesInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_REGISTER_OBSERVER)] =
        &BluetoothHidHostStub::RegisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_DEREGISTER_OBSERVER)] =
        &BluetoothHidHostStub::DeregisterObserverInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_VCUN_PLUG)] =
        &BluetoothHidHostStub::HidHostVCUnplugInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_SEND_DATA)] =
        &BluetoothHidHostStub::HidHostSendDataInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_SET_REPORT)] =
        &BluetoothHidHostStub::HidHostSetReportInner;
    memberFuncMap_[static_cast<uint32_t>(COMMAND_GET_REPORT)] =
        &BluetoothHidHostStub::HidHostGetReportInner;
    HILOGD("%{public}s ends.", __func__);
}

BluetoothHidHostStub::~BluetoothHidHostStub()
{
    HILOGD("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int BluetoothHidHostStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothHidHostStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothHidHostStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGE("local descriptor is not equal to remote");
        return IPC_INVOKER_TRANSLATE_ERR;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGW("BluetoothHfpHfStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothHidHostStub::ConnectInner(MessageParcel &data, MessageParcel &reply)
{
    const BluetoothRawAddress *device = data.ReadParcelable<BluetoothRawAddress>();
    bool result;
    HILOGD("BluetoothHidHostStub::ConnectInner");
    ErrCode ec = Connect(*device, result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        if (result == NO_ERROR) {
            reply.WriteInt32(NO_ERROR);
        }
    }
    return ERR_NONE;
}

ErrCode BluetoothHidHostStub::DisconnectInner(MessageParcel &data, MessageParcel &reply)
{
    const BluetoothRawAddress *device = data.ReadParcelable<BluetoothRawAddress>();
    bool result;
    HILOGD("BluetoothHidHostStub::DisconnectInner");
    ErrCode ec = Disconnect(*device, result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        if (result == NO_ERROR) {
            reply.WriteInt32(NO_ERROR);
        }
    }
    return ERR_NONE;
}

ErrCode BluetoothHidHostStub::GetDeviceStateInner(MessageParcel &data, MessageParcel &reply)
{
    const BluetoothRawAddress *device = data.ReadParcelable<BluetoothRawAddress>();
    int result;
    HILOGD("BluetoothHidHostStub::GetDeviceStateInner");
    ErrCode ec = GetDeviceState(*device, result);
    reply.WriteInt32(ec);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return ERR_NONE;
}

ErrCode BluetoothHidHostStub::GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<int32_t> states = {};
    int32_t stateSize = data.ReadInt32();
    HILOGD("BluetoothHidHostStub::GetDevicesByStatesInner");
    for (int i = 0; i < stateSize; i++) {
        int32_t state = data.ReadInt32();
        states.push_back(state);
    }
    std::vector<BluetoothRawAddress> rawAdds;
    ErrCode ec = GetDevicesByStates(states, rawAdds);
    if (ec != ERR_OK) {
        return ec;
    }
    reply.WriteInt32(rawAdds.size());
    for (auto rawAdd : rawAdds) {
        if (!reply.WriteParcelable(&rawAdd)) {
            return ERR_INVALID_STATE;
        }
    }
    return ERR_NONE;
}

ErrCode BluetoothHidHostStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothHidHostStub::RegisterObserverInner");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothHidHostObserver> observer = OHOS::iface_cast<IBluetoothHidHostObserver>(remote);
    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHidHostStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothHidHostStub::DeregisterObserverInner");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothHidHostObserver> observer = OHOS::iface_cast<IBluetoothHidHostObserver>(remote);
    DeregisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHidHostStub::HidHostVCUnplugInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothHidHostStub::HidHostVCUnplugInner");
    std::string device = data.ReadString();
    uint8_t id = data.ReadUint8();
    uint16_t size = data.ReadUint16();
    uint8_t type = data.ReadUint8();
    int result;
    ErrCode ec = HidHostVCUnplug(device, id, size, type, result);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return NO_ERROR;
}

ErrCode BluetoothHidHostStub::HidHostSendDataInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothHidHostStub::HidHostSendDataInner");
    std::string device = data.ReadString();
    uint8_t id = data.ReadUint8();
    uint16_t size = data.ReadUint16();
    uint8_t type = data.ReadUint8();
    int result;
    ErrCode ec = HidHostSendData(device, id, size, type, result);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return NO_ERROR;
}

ErrCode BluetoothHidHostStub::HidHostSetReportInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothHidHostStub::HidHostSetReportInner");
    std::string device = data.ReadString();
    uint8_t type = data.ReadUint8();
    uint16_t size = data.ReadUint16();
    uint8_t report = data.ReadUint8();
    int result;
    ErrCode ec = HidHostSetReport(device, type, size, report, result);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return NO_ERROR;
}

ErrCode BluetoothHidHostStub::HidHostGetReportInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("BluetoothHidHostStub::HidHostGetReportInner");
    std::string device = data.ReadString();
    uint8_t id = data.ReadUint8();
    uint16_t size = data.ReadUint16();
    uint8_t type = data.ReadUint8();
    int result;
    ErrCode ec = HidHostGetReport(device, id, size, type, result);
    if (SUCCEEDED(ec)) {
        reply.WriteInt32(result);
    }
    return NO_ERROR;
}
} // Bluetooth
} // OHOS
