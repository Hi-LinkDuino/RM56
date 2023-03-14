/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_LAUNCHER_SERVICE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_LAUNCHER_SERVICE_H

#include <string>
#include <vector>

#include "bundle_mgr_interface.h"
#include "securec.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "bundle_monitor.h"
#include "bundle_status_callback_interface.h"
#include "launcher_ability_info.h"
#include "launcher_shortcut_info.h"
#include "ability_info.h"
#include "application_info.h"
#include "want.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
class LauncherService : public virtual RefBase {
public:
    using Want = OHOS::AAFwk::Want;

    LauncherService();
    virtual ~LauncherService() = default;

    /**
     * @brief Registers a callback method for monitoring bundle installation, uninstallation, and update events.
     * @param callback Indicates the callback method to be called.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool RegisterCallback(const sptr<IBundleStatusCallback> &callback);
    /**
     * @brief Unregisters the callback method for monitoring bundle installation, uninstallation, and update events.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool UnRegisterCallback();
    /**
     * @brief Obtains launcher abilities based on the bundle name and user id.
     * @param bundleName Indicates the bundle name.
     * @param userId Indicates the user ID.
     * @param launcherAbilityInfos List of LauncherAbilityInfo objects if obtained;
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool GetAbilityList(
        const std::string &bundleName, const int userId, std::vector<LauncherAbilityInfo> &launcherAbilityInfos);
    /**
     * @brief Obtains launcher ability based on the want and user id.
     * @param want Indicates the application bundle name to be queried.
     * @param userId Indicates the user ID.
     * @param launcherAbilityInfo Indicates the obtained LauncherAbilityInfo object.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    virtual bool GetAbilityInfo(const Want &want, const int userId, LauncherAbilityInfo &launcherAbilityInfo);
    /**
     * @brief Obtains the ApplicationInfo based on a given bundle name.
     * @param bundleName Indicates the application bundle name to be queried.
     * @param flag Indicates the flag used to specify information contained
     *             in the ApplicationInfo object that will be returned.
     * @param userId Indicates the user ID.
     * @param applicationInfo Indicates the obtained ApplicationInfo object.
     * @return Returns true if the function is successfully called; returns false otherwise.
     */
    virtual bool GetApplicationInfo(const std::string &bundleName, const ApplicationFlag &flags, const int userId,
        ApplicationInfo &applicationInfo);
    /**
     * @brief Checks whether a bundle exists and is enabled.
     * @param bundleName Indicates the bundle name.
     * @return Returns true if the bundle is enabled; returns false otherwise.
     */
    virtual bool IsBundleEnabled(const std::string &bundleName);
    /**
     * @brief Checks whether an ability exists and is enabled.
     * @param abilityInfo Indicates information about the ability to check.
     * @return Returns true if the ability exists and is enabled; returns false otherwise.
     */
    virtual bool IsAbilityEnabled(const AbilityInfo &abilityInfo);
    /**
     * @brief Obtains information about the shortcuts of the application.
     * @param bundleName Indicates the bundle name of the application.
     * @param launcherShortcutInfo List of LauncherShortcutInfo objects if obtained.
     * @return Returns true if the function is successfully called; returns false otherwise.
     */
    virtual bool GetShortcutInfos(const std::string &bundleName, std::vector<ShortcutInfo> &shortcutInfo);
    /**
     * @brief Obtains information about the launcher ability of all application that appears on launcher.
     * @param userId Indicates the id for the user.
     * @param launcherAbilityInfo Indicates the obtained LauncherAbilityInfo object.
     * @return Returns true if the function is successfully called; returns false otherwise.
     */
    virtual bool GetAllLauncherAbilityInfos(int32_t userId, std::vector<LauncherAbilityInfo> &launcherAbilityInfos);

private:
    void init();
    static OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgr();

    std::shared_ptr<BundleMonitor> bundleMonitor_ = nullptr;
    static OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> bundleMgr_;
    static std::mutex bundleMgrMutex_;
    DISALLOW_COPY_AND_MOVE(LauncherService);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_LAUNCHER_SERVICE_H