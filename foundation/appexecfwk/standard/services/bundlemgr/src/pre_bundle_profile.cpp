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

#include "pre_bundle_profile.h"

#include "app_log_wrapper.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string INSTALL_LIST = "install_list";
const std::string UNINSTALL_LIST = "uninstall_list";
const std::string INSTALL_ABILITY_CONFIGS = "install_ability_configs";
const std::string BUNDLE_DIR = "bundle_dir";
const std::string REMOVABLE = "removable";
const std::string PRIORITY = "priority";
const std::string BUNDLE_NAME = "bundleName";
const std::string SYSTEMAPI = "systemapi";
const std::string KEEP_ALIVE = "keepAlive";
const std::string SINGLETON = "singleton";
const std::string USER_DATA_CLEARABLE = "userDataClearable";
const std::string MULTI_PROCESS = "multiProcess";
const std::string BOOTABLE = "bootable";
const std::string NOTIFICATION = "notification";
const std::string HIDE_DESKTOP_ICON = "hideDesktopIcon";
const std::string QUERY_PRIORITY = "queryPriority";
const std::string START_STATIC_BROADCAST = "startStaticBroadcast";
const std::string START_GUI_ABLITY_BACKGROUND = "startGuiAbilityBackground";
}

ErrCode PreBundleProfile::TransformTo(
    const nlohmann::json &jsonBuf, std::set<PreScanInfo> &scanInfos) const
{
    APP_LOGI("transform jsonBuf to PreScanInfos");
    if (jsonBuf.is_discarded()) {
        APP_LOGE("profile format error");
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }
    
    if (jsonBuf.find(INSTALL_LIST) == jsonBuf.end()) {
        return ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR;
    }

    auto arrays = jsonBuf.at(INSTALL_LIST);
    if (!arrays.is_array() || arrays.empty()) {
        APP_LOGE("value is not array");
        return ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR;
    }

    PreScanInfo preScanInfo;
    for (const auto &array : arrays) {
        if (!array.is_object()) {
            APP_LOGE("array is not json object");
            return ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR;
        }

        preScanInfo.Reset();
        const auto &jsonObjectEnd = array.end();
        int32_t parseResult = ERR_OK;
        GetValueIfFindKey<std::string>(array,
            jsonObjectEnd,
            BUNDLE_DIR,
            preScanInfo.bundleDir,
            JsonType::STRING,
            true,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            REMOVABLE,
            preScanInfo.removable,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<int32_t>(array,
            jsonObjectEnd,
            PRIORITY,
            preScanInfo.priority,
            JsonType::NUMBER,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        if (parseResult == ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP) {
            APP_LOGE("bundleDir must exist, and it is empty here");
            continue;
        }

        if (parseResult != ERR_OK) {
            APP_LOGE("parse from json failed");
            return parseResult;
        }

        if (std::find(scanInfos.begin(), scanInfos.end(), preScanInfo) != scanInfos.end()) {
            APP_LOGE("preScanInfo(%{public}s) has exit", preScanInfo.ToString().c_str());
            continue;
        }

        scanInfos.insert(preScanInfo);
    }

    return ERR_OK;
}

ErrCode PreBundleProfile::TransformTo(
    const nlohmann::json &jsonBuf, std::set<std::string> &bundleNames) const
{
    APP_LOGD("transform jsonBuf to bundleNames");
    if (jsonBuf.is_discarded()) {
        APP_LOGE("profile format error");
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }

    const auto &jsonObjectEnd = jsonBuf.end();
    int32_t parseResult = ERR_OK;
    std::vector<std::string> names;
    GetValueIfFindKey<std::vector<std::string>>(jsonBuf,
        jsonObjectEnd,
        UNINSTALL_LIST,
        names,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    for (auto name : names) {
        APP_LOGD("bundleNames %{public}s", name.c_str());
        bundleNames.insert(name);
    }

    return parseResult;
}

ErrCode PreBundleProfile::TransformTo(
    const nlohmann::json &jsonBuf, std::set<PreBundleConfigInfo> &preBundleConfigInfos) const
{
    APP_LOGI("transform jsonBuf to preBundleConfigInfos");
    if (jsonBuf.is_discarded()) {
        APP_LOGE("profile format error");
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }

    if (!jsonBuf.is_array() || jsonBuf.empty()) {
        APP_LOGE("value is not array");
        return ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR;
    }

    PreBundleConfigInfo preBundleConfigInfo;
    for (const auto &array : jsonBuf) {
        if (!array.is_object()) {
            APP_LOGE("array is not json object");
            return ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR;
        }

        preBundleConfigInfo.Reset();
        const auto &jsonObjectEnd = array.end();
        int32_t parseResult = ERR_OK;
        GetValueIfFindKey<std::string>(array,
            jsonObjectEnd,
            BUNDLE_NAME,
            preBundleConfigInfo.bundleName,
            JsonType::STRING,
            true,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            SYSTEMAPI,
            preBundleConfigInfo.systemapi,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            KEEP_ALIVE,
            preBundleConfigInfo.keepAlive,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            SINGLETON,
            preBundleConfigInfo.singleton,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            USER_DATA_CLEARABLE,
            preBundleConfigInfo.userDataClearable,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            MULTI_PROCESS,
            preBundleConfigInfo.multiProcess,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            BOOTABLE,
            preBundleConfigInfo.bootable,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            NOTIFICATION,
            preBundleConfigInfo.notification,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            HIDE_DESKTOP_ICON,
            preBundleConfigInfo.hideDesktopIcon,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            QUERY_PRIORITY,
            preBundleConfigInfo.queryPriority,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            START_STATIC_BROADCAST,
            preBundleConfigInfo.startStaticBroadcast,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        GetValueIfFindKey<bool>(array,
            jsonObjectEnd,
            START_GUI_ABLITY_BACKGROUND,
            preBundleConfigInfo.startGuiAbilityBackground,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        if (parseResult == ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP) {
            APP_LOGE("bundlename must exist, and it is empty here");
            continue;
        }

        if (parseResult != ERR_OK) {
            APP_LOGE("parse from json failed");
            return parseResult;
        }

        if (preBundleConfigInfos.find(preBundleConfigInfo) != preBundleConfigInfos.end()) {
            APP_LOGE("preBundleConfigInfo(%{public}s) has exit", preBundleConfigInfo.ToString().c_str());
            continue;
        }

        preBundleConfigInfos.insert(preBundleConfigInfo);
    }

    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS
