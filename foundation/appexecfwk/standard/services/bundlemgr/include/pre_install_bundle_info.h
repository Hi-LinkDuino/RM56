/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_INSTALL_BUNDLE_INFO_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_INSTALL_BUNDLE_INFO_H

#include "inner_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
class PreInstallBundleInfo {
public:
    /**
     * @brief Transform the PreInstallBundleInfo object to json.
     * @param jsonObject Indicates the obtained json object.
     * @return
     */
    void ToJson(nlohmann::json &jsonObject) const;
    /**
     * @brief Transform the json object to PreInstallBundleInfo object.
     * @param jsonObject Indicates the obtained json object.
     * @return Returns 0 if the json object parsed successfully; returns error code otherwise.
     */
    int32_t FromJson(const nlohmann::json &jsonObject);
    /**
     * @brief Transform the PreInstallBundleInfo object to string.
     * @return Returns the string object
     */
    std::string ToString() const;
    /**
     * @brief Get bundle name.
     * @return Return bundle name
     */
    std::string GetBundleName() const
    {
        return bundleName_;
    }
    /**
     * @brief Set bundle name.
     * @param bundleName bundle name.
     */
    void SetBundleName(const std::string &bundleName)
    {
        bundleName_ = bundleName;
    }
    /**
     * @brief Get versionCode.
     * @return Return versionCode.
     */
    uint32_t GetVersionCode() const
    {
        return versionCode_;
    }
    /**
     * @brief Set versionCode.
     * @param versionCode versionCode.
     */
    void SetVersionCode(const uint32_t &versionCode)
    {
        versionCode_ = versionCode;
    }
    /**
     * @brief Get bundle path.
     * @return Return bundle path
     */
    std::vector<std::string> GetBundlePaths() const
    {
        return bundlePaths_;
    }
    /**
     * @brief Add bundle path.
     * @param bundlePath bundle path.
     */
    void AddBundlePath(const std::string &bundlePath)
    {
        bool ret = std::find(
            bundlePaths_.begin(), bundlePaths_.end(), bundlePath) != bundlePaths_.end();
        if (!ret) {
            bundlePaths_.emplace_back(bundlePath);
        }
    }
    /**
     * @brief Delete bundle path.
     * @param bundlePath bundle path.
     */
    void DeleteBundlePath(const std::string &bundlePath)
    {
        auto iter = std::find(bundlePaths_.begin(), bundlePaths_.end(), bundlePath);
        if (iter != bundlePaths_.end()) {
            bundlePaths_.erase(iter);
        }
    }
    /**
     * @brief Has bundle path.
     * @param bundlePath bundle path.
     */
    bool HasBundlePath(const std::string &bundlePath)
    {
        return std::find(bundlePaths_.begin(), bundlePaths_.end(), bundlePath)
            != bundlePaths_.end();
    }
    /**
     * @brief Get AppType.
     * @return Returns the AppType.
     */
    Constants::AppType GetAppType() const
    {
        return appType_;
    }
    /**
     * @brief Set AppType.
     * @param appType Indicates the AppType to be set.
     */
    void SetAppType(Constants::AppType appType)
    {
        appType_ = appType;
    }
    /**
     * @brief operator.
     * @param PreInstallBundleInfo Indicates the PreInstallBundleInfo.
     */
    bool operator() (const PreInstallBundleInfo& info) const
    {
        return bundleName_ == info.GetBundleName();
    }
private:
    std::string bundleName_;
    uint32_t versionCode_;
    std::vector<std::string> bundlePaths_;
    Constants::AppType appType_ = Constants::AppType::SYSTEM_APP;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_INSTALL_BUNDLE_INFO_H
