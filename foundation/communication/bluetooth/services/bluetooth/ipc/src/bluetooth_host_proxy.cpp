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

#include "bluetooth_host_proxy.h"
#include "bluetooth_log.h"
namespace OHOS {
namespace Bluetooth {
void BluetoothHostProxy::RegisterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    HILOGI("BluetoothHostProxy::RegisterObserver start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::RegisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::RegisterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_REGISTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::RegisterObserver done fail, error: %{public}d", error);
        return;
    }
    HILOGI("BluetoothHostProxy::RegisterObserver success");
}

void BluetoothHostProxy::DeregisterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::DeregisterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::DeregisterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_DEREGISTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::DeregisterObserver done fail, error: %{public}d", error);
        return;
    }
}

bool BluetoothHostProxy::EnableBt()
{
    MessageParcel data;
    HILOGI("BluetoothHostProxy::EnableBt starts");
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::EnableBt WriteInterfaceToken error");
        return false;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_ENABLE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::EnableBt done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::DisableBt()
{
    HILOGI("BluetoothHostProxy::DisableBt starts");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::DisableBt WriteInterfaceToken error");
        return false;
    }

    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_DISABLE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::DisableBt done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

sptr<IRemoteObject> BluetoothHostProxy::GetProfile(const std::string &name)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetProfile WriteInterfaceToken error");
        return nullptr;
    }
    if (!data.WriteString(name)) {
        HILOGE("BluetoothHostProxy::GetProfile name error");
        return nullptr;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GETPROFILE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetProfile done fail, error: %{public}d", error);
        return nullptr;
    }
    return reply.ReadStrongParcelable<IRemoteObject>();
}

sptr<IRemoteObject> BluetoothHostProxy::GetBleRemote(const std::string &name)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetProfile WriteInterfaceToken error");
        return nullptr;
    }
    if (!data.WriteString(name)) {
        HILOGE("BluetoothHostProxy::GetProfile name error");
        return nullptr;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_BLE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetProfile done fail, error: %{public}d", error);
        return nullptr;
    }
    return reply.ReadStrongParcelable<IRemoteObject>();
}

bool BluetoothHostProxy::BluetoothFactoryReset()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::BluetoothFactoryReset WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_FACTORY_RESET, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::BluetoothFactoryReset done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int32_t BluetoothHostProxy::GetBtState()
{
    HILOGI("BluetoothHostProxy::GetBtState starts");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetBtState WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GETSTATE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBtState done fail, error: %{public}d", error);
        return -1;
    }
    return reply.ReadInt32();
}

std::string BluetoothHostProxy::GetLocalAddress()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetLocalAddress WriteInterfaceToken error");
        return std::string();
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_LOCAL_ADDRESS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetLocalAddress done fail, error: %{public}d", error);
        return std::string();
    }
    return reply.ReadString();
}

bool BluetoothHostProxy::DisableBle()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::DisableBle WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_DISABLE_BLE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::DisableBle done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::EnableBle()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::EnableBle WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};

    int32_t error = InnerTransact(IBluetoothHost::Code::BT_ENABLE_BLE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::EnableBle done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::IsBleEnabled()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::IsBleEnabled WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_IS_BLE_ENABLED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::IsBleEnabled done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

std::vector<uint32_t> BluetoothHostProxy::GetProfileList()
{
    std::vector<uint32_t> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetProfileList WriteInterfaceToken error");
        return vec;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_PROFILE_LIST, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetProfileList done fail, error: %{public}d", error);
        return vec;
    }
    if (!reply.ReadUInt32Vector(&vec)) {
        HILOGE("BluetoothHostProxy::GetProfileList Read reply fail");
        return vec;
    }
    return vec;
}

int32_t BluetoothHostProxy::GetMaxNumConnectedAudioDevices()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetMaxNumConnectedAudioDevices WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_MAXNUM_CONNECTED_AUDIODEVICES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetMaxNumConnectedAudioDevices done fail, error: %{public}d", error);
        return -1;
    }
    int32_t maxNum;
    if (!reply.ReadInt32(maxNum)) {
        HILOGE("BluetoothHostProxy::GetMaxNumConnectedAudioDevices Read reply fail");
        return -1;
    }
    return maxNum;
}

int32_t BluetoothHostProxy::GetBtConnectionState()
{
    int32_t state;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetBtConnectionState WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_BT_STATE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBtConnectionState done fail, error: %{public}d", error);
        return -1;
    }
    if (!reply.ReadInt32(state)) {
        HILOGE("BluetoothHostProxy::GetBtConnectionState Read reply fail");
        return -1;
    }
    return state;
}

int32_t BluetoothHostProxy::GetBtProfileConnState(uint32_t profileId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetBtProfileConnState WriteInterfaceToken error");
        return -1;
    }
    if (!data.WriteUint32(profileId)) {
        HILOGE("BluetoothHostProxy::GetBtProfileConnState WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_BT_PROFILE_CONNSTATE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBtProfileConnState done fail, error: %{public}d", error);
        return -1;
    }

    int32_t state;
    if (!reply.ReadInt32(state)) {
        HILOGE("BluetoothHostProxy::GetBtProfileConnState Read reply fail");
        return -1;
    }
    return state;
}

int32_t BluetoothHostProxy::GetLocalDeviceClass()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetLocalDeviceClass WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_LOCAL_DEVICE_CLASS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetLocalDeviceClass done fail, error: %{public}d", error);
        return -1;
    }

    int32_t result;
    if (!reply.ReadInt32(result)) {
        HILOGE("BluetoothHostProxy::GetLocalDeviceClass Read reply fail");
        return -1;
    }
    return result;
}

bool BluetoothHostProxy::SetLocalDeviceClass(const int32_t &deviceClass)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetLocalDeviceClass WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteUint32(deviceClass)) {
        HILOGE("BluetoothHostProxy::SetLocalDeviceClass WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_SET_LOCAL_DEVICE_CLASS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetLocalDeviceClass done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

std::string BluetoothHostProxy::GetLocalName()
{
    std::string Name;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetLocalName WriteInterfaceToken error");
        return "";
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_LOCAL_NAME, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetLocalName done fail, error: %{public}d", error);
        return "";
    }
    if (!reply.ReadString(Name)) {
        HILOGE("BluetoothHostProxy::GetLocalName Read reply fail");
        return "";
    }
    return Name;
}

bool BluetoothHostProxy::SetLocalName(const std::string &name)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetLocalName WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteString(name)) {
        HILOGE("BluetoothHostProxy::SetLocalName WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_SET_LOCAL_NAME, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetLocalName done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int32_t BluetoothHostProxy::GetBtScanMode()
{
    int32_t Mode;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetBtScanMode WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_BT_SCAN_MODE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBtScanMode done fail, error: %{public}d", error);
        return -1;
    }
    if (!reply.ReadInt32(Mode)) {
        HILOGE("BluetoothHostProxy::GetBtScanMode Read reply fail");
        return -1;
    }
    return Mode;
}

bool BluetoothHostProxy::SetBtScanMode(int32_t mode, int32_t duration)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetBtScanMode WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(mode)) {
        HILOGE("BluetoothHostProxy::SetBtScanMode WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(duration)) {
        HILOGE("BluetoothHostProxy::SetBtScanMode WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_SET_BT_SCAN_MODE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetBtScanMode done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int32_t BluetoothHostProxy::GetBondableMode(const int32_t transport)
{
    int32_t Mode;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetBondableMode WriteInterfaceToken error");
        return -1;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::GetBondableMode WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_BONDABLE_MODE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBondableMode done fail, error: %{public}d", error);
        return -1;
    }
    if (!reply.ReadInt32(Mode)) {
        HILOGE("BluetoothHostProxy::GetBondableMode Read reply fail");
        return -1;
    }
    return Mode;
}

bool BluetoothHostProxy::SetBondableMode(int32_t transport, int32_t mode)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetBondableMode WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::SetBondableMode WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(mode)) {
        HILOGE("BluetoothHostProxy::SetBondableMode WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_SET_BONDABLE_MODE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetBondableMode done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::StartBtDiscovery()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::StartBtDiscovery WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_START_BT_DISCOVERY, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::StartBtDiscovery done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::CancelBtDiscovery()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::CancelBtDiscovery WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};

    int32_t error = InnerTransact(IBluetoothHost::Code::BT_CANCEL_BT_DISCOVERY, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::CancelBtDiscovery done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::IsBtDiscovering(const int32_t transport)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::IsBtDiscovering WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::IsBtDiscovering WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_IS_BT_DISCOVERING, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::Start done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

long BluetoothHostProxy::GetBtDiscoveryEndMillis()
{
    long millis;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetBtDiscoveryEndMillis WriteInterfaceToken error");
        return -1;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_BT_DISCOVERY_END_MILLIS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBtDiscoveryEndMillis done fail, error: %{public}d", error);
        return -1;
    }
    millis = static_cast<long>(reply.ReadInt64());
    return millis;
}

std::vector<BluetoothRawAddress> BluetoothHostProxy::GetPairedDevices(const int32_t transport)
{
    std::vector<BluetoothRawAddress> vec;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetPairedDevices WriteInterfaceToken error");
        return vec;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::GetPairedDevices WriteInterfaceToken error");
        return vec;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_PAIRED_DEVICES, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetPairedDevices done fail, error: %{public}d", error);
        return vec;
    }
    int32_t size = reply.ReadInt32();
    for (int32_t i = 0; i < size; i++) {
        std::shared_ptr<BluetoothRawAddress> rawAddress(reply.ReadParcelable<BluetoothRawAddress>());
        if (!rawAddress) {
            return vec;
        }
        vec.push_back(*rawAddress);
    }
    return vec;
}

bool BluetoothHostProxy::RemovePair(const int32_t transport, const sptr<BluetoothRawAddress> &device)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::RemovePair WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::RemovePair WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteStrongParcelable(device)) {
        HILOGE("BluetoothHostProxy::RemovePair WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_REMOVE_PAIR, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::RemovePair done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::RemoveAllPairs()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::RemoveAllPairs WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};

    int32_t error = InnerTransact(IBluetoothHost::Code::BT_REMOVE_ALL_PAIRS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::RemoveAllPairs done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

void BluetoothHostProxy::RegisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::RegisterRemoteDeviceObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::RegisterRemoteDeviceObserver WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_REGISTER_REMOTE_DEVICE_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBtConnectionState done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothHostProxy::DeregisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::DeregisterRemoteDeviceObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::DeregisterRemoteDeviceObserver WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_DEREGISTER_REMOTE_DEVICE_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::DeregisterRemoteDeviceObserver done fail, error: %{public}d", error);
        return;
    }
    return;
}

int32_t BluetoothHostProxy::GetBleMaxAdvertisingDataLength()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetBleMaxAdvertisingDataLength WriteInterfaceToken error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_GET_BLE_MAX_ADVERTISING_DATALENGTH, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetBleMaxAdvertisingDataLength done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadInt32();
}

int32_t BluetoothHostProxy::GetDeviceType(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetDeviceType WriteInterfaceToken error");
        return 0;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::GetDeviceType transport error");
        return 0;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetDeviceType address error");
        return 0;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_DEVICE_TYPE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetDeviceType done fail, error: %{public}d", error);
        return 0;
    }
    return reply.ReadInt32();
}

int32_t BluetoothHostProxy::GetPhonebookPermission(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetPhonebookPermission WriteInterfaceToken error");
        return 0;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetPhonebookPermission address error");
        return 0;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_PHONEBOOK_PERMISSION, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetPhonebookPermission done fail, error: %{public}d", error);
        return 0;
    }
    return reply.ReadInt32();
}

bool BluetoothHostProxy::SetPhonebookPermission(const std::string &address, int32_t permission)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetPhonebookPermission WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::SetPhonebookPermission address error");
        return false;
    }
    if (!data.WriteInt32(permission)) {
        HILOGE("BluetoothHostProxy::SetPhonebookPermission permission error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::SET_PHONEBOOK_PERMISSION, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetPhonebookPermission done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int32_t BluetoothHostProxy::GetMessagePermission(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetMessagePermission WriteInterfaceToken error");
        return 0;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetMessagePermission address error");
        return 0;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_MESSAGE_PERMISSION, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetMessagePermission done fail, error: %{public}d", error);
        return 0;
    }
    return reply.ReadInt32();
}

bool BluetoothHostProxy::SetMessagePermission(const std::string &address, int32_t permission)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetMessagePermission WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::SetMessagePermission address error");
        return false;
    }
    if (!data.WriteInt32(permission)) {
        HILOGE("BluetoothHostProxy::SetMessagePermission permission error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::SET_MESSAGE_PERMISSION, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetMessagePermission done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int32_t BluetoothHostProxy::GetPowerMode(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetPowerMode WriteInterfaceToken error");
        return 0;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetPowerMode address error");
        return 0;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_POWER_MODE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetPowerMode done fail, error: %{public}d", error);
        return 0;
    }
    return reply.ReadInt32();
}

