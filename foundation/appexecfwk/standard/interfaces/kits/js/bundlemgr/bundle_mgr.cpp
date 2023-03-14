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
#include "bundle_mgr.h"

#include <string>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_death_recipient.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "cleancache_callback.h"
#include "if_system_ability_manager.h"
#include "installer_callback.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
#include "pixel_map_napi.h"
#endif
#include "securec.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace {
constexpr size_t NAPI_ERR_NO_ERROR = 0;
constexpr size_t ARGS_ASYNC_COUNT = 1;
constexpr size_t ARGS_MAX_COUNT = 10;
constexpr size_t CALLBACK_SIZE = 1;
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_TWO = 2;
constexpr size_t ARGS_SIZE_THREE = 3;
constexpr size_t ARGS_SIZE_FOUR = 4;
constexpr int32_t DEFAULT_INT32 = 0;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t PARAM2 = 2;
constexpr int32_t PARAM3 = 3;
constexpr int32_t NAPI_RETURN_FAILED = -1;
constexpr int32_t NAPI_RETURN_ZERO = 0;
constexpr int32_t NAPI_RETURN_ONE = 1;
constexpr int32_t NAPI_RETURN_TWO = 2;
constexpr int32_t NAPI_RETURN_THREE = 3;
constexpr int32_t CODE_SUCCESS = 0;
constexpr int32_t CODE_FAILED = -1;
constexpr int32_t OPERATION_FAILED = 1;
constexpr int32_t INVALID_PARAM = 2;
constexpr int32_t PARAM_TYPE_ERROR = 1;
constexpr int32_t UNDEFINED_ERROR = -1;
enum class InstallErrorCode {
    SUCCESS = 0,
    STATUS_INSTALL_FAILURE = 1,
    STATUS_INSTALL_FAILURE_ABORTED = 2,
    STATUS_INSTALL_FAILURE_INVALID = 3,
    STATUS_INSTALL_FAILURE_CONFLICT = 4,
    STATUS_INSTALL_FAILURE_STORAGE = 5,
    STATUS_INSTALL_FAILURE_INCOMPATIBLE = 6,
    STATUS_UNINSTALL_FAILURE = 7,
    STATUS_UNINSTALL_FAILURE_BLOCKED = 8,
    STATUS_UNINSTALL_FAILURE_ABORTED = 9,
    STATUS_UNINSTALL_FAILURE_CONFLICT = 10,
    STATUS_INSTALL_FAILURE_DOWNLOAD_TIMEOUT = 0x0B,
    STATUS_INSTALL_FAILURE_DOWNLOAD_FAILED = 0x0C,
    STATUS_RECOVER_FAILURE_INVALID = 0x0D,
    STATUS_ABILITY_NOT_FOUND = 0x40,
    STATUS_BMS_SERVICE_ERROR = 0x41,
    STATUS_FAILED_NO_SPACE_LEFT = 0X42,
    STATUS_GRANT_REQUEST_PERMISSIONS_FAILED = 0X43,
    STATUS_INSTALL_PERMISSION_DENIED = 0X44,
    STATUS_UNINSTALL_PERMISSION_DENIED = 0X45,
    STATUS_USER_NOT_EXIST = 0X50,
    STATUS_USER_FAILURE_INVALID = 0X51,
    STATUS_USER_CREATE_FALIED = 0X52,
    STATUS_USER_REMOVE_FALIED = 0X53,
};

const std::string PERMISSION_CHANGE = "permissionChange";
const std::string ANY_PERMISSION_CHANGE = "anyPermissionChange";

const std::vector<int32_t> PACKINFO_FLAGS = {
    BundlePackFlag::GET_PACK_INFO_ALL,
    BundlePackFlag::GET_PACKAGES,
    BundlePackFlag::GET_BUNDLE_SUMMARY,
    BundlePackFlag::GET_MODULE_SUMMARY,
};

thread_local std::mutex g_permissionsCallbackMutex;
thread_local std::mutex g_anyPermissionsCallbackMutex;

struct PermissionsKey {
    napi_ref callback = 0;
    std::vector<int32_t> uids;
    bool operator<(const PermissionsKey &other) const
    {
        return this->callback < other.callback;
    }
};

static OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> bundleMgr_ = nullptr;
std::mutex bundleMgrMutex_;

}  // namespace

AsyncWorkData::AsyncWorkData(napi_env napiEnv)
{
    env = napiEnv;
}

AsyncWorkData::~AsyncWorkData()
{
    if (callback) {
        APP_LOGD("AsyncWorkData::~AsyncWorkData delete callback");
        napi_delete_reference(env, callback);
        callback = nullptr;
    }
    if (asyncWork) {
        APP_LOGD("AsyncWorkData::~AsyncWorkData delete asyncWork");
        napi_delete_async_work(env, asyncWork);
        asyncWork = nullptr;
    }
}

napi_ref thread_local g_classBundleInstaller;

static OHOS::sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgr()
{
    if (bundleMgr_ == nullptr) {
        std::lock_guard<std::mutex> lock(bundleMgrMutex_);
        if (bundleMgr_ == nullptr) {
            auto systemAbilityManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (systemAbilityManager == nullptr) {
                APP_LOGE("GetBundleMgr GetSystemAbilityManager is null");
                return nullptr;
            }
            auto bundleMgrSa = systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
            if (bundleMgrSa == nullptr) {
                APP_LOGE("GetBundleMgr GetSystemAbility is null");
                return nullptr;
            }
            auto bundleMgr = OHOS::iface_cast<IBundleMgr>(bundleMgrSa);
            if (bundleMgr == nullptr) {
                APP_LOGE("GetBundleMgr iface_cast get null");
            }
            bundleMgr_ = bundleMgr;
        }
    }
    return bundleMgr_;
}

static bool CheckIsSystemApp()
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }

    int32_t uid = IPCSkeleton::GetCallingUid();
    return iBundleMgr->CheckIsSystemAppByUid(uid);
}

static void ConvertCustomizeData(napi_env env, napi_value objCustomizeData, const CustomizeData &customizeData)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, customizeData.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objCustomizeData, "name", nName));
    napi_value nValue;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, customizeData.value.c_str(), NAPI_AUTO_LENGTH, &nValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objCustomizeData, "value", nValue));
    napi_value nExtra;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, customizeData.extra.c_str(), NAPI_AUTO_LENGTH, &nExtra));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objCustomizeData, "extra", nExtra));
}

static void ConvertInnerMetadata(napi_env env, napi_value value, const Metadata &metadata)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, metadata.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "name", nName));
    napi_value nValue;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, metadata.value.c_str(), NAPI_AUTO_LENGTH, &nValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "value", nValue));
    napi_value nResource;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, metadata.resource.c_str(), NAPI_AUTO_LENGTH, &nResource));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "resource", nResource));
}

static void ConvertApplicationInfo(napi_env env, napi_value objAppInfo, const ApplicationInfo &appInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "name", nName));
    APP_LOGI("ConvertApplicationInfo name=%{public}s.", appInfo.name.c_str());

    napi_value nCodePath;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.codePath.c_str(), NAPI_AUTO_LENGTH, &nCodePath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "codePath", nCodePath));

    napi_value nAccessTokenId;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, appInfo.accessTokenId, &nAccessTokenId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "accessTokenId", nAccessTokenId));

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "description", nDescription));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "descriptionId", nDescriptionId));

    napi_value nIconPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.iconPath.c_str(), NAPI_AUTO_LENGTH, &nIconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "icon", nIconPath));

    napi_value nIconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.iconId, &nIconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "iconId", nIconId));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "label", nLabel));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "labelId", nLabelId));

    napi_value nIsSystemApp;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.isSystemApp, &nIsSystemApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "systemApp", nIsSystemApp));

    napi_value nSupportedModes;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.supportedModes, &nSupportedModes));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "supportedModes", nSupportedModes));

    napi_value nProcess;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.process.c_str(), NAPI_AUTO_LENGTH, &nProcess));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "process", nProcess));

    napi_value nEntryDir;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, appInfo.entryDir.c_str(), NAPI_AUTO_LENGTH, &nEntryDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "entryDir", nEntryDir));

    napi_value nPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nPermissions));
    for (size_t idx = 0; idx < appInfo.permissions.size(); idx++) {
        napi_value nPermission;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, appInfo.permissions[idx].c_str(), NAPI_AUTO_LENGTH, &nPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nPermissions, idx, nPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "permissions", nPermissions));

    napi_value nModuleSourceDirs;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleSourceDirs));
    for (size_t idx = 0; idx < appInfo.moduleSourceDirs.size(); idx++) {
        napi_value nModuleSourceDir;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, appInfo.moduleSourceDirs[idx].c_str(), NAPI_AUTO_LENGTH, &nModuleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleSourceDirs, idx, nModuleSourceDir));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "moduleSourceDirs", nModuleSourceDirs));

    napi_value nModuleInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nModuleInfos));
    for (size_t idx = 0; idx < appInfo.moduleInfos.size(); idx++) {
        napi_value objModuleInfos;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objModuleInfos));

        napi_value nModuleName;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, appInfo.moduleInfos[idx].moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objModuleInfos, "moduleName", nModuleName));

        napi_value nModuleSourceDir;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, appInfo.moduleInfos[idx].moduleSourceDir.c_str(), NAPI_AUTO_LENGTH, &nModuleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objModuleInfos, "moduleSourceDir", nModuleSourceDir));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nModuleInfos, idx, objModuleInfos));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "moduleInfos", nModuleInfos));

    napi_value nMetaData;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nMetaData));
    for (const auto &item : appInfo.metaData) {
        napi_value nCustomizeDataArray;
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nCustomizeDataArray));
        for (size_t j = 0; j < item.second.size(); j++) {
            napi_value nCustomizeData;
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nCustomizeData));
            ConvertCustomizeData(env, nCustomizeData, item.second[j]);
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nCustomizeDataArray, j, nCustomizeData));
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nMetaData, item.first.c_str(), nCustomizeDataArray));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "metaData", nMetaData));

    napi_value nMetadata;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nMetadata));
    for (const auto &item : appInfo.metadata) {
        napi_value nInnerMetadata;
        size_t len = item.second.size();
        NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, len, &nInnerMetadata));
        for (size_t index = 0; index < len; ++index) {
            napi_value nMeta;
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nMeta));
            ConvertInnerMetadata(env, nMeta, item.second[index]);
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nInnerMetadata, index, nMeta));
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nMetadata, item.first.c_str(), nInnerMetadata));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "metadata", nMetadata));

    napi_value nEnabled;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.enabled, &nEnabled));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "enabled", nEnabled));

    napi_value nFlags;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, DEFAULT_INT32, &nFlags));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "flags", nFlags));

    napi_value nUid;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, appInfo.uid, &nUid));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "uid", nUid));

    napi_value nEntityType;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, appInfo.entityType.c_str(), NAPI_AUTO_LENGTH,
        &nEntityType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "entityType", nEntityType));

    napi_value nRemovable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, appInfo.removable, &nRemovable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAppInfo, "removable", nRemovable));

    APP_LOGI("ConvertApplicationInfo entryDir=%{public}s.", appInfo.entryDir.c_str());
}

static void ConvertMetaData(napi_env env, napi_value objMetaData, const MetaData &metaData)
{
    for (size_t idx = 0; idx < metaData.customizeData.size(); idx++) {
        napi_value nCustomizeData;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nCustomizeData));
        ConvertCustomizeData(env, nCustomizeData, metaData.customizeData[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, objMetaData, idx, nCustomizeData));
    }
}

static void ConvertAbilityInfo(napi_env env, napi_value objAbilityInfo, const AbilityInfo &abilityInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "name", nName));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "label", nLabel));

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "description", nDescription));

    napi_value nIconPath;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.iconPath.c_str(), NAPI_AUTO_LENGTH, &nIconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "icon", nIconPath));

    napi_value nsrcPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.srcPath.c_str(), NAPI_AUTO_LENGTH, &nsrcPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "srcPath", nsrcPath));

    napi_value nLaunguage;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.srcLanguage.c_str(), NAPI_AUTO_LENGTH, &nLaunguage));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "srcLanguage", nLaunguage));

    napi_value nVisible;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, abilityInfo.visible, &nVisible));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "isVisible", nVisible));

    napi_value nPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nPermissions));
    for (size_t idx = 0; idx < abilityInfo.permissions.size(); idx++) {
        napi_value nPermission;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, abilityInfo.permissions[idx].c_str(), NAPI_AUTO_LENGTH, &nPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nPermissions, idx, nPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "permissions", nPermissions));

    napi_value nDeviceCapabilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nDeviceCapabilities));
    for (size_t idx = 0; idx < abilityInfo.deviceCapabilities.size(); idx++) {
        napi_value nDeviceCapability;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, abilityInfo.deviceCapabilities[idx].c_str(), NAPI_AUTO_LENGTH, &nDeviceCapability));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nDeviceCapabilities, idx, nDeviceCapability));
    }
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, objAbilityInfo, "deviceCapabilities", nDeviceCapabilities));

    napi_value nDeviceTypes;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nDeviceTypes));
    for (size_t idx = 0; idx < abilityInfo.deviceTypes.size(); idx++) {
        napi_value nDeviceType;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, abilityInfo.deviceTypes[idx].c_str(), NAPI_AUTO_LENGTH, &nDeviceType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nDeviceTypes, idx, nDeviceType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "deviceTypes", nDeviceTypes));

    napi_value nProcess;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.process.c_str(), NAPI_AUTO_LENGTH, &nProcess));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "process", nProcess));

    napi_value nUri;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.uri.c_str(), NAPI_AUTO_LENGTH, &nUri));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "uri", nUri));

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "bundleName", nBundleName));

    napi_value nModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "moduleName", nModuleName));

    napi_value nAppInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nAppInfo));
    ConvertApplicationInfo(env, nAppInfo, abilityInfo.applicationInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "applicationInfo", nAppInfo));

    napi_value nType;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(abilityInfo.type), &nType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "type", nType));

    napi_value nOrientation;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(abilityInfo.orientation), &nOrientation));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "orientation", nOrientation));

    napi_value nLaunchMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(abilityInfo.launchMode), &nLaunchMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "launchMode", nLaunchMode));

    napi_value nBackgroundModes;
    if (!abilityInfo.isModuleJson) {
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, abilityInfo.backgroundModes, &nBackgroundModes));
    } else {
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 0, &nBackgroundModes));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "backgroundModes", nBackgroundModes));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, abilityInfo.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "descriptionId", nDescriptionId));

    napi_value nFormEnabled;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, abilityInfo.formEnabled, &nFormEnabled));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "formEnabled", nFormEnabled));

    napi_value nIconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, abilityInfo.iconId, &nIconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "iconId", nIconId));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, abilityInfo.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "labelId", nLabelId));

    napi_value nSubType;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(abilityInfo.subType), &nSubType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "subType", nSubType));

    napi_value nReadPermission;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.readPermission.c_str(), NAPI_AUTO_LENGTH, &nReadPermission));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "readPermission", nReadPermission));

    napi_value nWritePermission;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.writePermission.c_str(), NAPI_AUTO_LENGTH, &nWritePermission));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "writePermission", nWritePermission));

    napi_value nTargetAbility;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, abilityInfo.targetAbility.c_str(), NAPI_AUTO_LENGTH, &nTargetAbility));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "targetAbility", nTargetAbility));

    napi_value nTheme;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.theme.c_str(), NAPI_AUTO_LENGTH, &nTheme));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "theme", nTheme));

    napi_value nMetaData;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nMetaData));
    ConvertMetaData(env, nMetaData, abilityInfo.metaData);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "metaData", nMetaData));

    napi_value nMetadata;
    size_t size = abilityInfo.metadata.size();
    NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, size, &nMetadata));
    for (size_t index = 0; index < size; ++index) {
        napi_value innerMeta;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &innerMeta));
        ConvertInnerMetadata(env, innerMeta, abilityInfo.metadata[index]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nMetadata, index, innerMeta));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "metadata", nMetadata));

    napi_value nEnabled;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, abilityInfo.enabled, &nEnabled));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "enabled", nEnabled));
}

static void ProcessAbilityInfos(
    napi_env env, napi_value result, const std::vector<OHOS::AppExecFwk::AbilityInfo> &abilityInfos)
{
    if (abilityInfos.size() > 0) {
        APP_LOGI("-----abilityInfos is not null-----");
        size_t index = 0;
        for (const auto &item : abilityInfos) {
            APP_LOGI("name{%s} ", item.name.c_str());
            napi_value objAbilityInfo = nullptr;
            napi_create_object(env, &objAbilityInfo);
            ConvertAbilityInfo(env, objAbilityInfo, item);
            napi_set_element(env, result, index, objAbilityInfo);
            index++;
        }
    } else {
        APP_LOGI("-----abilityInfos is null-----");
    }
}

static void ConvertExtensionInfos(napi_env env, napi_value result,
    std::vector<OHOS::AppExecFwk::ExtensionAbilityInfo> extensionInfos)
{
    APP_LOGD("convert extensionInfos start");
    if (extensionInfos.empty()) {
        APP_LOGW("extensionInfos is null");
        return;
    }

    for (size_t i = 0; i < extensionInfos.size(); ++i) {
        napi_value value = nullptr;
        napi_create_object(env, &value);
        napi_value nBundleName;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, extensionInfos[i].bundleName.c_str(),
            NAPI_AUTO_LENGTH, &nBundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "bundleName", nBundleName));

        napi_value nModuleName;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, extensionInfos[i].moduleName.c_str(),
            NAPI_AUTO_LENGTH, &nModuleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "moduleName", nModuleName));

        napi_value nName;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, extensionInfos[i].name.c_str(), NAPI_AUTO_LENGTH, &nName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "name", nName));

        napi_value nLabelId;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, extensionInfos[i].labelId, &nLabelId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "labelId", nLabelId));

        napi_value nDescriptionId;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, extensionInfos[i].descriptionId, &nDescriptionId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "descriptionId", nDescriptionId));

        napi_value nIconId;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, extensionInfos[i].iconId, &nIconId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "iconId", nIconId));

        napi_value nIsVisible;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, extensionInfos[i].visible, &nIsVisible));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "isVisible", nIsVisible));

        napi_value nExtensionAbilityType;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_int32(env, static_cast<int32_t>(extensionInfos[i].type), &nExtensionAbilityType));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "extensionAbilityType", nExtensionAbilityType));

        napi_value nPermission;
        size_t size = extensionInfos[i].permissions.size();
        NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, size, &nPermission));
        for (size_t j = 0; j < size; ++j) {
            napi_value innerPerm;
            NAPI_CALL_RETURN_VOID(env,
                napi_create_string_utf8(env, extensionInfos[i].permissions[j].c_str(), NAPI_AUTO_LENGTH, &innerPerm));
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nPermission, j, innerPerm));
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "permissions", nPermission));

        napi_value nApplication;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nApplication));
        ConvertApplicationInfo(env, nApplication, extensionInfos[i].applicationInfo);
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "applicationInfo", nApplication));

        napi_value nMetadata;
        size = extensionInfos[i].metadata.size();
        NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, size, &nMetadata));
        for (size_t index = 0; index < size; ++index) {
            napi_value innerMeta;
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &innerMeta));
            ConvertInnerMetadata(env, innerMeta, extensionInfos[i].metadata[index]);
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nMetadata, index, innerMeta));
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "metadata", nMetadata));

        napi_value nEnabled;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, extensionInfos[i].enabled, &nEnabled));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "enabled", nEnabled));

        napi_value nReadPermission;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, extensionInfos[i].readPermission.c_str(),
            NAPI_AUTO_LENGTH, &nReadPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "readPermission", nReadPermission));

        napi_value nWritePermission;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, extensionInfos[i].writePermission.c_str(),
            NAPI_AUTO_LENGTH, &nWritePermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "writePermission", nWritePermission));
        napi_set_element(env, result, i, value);
    }
    APP_LOGD("convert extensionInfos finished");
}

static void ConvertHapModuleInfo(napi_env env, napi_value objHapModuleInfo, const HapModuleInfo &hapModuleInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, hapModuleInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "name", nName));
    APP_LOGI("ConvertHapModuleInfo name=%{public}s.", hapModuleInfo.name.c_str());

    napi_value nModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, hapModuleInfo.moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "moduleName", nModuleName));
    APP_LOGI("ConvertHapModuleInfo moduleName=%{public}s.", hapModuleInfo.moduleName.c_str());

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, hapModuleInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "description", nDescription));
    APP_LOGI("ConvertHapModuleInfo description=%{public}s.", hapModuleInfo.description.c_str());

    napi_value ndescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 0, &ndescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "descriptionId", ndescriptionId));

    napi_value nIconPath;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, hapModuleInfo.iconPath.c_str(), NAPI_AUTO_LENGTH, &nIconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "iconPath", nIconPath));
    APP_LOGI("ConvertHapModuleInfo iconPath=%{private}s.", hapModuleInfo.iconPath.c_str());

    napi_value nIcon;
    std::string theIcon = "";
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, theIcon.c_str(), NAPI_AUTO_LENGTH, &nIcon));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "icon", nIcon));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, hapModuleInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "label", nLabel));
    APP_LOGI("ConvertHapModuleInfo label=%{public}s.", hapModuleInfo.label.c_str());

    napi_value nHashValue;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, hapModuleInfo.hashValue.c_str(), NAPI_AUTO_LENGTH, &nHashValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "hashValue", nHashValue));
    APP_LOGI("ConvertHapModuleInfo hashValue=%{public}s.", hapModuleInfo.hashValue.c_str());

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 0, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "labelId", nLabelId));

    napi_value nIconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 0, &nIconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "iconId", nIconId));

    napi_value nBackgroundImg;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, hapModuleInfo.backgroundImg.c_str(), NAPI_AUTO_LENGTH, &nBackgroundImg));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "backgroundImg", nBackgroundImg));
    APP_LOGI("ConvertHapModuleInfo backgroundImg=%{public}s.", hapModuleInfo.backgroundImg.c_str());

    napi_value nSupportedModes;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, hapModuleInfo.supportedModes, &nSupportedModes));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "supportedModes", nSupportedModes));

    napi_value nReqCapabilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nReqCapabilities));
    for (size_t idx = 0; idx < hapModuleInfo.reqCapabilities.size(); idx++) {
        napi_value nReqCapabilitie;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(
                env, hapModuleInfo.reqCapabilities[idx].c_str(), NAPI_AUTO_LENGTH, &nReqCapabilitie));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nReqCapabilities, idx, nReqCapabilitie));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "reqCapabilities", nReqCapabilities));

    napi_value nDeviceTypes;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nDeviceTypes));
    for (size_t idx = 0; idx < hapModuleInfo.deviceTypes.size(); idx++) {
        napi_value nDeviceType;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, hapModuleInfo.deviceTypes[idx].c_str(), NAPI_AUTO_LENGTH, &nDeviceType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nDeviceTypes, idx, nDeviceType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "deviceTypes", nDeviceTypes));

    napi_value nAbilityInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilityInfos));
    for (size_t idx = 0; idx < hapModuleInfo.abilityInfos.size(); idx++) {
        napi_value objAbilityInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objAbilityInfo));
        ConvertAbilityInfo(env, objAbilityInfo, hapModuleInfo.abilityInfos[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilityInfos, idx, objAbilityInfo));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "abilityInfo", nAbilityInfos));

    napi_value nColorMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(hapModuleInfo.colorMode), &nColorMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "colorMode", nColorMode));

    napi_value nMainAbilityName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, hapModuleInfo.mainAbility.c_str(), NAPI_AUTO_LENGTH, &nMainAbilityName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "mainAbilityName", nMainAbilityName));
    APP_LOGI("ConvertHapModuleInfo mainAbilityName=%{public}s.", hapModuleInfo.mainAbility.c_str());

    napi_value nInstallationFree;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, false, &nInstallationFree));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "installationFree", nInstallationFree));

    napi_value nMainElementName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, hapModuleInfo.mainElementName.c_str(), NAPI_AUTO_LENGTH,
        &nMainElementName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "mainElementName", nMainElementName));

    napi_value nExtensionAbilityInfos;
    napi_create_array_with_length(env, hapModuleInfo.extensionInfos.size(), &nExtensionAbilityInfos);
    ConvertExtensionInfos(env, nExtensionAbilityInfos, hapModuleInfo.extensionInfos);
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, objHapModuleInfo, "extensionAbilityInfo", nExtensionAbilityInfos));

    napi_value nMetadata;
    size_t size = hapModuleInfo.metadata.size();
    NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, size, &nMetadata));
    for (size_t index = 0; index < size; ++index) {
        napi_value innerMeta;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &innerMeta));
        ConvertInnerMetadata(env, innerMeta, hapModuleInfo.metadata[index]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nMetadata, index, innerMeta));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objHapModuleInfo, "metadata", nMetadata));
}

