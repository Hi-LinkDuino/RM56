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

#include "ble_service_data.h"

#include <algorithm>

#include "array"
#include "map"
#include "vector"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents scan settings.
 *
 * @since 6
 */

void BleScanSettingsImpl::SetReportDelay(long reportDelayMillis)
{
    reportDelayMillis_ = reportDelayMillis;
}

/**
 * @brief Get the report delay time.
 *
 * @return Returns Report delay time.
 * @since 6
 */
long BleScanSettingsImpl::GetReportDelayMillisValue() const
{
    return reportDelayMillis_;
}

void BleScanSettingsImpl::SetScanMode(int scanMode)
{
    scanMode_ = scanMode;
}

int BleScanSettingsImpl::GetScanMode() const
{
    return scanMode_;
}

void BleScanSettingsImpl::SetLegacy(bool legacy)
{
    legacy_ = legacy;
}

bool BleScanSettingsImpl::GetLegacy() const
{
    return legacy_;
}

void BleScanSettingsImpl::SetPhy(int phy)
{
    phy_ = phy;
}

int BleScanSettingsImpl::GetPhy() const
{
    return phy_;
}

void BleScanFilterImpl::SetDeviceId(std::string deviceId)
{
    deviceId_ = deviceId;
}

std::string BleScanFilterImpl::GetDeviceId() const
{
    return deviceId_;
}

void BleScanFilterImpl::SetName(std::string name)
{
    name_ = name;
}

std::string BleScanFilterImpl::GetName() const
{
    return name_;
}

void BleScanFilterImpl::SetServiceUuid(const Uuid &serviceUuid)
{
    serviceUuid_ = serviceUuid;
    hasServiceUuid_ = true;
}

bool BleScanFilterImpl::HasServiceUuid() const
{
    return hasServiceUuid_;
}

Uuid BleScanFilterImpl::GetServiceUuid() const
{
    return serviceUuid_;
}

void BleScanFilterImpl::SetServiceUuidMask(const Uuid &serviceUuidMask)
{
    serviceUuidMask_ = serviceUuidMask;
    hasServiceUuidMask_ = true;
}

bool BleScanFilterImpl::HasServiceUuidMask() const
{
    return hasServiceUuidMask_;
}

Uuid BleScanFilterImpl::GetServiceUuidMask() const
{
    return serviceUuidMask_;
}

void BleScanFilterImpl::SetServiceSolicitationUuid(const Uuid &serviceSolicitationUuid)
{
    serviceSolicitationUuid_ = serviceSolicitationUuid;
    hasSolicitationUuid_ = true;
}

bool BleScanFilterImpl::HasSolicitationUuid() const
{
    return hasSolicitationUuid_;
}

Uuid BleScanFilterImpl::GetServiceSolicitationUuid() const
{
    return serviceSolicitationUuid_;
}

void BleScanFilterImpl::SetServiceSolicitationUuidMask(const Uuid &serviceSolicitationUuidMask)
{
    serviceSolicitationUuidMask_ = serviceSolicitationUuidMask;
    hasSolicitationUuidMask_ = true;
}

bool BleScanFilterImpl::HasSolicitationUuidMask() const
{
    return hasSolicitationUuidMask_;
}

Uuid BleScanFilterImpl::GetServiceSolicitationUuidMask() const
{
    return serviceSolicitationUuidMask_;
}

void BleScanFilterImpl::SetServiceData(std::vector<uint8_t> serviceData)
{
    serviceData_ = serviceData;
}

std::vector<uint8_t> BleScanFilterImpl::GetServiceData() const
{
    return serviceData_;
}

void BleScanFilterImpl::SetServiceDataMask(std::vector<uint8_t> serviceDataMask)
{
    serviceDataMask_ = serviceDataMask;
}

std::vector<uint8_t> BleScanFilterImpl::GetServiceDataMask() const
{
    return serviceDataMask_;
}

void BleScanFilterImpl::SetManufacturerId(uint16_t manufacturerId)
{
    manufacturerId_ = manufacturerId;
}

uint16_t BleScanFilterImpl::GetManufacturerId() const
{
    return manufacturerId_;
}

void BleScanFilterImpl::SetManufactureData(std::vector<uint8_t> manufactureData)
{
    manufactureData_ = manufactureData;
}

std::vector<uint8_t> BleScanFilterImpl::GetManufactureData() const
{
    return manufactureData_;
}

void BleScanFilterImpl::SetManufactureDataMask(std::vector<uint8_t> manufactureDataMask)
{
    manufactureDataMask_ = manufactureDataMask;
}

std::vector<uint8_t> BleScanFilterImpl::GetManufactureDataMask() const
{
    return manufactureDataMask_;
}

void BleScanFilterImpl::SetClientId(int clientId)
{
    clientId_ = clientId;
}

