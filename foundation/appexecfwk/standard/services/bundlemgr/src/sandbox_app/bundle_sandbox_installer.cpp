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

#include "bundle_sandbox_installer.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "bundle_mgr_service.h"
#include "bundle_permission_mgr.h"
#include "bundle_sandbox_data_mgr.h"
#include "bundle_util.h"
#include "hitrace_meter.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "datetime_ex.h"
#include "installd_client.h"
#include "ohos/aafwk/content/want.h"
#include "perf_profile.h"
#include "scope_guard.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::Security;

BundleSandboxInstaller::BundleSandboxInstaller()
{
    APP_LOGD("bundle sandbox installer instance is created");
}

BundleSandboxInstaller::~BundleSandboxInstaller()
{
    APP_LOGD("bundle sandbox installer instance is destroyed");
}

ErrCode BundleSandboxInstaller::InstallSandboxApp(const std::string &bundleName, const int32_t &dlpType,
    const int32_t &userId, int32_t &appIndex)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    APP_LOGD("InstallSandboxApp %{public}s begin", bundleName.c_str());

    PerfProfile::GetInstance().SetBundleInstallStartTime(GetTickCount());

    if (bundleName.empty()) {
        APP_LOGE("the bundle name is empty");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    bundleName_ = bundleName;

    // 1. check whether original application installed or not
    InnerBundleInfo info;
    bool isExist = false;
    if (!GetInnerBundleInfo(info, isExist) || !isExist) {
        APP_LOGE("the bundle is not installed");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_APP_NOT_EXISTED;
    }
    InnerBundleInfo oldInfo = info;

    // 2. obtain userId
    if (userId < Constants::DEFAULT_USERID) {
        APP_LOGE("userId(%{public}d) is invalid.", userId);
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    userId_ = userId;

    if (!dataMgr_->HasUserId(userId_)) {
        APP_LOGE("the user %{public}d does not exist when when install sandbox application.", userId_);
        return ERR_APPEXECFWK_SANDBOX_INSTALL_USER_NOT_EXIST;
    }

    // 3. check whether original application installed at current userId or not
    InnerBundleUserInfo userInfo;
    if (!info.GetInnerBundleUserInfo(userId_, userInfo)) {
        APP_LOGE("the origin application is not installed at current user");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_NOT_INSTALLED_AT_SPECIFIED_USERID;
    }
    info.CleanInnerBundleUserInfos();
    ScopeGuard sandboxAppGuard([&] { SandboxAppRollBack(info, userId_); });

    // 4. generate the accesstoken id
    if (GetSandboxDataMgr() != ERR_OK) {
        APP_LOGE("get sandbox data mgr failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    auto newAppIndex = sandboxDataMgr_->GenerateSandboxAppIndex(bundleName_);
    if (newAppIndex == Constants::INITIAL_APP_INDEX) {
        APP_LOGE("invalid app index generated");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_INVALID_APP_INDEX;
    }
    info.SetAppIndex(newAppIndex);
    info.SetIsSandbox(true);
    uint32_t newTokenId = BundlePermissionMgr::CreateAccessTokenId(info, info.GetBundleName(), userId_, dlpType);

    // 5. grant permission
    // to be completed further
    if (!BundlePermissionMgr::GrantRequestPermissions(info, newTokenId)) {
        APP_LOGE("InstallSandboxApp grant permission failed");
        return ERR_APPEXECFWK_INSTALL_GRANT_REQUEST_PERMISSIONS_FAILED;
    }

    // 6. create data dir and generate uid and gid
    userInfo.bundleName = bundleName_ + Constants::FILE_UNDERLINE + std::to_string(newAppIndex);
    userInfo.gids.clear();
    dataMgr_->GenerateUidAndGid(userInfo);
    userInfo.bundleName = bundleName_;
    info.AddInnerBundleUserInfo(userInfo);
    info.SetBundleInstallTime(BundleUtil::GetCurrentTime(), userId_);
    info.SetBundleUpdateTime(0, userId_);
    info.SetAccessTokenId(newTokenId, userId_);

    ErrCode result = CreateSandboxDataDir(info, userInfo.uid, newAppIndex);
    if (result != ERR_OK) {
        APP_LOGE("InstallSandboxApp create sandbox dir failed");
        return result;
    }

    // 7. store new bundleInfo to sandbox data manager
    sandboxDataMgr_->SaveSandboxAppInfo(info, newAppIndex);
    sandboxAppGuard.Dismiss();
    dataMgr_->EnableBundle(bundleName_);

    // 8. SaveSandboxPersistentInfo
    StoreSandboxPersitentInfo(bundleName, newTokenId, newAppIndex, userId_);

    // 9. publish common event
    result = NotifySandboxAppStatus(info, userInfo.uid, SandboxInstallType::INSTALL);
    if (result != ERR_OK) {
        APP_LOGE("NotifySandboxAppStatus failed due to error : %{public}d", result);
    }

    appIndex = newAppIndex;
    PerfProfile::GetInstance().SetBundleInstallEndTime(GetTickCount());

    APP_LOGD("InstallSandboxApp %{public}s succesfully", bundleName.c_str());
    return result;
}

ErrCode BundleSandboxInstaller::UninstallSandboxApp(
    const std::string &bundleName, const int32_t &appIndex, const int32_t &userId)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    APP_LOGD("UninstallSandboxApp %{public}s _ %{public}d begin", bundleName.c_str(), appIndex);

    PerfProfile::GetInstance().SetBundleInstallStartTime(GetTickCount());

    // 1. check userId
    if (GetDataMgr() != ERR_OK) {
            APP_LOGE("Get dataMgr shared_ptr nullptr");
            return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    if (userId < Constants::DEFAULT_USERID) {
        APP_LOGE("userId(%{public}d) is invalid.", userId);
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }
    userId_ = userId;
    if (!dataMgr_->HasUserId(userId_)) {
        APP_LOGE("The user %{public}d does not exist when uninstall sandbox application.", userId_);
        return ERR_APPEXECFWK_SANDBOX_INSTALL_USER_NOT_EXIST;
    }

    // 2. check whether sandbox appinfo can be found from data manager or not
    InnerBundleInfo info;
    if (GetSandboxDataMgr() != ERR_OK) {
        APP_LOGE("get sandbox data mgr failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    ErrCode result = ERR_OK;
    if ((result = sandboxDataMgr_->GetSandboxAppInfo(bundleName, appIndex, userId_, info)) != ERR_OK) {
        APP_LOGE("UninstallSandboxApp no sandbox app info can be found");
        return result;
    }

    // 3. check whether sandbox app is installed at current user
    InnerBundleUserInfo userInfo;
    if (!info.GetInnerBundleUserInfo(userId_, userInfo)) {
        APP_LOGE("the sandbox app is not installed at this user %{public}d", userId_);
        return ERR_APPEXECFWK_SANDBOX_INSTALL_NOT_INSTALLED_AT_SPECIFIED_USERID;
    }

    // 4. delete accesstoken id and appIndex
    uint32_t accessTokenId = info.GetAccessTokenId(userId_);
    if (BundlePermissionMgr::DeleteAccessTokenId(accessTokenId) !=
        AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("delete accessToken failed");
    }

    if (!sandboxDataMgr_->DeleteSandboxAppIndex(bundleName, appIndex)) {
        APP_LOGE("delete sandbox app index failed");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_DELETE_APP_INDEX_FAILED;
    }

    // 5. remove data dir and uid, gid
    std::string innerBundleName = bundleName + Constants::FILE_UNDERLINE + std::to_string(appIndex);
    result = InstalldClient::GetInstance()->RemoveBundleDataDir(innerBundleName, userId_);
    if (result != ERR_OK) {
        APP_LOGE("fail to remove data dir: %{public}s, error is %{public}d", innerBundleName.c_str(), result);
        return result;
    }
    userInfo.bundleName = innerBundleName;
    info.AddInnerBundleUserInfo(userInfo);
    dataMgr_->RecycleUidAndGid(info);

    // 6. remove new innerBundleInfo from sandData manager
    sandboxDataMgr_->DeleteSandboxAppInfo(bundleName, appIndex);

    // 7. remove sandbox persistent info
    DeleteSandboxPersitentInfo(bundleName, accessTokenId, appIndex, userId);

    // 8. publish common event
    result = NotifySandboxAppStatus(info, userInfo.uid, SandboxInstallType::UNINSTALL);
    if (result != ERR_OK) {
        APP_LOGE("NotifySandboxAppStatus failed due to error : %{public}d", result);
    }

    PerfProfile::GetInstance().SetBundleInstallEndTime(GetTickCount());

    APP_LOGD("UninstallSandboxApp %{public}s _ %{public}d succesfully", bundleName.c_str(), appIndex);
    return result;
}

ErrCode BundleSandboxInstaller::CreateSandboxDataDir(
    InnerBundleInfo &info, const int32_t &uid, const int32_t &appIndex) const
{
    APP_LOGD("CreateSandboxDataDir %{public}s _ %{public}d begin", info.GetBundleName().c_str(), appIndex);
    std::string innerDataDir = info.GetBundleName() + Constants::FILE_UNDERLINE + std::to_string(appIndex);
    auto result = InstalldClient::GetInstance()->CreateBundleDataDir(innerDataDir, userId_, uid, uid,
        info.GetAppPrivilegeLevel());
    if (result != ERR_OK) {
        APP_LOGE("fail to create sandbox data dir, error is %{public}d", result);
        return result;
    }
    std::string dataBaseDir = Constants::BUNDLE_APP_DATA_BASE_DIR + Constants::BUNDLE_EL[1] +
        Constants::DATABASE + innerDataDir;
    info.SetAppDataBaseDir(dataBaseDir);
    APP_LOGD("CreateSandboxDataDir successfully");
    return result;
}

ErrCode BundleSandboxInstaller::NotifySandboxAppStatus(const InnerBundleInfo &info, const int32_t &uid,
    const SandboxInstallType &type) const
{
    OHOS::AAFwk::Want want;
    if (type == SandboxInstallType::INSTALL) {
        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_ADDED);
    } else if (type == SandboxInstallType::UNINSTALL) {
        want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SANDBOX_PACKAGE_REMOVED);
    } else {
        return ERR_APPEXECFWK_SANDBOX_INSTALL_UNKNOWN_INSTALL_TYPE;
    }
    ElementName element;
    element.SetBundleName(info.GetBundleName());
    element.SetAbilityName(info.GetMainAbility());
    want.SetElement(element);
    want.SetParam(Constants::UID, uid);
    want.SetParam(Constants::USER_ID, BundleUtil::GetUserIdByUid(uid));
    want.SetParam(Constants::ABILTY_NAME.data(), info.GetMainAbility());
    want.SetParam(Constants::SANDBOX_APP_INDEX, info.GetAppIndex());
    EventFwk::CommonEventData commonData { want };
    EventFwk::CommonEventManager::PublishCommonEvent(commonData);
    return ERR_OK;
}

void BundleSandboxInstaller::SandboxAppRollBack(InnerBundleInfo &info, const int32_t &userId)
{
    // when a sandbox is installed failed, some stuff, including uid, gid, appIndex,
    // accessToken Id and the data dir will be removed.
    APP_LOGD("SandboxAppRollBack begin");
    BundlePermissionMgr::DeleteAccessTokenId(info.GetAccessTokenId(userId));
    auto bundleName = info.GetBundleName();
    auto appIndex = info.GetAppIndex();
    if (GetSandboxDataMgr() != ERR_OK) {
        APP_LOGE("get sandbox data mgr failed");
        return;
    }
    sandboxDataMgr_->DeleteSandboxAppIndex(bundleName, appIndex);
    auto key = bundleName + Constants::FILE_UNDERLINE + std::to_string(appIndex);
    InnerBundleUserInfo userInfo;
    if (!info.GetInnerBundleUserInfo(userId, userInfo)) {
        APP_LOGW("SandboxAppRollBack cannot obtain the userInfo");
    } else {
        userInfo.bundleName = key;
        info.AddInnerBundleUserInfo(userInfo);
        dataMgr_->RecycleUidAndGid(info);
    }
    if (InstalldClient::GetInstance()->RemoveBundleDataDir(key, userId) != ERR_OK) {
        APP_LOGW("SandboxAppRollBack cannot remove the data dir");
    }
    APP_LOGD("SandboxAppRollBack finish");
}

ErrCode BundleSandboxInstaller::UninstallAllSandboxApps(const std::string &bundleName, int32_t userId)
{
    // All sandbox will be uninstalled when the original application is updated or uninstalled
    APP_LOGD("UninstallAllSandboxApps begin");
    if (bundleName.empty()) {
        APP_LOGE("UninstallAllSandboxApps failed due to empty bundle name");
        return ERR_APPEXECFWK_SANDBOX_INSTALL_PARAM_ERROR;
    }

    if (GetDataMgr() != ERR_OK) {
            APP_LOGE("UninstallAllSandboxApps get dataMgr shared_ptr nullptr");
            return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    if (GetSandboxDataMgr() != ERR_OK) {
        APP_LOGE("get sandbox data mgr failed");
        return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
    }
    auto sandboxAppInfoMap = sandboxDataMgr_->GetSandboxAppInfoMap();
    for_each(sandboxAppInfoMap.begin(), sandboxAppInfoMap.end(), [&bundleName, &sandboxAppInfoMap, &userId, this](
        std::unordered_map<std::string, InnerBundleInfo>::reference info)->void {
        auto pos = info.first.find(bundleName);
        if (pos != std::string::npos) {
            std::string appIndexStr = info.first.substr(bundleName.length() + 1);
            int32_t appIndex = Constants::INITIAL_APP_INDEX;
            if (!StrToInt(appIndexStr, appIndex)) {
                APP_LOGE("UninstallAllSandboxApps obtain appIndex failed");
                return;
            }
            auto userInfos = sandboxAppInfoMap[info.first].GetInnerBundleUserInfos();
            auto specifiedUserId = (userInfos.begin()->second).bundleUserInfo.userId;
            if (specifiedUserId == userId || userId == Constants::INVALID_USERID) {
                if (this->UninstallSandboxApp(bundleName, appIndex, specifiedUserId) != ERR_OK) {
                    APP_LOGE("UninstallSandboxApp failed");
                    return;
                }
            }
        }
    });

    APP_LOGD("UninstallAllSandboxApps finish");
    return ERR_OK;
}

bool BundleSandboxInstaller::GetInnerBundleInfo(InnerBundleInfo &info, bool &isAppExist)
{
    if (GetDataMgr() != ERR_OK) {
        APP_LOGE("Get dataMgr shared_ptr failed");
        return false;
    }
    isAppExist = dataMgr_->GetInnerBundleInfo(bundleName_, info);
    return true;
}

ErrCode BundleSandboxInstaller::GetSandboxDataMgr()
{
    if (!sandboxDataMgr_) {
        if (GetDataMgr() != ERR_OK) {
            APP_LOGE("Get dataMgr shared_ptr failed");
            return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
        }

        sandboxDataMgr_ = dataMgr_->GetSandboxDataMgr();
        if (!sandboxDataMgr_) {
            APP_LOGE("Get sandbox dataMgr shared_ptr nullptr");
            return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
        }
    }
    return ERR_OK;
}

ErrCode BundleSandboxInstaller::GetDataMgr()
{
    if (!dataMgr_) {
        dataMgr_ = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
        if (!dataMgr_) {
            APP_LOGE("Get dataMgr shared_ptr nullptr");
            return ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR;
        }
    }
    return ERR_OK;
}

void BundleSandboxInstaller::StoreSandboxPersitentInfo(const std::string &bundleName, uint32_t accessTokenId,
    int32_t appIndex, int32_t userId)
{
    SandboxAppPersistentInfo sandboxPersistentInfo = {
        .accessTokenId = accessTokenId,
        .appIndex = appIndex,
        .userId = userId
    };
    dataMgr_->StoreSandboxPersistentInfo(bundleName, sandboxPersistentInfo);
}

void BundleSandboxInstaller::DeleteSandboxPersitentInfo(const std::string &bundleName, uint32_t accessTokenId,
    int32_t appIndex, int32_t userId)
{
    SandboxAppPersistentInfo sandboxPersistentInfo = {
        .accessTokenId = accessTokenId,
        .appIndex = appIndex,
        .userId = userId
    };
    dataMgr_->DeleteSandboxPersistentInfo(bundleName, sandboxPersistentInfo);
}
} // AppExecFwk
} // OHOS