static void ConvertRequestPermissionUsedScene(napi_env env, napi_value result,
    const RequestPermissionUsedScene &requestPermissionUsedScene)
{
    napi_value nAbilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilities));
    for (size_t idx = 0; idx < requestPermissionUsedScene.abilities.size(); idx++) {
        napi_value objAbility;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, requestPermissionUsedScene.abilities[idx].c_str(),
                                    NAPI_AUTO_LENGTH, &objAbility));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilities, idx, objAbility));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "abilities", nAbilities));

    napi_value nWhen;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, requestPermissionUsedScene.when.c_str(), NAPI_AUTO_LENGTH, &nWhen));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "when", nWhen));
}

static void ConvertRequestPermission(napi_env env, napi_value result, const RequestPermission &requestPermission)
{
    napi_value nPermissionName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, requestPermission.name.c_str(), NAPI_AUTO_LENGTH, &nPermissionName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "name", nPermissionName));

    napi_value nReason;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, requestPermission.reason.c_str(), NAPI_AUTO_LENGTH, &nReason));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "reason", nReason));

    napi_value nReasonId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, requestPermission.reasonId, &nReasonId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "reasonId", nReasonId));

    napi_value nUsedScene;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nUsedScene));
    ConvertRequestPermissionUsedScene(env, nUsedScene, requestPermission.usedScene);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "usedScene", nUsedScene));
}

static void ConvertBundleInfo(napi_env env, napi_value objBundleInfo, const BundleInfo &bundleInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "name", nName));

    napi_value nVendor;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.vendor.c_str(), NAPI_AUTO_LENGTH, &nVendor));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "vendor", nVendor));

    napi_value nVersionCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, bundleInfo.versionCode, &nVersionCode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "versionCode", nVersionCode));

    napi_value nVersionName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.versionName.c_str(), NAPI_AUTO_LENGTH, &nVersionName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "versionName", nVersionName));

    napi_value nCpuAbi;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.cpuAbi.c_str(), NAPI_AUTO_LENGTH, &nCpuAbi));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "cpuAbi", nCpuAbi));

    napi_value nAppId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.appId.c_str(), NAPI_AUTO_LENGTH, &nAppId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "appId", nAppId));

    napi_value nEntryModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, bundleInfo.entryModuleName.c_str(), NAPI_AUTO_LENGTH, &nEntryModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "entryModuleName", nEntryModuleName));

    napi_value nCompatibleVersion;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.compatibleVersion, &nCompatibleVersion));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "compatibleVersion", nCompatibleVersion));

    napi_value nTargetVersion;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.targetVersion, &nTargetVersion));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "targetVersion", nTargetVersion));

    napi_value nUid;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInfo.uid, &nUid));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "uid", nUid));

    napi_value nInstallTime;
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, bundleInfo.installTime, &nInstallTime));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "installTime", nInstallTime));

    napi_value nUpdateTime;
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, bundleInfo.updateTime, &nUpdateTime));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "updateTime", nUpdateTime));

    napi_value nAppInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nAppInfo));
    ConvertApplicationInfo(env, nAppInfo, bundleInfo.applicationInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "appInfo", nAppInfo));

    napi_value nAbilityInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilityInfos));
    for (size_t idx = 0; idx < bundleInfo.abilityInfos.size(); idx++) {
        napi_value objAbilityInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objAbilityInfo));
        ConvertAbilityInfo(env, objAbilityInfo, bundleInfo.abilityInfos[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilityInfos, idx, objAbilityInfo));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "abilityInfos", nAbilityInfos));

    napi_value nHapModuleInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nHapModuleInfos));
    for (size_t idx = 0; idx < bundleInfo.hapModuleInfos.size(); idx++) {
        napi_value objHapModuleInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objHapModuleInfo));
        ConvertHapModuleInfo(env, objHapModuleInfo, bundleInfo.hapModuleInfos[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nHapModuleInfos, idx, objHapModuleInfo));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "hapModuleInfos", nHapModuleInfos));

    napi_value nReqPermissions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nReqPermissions));
    for (size_t idx = 0; idx < bundleInfo.reqPermissions.size(); idx++) {
        napi_value nReqPermission;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_string_utf8(env, bundleInfo.reqPermissions[idx].c_str(), NAPI_AUTO_LENGTH, &nReqPermission));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nReqPermissions, idx, nReqPermission));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "reqPermissions", nReqPermissions));

    napi_value nReqPermissionStates;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nReqPermissionStates));
    for (size_t idx = 0; idx < bundleInfo.reqPermissionStates.size(); idx++) {
        napi_value nReqPermissionState;
        NAPI_CALL_RETURN_VOID(env,
            napi_create_int32(env, bundleInfo.reqPermissionStates[idx], &nReqPermissionState));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nReqPermissionStates, idx, nReqPermissionState));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "reqPermissionStates",
        nReqPermissionStates));

    napi_value nIsCompressNativeLibs;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, false, &nIsCompressNativeLibs));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, objBundleInfo, "isCompressNativeLibs", nIsCompressNativeLibs));

    napi_value nIsSilentInstallation;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, std::string().c_str(), NAPI_AUTO_LENGTH, &nIsSilentInstallation));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, objBundleInfo, "isSilentInstallation", nIsSilentInstallation));

    napi_value nType;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, std::string().c_str(), NAPI_AUTO_LENGTH, &nType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "type", nType));

    napi_value nReqPermissionDetails;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nReqPermissionDetails));
    for (size_t idx = 0; idx < bundleInfo.reqPermissionDetails.size(); idx++) {
        napi_value objReqPermission;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objReqPermission));
        ConvertRequestPermission(env, objReqPermission, bundleInfo.reqPermissionDetails[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nReqPermissionDetails, idx, objReqPermission));
    }
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, objBundleInfo, "reqPermissionDetails", nReqPermissionDetails));

    napi_value nMinCompatibleVersionCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, DEFAULT_INT32, &nMinCompatibleVersionCode));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, objBundleInfo, "minCompatibleVersionCode", nMinCompatibleVersionCode));

    napi_value nEntryInstallationFree;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, bundleInfo.entryInstallationFree, &nEntryInstallationFree));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, objBundleInfo, "entryInstallationFree", nEntryInstallationFree));

    napi_value nExtensionAbilityInfos;
    napi_create_array_with_length(env, bundleInfo.extensionInfos.size(), &nExtensionAbilityInfos);
    ConvertExtensionInfos(env, nExtensionAbilityInfos, bundleInfo.extensionInfos);
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, objBundleInfo, "extensionAbilityInfo", nExtensionAbilityInfos));
}

static void ConvertFormCustomizeData(napi_env env, napi_value objformInfo, const FormCustomizeData &customizeData)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, customizeData.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "name", nName));
    napi_value nValue;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, customizeData.value.c_str(), NAPI_AUTO_LENGTH, &nValue));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "value", nValue));
}

static void ConvertFormWindow(napi_env env, napi_value objWindowInfo, const FormWindow &formWindow)
{
    napi_value nDesignWidth;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, formWindow.designWidth, &nDesignWidth));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objWindowInfo, "designWidth", nDesignWidth));
    napi_value nAutoDesignWidth;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, formWindow.autoDesignWidth, &nAutoDesignWidth));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objWindowInfo, "autoDesignWidth", nAutoDesignWidth));
}

static void ConvertFormInfo(napi_env env, napi_value objformInfo, const FormInfo &formInfo)
{
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, formInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "name", nName));
    APP_LOGI("ConvertFormInfo name=%{public}s.", formInfo.name.c_str());

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, formInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "description", nDescription));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, formInfo.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "descriptionId", nDescriptionId));

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, formInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "bundleName", nBundleName));

    napi_value nModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, formInfo.moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "moduleName", nModuleName));

    napi_value nAbilityName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, formInfo.abilityName.c_str(), NAPI_AUTO_LENGTH, &nAbilityName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "abilityName", nAbilityName));

    napi_value nRelatedBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, formInfo.relatedBundleName.c_str(), NAPI_AUTO_LENGTH, &nRelatedBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "relatedBundleName", nRelatedBundleName));

    napi_value nDefaultFlag;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, formInfo.defaultFlag, &nDefaultFlag));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "defaultFlag", nDefaultFlag));

    napi_value nFormVisibleNotify;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, formInfo.formVisibleNotify, &nFormVisibleNotify));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "formVisibleNotify", nFormVisibleNotify));

    napi_value nFormConfigAbility;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, formInfo.formConfigAbility.c_str(), NAPI_AUTO_LENGTH, &nFormConfigAbility));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "formConfigAbility", nFormConfigAbility));

    napi_value nType;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(formInfo.type), &nType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "type", nType));

    napi_value nColorMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(formInfo.colorMode), &nColorMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "colorMode", nColorMode));

    napi_value nSupportDimensions;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nSupportDimensions));
    for (size_t idx = 0; idx < formInfo.supportDimensions.size(); idx++) {
        napi_value nSupportDimension;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, formInfo.supportDimensions[idx], &nSupportDimension));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nSupportDimensions, idx, nSupportDimension));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "supportDimensions", nSupportDimensions));

    napi_value nDefaultDimension;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, formInfo.defaultDimension, &nDefaultDimension));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "defaultDimension", nDefaultDimension));

    napi_value nJsComponentName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, formInfo.jsComponentName.c_str(), NAPI_AUTO_LENGTH, &nJsComponentName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "jsComponentName", nJsComponentName));

    napi_value nUpdateDuration;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, formInfo.updateDuration, &nUpdateDuration));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "updateDuration", nUpdateDuration));

    napi_value nCustomizeDatas;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nCustomizeDatas));
    for (size_t idx = 0; idx < formInfo.customizeDatas.size(); idx++) {
        napi_value nCustomizeData;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nCustomizeData));
        ConvertFormCustomizeData(env, nCustomizeData, formInfo.customizeDatas[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nCustomizeDatas, idx, nCustomizeData));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "customizeDatas", nCustomizeDatas));

    napi_value nSrc;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, formInfo.src.c_str(), NAPI_AUTO_LENGTH, &nSrc));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "src", nSrc));
    APP_LOGI("ConvertFormInfo src=%{public}s.", formInfo.src.c_str());

    napi_value nWindow;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nWindow));
    ConvertFormWindow(env, nWindow, formInfo.window);
    APP_LOGI("ConvertFormInfo window.designWidth=%{public}d.", formInfo.window.designWidth);
    APP_LOGI("ConvertFormInfo window.autoDesignWidth=%{public}d.", formInfo.window.autoDesignWidth);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objformInfo, "window", nWindow));
}

static void ConvertWantInfo(napi_env env, napi_value objWantInfo, const Want &want)
{
    ElementName elementName = want.GetElement();
    napi_value nbundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetBundleName().c_str(), NAPI_AUTO_LENGTH, &nbundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objWantInfo, "bundleName", nbundleName));

    napi_value ndeviceId;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &ndeviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objWantInfo, "deviceId", ndeviceId));

    napi_value nabilityName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, elementName.GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &nabilityName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objWantInfo, "abilityName", nabilityName));

    napi_value naction;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, want.GetAction().c_str(), NAPI_AUTO_LENGTH, &naction));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objWantInfo, "action", naction));

    auto entities = want.GetEntities();
    napi_value nGetEntities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nGetEntities));
    if (entities.size() > 0) {
        size_t index = 0;
        for (const auto &item:entities) {
            napi_value objEntities;
            NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, item.c_str(), NAPI_AUTO_LENGTH, &objEntities));
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nGetEntities, index, objEntities));
            index++;
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objWantInfo, "entities", nGetEntities));
    }
}

static std::string GetStringFromNAPI(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_string) {
        APP_LOGE("GetStringFromNAPI type mismatch!");
        return "";
    }
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, NAPI_RETURN_ZERO, &size) != napi_ok) {
        APP_LOGE("can not get string size");
        return "";
    }
    result.reserve(size + NAPI_RETURN_ONE);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + NAPI_RETURN_ONE), &size) != napi_ok) {
        APP_LOGE("can not get string value");
        return "";
    }
    return result;
}

static napi_value ParseInt(napi_env env, int &param, napi_value args)
{
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    APP_LOGI("valuetype=%{public}d.", valuetype);
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. int32 expected.");
    int32_t value = 0;
    napi_get_value_int32(env, args, &value);
    APP_LOGI("param=%{public}d.", value);
    param = value;
    // create result code
    napi_value result = nullptr;
    napi_status status = napi_create_int32(env, NAPI_RETURN_ONE, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static napi_value GetCallbackErrorValue(napi_env env, int errCode)
{
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    return result;
}

static bool InnerGetApplicationInfos(napi_env env, int32_t flags, const int userId,
    std::vector<OHOS::AppExecFwk::ApplicationInfo> &appInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->GetApplicationInfos(flags, userId, appInfos);
}

static std::string InnerGetAbilityLabel(napi_env env, std::string &bundleName, std::string &moduleName,
    std::string &abilityName, bool hasModuleName)
{
    auto iBundleMgr = GetBundleMgr();
    if (iBundleMgr == nullptr) {
        APP_LOGE("can not get iBundleMgr");
        return "";
    }
    if (hasModuleName) {
        return iBundleMgr->GetAbilityLabel(bundleName, moduleName, abilityName);
    }
    return iBundleMgr->GetAbilityLabel(bundleName, abilityName);
}

static void ProcessApplicationInfos(
    napi_env env, napi_value result, const std::vector<OHOS::AppExecFwk::ApplicationInfo> &appInfos)
{
    if (appInfos.size() > 0) {
        APP_LOGI("-----appInfos is not null-----");
        size_t index = 0;
        for (const auto &item : appInfos) {
            APP_LOGI("name{%s} ", item.name.c_str());
            APP_LOGI("bundleName{%s} ", item.bundleName.c_str());
            for (const auto &moduleInfo : item.moduleInfos) {
                APP_LOGI("moduleName{%s} ", moduleInfo.moduleName.c_str());
                APP_LOGI("bundleName{%s} ", moduleInfo.moduleSourceDir.c_str());
            }
            napi_value objAppInfo;
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objAppInfo));
            ConvertApplicationInfo(env, objAppInfo, item);
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, objAppInfo));
            index++;
        }
    } else {
        APP_LOGI("-----appInfos is null-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetApplicationInfos(napi_env env, napi_callback_info info)
{
    APP_LOGD("NAPI_GetApplicationInfos called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGI("argc = [%{public}zu]", argc);
    AsyncApplicationInfosCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncApplicationInfosCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncApplicationInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->flags, argv[i]);
            if (argc == ARGS_SIZE_ONE) {
                asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
            }
        } else if ((i == ARGS_SIZE_ONE) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->userId, argv[i]);
        } else if ((i == ARGS_SIZE_ONE) && (valueType == napi_function)) {
            asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else if ((i == ARGS_SIZE_TWO) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    if (argc == 0) {
        asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        asyncCallbackInfo->message = "type mismatch";
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetApplicationInfo", NAPI_AUTO_LENGTH, &resource));

    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncApplicationInfosCallbackInfo* asyncCallbackInfo = (AsyncApplicationInfosCallbackInfo*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetApplicationInfos(asyncCallbackInfo->env,
                                                                  asyncCallbackInfo->flags,
                                                                  asyncCallbackInfo->userId,
                                                                  asyncCallbackInfo->appInfos);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncApplicationInfosCallbackInfo* asyncCallbackInfo = (AsyncApplicationInfosCallbackInfo*)data;
            std::unique_ptr<AsyncApplicationInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
                    NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[1]));
                    ProcessApplicationInfos(env, result[1], asyncCallbackInfo->appInfos);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
              } else {
                  NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
              }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static napi_value ParseStringArray(napi_env env, std::vector<std::string> &stringArray, napi_value args)
{
    APP_LOGD("begin to parse string array");
    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, args, &isArray));
    if (!isArray) {
        APP_LOGE("args not array");
        return nullptr;
    }
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, args, &arrayLength));
    APP_LOGD("length=%{public}ud", arrayLength);
    for (uint32_t j = 0; j < arrayLength; j++) {
        napi_value value = nullptr;
        NAPI_CALL(env, napi_get_element(env, args, j, &value));
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, value, &valueType));
        if (valueType != napi_string) {
            APP_LOGE("array inside not string type");
            stringArray.clear();
            return nullptr;
        }
        stringArray.push_back(GetStringFromNAPI(env, value));
    }
    // create result code
    napi_value result;
    napi_status status;
    status = napi_create_int32(env, NAPI_RETURN_ONE, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

// QueryAbilityInfos(want)
static bool InnerQueryAbilityInfos(napi_env env, const Want &want,
    int32_t flags, int32_t userId, std::vector<AbilityInfo> &abilityInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->QueryAbilityInfos(want, flags, userId, abilityInfos);
}

static bool ParseBundleOptions(napi_env env, BundleOptions &bundleOptions, napi_value args)
{
    APP_LOGD("begin to parse bundleOptions");
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("args not object type");
        return false;
    }

    napi_value prop = nullptr;
    status = napi_get_named_property(env, args, "userId", &prop);
    napi_typeof(env, prop, &valueType);
    if (valueType == napi_number) {
        napi_get_value_int32(env, prop, &bundleOptions.userId);
    }
    return true;
}

static bool ParseWant(napi_env env, Want &want, napi_value args)
{
    APP_LOGD("begin to parse want");
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("args not object type");
        return false;
    }
    std::string wantBundleName;
    std::string wantModuleName;
    std::string wantAbilityName;
    std::string wantDeviceId;
    std::string wantType;
    int32_t wantFlags = 0;
    std::string wantAction;
    std::string wantUri;
    std::vector<std::string> wantEntities;
    std::string elementUri;
    std::string elementDeviceId;
    std::string elementBundleName;
    std::string elementModuleName;
    std::string elementAbilityName;

    napi_value prop = nullptr;
    status = napi_get_named_property(env, args, "bundleName", &prop);
    wantBundleName = GetStringFromNAPI(env, prop);

    prop = nullptr;
    status = napi_get_named_property(env, args, "moduleName", &prop);
    napi_typeof(env, prop, &valueType);
    if (valueType == napi_string) {
        wantModuleName = GetStringFromNAPI(env, prop);
        if (wantModuleName.empty()) {
            APP_LOGE("error moduleName is empty!");
            return false;
        }
    }

    prop = nullptr;
    status = napi_get_named_property(env, args, "abilityName", &prop);
    wantAbilityName = GetStringFromNAPI(env, prop);

    prop = nullptr;
    status = napi_get_named_property(env, args, "deviceId", &prop);
    wantDeviceId = GetStringFromNAPI(env, prop);

    prop = nullptr;
    status = napi_get_named_property(env, args, "type", &prop);
    wantType = GetStringFromNAPI(env, prop);

    prop = nullptr;
    status = napi_get_named_property(env, args, "flags", &prop);
    napi_typeof(env, prop, &valueType);
    if (valueType == napi_number) {
        napi_get_value_int32(env, prop, &wantFlags);
    }

    prop = nullptr;
    status = napi_get_named_property(env, args, "action", &prop);
    wantAction = GetStringFromNAPI(env, prop);

    prop = nullptr;
    status = napi_get_named_property(env, args, "uri", &prop);
    wantUri = GetStringFromNAPI(env, prop);

    prop = nullptr;
    status = napi_get_named_property(env, args, "entities", &prop);
    ParseStringArray(env, wantEntities, prop);
    for (size_t idx = 0; idx < wantEntities.size(); idx++) {
        APP_LOGD("entity:%{public}s", wantEntities[idx].c_str());
        want.AddEntity(wantEntities[idx]);
    }

    napi_value elementProp = nullptr;
    status = napi_get_named_property(env, args, "elementName", &elementProp);
    napi_typeof(env, elementProp, &valueType);
    if (valueType == napi_object) {
        APP_LOGD("begin to parse want elementName");

        prop = nullptr;
        status = napi_get_named_property(env, elementProp, "deviceId", &prop);
        elementDeviceId = GetStringFromNAPI(env, prop);

        prop = nullptr;
        status = napi_get_named_property(env, elementProp, "uri", &prop);
        elementUri = GetStringFromNAPI(env, prop);

        prop = nullptr;
        status = napi_get_named_property(env, elementProp, "bundleName", &prop);
        if (status != napi_ok) {
            APP_LOGE("elementName bundleName incorrect!");
            return false;
        }
        napi_typeof(env, prop, &valueType);
        if (valueType != napi_string) {
            APP_LOGE("elementName bundleName type mismatch!");
            return false;
        }
        elementBundleName = GetStringFromNAPI(env, prop);

        prop = nullptr;
        status = napi_get_named_property(env, elementProp, "moduleName", &prop);
        elementModuleName = GetStringFromNAPI(env, prop);

        prop = nullptr;
        status = napi_get_named_property(env, elementProp, "abilityName", &prop);
        if (status != napi_ok) {
            APP_LOGE("elementName abilityName incorrect!");
            return false;
        }
        napi_typeof(env, prop, &valueType);
        if (valueType != napi_string) {
            APP_LOGE("elementName abilityName type mismatch!");
            return false;
        }
        elementAbilityName = GetStringFromNAPI(env, prop);
    }
    if (elementBundleName.empty()) {
        elementBundleName = wantBundleName;
    }
    if (elementModuleName.empty()) {
        elementModuleName = wantModuleName;
    }
    if (elementAbilityName.empty()) {
        elementAbilityName = wantAbilityName;
    }
    if (elementDeviceId.empty()) {
        elementDeviceId = wantDeviceId;
    }
    if (elementUri.empty()) {
        elementUri = wantUri;
    }
    APP_LOGD("bundleName:%{public}s, abilityName:%{public}s",
             elementBundleName.c_str(), elementAbilityName.c_str());
    APP_LOGD("action:%{public}s, uri:%{private}s, type:%{public}s, flags:%{public}d",
        wantAction.c_str(), elementUri.c_str(), wantType.c_str(), wantFlags);
    want.SetAction(wantAction);
    want.SetUri(elementUri);
    want.SetType(wantType);
    want.SetFlags(wantFlags);
    ElementName elementName(elementDeviceId, elementBundleName, elementAbilityName, elementModuleName);
    want.SetElement(elementName);
    return true;
}

