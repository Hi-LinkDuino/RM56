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

#ifndef ADAPTER_DEVICE_CONFIG_H
#define ADAPTER_DEVICE_CONFIG_H

#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "base/base_def.h"

/*
 * @brief The Bluetooth subsystem.
 */
namespace OHOS {
namespace bluetooth {
const std::string SECTION_HOST = "Host";
const std::string PROPERTY_DEVICE_ADDR = "Address";
const std::string PROPERTY_DEVICE_NAME = "DeviceName";
const std::string PROPERTY_CLASS_OF_DEVICE = "ClassOfDevice";
const std::string PROPERTY_IO_CAPABILITY = "IOCapability";
const std::string PROPERTY_DEVICE_TYPE = "DeviceType";
const std::string PROPERTY_SCAN_MODE = "ScanMode";
const std::string PROPERTY_BONDABLE_MODE = "BondableMode";
const std::string PROPERTY_DISCOVERABLE_TIMEOUT = "DiscoverableTimeout";
const std::string PROPERTY_LOCAL_PASSKEY = "LocalPasskey";
const std::string PROPERTY_SECURITY_MODE = "SecurityMode";
const std::string PROPERTY_LINK_KEY = "LinkKey";
const std::string PROPERTY_LINK_KEY_TYPE = "LinkKeyType";
const std::string PROPERTY_ALIAS_NAME = "AliasName";
const std::string PROPERTY_PAIR_FLAG = "PairFlag";
const std::string PROPERTY_BOND_FROM_LOCAL = "BondFromLocal";
const std::string PROPERTY_URI = "uri";
const std::string PROPERTY_REMOTE_UUIDS = "RemoteUuids";

const std::string PROPERTY_BLE_ROLES = "BleRoles";
const std::string PROPERTY_BLE_MODE_1_LEVEL = "BleModel1Level";
const std::string PROPERTY_BLE_MODE_2_LEVEL = "BleModel2Level";
const std::string PROPERTY_BLE_SECURITY = "BleSecurity";
const std::string PROPERTY_BLE_APPEARANCE = "Appearance";
const std::string PROPERTY_BLE_SCAN_MODE = "BleScanMode";
const std::string PROPERTY_BLE_LOCAL_ADDR_TYPE = "LocalAddrType";
const std::string PROPERTY_BLE_ADDR_TYPE = "BleAddrType";

const std::string PROPERTY_BLE_LOCAL_LTK = "LocalLtk";
const std::string PROPERTY_BLE_LOCAL_KEY_SIZE = "LocalKeySize";
const std::string PROPERTY_BLE_LOCAL_EDIV = "LocalEdiv";
const std::string PROPERTY_BLE_LOCAL_RAND = "LocalRand";
const std::string PROPERTY_BLE_LOCAL_IRK = "LocalIrk";
const std::string PROPERTY_BLE_LOCAL_CSRK = "LocalCsrk";
const std::string PROPERTY_BLE_LOCAL_IDENTITY_ADDR = "LocalIdentityAddr";
const std::string PROPERTY_BLE_LOCAL_PUBLIC_ADDR = "LocalPublicAddr";
const std::string PROPERTY_BLE_LOCAL_SIGN_COUNTER = "LocalSignCounter";

const std::string PROPERTY_BLE_PEER_KEY_TYPE = "PeerKeyType";
const std::string PROPERTY_BLE_PEER_LTK = "PeerLtk";
const std::string PROPERTY_BLE_PEER_KEY_SIZE = "PeerKeySize";
const std::string PROPERTY_BLE_PEER_EDIV = "PeerEdiv";
const std::string PROPERTY_BLE_PEER_RAND = "PeerRand";
const std::string PROPERTY_BLE_PEER_IDENTITY_ADDR_TYPE = "PeerIdentityAddrType";
const std::string PROPERTY_BLE_PEER_IDENTITY_ADDR = "PeerIdentityAddr";
const std::string PROPERTY_BLE_PEER_IRK = "PeerIrk";
const std::string PROPERTY_BLE_PEER_CSRK = "PeerCsrk";
const std::string PROPERTY_BLE_PEER_SIGN_COUNTER = "PeerSignCounter";
const std::string PROPERTY_BLE_PEER_ADDR_TYPE = "PeerAddrType";

const std::string SECTION_BLE_PAIRED_LIST = "Ble Paired Device List";

const std::string SECTION_BREDR_PAIRED_LIST = "Classic Paired Device List";

const std::string SECTION_GENERIC_ATTRIBUTE_SERVICE = "Generic Attribute Service";
const std::string PROPERTY_GATT_TRANSPORT = "GattTransport";
const std::string PROPERTY_GATTS_START_HANDLE = "GattsStartHandle";
const std::string PROPERTY_GATTS_END_HANDLE = "GattsEndHandle";

/**
 * @brief BREDR/BLE config.
 */
class IAdapterDeviceConfig {
public:
    virtual ~IAdapterDeviceConfig() = default;
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
     * @brief Load XML Document from specified path.
     * @param[in] path XML Document path.
     * @return true Success Load XML Document.
     * @return false Failed Load XML Document.
     */
    virtual bool Save() = 0;

    /**
     * @brief Get specified property value.
     *        Value type is int.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[out] value Value type is int.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(
        const std::string &section, const std::string &subSection, const std::string &property, int &value) = 0;

    /**
     * @brief Get specified property value.
     *        Value type is string.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[out] value Value type is string.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(
        const std::string &section, const std::string &subSection, const std::string &property, std::string &value) = 0;

    /**
     * @brief Get specified property value.
     *        Value type is bool.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[out] value Value type is bool.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(
        const std::string &section, const std::string &subSection, const std::string &property, bool &value) = 0;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[in] value Value type is const int.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(
        const std::string &section, const std::string &subSection, const std::string &property, const int &value) = 0;

    /**
     * @brief Set specified property value.
     *        Value type is string.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[in] value Value type is const string.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &subSection, const std::string &property,
        const std::string &value) = 0;

    /**
     * @brief Set specified property value.
     *        Value type is bool.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[in] value Value type is const bool.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(
        const std::string &section, const std::string &subSection, const std::string &property, const bool &value) = 0;

    /**
     * @brief Get Address
     * @param[in] section
     * @param[out] subSections
     * @return true Specified section has one or Multiple subSections.
     * @return false Specified section do not has any subSection.
     */
    virtual bool GetSubSections(const std::string &section, std::vector<std::string> &subSections) = 0;

    /**
     * @brief Remove XML document specified section.
     * @param[in] section
     * @param[in] subSection
     * @return true Success remove XML document specified section.
     * @return false Failed remove XML document specified section.
     */
    virtual bool RemoveSection(const std::string &section, const std::string &subSection) = 0;

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
     * @param[out] value String type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, std::string &value) = 0;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Bool type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, bool &value) = 0;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     * @param[in] section
     * @param[in] property
     * @param[in] value Value type is const int.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &property, const int &value) = 0;

    /**
     * @brief Set specified property value.
     *        Value type is string.
     * @param[in] section
     * @param[in] property
     * @param[in] value Value type is const string.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &property, const std::string &value) = 0;
};

class AdapterDeviceConfig : public IAdapterDeviceConfig {
public:
    /**
     * @brief Get the Instance object
     * @return IAdapterConfig*
     */
    static IAdapterDeviceConfig *GetInstance();

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
     * @brief Load XML Document from specified path.
     * @param[in] path XML Document path.
     * @return true Success Load XML Document.
     * @return false Failed Load XML Document.
     */
    virtual bool Save() override;

    /**
     * @brief Get specified property value.
     *        Value type is int.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[out] value Value type is int.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(
        const std::string &section, const std::string &subSection, const std::string &property, int &value) override;

    /**
     * @brief Get specified property value.
     *        Value type is string.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[out] value Value type is string.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &subSection, const std::string &property,
        std::string &value) override;

    /**
     * @brief Get specified property value.
     *        Value type is bool.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[out] value Value type is bool.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(
        const std::string &section, const std::string &subSection, const std::string &property, bool &value) override;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[in] value Value type is const int.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &subSection, const std::string &property,
        const int &value) override;

    /**
     * @brief Set specified property value.
     *        Value type is string.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[in] value Value type is const string.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &subSection, const std::string &property,
        const std::string &value) override;

    /**
     * @brief Set specified property value.
     *        Value type is bool.
     * @param[in] section
     * @param[in] subSection
     * @param[in] property
     * @param[in] value Value type is const bool.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &subSection, const std::string &property,
        const bool &value) override;

    /**
     * @brief Get Address
     * @param[in] section
     * @param[out] subSections
     * @return true Specified section has one or multiple subSections.
     * @return false Specified section do not has any subSection.
     */
    virtual bool GetSubSections(const std::string &section, std::vector<std::string> &subSections) override;

    /**
     * @brief Remove XML document specified section.
     * @param[in] section
     * @param[in] subSection
     * @return true Success remove XML document specified section.
     * @return false Failed remove XML document specified section.
     */
    virtual bool RemoveSection(const std::string &section, const std::string &subSection) override;

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
     * @param[out] value String type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, std::string &value) override;

    /**
     * @brief Get specified property value.
     * @param[in] section
     * @param[in] property
     * @param[out] value Bool type value.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     */
    virtual bool GetValue(const std::string &section, const std::string &property, bool &value) override;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     * @param[in] section
     * @param[in] property
     * @param[in] value Value type is const int.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &property, const int &value) override;

    /**
     * @brief Set specified property value.
     *        Value type is string.
     * @param[in] section
     * @param[in] property
     * @param[in] value Value type is const string.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     */
    virtual bool SetValue(const std::string &section, const std::string &property, const std::string &value) override;

private:
    /**
     * @brief Construct a new Adapter Config object
     */
    AdapterDeviceConfig();

    /**
     * @brief Destroy the Adapter Config object
     */
    ~AdapterDeviceConfig();

    std::mutex mutex_ {};
    static AdapterDeviceConfig *g_instance;
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif // ADAPTER_DEVICE_CONFIG_H