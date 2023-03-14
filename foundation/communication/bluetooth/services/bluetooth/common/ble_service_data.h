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
 * @brief Defines BLE advertiser, peripheral deviceand central manager functions,
 *  including scan settings and filters, advertising settings and data etc.
 *
 * @since 6
 */

/**
 * @file ble_data.cpp
 *
 * @brief Ble data class.
 *
 * @since 6
 */

#ifndef BLE_SERVICE_DATA_H
#define BLE_SERVICE_DATA_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>
#include "__functional_base"
#include "bt_def.h"
#include "bt_uuid.h"
#include "cstdint"
#include "iosfwd"
#include "raw_address.h"
#include "string"
#include "utility"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents scan settings.
 *
 * @since 6
 */
class BleScanSettingsImpl {
public:
    /**
     * @brief A constructor used to create a <b>BleScanSettingsInternal</b> instance.
     *
     * @since 6
     */
    BleScanSettingsImpl(){};

    /**
     * @brief A destructor used to delete the <b>BleScanSettingsInternal</b> instance.
     *
     * @since 6
     */
    ~BleScanSettingsImpl(){};

    /**
     * @brief Set report delay time.
     *
     * @param reportDelayMillis Report delay time.
     * @since 6
     */
    void SetReportDelay(long reportDelayMillis = 0);

    /**
     * @brief Get report delay time.
     *
     * @return Report delay time.
     * @since 6
     */
    long GetReportDelayMillisValue() const;

    /**
     * @brief Set scan mode.
     *
     * @param scanMode Scan mode.
     * @since 6
     */
    void SetScanMode(int scanMode);

    /**
     * @brief Get scan mode.
     *
     * @return Scan mode.
     * @since 6
     */
    int GetScanMode() const;

    /**
     * @brief Set legacy flag.
     *
     * @param legacy Legacy flag.
     * @since 6
     */
    void SetLegacy(bool legacy);

    /**
     * @brief Get legacy flag.
     *
     * @return Legacy flag.
     * @since 6
     */
    bool GetLegacy() const;

    /**
     * @brief Set phy value.
     *
     * @param phy Phy value.
     * @since 6
     */
    void SetPhy(int phy);

    /**
     * @brief Get phy value.
     *
     * @return Phy value.
     * @since 6
     */
    int GetPhy() const;

private:
    /// delay millistime
    long reportDelayMillis_ = 0;
    int scanMode_ = SCAN_MODE_LOW_POWER;
    bool legacy_ = true;
    int phy_ = PHY_LE_ALL_SUPPORTED;
};

/**
 * @brief Represents advertise settings.
 *
 * @since 6
 */
class BleAdvertiserSettingsImpl {
public:
    /**
     * @brief A constructor used to create a <b>BleAdvertiseSettingsInternal</b> instance.
     *
     * @since 6
     */
    BleAdvertiserSettingsImpl(){};

    /**
     * @brief A destructor used to delete the <b>BleAdvertiseSettingsInternal</b> instance.
     *
     * @since 6
     */
    ~BleAdvertiserSettingsImpl(){};

    /**
     * @brief Check if device service is connectable.
     *
     * @return Returns <b>true</b> if device service is connectable;
     *         returns <b>false</b> if device service is not connectable.
     * @since 6
     */
    bool IsConnectable() const;

    /**
     * @brief Set connectable.
     *
     * @param connectable Whether it is connectable.
     * @since 6
     */
    void SetConnectable(bool connectable);
    /**
     * @brief Check if advertiser is legacy mode.
     *
     * @return Returns <b>true</b> if advertiser is legacy mode;
     *         returns <b>false</b> if advertiser is not legacy mode.
     * @since 6
     */
    bool IsLegacyMode() const;

    /**
     * @brief Set legacyMode.
     *
     * @param connectable Whether it is legacyMode.
     * @since 6
     */
    void SetLegacyMode(bool legacyMode);

    /**
     * @brief Get advertise interval.
     *
     * @return Returns advertise interval.
     * @since 6
     */
    int GetInterval() const;

    /**
     * @brief Set advertise interval.
     *
     * @param interval Advertise interval.
     * @since 6
     */
    void SetInterval(int interval = BLE_ADV_DEFAULT_INTERVAL);

    /**
     * @brief Get advertiser Tx power.
     *
     * @return Returns advertiser Tx power.
     * @since 6
     */
    int GetTxPower() const;

    /**
     * @brief Set advertiser Tx power.
     *
     * @param txPower Advertiser Tx power.
     * @since 6
     */
    int SetTxPower(int txPower);

    /**
     * @brief Get primary phy.
     *
     * @return Returns primary phy.
     * @since 6
     */
    int GetPrimaryPhy() const;

    /**
     * @brief Set primary phy.
     *
     * @param primaryPhy Primary phy.
     * @since 6
     */
    void SetPrimaryPhy(int primaryPhy);

    /**
     * @brief Get second phy.
     *
     * @return Returns primary phy.
     * @since 6
     */
    int GetSecondaryPhy() const;

    /**
     * @brief Set second phy.
     *
     * @param secondaryPhy Second phy.
     * @since 6
     */
    void SetSecondaryPhy(int secondaryPhy);

private:
    /// Advertising interval.
    int interval_ = BLE_ADV_DEFAULT_INTERVAL;
    /// Advertising connectable.
    bool connectable_ = true;
    /// Advertising txPower.
    int txPower_ = BLE_ADV_TX_POWER_HIGH_VALUE;
    /// Advertising legacyMode.
    bool legacyMode_ = true;
    /// Advertising primaryPhy.
    int primaryPhy_ = BLE_ADVERTISEMENT_PHY_1M;
    /// Advertising secondaryPhy.
    int secondaryPhy_ = BLE_ADVERTISEMENT_PHY_1M;
};

/**
 * @brief Represents scan filter.
 *
 */
class BleScanFilterImpl {
public:
    BleScanFilterImpl() {}
    ~BleScanFilterImpl() {}

    /**
     * @brief Set device id.
     *
     * @param deviceId filter device id.
     */
    void SetDeviceId(std::string deviceId);

    /**
     * @brief Get filter device id.
     *
     * @return Returns filter device id.
     */
    std::string GetDeviceId() const;

    void SetName(std::string name);

    std::string GetName() const;

    void SetServiceUuid(const Uuid &serviceUuid);

    bool HasServiceUuid() const;

    Uuid GetServiceUuid() const;

    void SetServiceUuidMask(const Uuid &serviceUuidMask);

    bool HasServiceUuidMask() const;

    Uuid GetServiceUuidMask() const;

    void SetServiceSolicitationUuid(const Uuid &serviceSolicitationUuid);

    bool HasSolicitationUuid() const;

    Uuid GetServiceSolicitationUuid() const;

    void SetServiceSolicitationUuidMask(const Uuid &serviceSolicitationUuidMask);

    bool HasSolicitationUuidMask() const;

    Uuid GetServiceSolicitationUuidMask() const;

    void SetServiceData(std::vector<uint8_t> serviceData);

    std::vector<uint8_t> GetServiceData() const;

    void SetServiceDataMask(std::vector<uint8_t> serviceDataMask);

    std::vector<uint8_t> GetServiceDataMask() const;

    void SetManufacturerId(uint16_t manufacturerId);

    uint16_t GetManufacturerId() const;

    void SetManufactureData(std::vector<uint8_t> manufactureData);

    std::vector<uint8_t> GetManufactureData() const;

    void SetManufactureDataMask(std::vector<uint8_t> manufactureDataMask);

    std::vector<uint8_t> GetManufactureDataMask() const;

    void SetClientId(int clientId);

    int GetClientId() const;

    void SetFiltIndex(uint8_t filtIndex);

    uint8_t GetFiltIndex() const;

    void SetFilterAction(uint8_t action);

    uint8_t GetFilterAction() const;

private:
    std::string deviceId_;
    std::string name_;

    Uuid serviceUuid_;
    Uuid serviceUuidMask_;
    Uuid serviceSolicitationUuid_;
    Uuid serviceSolicitationUuidMask_;
    bool hasServiceUuid_ = false;
    bool hasServiceUuidMask_ = false;
    bool hasSolicitationUuid_ = false;
    bool hasSolicitationUuidMask_ = false;

    std::vector<uint8_t> serviceData_;
    std::vector<uint8_t> serviceDataMask_;

    uint16_t manufacturerId_ = 0;
    std::vector<uint8_t> manufactureData_;
    std::vector<uint8_t> manufactureDataMask_;

    int clientId_ = 0;
    uint8_t filtIndex_ = 0;
    uint8_t action_ = -1;
};

/**
 * @brief Represents advertise data.
 *
 * @since 6
 */
class BleAdvertiserDataImpl {
public:
    /**
     * @brief A constructor used to create a <b>BleAdvertiseDataInternal</b> instance.
     *
     * @since 6
     */
    BleAdvertiserDataImpl();

    /**
     * @brief A destructor used to delete the <b>BleAdvertiseDataInternal</b> instance.
     *
     * @since 6
     */
    ~BleAdvertiserDataImpl()
    {}

    /**
     * @brief Add manufacture data.
     *
     * @param manufacturerId Manufacture Id which addad data.
     * @param data Manufacture data
     * @since 6
     */
    int AddManufacturerData(uint16_t manufacturerId, const std::string &data);

    /**
     * @brief Add service data.
     *
     * @param uuid Uuid of service data.
     * @param data Service data.
     * @since 6
     */
    void AddServiceData(const Uuid &uuid, const std::string &data);

    /**
     * @brief Add characteristic value.
     *
     * @param adtype Type of the field.
     * @param data Field data.
     * @since 6
     */
    void AddCharacteristicValue(uint8_t adtype, const std::string &data);

    /**
     * @brief Add service uuid.
     *
     * @param uuid Service uuid.
     * @since 6
     */
    void AddServiceUuid(const Uuid &uuid);

    /**
     * @brief Set device appearance.
     *
     * @param appearance Device appearance.
     * @since 6
     */
    void SetAppearance(uint16_t appearance);

    /**
     * @brief Set complete services.
     *
     * @param uuid Service uuid.
     * @since 6
     */
    void SetCompleteServices(const Uuid &uuid);

    /**
     * @brief Set advertiser flag.
     *
     * @param flag Advertiser flag.
     * @since 6
     */
    void SetFlags(uint8_t flag);

    /**
     * @brief Get advertiser flag.
     *
     * @return flag Advertiser flag.
     * @since 6
     */
    uint8_t GetFlags() const;

    /**
     * @brief Set manufacture data.
     *
     * @param data Manufacture data.
     * @since 6
     */
    void SetManufacturerData(const std::string &data);

    /**
     * @brief Set device name.
     *
     * @param name Device name.
     * @since 6
     */
    void SetDeviceName(const std::string &name);

    /**
     * @brief Set Tx power level.
     *
     * @param txPowerLevel Tx power level.
     * @since 6
     */
    void SetTxPowerLevel(uint8_t txPowerLevel);

    /**
     * @brief Add service data.
     *
     * @param data Service data.
     * @since 6
     */
    void AddData(std::string data);

    /**
     * @brief Set advertiser data packet.
     *
     * @return Returns advertiser data packet.
     * @since 1.0
     * @version 1.0
     */
    void SetPayload(const std::string &payload);
    /**
     * @brief Get advertiser data packet.
     *
     * @return Returns advertiser data packet.
     * @since 6
     */
    std::string GetPayload() const;

private:
    /// Advertiser data packet
    std::string payload_ {};
    uint8_t advFlag_ {};

    /**
     * @brief Set advertiser data long name.
     *
     * @param name Bluetooth device name.
     * @since 6
     */
    void SetLongName(const std::string &name);

    /**
     * @brief Set advertiser data short name
     *
     * @param name Bluetooth device name.
     * @since 6
     */
    void SetShortName(const std::string &name);
};

/**
 * @brief Parse advertisement parameter .
 *
 * @since 6
 */
struct BlePeripheralDeviceParseAdvData {
    uint8_t *payload = nullptr;
    size_t length = 0;
};
/**
 * @brief Represents peripheral device.
 *
 * @since 6
 */
class BlePeripheralDevice {
public:
    /**
     * @brief A constructor used to create a <b>BlePeripheralDevice</b> instance.
     *
     * @since 6
     */
    BlePeripheralDevice();

    /**
     * @brief A destructor used to delete the <b>BlePeripheralDevice</b> instance.
     *
     * @since 6
     */
    ~BlePeripheralDevice();

    /**
     * @brief Get device address.
     *
     * @return Returns device address.
     * @since 6
     */
    RawAddress GetRawAddress() const;

    /**
     * @brief Get device Appearance.
     *
     * @return Returns device Appearance.
     * @since 6
     */
    uint16_t GetAppearance() const;

    /**
     * @brief Get Manufacturer Data.
     *
     * @return Returns Manufacturer Data.
     * @since 6
     */
    std::map<uint16_t, std::string> GetManufacturerData() const;

    /**
     * @brief Get device Name.
     *
     * @return Returns device Name.
     * @since 6
     */
    std::string GetName() const;

    /**
     * @brief Get device RSSI.
     *
     * @return Returns device RSSI.
     * @since 6
     */
    int8_t GetRSSI() const;

    /**
     * @brief Get service Data.
     *
     * @return Returns service data.
     * @since 6
     */
    std::vector<std::string> GetServiceData() const;

    /**
     * @brief Get Service Data.
     *
     * @param index Service data index.
     * @return Returns service data.
     * @since 6
     */
    std::string GetServiceData(int index) const;

    /**
     * @brief Get service data UUID.
     *
     * @return Returns service data UUID.
     * @since 6
     */
    std::vector<Uuid> GetServiceDataUUID() const;

    /**
     * @brief Get service data UUID.
     *
     * @param index Service data index.
     * @return Returns service data UUID.
     * @since 6
     */
    Uuid GetServiceDataUUID(int index) const;

    /**
     * @brief Get serviceU UUID.
     *
     * @return Returns service UUID.
     * @since 6
     */
    std::vector<Uuid> GetServiceUUID() const;

    /**
     * @brief Get service UUID.
     *
     * @param index Service UUID index.
     * @return Return service UUID.
     * @since 6
     */
    Uuid GetServiceUUID(int index) const;

    /**
     * @brief Get advertiser data packet.
     *
     * @return Returns advertiser data packet.
     * @since 6
     */
    uint8_t *GetPayload() const;

    /**
     * @brief Get advertising packet length.
     *
     * @return Returns advertising packet length.
     * @since 6
     */
    size_t GetPayloadLen() const;

    /**
     * @brief Get address type.
     *
     * @return Returns address type.
     * @since 6
     */
    int GetAddressType() const;

    /**
     * @brief Set address type.
     *
     * @param type Address type.
     * @since 6
     */
    void SetAddressType(int type);
    /**
     * @brief Check if include manufacture data.
     *
     * @return Returns <b>true</b> if include manufacture data;
     *         returns <b>false</b> if do not include manufacture data.
     * @since 6
     */
    bool IsManufacturerData() const;

    /**
     * @brief Check if include device rssi.
     *
     * @return Returns <b>true</b> if include device rssi;
     *         returns <b>false</b> if do not include device rssi.
     * @since 6
     */
    bool IsRSSI() const;

    /**
     * @brief Check if include service data.
     *
     * @return Returns <b>true</b> if include service data;
     *         returns <b>false</b> if do not include service data.
     * @since 6
     */
    bool IsServiceData() const;

    /**
     * @brief Check if include service UUID.
     *
     * @return Returns <b>true</b> if include service UUID;
     *         returns <b>false</b> if do not include service UUID.
     * @since 6
     */
    bool IsServiceUUID() const;