/**
 * Promise and async callback
 */
napi_value QueryAbilityInfos(napi_env env, napi_callback_info info)
{
    APP_LOGI("QueryAbilityInfos called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGI("argc = [%{public}zu]", argc);
    Want want;
    AsyncAbilityInfoCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncAbilityInfoCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncAbilityInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
    asyncCallbackInfo->want = want;
    asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;

    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_object)) {
            bool ret = ParseWant(env, asyncCallbackInfo->want, argv[i]);
            if (!ret) {
                asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            }
        } else if ((i == ARGS_SIZE_ONE) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->flags, argv[i]);
        } else if (i == ARGS_SIZE_TWO) {
            if (valueType == napi_number) {
                ParseInt(env, asyncCallbackInfo->userId, argv[i]);
            } else if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
                break;
            } else {
                asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            }
        } else if ((i == ARGS_SIZE_THREE) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        }
    }
    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "QueryAbilityInfos", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void *data) {
            AsyncAbilityInfoCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfoCallbackInfo *)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerQueryAbilityInfos(env, asyncCallbackInfo->want, asyncCallbackInfo->flags,
                    asyncCallbackInfo->userId, asyncCallbackInfo->abilityInfos);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncAbilityInfoCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfoCallbackInfo *)data;
            std::unique_ptr<AsyncAbilityInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "type mismatch",
                    NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[1]));
                    ProcessAbilityInfos(env, result[1], asyncCallbackInfo->abilityInfos);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env,
                        napi_create_string_utf8(env, "QueryAbilityInfos failed", NAPI_AUTO_LENGTH, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
                }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static bool InnerGetApplicationInfo(napi_env env, const std::string &bundleName, int32_t flags,
    const int userId, ApplicationInfo &appInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->GetApplicationInfo(bundleName, flags, userId, appInfo);
}

static napi_value ParseString(napi_env env, std::string &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    param = GetStringFromNAPI(env, args);
    APP_LOGI("param=%{public}s.", param.c_str());
    // create result code
    napi_value result;
    status = napi_create_int32(env, NAPI_RETURN_ONE, &result);
    NAPI_ASSERT(env, status == napi_ok, "napi_create_int32 error!");
    return result;
}

static bool InnerGetAbilityInfo(const std::string &bundleName, const std::string &moduleName,
    const std::string &abilityName, AbilityInfo &abilityInfo, bool hasModuleName)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    if (hasModuleName) {
        return iBundleMgr->GetAbilityInfo(bundleName, moduleName, abilityName, abilityInfo);
    }
    return iBundleMgr->GetAbilityInfo(bundleName, abilityName, abilityInfo);
}

void StartGetAbilityInfoExecuteCB(napi_env env, void *data)
{
    APP_LOGI("%{public}s called.", __func__);
    AsyncAbilityInfosCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfosCallbackInfo *)data;
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s asyncCallbackInfo == nullptr", __func__);
        return;
    }
    if (!asyncCallbackInfo->err) {
        asyncCallbackInfo->ret = InnerGetAbilityInfo(
            asyncCallbackInfo->bundleName, asyncCallbackInfo->moduleName,
            asyncCallbackInfo->abilityName, asyncCallbackInfo->abilityInfo,
            asyncCallbackInfo->hasModuleName);
    }
    APP_LOGI("%{public}s end.", __func__);
}

void StartGetAbilityInfoCompletedCB(napi_env env, napi_status status, void *data)
{
    APP_LOGI("%{public}s called.", __func__);
    AsyncAbilityInfosCallbackInfo *asyncCallbackInfo = (AsyncAbilityInfosCallbackInfo *)data;
    std::unique_ptr<AsyncAbilityInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result[2] = {0};
    if (asyncCallbackInfo->err) {
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, asyncCallbackInfo->err, &result[0]));
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
            NAPI_AUTO_LENGTH, &result[1]));
    } else {
        if (asyncCallbackInfo->ret) {
            NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[1]));
            ConvertAbilityInfo(env, result[1], asyncCallbackInfo->abilityInfo);
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 1, &result[0]));
            NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
        }
    }
    if (asyncCallbackInfo->deferred) {
        if (asyncCallbackInfo->ret) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
        }
    } else {
        napi_value callback = nullptr;
        napi_value placeHolder = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
        NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback, sizeof(result) / sizeof(result[0]),
            result, &placeHolder));
    }
    APP_LOGI("%{public}s end.", __func__);
}

static bool HasModuleName(napi_env env, size_t argc, napi_value *argv)
{
    bool hasModuleName = false;
    if (argc > 0) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[argc - 1], &valueType));
        if (valueType == napi_function) {
            hasModuleName = (argc == ARGS_SIZE_FOUR) ? true : false;
        } else {
            hasModuleName = (argc == ARGS_SIZE_THREE) ? true : false;
        }
    } else {
        APP_LOGE("%{public}s error : argc < 0", __func__);
    }
    return hasModuleName;
}

napi_value GetAbilityInfo(napi_env env, napi_callback_info info)
{
    APP_LOGD("NAPI_GetAbilityInfo called");
    size_t requireArgc = ARGS_SIZE_TWO;
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 2 parameter");
    AsyncAbilityInfosCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncAbilityInfosCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }
    std::unique_ptr<AsyncAbilityInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
    asyncCallbackInfo->hasModuleName = HasModuleName(env, argc, argv);
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, argv[i]);
        } else if ((i == PARAM1) && (valueType == napi_string)) {
            if (asyncCallbackInfo->hasModuleName) {
                ParseString(env, asyncCallbackInfo->moduleName, argv[i]);
            } else {
                ParseString(env, asyncCallbackInfo->abilityName, argv[i]);
            }
        } else if ((i == PARAM2) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->abilityName, argv[i]);
        } else if (((i == PARAM2) || (i == PARAM3)) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->err = INVALID_PARAM;
            asyncCallbackInfo->message = "type misMatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, StartGetAbilityInfoExecuteCB,
                       StartGetAbilityInfoCompletedCB, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

/**
 * Promise and async callback
 */
napi_value GetApplicationInfo(napi_env env, napi_callback_info info)
{
    APP_LOGD("NAPI_GetApplicationInfo called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGD("argc = [%{public}zu]", argc);

    AsyncApplicationInfoCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncApplicationInfoCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is nullptr");
        return nullptr;
    }
    std::unique_ptr<AsyncApplicationInfoCallbackInfo> callbackPtr {asyncCallbackInfo};

    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, argv[i]);
        } else if ((i == ARGS_SIZE_ONE) && valueType == napi_number) {
            ParseInt(env, asyncCallbackInfo->flags, argv[i]);
            if (argc == ARGS_SIZE_TWO) {
                asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
            }
        } else if ((i == ARGS_SIZE_TWO) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->userId, argv[i]);
        } else if ((i == ARGS_SIZE_TWO) && (valueType == napi_function)) {
            asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else if ((i == ARGS_SIZE_THREE) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    if (argc == 0) {
        asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        asyncCallbackInfo->message = "type mismatch";
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetApplicationInfo", NAPI_AUTO_LENGTH, &resource));

    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncApplicationInfoCallbackInfo* asyncCallbackInfo = (AsyncApplicationInfoCallbackInfo*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetApplicationInfo(asyncCallbackInfo->env,
                                                                 asyncCallbackInfo->bundleName,
                                                                 asyncCallbackInfo->flags,
                                                                 asyncCallbackInfo->userId,
                                                                 asyncCallbackInfo->appInfo);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncApplicationInfoCallbackInfo* asyncCallbackInfo = (AsyncApplicationInfoCallbackInfo*)data;
            std::unique_ptr<AsyncApplicationInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
                    NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[1]));
                    ConvertApplicationInfo(env, result[1], asyncCallbackInfo->appInfo);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
              } else {
                  NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
              }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static bool InnerGetBundleInfos(
    napi_env env, int32_t flags, int32_t userId, std::vector<OHOS::AppExecFwk::BundleInfo> &bundleInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->GetBundleInfos(flags, bundleInfos, userId);
}

static void ProcessBundleInfos(
    napi_env env, napi_value result, const std::vector<OHOS::AppExecFwk::BundleInfo> &bundleInfos)
{
    if (bundleInfos.size() > 0) {
        APP_LOGI("-----bundleInfos is not null-----");
        size_t index = 0;
        for (const auto &item : bundleInfos) {
            APP_LOGI("name{%s} ", item.name.c_str());
            APP_LOGI("bundleName{%s} ", item.applicationInfo.bundleName.c_str());
            for (const auto &moduleInfo : item.applicationInfo.moduleInfos) {
                APP_LOGI("moduleName{%s} ", moduleInfo.moduleName.c_str());
                APP_LOGI("moduleSourceDir{%s} ", moduleInfo.moduleSourceDir.c_str());
            }
            napi_value objBundleInfo = nullptr;
            napi_create_object(env, &objBundleInfo);
            ConvertBundleInfo(env, objBundleInfo, item);
            napi_set_element(env, result, index, objBundleInfo);
            index++;
        }
    } else {
        APP_LOGI("-----bundleInfos is null-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetBundleInfos(napi_env env, napi_callback_info info)
{
    APP_LOGD("NAPI GetBundleInfos called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {0};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGD("argc = [%{public}zu]", argc);
    AsyncBundleInfosCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncBundleInfosCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is nullptr");
        return nullptr;
    }
    std::unique_ptr<AsyncBundleInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->flags, argv[i]);
        } else if ((i == PARAM1) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->userId, argv[i]);
        } else if ((i == PARAM1) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else if ((i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetBundleInfos", NAPI_AUTO_LENGTH, &resource));

    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncBundleInfosCallbackInfo* asyncCallbackInfo = (AsyncBundleInfosCallbackInfo*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetBundleInfos(
                    env, asyncCallbackInfo->flags, asyncCallbackInfo->userId, asyncCallbackInfo->bundleInfos);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncBundleInfosCallbackInfo* asyncCallbackInfo = (AsyncBundleInfosCallbackInfo*)data;
            std::unique_ptr<AsyncBundleInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
                    NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[1]));
                    ProcessBundleInfos(env, result[1], asyncCallbackInfo->bundleInfos);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
              } else {
                  NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
              }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static bool InnerGetBundleInfo(
    napi_env env, const std::string &bundleName, int32_t flags, BundleOptions bundleOptions, BundleInfo &bundleInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    bool ret = iBundleMgr->GetBundleInfo(bundleName, flags, bundleInfo, bundleOptions.userId);
    if (!ret) {
        APP_LOGI("-----bundleInfo is not find-----");
    }
    return ret;
}

static bool InnerGetBundlePackInfo(const std::string &bundleName, int32_t flags, BundlePackInfo &bundlePackInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    bool ret = iBundleMgr->GetBundlePackInfo(bundleName, flags, bundlePackInfo);
    if (!ret) {
        APP_LOGE("bundlePackInfo is not find");
    }
    return ret;
}

static void ConvertSummaryApp(napi_env env, napi_value &app, const OHOS::AppExecFwk::BundlePackInfo &bundleInPackfos)
{
    napi_value bundleName;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, bundleInPackfos.summary.app.bundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, app, "bundleName", bundleName));
    napi_value version;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &version));
    napi_value versionName;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, bundleInPackfos.summary.app.version.name.c_str(), NAPI_AUTO_LENGTH, &versionName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, version, "name", versionName));
    napi_value versionCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInPackfos.summary.app.version.code, &versionCode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, version, "code", versionCode));
    napi_value minCompatibleVersionCode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, bundleInPackfos.summary.app.version.minCompatibleVersionCode,
                                   &minCompatibleVersionCode));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, version, "minCompatibleVersionCode", minCompatibleVersionCode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, app, "version", version));
}

static void ConvertModulesApiVersion(
    napi_env env, napi_value &modulesObject, const OHOS::AppExecFwk::PackageModule &module)
{
    napi_value apiVersion;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &apiVersion));
    napi_value releaseType;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, module.apiVersion.releaseType.c_str(), NAPI_AUTO_LENGTH, &releaseType));
    napi_value compatible;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, module.apiVersion.compatible, &compatible));
    napi_value target;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, module.apiVersion.target, &target));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, apiVersion, "releaseType", releaseType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, apiVersion, "compatible", compatible));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, apiVersion, "target", target));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, modulesObject, "apiVersion", apiVersion));
}

static void ConvertDeviceType(napi_env env, napi_value &Object, std::vector<std::string> deviceTypes)
{
    napi_value deviceType;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &deviceType));
    size_t typeIndex = 0;
    for (const auto &type : deviceTypes) {
        napi_value typeValue;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, type.c_str(), NAPI_AUTO_LENGTH, &typeValue));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, deviceType, typeIndex, typeValue));
        typeIndex++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, Object, "deviceType", deviceType));
}

static void ConvertDistro(napi_env env, napi_value &modulesObject, const OHOS::AppExecFwk::PackageModule &module)
{
    napi_value distro;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &distro));
    napi_value deliveryWithInstall;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, module.distro.deliveryWithInstall, &deliveryWithInstall));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, distro, "deliveryWithInstall", deliveryWithInstall));
    napi_value installationFree;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, module.distro.installationFree, &installationFree));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, distro, "installationFree", installationFree));
    napi_value moduleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, module.distro.moduleName.c_str(), NAPI_AUTO_LENGTH, &moduleName));
    napi_value moduleType;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, module.distro.moduleType.c_str(), NAPI_AUTO_LENGTH, &moduleType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, distro, "moduleName", moduleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, distro, "moduleType", moduleType));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, modulesObject, "distro", distro));
}

static void ConvertFormsInfo(napi_env env, napi_value &abilityObject,
    const std::vector<OHOS::AppExecFwk::AbilityFormInfo> forms)
{
    napi_value formsArray;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &formsArray));
    size_t index = 0;
    for (const auto &form : forms) {
        napi_value formObjdect;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &formObjdect));
        napi_value name;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, form.name.c_str(), NAPI_AUTO_LENGTH, &name));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObjdect, "name", name));
        napi_value type;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, form.type.c_str(), NAPI_AUTO_LENGTH, &type));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObjdect, "type", type));
        napi_value updateEnabled;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, form.updateEnabled, &updateEnabled));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObjdect, "updateEnabled", updateEnabled));
        napi_value scheduledUpdateTime;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, form.scheduledUpdateTime.c_str(),
            NAPI_AUTO_LENGTH, &scheduledUpdateTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObjdect, "scheduledUpdateTime",
            scheduledUpdateTime));
        napi_value updateDuration;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, form.updateDuration, &updateDuration));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObjdect, "updateDuration", updateDuration));
        napi_value supportDimensions;
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &supportDimensions));
        napi_value defaultDimension;
        size_t indexValue = 0;
        for (const auto &dimension : form.supportDimensions) {
            napi_value value;
            NAPI_CALL_RETURN_VOID(
                env, napi_create_string_utf8(env, dimension.c_str(), NAPI_AUTO_LENGTH, &value));
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, supportDimensions, indexValue, value));
            indexValue++;
        }
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, form.defaultDimension.c_str(), NAPI_AUTO_LENGTH, &defaultDimension));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObjdect, "supportDimensions", supportDimensions));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObjdect, "defaultDimension", defaultDimension));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, formsArray, index, formObjdect));
        index++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, abilityObject, "forms", formsArray));
}

static void ConvertAbilities(napi_env env, napi_value &modulesObject, const OHOS::AppExecFwk::PackageModule &module)
{
    napi_value abilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &abilities));
    size_t index = 0;
    for (const auto &ability : module.abilities) {
        napi_value abilityObject;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &abilityObject));
        napi_value name;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, ability.name.c_str(), NAPI_AUTO_LENGTH, &name));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, abilityObject, "name", name));
        napi_value label;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, ability.label.c_str(), NAPI_AUTO_LENGTH, &label));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, abilityObject, "label", label));
        napi_value visible;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, ability.visible, &visible));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, abilityObject, "visible", visible));
        ConvertFormsInfo(env, abilityObject, ability.forms);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, abilities, index, abilityObject));
        index++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, modulesObject, "abilities", abilities));
}

static void ConvertSummaryModules(
    napi_env env, napi_value &modulesArray, const OHOS::AppExecFwk::BundlePackInfo &bundleInPackfos)
{
    size_t index = 0;
    for (const auto &module : bundleInPackfos.summary.modules) {
        napi_value modulesObject;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &modulesObject));
        napi_value mainAbility;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, module.mainAbility.c_str(), NAPI_AUTO_LENGTH, &mainAbility));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, modulesObject, "mainAbility", mainAbility));
        ConvertModulesApiVersion(env, modulesObject, module);
        ConvertDeviceType(env, modulesObject, module.deviceType);
        ConvertDistro(env, modulesObject, module);
        ConvertAbilities(env, modulesObject, module);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, modulesArray, index, modulesObject));
        index++;
    }
}

static void ConvertPackageSummary(
    napi_env env, napi_value &jsSummary, const OHOS::AppExecFwk::BundlePackInfo &bundleInPackfos)
{
    napi_value app;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &app));
    ConvertSummaryApp(env, app, bundleInPackfos);
    napi_value modules;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &modules));
    ConvertSummaryModules(env, modules, bundleInPackfos);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, jsSummary, "app", app));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, jsSummary, "modules", modules));
}

static void ConvertPackages(
    napi_env env, napi_value &jsPackagesArray, const OHOS::AppExecFwk::BundlePackInfo &bundleInPackfos)
{
    size_t index = 0;
    for (const auto &package : bundleInPackfos.packages) {
        napi_value jsPackagesObject;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &jsPackagesObject));
        ConvertDeviceType(env, jsPackagesObject, package.deviceType);
        napi_value packageName;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, package.name.c_str(), NAPI_AUTO_LENGTH, &packageName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, jsPackagesObject, "name", packageName));
        napi_value moduleType;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, package.moduleType.c_str(), NAPI_AUTO_LENGTH, &moduleType));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, jsPackagesObject, "moduleType", moduleType));
        napi_value deliveryWithInstall;
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, package.deliveryWithInstall, &deliveryWithInstall));
        NAPI_CALL_RETURN_VOID(
            env, napi_set_named_property(env, jsPackagesObject, "deliveryWithInstall", deliveryWithInstall));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, jsPackagesArray, index, jsPackagesObject));
        index++;
    }
}

static void ConvertBundlePackInfo(
    napi_env env, napi_value &result, int32_t flags, const OHOS::AppExecFwk::BundlePackInfo &bundleInPackfos)
{
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result));
    if (static_cast<uint32_t>(flags) & BundlePackFlag::GET_PACKAGES) {
        napi_value jsPackagesArray;
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &jsPackagesArray));
        ConvertPackages(env, jsPackagesArray, bundleInPackfos);
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "packages", jsPackagesArray));
        return;
    }
    if (static_cast<uint32_t>(flags) & BundlePackFlag::GET_BUNDLE_SUMMARY) {
        napi_value jsSummary;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &jsSummary));
        napi_value app;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &app));
        ConvertSummaryApp(env, app, bundleInPackfos);
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, jsSummary, "app", app));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "summary", jsSummary));
        return;
    }
    if (static_cast<uint32_t>(flags) & BundlePackFlag::GET_MODULE_SUMMARY) {
        napi_value jsSummary;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &jsSummary));
        napi_value modules;
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &modules));
        ConvertSummaryModules(env, modules, bundleInPackfos);
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, jsSummary, "modules", modules));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "summary", jsSummary));
        return;
    }
    napi_value jsSummary;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &jsSummary));
    ConvertPackageSummary(env, jsSummary, bundleInPackfos);
    napi_value jsPackagesArray;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &jsPackagesArray));
    ConvertPackages(env, jsPackagesArray, bundleInPackfos);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "packages", jsPackagesArray));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "summary", jsSummary));
}

/**
 * Promise and async callback
 */
napi_value GetBundlePackInfo(napi_env env, napi_callback_info info)
{
    APP_LOGD("NAPI GetBundlePackInfo called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGD("argc = [%{public}zu]", argc);
    AsyncBundlePackInfoCallbackInfo *asyncCallbackInfo = new AsyncBundlePackInfoCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is nullptr");
        return nullptr;
    }
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, argv[i]);
        } else if ((i == PARAM1) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->flags, argv[i]);
        } else if ((i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            asyncCallbackInfo->message = "type mismatch";
        }
    }
    if (std::find(PACKINFO_FLAGS.begin(), PACKINFO_FLAGS.end(), asyncCallbackInfo->flags) == PACKINFO_FLAGS.end()) {
        asyncCallbackInfo->err = INVALID_PARAM;
        asyncCallbackInfo->message = "flag mismatch";
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }
    return GetBundlePackInfoWrap(env, promise, asyncCallbackInfo);
}

