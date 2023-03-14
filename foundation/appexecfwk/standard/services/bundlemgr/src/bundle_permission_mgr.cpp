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

#include "bundle_permission_mgr.h"

#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"
#include "bundle_parser.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::Security;
std::map<std::string, std::map<std::string, bool>> BundlePermissionMgr::defaultPermissions_;

bool BundlePermissionMgr::Init()
{
    BundleParser bundleParser;
    std::vector<DefaultPermission> permissions;
    if (bundleParser.ParseDefaultPermission(permissions) != ERR_OK) {
        APP_LOGE("BundlePermissionMgr::Init failed");
        return false;
    }
    defaultPermissions_.clear();
    for (const auto &permission : permissions) {
        std::map<std::string, bool> permissionInfo;
        APP_LOGD("BundlePermissionMgr::Init bundleName: %{public}s", permission.bundleName.c_str());
        for (const auto &info : permission.grantPermission) {
            APP_LOGD("BundlePermissionMgr::Init permissionName: %{public}s, userCancellable: %{public}d",
                info.name.c_str(), info.userCancellable);
            permissionInfo.try_emplace(info.name, info.userCancellable);
        }
        defaultPermissions_.try_emplace(permission.bundleName, permissionInfo);
    }
    APP_LOGD("BundlePermissionMgr::Init success");
    return true;
}

void BundlePermissionMgr::UnInit()
{
    APP_LOGD("BundlePermissionMgr::UnInit");
    defaultPermissions_.clear();
}

void BundlePermissionMgr::ConvertPermissionDef(
    const AccessToken::PermissionDef &permDef, PermissionDef &permissionDef)
{
    permissionDef.permissionName = permDef.permissionName;
    permissionDef.bundleName = permDef.bundleName;
    permissionDef.grantMode = permDef.grantMode;
    permissionDef.availableLevel = permDef.availableLevel;
    permissionDef.provisionEnable = permDef.provisionEnable;
    permissionDef.distributedSceneEnable = permDef.distributedSceneEnable;
    permissionDef.label = permDef.label;
    permissionDef.labelId = permDef.labelId;
    permissionDef.description = permDef.description;
    permissionDef.descriptionId = permDef.descriptionId;
}

// Convert from the struct DefinePermission that parsed from config.json
void BundlePermissionMgr::ConvertPermissionDef(
    AccessToken::PermissionDef &permDef, const DefinePermission &definePermission, const std::string &bundleName)
{
    permDef.permissionName = definePermission.name;
    permDef.bundleName = bundleName;
    permDef.grantMode = [&definePermission]() -> int {
        if (definePermission.grantMode ==
            ProfileReader::BUNDLE_MODULE_PROFILE_KEY_DEF_PERMISSIONS_GRANTMODE_SYSTEM_GRANT) {
            return AccessToken::GrantMode::SYSTEM_GRANT;
        }
        return AccessToken::GrantMode::USER_GRANT;
    }();

    permDef.availableLevel = GetTokenApl(definePermission.availableLevel);
    permDef.provisionEnable = definePermission.provisionEnable;
    permDef.distributedSceneEnable = definePermission.distributedSceneEnable;
    permDef.label = definePermission.label;
    permDef.labelId = definePermission.labelId;
    permDef.description = definePermission.description;
    permDef.descriptionId = definePermission.descriptionId;
}

AccessToken::ATokenAplEnum BundlePermissionMgr::GetTokenApl(const std::string &apl)
{
    if (apl == Profile::AVAILABLELEVEL_SYSTEM_CORE) {
        return AccessToken::ATokenAplEnum::APL_SYSTEM_CORE;
    }
    if (apl == Profile::AVAILABLELEVEL_SYSTEM_BASIC) {
        return AccessToken::ATokenAplEnum::APL_SYSTEM_BASIC;
    }
    return AccessToken::ATokenAplEnum::APL_NORMAL;
}

