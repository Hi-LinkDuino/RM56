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

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_status_callback.h"
#include "js_launcher_mgr.h"
#include "launcher_service.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS::AppExecFwk;

namespace {
const std::string REGISTERCALLBACK = "BundleStatusChange";
const std::string UNREGISTERCALLBACK = "BundleStatusChange";
constexpr int32_t NAPI_RETURN_ZERO = 0;
constexpr int32_t NAPI_RETURN_ONE = 1;
constexpr int32_t OPERATION_SUCESS = 0;
constexpr int32_t OPERATION_FAILED = 1;
constexpr int32_t OPERATION_TYPE_MIAMATCH = 2;
constexpr int32_t INDEX_ONE = 1;
constexpr int32_t INDEX_TWO = 2;
constexpr int32_t INDEX_THREE = 3;
}

struct AsyncHandleBundleContext {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = 0;
    OHOS::sptr<BundleStatusCallback> bundleStatusCallback = nullptr;
    std::vector<OHOS::AppExecFwk::LauncherAbilityInfo> launcherAbilityInfos;
    std::vector<OHOS::AppExecFwk::ShortcutInfo> shortcutInfos;
    std::string bundleName;
    std::string className;
    int32_t userId = 0;
    bool ret = false;
    uint32_t err = 0;
    std::string message;
};

static OHOS::sptr<OHOS::AppExecFwk::LauncherService> GetLauncherService()
{
    return OHOS::AppExecFwk::JSLauncherMgr::GetLauncherService();
}

static void ParseString(napi_env env, napi_value value, std::string& result)
{
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, NAPI_RETURN_ZERO, &size) != napi_ok) {
        APP_LOGE("can not get string size");
        return;
    }

    result.reserve(size + NAPI_RETURN_ONE);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + NAPI_RETURN_ONE), &size) != napi_ok) {
        APP_LOGE("can not get string value");
        return;
    }
}

static bool ParseBundleStatusCallback(napi_env env,
    OHOS::sptr<BundleStatusCallback>& bundleStatusCallback, napi_value args)
{
    APP_LOGD("parse bundleStatusCallback begin");
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("param type mismatch!");
        return false;
    }

    // parse added callback
    napi_ref addCallback = nullptr;
    napi_value addValue = nullptr;
    status = napi_get_named_property(env, args, "add", &addValue);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, addValue, &valueType);
    if (valueType != napi_function) {
        APP_LOGE("add param type mismatch!");
        return false;
    }
    napi_create_reference(env, addValue, NAPI_RETURN_ONE, &addCallback);

    // parse updated callback
    napi_ref updateCallback = nullptr;
    napi_value updateValue = nullptr;
    status = napi_get_named_property(env, args, "update", &updateValue);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, updateValue, &valueType);
    if (valueType != napi_function) {
        APP_LOGE("update param type mismatch!");
        return false;
    }
    napi_create_reference(env, updateValue, NAPI_RETURN_ONE, &updateCallback);

    // parse remove callback
    napi_ref removeCallback = nullptr;
    napi_value removeValue = nullptr;
    status = napi_get_named_property(env, args, "remove", &removeValue);
    NAPI_ASSERT(env, status == napi_ok, "property name incorrect!");
    napi_typeof(env, removeValue, &valueType);
    if (valueType != napi_function) {
        APP_LOGE("remove param type mismatch!");
        return false;
    }
    napi_create_reference(env, removeValue, NAPI_RETURN_ONE, &removeCallback);

    bundleStatusCallback = new BundleStatusCallback(env, addCallback, updateCallback, removeCallback);
    APP_LOGD("parse bundleStatusCallback end");
    return true;
}

