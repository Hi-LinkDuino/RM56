/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "device_auth.h"
#include "dm_constants.h"

int32_t (*deleteGroup)(int64_t requestId, const char *appId, const char *disbandParams)
{
    if (disbandParams == 0) {
        return DM_FAILED;
    }

    return DM_OK;
}

int32_t (*getRelatedGroups)(const char *appId, const char *peerDeviceId, char **returnGroupVec, uint32_t *groupNum)
{
    (void)appId;
    (void)peerDeviceId;
    (void)returnGroupVec;
    (void)groupNum;
    return 0;
}

int32_t (*createGroup)(int64_t requestId, const char *appId, const char *createParams)
{
    if (requestId == 0) {
        return DM_FAILED;
    }
    (void)appId;
    (void)createParams;
    return 0;
}

int32_t (*getRelatedGroups)(const char *appId, const char *peerDeviceId, char **returnGroupVec, uint32_t *groupNum)
{
    if (peerDeviceId == "123") {
        returnGroupVec = nullptr;
    }

    if (peerDeviceId == "12345") {
        groupNum = 0;
    }

    if (peerDeviceId == "34567") {
        groupNum = 1;
        returnGroupVec = "123";
    }
}
