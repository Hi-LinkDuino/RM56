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

#include "accesstoken_kit.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
AccessTokenIDEx AccessTokenKit::AllocHapToken(const HapInfoParams& info, const HapPolicyParams& policy)
{
    AccessTokenIDEx token;
    token.tokenIDEx = 1;
    token.tokenIdExStruct.tokenID = 1;
    token.tokenIdExStruct.tokenAttr = 1;
    return token;
}

int AccessTokenKit::UpdateHapToken(AccessTokenID tokenID, const std::string& appIDDesc, const HapPolicyParams& policy)
{
    return 0;
}

int AccessTokenKit::GetDefPermissions(AccessTokenID tokenID, std::vector<PermissionDef>& permList)
{
    return 0;
}

int AccessTokenKit::GetDefPermission(const std::string& permissionName, PermissionDef& permissionDefResult)
{
    return -1;
}

int AccessTokenKit::GrantPermission(AccessTokenID tokenID, const std::string& permissionName, int flag)
{
    return 0;
}

int AccessTokenKit::GetReqPermissions(AccessTokenID tokenID, std::vector<PermissionStateFull>& reqPermList,
    bool isSystemGrant)
{
    return 0;
}

int AccessTokenKit::VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName)
{
    return 0;
}

int AccessTokenKit::VerifyAccessToken(
    AccessTokenID callerTokenID, AccessTokenID firstTokenID, const std::string& permissionName)
{
    return 0;
}

ATokenTypeEnum AccessTokenKit::GetTokenTypeFlag(AccessTokenID tokenID)
{
    return TOKEN_NATIVE;
}

int AccessTokenKit::DeleteToken(AccessTokenID tokenID)
{
    return 0;
}

int AccessTokenKit::ClearUserGrantedPermissionState(AccessTokenID tokenID)
{
    return 0;
}

AccessTokenID AccessTokenKit::GetHapTokenID(int userID, const std::string& bundleName, int instIndex)
{
    return 0;
}
}
}
}