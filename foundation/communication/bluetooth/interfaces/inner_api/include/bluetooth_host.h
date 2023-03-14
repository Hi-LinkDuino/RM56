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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines bluetooth host, including observer and common functions.
 *
 * @since 6
 */

/**
 * @file bluetooth_host.h
 *
 * @brief Framework bluetooth host interface.
 *
 * @since 6
 */

#ifndef BLUETOOTH_HOST_H
#define BLUETOOTH_HOST_H

#include <string>

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"
#include "bluetooth_device_class.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Represents framework host device basic observer.
 *
 * @since 6
 */
class BluetoothHostObserver {
public:
    /**
     * @brief A destructor used to delete the <b>BluetoothHostObserver</b> instance.
     *
     * @since 6
     */
    virtual ~BluetoothHostObserver() = default;

    // common
    /**
     * @brief Adapter state change function.
     *
     * @param transport Transport type when state change.
     *        BTTransport::ADAPTER_BREDR : classic;
     *        BTTransport::ADAPTER_BLE : ble.
     * @param state Change to the new state.
     *        BTStateID::STATE_TURNING_ON;
     *        BTStateID::STATE_TURN_ON;
     *        BTStateID::STATE_TURNING_OFF;
     *        BTStateID::STATE_TURN_OFF.
     * @since 6
     */
    virtual void OnStateChanged(const int transport, const int status) = 0;

    // gap
    /**
     * @brief Discovery state changed observer.
     *
     * @param status Device discovery status.
     * @since 6
     */
    virtual void OnDiscoveryStateChanged(int status) = 0;

    /**
     * @brief Discovery result observer.
     *
     * @param device Remote device.
     * @since 6
     */
    virtual void OnDiscoveryResult(const BluetoothRemoteDevice &device) = 0;

    /**
     * @brief Pair request observer.
     *
     * @param device Remote device.
     * @since 6
     */
    virtual void OnPairRequested(const BluetoothRemoteDevice &device) = 0;

    /**
     * @brief Pair confirmed observer.
     *
     * @param device Remote device.
     * @param reqType Pair type.
     * @param number Paired passkey.
     * @since 6
     */
    virtual void OnPairConfirmed(const BluetoothRemoteDevice &device, int reqType, int number) = 0;

    /**
     * @brief Scan mode changed observer.
     *
     * @param mode Device scan mode.
     * @since 6
     */
    virtual void OnScanModeChanged(int mode) = 0;

    /**
     * @brief Device name changed observer.
     *
     * @param deviceName Device name.
     * @since 6
     */
    virtual void OnDeviceNameChanged(const std::string &deviceName) = 0;

    /**
     * @brief Device address changed observer.
     *
     * @param address Device address.
     * @since 6
     */
    virtual void OnDeviceAddrChanged(const std::string &address) = 0;
};

/**
 * @brief Represents remote device observer.
 *
 * @since 6
 */
class BluetoothRemoteDeviceObserver {
public:
    /**
     * @brief A destructor used to delete the <b>BluetoothRemoteDeviceObserver</b> instance.
     *
     * @since 6
     */
    virtual ~BluetoothRemoteDeviceObserver() = default;

    /**
     * @brief Pair status changed observer.
     *
     * @param device Remote device.
     * @param status Remote device pair status.
     * @since 6
     */
    virtual void OnPairStatusChanged(const BluetoothRemoteDevice &device, int status) = 0;

    /**
     * @brief Remote uuid changed observer.
     *
     * @param device Remote device.
     * @param uuids Remote device uuids.
     * @since 6
     */
    virtual void OnRemoteUuidChanged(const BluetoothRemoteDevice &device, const std::vector<ParcelUuid> &uuids) = 0;

    /**
     * @brief Remote name changed observer.
     *
     * @param device Remote device.
     * @param deviceName Remote device name.
     * @since 6
     */
    virtual void OnRemoteNameChanged(const BluetoothRemoteDevice &device, const std::string &deviceName) = 0;

    /**
     * @brief Remote alias changed observer.
     *
     * @param device Remote device.
     * @param alias Remote device alias.
     * @since 6
     */
    virtual void OnRemoteAliasChanged(const BluetoothRemoteDevice &device, const std::string &alias) = 0;

