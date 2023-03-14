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

#include "bundle_profile.h"

#include <fstream>
#include <sstream>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "common_profile.h"
#include "parameter.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace ProfileReader {

thread_local int32_t parseResult;
const std::set<std::string> MODULE_TYPE_SET = {
    "entry",
    "feature",
    "har"
};
const std::map<std::string, AbilityType> ABILITY_TYPE_MAP = {
    {"page", AbilityType::PAGE},
    {"service", AbilityType::SERVICE},
    {"data", AbilityType::DATA},
    {"form", AbilityType::FORM}
};
const std::map<std::string, DisplayOrientation> DISPLAY_ORIENTATION_MAP = {
    {"unspecified", DisplayOrientation::UNSPECIFIED},
    {"landscape", DisplayOrientation::LANDSCAPE},
    {"portrait", DisplayOrientation::PORTRAIT},
    {"followrecent", DisplayOrientation::FOLLOWRECENT}
};
const std::map<std::string, LaunchMode> LAUNCH_MODE_MAP = {
    {"singleton", LaunchMode::SINGLETON},
    {"singletop", LaunchMode::SINGLETOP},
    {"standard", LaunchMode::STANDARD},
    {"specified", LaunchMode::SPECIFIED}
};
const std::map<std::string, int32_t> dimensionMap = {
    {"1*2", 1},
    {"2*2", 2},
    {"2*4", 3},
    {"4*4", 4}
};
const std::map<std::string, FormType> formTypeMap = {
    {"JS", FormType::JS},
    {"Java", FormType::JAVA}
};
const std::map<std::string, ModuleColorMode> moduleColorMode = {
    {"auto", ModuleColorMode::AUTO},
    {"dark", ModuleColorMode::DARK},
    {"light", ModuleColorMode::LIGHT},
};
const std::map<std::string, FormsColorMode> formColorModeMap = {
    {"auto", FormsColorMode::AUTO_MODE},
    {"dark", FormsColorMode::DARK_MODE},
    {"light", FormsColorMode::LIGHT_MODE}
};
std::map<std::string, uint32_t> backgroundModeMap = {
    {KEY_DATA_TRANSFER, VALUE_DATA_TRANSFER},
    {KEY_AUDIO_PLAYBACK, VALUE_AUDIO_PLAYBACK},
    {KEY_AUDIO_RECORDING, VALUE_AUDIO_RECORDING},
    {KEY_LOCATION, VALUE_LOCATION},
    {KEY_BLUETOOTH_INTERACTION, VALUE_BLUETOOTH_INTERACTION},
    {KEY_MULTI_DEVICE_CONNECTION, VALUE_MULTI_DEVICE_CONNECTION},
    {KEY_WIFI_INTERACTION, VALUE_WIFI_INTERACTION},
    {KEY_VOIP, VALUE_VOIP},
    {KEY_TASK_KEEPING, VALUE_TASK_KEEPING},
    {KEY_PICTURE_IN_PICTURE, VALUE_PICTURE_IN_PICTURE},
    {KEY_SCREEN_FETCH, VALUE_SCREEN_FETCH}
};

struct Version {
    int32_t code = 0;
    std::string name;
    int32_t minCompatibleVersionCode = -1;
};

struct ApiVersion {
    uint32_t compatible = 0;
    uint32_t target = 0;
    std::string releaseType = "Release";
};
// config.json app
struct App {
    std::string bundleName;
    std::string originalName;
    std::string vendor;
    // pair first : if exist in config.json then true, otherwise false
    // pair second : actual value
    std::pair<bool, bool> removable = std::make_pair<>(false, true);
    Version version;
    ApiVersion apiVersion;
    bool singleton = false;
    int32_t iconId = 0;
    int32_t labelId = 0;
    bool userDataClearable = true;
    std::vector<std::string> targetBundleList;
};

struct ReqVersion {
    uint32_t compatible = 0;
    uint32_t target = 0;
};

struct Ark {
    ReqVersion reqVersion;
    std::string flag;
};

struct Domain {
    bool subDomains = false;
    std::string name;
};

struct DomainSetting {
    bool cleartextPermitted = false;
    std::vector<Domain> domains;
};

struct SecurityConfig {
    DomainSetting domainSetting;
};

struct Network {
    bool usesCleartext = false;
    SecurityConfig securityConfig;
};

struct Device {
    std::string jointUserId;
    std::string process;
    bool keepAlive = false;
    Ark ark;
    bool directLaunch = false;
    bool supportBackup = false;
    bool compressNativeLibs = true;
    Network network;
    bool debug = false;
};
// config.json  deviceConfig
struct DeviceConfig {
    Device defaultDevice;
    Device phone;
    Device tablet;
    Device tv;
    Device car;
    Device wearable;
    Device liteWearable;
    Device smartVision;
};

struct Form {
    std::vector<std::string> formEntity;
    int32_t minHeight = 0;
    int32_t defaultHeight = 0;
    int32_t minWidth = 0;
    int32_t defaultWidth = 0;
};

struct FormsCustomizeData {
    std::string name;
    std::string value;
};

struct FormsMetaData {
    std::vector<FormsCustomizeData> customizeData;
};

struct Window {
    int32_t designWidth = 720;
    bool autoDesignWidth = false;
};

struct Forms {
    std::string name;
    std::string description;
    int32_t descriptionId = 0;
    bool isDefault = false;
    std::string type;
    std::string src;
    Window window;
    std::string colorMode = "auto";
    std::vector<std::string> supportDimensions;
    std::string defaultDimension;
    std::vector<std::string> landscapeLayouts;
    std::vector<std::string> portraitLayouts;
    bool updateEnabled = false;
    std::string scheduledUpdateTime = "";
    int32_t updateDuration = 0;
    std::string deepLink;
    std::string formConfigAbility;
    bool formVisibleNotify = false;
    std::string jsComponentName;
    FormsMetaData metaData;
};

struct CustomizeData {
    std::string name;
    std::string value;
    std::string extra;
};

struct MetaData {
    std::vector<CustomizeData> customizeData;
};

struct UriPermission {
    std::string mode;
    std::string path;
};

struct Ability {
    std::string name;
    std::string originalName;
    std::string description;
    int32_t descriptionId = 0;
    std::string icon;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    int32_t priority = 0;
    std::string uri;
    std::string process;
    std::string launchType = "singleton";
    std::string theme;
    bool visible = false;
    bool continuable = false;
    std::vector<std::string> permissions;
    std::vector<Skill> skills;
    std::vector<std::string> deviceCapability;
    MetaData metaData;
    std::string type;
    std::string srcPath;
    std::string srcLanguage = "js";
    bool formEnabled = false;
    Form form;
    std::string orientation = "unspecified";
    std::vector<std::string> backgroundModes;
    bool grantPermission;
    UriPermission uriPermission;
    std::string readPermission;
    std::string writePermission;
    bool directLaunch = false;
    std::vector<std::string> configChanges;
    std::string mission;
    std::string targetAbility;
    bool multiUserShared = false;
    bool supportPipMode = false;
    bool formsEnabled = false;
    std::vector<Forms> formses;
    std::string startWindowIcon;
    int32_t startWindowIconId = 0;
    std::string startWindowBackground;
    int32_t startWindowBackgroundId = 0;
};

struct Js {
    std::string name = "default";
    std::vector<std::string> pages;
    Window window;
    std::string type = "normal";
};

struct Intent {
    std::string targetClass;
    std::string targetBundle;
};

struct CommonEvent {
    std::string name;
    std::string permission;
    std::vector<std::string> data;
    std::vector<std::string> type;
    std::vector<std::string> events;
};

struct Shortcut {
    std::string shortcutId;
    std::string label;
    std::string icon;
    int32_t iconId;
    int32_t labelId;
    std::vector<Intent> intents;
};

// config.json module
struct Module {
    std::string package;
    std::string name;
    std::string description;
    int32_t descriptionId = 0;
    std::string colorMode = "auto";
    std::vector<std::string> supportedModes;
    std::vector<std::string> reqCapabilities;
    std::vector<std::string> deviceType;
    std::vector<std::string> dependencies;
    Distro distro;
    MetaData metaData;
    std::vector<Ability> abilities;
    std::vector<Js> jses;
    std::vector<CommonEvent> commonEvents;
    std::vector<Shortcut> shortcuts;
    std::vector<DefinePermission> defPermissions;
    std::vector<RequestPermission> requestPermissions;
    std::vector<DefinePermission> definePermissions;
    std::string mainAbility;
    std::string srcPath;
};

// config.json
struct ConfigJson {
    App app;
    DeviceConfig deveicConfig;
    Module module;
};

/*
 * form_json is global static overload method in self namespace ProfileReader,
 * which need callback by json library, and can not rename this function,
 * so don't named according UpperCamelCase style
 */
void from_json(const nlohmann::json &jsonObject, Version &version)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_CODE,
        version.code,
        JsonType::NUMBER,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        version.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_MIN_COMPATIBLE_VERSION_CODE,
        version.minCompatibleVersionCode,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, ApiVersion &apiVersion)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_COMPATIBLE,
        apiVersion.compatible,
        JsonType::NUMBER,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_TARGET,
        apiVersion.target,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE,
        apiVersion.releaseType,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, App &app)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME,
        app.bundleName,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_ORIGINAL_NAME,
        app.originalName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Version>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_VERSION,
        app.version,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<ApiVersion>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_API_VERSION,
        app.apiVersion,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_VENDOR,
        app.vendor,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_SINGLETON,
        app.singleton,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_ICON_ID,
        app.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_LABEL_ID,
        app.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (jsonObject.find(BUNDLE_APP_PROFILE_KEY_REMOVABLE) != jsonObject.end()) {
        app.removable.first = true;
        GetValueIfFindKey<bool>(jsonObject,
            jsonObjectEnd,
            BUNDLE_APP_PROFILE_KEY_REMOVABLE,
            app.removable.second,
            JsonType::BOOLEAN,
            false,
            parseResult,
            ArrayType::NOT_ARRAY);
    }
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_USER_DATA_CLEARABLE,
        app.userDataClearable,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_APP_PROFILE_KEY_TARGETET_BUNDLE_LIST,
        app.targetBundleList,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
}

void from_json(const nlohmann::json &jsonObject, ReqVersion &reqVersion)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPATIBLE,
        reqVersion.compatible,
        JsonType::NUMBER,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TARGET,
        reqVersion.target,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Ark &ark)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<ReqVersion>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_REQ_VERSION,
        ark.reqVersion,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_FLAG,
        ark.flag,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Domain &domain)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUB_DOMAINS,
        domain.subDomains,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        domain.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, DomainSetting &domainSetting)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CLEAR_TEXT_PERMITTED,
        domainSetting.cleartextPermitted,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Domain>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS,
        domainSetting.domains,
        JsonType::ARRAY,
        true,
        parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, SecurityConfig &securityConfig)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<DomainSetting>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS,
        securityConfig.domainSetting,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Network &network)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT,
        network.usesCleartext,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<SecurityConfig>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG,
        network.securityConfig,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Device &device)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_JOINT_USER_ID,
        device.jointUserId,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS,
        device.process,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_KEEP_ALIVE,
        device.keepAlive,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Ark>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_ARK,
        device.ark,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH,
        device.directLaunch,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP,
        device.supportBackup,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEBUG,
        device.debug,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS,
        device.compressNativeLibs,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Network>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK,
        device.network,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, DeviceConfig &deviceConfig)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT,
        deviceConfig.defaultDevice,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE,
        deviceConfig.phone,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET,
        deviceConfig.tablet,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV,
        deviceConfig.tv,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR,
        deviceConfig.car,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE,
        deviceConfig.wearable,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE,
        deviceConfig.liteWearable,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Device>(jsonObject,
        jsonObjectEnd,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION,
        deviceConfig.smartVision,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Form &form)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORM_ENTITY,
        form.formEntity,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORM_MIN_HEIGHT,
        form.minHeight,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORM_DEFAULT_HEIGHT,
        form.defaultHeight,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORM_MIN_WIDTH,
        form.minWidth,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORM_DEFAULT_WIDTH,
        form.defaultWidth,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, CustomizeData &customizeData)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        customizeData.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_META_KEY_EXTRA,
        customizeData.extra,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_META_KEY_VALUE,
        customizeData.value,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, MetaData &metaData)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::vector<CustomizeData>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_META_KEY_CUSTOMIZE_DATA,
        metaData.customizeData,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, FormsCustomizeData &customizeDataForms)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        customizeDataForms.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_VALUE,
        customizeDataForms.value,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, FormsMetaData &formsMetaData)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::vector<FormsCustomizeData>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_CUSTOMIZE_DATA,
        formsMetaData.customizeData,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
}

void from_json(const nlohmann::json &jsonObject, Window &window)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH,
        window.designWidth,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH,
        window.autoDesignWidth,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Forms &forms)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        forms.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_IS_DEFAULT,
        forms.isDefault,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_TYPE,
        forms.type,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_SRC,
        forms.src,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Window>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_WINDOW,
        forms.window,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_SUPPORT_DIMENSIONS,
        forms.supportDimensions,
        JsonType::ARRAY,
        true,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_DEFAULT_DIMENSION,
        forms.defaultDimension,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_LANDSCAPE_LAYOUTS,
        forms.landscapeLayouts,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_PORTRAIT_LAYOUTS,
        forms.portraitLayouts,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_UPDATEENABLED,
        forms.updateEnabled,
        JsonType::BOOLEAN,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_JS_COMPONENT_NAME,
        forms.jsComponentName,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_DESCRIPTION,
        forms.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_DESCRIPTION_ID,
        forms.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_COLOR_MODE,
        forms.colorMode,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_SCHEDULED_UPDATE_TIME,
        forms.scheduledUpdateTime,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_UPDATE_DURATION,
        forms.updateDuration,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_DEEP_LINK,
        forms.deepLink,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_FORM_CONFIG_ABILITY,
        forms.formConfigAbility,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_FORMS_FORM_VISIBLE_NOTIFY,
        forms.formVisibleNotify,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<FormsMetaData>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_META_DATA,
        forms.metaData,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, UriPermission &uriPermission)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_MODE,
        uriPermission.mode,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_PATH,
        uriPermission.path,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Ability &ability)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        ability.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_ORIGINAL_NAME,
        ability.originalName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_TYPE,
        ability.type,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_SRCPATH,
        ability.srcPath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_SRCLANGUAGE,
        ability.srcLanguage,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_DESCRIPTION,
        ability.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_DESCRIPTION_ID,
        ability.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_ICON,
        ability.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_ICON_ID,
        ability.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_PROCESS,
        ability.process,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_LABEL,
        ability.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_LABEL_ID,
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
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_URI,
        ability.uri,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE,
        ability.launchType,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_LAUNCH_THEME,
        ability.theme,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_VISIBLE,
        ability.visible,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_CONTINUABLE,
        ability.continuable,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS,
        ability.permissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<Skill>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_SKILLS,
        ability.skills,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY,
        ability.deviceCapability,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<MetaData>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_META_DATA,
        ability.metaData,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_FORM_ENABLED,
        ability.formEnabled,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Form>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_FORM,
        ability.form,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_ORIENTATION,
        ability.orientation,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES,
        ability.backgroundModes,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_GRANT_PERMISSION,
        ability.grantPermission,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<UriPermission>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_URI_PERMISSION,
        ability.uriPermission,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION,
        ability.readPermission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION,
        ability.writePermission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH,
        ability.directLaunch,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES,
        ability.configChanges,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_MISSION,
        ability.mission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY,
        ability.targetAbility,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED,
        ability.multiUserShared,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE,
        ability.supportPipMode,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_FORMS_ENABLED,
        ability.formsEnabled,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Forms>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_FORMS,
        ability.formses,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_ICON,
        ability.startWindowIcon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_ICON_ID,
        ability.startWindowIconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_BACKGROUND,
        ability.startWindowBackground,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_BACKGROUND_ID,
        ability.startWindowBackgroundId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Js &js)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        js.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_PAGES,
        js.pages,
        JsonType::ARRAY,
        true,
        parseResult,
        ArrayType::STRING);
    // these are not required fields.
    GetValueIfFindKey<Window>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_WINDOW,
        js.window,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_TYPE,
        js.type,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Intent &intents)
{
    // these are not required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS,
        intents.targetClass,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE,
        intents.targetBundle,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, CommonEvent &commonEvent)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        commonEvent.name,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_EVENTS,
        commonEvent.events,
        JsonType::ARRAY,
        true,
        parseResult,
        ArrayType::STRING);
    // these are not required fields.
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_PERMISSION,
        commonEvent.permission,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DATA,
        commonEvent.data,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_TYPE,
        commonEvent.type,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
}

void from_json(const nlohmann::json &jsonObject, Shortcut &shortcut)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_ID,
        shortcut.shortcutId,
        JsonType::STRING,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_LABEL,
        shortcut.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_ICON,
        shortcut.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Intent>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_WANTS,
        shortcut.intents,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    // get label id
    GetValueIfFindKey<int32_t>(jsonObject,
         jsonObjectEnd,
         PROFILE_KEY_LABEL_ID,
         shortcut.labelId,
         JsonType::NUMBER,
         false,
         parseResult,
         ArrayType::NOT_ARRAY);
    // get icon id
    GetValueIfFindKey<int32_t>(jsonObject,
         jsonObjectEnd,
         BUNDLE_MODULE_PROFILE_KEY_ICON_ID,
         shortcut.iconId,
         JsonType::NUMBER,
         false,
         parseResult,
         ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, Module &module)
{
    // these are required fields.
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_PACKAGE,
        module.package,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_NAME,
        module.name,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE,
        module.deviceType,
        JsonType::ARRAY,
        true,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_COLOR_MODE,
        module.colorMode,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<Distro>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DISTRO,
        module.distro,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    // these are not required fields.
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_DESCRIPTION,
        module.description,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        PROFILE_KEY_DESCRIPTION_ID,
        module.descriptionId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES,
        module.supportedModes,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_REQ_CAPABILITIES,
        module.reqCapabilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<MetaData>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_META_DATA,
        module.metaData,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<Ability>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_ABILITIES,
        module.abilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Js>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_JS,
        module.jses,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<CommonEvent>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_COMMON_EVENTS,
        module.commonEvents,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<Shortcut>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS,
        module.shortcuts,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<DefinePermission>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS,
        module.defPermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<RequestPermission>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS,
        module.requestPermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::vector<DefinePermission>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_DEFINE_PERMISSIONS,
        module.definePermissions,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_MAIN_ABILITY,
        module.mainAbility,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_PROFILE_KEY_SRC_PATH,
        module.srcPath,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_MODULE_DEPENDENCIES,
        module.dependencies,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
}

void from_json(const nlohmann::json &jsonObject, ConfigJson &configJson)
{
    // Because it does not support exceptions, every element needs to be searched first
    APP_LOGI("read 'App' tag from config.json");
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<App>(jsonObject,
        jsonObjectEnd,
        BUNDLE_PROFILE_KEY_APP,
        configJson.app,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    APP_LOGI("read 'DeviceConfig' tag from config.json");
    GetValueIfFindKey<DeviceConfig>(jsonObject,
        jsonObjectEnd,
        BUNDLE_PROFILE_KEY_DEVICE_CONFIG,
        configJson.deveicConfig,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    APP_LOGI("read 'Module' tag from config.json");
    GetValueIfFindKey<Module>(jsonObject,
        jsonObjectEnd,
        BUNDLE_PROFILE_KEY_MODULE,
        configJson.module,
        JsonType::OBJECT,
        true,
        parseResult,
        ArrayType::NOT_ARRAY);
    APP_LOGI("read tag from config.json");
}

}  // namespace ProfileReader

namespace {

bool CheckBundleNameIsValid(const std::string &bundleName)
{
    if (bundleName.empty()) {
        return false;
    }
    if (bundleName.size() < Constants::MIN_BUNDLE_NAME || bundleName.size() > Constants::MAX_BUNDLE_NAME) {
        return false;
    }
    char head = bundleName.at(0);
    if (head < 'A' || ('Z' < head && head < 'a') || head > 'z') {
        return false;
    }
    for (const auto &c : bundleName) {
        if (c < '.' || c == '/' || ('9' < c && c < 'A') || ('Z' < c && c < '_') || c == '`' || c > 'z') {
            return false;
        }
    }
    return true;
}

bool CheckModuleInfosIsValid(ProfileReader::ConfigJson &configJson)
{
    if (configJson.module.deviceType.empty()) {
        APP_LOGE("module deviceType invalid");
        return false;
    }
    if (!configJson.module.abilities.empty()) {
        for (const auto &ability : configJson.module.abilities) {
            if (ability.name.empty() || ability.type.empty()) {
                APP_LOGE("ability name or type invalid");
                return false;
            }
        }
    }
    if (configJson.app.version.code <= 0) {
        APP_LOGE("version code invalid");
        return false;
    }
    auto iter =
        std::find_if(configJson.module.deviceType.begin(), configJson.module.deviceType.end(), [](const auto &d) {
            return ((d.compare(ProfileReader::BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE) == 0 ||
                     d.compare(ProfileReader::BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION) == 0));
        });
    if (iter != configJson.module.deviceType.end()) {
        APP_LOGE("this is a lite device app, ignores other check");
        // if lite device hap doesn't have a module package name, assign it as bundle name.
        if (configJson.module.package.empty()) {
            configJson.module.package = configJson.app.bundleName;
        }
        return true;
    }
    if (configJson.module.package.empty()) {
        APP_LOGE("module package invalid");
        return false;
    }
    if (configJson.module.distro.moduleName.empty()) {
        APP_LOGE("module distro invalid");
        return false;
    }
    return true;
}
uint32_t GetFormEntity(const std::vector<std::string> &formEntity)
{
    if (ProfileReader::formEntityMap.empty()) {
        ProfileReader::formEntityMap.insert({ProfileReader::KEY_HOME_SCREEN, ProfileReader::VALUE_HOME_SCREEN});
        ProfileReader::formEntityMap.insert({ProfileReader::KEY_SEARCHBOX, ProfileReader::VALUE_SEARCHBOX});
    }

    uint32_t formEntityInBinary = 0;
    for (const auto &item : formEntity) {
        if (ProfileReader::formEntityMap.find(item) != ProfileReader::formEntityMap.end()) {
            formEntityInBinary |= ProfileReader::formEntityMap[item];
        }
    }
    return formEntityInBinary;
}

bool ConvertFormInfo(FormInfo &formInfo, const ProfileReader::Forms &form)
{
    formInfo.name = form.name;
    formInfo.description = form.description;
    formInfo.descriptionId = form.descriptionId;
    formInfo.formConfigAbility = form.formConfigAbility;
    formInfo.formVisibleNotify = form.formVisibleNotify;
    formInfo.deepLink = form.deepLink;
    formInfo.defaultFlag = form.isDefault;
    auto type = std::find_if(std::begin(ProfileReader::formTypeMap),
        std::end(ProfileReader::formTypeMap),
        [&form](const auto &item) { return item.first == form.type; });
    if (type != ProfileReader::formTypeMap.end()) {
        formInfo.type = type->second;
    }
    auto colorMode = std::find_if(std::begin(ProfileReader::formColorModeMap),
        std::end(ProfileReader::formColorModeMap),
        [&form](const auto &item) { return item.first == form.colorMode; });
    if (colorMode != ProfileReader::formColorModeMap.end()) {
        formInfo.colorMode = colorMode->second;
    }
    formInfo.updateEnabled = form.updateEnabled;
    formInfo.scheduledUpdateTime = form.scheduledUpdateTime;
    formInfo.updateDuration = form.updateDuration;
    formInfo.jsComponentName = form.jsComponentName;
    for (auto &data : form.metaData.customizeData) {
        FormCustomizeData customizeData;
        customizeData.name = data.name;
        customizeData.value = data.value;
        formInfo.customizeDatas.emplace_back(customizeData);
    }
    for (const auto &dimensions : form.supportDimensions) {
        auto dimension = std::find_if(std::begin(ProfileReader::dimensionMap),
            std::end(ProfileReader::dimensionMap),
            [&dimensions](const auto &item) { return item.first == dimensions; });
        if (dimension != ProfileReader::dimensionMap.end()) {
            formInfo.supportDimensions.emplace_back(dimension->second);
        }
    }
    auto dimension = std::find_if(std::begin(ProfileReader::dimensionMap),
        std::end(ProfileReader::dimensionMap),
        [&form](const auto &item) { return item.first == form.defaultDimension; });
    if (dimension != ProfileReader::dimensionMap.end()) {
        formInfo.defaultDimension = dimension->second;
    }
    formInfo.landscapeLayouts = form.landscapeLayouts;
    formInfo.portraitLayouts = form.portraitLayouts;
    formInfo.src = form.src;
    formInfo.window.autoDesignWidth = form.window.autoDesignWidth;
    formInfo.window.designWidth = form.window.designWidth;
    return true;
}

bool ToApplicationInfo(const ProfileReader::ConfigJson &configJson,
    ApplicationInfo &applicationInfo, bool isPreInstallApp, const BundleExtractor &bundleExtractor)
{
    APP_LOGD("transform ConfigJson to ApplicationInfo");
    applicationInfo.name = configJson.app.bundleName;
    applicationInfo.bundleName = configJson.app.bundleName;

    applicationInfo.versionCode = static_cast<uint32_t>(configJson.app.version.code);
    applicationInfo.versionName = configJson.app.version.name;
    if (configJson.app.version.minCompatibleVersionCode != -1) {
        applicationInfo.minCompatibleVersionCode = configJson.app.version.minCompatibleVersionCode;
    } else {
        applicationInfo.minCompatibleVersionCode = static_cast<int32_t>(applicationInfo.versionCode);
    }

    applicationInfo.apiCompatibleVersion = configJson.app.apiVersion.compatible;
    applicationInfo.apiTargetVersion = configJson.app.apiVersion.target;
    applicationInfo.apiReleaseType = configJson.app.apiVersion.releaseType;

    // if there is main ability, it's icon label description will be set to applicationInfo.

    if (applicationInfo.isSystemApp && isPreInstallApp) {
        applicationInfo.keepAlive = configJson.deveicConfig.defaultDevice.keepAlive;
        applicationInfo.singleton = configJson.app.singleton;
        applicationInfo.userDataClearable = configJson.app.userDataClearable;
        if (configJson.app.removable.first) {
            applicationInfo.removable = configJson.app.removable.second;
        } else {
            applicationInfo.removable = false;
        }
    }

    applicationInfo.debug = configJson.deveicConfig.defaultDevice.debug;
    applicationInfo.deviceId = Constants::CURRENT_DEVICE_ID;
    applicationInfo.distributedNotificationEnabled = true;
    applicationInfo.entityType = Profile::APP_ENTITY_TYPE_DEFAULT_VALUE;
    applicationInfo.process = configJson.deveicConfig.defaultDevice.process;
    auto it = find(configJson.module.supportedModes.begin(),
        configJson.module.supportedModes.end(),
        ProfileReader::MODULE_SUPPORTED_MODES_VALUE_DRIVE);
    if (it != configJson.module.supportedModes.end()) {
        applicationInfo.supportedModes = 1;
    } else {
        applicationInfo.supportedModes = 0;
    }
    applicationInfo.vendor = configJson.app.vendor;

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

    // for SystemResource
    applicationInfo.iconId = configJson.app.iconId;
    applicationInfo.labelId = configJson.app.labelId;

    applicationInfo.enabled = true;
    for (const auto &targetBundle : configJson.app.targetBundleList) {
        APP_LOGD("targetBundle = %{public}s", targetBundle.c_str());
        applicationInfo.targetBundleList.emplace_back(targetBundle);
    }

    return true;
}

bool ToBundleInfo(const ProfileReader::ConfigJson &configJson, const ApplicationInfo &applicationInfo,
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
    if (configJson.module.jses.empty()) {
        bundleInfo.isNativeApp = true;
    }

    if (innerModuleInfo.isEntry) {
        bundleInfo.mainEntry = innerModuleInfo.modulePackage;
        bundleInfo.entryModuleName = innerModuleInfo.moduleName;
    }

    return true;
}

void GetMetaData(MetaData &metaData, const ProfileReader::MetaData &profileMetaData)
{
    for (const auto &item : profileMetaData.customizeData) {
        CustomizeData customizeData;
        customizeData.name = item.name;
        customizeData.extra = item.extra;
        customizeData.value = item.value;
        metaData.customizeData.emplace_back(customizeData);
    }
}

uint32_t GetBackgroundModes(const std::vector<std::string>& backgroundModes)
{
    uint32_t backgroundMode = 0;
    for (const auto& item : backgroundModes) {
        if (ProfileReader::backgroundModeMap.find(item) != ProfileReader::backgroundModeMap.end()) {
            backgroundMode |= ProfileReader::backgroundModeMap[item];
        }
    }
    return backgroundMode;
}

bool ToInnerModuleInfo(const ProfileReader::ConfigJson &configJson, InnerModuleInfo &innerModuleInfo)
{
    innerModuleInfo.modulePackage = configJson.module.package;
    innerModuleInfo.moduleName = configJson.module.distro.moduleName;
    innerModuleInfo.installationFree = configJson.module.distro.installationFree;
    innerModuleInfo.description = configJson.module.description;
    innerModuleInfo.descriptionId = configJson.module.descriptionId;
    auto colorModeInfo = std::find_if(std::begin(ProfileReader::moduleColorMode),
        std::end(ProfileReader::moduleColorMode),
        [&configJson](const auto &item) { return item.first == configJson.module.colorMode; });
    if (colorModeInfo != ProfileReader::moduleColorMode.end()) {
        innerModuleInfo.colorMode = colorModeInfo->second;
    }
    GetMetaData(innerModuleInfo.metaData, configJson.module.metaData);
    innerModuleInfo.distro = configJson.module.distro;
    innerModuleInfo.reqCapabilities = configJson.module.reqCapabilities;
    innerModuleInfo.requestPermissions = configJson.module.requestPermissions;
    innerModuleInfo.definePermissions = configJson.module.definePermissions;
    innerModuleInfo.defPermissions = configJson.module.defPermissions;
    if (configJson.module.mainAbility.substr(0, 1) == ".") {
        innerModuleInfo.mainAbility = configJson.module.package + configJson.module.mainAbility;
    } else {
        innerModuleInfo.mainAbility = configJson.module.mainAbility;
    }
    innerModuleInfo.srcPath = configJson.module.srcPath;
    std::string moduleType = innerModuleInfo.distro.moduleType;
    if (ProfileReader::MODULE_TYPE_SET.find(moduleType) != ProfileReader::MODULE_TYPE_SET.end()) {
        if (moduleType == ProfileReader::MODULE_DISTRO_MODULE_TYPE_VALUE_ENTRY) {
            innerModuleInfo.isEntry = true;
        }
    }
    innerModuleInfo.dependencies = configJson.module.dependencies;

    innerModuleInfo.isModuleJson = false;
    return true;
}

bool ToAbilityInfo(const ProfileReader::ConfigJson &configJson, const ProfileReader::Ability &ability,
    AbilityInfo &abilityInfo, bool isSystemApp,  bool isPreInstallApp)
{
    abilityInfo.name = ability.name;
    if (ability.srcLanguage != "c++" && ability.name.substr(0, 1) == ".") {
        abilityInfo.name = configJson.module.package + ability.name;
    }
    abilityInfo.label = ability.label;
    abilityInfo.description = ability.description;
    abilityInfo.iconPath = ability.icon;
    abilityInfo.labelId = ability.labelId;
    abilityInfo.descriptionId = ability.descriptionId;
    abilityInfo.iconId = ability.iconId;
    abilityInfo.visible = ability.visible;
    abilityInfo.continuable = ability.continuable;
    abilityInfo.kind = ability.type;
    abilityInfo.srcPath = ability.srcPath;
    abilityInfo.srcLanguage = ability.srcLanguage;
    if (isSystemApp && isPreInstallApp) {
        abilityInfo.priority = ability.priority;
    }

    std::transform(
        abilityInfo.srcLanguage.begin(), abilityInfo.srcLanguage.end(), abilityInfo.srcLanguage.begin(), ::tolower);
    if (abilityInfo.srcLanguage != ProfileReader::BUNDLE_MODULE_PROFILE_KEY_JS &&
        abilityInfo.srcLanguage != ProfileReader::BUNDLE_MODULE_PROFILE_KEY_JS_TYPE_ETS) {
        abilityInfo.isNativeAbility = true;
    }
    auto iterType = std::find_if(std::begin(ProfileReader::ABILITY_TYPE_MAP),
        std::end(ProfileReader::ABILITY_TYPE_MAP),
        [&ability](const auto &item) { return item.first == ability.type; });
    if (iterType != ProfileReader::ABILITY_TYPE_MAP.end()) {
        abilityInfo.type = iterType->second;
    } else {
        APP_LOGE("ability type invalid.");
        return false;
    }

    auto iterOrientation = std::find_if(std::begin(ProfileReader::DISPLAY_ORIENTATION_MAP),
        std::end(ProfileReader::DISPLAY_ORIENTATION_MAP),
        [&ability](const auto &item) { return item.first == ability.orientation; });
    if (iterOrientation != ProfileReader::DISPLAY_ORIENTATION_MAP.end()) {
        abilityInfo.orientation = iterOrientation->second;
    }

    auto iterLaunch = std::find_if(std::begin(ProfileReader::LAUNCH_MODE_MAP),
        std::end(ProfileReader::LAUNCH_MODE_MAP),
        [&ability](const auto &item) { return item.first == ability.launchType; });
    if (iterLaunch != ProfileReader::LAUNCH_MODE_MAP.end()) {
        abilityInfo.launchMode = iterLaunch->second;
    }

    for (const auto &permission : ability.permissions) {
        abilityInfo.permissions.emplace_back(permission);
    }
    abilityInfo.process = (ability.process.empty()) ? configJson.deveicConfig.defaultDevice.process : ability.process;
    abilityInfo.theme = ability.theme;
    abilityInfo.deviceTypes = configJson.module.deviceType;
    abilityInfo.deviceCapabilities = ability.deviceCapability;
    if (iterType->second == AbilityType::DATA &&
        ability.uri.find(Constants::DATA_ABILITY_URI_PREFIX) == std::string::npos) {
        APP_LOGE("ability uri invalid.");
        return false;
    }
    abilityInfo.uri = ability.uri;
    abilityInfo.package = configJson.module.package;
    abilityInfo.bundleName = configJson.app.bundleName;
    abilityInfo.moduleName = configJson.module.distro.moduleName;
    abilityInfo.applicationName = configJson.app.bundleName;
    abilityInfo.targetAbility = ability.targetAbility;
    abilityInfo.enabled = true;
    abilityInfo.supportPipMode = ability.supportPipMode;
    abilityInfo.readPermission = ability.readPermission;
    abilityInfo.writePermission = ability.writePermission;
    abilityInfo.configChanges = ability.configChanges;
    abilityInfo.formEntity = GetFormEntity(ability.form.formEntity);
    abilityInfo.minFormHeight = ability.form.minHeight;
    abilityInfo.defaultFormHeight = ability.form.defaultHeight;
    abilityInfo.minFormWidth = ability.form.minWidth;
    abilityInfo.defaultFormWidth = ability.form.defaultWidth;
    GetMetaData(abilityInfo.metaData, ability.metaData);
    abilityInfo.formEnabled = ability.formsEnabled;
    abilityInfo.backgroundModes = GetBackgroundModes(ability.backgroundModes);
    abilityInfo.isModuleJson = false;
    abilityInfo.startWindowIcon = ability.startWindowIcon;
    abilityInfo.startWindowIconId = ability.startWindowIconId;
    abilityInfo.startWindowBackground = ability.startWindowBackground;
    abilityInfo.startWindowBackgroundId = ability.startWindowBackgroundId;
    return true;
}

bool ToInnerBundleInfo(ProfileReader::ConfigJson &configJson, const BundleExtractor &bundleExtractor,
    InnerBundleInfo &innerBundleInfo)
{
    APP_LOGD("transform profile configJson to innerBundleInfo");
    if (!CheckBundleNameIsValid(configJson.app.bundleName)) {
        APP_LOGE("bundle name is invalid");
        return false;
    }
    if (!CheckModuleInfosIsValid(configJson)) {
        APP_LOGE("module infos is invalid");
        return false;
    }
    bool isPreInstallApp = innerBundleInfo.IsPreInstallApp();

    ApplicationInfo applicationInfo;
    applicationInfo.isSystemApp = innerBundleInfo.GetAppType() == Constants::AppType::SYSTEM_APP;
    ToApplicationInfo(configJson, applicationInfo, isPreInstallApp, bundleExtractor);

    InnerModuleInfo innerModuleInfo;
    ToInnerModuleInfo(configJson, innerModuleInfo);

    BundleInfo bundleInfo;
    ToBundleInfo(configJson, applicationInfo, innerModuleInfo, isPreInstallApp, bundleInfo);

    for (const auto &info : configJson.module.shortcuts) {
        ShortcutInfo shortcutInfo;
        shortcutInfo.id = info.shortcutId;
        shortcutInfo.bundleName = configJson.app.bundleName;
        shortcutInfo.icon = info.icon;
        shortcutInfo.label = info.label;
        shortcutInfo.iconId = info.iconId;
        shortcutInfo.labelId = info.labelId;
        for (const auto &intent : info.intents) {
            ShortcutIntent shortcutIntent;
            shortcutIntent.targetBundle = intent.targetBundle;
            shortcutIntent.targetClass = intent.targetClass;
            shortcutInfo.intents.emplace_back(shortcutIntent);
        }
        std::string shortcutkey;
        shortcutkey.append(configJson.app.bundleName).append(".")
            .append(configJson.module.package).append(".").append(info.shortcutId);
        innerBundleInfo.InsertShortcutInfos(shortcutkey, shortcutInfo);
    }
    if (innerBundleInfo.GetAppType() == Constants::AppType::SYSTEM_APP) {
        for (const auto &info : configJson.module.commonEvents) {
            CommonEventInfo commonEvent;
            commonEvent.name = info.name;
            commonEvent.bundleName = configJson.app.bundleName;
            commonEvent.permission = info.permission;
            commonEvent.data = info.data;
            commonEvent.type = info.type;
            commonEvent.events = info.events;
            std::string commonEventKey;
            commonEventKey.append(configJson.app.bundleName).append(".")
                .append(configJson.module.package).append(".").append(info.name);
            innerBundleInfo.InsertCommonEvents(commonEventKey, commonEvent);
        }
    }
    bool find = false;
    for (const auto &ability : configJson.module.abilities) {
        AbilityInfo abilityInfo;
        if (!ToAbilityInfo(configJson, ability, abilityInfo, applicationInfo.isSystemApp, isPreInstallApp)) {
            APP_LOGE("parse to abilityInfo failed");
            return false;
        }
        std::string keyName;
        keyName.append(configJson.app.bundleName).append(".")
            .append(configJson.module.package).append(".").append(abilityInfo.name);
        innerModuleInfo.abilityKeys.emplace_back(keyName);
        innerModuleInfo.skillKeys.emplace_back(keyName);
        innerBundleInfo.InsertSkillInfo(keyName, ability.skills);
        std::vector<FormInfo> formInfos;
        for (const auto &form : ability.formses) {
            FormInfo formInfo;
            ConvertFormInfo(formInfo, form);
            formInfo.abilityName = ability.name;
            if (ability.srcLanguage != "c++" && ability.name.substr(0, 1) == ".") {
                formInfo.abilityName = configJson.module.package + ability.name;
            }
            formInfo.bundleName = configJson.app.bundleName;
            formInfo.moduleName = configJson.module.distro.moduleName;
            formInfo.package = configJson.module.package;
            formInfo.originalBundleName = configJson.app.originalName;
            formInfos.emplace_back(formInfo);
        }
        innerBundleInfo.InsertFormInfos(keyName, formInfos);
        if (!find) {
            for (const auto &skill : ability.skills) {
                if (std::find(skill.actions.begin(), skill.actions.end(), Constants::INTENT_ACTION_HOME) !=
                        skill.actions.end() &&
                        std::find(skill.entities.begin(), skill.entities.end(), Constants::INTENT_ENTITY_HOME) !=
                        skill.entities.end() &&
                    (!find)) {
                    innerModuleInfo.entryAbilityKey = keyName;
                    // if there is main ability, it's label will be the application's label
                    applicationInfo.label = ability.label;
                    applicationInfo.labelId = ability.labelId;
                    applicationInfo.iconPath = ability.icon;
                    applicationInfo.iconId = ability.iconId;
                    applicationInfo.description = ability.description;
                    applicationInfo.descriptionId = ability.descriptionId;
                    if (innerModuleInfo.label.empty()) {
                        innerModuleInfo.label = ability.label;
                        innerModuleInfo.labelId = ability.labelId;
                    }
                    find = true;
                }
                if (std::find(skill.entities.begin(), skill.entities.end(), Constants::FLAG_HOME_INTENT_FROM_SYSTEM) !=
                    skill.entities.end() && isPreInstallApp) {
                    applicationInfo.isLauncherApp = true;
                    abilityInfo.isLauncherAbility = true;
                }
            }
        }
        innerBundleInfo.InsertAbilitiesInfo(keyName, abilityInfo);
    }

    innerBundleInfo.SetCurrentModulePackage(configJson.module.package);
    innerBundleInfo.SetBaseApplicationInfo(applicationInfo);
    innerBundleInfo.SetBaseBundleInfo(bundleInfo);
    innerBundleInfo.InsertInnerModuleInfo(configJson.module.package, innerModuleInfo);
    return true;
}

}  // namespace

ErrCode BundleProfile::TransformTo(const std::ostringstream &source, const BundleExtractor &bundleExtractor,
    InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGI("transform profile stream to bundle info");
    ProfileReader::ConfigJson configJson;
    nlohmann::json jsonObject = nlohmann::json::parse(source.str(), nullptr, false);
    if (jsonObject.is_discarded()) {
        APP_LOGE("bad profile");
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }
    configJson = jsonObject.get<ProfileReader::ConfigJson>();
    if (ProfileReader::parseResult != ERR_OK) {
        APP_LOGE("parseResult is %{public}d", ProfileReader::parseResult);
        int32_t ret = ProfileReader::parseResult;
        // need recover parse result to ERR_OK
        ProfileReader::parseResult = ERR_OK;
        return ret;
    }
    if (!ToInnerBundleInfo(configJson, bundleExtractor, innerBundleInfo)) {
        return ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleProfile::TransformTo(const std::ostringstream &source, BundlePackInfo &bundlePackInfo)
{
    APP_LOGI("transform packinfo stream to bundle pack info");
    nlohmann::json jsonObject = nlohmann::json::parse(source.str(), nullptr, false);
    if (jsonObject.is_discarded()) {
        APP_LOGE("bad profile");
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }
    bundlePackInfo = jsonObject.get<BundlePackInfo>();
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS
