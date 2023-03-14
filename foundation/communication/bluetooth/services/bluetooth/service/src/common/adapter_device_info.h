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

#ifndef ADAPTER_DEVICE_INFO_H
#define ADAPTER_DEVICE_INFO_H

#include <mutex>
#include <string>
#include <vector>

#include "base/base_def.h"

/*
 * @brief The Bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
const std::string SECTION_BLE = "BLE";
const std::string PROPERTY_SYSYTEM_ID = "SystemId";
const std::string PROPERTY_PNP_ID = "PnpId";
const std::string PROPERTY_IEEE_INFO = "IEEEInfo";
const std::string PROPERTY_MANUFACTURER_NAME = "ManufactureName";
const std::string PROPERTY_MODEL_NUMBER = "ModelNumber";
const std::string PROPERTY_SERIAL_NUMBER = "SerialNumber";
const std::string PROPERTY_HARDWARE_REVISION = "HardwareRevision";
const std::string PROPERTY_FIRMWARE_REVISION = "FirmwareRevision";
const std::string PROPERTY_SOFTWARE_REVISION = "SoftwareRevision";

const std::string DEVICE_INFO_PATH = "./bt_device_info.xml";
const std::string SECTION_BREDR = "BR/EDR";
const std::string PROPERTY_SPECIFICATION_ID = "SpecificationID";
const std::string PROPERTY_VENDOR_ID = "VendorID";
const std::string PROPERTY_PRODUCT_ID = "ProductID";
const std::string PROPERTY_VERSION = "Version";
const std::string PROPERTY_PRIMARY_RECORD = "PrimaryRecord";
const std::string PROPERTY_VENDOR_ID_SOURCE = "VendorIDSource";

/**
 * @brief Device info config.
 */
class IAdapterDeviceInfo {
public:
    virtual ~IAdapterDeviceInfo() = default;
    /**
     * @brief Load XML Document from specified path.
     * @return true Success Load XML Document.
     * @return false Failed Load XML Document.
     */
    virtual bool Load() = 0;

    /**
     * @brief Reload XML Document from specified path.
     * @return true Success reload XML Document.
     * @return false Failed reload XML Document.
     */
    virtual bool Reload() = 0;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value String type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, std::string &value) = 0;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Int type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, int &value) = 0;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Bool type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, bool &value) = 0;
};

class AdapterDeviceInfo : public IAdapterDeviceInfo {
public:
    /**
     * @brief Get the Instance object
     * @return IAdapterConfig*
     */
    static IAdapterDeviceInfo *GetInstance();

    /**
     * @brief Load XML Document from specified path.
     * @return true Success Load XML Document.
     * @return false Failed Load XML Document.
     */
    virtual bool Load() override;

    /**
     * @brief Reload XML Document from specified path.
     * @return true Success reload XML Document.
     * @return false Failed reload XML Document.
     */
    virtual bool Reload() override;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value String type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, std::string &value) override;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Int type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, int &value) override;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Bool type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, bool &value) override;

private:
    /**
     * @brief Construct a new Adapter Config object
     */
    AdapterDeviceInfo();

    /**
     * @brief Destroy the Adapter Config object
     */
    ~AdapterDeviceInfo();

    std::mutex mutex_ {};
    static AdapterDeviceInfo *g_instance;
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif // ADAPTER_DEVICE_INFO_H