    /**
     * @brief Remote cod changed observer.
     *
     * @param device Remote device.
     * @param cod Remote device cod.
     * @since 6
     */
    virtual void OnRemoteCodChanged(const BluetoothRemoteDevice &device, const BluetoothDeviceClass &cod) = 0;

    /**
     * @brief Remote battery level changed observer.
     *
     * @param device Remote device.
     * @param cod Remote device battery Level.
     * @since 6
     */
    virtual void OnRemoteBatteryLevelChanged(const BluetoothRemoteDevice &device, int batteryLevel) = 0;

    /**
     * @brief Remote rssi event observer.
     *
     * @param device Remote device.
     * @param rssi Remote device rssi.
     * @param status Read status.
     * @since 6
     */
    virtual void OnReadRemoteRssiEvent(const BluetoothRemoteDevice &device, int rssi, int status) = 0;
};

/**
 * @brief Represents framework host device.
 *
 * @since 6
 */
class BLUETOOTH_API BluetoothHost {
public:
    // common
    /**
     * @brief Get default host device.
     *
     * @return Returns the singleton instance.
     * @since 6
     */
    static BluetoothHost &GetDefaultHost();

    /**
     * @brief Get remote device instance.
     *
     * @param addr Remote device address.
     * @param transport Adapter transport.
     * @return Returns remote device instance.
     * @since 6
     */
    BluetoothRemoteDevice GetRemoteDevice(const std::string &addr, int transport) const;

    /**
     * @brief Register observer.
     *
     * @param observer Class BluetoothHostObserver pointer to register observer.
     * @since 6
     */
    void RegisterObserver(BluetoothHostObserver &observer);

    /**
     * @brief Deregister observer.
     *
     * @param observer Class BluetoothHostObserver pointer to deregister observer.
     * @since 6
     */
    void DeregisterObserver(BluetoothHostObserver &observer);

    /**
     * @brief Enable classic.
     *
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    bool EnableBt();

    /**
     * @brief Disable classic.
     *
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    bool DisableBt();

    /**
     * @brief Get classic enable/disable state.
     *
     * @return Returns classic enable/disable state.
     *         BTStateID::STATE_TURNING_ON;
     *         BTStateID::STATE_TURN_ON;
     *         BTStateID::STATE_TURNING_OFF;
     *         BTStateID::STATE_TURN_OFF.
     * @since 6
     */
    int GetBtState() const;

    /**
     * @brief Disable ble.
     *
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    bool DisableBle();

    /**
     * @brief Enable ble.
     *
     * @return Returns <b>true</b> if the operation is accepted;
     *         returns <b>false</b> if the operation is rejected.
     * @since 6
     */
    bool EnableBle();

    /**
     * @brief Get ble enable/disable state.
     *
     * @return Returns <b>true</b> if ble is enabled;
     *         returns <b>false</b> if ble is not enabled.
     * @since 6
     */
    bool IsBleEnabled() const;

    /**
     * @brief Factory reset bluetooth service.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool BluetoothFactoryReset();

    /**
     * @brief Get profile service ID list.
     *
     * @return Returns vector of enabled profile services ID.
     * @since 6
     */
    std::vector<uint32_t> GetProfileList() const;

    /**
     * @brief Get max audio connected devices number.
     *
     * @return Returns max device number that audio can connect.
     * @since 6
     */
    int GetMaxNumConnectedAudioDevices() const;

    /**
     * @brief Get bluetooth connects state.
     *
     * @return Returns bluetooth connects state.
     *         BTConnectState::CONNECTING;
     *         BTConnectState::CONNECTED;
     *         BTConnectState::DISCONNECTING;
     *         BTConnectState::DISCONNECTED.
     * @since 6
     */
    int GetBtConnectionState() const;

    /**
     * @brief Get profile service connect state.
     *
     * @param profileID Profile service ID.
     * @return Returns connect state for designated profile service.
     *         BTConnectState::CONNECTING;
     *         BTConnectState::CONNECTED;
     *         BTConnectState::DISCONNECTING;
     *         BTConnectState::DISCONNECTED.
     * @since 6
     */
    int GetBtProfileConnState(uint32_t profileId) const;

