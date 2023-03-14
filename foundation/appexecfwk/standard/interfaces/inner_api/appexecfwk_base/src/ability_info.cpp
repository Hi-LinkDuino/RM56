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

#include "ability_info.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "bundle_constants.h"
#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_PACKAGE = "package";
const std::string JSON_KEY_NAME = "name";
const std::string JSON_KEY_BUNDLE_NAME = "bundleName";
const std::string JSON_KEY_APPLICATION_NAME = "applicationName";
const std::string JSON_KEY_LABEL = "label";
const std::string JSON_KEY_DESCRIPTION = "description";
const std::string JSON_KEY_ICON_PATH = "iconPath";
const std::string JSON_KEY_THEME = "theme";
const std::string JSON_KEY_VISIBLE = "visible";
const std::string JSON_KEY_KIND = "kind";
const std::string JSON_KEY_TYPE = "type";
const std::string JSON_KEY_EXTENSION_ABILITY_TYPE = "extensionAbilityType";
const std::string JSON_KEY_ORIENTATION = "orientation";
const std::string JSON_KEY_LAUNCH_MODE = "launchMode";
const std::string JSON_KEY_CODE_PATH = "codePath";
const std::string JSON_KEY_RESOURCE_PATH = "resourcePath";
const std::string JSON_KEY_PERMISSIONS = "permissions";
const std::string JSON_KEY_PROCESS = "process";
const std::string JSON_KEY_DEVICE_TYPES = "deviceTypes";
const std::string JSON_KEY_DEVICE_CAPABILITIES = "deviceCapabilities";
const std::string JSON_KEY_URI = "uri";
const std::string JSON_KEY_MODULE_NAME = "moduleName";
const std::string JSON_KEY_IS_LAUNCHER_ABILITY = "isLauncherAbility";
const std::string JSON_KEY_IS_NATIVE_ABILITY = "isNativeAbility";
const std::string JSON_KEY_ENABLED = "enabled";
const std::string JSON_KEY_SUPPORT_PIP_MODE = "supportPipMode";
const std::string JSON_KEY_TARGET_ABILITY = "targetAbility";
const std::string APPLICATION_INFO = "applicationInfo";
const std::string JSON_KEY_READ_PERMISSION = "readPermission";
const std::string JSON_KEY_WRITE_PERMISSION = "writePermission";
const std::string JSON_KEY_CONFIG_CHANGES = "configChanges";
const std::string JSON_KEY_FORM = "form";
const std::string JSON_KEY_FORM_ENTITY = "formEntity";
const std::string JSON_KEY_MIN_FORM_HEIGHT = "minFormHeight";
const std::string JSON_KEY_DEFAULT_FORM_HEIGHT = "defaultFormHeight";
const std::string JSON_KEY_MIN_FORM_WIDTH = "minFormWidth";
const std::string JSON_KEY_DEFAULT_FORM_WIDTH = "defaultFormWidth";
const std::string JSON_KEY_BACKGROUND_MODES = "backgroundModes";
const std::string JSON_KEY_CUSTOMIZE_DATA = "customizeData";
const std::string JSON_KEY_META_DATA = "metaData";
const std::string JSON_KEY_META_VALUE = "value";
const std::string JSON_KEY_META_EXTRA = "extra";
const std::string JSON_KEY_LABEL_ID = "labelId";
const std::string JSON_KEY_DESCRIPTION_ID = "descriptionId";
const std::string JSON_KEY_ICON_ID = "iconId";
const std::string JSON_KEY_FORM_ENABLED = "formEnabled";
const std::string JSON_KEY_SRC_PATH = "srcPath";
const std::string JSON_KEY_SRC_LANGUAGE = "srcLanguage";
const std::string JSON_KEY_START_WINDOW_ICON = "startWindowIcon";
const std::string JSON_KEY_START_WINDOW_ICON_ID = "startWindowIconId";
const std::string JSON_KEY_START_WINDOW_BACKGROUND = "startWindowBackground";
const std::string JSON_KEY_START_WINDOW_BACKGROUND_ID = "startWindowBackgroundId";
const std::string META_DATA = "metadata";
const std::string META_DATA_NAME = "name";
const std::string META_DATA_VALUE = "value";
const std::string META_DATA_RESOURCE = "resource";
const std::string SRC_ENTRANCE = "srcEntrance";
const std::string IS_MODULE_JSON = "isModuleJson";
const std::string IS_STAGE_BASED_MODEL = "isStageBasedModel";
const std::string CONTINUABLE = "continuable";
const std::string PRIORITY = "priority";
}  // namespace

