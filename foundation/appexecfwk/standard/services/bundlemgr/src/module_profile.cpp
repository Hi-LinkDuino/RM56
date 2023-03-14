/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "module_profile.h"

#include <sstream>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "common_profile.h"
#include "parameter.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace Profile {
thread_local int32_t parseResult;

const std::set<std::string> MODULE_TYPE_SET = {
    "entry",
    "feature",
    "har"
};

const std::set<std::string> DEVICE_TYPE_SET = {
    "phone",
    "tablet",
    "tv",
    "wearable",
    "liteWearable",
    "car",
    "smartVision",
    "router"
};

const std::set<std::string> VIRTUAL_MACHINE_SET = {
    "ark",
    "default"
};

const std::set<std::string> UI_SYNTAX_SET = {
    "hml",
    "ets"
};

const std::map<std::string, uint32_t> BACKGROUND_MODES_MAP = {
    {ProfileReader::KEY_DATA_TRANSFER, ProfileReader::VALUE_DATA_TRANSFER},
    {ProfileReader::KEY_AUDIO_PLAYBACK, ProfileReader::VALUE_AUDIO_PLAYBACK},
    {ProfileReader::KEY_AUDIO_RECORDING, ProfileReader::VALUE_AUDIO_RECORDING},
    {ProfileReader::KEY_LOCATION, ProfileReader::VALUE_LOCATION},
    {ProfileReader::KEY_BLUETOOTH_INTERACTION, ProfileReader::VALUE_BLUETOOTH_INTERACTION},
    {ProfileReader::KEY_MULTI_DEVICE_CONNECTION, ProfileReader::VALUE_MULTI_DEVICE_CONNECTION},
    {ProfileReader::KEY_WIFI_INTERACTION, ProfileReader::VALUE_WIFI_INTERACTION},
    {ProfileReader::KEY_VOIP, ProfileReader::VALUE_VOIP},
    {ProfileReader::KEY_TASK_KEEPING, ProfileReader::VALUE_TASK_KEEPING},
    {ProfileReader::KEY_PICTURE_IN_PICTURE, ProfileReader::VALUE_PICTURE_IN_PICTURE},
    {ProfileReader::KEY_SCREEN_FETCH, ProfileReader::VALUE_SCREEN_FETCH}
};

const std::vector<std::string> EXTENSION_TYPE_SET = {
    "form",
    "workScheduler",
    "inputMethod",
    "service",
    "accessibility",
    "dataShare",
    "fileShare",
    "staticSubscriber",
    "wallpaper",
    "backup",
    "window"
};

const std::set<std::string> ENTITY_TYPE_SET = {
    "game",
    "media",
    "communication",
    "news",
    "travel",
    "utility",
    "shopping",
    "education",
    "kids",
    "business",
    "photography",
    "unspecified"
};

const std::set<std::string> GRANT_MODE_SET = {
    "system_grant",
    "user_grant"
};

const std::set<std::string> AVAILABLE_LEVEL_SET = {
    "system_core",
    "system_basic",
    "normal"
};

const std::map<std::string, LaunchMode> LAUNCH_MODE_MAP = {
    {"singleton", LaunchMode::SINGLETON},
    {"standard", LaunchMode::STANDARD},
    {"specified", LaunchMode::SPECIFIED}
};

struct DeviceConfig {
    // pair first : if exist in module.json then true, otherwise false
    // pair second : actual value
    std::pair<bool, int32_t> minAPIVersion = std::make_pair<>(false, 0);
    std::pair<bool, bool> distributedNotificationEnabled = std::make_pair<>(false, false);
    std::pair<bool, bool> keepAlive = std::make_pair<>(false, false);
    std::pair<bool, bool> removable = std::make_pair<>(false, true);
    std::pair<bool, bool> singleton = std::make_pair<>(false, false);
    std::pair<bool, bool> userDataClearable = std::make_pair<>(false, true);
    std::pair<bool, bool> accessible = std::make_pair<>(false, true);
};

struct Metadata {
    std::string name;
    std::string value;
    std::string resource;
};

struct Ability {
    std::string name;
    std::string srcEntrance;
    std::string launchType = ABILITY_LAUNCH_TYPE_DEFAULT_VALUE;
    std::string description;
    int32_t descriptionId = 0;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    int32_t priority = 0;
    std::vector<std::string> permissions;
    std::vector<Metadata> metadata;
    bool visible = false;
    bool continuable = false;
    std::vector<Skill> skills;
    std::vector<std::string> backgroundModes;
    std::string startWindowIcon;
    int32_t startWindowIconId = 0;
    std::string startWindowBackground;
    int32_t startWindowBackgroundId = 0;
};

struct Extension {
    std::string name;
    std::string srcEntrance;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    std::string description;
    int32_t descriptionId = 0;
    int32_t priority = 0;
    std::string type;
    std::string readPermission;
    std::string writePermission;
    std::string uri;
    std::vector<std::string> permissions;
    bool visible = false;
    std::vector<Skill> skills;
    std::vector<Metadata> metadata;
};

struct App {
    std::string bundleName;
    bool debug = false;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    std::string description;
    int32_t descriptionId = 0;
    std::string vendor;
    int32_t versionCode = 0;
    std::string versionName;
    int32_t minCompatibleVersionCode = -1;
    int32_t minAPIVersion = 0;
    int32_t targetAPIVersion = 0;
    std::string apiReleaseType = APP_API_RELEASETYPE_DEFAULT_VALUE;
    bool distributedNotificationEnabled = true;
    std::string entityType = APP_ENTITY_TYPE_DEFAULT_VALUE;
    bool keepAlive = false;
    std::pair<bool, bool> removable = std::make_pair<>(false, true);
    bool singleton = false;
    bool userDataClearable = true;
    bool accessible = false;
    std::vector<std::string> targetBundleList;
    std::map<std::string, DeviceConfig> deviceConfigs;
};

struct Module {
    std::string name;
    std::string type;
    std::string srcEntrance;
    std::string description;
    int32_t descriptionId = 0;
    std::string process;
    std::string mainElement;
    std::vector<std::string> deviceTypes;
    bool deliveryWithInstall = false;
    bool installationFree = false;
    std::string virtualMachine = MODULE_VIRTUAL_MACHINE_DEFAULT_VALUE;
    std::string uiSyntax = MODULE_UI_SYNTAX_DEFAULT_VALUE;
    std::string pages;
    std::vector<Metadata> metadata;
    std::vector<Ability> abilities;
    std::vector<Extension> extensionAbilities;
    std::vector<RequestPermission> requestPermissions;
    std::vector<DefinePermission> definePermissions;
    std::vector<std::string> dependencies;
};

struct ModuleJson {
    App app;
    Module module;
};

void from_json(const nlohmann::json &jsonObject, Metadata &metadata)
{
    APP_LOGD("read metadata tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
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
}

void from_json(const nlohmann::json &jsonObject, Ability &ability)
{
    APP_LOGD("read ability tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ABILITY_NAME,
        ability.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SRC_ENTRANCE,
        ability.srcEntrance,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ABILITY_LAUNCH_TYPE,
        ability.launchType,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        ability.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        ability.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ICON,
        ability.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ICON_ID,
        ability.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        LABEL,
        ability.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        LABEL_ID,
        ability.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PRIORITY,
        ability.priority,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        PERMISSIONS,
        ability.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        META_DATA,
        ability.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        VISIBLE,
        ability.visible,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        ABILITY_CONTINUABLE,
        ability.continuable,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Skill>>(jsonObject,
        jsonObjectEnd,
        SKILLS,
        ability.skills,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        ABILITY_BACKGROUNDMODES,
        ability.backgroundModes,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ABILITY_START_WINDOW_ICON,
        ability.startWindowIcon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ABILITY_START_WINDOW_ICON_ID,
        ability.startWindowIconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ABILITY_START_WINDOW_BACKGROUND,
        ability.startWindowBackground,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ABILITY_START_WINDOW_BACKGROUND_ID,
        ability.startWindowBackgroundId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Extension &extension)
{
    APP_LOGD("read extension tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        EXTENSION_ABILITY_NAME,
        extension.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SRC_ENTRANCE,
        extension.srcEntrance,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ICON,
        extension.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ICON_ID,
        extension.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        LABEL,
        extension.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        LABEL_ID,
        extension.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        extension.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        extension.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PRIORITY,
        extension.priority,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        EXTENSION_ABILITY_TYPE,
        extension.type,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        EXTENSION_ABILITY_READ_PERMISSION,
        extension.readPermission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        EXTENSION_ABILITY_WRITE_PERMISSION,
        extension.writePermission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        EXTENSION_URI,
        extension.uri,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        PERMISSIONS,
        extension.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        VISIBLE,
        extension.visible,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Skill>>(jsonObject,
        jsonObjectEnd,
        SKILLS,
        extension.skills,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        META_DATA,
        extension.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, DeviceConfig &deviceConfig)
{
    const auto &jsonObjectEnd = jsonObject.end();
    if (jsonObject.find(MIN_API_VERSION) != jsonObjectEnd) {
        deviceConfig.minAPIVersion.first = true;
        GetValueIfFindKey<int32_t>(jsonObject,
            jsonObjectEnd,
            MIN_API_VERSION,
            deviceConfig.minAPIVersion.second,
            JsonType::NUMBER,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_DISTRIBUTED_NOTIFICATION_ENABLED) != jsonObjectEnd) {
        deviceConfig.distributedNotificationEnabled.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_DISTRIBUTED_NOTIFICATION_ENABLED,
            deviceConfig.distributedNotificationEnabled.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_KEEP_ALIVE) != jsonObjectEnd) {
        deviceConfig.keepAlive.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_KEEP_ALIVE,
            deviceConfig.keepAlive.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_REMOVABLE) != jsonObjectEnd) {
        deviceConfig.removable.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_REMOVABLE,
            deviceConfig.removable.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_SINGLETON) != jsonObjectEnd) {
        deviceConfig.singleton.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_SINGLETON,
            deviceConfig.singleton.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_USER_DATA_CLEARABLE) != jsonObjectEnd) {
        deviceConfig.userDataClearable.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_USER_DATA_CLEARABLE,
            deviceConfig.userDataClearable.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    if (jsonObject.find(DEVICE_CONFIG_ACCESSIBLE) != jsonObjectEnd) {
        deviceConfig.accessible.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            DEVICE_CONFIG_ACCESSIBLE,
            deviceConfig.accessible.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
}

void from_json(const nlohmann::json &jsonObject, App &app)
{
    APP_LOGD("read app tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_BUNDLE_NAME,
        app.bundleName,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_DEBUG,
        app.debug,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ICON,
        app.icon,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ICON_ID,
        app.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        LABEL,
        app.label,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        LABEL_ID,
        app.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        app.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        app.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_VENDOR,
        app.vendor,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_VERSION_CODE,
        app.versionCode,
        JsonType::NUMBER,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_VERSION_NAME,
        app.versionName,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_MIN_COMPATIBLE_VERSION_CODE,
        app.minCompatibleVersionCode,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_MIN_API_VERSION,
        app.minAPIVersion,
        JsonType::NUMBER,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        APP_TARGET_API_VERSION,
        app.targetAPIVersion,
        JsonType::NUMBER,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_API_RELEASETYPE,
        app.apiReleaseType,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_DISTRIBUTED_NOTIFICATION_ENABLED,
        app.distributedNotificationEnabled,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        APP_ENTITY_TYPE,
        app.entityType,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_KEEP_ALIVE,
        app.keepAlive,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        APP_TARGETBUNDLELIST,
        app.targetBundleList,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    if (jsonObject.find(APP_REMOVABLE) != jsonObject.end()) {
        app.removable.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            APP_REMOVABLE,
            app.removable.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_SINGLETON,
        app.singleton,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_USER_DATA_CLEARABLE,
        app.userDataClearable,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        APP_ACCESSIBLE,
        app.accessible,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (jsonObject.find(APP_PHONE) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_PHONE,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_PHONE] = deviceConfig;
    }
    if (jsonObject.find(APP_TABLET) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_TABLET,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_TABLET] = deviceConfig;
    }
    if (jsonObject.find(APP_TV) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_TV,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_TV] = deviceConfig;
    }
    if (jsonObject.find(APP_WEARABLE) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_WEARABLE,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_WEARABLE] = deviceConfig;
    }
    if (jsonObject.find(APP_LITE_WEARABLE) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_LITE_WEARABLE,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_LITE_WEARABLE] = deviceConfig;
    }
    if (jsonObject.find(APP_CAR) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_CAR,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_CAR] = deviceConfig;
    }
    if (jsonObject.find(APP_SMART_VISION) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_SMART_VISION,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_SMART_VISION] = deviceConfig;
    }
    if (jsonObject.find(APP_ROUTER) != jsonObjectEnd) {
        DeviceConfig deviceConfig;
        GetValueIfFindKey<DeviceConfig>(jsonObject,
            jsonObjectEnd,
            APP_ROUTER,
            deviceConfig,
            JsonType::OBJECT,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
        app.deviceConfigs[APP_ROUTER] = deviceConfig;
    }
}