Security::AccessToken::HapPolicyParams BundlePermissionMgr::CreateHapPolicyParam(
    const InnerBundleInfo &innerBundleInfo)
{
    AccessToken::HapPolicyParams hapPolicy;
    std::string apl = innerBundleInfo.GetAppPrivilegeLevel();
    APP_LOGD("BundlePermissionMgr::CreateHapPolicyParam apl : %{public}s", apl.c_str());
    std::vector<AccessToken::PermissionDef> permDef = GetPermissionDefList(innerBundleInfo);
    std::vector<AccessToken::PermissionStateFull> permStateFull = GetPermissionStateFullList(innerBundleInfo);
    hapPolicy.apl = GetTokenApl(apl);
    hapPolicy.domain = "domain";
    hapPolicy.permList = permDef;
    hapPolicy.permStateList = permStateFull;
    return hapPolicy;
}

AccessToken::AccessTokenID BundlePermissionMgr::CreateAccessTokenId(
    const InnerBundleInfo &innerBundleInfo, const std::string bundleName, const int32_t userId, const int32_t dlpType)
{
    APP_LOGD("BundlePermissionMgr::CreateAccessTokenId bundleName = %{public}s, userId = %{public}d",
        bundleName.c_str(), userId);
    AccessToken::HapInfoParams hapInfo;
    hapInfo.userID = userId;
    hapInfo.bundleName = bundleName;
    hapInfo.instIndex = innerBundleInfo.GetAppIndex();
    hapInfo.appIDDesc = innerBundleInfo.GetAppId();
    hapInfo.dlpType = dlpType;
    AccessToken::HapPolicyParams hapPolicy = CreateHapPolicyParam(innerBundleInfo);
    AccessToken::AccessTokenIDEx accessToken = AccessToken::AccessTokenKit::AllocHapToken(hapInfo, hapPolicy);
    APP_LOGD("BundlePermissionMgr::CreateAccessTokenId accessTokenId = %{public}u",
             accessToken.tokenIdExStruct.tokenID);
    return accessToken.tokenIdExStruct.tokenID;
}

bool BundlePermissionMgr::UpdateDefineAndRequestPermissions(const Security::AccessToken::AccessTokenID tokenId,
    const InnerBundleInfo &oldInfo, const InnerBundleInfo &newInfo, std::vector<std::string> &newRequestPermName)
{
    APP_LOGD("BundlePermissionMgr::UpdateDefineAndRequestPermissions bundleName = %{public}s",
        newInfo.GetBundleName().c_str());
    std::vector<AccessToken::PermissionDef> defPermList = GetPermissionDefList(newInfo);
    std::vector<AccessToken::PermissionDef> newDefPermList;
    if (!GetNewPermissionDefList(tokenId, defPermList, newDefPermList)) {
        return false;
    }
    std::vector<AccessToken::PermissionStateFull> reqPermissionStateList = GetPermissionStateFullList(newInfo);
    std::vector<AccessToken::PermissionStateFull> newPermissionStateList;
    if (!GetNewPermissionStateFull(tokenId, reqPermissionStateList, newPermissionStateList, newRequestPermName)) {
        return false;
    }
    // delete old definePermission
    std::vector<std::string> needDeleteDefinePermission = GetNeedDeleteDefinePermissionName(oldInfo, newInfo);
    for (const auto &name : needDeleteDefinePermission) {
        auto iter = std::find_if(newDefPermList.begin(), newDefPermList.end(), [&name](const auto &defPerm) {
            return defPerm.permissionName == name;
        });
        if (iter != newDefPermList.end()) {
            APP_LOGD("delete definePermission %{public}s", name.c_str());
            newDefPermList.erase(iter);
        }
    }
    // delete old requestPermission
    std::vector<std::string> needDeleteRequestPermission = GetNeedDeleteRequestPermissionName(oldInfo, newInfo);
    for (const auto &name : needDeleteRequestPermission) {
        auto iter = std::find_if(newPermissionStateList.begin(), newPermissionStateList.end(),
            [&name](const auto &defPerm) {
            return defPerm.permissionName == name;
        });
        if (iter != newPermissionStateList.end()) {
            APP_LOGD("delete requestPermission %{public}s", name.c_str());
            newPermissionStateList.erase(iter);
        }
    }
    AccessToken::HapPolicyParams hapPolicy;
    std::string apl = newInfo.GetAppPrivilegeLevel();
    APP_LOGD("apl : %{public}s, newDefPermList size : %{public}zu, newPermissionStateList size : %{public}zu",
             apl.c_str(), newDefPermList.size(), newPermissionStateList.size());
    hapPolicy.apl = GetTokenApl(apl);
    hapPolicy.domain = "domain"; // default
    hapPolicy.permList = newDefPermList;
    hapPolicy.permStateList = newPermissionStateList;
    std::string appId = newInfo.GetAppId();
    int32_t ret = AccessToken::AccessTokenKit::UpdateHapToken(tokenId, appId, hapPolicy);
    if (ret != AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("UpdateDefineAndRequestPermissions UpdateHapToken failed errcode: %{public}d", ret);
        return false;
    }
    APP_LOGD("BundlePermissionMgr::UpdateDefineAndRequestPermissions end");
    return true;
}

std::vector<std::string> BundlePermissionMgr::GetNeedDeleteDefinePermissionName(const InnerBundleInfo &oldInfo,
    const InnerBundleInfo &newInfo)
{
    std::vector<DefinePermission> oldDefinePermissions = oldInfo.GetAllDefinePermissions();
    std::vector<DefinePermission> newDefinePermissions = newInfo.GetAllDefinePermissions();
    std::vector<std::string> needDeleteDefinePermission;
    for (const auto &defPerm : oldDefinePermissions) {
        auto iter = std::find_if(newDefinePermissions.begin(), newDefinePermissions.end(),
            [&defPerm](const auto &perm) {
            return defPerm.name == perm.name;
        });
        if (iter == newDefinePermissions.end()) {
            APP_LOGD("GetNeedDeleteDefinePermissionName need delete %{public}s", defPerm.name.c_str());
            needDeleteDefinePermission.emplace_back(defPerm.name);
        }
    }
    return needDeleteDefinePermission;
}

std::vector<std::string> BundlePermissionMgr::GetNeedDeleteRequestPermissionName(const InnerBundleInfo &oldInfo,
    const InnerBundleInfo &newInfo)
{
    std::vector<RequestPermission> oldRequestPermissions = oldInfo.GetAllRequestPermissions();
    std::vector<RequestPermission> newRequestPermissions = newInfo.GetAllRequestPermissions();
    std::vector<std::string> needDeleteRequestPermission;
    for (const auto &reqPerm : oldRequestPermissions) {
        auto iter = std::find_if(newRequestPermissions.begin(), newRequestPermissions.end(),
            [&reqPerm](const auto &perm) {
            return reqPerm.name == perm.name;
        });
        if (iter == newRequestPermissions.end()) {
            APP_LOGD("GetNeedDeleteRequestPermissionName need delete %{public}s", reqPerm.name.c_str());
            needDeleteRequestPermission.emplace_back(reqPerm.name);
        }
    }
    return needDeleteRequestPermission;
}

bool BundlePermissionMgr::GetNewPermissionDefList(Security::AccessToken::AccessTokenID tokenId,
    const std::vector<Security::AccessToken::PermissionDef> &permissionDef,
    std::vector<Security::AccessToken::PermissionDef> &newPermissionDef)
{
    int32_t ret = AccessToken::AccessTokenKit::GetDefPermissions(tokenId, newPermissionDef);
    if (ret != AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("BundlePermissionMgr::GetNewPermissionDefList GetDefPermissions failed errcode: %{public}d", ret);
        return false;
    }
    for (const auto &perm : permissionDef) {
        if (std::find_if(newPermissionDef.begin(), newPermissionDef.end(), [&perm](const auto &newPerm) {
            return newPerm.permissionName == perm.permissionName;
            }) == newPermissionDef.end()) {
            APP_LOGD("BundlePermissionMgr::GetNewPermissionDefList add define permission %{public}s",
                     perm.permissionName.c_str());
            newPermissionDef.emplace_back(perm);
        }
    }
    return true;
}