bool AbilityInfo::ReadFromParcel(Parcel &parcel)
{
    MessageParcel *messageParcel = reinterpret_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        APP_LOGE("Type conversion failed");
        return false;
    }
    uint32_t length = messageParcel->ReadUint32();
    if (length == 0) {
        APP_LOGE("Invalid data length");
        return false;
    }
    const char *data = reinterpret_cast<const char *>(messageParcel->ReadRawData(length));
    if (!data) {
        APP_LOGE("Fail to read raw data, length = %{public}d", length);
        return false;
    }
    nlohmann::json jsonObject = nlohmann::json::parse(data, nullptr, false);
    if (jsonObject.is_discarded()) {
        APP_LOGE("failed to parse ApplicationInfo");
        return false;
    }
    *this = jsonObject.get<AbilityInfo>();
    return true;
}

AbilityInfo *AbilityInfo::Unmarshalling(Parcel &parcel)
{
    AbilityInfo *info = new (std::nothrow) AbilityInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool AbilityInfo::Marshalling(Parcel &parcel) const
{
    MessageParcel *messageParcel = reinterpret_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        APP_LOGE("Type conversion failed");
        return false;
    }
    nlohmann::json json = *this;
    std::string str = json.dump();
    if (!messageParcel->WriteUint32(str.size() + 1)) {
        APP_LOGE("Failed to write data size");
        return false;
    }
    if (!messageParcel->WriteRawData(str.c_str(), str.size() + 1)) {
        APP_LOGE("Failed to write data");
        return false;
    }
    return true;
}

void AbilityInfo::Dump(std::string prefix, int fd)
{
    APP_LOGI("called dump Abilityinfo");
    if (fd < 0) {
        APP_LOGE("dump Abilityinfo fd error");
        return;
    }
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        APP_LOGE("dump Abilityinfo fcntl error, errno : %{public}d", errno);
        return;
    }
    uint uflags = static_cast<uint>(flags);
    uflags &= O_ACCMODE;
    if ((uflags == O_WRONLY) || (uflags == O_RDWR)) {
        nlohmann::json jsonObject = *this;
        std::string result;
        result.append(prefix);
        result.append(jsonObject.dump(Constants::DUMP_INDENT));
        int ret = TEMP_FAILURE_RETRY(write(fd, result.c_str(), result.size()));
        if (ret < 0) {
            APP_LOGE("dump Abilityinfo write error, errno : %{public}d", errno);
        }
    }
    return;
}

void to_json(nlohmann::json &jsonObject, const CustomizeData &customizeData)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_NAME, customizeData.name},
        {JSON_KEY_META_VALUE, customizeData.value},
        {JSON_KEY_META_EXTRA, customizeData.extra}
    };
}

void to_json(nlohmann::json &jsonObject, const MetaData &metaData)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_CUSTOMIZE_DATA, metaData.customizeData}
    };
}

void to_json(nlohmann::json &jsonObject, const Metadata &metadata)
{
    jsonObject = nlohmann::json {
        {META_DATA_NAME, metadata.name},
        {META_DATA_VALUE, metadata.value},
        {META_DATA_RESOURCE, metadata.resource}
    };
}