static void ConvertApplicationInfo(napi_env env, napi_value objAppInfo,
    const OHOS::AppExecFwk::ApplicationInfo &appInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "name", nName));

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "description", nDescription));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "descriptionId", nDescriptionId));

    napi_value nIsSystemApp;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.isSystemApp, &nIsSystemApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "systemApp", nIsSystemApp));

    napi_value nEnabled;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.enabled, &nEnabled));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "enabled", nEnabled));

    napi_value nRemovable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.removable, &nRemovable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "removable", nRemovable));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "label", nLabel));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "labelId", nLabelId));

    napi_value nIconPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.iconPath.c_str(), NAPI_AUTO_LENGTH, &nIconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "icon", nIconPath));

    napi_value nIconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.iconId, &nIconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "iconId", nIconId));

    napi_value nSupportedModes;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.supportedModes, &nSupportedModes));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "supportedModes", nSupportedModes));

    napi_value nProcess;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.process.c_str(), NAPI_AUTO_LENGTH, &nProcess));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "process", nProcess));

    napi_value nModuleSourceDirs;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleSourceDirs));
    for (size_t idx = 0; idx < appInfo.moduleSourceDirs.size(); idx++) {
        napi_value nModuleSourceDir;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, appInfo.moduleSourceDirs[idx].c_str(),
                                    NAPI_AUTO_LENGTH, &nModuleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleSourceDirs, idx, nModuleSourceDir));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "moduleSourceDirs", nModuleSourceDirs));

    napi_value nPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nPermissions));
    for (size_t idx = 0; idx < appInfo.permissions.size(); idx++) {
        napi_value nPermission;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, appInfo.permissions[idx].c_str(), NAPI_AUTO_LENGTH, &nPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nPermissions, idx, nPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "permissions", nPermissions));

    napi_value nModuleInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleInfos));
    for (size_t idx = 0; idx < appInfo.moduleInfos.size(); idx++) {
        napi_value objModuleInfos;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objModuleInfos));

        napi_value nModuleName;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env,
                                         appInfo.moduleInfos[idx].moduleName.c_str(),
                                         NAPI_AUTO_LENGTH, &nModuleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objModuleInfos, "moduleName", nModuleName));

        napi_value nModuleSourceDir;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env,
                                         appInfo.moduleInfos[idx].moduleSourceDir.c_str(),
                                         NAPI_AUTO_LENGTH, &nModuleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objModuleInfos, "moduleSourceDir", nModuleSourceDir));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleInfos, idx, objModuleInfos));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "moduleInfos", nModuleInfos));

    napi_value nEntryDir;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.entryDir.c_str(), NAPI_AUTO_LENGTH, &nEntryDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "entryDir", nEntryDir));

    napi_value nFlags;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.flags, &nFlags));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "flags", nFlags));

    APP_LOGI("ConvertApplicationInfo entryDir=%{public}s.", appInfo.entryDir.c_str());
}

static void ConvertElementName(napi_env env, napi_value elementInfo, const OHOS::AppExecFwk::ElementName &elementName)
{
    // wrap deviceId
    napi_value deviceId;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &deviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, elementInfo, "deviceId", deviceId));

    // wrap bundleName
    napi_value bundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetBundleName().c_str(), NAPI_AUTO_LENGTH, &bundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, elementInfo, "bundleName", bundleName));

    // wrap abilityName
    napi_value abilityName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &abilityName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, elementInfo, "abilityName", abilityName));
}

static void ConvertLauncherAbilityInfo(napi_env env, napi_value objAbilityInfo,
    const OHOS::AppExecFwk::LauncherAbilityInfo &launcherAbilityInfo)
{
    // wrap labelId
    napi_value labelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, launcherAbilityInfo.labelId, &labelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "labelId", labelId));

    // wrap iconId
    napi_value iconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, launcherAbilityInfo.iconId, &iconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "iconId", iconId));

    // wrap userId
    napi_value userId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, launcherAbilityInfo.userId, &userId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "userId", userId));

    // wrap installTime
    napi_value installTime;
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, launcherAbilityInfo.installTime, &installTime));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "installTime", installTime));

    // wrap elementName
    napi_value elementName;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &elementName));
    ConvertElementName(env, elementName, launcherAbilityInfo.elementName);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "elementName", elementName));

    // wrap application
    napi_value appInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &appInfo));
    ConvertApplicationInfo(env, appInfo, launcherAbilityInfo.applicationInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "applicationInfo", appInfo));
}