bool BundlePermissionMgr::GetNewPermissionStateFull(Security::AccessToken::AccessTokenID tokenId,
    const std::vector<Security::AccessToken::PermissionStateFull> &permissionState,
    std::vector<Security::AccessToken::PermissionStateFull> &newPermissionState,
    std::vector<std::string> &newRequestPermName)
{
    if (!GetAllReqPermissionStateFull(tokenId, newPermissionState)) {
        APP_LOGE("BundlePermissionMgr::GetNewPermissionStateFull failed");
        return false;
    }
    for (const auto &perm : permissionState) {
        if (std::find_if(newPermissionState.begin(), newPermissionState.end(), [&perm](const auto &newPerm) {
            return newPerm.permissionName == perm.permissionName;
            }) == newPermissionState.end()) {
            APP_LOGD("BundlePermissionMgr::GetNewPermissionStateFull add request permission %{public}s",
                     perm.permissionName.c_str());
            newPermissionState.emplace_back(perm);
            newRequestPermName.emplace_back(perm.permissionName);
        }
    }
    return true;
}

bool BundlePermissionMgr::AddDefineAndRequestPermissions(const Security::AccessToken::AccessTokenID tokenId,
    const InnerBundleInfo &innerBundleInfo, std::vector<std::string> &newRequestPermName)
{
    APP_LOGD("BundlePermissionMgr::AddDefineAndRequestPermissions start");
    std::vector<AccessToken::PermissionDef> defPermList = GetPermissionDefList(innerBundleInfo);
    std::vector<AccessToken::PermissionDef> newDefPermList;
    if (!GetNewPermissionDefList(tokenId, defPermList, newDefPermList)) {
        return false;
    }

    std::vector<AccessToken::PermissionStateFull> reqPermissionStateList = GetPermissionStateFullList(innerBundleInfo);
    std::vector<AccessToken::PermissionStateFull> newPermissionStateList;
    if (!GetNewPermissionStateFull(tokenId, reqPermissionStateList, newPermissionStateList, newRequestPermName)) {
        return false;
    }

    AccessToken::HapPolicyParams hapPolicy;
    std::string apl = innerBundleInfo.GetAppPrivilegeLevel();
    APP_LOGD("BundlePermissionMgr::AddDefineAndRequestPermissions apl : %{public}s, newDefPermList size : %{public}zu, \
             newPermissionStateList size : %{public}zu", apl.c_str(), newDefPermList.size(),
             newPermissionStateList.size());
    hapPolicy.apl = GetTokenApl(apl);
    hapPolicy.domain = "domain"; // default
    hapPolicy.permList = newDefPermList;
    hapPolicy.permStateList = newPermissionStateList;
    std::string appId = innerBundleInfo.GetAppId();
    int32_t ret = AccessToken::AccessTokenKit::UpdateHapToken(tokenId, appId, hapPolicy);
    if (ret != AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("BundlePermissionMgr::AddDefineAndRequestPermissions UpdateHapToken failed errcode: %{public}d", ret);
        return false;
    }
    APP_LOGD("BundlePermissionMgr::AddDefineAndRequestPermissions end");
    return true;
}

int32_t BundlePermissionMgr::DeleteAccessTokenId(const AccessToken::AccessTokenID tokenId)
{
    return AccessToken::AccessTokenKit::DeleteToken(tokenId);
}

int32_t BundlePermissionMgr::ClearUserGrantedPermissionState(const AccessToken::AccessTokenID tokenId)
{
    return AccessToken::AccessTokenKit::ClearUserGrantedPermissionState(tokenId);
}

std::vector<AccessToken::PermissionDef> BundlePermissionMgr::GetPermissionDefList(
    const InnerBundleInfo &innerBundleInfo)
{
    const auto bundleName = innerBundleInfo.GetBundleName();
    const auto defPermissions = innerBundleInfo.GetAllDefinePermissions();
    std::vector<AccessToken::PermissionDef> permList;
    if (!defPermissions.empty()) {
        for (const auto &defPermission : defPermissions) {
            AccessToken::PermissionDef perm;
            APP_LOGD("defPermission %{public}s", defPermission.name.c_str());
            ConvertPermissionDef(perm, defPermission, bundleName);
            permList.emplace_back(perm);
        }
    }
    return permList;
}

std::vector<AccessToken::PermissionStateFull> BundlePermissionMgr::GetPermissionStateFullList(
    const InnerBundleInfo &innerBundleInfo)
{
    auto reqPermissions = innerBundleInfo.GetAllRequestPermissions();
    std::vector<std::string> grantPermList;
    std::vector<AccessToken::PermissionStateFull> permStateFullList;
    if (!reqPermissions.empty()) {
        for (const auto &reqPermission : reqPermissions) {
            AccessToken::PermissionStateFull perState;
            perState.permissionName = reqPermission.name;
            perState.isGeneral = true;
            perState.resDeviceID.emplace_back(innerBundleInfo.GetBaseApplicationInfo().deviceId);
            perState.grantStatus.emplace_back(AccessToken::PermissionState::PERMISSION_DENIED);
            perState.grantFlags.emplace_back(AccessToken::PermissionFlag::PERMISSION_DEFAULT_FLAG);
            permStateFullList.emplace_back(perState);
        }
    } else {
        APP_LOGD("BundlePermissionMgr::GetPermissionStateFullList requestPermission is empty");
    }
    return permStateFullList;
}

bool BundlePermissionMgr::GrantPermission(
    const Security::AccessToken::AccessTokenID tokenId,
    const std::string &permissionName,
    const Security::AccessToken::PermissionFlag flag,
    const std::string &bundleName)
{
    int32_t ret = AccessToken::AccessTokenKit::GrantPermission(tokenId, permissionName, flag);
    if (ret != AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("GrantPermission failed, bundleName:%{public}s, request permission:%{public}s, err:%{public}d",
            bundleName.c_str(), permissionName.c_str(), ret);
        return false;
    }
    return true;
}

bool BundlePermissionMgr::InnerGrantRequestPermissions(Security::AccessToken::AccessTokenID tokenId,
    const std::vector<RequestPermission> &reqPermissions,
    const InnerBundleInfo &innerBundleInfo)
{
    std::string bundleName = innerBundleInfo.GetBundleName();
    APP_LOGD("InnerGrantRequestPermissions start, bundleName:%{public}s", bundleName.c_str());
    std::string apl = innerBundleInfo.GetAppPrivilegeLevel();
    std::vector<std::string> acls = innerBundleInfo.GetAllowedAcls();
    std::vector<std::string> systemGrantPermList;
    std::vector<std::string> userGrantPermList;
    for (const auto &reqPermission : reqPermissions) {
        APP_LOGD("InnerGrantRequestPermissions add request permission %{public}s", reqPermission.name.c_str());
        AccessToken::PermissionDef permDef;
        int32_t ret = AccessToken::AccessTokenKit::GetDefPermission(reqPermission.name, permDef);
        if (ret != AccessToken::AccessTokenKitRet::RET_SUCCESS) {
            APP_LOGE("get permission def failed, request permission name: %{public}s", reqPermission.name.c_str());
            continue;
        }
        if (CheckGrantPermission(permDef, apl, acls)) {
            if (permDef.grantMode == AccessToken::GrantMode::SYSTEM_GRANT) {
                systemGrantPermList.emplace_back(reqPermission.name);
            } else {
                userGrantPermList.emplace_back(reqPermission.name);
            }
        } else {
            return false;
        }
    }
    APP_LOGD("bundleName:%{public}s, add system_grant permission: %{public}zu, add user_grant permission: %{public}zu",
        bundleName.c_str(), systemGrantPermList.size(), userGrantPermList.size());
    for (const auto &perm : systemGrantPermList) {
        if (!GrantPermission(tokenId, perm, AccessToken::PermissionFlag::PERMISSION_SYSTEM_FIXED, bundleName)) {
            return false;
        }
    }
    if (innerBundleInfo.IsPreInstallApp()) {
        for (const auto &perm: userGrantPermList) {
            bool userCancellable = false;
            if (!CheckPermissionInDefaultPermissions(bundleName, perm, userCancellable)) {
                continue;
            }
            AccessToken::PermissionFlag flag = userCancellable ?
                AccessToken::PermissionFlag::PERMISSION_DEFAULT_FLAG :
                AccessToken::PermissionFlag::PERMISSION_SYSTEM_FIXED;
            if (!GrantPermission(tokenId, perm, flag, bundleName)) {
                return false;
            }
        }
    }
    APP_LOGD("InnerGrantRequestPermissions end, bundleName:%{public}s", bundleName.c_str());
    return true;
}