void to_json(nlohmann::json &jsonObject, const AbilityInfo &abilityInfo)
{
    APP_LOGD("AbilityInfo to_json begin");
    jsonObject = nlohmann::json {
        {JSON_KEY_NAME, abilityInfo.name},
        {JSON_KEY_LABEL, abilityInfo.label},
        {JSON_KEY_DESCRIPTION, abilityInfo.description},
        {JSON_KEY_ICON_PATH, abilityInfo.iconPath},
        {JSON_KEY_LABEL_ID, abilityInfo.labelId},
        {JSON_KEY_DESCRIPTION_ID, abilityInfo.descriptionId},
        {JSON_KEY_ICON_ID, abilityInfo.iconId},
        {JSON_KEY_THEME, abilityInfo.theme},
        {JSON_KEY_VISIBLE, abilityInfo.visible},
        {JSON_KEY_KIND, abilityInfo.kind},
        {JSON_KEY_TYPE, abilityInfo.type},
        {JSON_KEY_EXTENSION_ABILITY_TYPE, abilityInfo.extensionAbilityType},
        {JSON_KEY_ORIENTATION, abilityInfo.orientation},
        {JSON_KEY_LAUNCH_MODE, abilityInfo.launchMode},
        {JSON_KEY_SRC_PATH, abilityInfo.srcPath},
        {JSON_KEY_SRC_LANGUAGE, abilityInfo.srcLanguage},
        {JSON_KEY_PERMISSIONS, abilityInfo.permissions},
        {JSON_KEY_PROCESS, abilityInfo.process},
        {JSON_KEY_DEVICE_TYPES, abilityInfo.deviceTypes},
        {JSON_KEY_DEVICE_CAPABILITIES, abilityInfo.deviceCapabilities},
        {JSON_KEY_URI, abilityInfo.uri},
        {JSON_KEY_TARGET_ABILITY, abilityInfo.targetAbility},
        {APPLICATION_INFO, abilityInfo.applicationInfo},
        {JSON_KEY_IS_LAUNCHER_ABILITY, abilityInfo.isLauncherAbility},
        {JSON_KEY_IS_NATIVE_ABILITY, abilityInfo.isNativeAbility},
        {JSON_KEY_ENABLED, abilityInfo.enabled},
        {JSON_KEY_SUPPORT_PIP_MODE, abilityInfo.supportPipMode},
        {JSON_KEY_FORM_ENABLED, abilityInfo.formEnabled},
        {JSON_KEY_READ_PERMISSION, abilityInfo.readPermission},
        {JSON_KEY_WRITE_PERMISSION, abilityInfo.writePermission},
        {JSON_KEY_CONFIG_CHANGES, abilityInfo.configChanges},
        {JSON_KEY_FORM_ENTITY, abilityInfo.formEntity},
        {JSON_KEY_MIN_FORM_HEIGHT, abilityInfo.minFormHeight},
        {JSON_KEY_DEFAULT_FORM_HEIGHT, abilityInfo.defaultFormHeight},
        {JSON_KEY_MIN_FORM_WIDTH, abilityInfo.minFormWidth},
        {JSON_KEY_DEFAULT_FORM_WIDTH, abilityInfo.defaultFormWidth},
        {JSON_KEY_META_DATA, abilityInfo.metaData},
        {JSON_KEY_BACKGROUND_MODES, abilityInfo.backgroundModes},
        {JSON_KEY_PACKAGE, abilityInfo.package},
        {JSON_KEY_BUNDLE_NAME, abilityInfo.bundleName},
        {JSON_KEY_MODULE_NAME, abilityInfo.moduleName},
        {JSON_KEY_APPLICATION_NAME, abilityInfo.applicationName},
        {JSON_KEY_CODE_PATH, abilityInfo.codePath},
        {JSON_KEY_RESOURCE_PATH, abilityInfo.resourcePath},
        {SRC_ENTRANCE, abilityInfo.srcEntrance},
        {META_DATA, abilityInfo.metadata},
        {IS_MODULE_JSON, abilityInfo.isModuleJson},
        {IS_STAGE_BASED_MODEL, abilityInfo.isStageBasedModel},
        {CONTINUABLE, abilityInfo.continuable},
        {PRIORITY, abilityInfo.priority},
        {JSON_KEY_START_WINDOW_ICON, abilityInfo.startWindowIcon},
        {JSON_KEY_START_WINDOW_ICON_ID, abilityInfo.startWindowIconId},
        {JSON_KEY_START_WINDOW_BACKGROUND, abilityInfo.startWindowBackground},
        {JSON_KEY_START_WINDOW_BACKGROUND_ID, abilityInfo.startWindowBackgroundId}
    };
}

void from_json(const nlohmann::json &jsonObject, CustomizeData &customizeData)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_NAME,
        customizeData.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_META_VALUE,
        customizeData.value,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_META_EXTRA,
        customizeData.extra,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, MetaData &metaData)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::vector<CustomizeData>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CUSTOMIZE_DATA,
        metaData.customizeData,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, Metadata &metadata)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        META_DATA_NAME,
        metadata.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        META_DATA_VALUE,
        metadata.value,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        META_DATA_RESOURCE,
        metadata.resource,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read Ability Metadata from database error, error code : %{public}d", parseResult);
    }
}

void from_json(const nlohmann::json &jsonObject, AbilityInfo &abilityInfo)
{
    APP_LOGD("AbilityInfo from_json begin");
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_NAME,
        abilityInfo.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_LABEL,
        abilityInfo.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DESCRIPTION,
        abilityInfo.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ICON_PATH,
        abilityInfo.iconPath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_LABEL_ID,
        abilityInfo.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DESCRIPTION_ID,
        abilityInfo.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ICON_ID,
        abilityInfo.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_THEME,
        abilityInfo.theme,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_VISIBLE,
        abilityInfo.visible,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_KIND,
        abilityInfo.kind,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<AbilityType>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TYPE,
        abilityInfo.type,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<ExtensionAbilityType>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_EXTENSION_ABILITY_TYPE,
        abilityInfo.extensionAbilityType,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<DisplayOrientation>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ORIENTATION,
        abilityInfo.orientation,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<LaunchMode>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_LAUNCH_MODE,
        abilityInfo.launchMode,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_SRC_PATH,
        abilityInfo.srcPath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_SRC_LANGUAGE,
        abilityInfo.srcLanguage,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PERMISSIONS,
        abilityInfo.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PROCESS,
        abilityInfo.process,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DEVICE_TYPES,
        abilityInfo.deviceTypes,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DEVICE_CAPABILITIES,
        abilityInfo.deviceCapabilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_URI,
        abilityInfo.uri,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TARGET_ABILITY,
        abilityInfo.targetAbility,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<ApplicationInfo>(jsonObject,
        jsonObjectEnd,
        APPLICATION_INFO,
        abilityInfo.applicationInfo,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_IS_LAUNCHER_ABILITY,
        abilityInfo.isLauncherAbility,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_IS_NATIVE_ABILITY,
        abilityInfo.isNativeAbility,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ENABLED,
        abilityInfo.enabled,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_SUPPORT_PIP_MODE,
        abilityInfo.supportPipMode,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_FORM_ENABLED,
        abilityInfo.formEnabled,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_READ_PERMISSION,
        abilityInfo.readPermission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_WRITE_PERMISSION,
        abilityInfo.writePermission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CONFIG_CHANGES,
        abilityInfo.configChanges,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_FORM_ENTITY,
        abilityInfo.formEntity,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_MIN_FORM_HEIGHT,
        abilityInfo.minFormHeight,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DEFAULT_FORM_HEIGHT,
        abilityInfo.defaultFormHeight,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_MIN_FORM_WIDTH,
        abilityInfo.minFormWidth,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_DEFAULT_FORM_WIDTH,
        abilityInfo.defaultFormWidth,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<MetaData>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_META_DATA,
        abilityInfo.metaData,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BACKGROUND_MODES,
        abilityInfo.backgroundModes,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PACKAGE,
        abilityInfo.package,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_NAME,
        abilityInfo.bundleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_MODULE_NAME,
        abilityInfo.moduleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_APPLICATION_NAME,
        abilityInfo.applicationName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CODE_PATH,
        abilityInfo.codePath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_RESOURCE_PATH,
        abilityInfo.resourcePath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SRC_ENTRANCE,
        abilityInfo.srcEntrance,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        META_DATA,
        abilityInfo.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        IS_MODULE_JSON,
        abilityInfo.isModuleJson,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        IS_STAGE_BASED_MODEL,
        abilityInfo.isStageBasedModel,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        CONTINUABLE,
        abilityInfo.continuable,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PRIORITY,
        abilityInfo.priority,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_START_WINDOW_ICON,
        abilityInfo.startWindowIcon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_START_WINDOW_ICON_ID,
        abilityInfo.startWindowIconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_START_WINDOW_BACKGROUND,
        abilityInfo.startWindowBackground,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_START_WINDOW_BACKGROUND_ID,
        abilityInfo.startWindowBackgroundId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("AbilityInfo from_json error, error code : %{public}d", parseResult);
    }
}

void AbilityInfo::ConvertToCompatiableAbilityInfo(CompatibleAbilityInfo& compatibleAbilityInfo) const
{
    APP_LOGE("AbilityInfo::ConvertToCompatiableAbilityInfo called");
    compatibleAbilityInfo.package = package;
    compatibleAbilityInfo.name = name;
    compatibleAbilityInfo.label = label;
    compatibleAbilityInfo.description = description;
    compatibleAbilityInfo.iconPath = iconPath;
    compatibleAbilityInfo.uri = uri;
    compatibleAbilityInfo.moduleName = moduleName;
    compatibleAbilityInfo.process = process;
    compatibleAbilityInfo.targetAbility = targetAbility;
    compatibleAbilityInfo.appName = appName;
    compatibleAbilityInfo.privacyUrl = privacyUrl;
    compatibleAbilityInfo.privacyName = privacyName;
    compatibleAbilityInfo.downloadUrl = downloadUrl;
    compatibleAbilityInfo.versionName = versionName;
    compatibleAbilityInfo.backgroundModes = backgroundModes;
    compatibleAbilityInfo.packageSize = packageSize;
    compatibleAbilityInfo.visible = visible;
    compatibleAbilityInfo.formEnabled = formEnabled;
    compatibleAbilityInfo.multiUserShared = multiUserShared;
    compatibleAbilityInfo.type = type;
    compatibleAbilityInfo.subType = subType;
    compatibleAbilityInfo.orientation = orientation;
    compatibleAbilityInfo.launchMode = launchMode;
    compatibleAbilityInfo.permissions = permissions;
    compatibleAbilityInfo.deviceTypes = deviceTypes;
    compatibleAbilityInfo.deviceCapabilities = deviceCapabilities;
    compatibleAbilityInfo.supportPipMode = supportPipMode;
    compatibleAbilityInfo.grantPermission = grantPermission;
    compatibleAbilityInfo.readPermission = readPermission;
    compatibleAbilityInfo.writePermission = writePermission;
    compatibleAbilityInfo.uriPermissionMode = uriPermissionMode;
    compatibleAbilityInfo.uriPermissionPath = uriPermissionPath;
    compatibleAbilityInfo.directLaunch = directLaunch;
    compatibleAbilityInfo.bundleName = bundleName;
    compatibleAbilityInfo.className = className;
    compatibleAbilityInfo.originalClassName = originalClassName;
    compatibleAbilityInfo.deviceId = deviceId;
    CompatibleApplicationInfo convertedCompatibleApplicationInfo;
    applicationInfo.ConvertToCompatibleApplicationInfo(convertedCompatibleApplicationInfo);
    compatibleAbilityInfo.applicationInfo = convertedCompatibleApplicationInfo;
    compatibleAbilityInfo.formEntity = formEntity;
    compatibleAbilityInfo.minFormHeight = minFormHeight;
    compatibleAbilityInfo.defaultFormHeight = defaultFormHeight;
    compatibleAbilityInfo.minFormWidth = minFormWidth;
    compatibleAbilityInfo.defaultFormWidth = defaultFormWidth;
    compatibleAbilityInfo.iconId = iconId;
    compatibleAbilityInfo.labelId = labelId;
    compatibleAbilityInfo.descriptionId = descriptionId;
    compatibleAbilityInfo.enabled = enabled;
}
}  // namespace AppExecFwk
}  // namespace OHOS
