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
 * @brief Defines adapter ble, including observer, callbacks and common functions.
 *
 * @since 6
 */

/**
 * @file interface_adapter_ble.h
 *
 * @brief Adapter ble interface.
 *
 * @since 6
 */

#ifndef INTERFACE_ADAPTER_BLE_H
#define INTERFACE_ADAPTER_BLE_H

#include "interface_adapter.h"
#include "ble_service_data.h"
#include <memory>

#ifndef NO_SANITIZE
#ifdef __has_attribute
#if __has_attribute(no_sanitize)
#define NO_SANITIZE(type) __attribute__((no_sanitize(type)))
#endif
#endif
#endif

#ifndef NO_SANITIZE
#define NO_SANITIZE(type)
#endif

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents central manager callbacks.
 *
 * @since 6
 */
class IBleCentralManagerCallback {
public:
    /**
     * @brief A destructor used to delete the <b>IBleCentralManagerCallback</b> instance.
     *
     * @since 6
     */
    virtual ~IBleCentralManagerCallback() = default;

    /**
     * @brief Scan callback.
     *
     * @param result Scan result.
     * @since 6
     */
    virtual void OnScanCallback(const BleScanResultImpl &result) = 0;

    /**
     * @brief Scan results event callback.
     *
     * @param results Scan results.
     * @since 6
     */
    virtual void OnBleBatchScanResultsEvent(std::vector<BleScanResultImpl> &results) = 0;

    /**
     * @brief Start or Stop scan event callback.
     *
     * @param resultCode Start scan result code.
     * @since 6
     */
    virtual void OnStartOrStopScanEvent(int resultCode, bool isStartScan) = 0;
};

/**
 * @brief Represents advertise callbacks.
 *
 * @since 6
 */
class IBleAdvertiserCallback {
public:
    virtual ~IBleAdvertiserCallback() = default;
    virtual void OnStartResultEvent(int result, uint8_t advHandle, int opcode = BLE_ADV_DEFAULT_OP_CODE) = 0;
    virtual void OnAutoStopAdvEvent(uint8_t advHandle) = 0;
};

/**
 * @brief Represents ble adapter observer.
 *
 * @since 6
 */
class IAdapterBleObserver {
public:
    /**
     * @brief A destructor used to delete the <b>IBleAdapterObserver</b> instance.
     *
     * @since 6
     */
    virtual ~IAdapterBleObserver() = default;

    /**
     * @brief Discovery state changed observer.
     *
     * @param status Device discovery status.
     * @since 6
     */
    virtual void OnDiscoveryStateChanged(const int status) = 0;

    /**
     * @brief Discovery result observer.
     *
     * @param device Remote device.
     * @since 6
     */
    virtual void OnDiscoveryResult(const RawAddress &device) = 0;

    /**
     * @brief Pair request observer.
     *
     * @param device Remote device.
     * @since 6
     */
    virtual void OnPairRequested(const BTTransport transport, const RawAddress &device) = 0;

    /**
     * @brief Pair confirmed observer.
     *
     * @param device Remote device.
     * @param reqType Pair type.
     * @param number Paired passkey.
     * @since 6
     */
    virtual void OnPairConfirmed(
        const BTTransport transport, const RawAddress &device, const int reqType, const int number) = 0;

    /**
     * @brief Scan mode changed observer.
     *
     * @param mode Device scan mode.
     * @since 6
     */
    virtual void OnScanModeChanged(const int mode) = 0;

    /**
     * @brief Device name changed observer.
     *
     * @param deviceName Device name.
     * @since 6
     */
    virtual void OnDeviceNameChanged(const std::string deviceName) = 0;

    /**
     * @brief Device address changed observer.
     *
     * @param address Device address.
     * @since 6
     */
    virtual void OnDeviceAddrChanged(const std::string address) = 0;

    /**
     * @brief Advertising state changed observer.
     *
     * @param state Advertising state.
     * @since 6
     */
    virtual void OnAdvertisingStateChanged(const int state) = 0;
};

/**
 * @brief Represents peripheral callback.
 *
 * @since 6
 */
class IBlePeripheralCallback {
public:
    /**
     * @brief A destructor used to delete the <b>IBlePeripheralCallback</b> instance.
     *
     * @since 6
     */
    virtual ~IBlePeripheralCallback() = default;

    /**
     * @brief Read remote rssi event callback.
     *
     * @param device Remote device.
     * @param rssi Remote device rssi.
     * @param status Read status.
     * @since 6
     */
    virtual void OnReadRemoteRssiEvent(const RawAddress &device, int rssi, int status) = 0;
    /**
     * @brief Read remote rssi event callback.
     *
     * @param device Remote device.
     * @param rssi Remote device rssi.
     * @param status Read status.
     * @since 6
     */
    virtual void OnPairStatusChanged(const BTTransport transport, const RawAddress &device, int status) = 0;
};

/**
 * @brief Represents ble adapter interface.
 *
 * @since 6
 */
class IAdapterBle : public IAdapter {
public:
    /**
     * @brief Register central manager callback.
     *
     * @param callback Class IBleCentralManagerCallback pointer to register callback.
     * @since 6
     */
    virtual void RegisterBleCentralManagerCallback(IBleCentralManagerCallback &callback) = 0;

    /**
     * @brief Deregister central manager callback.
     *
     * @since 6
     */
    virtual void DeregisterBleCentralManagerCallback() const = 0;

    /**
     * @brief Register advertiser callback.
     *
     * @param callback Class IBleAdvertiseCallback pointer to register callback.
     * @since 6
     */
    virtual void RegisterBleAdvertiserCallback(IBleAdvertiserCallback &callback) = 0;

    /**
     * @brief Deregister advertiser callback.
     *
     * @since 6
     */
    virtual void DeregisterBleAdvertiserCallback() const = 0;

    /**
     * @brief Read remote device rssi value.
     *
     * @param device Remote device
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool ReadRemoteRssiValue(const RawAddress &device) const = 0;

    /**
     * @brief Register ble adapter observer.
     *
     * @param observer Class IBleAdapterObserver pointer to register observer.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool RegisterBleAdapterObserver(IAdapterBleObserver &observer) const = 0;

    /**
     * @brief Deregister ble adapter observer.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool DeregisterBleAdapterObserver(IAdapterBleObserver &observer) const = 0;

    /**
     * @brief Register peripheral callback.
     *
     * @param callback Class IBlePeripheralCallback pointer to register callback.
     * @since 6
     */
    virtual void RegisterBlePeripheralCallback(IBlePeripheralCallback &callback) const = 0;

    /**
     * @brief Deregister peripheral callback.
     *
     * @since 6
     */
    virtual void DeregisterBlePeripheralCallback(IBlePeripheralCallback &callback) const = 0;

    /**
     * @brief Get device IO capability.
     *
     * @return Returns device IO capability.
     * @since 6
     */
    virtual int GetIoCapability() const = 0;

    /**
     * @brief Set device IO capability.
     *
     * @param ioCapability IO capability.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    virtual bool SetIoCapability(int ioCapability) const = 0;

    /**
     * @brief Get max advertising data length.
     *
     * @return Returns max advertising data length.
     * @since 6
     */
    virtual int GetBleMaxAdvertisingDataLength() const = 0;

    /**
     * @brief Get peer device address type.
     *
     * @param device Remote device.
     * @return Returns peer device address type.
     * @since 6
     */
    virtual int GetPeerDeviceAddrType(const RawAddress &device) const = 0;

    /**
     * @brief Check if device is discovering.
     *
     * @return Returns <b>true</b> if device is discovering;
     *         returns <b>false</b> if device is not discovering.
     * @since 6
     */
    virtual bool IsBtDiscovering() const = 0;

    /**
     * @brief Get advertiser id.
     *
     * @return Returns advertiser handle.
     * @since 6
     */
    virtual uint8_t GetAdvertiserHandle() const = 0;

    /**
     * @brief Get advertiser status.
     *
     * @return Returns advertiser status.
     * @since 6
     */
    virtual int GetAdvertisingStatus() const = 0;

    /**
     * @brief Get Link Layer Privacy Supported.
     *
     * @return True:supported; False:not supported.
     * @since 6
     */
    virtual bool IsLlPrivacySupported() const = 0;

    /**
     * @brief Add characteristic value.
     *
     * @param adtype Type of the field.
     * @param data Field data.
     * @since 6
     */
    virtual void AddCharacteristicValue(uint8_t adtype, const std::string &data) const = 0;

    /**
     * @brief Start advertising.
     *
     * @param settings Advertise settings.
     * @param advData Advertise data.
     * @param scanResponse Scan response data
     * @param advHandle Advertise handle
     * @since 6
     */
    virtual void StartAdvertising(const BleAdvertiserSettingsImpl &settings, const BleAdvertiserDataImpl &advData,
        const BleAdvertiserDataImpl &scanResponse, uint8_t advHandle) const = 0;

    /**
     * @brief Stop advertising.
     *
     * @param advHandle Advertise handle
     * @since 6
     */
    virtual void StopAdvertising(uint8_t advHandle) const = 0;

    /**
     * @brief Cleans up advertisers.
     *
     * @since 6
     */
    virtual void Close(uint8_t advHandle) const = 0;

    /**
     * @brief Start scan
     *
     * @since 6
     */
    virtual void StartScan() const = 0;

    /**
     * @brief Start scan
     *
     * @param setting Scan setting.
     * @since 6
     */
    virtual void StartScan(const BleScanSettingsImpl &setting) const = 0;

    /**
     * @brief Stop scan.
     *
     * @since 6
     */
    virtual void StopScan() const = 0;

    /**
     * @brief Config scan filter
     *
     * @param filter Scan filter.
     * @return client id
     */
    virtual int ConfigScanFilter(const int clientId, const std::vector<BleScanFilterImpl> &filters) = 0;

    /**
     * @brief Remove scan filter
     *
     * @param clientId client id.
     */
    virtual void RemoveScanFilter(const int clientId) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_ADAPTER_BLE_H
