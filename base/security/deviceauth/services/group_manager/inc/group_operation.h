/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef GROUP_OPERATION_H
#define GROUP_OPERATION_H

#include "device_auth.h"
#include "json_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t (*createGroup)(int32_t osAccountId, int64_t requestId, const char *appId, const char *createParams);
    int32_t (*deleteGroup)(int32_t osAccountId, int64_t requestId, const char *appId, const char *disbandParams);
    int32_t (*addMember)(int32_t osAccountId, int64_t requestId, const char *appId, const char *addParams);
    int32_t (*deleteMember)(int32_t osAccountId, int64_t requestId, const char *appId, const char *deleteParams);
    int32_t (*processBindData)(int64_t requestId, const uint8_t *data, uint32_t dataLen);
    int32_t (*confirmRequest)(int32_t osAccountId, int64_t requestId, const char *appId, const char *confirmParams);
    int32_t (*generateAccountGroupId)(int32_t groupType, const char *userIdHash, const char *sharedUserIdHash,
        char **returnGroupId);
    int32_t (*syncAcrossAccountGroup)(const char *appId, const char *userIdHash, const char *deviceId,
        const CJson *sharedUserIdHashList);
    int32_t (*addGroupRole)(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
        const char *roleAppId);
    int32_t (*deleteGroupRole)(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
        const char *roleAppId);
    int32_t (*getGroupRoles)(int32_t osAccuntId, bool isManager, const char *appId, const char *groupId,
        char **returnJsonStr, uint32_t *returnSize);
    int32_t (*regListener)(const char *appId, const DataChangeListener *listener);
    int32_t (*unRegListener)(const char *appId);
    int32_t (*checkAccessToGroup)(int32_t osAccountId, const char *appId, const char *groupId);
    int32_t (*getAccessibleGroupInfoById)(int32_t osAccountId, const char *appId, const char *groupId,
        char **returnGroupInfo);
    int32_t (*getAccessibleGroupInfo)(int32_t osAccountId, const char *appId, const char *queryParams,
        char **returnGroupVec, uint32_t *groupNum);
    int32_t (*getAccessibleJoinedGroups)(int32_t osAccountId, const char *appId, int groupType,
        char **returnGroupVec, uint32_t *groupNum);
    int32_t (*getAccessibleRelatedGroups)(int32_t osAccountId, const char *appId, const char *peerDeviceId, bool isUdid,
        char **returnGroupVec, uint32_t *groupNum);
    int32_t (*getAccessibleDeviceInfoById)(int32_t osAccountId, const char *appId, const char *deviceId, bool isUdid,
        const char *groupId, char **returnDeviceInfo);
    int32_t (*getAccessibleTrustedDevices)(int32_t osAccountId, const char *appId, const char *groupId,
        char **returnDevInfoVec, uint32_t *deviceNum);
    bool (*isDeviceInAccessibleGroup)(int32_t osAccountId, const char *appId, const char *groupId,
        const char *deviceId, bool isUdid);
    int32_t (*getPkInfoList)(int32_t osAccountId, const char *appId, const char *queryParams, char **returnInfoList,
        uint32_t *returnInfoNum);
    void (*destroyInfo)(char **returnInfo);
} GroupImpl;

int32_t InitGroupRelatedModule(void);
void DestroyGroupRelatedModule(void);
const GroupImpl *GetGroupImplInstance(void);
bool IsGroupSupport(void);

#ifdef __cplusplus
}
#endif
#endif