int BleScanFilterImpl::GetClientId() const
{
    return clientId_;
}

void BleScanFilterImpl::SetFiltIndex(uint8_t filtIndex)
{
    filtIndex_ = filtIndex;
}

uint8_t BleScanFilterImpl::GetFiltIndex() const
{
    return filtIndex_;
}

void BleScanFilterImpl::SetFilterAction(uint8_t action)
{
    action_ = action;
}

uint8_t BleScanFilterImpl::GetFilterAction() const
{
    return action_;
}

/**
 * @brief Check if the device service is connectable.
 *
 * @return Returns <b>true</b> if device service is connectable;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BleAdvertiserSettingsImpl::IsConnectable() const
{
    return connectable_;
}

/**
 * @brief Set whether the device service is connectable.
 *
 * @param connectable Whether the device service is connectable.
 * @since 6
 */
void BleAdvertiserSettingsImpl::SetConnectable(bool connectable)
{
    connectable_ = connectable;
}

/**
 * @brief Check if the advertiser is in legacy mode.
 *
 * @return Returns <b>true</b> if the advertiser is in legacy mode;
 *         Returns <b>false</b> otherwisee.
 * @since 6
 */
bool BleAdvertiserSettingsImpl::IsLegacyMode() const
{
    return legacyMode_;
}

/**
 * @brief Set whether to enable the legacy mode.
 *
 * @param connectable Whether to enable the legacy mode
 * @since 6
 */
void BleAdvertiserSettingsImpl::SetLegacyMode(bool legacyMode)
{
    legacyMode_ = legacyMode;
}

/**
 * @brief Get advertise interval.
 *
 * @return Returns the advertising interval.
 * @since 6
 */
int BleAdvertiserSettingsImpl::GetInterval() const
{
    return interval_;
}

/**
 * @brief Set advertise interval.
 *
 * @param interval Advertise interval.
 * @since 6
 */
void BleAdvertiserSettingsImpl::SetInterval(int interval)
{
    interval_ = interval;
}

/**
 * @brief Get the advertiser Tx power.
 *
 * @return Returns advertiser Tx power.
 * @since 6
 */
int BleAdvertiserSettingsImpl::GetTxPower() const
{
    return txPower_;
}

/**
 * @brief Set the advertiser Tx power.
 *
 * @param txPowerthe advertiser Tx power.
 * @since 6
 */
int BleAdvertiserSettingsImpl::SetTxPower(int txPower)
{
    if (txPower > BLE_ADV_TX_POWER_HIGH || txPower < BLE_ADV_TX_POWER_ULTRA_LOW) {
        return RET_BAD_PARAM;
    }
    switch (txPower) {
        case BLE_ADV_TX_POWER_ULTRA_LOW:
            txPower_ = BLE_ADV_TX_POWER_ULTRA_LOW_VALUE;
            break;
        case BLE_ADV_TX_POWER_LOW:
            txPower_ = BLE_ADV_TX_POWER_LOW_VALUE;
            break;
        case BLE_ADV_TX_POWER_MEDIUM:
            txPower_ = BLE_ADV_TX_POWER_MEDIUM_VALUE;
            break;
        case BLE_ADV_TX_POWER_HIGH:
            txPower_ = BLE_ADV_TX_POWER_HIGH_VALUE;
            break;
        default:
            txPower_ = BLE_ADV_TX_POWER_LOW_VALUE;
            break;
    }

    return RET_NO_ERROR;
}

/**
 * @brief Get the primary phy.
 *
 * @return Returns the primary phy.
 * @since 6
 */
int BleAdvertiserSettingsImpl::GetPrimaryPhy() const
{
    return primaryPhy_;
}

/**
 * @brief Set the primary phy.
 *
 * @param primaryPhy Primary phy.
 * @since 6
 */
void BleAdvertiserSettingsImpl::SetPrimaryPhy(int primaryPhy)
{
    primaryPhy_ = primaryPhy;
}

/**
 * @brief Get the secondary Phy.
 *
 * @return Returns primary phy.
 * @since 6
 */
int BleAdvertiserSettingsImpl::GetSecondaryPhy() const
{
    return secondaryPhy_;
}

/**
 * @brief Set the secondary phy.
 *
 * @param secondaryPhy Secondary Phy.
 * @since 6
 */
void BleAdvertiserSettingsImpl::SetSecondaryPhy(int secondaryPhy)
{
    secondaryPhy_ = secondaryPhy;
}

/**
 * @brief A constructor used to create a <b>BleAdvertiseDataInternal</b> instance.
 *
 * @since 6
 */
BleAdvertiserDataImpl::BleAdvertiserDataImpl() : payload_()
{}

/**
 * @brief Add manufacturer data.
 *
 * @param manufacturerId manufacturer Id which addad data.
 * @param data manufacturer data
 * @since 6
 */
int BleAdvertiserDataImpl::AddManufacturerData(uint16_t manufacturerId, const std::string &data)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    cdata[0] = static_cast<char>(manufacturerId & 0xFF);
    cdata[1] = static_cast<char>((manufacturerId >> BLE_ONE_BYTE_LEN) & 0xFF);
    SetManufacturerData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) + data);

    return RET_NO_ERROR;
}

/**
 * @brief Add service data.
 *
 * @param uuid Uuid of service data.
 * @param data Service data.
 * @since 6
 */
void BleAdvertiserDataImpl::AddServiceData(const Uuid &uuid, const std::string &data)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    switch (uuid.GetUuidType()) {
        case Uuid::UUID16_BYTES_TYPE: {
            /// [Len] [0x16] [UUID16] data
            cdata[0] = data.length() + BLE_UUID_LEN_16 + 1;
            cdata[1] = BLE_AD_TYPE_SERVICE_DATA;  /// 0x16
            uint16_t uuid16 = uuid.ConvertTo16Bits();
            AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) +
                    std::string(reinterpret_cast<char *>(&uuid16), BLE_UUID_LEN_16) + data);
            break;
        }

        case Uuid::UUID32_BYTES_TYPE: {
            /// [Len] [0x20] [UUID32] data
            cdata[0] = data.length() + BLE_UUID_LEN_32 + 1;
            cdata[1] = BLE_AD_TYPE_32SERVICE_DATA;  /// 0x20
            uint32_t uuid32 = uuid.ConvertTo32Bits();
            AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) +
                    std::string(reinterpret_cast<char *>(&uuid32), BLE_UUID_LEN_32) + data);
            break;
        }

        case Uuid::UUID128_BYTES_TYPE: {
            /// [Len] [0x21] [UUID128] data
            cdata[0] = data.length() + BLE_UUID_LEN_128 + 1;
            cdata[1] = BLE_AD_TYPE_128SERVICE_DATA;  /// 0x21
            uint8_t uuidData[BLE_UUID_LEN_128];
            uuid.ConvertToBytesLE(uuidData);
            AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) +
                    std::string(reinterpret_cast<char *>(uuidData), BLE_UUID_LEN_128) + data);
            break;
        }

        default:
            return;
    }
}

/**
 * @brief Add characteristic value.
 *
 * @param advertiser type of the field.
 * @param advertiser data.
 * @since 6
 */
void BleAdvertiserDataImpl::AddCharacteristicValue(uint8_t adtype, const std::string &data)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    cdata[0] = data.length() + 1;
    cdata[1] = adtype;
    AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) + data);
}

/**
 * @brief Add service uuid.
 *
 * @param uuid Service uuid.
 * @since 6
 */
void BleAdvertiserDataImpl::AddServiceUuid(const Uuid &uuid)
{
    SetCompleteServices(uuid);
}

/**
 * @brief Set device appearance.
 *
 * @param appearance Device appearance.
 * @since 6
 */
void BleAdvertiserDataImpl::SetAppearance(uint16_t appearance)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    cdata[0] = BLE_ADV_DATA_BYTE_FIELD_LEN;
    cdata[1] = BLE_AD_TYPE_APPEARANCE;  /// 0x19
    AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) +
            std::string(reinterpret_cast<char *>(&appearance), BLE_ADV_DATA_FIELD_TYPE_AND_LEN));
}

/**
 * @brief Set complete services.
 *
 * @param uuid Service uuid.
 * @since 6
 */
void BleAdvertiserDataImpl::SetCompleteServices(const Uuid &uuid)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    switch (uuid.GetUuidType()) {
        case Uuid::UUID16_BYTES_TYPE: {
            /// [Len] [0x02] [LL] [HH]
            cdata[0] = BLE_UUID_LEN_16 + 1;
            cdata[1] = BLE_AD_TYPE_16SRV_CMPL;  /// 0x03
            uint16_t uuid16 = uuid.ConvertTo16Bits();
            AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) +
                    std::string(reinterpret_cast<char *>(&uuid16), BLE_UUID_LEN_16));
            break;
        }

        case Uuid::UUID32_BYTES_TYPE: {
            /// [Len] [0x04] [LL] [LL] [HH] [HH]
            cdata[0] = BLE_UUID_LEN_32 + 1;
            cdata[1] = BLE_AD_TYPE_32SRV_CMPL;  /// 0x05
            uint32_t uuid32 = uuid.ConvertTo32Bits();
            AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) +
                    std::string(reinterpret_cast<char *>(&uuid32), BLE_UUID_LEN_32));
            break;
        }

        case Uuid::UUID128_BYTES_TYPE: {
            /// [Len] [0x04] [0] [1] ... [15]
            cdata[0] = BLE_UUID_LEN_128 + 1;
            cdata[1] = BLE_AD_TYPE_128SRV_CMPL;  /// 0x07
            uint8_t uuidData[BLE_UUID_LEN_128];
            uuid.ConvertToBytesLE(uuidData);
            AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) +
                    std::string(reinterpret_cast<char *>(uuidData), BLE_UUID_LEN_128));
            break;
        }

        default:
            return;
    }
}

/**
 * @brief Set advertiser flag.
 *
 * @param flag Advertiser flag.
 * @since 6
 */
void BleAdvertiserDataImpl::SetFlags(uint8_t flag)
{
    char cdata[BLE_ADV_DATA_BYTE_FIELD_LEN];
    cdata[0] = BLE_ADV_DATA_FIELD_TYPE_AND_LEN;
    cdata[1] = BLE_AD_TYPE_FLAG;  /// 0x01
    cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN] = flag;
    advFlag_ = flag;
    AddData(std::string(cdata, BLE_ADV_DATA_BYTE_FIELD_LEN));
}

uint8_t BleAdvertiserDataImpl::GetFlags() const
{
    return advFlag_;
}

/**
 * @brief Set manufacturer data.
 *
 * @param data manufacturer data.
 * @since 6
 */
void BleAdvertiserDataImpl::SetManufacturerData(const std::string &data)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    cdata[0] = data.length() + 1;
    cdata[1] = BLE_AD_MANUFACTURER_SPECIFIC_TYPE;  /// 0xff
    AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) + data);
}

/**
 * @brief Set device name.
 *
 * @param name Device name.
 * @since 6
 */
void BleAdvertiserDataImpl::SetDeviceName(const std::string &name)
{
    if (name.length() > DEVICE_NAME_MAX_LEN) {
        SetLongName(name);
    } else {
        SetShortName(name);
    }
}

/**
 * @brief Set Tx power level.
 *
 * @param txPowerLevel Tx power level.
 * @since 6
 */
void BleAdvertiserDataImpl::SetTxPowerLevel(uint8_t txPowerLevel)
{
    char cdata[BLE_ADV_DATA_BYTE_FIELD_LEN];
    cdata[0] = BLE_ADV_DATA_FIELD_TYPE_AND_LEN;
    cdata[1] = BLE_AD_TYPE_TX_PWR;  /// 0x09
    cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN] = txPowerLevel;
    AddData(std::string(cdata, BLE_ADV_DATA_BYTE_FIELD_LEN));
}

/**
 * @brief Add service data.
 *
 * @param data Service data.
 * @since 6
 */
void BleAdvertiserDataImpl::AddData(std::string data)
{
    payload_.append(data);
}

/**
 * @brief Get advertiser data packet.
 *
 * @return Returns advertiser data packet.
 * @since 6
 */
void BleAdvertiserDataImpl::SetPayload(const std::string &payload)
{
    payload_ = payload;
}

/**
 * @brief Get advertiser data packet.
 *
 * @return Returns advertiser data packet.
 * @since 6
 */
std::string BleAdvertiserDataImpl::GetPayload() const
{
    return payload_;
}

/**
 * @brief Set advertiser data long name.
 *
 * @param name Bluetooth device name.
 * @since 6
 */
void BleAdvertiserDataImpl::SetLongName(const std::string &name)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    cdata[0] = name.length() + 1;
    cdata[1] = BLE_AD_TYPE_NAME_SHORT;  /// 0x08
    AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) + name);
}

/**
 * @brief Set advertiser data short name.
 *
 * @param name Bluetooth device name.
 * @since 6
 */
void BleAdvertiserDataImpl::SetShortName(const std::string &name)
{
    char cdata[BLE_ADV_DATA_FIELD_TYPE_AND_LEN];
    cdata[0] = name.length() + 1;
    cdata[1] = BLE_AD_TYPE_NAME_CMPL;  /// 0x09
    AddData(std::string(cdata, BLE_ADV_DATA_FIELD_TYPE_AND_LEN) + name);
}

BlePeripheralDevice::BlePeripheralDevice()
    : manufacturerData_(),
      name_(),
      serviceUUIDs_(),
      txPower_(BLE_ADDR_TYPE_RANDOM),
      serviceData_(),
      serviceDataUUIDs_(),
      pairState_(BLE_PAIR_NONE),
      ioCapability_(BLE_IO_CAP_NONE),
      aliasName_()
{
    manufacturerData_.clear();
    serviceDataUUIDs_.clear();
    serviceData_.clear();
    serviceUUIDs_.clear();
}

BlePeripheralDevice::~BlePeripheralDevice()
{
    manufacturerData_.clear();
    serviceUUIDs_.clear();
    serviceData_.clear();
    serviceDataUUIDs_.clear();
}

/**
 * @brief Get device address.
 *
 * @return Returns device address.
 * @since 6
 */
RawAddress BlePeripheralDevice::GetRawAddress() const
{
    return address_;
}

/**
 * @brief Get device appearance.
 *
 * @return Returns the device appearance.
 * @since 6
 */
uint16_t BlePeripheralDevice::GetAppearance() const
{
    return appearance_;
}

/**
 * @brief Get the manufacturer data.
 *
 * @return Returns the manufacturer data.
 * @since 6
 */
std::map<uint16_t, std::string> BlePeripheralDevice::GetManufacturerData() const
{
    return manufacturerData_;
}

/**
 * @brief the device name.
 *
 * @return Returns device Name.
 * @since 6
 */
std::string BlePeripheralDevice::GetName() const
{
    return name_;
}

/**
 * @brief Get device RSSI.
 *
 * @return Returns device RSSI.
 * @since 6
 */
int8_t BlePeripheralDevice::GetRSSI() const
{
    return rssi_;
}

/**
 * @brief Get service data.
 *
 * @return Returns service data.
 * @since 6
 */
std::vector<std::string> BlePeripheralDevice::GetServiceData() const
{
    return serviceData_;
}

/**
 * @brief Get Service Data.
 *
 * @param index Service data index.
 * @return Returns service data.
 * @since 6
 */
std::string BlePeripheralDevice::GetServiceData(int index) const
{
    return serviceData_.empty() ? "" : ((size_t)index < serviceData_.size() ? serviceData_[index] : "");
}

/**
 * @brief Get service data UUID.
 *
 * @return Returns service data UUID.
 * @since 6
 */
std::vector<Uuid> BlePeripheralDevice::GetServiceDataUUID() const
{
    return serviceDataUUIDs_;
}

/**
 * @brief Get service data UUID.
 *
 * @param index Service data index.
 * @return Returns service data UUID.
 * @since 6
 */
Uuid BlePeripheralDevice::GetServiceDataUUID(int index) const
{
    Uuid uuid {};
    return serviceDataUUIDs_.empty() ? uuid : serviceDataUUIDs_[index];
}

/**
 * @brief Get the service UUID.
 *
 * @return Returns service UUID.
 * @since 6
 */
std::vector<Uuid> BlePeripheralDevice::GetServiceUUID() const
{
    return serviceUUIDs_;
}

/**
 * @brief Get service UUID.
 *
 * @param index Service UUID index.
 * @return Return service UUID.
 * @since 6
 */
Uuid BlePeripheralDevice::GetServiceUUID(int index) const
{
    Uuid uuid {};
    return serviceUUIDs_.empty() ? uuid : serviceUUIDs_[index];
}

/**
 * @brief Get address type.
 *
 * @return Returns address type.
 * @since 6
 */
int BlePeripheralDevice::GetAddressType() const
{
    return addressType_;
}

/**
 * @brief Set address type.
 *
 * @param type Address type.
 * @since 6
 */
void BlePeripheralDevice::SetAddressType(int type)
{
    addressType_ = type;
}

/**
 * @brief Check if manufacturer data is included.
 *
 * @return Returns <b>true</b> if manufacturer data is included;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::IsManufacturerData() const
{
    return isManufacturerData_;
}

/**
 * @brief Check if the device RSSI is included.
 *
 * @return Returns <b>true</b> if include device rssi;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::IsRSSI() const
{
    return isRSSI_;
}

/**
 * @brief Check if service data is included.
 *
 * @return Returns <b>true</b> if include service data;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::IsServiceData() const
{
    return isServiceData_;
}

/**
 * @brief Check if the service UUID is included.
 *
 * @return Returns <b>true</b> if the service UUID is included;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::IsServiceUUID() const
{
    return isServiceUUID_;
}

/**
 * @brief Set device address.
 *
 * @param address device address.
 * @since 6
 */
void BlePeripheralDevice::SetAddress(const RawAddress &address)
{
    address_ = address;
}

/**
 * @brief Set RSSI value.
 *
 * @param RSSI value.
 * @since 6
 */
void BlePeripheralDevice::SetRSSI(int8_t rssi)
{
    rssi_ = rssi;
    isRSSI_ = true;
}
/**
 * @brief Check whether device is connectable.
 *
 * @param [in] rssi value.
 * return Returns <b>true</b> if device is connectable.
 *        Returns <b>false</b> otherwisee.
 */
bool BlePeripheralDevice::IsConnectable() const
{
    return connectable_;
}
/**
 * @brief Sets whether the peer device is connectable.
 *
 * @param peer device's connectable.
 */
void BlePeripheralDevice::SetConnectable(bool connectable)
{
    connectable_ = connectable;
}
/**
 * @brief Parse advertisement packets.
 *
 * @param payload Advertisement packet.
 * @param total_len Advertisement packet length.
 * @since 6
 */
void BlePeripheralDevice::ParseAdvertiserment(BlePeripheralDeviceParseAdvData &parseAdvData)
{
    size_t sizeConsumed = 0;
    bool finished = false;
    size_t totalLength = parseAdvData.length;
    payload_ = parseAdvData.payload;
    payloadLen_ = parseAdvData.length;

    while (!finished) {
        size_t length = *parseAdvData.payload;
        sizeConsumed += 1 + length;
        if (sizeConsumed > totalLength) {
            break;
        }
        parseAdvData.payload++;

        if (length != 0) {
            uint8_t advType = *parseAdvData.payload;
            parseAdvData.payload++;
            length--;
            parseAdvData.length = length;
            BuildAdvertiserData(advType, parseAdvData);
            parseAdvData.payload += length;
        }
        if (sizeConsumed >= totalLength) {
            finished = true;
        }
    }
}

void BlePeripheralDevice::BuildAdvertiserData(uint8_t advType, BlePeripheralDeviceParseAdvData &parseAdvData)
{
    switch (advType) {
        case BLE_AD_TYPE_NAME_CMPL:   /// Data Type: 0x09
        case BLE_AD_TYPE_NAME_SHORT:  /// Data Type: 0x08
            SetName(std::string(reinterpret_cast<char *>(parseAdvData.payload), parseAdvData.length));
            break;
        case BLE_AD_TYPE_TX_PWR:  /// Data Type: 0x0A
            SetTXPower(*parseAdvData.payload);
            break;
        case BLE_AD_TYPE_APPEARANCE:  /// Data Type: 0x19
            SetAppearance(*reinterpret_cast<uint16_t *>(parseAdvData.payload));
            break;
        case BLE_AD_TYPE_FLAG:  /// Data Type: 0x01
            SetAdFlag(*parseAdvData.payload);
            break;
        case BLE_AD_TYPE_16SRV_CMPL:
        case BLE_AD_TYPE_16SRV_PART:  /// Data Type: 0x02
            SetServiceUUID16Bits(parseAdvData);
            break;
        case BLE_AD_TYPE_32SRV_CMPL:
        case BLE_AD_TYPE_32SRV_PART:  /// Data Type: 0x04
            SetServiceUUID32Bits(parseAdvData);
            break;
        case BLE_AD_TYPE_128SRV_CMPL:  /// Data Type: 0x07
        case BLE_AD_TYPE_128SRV_PART:  /// Data Type: 0x06
            SetServiceUUID128Bits(parseAdvData);
            break;
        case BLE_AD_MANUFACTURER_SPECIFIC_TYPE:
            SetManufacturerData(std::string(reinterpret_cast<char *>(parseAdvData.payload), parseAdvData.length));
            break;
        case BLE_AD_TYPE_SERVICE_DATA:  /// Data Type: 0x16
            SetServiceDataUUID16Bits(parseAdvData);
            break;
        case BLE_AD_TYPE_32SERVICE_DATA:  /// Data Type: 0x20
            SetServiceDataUUID32Bits(parseAdvData);
            break;
        case BLE_AD_TYPE_128SERVICE_DATA:  /// Data Type: 0x21
            SetServiceDataUUID128Bits(parseAdvData);
            break;
        default:
            break;
    }
}

void BlePeripheralDevice::SetServiceUUID16Bits(BlePeripheralDeviceParseAdvData &parseAdvData)
{
    for (size_t var = 0; var < parseAdvData.length / BLE_UUID_LEN_16; ++var) {
        SetServiceUUID(
            Uuid::ConvertFrom16Bits(*reinterpret_cast<uint16_t *>(parseAdvData.payload + var * BLE_UUID_LEN_16)));
    }
}

void BlePeripheralDevice::SetServiceUUID32Bits(BlePeripheralDeviceParseAdvData &parseAdvData)
{
    for (size_t var = 0; var < parseAdvData.length / BLE_UUID_LEN_32; ++var) {
        SetServiceUUID(
            Uuid::ConvertFrom32Bits(*reinterpret_cast<uint32_t *>(parseAdvData.payload + var * BLE_UUID_LEN_32)));
    }
}

void BlePeripheralDevice::SetServiceUUID128Bits(const BlePeripheralDeviceParseAdvData &parseAdvData)
{
    for (size_t var = 0; var < parseAdvData.length / BLE_UUID_LEN_128; ++var) {
        std::array<uint8_t, BLE_UUID_LEN_128> data = {};
        for (int i = 0; i < BLE_UUID_LEN_128; i++) {
            data[i] = *(parseAdvData.payload + var * BLE_UUID_LEN_128 + i);
        }
        SetServiceUUID(Uuid::ConvertFromBytesLE(data.data()));
    }
}

void BlePeripheralDevice::SetServiceDataUUID16Bits(BlePeripheralDeviceParseAdvData &parseAdvData)
{
    if (parseAdvData.length < BLE_UUID_LEN_16) {
        return;
    }
    uint16_t uuid = *(reinterpret_cast<uint16_t *>(parseAdvData.payload));
    std::string data = "";
    if (parseAdvData.length > BLE_UUID_LEN_16) {
        data = std::string(
            reinterpret_cast<char *>(parseAdvData.payload + BLE_UUID_LEN_16), parseAdvData.length - BLE_UUID_LEN_16);
    }
    SetServiceDataUUID(Uuid::ConvertFrom16Bits(uuid), data);
}

void BlePeripheralDevice::SetServiceDataUUID32Bits(BlePeripheralDeviceParseAdvData &parseAdvData)
{
    if (parseAdvData.length < BLE_UUID_LEN_32) {
        return;
    }
    uint32_t uuid = *(reinterpret_cast<uint32_t *>(parseAdvData.payload));
    std::string data = "";
    if (parseAdvData.length > BLE_UUID_LEN_32) {
        data = std::string(
            reinterpret_cast<char *>(parseAdvData.payload + BLE_UUID_LEN_32), parseAdvData.length - BLE_UUID_LEN_32);
    }
    SetServiceDataUUID(Uuid::ConvertFrom32Bits(uuid), data);
}

void BlePeripheralDevice::SetServiceDataUUID128Bits(BlePeripheralDeviceParseAdvData &parseAdvData)
{
    if (parseAdvData.length < BLE_UUID_LEN_128) {
        return;
    }
    std::string data = "";
    if (parseAdvData.length > BLE_UUID_LEN_128) {
        data = std::string(
            reinterpret_cast<char *>(parseAdvData.payload + BLE_UUID_LEN_128), parseAdvData.length - BLE_UUID_LEN_128);
    }
    SetServiceDataUUID(Uuid::ConvertFromBytesLE(parseAdvData.payload), data);
}

/**
 * @brief Set device name.
 *
 * @param name Device name.
 * @since 6
 */
void BlePeripheralDevice::SetName(const std::string &name)
{
    name_ = name;
    isName_ = true;
}

/**
 * @brief Set device roles.
 *
 * @param roles Device roles.
 * @since 6
 */
void BlePeripheralDevice::SetRoles(uint8_t roles)
{
    roles_ = roles;
}

/**
 * @brief Set bonded from local.
 *
 * @param flag Advertiser flag.
 * @since 6
 */
void BlePeripheralDevice::SetBondedFromLocal(bool flag)
{
    bondFlag_ = flag;
}

/**
 * @brief Set acl connection state.
 *
 * @param connectState Acl connection state.
 * @since 6
 */
void BlePeripheralDevice::SetAclConnectState(int connectState)
{
    aclConnected_ = connectState;
}

/**
 * @brief Set acl connection handle.
 *
 * @param handle Acl connection handle.
 * @since 6
 */
void BlePeripheralDevice::SetConnectionHandle(int handle)
{
    connectionHandle_ = handle;
}

/**
 * @brief Check if device acl is connected.
 *
 * @return Returns <b>true</b> if device acl is connected;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::IsAclConnected() const
{
    if (aclConnected_ != BLE_CONNECTION_STATE_DISCONNECTED) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Check if device acl Encrypted.
 *
 * @return Returns <b>true</b> if device acl is Encrypted;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::IsAclEncrypted() const
{
    if (aclConnected_ > BLE_CONNECTION_STATE_CONNECTED) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Check if device was bonded from local.
 *
 * @return Returns <b>true</b> if device was bonded from local;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::IsBondedFromLocal() const
{
    return bondFlag_;
}

/**
 * @brief Get acl connection handle.
 *
 * @return Returns acl connection handle;
 * @since 6
 */
int BlePeripheralDevice::GetConnectionHandle() const
{
    return connectionHandle_;
}

/**
 * @brief Get device type.
 *
 * @return Returns device type.
 * @since 6
 */
uint8_t BlePeripheralDevice::GetDeviceType() const
{
    if (adFlag_ & BLE_ADV_FLAG_BREDR_NOT_SPT) {
        return BLE_BT_DEVICE_TYPE_DUMO;
    }
    return BLE_BT_DEVICE_TYPE_BLE;
}

/**
 * @brief Get advertising flag.
 *
 * @return Returns advertising flag.
 * @since 6
 */
uint8_t BlePeripheralDevice::GetAdFlag() const
{
    return adFlag_;
}

/**
 * @brief Get paired status.
 *
 * @return Returns paired status.
 * @since 6
 */
uint8_t BlePeripheralDevice::GetPairedStatus() const
{
    return pairState_;
}

