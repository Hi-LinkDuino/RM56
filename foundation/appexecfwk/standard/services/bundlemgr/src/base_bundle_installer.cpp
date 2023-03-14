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

#include "base_bundle_installer.h"

#include "nlohmann/json.hpp"

#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
#include "aging/bundle_aging_mgr.h"
#endif
#include "ability_manager_helper.h"
#include "app_log_wrapper.h"
#include "bundle_clone_mgr.h"
#include "bundle_constants.h"
#include "bundle_extractor.h"
#include "bundle_mgr_service.h"
#include "bundle_sandbox_installer.h"
#include "bundle_parser.h"
#include "bundle_permission_mgr.h"
#include "bundle_util.h"
#include "bundle_verify_mgr.h"
#include "hitrace_meter.h"
#include "datetime_ex.h"
#include "installd_client.h"
#include "perf_profile.h"
#include "scope_guard.h"
#include "string_ex.h"
#include "systemcapability.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::Security;
namespace {
std::string GetHapPath(const InnerBundleInfo &info, const std::string &moduleName)
{
    return info.GetAppCodePath() + Constants::PATH_SEPARATOR
        + moduleName + Constants::INSTALL_FILE_SUFFIX;
}

std::string GetHapPath(const InnerBundleInfo &info)
{
    return GetHapPath(info, info.GetModuleName(info.GetCurrentModulePackage()));
}
}

BaseBundleInstaller::BaseBundleInstaller()
{
    APP_LOGI("base bundle installer instance is created");
}

BaseBundleInstaller::~BaseBundleInstaller()
{
    APP_LOGI("base bundle installer instance is destroyed");
}

ErrCode BaseBundleInstaller::InstallBundle(
    const std::string &bundlePath, const InstallParam &installParam, const Constants::AppType appType)
{
    std::vector<std::string> bundlePaths { bundlePath };
    return InstallBundle(bundlePaths, installParam, appType);
}

ErrCode BaseBundleInstaller::InstallBundle(
    const std::vector<std::string> &bundlePaths, const InstallParam &installParam, const Constants::AppType appType)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    APP_LOGD("begin to process bundle install");

    PerfProfile::GetInstance().SetBundleInstallStartTime(GetTickCount());

    int32_t uid = Constants::INVALID_UID;
    ErrCode result = ProcessBundleInstall(bundlePaths, installParam, appType, uid);
    if (installParam.needSendEvent && dataMgr_ && !bundleName_.empty()) {
        dataMgr_->NotifyBundleStatus(bundleName_,
            Constants::EMPTY_STRING,
            mainAbility_,
            result,
            isAppExist_ ? NotifyType::UPDATE : NotifyType::INSTALL,
            uid);
    }

    if (result == ERR_OK) {
        DistributedDataStorage::GetInstance()->SaveStorageDistributeInfo(bundleName_, userId_);
    }

    SendBundleSystemEvent(
        bundleName_,
        (isAppExist_ ? BundleEventType::UPDATE : BundleEventType::INSTALL),
        installParam,
        sysEventInfo_.preBundleScene,
        result);
    PerfProfile::GetInstance().SetBundleInstallEndTime(GetTickCount());
    APP_LOGD("finish to process bundle install");
    return result;
}

ErrCode BaseBundleInstaller::InstallBundleByBundleName(
    const std::string &bundleName, const InstallParam &installParam)
{
    APP_LOGD("begin to process bundle install by bundleName, which is %{public}s.", bundleName.c_str());
    PerfProfile::GetInstance().SetBundleInstallStartTime(GetTickCount());

    int32_t uid = Constants::INVALID_UID;
    ErrCode result = ProcessInstallBundleByBundleName(bundleName, installParam, uid);
    if (installParam.needSendEvent && dataMgr_ && !bundleName.empty()) {
        dataMgr_->NotifyBundleStatus(
            bundleName, Constants::EMPTY_STRING, Constants::EMPTY_STRING, result, NotifyType::INSTALL, uid);
    }

    if (result == ERR_OK) {
        DistributedDataStorage::GetInstance()->SaveStorageDistributeInfo(bundleName, userId_);
    }

    SendBundleSystemEvent(
        bundleName,
        BundleEventType::INSTALL,
        installParam,
        InstallScene::CREATE_USER,
        result);
    PerfProfile::GetInstance().SetBundleInstallEndTime(GetTickCount());
    APP_LOGD("finish to process %{public}s bundle install", bundleName.c_str());
    return result;
}

ErrCode BaseBundleInstaller::Recover(
    const std::string &bundleName, const InstallParam &installParam)
{
    APP_LOGD("begin to process bundle recover by bundleName, which is %{public}s.", bundleName.c_str());
    PerfProfile::GetInstance().SetBundleInstallStartTime(GetTickCount());
    if (!BundlePermissionMgr::Init()) {
        APP_LOGW("BundlePermissionMgr::Init failed");
    }
    int32_t uid = Constants::INVALID_UID;
    ErrCode result = ProcessRecover(bundleName, installParam, uid);
    if (installParam.needSendEvent && dataMgr_ && !bundleName_.empty() && !modulePackage_.empty()) {
        dataMgr_->NotifyBundleStatus(
            bundleName, Constants::EMPTY_STRING, Constants::EMPTY_STRING, result, NotifyType::INSTALL, uid);
    }

    if (result == ERR_OK) {
        DistributedDataStorage::GetInstance()->SaveStorageDistributeInfo(bundleName, userId_);
    }

    auto recoverInstallParam = installParam;
    recoverInstallParam.isPreInstallApp = true;
    SendBundleSystemEvent(
        bundleName,
        BundleEventType::RECOVER,
        recoverInstallParam,
        sysEventInfo_.preBundleScene,
        result);
    PerfProfile::GetInstance().SetBundleInstallEndTime(GetTickCount());
    BundlePermissionMgr::UnInit();
    APP_LOGD("finish to process %{public}s bundle recover", bundleName.c_str());
    return result;
}

ErrCode BaseBundleInstaller::UninstallBundle(const std::string &bundleName, const InstallParam &installParam)
{
    APP_LOGD("begin to process %{public}s bundle uninstall", bundleName.c_str());
    PerfProfile::GetInstance().SetBundleUninstallStartTime(GetTickCount());

    // uninstall all sandbox app before
    UninstallAllSandboxApps(bundleName, installParam.userId);

    int32_t uid = Constants::INVALID_UID;
    ErrCode result = ProcessBundleUninstall(bundleName, installParam, uid);
    if (installParam.needSendEvent && dataMgr_) {
        dataMgr_->NotifyBundleStatus(
            bundleName, Constants::EMPTY_STRING, Constants::EMPTY_STRING, result, NotifyType::UNINSTALL_BUNDLE, uid);
    }

    if (result == ERR_OK) {
        DistributedDataStorage::GetInstance()->DeleteStorageDistributeInfo(bundleName, userId_);
    }

    SendBundleSystemEvent(
        bundleName,
        BundleEventType::UNINSTALL,
        installParam,
        sysEventInfo_.preBundleScene,
        result);
    PerfProfile::GetInstance().SetBundleUninstallEndTime(GetTickCount());
    APP_LOGD("finish to process %{public}s bundle uninstall", bundleName.c_str());
    return result;
}

ErrCode BaseBundleInstaller::UninstallBundle(
    const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam)
{
    APP_LOGD("begin to process %{public}s module in %{public}s uninstall", modulePackage.c_str(), bundleName.c_str());
    PerfProfile::GetInstance().SetBundleUninstallStartTime(GetTickCount());

    // uninstall all sandbox app before
    UninstallAllSandboxApps(bundleName, installParam.userId);

    int32_t uid = Constants::INVALID_UID;
    ErrCode result = ProcessBundleUninstall(bundleName, modulePackage, installParam, uid);
    if (installParam.needSendEvent && dataMgr_) {
        dataMgr_->NotifyBundleStatus(
            bundleName, modulePackage, Constants::EMPTY_STRING, result, NotifyType::UNINSTALL_MODULE, uid);
    }

    if (result == ERR_OK) {
        InnerBundleInfo innerBundleInfo;
        if (!dataMgr_->GetInnerBundleInfo(bundleName, innerBundleInfo)) {
            DistributedDataStorage::GetInstance()->DeleteStorageDistributeInfo(bundleName, userId_);
        } else {
            DistributedDataStorage::GetInstance()->SaveStorageDistributeInfo(bundleName, userId_);
            dataMgr_->EnableBundle(bundleName);
        }
    }

    SendBundleSystemEvent(
        bundleName,
        BundleEventType::UNINSTALL,
        installParam,
        sysEventInfo_.preBundleScene,
        result);
    PerfProfile::GetInstance().SetBundleUninstallEndTime(GetTickCount());
    APP_LOGD("finish to process %{public}s module in %{public}s uninstall", modulePackage.c_str(), bundleName.c_str());
    return result;
}

void BaseBundleInstaller::UpdateInstallerState(const InstallerState state)
{
    APP_LOGD("UpdateInstallerState in BaseBundleInstaller state %{public}d", state);
    SetInstallerState(state);
}

void BaseBundleInstaller::SaveOldRemovableInfo(
    InnerModuleInfo &newModuleInfo, InnerBundleInfo &oldInfo, bool existModule)
{
    if (existModule) {
        // save old module useId isRemovable info to new module
        auto oldModule = oldInfo.FetchInnerModuleInfos().find(newModuleInfo.modulePackage);
        if (oldModule == oldInfo.FetchInnerModuleInfos().end()) {
            APP_LOGE("can not find module %{public}s in oldInfo", newModuleInfo.modulePackage.c_str());
            return;
        }
        for (const auto &remove : oldModule->second.isRemovable) {
            auto result = newModuleInfo.isRemovable.try_emplace(remove.first, remove.second);
            if (!result.second) {
                APP_LOGE("%{public}s removable add %{public}s from old:%{public}d failed",
                    newModuleInfo.modulePackage.c_str(), remove.first.c_str(), remove.second);
            }
            APP_LOGD("%{public}s removable add %{public}s from old:%{public}d",
                newModuleInfo.modulePackage.c_str(), remove.first.c_str(), remove.second);
        }
    }
}

void BaseBundleInstaller::CheckEnableRemovable(std::unordered_map<std::string, InnerBundleInfo> &newInfos,
    InnerBundleInfo &oldInfo, int32_t &userId, bool isFreeInstallFlag, bool isAppExist)
{
    bool existModule = false;
    for (auto &item : newInfos) {
        std::map<std::string, InnerModuleInfo> &moduleInfo = item.second.FetchInnerModuleInfos();
        bool hasInstalledInUser = oldInfo.HasInnerBundleUserInfo(userId);
        // now there are three cases for set haps isRemovable true:
        // 1. FREE_INSTALL flag
        // 2. bundle not exist in current user
        // 3. bundle exist, hap not exist
        // 4. hap exist not in current userId
        for (auto &iter : moduleInfo) {
            APP_LOGD("modulePackage:(%{public}s), userId:%{public}d, flag:%{public}d, isAppExist:%{public}d",
                iter.second.modulePackage.c_str(), userId, isFreeInstallFlag, isAppExist);
            existModule = oldInfo.FindModule(iter.second.modulePackage);
            bool hasModuleInUser = item.second.IsUserExistModule(iter.second.moduleName, userId);
            APP_LOGD("hasInstalledInUser:%{public}d, existModule:(%{public}d), hasModuleInUser:(%{public}d)",
                hasInstalledInUser, existModule, hasModuleInUser);
            if (isFreeInstallFlag && (!isAppExist || !hasInstalledInUser || !existModule || !hasModuleInUser)) {
                APP_LOGD("hasInstalledInUser:%{public}d, isAppExist:%{public}d existModule:(%{public}d)",
                    hasInstalledInUser, isAppExist, existModule);
                item.second.SetModuleRemovable(iter.second.moduleName, true, userId);
                SaveOldRemovableInfo(iter.second, oldInfo, existModule);
            }
        }
    }
}

ErrCode BaseBundleInstaller::InnerProcessBundleInstall(std::unordered_map<std::string, InnerBundleInfo> &newInfos,
    InnerBundleInfo &oldInfo, const InstallParam &installParam, int32_t &uid)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    APP_LOGI("InnerProcessBundleInstall with bundleName %{public}s, userId is %{public}d", bundleName_.c_str(),
        userId_);
    if (installParam.needSavePreInstallInfo) {
        PreInstallBundleInfo preInstallBundleInfo;
        dataMgr_->GetPreInstallBundleInfo(bundleName_, preInstallBundleInfo);
        preInstallBundleInfo.SetAppType(newInfos.begin()->second.GetAppType());
        preInstallBundleInfo.SetVersionCode(newInfos.begin()->second.GetVersionCode());
        for (const auto &item : newInfos) {
            preInstallBundleInfo.AddBundlePath(item.first);
        }
        dataMgr_->SavePreInstallBundleInfo(bundleName_, preInstallBundleInfo);
    }

    // singleton app can only be installed in U0 and U0 can only install singleton app.
    bool isSingleton = newInfos.begin()->second.IsSingleton();
    if ((isSingleton && (userId_ != Constants::DEFAULT_USERID)) ||
        (!isSingleton && (userId_ == Constants::DEFAULT_USERID))) {
        APP_LOGW("singleton(%{public}d) app(%{public}s) and user(%{public}d) are not matched.",
            isSingleton, bundleName_.c_str(), userId_);
        return ERR_APPEXECFWK_INSTALL_ZERO_USER_WITH_NO_SINGLETON;
    }

    // try to get the bundle info to decide use install or update. Always keep other exceptions below this line.
    if (!GetInnerBundleInfo(oldInfo, isAppExist_)) {
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }
    APP_LOGI("flag:%{public}d, userId:%{public}d, isAppExist:%{public}d",
        installParam.installFlag, userId_, isAppExist_);
    bool isFreeInstallFlag = (installParam.installFlag == InstallFlag::FREE_INSTALL);
    CheckEnableRemovable(newInfos, oldInfo, userId_, isFreeInstallFlag, isAppExist_);

    ErrCode result = ERR_OK;
    if (isAppExist_) {
        // to check new or old modle, application or hm service of the bundle.
        result = CheckHapModleOrType(oldInfo, newInfos);
        CHECK_RESULT(result, "bundle modle or type is not same %{public}d");
        // to guarantee that the hap version can be compatible.
        result = CheckVersionCompatibility(oldInfo);
        CHECK_RESULT(result, "The app has been installed and update lower version bundle %{public}d");

        hasInstalledInUser_ = oldInfo.HasInnerBundleUserInfo(userId_);
        if (!hasInstalledInUser_) {
            APP_LOGD("new userInfo with bundleName %{public}s and userId %{public}d",
                bundleName_.c_str(), userId_);
            InnerBundleUserInfo newInnerBundleUserInfo;
            newInnerBundleUserInfo.bundleUserInfo.userId = userId_;
            newInnerBundleUserInfo.bundleName = bundleName_;
            oldInfo.AddInnerBundleUserInfo(newInnerBundleUserInfo);
            ScopeGuard userGuard([&] { RemoveBundleUserData(oldInfo, false); });
            uint32_t tokenId = CreateAccessTokenId(oldInfo);
            oldInfo.SetAccessTokenId(tokenId, userId_);
            result = GrantRequestPermissions(oldInfo, tokenId);
            CHECK_RESULT(result, "GrantRequestPermissions failed %{public}d");

            result = CreateBundleUserData(oldInfo);
            CHECK_RESULT(result, "CreateBundleUserData failed %{public}d");

            userGuard.Dismiss();
        }

        for (auto &info : newInfos) {
            std::string packageName = info.second.GetCurrentModulePackage();
            if (oldInfo.FindModule(packageName)) {
                installedModules_[packageName] = true;
            }
        }
    }

    auto it = newInfos.begin();
    if (!isAppExist_) {
        APP_LOGI("app is not exist");
        InnerBundleInfo &newInfo = it->second;
        modulePath_ = it->first;
        InnerBundleUserInfo newInnerBundleUserInfo;
        newInnerBundleUserInfo.bundleUserInfo.userId = userId_;
        newInnerBundleUserInfo.bundleName = bundleName_;
        newInfo.AddInnerBundleUserInfo(newInnerBundleUserInfo);
        APP_LOGI("SetIsFreeInstallApp(%{public}d)", InstallFlag::FREE_INSTALL == installParam.installFlag);
        newInfo.SetIsFreeInstallApp(InstallFlag::FREE_INSTALL == installParam.installFlag);
        result = ProcessBundleInstallStatus(newInfo, uid);
        CHECK_RESULT(result, "ProcessBundleInstallStatus failed %{public}d");

        it++;
        hasInstalledInUser_ = true;
    }

    InnerBundleInfo bundleInfo;
    bool isBundleExist = false;
    if (!GetInnerBundleInfo(bundleInfo, isBundleExist) || !isBundleExist) {
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    InnerBundleUserInfo innerBundleUserInfo;
    if (!bundleInfo.GetInnerBundleUserInfo(userId_, innerBundleUserInfo)) {
        APP_LOGE("oldInfo do not have user");
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    ScopeGuard userGuard([&] {
        if (!hasInstalledInUser_ || (!isAppExist_)) {
            RemoveBundleUserData(oldInfo, false);
        }
    });

    // update haps
    for (; it != newInfos.end(); ++it) {
        modulePath_ = it->first;
        InnerBundleInfo &newInfo = it->second;
        newInfo.AddInnerBundleUserInfo(innerBundleUserInfo);
        bool isReplace = (installParam.installFlag == InstallFlag::REPLACE_EXISTING ||
            installParam.installFlag == InstallFlag::FREE_INSTALL);
        // app exist, but module may not
        if ((result = ProcessBundleUpdateStatus(
            bundleInfo, newInfo, isReplace, installParam.noSkipsKill)) != ERR_OK) {
            break;
        }
    }

    if (result == ERR_OK) {
        userGuard.Dismiss();
    }

    uid = bundleInfo.GetUid(userId_);
    mainAbility_ = bundleInfo.GetMainAbility();
    return result;
}

uint32_t BaseBundleInstaller::CreateAccessTokenId(const InnerBundleInfo &info)
{
    return BundlePermissionMgr::CreateAccessTokenId(info, info.GetBundleName(), userId_);
}

ErrCode BaseBundleInstaller::GrantRequestPermissions(const InnerBundleInfo &info, const uint32_t tokenId)
{
    if (!BundlePermissionMgr::GrantRequestPermissions(info, tokenId)) {
        APP_LOGE("GrantRequestPermissions failed, bundleName: %{public}s", info.GetBundleName().c_str());
        return ERR_APPEXECFWK_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED;
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessBundleInstall(const std::vector<std::string> &inBundlePaths,
    const InstallParam &installParam, const Constants::AppType appType, int32_t &uid)
{
    APP_LOGD("ProcessBundleInstall bundlePath install");
    if (!dataMgr_) {
        dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
        if (!dataMgr_) {
            APP_LOGE("Get dataMgr shared_ptr nullptr");
            return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
        }
    }

    userId_ = GetUserId(installParam.userId);
    if (userId_ == Constants::INVALID_USERID) {
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    if (!dataMgr_->HasUserId(userId_)) {
        APP_LOGE("The user %{public}d does not exist when install.", userId_);
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    std::vector<std::string> bundlePaths;
    // check hap paths
    ErrCode result = BundleUtil::CheckFilePath(inBundlePaths, bundlePaths);
    CHECK_RESULT(result, "hap file check failed %{public}d");
    UpdateInstallerState(InstallerState::INSTALL_BUNDLE_CHECKED);                  // ---- 5%

    // check syscap
    result = CheckSysCap(bundlePaths);
    CHECK_RESULT(result, "hap syscap check failed %{public}d");
    UpdateInstallerState(InstallerState::INSTALL_SYSCAP_CHECKED);                  // ---- 10%

    // verify signature info for all haps
    std::vector<Security::Verify::HapVerifyResult> hapVerifyResults;
    result = CheckMultipleHapsSignInfo(bundlePaths, installParam, hapVerifyResults);
    CHECK_RESULT(result, "hap files check signature info failed %{public}d");
    UpdateInstallerState(InstallerState::INSTALL_SIGNATURE_CHECKED);               // ---- 15%

    // parse the bundle infos for all haps
    // key is bundlePath , value is innerBundleInfo
    std::unordered_map<std::string, InnerBundleInfo> newInfos;
    result = ParseHapFiles(bundlePaths, installParam, appType, hapVerifyResults, newInfos);
    CHECK_RESULT(result, "parse haps file failed %{public}d");
    UpdateInstallerState(InstallerState::INSTALL_PARSED);                          // ---- 20%

    // check hap hash param
    result = CheckHapHashParams(newInfos, installParam.hashParams);
    CHECK_RESULT(result, "check hap hash param failed %{public}d");
    UpdateInstallerState(InstallerState::INSTALL_HAP_HASH_PARAM_CHECKED);          // ---- 25%

    // check versioncode and bundleName
    result = CheckAppLabelInfo(newInfos);
    CHECK_RESULT(result, "verisoncode or bundleName is different in all haps %{public}d");
    UpdateInstallerState(InstallerState::INSTALL_VERSION_AND_BUNDLENAME_CHECKED);  // ---- 30%

    // uninstall all sandbox app before
    UninstallAllSandboxApps(bundleName_);
    UpdateInstallerState(InstallerState::INSTALL_REMOVE_SANDBOX_APP);              // ---- 50%

    // this state should always be set when return
    ScopeGuard stateGuard([&] { dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::INSTALL_SUCCESS); });

    // this state should always be set when return
    ScopeGuard enableGuard([&] { dataMgr_->EnableBundle(bundleName_); });
    InnerBundleInfo oldInfo;
    result = InnerProcessBundleInstall(newInfos, oldInfo, installParam, uid);
    CHECK_RESULT_WITH_ROLLBACK(result, "internal processing failed with result %{public}d", newInfos, oldInfo);
    UpdateInstallerState(InstallerState::INSTALL_INFO_SAVED);                      // ---- 80%

    // rename for all temp dirs
    for (const auto &info : newInfos) {
        if (info.second.IsOnlyCreateBundleUser()) {
            continue;
        }
        if ((result = RenameModuleDir(info.second)) != ERR_OK) {
            break;
        }
    }
    UpdateInstallerState(InstallerState::INSTALL_RENAMED);                         // ---- 90%

    CHECK_RESULT_WITH_ROLLBACK(result, "rename temp dirs failed with result %{public}d", newInfos, oldInfo);
    if (!uninstallModuleVec_.empty()) {
        UninstallLowerVersionFeature(uninstallModuleVec_);
    }

    SaveHapPathToRecords(installParam.isPreInstallApp, newInfos);
    UpdateInstallerState(InstallerState::INSTALL_SUCCESS);                         // ---- 100%
    APP_LOGD("finish ProcessBundleInstall bundlePath install touch off aging");
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
    if (installParam.installFlag == InstallFlag::FREE_INSTALL) {
        DelayedSingleton<BundleMgrService>::GetInstance()->GetAgingMgr()->Start(
            BundleAgingMgr::AgingTriggertype::FREE_INSTALL);
    }
#endif
    OnSingletonChange();
    return result;
}

void BaseBundleInstaller::RollBack(const std::unordered_map<std::string, InnerBundleInfo> &newInfos,
    InnerBundleInfo &oldInfo)
{
    APP_LOGD("start rollback due to install failed");
    if (!isAppExist_) {
        RemoveBundleAndDataDir(newInfos.begin()->second, false);
        // delete accessTokenId
        if (BundlePermissionMgr::DeleteAccessTokenId(newInfos.begin()->second.GetAccessTokenId(userId_)) !=
            AccessToken::AccessTokenKitRet::RET_SUCCESS) {
            APP_LOGE("delete accessToken failed");
        }
        // remove innerBundleInfo
        RemoveInfo(bundleName_, "");
        return;
    }
    InnerBundleInfo preInfo;
    bool isExist = false;
    if (!GetInnerBundleInfo(preInfo, isExist) || !isExist) {
        APP_LOGI("finish rollback due to install failed");
        return;
    }
    for (const auto &info : newInfos) {
        RollBack(info.second, oldInfo);
    }
    // need delete definePermissions and requestPermissions
    ErrCode ret = UpdateDefineAndRequestPermissions(preInfo, oldInfo);
    if (ret != ERR_OK) {
        return;
    }
    APP_LOGD("finish rollback due to install failed");
}

ErrCode BaseBundleInstaller::UpdateDefineAndRequestPermissions(const InnerBundleInfo &oldInfo,
    const InnerBundleInfo &newInfo)
{
    APP_LOGD("UpdateDefineAndRequestPermissions %{public}s start", bundleName_.c_str());
    auto bundleUserInfos = newInfo.GetInnerBundleUserInfos();
    for (const auto &uerInfo : bundleUserInfos) {
        if (uerInfo.second.accessTokenId == 0) {
            continue;
        }
        std::vector<std::string> newRequestPermName;
        if (!BundlePermissionMgr::UpdateDefineAndRequestPermissions(uerInfo.second.accessTokenId, oldInfo,
            newInfo, newRequestPermName)) {
            APP_LOGE("UpdateDefineAndRequestPermissions %{public}s failed", bundleName_.c_str());
            return ERR_APPEXECFWK_INSTALL_UPDATE_HAP_TOKEN_FAILED;
        }
        if (!BundlePermissionMgr::GrantRequestPermissions(newInfo, newRequestPermName, uerInfo.second.accessTokenId)) {
            APP_LOGE("BundlePermissionMgr::GrantRequestPermissions failed %{public}s", bundleName_.c_str());
            return ERR_APPEXECFWK_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED;
        }
    }
    APP_LOGD("UpdateDefineAndRequestPermissions %{public}s end", bundleName_.c_str());
    return ERR_OK;
}

void BaseBundleInstaller::RollBack(const InnerBundleInfo &info, InnerBundleInfo &oldInfo)
{
    // rollback hap installed
    if (installedModules_[info.GetCurrentModulePackage()]) {
        std::string createModulePath = info.GetAppCodePath() + Constants::PATH_SEPARATOR +
            info.GetCurrentModulePackage() + Constants::TMP_SUFFIX;
        RemoveModuleDir(createModulePath);
        oldInfo.SetCurrentModulePackage(info.GetCurrentModulePackage());
        RollBackMoudleInfo(bundleName_, oldInfo);
    } else {
        auto modulePackage = info.GetCurrentModulePackage();
        RemoveModuleDir(info.GetModuleDir(modulePackage));
        // remove module info
        RemoveInfo(bundleName_, modulePackage);
    }
}

void BaseBundleInstaller::RemoveInfo(const std::string &bundleName, const std::string &packageName)
{
    APP_LOGD("remove innerBundleInfo due to rollback");
    if (packageName.empty()) {
        dataMgr_->UpdateBundleInstallState(bundleName, InstallState::UPDATING_FAIL);
    } else {
        InnerBundleInfo innerBundleInfo;
        bool isExist = false;
        if (!GetInnerBundleInfo(innerBundleInfo, isExist) || !isExist) {
            APP_LOGI("finish rollback due to install failed");
            return;
        }
        dataMgr_->UpdateBundleInstallState(bundleName, InstallState::ROLL_BACK);
        dataMgr_->RemoveModuleInfo(bundleName, packageName, innerBundleInfo);
    }
    APP_LOGD("finish to remove innerBundleInfo due to rollback");
}

void BaseBundleInstaller::RollBackMoudleInfo(const std::string &bundleName, InnerBundleInfo &oldInfo)
{
    APP_LOGD("rollBackMoudleInfo due to rollback");
    InnerBundleInfo innerBundleInfo;
    bool isExist = false;
    if (!GetInnerBundleInfo(innerBundleInfo, isExist) || !isExist) {
        return;
    }
    dataMgr_->UpdateBundleInstallState(bundleName, InstallState::ROLL_BACK);
    dataMgr_->UpdateInnerBundleInfo(bundleName, oldInfo, innerBundleInfo);
    APP_LOGD("finsih rollBackMoudleInfo due to rollback");
}

ErrCode BaseBundleInstaller::ProcessBundleUninstall(
    const std::string &bundleName, const InstallParam &installParam, int32_t &uid)
{
    APP_LOGD("start to process %{public}s bundle uninstall", bundleName.c_str());
    if (bundleName.empty()) {
        APP_LOGE("uninstall bundle name empty");
        return ERR_APPEXECFWK_UNINSTALL_INVALID_NAME;
    }

    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr_) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    userId_ = GetUserId(installParam.userId);
    if (userId_ == Constants::INVALID_USERID) {
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    if (!dataMgr_->HasUserId(userId_)) {
        APP_LOGE("The user %{public}d does not exist when uninstall.", userId_);
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    auto &mtx = dataMgr_->GetBundleMutex(bundleName);
    std::lock_guard lock {mtx};
    InnerBundleInfo oldInfo;
    if (!dataMgr_->GetInnerBundleInfo(bundleName, oldInfo)) {
        APP_LOGE("uninstall bundle info missing");
        return ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE;
    }

    versionCode_ = oldInfo.GetVersionCode();
    ScopeGuard enableGuard([&] { dataMgr_->EnableBundle(bundleName); });
    InnerBundleUserInfo curInnerBundleUserInfo;
    if (!oldInfo.GetInnerBundleUserInfo(userId_, curInnerBundleUserInfo)) {
        APP_LOGE("bundle(%{public}s) get user(%{public}d) failed when uninstall.",
            oldInfo.GetBundleName().c_str(), userId_);
        return ERR_APPEXECFWK_USER_NOT_INSTALL_HAP;
    }

    uid = curInnerBundleUserInfo.uid;
    if (!installParam.forceExecuted && oldInfo.GetBaseApplicationInfo().isSystemApp &&
        !oldInfo.IsRemovable() && installParam.noSkipsKill) {
        APP_LOGE("uninstall system app");
        return ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR;
    }

    std::string cloneName;
    if (dataMgr_->GetClonedBundleName(bundleName, cloneName)) {
        APP_LOGI("GetClonedBundleName new name %{public}s ", cloneName.c_str());
        cloneMgr_->RemoveClonedBundle(bundleName, cloneName);
    }

    if (oldInfo.GetInnerBundleUserInfos().size() > 1) {
        APP_LOGD("only delete userinfo %{public}d", userId_);
        return RemoveBundleUserData(oldInfo, installParam.isKeepData);
    }

    if (!dataMgr_->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START)) {
        APP_LOGE("uninstall already start");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    // reboot scan case will not kill the bundle
    if (installParam.noSkipsKill) {
        // kill the bundle process during uninstall.
        if (!AbilityManagerHelper::UninstallApplicationProcesses(oldInfo.GetApplicationName(), uid)) {
            APP_LOGE("can not kill process");
            dataMgr_->UpdateBundleInstallState(bundleName, InstallState::INSTALL_SUCCESS);
            return ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR;
        }
    }

    std::string packageName;
    oldInfo.SetInstallMark(bundleName, packageName, InstallExceptionStatus::UNINSTALL_BUNDLE_START);
    if (!dataMgr_->SaveInnerBundleInfo(oldInfo)) {
        APP_LOGE("save install mark failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }

    ErrCode result = RemoveBundle(oldInfo, installParam.isKeepData);
    if (result != ERR_OK) {
        APP_LOGE("remove whole bundle failed");
        return result;
    }

    enableGuard.Dismiss();
    APP_LOGD("finish to process %{public}s bundle uninstall", bundleName.c_str());
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessBundleUninstall(
    const std::string &bundleName, const std::string &modulePackage, const InstallParam &installParam, int32_t &uid)
{
    APP_LOGD("start to process %{public}s in %{public}s uninstall", bundleName.c_str(), modulePackage.c_str());
    if (bundleName.empty() || modulePackage.empty()) {
        APP_LOGE("uninstall bundle name or module name empty");
        return ERR_APPEXECFWK_UNINSTALL_INVALID_NAME;
    }

    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    cloneMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetCloneMgr();
    if (!dataMgr_) {
        APP_LOGE("Get dataMgr shared_ptr nullptr");
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    userId_ = GetUserId(installParam.userId);
    if (userId_ == Constants::INVALID_USERID) {
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    if (!dataMgr_->HasUserId(userId_)) {
        APP_LOGE("The user %{public}d does not exist when uninstall.", userId_);
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    auto &mtx = dataMgr_->GetBundleMutex(bundleName);
    std::lock_guard lock {mtx};
    InnerBundleInfo oldInfo;
    if (!dataMgr_->GetInnerBundleInfo(bundleName, oldInfo)) {
        APP_LOGE("uninstall bundle info missing");
        return ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_BUNDLE;
    }

    versionCode_ = oldInfo.GetVersionCode();
    ScopeGuard enableGuard([&] { dataMgr_->EnableBundle(bundleName); });
    InnerBundleUserInfo curInnerBundleUserInfo;
    if (!oldInfo.GetInnerBundleUserInfo(userId_, curInnerBundleUserInfo)) {
        APP_LOGE("bundle(%{public}s) get user(%{public}d) failed when uninstall.",
            oldInfo.GetBundleName().c_str(), userId_);
        return ERR_APPEXECFWK_USER_NOT_INSTALL_HAP;
    }

    uid = curInnerBundleUserInfo.uid;
    if (!installParam.forceExecuted && oldInfo.GetBaseApplicationInfo().isSystemApp
        && !oldInfo.IsRemovable() && installParam.noSkipsKill) {
        APP_LOGE("uninstall system app");
        return ERR_APPEXECFWK_UNINSTALL_SYSTEM_APP_ERROR;
    }

    bool isModuleExist = oldInfo.FindModule(modulePackage);
    if (!isModuleExist) {
        APP_LOGE("uninstall bundle info missing");
        return ERR_APPEXECFWK_UNINSTALL_MISSING_INSTALLED_MODULE;
    }

    if (!dataMgr_->UpdateBundleInstallState(bundleName, InstallState::UNINSTALL_START)) {
        APP_LOGE("uninstall already start");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    ScopeGuard stateGuard([&] { dataMgr_->UpdateBundleInstallState(bundleName, InstallState::INSTALL_SUCCESS); });

    // reboot scan case will not kill the bundle
    if (installParam.noSkipsKill) {
        // kill the bundle process during uninstall.
        if (!AbilityManagerHelper::UninstallApplicationProcesses(oldInfo.GetApplicationName(), uid)) {
            APP_LOGE("can not kill process");
            return ERR_APPEXECFWK_UNINSTALL_KILLING_APP_ERROR;
        }
    }

    oldInfo.SetInstallMark(bundleName, modulePackage, InstallExceptionStatus::UNINSTALL_PACKAGE_START);
    if (!dataMgr_->SaveInnerBundleInfo(oldInfo)) {
        APP_LOGE("save install mark failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }

    bool onlyInstallInUser = oldInfo.GetInnerBundleUserInfos().size() == 1;
    // if it is the only module in the bundle
    if (oldInfo.IsOnlyModule(modulePackage)) {
        APP_LOGI("%{public}s is only module", modulePackage.c_str());
        std::string cloneName;
        if (dataMgr_->GetClonedBundleName(bundleName, cloneName)) {
            APP_LOGI("GetClonedBundleName new name %{public}s ", cloneName.c_str());
            cloneMgr_->RemoveClonedBundle(bundleName, cloneName);
        }

        enableGuard.Dismiss();
        stateGuard.Dismiss();
        if (onlyInstallInUser) {
            return RemoveBundle(oldInfo, installParam.isKeepData);
        }

        return RemoveBundleUserData(oldInfo, installParam.isKeepData);
    }

    ErrCode result = ERR_OK;
    if (onlyInstallInUser) {
        APP_LOGI("%{public}s is only install at the userId %{public}d", bundleName.c_str(), userId_);
        result = RemoveModuleAndDataDir(oldInfo, modulePackage, userId_, installParam.isKeepData);
    } else {
        if (!installParam.isKeepData) {
            result = RemoveModuleDataDir(oldInfo, modulePackage, userId_);
        }
    }

    if (result != ERR_OK) {
        APP_LOGE("remove module dir failed");
        return result;
    }

    oldInfo.SetInstallMark(bundleName, modulePackage, InstallExceptionStatus::INSTALL_FINISH);
    APP_LOGD("start to remove module info of %{public}s in %{public}s ", modulePackage.c_str(), bundleName.c_str());
    if (!dataMgr_->RemoveModuleInfo(bundleName, modulePackage, oldInfo)) {
        APP_LOGE("RemoveModuleInfo failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    APP_LOGD("finish to process %{public}s in %{public}s uninstall", bundleName.c_str(), modulePackage.c_str());
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessInstallBundleByBundleName(
    const std::string &bundleName, const InstallParam &installParam, int32_t &uid)
{
    APP_LOGD("Process Install Bundle(%{public}s) start", bundleName.c_str());
    return InnerProcessInstallByPreInstallInfo(bundleName, installParam, uid, false);
}

ErrCode BaseBundleInstaller::ProcessRecover(
    const std::string &bundleName, const InstallParam &installParam, int32_t &uid)
{
    APP_LOGD("Process Recover Bundle(%{public}s) start", bundleName.c_str());
    return InnerProcessInstallByPreInstallInfo(bundleName, installParam, uid, true);
}

ErrCode BaseBundleInstaller::InnerProcessInstallByPreInstallInfo(
    const std::string &bundleName, const InstallParam &installParam, int32_t &uid, bool recoverMode)
{
    dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr_) {
        APP_LOGE("Get dataMgr shared_ptr nullptr.");
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    userId_ = GetUserId(installParam.userId);
    if (userId_ == Constants::INVALID_USERID) {
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    if (!dataMgr_->HasUserId(userId_)) {
        APP_LOGE("The user %{public}d does not exist.", userId_);
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    {
        auto &mtx = dataMgr_->GetBundleMutex(bundleName);
        std::lock_guard lock {mtx};
        InnerBundleInfo oldInfo;
        bool isAppExist = dataMgr_->GetInnerBundleInfo(bundleName, oldInfo);
        if (isAppExist) {
            dataMgr_->EnableBundle(bundleName);
            versionCode_ = oldInfo.GetVersionCode();
            if (oldInfo.HasInnerBundleUserInfo(userId_)) {
                APP_LOGE("App is exist in user(%{public}d).", userId_);
                return ERR_APPEXECFWK_INSTALL_ALREADY_EXIST;
            }

            bool isSingleton = oldInfo.IsSingleton();
            if ((isSingleton && (userId_ != Constants::DEFAULT_USERID)) ||
                (!isSingleton && (userId_ == Constants::DEFAULT_USERID))) {
                APP_LOGW("singleton(%{public}d) app(%{public}s) and user(%{public}d) are not matched.",
                    isSingleton, bundleName_.c_str(), userId_);
                return ERR_APPEXECFWK_INSTALL_ZERO_USER_WITH_NO_SINGLETON;
            }

            InnerBundleUserInfo curInnerBundleUserInfo;
            curInnerBundleUserInfo.bundleUserInfo.userId = userId_;
            curInnerBundleUserInfo.bundleName = bundleName;
            oldInfo.AddInnerBundleUserInfo(curInnerBundleUserInfo);
            ScopeGuard userGuard([&] { RemoveBundleUserData(oldInfo, false); });
            uint32_t tokenId = CreateAccessTokenId(oldInfo);
            oldInfo.SetAccessTokenId(tokenId, userId_);
            ErrCode result = GrantRequestPermissions(oldInfo, tokenId);
            if (result != ERR_OK) {
                return result;
            }

            result = CreateBundleUserData(oldInfo);
            if (result != ERR_OK) {
                return result;
            }

            userGuard.Dismiss();
            uid = oldInfo.GetUid(userId_);
            return ERR_OK;
        }
    }

    PreInstallBundleInfo preInstallBundleInfo;
    preInstallBundleInfo.SetBundleName(bundleName);
    if (!dataMgr_->GetPreInstallBundleInfo(bundleName, preInstallBundleInfo)
        || preInstallBundleInfo.GetBundlePaths().empty()) {
        APP_LOGE("Get PreInstallBundleInfo faile, bundleName: %{public}s.", bundleName.c_str());
        return ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE;
    }

    if (recoverMode && preInstallBundleInfo.GetAppType() != Constants::AppType::SYSTEM_APP) {
        APP_LOGE("recover failed due to not system app");
        return ERR_APPEXECFWK_RECOVER_GET_BUNDLEPATH_ERROR;
    }

    APP_LOGD("Get preInstall bundlePath success.");
    std::vector<std::string> pathVec { preInstallBundleInfo.GetBundlePaths() };
    auto innerInstallParam = installParam;
    innerInstallParam.isPreInstallApp = true;
    return ProcessBundleInstall(pathVec, innerInstallParam, preInstallBundleInfo.GetAppType(), uid);
}

ErrCode BaseBundleInstaller::RemoveBundle(InnerBundleInfo &info, bool isKeepData)
{
    ErrCode result = RemoveBundleAndDataDir(info, isKeepData);
    if (result != ERR_OK) {
        APP_LOGE("remove bundle dir failed");
        dataMgr_->UpdateBundleInstallState(info.GetBundleName(), InstallState::UNINSTALL_FAIL);
        return result;
    }

    if (!dataMgr_->UpdateBundleInstallState(info.GetBundleName(), InstallState::UNINSTALL_SUCCESS)) {
        APP_LOGE("delete inner info failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    if (BundlePermissionMgr::DeleteAccessTokenId(info.GetAccessTokenId(userId_)) !=
        AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("delete accessToken failed");
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessBundleInstallStatus(InnerBundleInfo &info, int32_t &uid)
{
    if (!verifyUriPrefix(info, userId_)) {
        APP_LOGE("verifyUriPrefix failed");
        return ERR_APPEXECFWK_INSTALL_URI_DUPLICATE;
    }
    modulePackage_ = info.GetCurrentModulePackage();
    APP_LOGD("ProcessBundleInstallStatus with bundleName %{public}s and packageName %{public}s",
        bundleName_.c_str(), modulePackage_.c_str());
    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::INSTALL_START)) {
        APP_LOGE("install already start");
        return ERR_APPEXECFWK_INSTALL_STATE_ERROR;
    }
    info.SetInstallMark(bundleName_, modulePackage_, InstallExceptionStatus::INSTALL_START);
    if (!dataMgr_->SaveInnerBundleInfo(info)) {
        APP_LOGE("save install mark to storage failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    ScopeGuard stateGuard([&] { dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::INSTALL_FAIL); });
    ErrCode result = CreateBundleAndDataDir(info);
    if (result != ERR_OK) {
        APP_LOGE("create bundle and data dir failed");
        return result;
    }

    ScopeGuard bundleGuard([&] { RemoveBundleAndDataDir(info, false); });
    std::string modulePath = info.GetAppCodePath() + Constants::PATH_SEPARATOR + modulePackage_;
    result = ExtractModule(info, modulePath);
    if (result != ERR_OK) {
        APP_LOGE("extract module failed");
        return result;
    }

    info.SetInstallMark(bundleName_, modulePackage_, InstallExceptionStatus::INSTALL_FINISH);
    uid = info.GetUid(userId_);
    info.SetBundleInstallTime(BundleUtil::GetCurrentTime(), userId_);
    uint32_t tokenId = CreateAccessTokenId(info);
    info.SetAccessTokenId(tokenId, userId_);
    result = GrantRequestPermissions(info, tokenId);
    if (result != ERR_OK) {
        return result;
    }
    if (!dataMgr_->AddInnerBundleInfo(bundleName_, info)) {
        APP_LOGE("add bundle %{public}s info failed", bundleName_.c_str());
        dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UNINSTALL_START);
        dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UNINSTALL_SUCCESS);
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    stateGuard.Dismiss();
    bundleGuard.Dismiss();

    APP_LOGD("finish to call processBundleInstallStatus");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessBundleUpdateStatus(
    InnerBundleInfo &oldInfo, InnerBundleInfo &newInfo, bool isReplace, bool noSkipsKill)
{
    modulePackage_ = newInfo.GetCurrentModulePackage();
    if (modulePackage_.empty()) {
        APP_LOGE("get current package failed");
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    if (isFeatureNeedUninstall_) {
        uninstallModuleVec_.emplace_back(modulePackage_);
    }

    if (oldInfo.IsSingleton() && !newInfo.IsSingleton()) {
        singletonState_ = SingletonState::SINGLETON_TO_NON;
    } else if (!oldInfo.IsSingleton() && newInfo.IsSingleton()) {
        singletonState_ = SingletonState::NON_TO_SINGLETON;
    }

    APP_LOGD("ProcessBundleUpdateStatus with bundleName %{public}s and packageName %{public}s",
        newInfo.GetBundleName().c_str(), modulePackage_.c_str());
    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UPDATING_START)) {
        APP_LOGE("update already start");
        return ERR_APPEXECFWK_INSTALL_STATE_ERROR;
    }

    if (oldInfo.GetProvisionId() != newInfo.GetProvisionId()) {
        APP_LOGE("the signature of the new bundle is not the same as old one");
        return ERR_APPEXECFWK_INSTALL_FAILED_INCONSISTENT_SIGNATURE;
    }
    APP_LOGD("ProcessBundleUpdateStatus noSkipsKill = %{public}d", noSkipsKill);
    // now there are two cases for updating:
    // 1. bundle exist, hap exist, update hap
    // 2. bundle exist, install new hap
    bool isModuleExist = oldInfo.FindModule(modulePackage_);
    newInfo.RestoreFromOldInfo(oldInfo);
    auto result = isModuleExist ? ProcessModuleUpdate(newInfo, oldInfo,
        isReplace, noSkipsKill) : ProcessNewModuleInstall(newInfo, oldInfo);
    if (result != ERR_OK) {
        APP_LOGE("install module failed %{public}d", result);
        return result;
    }

    APP_LOGD("finish to call ProcessBundleUpdateStatus");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessNewModuleInstall(InnerBundleInfo &newInfo, InnerBundleInfo &oldInfo)
{
    APP_LOGD("ProcessNewModuleInstall %{public}s, userId: %{public}d.",
        newInfo.GetBundleName().c_str(), userId_);
    if (!verifyUriPrefix(newInfo, userId_)) {
        APP_LOGE("verifyUriPrefix failed");
        return ERR_APPEXECFWK_INSTALL_URI_DUPLICATE;
    }

    if (newInfo.HasEntry() && oldInfo.HasEntry()) {
        APP_LOGE("install more than one entry module");
        return ERR_APPEXECFWK_INSTALL_ENTRY_ALREADY_EXIST;
    }

    // same version need to check app label
    ErrCode result = ERR_OK;
    if (oldInfo.GetVersionCode() == newInfo.GetVersionCode()) {
        result = CheckAppLabel(oldInfo, newInfo);
        if (result != ERR_OK) {
            APP_LOGE("CheckAppLabel failed %{public}d", result);
            return result;
        }
    }

    oldInfo.SetInstallMark(bundleName_, modulePackage_, InstallExceptionStatus::UPDATING_NEW_START);
    if (!dataMgr_->SaveInnerBundleInfo(oldInfo)) {
        APP_LOGE("save install mark failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    std::string modulePath = newInfo.GetAppCodePath() + Constants::PATH_SEPARATOR + modulePackage_;
    result = ExtractModule(newInfo, modulePath);
    if (result != ERR_OK) {
        APP_LOGE("extract module and rename failed");
        return result;
    }
    ScopeGuard moduleGuard([&] { RemoveModuleDir(modulePath); });
    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UPDATING_SUCCESS)) {
        APP_LOGE("new moduleupdate state failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    oldInfo.SetInstallMark(bundleName_, modulePackage_, InstallExceptionStatus::INSTALL_FINISH);

    auto bundleUserInfos = oldInfo.GetInnerBundleUserInfos();
    for (const auto &info : bundleUserInfos) {
        if (info.second.accessTokenId == 0) {
            continue;
        }
        std::vector<std::string> newRequestPermName;
        if (!BundlePermissionMgr::AddDefineAndRequestPermissions(info.second.accessTokenId, newInfo,
            newRequestPermName)) {
            APP_LOGE("BundlePermissionMgr::AddDefineAndRequestPermissions failed %{public}s", bundleName_.c_str());
            return ERR_APPEXECFWK_INSTALL_UPDATE_HAP_TOKEN_FAILED;
        }
        if (!BundlePermissionMgr::GrantRequestPermissions(newInfo, newRequestPermName, info.second.accessTokenId)) {
            APP_LOGE("BundlePermissionMgr::GrantRequestPermissions failed %{public}s", bundleName_.c_str());
            return ERR_APPEXECFWK_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED;
        }
    }

    oldInfo.SetBundleUpdateTime(BundleUtil::GetCurrentTime(), userId_);
    if (!dataMgr_->AddNewModuleInfo(bundleName_, newInfo, oldInfo)) {
        APP_LOGE(
            "add module %{public}s to innerBundleInfo %{public}s failed", modulePackage_.c_str(), bundleName_.c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    moduleGuard.Dismiss();
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ProcessModuleUpdate(InnerBundleInfo &newInfo,
    InnerBundleInfo &oldInfo, bool isReplace, bool noSkipsKill)
{
    APP_LOGD("ProcessModuleUpdate, bundleName : %{public}s, moduleName : %{public}s, userId: %{public}d.",
        newInfo.GetBundleName().c_str(), newInfo.GetCurrentModulePackage().c_str(), userId_);
    if (!verifyUriPrefix(newInfo, userId_, true)) {
        APP_LOGE("verifyUriPrefix failed");
        return ERR_APPEXECFWK_INSTALL_URI_DUPLICATE;
    }

    ErrCode result = ERR_OK;
    if (versionCode_ == oldInfo.GetVersionCode()) {
        if ((result = CheckAppLabel(oldInfo, newInfo)) != ERR_OK) {
            APP_LOGE("CheckAppLabel failed %{public}d", result);
            return result;
        }

        if (!isReplace) {
            if (hasInstalledInUser_) {
                APP_LOGE("fail to install already existing bundle using normal flag");
                return ERR_APPEXECFWK_INSTALL_ALREADY_EXIST;
            }

            // app versionCode equals to the old and do not need to update module
            // and only need to update userInfo
            newInfo.SetOnlyCreateBundleUser(true);
            if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UPDATING_SUCCESS)) {
                APP_LOGE("update state failed");
                return ERR_APPEXECFWK_INSTALL_STATE_ERROR;
            }
            return ERR_OK;
        }
    }

    APP_LOGE("ProcessModuleUpdate noSkipsKill = %{public}d", noSkipsKill);
    // reboot scan case will not kill the bundle
    if (noSkipsKill) {
        // kill the bundle process during updating
        if (!AbilityManagerHelper::UninstallApplicationProcesses(
            oldInfo.GetApplicationName(), oldInfo.GetUid(userId_))) {
            APP_LOGE("fail to kill running application");
            return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
        }
    }

    oldInfo.SetInstallMark(bundleName_, modulePackage_, InstallExceptionStatus::UPDATING_EXISTED_START);
    if (!dataMgr_->SaveInnerBundleInfo(oldInfo)) {
        APP_LOGE("save install mark failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }

    moduleTmpDir_ = newInfo.GetAppCodePath() + Constants::PATH_SEPARATOR + modulePackage_ + Constants::TMP_SUFFIX;
    result = ExtractModule(newInfo, moduleTmpDir_);
    if (result != ERR_OK) {
        APP_LOGE("extract module and rename failed");
        return result;
    }

    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UPDATING_SUCCESS)) {
        APP_LOGE("old module update state failed");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    newInfo.RestoreModuleInfo(oldInfo);
    oldInfo.SetInstallMark(bundleName_, modulePackage_, InstallExceptionStatus::UPDATING_FINISH);
    oldInfo.SetBundleUpdateTime(BundleUtil::GetCurrentTime(), userId_);
    auto noUpdateInfo = oldInfo;
    if (!dataMgr_->UpdateInnerBundleInfo(bundleName_, newInfo, oldInfo)) {
        APP_LOGE("update innerBundleInfo %{public}s failed", bundleName_.c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    auto bundleUserInfos = oldInfo.GetInnerBundleUserInfos();
    for (const auto &info : bundleUserInfos) {
        if (info.second.accessTokenId == 0) {
            continue;
        }

        std::vector<std::string> newRequestPermName;
        if (!BundlePermissionMgr::UpdateDefineAndRequestPermissions(info.second.accessTokenId, noUpdateInfo,
            oldInfo, newRequestPermName)) {
            APP_LOGE("UpdateDefineAndRequestPermissions %{public}s failed", bundleName_.c_str());
            return ERR_APPEXECFWK_INSTALL_UPDATE_HAP_TOKEN_FAILED;
        }

        if (!BundlePermissionMgr::GrantRequestPermissions(oldInfo, newRequestPermName, info.second.accessTokenId)) {
            APP_LOGE("BundlePermissionMgr::GrantRequestPermissions failed %{public}s", bundleName_.c_str());
            return ERR_APPEXECFWK_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED;
        }
    }

    ErrCode ret = SetDirApl(newInfo);
    if (ret != ERR_OK) {
        APP_LOGE("SetDirApl failed");
        return ret;
    }

    return ERR_OK;
}

ErrCode BaseBundleInstaller::SetDirApl(const InnerBundleInfo &info)
{
    for (const auto &el : Constants::BUNDLE_EL) {
        std::string baseBundleDataDir = Constants::BUNDLE_APP_DATA_BASE_DIR +
                                        el +
                                        Constants::FILE_SEPARATOR_CHAR +
                                        std::to_string(userId_);
        std::string baseDataDir = baseBundleDataDir + Constants::BASE + info.GetBundleName();
        ErrCode result = InstalldClient::GetInstance()->SetDirApl(
            baseDataDir, info.GetBundleName(), info.GetAppPrivilegeLevel());
        if (result != ERR_OK) {
            APP_LOGE("fail to SetDirApl baseDir dir, error is %{public}d", result);
            return result;
        }
        std::string databaseDataDir = baseBundleDataDir + Constants::DATABASE + info.GetBundleName();
        result = InstalldClient::GetInstance()->SetDirApl(
            databaseDataDir, info.GetBundleName(), info.GetAppPrivilegeLevel());
        if (result != ERR_OK) {
            APP_LOGE("fail to SetDirApl databaseDir dir, error is %{public}d", result);
            return result;
        }
    }

    return ERR_OK;
}

ErrCode BaseBundleInstaller::CreateBundleAndDataDir(InnerBundleInfo &info) const
{
    ErrCode result = CreateBundleCodeDir(info);
    if (result != ERR_OK) {
        APP_LOGE("fail to create bundle code dir, error is %{public}d", result);
        return result;
    }
    ScopeGuard codePathGuard([&] { InstalldClient::GetInstance()->RemoveDir(info.GetAppCodePath()); });
    result = CreateBundleDataDir(info);
    if (result != ERR_OK) {
        APP_LOGE("fail to create bundle data dir, error is %{public}d", result);
        return result;
    }
    codePathGuard.Dismiss();
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CreateBundleCodeDir(InnerBundleInfo &info) const
{
    auto appCodePath = Constants::BUNDLE_CODE_DIR + Constants::PATH_SEPARATOR + bundleName_;
    APP_LOGD("create bundle dir %{private}s", appCodePath.c_str());
    ErrCode result = InstalldClient::GetInstance()->CreateBundleDir(appCodePath);
    if (result != ERR_OK) {
        APP_LOGE("fail to create bundle dir, error is %{public}d", result);
        return result;
    }

    info.SetAppCodePath(appCodePath);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CreateBundleDataDir(InnerBundleInfo &info) const
{
    InnerBundleUserInfo newInnerBundleUserInfo;
    if (!info.GetInnerBundleUserInfo(userId_, newInnerBundleUserInfo)) {
        APP_LOGE("bundle(%{public}s) get user(%{public}d) failed.",
            info.GetBundleName().c_str(), userId_);
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    if (!dataMgr_->GenerateUidAndGid(newInnerBundleUserInfo)) {
        APP_LOGE("fail to gererate uid and gid");
        return ERR_APPEXECFWK_INSTALL_GENERATE_UID_ERROR;
    }

    auto result = InstalldClient::GetInstance()->CreateBundleDataDir(info.GetBundleName(), userId_,
        newInnerBundleUserInfo.uid, newInnerBundleUserInfo.uid, info.GetAppPrivilegeLevel());
    if (result != ERR_OK) {
        APP_LOGE("fail to create bundle data dir, error is %{public}d", result);
        return result;
    }

    std::string dataBaseDir = Constants::BUNDLE_APP_DATA_BASE_DIR + Constants::BUNDLE_EL[1] +
        Constants::DATABASE + info.GetBundleName();
    info.SetAppDataBaseDir(dataBaseDir);
    info.AddInnerBundleUserInfo(newInnerBundleUserInfo);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ExtractModule(InnerBundleInfo &info, const std::string &modulePath)
{
    std::string targetSoPath;
    std::string nativeLibraryPath = info.GetBaseApplicationInfo().nativeLibraryPath;
    if (!nativeLibraryPath.empty()) {
        targetSoPath.append(Constants::BUNDLE_CODE_DIR).append(Constants::PATH_SEPARATOR)
            .append(info.GetBundleName()).append(Constants::PATH_SEPARATOR)
            .append(nativeLibraryPath).append(Constants::PATH_SEPARATOR);
    }
    std::string cpuAbi = info.GetBaseApplicationInfo().cpuAbi;
    APP_LOGD("begin to extract module files, modulePath : %{private}s, targetSoPath : %{private}s, cpuAbi : %{public}s",
        modulePath.c_str(), targetSoPath.c_str(), cpuAbi.c_str());
    auto result = ExtractModuleFiles(info, modulePath, targetSoPath, cpuAbi);
    if (result != ERR_OK) {
        APP_LOGE("fail to extrace module dir, error is %{public}d", result);
        return result;
    }

    if (info.IsPreInstallApp()) {
        info.SetModuleHapPath(modulePath_);
    } else {
        info.SetModuleHapPath(GetHapPath(info));
    }

    auto moduleDir = info.GetAppCodePath() + Constants::PATH_SEPARATOR + info.GetCurrentModulePackage();
    info.AddModuleSrcDir(moduleDir);
    info.AddModuleResPath(moduleDir);
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RemoveBundleAndDataDir(const InnerBundleInfo &info, bool isKeepData) const
{
    // remove bundle dir
    auto result = RemoveBundleCodeDir(info);
    if (result != ERR_OK) {
        APP_LOGE("fail to remove bundle dir %{private}s, error is %{public}d", info.GetAppCodePath().c_str(), result);
        return result;
    }
    if (!isKeepData) {
        result = RemoveBundleDataDir(info);
        if (result != ERR_OK) {
            APP_LOGE("fail to remove bundleData dir %{private}s, error is %{public}d",
                info.GetBundleName().c_str(), result);
        }
    }
    return result;
}

ErrCode BaseBundleInstaller::RemoveBundleCodeDir(const InnerBundleInfo &info) const
{
    auto result = InstalldClient::GetInstance()->RemoveDir(info.GetAppCodePath());
    if (result != ERR_OK) {
        APP_LOGE("fail to remove bundle code dir %{public}s, error is %{public}d",
            info.GetAppCodePath().c_str(), result);
    }
    return result;
}

ErrCode BaseBundleInstaller::RemoveBundleDataDir(const InnerBundleInfo &info) const
{
    ErrCode result =
        InstalldClient::GetInstance()->RemoveBundleDataDir(info.GetBundleName(), userId_);
    if (result != ERR_OK) {
        APP_LOGE("fail to remove bundleName: %{public}s, error is %{public}d",
            info.GetBundleName().c_str(), result);
    }
    return result;
}

ErrCode BaseBundleInstaller::RemoveModuleAndDataDir(
    const InnerBundleInfo &info, const std::string &modulePackage, int32_t userId, bool isKeepData) const
{
    APP_LOGD("RemoveModuleAndDataDir with package name %{public}s", modulePackage.c_str());
    auto moduleDir = info.GetModuleDir(modulePackage);
    auto result = RemoveModuleDir(moduleDir);
    if (result != ERR_OK) {
        APP_LOGE("fail to remove module dir, error is %{public}d", result);
        return result;
    }

    // remove hap
    result = RemoveModuleDir(GetHapPath(info, info.GetModuleName(modulePackage)));
    if (result != ERR_OK) {
        APP_LOGE("fail to remove module hap, error is %{public}d", result);
        return result;
    }

    if (!isKeepData) {
        // uninstall hap remove current userId data dir
        if (userId != Constants::UNSPECIFIED_USERID) {
            RemoveModuleDataDir(info, modulePackage, userId);
            return ERR_OK;
        }

        // updata hap remove all lower version data dir
        for (auto infoItem : info.GetInnerBundleUserInfos()) {
            int32_t installedUserId = infoItem.second.bundleUserInfo.userId;
            RemoveModuleDataDir(info, modulePackage, installedUserId);
        }
    }
    APP_LOGD("RemoveModuleAndDataDir successfully");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RemoveModuleDir(const std::string &modulePath) const
{
    APP_LOGD("module dir %{private}s to be removed", modulePath.c_str());
    return InstalldClient::GetInstance()->RemoveDir(modulePath);
}

ErrCode BaseBundleInstaller::RemoveModuleDataDir(
    const InnerBundleInfo &info, const std::string &modulePackage, int32_t userId) const
{
    APP_LOGD("RemoveModuleDataDir bundleName: %{public}s  modulePackage: %{public}s",
             info.GetBundleName().c_str(),
             modulePackage.c_str());
    auto hapModuleInfo = info.FindHapModuleInfo(modulePackage);
    if (!hapModuleInfo) {
        APP_LOGE("fail to findHapModule info modulePackage: %{public}s", modulePackage.c_str());
        return ERR_NO_INIT;
    }
    std::string moduleDataDir = info.GetBundleName() + Constants::HAPS + (*hapModuleInfo).moduleName;
    APP_LOGD("RemoveModuleDataDir moduleDataDir: %{public}s", moduleDataDir.c_str());
    auto result = InstalldClient::GetInstance()->RemoveModuleDataDir(moduleDataDir, userId);
    if (result != ERR_OK) {
        APP_LOGE("fail to remove HapModuleData dir, error is %{public}d", result);
    }
    return result;
}

ErrCode BaseBundleInstaller::ParseBundleInfo(const std::string &bundleFilePath, InnerBundleInfo &info,
    BundlePackInfo &packInfo) const
{
    BundleParser bundleParser;
    ErrCode result = bundleParser.Parse(bundleFilePath, info);
    if (result != ERR_OK) {
        APP_LOGE("parse bundle info failed, error: %{public}d", result);
        return result;
    }
    if (!packInfo.GetValid()) {
        result = bundleParser.ParsePackInfo(bundleFilePath, packInfo);
        if (result != ERR_OK) {
            APP_LOGE("parse bundle pack info failed, error: %{public}d", result);
            return result;
        }
        info.SetBundlePackInfo(packInfo);
        packInfo.SetValid(true);
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ExtractModuleFiles(const InnerBundleInfo &info, const std::string &modulePath,
    const std::string &targetSoPath, const std::string &cpuAbi)
{
    APP_LOGD("extract module to %{private}s", modulePath.c_str());
    auto result = InstalldClient::GetInstance()->ExtractModuleFiles(modulePath_, modulePath, targetSoPath, cpuAbi);
    if (result != ERR_OK) {
        APP_LOGE("extract module files failed, error is %{public}d", result);
        return result;
    }

    return ERR_OK;
}

ErrCode BaseBundleInstaller::RenameModuleDir(const InnerBundleInfo &info) const
{
    auto moduleDir = info.GetAppCodePath() + Constants::PATH_SEPARATOR + info.GetCurrentModulePackage();
    APP_LOGD("rename module to %{public}s", moduleDir.c_str());
    auto result = InstalldClient::GetInstance()->RenameModuleDir(moduleDir + Constants::TMP_SUFFIX, moduleDir);
    if (result != ERR_OK) {
        APP_LOGE("rename module dir failed, error is %{public}d", result);
        return result;
    }
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CheckSysCap(const std::vector<std::string> &bundlePaths)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    APP_LOGD("check hap syscaps start.");
    if (bundlePaths.empty()) {
        APP_LOGE("check hap syscaps failed due to empty bundlePaths!");
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }

    ErrCode result = ERR_OK;
    BundleParser bundleParser;
    for (auto bundlePath : bundlePaths) {
        std::vector<std::string> bundleSysCaps;
        result = bundleParser.ParseSysCap(bundlePath, bundleSysCaps);
        if (result != ERR_OK) {
            APP_LOGE("parse bundle syscap failed, error: %{public}d", result);
            return result;
        }

        for (auto bundleSysCapItem : bundleSysCaps) {
            APP_LOGD("check syscap(%{public}s)", bundleSysCapItem.c_str());
            if (!HasSystemCapability(bundleSysCapItem.c_str())) {
                APP_LOGE("check syscap failed which %{public}s is not exsit",
                    bundleSysCapItem.c_str());
                return ERR_APPEXECFWK_INSTALL_CHECK_SYSCAP_FAILED;
            }
        }
    }

    APP_LOGD("finish check hap syscaps");
    return result;
}

ErrCode BaseBundleInstaller::CheckMultipleHapsSignInfo(const std::vector<std::string> &bundlePaths,
    const InstallParam &installParam, std::vector<Security::Verify::HapVerifyResult>& hapVerifyRes) const
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    APP_LOGD("Check multiple haps signInfo");
    if (bundlePaths.empty()) {
        APP_LOGE("check hap sign info failed due to empty bundlePaths!");
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }
    for (const std::string &bundlePath : bundlePaths) {
        Security::Verify::HapVerifyResult hapVerifyResult;
        auto verifyRes = BundleVerifyMgr::HapVerify(bundlePath, hapVerifyResult);
        if (verifyRes != ERR_OK) {
            APP_LOGE("hap file verify failed");
            return verifyRes;
        }
        hapVerifyRes.emplace_back(hapVerifyResult);
    }
    if (hapVerifyRes.empty()) {
        APP_LOGE("no sign info in the all haps!");
        return ERR_APPEXECFWK_INSTALL_FAILED_INCOMPATIBLE_SIGNATURE;
    }
    auto appId = hapVerifyRes[0].GetProvisionInfo().appId;
    APP_LOGD("bundle appid is %{private}s", appId.c_str());
    auto isValid = std::any_of(hapVerifyRes.begin(), hapVerifyRes.end(), [&](auto &hapVerifyResult) {
        APP_LOGD("module appid is %{private}s", hapVerifyResult.GetProvisionInfo().appId.c_str());
        return appId != hapVerifyResult.GetProvisionInfo().appId;
    });
    if (isValid) {
        APP_LOGE("hap files have different signuature info");
        return ERR_APPEXECFWK_INSTALL_FAILED_INCONSISTENT_SIGNATURE;
    }
    auto apl = hapVerifyRes[0].GetProvisionInfo().bundleInfo.apl;
    APP_LOGD("bundle apl is %{public}s", apl.c_str());
    isValid = std::any_of(hapVerifyRes.begin(), hapVerifyRes.end(), [&](auto &hapVerifyResult) {
        APP_LOGD("module appid is %{private}s", hapVerifyResult.GetProvisionInfo().bundleInfo.apl.c_str());
        return apl != hapVerifyResult.GetProvisionInfo().bundleInfo.apl;
    });
    if (isValid) {
        APP_LOGE("hap files have different apl info");
        return ERR_APPEXECFWK_INSTALL_FAILED_INCONSISTENT_SIGNATURE;
    }
    APP_LOGD("finish check multiple haps signInfo");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::ParseHapFiles(const std::vector<std::string> &bundlePaths,
    const InstallParam &installParam, const Constants::AppType appType,
    std::vector<Security::Verify::HapVerifyResult> &hapVerifyRes,
    std::unordered_map<std::string, InnerBundleInfo> &infos)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    APP_LOGD("Parse hap file");
    ErrCode result = ERR_OK;
    BundlePackInfo packInfo;
    for (uint32_t i = 0; i < bundlePaths.size(); ++i) {
        InnerBundleInfo newInfo;
        newInfo.SetAppType(appType);
        Security::Verify::ProvisionInfo provisionInfo = hapVerifyRes[i].GetProvisionInfo();
        bool isSystemApp = (provisionInfo.bundleInfo.appFeature == Constants::HOS_SYSTEM_APP ||
            provisionInfo.bundleInfo.appFeature == Constants::OHOS_SYSTEM_APP);
        if (isSystemApp) {
            newInfo.SetAppType(Constants::AppType::SYSTEM_APP);
        }
        newInfo.SetUserId(installParam.userId);
        newInfo.SetIsPreInstallApp(installParam.isPreInstallApp);
        result = ParseBundleInfo(bundlePaths[i], newInfo, packInfo);
        if (result != ERR_OK) {
            APP_LOGE("bundle parse failed %{public}d", result);
            return result;
        }
        if (newInfo.HasEntry()) {
            if (isContainEntry_) {
                APP_LOGE("more than one entry hap in the direction!");
                return ERR_APPEXECFWK_INSTALL_INVALID_NUMBER_OF_ENTRY_HAP;
            } else {
                isContainEntry_ = true;
            }
        }
        SetEntryInstallationFree(packInfo, newInfo);
        newInfo.SetProvisionId(provisionInfo.appId);
        newInfo.SetAppFeature(provisionInfo.bundleInfo.appFeature);
        newInfo.SetAppPrivilegeLevel(provisionInfo.bundleInfo.apl);
        newInfo.SetAllowedAcls(provisionInfo.acls.allowedAcls);

        if ((result = CheckSystemSize(bundlePaths[i], appType)) != ERR_OK) {
            APP_LOGE("install failed due to insufficient disk memory");
            return result;
        }

        infos.emplace(bundlePaths[i], newInfo);
    }
    APP_LOGD("finish parse hap file");
    return result;
}

ErrCode BaseBundleInstaller::CheckHapHashParams(
    std::unordered_map<std::string, InnerBundleInfo> &infos,
    std::map<std::string, std::string> hashParams)
{
    if (hashParams.empty()) {
        APP_LOGD("hashParams is empty");
        return ERR_OK;
    }

    std::vector<std::string> hapModuleNames;
    for (auto &info : infos) {
        std::vector<std::string> moduleNames;
        info.second.GetModuleNames(moduleNames);
        if (moduleNames.empty()) {
            APP_LOGE("hap(%{public}s) moduleName is empty", info.first.c_str());
            return ERR_APPEXECFWK_INSTALL_FAILED_MODULE_NAME_EMPTY;
        }

        if (std::find(hapModuleNames.begin(), hapModuleNames.end(), moduleNames[0]) != hapModuleNames.end()) {
            APP_LOGE("hap moduleName(%{public}s) duplicate", moduleNames[0].c_str());
            return ERR_APPEXECFWK_INSTALL_FAILED_MODULE_NAME_DUPLICATE;
        }

        hapModuleNames.emplace_back(moduleNames[0]);
        auto hashParamIter = hashParams.find(moduleNames[0]);
        if (hashParamIter != hashParams.end()) {
            info.second.SetModuleHashValue(hashParamIter->second);
            hashParams.erase(hashParamIter);
        }
    }

    if (!hashParams.empty()) {
        APP_LOGE("Some hashParam moduleName is not exist in hap moduleNames");
        return ERR_APPEXECFWK_INSTALL_FAILED_CHECK_HAP_HASH_PARAM;
    }

    return ERR_OK;
}

ErrCode BaseBundleInstaller::CheckAppLabelInfo(const std::unordered_map<std::string, InnerBundleInfo> &infos)
{
    APP_LOGD("Check APP label");
    ErrCode ret = ERR_OK;
    std::string bundleName = (infos.begin()->second).GetBundleName();
    std::string vendor = (infos.begin()->second).GetVendor();
    versionCode_ = (infos.begin()->second).GetVersionCode();
    std::string versionName = (infos.begin()->second).GetVersionName();
    uint32_t minCompatibleVersionCode = (infos.begin()->second).GetMinCompatibleVersionCode();
    uint32_t target = (infos.begin()->second).GetTargetVersion();
    std::string releaseType = (infos.begin()->second).GetReleaseType();
    uint32_t compatible = (infos.begin()->second).GetCompatibleVersion();
    bool singleton = (infos.begin()->second).IsSingleton();
    Constants::AppType appType = (infos.begin()->second).GetAppType();

    for (const auto &info : infos) {
        // check bundleName
        if (bundleName != info.second.GetBundleName()) {
            return ERR_APPEXECFWK_INSTALL_BUNDLENAME_NOT_SAME;
        }
        // check version
        if (versionCode_ != info.second.GetVersionCode()) {
            return ERR_APPEXECFWK_INSTALL_VERSIONCODE_NOT_SAME;
        }
        if (versionName != info.second.GetVersionName()) {
            return ERR_APPEXECFWK_INSTALL_VERSIONNAME_NOT_SAME;
        }
        if (minCompatibleVersionCode != info.second.GetMinCompatibleVersionCode()) {
            return ERR_APPEXECFWK_INSTALL_MINCOMPATIBLE_VERSIONCODE_NOT_SAME;
        }
        // check vendor
        if (vendor != info.second.GetVendor()) {
            return ERR_APPEXECFWK_INSTALL_VENDOR_NOT_SAME;
        }
        // check release type
        if (target != info.second.GetTargetVersion()) {
            return ERR_APPEXECFWK_INSTALL_RELEASETYPE_TARGET_NOT_SAME;
        }
        if (compatible != info.second.GetCompatibleVersion()) {
            return ERR_APPEXECFWK_INSTALL_RELEASETYPE_COMPATIBLE_NOT_SAME;
        }
        if (releaseType != info.second.GetReleaseType()) {
            return ERR_APPEXECFWK_INSTALL_RELEASETYPE_NOT_SAME;
        }
        if (singleton != info.second.IsSingleton()) {
            return ERR_APPEXECFWK_INSTALL_SINGLETON_NOT_SAME;
        }
        if (appType != info.second.GetAppType()) {
            return ERR_APPEXECFWK_INSTALL_APPTYPE_NOT_SAME;
        }
    }
    bundleName_ = bundleName;
    APP_LOGD("finish check APP label");
    return ret;
}

bool BaseBundleInstaller::GetInnerBundleInfo(InnerBundleInfo &info, bool &isAppExist)
{
    if (!dataMgr_) {
        dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
        if (!dataMgr_) {
            APP_LOGE("Get dataMgr shared_ptr nullptr");
            return false;
        }
    }
    auto &mtx = dataMgr_->GetBundleMutex(bundleName_);
    std::lock_guard lock { mtx };
    isAppExist = dataMgr_->GetInnerBundleInfo(bundleName_, info);
    return true;
}

ErrCode BaseBundleInstaller::CheckVersionCompatibility(const InnerBundleInfo &oldInfo)
{
    if (oldInfo.GetEntryInstallationFree()) {
        return CheckVersionCompatibilityForHmService(oldInfo);
    }
    return CheckVersionCompatibilityForApplication(oldInfo);
}

// In the process of hap updating, the version code of the entry hap which is about to be updated must not less the
// version code of the current entry haps in the device; if no-entry hap in the device, the updating haps should
// have same version code with the current version code; if the no-entry haps is to be updated, which should has the
// same version code with that of the entry hap in the device.
ErrCode BaseBundleInstaller::CheckVersionCompatibilityForApplication(const InnerBundleInfo &oldInfo)
{
    APP_LOGD("start to check version compatibility for application");
    if (oldInfo.HasEntry()) {
        if (isContainEntry_ && versionCode_ < oldInfo.GetVersionCode()) {
            APP_LOGE("fail to update lower version bundle");
            return ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE;
        }
        if (!isContainEntry_ && versionCode_ > oldInfo.GetVersionCode()) {
            APP_LOGE("version code is not compatible");
            return ERR_APPEXECFWK_INSTALL_VERSION_NOT_COMPATIBLE;
        }
        if (!isContainEntry_ && versionCode_ < oldInfo.GetVersionCode()) {
            APP_LOGE("version code is not compatible");
            return ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE;
        }
    } else {
        if (versionCode_ < oldInfo.GetVersionCode()) {
            APP_LOGE("fail to update lower version bundle");
            return ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE;
        }
    }

    if (versionCode_ > oldInfo.GetVersionCode()) {
        APP_LOGD("need to uninstall lower version feature hap");
        isFeatureNeedUninstall_ = true;
    }
    APP_LOGD("finish to check version compatibility for application");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CheckVersionCompatibilityForHmService(const InnerBundleInfo &oldInfo)
{
    APP_LOGD("start to check version compatibility for hm service");
    if (versionCode_ < oldInfo.GetVersionCode()) {
        APP_LOGE("fail to update lower version bundle");
        return ERR_APPEXECFWK_INSTALL_VERSION_DOWNGRADE;
    }
    if (versionCode_ > oldInfo.GetVersionCode()) {
        APP_LOGD("need to uninstall lower version hap");
        isFeatureNeedUninstall_ = true;
    }
    APP_LOGD("finish to check version compatibility for hm service");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::UninstallLowerVersionFeature(const std::vector<std::string> &packageVec)
{
    APP_LOGD("start to uninstall lower version feature hap");
    InnerBundleInfo info;
    bool isExist = false;
    if (!GetInnerBundleInfo(info, isExist) || !isExist) {
        return ERR_APPEXECFWK_UNINSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    if (!dataMgr_->UpdateBundleInstallState(bundleName_, InstallState::UNINSTALL_START)) {
        APP_LOGE("uninstall already start");
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    // kill the bundle process during uninstall.
    if (!AbilityManagerHelper::UninstallApplicationProcesses(info.GetApplicationName(), info.GetUid(userId_))) {
        APP_LOGW("can not kill process");
    }
    std::vector<std::string> moduleVec = info.GetModuleNameVec();
    InnerBundleInfo oldInfo = info;
    for (const auto &package : moduleVec) {
        if (find(packageVec.begin(), packageVec.end(), package) == packageVec.end()) {
            APP_LOGD("uninstall package %{public}s", package.c_str());
            ErrCode result = RemoveModuleAndDataDir(info, package, Constants::UNSPECIFIED_USERID, true);
            if (result != ERR_OK) {
                APP_LOGE("remove module dir failed");
                return result;
            }
            if (!dataMgr_->RemoveModuleInfo(bundleName_, package, info)) {
                APP_LOGE("RemoveModuleInfo failed");
                return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
            }
        }
    }
    // need to delete lower version feature hap definePermissions and requestPermissions
    APP_LOGD("delete lower version feature hap definePermissions and requestPermissions");
    ErrCode ret = UpdateDefineAndRequestPermissions(oldInfo, info);
    if (ret != ERR_OK) {
        return ret;
    }
    APP_LOGD("finish to uninstall lower version feature hap");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CheckSystemSize(const std::string &bundlePath, const Constants::AppType appType) const
{
    if ((appType == Constants::AppType::SYSTEM_APP) &&
        (BundleUtil::CheckSystemSize(bundlePath, Constants::SYSTEM_APP_INSTALL_PATH))) {
        return ERR_OK;
    }
    if ((appType == Constants::AppType::THIRD_SYSTEM_APP) &&
        (BundleUtil::CheckSystemSize(bundlePath, Constants::THIRD_SYSTEM_APP_INSTALL_PATH))) {
        return ERR_OK;
    }
    if ((appType == Constants::AppType::THIRD_PARTY_APP) &&
        (BundleUtil::CheckSystemSize(bundlePath, Constants::THIRD_PARTY_APP_INSTALL_PATH))) {
        return ERR_OK;
    }
    APP_LOGE("install failed due to insufficient disk memory");
    return ERR_APPEXECFWK_INSTALL_DISK_MEM_INSUFFICIENT;
}

int32_t BaseBundleInstaller::GetUserId(const int32_t &userId) const
{
    if (userId < Constants::DEFAULT_USERID) {
        APP_LOGE("userId(%{public}d) is invalid.", userId);
        return Constants::INVALID_USERID;
    }

    APP_LOGD("BundleInstaller GetUserId, now userId is %{public}d", userId);
    return userId;
}

ErrCode BaseBundleInstaller::CreateBundleUserData(InnerBundleInfo &innerBundleInfo)
{
    APP_LOGD("CreateNewUserData %{public}s userId: %{public}d.",
        innerBundleInfo.GetBundleName().c_str(), userId_);
    if (!innerBundleInfo.HasInnerBundleUserInfo(userId_)) {
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    ErrCode result = CreateBundleDataDir(innerBundleInfo);
    if (result != ERR_OK) {
        RemoveBundleDataDir(innerBundleInfo);
        return result;
    }

    innerBundleInfo.SetBundleInstallTime(BundleUtil::GetCurrentTime(), userId_);
    InnerBundleUserInfo innerBundleUserInfo;
    if (!innerBundleInfo.GetInnerBundleUserInfo(userId_, innerBundleUserInfo)) {
        APP_LOGE("oldInfo do not have user");
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    if (!dataMgr_->AddInnerBundleUserInfo(innerBundleInfo.GetBundleName(), innerBundleUserInfo)) {
        APP_LOGE("update bundle user info to db failed %{public}s when createNewUser",
            innerBundleInfo.GetBundleName().c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    return ERR_OK;
}

ErrCode BaseBundleInstaller::UninstallAllSandboxApps(const std::string &bundleName, int32_t userId)
{
    // All sandbox will be uninstalled when the original application is updated or uninstalled
    APP_LOGD("UninstallAllSandboxApps begin");
    if (bundleName.empty()) {
        APP_LOGE("UninstallAllSandboxApps failed due to empty bundle name");
        return ERR_APPEXECFWK_INSTALL_PARAM_ERROR;
    }
    std::shared_ptr<BundleSandboxInstaller> installer = std::make_shared<BundleSandboxInstaller>();
    if (installer == nullptr) {
        APP_LOGE("UninstallAllSandboxApps failed due to installer nullptr");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    if (installer->UninstallAllSandboxApps(bundleName, userId) != ERR_OK) {
        APP_LOGE("UninstallAllSandboxApps failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    APP_LOGD("UninstallAllSandboxApps finish");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::CheckAppLabel(const InnerBundleInfo &oldInfo, const InnerBundleInfo &newInfo) const
{
    // check app label for inheritance installation
    APP_LOGD("CheckAppLabel begin");
    if (oldInfo.GetVersionName() != newInfo.GetVersionName()) {
        return ERR_APPEXECFWK_INSTALL_VERSIONNAME_NOT_SAME;
    }
    if (oldInfo.GetMinCompatibleVersionCode() != newInfo.GetMinCompatibleVersionCode()) {
        return ERR_APPEXECFWK_INSTALL_MINCOMPATIBLE_VERSIONCODE_NOT_SAME;
    }
    if (oldInfo.GetVendor() != newInfo.GetVendor()) {
        return ERR_APPEXECFWK_INSTALL_VENDOR_NOT_SAME;
    }
    if (oldInfo.GetTargetVersion()!= newInfo.GetTargetVersion()) {
        return ERR_APPEXECFWK_INSTALL_RELEASETYPE_TARGET_NOT_SAME;
    }
    if (oldInfo.GetCompatibleVersion() != newInfo.GetCompatibleVersion()) {
        return ERR_APPEXECFWK_INSTALL_RELEASETYPE_COMPATIBLE_NOT_SAME;
    }
    if (oldInfo.GetReleaseType() != newInfo.GetReleaseType()) {
        return ERR_APPEXECFWK_INSTALL_RELEASETYPE_NOT_SAME;
    }
    APP_LOGD("CheckAppLabel end");
    return ERR_OK;
}

ErrCode BaseBundleInstaller::RemoveBundleUserData(InnerBundleInfo &innerBundleInfo, bool needRemoveData)
{
    auto bundleName = innerBundleInfo.GetBundleName();
    APP_LOGD("remove user(%{public}d) in bundle(%{public}s).", userId_, bundleName.c_str());
    if (!innerBundleInfo.HasInnerBundleUserInfo(userId_)) {
        return ERR_APPEXECFWK_USER_NOT_EXIST;
    }

    if (!needRemoveData) {
        ErrCode result = RemoveBundleDataDir(innerBundleInfo);
        if (result != ERR_OK) {
            APP_LOGE("remove user data directory failed.");
            return result;
        }
    }

    // delete accessTokenId
    if (BundlePermissionMgr::DeleteAccessTokenId(innerBundleInfo.GetAccessTokenId(userId_)) !=
        AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("delete accessToken failed");
    }

    innerBundleInfo.RemoveInnerBundleUserInfo(userId_);
    if (!dataMgr_->RemoveInnerBundleUserInfo(bundleName, userId_)) {
        APP_LOGE("update bundle user info to db failed %{public}s when remove user",
            bundleName.c_str());
        return ERR_APPEXECFWK_INSTALL_BUNDLE_MGR_SERVICE_ERROR;
    }

    return ERR_OK;
}

bool BaseBundleInstaller::verifyUriPrefix(const InnerBundleInfo &info, int32_t userId, bool isUpdate) const
{
    // uriPrefix must be unique
    // verify current module uriPrefix
    std::vector<std::string> currentUriPrefixList;
    info.GetUriPrefixList(currentUriPrefixList);
    if (currentUriPrefixList.empty()) {
        APP_LOGD("current module not include uri, verify uriPrefix success");
        return true;
    }
    std::set<std::string> set;
    for (const std::string &currentUriPrefix : currentUriPrefixList) {
        if (!set.insert(currentUriPrefix).second) {
            APP_LOGE("current module contains duplicate uriPrefix, verify uriPrefix failed");
            APP_LOGE("bundleName : %{public}s, moduleName : %{public}s, uriPrefix : %{public}s",
                info.GetBundleName().c_str(), info.GetCurrentModulePackage().c_str(), currentUriPrefix.c_str());
            return false;
        }
    }
    set.clear();
    // verify exist bundle uriPrefix
    if (dataMgr_ == nullptr) {
        APP_LOGE("dataMgr_ is null, verify uriPrefix failed");
        return false;
    }
    std::vector<std::string> uriPrefixList;
    std::string excludeModule;
    if (isUpdate) {
        excludeModule.append(info.GetBundleName()).append(".").append(info.GetCurrentModulePackage()).append(".");
    }
    dataMgr_->GetAllUriPrefix(uriPrefixList, userId, excludeModule);
    if (uriPrefixList.empty()) {
        APP_LOGD("uriPrefixList empty, verify uriPrefix success");
        return true;
    }
    for (const std::string &currentUriPrefix : currentUriPrefixList) {
        auto iter = std::find(uriPrefixList.cbegin(), uriPrefixList.cend(), currentUriPrefix);
        if (iter != uriPrefixList.cend()) {
            APP_LOGE("uriPrefix alread exist in device, uriPrefix : %{public}s", currentUriPrefix.c_str());
            APP_LOGE("verify uriPrefix failed");
            return false;
        }
    }
    APP_LOGD("verify uriPrefix success");
    return true;
}

ErrCode BaseBundleInstaller::CheckHapModleOrType(const InnerBundleInfo &innerBundleInfo,
    const std::unordered_map<std::string, InnerBundleInfo> &infos) const
{
    for (const auto &item : infos) {
        if (innerBundleInfo.GetIsNewVersion() != item.second.GetIsNewVersion()) {
            APP_LOGE("CheckHapModleOrType cannot install new modle and old modle simultaneously");
            return ERR_APPEXECFWK_INSTALL_STATE_ERROR;
        }
        if (innerBundleInfo.GetEntryInstallationFree() != item.second.GetEntryInstallationFree()) {
            APP_LOGE("CheckHapModleOrType cannot install application and hm service simultaneously");
            return ERR_APPEXECFWK_INSTALL_TYPE_ERROR;
        }
    }
    return ERR_OK;
}

void BaseBundleInstaller::SaveHapPathToRecords(
    bool isPreInstallApp, const std::unordered_map<std::string, InnerBundleInfo> &infos)
{
    if (isPreInstallApp) {
        APP_LOGD("PreInstallApp do not need to save hap path to record");
        return;
    }

    for (const auto &item : infos) {
        auto hapPathIter = hapPathRecords_.find(item.first);
        if (hapPathIter == hapPathRecords_.end()) {
            hapPathRecords_.emplace(item.first, GetHapPath(item.second));
        }
    }
}

void BaseBundleInstaller::SaveHapToInstallPath(bool moveFileMode)
{
    for (const auto &hapPathRecord : hapPathRecords_) {
        APP_LOGD("Save from(%{public}s) to(%{public}s)",
            hapPathRecord.first.c_str(), hapPathRecord.second.c_str());
        if (moveFileMode) {
            if (!BundleUtil::RenameFile(hapPathRecord.first, hapPathRecord.second)) {
                APP_LOGE("Move hap to install path failed");
                return;
            }

            continue;
        }

        if (!BundleUtil::CopyFile(hapPathRecord.first, hapPathRecord.second)) {
            APP_LOGE("Copy hap to install path failed");
            return;
        }
    }
}

void BaseBundleInstaller::SetEntryInstallationFree(const BundlePackInfo &bundlePackInfo,
    InnerBundleInfo &innrBundleInfo)
{
    APP_LOGI("SetEntryInstallationFree start");
    if (!bundlePackInfo.GetValid()) {
        APP_LOGW("no pack.info in the hap file");
        return;
    }
    auto packageModule = bundlePackInfo.summary.modules;
    auto installationFree = std::any_of(packageModule.begin(), packageModule.end(), [&](auto &module) {
        return module.distro.moduleType == "entry" && module.distro.installationFree;
    });
    if (installationFree) {
        APP_LOGI("install or upddate hm service");
    }
    innrBundleInfo.SetEntryInstallationFree(installationFree);
    APP_LOGI("SetEntryInstallationFree end");
}

void BaseBundleInstaller::ResetInstallProperties()
{
    isContainEntry_ = false;
    isAppExist_ = false;
    hasInstalledInUser_ = false;
    isFeatureNeedUninstall_ = false;
    versionCode_ = 0;
    uninstallModuleVec_.clear();
    installedModules_.clear();
    state_ = InstallerState::INSTALL_START;
    singletonState_ = SingletonState::DEFAULT;
    sysEventInfo_.Reset();
}

void BaseBundleInstaller::OnSingletonChange()
{
    if (singletonState_ == SingletonState::DEFAULT) {
        return;
    }

    InnerBundleInfo info;
    bool isExist = false;
    if (!GetInnerBundleInfo(info, isExist) || !isExist) {
        APP_LOGE("Get innerBundleInfo failed when singleton changed");
        return;
    }

    InstallParam installParam;
    installParam.needSendEvent = false;
    installParam.forceExecuted = true;
    installParam.noSkipsKill = false;
    if (singletonState_ == SingletonState::SINGLETON_TO_NON) {
        APP_LOGD("Bundle changes from singleton app to non singleton app");
        installParam.userId = Constants::DEFAULT_USERID;
        UninstallBundle(bundleName_, installParam);
        return;
    }

    if (singletonState_ == SingletonState::NON_TO_SINGLETON) {
        APP_LOGD("Bundle changes from non singleton app to singleton app");
        for (auto infoItem : info.GetInnerBundleUserInfos()) {
            int32_t installedUserId = infoItem.second.bundleUserInfo.userId;
            if (installedUserId == Constants::DEFAULT_USERID) {
                continue;
            }

            installParam.userId = installedUserId;
            UninstallBundle(bundleName_, installParam);
        }
    }
}

void BaseBundleInstaller::SendBundleSystemEvent(const std::string &bundleName, BundleEventType bundleEventType,
    const InstallParam &installParam, InstallScene preBundleScene, ErrCode errCode)
{
    sysEventInfo_.bundleName = bundleName;
    sysEventInfo_.isPreInstallApp = installParam.isPreInstallApp;
    sysEventInfo_.errCode = errCode;
    sysEventInfo_.isFreeInstallMode = (installParam.installFlag == InstallFlag::FREE_INSTALL);
    sysEventInfo_.userId = userId_;
    sysEventInfo_.versionCode = versionCode_;
    sysEventInfo_.preBundleScene = preBundleScene;
    EventReport::SendBundleSystemEvent(bundleEventType, sysEventInfo_);
}
}  // namespace AppExecFwk
}  // namespace OHOS
