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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_COMMON_PROFILE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_COMMON_PROFILE_H

#include <string>

#include "nlohmann/json.hpp"

namespace OHOS {
namespace AppExecFwk {
namespace ProfileReader {
// common tag
const std::string PROFILE_KEY_NAME = "name";
const std::string PROFILE_KEY_ORIGINAL_NAME = "originalName";
const std::string PROFILE_KEY_LABEL = "label";
const std::string PROFILE_KEY_ICON_ID = "iconId";
const std::string PROFILE_KEY_LABEL_ID = "labelId";
const std::string PROFILE_KEY_DESCRIPTION = "description";
const std::string PROFILE_KEY_DESCRIPTION_ID = "descriptionId";
const std::string PROFILE_KEY_TYPE = "type";
const std::string PROFILE_KEY_SRCPATH = "srcPath";
const std::string PROFILE_KEY_SRCLANGUAGE = "srcLanguage";
const std::string PRIORITY = "priority";

// bundle profile tag
const std::string BUNDLE_PROFILE_KEY_APP = "app";
const std::string BUNDLE_PROFILE_KEY_DEVICE_CONFIG = "deviceConfig";
const std::string BUNDLE_PROFILE_KEY_MODULE = "module";
// sub  BUNDLE_PROFILE_KEY_APP
const std::string BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME = "bundleName";
const std::string BUNDLE_APP_PROFILE_KEY_VENDOR = "vendor";
const std::string BUNDLE_APP_PROFILE_KEY_VERSION = "version";
const std::string BUNDLE_APP_PROFILE_KEY_API_VERSION = "apiVersion";
const std::string BUNDLE_APP_PROFILE_KEY_SINGLETON = "singleton";
const std::string BUNDLE_APP_PROFILE_KEY_REMOVABLE = "removable";
const std::string BUNDLE_APP_PROFILE_KEY_USER_DATA_CLEARABLE = "userDataClearable";
const std::string BUNDLE_APP_PROFILE_KEY_TARGETET_BUNDLE_LIST = "targetBundleList";
// sub BUNDLE_APP_PROFILE_KEY_VERSION
const std::string BUNDLE_APP_PROFILE_KEY_CODE = "code";
const std::string BUNDLE_APP_PROFILE_KEY_MIN_COMPATIBLE_VERSION_CODE = "minCompatibleVersionCode";
// sub BUNDLE_APP_PROFILE_KEY_API_VERSION
const std::string BUNDLE_APP_PROFILE_KEY_COMPATIBLE = "compatible";
const std::string BUNDLE_APP_PROFILE_KEY_TARGET = "target";
const std::string BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE = "releaseType";
const std::string APP_RELEASE_TYPE_VALUE_RELEASE = "Release";
// sub  BUNDLE_PROFILE_KEY_DEVICE_CONFIG
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT = "default";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE = "phone";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET = "tablet";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV = "tv";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR = "car";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE = "wearable";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE = "liteWearable";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION = "smartVision";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_JOINT_USER_ID = "jointUserId";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS = "process";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_KEEP_ALIVE = "keepAlive";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_ARK = "ark";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH = "directLaunch";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP = "supportBackup";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEBUG = "debug";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS = "compressNativeLibs";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK = "network";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_REQ_VERSION = "reqVersion";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_FLAG = "flag";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPATIBLE = "compatible";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TARGET = "target";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT = "usesCleartext";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG = "securityConfig";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS = "domainSettings";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CLEAR_TEXT_PERMITTED = "cleartextPermitted";
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS = "domains";
// sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS
const std::string BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUB_DOMAINS = "subDomains";
// sub BUNDLE_PROFILE_KEY_MODULE
const std::string BUNDLE_MODULE_PROFILE_KEY_PACKAGE = "package";
const std::string BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES = "supportedModes";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_CAPABILITIES = "reqCapabilities";
const std::string BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_REQ_CAPABILITIES = "reqCapabilities";
const std::string BUNDLE_MODULE_DEPENDENCIES = "dependencies";
const std::string MODULE_SUPPORTED_MODES_VALUE_DRIVE = "drive";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE = "deviceType";
const std::string BUNDLE_MODULE_PROFILE_KEY_COLOR_MODE = "colorMode";
const std::string BUNDLE_MODULE_PROFILE_KEY_DISTRO = "distro";
const std::string BUNDLE_MODULE_PROFILE_KEY_META_DATA = "metaData";
const std::string BUNDLE_MODULE_PROFILE_KEY_ABILITIES = "abilities";
const std::string BUNDLE_MODULE_PROFILE_KEY_JS = "js";
const std::string BUNDLE_MODULE_PROFILE_KEY_COMMON_EVENTS = "commonEvents";
const std::string BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS = "shortcuts";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS = "defPermissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEFINE_PERMISSIONS = "definePermissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_NAME = "name";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE = "grantMode";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE_USER_GRANT = "user_grant";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE_SYSTEM_GRANT = "system_grant";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE = "availableScope";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_SIGNATURE = "signature";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_PRIVILEGED = "privileged";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_AVAILABLESCOPE_RESTRICTED = "restricted";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL = "label";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_LABEL_ID = "labelId";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION = "description";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_DESCRIPTION_ID = "descriptionId";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS = "reqPermissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQUEST_PERMISSIONS = "requestPermissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_NAME = "name";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_REASON = "reason";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_USEDSCENE = "usedScene";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_ABILITY = "ability";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN = "when";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN_INUSE = "inuse";
const std::string BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS_WHEN_ALWAYS = "always";
const std::string BUNDLE_MODULE_PROFILE_KEY_CUSTOMIZE_DATA = "customizeData";
const std::string BUNDLE_MODULE_PROFILE_KEY_MAIN_ABILITY = "mainAbility";
const std::string BUNDLE_MODULE_PROFILE_KEY_SRC_PATH = "srcPath";
// sub BUNDLE_MODULE_PROFILE_KEY_DISTRO
const std::string BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL = "deliveryWithInstall";
const std::string BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME = "moduleName";
const std::string BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE = "moduleType";
const std::string BUNDLE_MODULE_PROFILE_KEY_MODULE_INSTALLATION_FREE = "installationFree";
// sub BUNDLE_MODULE_PROFILE_KEY_SKILLS
const std::string BUNDLE_MODULE_PROFILE_KEY_ACTIONS = "actions";
const std::string BUNDLE_MODULE_PROFILE_KEY_ENTITIES = "entities";
const std::string BUNDLE_MODULE_PROFILE_KEY_URIS = "uris";
// sub BUNDLE_MODULE_PROFILE_KEY_URIS
const std::string BUNDLE_MODULE_PROFILE_KEY_SCHEME = "scheme";
const std::string BUNDLE_MODULE_PROFILE_KEY_HOST = "host";
const std::string BUNDLE_MODULE_PROFILE_KEY_PORT = "port";
const std::string BUNDLE_MODULE_PROFILE_KEY_PATH = "path";
const std::string BUNDLE_MODULE_PROFILE_KEY_PATHSTARTWITH = "pathStartWith";
const std::string BUNDLE_MODULE_PROFILE_KEY_PATHREGX = "pathRegx";
const std::string BUNDLE_MODULE_PROFILE_KEY_PATHREGEX = "pathRegex";
const std::string BUNDLE_MODULE_PROFILE_KEY_TYPE = "type";
// sub BUNDLE_MODULE_PROFILE_KEY_META_DATA
const std::string BUNDLE_MODULE_META_KEY_CUSTOMIZE_DATA = "customizeData";
const std::string BUNDLE_MODULE_META_KEY_NAME = "name";
const std::string BUNDLE_MODULE_META_KEY_VALUE = "value";
const std::string BUNDLE_MODULE_META_KEY_EXTRA = "extra";
// sub BUNDLE_MODULE_PROFILE_KEY_DISTRO_TYPE
const std::string MODULE_DISTRO_MODULE_TYPE_VALUE_ENTRY = "entry";
const std::string MODULE_DISTRO_MODULE_TYPE_VALUE_FEATURE = "feature";
const std::string MODULE_DISTRO_MODULE_TYPE_VALUE_HAR = "har";
// sub BUNDLE_MODULE_PROFILE_KEY_ABILITIES
const std::string BUNDLE_MODULE_PROFILE_KEY_ICON = "icon";
const std::string BUNDLE_MODULE_PROFILE_KEY_ICON_ID = "iconId";
const std::string BUNDLE_MODULE_PROFILE_KEY_URI = "uri";
const std::string BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE = "launchType";
const std::string BUNDLE_MODULE_PROFILE_KEY_LAUNCH_THEME = "theme";
const std::string BUNDLE_MODULE_PROFILE_KEY_VISIBLE = "visible";
const std::string BUNDLE_MODULE_PROFILE_KEY_CONTINUABLE = "continuable";
const std::string BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS = "permissions";
const std::string BUNDLE_MODULE_PROFILE_KEY_SKILLS = "skills";
const std::string BUNDLE_MODULE_PROFILE_KEY_PROCESS = "process";
const std::string BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY = "deviceCapability";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORM_ENABLED = "formEnabled";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORM = "form";
const std::string BUNDLE_MODULE_PROFILE_KEY_ORIENTATION = "orientation";
const std::string BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES = "backgroundModes";
const std::string BUNDLE_MODULE_PROFILE_KEY_GRANT_PERMISSION = "grantPermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_URI_PERMISSION = "uriPermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION = "readPermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION = "writePermission";
const std::string BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH = "directLaunch";
const std::string BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES = "configChanges";
const std::string BUNDLE_MODULE_PROFILE_KEY_MISSION = "mission";
const std::string BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY = "targetAbility";
const std::string BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED = "multiUserShared";
const std::string BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE = "supportPipMode";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORMS_ENABLED = "formsEnabled";
const std::string BUNDLE_MODULE_PROFILE_KEY_FORMS = "forms";
const std::string BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_ICON = "startWindowIcon";
const std::string BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_ICON_ID = "startWindowIconId";
const std::string BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_BACKGROUND = "startWindowBackground";
const std::string BUNDLE_MODULE_PROFILE_KEY_START_WINDOW_BACKGROUND_ID = "startWindowBackgroundId";
// sub BUNDLE_MODULE_PROFILE_KEY_FORM
const std::string BUNDLE_MODULE_PROFILE_KEY_MODE = "mode";
// sub BUNDLE_MODULE_PROFILE_KEY_FORM
const std::string BUNDLE_MODULE_PROFILE_FORM_ENTITY = "formEntity";
const std::string BUNDLE_MODULE_PROFILE_FORM_MIN_HEIGHT = "minHeight";
const std::string BUNDLE_MODULE_PROFILE_FORM_DEFAULT_HEIGHT = "defaultHeight";
const std::string BUNDLE_MODULE_PROFILE_FORM_MIN_WIDTH = "minWidth";
const std::string BUNDLE_MODULE_PROFILE_FORM_DEFAULT_WIDTH = "defaultWidth";
// sub BUNDLE_MODULE_PROFILE_KEY_FORMS
const std::string BUNDLE_MODULE_PROFILE_FORMS_IS_DEFAULT = "isDefault";
const std::string BUNDLE_MODULE_PROFILE_FORMS_COLOR_MODE = "colorMode";
const std::string BUNDLE_MODULE_PROFILE_FORMS_SUPPORT_DIMENSIONS = "supportDimensions";
const std::string BUNDLE_MODULE_PROFILE_FORMS_DEFAULT_DIMENSION = "defaultDimension";
const std::string BUNDLE_MODULE_PROFILE_FORMS_LANDSCAPE_LAYOUTS = "landscapeLayouts";
const std::string BUNDLE_MODULE_PROFILE_FORMS_PORTRAIT_LAYOUTS = "portraitLayouts";
const std::string BUNDLE_MODULE_PROFILE_FORMS_UPDATEENABLED = "updateEnabled";
const std::string BUNDLE_MODULE_PROFILE_FORMS_SCHEDULED_UPDATE_TIME = "scheduledUpdateTime";
const std::string BUNDLE_MODULE_PROFILE_FORMS_UPDATE_DURATION = "updateDuration";
const std::string BUNDLE_MODULE_PROFILE_FORMS_DEEP_LINK = "deepLink";
const std::string BUNDLE_MODULE_PROFILE_FORMS_JS_COMPONENT_NAME = "jsComponentName";
const std::string BUNDLE_MODULE_PROFILE_FORMS_VALUE = "value";
const std::string BUNDLE_MODULE_PROFILE_FORMS_FORM_CONFIG_ABILITY = "formConfigAbility";
const std::string BUNDLE_MODULE_PROFILE_FORMS_FORM_VISIBLE_NOTIFY = "formVisibleNotify";
const std::string BUNDLE_MODULE_PROFILE_FORMS_SRC = "src";
// sub BUNDLE_MODULE_PROFILE_KEY_JS
const std::string BUNDLE_MODULE_PROFILE_KEY_PAGES = "pages";
const std::string BUNDLE_MODULE_PROFILE_KEY_WINDOW = "window";
// sub BUNDLE_MODULE_PROFILE_KEY_COMMON_EVENTS
const std::string BUNDLE_MODULE_PROFILE_KEY_PERMISSION = "permission";
const std::string BUNDLE_MODULE_PROFILE_KEY_DATA = "data";
const std::string BUNDLE_MODULE_PROFILE_KEY_EVENTS = "events";
const std::string MODULE_ABILITY_JS_TYPE_VALUE_NORMAL = "normal";
const std::string MODULE_ABILITY_JS_TYPE_VALUE_FORM = "form";
// sub BUNDLE_MODULE_PROFILE_KEY_WINDOW
const std::string BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH = "designWidth";
const std::string BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH = "autoDesignWidth";
// sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS
const std::string BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_ID = "shortcutId";
const std::string BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_WANTS = "intents";
// sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_WANTS
const std::string BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS = "targetClass";
const std::string BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE = "targetBundle";
// sub BUNDLE_INSTALL_MARK
const std::string BUNDLE_INSTALL_MARK_BUNDLE = "installMarkBundle";
const std::string BUNDLE_INSTALL_MARK_PACKAGE = "installMarkPackage";
const std::string BUNDLE_INSTALL_MARK_STATUS = "installMarkStatus";
// sub BUNDLE_SANDBOX_PERSISTENT_INFO
const std::string BUNDLE_SANDBOX_PERSISTENT_ACCESS_TOKEN_ID = "accessTokenId";
const std::string BUNDLE_SANDBOX_PERSISTENT_APP_INDEX = "appIndex";
const std::string BUNDLE_SANDBOX_PERSISTENT_USER_ID = "userId";

const uint32_t VALUE_HOME_SCREEN = 1 << 0;
// 000010 represents supporting search box
const uint32_t VALUE_SEARCHBOX = 1 << 1;

const std::string KEY_HOME_SCREEN = "homeScreen";
const std::string KEY_SEARCHBOX = "searchbox";

const std::string BUNDLE_MODULE_PROFILE_KEY_JS_TYPE_ETS = "ets";

static std::map<std::string, uint32_t> formEntityMap;

extern thread_local int32_t parseResult;

// background modes
// different bits in the binary represent different services
// 0000001 represents data transmission services
constexpr uint32_t VALUE_DATA_TRANSFER = 1 << 0;

// 0000 0010 represents audio output service
constexpr uint32_t VALUE_AUDIO_PLAYBACK = 1 << 1;

// 0000 0100 represents audio input service
constexpr uint32_t VALUE_AUDIO_RECORDING = 1 << 2;

// 0000 1000 represents positioning navigation service
constexpr uint32_t VALUE_LOCATION = 1 << 3;

// 0001 0000 represents bluetooth scanning, connection, transmission service (wearing)
constexpr uint32_t VALUE_BLUETOOTH_INTERACTION = 1 << 4;

// 0010 0000 represents multi device connection services
constexpr uint32_t VALUE_MULTI_DEVICE_CONNECTION = 1 << 5;

// 0100 0000 represents WiFi scanning, connection, transmission services (clone, multi-screen)
constexpr uint32_t VALUE_WIFI_INTERACTION = 1 << 6;

// 1000 0000 represents audio call,VOIP service
constexpr uint32_t VALUE_VOIP = 1 << 7;

// 1 0000 0000 represents task Keeping service
constexpr uint32_t VALUE_TASK_KEEPING = 1 << 8;

// 10 0000 0000 represents picture in picture service
constexpr uint32_t VALUE_PICTURE_IN_PICTURE = 1 << 9;

// 100 0000 0000 represents screen fetch service
constexpr uint32_t VALUE_SCREEN_FETCH = 1 << 10;

const std::string KEY_DATA_TRANSFER = "dataTransfer";
const std::string KEY_AUDIO_PLAYBACK = "audioPlayback";
const std::string KEY_AUDIO_RECORDING = "audioRecording";
const std::string KEY_LOCATION = "location";
const std::string KEY_BLUETOOTH_INTERACTION = "bluetoothInteraction";
const std::string KEY_MULTI_DEVICE_CONNECTION = "multiDeviceConnection";
const std::string KEY_WIFI_INTERACTION = "wifiInteraction";
const std::string KEY_VOIP = "voip";
const std::string KEY_TASK_KEEPING = "taskKeeping";
const std::string KEY_PICTURE_IN_PICTURE = "pictureInPicture";
const std::string KEY_SCREEN_FETCH = "screenFetch";
}  // namespace ProfileReader

namespace Profile {
// common
const std::string ICON = "icon";
const std::string ICON_ID = "iconId";
const std::string LABEL = "label";
const std::string LABEL_ID = "labelId";
const std::string DESCRIPTION = "description";
const std::string DESCRIPTION_ID = "descriptionId";
const std::string META_DATA = "metadata";
const std::string SKILLS = "skills";
const std::string SRC_ENTRANCE = "srcEntrance";
const std::string PERMISSIONS = "permissions";
const std::string VISIBLE = "visible";
const std::string SRC_LANGUAGE = "srcLanguage";
const std::string PRIORITY = "priority";
// module.json
const std::string APP = "app";
const std::string MODULE = "module";
// app
const std::string APP_BUNDLE_NAME = "bundleName";
const std::string APP_DEBUG = "debug";
const std::string APP_VENDOR = "vendor";
const std::string APP_VERSION_CODE = "versionCode";
const std::string APP_VERSION_NAME = "versionName";
const std::string APP_MIN_COMPATIBLE_VERSION_CODE = "minCompatibleVersionCode";
const std::string APP_MIN_API_VERSION = "minAPIVersion";
const std::string APP_TARGET_API_VERSION = "targetAPIVersion";
const std::string APP_API_RELEASETYPE = "apiReleaseType";
const std::string APP_API_RELEASETYPE_DEFAULT_VALUE = "Release";
const std::string APP_DISTRIBUTED_NOTIFICATION_ENABLED = "distributedNotificationEnabled";
const std::string APP_ENTITY_TYPE = "entityType";
const std::string APP_ENTITY_TYPE_DEFAULT_VALUE = "unspecified";
const std::string APP_KEEP_ALIVE = "keepAlive";
const std::string APP_REMOVABLE = "removable";
const std::string APP_SINGLETON = "singleton";
const std::string APP_USER_DATA_CLEARABLE = "userDataClearable";
const std::string APP_PHONE = "phone";
const std::string APP_TABLET = "tablet";
const std::string APP_TV = "tv";
const std::string APP_WEARABLE = "wearable";
const std::string APP_LITE_WEARABLE = "liteWearable";
const std::string APP_CAR = "car";
const std::string APP_SMART_VISION = "smartVision";
const std::string APP_ROUTER = "router";
const std::string APP_ACCESSIBLE = "accessible";
const std::string APP_TARGETBUNDLELIST = "targetBundleList";
// module
const std::string MODULE_NAME = "name";
const std::string MODULE_TYPE = "type";
const std::string MODULE_PROCESS = "process";
const std::string MODULE_MAIN_ELEMENT = "mainElement";
const std::string MODULE_DEVICE_TYPES = "deviceTypes";
const std::string MODULE_DELIVERY_WITH_INSTALL = "deliveryWithInstall";
const std::string MODULE_INSTALLATION_FREE = "installationFree";
const std::string MODULE_VIRTUAL_MACHINE = "virtualMachine";
const std::string MODULE_VIRTUAL_MACHINE_DEFAULT_VALUE = "default";
const std::string MODULE_UI_SYNTAX = "uiSyntax";
const std::string MODULE_UI_SYNTAX_DEFAULT_VALUE = "hml";
const std::string MODULE_PAGES = "pages";
const std::string MODULE_ABILITIES = "abilities";
const std::string MODULE_EXTENSION_ABILITIES = "extensionAbilities";
const std::string MODULE_REQUEST_PERMISSIONS = "requestPermissions";
const std::string MODULE_DEFINE_PERMISSIONS = "definePermissions";
const std::string MODULE_DEPENDENCIES = "dependencies";
// module type
const std::string MODULE_TYPE_ENTRY = "entry";
const std::string MODULE_TYPE_FEATURE = "feature";
const std::string MODULE_TYPE_HAR = "har";
// deviceConfig
const std::string MIN_API_VERSION = "minAPIVersion";
const std::string DEVICE_CONFIG_DISTRIBUTED_NOTIFICATION_ENABLED = "distributedNotificationEnabled";
const std::string DEVICE_CONFIG_KEEP_ALIVE = "keepAlive";
const std::string DEVICE_CONFIG_REMOVABLE = "removable";
const std::string DEVICE_CONFIG_SINGLETON = "singleton";
const std::string DEVICE_CONFIG_USER_DATA_CLEARABLE = "userDataClearable";
const std::string DEVICE_CONFIG_ACCESSIBLE = "accessible";
// metadata
const std::string META_DATA_NAME = "name";
const std::string META_DATA_VALUE = "value";
const std::string META_DATA_RESOURCE = "resource";
// metadata reserved
const std::string META_DATA_FORM = "ohos.extension.form";
const std::string META_DATA_SHORTCUTS = "ohos.ability.shortcuts";
const std::string META_DATA_COMMON_EVENTS = "ohos.extension.staticSubscriber";
// ability
const std::string ABILITY_NAME = "name";
const std::string ABILITY_LAUNCH_TYPE = "launchType";
const std::string ABILITY_LAUNCH_TYPE_DEFAULT_VALUE = "singleton";
const std::string ABILITY_BACKGROUNDMODES = "backgroundModes";
const std::string ABILITY_CONTINUABLE = "continuable";
const std::string ABILITY_START_WINDOW_ICON = "startWindowIcon";
const std::string ABILITY_START_WINDOW_ICON_ID = "startWindowIconId";
const std::string ABILITY_START_WINDOW_BACKGROUND = "startWindowBackground";
const std::string ABILITY_START_WINDOW_BACKGROUND_ID = "startWindowBackgroundId";
// extension ability
const std::string EXTENSION_ABILITY_NAME = "name";
const std::string EXTENSION_ABILITY_TYPE = "type";
const std::string EXTENSION_URI = "uri";
const std::string EXTENSION_ABILITY_READ_PERMISSION = "readPermission";
const std::string EXTENSION_ABILITY_WRITE_PERMISSION = "writePermission";
// requestPermission
const std::string REQUESTPERMISSION_NAME = "name";
const std::string REQUESTPERMISSION_REASON = "reason";
const std::string REQUESTPERMISSION_REASON_ID = "reasonId";
const std::string REQUESTPERMISSION_USEDSCENE = "usedScene";
const std::string REQUESTPERMISSION_ABILITIES = "abilities";
const std::string REQUESTPERMISSION_WHEN = "when";
// definePermission
const std::string DEFINEPERMISSION_NAME = "name";
const std::string DEFINEPERMISSION_GRANT_MODE = "grantMode";
const std::string DEFINEPERMISSION_AVAILABLE_LEVEL = "availableLevel";
const std::string DEFINEPERMISSION_PROVISION_ENABLE = "provisionEnable";
const std::string DEFINEPERMISSION_DISTRIBUTED_SCENE_ENABLE = "distributedSceneEnable";
const std::string DEFINEPERMISSION_GRANT_MODE_DEFAULT_VALUE = "system_grant";
const std::string DEFINEPERMISSION_AVAILABLE_LEVEL_DEFAULT_VALUE = "normal";
// apl
const std::string AVAILABLELEVEL_NORMAL = "normal";
const std::string AVAILABLELEVEL_SYSTEM_BASIC = "system_basic";
const std::string AVAILABLELEVEL_SYSTEM_CORE = "system_core";

extern thread_local int32_t parseResult;
}  // namespace Profile
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_COMMON_PROFILE_H