napi_value GetBundlePackInfoWrap(napi_env env, napi_value promise, AsyncBundlePackInfoCallbackInfo *asyncCallbackInfo)
{
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetBundlePackInfoPromise", NAPI_AUTO_LENGTH, &resource));
    std::unique_ptr<AsyncBundlePackInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncBundlePackInfoCallbackInfo* asyncCallbackInfo = (AsyncBundlePackInfoCallbackInfo*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetBundlePackInfo(asyncCallbackInfo->bundleName,
                    asyncCallbackInfo->flags, asyncCallbackInfo->bundlePackInfo);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncBundlePackInfoCallbackInfo* asyncCallbackInfo = (AsyncBundlePackInfoCallbackInfo*)data;
            std::unique_ptr<AsyncBundlePackInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
                    NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[1]));
                    ConvertBundlePackInfo(env, result[1], asyncCallbackInfo->flags, asyncCallbackInfo->bundlePackInfo);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
              } else {
                  NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
              }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

/**
 * Promise and async callback
 */
napi_value GetBundleInfo(napi_env env, napi_callback_info info)
{
    APP_LOGD("NAPI GetBundleInfo called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGD("argc = [%{public}zu]", argc);
    AsyncBundleInfoCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncBundleInfoCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is nullptr");
        return nullptr;
    }
    std::unique_ptr<AsyncBundleInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->param, argv[i]);
        } else if ((i == PARAM1) && valueType == napi_number) {
            ParseInt(env, asyncCallbackInfo->flags, argv[i]);
        } else if ((i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else if ((i == PARAM2) && (valueType == napi_object)) {
            bool ret = ParseBundleOptions(env, asyncCallbackInfo->bundleOptions, argv[i]);
            if (!ret) {
                asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            }
        } else if ((i == PARAM3) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetBundleInfo", NAPI_AUTO_LENGTH, &resource));

    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncBundleInfoCallbackInfo* asyncCallbackInfo = (AsyncBundleInfoCallbackInfo*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetBundleInfo(asyncCallbackInfo->env, asyncCallbackInfo->param,
                    asyncCallbackInfo->flags, asyncCallbackInfo->bundleOptions, asyncCallbackInfo->bundleInfo);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncBundleInfoCallbackInfo* asyncCallbackInfo = (AsyncBundleInfoCallbackInfo*)data;
            std::unique_ptr<AsyncBundleInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
                    NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[1]));
                    ConvertBundleInfo(env, result[1], asyncCallbackInfo->bundleInfo);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
              } else {
                  NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
              }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static bool InnerGetArchiveInfo(
    napi_env env, const std::string &hapFilePath, const int32_t flags, BundleInfo &bundleInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    };
    bool ret = iBundleMgr->GetBundleArchiveInfo(hapFilePath, flags, bundleInfo);
    if (!ret) {
        APP_LOGD("ArchiveInfo not found");
    }
    return ret;
}
/**
 * Promise and async callback
 */
napi_value GetBundleArchiveInfo(napi_env env, napi_callback_info info)
{
    APP_LOGD("NAPI GetBundleArchiveInfo called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGD("argc = [%{public}zu]", argc);
    AsyncBundleInfoCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncBundleInfoCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is nullptr");
        return nullptr;
    }
    std::unique_ptr<AsyncBundleInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == PARAM0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->param, argv[i]);
        } else if ((i == PARAM1) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->flags, argv[i]);
        } else if ((i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetBundleArchiveInfo", NAPI_AUTO_LENGTH, &resource));

    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncBundleInfoCallbackInfo* asyncCallbackInfo = (AsyncBundleInfoCallbackInfo*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetArchiveInfo(env, asyncCallbackInfo->param,
                    asyncCallbackInfo->flags, asyncCallbackInfo->bundleInfo);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncBundleInfoCallbackInfo* asyncCallbackInfo = (AsyncBundleInfoCallbackInfo*)data;
            std::unique_ptr<AsyncBundleInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
                    NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[1]));
                    ConvertBundleInfo(env, result[1], asyncCallbackInfo->bundleInfo);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
              if (asyncCallbackInfo->ret) {
                  NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
              } else {
                  NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
              }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static bool InnerGetLaunchWantForBundle(
    napi_env env, const std::string &bundleName, Want &want)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    bool ret = iBundleMgr->GetLaunchWantForBundle(bundleName, want);
    if (!ret) {
        APP_LOGE("-----launchWantForBundle is not find-----");
    }
    return ret;
}
napi_value GetLaunchWantForBundle(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[PARAM2] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGI("ARGCSIZE is =%{public}zu.", argc);
    std::string bundleName;
    AsyncLaunchWantForBundleCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncLaunchWantForBundleCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncLaunchWantForBundleCallbackInfo> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, argv[i]);
        } else if ((i == 1) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        }
    }
    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env,  &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "JSGetLaunchWantForBundle", NAPI_AUTO_LENGTH, &resource));

    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void *data) {
            AsyncLaunchWantForBundleCallbackInfo *asyncCallbackInfo = (AsyncLaunchWantForBundleCallbackInfo *)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret =
                    InnerGetLaunchWantForBundle(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->want);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncLaunchWantForBundleCallbackInfo* asyncCallbackInfo = (AsyncLaunchWantForBundleCallbackInfo*)data;
            std::unique_ptr<AsyncLaunchWantForBundleCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "type mismatch", NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[1]));
                    ConvertWantInfo(env, result[1], asyncCallbackInfo->want);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "getLaunchWantForBundle failed",
                        NAPI_AUTO_LENGTH, &result[1]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
                }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static void ConvertPermissionDef(napi_env env, napi_value result, const PermissionDef &permissionDef)
{
    napi_value nPermissionName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, permissionDef.permissionName.c_str(), NAPI_AUTO_LENGTH, &nPermissionName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "permissionName", nPermissionName));
    APP_LOGI("InnerGetPermissionDef name=%{public}s.", permissionDef.permissionName.c_str());

    napi_value nGrantMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, permissionDef.grantMode, &nGrantMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "grantMode", nGrantMode));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, permissionDef.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "labelId", nLabelId));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, permissionDef.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "descriptionId", nDescriptionId));
}

static bool InnerGetPermissionDef(napi_env env, const std::string &permissionName, PermissionDef &permissionDef)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    };
    bool ret = iBundleMgr->GetPermissionDef(permissionName, permissionDef);
    if (!ret) {
        APP_LOGE("permissionName is not find");
    }
    return ret;
}

/**
 * Promise and async callback
 */
napi_value GetPermissionDef(napi_env env, napi_callback_info info)
{
    APP_LOGD("GetPermissionDef called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    APP_LOGD("argc = [%{public}zu]", argc);
    AsyncPermissionDefCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncPermissionDefCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncPermissionDefCallbackInfo> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valuetype));
        if ((i == PARAM0) && (valuetype == napi_string)) {
            ParseString(env, asyncCallbackInfo->permissionName, argv[i]);
        } else if ((i == PARAM1) && (valuetype == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->err = PARAM_TYPE_ERROR;
            asyncCallbackInfo->message = "type mismatch";
        }
    }

    napi_value promise = nullptr;
    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }

    napi_value resourceName;
    NAPI_CALL(env, napi_create_string_latin1(env, "GetPermissionDef", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            AsyncPermissionDefCallbackInfo *asyncCallbackInfo = (AsyncPermissionDefCallbackInfo *)data;
            APP_LOGD("asyncCallbackInfo->permissionName=%{public}s.", asyncCallbackInfo->permissionName.c_str());
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetPermissionDef(env, asyncCallbackInfo->permissionName,
                                                               asyncCallbackInfo->permissionDef);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncPermissionDefCallbackInfo *asyncCallbackInfo = (AsyncPermissionDefCallbackInfo *)data;
            std::unique_ptr<AsyncPermissionDefCallbackInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[ARGS_SIZE_TWO] = {0};
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(asyncCallbackInfo->err),
                    &result[PARAM0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->message.c_str(),
                    NAPI_AUTO_LENGTH, &result[PARAM1]));
            } else {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, CODE_SUCCESS, &result[PARAM0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[PARAM1]));
                    ConvertPermissionDef(env, result[PARAM1], asyncCallbackInfo->permissionDef);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, OPERATION_FAILED, &result[PARAM0]));
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[PARAM1]));
                }
            }
            if (asyncCallbackInfo->deferred) {
                if (asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[PARAM1]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[PARAM0]));
                }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[PARAM0]), result, &placeHolder));
            }
        },
        (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static void InnerInstall(napi_env env, const std::vector<std::string> &bundleFilePath, InstallParam &installParam,
    InstallResult &installResult)
{
    if (bundleFilePath.empty()) {
        installResult.resultCode = static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FILE_PATH_INVALID);
        return;
    }
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return;
    }
    auto iBundleInstaller = iBundleMgr->GetBundleInstaller();
    if ((iBundleInstaller == nullptr) || (iBundleInstaller->AsObject() == nullptr)) {
        APP_LOGE("can not get iBundleInstaller");
        return;
    }

    if (installParam.installFlag == InstallFlag::NORMAL) {
        installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    }

    OHOS::sptr<InstallerCallback> callback = new (std::nothrow) InstallerCallback();
    if (!callback) {
        APP_LOGE("callback nullptr");
        return;
    }

    sptr<BundleDeathRecipient> recipient(new (std::nothrow) BundleDeathRecipient(callback));
    iBundleInstaller->AsObject()->AddDeathRecipient(recipient);
    ErrCode res = iBundleInstaller->StreamInstall(bundleFilePath, installParam, callback);

    if (res == ERR_APPEXECFWK_INSTALL_PARAM_ERROR) {
        APP_LOGE("install param error");
        installResult.resultCode = IStatusReceiver::ERR_INSTALL_PARAM_ERROR;
        installResult.resultMsg = "STATUS_INSTALL_FAILURE_INVALID";
    } else if (res == ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR) {
        APP_LOGE("install internal error");
        installResult.resultCode = IStatusReceiver::ERR_INSTALL_INTERNAL_ERROR;
        installResult.resultMsg = "STATUS_INSTALL_FAILURE";
    } else if (res == ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID) {
        APP_LOGE("install invalid path");
        installResult.resultCode = IStatusReceiver::ERR_INSTALL_FILE_PATH_INVALID;
        installResult.resultMsg = "STATUS_INSTALL_FAILURE_INVALID";
    } else {
        installResult.resultCode = callback->GetResultCode();
        APP_LOGD("InnerInstall resultCode %{public}d", installResult.resultCode);
        installResult.resultMsg = callback->GetResultMsg();
        APP_LOGD("InnerInstall resultMsg %{public}s", installResult.resultMsg.c_str());
    }
}

static void InnerRecover(napi_env env, const std::string &bundleName, InstallParam &installParam,
    InstallResult &installResult)
{
    if (bundleName.empty()) {
        installResult.resultCode = static_cast<int32_t>(IStatusReceiver::ERR_RECOVER_INVALID_BUNDLE_NAME);
        return;
    }
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return;
    }
    auto iBundleInstaller = iBundleMgr->GetBundleInstaller();
    if (!iBundleInstaller) {
        APP_LOGE("can not get iBundleInstaller");
        return;
    }

    OHOS::sptr<InstallerCallback> callback = new (std::nothrow) InstallerCallback();
    if (!callback) {
        APP_LOGE("callback nullptr");
        return;
    }
    iBundleInstaller->Recover(bundleName, installParam, callback);
    installResult.resultMsg = callback->GetResultMsg();
    APP_LOGD("InnerRecover resultMsg %{public}s.", installResult.resultMsg.c_str());
    installResult.resultCode = callback->GetResultCode();
    APP_LOGD("InnerRecover resultCode %{public}d.", installResult.resultCode);
}
/**
 * Promise and async callback
 */
napi_value GetBundleInstaller(napi_env env, napi_callback_info info)
{
    APP_LOGI("GetBundleInstaller called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    APP_LOGI("argc = [%{public}zu]", argc);

    AsyncGetBundleInstallerCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncGetBundleInstallerCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncGetBundleInstallerCallbackInfo> callbackPtr {asyncCallbackInfo};
    if (argc > (ARGS_SIZE_ONE - CALLBACK_SIZE)) {
        APP_LOGI("GetBundleInstaller asyncCallback.");
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[PARAM0], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetBundleInstaller", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {},
            [](napi_env env, napi_status status, void *data) {
                AsyncGetBundleInstallerCallbackInfo *asyncCallbackInfo = (AsyncGetBundleInstallerCallbackInfo *)data;
                std::unique_ptr<AsyncGetBundleInstallerCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                napi_value m_classBundleInstaller = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, g_classBundleInstaller,
                    &m_classBundleInstaller));
                NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
                NAPI_CALL_RETURN_VOID(env, napi_new_instance(env, m_classBundleInstaller, 0, nullptr, &result[PARAM1]));
                result[PARAM0] = GetCallbackErrorValue(env, CODE_SUCCESS);
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO,
                    &result[PARAM0], &callResult));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        napi_value result;
        napi_create_int32(env, NAPI_RETURN_ONE, &result);
        return result;
    } else {
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetBundleInstaller", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {},
            [](napi_env env, napi_status status, void *data) {
                APP_LOGI("=================load=================");
                AsyncGetBundleInstallerCallbackInfo *asyncCallbackInfo = (AsyncGetBundleInstallerCallbackInfo *)data;
                std::unique_ptr<AsyncGetBundleInstallerCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result;
                napi_value m_classBundleInstaller = nullptr;
                napi_get_reference_value(env, g_classBundleInstaller, &m_classBundleInstaller);
                napi_new_instance(env, m_classBundleInstaller, 0, nullptr, &result);
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        return promise;
    }
}

static bool ParseHashParam(napi_env env, std::string &key, std::string &value, napi_value args)
{
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("args type incorrect!");
        return false;
    }

    napi_value property = nullptr;
    bool hasKey = false;
    napi_has_named_property(env, args, "moduleName", &hasKey);
    if (!hasKey) {
        APP_LOGE("parse HashParam failed due to moduleName is not exist!");
        return false;
    }
    status = napi_get_named_property(env, args, "moduleName", &property);
    if (status != napi_ok) {
        APP_LOGE("napi get named moduleName property error!");
        return false;
    }
    ParseString(env, key, property);
    if (key.empty()) {
        APP_LOGE("param string moduleName is empty.");
        return false;
    }
    APP_LOGD("ParseHashParam moduleName=%{public}s.", key.c_str());

    property = nullptr;
    hasKey = false;
    napi_has_named_property(env, args, "hashValue", &hasKey);
    if (!hasKey) {
        APP_LOGE("parse HashParam failed due to hashValue is not exist!");
        return false;
    }
    status = napi_get_named_property(env, args, "hashValue", &property);
    if (status != napi_ok) {
        APP_LOGE("napi get named hashValue property error!");
        return false;
    }
    ParseString(env, value, property);
    if (value.empty()) {
        APP_LOGE("param string hashValue is empty.");
        return false;
    }
    APP_LOGD("ParseHashParam hashValue=%{public}s.", value.c_str());
    return true;
}

static bool ParseHashParams(napi_env env, std::map<std::string, std::string> &hashParams, napi_value args)
{
    bool isArray = false;
    uint32_t arrayLength = 0;
    napi_value valueAry = 0;
    napi_valuetype valueAryType = napi_undefined;
    NAPI_CALL(env, napi_is_array(env, args, &isArray));
    if (!isArray) {
        APP_LOGE("hashParams is not array!");
        return false;
    }

    NAPI_CALL(env, napi_get_array_length(env, args, &arrayLength));
    APP_LOGD("ParseHashParams property is array, length=%{public}ud", arrayLength);
    for (uint32_t j = 0; j < arrayLength; j++) {
        NAPI_CALL(env, napi_get_element(env, args, j, &valueAry));
        NAPI_CALL(env, napi_typeof(env, valueAry, &valueAryType));
        std::string key;
        std::string value;
        if (!ParseHashParam(env, key, value, valueAry)) {
            APP_LOGD("parse hash param failed");
            return false;
        }

        if (hashParams.find(key) != hashParams.end()) {
            APP_LOGD("moduleName(%{public}s) is duplicate", key.c_str());
            return false;
        }

        hashParams.emplace(key, value);
    }

    return true;
}

static bool ParseInstallParam(napi_env env, InstallParam &installParam, napi_value args)
{
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("args type incorrect!");
        return false;
    }

    napi_value property = nullptr;
    bool hasKey = false;
    napi_has_named_property(env, args, "userId", &hasKey);
    if (hasKey) {
        status = napi_get_named_property(env, args, "userId", &property);
        if (status != napi_ok) {
            APP_LOGE("napi get named userId property error!");
            return false;
        }

        napi_typeof(env, property, &valueType);
        if (valueType != napi_number) {
            APP_LOGE("param(userId) type incorrect!");
            return false;
        }

        int userId = Constants::UNSPECIFIED_USERID;
        NAPI_CALL(env, napi_get_value_int32(env, property, &userId));
        if (userId < Constants::DEFAULT_USERID) {
            APP_LOGE("param userId(%{public}d) is invalid.", userId);
            return false;
        }
        installParam.userId = userId;
    }
    APP_LOGI("ParseInstallParam userId=%{public}d.", installParam.userId);

    property = nullptr;
    hasKey = false;
    napi_has_named_property(env, args, "installFlag", &hasKey);
    if (hasKey) {
        status = napi_get_named_property(env, args, "installFlag", &property);
        if (status != napi_ok) {
            APP_LOGE("napi get named installFlag property error!");
            return false;
        }

        napi_typeof(env, property, &valueType);
        if (valueType != napi_number) {
            APP_LOGE("param(installFlag) type incorrect!");
            return false;
        }

        int installFlag = 0;
        NAPI_CALL(env, napi_get_value_int32(env, property, &installFlag));
        installParam.installFlag = static_cast<OHOS::AppExecFwk::InstallFlag>(installFlag);
    }
    APP_LOGI("ParseInstallParam installFlag=%{public}d.", installParam.installFlag);

    property = nullptr;
    hasKey = false;
    napi_has_named_property(env, args, "isKeepData", &hasKey);
    if (hasKey) {
        status = napi_get_named_property(env, args, "isKeepData", &property);
        if (status != napi_ok) {
            APP_LOGE("napi get named isKeepData property error!");
            return false;
        }

        napi_typeof(env, property, &valueType);
        if (valueType != napi_boolean) {
            APP_LOGE("param(isKeepData) type incorrect!");
            return false;
        }

        bool isKeepData = false;
        NAPI_CALL(env, napi_get_value_bool(env, property, &isKeepData));
        installParam.isKeepData = isKeepData;
    }
    APP_LOGI("ParseInstallParam isKeepData=%{public}d.", installParam.isKeepData);

    property = nullptr;
    hasKey = false;
    napi_has_named_property(env, args, "hashParams", &hasKey);
    if (hasKey) {
        status = napi_get_named_property(env, args, "hashParams", &property);
        if (status != napi_ok) {
            APP_LOGE("napi get named hashParams property error!");
            return false;
        }

        if (!ParseHashParams(env, installParam.hashParams, property)) {
            APP_LOGE("parse hash params failed!");
            return false;
        }
    }
    APP_LOGI("ParseInstallParam hashParams size = %{public}zu.", installParam.hashParams.size());
    return true;
}

static void ConvertInstallResult(InstallResult &installResult)
{
    APP_LOGI("ConvertInstallResult = %{public}s.", installResult.resultMsg.c_str());
    switch (installResult.resultCode) {
        case static_cast<int32_t>(IStatusReceiver::SUCCESS):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::SUCCESS);
            installResult.resultMsg = "SUCCESS";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_INTERNAL_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_HOST_INSTALLER_FAILED):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE);
            installResult.resultMsg = "STATUS_INSTALL_FAILURE";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_VERIFICATION_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_INCOMPATIBLE_SIGNATURE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARAM_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FILE_PATH_INVALID):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_INVALID_HAP_SIZE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_INVALID_HAP_NAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_INVALID_BUNDLE_FILE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_INVALID_NUMBER_OF_ENTRY_HAP):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_UNEXPECTED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_MISSING_BUNDLE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_NO_PROFILE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_BAD_PROFILE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_PROFILE_PROP_TYPE_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_PROFILE_MISSING_PROP):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_PERMISSION_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_RPCID_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_INVALID_SIGNATURE_FILE_PATH):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_BAD_BUNDLE_SIGNATURE_FILE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_NO_BUNDLE_SIGNATURE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_VERIFY_APP_PKCS7_FAIL):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_PROFILE_PARSE_FAIL):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_APP_SOURCE_NOT_TRUESTED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_BAD_DIGEST):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_BUNDLE_INTEGRITY_VERIFICATION_FAILURE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_FILE_SIZE_TOO_LARGE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_BAD_PUBLICKEY):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_BAD_BUNDLE_SIGNATURE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_NO_PROFILE_BLOCK_FAIL):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_BUNDLE_SIGNATURE_VERIFICATION_FAILURE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_MODULE_NAME_EMPTY):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_MODULE_NAME_DUPLICATE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_CHECK_HAP_HASH_PARAM):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_VERIFY_SOURCE_INIT_FAIL):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_INVALID);
            installResult.resultMsg = "STATUS_INSTALL_FAILURE_INVALID";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PARSE_MISSING_ABILITY):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_ABILITY_NOT_FOUND);
            installResult.resultMsg = "STATUS_ABILITY_NOT_FOUND";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_VERSION_DOWNGRADE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_FAILED_INCONSISTENT_SIGNATURE):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_INCOMPATIBLE);
            installResult.resultMsg = "STATUS_INSTALL_FAILURE_INCOMPATIBLE";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_PERMISSION_DENIED):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_PERMISSION_DENIED);
            installResult.resultMsg = "STATUS_INSTALL_PERMISSION_DENIED";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_ENTRY_ALREADY_EXIST):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_ALREADY_EXIST):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_BUNDLENAME_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_VERSIONCODE_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_VERSIONNAME_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_MINCOMPATIBLE_VERSIONCODE_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_VENDOR_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_RELEASETYPE_TARGET_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_RELEASETYPE_COMPATIBLE_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_SINGLETON_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_ZERO_USER_WITH_NO_SINGLETON):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_CHECK_SYSCAP_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_APPTYPE_NOT_SAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_URI_DUPLICATE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_VERSION_NOT_COMPATIBLE):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_TYPE_ERROR):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_CONFLICT);
            installResult.resultMsg = "STATUS_INSTALL_FAILURE_CONFLICT";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_PARAM_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_GET_PROXY_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_CREATE_DIR_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_CREATE_DIR_EXIST):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_CHOWN_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_REMOVE_DIR_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_EXTRACT_FILES_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_RNAME_DIR_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALLD_CLEAN_DIR_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_STATE_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_GENERATE_UID_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_INSTALLD_SERVICE_ERROR):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_STORAGE);
            installResult.resultMsg = "STATUS_INSTALL_FAILURE_STORAGE";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_PERMISSION_DENIED):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_PERMISSION_DENIED);
            installResult.resultMsg = "STATUS_UNINSTALL_PERMISSION_DENIED";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_INVALID_NAME):
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_PARAM_ERROR):
            if (CheckIsSystemApp()) {
                installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_FAILURE_ABORTED);
                installResult.resultMsg = "STATUS_UNINSTALL_FAILURE_ABORTED";
                break;
            }
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_SYSTEM_APP_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_KILLING_APP_ERROR):
            if (CheckIsSystemApp()) {
                installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_FAILURE_CONFLICT);
                installResult.resultMsg = "STATUS_UNINSTALL_FAILURE_CONFLICT";
                break;
            }
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_MISSING_INSTALLED_BUNDLE):
        case static_cast<int32_t>(IStatusReceiver::ERR_UNINSTALL_MISSING_INSTALLED_MODULE):
        case static_cast<int32_t>(IStatusReceiver::ERR_USER_NOT_INSTALL_HAP):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_FAILURE);
            installResult.resultMsg = "STATUS_UNINSTALL_FAILURE";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_RECOVER_GET_BUNDLEPATH_ERROR):
        case static_cast<int32_t>(IStatusReceiver::ERR_RECOVER_INVALID_BUNDLE_NAME):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_RECOVER_FAILURE_INVALID);
            installResult.resultMsg = "STATUS_RECOVER_FAILURE_INVALID";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_FAILED_SERVICE_DIED):
        case static_cast<int32_t>(IStatusReceiver::ERR_FAILED_GET_INSTALLER_PROXY):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_BMS_SERVICE_ERROR);
            installResult.resultMsg = "STATUS_BMS_SERVICE_ERROR";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_DISK_MEM_INSUFFICIENT):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_FAILED_NO_SPACE_LEFT);
            installResult.resultMsg = "STATUS_FAILED_NO_SPACE_LEFT";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED):
        case static_cast<int32_t>(IStatusReceiver::ERR_INSTALL_UPDATE_HAP_TOKEN_FAILED):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_GRANT_REQUEST_PERMISSIONS_FAILED);
            installResult.resultMsg = "STATUS_GRANT_REQUEST_PERMISSIONS_FAILED";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_USER_NOT_EXIST):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_USER_NOT_EXIST);
            installResult.resultMsg = "STATUS_USER_NOT_EXIST";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_USER_CREATE_FALIED):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_USER_CREATE_FALIED);
            installResult.resultMsg = "STATUS_USER_CREATE_FALIED";
            break;
        case static_cast<int32_t>(IStatusReceiver::ERR_USER_REMOVE_FALIED):
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_USER_REMOVE_FALIED);
            installResult.resultMsg = "STATUS_USER_REMOVE_FALIED";
            break;
        default:
            installResult.resultCode = static_cast<int32_t>(InstallErrorCode::STATUS_BMS_SERVICE_ERROR);
            installResult.resultMsg = "STATUS_BMS_SERVICE_ERROR";
            break;
    }
}

/**
 * Promise and async callback
 */
napi_value Install(napi_env env, napi_callback_info info)
{
    APP_LOGI("Install called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    APP_LOGI("argc = [%{public}zu]", argc);
    AsyncInstallCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncInstallCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};

    std::vector<std::string> bundleFilePaths;
    InstallParam installParam;
    napi_value retFirst = nullptr;
    bool retSecond = true;
    retFirst = ParseStringArray(env, bundleFilePaths, argv[PARAM0]);
    retSecond = ParseInstallParam(env, installParam, argv[PARAM1]);
    if (retFirst == nullptr || !retSecond) {
        APP_LOGE("Install installParam error.");
        asyncCallbackInfo->errCode = PARAM_TYPE_ERROR;
    }
    asyncCallbackInfo->hapFiles = bundleFilePaths;
    asyncCallbackInfo->installParam = installParam;
    if (argc > (ARGS_SIZE_THREE - CALLBACK_SIZE)) {
        APP_LOGI("Install asyncCallback.");
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[ARGS_SIZE_TWO], &valuetype));
        if (valuetype != napi_function) {
            APP_LOGE("Wrong argument type. Function expected.");
            delete asyncCallbackInfo;
            return nullptr;
        }
        NAPI_CALL(env, napi_create_reference(env, argv[ARGS_SIZE_TWO], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "Install", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                if (!asyncCallbackInfo->errCode) {
                    InnerInstall(env,
                        asyncCallbackInfo->hapFiles,
                        asyncCallbackInfo->installParam,
                        asyncCallbackInfo->installResult);
                }
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                if (!asyncCallbackInfo->errCode) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[PARAM1]));
                    ConvertInstallResult(asyncCallbackInfo->installResult);
                    napi_value nResultMsg;
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(
                        env, asyncCallbackInfo->installResult.resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg));
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM1], "statusMessage",
                        nResultMsg));
                    napi_value nResultCode;
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->installResult.resultCode,
                        &nResultCode));
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM1], "status", nResultCode));
                    result[PARAM0] = GetCallbackErrorValue(
                        env, (asyncCallbackInfo->installResult.resultCode == 0) ? CODE_SUCCESS : CODE_FAILED);
                } else {
                    napi_value nResultMsg;
                    std::string msg = "error param type.";
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, msg.c_str(), NAPI_AUTO_LENGTH,
                        &nResultMsg));
                    result[PARAM0] = GetCallbackErrorValue(env, CODE_FAILED);
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM0], "Message", nResultMsg));
                }
                NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO,
                    &result[PARAM0], &callResult));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        napi_value result;
        napi_create_int32(env, NAPI_RETURN_ONE, &result);
        return result;
    } else {
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "Install", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                InnerInstall(env,
                    asyncCallbackInfo->hapFiles,
                    asyncCallbackInfo->installParam,
                    asyncCallbackInfo->installResult);
            },
            [](napi_env env, napi_status status, void *data) {
                APP_LOGI("=================load=================");
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
                ConvertInstallResult(asyncCallbackInfo->installResult);
                napi_value result;
                NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result));
                napi_value nResultMsg;
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(
                    env, asyncCallbackInfo->installResult.resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg));
                NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "statusMessage", nResultMsg));
                napi_value nResultCode;
                NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->installResult.resultCode,
                    &nResultCode));
                NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "status", nResultCode));
                NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred,
                    result));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        return promise;
    }
}

napi_value Recover(napi_env env, napi_callback_info info)
{
    APP_LOGD("Recover by bundleName called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    APP_LOGD("argc = [%{public}zu]", argc);
    AsyncInstallCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncInstallCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
    std::string bundleName;
    ParseString(env, bundleName, argv[PARAM0]);
    InstallParam installParam;
    if (!ParseInstallParam(env, installParam, argv[PARAM1])) {
        APP_LOGE("Recover installParam error.");
        asyncCallbackInfo->errCode = PARAM_TYPE_ERROR;
    }

    asyncCallbackInfo->installParam = installParam;
    asyncCallbackInfo->bundleName = bundleName;
    if (argc > (ARGS_SIZE_THREE - CALLBACK_SIZE)) {
        APP_LOGD("Recover by bundleName asyncCallback.");
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[ARGS_SIZE_TWO], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[ARGS_SIZE_TWO], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "Recover", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                if (!asyncCallbackInfo->errCode) {
                    InnerRecover(env,
                        asyncCallbackInfo->bundleName,
                        asyncCallbackInfo->installParam,
                        asyncCallbackInfo->installResult);
                }
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                if (!asyncCallbackInfo->errCode) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[PARAM1]));
                    ConvertInstallResult(asyncCallbackInfo->installResult);
                    napi_value nResultMsg;
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(
                        env, asyncCallbackInfo->installResult.resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg));
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM1], "statusMessage",
                        nResultMsg));
                    napi_value nResultCode;
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->installResult.resultCode,
                        &nResultCode));
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM1], "status", nResultCode));
                    result[PARAM0] = GetCallbackErrorValue(
                        env, (asyncCallbackInfo->installResult.resultCode == 0) ? CODE_SUCCESS : CODE_FAILED);
                } else {
                    napi_value nResultMsg;
                    std::string msg = "error param type.";
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, msg.c_str(), NAPI_AUTO_LENGTH,
                        &nResultMsg));
                    result[PARAM0] = GetCallbackErrorValue(env, CODE_FAILED);
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM0], "Message", nResultMsg));
                }
                NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO,
                    &result[PARAM0], &callResult));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        napi_value result;
        napi_create_int32(env, NAPI_RETURN_ONE, &result);
        return result;
    } else {
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "Recover", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                InnerRecover(env,
                    asyncCallbackInfo->bundleName,
                    asyncCallbackInfo->installParam,
                    asyncCallbackInfo->installResult);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
                ConvertInstallResult(asyncCallbackInfo->installResult);
                napi_value result;
                NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result));
                napi_value nResultMsg;
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(
                    env, asyncCallbackInfo->installResult.resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg));
                NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "statusMessage", nResultMsg));
                napi_value nResultCode;
                NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->installResult.resultCode,
                    &nResultCode));
                NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "status", nResultCode));
                NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred,
                    result));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        return promise;
    }
}

static void InnerUninstall(
    napi_env env, const std::string &bundleName, InstallParam &installParam, InstallResult &installResult)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return;
    }
    auto iBundleInstaller = iBundleMgr->GetBundleInstaller();
    if ((iBundleInstaller == nullptr) || (iBundleInstaller->AsObject() == nullptr)) {
        APP_LOGE("can not get iBundleInstaller");
        return;
    }
    installParam.installFlag = InstallFlag::NORMAL;
    OHOS::sptr<InstallerCallback> callback = new (std::nothrow) InstallerCallback();
    if (!callback) {
        APP_LOGE("callback nullptr");
        return;
    }

    sptr<BundleDeathRecipient> recipient(new (std::nothrow) BundleDeathRecipient(callback));
    iBundleInstaller->AsObject()->AddDeathRecipient(recipient);
    iBundleInstaller->Uninstall(bundleName, installParam, callback);
    installResult.resultMsg = callback->GetResultMsg();
    APP_LOGI("-----InnerUninstall resultMsg %{public}s-----", installResult.resultMsg.c_str());
    installResult.resultCode = callback->GetResultCode();
    APP_LOGI("-----InnerUninstall resultCode %{public}d-----", installResult.resultCode);
}
/**
 * Promise and async callback
 */
napi_value Uninstall(napi_env env, napi_callback_info info)
{
    APP_LOGI("Uninstall called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    APP_LOGI("argc = [%{public}zu]", argc);
    AsyncInstallCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncInstallCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
    std::string bundleName;
    ParseString(env, bundleName, argv[PARAM0]);
    InstallParam installParam;
    if (!ParseInstallParam(env, installParam, argv[PARAM1])) {
        APP_LOGE("Uninstall installParam error.");
        asyncCallbackInfo->errCode = PARAM_TYPE_ERROR;
    }

    asyncCallbackInfo->installParam = installParam;
    asyncCallbackInfo->bundleName = bundleName;
    if (argc > (ARGS_SIZE_THREE - CALLBACK_SIZE)) {
        APP_LOGI("Uninstall asyncCallback.");
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[ARGS_SIZE_TWO], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[ARGS_SIZE_TWO], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "Uninstall", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                if (!asyncCallbackInfo->errCode) {
                    InnerUninstall(env,
                        asyncCallbackInfo->bundleName,
                        asyncCallbackInfo->installParam,
                        asyncCallbackInfo->installResult);
                }
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                if (!asyncCallbackInfo->errCode) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[PARAM1]));
                    ConvertInstallResult(asyncCallbackInfo->installResult);
                    napi_value nResultMsg;
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(
                        env, asyncCallbackInfo->installResult.resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg));
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM1], "statusMessage",
                        nResultMsg));
                    napi_value nResultCode;
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->installResult.resultCode,
                        &nResultCode));
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM1], "status", nResultCode));
                    result[PARAM0] = GetCallbackErrorValue(
                        env, (asyncCallbackInfo->installResult.resultCode == 0) ? CODE_SUCCESS : CODE_FAILED);
                } else {
                    napi_value nResultMsg;
                    std::string msg = "error param type.";
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, msg.c_str(), NAPI_AUTO_LENGTH,
                        &nResultMsg));
                    result[PARAM0] = GetCallbackErrorValue(env, CODE_FAILED);
                    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result[PARAM0], "Message", nResultMsg));
                }
                NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO,
                    &result[PARAM0], &callResult));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        napi_value result;
        napi_create_int32(env, NAPI_RETURN_ONE, &result);
        return result;
    } else {
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "Install", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                InnerUninstall(
                    env, asyncCallbackInfo->param, asyncCallbackInfo->installParam, asyncCallbackInfo->installResult);
            },
            [](napi_env env, napi_status status, void *data) {
                APP_LOGI("=================load=================");
                AsyncInstallCallbackInfo *asyncCallbackInfo = (AsyncInstallCallbackInfo *)data;
                std::unique_ptr<AsyncInstallCallbackInfo> callbackPtr {asyncCallbackInfo};
                ConvertInstallResult(asyncCallbackInfo->installResult);
                napi_value result;
                NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result));
                napi_value nResultMsg;
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(
                    env, asyncCallbackInfo->installResult.resultMsg.c_str(), NAPI_AUTO_LENGTH, &nResultMsg));
                NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "statusMessage", nResultMsg));
                napi_value nResultCode;
                NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->installResult.resultCode,
                    &nResultCode));
                NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "status", nResultCode));
                NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred,
                    result));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        return promise;
    }
}

napi_value BundleInstallerConstructor(napi_env env, napi_callback_info info)
{
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

static bool InnerGetAllFormsInfo(napi_env env, std::vector<OHOS::AppExecFwk::FormInfo> &formInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->GetAllFormsInfo(formInfos);
}

static void ProcessFormsInfo(napi_env env, napi_value result, const std::vector<OHOS::AppExecFwk::FormInfo> &formInfos)
{
    if (formInfos.size() > 0) {
        APP_LOGI("-----formInfos is not null-----");
        size_t index = 0;
        for (const auto &item : formInfos) {
            APP_LOGI("name{%s} ", item.name.c_str());
            APP_LOGI("bundleName{%s} ", item.bundleName.c_str());
            napi_value objFormInfo;
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objFormInfo));
            ConvertFormInfo(env, objFormInfo, item);
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, objFormInfo));
            index++;
        }
    } else {
        APP_LOGI("-----formInfos is null-----");
    }
}
/**
 * Promise and async callback
 */
napi_value GetAllFormsInfo(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGI("ARGCSIZE is =%{public}zu.", argc);

    AsyncFormInfosCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncFormInfosCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncFormInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
    if (argc > (ARGS_SIZE_ONE - CALLBACK_SIZE)) {
        APP_LOGI("GetAllFormsInfo asyncCallback.");
        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetAllFormsInfo", NAPI_AUTO_LENGTH, &resourceName));
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[PARAM0], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));

        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncFormInfosCallbackInfo *asyncCallbackInfo = (AsyncFormInfosCallbackInfo *)data;
                asyncCallbackInfo->ret = InnerGetAllFormsInfo(env, asyncCallbackInfo->formInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncFormInfosCallbackInfo *asyncCallbackInfo = (AsyncFormInfosCallbackInfo *)data;
                std::unique_ptr<AsyncFormInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
                NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[PARAM1]));
                ProcessFormsInfo(env, result[PARAM1], asyncCallbackInfo->formInfos);
                result[PARAM0] = GetCallbackErrorValue(env, asyncCallbackInfo->ret ? CODE_SUCCESS : CODE_FAILED);
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO,
                    &result[PARAM0], &callResult));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        napi_value result;
        napi_create_int32(env, NAPI_RETURN_ONE, &result);
        return result;
    } else {
        APP_LOGI("GetFormInfos promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetFormInfos", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncFormInfosCallbackInfo *asyncCallbackInfo = (AsyncFormInfosCallbackInfo *)data;
                InnerGetAllFormsInfo(env, asyncCallbackInfo->formInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                APP_LOGI("=================load=================");
                AsyncFormInfosCallbackInfo *asyncCallbackInfo = (AsyncFormInfosCallbackInfo *)data;
                std::unique_ptr<AsyncFormInfosCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result;
                NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result));
                ProcessFormsInfo(env, result, asyncCallbackInfo->formInfos);
                NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred,
                    result));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        return promise;
    }
}

static bool InnerGetFormInfosByModule(napi_env env, const std::string &bundleName, const std::string &moduleName,
    std::vector<OHOS::AppExecFwk::FormInfo> &formInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->GetFormsInfoByModule(bundleName, moduleName, formInfos);
}
/**
 * Promise and async callback
 */
napi_value GetFormsInfoByModule(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGI("ARGCSIZE is =%{public}zu.", argc);
    std::string bundleName;
    std::string moduleName;
    ParseString(env, bundleName, argv[PARAM0]);
    ParseString(env, moduleName, argv[PARAM1]);

    AsyncFormInfosByModuleCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncFormInfosByModuleCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncFormInfosByModuleCallbackInfo> callbackPtr {asyncCallbackInfo};
    asyncCallbackInfo->bundleName = bundleName;
    asyncCallbackInfo->moduleName = moduleName;

    if (argc > (ARGS_SIZE_THREE - CALLBACK_SIZE)) {
        APP_LOGI("GetFormsInfoByModule asyncCallback.");
        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetFormsInfoByModule", NAPI_AUTO_LENGTH, &resourceName));
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[ARGS_SIZE_TWO], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[ARGS_SIZE_TWO], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));

        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncFormInfosByModuleCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByModuleCallbackInfo *)data;
                asyncCallbackInfo->ret = InnerGetFormInfosByModule(
                    env, asyncCallbackInfo->bundleName, asyncCallbackInfo->moduleName, asyncCallbackInfo->formInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncFormInfosByModuleCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByModuleCallbackInfo *)data;
                std::unique_ptr<AsyncFormInfosByModuleCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
                NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[PARAM1]));
                ProcessFormsInfo(env, result[PARAM1], asyncCallbackInfo->formInfos);
                result[PARAM0] = GetCallbackErrorValue(env, asyncCallbackInfo->ret ? CODE_SUCCESS : CODE_FAILED);
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO,
                    &result[PARAM0], &callResult));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        napi_value result;
        napi_create_int32(env, NAPI_RETURN_ONE, &result);
        return result;
    } else {
        APP_LOGI("GetFormsInfoByModule promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetFormsInfoByModule", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncFormInfosByModuleCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByModuleCallbackInfo *)data;
                InnerGetFormInfosByModule(
                    env, asyncCallbackInfo->bundleName, asyncCallbackInfo->moduleName, asyncCallbackInfo->formInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                APP_LOGI("=================load=================");
                AsyncFormInfosByModuleCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByModuleCallbackInfo *)data;
                std::unique_ptr<AsyncFormInfosByModuleCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result;
                NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result));
                ProcessFormsInfo(env, result, asyncCallbackInfo->formInfos);
                NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred,
                    result));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        return promise;
    }
}

static bool InnerGetFormInfosByApp(
    napi_env env, const std::string &bundleName, std::vector<OHOS::AppExecFwk::FormInfo> &formInfos)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->GetFormsInfoByApp(bundleName, formInfos);
}
/**
 * Promise and async callback
 */
napi_value GetFormsInfoByApp(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    APP_LOGI("ARGCSIZE is =%{public}zu.", argc);
    std::string bundleName;
    ParseString(env, bundleName, argv[PARAM0]);

    AsyncFormInfosByAppCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncFormInfosByAppCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncFormInfosByAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    asyncCallbackInfo->bundleName = bundleName;
    if (argc > (ARGS_SIZE_TWO - CALLBACK_SIZE)) {
        APP_LOGI("GetFormsInfoByApp asyncCallback.");
        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetFormsInfoByApp", NAPI_AUTO_LENGTH, &resourceName));
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, argv[ARGS_SIZE_ONE], &valuetype);
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        NAPI_CALL(env, napi_create_reference(env, argv[ARGS_SIZE_ONE], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));

        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncFormInfosByAppCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByAppCallbackInfo *)data;
                asyncCallbackInfo->ret =
                    InnerGetFormInfosByApp(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->formInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                AsyncFormInfosByAppCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByAppCallbackInfo *)data;
                std::unique_ptr<AsyncFormInfosByAppCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result[ARGS_SIZE_TWO] = {0};
                napi_value callback = 0;
                napi_value undefined = 0;
                napi_value callResult = 0;
                NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
                NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[PARAM1]));
                ProcessFormsInfo(env, result[PARAM1], asyncCallbackInfo->formInfos);
                result[PARAM0] = GetCallbackErrorValue(env, asyncCallbackInfo->ret ? CODE_SUCCESS : CODE_FAILED);
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_SIZE_TWO,
                    &result[PARAM0], &callResult));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        napi_value result;
        napi_create_int32(env, NAPI_RETURN_ONE, &result);
        return result;
    } else {
        APP_LOGI("GetFormsInfoByApp promise.");
        napi_deferred deferred;
        napi_value promise;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
        asyncCallbackInfo->deferred = deferred;

        napi_value resourceName;
        NAPI_CALL(env, napi_create_string_latin1(env, "GetFormsInfoByApp", NAPI_AUTO_LENGTH, &resourceName));
        NAPI_CALL(env, napi_create_async_work(
            env,
            nullptr,
            resourceName,
            [](napi_env env, void *data) {
                AsyncFormInfosByAppCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByAppCallbackInfo *)data;
                InnerGetFormInfosByApp(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->formInfos);
            },
            [](napi_env env, napi_status status, void *data) {
                APP_LOGI("=================load=================");
                AsyncFormInfosByAppCallbackInfo *asyncCallbackInfo = (AsyncFormInfosByAppCallbackInfo *)data;
                std::unique_ptr<AsyncFormInfosByAppCallbackInfo> callbackPtr {asyncCallbackInfo};
                napi_value result;
                NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result));
                ProcessFormsInfo(env, result, asyncCallbackInfo->formInfos);
                NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred,
                    result));
            },
            (void *)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));
        NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
        callbackPtr.release();
        return promise;
    }
}

static bool InnerGetBundleGids(napi_env env, const std::string &bundleName, std::vector<int> &gids)
{
    auto iBundleMgr = GetBundleMgr();
    if (iBundleMgr == nullptr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    };
    auto ret = iBundleMgr->GetBundleGids(bundleName, gids);
    return ret;
}

static void ProcessGids(napi_env env, napi_value result, const std::vector<int32_t> &gids)
{
    if (gids.size() > 0) {
        APP_LOGI("-----gids is not null-----");
        size_t index = 0;
        for (const auto &item : gids) {
            napi_value value;
            napi_create_int32(env, item, &value);
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, value));
            index++;
        }
        APP_LOGI("-----gids is not null  end-----");
    } else {
        APP_LOGI("-----ShortcutInfos is null-----");
    }
}

napi_value WrapVoidToJS(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

napi_value WrapUndefinedToJS(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

AsyncGetBundleGidsCallbackInfo *CreateAsyncGetBundleGidsCallbackInfo(napi_env env)
{
    APP_LOGI("%{public}s called.", __func__);
    AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncGetBundleGidsCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    APP_LOGI("%{public}s end.", __func__);
    return asyncCallbackInfo;
}

void GetBundleGidsExecute(napi_env env, void *data)
{
    APP_LOGI("NAPI_GetBundleGids, worker pool thread execute.");
    AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo = static_cast<AsyncGetBundleGidsCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("NAPI_GetBundleGids, asyncCallbackInfo == nullptr");
        return;
    }

    bool ret = InnerGetBundleGids(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->gids);

    if (!ret) {
        asyncCallbackInfo->err = NAPI_RETURN_FAILED;
    }
}

void GetBundleGidsAsyncComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_GetBundleGids, main event thread complete.");
    AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo = static_cast<AsyncGetBundleGidsCallbackInfo *>(data);
    std::unique_ptr<AsyncGetBundleGidsCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_SIZE_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    result[PARAM0] = GetCallbackErrorValue(env, asyncCallbackInfo->err);

    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result[PARAM1]));
        ProcessGids(env, result[PARAM1], asyncCallbackInfo->gids);
    } else {
        result[PARAM1] = WrapUndefinedToJS(env);
    }

    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
    NAPI_CALL_RETURN_VOID(env,
        napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[PARAM0]), result, &callResult));
    APP_LOGI("NAPI_GetApplicationInfo, main event thread complete end.");
}

void GetBundleGidsPromiseComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_GetBundleGids, main event thread complete.");
    AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo = static_cast<AsyncGetBundleGidsCallbackInfo *>(data);
    std::unique_ptr<AsyncGetBundleGidsCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result = nullptr;
    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &result));
        ProcessGids(env, result, asyncCallbackInfo->gids);
        NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result));
    } else {
        result = GetCallbackErrorValue(env, asyncCallbackInfo->err);
        NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result));
    }
    APP_LOGI("NAPI_GetApplicationInfo, main event thread complete end.");
}

napi_value GetBundleGidsAsync(
    napi_env env, napi_value *args, const size_t argCallback, AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
    } else {
        return nullptr;
    }

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, GetBundleGidsExecute, GetBundleGidsAsyncComplete,
                       (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value GetBundleGidsPromise(napi_env env, AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, GetBundleGidsExecute,
                       GetBundleGidsPromiseComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}

napi_value GetBundleGidsWrap(napi_env env, napi_callback_info info, AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    size_t argcAsync = ARGS_SIZE_TWO;
    const size_t argcPromise = ARGS_SIZE_ONE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[PARAM0], &valueType));
    if (valueType == napi_string) {
        ParseString(env, asyncCallbackInfo->bundleName, args[PARAM0]);
    } else {
        asyncCallbackInfo->err = INVALID_PARAM;
    }

    if (argcAsync > argcPromise) {
        ret = GetBundleGidsAsync(env, args, argcAsync - 1, asyncCallbackInfo);
    } else {
        ret = GetBundleGidsPromise(env, asyncCallbackInfo);
    }
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return ret;
}

napi_value GetBundleGids(napi_env env, napi_callback_info info)
{
    APP_LOGI("NAPI_GetBundleGids start");
    AsyncGetBundleGidsCallbackInfo *asyncCallbackInfo = CreateAsyncGetBundleGidsCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return WrapVoidToJS(env);
    }
    std::unique_ptr<AsyncGetBundleGidsCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value ret = GetBundleGidsWrap(env, info, asyncCallbackInfo);
    if (ret == nullptr) {
        APP_LOGE("%{public}s ret == nullptr", __func__);
        ret = WrapVoidToJS(env);
    } else {
        callbackPtr.release();
    }
    APP_LOGI("%{public}s end.", __func__);
    return ret;
}

static bool InnerSetApplicationEnabled(napi_env env, const std::string &bundleName, bool isEnable)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    auto result = iBundleMgr->SetApplicationEnabled(bundleName, isEnable);
    if (result) {
        APP_LOGE("InnerSetApplicationEnabled::SetApplicationEnabled");
    }
    return result;
}

static bool InnerSetAbilityEnabled(napi_env env, const OHOS::AppExecFwk::AbilityInfo &abilityInfo, bool isEnable)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    auto result = iBundleMgr->SetAbilityEnabled(abilityInfo, isEnable);
    if (result) {
        APP_LOGE("InnerSetAbilityEnabled::SetAbilityEnabled");
    }
    return result;
}

static bool InnerCleanBundleCacheCallback(
    const std::string& bundleName, const OHOS::sptr<CleanCacheCallback>& cleanCacheCallback)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    if (!cleanCacheCallback) {
        APP_LOGE("callback nullptr");
        return false;
    }
    int32_t userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
    auto result = iBundleMgr->CleanBundleCacheFiles(bundleName, cleanCacheCallback, userId);
    if (!result) {
        APP_LOGE("CleanBundleDataFiles call error");
        return result;
    }

    return result;
}

napi_value SetApplicationEnabled(napi_env env, napi_callback_info info)
{
    size_t requireArgc = ARGS_SIZE_TWO;
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 2 parameter");

    EnabledInfo *asyncCallbackInfo = new (std::nothrow) EnabledInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    asyncCallbackInfo->env = env;
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, argv[i]);
        } else if ((i == PARAM1) && (valueType == napi_boolean)) {
            bool isEnable = false;
            NAPI_CALL(env, napi_get_value_bool(env, argv[i], &isEnable));
            asyncCallbackInfo->isEnable = isEnable;
        } else if ((i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->errCode = INVALID_PARAM;
            asyncCallbackInfo->errMssage = "type misMatch";
        }
    }
    napi_value promise = nullptr;

    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "JSSetApplicationEnabled", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            EnabledInfo* asyncCallbackInfo = (EnabledInfo*)data;
            if (!asyncCallbackInfo->errCode) {
                asyncCallbackInfo->result = InnerSetApplicationEnabled(asyncCallbackInfo->env,
                                                                       asyncCallbackInfo->bundleName,
                                                                       asyncCallbackInfo->isEnable);
                if (!asyncCallbackInfo->result) {
                    asyncCallbackInfo->errCode = OPERATION_FAILED;
                }
            }
        },
        [](napi_env env, napi_status status, void* data) {
            EnabledInfo* asyncCallbackInfo = (EnabledInfo*)data;
            std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[1] = { 0 };
            if (asyncCallbackInfo->errCode) {
                NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->errCode, &result[0]));
            }
            if (asyncCallbackInfo->callback) {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            } else {
                if (asyncCallbackInfo->errCode) {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[0]));
                }
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

napi_value SetAbilityEnabled(napi_env env, napi_callback_info info)
{
    size_t requireArgc = ARGS_SIZE_TWO;
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = { 0 };
    napi_value thisArg = nullptr;
    void* data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 2 parameter");

    EnabledInfo* asyncCallbackInfo = new (std::nothrow) EnabledInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    asyncCallbackInfo->env = env;
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_object)) {
            if (!UnwrapAbilityInfo(env, argv[i], asyncCallbackInfo->abilityInfo)) {
                asyncCallbackInfo->errCode = INVALID_PARAM;
                asyncCallbackInfo->errMssage = "type misMatch";
            }
        } else if ((i == PARAM1) && (valueType == napi_boolean)) {
            bool isEnable = false;
            NAPI_CALL(env, napi_get_value_bool(env, argv[i], &isEnable));
            asyncCallbackInfo->isEnable = isEnable;
        } else if ((i == PARAM2) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            break;
        } else {
            asyncCallbackInfo->errCode = INVALID_PARAM;
            asyncCallbackInfo->errMssage = "type misMatch";
        }
    }
    napi_value promise = nullptr;

    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "JSSetApplicationEnabled", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            EnabledInfo* asyncCallbackInfo = (EnabledInfo*)data;
            if (!asyncCallbackInfo->errCode) {
                asyncCallbackInfo->result = InnerSetAbilityEnabled(asyncCallbackInfo->env,
                                                                   asyncCallbackInfo->abilityInfo,
                                                                   asyncCallbackInfo->isEnable);
                if (!asyncCallbackInfo->result) {
                    asyncCallbackInfo->errCode = OPERATION_FAILED;
                }
            }
        },
        [](napi_env env, napi_status status, void* data) {
            EnabledInfo* asyncCallbackInfo = (EnabledInfo*)data;
            std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[1] = { 0 };
            NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->errCode, &result[0]));
            if (asyncCallbackInfo->callback) {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            } else {
                if (asyncCallbackInfo->errCode) {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[0]));
                }
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

static bool InnerIsAbilityEnabled(napi_env env, const OHOS::AppExecFwk::AbilityInfo &abilityInfo)
{
    auto iBundleMgr = GetBundleMgr();
    if (iBundleMgr == nullptr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    auto result = iBundleMgr->IsAbilityEnabled(abilityInfo);
    if (result) {
        APP_LOGI("InnerIsAbilityEnabled::IsAbilityEnabled");
    }
    return result;
}

EnabledInfo *CreateAsyncIsAbilityEnabledCallbackInfo(napi_env env)
{
    APP_LOGI("%{public}s called.", __func__);
    EnabledInfo *asyncCallbackInfo = new (std::nothrow) EnabledInfo(env);

    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    APP_LOGI("%{public}s end.", __func__);
    return asyncCallbackInfo;
}

void IsAbilityEnabledExecute(napi_env env, void *data)
{
    APP_LOGI("NAPI_IsAbilityEnabled, worker pool thread execute.");
    EnabledInfo *asyncCallbackInfo = static_cast<EnabledInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("NAPI_IsAbilityEnabled, asyncCallbackInfo == nullptr");
        return;
    }
    asyncCallbackInfo->result = InnerIsAbilityEnabled(env, asyncCallbackInfo->abilityInfo);
}

void IsAbilityEnabledAsyncComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_IsAbilityEnabled, main event thread complete.");
    EnabledInfo *asyncCallbackInfo = static_cast<EnabledInfo *>(data);
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_SIZE_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->errCode, &result[PARAM0]));

    if (asyncCallbackInfo->errCode == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result[PARAM1]));
    } else {
        result[PARAM1] = WrapUndefinedToJS(env);
    }

    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
    NAPI_CALL_RETURN_VOID(env,
        napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[PARAM0]), result, &callResult));
    APP_LOGI("NAPI_IsAbilityEnabled, main event thread complete end.");
}

void IsAbilityEnabledPromiseComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_GetBundleGids, main event thread complete.");
    EnabledInfo *asyncCallbackInfo = static_cast<EnabledInfo *>(data);
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    napi_value result = nullptr;
    if (asyncCallbackInfo->errCode == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result));
        NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result));
    } else {
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->errCode, &result));
        NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result));
    }
    APP_LOGI("NAPI_GetApplicationInfo, main event thread complete end.");
}

napi_value IsAbilityEnabledAsync(
    napi_env env, napi_value args, EnabledInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args, &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args, NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
    } else {
        return nullptr;
    }

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, IsAbilityEnabledExecute,
                       IsAbilityEnabledAsyncComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value IsAbilityEnabledPromise(napi_env env, EnabledInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, IsAbilityEnabledExecute,
                       IsAbilityEnabledPromiseComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}

napi_value IsAbilityEnabledWrap(napi_env env, napi_callback_info info, EnabledInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    size_t argcAsync = ARGS_SIZE_TWO;
    const size_t argcPromise = ARGS_SIZE_ONE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, args[PARAM0], &valueType);
    if (valueType == napi_object) {
        if (!UnwrapAbilityInfo(env, args[PARAM0], asyncCallbackInfo->abilityInfo)) {
            asyncCallbackInfo->errCode = INVALID_PARAM;
        }
    } else {
        asyncCallbackInfo->errCode = INVALID_PARAM;
    }

    if (argcAsync > argcPromise) {
        ret = IsAbilityEnabledAsync(env, args[PARAM1], asyncCallbackInfo);
    } else {
        ret = IsAbilityEnabledPromise(env, asyncCallbackInfo);
    }
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return ret;
}

napi_value IsAbilityEnabled(napi_env env, napi_callback_info info)
{
    APP_LOGI("NAPI_IsAbilityEnabled start");
    EnabledInfo *asyncCallbackInfo = CreateAsyncIsAbilityEnabledCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return WrapVoidToJS(env);
    }
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    napi_value ret = IsAbilityEnabledWrap(env, info, asyncCallbackInfo);

    if (ret == nullptr) {
        APP_LOGE("%{public}s ret == nullptr", __func__);
        ret = WrapVoidToJS(env);
    } else {
        callbackPtr.release();
    }
    APP_LOGI("%{public}s end.", __func__);
    return ret;
}

static bool InnerIsApplicationEnabled(napi_env env, const std::string &bundleName)
{
    auto iBundleMgr = GetBundleMgr();
    if (iBundleMgr == nullptr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    auto result = iBundleMgr->IsApplicationEnabled(bundleName);
    if (result) {
        APP_LOGI("InnerIsApplicationEnabled::IsApplicationEnabled");
    }
    return result;
}

EnabledInfo *CreateAsyncIsApplicationEnabledCallbackInfo(napi_env env)
{
    APP_LOGI("%{public}s called.", __func__);
    EnabledInfo *asyncCallbackInfo = new (std::nothrow) EnabledInfo(env);

    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    APP_LOGI("%{public}s end.", __func__);
    return asyncCallbackInfo;
}

void IsApplicationEnabledExecute(napi_env env, void *data)
{
    APP_LOGI("NAPI_IsApplicationEnabled, worker pool thread execute.");
    EnabledInfo *asyncCallbackInfo = static_cast<EnabledInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("NAPI_IsApplicationEnabled, asyncCallbackInfo == nullptr");
        return;
    }
    asyncCallbackInfo->result = InnerIsApplicationEnabled(env, asyncCallbackInfo->bundleName);
}

void IsApplicationEnabledAsyncComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_ApplicationEnabled, main event thread complete.");
    EnabledInfo *asyncCallbackInfo = static_cast<EnabledInfo *>(data);
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_SIZE_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    if (asyncCallbackInfo->errCode) {
        napi_create_int32(env, asyncCallbackInfo->errCode, &result[PARAM0]);
    }

    if (asyncCallbackInfo->errCode == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result[PARAM1]));
    } else {
        result[PARAM1] = WrapUndefinedToJS(env);
    }

    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
    NAPI_CALL_RETURN_VOID(env,
        napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[PARAM0]), result, &callResult));
    APP_LOGI("NAPI_ApplicationEnabled, main event thread complete end.");
}

void IsApplicationEnabledPromiseComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_IsApplicationEnabled, main event thread complete.");
    EnabledInfo *asyncCallbackInfo = static_cast<EnabledInfo *>(data);
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    napi_value result = nullptr;
    if (asyncCallbackInfo->errCode == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result));
        NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result));
    } else {
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->errCode, &result));
        NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result));
    }
    APP_LOGI("NAPI_IsApplicationEnabled, main event thread complete end.");
}

napi_value IsApplicationEnabledAsync(
    napi_env env, napi_value *args, const size_t argCallback, EnabledInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
    } else {
        return nullptr;
    }

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, IsApplicationEnabledExecute,
                       IsApplicationEnabledAsyncComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value IsApplicationEnabledPromise(napi_env env, EnabledInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, IsApplicationEnabledExecute,
                       IsApplicationEnabledPromiseComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}

napi_value IsApplicationEnabledWrap(napi_env env, napi_callback_info info, EnabledInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    size_t argcAsync = ARGS_SIZE_TWO;
    const size_t argcPromise = ARGS_SIZE_ONE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, args[PARAM0], &valueType);
    if (valueType == napi_string) {
        ParseString(env, asyncCallbackInfo->bundleName, args[PARAM0]);
    } else {
        asyncCallbackInfo->errCode = INVALID_PARAM;
        asyncCallbackInfo->errMssage = "type misMatch";
    }

    if (argcAsync > argcPromise) {
        ret = IsApplicationEnabledAsync(env, args, argcAsync - 1, asyncCallbackInfo);
    } else {
        ret = IsApplicationEnabledPromise(env, asyncCallbackInfo);
    }
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return ret;
}

napi_value IsApplicationEnabled(napi_env env, napi_callback_info info)
{
    APP_LOGI("NAPI_IsApplicationEnabled start");
    EnabledInfo *asyncCallbackInfo = CreateAsyncIsApplicationEnabledCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return WrapVoidToJS(env);
    }
    std::unique_ptr<EnabledInfo> callbackPtr {asyncCallbackInfo};
    napi_value ret = IsApplicationEnabledWrap(env, info, asyncCallbackInfo);

    if (ret == nullptr) {
        APP_LOGE("%{public}s ret == nullptr", __func__);
        ret = WrapVoidToJS(env);
    } else {
        callbackPtr.release();
    }
    APP_LOGI("%{public}s end.", __func__);
    return ret;
}

static bool InnerIsModuleRemovableExecute(napi_env env, const std::string &bundleName, const std::string &moduleName)
{
    auto iBundleMgr = GetBundleMgr();
    if (iBundleMgr == nullptr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    auto result = iBundleMgr->IsModuleRemovable(bundleName, moduleName);
    if (result) {
        APP_LOGI("InnerIsModuleRemovableExecute::IsModuleRemovable");
    }
    return result;
}

void IsModuleRemovableExecute(napi_env env, void *data)
{
    APP_LOGI("NAPI_IsModuleRemovable, worker pool thread execute.");
    AsyncModuleRemovableCallbackInfo *asyncCallbackInfo = static_cast<AsyncModuleRemovableCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("NAPI_IsModuleRemovable, asyncCallbackInfo == nullptr");
        return;
    }
    asyncCallbackInfo->result =
        InnerIsModuleRemovableExecute(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->moduleName);
}

void IsModuleRemovableAsyncComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_IsModuleRemovable, main event thread complete.");
    AsyncModuleRemovableCallbackInfo *asyncCallbackInfo = static_cast<AsyncModuleRemovableCallbackInfo *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_SIZE_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    if (asyncCallbackInfo->err) {
        napi_create_int32(env, asyncCallbackInfo->err, &result[PARAM0]);
    }

    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result[PARAM1]));
    } else {
        result[PARAM1] = WrapUndefinedToJS(env);
    }

    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
    NAPI_CALL_RETURN_VOID(env,
        napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[PARAM0]), result, &callResult));

    if (asyncCallbackInfo->callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, asyncCallbackInfo->callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, asyncCallbackInfo->asyncWork));
    delete asyncCallbackInfo;
    asyncCallbackInfo = nullptr;
    APP_LOGI("NAPI_ModuleRemovable, main event thread complete end.");
}

void IsModuleRemovablePromiseComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_IsModuleRemovable, main event thread complete.");
    AsyncModuleRemovableCallbackInfo *asyncCallbackInfo = static_cast<AsyncModuleRemovableCallbackInfo *>(data);
    napi_value result = nullptr;
    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result));
        napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
    } else {
        napi_create_int32(env, asyncCallbackInfo->err, &result);
        napi_reject_deferred(env, asyncCallbackInfo->deferred, result);
    }

    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    delete asyncCallbackInfo;
    asyncCallbackInfo = nullptr;
    APP_LOGI("NAPI_IsModuleRemovable, main event thread complete end.");
}

napi_value IsModuleRemovableAsync(
    napi_env env, napi_value *args, const size_t argCallback, AsyncModuleRemovableCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param is nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
    } else {
        return nullptr;
    }

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, IsModuleRemovableExecute,
                       IsModuleRemovableAsyncComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value IsModuleRemovablePromise(napi_env env, AsyncModuleRemovableCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param is nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, IsModuleRemovableExecute,
                       IsModuleRemovablePromiseComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}

napi_value GetModuleRemovableWrap(
    napi_env env, napi_callback_info info, AsyncModuleRemovableCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo is nullptr.", __func__);
        return nullptr;
    }

    size_t argcAsync = ARGS_SIZE_THREE;
    const size_t argcPromise = ARGS_SIZE_TWO;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, args[PARAM0], &valueType);
    if (valueType == napi_string) {
        ParseString(env, asyncCallbackInfo->bundleName, args[PARAM0]);
    } else {
        asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        asyncCallbackInfo->errMssage = "type misMatch";
    }

    napi_valuetype secondValueType = napi_undefined;
    napi_typeof(env, args[PARAM1], &secondValueType);
    if (secondValueType == napi_string) {
        ParseString(env, asyncCallbackInfo->moduleName, args[PARAM1]);
    } else {
        asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        asyncCallbackInfo->errMssage = "type misMatch";
    }

    APP_LOGI("bundleName: %{public}s, moduleName: %{public}s",
        asyncCallbackInfo->bundleName.c_str(), asyncCallbackInfo->moduleName.c_str());
    if (asyncCallbackInfo->bundleName.empty() || asyncCallbackInfo->moduleName.empty()) {
        asyncCallbackInfo->err = INVALID_PARAM;
        asyncCallbackInfo->errMssage = "invalid param";
    }

    if (argcAsync > argcPromise) {
        ret = IsModuleRemovableAsync(env, args, argcAsync - 1, asyncCallbackInfo);
    } else {
        ret = IsModuleRemovablePromise(env, asyncCallbackInfo);
    }
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return ret;
}

AsyncModuleRemovableCallbackInfo *CreateModuleRemovableCallbackInfo(napi_env env)
{
    APP_LOGI("%{public}s called.", __func__);
    AsyncModuleRemovableCallbackInfo *asyncCallbackInfo = new AsyncModuleRemovableCallbackInfo {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };

    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo is nullptr.", __func__);
        return nullptr;
    }

    APP_LOGI("%{public}s end.", __func__);
    return asyncCallbackInfo;
}

napi_value IsModuleRemovable(napi_env env, napi_callback_info info)
{
    APP_LOGI("NAPI_IsModuleRemovable start");
    AsyncModuleRemovableCallbackInfo *asyncCallbackInfo = CreateModuleRemovableCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return WrapVoidToJS(env);
    }

    napi_value ret = GetModuleRemovableWrap(env, info, asyncCallbackInfo);

    if (ret == nullptr) {
        APP_LOGE("%{public}s ret is nullptr", __func__);
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    APP_LOGI("%{public}s end.", __func__);
    return ret;
}

static bool InnerSetModuleUpgradeFlagExecute(napi_env env,
    const std::string &bundleName, const std::string &moduleName, int32_t upgradeFlag)
{
    auto iBundleMgr = GetBundleMgr();
    if (iBundleMgr == nullptr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    auto result = iBundleMgr->SetModuleUpgradeFlag(bundleName, moduleName, upgradeFlag);
    if (result) {
        APP_LOGI("InnerSetModuleUpgradeFlagExecute::SetModuleUpgradeFlag");
    }
    return result;
}

void SetModuleUpgradeFlagExecute(napi_env env, void *data)
{
    APP_LOGI("NAPI_SetModuleUpgradeFlag, worker pool thread execute.");
    AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo = static_cast<AsyncModuleUpgradeFlagCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("NAPI_SetModuleUpgradeFlag, asyncCallbackInfo == nullptr");
        return;
    }
    asyncCallbackInfo->result = InnerSetModuleUpgradeFlagExecute(env, asyncCallbackInfo->bundleName,
        asyncCallbackInfo->moduleName, asyncCallbackInfo->upgradeFlag);
}

void SetModuleUpgradeFlagAsyncComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_SetModuleUpgradeFlag, main event thread complete.");
    AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo = static_cast<AsyncModuleUpgradeFlagCallbackInfo *>(data);
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_SIZE_THREE] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    if (asyncCallbackInfo->err) {
        napi_create_int32(env, asyncCallbackInfo->err, &result[PARAM0]);
    }

    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result[PARAM1]));
    } else {
        result[PARAM1] = WrapUndefinedToJS(env);
    }

    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
    NAPI_CALL_RETURN_VOID(env,
        napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[PARAM0]), result, &callResult));

    if (asyncCallbackInfo->callback != nullptr) {
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, asyncCallbackInfo->callback));
    }
    NAPI_CALL_RETURN_VOID(env, napi_delete_async_work(env, asyncCallbackInfo->asyncWork));
    delete asyncCallbackInfo;
    asyncCallbackInfo = nullptr;
    APP_LOGI("NAPI_ModuleUpgradeFlag, main event thread complete end.");
}

void SetModuleUpgradeFlagPromiseComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_SetModuleUpgradeFlag, main event thread complete.");
    AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo = static_cast<AsyncModuleUpgradeFlagCallbackInfo *>(data);
    napi_value result = nullptr;
    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->result, &result));
        napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
    } else {
        napi_create_int32(env, asyncCallbackInfo->err, &result);
        napi_reject_deferred(env, asyncCallbackInfo->deferred, result);
    }

    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    delete asyncCallbackInfo;
    asyncCallbackInfo = nullptr;
    APP_LOGI("NAPI_SetModuleUpgradeFlag, main event thread complete end.");
}

napi_value SetModuleUpgradeFlagAsync(
    napi_env env, napi_value *args, const size_t argCallback, AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param is nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);

    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, args[argCallback], &valuetype));
    if (valuetype == napi_function) {
        NAPI_CALL(env, napi_create_reference(env, args[argCallback], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
    } else {
        return nullptr;
    }

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, SetModuleUpgradeFlagExecute,
                       SetModuleUpgradeFlagAsyncComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value SetModuleUpgradeFlagPromise(napi_env env, AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param is nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, SetModuleUpgradeFlagExecute,
                       SetModuleUpgradeFlagPromiseComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}

napi_value SetModuleUpgradeFlagWrap(
    napi_env env, napi_callback_info info, AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo)
{
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    size_t argcAsync = ARGS_SIZE_FOUR;
    const size_t argcPromise = ARGS_SIZE_THREE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, args[PARAM0], &valueType);
    if (valueType == napi_string) {
        ParseString(env, asyncCallbackInfo->bundleName, args[PARAM0]);
    } else {
        asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        asyncCallbackInfo->errMssage = "type misMatch";
    }
    napi_valuetype secondValueType = napi_undefined;
    napi_typeof(env, args[PARAM1], &secondValueType);
    if (secondValueType == napi_string) {
        ParseString(env, asyncCallbackInfo->moduleName, args[PARAM1]);
    } else {
        asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        asyncCallbackInfo->errMssage = "type misMatch";
    }
    napi_valuetype thirdValueType = napi_undefined;
    napi_typeof(env, args[PARAM2], &thirdValueType);
    if (thirdValueType == napi_number) {
        ParseInt(env, asyncCallbackInfo->upgradeFlag,  args[PARAM2]);
    } else {
        asyncCallbackInfo->err = PARAM_TYPE_ERROR;
        asyncCallbackInfo->errMssage = "type misMatch";
    }
    if (asyncCallbackInfo->bundleName.empty() || asyncCallbackInfo->moduleName.empty()) {
        asyncCallbackInfo->err = INVALID_PARAM;
        asyncCallbackInfo->errMssage = "invalid param";
    }
    if (argcAsync > argcPromise) {
        return SetModuleUpgradeFlagAsync(env, args, argcAsync - 1, asyncCallbackInfo);
    } else {
        return SetModuleUpgradeFlagPromise(env, asyncCallbackInfo);
    }
}

AsyncModuleUpgradeFlagCallbackInfo *CreateModuleUpgradeFlagCallbackInfo(napi_env env)
{
    APP_LOGI("%{public}s called.", __func__);
    AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo = new AsyncModuleUpgradeFlagCallbackInfo {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };

    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo is nullptr.", __func__);
        return nullptr;
    }

    APP_LOGI("%{public}s end.", __func__);
    return asyncCallbackInfo;
}

napi_value SetModuleUpgradeFlag(napi_env env, napi_callback_info info)
{
    APP_LOGI("NAPI_SetModuleUpgradeFlag start");
    AsyncModuleUpgradeFlagCallbackInfo *asyncCallbackInfo = CreateModuleUpgradeFlagCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return WrapVoidToJS(env);
    }

    napi_value ret = SetModuleUpgradeFlagWrap(env, info, asyncCallbackInfo);

    if (ret == nullptr) {
        APP_LOGE("%{public}s ret is nullptr", __func__);
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
        ret = WrapVoidToJS(env);
    }
    APP_LOGI("%{public}s end.", __func__);
    return ret;
}

AsyncAbilityLabelCallbackInfo *CreateAbilityLabelCallbackInfo(napi_env env)
{
    APP_LOGI("%{public}s called.", __func__);
    AsyncAbilityLabelCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncAbilityLabelCallbackInfo(env);

    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    APP_LOGI("%{public}s end.", __func__);
    return asyncCallbackInfo;
}

void GetAbilityLabelExecute(napi_env env, void *data)
{
    APP_LOGI("NAPI_GetAbilityLabel, worker pool thread execute.");
    AsyncAbilityLabelCallbackInfo *asyncCallbackInfo = static_cast<AsyncAbilityLabelCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("NAPI_GetAbilityLabel, asyncCallbackInfo == nullptr");
        return;
    }
    if (!asyncCallbackInfo->err) {
        asyncCallbackInfo->abilityLabel =
        InnerGetAbilityLabel(env, asyncCallbackInfo->bundleName, asyncCallbackInfo->moduleName,
            asyncCallbackInfo->className, asyncCallbackInfo->hasModuleName);
        if (asyncCallbackInfo->abilityLabel == "") {
            asyncCallbackInfo->err = OPERATION_FAILED;
        }
    }
}

void GetAbilityLabelAsyncComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_GetAbilityLabel, main event thread complete.");
    AsyncAbilityLabelCallbackInfo *asyncCallbackInfo = static_cast<AsyncAbilityLabelCallbackInfo *>(data);
    std::unique_ptr<AsyncAbilityLabelCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value callback = nullptr;
    napi_value undefined = nullptr;
    napi_value result[ARGS_SIZE_TWO] = {nullptr};
    napi_value callResult = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->err, &result[PARAM0]));

    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->abilityLabel.c_str(),
            NAPI_AUTO_LENGTH, &result[PARAM1]));
    } else {
        result[PARAM1] = WrapUndefinedToJS(env);
    }

    NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
    NAPI_CALL_RETURN_VOID(env,
        napi_call_function(env, undefined, callback, sizeof(result) / sizeof(result[PARAM0]), result, &callResult));
    APP_LOGI("NAPI_GetAbilityLabel, main event thread complete end.");
}

void GetAbilityLabelPromiseComplete(napi_env env, napi_status status, void *data)
{
    APP_LOGI("NAPI_GetAbilityLabel, main event thread complete.");
    AsyncAbilityLabelCallbackInfo *asyncCallbackInfo = static_cast<AsyncAbilityLabelCallbackInfo *>(data);
    std::unique_ptr<AsyncAbilityLabelCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result = nullptr;
    if (asyncCallbackInfo->err == NAPI_ERR_NO_ERROR) {
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, asyncCallbackInfo->abilityLabel.c_str(), NAPI_AUTO_LENGTH, &result));
        NAPI_CALL_RETURN_VOID(
            env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result));
    } else {
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->err, &result));
        NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result));
    }
    APP_LOGI("NAPI_GetAbilityLabel, main event thread complete end.");
}

napi_value GetAbilityLabelAsync(
    napi_env env, AsyncAbilityLabelCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, GetAbilityLabelExecute,
                       GetAbilityLabelAsyncComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return result;
}

napi_value GetAbilityLabelPromise(napi_env env, AsyncAbilityLabelCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncCallbackInfo->deferred = deferred;

    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, GetAbilityLabelExecute,
                       GetAbilityLabelPromiseComplete, (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}

napi_value GetAbilityLabelWrap(napi_env env, napi_callback_info info, AsyncAbilityLabelCallbackInfo *asyncCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncCallbackInfo == nullptr.", __func__);
        return nullptr;
    }

    size_t argcAsync = ARGS_SIZE_FOUR;
    const size_t argcPromise = ARGS_SIZE_THREE;
    const size_t argCountWithAsync = argcPromise + ARGS_ASYNC_COUNT;
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync > argCountWithAsync || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }
    asyncCallbackInfo->hasModuleName = HasModuleName(env, argcAsync, args);
    for (size_t i = 0; i < argcAsync; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, args[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, args[i]);
        } else if ((i == PARAM1) && (valueType == napi_string)) {
            if (asyncCallbackInfo->hasModuleName) {
                ParseString(env, asyncCallbackInfo->moduleName, args[i]);
            } else {
                ParseString(env, asyncCallbackInfo->className, args[i]);
            }
        } else if ((i == PARAM2) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->className, args[i]);
        } else if (((i == PARAM2) || (i == PARAM3)) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->err = INVALID_PARAM;
            asyncCallbackInfo->message = "type misMatch";
        }
    }
    if (asyncCallbackInfo->callback) {
        ret = GetAbilityLabelAsync(env, asyncCallbackInfo);
    } else {
        ret = GetAbilityLabelPromise(env, asyncCallbackInfo);
    }
    APP_LOGI("%{public}s, asyncCallback end.", __func__);
    return ret;
}

napi_value GetAbilityLabel(napi_env env, napi_callback_info info)
{
    APP_LOGI("NAPI_GetAbilityLabel start");
    AsyncAbilityLabelCallbackInfo *asyncCallbackInfo = CreateAbilityLabelCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return WrapVoidToJS(env);
    }
    std::unique_ptr<AsyncAbilityLabelCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value ret = GetAbilityLabelWrap(env, info, asyncCallbackInfo);

    if (ret == nullptr) {
        APP_LOGD("%{public}s ret == nullptr", __func__);
        ret = WrapVoidToJS(env);
    } else {
        callbackPtr.release();
    }
    APP_LOGI("%{public}s end.", __func__);
    return ret;
}

bool UnwrapAbilityInfo(napi_env env, napi_value param, OHOS::AppExecFwk::AbilityInfo& abilityInfo)
{
    napi_status status;
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, param, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("param type mismatch!");
        return false;
    }

    napi_value prop = nullptr;
    // parse bundleName
    status = napi_get_named_property(env, param, "bundleName", &prop);
    napi_typeof(env, prop, &valueType);
    if (valueType == napi_undefined) {
        return false;
    }
    std::string bundleName = GetStringFromNAPI(env, prop);
    abilityInfo.bundleName = bundleName;

    // parse moduleName
    status = napi_get_named_property(env, param, "moduleName", &prop);
    napi_typeof(env, prop, &valueType);
    if (valueType == napi_string) {
        std::string moduleName = GetStringFromNAPI(env, prop);
        if (moduleName.empty()) {
            APP_LOGE("error moduleName is empty!");
            return false;
        }
        abilityInfo.moduleName = moduleName;
    }

    // parse abilityName
    status = napi_get_named_property(env, param, "name", &prop);
    napi_typeof(env, prop, &valueType);
    if (valueType == napi_undefined) {
        return false;
    }
    std::string name = GetStringFromNAPI(env, prop);
    abilityInfo.name = name;
    return true;
}

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
static std::shared_ptr<Media::PixelMap> InnerGetAbilityIcon(
    napi_env env, const std::string &bundleName, const std::string &moduleName, const std::string &abilityName,
    bool hasModuleName)
{
    if (bundleName.empty() || abilityName.empty()) {
        APP_LOGE("bundleName or abilityName is invalid param");
        return nullptr;
    }
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return nullptr;
    }
    if (hasModuleName) {
        return iBundleMgr->GetAbilityPixelMapIcon(bundleName, moduleName, abilityName);
    }
    return iBundleMgr->GetAbilityPixelMapIcon(bundleName, abilityName);
}

napi_value GetAbilityIcon(napi_env env, napi_callback_info info)
{
    size_t requireArgc = ARGS_SIZE_TWO;
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 2 parameter");

    AsyncAbilityInfo *asyncCallbackInfo = new (std::nothrow) AsyncAbilityInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncAbilityInfo> callbackPtr {asyncCallbackInfo};
    asyncCallbackInfo->hasModuleName = HasModuleName(env, argc, argv);
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == PARAM0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, argv[i]);
        } else if ((i == PARAM1) && (valueType == napi_string)) {
            if (asyncCallbackInfo->hasModuleName) {
                ParseString(env, asyncCallbackInfo->moduleName, argv[i]);
            } else {
                ParseString(env, asyncCallbackInfo->abilityName, argv[i]);
            }
        } else if ((i == PARAM2) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->abilityName, argv[i]);
        } else if (((i == PARAM2) || (i == PARAM3)) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->errCode = INVALID_PARAM;
            asyncCallbackInfo->errMssage = "type misMatch";
        }
    }
    napi_value promise = nullptr;

    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetAbilityIcon", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncAbilityInfo* asyncCallbackInfo = (AsyncAbilityInfo*)data;
            if (!asyncCallbackInfo->errCode) {
                asyncCallbackInfo->pixelMap = InnerGetAbilityIcon(asyncCallbackInfo->env,
                                                                  asyncCallbackInfo->bundleName,
                                                                  asyncCallbackInfo->moduleName,
                                                                  asyncCallbackInfo->abilityName,
                                                                  asyncCallbackInfo->hasModuleName);
                if (!asyncCallbackInfo->pixelMap) {
                    asyncCallbackInfo->errCode = OPERATION_FAILED;
                }
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncAbilityInfo* asyncCallbackInfo = (AsyncAbilityInfo*)data;
            std::unique_ptr<AsyncAbilityInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[2] = { 0 };
            if (asyncCallbackInfo->errCode) {
                NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->errCode, &result[0]));
            } else {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[1]));
                result[1] = Media::PixelMapNapi::CreatePixelMap(env, asyncCallbackInfo->pixelMap);
            }
            if (asyncCallbackInfo->callback) {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            } else {
                if (asyncCallbackInfo->errCode) {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[1]));
                }
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}
#endif

static bool InnerGetNameForUid(int32_t uid, std::string &bundleName)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    return iBundleMgr->GetNameForUid(uid, bundleName);
}

napi_value GetNameForUid(napi_env env, napi_callback_info info)
{
    size_t requireArgc = ARGS_SIZE_ONE;
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 1 parameter");
    AsyncGetNameByUidInfo *asyncCallbackInfo = new (std::nothrow) AsyncGetNameByUidInfo(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncGetNameByUidInfo> callbackPtr {asyncCallbackInfo};

    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_number)) {
            ParseInt(env, asyncCallbackInfo->uid, argv[i]);
        } else if ((i == 1) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->err = INVALID_PARAM;
        }
    }
    napi_value promise = nullptr;

    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "JSGetNameForUid", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncGetNameByUidInfo* asyncCallbackInfo = (AsyncGetNameByUidInfo*)data;
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret = InnerGetNameForUid(asyncCallbackInfo->uid, asyncCallbackInfo->bundleName);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncGetNameByUidInfo* asyncCallbackInfo = (AsyncGetNameByUidInfo*)data;
            std::unique_ptr<AsyncGetNameByUidInfo> callbackPtr {asyncCallbackInfo};
            napi_value result[ARGS_SIZE_TWO] = { 0 };
            // set error code
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->err, &result[PARAM0]));
            } else {
                if (!asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, OPERATION_FAILED, &result[PARAM0]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, CODE_SUCCESS, &result[PARAM0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, asyncCallbackInfo->bundleName.c_str(),
                        NAPI_AUTO_LENGTH, &result[PARAM1]));
                }
            }
            // implement callback or promise
            if (asyncCallbackInfo->deferred) {
                if (!asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[PARAM0]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[PARAM1]));
                }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[PARAM0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

napi_value ClearBundleCache(napi_env env, napi_callback_info info)
{
    size_t requireArgc = ARGS_SIZE_ONE;
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { 0 };
    napi_value thisArg = nullptr;
    void *data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    NAPI_ASSERT(env, argc >= requireArgc, "requires 1 parameter");

    AsyncHandleBundleContext *asyncCallbackInfo = new (std::nothrow) AsyncHandleBundleContext(env);
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncHandleBundleContext> callbackPtr {asyncCallbackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);
        if ((i == 0) && (valueType == napi_string)) {
            ParseString(env, asyncCallbackInfo->bundleName, argv[i]);
        } else if ((i == 1) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
        } else {
            asyncCallbackInfo->err = INVALID_PARAM;
        }
    }
    napi_value promise = nullptr;

    if (asyncCallbackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &asyncCallbackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "JSCleanBundleCache", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            if (asyncCallbackInfo->cleanCacheCallback == nullptr) {
                asyncCallbackInfo->cleanCacheCallback = new (std::nothrow) CleanCacheCallback(UNDEFINED_ERROR);
            }
            if (!asyncCallbackInfo->err) {
                asyncCallbackInfo->ret =
                    InnerCleanBundleCacheCallback(asyncCallbackInfo->bundleName, asyncCallbackInfo->cleanCacheCallback);
            }
        },
        [](napi_env env, napi_status status, void* data) {
            AsyncHandleBundleContext* asyncCallbackInfo = (AsyncHandleBundleContext*)data;
            std::unique_ptr<AsyncHandleBundleContext> callbackPtr {asyncCallbackInfo};
            napi_value result[1] = { 0 };
            // set error code
            if (asyncCallbackInfo->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, asyncCallbackInfo->err, &result[0]));
            } else {
                if (!asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, OPERATION_FAILED, &result[0]));
                } else {
                    if (asyncCallbackInfo->cleanCacheCallback) {
                        // wait for OnCleanCacheFinished
                        uv_sem_wait(&(asyncCallbackInfo->cleanCacheCallback->uvSem_));
                        asyncCallbackInfo->ret = asyncCallbackInfo->cleanCacheCallback->GetErr() ? false : true;
                        if (!asyncCallbackInfo->cleanCacheCallback->GetErr()) {
                            NAPI_CALL_RETURN_VOID(env, napi_get_undefined(env, &result[0]));
                        } else {
                            NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                asyncCallbackInfo->cleanCacheCallback->GetErr(), &result[0]));
                        }
                    }
                }
            }
            // implement callback or promise
            if (asyncCallbackInfo->deferred) {
                if (!asyncCallbackInfo->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[0]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[0]));
                }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)asyncCallbackInfo, &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

void CreateAbilityTypeObject(napi_env env, napi_value value)
{
    napi_value nUnknow;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(AbilityType::UNKNOWN), &nUnknow));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "UNKNOWN", nUnknow));
    napi_value nPage;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(AbilityType::PAGE), &nPage));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PAGE", nPage));
    napi_value nService;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(AbilityType::SERVICE), &nService));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SERVICE", nService));
    napi_value nData;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(AbilityType::DATA), &nData));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "DATA", nData));
}

void CreateAbilitySubTypeObject(napi_env env, napi_value value)
{
    napi_value nUnspecified;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nUnspecified));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "UNSPECIFIED", nUnspecified));
    napi_value nCa;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ONE, &nCa));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "CA", nCa));
}

void CreateDisplayOrientationObject(napi_env env, napi_value value)
{
    napi_value nUnspecified;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_int32(env, static_cast<int32_t>(DisplayOrientation::UNSPECIFIED), &nUnspecified));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "UNSPECIFIED", nUnspecified));
    napi_value nLandscape;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_int32(env, static_cast<int32_t>(DisplayOrientation::LANDSCAPE), &nLandscape));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "LANDSCAPE", nLandscape));
    napi_value nPortrait;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(DisplayOrientation::PORTRAIT), &nPortrait));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PORTRAIT", nPortrait));
    napi_value nFollowrecent;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_int32(env, static_cast<int32_t>(DisplayOrientation::FOLLOWRECENT), &nFollowrecent));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "FOLLOW_RECENT", nFollowrecent));
}

void CreateLaunchModeObject(napi_env env, napi_value value)
{
    napi_value nSingleton;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(LaunchMode::SINGLETON), &nSingleton));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SINGLETON", nSingleton));
    napi_value nSingletop;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(LaunchMode::SINGLETOP), &nSingletop));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SINGLETOP", nSingletop));
    napi_value nStandard;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(LaunchMode::STANDARD), &nStandard));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STANDARD", nStandard));
}

void CreateModuleUpdateFlagObject(napi_env env, napi_value value)
{
    napi_value nFlagModuleUpgradeCheck;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nFlagModuleUpgradeCheck));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "FLAG_MODULE_UPGRADE_CHECK", nFlagModuleUpgradeCheck));
    napi_value nFlagModuleUpgradeInstall;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ONE, &nFlagModuleUpgradeInstall));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "FLAG_MODULE_UPGRADE_INSTALL", nFlagModuleUpgradeInstall));
    napi_value nFlagModuleUpgradeinstallWithConfigWindows;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_TWO, &nFlagModuleUpgradeinstallWithConfigWindows));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(
            env, value, "FLAG_MODULE_UPGRADE_INSTALL_WITH_CONFIG_WINDOWS",
            nFlagModuleUpgradeinstallWithConfigWindows));
}

void CreateFormTypeObject(napi_env env, napi_value value)
{
    napi_value nJava;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nJava));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "JAVA", nJava));
    napi_value nJs;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ONE, &nJs));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "JS", nJs));
}

void CreateColorModeObject(napi_env env, napi_value value)
{
    napi_value nAutoMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_FAILED, &nAutoMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "AUTO_MODE", nAutoMode));
    napi_value nDarkMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nDarkMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "DARK_MODE", nDarkMode));
    napi_value nLightMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ONE, &nLightMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "LIGHT_MODE", nLightMode));
}

void CreateGrantStatusObject(napi_env env, napi_value value)
{
    napi_value nPermissionDenied;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_FAILED, &nPermissionDenied));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PERMISSION_DENIED", nPermissionDenied));
    napi_value nPermissionGranted;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nPermissionGranted));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PERMISSION_GRANTED", nPermissionGranted));
}

void CreateModuleRemoveFlagObject(napi_env env, napi_value value)
{
    napi_value nFlagModuleNotUsedByForm;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nFlagModuleNotUsedByForm));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "FLAG_MODULE_NOT_USED_BY_FORM", nFlagModuleNotUsedByForm));
    napi_value nFlagModuleUsedByForm;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ONE, &nFlagModuleUsedByForm));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "FLAG_MODULE_USED_BY_FORM", nFlagModuleUsedByForm));
    napi_value nFlagModuleNotUsedByShortcut;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_TWO, &nFlagModuleNotUsedByShortcut));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "FLAG_MODULE_NOT_USED_BY_SHORTCUT", nFlagModuleNotUsedByShortcut));
    napi_value nFlagModuleUsedByShortcut;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_THREE, &nFlagModuleUsedByShortcut));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "FLAG_MODULE_USED_BY_SHORTCUT", nFlagModuleUsedByShortcut));
}

void CreateSignatureCompareResultObject(napi_env env, napi_value value)
{
    napi_value nSignatureMatched;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nSignatureMatched));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SIGNATURE_MATCHED", nSignatureMatched));
    napi_value nSignatureNotMatched;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ONE, &nSignatureNotMatched));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SIGNATURE_NOT_MATCHED", nSignatureNotMatched));
    napi_value nSignatureUnknownBundle;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_TWO, &nSignatureUnknownBundle));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "SIGNATURE_UNKNOWN_BUNDLE", nSignatureUnknownBundle));
}

void CreateShortcutExistenceObject(napi_env env, napi_value value)
{
    napi_value nShortcutExistenceExists;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nShortcutExistenceExists));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "SHORTCUT_EXISTENCE_EXISTS", nShortcutExistenceExists));
    napi_value nShortcutExistenceNotExists;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ONE, &nShortcutExistenceNotExists));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "SHORTCUT_EXISTENCE_NOT_EXISTS", nShortcutExistenceNotExists));
    napi_value nShortcutExistenceUnknow;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_TWO, &nShortcutExistenceUnknow));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "SHORTCUT_EXISTENCE_UNKNOW", nShortcutExistenceUnknow));
}

void CreateQueryShortCutFlagObject(napi_env env, napi_value value)
{
    napi_value nQueryShortCutHome;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, NAPI_RETURN_ZERO, &nQueryShortCutHome));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "QUERY_SHORTCUT_HOME", nQueryShortCutHome));
}

void CreateBundleFlagObject(napi_env env, napi_value value)
{
    napi_value nGetAllApplicationInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(ApplicationFlag::GET_ALL_APPLICATION_INFO),
                                                 &nGetAllApplicationInfo));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "GET_ALL_APPLICATION_INFO", nGetAllApplicationInfo));

    napi_value nGetBundleDefault;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_int32(env, static_cast<int32_t>(BundleFlag::GET_BUNDLE_DEFAULT), &nGetBundleDefault));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_BUNDLE_DEFAULT", nGetBundleDefault));
    napi_value nGetBundleWithAbilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(BundleFlag::GET_BUNDLE_WITH_ABILITIES),
                                                 &nGetBundleWithAbilities));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_BUNDLE_WITH_ABILITIES", nGetBundleWithAbilities));
    napi_value nGetBundleWithRequestedPermission;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION),
                                                 &nGetBundleWithRequestedPermission));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(
            env, value, "GET_BUNDLE_WITH_REQUESTED_PERMISSION", nGetBundleWithRequestedPermission));

    napi_value nGetAbilityInfoWithPermission;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
                          static_cast<int32_t>(AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION),
                          &nGetAbilityInfoWithPermission));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_ABILITY_INFO_WITH_PERMISSION", nGetAbilityInfoWithPermission));
    napi_value nGetAbilityInfoWithApplication;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
                          static_cast<int32_t>(AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION),
                          &nGetAbilityInfoWithApplication));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_ABILITY_INFO_WITH_APPLICATION", nGetAbilityInfoWithApplication));
    napi_value nGetAbilityInfoSystemappOnly;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(AbilityInfoFlag::GET_ABILITY_INFO_SYSTEMAPP_ONLY),
                                                 &nGetAbilityInfoSystemappOnly));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_ABILITY_INFO_SYSTEMAPP_ONLY", nGetAbilityInfoSystemappOnly));
    napi_value nGetAbilityInfoWithMetadata;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA),
                                                 &nGetAbilityInfoWithMetadata));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_ABILITY_INFO_WITH_METADATA", nGetAbilityInfoWithMetadata));
    napi_value nGetBundleWithExtensionAbility;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO),
                                                 &nGetBundleWithExtensionAbility));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_BUNDLE_WITH_EXTENSION_ABILITY", nGetBundleWithExtensionAbility));
    napi_value nGetBundleWithHashValue;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(BundleFlag::GET_BUNDLE_WITH_HASH_VALUE),
                                                 &nGetBundleWithHashValue));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_BUNDLE_WITH_HASH_VALUE", nGetBundleWithHashValue));
    napi_value nGetAbilityInfoWithDisable;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
                                                 static_cast<int32_t>(AbilityInfoFlag::GET_ABILITY_INFO_WITH_DISABLE),
                                                 &nGetAbilityInfoWithDisable));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "GET_ABILITY_INFO_WITH_DISABLE", nGetAbilityInfoWithDisable));

    napi_value nGetApplicationInfoWithPermission;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION),
            &nGetApplicationInfoWithPermission));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env,
                                                       value,
                                                       "GET_APPLICATION_INFO_WITH_PERMISSION",
                                                       nGetApplicationInfoWithPermission));
    napi_value nGetApplicationInfoWithMetadata;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ApplicationFlag::GET_APPLICATION_INFO_WITH_METADATA),
            &nGetApplicationInfoWithMetadata));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, value, "GET_APPLICATION_INFO_WITH_METADATA", nGetApplicationInfoWithMetadata));
    napi_value nGetApplicationInfoWithDisable;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ApplicationFlag::GET_APPLICATION_INFO_WITH_DISABLE),
            &nGetApplicationInfoWithDisable));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, value, "GET_APPLICATION_INFO_WITH_DISABLE", nGetApplicationInfoWithDisable));
}

void CreateExtensionFlagObject(napi_env env, napi_value value)
{
    napi_value nGetExtensionInfoDefault;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityInfoFlag::GET_EXTENSION_INFO_DEFAULT),
            &nGetExtensionInfoDefault));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, value, "GET_EXTENSION_INFO_DEFAULT", nGetExtensionInfoDefault));

    napi_value nGetExtensionInfoWithPermission;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityInfoFlag::GET_EXTENSION_INFO_WITH_PERMISSION),
            &nGetExtensionInfoWithPermission));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, value, "GET_EXTENSION_INFO_WITH_PERMISSION", nGetExtensionInfoWithPermission));

    napi_value nGetExtensionInfoWithApplication;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityInfoFlag::GET_EXTENSION_INFO_WITH_APPLICATION),
            &nGetExtensionInfoWithApplication));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, value, "GET_EXTENSION_INFO_WITH_APPLICATION", nGetExtensionInfoWithApplication));

    napi_value nGetExtensionInfoWithMetaData;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityInfoFlag::GET_EXTENSION_INFO_WITH_METADATA),
            &nGetExtensionInfoWithMetaData));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, value, "GET_EXTENSION_INFO_WITH_METADATA", nGetExtensionInfoWithMetaData));
}

void CreateInstallErrorCodeObject(napi_env env, napi_value value)
{
    napi_value nSuccess;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(InstallErrorCode::SUCCESS), &nSuccess));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SUCCESS", nSuccess));
    napi_value nStatusInstallFailure;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(
            env, static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE), &nStatusInstallFailure));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATUS_INSTALL_FAILURE", nStatusInstallFailure));
    napi_value nStatusInstallFailureAborted;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_ABORTED),
            &nStatusInstallFailureAborted));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_INSTALL_FAILURE_ABORTED", nStatusInstallFailureAborted));
    napi_value nStatusInstallFailureInvalid;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_INVALID),
            &nStatusInstallFailureInvalid));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_INSTALL_FAILURE_INVALID", nStatusInstallFailureInvalid));
    napi_value nStatusInstallFailureConflict;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_CONFLICT),
            &nStatusInstallFailureConflict));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_INSTALL_FAILURE_CONFLICT", nStatusInstallFailureConflict));
    napi_value nStatusInstallFailureStorage;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_STORAGE),
            &nStatusInstallFailureStorage));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_INSTALL_FAILURE_STORAGE", nStatusInstallFailureStorage));
    napi_value nStatusInstallFailureIncompatible;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_INCOMPATIBLE),
            &nStatusInstallFailureIncompatible));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, value, "STATUS_INSTALL_FAILURE_INCOMPATIBLE", nStatusInstallFailureIncompatible));
    napi_value nStatusUninstallFailure;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(
            env, static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_FAILURE), &nStatusUninstallFailure));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_UNINSTALL_FAILURE", nStatusUninstallFailure));
    napi_value nStatusUninstallFailureBlocked;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_FAILURE_BLOCKED),
            &nStatusUninstallFailureBlocked));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_UNINSTALL_FAILURE_BLOCKED", nStatusUninstallFailureBlocked));
    napi_value nStatusUninstallFailureAborted;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_FAILURE_ABORTED),
            &nStatusUninstallFailureAborted));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_UNINSTALL_FAILURE_ABORTED", nStatusUninstallFailureAborted));
    napi_value nStatusUninstallFailureConflict;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_FAILURE_CONFLICT),
            &nStatusUninstallFailureConflict));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(
            env, value, "STATUS_UNINSTALL_FAILURE_CONFLICT", nStatusUninstallFailureConflict));
    napi_value nStatusInstallFailureDownloadTimeout;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_DOWNLOAD_TIMEOUT),
            &nStatusInstallFailureDownloadTimeout));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(
            env, value, "STATUS_INSTALL_FAILURE_DOWNLOAD_TIMEOUT", nStatusInstallFailureDownloadTimeout));
    napi_value nStatusInstallFailureDownloadFailed;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env,
            static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_FAILURE_DOWNLOAD_FAILED),
            &nStatusInstallFailureDownloadFailed));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(
            env, value, "STATUS_INSTALL_FAILURE_DOWNLOAD_FAILED", nStatusInstallFailureDownloadFailed));
    napi_value nStatusAbilityNotFound;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(
            env, static_cast<int32_t>(InstallErrorCode::STATUS_ABILITY_NOT_FOUND), &nStatusAbilityNotFound));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, value, "STATUS_ABILITY_NOT_FOUND", nStatusAbilityNotFound));
    napi_value nBmsServiceError;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(InstallErrorCode::STATUS_BMS_SERVICE_ERROR), &nBmsServiceError));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATUS_BMS_SERVICE_ERROR", nBmsServiceError));
    napi_value nStatusGrantRequestPermissionsFailed;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(InstallErrorCode::STATUS_GRANT_REQUEST_PERMISSIONS_FAILED),
                          &nStatusGrantRequestPermissionsFailed));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATUS_GRANT_REQUEST_PERMISSIONS_FAILED",
                          nStatusGrantRequestPermissionsFailed));
    napi_value nStatusInstallPermissionDenied;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(InstallErrorCode::STATUS_INSTALL_PERMISSION_DENIED),
                          &nStatusInstallPermissionDenied));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATUS_INSTALL_PERMISSION_DENIED",
                          nStatusInstallPermissionDenied));
    napi_value nStatusUnInstallPermissionDenied;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(InstallErrorCode::STATUS_UNINSTALL_PERMISSION_DENIED),
                          &nStatusUnInstallPermissionDenied));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATUS_UNINSTALL_PERMISSION_DENIED",
                          nStatusUnInstallPermissionDenied));
    napi_value nNoSpaceLeft;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(InstallErrorCode::STATUS_FAILED_NO_SPACE_LEFT), &nNoSpaceLeft));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATUS_FAILED_NO_SPACE_LEFT", nNoSpaceLeft));
}

static bool ParseWant(napi_env env, AsyncExtensionInfoCallbackInfo &info, napi_value args)
{
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, args, &valueType));
    if (valueType != napi_object) {
        APP_LOGE("args not object type");
        return false;
    }

    napi_value prop = nullptr;
    bool ret = false;
    napi_has_named_property(env, args, "extensionAbilityName", &ret);
    if (!ret) {
        APP_LOGW("cannot find the property of extensionAbilityName");
    } else {
        napi_get_named_property(env, args, "extensionAbilityName", &prop);
        napi_typeof(env, prop, &valueType);
        if (valueType != napi_string) {
            APP_LOGE("extensionAbilityName is not type of string");
            return false;
        }
        info.extensionAbilityName = GetStringFromNAPI(env, prop);
        APP_LOGD("extensionAbilityInfo name is %{public}s", info.extensionAbilityName.c_str());
    }

    prop = nullptr;
    ret = false;
    napi_has_named_property(env, args, "extensionAbilityType", &ret);
    if (!ret) {
        APP_LOGW("cannot find the property of extensionAbilityType");
    } else {
        napi_get_named_property(env, args, "extensionAbilityType", &prop);
        napi_typeof(env, prop, &valueType);
        if (valueType != napi_number) {
            APP_LOGE("extensionAbilityType is not type of number");
            return false;
        }
        napi_get_value_int32(env, prop, &(info.extensionAbilityType));
        APP_LOGD("extensionAbilityInfo type is %{public}d", info.extensionAbilityType);
    }

    if (!ParseWant(env, info.want, args)) {
        APP_LOGE("parse want failed");
        return false;
    }
    ElementName element = info.want.GetElement();
    ElementName newElement(element.GetDeviceID(), element.GetBundleName(), info.extensionAbilityName,
        element.GetModuleName());
    info.want.SetElement(newElement);
    return true;
}

static bool InnerQueryExtensionInfo(napi_env env, AsyncExtensionInfoCallbackInfo &info)
{
    auto iBundleMgr = GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return false;
    }
    APP_LOGD("action:%{public}s, uri:%{private}s, type:%{public}s, flags:%{public}d",
        info.want.GetAction().c_str(), info.want.GetUriString().c_str(), info.want.GetType().c_str(), info.flags);

    if (info.extensionAbilityType < 0) {
        APP_LOGD("query extensionAbilityInfo without type");
        return iBundleMgr->QueryExtensionAbilityInfos(info.want, info.flags, info.userId, info.extensionInfos);
    } else {
        auto type = static_cast<ExtensionAbilityType>(info.extensionAbilityType);
        APP_LOGD("query extensionAbilityInfo with type");
        return iBundleMgr->QueryExtensionAbilityInfos(info.want, type, info.flags, info.userId, info.extensionInfos);
    }
}

/**
 * Promise and async callback
 */
napi_value QueryExtensionInfoByWant(napi_env env, napi_callback_info info)
{
    APP_LOGD("QueryExtensionInfoByWant start in NAPI");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = { nullptr };

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    APP_LOGD("the count of input arguments is [%{public}zu]", argc);

    AsyncExtensionInfoCallbackInfo *callBackInfo = new (std::nothrow) AsyncExtensionInfoCallbackInfo(env);
    if (callBackInfo == nullptr) {
        APP_LOGE("QueryExtensionInfoByWant failed due to null callBackInfo");
        return nullptr;
    }

    if (argc < ARGS_SIZE_TWO || argc > ARGS_SIZE_FOUR) {
        APP_LOGE("the number of input arguments is invalid");
        delete callBackInfo;
        callBackInfo = nullptr;
        return nullptr;
    }
    std::unique_ptr<AsyncExtensionInfoCallbackInfo> callbackPtr {callBackInfo};
    for (size_t i = 0; i < argc; ++i) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valueType));
        if ((i == 0) && (valueType == napi_object)) {
            if (!ParseWant(env, *callBackInfo, argv[i])) {
                callBackInfo->err = PARAM_TYPE_ERROR;
            }
        } else if ((i == ARGS_SIZE_ONE) && (valueType == napi_number)) {
            ParseInt(env, callBackInfo->flags, argv[i]);
        } else if (i == ARGS_SIZE_TWO) {
            if (valueType == napi_number) {
                ParseInt(env, callBackInfo->userId, argv[i]);
            } else if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &callBackInfo->callback));
                break;
            } else {
                callBackInfo->err = PARAM_TYPE_ERROR;
            }
        } else if ((i == ARGS_SIZE_THREE) && (valueType == napi_function)) {
            NAPI_CALL(env, napi_create_reference(env, argv[i], NAPI_RETURN_ONE, &callBackInfo->callback));
        } else {
            callBackInfo->err = PARAM_TYPE_ERROR;
        }
    }

    APP_LOGD("QueryExtensionInfoByWant finish to parse arguments");
    napi_value promise = nullptr;
    if (callBackInfo->callback == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &callBackInfo->deferred, &promise));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &promise));
    }
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "QueryExtensionInfoByWant", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resource,
        [](napi_env env, void *data) {
            AsyncExtensionInfoCallbackInfo *info = (AsyncExtensionInfoCallbackInfo *)data;
            if (!info->err) {
                info->ret = InnerQueryExtensionInfo(env, *info);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncExtensionInfoCallbackInfo *info = (AsyncExtensionInfoCallbackInfo *)data;
            std::unique_ptr<AsyncExtensionInfoCallbackInfo> callbackPtr {info};
            napi_value result[2] = { 0 };
            if (info->err) {
                NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(info->err), &result[0]));
                NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "type mismatch", NAPI_AUTO_LENGTH, &result[1]));
            } else {
                if (info->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 0, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_array_with_length(env, info->extensionInfos.size(),
                        &result[1]));
                    ConvertExtensionInfos(env, result[1], info->extensionInfos);
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, 1, &result[0]));
                    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, "QueryExtensionInfoByWant failed",
                        NAPI_AUTO_LENGTH, &result[1]));
                }
            }
            if (info->deferred) {
                if (info->ret) {
                    NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, info->deferred, result[1]));
                } else {
                    NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, info->deferred, result[0]));
                }
            } else {
                napi_value callback = nullptr;
                napi_value placeHolder = nullptr;
                NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, info->callback, &callback));
                NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
                    sizeof(result) / sizeof(result[0]), result, &placeHolder));
            }
        },
        (void*)callBackInfo, &callBackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callBackInfo->asyncWork));
    callbackPtr.release();
    return promise;
}

void CreateExtensionAbilityTypeObject(napi_env env, napi_value value)
{
    napi_value nForm;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::FORM), &nForm));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "FORM", nForm));

    napi_value nWorkSchedule;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::WORK_SCHEDULER), &nWorkSchedule));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WORK_SCHEDULER", nWorkSchedule));

    napi_value nInputMethod;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::INPUTMETHOD), &nInputMethod));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "INPUT_METHOD", nInputMethod));

    napi_value nService;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::SERVICE), &nService));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SERVICE", nService));

    napi_value nAccessibility;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::ACCESSIBILITY), &nAccessibility));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "ACCESSIBILITY", nAccessibility));

    napi_value nDataShare;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::DATASHARE), &nDataShare));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "DATA_SHARE", nDataShare));

    napi_value nFileShare;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::FILESHARE), &nFileShare));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "FILE_SHARE", nFileShare));

    napi_value nStaticSubscriber;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::STATICSUBSCRIBER), &nStaticSubscriber));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATIC_SUBSCRIBER", nStaticSubscriber));

    napi_value nWallpaper;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
        static_cast<int32_t>(ExtensionAbilityType::WALLPAPER), &nWallpaper));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WALLPAPER", nWallpaper));

    napi_value nBackup;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
        static_cast<int32_t>(ExtensionAbilityType::BACKUP), &nBackup));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "BACKUP", nBackup));

    napi_value nWindow;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
        static_cast<int32_t>(ExtensionAbilityType::WINDOW), &nWindow));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WINDOW", nWindow));

    napi_value nUnspecified;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ExtensionAbilityType::UNSPECIFIED), &nUnspecified));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "UNSPECIFIED", nUnspecified));
}
}  // namespace AppExecFwk
}  // namespace OHOS
