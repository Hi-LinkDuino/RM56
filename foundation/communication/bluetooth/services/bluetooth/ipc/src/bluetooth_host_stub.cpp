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

#include "bluetooth_host_stub.h"
#include "bluetooth_log.h"
#include "ipc_types.h"
#include "raw_address.h"
#include "string_ex.h"

namespace OHOS {
namespace Bluetooth {
const std::map<uint32_t, std::function<ErrCode(BluetoothHostStub *, MessageParcel &, MessageParcel &)>>
    BluetoothHostStub::memberFuncMap_ = {
        {BluetoothHostStub::BT_REGISTER_OBSERVER,
            std::bind(&BluetoothHostStub::RegisterObserverInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_DEREGISTER_OBSERVER,
            std::bind(&BluetoothHostStub::DeregisterObserverInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_ENABLE,
            std::bind(&BluetoothHostStub::EnableBtInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_DISABLE,
            std::bind(&BluetoothHostStub::DisableBtInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GETPROFILE,
            std::bind(&BluetoothHostStub::GetProfileInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_BLE,
            std::bind(&BluetoothHostStub::GetBleRemoteInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_FACTORY_RESET,
            std::bind(&BluetoothHostStub::BluetoothFactoryResetInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GETSTATE,
            std::bind(&BluetoothHostStub::GetBtStateInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_LOCAL_ADDRESS,
            std::bind(&BluetoothHostStub::GetLocalAddressInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_DISABLE_BLE,
            std::bind(&BluetoothHostStub::DisableBleInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_ENABLE_BLE,
            std::bind(&BluetoothHostStub::EnableBleInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_IS_BLE_ENABLED,
            std::bind(&BluetoothHostStub::IsBleEnabledInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_PROFILE_LIST,
            std::bind(&BluetoothHostStub::GetProfileListInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_MAXNUM_CONNECTED_AUDIODEVICES,
            std::bind(&BluetoothHostStub::GetMaxNumConnectedAudioDevicesInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_BT_STATE,
            std::bind(&BluetoothHostStub::GetBtConnectionStateInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_BT_PROFILE_CONNSTATE,
            std::bind(&BluetoothHostStub::GetBtProfileConnStateInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_LOCAL_DEVICE_CLASS,
            std::bind(&BluetoothHostStub::GetLocalDeviceClassInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_SET_LOCAL_DEVICE_CLASS,
            std::bind(&BluetoothHostStub::SetLocalDeviceClassInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_LOCAL_NAME,
            std::bind(&BluetoothHostStub::GetLocalNameInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_SET_LOCAL_NAME,
            std::bind(&BluetoothHostStub::SetLocalNameInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_BT_SCAN_MODE,
            std::bind(&BluetoothHostStub::GetBtScanModeInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_SET_BT_SCAN_MODE,
            std::bind(&BluetoothHostStub::SetBtScanModeInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_BONDABLE_MODE,
            std::bind(&BluetoothHostStub::GetBondableModeInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_SET_BONDABLE_MODE,
            std::bind(&BluetoothHostStub::SetBondableModeInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_START_BT_DISCOVERY,
            std::bind(&BluetoothHostStub::StartBtDiscoveryInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_CANCEL_BT_DISCOVERY,
            std::bind(&BluetoothHostStub::CancelBtDiscoveryInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_IS_BT_DISCOVERING,
            std::bind(&BluetoothHostStub::IsBtDiscoveringInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_BT_DISCOVERY_END_MILLIS,
            std::bind(&BluetoothHostStub::GetBtDiscoveryEndMillisInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_PAIRED_DEVICES,
            std::bind(&BluetoothHostStub::GetPairedDevicesInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_REMOVE_PAIR,
            std::bind(&BluetoothHostStub::RemovePairInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_REMOVE_ALL_PAIRS,
            std::bind(&BluetoothHostStub::RemoveAllPairsInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_REGISTER_REMOTE_DEVICE_OBSERVER,
            std::bind(&BluetoothHostStub::RegisterRemoteDeviceObserverInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {BluetoothHostStub::BT_DEREGISTER_REMOTE_DEVICE_OBSERVER,
            std::bind(&BluetoothHostStub::DeregisterRemoteDeviceObserverInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {BluetoothHostStub::BT_GET_BLE_MAX_ADVERTISING_DATALENGTH,
            std::bind(&BluetoothHostStub::GetBleMaxAdvertisingDataLengthInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {BluetoothHostStub::GET_DEVICE_TYPE,
            std::bind(&BluetoothHostStub::GetDeviceTypeInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_PHONEBOOK_PERMISSION,
            std::bind(&BluetoothHostStub::GetPhonebookPermissionInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::SET_PHONEBOOK_PERMISSION,
            std::bind(&BluetoothHostStub::SetPhonebookPermissionInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_MESSAGE_PERMISSION,
            std::bind(&BluetoothHostStub::GetMessagePermissionInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::SET_MESSAGE_PERMISSION,
            std::bind(&BluetoothHostStub::SetMessagePermissionInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_POWER_MODE,
            std::bind(&BluetoothHostStub::GetPowerModeInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_DEVICE_NAME,
            std::bind(&BluetoothHostStub::GetDeviceNameInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_DEVICE_ALIAS,
            std::bind(&BluetoothHostStub::GetDeviceAliasInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::SET_DEVICE_ALIAS,
            std::bind(&BluetoothHostStub::SetDeviceAliasInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_DEVICE_BATTERY_LEVEL,
            std::bind(&BluetoothHostStub::GetDeviceBatteryLevelInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_PAIR_STATE,
            std::bind(&BluetoothHostStub::GetPairStateInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::START_PAIR,
            std::bind(&BluetoothHostStub::StartPairInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::CANCEL_PAIRING,
            std::bind(&BluetoothHostStub::CancelPairingInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::IS_BONDED_FROM_LOCAL,
            std::bind(&BluetoothHostStub::IsBondedFromLocalInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::IS_ACL_CONNECTED,
            std::bind(&BluetoothHostStub::IsAclConnectedInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::IS_ACL_ENCRYPTED,
            std::bind(&BluetoothHostStub::IsAclEncryptedInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_DEVICE_CLASS,
            std::bind(&BluetoothHostStub::GetDeviceClassInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::SET_DEVICE_PIN,
            std::bind(&BluetoothHostStub::SetDevicePinInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::SET_DEVICE_PAIRING_CONFIRMATION,
            std::bind(&BluetoothHostStub::SetDevicePairingConfirmationInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {BluetoothHostStub::SET_DEVICE_PASSKEY,
            std::bind(&BluetoothHostStub::SetDevicePasskeyInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::PAIR_REQUEST_PEPLY,
            std::bind(&BluetoothHostStub::PairRequestReplyInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::READ_REMOTE_RSSI_VALUE,
            std::bind(&BluetoothHostStub::ReadRemoteRssiValueInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_LOCAL_SUPPORTED_UUIDS,
            std::bind(&BluetoothHostStub::GetLocalSupportedUuidsInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::GET_DEVICE_UUIDS,
            std::bind(&BluetoothHostStub::GetDeviceUuidsInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_REGISTER_BLE_ADAPTER_OBSERVER,
            std::bind(&BluetoothHostStub::RegisterBleAdapterObserverInner, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3)},
        {BluetoothHostStub::BT_DEREGISTER_BLE_ADAPTER_OBSERVER,
            std::bind(&BluetoothHostStub::DeregisterBleAdapterObserverInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {BluetoothHostStub::BT_REGISTER_BLE_PERIPHERAL_OBSERVER,
            std::bind(&BluetoothHostStub::RegisterBlePeripheralCallbackInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},
        {BluetoothHostStub::BT_DEREGISTER_BLE_PERIPHERAL_OBSERVER,
            std::bind(&BluetoothHostStub::DeregisterBlePeripheralCallbackInner, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3)},

};

BluetoothHostStub::BluetoothHostStub(){};
BluetoothHostStub::~BluetoothHostStub()
{}
int32_t BluetoothHostStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("BluetoothHostStub::OnRemoteRequest, cmd = %{public}u, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = BluetoothHostStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGE("BluetoothHostStub::OnRemoteRequest, local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return memberFunc(this, data, reply);
        }
    }
    HILOGW("BluetoothHostStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode BluetoothHostStub::RegisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothHostStub::RegisterObserverInner starts");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothHostObserver> observer = OHOS::iface_cast<IBluetoothHostObserver>(remote);
    RegisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHostStub::DeregisterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothHostStub::DeregisterObserverInner starts");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    const sptr<IBluetoothHostObserver> observer = OHOS::iface_cast<IBluetoothHostObserver>(remote);
    DeregisterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHostStub::EnableBtInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = EnableBt();
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::DisableBtInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = DisableBt();
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetProfileInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothHostStub::GetProfileInner starts");
    std::string name = data.ReadString();
    sptr<IRemoteObject> result = GetProfile(name);
    bool ret = reply.WriteStrongParcelable(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBleRemoteInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothHostStub::GetBleRemoteInner starts");
    std::string name = data.ReadString();
    sptr<IRemoteObject> result = GetBleRemote(name);
    bool ret = reply.WriteStrongParcelable(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::BluetoothFactoryResetInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothHostStub::BluetoothFactoryResetInner starts");
    bool result = BluetoothFactoryReset();
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBtStateInner(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("BluetoothHostStub::GetBtStateInner starts");
    int32_t result = GetBtState();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetLocalAddressInner(MessageParcel &data, MessageParcel &reply)
{
    std::string result = GetLocalAddress();
    bool ret = reply.WriteString(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::DisableBleInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = DisableBle();
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::EnableBleInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = EnableBle();
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::IsBleEnabledInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = IsBleEnabled();
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetProfileListInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<uint32_t> result = GetProfileList();
    bool ret = reply.WriteUInt32Vector(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetMaxNumConnectedAudioDevicesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetMaxNumConnectedAudioDevices();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBtConnectionStateInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetBtConnectionState();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBtProfileConnStateInner(MessageParcel &data, MessageParcel &reply)
{
    uint32_t profileId;
    data.ReadUint32(profileId);
    int32_t result = GetBtProfileConnState(profileId);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetLocalDeviceClassInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetLocalDeviceClass();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetLocalDeviceClassInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t deviceClass;
    data.ReadInt32(deviceClass);
    bool result = SetLocalDeviceClass(deviceClass);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetLocalNameInner(MessageParcel &data, MessageParcel &reply)
{
    std::string result = GetLocalName();
    bool ret = reply.WriteString(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetLocalNameInner(MessageParcel &data, MessageParcel &reply)
{
    std::string name;
    data.ReadString(name);
    bool result = SetLocalName(name);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetDeviceTypeInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::GetDeviceType transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetDeviceType address failed");
        return TRANSACTION_ERR;
    }
    int result = GetDeviceType(transport, address);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBtScanModeInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetBtScanMode();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetPhonebookPermissionInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetPhonebookPermission address failed");
        return TRANSACTION_ERR;
    }
    int result = GetPhonebookPermission(address);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetBtScanModeInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t mode;
    data.ReadInt32(mode);
    int32_t duration;
    data.ReadInt32(duration);
    bool result = SetBtScanMode(mode, duration);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetPhonebookPermissionInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::SetPhonebookPermission address failed");
        return TRANSACTION_ERR;
    }
    int32_t permission;
    if (!data.ReadInt32(permission)) {
        HILOGE("BluetoothHostStub::SetPhonebookPermission permission failed");
        return TRANSACTION_ERR;
    }
    bool result = SetPhonebookPermission(address, permission);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBondableModeInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    data.ReadInt32(transport);
    int32_t result = GetBondableMode(transport);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}
ErrCode BluetoothHostStub::GetMessagePermissionInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetMessagePermission address failed");
        return TRANSACTION_ERR;
    }
    int result = GetMessagePermission(address);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetBondableModeInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    data.ReadInt32(transport);
    int32_t mode;
    data.ReadInt32(mode);

    bool result = SetBondableMode(transport, mode);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetMessagePermissionInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::SetMessagePermission address failed");
        return TRANSACTION_ERR;
    }
    int32_t permission;
    if (!data.ReadInt32(permission)) {
        HILOGE("BluetoothHostStub::SetMessagePermission permission failed");
        return TRANSACTION_ERR;
    }
    bool result = SetMessagePermission(address, permission);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetPowerModeInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetPowerMode address failed");
        return TRANSACTION_ERR;
    }
    int result = GetPowerMode(address);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::StartBtDiscoveryInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = StartBtDiscovery();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::CancelBtDiscoveryInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = CancelBtDiscovery();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetDeviceNameInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::GetDeviceName transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetDeviceName address failed");
        return TRANSACTION_ERR;
    }
    std::string result = GetDeviceName(transport, address);
    bool ret = reply.WriteString(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return ERR_INVALID_VALUE;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetDeviceAliasInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetDeviceAlias address failed");
        return TRANSACTION_ERR;
    }
    std::string result = GetDeviceAlias(address);
    bool ret = reply.WriteString(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::IsBtDiscoveringInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    data.ReadInt32(transport);
    bool result = IsBtDiscovering(transport);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetDeviceAliasInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::SetDeviceAlias address failed");
        return TRANSACTION_ERR;
    }
    std::string aliasName;
    if (!data.ReadString(aliasName)) {
        HILOGE("BluetoothHostStub::SetDeviceAlias aliasName failed");
        return TRANSACTION_ERR;
    }
    bool result = SetDeviceAlias(address, aliasName);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetDeviceBatteryLevelInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetDeviceBatteryLevel address failed");
        return TRANSACTION_ERR;
    }
    int result = GetDeviceBatteryLevel(address);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBtDiscoveryEndMillisInner(MessageParcel &data, MessageParcel &reply)
{
    int64_t result = GetBtDiscoveryEndMillis();
    bool ret = reply.WriteInt64(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetPairStateInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::GetPairState transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetPairState address failed");
        return TRANSACTION_ERR;
    }
    int result = GetPairState(transport, address);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetPairedDevicesInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport = data.ReadInt32();
    std::vector<BluetoothRawAddress> result = GetPairedDevices(transport);
    bool ret = true;
    if (!reply.WriteInt32(result.size())) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        ret = false;
    } else {
        for (auto device : result) {
            reply.WriteParcelable(&device);
        }
    }
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::StartPairInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::StartPair transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::StartPair address failed");
        return TRANSACTION_ERR;
    }
    bool result = StartPair(transport, address);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::RemovePairInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport = data.ReadInt32();
    sptr<BluetoothRawAddress> device = data.ReadStrongParcelable<BluetoothRawAddress>();
    bool result = RemovePair(transport, device);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::CancelPairingInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::CancelPairing transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::CancelPairing address failed");
        return TRANSACTION_ERR;
    }
    bool result = CancelPairing(transport, address);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::RemoveAllPairsInner(MessageParcel &data, MessageParcel &reply)
{
    bool result = RemoveAllPairs();
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }

    return NO_ERROR;
}

ErrCode BluetoothHostStub::IsBondedFromLocalInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::IsBondedFromLocal transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::IsBondedFromLocal address failed");
        return TRANSACTION_ERR;
    }
    bool result = IsBondedFromLocal(transport, address);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetDevicePinInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::SetDevicePin address failed");
        return TRANSACTION_ERR;
    }
    std::string pin;
    if (!data.ReadString(pin)) {
        HILOGE("BluetoothHostStub::SetDevicePin pin failed");
        return TRANSACTION_ERR;
    }
    bool result = SetDevicePin(address, pin);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::IsAclConnectedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::IsAclConnected transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::IsAclConnected address failed");
        return TRANSACTION_ERR;
    }
    bool result = IsAclConnected(transport, address);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::RegisterRemoteDeviceObserverInner(MessageParcel &data, MessageParcel &reply)
{
    auto tempObject = data.ReadRemoteObject();
    sptr<IBluetoothRemoteDeviceObserver> observer;
    observer = iface_cast<IBluetoothRemoteDeviceObserver>(tempObject);
    RegisterRemoteDeviceObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHostStub::DeregisterRemoteDeviceObserverInner(MessageParcel &data, MessageParcel &reply)
{
    auto tempObject = data.ReadRemoteObject();
    sptr<IBluetoothRemoteDeviceObserver> observer;
    observer = iface_cast<IBluetoothRemoteDeviceObserver>(tempObject);
    DeregisterRemoteDeviceObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetBleMaxAdvertisingDataLengthInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetBleMaxAdvertisingDataLength();
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::IsAclEncryptedInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::IsAclEncrypted transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::IsAclEncrypted address failed");
        return TRANSACTION_ERR;
    }
    bool result = IsAclEncrypted(transport, address);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetDeviceClassInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::GetDeviceClass address failed");
        return TRANSACTION_ERR;
    }
    int result = GetDeviceClass(address);
    bool ret = reply.WriteInt32(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetDevicePairingConfirmationInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::SetDevicePairingConfirmation transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::SetDevicePairingConfirmation address failed");
        return TRANSACTION_ERR;
    }
    bool accept = false;
    if (!data.ReadBool(accept)) {
        HILOGE("BluetoothHostStub::SetDevicePairingConfirmation accept failed");
        return TRANSACTION_ERR;
    }
    bool result = SetDevicePairingConfirmation(transport, address, accept);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::SetDevicePasskeyInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::SetDevicePasskey transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::SetDevicePasskey address failed");
        return TRANSACTION_ERR;
    }
    int32_t passkey;
    if (!data.ReadInt32(passkey)) {
        HILOGE("BluetoothHostStub::SetDevicePasskey passkey failed");
        return TRANSACTION_ERR;
    }
    bool accept = false;
    if (!data.ReadBool(accept)) {
        HILOGE("BluetoothHostStub::SetDevicePasskey accept failed");
        return TRANSACTION_ERR;
    }
    bool result = SetDevicePasskey(transport, address, passkey, accept);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::PairRequestReplyInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostStub::PairRequestReply transport failed");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::PairRequestReply address failed");
        return TRANSACTION_ERR;
    }
    bool accept = false;
    if (!data.ReadBool(accept)) {
        HILOGE("BluetoothHostStub::PairRequestReply accept failed");
        return TRANSACTION_ERR;
    }
    bool result = PairRequestReply(transport, address, accept);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::ReadRemoteRssiValueInner(MessageParcel &data, MessageParcel &reply)
{
    std::string address;
    if (!data.ReadString(address)) {
        HILOGE("BluetoothHostStub::ReadRemoteRssiValue address failed");
        return TRANSACTION_ERR;
    }
    bool result = ReadRemoteRssiValue(address);
    bool ret = reply.WriteBool(result);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetLocalSupportedUuidsInner(MessageParcel &data, MessageParcel &reply)
{
    std::vector<std::string> uuids;
    GetLocalSupportedUuids(uuids);
    int32_t size = uuids.size();
    bool ret = reply.WriteInt32(size);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    for (auto uuid : uuids) {
        if (!reply.WriteString(uuid)) {
            HILOGE("BluetoothHostStub: GetLocalSupportedUuidsInner write uuid error");
            return TRANSACTION_ERR;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::GetDeviceUuidsInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t transport = 0;
    if (!data.ReadInt32(transport)) {
        HILOGE("BluetoothHostProxy::GetDeviceUuids Read transport error");
        return TRANSACTION_ERR;
    }
    std::string address;
    if (!data.WriteString(address)) {
        HILOGE("BluetoothHostProxy::GetDeviceUuids Read address error");
        return TRANSACTION_ERR;
    }
    std::vector<bluetooth::Uuid> uuids = GetDeviceUuids(transport, address);
    int32_t size = uuids.size();
    bool ret = reply.WriteInt32(size);
    if (!ret) {
        HILOGE("BluetoothHostStub: reply writing failed in: %{public}s.", __func__);
        return TRANSACTION_ERR;
    }
    for (auto uuid : uuids) {
        if (!ParcelBtUuid::WriteToParcel(reply, uuid)) {
            HILOGE("BluetoothHostStub: GetDeviceUuidsInner write uuid error");
            return TRANSACTION_ERR;
        }
    }
    return NO_ERROR;
}

ErrCode BluetoothHostStub::RegisterBleAdapterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    auto tempObject = data.ReadRemoteObject();
    sptr<IBluetoothHostObserver> observer;
    observer = iface_cast<IBluetoothHostObserver>(tempObject);
    RegisterBleAdapterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHostStub::DeregisterBleAdapterObserverInner(MessageParcel &data, MessageParcel &reply)
{
    auto tempObject = data.ReadRemoteObject();
    sptr<IBluetoothHostObserver> observer;
    observer = iface_cast<IBluetoothHostObserver>(tempObject);
    DeregisterBleAdapterObserver(observer);
    return NO_ERROR;
}

ErrCode BluetoothHostStub::RegisterBlePeripheralCallbackInner(MessageParcel &data, MessageParcel &reply)
{
    auto tempObject = data.ReadRemoteObject();
    sptr<IBluetoothBlePeripheralObserver> observer;
    observer = iface_cast<IBluetoothBlePeripheralObserver>(tempObject);
    RegisterBlePeripheralCallback(observer);
    return NO_ERROR;
}

ErrCode BluetoothHostStub::DeregisterBlePeripheralCallbackInner(MessageParcel &data, MessageParcel &reply)
{
    auto tempObject = data.ReadRemoteObject();
    sptr<IBluetoothBlePeripheralObserver> observer;
    observer = iface_cast<IBluetoothBlePeripheralObserver>(tempObject);
    DeregisterBlePeripheralCallback(observer);
    return NO_ERROR;
}
}  // namespace Bluetooth
}  // namespace OHOS