bool BundlePermissionMgr::GrantRequestPermissions(const InnerBundleInfo &innerBundleInfo,
    const AccessToken::AccessTokenID tokenId)
{
    std::vector<RequestPermission> reqPermissions = innerBundleInfo.GetAllRequestPermissions();
    return InnerGrantRequestPermissions(tokenId, reqPermissions, innerBundleInfo);
}

bool BundlePermissionMgr::GrantRequestPermissions(const InnerBundleInfo &innerBundleInfo,
    const std::vector<std::string> &requestPermName,
    const AccessToken::AccessTokenID tokenId)
{
    std::vector<RequestPermission> reqPermissions = innerBundleInfo.GetAllRequestPermissions();
    std::vector<RequestPermission> newRequestPermissions;
    for (const auto &name : requestPermName) {
        auto iter = find_if(reqPermissions.begin(), reqPermissions.end(), [&name](const auto &req) {
            return name == req.name;
        });
        if (iter != reqPermissions.end()) {
            newRequestPermissions.emplace_back(*iter);
        }
    }
    return InnerGrantRequestPermissions(tokenId, newRequestPermissions, innerBundleInfo);
}

bool BundlePermissionMgr::GetAllReqPermissionStateFull(AccessToken::AccessTokenID tokenId,
    std::vector<AccessToken::PermissionStateFull> &newPermissionState)
{
    std::vector<AccessToken::PermissionStateFull> userGrantReqPermList;
    int32_t ret = AccessToken::AccessTokenKit::GetReqPermissions(tokenId, userGrantReqPermList, false);
    if (ret != AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("GetAllReqPermissionStateFull get user grant failed errcode: %{public}d", ret);
        return false;
    }
    std::vector<AccessToken::PermissionStateFull> systemGrantReqPermList;
    ret = AccessToken::AccessTokenKit::GetReqPermissions(tokenId, systemGrantReqPermList, true);
    if (ret != AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        APP_LOGE("GetAllReqPermissionStateFull get system grant failed errcode: %{public}d", ret);
        return false;
    }
    newPermissionState = userGrantReqPermList;
    for (auto &perm : systemGrantReqPermList) {
        newPermissionState.emplace_back(perm);
    }
    return true;
}

bool BundlePermissionMgr::GetRequestPermissionStates(BundleInfo &bundleInfo)
{
    std::vector<std::string> requestPermission = bundleInfo.reqPermissions;
    if (requestPermission.empty()) {
        APP_LOGD("GetRequestPermissionStates requestPermission empty");
        return true;
    }
    uint32_t tokenId = bundleInfo.applicationInfo.accessTokenId;
    std::vector<Security::AccessToken::PermissionStateFull> allPermissionState;
    if (!GetAllReqPermissionStateFull(tokenId, allPermissionState)) {
        APP_LOGE("BundlePermissionMgr::GetRequestPermissionStates failed");
        return false;
    }
    std::string deviceId = bundleInfo.applicationInfo.deviceId;
    for (auto &req : requestPermission) {
        auto iter = std::find_if(allPermissionState.begin(), allPermissionState.end(),
            [&req](const auto &perm) {
                return perm.permissionName == req;
            });
        if (iter != allPermissionState.end()) {
            APP_LOGD("GetRequestPermissionStates request permission name: %{public}s", req.c_str());
            for (std::vector<std::string>::size_type i = 0; i < iter->resDeviceID.size(); i++) {
                if (iter->resDeviceID[i] == deviceId) {
                    bundleInfo.reqPermissionStates.emplace_back(iter->grantStatus[i]);
                    break;
                }
            }
        } else {
            APP_LOGE("request permission name : %{public}s is not exit", req.c_str());
            bundleInfo.reqPermissionStates.emplace_back(Constants::PERMISSION_NOT_GRANTED);
        }
    }
    return true;
}

