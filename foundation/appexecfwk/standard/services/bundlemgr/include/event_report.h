/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_EVENT_REPORT_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_EVENT_REPORT_H

#include <string>

#include "appexecfwk_errors.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {
enum class BMSEventType {
    UNKNOW = 0,
    /***********FAULT EVENT**************/
    BUNDLE_INSTALL_EXCEPTION,
    BUNDLE_UNINSTALL_EXCEPTION,
    BUNDLE_UPDATE_EXCEPTION,
    PRE_BUNDLE_RECOVER_EXCEPTION,
    BUNDLE_STATE_CHANGE_EXCEPTION,
    BUNDLE_CLEAN_CACHE_EXCEPTION,
    /***********BEHAVIOR EVENT***********/
    BOOT_SCAN_START,
    BOOT_SCAN_END,
    BUNDLE_INSTALL,
    BUNDLE_UNINSTALL,
    BUNDLE_UPDATE,
    PRE_BUNDLE_RECOVER,
    BUNDLE_STATE_CHANGE,
    BUNDLE_CLEAN_CACHE,
};

enum class BundleEventType {
    UNKNOW = 0,
    INSTALL,
    UNINSTALL,
    UPDATE,
    RECOVER,
};

enum class InstallScene {
    NORMAL = 0,
    BOOT,
    REBOOT,
    CREATE_USER,
    REMOVE_USER,
};

enum HiSysEventType {
    FAULT     = 1,    // system fault event
    STATISTIC = 2,    // system statistic event
    SECURITY  = 3,    // system security event
    BEHAVIOR  = 4     // system behavior event
};

struct EventInfo {
    int32_t userId = Constants::INVALID_USERID;
    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    int64_t timeStamp = 0;
    uint32_t versionCode = 0;

    // olny used for preBundle
    bool isPreInstallApp = false;
    InstallScene preBundleScene = InstallScene::NORMAL;

    // olny used for clean cache
    bool isCleanCache = true;

    // olny used for component diable or enable
    bool isEnable = false;

    // olny used for free install
    bool isFreeInstallMode = false;

    // olny used in fault event
    ErrCode errCode = ERR_OK;

    void Reset()
    {
        userId = Constants::INVALID_USERID;
        bundleName.clear();
        moduleName.clear();
        abilityName.clear();
        versionCode = 0;
        timeStamp = 0;
        preBundleScene = InstallScene::NORMAL;
        isCleanCache = false;
        isPreInstallApp = false;
        isFreeInstallMode = false;
        isEnable = false;
        errCode = ERR_OK;
    }
};

class EventReport {
public:
    /**
     * @brief Send bundle system events.
     * @param bundleEventType Indicates the bundle eventType.
     * @param eventInfo Indicates the eventInfo.
     */
    static void SendBundleSystemEvent(BundleEventType bundleEventType, const EventInfo& eventInfo);
    /**
     * @brief Send scan system events.
     * @param bMSEventType Indicates the bMSEventType.
     */
    static void SendScanSysEvent(BMSEventType bMSEventType);
    /**
     * @brief Send component diable or enable system events.
     * @param bundleName Indicates the bundleName.
     * @param abilityName Indicates the abilityName.
     * @param userId Indicates the userId.
     * @param isEnable Indicates the isEnable.
     * @param exception Indicates the exception.
     */
    static void SendComponentStateSysEvent(const std::string &bundleName,
        const std::string &abilityName, int32_t userId, bool isEnable, bool exception);
    /**
     * @brief Send clean cache system events.
     * @param bundleName Indicates the bundleName.
     * @param userId Indicates the userId.
     * @param isCleanCache Indicates the isCleanCache.
     * @param exception Indicates the exception.
     */
    static void SendCleanCacheSysEvent(
        const std::string &bundleName, int32_t userId, bool isCleanCache, bool exception);
    /**
     * @brief Send system events.
     * @param eventType Indicates the bms eventInfo.
     * @param eventInfo Indicates the eventInfo.
     */
    static void SendSystemEvent(BMSEventType eventType, const EventInfo& eventInfo);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_EVENT_REPORT_H