/**
 * @brief Set paired status.
 *
 * @param status Paired status.
 * @return Returns <b>true</b> if the operation is successful;
 *         Returns <b>false</b> otherwise.
 * @since 6
 */
bool BlePeripheralDevice::SetPairedStatus(uint8_t status)
{
    if (BLE_PAIR_NONE > status || BLE_PAIR_CANCELING < status) {
        return false;
    }
    if (pairState_ == status) {
        return true;
    }
    pairState_ = status;
    return true;
}

/**
 * @brief Set alias name.
 *
 * @param name Device alias name.
 * @since 6
 */
void BlePeripheralDevice::SetAliasName(const std::string &name)
{
    aliasName_ = name;
}

/**
 * @brief Get alias name.
 *
 * @return Returns alias name.
 * @since 6
 */
std::string BlePeripheralDevice::GetAliasName() const
{
    return aliasName_;
}

/**
 * @brief Set IO capability.
 *
 * @param io IO capability
 * @since 6
 */
void BlePeripheralDevice::SetIoCapability(uint8_t io)
{
    ioCapability_ = io;
}

/**
 * @brief Get IO capability.
 *
 * @return Returns IO capability.
 * @since 6
 */
uint8_t BlePeripheralDevice::GetIoCapability() const
{
    return ioCapability_;
}

/**
 * @brief Set advertising flag.
 *
 * @param adFlag Advertising flag.
 * @since 6
 */
void BlePeripheralDevice::SetAdFlag(uint8_t adFlag)
{
    adFlag_ = adFlag;
}

/**
 * @brief Set device appearance.
 *
 * @param device Appearance.
 * @since 6
 */
void BlePeripheralDevice::SetAppearance(uint16_t appearance)
{
    appearance_ = appearance;
    isAppearance_ = true;
}

/**
 * @brief Set manufacturer data.
 *
 * @param manufacturerData Manufacturer data.
 * @since 6
 */
void BlePeripheralDevice::SetManufacturerData(std::string manufacturerData)
{
    if (manufacturerData.size() > BLE_UUID_LEN_16) {
        uint16_t manufacturerId = uint8_t(manufacturerData[0]) | (uint16_t(manufacturerData[1]) << BLE_ONE_BYTE_LEN);
        auto iter = manufacturerData_.find(manufacturerId);
        if (iter == manufacturerData_.end()) {
            manufacturerData_.insert(std::make_pair(manufacturerId, manufacturerData.substr(BLE_UUID_LEN_16)));
        }
        isManufacturerData_ = true;
    } else {
        manufacturerData_.clear();
        isManufacturerData_ = false;
    }
}

/**
 * @brief Set service data UUID.
 *
 * @param uuid Service data UUID.
 * @since 6
 */
void BlePeripheralDevice::SetServiceDataUUID(Uuid uuid, std::string data)
{
    isServiceData_ = true;
    auto iter = std::find(serviceDataUUIDs_.begin(), serviceDataUUIDs_.end(), uuid);
    if (iter == serviceDataUUIDs_.end()) {
        serviceDataUUIDs_.push_back(uuid);
        serviceData_.push_back(data);
    }
}

/**
 * @brief Set service UUID.
 *
 * @param serviceUUID Service UUID.
 * @since 6
 */
void BlePeripheralDevice::SetServiceUUID(Uuid serviceUUID)
{
    isServiceUUID_ = true;
    auto iter = std::find(serviceUUIDs_.begin(), serviceUUIDs_.end(), serviceUUID);
    if (iter == serviceUUIDs_.end()) {
        serviceUUIDs_.push_back(serviceUUID);
    }
}
/**
 * @brief Set TX power.
 *
 * @param txPower TX power.
 * @since 6
 */
void BlePeripheralDevice::SetTXPower(int8_t txPower)
{
    isTXPower_ = true;
    txPower_ = txPower;
}
/**
 * @brief Get peripheral device.
 *
 * @return Returns peripheral device pointer.
 * @since 6
 */
BlePeripheralDevice BleScanResultImpl::GetPeripheralDevice() const
{
    return peripheralDevice_;
}

/**
 * @brief Set peripheral device.
 *
 * @param dev Peripheral device.
 * @since 6
 */
void BleScanResultImpl::SetPeripheralDevice(const BlePeripheralDevice &dev)
{
    peripheralDevice_ = dev;
}

/**
 * @brief Get advertiser data packet.
 *
 * @return Returns advertiser data packet.
 * @since 6
 */
uint8_t *BlePeripheralDevice::GetPayload() const
{
    return payload_;
}

/**
 * @brief Get advertising packet length.
 *
 * @return Returns advertising packet length.
 * @since 6
 */
size_t BlePeripheralDevice::GetPayloadLen() const
{
    return payloadLen_;
}
}  // namespace bluetooth
}  // namespace OHOS