bool BundlePermissionMgr::CheckGrantPermission(
    const AccessToken::PermissionDef &permDef,
    const std::string &apl,
    const std::vector<std::string> &acls)
{
    AccessToken::ATokenAplEnum availableLevel = permDef.availableLevel;
    APP_LOGD("BundlePermissionMgr::CheckGrantPermission availableLevel %{public}d, apl %{public}s",
             availableLevel, apl.c_str());
    switch (availableLevel) {
        case AccessToken::ATokenAplEnum::APL_NORMAL: {
            return true;
        }
        case AccessToken::ATokenAplEnum::APL_SYSTEM_BASIC: {
            if ((apl == Profile::AVAILABLELEVEL_SYSTEM_BASIC) ||
                (apl == Profile::AVAILABLELEVEL_SYSTEM_CORE)) {
                return true;
            }
            break;
        }
        case AccessToken::ATokenAplEnum::APL_SYSTEM_CORE: {
            if (apl == Profile::AVAILABLELEVEL_SYSTEM_CORE) {
                return true;
            }
            break;
        }
        default:
            APP_LOGE("availableLevel %{public}d error", availableLevel);
            break;
    }
    if (permDef.provisionEnable) {
        APP_LOGD("CheckGrantPermission acls size: %{public}zu", acls.size());
        for (auto &perm : acls) {
            if (permDef.permissionName == perm) {
                return true;
            }
        }
    }
    APP_LOGE("BundlePermissionMgr::CheckGrantPermission failed permission name : %{public}s",
             permDef.permissionName.c_str());
    return false;
}

bool BundlePermissionMgr::VerifyCallingPermission(const std::string &permissionName)
{
    APP_LOGD("VerifyCallingPermission permission %{public}s", permissionName.c_str());
    AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    APP_LOGD("callerToken : %{private}u", callerToken);
    AccessToken::ATokenTypeEnum tokenType = AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        APP_LOGD("caller tokenType is native, verify success");
        return true;
    }
    int32_t ret = AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permissionName);
    if (ret == AccessToken::PermissionState::PERMISSION_DENIED) {
        APP_LOGE("permission %{public}s: PERMISSION_DENIED", permissionName.c_str());
        return false;
    }
    APP_LOGD("verify AccessToken success");
    return true;
}

int32_t BundlePermissionMgr::VerifyPermission(
    const std::string &bundleName, const std::string &permissionName, const int32_t userId)
{
    APP_LOGD("VerifyPermission bundleName %{public}s, permission %{public}s", bundleName.c_str(),
             permissionName.c_str());
    AccessToken::AccessTokenID tokenId = AccessToken::AccessTokenKit::GetHapTokenID(userId,
        bundleName, 0);
    return AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, permissionName);
}

bool BundlePermissionMgr::GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef)
{
    APP_LOGD("BundlePermissionMgr::GetPermissionDef permission %{public}s", permissionName.c_str());
    AccessToken::PermissionDef accessTokenPermDef;
    int32_t ret = AccessToken::AccessTokenKit::GetDefPermission(permissionName, accessTokenPermDef);
    if (ret == AccessToken::AccessTokenKitRet::RET_SUCCESS) {
        ConvertPermissionDef(accessTokenPermDef, permissionDef);
        return true;
    }
    return false;
}

bool BundlePermissionMgr::CheckPermissionInDefaultPermissions(const std::string &bundleName,
    const std::string &permissionName, bool &userCancellable)
{
    auto iterBundleName = defaultPermissions_.find(bundleName);
    if (iterBundleName == defaultPermissions_.end()) {
        APP_LOGW("bundleName: %{public}s does not exist in defaultPermissions",
            bundleName.c_str());
        return false;
    }
    std::map<std::string, bool> info = iterBundleName->second;
    auto iterPermission = info.find(permissionName);
    if (iterPermission == info.end()) {
        APP_LOGW("bundleName: %{public}s, permissionName: %{public}s does not exist in defaultPermissions",
            bundleName.c_str(), permissionName.c_str());
        return false;
    }
    userCancellable = iterPermission->second;
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS