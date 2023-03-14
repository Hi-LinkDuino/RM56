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
 * @brief Defines ble advertiser, peripheral deviceand central manager functions,
 *  including scan settings and filters, advertise settings and data etc.
 *
 * @since 6
 */

/**
 * @file ble_data.h
 *
 * @brief Ble data class.
 *
 * @since 6
 */

#ifndef BLE_PARCEL_DATA_H
#define BLE_PARCEL_DATA_H

#include <map>
#include <vector>

#include "bt_uuid.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class AdvertiserData {
public:
    /**
     * @brief A constructor used to create a <b>BleAdvertiserData</b> instance.
     *
     * @since 6
     */
    AdvertiserData(){};

    /**
     * @brief A destructor used to delete the <b>BleAdvertiserData</b> instance.
     *
     * @since 6
     */
    ~AdvertiserData(){};

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
     * @brief Get advertiser flag.
     *
     * @return Returns advertiser flag.
     * @since 6
     */
    uint8_t GetAdvFlag() const
    {
        return advFlag_;
    }

    /**
     * @brief Get payload.
     *
     * @return Returns payload.
     * @since 6
     */
    std::string GetPayload() const
    {
        return payload_;
    }

    /**
     * @brief Set advertiser flag.
     *
     * @param flag Advertiser flag.
     * @since 6
     */
    void SetAdvFlag(uint8_t flag)
    {
        advFlag_ = flag;
    }

    /**
     * @brief Set payload data.
     *
     * @param Payload payload.
     * @since 6
     */
    void SetPayload(const std::string &payload)
    {
        payload_ = payload;
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
    void AddServiceData(bluetooth::Uuid uuid, std::string serviceData)
    {
        serviceData_.insert(std::make_pair(uuid, serviceData));
    }

    /**
     * @brief Add service uuid.
     *
     * @param serviceUuid Service uuid.
     * @since 6
     */
    void AddServiceUuid(const bluetooth::Uuid &serviceUuid)
    {
        serviceUuids_.push_back(serviceUuid);
    }

public:
    std::vector<Uuid> serviceUuids_ {};
    std::map<uint16_t, std::string> manufacturerSpecificData_ {};
    std::map<Uuid, std::string> serviceData_ {};
    uint8_t advFlag_ {};
    std::string payload_ = "";
};

/**
 * @brief Represents advertise settings.
 *
 * @since 6
 */
class AdvertiserSettings {
public:
    /**
     * @brief A constructor used to create a <b>BleAdvertiseSettings</b> instance.
     *
     * @since 6
     */
    AdvertiserSettings(){};

    /**
     * @brief A destructor used to delete the <b>BleAdvertiseSettings</b> instance.
     *
     * @since 6
     */
    ~AdvertiserSettings(){};

    /**
     * @brief Check if device service is connectable.
     *
     * @return Returns <b>true</b> if device service is connectable;
     *         returns <b>false</b> if device service is not connectable.
     * @since 6
     */
    bool IsConnectable() const
    {
        return connectable_;
    }

    /**
     * @brief Check if advertiser is legacy mode.
     *
     * @return Returns <b>true</b> if advertiser is legacy mode;
     *         returns <b>false</b> if advertiser is not legacy mode.
     * @since 6
     */
    bool IsLegacyMode() const
    {
        return legacyMode_;
    }

    /**
     * @brief Get advertise interval.
     *
     * @return Returns advertise interval.
     * @since 6
     */
    int GetInterval() const
    {
        return interval_;
    }

    /**
     * @brief Get Tx power.
     *
     * @return Returns Tx power.
     * @since 6
     */
    int GetTxPower() const
    {
        return txPower_;
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
     * @brief Set legacyMode.
     *
     * @param connectable Whether it is legacyMode.
     * @since 6
     */
    void SetLegacyMode(bool legacyMode)
    {
        legacyMode_ = legacyMode;
    }

    /**
     * @brief Set advertise interval.
     *
     * @param interval Advertise interval.
     * @since 6
     */
    void SetInterval(uint16_t interval)
    {
        interval_ = interval;
    }

    /**
     * @brief Set Tx power.
     *
     * @param txPower Tx power.
     * @since 6
     */
    void SetTxPower(uint8_t txPower)
    {
        txPower_ = txPower;
    }

    /**
     * @brief Get primary phy.
     *
     * @return Returns primary phy.
     * @since 6
     */
    int GetPrimaryPhy() const
    {
        return primaryPhy_;
    }

    /**
     * @brief Set primary phy.
     *
     * @param primaryPhy Primary phy.
     * @since 6
     */
    void SetPrimaryPhy(int primaryPhy)
    {
        primaryPhy_ = primaryPhy;
    }

    /**
     * @brief Get second phy.
     *
     * @return Returns primary phy.
     * @since 6
     */
    int GetSecondaryPhy() const
    {
        return secondaryPhy_;
    }

    /**
     * @brief Set second phy.
     *
     * @param secondaryPhy Second phy.
     * @since 6
     */
    void SetSecondaryPhy(int secondaryPhy)
    {
        secondaryPhy_ = secondaryPhy;
    }

public:
    bool connectable_ {};
    bool legacyMode_ {};
    uint16_t interval_ {};
    uint8_t txPower_ {};
    int primaryPhy_ {};
    int secondaryPhy_ {};
};

class ScanResult {
public:
    /**
     * @brief A constructor used to create a <b>BleScanResult</b> instance.
     *
     * @since 6
     */
    ScanResult(){};

    /**
     * @brief A destructor used to delete the <b>BleScanResult</b> instance.
     *
     * @since 6
     */
    ~ScanResult(){};

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
     * @brief Get peripheral device.
     *
     * @return Returns peripheral device pointer.
     * @since 6
     */
    RawAddress &GetPeripheralDevice()
    {
        return addr_;
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

    void SetPayload(const std::string payload)
    {
        payload_ = payload;
    }

    std::string GetPayload() const
    {
        return payload_;
    }

public:
    std::vector<Uuid> serviceUuids_ {};
    std::map<uint16_t, std::string> manufacturerSpecificData_ {};
    std::map<Uuid, std::string> serviceData_ {};
    RawAddress addr_ {};
    int8_t rssi_ {};
    bool connectable_ {};
    uint8_t advertiseFlag_ {};
    std::string payload_ {};
};

/**
 * @brief Represents Scan settings.
 *
 * @since 6
 */
class ScanSettings {
public:
    /**
     * @brief A constructor used to create a <b>BleScanSettings</b> instance.
     *
     * @since 6
     */
    ScanSettings(){};

    /**
     * @brief A destructor used to delete the <b>BleScanSettings</b> instance.
     *
     * @since 6
     */
    ~ScanSettings(){};

    /**
     * @brief Set report delay time.
     *
     * @param reportDelayMillis Report delay time.
     * @since 6
     */
    void SetReportDelay(long reportDelayMillis)
    {
        reportDelayMillis_ = reportDelayMillis;
    }

    /**
     * @brief Get report delay time.
     *
     * @return Returns Report delay time.
     * @since 6
     */
    long GetReportDelayMillisValue() const
    {
        return reportDelayMillis_;
    }

    /**
     * @brief Set scan mode.
     *
     * @param scanMode Scan mode.
     * @return If the scanMode is invalid.
     * @since 6
     */
    void SetScanMode(int scanMode)
    {
        scanMode_ = scanMode;
    }

    /**
     * @brief Get scan mode.
     *
     * @return Scan mode.
     * @since 6
     */
    int GetScanMode() const
    {
        return scanMode_;
    }

    /**
     * @brief Set legacy flag.
     *
     * @param legacy Legacy value.
     * @since 6
     */
    void SetLegacy(bool legacy)
    {
        legacy_ = legacy;
    }

    /**
     * @brief Get legacy flag.
     *
     * @return Legacy flag.
     * @since 6
     */
    bool GetLegacy() const
    {
        return legacy_;
    }

