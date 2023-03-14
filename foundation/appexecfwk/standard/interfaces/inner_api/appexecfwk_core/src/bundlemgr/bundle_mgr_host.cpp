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

#include "bundle_mgr_host.h"

#include <cinttypes>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "hitrace_meter.h"
#include "datetime_ex.h"
#include "ipc_types.h"
#include "json_util.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t LIMIT_PARCEL_SIZE = 1024;
const int32_t ASHMEM_LEN = 16;

void SplitString(const std::string &source, std::vector<std::string> &strings)
{
    int splitSize = (source.size() / LIMIT_PARCEL_SIZE);
    if ((source.size() % LIMIT_PARCEL_SIZE) != 0) {
        splitSize++;
    }
    APP_LOGD("the dump string split into %{public}d size", splitSize);
    for (int i = 0; i < splitSize; i++) {
        int32_t start = LIMIT_PARCEL_SIZE * i;
        strings.emplace_back(source.substr(start, LIMIT_PARCEL_SIZE));
    }
}

inline void ClearAshmem(sptr<Ashmem> &optMem)
{
    if (optMem != nullptr) {
        optMem->UnmapAshmem();
        optMem->CloseAshmem();
    }
}
}  // namespace

int BundleMgrHost::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGD("bundle mgr host onReceived message, the message code is %{public}u", code);
    std::u16string descripter = BundleMgrHost::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("fail to write reply message in bundle mgr host due to the reply is nullptr");
        return OBJECT_NULL;
    }

    ErrCode errCode = ERR_OK;
    switch (code) {
        case IBundleMgr::Message::GET_APPLICATION_INFO:
            errCode = HandleGetApplicationInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_APPLICATION_INFO_WITH_INT_FLAGS:
            errCode = HandleGetApplicationInfoWithIntFlags(data, reply);
            break;
        case IBundleMgr::Message::GET_APPLICATION_INFOS:
            errCode = HandleGetApplicationInfos(data, reply);
            break;
        case IBundleMgr::Message::GET_APPLICATION_INFOS_WITH_INT_FLAGS:
            errCode = HandleGetApplicationInfosWithIntFlags(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_INFO:
            errCode = HandleGetBundleInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_INFO_WITH_INT_FLAGS:
            errCode = HandleGetBundleInfoWithIntFlags(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_PACK_INFO:
            errCode = HandleGetBundlePackInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_PACK_INFO_WITH_INT_FLAGS:
            errCode = HandleGetBundlePackInfoWithIntFlags(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_INFOS:
            errCode = HandleGetBundleInfos(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_INFOS_WITH_INT_FLAGS:
            errCode = HandleGetBundleInfosWithIntFlags(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_NAME_FOR_UID:
            errCode = HandleGetBundleNameForUid(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLES_FOR_UID:
            errCode = HandleGetBundlesForUid(data, reply);
            break;
        case IBundleMgr::Message::GET_NAME_FOR_UID:
            errCode = HandleGetNameForUid(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_GIDS:
            errCode = HandleGetBundleGids(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_GIDS_BY_UID:
            errCode = HandleGetBundleGidsByUid(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_INFOS_BY_METADATA:
            errCode = HandleGetBundleInfosByMetaData(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFO:
            errCode = HandleQueryAbilityInfo(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFO_MUTI_PARAM:
            errCode = HandleQueryAbilityInfoMutiparam(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFOS:
            errCode = HandleQueryAbilityInfos(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFOS_MUTI_PARAM:
            errCode = HandleQueryAbilityInfosMutiparam(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFOS_FOR_CLONE:
            errCode = HandleQueryAbilityInfosForClone(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ALL_ABILITY_INFOS:
            errCode = HandleQueryAllAbilityInfos(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFO_BY_URI:
            errCode = HandleQueryAbilityInfoByUri(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFOS_BY_URI:
            errCode = HandleQueryAbilityInfosByUri(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFO_BY_URI_FOR_USERID:
            errCode = HandleQueryAbilityInfoByUriForUserId(data, reply);
            break;
        case IBundleMgr::Message::QUERY_KEEPALIVE_BUNDLE_INFOS:
            errCode = HandleQueryKeepAliveBundleInfos(data, reply);
            break;
        case IBundleMgr::Message::GET_ABILITY_LABEL:
            errCode = HandleGetAbilityLabel(data, reply);
            break;
        case IBundleMgr::Message::GET_ABILITY_LABEL_WITH_MODULE_NAME:
            errCode = HandleGetAbilityLabelWithModuleName(data, reply);
            break;
        case IBundleMgr::Message::CHECK_IS_SYSTEM_APP_BY_UID:
            errCode = HandleCheckIsSystemAppByUid(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_ARCHIVE_INFO:
            errCode = HandleGetBundleArchiveInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_ARCHIVE_INFO_WITH_INT_FLAGS:
            errCode = HandleGetBundleArchiveInfoWithIntFlags(data, reply);
            break;
        case IBundleMgr::Message::GET_HAP_MODULE_INFO:
            errCode = HandleGetHapModuleInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_LAUNCH_WANT_FOR_BUNDLE:
            errCode = HandleGetLaunchWantForBundle(data, reply);
            break;
        case IBundleMgr::Message::CHECK_PUBLICKEYS:
            errCode = HandleCheckPublicKeys(data, reply);
            break;
        case IBundleMgr::Message::GET_PERMISSION_DEF:
            errCode = HandleGetPermissionDef(data, reply);
            break;
        case IBundleMgr::Message::HAS_SYSTEM_CAPABILITY:
            errCode = HandleHasSystemCapability(data, reply);
            break;
        case IBundleMgr::Message::GET_SYSTEM_AVAILABLE_CAPABILITIES:
            errCode = HandleGetSystemAvailableCapabilities(data, reply);
            break;
        case IBundleMgr::Message::IS_SAFE_MODE:
            errCode = HandleIsSafeMode(data, reply);
            break;
        case IBundleMgr::Message::CLEAN_BUNDLE_CACHE_FILES:
            errCode = HandleCleanBundleCacheFiles(data, reply);
            break;
        case IBundleMgr::Message::CLEAN_BUNDLE_DATA_FILES:
            errCode = HandleCleanBundleDataFiles(data, reply);
            break;
        case IBundleMgr::Message::REGISTER_BUNDLE_STATUS_CALLBACK:
            errCode = HandleRegisterBundleStatusCallback(data, reply);
            break;
        case IBundleMgr::Message::CLEAR_BUNDLE_STATUS_CALLBACK:
            errCode = HandleClearBundleStatusCallback(data, reply);
            break;
        case IBundleMgr::Message::UNREGISTER_BUNDLE_STATUS_CALLBACK:
            errCode = HandleUnregisterBundleStatusCallback(data, reply);
            break;
        case IBundleMgr::Message::IS_APPLICATION_ENABLED:
            errCode = HandleIsApplicationEnabled(data, reply);
            break;
        case IBundleMgr::Message::SET_APPLICATION_ENABLED:
            errCode = HandleSetApplicationEnabled(data, reply);
            break;
        case IBundleMgr::Message::IS_ABILITY_ENABLED:
            errCode = HandleIsAbilityEnabled(data, reply);
            break;
        case IBundleMgr::Message::SET_ABILITY_ENABLED:
            errCode = HandleSetAbilityEnabled(data, reply);
            break;
        case IBundleMgr::Message::GET_ABILITY_INFO:
            errCode = HandleGetAbilityInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_ABILITY_INFO_WITH_MODULE_NAME:
            errCode = HandleGetAbilityInfoWithModuleName(data, reply);
            break;
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
        case IBundleMgr::Message::GET_ABILITY_PIXELMAP_ICON:
            errCode = HandleGetAbilityPixelMapIcon(data, reply);
            break;
        case IBundleMgr::Message::GET_ABILITY_PIXELMAP_ICON_WITH_MODULE_NAME:
            errCode = HandleGetAbilityPixelMapIconWithModuleName(data, reply);
            break;
#endif
        case IBundleMgr::Message::DUMP_INFOS:
            errCode = HandleDumpInfos(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_INSTALLER:
            errCode = HandleGetBundleInstaller(data, reply);
            break;
        case IBundleMgr::Message::GET_ALL_FORMS_INFO:
            errCode = HandleGetAllFormsInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_FORMS_INFO_BY_APP:
            errCode = HandleGetFormsInfoByApp(data, reply);
            break;
        case IBundleMgr::Message::GET_FORMS_INFO_BY_MODULE:
            errCode = HandleGetFormsInfoByModule(data, reply);
            break;
        case IBundleMgr::Message::GET_SHORTCUT_INFO:
            errCode = HandleGetShortcutInfos(data, reply);
            break;
        case IBundleMgr::Message::GET_ALL_COMMON_EVENT_INFO:
            errCode = HandleGetAllCommonEventInfo(data, reply);
            break;
        case IBundleMgr::Message::CHECK_BUNDLE_NAME_IN_ALLOWLIST:
            errCode = HandleCheckBundleNameInAllowList(data, reply);
            break;
        case IBundleMgr::Message::BUNDLE_CLONE:
            errCode = HandleBundleClone(data, reply);
            break;
        case IBundleMgr::Message::REMOVE_CLONED_BUNDLE:
            errCode = HandleRemoveClonedBundle(data, reply);
            break;
        case IBundleMgr::Message::GET_BUNDLE_USER_MGR:
            errCode = HandleGetBundleUserMgr(data, reply);
            break;
        case IBundleMgr::Message::GET_DISTRIBUTE_BUNDLE_INFO:
            errCode = HandleGetDistributedBundleInfo(data, reply);
            break;
        case IBundleMgr::Message::GET_APPLICATION_PRIVILEGE_LEVEL:
            errCode = HandleGetAppPrivilegeLevel(data, reply);
            break;
        case IBundleMgr::Message::QUERY_EXTENSION_INFO_WITHOUT_TYPE:
            errCode = HandleQueryExtAbilityInfosWithoutType(data, reply);
            break;
        case IBundleMgr::Message::QUERY_EXTENSION_INFO:
            errCode = HandleQueryExtAbilityInfos(data, reply);
            break;
        case IBundleMgr::Message::QUERY_EXTENSION_INFO_BY_TYPE:
            errCode = HandleQueryExtAbilityInfosByType(data, reply);
            break;
        case IBundleMgr::Message::VERIFY_CALLING_PERMISSION:
            errCode = HandleVerifyCallingPermission(data, reply);
            break;
        case IBundleMgr::Message::GET_ACCESSIBLE_APP_CODE_PATH:
            errCode = HandleGetAccessibleAppCodePaths(data, reply);
            break;
        case IBundleMgr::Message::QUERY_EXTENSION_ABILITY_INFO_BY_URI:
            errCode = HandleQueryExtensionAbilityInfoByUri(data, reply);
            break;
        case IBundleMgr::Message::GET_APPID_BY_BUNDLE_NAME:
            errCode = HandleGetAppIdByBundleName(data, reply);
            break;
        case IBundleMgr::Message::GET_APP_TYPE:
            errCode = HandleGetAppType(data, reply);
            break;
        case IBundleMgr::Message::GET_UID_BY_BUNDLE_NAME:
            errCode = HandleGetUidByBundleName(data, reply);
            break;
        case IBundleMgr::Message::IS_MODULE_REMOVABLE:
            errCode = HandleIsModuleRemovable(data, reply);
            break;
        case IBundleMgr::Message::SET_MODULE_REMOVABLE:
            errCode = HandleSetModuleRemovable(data, reply);
            break;
        case IBundleMgr::Message::QUERY_ABILITY_INFO_WITH_CALLBACK:
            errCode = HandleQueryAbilityInfoWithCallback(data, reply);
            break;
        case IBundleMgr::Message::UPGRADE_ATOMIC_SERVICE:
            errCode = HandleUpgradeAtomicService(data, reply);
            break;
        case IBundleMgr::Message::IS_MODULE_NEED_UPDATE:
            errCode = HandleGetModuleUpgradeFlag(data, reply);
            break;
        case IBundleMgr::Message::SET_MODULE_NEED_UPDATE:
            errCode = HandleSetModuleUpgradeFlag(data, reply);
            break;
        case IBundleMgr::Message::GET_HAP_MODULE_INFO_WITH_USERID:
            errCode = HandleGetHapModuleInfoWithUserId(data, reply);
            break;
        case IBundleMgr::Message::IMPLICIT_QUERY_INFO_BY_PRIORITY:
            errCode = HandleImplicitQueryInfoByPriority(data, reply);
            break;
        case IBundleMgr::Message::GET_ALL_DEPENDENT_MODULE_NAMES:
            errCode = HandleGetAllDependentModuleNames(data, reply);
            break;
        case IBundleMgr::Message::GET_SANDBOX_APP_BUNDLE_INFO:
            errCode = HandleGetSandboxBundleInfo(data, reply);
            break;
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    // if ERR_OK return ipc NO_ERROR, else return ipc UNKNOW_ERROR
    return (errCode == ERR_OK) ? NO_ERROR : UNKNOWN_ERROR;
}

ErrCode BundleMgrHost::HandleGetApplicationInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();
    ApplicationFlag flag = static_cast<ApplicationFlag>(data.ReadInt32());
    int userId = data.ReadInt32();
    APP_LOGI("name %{public}s, flag %{public}d, userId %{public}d", name.c_str(), flag, userId);

    ApplicationInfo info;
    bool ret = GetApplicationInfo(name, flag, userId, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetApplicationInfoWithIntFlags(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();
    int flags = data.ReadInt32();
    int userId = data.ReadInt32();
    APP_LOGD("name %{public}s, flags %{public}d, userId %{public}d", name.c_str(), flags, userId);

    ApplicationInfo info;
    bool ret = GetApplicationInfo(name, flags, userId, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetApplicationInfos(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    ApplicationFlag flag = static_cast<ApplicationFlag>(data.ReadInt32());
    int userId = data.ReadInt32();
    std::vector<ApplicationInfo> infos;
    bool ret = GetApplicationInfos(flag, userId, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetApplicationInfosWithIntFlags(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    int flags = data.ReadInt32();
    int userId = data.ReadInt32();
    std::vector<ApplicationInfo> infos;
    bool ret = GetApplicationInfos(flags, userId, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();
    BundleFlag flag = static_cast<BundleFlag>(data.ReadInt32());
    int userId = data.ReadInt32();
    APP_LOGI("name %{public}s, flag %{public}d", name.c_str(), flag);
    BundleInfo info;
    bool ret = GetBundleInfo(name, flag, info, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfoWithIntFlags(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();
    int flags = data.ReadInt32();
    int userId = data.ReadInt32();
    APP_LOGD("name %{public}s, flags %{public}d", name.c_str(), flags);
    BundleInfo info;
    bool ret = GetBundleInfo(name, flags, info, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundlePackInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();
    BundlePackFlag flag = static_cast<BundlePackFlag>(data.ReadInt32());
    APP_LOGD("name %{public}s, flag %{public}d", name.c_str(), flag);
    BundlePackInfo info;
    bool ret = GetBundlePackInfo(name, flag, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundlePackInfoWithIntFlags(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();
    int flags = data.ReadInt32();
    APP_LOGD("name %{public}s, flags %{public}d", name.c_str(), flags);
    BundlePackInfo info;
    bool ret = GetBundlePackInfo(name, flags, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfos(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    BundleFlag flag = static_cast<BundleFlag>(data.ReadInt32());
    int userId = data.ReadInt32();

    std::vector<BundleInfo> infos;
    reply.SetDataCapacity(Constants::MAX_CAPACITY_BUNDLES);
    bool ret = GetBundleInfos(flag, infos, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVectorIntoAshmem(infos, __func__, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfosWithIntFlags(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    int flags = data.ReadInt32();
    int userId = data.ReadInt32();

    std::vector<BundleInfo> infos;
    reply.SetDataCapacity(Constants::MAX_CAPACITY_BUNDLES);
    bool ret = GetBundleInfos(flags, infos, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVectorIntoAshmem(infos, __func__, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleNameForUid(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    int uid = data.ReadInt32();
    std::string name;
    bool ret = GetBundleNameForUid(uid, name);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteString(name)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundlesForUid(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    int uid = data.ReadInt32();
    std::vector<std::string> names;
    bool ret = GetBundlesForUid(uid, names);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteStringVector(names)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetNameForUid(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    int uid = data.ReadInt32();
    std::string name;
    bool ret = GetNameForUid(uid, name);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteString(name)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleGids(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();

    std::vector<int> gids;
    bool ret = GetBundleGids(name, gids);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteInt32Vector(gids)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleGidsByUid(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string name = data.ReadString();
    int uid = data.ReadInt32();

    std::vector<int> gids;
    bool ret = GetBundleGidsByUid(name, uid, gids);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteInt32Vector(gids)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleInfosByMetaData(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string metaData = data.ReadString();

    std::vector<BundleInfo> infos;
    bool ret = GetBundleInfosByMetaData(metaData, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    AbilityInfo info;
    bool ret = QueryAbilityInfo(*want, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfoMutiparam(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t flags = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    AbilityInfo info;
    bool ret = QueryAbilityInfo(*want, flags, userId, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfos(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::vector<AbilityInfo> abilityInfos;
    bool ret = QueryAbilityInfos(*want, abilityInfos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(abilityInfos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfosMutiparam(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t flags = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    std::vector<AbilityInfo> abilityInfos;
    bool ret = QueryAbilityInfos(*want, flags, userId, abilityInfos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(abilityInfos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAllAbilityInfos(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t userId = data.ReadInt32();
    std::vector<AbilityInfo> abilityInfos;
    bool ret = QueryAllAbilityInfos(*want, userId, abilityInfos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(abilityInfos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfosForClone(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    std::vector<AbilityInfo> abilityInfos;
    bool ret = QueryAbilityInfosForClone(*want, abilityInfos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(abilityInfos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfoByUri(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string abilityUri = data.ReadString();
    AbilityInfo info;
    bool ret = QueryAbilityInfoByUri(abilityUri, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfosByUri(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string abilityUri = data.ReadString();
    std::vector<AbilityInfo> abilityInfos;
    bool ret = QueryAbilityInfosByUri(abilityUri, abilityInfos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(abilityInfos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfoByUriForUserId(Parcel &data, Parcel &reply)
{
    std::string abilityUri = data.ReadString();
    int32_t userId = data.ReadInt32();
    AbilityInfo info;
    bool ret = QueryAbilityInfoByUri(abilityUri, userId, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryKeepAliveBundleInfos(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::vector<BundleInfo> infos;
    bool ret = QueryKeepAliveBundleInfos(infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAbilityLabel(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string abilityName = data.ReadString();

    APP_LOGI("bundleName %{public}s, abilityName %{public}s", bundleName.c_str(), abilityName.c_str());
    BundleInfo info;
    std::string label = GetAbilityLabel(bundleName, abilityName);
    if (!reply.WriteString(label)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAbilityLabelWithModuleName(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();
    std::string abilityName = data.ReadString();

    APP_LOGI("bundleName %{public}s, moduleName %{public}s, abilityName %{public}s",
        bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
    BundleInfo info;
    std::string label = GetAbilityLabel(bundleName, moduleName, abilityName);
    if (!reply.WriteString(label)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCheckIsSystemAppByUid(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    int uid = data.ReadInt32();
    bool ret = CheckIsSystemAppByUid(uid);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleArchiveInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string hapFilePath = data.ReadString();
    BundleFlag flag = static_cast<BundleFlag>(data.ReadInt32());
    APP_LOGD("hapFilePath %{private}s, flag %{public}d", hapFilePath.c_str(), flag);

    BundleInfo info;
    bool ret = GetBundleArchiveInfo(hapFilePath, flag, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleArchiveInfoWithIntFlags(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string hapFilePath = data.ReadString();
    int32_t flags = data.ReadInt32();
    APP_LOGD("hapFilePath %{private}s, flagS %{public}d", hapFilePath.c_str(), flags);

    BundleInfo info;
    bool ret = GetBundleArchiveInfo(hapFilePath, flags, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetHapModuleInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<AbilityInfo> abilityInfo(data.ReadParcelable<AbilityInfo>());
    if (!abilityInfo) {
        APP_LOGE("ReadParcelable<abilityInfo> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    HapModuleInfo info;
    bool ret = GetHapModuleInfo(*abilityInfo, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetHapModuleInfoWithUserId(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<AbilityInfo> abilityInfo(data.ReadParcelable<AbilityInfo>());
    if (!abilityInfo) {
        APP_LOGE("ReadParcelable<abilityInfo> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t userId = data.ReadInt32();
    HapModuleInfo info;
    bool ret = GetHapModuleInfo(*abilityInfo, userId, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetLaunchWantForBundle(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    APP_LOGI("name %{public}s", bundleName.c_str());

    Want want;
    bool ret = GetLaunchWantForBundle(bundleName, want);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&want)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCheckPublicKeys(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string firstBundleName = data.ReadString();
    std::string secondBundleName = data.ReadString();

    APP_LOGI(
        "firstBundleName %{public}s, secondBundleName %{public}s", firstBundleName.c_str(), secondBundleName.c_str());
    int ret = CheckPublicKeys(firstBundleName, secondBundleName);
    if (!reply.WriteInt32(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetPermissionDef(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string permissionName = data.ReadString();
    APP_LOGI("name %{public}s", permissionName.c_str());

    PermissionDef permissionDef;
    bool ret = GetPermissionDef(permissionName, permissionDef);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&permissionDef)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleHasSystemCapability(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string capName = data.ReadString();

    bool ret = HasSystemCapability(capName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetSystemAvailableCapabilities(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::vector<std::string> caps;
    bool ret = GetSystemAvailableCapabilities(caps);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteStringVector(caps)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleIsSafeMode(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool ret = IsSafeMode();
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCleanBundleCacheFiles(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    sptr<ICleanCacheCallback> cleanCacheCallback = iface_cast<ICleanCacheCallback>(object);
    int32_t userId = data.ReadInt32();

    bool ret = CleanBundleCacheFiles(bundleName, cleanCacheCallback, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCleanBundleDataFiles(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    int userId = data.ReadInt32();

    bool ret = CleanBundleDataFiles(bundleName, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleRegisterBundleStatusCallback(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    sptr<IBundleStatusCallback> BundleStatusCallback = iface_cast<IBundleStatusCallback>(object);

    bool ret = false;
    if (bundleName.empty() || !BundleStatusCallback) {
        APP_LOGE("Get BundleStatusCallback failed");
    } else {
        BundleStatusCallback->SetBundleName(bundleName);
        ret = RegisterBundleStatusCallback(BundleStatusCallback);
    }
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleClearBundleStatusCallback(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    if (object == nullptr) {
        APP_LOGE("read failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    sptr<IBundleStatusCallback> BundleStatusCallback = iface_cast<IBundleStatusCallback>(object);

    bool ret = ClearBundleStatusCallback(BundleStatusCallback);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleUnregisterBundleStatusCallback(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool ret = UnregisterBundleStatusCallback();
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleDumpInfos(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    DumpFlag flag = static_cast<DumpFlag>(data.ReadInt32());
    std::string bundleName = data.ReadString();
    int32_t userId = data.ReadInt32();

    std::string result;
    APP_LOGI("dump info name %{public}s", bundleName.c_str());
    bool ret = DumpInfos(flag, bundleName, userId, result);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        std::vector<std::string> dumpInfos;
        SplitString(result, dumpInfos);
        if (!reply.WriteStringVector(dumpInfos)) {
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleIsApplicationEnabled(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    bool ret = IsApplicationEnabled(bundleName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleSetApplicationEnabled(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    bool isEnable = data.ReadBool();
    int32_t userId = data.ReadInt32();
    bool ret = SetApplicationEnabled(bundleName, isEnable, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleIsAbilityEnabled(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<AbilityInfo> abilityInfo(data.ReadParcelable<AbilityInfo>());
    if (!abilityInfo) {
        APP_LOGE("ReadParcelable<abilityInfo> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    bool ret = IsAbilityEnabled(*abilityInfo);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleSetAbilityEnabled(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<AbilityInfo> abilityInfo(data.ReadParcelable<AbilityInfo>());
    if (!abilityInfo) {
        APP_LOGE("ReadParcelable<abilityInfo> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    bool isEnabled = data.ReadBool();
    int32_t userId = data.ReadInt32();
    bool ret = SetAbilityEnabled(*abilityInfo, isEnabled, userId);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAbilityInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    AbilityInfo info;
    std::string bundleName = data.ReadString();
    std::string abilityName = data.ReadString();
    bool ret = GetAbilityInfo(bundleName, abilityName, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAbilityInfoWithModuleName(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    AbilityInfo info;
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();
    std::string abilityName = data.ReadString();
    bool ret = GetAbilityInfo(bundleName, moduleName, abilityName, info);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

#ifdef BUNDLE_FRAMEWORK_GRAPHICS
ErrCode BundleMgrHost::HandleGetAbilityPixelMapIcon(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string abilityName = data.ReadString();

    APP_LOGI("HandleGetAbilityPixelMapIcon:%{public}s, %{public}s", bundleName.c_str(), abilityName.c_str());
    std::shared_ptr<Media::PixelMap> pixelMap = GetAbilityPixelMapIcon(bundleName, abilityName);
    if (!pixelMap) {
        APP_LOGE("GetAbilityPixelMapIcon failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!reply.WriteBool(true)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!reply.WriteParcelable(pixelMap.get())) {
        APP_LOGE("pixelMap write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAbilityPixelMapIconWithModuleName(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();
    std::string abilityName = data.ReadString();

    APP_LOGI("HandleGetAbilityPixelMapIcon:%{public}s, %{public}s, %{public}s",
        bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
    std::shared_ptr<Media::PixelMap> pixelMap = GetAbilityPixelMapIcon(bundleName, moduleName, abilityName);
    if (!pixelMap) {
        APP_LOGE("GetAbilityPixelMapIcon failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!reply.WriteBool(true)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (!reply.WriteParcelable(pixelMap.get())) {
        APP_LOGE("pixelMap write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}
#endif

ErrCode BundleMgrHost::HandleGetBundleInstaller(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    sptr<IBundleInstaller> installer = GetBundleInstaller();
    if (!installer) {
        APP_LOGE("bundle installer is nullptr");
        return ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED;
    }

    if (!reply.WriteObject<IRemoteObject>(installer->AsObject())) {
        APP_LOGE("failed to reply bundle installer to client, for write parcel error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetBundleUserMgr(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    sptr<IBundleUserMgr> bundleUserMgr = GetBundleUserMgr();
    if (!bundleUserMgr) {
        APP_LOGE("bundle installer is nullptr");
        return ERR_APPEXECFWK_INSTALL_HOST_INSTALLER_FAILED;
    }

    if (!reply.WriteObject<IRemoteObject>(bundleUserMgr->AsObject())) {
        APP_LOGE("failed to reply bundle installer to client, for write parcel error");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAllFormsInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::vector<FormInfo> infos;
    bool ret = GetAllFormsInfo(infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetFormsInfoByApp(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundlename = data.ReadString();
    std::vector<FormInfo> infos;
    bool ret = GetFormsInfoByApp(bundlename, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetFormsInfoByModule(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundlename = data.ReadString();
    std::string modulename = data.ReadString();
    std::vector<FormInfo> infos;
    bool ret = GetFormsInfoByModule(bundlename, modulename, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetShortcutInfos(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundlename = data.ReadString();
    std::vector<ShortcutInfo> infos;
    bool ret = GetShortcutInfos(bundlename, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAllCommonEventInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string eventKey = data.ReadString();
    std::vector<CommonEventInfo> infos;
    bool ret = GetAllCommonEventInfo(eventKey, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    if (ret) {
        if (!WriteParcelableVector(infos, reply)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleCheckBundleNameInAllowList(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    APP_LOGI("bundleName %{public}s," PRId64, bundleName.c_str());
    bool ret = CheckBundleNameInAllowList(bundleName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleBundleClone(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    APP_LOGI("bundleName %{public}s," PRId64, bundleName.c_str());
    bool ret = BundleClone(bundleName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleRemoveClonedBundle(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    int32_t uid = data.ReadInt32();
    APP_LOGI("bundleName %{public}s," PRId64, bundleName.c_str());
    bool ret = RemoveClonedBundle(bundleName, uid);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetDistributedBundleInfo(Parcel &data, Parcel &reply)
{
    std::string networkId = data.ReadString();
    std::string bundleName = data.ReadString();
    DistributedBundleInfo distributedBundleInfo;
    bool ret = GetDistributedBundleInfo(networkId, bundleName, distributedBundleInfo);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&distributedBundleInfo)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAppPrivilegeLevel(Parcel &data, Parcel &reply)
{
    std::string bundleName = data.ReadString();
    int32_t userId = data.ReadInt32();
    auto ret = GetAppPrivilegeLevel(bundleName, userId);
    if (!reply.WriteString(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryExtAbilityInfosWithoutType(Parcel &data, Parcel &reply)
{
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    int32_t flag = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    std::vector<ExtensionAbilityInfo> infos;
    bool ret = QueryExtensionAbilityInfos(*want, flag, userId, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write result failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret && !WriteParcelableVector(infos, reply)) {
        APP_LOGE("write extension infos failed");

        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryExtAbilityInfos(Parcel &data, Parcel &reply)
{
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }

    ExtensionAbilityType type = static_cast<ExtensionAbilityType>(data.ReadInt32());
    int32_t flag = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    std::vector<ExtensionAbilityInfo> infos;
    bool ret = QueryExtensionAbilityInfos(*want, type, flag, userId, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write result failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret && !WriteParcelableVector(infos, reply)) {
        APP_LOGE("write extension infos failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryExtAbilityInfosByType(Parcel &data, Parcel &reply)
{
    ExtensionAbilityType type = static_cast<ExtensionAbilityType>(data.ReadInt32());
    int32_t userId = data.ReadInt32();
    std::vector<ExtensionAbilityInfo> infos;

    bool ret = QueryExtensionAbilityInfos(type, userId, infos);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write result failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret && !WriteParcelableVector(infos, reply)) {
        APP_LOGE("write extension infos failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleVerifyCallingPermission(Parcel &data, Parcel &reply)
{
    std::string permission = data.ReadString();

    bool ret = VerifyCallingPermission(permission);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write result failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAccessibleAppCodePaths(Parcel &data, Parcel &reply)
{
    int32_t userId = data.ReadInt32();
    std::vector<std::string> vec = GetAccessibleAppCodePaths(userId);
    bool ret = vec.empty() ? false : true;
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write result failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret && !reply.WriteStringVector(vec)) {
        APP_LOGE("write code paths failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleQueryExtensionAbilityInfoByUri(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string uri = data.ReadString();
    int32_t userId = data.ReadInt32();
    ExtensionAbilityInfo extensionAbilityInfo;
    bool ret = QueryExtensionAbilityInfoByUri(uri, userId, extensionAbilityInfo);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&extensionAbilityInfo)) {
            APP_LOGE("write failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAppIdByBundleName(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    int32_t userId = data.ReadInt32();
    std::string appId = GetAppIdByBundleName(bundleName, userId);
    APP_LOGD("appId is %{private}s", appId.c_str());
    if (!reply.WriteString(appId)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAppType(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string appType = GetAppType(bundleName);
    APP_LOGD("appType is %{public}s", appType.c_str());
    if (!reply.WriteString(appType)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetUidByBundleName(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    int32_t userId = data.ReadInt32();
    int32_t uid = GetUidByBundleName(bundleName, userId);
    APP_LOGD("uid is %{public}d", uid);
    if (!reply.WriteInt32(uid)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleIsModuleRemovable(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();

    APP_LOGD("bundleName %{public}s, moduleName %{public}s", bundleName.c_str(), moduleName.c_str());
    bool ret = IsModuleRemovable(bundleName, moduleName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleSetModuleRemovable(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();
    bool isEnable = data.ReadBool();
    APP_LOGD("bundleName %{public}s, moduleName %{public}s", bundleName.c_str(), moduleName.c_str());
    bool ret = SetModuleRemovable(bundleName, moduleName, isEnable);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetModuleUpgradeFlag(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();

    APP_LOGD("bundleName %{public}s, moduleName %{public}s", bundleName.c_str(), moduleName.c_str());
    bool ret = GetModuleUpgradeFlag(bundleName, moduleName);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleSetModuleUpgradeFlag(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();
    int32_t upgradeFlag = data.ReadInt32();
    APP_LOGD("bundleName %{public}s, moduleName %{public}s", bundleName.c_str(), moduleName.c_str());
    bool ret = SetModuleUpgradeFlag(bundleName, moduleName, upgradeFlag);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleImplicitQueryInfoByPriority(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t flags = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    AbilityInfo abilityInfo;
    ExtensionAbilityInfo extensionInfo;
    bool ret = ImplicitQueryInfoByPriority(*want, flags, userId, abilityInfo, extensionInfo);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&abilityInfo)) {
            APP_LOGE("write AbilityInfo failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        if (!reply.WriteParcelable(&extensionInfo)) {
            APP_LOGE("write ExtensionAbilityInfo failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetAllDependentModuleNames(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    std::string moduleName = data.ReadString();
    std::vector<std::string> dependentModuleNames;
    bool ret = GetAllDependentModuleNames(bundleName, moduleName, dependentModuleNames);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write result failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret && !reply.WriteStringVector(dependentModuleNames)) {
        APP_LOGE("write dependentModuleNames failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    return ERR_OK;
}

ErrCode BundleMgrHost::HandleGetSandboxBundleInfo(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::string bundleName = data.ReadString();
    int32_t appIndex = data.ReadInt32();
    int32_t userId = data.ReadInt32();

    BundleInfo info;
    auto res = GetSandboxBundleInfo(bundleName, appIndex, userId, info);
    if (!reply.WriteInt32(res)) {
        return ERR_APPEXECFWK_SANDBOX_INSTALL_WRITE_PARCEL_ERROR;
    }
    if (res == ERR_OK && !reply.WriteParcelable(&info)) {
        return ERR_APPEXECFWK_SANDBOX_INSTALL_WRITE_PARCEL_ERROR;
    }
    return ERR_OK;
}

template<typename T>
bool BundleMgrHost::WriteParcelableVector(std::vector<T> &parcelableVector, Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        APP_LOGE("write ParcelableVector failed");
        return false;
    }

    for (auto &parcelable : parcelableVector) {
        if (!reply.WriteParcelable(&parcelable)) {
            APP_LOGE("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

template<typename T>
bool BundleMgrHost::WriteParcelableVectorIntoAshmem(
    std::vector<T> &parcelableVector, const char *ashmemName, Parcel &reply)
{
    APP_LOGD("Write parcelable vector into ashmem");
    if (ashmemName == nullptr) {
        APP_LOGE("AshmemName is null");
        return false;
    }

    if (!reply.WriteInt32(parcelableVector.size())) {
        APP_LOGE("Write parcelable vector size failed");
        return false;
    }

    MessageParcel *messageParcel = reinterpret_cast<MessageParcel *>(&reply);
    if (!messageParcel) {
        APP_LOGE("Type conversion failed");
        return false;
    }

    // Calculate the size of the ashmem,
    // and get content that needs to be stored in ashmem.
    int32_t totalSize = 0;
    std::vector<std::string> infoStrs;
    for (auto &parcelable : parcelableVector) {
        auto str = GetJsonStrFromInfo<T>(parcelable);
        infoStrs.emplace_back(str);
        totalSize += ASHMEM_LEN;
        totalSize += strlen(str.c_str());
    }

    if (infoStrs.empty() || totalSize <= 0) {
        APP_LOGE("The size of the ashmem is invalid or the content is empty");
        return false;
    }

    // The ashmem name must be unique.
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(
        (ashmemName + std::to_string(AllocatAshmemNum())).c_str(), totalSize);
    if (ashmem == nullptr) {
        APP_LOGE("Create shared memory fail");
        return false;
    }

    // Set the read/write mode of the ashme.
    bool ret = ashmem->MapReadAndWriteAshmem();
    if (!ret) {
        APP_LOGE("Map shared memory fail");
        return false;
    }

    // Write the size and content of each item to the ashmem.
    // The size of item use ASHMEM_LEN.
    int32_t offset = 0;
    for (auto &infoStr : infoStrs) {
        int itemLen = static_cast<int>(strlen(infoStr.c_str()));
        ret = ashmem->WriteToAshmem(std::to_string(itemLen).c_str(), ASHMEM_LEN, offset);
        if (!ret) {
            APP_LOGE("Write itemLen to shared memory fail");
            ClearAshmem(ashmem);
            return false;
        }

        offset += ASHMEM_LEN;
        ret = ashmem->WriteToAshmem(infoStr.c_str(), itemLen, offset);
        if (!ret) {
            APP_LOGE("Write info to shared memory fail");
            ClearAshmem(ashmem);
            return false;
        }

        offset += itemLen;
    }

    ret = messageParcel->WriteAshmem(ashmem);
    ClearAshmem(ashmem);
    if (!ret) {
        APP_LOGE("Write ashmem to parcel fail");
        return false;
    }

    APP_LOGD("Write parcelable vector into ashmem success");
    return true;
}

int32_t BundleMgrHost::AllocatAshmemNum()
{
    std::lock_guard<std::mutex> lock(bundleAshmemMutex_);
    return ashmemNum_++;
}

ErrCode BundleMgrHost::HandleQueryAbilityInfoWithCallback(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("ReadParcelable<want> failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t flags = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    sptr<IRemoteObject> object = data.ReadObject<IRemoteObject>();
    AbilityInfo info;
    bool ret = QueryAbilityInfo(*want, flags, userId, info, object);
    if (!reply.WriteBool(ret)) {
        APP_LOGE("write ret failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    if (ret) {
        if (!reply.WriteParcelable(&info)) {
            APP_LOGE("write info failed");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
    }
    return ERR_OK;
}
ErrCode BundleMgrHost::HandleUpgradeAtomicService(Parcel &data, Parcel &reply)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    std::unique_ptr<Want> want(data.ReadParcelable<Want>());
    if (!want) {
        APP_LOGE("read parcelable want failed");
        return ERR_APPEXECFWK_PARCEL_ERROR;
    }
    int32_t userId = data.ReadInt32();
    UpgradeAtomicService(*want, userId);
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS
