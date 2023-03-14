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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_SCAN_INFO_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_SCAN_INFO_H

#include <string>

namespace OHOS {
namespace AppExecFwk {
namespace {
    std::string GetBoolStrVal(bool val)
    {
        return val ? "true" : "false";
    }
}
struct PreBundleConfigInfo {
    std::string bundleName;
    bool systemapi = false;
    bool keepAlive = false;
    bool singleton = false;
    bool userDataClearable = false;
    bool multiProcess = false;
    bool bootable = false;
    bool notification = false;
    bool hideDesktopIcon = false;
    bool queryPriority = false;
    bool startStaticBroadcast = false;
    bool startGuiAbilityBackground = false;

    bool operator <(const PreBundleConfigInfo &preBundleConfigInfo) const
    {
        return bundleName < preBundleConfigInfo.bundleName;
    }

    void Reset()
    {
        bundleName.clear();
        systemapi = false;
        keepAlive = false;
        singleton = false;
        userDataClearable = false;
        multiProcess = false;
        bootable = false;
        notification = false;
        hideDesktopIcon = false;
        queryPriority = false;
        startStaticBroadcast = false;
        startGuiAbilityBackground = false;
    }

    std::string ToString()
    {
        return "[ bundleName = " + bundleName + ", systemapi = " + GetBoolStrVal(systemapi)
            + ", singleton = " + GetBoolStrVal(singleton) + "]";
    }
};

struct PreScanInfo {
    std::string bundleDir;
    bool removable = false;
    int32_t priority = 0;

    bool operator < (const PreScanInfo &preScanInfo) const
    {
        if (bundleDir == preScanInfo.bundleDir) {
            return false;
        }

        return priority >= preScanInfo.priority;
    }

    friend bool operator == (const PreScanInfo& oldPreScanInfo, const PreScanInfo& newPreScanInfo)
    {
        return oldPreScanInfo.bundleDir == newPreScanInfo.bundleDir;
    }

    void Reset()
    {
        bundleDir.clear();
        removable = false;
        priority = 0;
    }

    std::string ToString()
    {
        return "[ bundleDir = " + bundleDir + ", removable = " + GetBoolStrVal(removable)
            + ", priority = " + std::to_string(priority) + "]";
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_PRE_SCAN_INFO_H
