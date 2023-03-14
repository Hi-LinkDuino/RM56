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

#ifndef PEER_TO_PEER_GROUP_H
#define PEER_TO_PEER_GROUP_H

#include "base_group.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    BaseGroup base;
    int32_t (*addMember)(int32_t osAccountId, int64_t requestId, CJson *jsonParams,
        const DeviceAuthCallback *callback);
    int32_t (*deleteMember)(int32_t osAccountId, int64_t requestId, CJson *jsonParams,
        const DeviceAuthCallback *callback);
    int32_t (*processData)(int64_t requestId, CJson *jsonParams,
        const DeviceAuthCallback *callback);
    int32_t (*addGroupRole)(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
        const char *roleAppId);
    int32_t (*deleteGroupRole)(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
        const char *roleAppId);
    int32_t (*getGroupRoles)(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
        char **returnJsonStr, uint32_t *returnSize);
} PeerToPeerGroup;

BaseGroup *GetPeerToPeerGroupInstance(void);
bool IsPeerToPeerGroupSupported(void);

#ifdef __cplusplus
}
#endif
#endif
