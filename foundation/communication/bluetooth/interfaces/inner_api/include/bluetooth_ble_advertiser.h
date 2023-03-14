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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines advertiser, including avertise data and callbacks, and advertiser functions.
 *
 * @since 6
 */

/**
 * @file bluetooth_ble_advertiser.h
 *
 * @brief Advertiser common functions.
 *
 * @since 6
 */

#ifndef BLUETOOTH_BLE_ADVERTISER_H
#define BLUETOOTH_BLE_ADVERTISER_H

#include "bluetooth_def.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Represents advertise data.
 *
 * @since 6
 */
class BLUETOOTH_API BleAdvertiserData {
public:
    /**
     * @brief A constructor used to create a <b>BleAdvertiserData</b> instance.
     *
     * @since 6
     */
    BleAdvertiserData();

    /**
     * @brief A destructor used to delete the <b>BleAdvertiserData</b> instance.
     *
     * @since 6
     */
    ~BleAdvertiserData();

    /**
     * @brief Add manufacture data.
     *
     * @param manufacturerId Manufacture Id which addad data.
     * @param data Manufacture data
     * @since 6
     */
    void AddManufacturerData(uint16_t manufacturerId, const std::string &data);

    /**
     * @brief Add service data.
     *
     * @param uuid Uuid of service data.
     * @param serviceData Service data.
     * @since 6
     */
    void AddServiceData(const ParcelUuid &uuid, const std::string &serviceData);

    /**
     * @brief Add service uuid.
     *
     * @param serviceUuid Service uuid.
     * @since 6
     */
    void AddServiceUuid(const ParcelUuid &serviceUuid);

    /**
     * @brief Get manufacture data.
     *
     * @return Returns manufacture data.
     * @since 6
     */
    std::map<uint16_t, std::string> GetManufacturerData() const;

    /**
     * @brief Get service data.
     *
     * @return Returns service data.
     * @since 6
     */
    std::map<ParcelUuid, std::string> GetServiceData() const;

    /**
     * @brief Get service uuids.
     *
     * @return Returns service uuids.
     * @since 6
     */
    std::vector<ParcelUuid> GetServiceUuids() const;

    /**
     * @brief Get advertiser flag.
     *
     * @return Returns advertiser flag.
     * @since 6
     */
    uint8_t GetAdvFlag() const;

    /**
     * @brief Set advertiser flag.
     *
     * @param flag Advertiser flag.
     * @return Returns error flag.
     * @since 6
     */
    void SetAdvFlag(uint8_t flag);

private:
    std::vector<ParcelUuid> serviceUuids_{};
    std::map<uint16_t, std::string> manufacturerSpecificData_{};
    std::map<ParcelUuid, std::string> serviceData_{};
    uint8_t advFlag_ = BLE_ADV_FLAG_GEN_DISC;
};

/**
 * @brief Represents advertise settings.
 *
 * @since 6
 */
class BLUETOOTH_API BleAdvertiserSettings {
public:
    /**
     * @brief A constructor used to create a <b>BleAdvertiseSettings</b> instance.
     *
     * @since 6
     */
    BleAdvertiserSettings();

    /**
     * @brief A destructor used to delete the <b>BleAdvertiseSettings</b> instance.
     *
     * @since 6
     */
    ~BleAdvertiserSettings();

    /**
     * @brief Check if device service is connctable.
     *
     * @return Returns <b>true</b> if device service is connctable;
     *         returns <b>false</b> if device service is not connctable.
     * @since 6
     */
    bool IsConnectable() const;

    /**
     * @brief Check if advertiser is legacy mode.
     *
     * @return Returns <b>true</b> if advertiser is legacy mode;
     *         returns <b>false</b> if advertiser is not legacy mode.
     * @since 6
     */
    bool IsLegacyMode() const;

    /**
     * @brief Set connectable.
     *
     * @param connectable Whether it is connectable.
     * @since 6
     */
    void SetConnectable(bool connectable);

    /**
     * @brief Set legacyMode.
     *
     * @param legacyMode Whether it is legacyMode.
     * @since 6
     */
    void SetLegacyMode(bool legacyMode);

    /**
     * @brief Get advertise interval.
     *
     * @return Returns advertise interval.
     * @since 6
     */
    uint16_t GetInterval() const;

    /**
     * @brief Get Tx power.
     *
     * @return Returns Tx power.
     * @since 6
     */
    uint8_t GetTxPower() const;

    /**
     * @brief Set advertise interval.
     *
     * @param interval Advertise interval.
     * @since 6
     */
    void SetInterval(uint16_t interval);

    /**
     * @brief Set Tx power.
     *
     * @param txPower Tx power.
     * @since 6
     */
    void SetTxPower(uint8_t txPower);

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
    bool connectable_ = true;
    bool legacyMode_ = true;
    uint16_t interval_ = BLE_ADV_DEFAULT_INTERVAL;
    uint8_t txPower_ = BLE_ADV_TX_POWER_LOW;
    int primaryPhy_ = BLE_ADVERTISEMENT_PHY_1M;
    int secondaryPhy_ = BLE_ADVERTISEMENT_PHY_1M;
};

/**
 * @brief Represents advertise callback.
 *
 * @since 6
 */
class BleAdvertiseCallback {
public:
    /**
     * @brief A destructor used to delete the <b>BleAdvertiseCallback</b> instance.
     *
     * @since 6
     */
    virtual ~BleAdvertiseCallback() = default;

    /**
     * @brief Start advertising result event callback.
     *
     * @param result Start advertising result.
     * @since 6
     */
    virtual void OnStartResultEvent(int result) = 0;
};

/**
 * @brief Represents advertiser.
 *
 * @since 6
 */
class BLUETOOTH_API BleAdvertiser {
public:
    /**
     * @brief A constructor used to create a <b>BleAdvertiser</b> instance.
     *
     * @since 6
     */
    explicit BleAdvertiser();

    /**
     * @brief A destructor used to delete the <b>BleAdvertiser</b> instance.
     *
     * @since 6
     */
    ~BleAdvertiser();

    /**
     * @brief Start advertising.
     *
     * @param settings Advertise settings.
     * @param advData Advertise data.
     * @param scanResponse Scan response.
     * @param callback Advertise callback.
     * @since 6
     */
    void StartAdvertising(const BleAdvertiserSettings &settings, const BleAdvertiserData &advData,
        const BleAdvertiserData &scanResponse, BleAdvertiseCallback &callback);

    /**
     * @brief Start advertising.
     *
     * @param settings Advertise settings.
     * @param advData Advertise data.
     * @param scanResponse Scan response.
     * @param callback Advertise callback.
     * @since 6
     */
    void StartAdvertising(const BleAdvertiserSettings &settings, const std::vector<uint8_t> &advData,
        const std::vector<uint8_t> &scanResponse, BleAdvertiseCallback &callback);

    void StopAdvertising(BleAdvertiseCallback &callback);

    /**
     * @brief Cleans up advertisers.
     *
     * @param callback Advertise callback.
     * @since 6
     */
    void Close(BleAdvertiseCallback &callback);

private:
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BleAdvertiser);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif