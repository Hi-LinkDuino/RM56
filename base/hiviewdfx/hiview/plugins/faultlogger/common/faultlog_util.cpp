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

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "constants.h"
#include "faultlog_info.h"
#include "logger.h"
#include "string_util.h"
#include "time_util.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("Faultlogger-util");
namespace {
constexpr int DEFAULT_BUFFER_SIZE = 64;

sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        return nullptr;
    }

    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}
} // namespace

std::string GetFormatedTime(uint64_t time)
{
    if (time > LONG_MAX) {
        return "00000000000000";
    }

    time_t out = static_cast<time_t>(time);
    struct tm tmStruct {0};
    struct tm* timeInfo = localtime_r(&out, &tmStruct);
    if (timeInfo == nullptr) {
        return "00000000000000";
    }

    char buf[DEFAULT_BUFFER_SIZE] = {0};
    strftime(buf, DEFAULT_BUFFER_SIZE - 1, "%Y%m%d%H%M%S", timeInfo);
    return std::string(buf, strlen(buf));
}

std::string GetFaultNameByType(int32_t faultType, bool asFileName)
{
    switch (faultType) {
        case FaultLogType::JS_CRASH:
            return asFileName ? "jscrash" : "JS_ERROR";
        case FaultLogType::JAVA_CRASH:
            return asFileName ? "javacrash" : "JAVA_CRASH";
        case FaultLogType::CPP_CRASH:
            return asFileName ? "cppcrash" : "CPP_CRASH";
        case FaultLogType::APP_FREEZE:
            return asFileName ? "appfreeze" : "APP_FREEZE";
        case FaultLogType::SYS_FREEZE:
            return asFileName ? "sysfreeze" : "SYS_FREEZE";
        default:
            break;
    }
    return "Unknown";
}

std::string GetFaultLogName(const FaultLogInfo& info)
{
    std::string name = info.module;
    if (name.find("/") != std::string::npos) {
        name = info.module.substr(info.module.find_last_of("/") + 1);
    }

    std::string ret = "";
    ret.append(GetFaultNameByType(info.faultLogType, true));
    ret.append("-");
    ret.append(name);
    ret.append("-");
    ret.append(std::to_string(info.id));
    ret.append("-");
    ret.append(GetFormatedTime(info.time));
    return ret;
}

int32_t GetLogTypeByName(const std::string& type)
{
    if (type == "jscrash") {
        return FaultLogType::JS_CRASH;
    } else if (type == "javacrash") {
        return FaultLogType::JAVA_CRASH;
    } else if (type == "cppcrash") {
        return FaultLogType::CPP_CRASH;
    } else if (type == "appfreeze") {
        return FaultLogType::APP_FREEZE;
    } else if (type == "sysfreeze") {
        return FaultLogType::SYS_FREEZE;
    } else if (type == "all" || type == "ALL") {
        return FaultLogType::ALL;
    } else {
        return -1;
    }
}

FaultLogInfo ExtractInfoFromFileName(const std::string& fileName)
{
    // FileName LogType-PackageName-Uid-YYYYMMDDHHMMSS
    FaultLogInfo info;
    std::vector<std::string> splitStr;
    const int32_t expectedVecSize = 4;
    StringUtil::SplitStr(fileName, "-", splitStr);
    if (splitStr.size() == expectedVecSize) {
        info.faultLogType = GetLogTypeByName(splitStr[0]);                 // 0 : index of log type
        info.module = splitStr[1];                                         // 1 : index of module name
        StringUtil::ConvertStringTo<int32_t>(splitStr[2], info.id);        // 2 : index of uid
        info.time = TimeUtil::StrToTimeStamp(splitStr[3], "%Y%m%d%H%M%S"); // 3 : index of timestamp
    }
    info.pid = 0;
    return info;
}

FaultLogInfo ExtractInfoFromTempFile(const std::string& fileName)
{
    // FileName LogType-pid-time
    FaultLogInfo info;
    std::vector<std::string> splitStr;
    const int32_t expectedVecSize = 3;
    StringUtil::SplitStr(fileName, "-", splitStr);
    if (splitStr.size() == expectedVecSize) {
        info.faultLogType = GetLogTypeByName(splitStr[0]);                 // 0 : index of log type
        StringUtil::ConvertStringTo<int32_t>(splitStr[1], info.pid);       // 1 : index of pid
        StringUtil::ConvertStringTo<int64_t>(splitStr[2], info.time);      // 2 : index of timestamp
    }
    return info;
}

std::string RegulateModuleNameIfNeed(const std::string& name)
{
    std::vector<std::string> splitStr;
    StringUtil::SplitStr(name, "/\\", splitStr);
    auto size = splitStr.size();
    if (size > 0) {
        return splitStr[size - 1];
    }
    return name;
}

std::vector<std::string> GetApplicationNamesById(int32_t uid)
{
    std::vector<std::string> bundleNames;
    sptr<AppExecFwk::IBundleMgr> mgr = GetBundleMgrProxy();
    if (mgr != nullptr) {
        HIVIEW_LOGD("mgr != nullptr");
        mgr->GetBundlesForUid(uid, bundleNames);
    } else {
        HIVIEW_LOGD("mgr == nullptr");
    }
    HIVIEW_LOGD("bundleNames is %{public}d", bundleNames.size());
    return bundleNames;
}

std::string GetApplicationNameById(int32_t uid)
{
    HIVIEW_LOGD("called");
    std::vector<std::string> bundleNames = GetApplicationNamesById(uid);
    if (bundleNames.empty()) {
        return "";
    }

    return bundleNames.front();
}

std::string GetApplicationVersion(int32_t uid, const std::string& bundleName)
{
    sptr<AppExecFwk::IBundleMgr> mgr = GetBundleMgrProxy();
    AppExecFwk::BundleInfo info;
    if ((mgr != nullptr) &&
        (mgr->GetBundleInfo(bundleName, AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, info) != ERR_OK)) {
        return "";
    }
    return info.versionName;
}
} // namespace HiviewDFX
} // namespace OHOS