void from_json(const nlohmann::json &jsonObject, Module &module)
{
    APP_LOGD("read module tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_NAME,
        module.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_TYPE,
        module.type,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SRC_ENTRANCE,
        module.srcEntrance,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION,
        module.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        DESCRIPTION_ID,
        module.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PROCESS,
        module.process,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_MAIN_ELEMENT,
        module.mainElement,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEVICE_TYPES,
        module.deviceTypes,
        JsonType::ARRAY,
        true,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_DELIVERY_WITH_INSTALL,
        module.deliveryWithInstall,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        MODULE_INSTALLATION_FREE,
        module.installationFree,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_VIRTUAL_MACHINE,
        module.virtualMachine,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_UI_SYNTAX,
        module.uiSyntax,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        MODULE_PAGES,
        module.pages,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Metadata>>(jsonObject,
        jsonObjectEnd,
        META_DATA,
        module.metadata,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Ability>>(jsonObject,
        jsonObjectEnd,
        MODULE_ABILITIES,
        module.abilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Extension>>(jsonObject,
        jsonObjectEnd,
        MODULE_EXTENSION_ABILITIES,
        module.extensionAbilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<RequestPermission>>(jsonObject,
        jsonObjectEnd,
        MODULE_REQUEST_PERMISSIONS,
        module.requestPermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<DefinePermission>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEFINE_PERMISSIONS,
        module.definePermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        MODULE_DEPENDENCIES,
        module.dependencies,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
}

void from_json(const nlohmann::json &jsonObject, ModuleJson &moduleJson)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<App>(jsonObject,
        jsonObjectEnd,
        APP,
        moduleJson.app,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Module>(jsonObject,
        jsonObjectEnd,
        MODULE,
        moduleJson.module,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
}
}  // namespace Profile

namespace {
void GetMetadata(std::vector<Metadata> &metadata, const std::vector<Profile::Metadata> &profileMetadata)
{
    for (const Profile::Metadata &item : profileMetadata) {
        Metadata tmpMetadata;
        tmpMetadata.name = item.name;
        tmpMetadata.value = item.value;
        tmpMetadata.resource = item.resource;
        metadata.emplace_back(tmpMetadata);
    }
}

bool CheckBundleNameIsValid(const std::string &bundleName)
{
    if (bundleName.empty()) {
        return false;
    }
    if (bundleName.size() < Constants::MIN_BUNDLE_NAME || bundleName.size() > Constants::MAX_BUNDLE_NAME) {
        return false;
    }
    char head = bundleName.at(0);
    if (head < 'A' || (head > 'Z' && head < 'a') || head > 'z') {
        return false;
    }
    for (const auto &c : bundleName) {
        if (c < '.' || c == '/' || (c > '9' && c < 'A') || (c > 'Z' && c < '_') || c == '`' || c > 'z') {
            return false;
        }
    }
    return true;
}

bool ToApplicationInfo(const Profile::App &app, ApplicationInfo &applicationInfo,
    bool isPreInstallApp, const BundleExtractor &bundleExtractor)
{
    APP_LOGD("transform ModuleJson to ApplicationInfo");
    applicationInfo.name = app.bundleName;
    applicationInfo.bundleName = app.bundleName;

    applicationInfo.versionCode = static_cast<uint32_t>(app.versionCode);
    applicationInfo.versionName = app.versionName;
    if (app.minCompatibleVersionCode != -1) {
        applicationInfo.minCompatibleVersionCode = app.minCompatibleVersionCode;
    } else {
        applicationInfo.minCompatibleVersionCode = static_cast<int32_t>(applicationInfo.versionCode);
    }

    applicationInfo.apiCompatibleVersion = app.minAPIVersion;
    applicationInfo.apiTargetVersion = app.targetAPIVersion;
    
    applicationInfo.iconPath = app.icon;
    applicationInfo.iconId = app.iconId;
    applicationInfo.label = app.label;
    applicationInfo.labelId = app.labelId;
    applicationInfo.description = app.description;
    applicationInfo.descriptionId = app.descriptionId;
    applicationInfo.targetBundleList = app.targetBundleList;

    if (applicationInfo.isSystemApp && isPreInstallApp) {
        applicationInfo.keepAlive = app.keepAlive;
        applicationInfo.singleton = app.singleton;
        applicationInfo.userDataClearable = app.userDataClearable;
        if (app.removable.first) {
            applicationInfo.removable = app.removable.second;
        } else {
            applicationInfo.removable = false;
        }
        applicationInfo.accessible = app.accessible;
    }

    applicationInfo.apiReleaseType = app.apiReleaseType;
    applicationInfo.debug = app.debug;
    applicationInfo.deviceId = Constants::CURRENT_DEVICE_ID;
    applicationInfo.distributedNotificationEnabled = app.distributedNotificationEnabled;
    if (Profile::ENTITY_TYPE_SET.find(app.entityType) != Profile::ENTITY_TYPE_SET.end()) {
        applicationInfo.entityType = app.entityType;
    } else {
        applicationInfo.entityType = Profile::APP_ENTITY_TYPE_DEFAULT_VALUE;
    }
    applicationInfo.vendor = app.vendor;

    // device adapt
    std::string deviceType = GetDeviceType();
    APP_LOGD("deviceType : %{public}s", deviceType.c_str());
    if (app.deviceConfigs.find(deviceType) != app.deviceConfigs.end()) {
        Profile::DeviceConfig deviceConfig = app.deviceConfigs.at(deviceType);
        if (deviceConfig.minAPIVersion.first) {
            applicationInfo.apiCompatibleVersion = static_cast<uint32_t>(deviceConfig.minAPIVersion.second);
        }
        if (deviceConfig.distributedNotificationEnabled.first) {
            applicationInfo.distributedNotificationEnabled = deviceConfig.distributedNotificationEnabled.second;
        }
        if (applicationInfo.isSystemApp && isPreInstallApp) {
            if (deviceConfig.keepAlive.first) {
                applicationInfo.keepAlive = deviceConfig.keepAlive.second;
            }
            if (deviceConfig.singleton.first) {
                applicationInfo.singleton = deviceConfig.singleton.second;
            }
            if (deviceConfig.userDataClearable.first) {
                applicationInfo.userDataClearable = deviceConfig.userDataClearable.second;
            }
            if (deviceConfig.removable.first) {
                applicationInfo.removable = deviceConfig.removable.second;
            }
            if (deviceConfig.accessible.first) {
                applicationInfo.accessible = deviceConfig.accessible.second;
            }
        }
    }

    // handle native so
    std::string abis = GetAbiList();
    bool isDefault = abis == Constants::ABI_DEFAULT;
    APP_LOGD("abi list : %{public}s, isDefault : %{public}d", abis.c_str(), isDefault);

    bool soExist = bundleExtractor.IsDirExist(Constants::LIBS);
    if (!soExist) {
        APP_LOGD("so not exist");
        applicationInfo.nativeLibraryPath = Constants::EMPTY_STRING;
        std::string targetAbi = Constants::ARM_EABI_V7A;
        if (!isDefault) {
            for (const auto &item : Constants::ABI_MAP) {
                if (abis.find(item.first) == 0) {
                    targetAbi = item.first;
                    break;
                }
            }
        }
        applicationInfo.cpuAbi = targetAbi;
    } else {
        APP_LOGD("so exist");
        std::string targetAbi = bundleExtractor.IsDirExist(Constants::LIBS_ARM_EABI_V7A) ?
            Constants::ARM_EABI_V7A : Constants::ARM_EABI;
        std::string targetLibName = Constants::ARM;
        if (!isDefault) {
            std::vector<std::string> abiList;
            SplitStr(abis, Constants::ABI_SEPARATOR, abiList, false, false);
            for (const std::string &abi : abiList) {
                std::string libsPath;
                libsPath.append(Constants::LIBS).append(abi).append(Constants::PATH_SEPARATOR);
                if (Constants::ABI_MAP.find(abi) != Constants::ABI_MAP.end() && bundleExtractor.IsDirExist(libsPath)) {
                    targetAbi = abi;
                    targetLibName = Constants::ABI_MAP.at(abi);
                    break;
                }
            }
        }
        applicationInfo.nativeLibraryPath = Constants::LIBS + targetLibName;
        applicationInfo.cpuAbi = targetAbi;
    }
    APP_LOGD("nativeLibraryPath : %{private}s, cpuAbi : %{public}s",
        applicationInfo.nativeLibraryPath.c_str(), applicationInfo.cpuAbi.c_str());

    applicationInfo.enabled = true;
    return true;
}

bool ToBundleInfo(const ApplicationInfo &applicationInfo,
    const InnerModuleInfo &innerModuleInfo, bool isPreInstallApp, BundleInfo &bundleInfo)
{
    bundleInfo.name = applicationInfo.bundleName;

    bundleInfo.versionCode = static_cast<uint32_t>(applicationInfo.versionCode);
    bundleInfo.versionName = applicationInfo.versionName;
    bundleInfo.minCompatibleVersionCode = static_cast<uint32_t>(applicationInfo.minCompatibleVersionCode);

    bundleInfo.compatibleVersion = static_cast<uint32_t>(applicationInfo.apiCompatibleVersion);
    bundleInfo.targetVersion = static_cast<uint32_t>(applicationInfo.apiTargetVersion);

    bundleInfo.isKeepAlive = applicationInfo.keepAlive;
    bundleInfo.singleton = applicationInfo.singleton;
    bundleInfo.isPreInstallApp = isPreInstallApp;

    bundleInfo.vendor = applicationInfo.vendor;
    bundleInfo.releaseType = applicationInfo.apiReleaseType;
    bundleInfo.isNativeApp = false;

    if (innerModuleInfo.isEntry) {
        bundleInfo.mainEntry = innerModuleInfo.moduleName;
        bundleInfo.entryModuleName = innerModuleInfo.moduleName;
    }

    return true;
}

uint32_t GetBackgroundModes(const std::vector<std::string> &backgroundModes)
{
    uint32_t backgroundMode = 0;
    for (const std::string &item : backgroundModes) {
        if (Profile::BACKGROUND_MODES_MAP.find(item) != Profile::BACKGROUND_MODES_MAP.end()) {
            backgroundMode |= Profile::BACKGROUND_MODES_MAP.at(item);
        }
    }
    return backgroundMode;
}

bool ToAbilityInfo(const Profile::ModuleJson &moduleJson, const Profile::Ability &ability,
    AbilityInfo &abilityInfo, bool isSystemApp, bool isPreInstallApp)
{
    APP_LOGD("transform ModuleJson to AbilityInfo");
    abilityInfo.name = ability.name;
    abilityInfo.srcEntrance = ability.srcEntrance;
    abilityInfo.description = ability.description;
    abilityInfo.descriptionId = ability.descriptionId;
    abilityInfo.iconPath = ability.icon;
    abilityInfo.iconId = ability.iconId;
    abilityInfo.label = ability.label;
    abilityInfo.labelId = ability.labelId;
    if (isSystemApp && isPreInstallApp) {
        abilityInfo.priority = ability.priority;
    }
    abilityInfo.permissions = ability.permissions;
    abilityInfo.visible = ability.visible;
    abilityInfo.continuable = ability.continuable;
    abilityInfo.backgroundModes = GetBackgroundModes(ability.backgroundModes);
    GetMetadata(abilityInfo.metadata, ability.metadata);
    abilityInfo.package = moduleJson.module.name;
    abilityInfo.bundleName = moduleJson.app.bundleName;
    abilityInfo.moduleName = moduleJson.module.name;
    abilityInfo.applicationName = moduleJson.app.bundleName;
    auto iterLaunch = std::find_if(std::begin(Profile::LAUNCH_MODE_MAP),
        std::end(Profile::LAUNCH_MODE_MAP),
        [&ability](const auto &item) { return item.first == ability.launchType; });
    if (iterLaunch != Profile::LAUNCH_MODE_MAP.end()) {
        abilityInfo.launchMode = iterLaunch->second;
    }
    abilityInfo.enabled = true;
    abilityInfo.isModuleJson = true;
    abilityInfo.isStageBasedModel = true;
    abilityInfo.type = AbilityType::PAGE;
    for (const std::string &deviceType : moduleJson.module.deviceTypes) {
        if (Profile::DEVICE_TYPE_SET.find(deviceType) != Profile::DEVICE_TYPE_SET.end()) {
            abilityInfo.deviceTypes.emplace_back(deviceType);
        }
    }
    abilityInfo.startWindowIcon = ability.startWindowIcon;
    abilityInfo.startWindowIconId = ability.startWindowIconId;
    abilityInfo.startWindowBackground = ability.startWindowBackground;
    abilityInfo.startWindowBackgroundId = ability.startWindowBackgroundId;
    return true;
}

ExtensionAbilityType ConvertToExtensionAbilityType(const std::string &type)
{
    for (size_t index = 0; index < Profile::EXTENSION_TYPE_SET.size(); ++index) {
        if (Profile::EXTENSION_TYPE_SET[index] == type) {
            return static_cast<ExtensionAbilityType>(index);
        }
    }

    return ExtensionAbilityType::UNSPECIFIED;
}

bool ToExtensionInfo(const Profile::ModuleJson &moduleJson, const Profile::Extension &extension,
    ExtensionAbilityInfo &extensionInfo, bool isSystemApp, bool isPreInstallApp)
{
    APP_LOGD("transform ModuleJson to ExtensionAbilityInfo");
    extensionInfo.name = extension.name;
    extensionInfo.srcEntrance = extension.srcEntrance;
    extensionInfo.icon = extension.icon;
    extensionInfo.iconId = extension.iconId;
    extensionInfo.label = extension.label;
    extensionInfo.labelId = extension.labelId;
    extensionInfo.description = extension.description;
    extensionInfo.descriptionId = extension.descriptionId;
    extensionInfo.type = ConvertToExtensionAbilityType(extension.type);
    if (isSystemApp && isPreInstallApp) {
        extensionInfo.priority = extension.priority;
        extensionInfo.readPermission = extension.readPermission;
        extensionInfo.writePermission = extension.writePermission;
    }
    extensionInfo.uri = extension.uri;
    extensionInfo.permissions = extension.permissions;
    extensionInfo.visible = extension.visible;
    GetMetadata(extensionInfo.metadata, extension.metadata);
    extensionInfo.bundleName = moduleJson.app.bundleName;
    extensionInfo.moduleName = moduleJson.module.name;
    if (extensionInfo.type != ExtensionAbilityType::SERVICE &&
        extensionInfo.type != ExtensionAbilityType::DATASHARE) {
        extensionInfo.process = extensionInfo.bundleName;
        extensionInfo.process.append(".");
        extensionInfo.process.append(extensionInfo.moduleName);
        extensionInfo.process.append(":");
        extensionInfo.process.append(extensionInfo.name);
    }
    return true;
}

void GetPermissions(const Profile::ModuleJson &moduleJson, InnerModuleInfo &innerModuleInfo,
    bool isSystemApp, bool isPreInstallApp)
{
    if (isSystemApp && isPreInstallApp) {
        for (const DefinePermission &definePermission : moduleJson.module.definePermissions) {
            if (definePermission.name.empty()) {
                continue;
            }
            if (Profile::GRANT_MODE_SET.find(definePermission.grantMode) == Profile::GRANT_MODE_SET.end()) {
                continue;
            }
            if (Profile::AVAILABLE_LEVEL_SET.find(definePermission.availableLevel)
                == Profile::AVAILABLE_LEVEL_SET.end()) {
                continue;
            }
            innerModuleInfo.definePermissions.emplace_back(definePermission);
        }
    }
    for (const RequestPermission &requestPermission : moduleJson.module.requestPermissions) {
        if (requestPermission.name.empty()) {
            continue;
        }
        innerModuleInfo.requestPermissions.emplace_back(requestPermission);
    }
}

bool ToInnerModuleInfo(const Profile::ModuleJson &moduleJson, InnerModuleInfo &innerModuleInfo,
    bool isSystemApp, bool isPreInstallApp)
{
    APP_LOGD("transform ModuleJson to InnerModuleInfo");
    innerModuleInfo.modulePackage = moduleJson.module.name;
    innerModuleInfo.moduleName = moduleJson.module.name;
    innerModuleInfo.description = moduleJson.module.description;
    innerModuleInfo.descriptionId = moduleJson.module.descriptionId;
    GetMetadata(innerModuleInfo.metadata, moduleJson.module.metadata);
    innerModuleInfo.distro.deliveryWithInstall = moduleJson.module.deliveryWithInstall;
    innerModuleInfo.distro.installationFree = moduleJson.module.installationFree;
    innerModuleInfo.distro.moduleName = moduleJson.module.name;
    innerModuleInfo.installationFree = moduleJson.module.installationFree;
    if (Profile::MODULE_TYPE_SET.find(moduleJson.module.type) != Profile::MODULE_TYPE_SET.end()) {
        innerModuleInfo.distro.moduleType = moduleJson.module.type;
        if (moduleJson.module.type == Profile::MODULE_TYPE_ENTRY) {
            innerModuleInfo.isEntry = true;
        }
    }
    innerModuleInfo.mainAbility = moduleJson.module.mainElement;
    innerModuleInfo.srcEntrance = moduleJson.module.srcEntrance;
    if (!moduleJson.module.process.empty()) {
        innerModuleInfo.process = moduleJson.module.process;
    } else {
        innerModuleInfo.process = moduleJson.app.bundleName;
    }
    for (const std::string &deviceType : moduleJson.module.deviceTypes) {
        if (Profile::DEVICE_TYPE_SET.find(deviceType) != Profile::DEVICE_TYPE_SET.end()) {
            innerModuleInfo.deviceTypes.emplace_back(deviceType);
        }
    }
    if (Profile::VIRTUAL_MACHINE_SET.find(moduleJson.module.virtualMachine) != Profile::VIRTUAL_MACHINE_SET.end()) {
        innerModuleInfo.virtualMachine = moduleJson.module.virtualMachine;
    }
    if (Profile::UI_SYNTAX_SET.find(moduleJson.module.uiSyntax) != Profile::UI_SYNTAX_SET.end()) {
        innerModuleInfo.uiSyntax = moduleJson.module.uiSyntax;
    }
    innerModuleInfo.pages = moduleJson.module.pages;
    GetPermissions(moduleJson, innerModuleInfo, isSystemApp, isPreInstallApp);
    innerModuleInfo.dependencies = moduleJson.module.dependencies;
    innerModuleInfo.isModuleJson = true;
    innerModuleInfo.isStageBasedModel = true;
    // abilities and extensionAbilities store in InnerBundleInfo
    return true;
}

bool ToInnerBundleInfo(const Profile::ModuleJson &moduleJson, const BundleExtractor &bundleExtractor,
    InnerBundleInfo &innerBundleInfo)
{
    APP_LOGD("transform ModuleJson to InnerBundleInfo");
    if (!CheckBundleNameIsValid(moduleJson.app.bundleName)) {
        APP_LOGE("bundle name is invalid");
        return false;
    }
    bool isPreInstallApp = innerBundleInfo.IsPreInstallApp();
    ApplicationInfo applicationInfo;
    applicationInfo.isSystemApp = innerBundleInfo.GetAppType() == Constants::AppType::SYSTEM_APP;
    ToApplicationInfo(moduleJson.app, applicationInfo, isPreInstallApp, bundleExtractor);

    InnerModuleInfo innerModuleInfo;
    ToInnerModuleInfo(moduleJson, innerModuleInfo, applicationInfo.isSystemApp, isPreInstallApp);

    BundleInfo bundleInfo;
    ToBundleInfo(applicationInfo, innerModuleInfo, isPreInstallApp, bundleInfo);

    // handle abilities
    bool findEntry = false;
    for (const Profile::Ability &ability : moduleJson.module.abilities) {
        AbilityInfo abilityInfo;
        ToAbilityInfo(moduleJson, ability, abilityInfo, applicationInfo.isSystemApp, isPreInstallApp);
        std::string key;
        key.append(moduleJson.app.bundleName).append(".")
            .append(moduleJson.module.name).append(".").append(abilityInfo.name);
        innerModuleInfo.abilityKeys.emplace_back(key);
        innerModuleInfo.skillKeys.emplace_back(key);
        innerBundleInfo.InsertSkillInfo(key, ability.skills);
        innerBundleInfo.InsertAbilitiesInfo(key, abilityInfo);
        if (findEntry) {
            continue;
        }
        // get entry ability
        for (const auto &skill : ability.skills) {
            bool isEntryAction = std::find(skill.actions.begin(), skill.actions.end(),
                Constants::INTENT_ACTION_HOME) != skill.actions.end();
            bool isEntryEntity = std::find(skill.entities.begin(), skill.entities.end(),
                Constants::INTENT_ENTITY_HOME) != skill.entities.end();
            if (isEntryAction && isEntryEntity) {
                innerModuleInfo.entryAbilityKey = key;
                innerModuleInfo.label = ability.label;
                innerModuleInfo.labelId = ability.labelId;
                // get launcher application and ability
                bool isLauncherEntity = std::find(skill.entities.begin(), skill.entities.end(),
                    Constants::FLAG_HOME_INTENT_FROM_SYSTEM) != skill.entities.end();
                if (isLauncherEntity && isPreInstallApp) {
                    applicationInfo.isLauncherApp = true;
                    abilityInfo.isLauncherAbility = true;
                }
                findEntry = true;
                break;
            }
        }
    }

    // handle extensionAbilities
    for (const Profile::Extension &extension : moduleJson.module.extensionAbilities) {
        ExtensionAbilityInfo extensionInfo;
        ToExtensionInfo(moduleJson, extension, extensionInfo, applicationInfo.isSystemApp, isPreInstallApp);
        std::string key;
        key.append(moduleJson.app.bundleName).append(".")
            .append(moduleJson.module.name).append(".").append(extension.name);
        innerModuleInfo.extensionKeys.emplace_back(key);
        innerModuleInfo.extensionSkillKeys.emplace_back(key);
        innerBundleInfo.InsertExtensionSkillInfo(key, extension.skills);
        innerBundleInfo.InsertExtensionInfo(key, extensionInfo);
    }

    innerBundleInfo.SetCurrentModulePackage(moduleJson.module.name);
    innerBundleInfo.SetBaseApplicationInfo(applicationInfo);
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    innerBundleInfo.InsertInnerModuleInfo(moduleJson.module.name, innerModuleInfo);
    return true;
}
}  // namespace

ErrCode ModuleProfile::TransformTo(const std::ostringstream &source, const BundleExtractor &bundleExtractor,
    InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGD("transform module.json stream to InnerBundleInfo");
    Profile::ModuleJson moduleJson;
    nlohmann::json jsonObject = nlohmann::json::parse(source.str(), nullptr, false);
    if (jsonObject.is_discarded()) {
        APP_LOGE("bad profile");
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }
    moduleJson = jsonObject.get<Profile::ModuleJson>();
    if (Profile::parseResult != ERR_OK) {
        APP_LOGE("parseResult is %{public}d", Profile::parseResult);
        int32_t ret = Profile::parseResult;
        // need recover parse result to ERR_OK
        Profile::parseResult = ERR_OK;
        return ret;
    }
    if (!ToInnerBundleInfo(moduleJson, bundleExtractor, innerBundleInfo)) {
        return ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR;
    }
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS
