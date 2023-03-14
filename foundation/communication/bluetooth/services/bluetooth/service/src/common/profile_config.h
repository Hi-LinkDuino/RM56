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

#ifndef PROFILE_CONFIG_H
#define PROFILE_CONFIG_H

#include <string>
#include <vector>

#include "base/base_def.h"

namespace OHOS {
namespace bluetooth {
const std::string SECTION_CONNECTION_POLICIES = "ConnectionPolicies";
const std::string SECTION_PERMISSION = "Permission";
const std::string SECTION_CODE_CS_SUPPORT = "CodecsSupport";

const std::string PROPERTY_A2DP_CONNECTION_POLICY = "A2dpConnectionPolicy";
const std::string PROPERTY_A2DP_SINK_CONNECTION_POLICY = "A2dpSinkConnectionPolicy";
const std::string PROPERTY_HFP_CONNECTION_POLICY = "HfpConnectionPolicy";
const std::string PROPERTY_HFP_CLIENT_CONNECTION_POLICY = "HfpClientConnectionPolicy";
const std::string PROPERTY_PBAP_CONNECTION_POLICY = "PbapConnectionPolicy";
const std::string PROPERTY_PBAP_CLIENT_CONNECTION_POLICY = "PbapClientConnectionPolicy";
const std::string PROPERTY_MAP_CONNECTION_POLICY = "MapConnectionPolicy";

const std::string PROPERTY_MAP_CLIENT_CONNECTION_POLICY = "MapClientConnectionPolicy";
const std::string PROPERTY_GATT_CLIENT_CONNECTION_POLICY = "GattClientConnectionPolicy";
const std::string PROPERTY_GATT_SERVER_CONNECTION_POLICY = "GattServerConnectionPolicy";
const std::string PROPERTY_PHONEBOOK_PERMISSION = "PhonebookPermission";
const std::string PROPERTY_MESSAGE_PERMISSION = "MessagePermission";
const std::string PROPERTY_A2DP_SUPPORTS_OPTIONAL_CODECS = "A2dpSupportsOptionalCodecs";
const std::string PROPERTY_A2DP_OPTIONAL_CODECS_ENABLED = "A2dpOptionalCodecsEnabled";

class IProfileConfig {
public:
    virtual ~IProfileConfig() = default;
    /**
     * @brief Load XML document form file.
     *
     * @return true Success load xml document.
     * @return false Failed load xml document.
     * @since 6
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
     *        Value type is int.
     *
     * @param addr Bluetooth device addr.
     * @param section
     * @param property
     * @param value Value type is int.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     * @since 6
     */
    virtual bool GetValue(
        const std::string &addr, const std::string &section, const std::string &property, int &value) = 0;

    /**
     * @brief Get specified property value.
     *        Value type is int.
     *
     * @param addr
     * @param section
     * @param property
     * @param value Value type is bool.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     * @since 6
     */
    virtual bool GetValue(
        const std::string &addr, const std::string &section, const std::string &property, bool &value) = 0;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     *
     * @param section
     * @param subSection
     * @param property
     * @param value Value type is const int.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     * @since 6
     */
    virtual bool SetValue(
        const std::string &addr, const std::string &section, const std::string &property, int &value) = 0;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     *
     * @param section
     * @param subSection
     * @param property
     * @param value Value type is const bool.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     * @since 6
     */
    virtual bool SetValue(
        const std::string &addr, const std::string &section, const std::string &property, bool &value) = 0;

    /**
     * @brief Remove specified addr, as well as it's all sections and properties.
     *
     * @param Addr
     * @return true Success remove specified addr's value.
     * @return false Failed remove specified addr's value.
     * @since 6
     */
    virtual bool RemoveAddr(const std::string &addr) = 0;

    /**
     * @brief Remove specified property value.
     *        Value type is int.
     * @param addr
     * @param section
     * @param property
     * @param value Value type is const int.
     * @return true Success remove specified property's value.
     * @return false Failed remove specified property's value.
     * @since 6
     */
    virtual bool RemoveProperty(const std::string &addr, const std::string &section, const std::string &property) = 0;

    /**
     * @brief Whether XML document has specified section.
     *
     * @param addr
     * @param section
     * @return true XML document has specified section.
     * @return false XML document doesnot have specified section.
     * @since 6
     */
    virtual bool HasSection(const std::string &addr, const std::string &section) = 0;
};

class ProfileConfig : public IProfileConfig {
public:
    /**
     * @brief Get the Instance object
     *
     * @return IProfileConfig*
     * @since 6
     */
    static IProfileConfig *GetInstance();

    /**
     * @brief Load XML document form file.
     *
     * @return true Success load xml document.
     * @return false Failed load xml document.
     * @since 6
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
     *        Value type is int.
     *
     * @param addr Bluetooth device addr.
     * @param section
     * @param property
     * @param value Value type is int.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     * @since 6
     */
    virtual bool GetValue(
        const std::string &addr, const std::string &section, const std::string &property, int &value) override;

    /**
     * @brief Get specified property value.
     *        Value type is int.
     *
     * @param addr
     * @param section
     * @param property
     * @param value Value type is bool.
     * @return true Success get specified property's value.
     * @return false Failed get specified property's value.
     * @since 6
     */
    virtual bool GetValue(
        const std::string &addr, const std::string &section, const std::string &property, bool &value) override;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     *
     * @param section
     * @param subSection
     * @param property
     * @param value Value type is const int.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     * @since 6
     */
    virtual bool SetValue(
        const std::string &addr, const std::string &section, const std::string &property, int &value) override;

    /**
     * @brief Set specified property value.
     *        Value type is int.
     *
     * @param section
     * @param subSection
     * @param property
     * @param value Value type is const bool.
     * @return true Success set specified property's value.
     * @return false Failed set specified property's value.
     * @since 6
     */
    virtual bool SetValue(
        const std::string &addr, const std::string &section, const std::string &property, bool &value) override;

    /**
     * @brief Remove specified addr, as well as it's all sections and properties.
     *
     * @param Addr
     * @return true Success remove specified addr's value.
     * @return false Failed remove specified addr's value.
     * @since 6
     */
    virtual bool RemoveAddr(const std::string &addr) override;

    /**
     * @brief Remove specified property value.
     *        Value type is int.
     * @param addr
     * @param section
     * @param property
     * @param value Value type is const int.
     * @return true Success remove specified property's value.
     * @return false Failed remove specified property's value.
     * @since 6
     */
    virtual bool RemoveProperty(
        const std::string &addr, const std::string &section, const std::string &property) override;

    /**
     * @brief Whether XML document has specified section.
     *
     * @param addr
     * @param section
     * @return true XML document has specified section.
     * @return false XML document doesnot have specified section.
     * @since 6
     */
    virtual bool HasSection(const std::string &addr, const std::string &section) override;

private:
    /**
     * @brief Construct a new Profile Config object
     */
    ProfileConfig();

    /**
     * @brief Destroy the Profile Config object
     */
    ~ProfileConfig();

    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PROFILE_CONFIG_H