    /**
     * @brief Get local device supported uuids.
     *
     * @param[out] Vector which use to return support uuids.
     * @since 6
     */
    void GetLocalSupportedUuids(std::vector<ParcelUuid> &uuids);

    /**
     * @brief Start adapter manager, passthrough only.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool Start();

    /**
     * @brief Stop adapter manager, passthrough only.
     *
     * @since 6
     */
    void Stop();

    // gap
    /**
     * @brief Get local device class.
     *
     * @return Returns local device class.
     * @since 6
     */
    BluetoothDeviceClass GetLocalDeviceClass() const;

    /**
     * @brief Set local device class.
     *
     * @param deviceClass Device class.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetLocalDeviceClass(const BluetoothDeviceClass &deviceClass);

    /**
     * @brief Get local device address.
     *
     * @return Returns local device address.
     * @since 6
     */
    std::string GetLocalAddress() const;

    /**
     * @brief Get local device name.
     *
     * @return Returns local device name.
     * @since 6
     */
    std::string GetLocalName() const;

    /**
     * @brief Set local device name.
     *
     * @param name Device name.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetLocalName(const std::string &name);

    /**
     * @brief Get device scan mode.
     *
     * @return Returns bluetooth scan mode.
     * @since 6
     */
    int GetBtScanMode() const;

    /**
     * @brief Set device scan mode.
     *
     * @param mode Scan mode.
     * @param duration Scan time.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetBtScanMode(int mode, int duration);

    /**
     * @brief Get local device bondable mode.
     *
     * @param transport Adapter transport.
     * @return Returns local device bondable mode.
     * @since 6
     */
    int GetBondableMode(int transport) const;

    /**
     * @brief Set local device bondable mode.
     *
     * @param transport Adapter transport.
     * @param mode Device bondable mode.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetBondableMode(int transport, int mode);

    /**
     * @brief Get device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool StartBtDiscovery();

    /**
     * @brief Cancel device discovery.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool CancelBtDiscovery();

    /**
     * @brief Check if device is discovering.
     *
     * @return Returns <b>true</b> if device is discovering;
     *         returns <b>false</b> if device is not discovering.
     * @since 6
     */
    bool IsBtDiscovering(int transport = BT_TRANSPORT_BREDR) const;

    /**
     * @brief Get device discovery end time.
     *
     * @return Returns device discovery end time.
     * @since 6
     */
    long GetBtDiscoveryEndMillis() const;

    /**
     * @brief Get paired devices.
     *
     * @param transport Adapter transport.
     * @return Returns paired devices vector.
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetPairedDevices(int transport) const;

    /**
     * @brief Remove pair.
     *
     * @param device Remote device address.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool RemovePair(const BluetoothRemoteDevice &device);

    /**
     * @brief Remove all pairs.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool RemoveAllPairs();

    /**
     * @brief Check if bluetooth address is valid.
     *
     * @param addr Bluetooth address.
     * @return Returns <b>true</b> if bluetooth address is valid;
     *         returns <b>false</b> if bluetooth address is not valid.
     * @since 6
     */
    static bool IsValidBluetoothAddr(const std::string &addr);

    /**
     * @brief Register remote device observer.
     *
     * @param observer Class BluetoothRemoteDeviceObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    void RegisterRemoteDeviceObserver(BluetoothRemoteDeviceObserver &observer);

    /**
     * @brief Deregister remote device observer.
     *
     * @param observer Class BluetoothRemoteDeviceObserver pointer to deregister observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    void DeregisterRemoteDeviceObserver(BluetoothRemoteDeviceObserver &observer);

    /**
     * @brief Get max advertising data length.
     *
     * @return Returns max advertising data length.
     * @since 6
     */
    int GetBleMaxAdvertisingDataLength() const;

private:
    static BluetoothHost hostAdapter_;

    /**
     * @brief A constructor used to create a <b>BluetoothHost</b> instance.
     *
     * @since 6
     */
    BluetoothHost();

    /**
     * @brief A destructor used to delete the <b>BluetoothHost</b> instance.
     *
     * @since 6
     */
    ~BluetoothHost();

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothHost);
    BLUETOOTH_DECLARE_IMPL();
};
} // namespace Bluetooth
} // namespace OHOS

#endif  // BLUETOOTH_HOST_H