static bool ParseLauncherAbilityInfo(
    napi_env env, napi_value result,
    std::vector<OHOS::AppExecFwk::LauncherAbilityInfo> &launcherAbilityInfos)
{
    if (launcherAbilityInfos.empty()) {
        return false;
    }
    size_t index = 0;
    for (const auto& abilityInfo : launcherAbilityInfos) {
        napi_value objAbilityInfo = nullptr;
        napi_create_object(env, &objAbilityInfo);
        ConvertLauncherAbilityInfo(env, objAbilityInfo, abilityInfo);
        napi_set_element(env, result, index, objAbilityInfo);
        index++;
    }
    return true;
}

static void ConvertShortcutIntent(napi_env env, napi_value objShortcutInfo,
                                  const OHOS::AppExecFwk::ShortcutIntent &shortcutIntent)
{
    napi_value nTargetBundle;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, shortcutIntent.targetBundle.c_str(), NAPI_AUTO_LENGTH, &nTargetBundle));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "targetBundle", nTargetBundle));
    napi_value nTargetClass;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, shortcutIntent.targetClass.c_str(), NAPI_AUTO_LENGTH, &nTargetClass));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "targetClass", nTargetClass));
}

static void ConvertShortcutInfo(
    napi_env env, napi_value objShortcutInfo,
    const OHOS::AppExecFwk::ShortcutInfo &shortcutInfo)
{
    // wrap id
    napi_value shortId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env,
                                                       shortcutInfo.id.c_str(), NAPI_AUTO_LENGTH, &shortId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "id", shortId));

    // wrap bundleName
    napi_value bundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, shortcutInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "bundleName", bundleName));

    // wrap hostAbility
    napi_value hostAbility;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, shortcutInfo.hostAbility.c_str(), NAPI_AUTO_LENGTH, &hostAbility));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "hostAbility", hostAbility));

    // wrap icon
    napi_value icon;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, shortcutInfo.icon.c_str(), NAPI_AUTO_LENGTH, &icon));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "icon", icon));

    // wrap iconId
    napi_value iconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, shortcutInfo.iconId, &iconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "iconId", iconId));

    // wrap label
    napi_value label;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, shortcutInfo.label.c_str(), NAPI_AUTO_LENGTH, &label));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "label", label));

    // wrap labelId
    napi_value labelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, shortcutInfo.labelId, &labelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "labelId", labelId));

    // wrap disableMessage
    napi_value disableMessage;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, shortcutInfo.disableMessage.c_str(), NAPI_AUTO_LENGTH, &disableMessage));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "disableMessage", disableMessage));

    // wrap wants
    napi_value intents;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &intents));
    for (size_t index = 0; index < shortcutInfo.intents.size(); ++index) {
        napi_value intent;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &intent));
        ConvertShortcutIntent(env, intent, shortcutInfo.intents[index]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, intents, index, intent));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "wants", intents));

    // wrap isStatic
    napi_value isStatic;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, shortcutInfo.isStatic, &isStatic));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "isStatic", isStatic));

    // wrap isHomeShortcut
    napi_value isHomeShortcut;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, shortcutInfo.isHomeShortcut, &isHomeShortcut));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "isHomeShortcut", isHomeShortcut));

    // wrap isEnabled
    napi_value isEnabled;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, shortcutInfo.isEnables, &isEnabled));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objShortcutInfo, "isEnabled", isEnabled));
}

static void ParseShortcutInfo(napi_env env, napi_value result,
                              const std::vector<OHOS::AppExecFwk::ShortcutInfo> &shortcutInfos)
{
    if (shortcutInfos.empty()) {
        APP_LOGE("launcher shortcut info is empty");
        return;
    }
    size_t index = 0;
    for (const auto& item : shortcutInfos) {
        napi_value objShortcutInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objShortcutInfo));
        ConvertShortcutInfo(env, objShortcutInfo, item);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, objShortcutInfo));
        index++;
    }
    return;
}


