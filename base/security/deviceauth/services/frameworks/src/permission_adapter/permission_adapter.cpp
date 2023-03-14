/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "permission_adapter.h"

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"

#include "device_auth_defines.h"
#include "hc_log.h"

using namespace OHOS;
using namespace OHOS::Security::AccessToken;

int32_t CheckPermission(void)
{
    AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenType(tokenId);
    if (tokenType == TOKEN_NATIVE) {
        NativeTokenInfo findInfo;
        if (AccessTokenKit::GetNativeTokenInfo(tokenId, findInfo) != 0) {
            LOGE("GetNativeTokenInfo failed!");
            return HC_ERROR;
        }
        if (findInfo.apl == APL_SYSTEM_CORE) {
            LOGI("Check permission(APL3=SYSTEM_CORE) success!");
            return HC_SUCCESS;
        } else {
            LOGE("Check permission(APL3=SYSTEM_CORE) failed! APL: %d", findInfo.apl);
            return HC_ERROR;
        }
    } else {
        LOGE("Invalid token type: %d", tokenType);
        return HC_ERROR;
    }
}