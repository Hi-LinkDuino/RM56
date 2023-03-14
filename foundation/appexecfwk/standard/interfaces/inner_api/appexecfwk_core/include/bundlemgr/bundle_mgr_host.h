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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H

#include <mutex>

#include "iremote_stub.h"
#include "nocopyable.h"

#include "appexecfwk_errors.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrHost : public IRemoteStub<IBundleMgr> {
public:
    BundleMgrHost() = default;
    virtual ~BundleMgrHost() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    /**
     * @brief Handles the GetApplicationInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetApplicationInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfoWithIntFlags(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetApplicationInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetApplicationInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfosWithIntFlags(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundlePackInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundlePackInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundlePackInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundlePackInfoWithIntFlags(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfoWithIntFlags(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfosWithIntFlags(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleNameForUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleNameForUid(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundlesForUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundlesForUid(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetNameForUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetNameForUid(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleGids function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleGids(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleGidsByUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleGidsByUid(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInfosByMetaData function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfosByMetaData(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfo(Parcel &data, Parcel &reply);

     /**
     * @brief Handles the QueryAbilityInfo function called from a IBundleMgr proxy object with callback.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoWithCallback(Parcel &data, Parcel &reply);

    /**
     * @brief Handles the UpgradeAtomicService function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleUpgradeAtomicService(Parcel &data, Parcel &reply);

    /**
     * @brief Handles the QueryAbilityInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoMutiparam(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfosForClone function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfosForClone(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfoByUri function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoByUri(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfoByUri function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfosByUri(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfosMutiparam function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfosMutiparam(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleQueryAllAbilityInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAllAbilityInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryAbilityInfoByUri function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoByUriForUserId(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the QueryKeepAliveBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryKeepAliveBundleInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetAbilityLabel function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAbilityLabel(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetAbilityLabel function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAbilityLabelWithModuleName(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CheckIsSystemAppByUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCheckIsSystemAppByUid(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleArchiveInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleArchiveInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleArchiveInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleArchiveInfoWithIntFlags(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetHapModuleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetHapModuleInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetHapModuleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetHapModuleInfoWithUserId(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetLaunchWantForBundle function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetLaunchWantForBundle(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CheckPublicKeys function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCheckPublicKeys(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetPermissionDef function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetPermissionDef(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HasSystemCapability function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleHasSystemCapability(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetSystemAvailableCapabilities function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetSystemAvailableCapabilities(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the IsSafeMode function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsSafeMode(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CleanBundleCacheFiles function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCleanBundleCacheFiles(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the CleanBundleDataFiles function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCleanBundleDataFiles(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the RegisterBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleRegisterBundleStatusCallback(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the ClearBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleClearBundleStatusCallback(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the UnregisterBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleUnregisterBundleStatusCallback(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the DumpInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleDumpInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleInstaller function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInstaller(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetBundleUserMgr function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleUserMgr(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the IsApplicationEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsApplicationEnabled(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the SetApplicationEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetApplicationEnabled(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the IsAbilityEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsAbilityEnabled(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the SetAbilityEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetAbilityEnabled(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetAllFormsInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAllFormsInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetFormsInfoByApp function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetFormsInfoByApp(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleGetFormsInfoByModule function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetFormsInfoByModule(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleGetShortcutInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetShortcutInfos(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleGetAllCommonEventInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAllCommonEventInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleRemoveClonedBundle function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleRemoveClonedBundle(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleBundleClone function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleBundleClone(Parcel &data, Parcel &reply);
    /**
     * @brief Determine whether the application is in the allow list.
     * @param bundleName Indicates the bundle Names.
     * @return Returns true if bundle name in the allow list successfully; returns false otherwise.
     */
    ErrCode HandleCheckBundleNameInAllowList(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the GetDistributedBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetDistributedBundleInfo(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleGetAppPrivilegeLevel function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAppPrivilegeLevel(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleQueryExtAbilityInfosWithoutType function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryExtAbilityInfosWithoutType(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleQueryExtensionInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryExtAbilityInfos(Parcel &data, Parcel &reply);

    ErrCode HandleQueryExtAbilityInfosByType(Parcel &data, Parcel &reply);

    ErrCode HandleVerifyCallingPermission(Parcel &data, Parcel &reply);

    ErrCode HandleGetAccessibleAppCodePaths(Parcel &data, Parcel &reply);

    ErrCode HandleQueryExtensionAbilityInfoByUri(Parcel &data, Parcel &reply);

    ErrCode HandleGetAppIdByBundleName(Parcel &data, Parcel &reply);

    ErrCode HandleGetAppType(Parcel &data, Parcel &reply);

    ErrCode HandleGetUidByBundleName(Parcel &data, Parcel &reply);
#ifdef BUNDLE_FRAMEWORK_GRAPHICS
    ErrCode HandleGetAbilityPixelMapIcon(Parcel &data, Parcel &reply);

    ErrCode HandleGetAbilityPixelMapIconWithModuleName(Parcel &data, Parcel &reply);
#endif
    ErrCode HandleGetAbilityInfo(Parcel &data, Parcel &reply);

    ErrCode HandleGetAbilityInfoWithModuleName(Parcel &data, Parcel &reply);

    /**
     * @brief Handles the HandleGetModuleUpgradeFlag function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetModuleUpgradeFlag(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleSetModuleUpgradeFlag function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetModuleUpgradeFlag(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleIsModuleRemovable function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsModuleRemovable(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the HandleSetModuleRemovable function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetModuleRemovable(Parcel &data, Parcel &reply);

    ErrCode HandleImplicitQueryInfoByPriority(Parcel &data, Parcel &reply);

    ErrCode HandleGetAllDependentModuleNames(Parcel &data, Parcel &reply);

    ErrCode HandleGetSandboxBundleInfo(Parcel &data, Parcel &reply);

private:
    /**
     * @brief Write a parcelabe vector objects to the proxy node.
     * @param parcelableVector Indicates the objects to be write.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if objects send successfully; returns false otherwise.
     */
    template<typename T>
    bool WriteParcelableVector(std::vector<T> &parcelableVector, Parcel &reply);
    /**
     * @brief Write a parcelabe vector objects to ashmem.
     * @param parcelableVector Indicates the objects to be write.
     * @param ashmemName Indicates the ashmem name;
     * @param reply Indicates the reply to be sent;
     * @return Returns true if objects send successfully; returns false otherwise.
     */
    template<typename T>
    bool WriteParcelableVectorIntoAshmem(
        std::vector<T> &parcelableVector, const char *ashmemName, Parcel &reply);
    /**
     * @brief Allocat ashmem num.
     * @return Returns ashmem num.
     */
    int32_t AllocatAshmemNum();

    std::mutex bundleAshmemMutex_;
    int32_t ashmemNum_ = 0;
    DISALLOW_COPY_AND_MOVE(BundleMgrHost);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H