static bool InnerJSLauncherServiceOn(napi_env env, OHOS::sptr<BundleStatusCallback> callbackRef)
{
    if (!callbackRef) {
        APP_LOGE("Input null BundleStatusCallback");
    }
    auto launcher = GetLauncherService();
    if (!launcher) {
        APP_LOGE("can not get launcher");
        return false;
    }

    auto result = launcher->RegisterCallback(callbackRef);
    if (!result) {
        APP_LOGE("RegisterBundleStatusCallback call error");
        return false;
    }
    return true;
}

static napi_value JSLauncherServiceOn(napi_env env, napi_callback_info info)
{
    size_t requireArgc = 2;
    size_t argc = 3;
    napi_value argv[3] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 2 parameter");
    std::string command;

    AsyncHandleBundleContext *asyncCallbackInfo = new AsyncHandleBundleContext();
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    asyncCallbackInfo->env = env;
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_string)) {
            ParseString(env, argv[i], command);
        } else if ((i == INDEX_ONE) && (valueType == napi_object)) {
            bool res = ParseBundleStatusCallback(env, asyncCallbackInfo->bundleStatusCallback, argv[i]);
            if (!res) {
                asyncCallbackInfo->err = OPERATION_TYPE_MIAMATCH;
                asyncCallbackInfo->message = "type mismatch";
            }
        } else if ((i == INDEX_TWO) && (valueType == napi_function))  {
            napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callbackRef);
            break;
        } else {
            asyncCallbackInfo->err = OPERATION_TYPE_MIAMATCH;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    napi_value promise = nullptr;
    if (command != REGISTERCALLBACK) {
        APP_LOGE("Input wrong command");
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
        return promise;
    }

    if (asyncCallbackInfo->callbackRef == nullptr) {
        napi_create_promise(env, &asyncCallbackInfo->deferred, &promise);
    } else {
        napi_get_undefined(env,  &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSLauncherServiceOn", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerJSLauncherServiceOn(env, asyncCallbackInfo->bundleStatusCallback);
            }
            napi_value result[2] = { 0 };
            // wrap result
            if (asyncCallbackInfo->err) {
                napi_create_uint32(env, asyncCallbackInfo->err, &result[0]);
                napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(), NAPI_AUTO_LENGTH, &result[1]);
            } else {
                if (asyncCallbackInfo->ret) {
                    napi_create_uint32(env, OPERATION_SUCESS, &result[0]);
                    napi_create_string_utf8(env, "register success", NAPI_AUTO_LENGTH, &result[1]);
                } else {
                    napi_create_uint32(env, OPERATION_FAILED, &result[0]);
                    napi_create_string_utf8(env, "register failed", NAPI_AUTO_LENGTH, &result[1]);
                }
            }
            // callback or promise return
            if (asyncCallbackInfo->deferred) {
                if (asyncCallbackInfo->ret) {
                    napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]);
                } else {
                    napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]);
                }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback);
                napi_call_function(env, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder);
                napi_delete_reference(env, asyncCallbackInfo->callbackRef);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);

    return promise;
}

static bool InnerJSLauncherServiceOff()
{
    auto launcher = GetLauncherService();
    if (!launcher) {
        APP_LOGE("can not get launcher");
        return false;
    }

    auto result = launcher->UnRegisterCallback();
    if (!result) {
        APP_LOGE("RegisterBundleStatusCallback call error");
        return false;
    }
    return true;
}

static napi_value JSLauncherServiceOff(napi_env env, napi_callback_info info)
{
    size_t requireArgc = INDEX_ONE;
    size_t argc = INDEX_TWO;
    napi_value argv[INDEX_TWO] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 1 parameter");
    std::string command;
    AsyncHandleBundleContext *asyncCallbackInfo = new AsyncHandleBundleContext();
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    asyncCallbackInfo->env = env;
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_string)) {
            ParseString(env, argv[i], command);
        } else if ((i == INDEX_ONE) && (valueType == napi_function))  {
            napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callbackRef);
        } else {
            NAPI_ASSERT(env, false, "type mismatch");
        }
    }

    napi_value promise = nullptr;
    if (command != UNREGISTERCALLBACK) {
        APP_LOGE("Input wrong command");
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
        return promise;
    }

    if (asyncCallbackInfo->callbackRef == nullptr) {
        napi_create_promise(env, &asyncCallbackInfo->deferred, &promise);
    } else {
        napi_get_undefined(env,  &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSLauncherServiceOn", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerJSLauncherServiceOff();
            }
            napi_value result[INDEX_TWO] = { 0 };
            if (asyncCallbackInfo->ret) {
                napi_create_uint32(env, 0, &result[0]);
                napi_create_string_utf8(env, "unregister success", NAPI_AUTO_LENGTH, &result[INDEX_ONE]);
            } else {
                napi_create_uint32(env, INDEX_ONE, &result[0]);
                napi_create_string_utf8(env, "unregister failed", NAPI_AUTO_LENGTH, &result[INDEX_ONE]);
            }
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[INDEX_ONE]);
              } else {
                  napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]);
              }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback);
                napi_call_function(env, nullptr, callback, sizeof(result) / sizeof(result[0]), result, &placeHolder);
                napi_delete_reference(env, asyncCallbackInfo->callbackRef);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);

    return promise;
}

static bool InnerJSGetAllLauncherAbilityInfos(napi_env env, uint32_t userId,
    std::vector<OHOS::AppExecFwk::LauncherAbilityInfo> &launcherAbilityInfos)
{
    auto launcher = GetLauncherService();
    if (!launcher) {
        APP_LOGE("can not get launcher");
        return false;
    }

    auto result = launcher->GetAllLauncherAbilityInfos(userId, launcherAbilityInfos);
    if (!result) {
        APP_LOGE("GetAllLauncherAbilityInfos call error");
        return false;
    }
    return true;
}

static napi_value JSGetAllLauncherAbilityInfos(napi_env env, napi_callback_info info)
{
    size_t argc = INDEX_TWO;
    napi_value argv[INDEX_TWO] = { 0 };
    size_t requireArgc = INDEX_ONE;
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 1 parameter");
    AsyncHandleBundleContext *asyncCallbackInfo = new AsyncHandleBundleContext();
    asyncCallbackInfo->env = env;
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_number)) {
            napi_get_value_int32(env, argv[i], &asyncCallbackInfo->userId);
        } else if ((i == INDEX_ONE) && (valueType == napi_function))  {
            napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callbackRef);
            break;
        } else {
            asyncCallbackInfo->err = OPERATION_TYPE_MIAMATCH;
            asyncCallbackInfo->message = "type mismatch";
        }
    }
    napi_value promise = nullptr;
    if (asyncCallbackInfo->callbackRef == nullptr) {
        napi_create_promise(env, &asyncCallbackInfo->deferred, &promise);
    } else {
        napi_get_undefined(env,  &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSGetAllLauncherAbilityInfos", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerJSGetAllLauncherAbilityInfos(asyncCallbackInfo->env,
                                                                           asyncCallbackInfo->userId,
                                                                           asyncCallbackInfo->launcherAbilityInfos);
            }
        },
        [](napi_env env, napi_status status, void* data) {
          AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
          napi_value result[INDEX_TWO] = { 0 };
          // wrap result
          if (asyncCallbackInfo->err) {
              napi_create_int32(env, asyncCallbackInfo->err, &result[0]);
              napi_get_undefined(env, &result[INDEX_ONE]);
          } else {
              if (asyncCallbackInfo->ret) {
                  napi_create_int32(env, OPERATION_SUCESS, &result[0]);
                  napi_create_array(env, &result[INDEX_ONE]);
                  ParseLauncherAbilityInfo(env, result[INDEX_ONE], asyncCallbackInfo->launcherAbilityInfos);
              } else {
                  napi_create_int32(env, OPERATION_FAILED, &result[0]);
                  napi_get_undefined(env, &result[INDEX_ONE]);
              }
          }
          // return callback or promise
          if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[INDEX_ONE]);
              } else {
                  napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]);
              }
          } else {
              napi_value callback = nullptr;
              napi_value callResult = 0;
              napi_value undefined = 0;
              napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback);
              napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[0]), result, &callResult);
              napi_delete_reference(env, asyncCallbackInfo->callbackRef);
          }
          napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
          delete asyncCallbackInfo;
          asyncCallbackInfo = nullptr;
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);

    return promise;
}

static bool InnerJSGetLauncherAbilityInfos(napi_env env, std::string& bundleName,
    uint32_t userId, std::vector<OHOS::AppExecFwk::LauncherAbilityInfo>& launcherAbilityInfos)
{
    auto launcher = GetLauncherService();
    if (!launcher) {
        APP_LOGE("can not get launcher");
        return false;
    }

    auto result = launcher->GetAbilityList(bundleName, userId, launcherAbilityInfos);
    if (!result) {
        APP_LOGE("GetAbilityList call error");
        return false;
    }
    return true;
}

static napi_value JSGetLauncherAbilityInfos(napi_env env, napi_callback_info info)
{
    size_t argc = INDEX_THREE;
    napi_value argv[INDEX_THREE] = { 0 };
    size_t requireArgc = INDEX_TWO;
    napi_value thisArg = nullptr;
    void *data = nullptr;
    std::string bundleName;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 2 parameter");
    AsyncHandleBundleContext *asyncCallbackInfo = new AsyncHandleBundleContext();
    asyncCallbackInfo->env = env;

    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_string)) {
            ParseString(env, argv[i], asyncCallbackInfo->bundleName);
        } else if ((i == INDEX_ONE) && (valueType == napi_number)) {
            napi_get_value_int32(env, argv[i], &asyncCallbackInfo->userId);
        } else if ((i == INDEX_TWO) && (valueType == napi_function)) {
            napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callbackRef);
            break;
        } else {
            asyncCallbackInfo->err = OPERATION_TYPE_MIAMATCH;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callbackRef == nullptr) {
        napi_create_promise(env, &asyncCallbackInfo->deferred, &promise);
    } else {
        napi_get_undefined(env,  &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSGetLauncherAbilityInfos", NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            if (!asyncCallbackInfo->err) {
              asyncCallbackInfo->ret = InnerJSGetLauncherAbilityInfos(asyncCallbackInfo->env,
                                                                      asyncCallbackInfo->bundleName,
                                                                      asyncCallbackInfo->userId,
                                                                      asyncCallbackInfo->launcherAbilityInfos);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            napi_value result[INDEX_TWO] = { 0 };
            // wrap result
            if (asyncCallbackInfo->err) {
                napi_create_int32(env, asyncCallbackInfo->err, &result[0]);
                napi_get_undefined(env, &result[INDEX_ONE]);
            } else {
                if (asyncCallbackInfo->ret) {
                  napi_create_uint32(env, OPERATION_SUCESS, &result[0]);
                  napi_create_array(env, &result[INDEX_ONE]);
                  ParseLauncherAbilityInfo(env, result[INDEX_ONE], asyncCallbackInfo->launcherAbilityInfos);
                } else {
                  napi_create_uint32(env, OPERATION_FAILED, &result[0]);
                  napi_get_undefined(env, &result[INDEX_ONE]);
                }
            }
            // return callback or promise
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[INDEX_ONE]);
              } else {
                  napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]);
              }
            } else {
                napi_value callback = nullptr;
                napi_value callResult = 0;
                napi_value undefined = 0;
                napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback);
                napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[0]), result, &callResult);
                napi_delete_reference(env, asyncCallbackInfo->callbackRef);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    return promise;
}

static bool InnerJSGetShortcutInfos(napi_env env, const std::string& bundleName,
    std::vector<OHOS::AppExecFwk::ShortcutInfo>& shortcutInfos)
{
    auto launcher = GetLauncherService();
    if (!launcher) {
        APP_LOGE("can not get launcher");
        return false;
    }
    auto result = launcher->GetShortcutInfos(bundleName, shortcutInfos);
    if (!result) {
        APP_LOGE("GetShortcutInfos call error");
        return false;
    }
    return true;
}

static napi_value JSGetShortcutInfos(napi_env env, napi_callback_info info)
{
    size_t argc = INDEX_TWO;
    napi_value argv[INDEX_TWO] = { 0 };
    size_t requireArgc = INDEX_ONE;
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 1 parameter");
    AsyncHandleBundleContext *asyncCallbackInfo = new AsyncHandleBundleContext();
    asyncCallbackInfo->env = env;

    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_string)) {
            ParseString(env, argv[i], asyncCallbackInfo->bundleName);
        } else if ((i == INDEX_ONE) && (valueType == napi_function)) {
            napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callbackRef);
            break;
        } else {
            asyncCallbackInfo->err = OPERATION_TYPE_MIAMATCH;
            asyncCallbackInfo->message = "type mismatch";
        }
    }
    napi_value promise = nullptr;
    if (asyncCallbackInfo->callbackRef == nullptr) {
        napi_create_promise(env, &asyncCallbackInfo->deferred, &promise);
    } else {
        napi_get_undefined(env,  &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(env, "JSGetShortcutInfos", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerJSGetShortcutInfos(asyncCallbackInfo->env,
                                                                 asyncCallbackInfo->bundleName,
                                                                 asyncCallbackInfo->shortcutInfos);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            napi_value result[INDEX_TWO] = { 0 };
            // wrap result
            if (asyncCallbackInfo->err) {
                napi_create_int32(env, asyncCallbackInfo->err, &result[0]);
                napi_get_undefined(env, &result[INDEX_ONE]);
            } else {
                if (asyncCallbackInfo->ret) {
                    napi_create_uint32(env, OPERATION_SUCESS, &result[0]);
                    napi_create_array(env, &result[INDEX_ONE]);
                    ParseShortcutInfo(env, result[INDEX_ONE], asyncCallbackInfo->shortcutInfos);
                } else {
                    napi_create_uint32(env, OPERATION_FAILED, &result[0]);
                    napi_get_undefined(env, &result[INDEX_ONE]);
                }
            }
            // return callback or promise
            if (asyncCallbackInfo->deferred) {
                if (asyncCallbackInfo->ret) {
                    napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]);
                } else {
                    napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]);
                }
            } else {
                napi_value callback = nullptr;
                napi_value callResult = 0;
                napi_value undefined = 0;
                napi_get_reference_value(env, asyncCallbackInfo->callbackRef, &callback);
                napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[0]), result, &callResult);
                napi_delete_reference(env, asyncCallbackInfo->callbackRef);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);

    return promise;
}

static napi_value LauncherServiceExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor launcherDesc[] = {
        DECLARE_NAPI_FUNCTION("on", JSLauncherServiceOn),
        DECLARE_NAPI_FUNCTION("off", JSLauncherServiceOff),
        DECLARE_NAPI_FUNCTION("getAllLauncherAbilityInfos", JSGetAllLauncherAbilityInfos),
        DECLARE_NAPI_FUNCTION("getLauncherAbilityInfos", JSGetLauncherAbilityInfos),
        DECLARE_NAPI_FUNCTION("getShortcutInfos", JSGetShortcutInfos),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(launcherDesc) / sizeof(launcherDesc[0]), launcherDesc));
    return exports;
}

static napi_module launcherServiceModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = LauncherServiceExport,
    .nm_modname = "bundle.innerBundleManager",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void LauncherServiceRegister()
{
    napi_module_register(&launcherServiceModule);
}

