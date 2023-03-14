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

#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include "device_auth.h"
#include "json_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t InitGroupManager(void);
void DestroyGroupManager(void);

int32_t CreateGroupImpl(int32_t osAccountId, int64_t requestId, const char *appId, const char *createParams);
int32_t DeleteGroupImpl(int32_t osAccountId, int64_t requestId, const char *appId, const char *disbandParams);
int32_t AddMemberToGroupImpl(int32_t osAccountId, int64_t requestId, const char *appId, const char *addParams);
int32_t DeleteMemberFromGroupImpl(int32_t osAccountId, int64_t requestId, const char *appId, const char *deleteParams);
int32_t ProcessBindDataImpl(int64_t requestId, const uint8_t *data, uint32_t dataLen);
int32_t ConfirmRequestImpl(int32_t osAccountId, int64_t requestId, const char *appId, const char *confirmParams);
int32_t GenerateAccountGroupIdImpl(int32_t groupType, const char *userIdHash, const char *sharedUserIdHash,
    char **returnGroupId);
int32_t SyncAcrossAccountGroupImpl(const char *appId, const char *userIdHash, const char *deviceId,
    const CJson *sharedUserIdHashList);
int32_t AddGroupManagerImpl(int32_t osAccountId, const char *appId, const char *groupId, const char *managerAppId);
int32_t AddGroupFriendImpl(int32_t osAccountId, const char *appId, const char *groupId, const char *friendAppId);
int32_t DeleteGroupManagerImpl(int32_t osAccountId, const char *appId, const char *groupId, const char *managerAppId);
int32_t DeleteGroupFriendImpl(int32_t osAccountId, const char *appId, const char *groupId, const char *friendAppId);
int32_t GetGroupManagersImpl(int32_t osAccountId, const char *appId, const char *groupId,
    char **returnManagers, uint32_t *returnSize);
int32_t GetGroupFriendsImpl(int32_t osAccountId, const char *appId, const char *groupId,
    char **returnFriends, uint32_t *returnSize);

int32_t RegListenerImpl(const char *appId, const DataChangeListener *listener);
int32_t UnRegListenerImpl(const char *appId);

int32_t CheckAccessToGroupImpl(int32_t osAccountId, const char *appId, const char *groupId);
int32_t GetGroupInfoByIdImpl(int32_t osAccountId, const char *appId, const char *groupId, char **returnGroupInfo);
int32_t GetGroupInfoImpl(int32_t osAccountId, const char *appId, const char *queryParams,
    char **returnGroupVec, uint32_t *groupNum);
int32_t GetJoinedGroupsImpl(int32_t osAccountId, const char *appId, int groupType,
    char **returnGroupVec, uint32_t *groupNum);
int32_t GetRelatedGroupsImpl(int32_t osAccountId, const char *appId, const char *peerDeviceId,
    char **returnGroupVec, uint32_t *groupNum);
int32_t GetDeviceInfoByIdImpl(int32_t osAccountId, const char *appId, const char *deviceId, const char *groupId,
    char **returnDeviceInfo);
int32_t GetTrustedDevicesImpl(int32_t osAccountId, const char *appId, const char *groupId,
    char **returnDevInfoVec, uint32_t *deviceNum);
bool IsDeviceInGroupImpl(int32_t osAccountId, const char *appId, const char *groupId, const char *deviceId);
int32_t GetPkInfoListImpl(int32_t osAccountId, const char *appId, const char *queryParams,
    char **returnInfoList, uint32_t *returnInfoNum);
void DestroyInfoImpl(char **returnInfo);

int32_t BindPeerImpl(int64_t requestId, const char *appId, const char *bindParams);
int32_t UnbindPeerImpl(int64_t requestId, const char *appId, const char *unbindParams);
int32_t ProcessLiteDataImpl(int64_t requestId, const char *appId, const uint8_t *data, uint32_t dataLen);

int32_t AuthKeyAgreeImpl(int64_t requestId, const char *appId, const char *agreeParams);
int32_t ProcessKeyAgreeDataImpl(int64_t requestId, const char *appId, const uint8_t *data, uint32_t dataLen);

#ifdef __cplusplus
}
#endif
#endif