    /**
     * @brief set device address.
     *
     * @param address device address.
     * @since 6
     */
    void SetAddress(const RawAddress &address);

    /**
     * @brief set rssi value.
     *
     * @param rssi rssi value.
     * @since 6
     */
    void SetRSSI(int8_t rssi);

    /**
     * @brief set rssi value.
     *
     * @param [in] rssi value.
     */
    bool IsConnectable() const;

    /**
     * @brief set rssi value.
     *
     * @param [in] rssi value.
     */
    void SetConnectable(bool connectable);

    /**
     * @brief Parse advertisement data.
     *
     * @param payload Advertisement packet.
     * @param totalLen Advertisement packet total len.
     * @since 6
     */
    void ParseAdvertiserment(BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Build advertisement data.
     *
     * @param advType Advertisement packet type.
     * @param payload Advertisement packet.
     * @param length Advertisement packet len.
     *
     * @since 6
     */
    void BuildAdvertiserData(uint8_t advType, BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Set service uuid 16 bit data.
     *
     * @param payload Advertisement packet.
     * @param total_len Advertisement packet len.
     * @since 6
     */
    void SetServiceUUID16Bits(BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Set service uuid 32 bit data.
     *
     * @param payload Advertisement packet.
     * @param total_len Advertisement packet len.
     * @since 6
     */
    void SetServiceUUID32Bits(BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Set service uuid 128 bit data.
     *
     * @param payload Advertisement packet.
     * @param total_len Advertisement packet len.
     * @since 6
     */
    void SetServiceUUID128Bits(const BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Set service data uuid 16 bit data.
     *
     * @param payload Advertisement packet.
     * @param total_len Advertisement packet len.
     * @since 6
     */
    void SetServiceDataUUID16Bits(BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Set service data uuid 32 bit data.
     *
     * @param payload Advertisement packet.
     * @param total_len Advertisement packet len.
     * @since 6
     */
    void SetServiceDataUUID32Bits(BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Set service data uuid 128 bit data.
     *
     * @param payload Advertisement packet.
     * @param total_len Advertisement packet len.
     * @since 6
     */
    void SetServiceDataUUID128Bits(BlePeripheralDeviceParseAdvData &parseAdvData);

    /**
     * @brief Set device name.
     *
     * @param name Device name.
     * @since 6
     */
    void SetName(const std::string &name);

    /**
     * @brief Set device roles.
     *
     * @param roles Device roles.
     * @since 6
     */
    void SetRoles(uint8_t roles);

    /**
     * @brief Set bonded from local.
     *
     * @param flag Advertiser flag.
     * @since 6
     */
    void SetBondedFromLocal(bool flag);

    /**
     * @brief Set acl connect state.
     *
     * @param connectState Acl connect state.
     * @since 6
     */
    void SetAclConnectState(int connectState);

    /**
     * @brief Set acl connection handle.
     *
     * @param handle Acl connection handle.
     * @since 6
     */
    void SetConnectionHandle(int handle);

    /**
     * @brief Check if device acl connected.
     *
     * @return Returns <b>true</b> if device acl connected;
     *         returns <b>false</b> if device does not acl connect.
     * @since 6
     */
    bool IsAclConnected() const;

    /**
     * @brief Check if device acl Encrypted.
     *
     * @return Returns <b>true</b> if device acl Encrypted;
     *         returns <b>false</b> if device does not acl Encrypt.
     * @since 6
     */
    bool IsAclEncrypted() const;

    /**
     * @brief Check if device was bonded from local.
     *
     * @return Returns <b>true</b> if device was bonded from local;
     *         returns <b>false</b> if device was not bonded from local.
     * @since 6
     */
    bool IsBondedFromLocal() const;

    /**
     * @brief Get acl connection handle.
     *
     * @return Returns acl connection handle;
     * @since 6
     */
    int GetConnectionHandle() const;

    /**
     * @brief Get device type.
     *
     * @return Returns device type.
     * @since 6
     */
    uint8_t GetDeviceType() const;

    /**
     * @brief Get advertising flag.
     *
     * @return Returns advertising flag.
     * @since 6
     */
    uint8_t GetAdFlag() const;

    /**
     * @brief Get paired status.
     *
     * @return Returns paired status.
     * @since 6
     */
    uint8_t GetPairedStatus() const;

    /**
     * @brief Set paired status.
     *
     * @param status Paired status.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool SetPairedStatus(uint8_t status);

    /**
     * @brief Set alias name.
     *
     * @param name Device alias name.
     * @since 6
     */
    void SetAliasName(const std::string &name);

    /**
     * @brief Get alias name.
     *
     * @return Returns alias name.
     * @since 6
     */
    std::string GetAliasName() const;

    /**
     * @brief Set IO capability.
     *
     * @param io IO capability
     * @since 6
     */
    void SetIoCapability(uint8_t io);

    /**
     * @brief Get IO capability.
     *
     * @return Returns IO capability.
     * @since 6
     */
    uint8_t GetIoCapability() const;

    /**
     * @brief Set device type.
     *
     * @param type Device type.
     * @since 6
     */
    void SetDeviceType(uint8_t type);

    /**
     * @brief Set manufacturer data.
     *
     * @param manufacturerData Manufacturer data.
     * @since 6
     */
    void SetManufacturerData(std::string manufacturerData);

private:
    /**
     * @brief Set advertising flag.
     *
     * @param adFlag Advertising flag.
     * @since 6
     */
    void SetAdFlag(uint8_t adFlag);

    /**
     * @brief Set appearance.
     *
     * @param appearance Appearance.
     * @since 6
     */
    void SetAppearance(uint16_t appearance);

    /**
     * @brief Set service data UUID.
     *
     * @param uuid Service data UUID.
     * @since 6
     */
    void SetServiceDataUUID(Uuid uuid, std::string data);

    /**
     * @brief Set service  UUID.
     *
     * @param serviceUUID Service  UUID.
     * @since 6
     */
    void SetServiceUUID(Uuid serviceUUID);
    /**
     * @brief Set TX power.
     *
     * @param txPower TX power.
     * @since 6
     */
    void SetTXPower(int8_t txPower);

    /// include appearance?
    bool isAppearance_ = false;
    /// include Manufacturer Data?
    bool isManufacturerData_ = false;
    /// include device name?
    bool isName_ = false;
    /// include rssi value?
    bool isRSSI_ = false;
    /// include service data?
    bool isServiceData_ = false;
    /// include service uuid?
    bool isServiceUUID_ = false;
    /// include tx power?
    bool isTXPower_ = false;
    /// peer roles
    uint8_t roles_ = 0;
    /// device address
    RawAddress address_ = RawAddress("00:00:00:00:00:00");
    /// device address
    RawAddress currentAddr_ = RawAddress("00:00:00:00:00:00");
    /// advertising flag
    uint8_t adFlag_ = 0;
    /// appearance
    uint16_t appearance_ = 0;
    /// manufacturer Data
    std::map<uint16_t, std::string> manufacturerData_ {};
    /// device name
    std::string name_ {};
    /// rssi value
    int8_t rssi_ = 0;
    /// service uuid
    std::vector<Uuid> serviceUUIDs_ {};
    /// tx power
    int8_t txPower_ {};
    /// service data
    std::vector<std::string> serviceData_ {};
    /// service data uuid
    std::vector<Uuid> serviceDataUUIDs_ {};
    /// address type
    uint8_t addressType_ = BLE_ADDR_TYPE_RANDOM;
    int aclConnected_ = 0;
    int connectionHandle_ = 0;
    bool bondFlag_ = false;
    uint8_t pairState_ {};
    uint8_t ioCapability_ {};
    std::string aliasName_ {};
    bool connectable_ = true;
    uint8_t* payload_ {};
    size_t payloadLen_ = 0;
};

/**
 * @brief Represents scan result.
 *
 * @since 6
 */
class BleScanResultImpl {
public:
    /**
     * @brief A constructor used to create a <b>BleScanResultInternal</b> instance.
     *
     * @since 6
     */
    BleScanResultImpl() : peripheralDevice_()
    {}

    /**
     * @brief A destructor used to delete the <b>BleScanResultInternal</b> instance.
     *
     * @since 6
     */
    ~BleScanResultImpl()
    {}

    /**
     * @brief Get peripheral device.
     *
     * @return Returns peripheral device pointer.
     * @since 6
     */
    BlePeripheralDevice GetPeripheralDevice() const;

    /**
     * @brief Set peripheral device.
     *
     * @param dev Peripheral device.
     * @since 6
     */
    void SetPeripheralDevice(const BlePeripheralDevice &dev);

    /**
     * @brief Get service uuids.
     *
     * @return Returns service uuids.
     * @since 6
     */
    std::vector<Uuid> GetServiceUuids() const
    {
        return serviceUuids_;
    }

    /**
     * @brief Get manufacture data.
     *
     * @return Returns manufacture data.
     * @since 6
     */
    std::map<uint16_t, std::string> GetManufacturerData() const
    {
        return manufacturerSpecificData_;
    }

    /**
     * @brief Get service data.
     *
     * @return Returns service data.
     * @since 6
     */
    std::map<Uuid, std::string> GetServiceData() const
    {
        return serviceData_;
    }

    /**
     * @brief Get peer device rssi.
     *
     * @return Returns peer device rssi.
     * @since 6
     */
    int8_t GetRssi() const
    {
        return rssi_;
    }

    /**
     * @brief Check if device is connectable.
     *
     * @return Returns <b>true</b> if device is connectable;
     *         returns <b>false</b> if device is not connectable.
     * @since 6
     */
    bool IsConnectable() const
    {
        return connectable_;
    }

    /**
     * @brief Get advertiser flag.
     *
     * @return Returns advertiser flag.
     * @since 6
     */
    uint8_t GetAdvertiseFlag() const
    {
        return advertiseFlag_;
    }

    /**
     * @brief Add manufacture data.
     *
     * @param manufacturerId Manufacture Id which addad data.
     * @since 6
     */
    void AddManufacturerData(uint16_t manufacturerId, std::string data)
    {
        manufacturerSpecificData_.insert(std::make_pair(manufacturerId, data));
    }

    /**
     * @brief Add service data.
     *
     * @param uuid Uuid of service data.
     * @param serviceData Service data.
     * @since 6
     */
    void AddServiceData(Uuid uuid, std::string serviceData)
    {
        serviceData_.insert(std::make_pair(uuid, serviceData));
    }

    /**
     * @brief Add service uuid.
     *
     * @param serviceUuid Service uuid.
     * @since 6
     */
    void AddServiceUuid(const Uuid &serviceUuid)
    {
        serviceUuids_.push_back(serviceUuid);
    }

    /**
     * @brief Set peripheral device.
     *
     * @param device Remote device.
     * @since 6
     */
    void SetPeripheralDevice(const RawAddress &device)
    {
        addr_ = device;
    }

    /**
     * @brief Set peer device rssi.
     *
     * @param rssi Peer device rssi.
     * @since 6
     */
    void SetRssi(int8_t rssi)
    {
        rssi_ = rssi;
    }

    /**
     * @brief Set connectable.
     *
     * @param connectable Whether it is connectable.
     * @since 6
     */
    void SetConnectable(bool connectable)
    {
        connectable_ = connectable;
    }

    /**
     * @brief Set advertiser flag.
     *
     * @param flag Advertiser flag.
     * @since 6
     */
    void SetAdvertiseFlag(uint8_t flag)
    {
        advertiseFlag_ = flag;
    }

private:
    /// scan device results
    BlePeripheralDevice peripheralDevice_;
    std::vector<Uuid> serviceUuids_ {};
    std::map<uint16_t, std::string> manufacturerSpecificData_ {};
    std::map<Uuid, std::string> serviceData_ {};
    RawAddress addr_ {};
    int8_t rssi_ {};
    bool connectable_ {};
    uint8_t advertiseFlag_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  /// BLE_SERVICE_DATA_H