    /**
     * @brief Set phy value.
     *
     * @param phy Phy value.
     * @since 6
     */
    void SetPhy(int phy)
    {
        phy_ = phy;
    }

    /**
     * @brief Get phy value.
     *
     * @return Phy value.
     * @since 6
     */
    int GetPhy() const
    {
        return phy_;
    }

public:
    long reportDelayMillis_ = 0;
    int scanMode_ = 0;
    bool legacy_ = true;
    int phy_ = 255;
};

/**
 * @brief Represents Scan filter.
 *
 */
class ScanFilter {
public:
    /**
     * @brief A constructor used to create a <b>BleScanFilter</b> instance.
     *
     */
    ScanFilter() {}

    /**
     * @brief A destructor used to delete the <b>BleScanFilter</b> instance.
     *
     */
    ~ScanFilter() {}

    /**
     * @brief Set device id.
     *
     * @param deviceId device id.
     */
    void SetDeviceId(std::string deviceId)
    {
        deviceId_ = deviceId;
    }

    /**
     * @brief Get device id.
     *
     * @return Returns device id.
     */
    std::string GetDeviceId() const
    {
        return deviceId_;
    }

    void SetName(std::string name)
    {
        name_ = name;
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetServiceUuid(const Uuid uuid)
    {
        serviceUuid_ = uuid;
        hasServiceUuid_ = true;
    }

    bool HasServiceUuid()
    {
        return hasServiceUuid_;
    }

    Uuid GetServiceUuid() const
    {
        return serviceUuid_;
    }

    void SetServiceUuidMask(const Uuid serviceUuidMask)
    {
        serviceUuidMask_ = serviceUuidMask;
        hasServiceUuidMask_ = true;
    }

    bool HasServiceUuidMask()
    {
        return hasServiceUuidMask_;
    }

    Uuid GetServiceUuidMask() const
    {
        return serviceUuidMask_;
    }

    void SetServiceSolicitationUuid(const Uuid serviceSolicitationUuid)
    {
        serviceSolicitationUuid_ = serviceSolicitationUuid;
        hasSolicitationUuid_ = true;
    }

    bool HasSolicitationUuid()
    {
        return hasSolicitationUuid_;
    }

    Uuid GetServiceSolicitationUuid() const
    {
        return serviceSolicitationUuid_;
    }

    void SetServiceSolicitationUuidMask(const Uuid serviceSolicitationUuidMask)
    {
        serviceSolicitationUuidMask_ = serviceSolicitationUuidMask;
        hasSolicitationUuidMask_ = true;
    }

    bool HasSolicitationUuidMask()
    {
        return hasSolicitationUuidMask_;
    }

    Uuid GetServiceSolicitationUuidMask() const
    {
        return serviceSolicitationUuidMask_;
    }

    void SetServiceData(std::vector<uint8_t> serviceData)
    {
        serviceData_ = serviceData;
    }

    std::vector<uint8_t> GetServiceData() const
    {
        return serviceData_;
    }

    void SetServiceDataMask(std::vector<uint8_t> serviceDataMask)
    {
        serviceDataMask_ = serviceDataMask;
    }

    std::vector<uint8_t> GetServiceDataMask() const
    {
        return serviceDataMask_;
    }

    void SetManufacturerId(uint16_t manufacturerId)
    {
        manufacturerId_ = manufacturerId;
    }

    uint16_t GetManufacturerId() const
    {
        return manufacturerId_;
    }

    void SetManufactureData(std::vector<uint8_t> manufactureData)
    {
        manufactureData_ = manufactureData;
    }

    std::vector<uint8_t> GetManufactureData() const
    {
        return manufactureData_;
    }

    void SetManufactureDataMask(std::vector<uint8_t> manufactureDataMask)
    {
        manufactureDataMask_ = manufactureDataMask;
    }

    std::vector<uint8_t> GetManufactureDataMask() const
    {
        return manufactureDataMask_;
    }

public:
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
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  /// BLE_PARCEL_DATA_H
