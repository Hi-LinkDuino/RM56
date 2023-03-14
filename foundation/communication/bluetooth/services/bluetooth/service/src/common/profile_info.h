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

#ifndef PROFILE_INFO_H
#define PROFILE_INFO_H

#include <string>
#include <vector>
#include "bt_def.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents profile information.
 *
 * @since 6
 */
struct ProfileInfo {
public:
    /**
     * @brief A constructor used to create an <b>ProfileInfo</b> instance.
     *
     * @since 6
     */
    ProfileInfo() = delete;
    /**
     * @brief A constructor used to create an <b>ProfileInfo</b> instance.
     *
     * @param name Profile service name.
     * @param id Profile service Id.
     * @param uuid Profile service uuid.
     * @since 6
     */
    ProfileInfo(const std::string &name, uint32_t id, const std::string &uuid) : name_(name), id_(id), uuid_(uuid){};
    /**
     * @brief A destructor used to delete the <b>ProfileInfo</b> instance.
     *
     * @since 6
     */
    ~ProfileInfo() = default;
    std::string name_ = {""};
    uint32_t id_ = {0};
    std::string uuid_ = {""};
};

class SupportProfilesInfo {
public:
    /**
     * @brief A constructor used to create an <b>SupportProfilesInfo</b> instance.
     *
     * @since 6
     */
    SupportProfilesInfo() = default;
    /**
     * @brief A destructor used to delete the <b>SupportProfilesInfo</b> instance.
     *
     * @since 6
     */
    ~SupportProfilesInfo() = default;
    /**
     * @brief Get supported profiles vector.
     *
     * @return Returns supported profiles vector.
     * @since 6
     */
    static const std::vector<ProfileInfo> &GetSupportProfiles();
    /**
     * @brief Get supported profiles vector.
     *
     * @param id Profile Id.
     * @return Returns profile name by Id.
     * @since 6
     */
    static std::string IdToName(uint32_t id);
    /**
     * @brief Get supported profiles vector.
     *
     * @param transport Adapter transport.
     * @return Returns profiles information by adapter transport.
     * @since 6
     */
    static const std::vector<ProfileInfo> GetConfigSupportProfiles(BTTransport transport);

private:
    static const std::vector<ProfileInfo> SUPPORT_FILES;
};

#define GET_CONFIG_PROFILES SupportProfilesInfo::GetConfigSupportProfiles
#define GET_SUPPORT_PROFILES SupportProfilesInfo::GetSupportProfiles
}  // namespace bluetooth
}  // namespace OHOS

#endif  // PROFILE_CONFIG_H