std::string BluetoothHostProxy::GetDeviceName(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetDeviceName WriteInterfaceToken error");
        return std::string();
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::GetDeviceName transport error");
        return std::string();
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetDeviceName address error");
        return std::string();
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_DEVICE_NAME, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetDeviceName done fail, error: %{public}d", error);
        return std::string();
    }
    return reply.ReadString();
}

std::string BluetoothHostProxy::GetDeviceAlias(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetDeviceAlias WriteInterfaceToken error");
        return std::string();
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetDeviceAlias address error");
        return std::string();
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_DEVICE_ALIAS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetDeviceAlias done fail, error: %{public}d", error);
        return std::string();
    }
    return reply.ReadString();
}

bool BluetoothHostProxy::SetDeviceAlias(const std::string &address, const std::string &aliasName)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetDeviceAlias WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::SetDeviceAlias address error");
        return false;
    }
    if (!data.WriteString(aliasName)) {
        HILOGE("BluetoothHostProxy::SetDeviceAlias aliasName error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::SET_DEVICE_ALIAS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetDeviceAlias done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int32_t BluetoothHostProxy::GetDeviceBatteryLevel(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetDeviceBatteryLevel WriteInterfaceToken error");
        return 0;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetDeviceBatteryLevel address error");
        return 0;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_DEVICE_BATTERY_LEVEL, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetDeviceBatteryLevel done fail, error: %{public}d", error);
        return 0;
    }
    return reply.ReadInt32();
}

int32_t BluetoothHostProxy::GetPairState(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetPairState WriteInterfaceToken error");
        return 0;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::GetPairState transport error");
        return 0;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetPairState address error");
        return 0;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_PAIR_STATE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetPairState done fail, error: %{public}d", error);
        return 0;
    }
    return reply.ReadInt32();
}

bool BluetoothHostProxy::StartPair(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::StartPair WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::StartPair transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::StartPair address error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::START_PAIR, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::StartPair done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::CancelPairing(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::CancelPairing WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::CancelPairing transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::CancelPairing address error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::CANCEL_PAIRING, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::CancelPairing done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::IsBondedFromLocal(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::IsBondedFromLocal WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::IsBondedFromLocal transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::IsBondedFromLocal address error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::IS_BONDED_FROM_LOCAL, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::IsBondedFromLocal done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::IsAclConnected(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::IsAclConnected WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::IsAclConnected transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::IsAclConnected address error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::IS_ACL_CONNECTED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::IsAclConnected done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::IsAclEncrypted(int32_t transport, const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::IsAclEncrypted WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::IsAclEncrypted transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::IsAclEncrypted address error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::IS_ACL_ENCRYPTED, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::IsAclEncrypted done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

int32_t BluetoothHostProxy::GetDeviceClass(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetDeviceClass WriteInterfaceToken error");
        return 0;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetDeviceClass address error");
        return 0;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_DEVICE_CLASS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetDeviceClass done fail, error: %{public}d", error);
        return 0;
    }
    int32_t cod = 0;
    if (!reply.ReadInt32(cod)) {
        HILOGE("BluetoothHostProxy::GetDeviceClass get cod failed.");
    }
    return cod;
}

bool BluetoothHostProxy::SetDevicePin(const std::string &address, const std::string &pin)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetDevicePin WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::SetDevicePin address error");
        return false;
    }
    if (!data.WriteString(pin)) {
        HILOGE("BluetoothHostProxy::SetDevicePin pin error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::SET_DEVICE_PIN, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetDevicePin done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::SetDevicePairingConfirmation(int32_t transport, const std::string &address, bool accept)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetDevicePairingConfirmation WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::SetDevicePairingConfirmation transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::SetDevicePairingConfirmation address error");
        return false;
    }
    if (!data.WriteBool(accept)) {
        HILOGE("BluetoothHostProxy::SetDevicePairingConfirmation accept error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::SET_DEVICE_PAIRING_CONFIRMATION, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetDevicePairingConfirmation done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::SetDevicePasskey(int32_t transport, const std::string &address, int32_t passkey, bool accept)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::SetDevicePasskey WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::SetDevicePasskey transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::SetDevicePasskey address error");
        return false;
    }
    if (!data.WriteInt32(passkey)) {
        HILOGE("BluetoothHostProxy::SetDevicePasskey passkey error");
        return false;
    }
    if (!data.WriteBool(accept)) {
        HILOGE("BluetoothHostProxy::SetDevicePasskey accept error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::SET_DEVICE_PASSKEY, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::SetDevicePasskey done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::PairRequestReply(int32_t transport, const std::string &address, bool accept)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::PairRequestReply WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::PairRequestReply transport error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::PairRequestReply address error");
        return false;
    }
    if (!data.WriteBool(accept)) {
        HILOGE("BluetoothHostProxy::PairRequestReply accept error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::PAIR_REQUEST_PEPLY, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::PairRequestReply done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

bool BluetoothHostProxy::ReadRemoteRssiValue(const std::string &address)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::ReadRemoteRssiValue WriteInterfaceToken error");
        return false;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::ReadRemoteRssiValue address error");
        return false;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::READ_REMOTE_RSSI_VALUE, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::ReadRemoteRssiValue done fail, error: %{public}d", error);
        return false;
    }
    return reply.ReadBool();
}

void BluetoothHostProxy::GetLocalSupportedUuids(std::vector<std::string> &uuids)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetLocalSupportedUuids WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_LOCAL_SUPPORTED_UUIDS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetLocalSupportedUuids done fail, error: %{public}d", error);
        return;
    }
    int32_t size = reply.ReadInt32();
    std::string uuid;
    for (int32_t i = 0; i < size; i++) {
        uuid = reply.ReadString();
        uuids.push_back(uuid);
    }
}

std::vector<bluetooth::Uuid> BluetoothHostProxy::GetDeviceUuids(int32_t transport, const std::string &address)
{
    std::vector<bluetooth::Uuid> uuids;
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::GetDeviceUuids WriteInterfaceToken error");
        return uuids;
    }
    if (!data.WriteInt32(transport)) {
        HILOGE("BluetoothHostProxy::GetDeviceUuids Write transport error");
        return uuids;
    }
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetDeviceUuids Write address error");
        return uuids;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::GET_DEVICE_UUIDS, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::GetDeviceUuids done fail, error: %{public}d", error);
        return uuids;
    }

    int32_t size = reply.ReadInt32();
    bluetooth::Uuid uuid;
    for (int32_t i = 0; i < size; i++) {
        uuid = ParcelBtUuid::ReadFromParcel(reply);
        uuids.push_back(uuid);
    }
    return uuids;
}

void BluetoothHostProxy::RegisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    HILOGI("BluetoothHostProxy::RegisterBleAdapterObserver start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::RegisterBleAdapterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::RegisterBleAdapterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_REGISTER_BLE_ADAPTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::RegisterBleAdapterObserver done fail, error: %{public}d", error);
        return;
    }
    HILOGI("BluetoothHostProxy::RegisterBleAdapterObserver success");
}

void BluetoothHostProxy::DeregisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer)
{
    HILOGI("BluetoothHostProxy::DeregisterBleAdapterObserver start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::DeregisterBleAdapterObserver WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::DeregisterBleAdapterObserver error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_DEREGISTER_BLE_ADAPTER_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::DeregisterBleAdapterObserver done fail, error: %{public}d", error);
        return;
    }
}

void BluetoothHostProxy::RegisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer)
{
    HILOGI("BluetoothHostProxy::RegisterBlePeripheralCallback start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::RegisterBlePeripheralCallback WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::RegisterBlePeripheralCallback WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_REGISTER_BLE_PERIPHERAL_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::RegisterBlePeripheralCallback done fail, error: %{public}d", error);
        return;
    }
    return;
}

void BluetoothHostProxy::DeregisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer)
{
    HILOGI("BluetoothHostProxy::DeregisterBlePeripheralCallback start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(BluetoothHostProxy::GetDescriptor())) {
        HILOGE("BluetoothHostProxy::DeregisterBlePeripheralCallback WriteInterfaceToken error");
        return;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        HILOGE("BluetoothHostProxy::DeregisterBlePeripheralCallback WriteInterfaceToken error");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    int32_t error = InnerTransact(IBluetoothHost::Code::BT_DEREGISTER_BLE_PERIPHERAL_OBSERVER, option, data, reply);
    if (error != NO_ERROR) {
        HILOGE("BluetoothHostProxy::DeregisterBlePeripheralCallback done fail, error: %{public}d", error);
        return;
    }
    return;
}

ErrCode BluetoothHostProxy::InnerTransact(
    uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply)
{
    auto remote = Remote();
    if (remote == nullptr) {
        HILOGW("[InnerTransact] fail: get Remote fail code %{public}d", code);
        return OBJECT_NULL;
    }
    int32_t err = remote->SendRequest(code, data, reply, flags);
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
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
