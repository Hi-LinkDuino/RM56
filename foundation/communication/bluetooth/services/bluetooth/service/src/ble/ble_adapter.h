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

#ifndef BLE_ADAPTER_H
#define BLE_ADAPTER_H

#include <map>
#include <memory>

#include "context.h"
#include "interface_adapter_ble.h"
#include "raw_address.h"

#include "base_observer_list.h"
#include "ble_advertiser_impl.h"
#include "ble_central_manager_impl.h"
#include "ble_properties.h"
#include "ble_security.h"
#include "bt_uuid.h"
#include "btm.h"

/*
 * @brief The Bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
/**
 *  @brief BLE Adpter implementation class
 */
class BleAdapter : public IAdapterBle, public utility::Context {
public:
    /**
     * @brief Constructor.
     */
    BleAdapter();

    /**
     * @brief Destructor.
     */
    ~BleAdapter();

    utility::Context *GetContext() override;

    /**
     *  @brief Turn on the BLE Bluetooth adapter
     *
     *  @return @c true Turn on BLE Bluetooth successfully
     *          @c false Failed to turn on BLE Bluetooth
     */
    void Enable() override;

    /**
     *  @brief Turn off the BLE Bluetooth adapter
     *
     *  @return @c true Turn off BLE Bluetooth successfully
     *          @c false Failed to turn off BLE Bluetooth
     */
    void Disable() override;

    /**
     *  @brief Processing after Bluetooth startup
     *
     *  @return @c true success
     *          @c false failure
     */
    void PostEnable() override;

    /**
     *  @brief Get local host bluetooth address
     *
     *  @return @c Local host bluetooth address
     */
    std::string GetLocalAddress() const override;

    /**
     *  @brief Get local host bluetooth name
     *
     *  @return @c Local host bluetooth name
     */
    std::string GetLocalName() const override;

    /**
     *  @brief Set local host bluetooth name
     *
     *  @param [in] name Device name.
     *  @return @c true success
     *          @c false failure
     */
    bool SetLocalName(const std::string &name) const override;

    /// add adapter manager common api
    std::string GetDeviceName(const RawAddress &device) const override;
    std::vector<Uuid> GetDeviceUuids(const RawAddress &device) const override;
    std::vector<RawAddress> GetPairedDevices() const override;
    std::vector<RawAddress> GetConnectedDevices() const;
    bool StartPair(const RawAddress &device) override;
    bool CancelPairing(const RawAddress &device) override;
    bool RemovePair(const RawAddress &device) override;
    bool RemoveAllPairs() override;
    bool IsBondedFromLocal(const RawAddress &device) const override;
    bool SetDevicePasskey(const RawAddress &device, int passkey, bool accept) const override;
    bool PairRequestReply(const RawAddress &device, bool accept) const override;
    bool IsAclConnected(const RawAddress &device) const override;
    bool IsAclEncrypted(const RawAddress &device) const override;
    int GetPairState(const RawAddress &device) const override;
    int GetBondableMode() const override;
    bool SetBondableMode(int mode) const override;
    bool SetDevicePairingConfirmation(const RawAddress &device, bool accept) const override;
    int GetDeviceType(const RawAddress &device) const override;
    int GetBleMaxAdvertisingDataLength() const override;
    int GetIoCapability() const override;
    bool SetIoCapability(int ioCapability) const override;
    bool IsBleEnabled() const;
    bool IsBtDiscovering() const override;

    /// FW api passthrough from service
    void StartAdvertising(const BleAdvertiserSettingsImpl &settings, const BleAdvertiserDataImpl &advData,
        const BleAdvertiserDataImpl &scanResponse, uint8_t advHandle) const override;
    void StopAdvertising(uint8_t advHandle) const override;
    void Close(uint8_t advHandle) const override;
    void StartScan() const override;
    void StartScan(const BleScanSettingsImpl &setting) const override;
    void StopScan() const override;
    int ConfigScanFilter(const int clientId, const std::vector<BleScanFilterImpl> &filters) override;
    void RemoveScanFilter(const int clientId) override;
    int GetAdvertisingStatus() const override;
    bool IsLlPrivacySupported() const override;
    void AddCharacteristicValue(uint8_t adtype, const std::string &data) const override;

    void RegisterBleAdvertiserCallback(IBleAdvertiserCallback &callback) override;
    void DeregisterBleAdvertiserCallback() const override;
    void RegisterBleCentralManagerCallback(IBleCentralManagerCallback &callback) override;
    void DeregisterBleCentralManagerCallback() const override;
    void RegisterBlePeripheralCallback(IBlePeripheralCallback &callback) const override;
    void DeregisterBlePeripheralCallback(IBlePeripheralCallback &callback) const override;
    void RegisterBleSecurityCallback(BaseObserverList<IAdapterBleObserver> &callback);
    void DeregisterBleSecurityCallback() const;

    bool RegisterBleAdapterObserver(IAdapterBleObserver &observer) const override;
    bool DeregisterBleAdapterObserver(IAdapterBleObserver &observer) const override;
    int GetPeerDeviceAddrType(const RawAddress &device) const override;

    /// pair
    void LePairComplete(const RawAddress &device, const int status) const;
    void LePairingStatus(const RawAddress &device) const;
    void EncryptionComplete(const RawAddress &device) const;
    void NotifyAllWaitContinue() const;
    bool IsRemovePairedDevice(const RawAddress &device) const;

    void OnStartAdvertisingEvt() const;
    void OnStopAdvertisingEvt() const;

    /**
     * @brief Read Remote Rssi Value.
     *
     * @return @c true sucessfull otherwise false.
     */
    bool ReadRemoteRssiValue(const RawAddress &device) const override;
    uint8_t GetAdvertiserHandle() const override;
    bool RemovePairWithDisConnect(const RawAddress &device, bool isDisconnect = true) const;

private:
    int RegisterCallbackToBtm();
    int DeregisterCallbackToBtm() const;
    int DeregisterAllCallback() const;
    bool EnableTask();
    bool DisableTask();
    bool PostEnableTask() const;
    void StartOrStopAdvAndScan(
        const STOP_ALL_ADV_TYPE &stopAllAdvType, const STOP_SCAN_TYPE &scanType, bool isStartAdvAndScan = false) const;
    void LoadConfig() const;
    void LeConnectionCompleteTask(uint8_t status, uint16_t connectionHandle, const BtAddr &addr, uint8_t role) const;
    void LeDisconnectionCompleteTask(uint8_t status, uint16_t connectionHandle, uint8_t reason) const;
    void OnReadRemoteRssiEventTask(uint8_t status, const BtAddr &addr, int8_t rssi) const;
    // pair
    void ReadPeerDeviceInfoFromConf(const std::vector<std::string> &pairedAddrList) const;
    bool SavePeerDeviceInfoToConf(const std::map<std::string, BlePeripheralDevice> &peerConnDeviceList) const;
    void SavePeerDevices2BTM(const std::map<std::string, BlePeripheralDevice> &peerConnDeviceList) const;
    void ClearPeerDeviceInfo() const;
    void ClearScanResultInfo() const;
    int SetRpaAddrAndTypeToBtm();
    int SetLocalIrkAndIdentityAddrToBtm() const;
    int InitBtmAndGap();
    int SetBleRoles() const;
    void ExAdvClearHandle() const;
    std::string ReadRemoteDeviceNameByGatt(const RawAddress &addr, int appID) const;
    int RegisterGattClientApplication(const RawAddress &addr) const;
    void DeregisterGattClientApplication(int appID) const;

    /// BTM ACL Change status callback.
    static void LeConnectionComplete(
        uint8_t status, uint16_t connectionHandle, const BtAddr *addr, uint8_t role, void *context);
    static void LeDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context);
    static void OnReadRemoteRssiEvent(uint8_t status, const BtAddr *addr, int8_t rssi, void *context);
    static void GenResPriAddrResult(uint8_t result, const uint8_t addr[BT_ADDRESS_SIZE], void *context);

    BT_DISALLOW_COPY_AND_ASSIGN(BleAdapter);
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  /// BLE_ADAPTER_H
