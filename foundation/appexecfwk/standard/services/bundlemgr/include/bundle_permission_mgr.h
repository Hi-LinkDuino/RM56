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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PERMISSION_MGR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PERMISSION_MGR_H

#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "inner_bundle_info.h"
#include "permission_define.h"

namespace OHOS {
namespace AppExecFwk {
class BundlePermissionMgr {
public:
    /**
     * @brief Initialize BundlePermissionMgr, which is only called when the system starts.
     * @return Returns true if successfully initialized BundlePermissionMgr; returns false otherwise.
     */
    static bool Init();

    static void UnInit();

    /**
     * @brief Verify whether a specified bundle has been granted a specific permission.
     * @param bundleName Indicates the name of the bundle to check.
     * @param permission Indicates the permission to check.
     * @param userId Indicates the userId of the bundle.
     * @return Returns 0 if the bundle has the permission; returns -1 otherwise.
     */
    static int32_t VerifyPermission(const std::string &bundleName, const std::string &permissionName,
        const int32_t userId);
    /**
     * @brief Obtains detailed information about a specified permission.
     * @param permissionName Indicates the name of the permission.
     * @param permissionDef Indicates the object containing detailed information about the given permission.
     * @return Returns true if the PermissionDef object is successfully obtained; returns false otherwise.
     */
    static bool GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef);
    /**
     * @brief Requests a certain permission from user.
     * @param bundleName Indicates the name of the bundle.
     * @param permission Indicates the permission to request.
     * @param userId Indicates the userId of the bundle.
     * @return Returns true if the permission request successfully; returns false otherwise.
     */
    static bool RequestPermissionFromUser(
        const std::string &bundleName, const std::string &permissionName, const int32_t userId);

    static Security::AccessToken::AccessTokenID CreateAccessTokenId(
        const InnerBundleInfo &innerBundleInfo, const std::string bundleName, const int32_t userId,
        const int32_t dlpType = 0);

    static bool UpdateDefineAndRequestPermissions(const Security::AccessToken::AccessTokenID tokenId,
        const InnerBundleInfo &oldInfo, const InnerBundleInfo &newInfo, std::vector<std::string> &newRequestPermName);

    static bool AddDefineAndRequestPermissions(const Security::AccessToken::AccessTokenID tokenId,
        const InnerBundleInfo &innerBundleInfo, std::vector<std::string> &newRequestPermName);

    static int32_t DeleteAccessTokenId(const Security::AccessToken::AccessTokenID tokenId);

    static bool GrantRequestPermissions(const InnerBundleInfo &innerBundleInfo,
        const Security::AccessToken::AccessTokenID tokenId);

    static bool GrantRequestPermissions(const InnerBundleInfo &innerBundleInfo,
        const std::vector<std::string> &requestPermName,
        const Security::AccessToken::AccessTokenID tokenId);

    static bool GetRequestPermissionStates(BundleInfo &bundleInfo);

    static int32_t ClearUserGrantedPermissionState(const Security::AccessToken::AccessTokenID tokenId);

    static bool VerifyCallingPermission(const std::string &permissionName);

private:
    static std::vector<Security::AccessToken::PermissionDef> GetPermissionDefList(
        const InnerBundleInfo &innerBundleInfo);

    static std::vector<Security::AccessToken::PermissionStateFull> GetPermissionStateFullList(
        const InnerBundleInfo &innerBundleInfo);

    static bool CheckGrantPermission(const Security::AccessToken::PermissionDef &permDef,
        const std::string &apl,
        const std::vector<std::string> &acls);

    static bool GetNewPermissionDefList(Security::AccessToken::AccessTokenID tokenId,
        const std::vector<Security::AccessToken::PermissionDef> &permissionDef,
        std::vector<Security::AccessToken::PermissionDef> &newPermission);

    static bool GetNewPermissionStateFull(Security::AccessToken::AccessTokenID tokenId,
        const std::vector<Security::AccessToken::PermissionStateFull> &permissionState,
        std::vector<Security::AccessToken::PermissionStateFull> &newPermissionState,
        std::vector<std::string> &newRequestPermName);

    static bool GetAllReqPermissionStateFull(Security::AccessToken::AccessTokenID tokenId,
        std::vector<Security::AccessToken::PermissionStateFull> &newPermissionState);

    static bool InnerGrantRequestPermissions(Security::AccessToken::AccessTokenID tokenId,
        const std::vector<RequestPermission> &reqPermissions,
        const InnerBundleInfo &innerBundleInfo);

    static Security::AccessToken::ATokenAplEnum GetTokenApl(const std::string &apl);

    static Security::AccessToken::HapPolicyParams CreateHapPolicyParam(const InnerBundleInfo &innerBundleInfo);

    static void ConvertPermissionDef(const Security::AccessToken::PermissionDef &permDef,
        PermissionDef &permissionDef);
    static void ConvertPermissionDef(
        Security::AccessToken::PermissionDef &permDef, const DefinePermission &defPermission,
        const std::string &bundleName);

    static std::vector<std::string> GetNeedDeleteDefinePermissionName(const InnerBundleInfo &oldInfo,
        const InnerBundleInfo &newInfo);

    static std::vector<std::string> GetNeedDeleteRequestPermissionName(const InnerBundleInfo &oldInfo,
        const InnerBundleInfo &newInfo);

    static bool CheckPermissionInDefaultPermissions(const std::string &bundleName, const std::string &permissionName,
        bool &userCancellable);

    static bool GrantPermission(const Security::AccessToken::AccessTokenID tokenId,
        const std::string &permissionName, const Security::AccessToken::PermissionFlag flag,
        const std::string &bundleName);

    static std::map<std::string, std::map<std::string, bool>> defaultPermissions_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PERMISSION_MGR_H