/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_response.h"

#include <dlfcn.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ipc_skeleton.h"

#include "hks_log.h"
#include "hks_mem.h"
#include "hks_type_inner.h"

#ifdef SUPPORT_ACCESS_TOCKEN
#include "accesstoken_kit.h"
#endif

#ifdef SUPPORT_ACCOUNT_API
#include "os_account_manager.h"
#endif

using namespace OHOS;

void HksSendResponse(const uint8_t *context, int32_t result, const struct HksBlob *response)
{
    if (context == nullptr) {
        HKS_LOG_E("SendResponse NULL Pointer");
        return;
    }

    MessageParcel *reply = (MessageParcel *)context;
    reply->WriteInt32(result);
    if (response == nullptr) {
        reply->WriteUint32(0);
    } else {
        reply->WriteUint32(response->size);
        reply->WriteBuffer(response->data, (size_t)response->size);
    }
}

int32_t HksGetProcessNameForIPC(const uint8_t *context, struct HksBlob *processName)
{
    if ((context == nullptr) || (processName == nullptr)) {
        HKS_LOG_D("Don't need get process name in hosp.");
        return HKS_SUCCESS;
    }

    auto callingUid = IPCSkeleton::GetCallingUid();
    uint8_t *name = (uint8_t *)HksMalloc(sizeof(callingUid));
    if (name == nullptr) {
        HKS_LOG_E("GetProcessName malloc failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memcpy_s(name, sizeof(callingUid), &callingUid, sizeof(callingUid));
    processName->size = sizeof(callingUid);
    processName->data = name;
    return HKS_SUCCESS;
}

int32_t HksGetProcessInfoForIPC(const uint8_t *context, struct HksProcessInfo *processInfo)
{
    if ((context == nullptr) || (processInfo == nullptr)) {
        HKS_LOG_D("Don't need get process name in hosp.");
        return HKS_SUCCESS;
    }

    auto callingUid = IPCSkeleton::GetCallingUid();
    uint8_t *name = (uint8_t *)HksMalloc(sizeof(callingUid));
    if (name == nullptr) {
        HKS_LOG_E("GetProcessName malloc failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memcpy_s(name, sizeof(callingUid), &callingUid, sizeof(callingUid));
    processInfo->processName.size = sizeof(callingUid);
    processInfo->processName.data = name;

    int userId = 0;
#ifdef SUPPORT_ACCOUNT_API
    OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    HKS_LOG_I("HksGetProcessInfoForIPC callingUid = %d, userId = %d", callingUid, userId);
#endif

    uint32_t size;
    if (userId == 0) {
        size = strlen("0");
    } else {
        size = sizeof(userId);
    }

    uint8_t *name1 = (uint8_t *)HksMalloc(size);
    if (name1 == nullptr) {
        HKS_LOG_E("user id malloc failed.");
        HksFree(name);
        processInfo->userId.data = nullptr;
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (userId == 0) {
        (void)memcpy_s(name1, size, "0", size); /* ignore \0 at the end */
    } else {
        (void)memcpy_s(name1, size, &userId, size);
    }

    processInfo->userId.size = size;
    processInfo->userId.data = name1;

    return HKS_SUCCESS;
}

#ifdef SUPPORT_ACCESS_TOCKEN
int32_t Apl3Check()
{
    OHOS::Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    HKS_LOG_I("AccessTokenID is %llx!", tokenId);
    OHOS::Security::AccessToken::ATokenTypeEnum tokenType =
    OHOS::Security::AccessToken::AccessTokenKit::GetTokenType(tokenId);
    if (tokenType == OHOS::Security::AccessToken::TOKEN_HAP) {
        OHOS::Security::AccessToken::HapTokenInfo findInfo;
        if (OHOS::Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, findInfo) != 0) {
            HKS_LOG_E("GetHapTokenInfo failed!");
            return HKS_FAILURE;
        }
        if (findInfo.apl == OHOS::Security::AccessToken::APL_SYSTEM_CORE) {
            HKS_LOG_I("check APL3 success!");
            return HKS_SUCCESS;
        } else {
            HKS_LOG_E("check APL3 failed!");
            return HKS_FAILURE;
        }
    }

    if (tokenType == OHOS::Security::AccessToken::TOKEN_NATIVE) {
        OHOS::Security::AccessToken::NativeTokenInfo findInfo;
        if (OHOS::Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenId, findInfo) != 0) {
            HKS_LOG_E("GetNativeTokenInfo failed!");
            return HKS_FAILURE;
        }
        if (findInfo.apl == OHOS::Security::AccessToken::APL_SYSTEM_CORE) {
            HKS_LOG_I("check APL3 success!");
            return HKS_SUCCESS;
        } else {
            HKS_LOG_E("check APL3 failed!");
            return HKS_FAILURE;
        }
    }
    HKS_LOG_E("GetTokenTypeFlag type wrong!");
    return HKS_FAILURE;
}
#endif
