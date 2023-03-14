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

#ifndef OHOS_BLUETOOTH_STANDARD_HOST_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_HOST_INTERFACE_H

#include "../../../../services/bluetooth/ipc/parcel/bluetooth_raw_address.h"
#include "../../../../services/bluetooth/ipc/parcel/parcel_bt_uuid.h"
#include "../parcel/parcel_bt_uuid.h"
#include "i_bluetooth_ble_peripheral_observer.h"
#include "i_bluetooth_host_observer.h"
#include "i_bluetooth_remote_device_observer.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Bluetooth {
namespace {
const std::string PROFILE_GATT_CLIENT = "GattClientServer";
const std::string PROFILE_GATT_SERVER = "GattServerServer";
const std::string PROFILE_A2DP_SRC = "A2dpSrcServer";
const std::string PROFILE_A2DP_SINK = "A2dpSnkServer";
const std::string PROFILE_AVRCP_CT = "AvrcpCtServer";
const std::string PROFILE_AVRCP_TG = "AvrcpTgServer";
const std::string PROFILE_HFP_AG = "HfpAgServer";
const std::string PROFILE_HFP_HF = "HfpHfServer";
const std::string PROFILE_MAP_MCE = "MapMceServer";
const std::string PROFILE_MAP_MSE = "MapMseServer";
const std::string PROFILE_PBAP_PCE = "PbapPceServer";
const std::string PROFILE_PBAP_PSE = "PbapPseServer";
const std::string PROFILE_SPP = "SocketServer";
const std::string PROFILE_DI = "DIServer";
const std::string BLE_ADVERTISER_SERVER = "BleAdvertiserServer";
const std::string BLE_CENTRAL_MANAGER_SERVER = "BleCentralMangerServer";
const std::string PROFILE_HID_HOST_SERVER = "BluetoothHidHostServer";
const std::string PROFILE_PAN_SERVER = "BluetoothPanServer";
const std::string PROFILE_OPP_SERVER = "BluetoothOppServer";
}  // namespace

class IBluetoothHost : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothHost");

    enum Code {
        BT_REGISTER_OBSERVER = 0,
        BT_DEREGISTER_OBSERVER,
        BT_ENABLE,
        BT_DISABLE,
        BT_GETSTATE,
        BT_GETPROFILE,
        BT_GET_BLE,
        BT_FACTORY_RESET,
        BT_DISABLE_BLE,
        BT_ENABLE_BLE,
        BT_IS_BLE_ENABLED,
        BT_GET_PROFILE_LIST,
        BT_GET_MAXNUM_CONNECTED_AUDIODEVICES,
        BT_GET_BT_STATE,
        BT_GET_BT_PROFILE_CONNSTATE,
        BT_GET_LOCAL_DEVICE_CLASS,
        BT_SET_LOCAL_DEVICE_CLASS,
        BT_GET_LOCAL_ADDRESS,
        BT_GET_LOCAL_NAME,
        BT_SET_LOCAL_NAME,
        BT_GET_BT_SCAN_MODE,
        BT_SET_BT_SCAN_MODE,
        BT_GET_BONDABLE_MODE,
        BT_SET_BONDABLE_MODE,
        BT_START_BT_DISCOVERY,
        BT_CANCEL_BT_DISCOVERY,
        BT_IS_BT_DISCOVERING,
        BT_GET_BT_DISCOVERY_END_MILLIS,
        BT_GET_PAIRED_DEVICES,
        BT_REMOVE_PAIR,
        BT_REMOVE_ALL_PAIRS,
        BT_REGISTER_REMOTE_DEVICE_OBSERVER,
        BT_DEREGISTER_REMOTE_DEVICE_OBSERVER,
        BT_GET_BLE_MAX_ADVERTISING_DATALENGTH,
        GET_DEVICE_TYPE,
        GET_PHONEBOOK_PERMISSION,
        SET_PHONEBOOK_PERMISSION,
        GET_MESSAGE_PERMISSION,
        SET_MESSAGE_PERMISSION,
        GET_POWER_MODE,
        GET_DEVICE_NAME,
        GET_DEVICE_ALIAS,
        SET_DEVICE_ALIAS,
        GET_DEVICE_BATTERY_LEVEL,
        GET_PAIR_STATE,
        START_PAIR,
        CANCEL_PAIRING,
        IS_BONDED_FROM_LOCAL,
        IS_ACL_CONNECTED,
        IS_ACL_ENCRYPTED,
        GET_DEVICE_CLASS,
        SET_DEVICE_PIN,
        SET_DEVICE_PAIRING_CONFIRMATION,
        SET_DEVICE_PASSKEY,
        PAIR_REQUEST_PEPLY,
        READ_REMOTE_RSSI_VALUE,
        GET_LOCAL_SUPPORTED_UUIDS,
        GET_DEVICE_UUIDS,
        BT_REGISTER_BLE_ADAPTER_OBSERVER,
        BT_DEREGISTER_BLE_ADAPTER_OBSERVER,
        BT_REGISTER_BLE_PERIPHERAL_OBSERVER,
        BT_DEREGISTER_BLE_PERIPHERAL_OBSERVER,
    };

    virtual void RegisterObserver(const sptr<IBluetoothHostObserver> &observer) = 0;
    virtual void DeregisterObserver(const sptr<IBluetoothHostObserver> &observer) = 0;
    virtual bool EnableBt() = 0;
    virtual bool DisableBt() = 0;
    virtual sptr<IRemoteObject> GetProfile(const std::string &name) = 0;
    virtual sptr<IRemoteObject> GetBleRemote(const std::string &name) = 0;
    virtual bool BluetoothFactoryReset() = 0;
    virtual int32_t GetBtState() = 0;
    virtual std::string GetLocalAddress() = 0;
    virtual bool DisableBle() = 0;
    virtual bool EnableBle() = 0;
    virtual bool IsBleEnabled() = 0;
    virtual std::vector<uint32_t> GetProfileList() = 0;
    virtual int32_t GetMaxNumConnectedAudioDevices() = 0;
    virtual int32_t GetBtConnectionState() = 0;
    virtual int32_t GetBtProfileConnState(uint32_t profileId) = 0;
    virtual int32_t GetLocalDeviceClass() = 0;
    virtual bool SetLocalDeviceClass(const int32_t &deviceClass) = 0;
    virtual std::string GetLocalName() = 0;
    virtual bool SetLocalName(const std::string &name) = 0;
    virtual int32_t GetBtScanMode() = 0;
    virtual bool SetBtScanMode(int32_t mode, int32_t duration) = 0;
    virtual int32_t GetBondableMode(const int32_t transport) = 0;
    virtual bool SetBondableMode(int32_t transport, int32_t mode) = 0;
    virtual bool StartBtDiscovery() = 0;
    virtual bool CancelBtDiscovery() = 0;
    virtual bool IsBtDiscovering(const int32_t transport) = 0;
    virtual long GetBtDiscoveryEndMillis() = 0;
    virtual std::vector<BluetoothRawAddress> GetPairedDevices(const int32_t transport) = 0;
    virtual bool RemovePair(const int32_t transport, const sptr<BluetoothRawAddress> &device) = 0;
    virtual bool RemoveAllPairs() = 0;
    virtual void RegisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer) = 0;
    virtual void DeregisterRemoteDeviceObserver(const sptr<IBluetoothRemoteDeviceObserver> &observer) = 0;
    virtual int32_t GetBleMaxAdvertisingDataLength() = 0;
    virtual int32_t GetDeviceType(int32_t transport, const std::string &address) = 0;
    virtual int32_t GetPhonebookPermission(const std::string &address) = 0;
    virtual bool SetPhonebookPermission(const std::string &address, int32_t permission) = 0;
    virtual int32_t GetMessagePermission(const std::string &address) = 0;
    virtual bool SetMessagePermission(const std::string &address, int32_t permission) = 0;
    virtual int32_t GetPowerMode(const std::string &address) = 0;
    virtual std::string GetDeviceName(int32_t transport, const std::string &address) = 0;
    virtual std::string GetDeviceAlias(const std::string &address) = 0;
    virtual bool SetDeviceAlias(const std::string &address, const std::string &aliasName) = 0;
    virtual int32_t GetDeviceBatteryLevel(const std::string &address) = 0;
    virtual int32_t GetPairState(int32_t transport, const std::string &address) = 0;
    virtual bool StartPair(int32_t transport, const std::string &address) = 0;
    virtual bool CancelPairing(int32_t transport, const std::string &address) = 0;
    virtual bool IsBondedFromLocal(int32_t transport, const std::string &address) = 0;
    virtual bool IsAclConnected(int32_t transport, const std::string &address) = 0;
    virtual bool IsAclEncrypted(int32_t transport, const std::string &address) = 0;
    virtual int32_t GetDeviceClass(const std::string &address) = 0;
    virtual bool SetDevicePin(const std::string &address, const std::string &pin) = 0;
    virtual bool SetDevicePairingConfirmation(int32_t transport, const std::string &address, bool accept) = 0;
    virtual bool SetDevicePasskey(int32_t transport, const std::string &address, int32_t passkey, bool accept) = 0;
    virtual bool PairRequestReply(int32_t transport, const std::string &address, bool accept) = 0;
    virtual bool ReadRemoteRssiValue(const std::string &address) = 0;
    virtual void GetLocalSupportedUuids(std::vector<std::string> &uuids) = 0;
    virtual std::vector<bluetooth::Uuid> GetDeviceUuids(int32_t transport, const std::string &address) = 0;
    virtual void RegisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer) = 0;
    virtual void DeregisterBleAdapterObserver(const sptr<IBluetoothHostObserver> &observer) = 0;
    virtual void RegisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer) = 0;
    virtual void DeregisterBlePeripheralCallback(const sptr<IBluetoothBlePeripheralObserver> &observer) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_HOST_INTERFACE_H
