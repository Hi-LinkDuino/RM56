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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_CONSTANTS_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_CONSTANTS_H

#include<map>
#include <string>
#include <vector>

namespace OHOS {
namespace AppExecFwk {
namespace Constants {
const std::string EMPTY_STRING = "";
const std::string BUNDLE_PROFILE_NAME = "config.json";
const std::string MODULE_PROFILE_NAME = "module.json";
const std::string BUNDLE_PACKFILE_NAME = "pack.info";
const std::string SYSCAP_NAME = "rpcid.sc";
const std::string INSTALL_FILE_SUFFIX = ".hap";
const std::string PROFILE_FILE_SUFFIX = ".json";
const std::string PROFILE_FILE_COLON = ":";
const std::string PATH_SEPARATOR = "/";
const std::string FILE_UNDERLINE = "_";
const std::string ILLEGAL_PATH_FIELD = "../";
const char DOT_SUFFIX = '.';
const char FILE_SEPARATOR_CHAR = '/';
const std::string CURRENT_DEVICE_ID = "PHONE-001";
const std::string BUNDLE_DATA_BASE_DIR = "/data/bundlemgr";
const std::string BUNDLE_DATA_BASE_FILE = BUNDLE_DATA_BASE_DIR + "/bmsdb.json";
const std::string SYSTEM_APP_SCAN_PATH = "/system/app";
const std::string SYSTEM_RESOURCES_APP_PATH = "/system/app/SystemResources.hap";
const std::string SYSTEM_RESOURCES_APP_PATH_NEW = "/system/app/ohos.global.systemres";
const std::string SYSTEM_APP_INSTALL_PATH = "/data/accounts";
const std::string THIRD_SYSTEM_APP_SCAN_PATH = "/system/vendor";
const std::string THIRD_SYSTEM_APP_INSTALL_PATH = "/data/accounts";
const std::string THIRD_PARTY_APP_INSTALL_PATH = "/data/accounts";
const std::string EXTRACT_TMP_PATH = "/data/sadata/install_tmp/bundle_haps";
const std::string HAP_COPY_PATH = "/data/sadata/install_tmp/Tmp_";
const std::string USER_ACCOUNT_DIR = "account";
const std::string APP_CODE_DIR = "applications";
const std::string APP_DATA_DIR = "appdata";
const std::string DATA_BASE_DIR = "database";
const std::string DATA_DIR = "files";
const std::string CACHE_DIR = "cache";
const std::string CACHES_DIR = "caches";
const std::string SHARED_DIR = "shared";
const std::string SHARED_PREFERENCE_DIR = "sharedPreference";
const std::string TMP_SUFFIX = "_tmp";
const std::string ASSETS_DIR = "assets";
const std::string RESOURCES_INDEX = "resources.index";
const std::string BUNDLE_CODE_DIR = "/data/app/el1/bundle/public";
const std::string BUNDLE_BASE_CODE_DIR = "/data/app/el1/bundle";
const std::string BUNDLE_APP_DATA_BASE_DIR = "/data/app/";
const std::string BASE = "/base/";
const std::string DATABASE = "/database/";
const std::string HAPS = "/haps/";
const std::string BUNDLE_MANAGER_SERVICE_PATH = "/data/service/el1/public/bms/bundle_manager_service";
const std::string BUNDLE_USER_INFO_PATH = BUNDLE_MANAGER_SERVICE_PATH + "/bundle_user_info.json";
const std::string DISTRIBUTED_FILE = "/data/service/el2/%/hmdfs/account/data/";
const std::string DISTRIBUTED_FILE_NON_ACCOUNT = "/data/service/el2/%/hmdfs/non_account/data/";
const std::string DISTRIBUTED_FILE_PROPERTY = "const.distributed_file_property.enabled";
const std::vector<std::string> BUNDLE_EL = {"el1", "el2"};
const std::vector<std::string> BUNDLE_DATA_DIR = {
    "/cache",
    "/files",
    "/temp",
    "/preferences",
    "/haps",
    "/el3/base",
    "/el4/base"
};

const std::string BMS_SERVICE_NAME = "BundleMgrService";
const std::string INSTALLD_SERVICE_NAME = "installd";
const std::string SYSTEM_APP = "system";
const std::string THIRD_PARTY_APP = "third-party";
constexpr int START_USERID = 100;
constexpr int DEFAULT_USERID = 0;
constexpr int INVALID_USERID = -1;
constexpr int UNSPECIFIED_USERID = -2;
constexpr int ALL_USERID = -3;
constexpr int ANY_USERID = -4;
constexpr int NOT_EXIST_USERID = -5;
constexpr int PATH_MAX_SIZE = 256;
constexpr int SIGNATURE_MATCHED = 0;
constexpr int SIGNATURE_NOT_MATCHED = 1;
constexpr int SIGNATURE_UNKNOWN_BUNDLE = 2;
constexpr int PERMISSION_GRANTED = 0;
constexpr int PERMISSION_NOT_GRANTED = -1;
constexpr int DUMP_INDENT = 4;
constexpr unsigned int INSTALLD_UMASK = 0000;

// native so
const std::string ABI_DEFAULT = "default";
const std::string ABI_SEPARATOR = ",";
const std::string ARM_EABI = "armeabi";
const std::string ARM_EABI_V7A = "armeabi-v7a";
const std::string ARM64_V8A = "arm64-v8a";
const std::string X86 = "x86";
const std::string X86_64 = "x86_64";
const std::string LIBS = "libs/";
const std::string LIBS_ARM_EABI_V7A = "libs/armeabi-v7a/";
const std::string ARM = "arm";
const std::map<std::string, std::string> ABI_MAP = {
    {ARM_EABI, "arm"},
    {ARM_EABI_V7A, "arm"},
    {ARM64_V8A, "arm64"},
    {X86, "x86"},
    {X86_64, "x86_64"},
};
const std::string SO_SUFFIX = ".so";

// uid and gid
constexpr int32_t INVALID_UID = -1;
constexpr int32_t INVALID_GID = -1;
constexpr int32_t INVALID_BUNDLEID = -1;
constexpr int32_t INVALID_ACCESS_TOKEN_ID = -1;
constexpr int32_t ROOT_UID = 0;
constexpr int32_t C_UESRID = 200; // clone bundle userId.
constexpr int32_t BMS_UID = 1000;
constexpr int32_t BMS_GID = 1000;
constexpr int32_t BASE_SYS_UID = 2100;
constexpr int32_t MAX_SYS_UID = 2899;
constexpr int32_t BASE_SYS_VEN_UID = 5000;
constexpr int32_t MAX_SYS_VEN_UID = 5999;
constexpr int32_t BASE_APP_UID = 10000;
constexpr int32_t BASE_USER_RANGE = 200000;
constexpr int32_t MAX_APP_UID = 65535;
constexpr int32_t U_VALUE = 100000;
constexpr int32_t DATABASE_DIR_GID = 1000;
constexpr int32_t DFS_GID = 1009;
const std::string PROFILE_KEY_UID_SIZE = "size";
const std::string PROFILE_KEY_UID_AND_GID = "uid_and_gid";

// permissions
const std::string PERMISSION_INSTALL_BUNDLE = "ohos.permission.INSTALL_BUNDLE";
const std::string PERMISSION_GET_BUNDLE_INFO = "ohos.permission.GET_BUNDLE_INFO";
const std::string PERMISSION_GET_BUNDLE_INFO_PRIVILEGED = "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED";
const std::string PERMISSION_CHANGE_ABILITY_ENABLED_STATE = "ohos.permission.CHANGE_ABILITY_ENABLED_STATE";
const std::string PERNISSION_REMOVECACHEFILE = "ohos.permission.REMOVE_CACHE_FILES";

// install list permissions file
const std::string INSTALL_LIST_PERMISSIONS_FILE_PATH = "/system/etc/permission/install_list_permissions.json";

enum class AppType {
    SYSTEM_APP = 0,
    THIRD_SYSTEM_APP,
    THIRD_PARTY_APP,
};

const std::string INTENT_ACTION_HOME = "action.system.home";
const std::string INTENT_ENTITY_HOME = "entity.system.home";
const std::string FLAG_HOME_INTENT_FROM_SYSTEM = "flag.home.intent.from.system";

// the ability file folder name.
const std::string LIB_FOLDER_NAME = "libs";
const std::string RES_FOLDER_NAME = "resources";

constexpr uint8_t MAX_LABLE_LEN = 30;
constexpr uint8_t MAX_BUNDLE_NAME = 255;
constexpr uint8_t MIN_BUNDLE_NAME = 7;
constexpr uint8_t MAX_VENDOR = 255;
constexpr uint8_t EQUAL_ZERO = 0;
constexpr uint8_t MAX_MODULE_PACKAGE = 127;
constexpr uint8_t MAX_MODULE_NAME = 255;
constexpr uint8_t MAX_MODULE_ABILITIES_READPERMISSION = 255;
constexpr uint8_t MAX_MODULE_ABILITIES_WRITEPERMISSION = 255;
constexpr uint8_t MAX_MODULE_SHORTCUTID = 63;
constexpr uint8_t MAX_MODULE_LABEL = 63;
// max number of haps under one direction
constexpr uint8_t MAX_HAP_NUMBER = 128;

// distributed database
const std::string APP_ID = "bundle_manager_service";
const std::string STORE_ID = "installed_bundle_datas";
const std::string ABILITY_USAGE_STORE_ID = "ability_usage_datas";
const std::string PRE_INSTALL_DATA_STORE_ID = "preinstall_usage_datas";
const std::string DISTRIBUTE_DATA_STORE_ID = "distribute_bundle_datas";

// single max hap size
constexpr int64_t ONE_GB = 1024 * 1024 * 1024;
constexpr int64_t MAX_HAP_SIZE = ONE_GB * 4;  // 4GB

const std::string UID = "uid";
const std::string USER_ID = "userId";
const std::string BUNDLE_NAME = "bundleName";
const int32_t MAX_LIMIT_SIZE = 4;

const std::string DATA_ABILITY_URI_PREFIX = "dataability://";
const char DATA_ABILITY_URI_SEPARATOR = '/';

const std::string PARAM_URI_SEPARATOR = ":///";
const uint32_t PARAM_URI_SEPARATOR_LEN = 4;
const std::string URI_SEPARATOR = "://";
const uint32_t URI_SEPARATOR_LEN = 3;
const std::string SEPARATOR = "/";

const int MAX_DIMENSION_SIZE = 10;
const int MAX_DEVICETYPE_SIZE = 50;

// appFeature
const std::string HOS_NORMAL_APP = "hos_normal_app";
const std::string HOS_SYSTEM_APP = "hos_system_app";
const std::string OHOS_SYSTEM_APP = "ohos_system_app";

// ipc
const int32_t MAX_CAPACITY_BUNDLES = 5 * 1024 * 1000; // 5M

// file size
const int32_t INVALID_FILE_SIZE = -1;

// permission
const std::string LISTEN_BUNDLE_CHANGE = "ohos.permission.LISTEN_BUNDLE_CHANGE";

// hmdfs config
constexpr std::string_view ABILTY_NAME = "abilityName";
const std::string HMDFS_CONFIG_PATH {"/config/hmdfs/"};
const std::string BUNDLE_ID_FILE = { "bid" };
const std::string PATH_SEPERATE {"/"};

// pre bundle profile
const std::string DEFAULT_PRE_BUNDLE_PROFILE = "/system/etc/bundle";
const std::string PREINSTALL_CONFIG = "/preinstall_config.json";
const std::string PREINSTALL_UNINSTALL_CONFIG = "/pre_uninstall_config.json";
const std::string PREINSTALL_ABILITY_CONFIG = "/preinstall_ability_config.json";

// sandbox application
const std::string SANDBOX_APP_INDEX = "sandbox_app_index";
const int32_t INITIAL_APP_INDEX = 0;
const int32_t MAX_APP_INDEX = 100;
}  // namespace Constants
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_CONSTANTS_H
