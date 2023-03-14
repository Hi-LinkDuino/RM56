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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_ACCESS_TOKEN_KIT_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_ACCESS_TOKEN_KIT_H

#include <string>
#include <vector>

#include "access_token.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
class AccessTokenKit {
public:
    static AccessTokenIDEx AllocHapToken(const HapInfoParams& info, const HapPolicyParams& policy);
    static int UpdateHapToken(AccessTokenID tokenID, const std::string& appIDDesc, const HapPolicyParams& policy);
    static int DeleteToken(AccessTokenID tokenID);
    /* Get token type by ATM service */
    static ATokenTypeEnum GetTokenType(AccessTokenID tokenID);
    /* Get token type from flag in tokenId, which doesn't depend on ATM service */
    static ATokenTypeEnum GetTokenTypeFlag(AccessTokenID tokenID);
    static int VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName);
    static int VerifyAccessToken(
        AccessTokenID callerTokenID, AccessTokenID firstTokenID, const std::string& permissionName);
    static int GetDefPermission(const std::string& permissionName, PermissionDef& permissionDefResult);
    static int GetDefPermissions(AccessTokenID tokenID, std::vector<PermissionDef>& permList);
    static int GetReqPermissions(
        AccessTokenID tokenID, std::vector<PermissionStateFull>& reqPermList, bool isSystemGrant);
    static int GrantPermission(AccessTokenID tokenID, const std::string& permissionName, int flag);
    static int ClearUserGrantedPermissionState(AccessTokenID tokenID);
    static AccessTokenID GetHapTokenID(int userID, const std::string& bundleName, int instIndex);
};
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_ACCESS_TOKEN_KIT_H