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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H

#include "base_bundle_installer.h"

namespace OHOS {
namespace AppExecFwk {
class SystemBundleInstaller : public BaseBundleInstaller {
public:
    SystemBundleInstaller();
    virtual ~SystemBundleInstaller() override;
    /**
     * @brief Install system and system vendor bundles.
     * @param filePath Indicates the filePath.
     * @param appType Indicates the bundle type.
     * @param userId Indicates the user ID.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool InstallSystemBundle(const std::string &filePath,
        Constants::AppType appType, int32_t userId = Constants::UNSPECIFIED_USERID);
    /**
     * @brief OTA Install system and system vendor bundles.
     * @param filePath Indicates the filePaths.
     * @param appType Indicates the bundle type.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool OTAInstallSystemBundle(const std::vector<std::string> &filePaths, Constants::AppType appType);
    /**
     * @brief Uninstall system and system vendor bundles.
     * @param bundleName Indicates the bundle name.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool UninstallSystemBundle(const std::string &bundleName);
    /**
     * @brief Uninstall system and system vendor bundles.
     * @param bundleName Indicates the bundle name.
     * @param moduleName Indicates the module name.
     * @return Returns true if this function called successfully; returns false otherwise.
     */
    bool UninstallSystemBundle(const std::string &bundleName, const std::string &modulePackage);

private:

    DISALLOW_COPY_AND_MOVE(SystemBundleInstaller);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_SYSTEM_BUNDLE_INSTALLER_H