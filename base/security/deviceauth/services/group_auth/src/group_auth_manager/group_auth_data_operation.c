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

#include "group_auth_data_operation.h"
#include "auth_session_util.h"
#include "common_defs.h"
#include "device_auth.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_string.h"
#include "hc_vector.h"

static bool GaDeepCopyDeviceEntry(const TrustedDeviceEntry *entry, TrustedDeviceEntry *returnEntry)
{
    returnEntry->groupEntry = NULL;
    if (!StringSet(&returnEntry->groupId, entry->groupId)) {
        LOGE("[GA]: Failed to copy udid!");
        return false;
    }
    if (!StringSet(&returnEntry->udid, entry->udid)) {
        LOGE("[GA]: Failed to copy udid!");
        return false;
    }
    if (!StringSet(&returnEntry->authId, entry->authId)) {
        LOGE("[GA]: Failed to copy authId!");
        return false;
    }
    if (!StringSet(&returnEntry->serviceType, entry->serviceType)) {
        LOGE("[GA]: Failed to copy serviceType!");
        return false;
    }
    if (!StringSet(&returnEntry->userIdHash, entry->userIdHash)) {
        LOGE("[GA]: Failed to copy userIdHash!");
        return false;
    }
    returnEntry->credential = entry->credential;
    returnEntry->devType = entry->devType;
    returnEntry->lastTm = entry->lastTm;
    return true;
}

static bool GaDeepCopyGroupEntry(const TrustedGroupEntry *entry, TrustedGroupEntry *returnEntry)
{
    if (HC_VECTOR_SIZE(&entry->managers) <= 0) {
        LOGE("[GA]: The group owner is lost!");
        return false;
    }
    HcString entryOwner = HC_VECTOR_GET(&entry->managers, 0);
    if (!StringSet(&returnEntry->name, entry->name)) {
        LOGE("[GA]: Failed to copy groupName!");
        return false;
    }
    if (!StringSet(&returnEntry->id, entry->id)) {
        LOGE("[GA]: Failed to copy groupId!");
        return false;
    }
    if (!StringSet(&returnEntry->userIdHash, entry->userIdHash)) {
        LOGE("[GA]: Failed to copy userIdHash!");
        return false;
    }
    returnEntry->type = entry->type;
    returnEntry->visibility = entry->visibility;
    returnEntry->expireTime = entry->expireTime;
    HcString ownerName = CreateString();
    if (!StringSet(&ownerName, entryOwner)) {
        LOGE("[GA]: Failed to copy groupOwner!");
        DeleteString(&ownerName);
        return false;
    }
    if (returnEntry->managers.pushBack(&returnEntry->managers, &ownerName) == NULL) {
        LOGE("[GA]: Failed to push groupOwner to managers!");
        DeleteString(&ownerName);
        return false;
    }
    return true;
}

static bool GaIsGroupManager(const char *appId, const TrustedGroupEntry *entry)
{
    uint32_t index;
    HcString *manager = NULL;
    const char *managerStr = NULL;
    FOR_EACH_HC_VECTOR(entry->managers, index, manager) {
        managerStr = StringGet(manager);
        if ((managerStr != NULL) && (strcmp(managerStr, appId) == 0)) {
            return true;
        }
    }
    return false;
}

static bool GaIsGroupFriend(const char *appId, const TrustedGroupEntry *entry)
{
    uint32_t index;
    HcString *trustedFriend = NULL;
    const char *friendStr = NULL;
    FOR_EACH_HC_VECTOR(entry->friends, index, trustedFriend) {
        friendStr = StringGet(trustedFriend);
        if ((friendStr != NULL) && (strcmp(friendStr, appId) == 0)) {
            return true;
        }
    }
    return false;
}

static int32_t GetGroupEntryById(int32_t osAccountId, const char *groupId, TrustedGroupEntry *returnEntry)
{
    if (returnEntry == NULL) {
        LOGE("The input returnEntry is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    uint32_t groupIndex;
    TrustedGroupEntry **entry = NULL;
    GroupEntryVec groupEntryVec = CreateGroupEntryVec();
    QueryGroupParams groupParams = InitQueryGroupParams();
    groupParams.groupId = groupId;
    if (QueryGroups(osAccountId, &groupParams, &groupEntryVec) != HC_SUCCESS) {
        LOGE("query groups failed!");
        ClearGroupEntryVec(&groupEntryVec);
        return HC_ERR_GROUP_NOT_EXIST;
    }
    FOR_EACH_HC_VECTOR(groupEntryVec, groupIndex, entry) {
        if ((entry != NULL) && (*entry != NULL)) {
            if (!GaDeepCopyGroupEntry(*entry, returnEntry)) {
                ClearGroupEntryVec(&groupEntryVec);
                return HC_ERR_GROUP_NOT_EXIST;
            }
            ClearGroupEntryVec(&groupEntryVec);
            return HC_SUCCESS;
        }
    }
    ClearGroupEntryVec(&groupEntryVec);
    return HC_ERR_GROUP_NOT_EXIST;
}

bool GaIsGroupAccessible(int32_t osAccountId, const char *groupId, const char *appId)
{
    if ((groupId == NULL) || (appId == NULL)) {
        LOGE("The input groupId or appId is NULL!");
        return false;
    }
    TrustedGroupEntry *entry = CreateGroupEntry();
    if (entry == NULL) {
        LOGE("Failed to create group entry!");
        return false;
    }
    int32_t res = GetGroupEntryById(osAccountId, groupId, entry);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get group entry by groupId!");
        DestroyGroupEntry(entry);
        return false;
    }
    if ((entry->visibility == GROUP_VISIBILITY_PUBLIC) ||
        (GaIsGroupManager(appId, entry)) ||
        (GaIsGroupFriend(appId, entry))) {
        DestroyGroupEntry(entry);
        return true;
    }
    DestroyGroupEntry(entry);
    return false;
}

int32_t GaGetTrustedDeviceEntryById(int32_t osAccountId, const char *deviceId,
    bool isUdid, const char *groupId, TrustedDeviceEntry *returnDeviceEntry)
{
    if (returnDeviceEntry == NULL) {
        LOGE("The input returnEntry is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    uint32_t index;
    TrustedDeviceEntry **deviceEntry = NULL;
    DeviceEntryVec deviceEntryVec = CREATE_HC_VECTOR(DeviceEntryVec);
    QueryDeviceParams params = InitQueryDeviceParams();
    params.groupId = groupId;
    if (isUdid) {
        params.udid = deviceId;
    } else {
        params.authId = deviceId;
    }
    if (QueryDevices(osAccountId, &params, &deviceEntryVec) != HC_SUCCESS) {
        LOGE("query trusted devices failed!");
        ClearDeviceEntryVec(&deviceEntryVec);
        return HC_ERR_DEVICE_NOT_EXIST;
    }
    FOR_EACH_HC_VECTOR(deviceEntryVec, index, deviceEntry) {
        if ((deviceEntry != NULL) && (*deviceEntry != NULL)) {
            if (!GaDeepCopyDeviceEntry(*deviceEntry, returnDeviceEntry)) {
                ClearDeviceEntryVec(&deviceEntryVec);
                return HC_ERR_GROUP_NOT_EXIST;
            }
            ClearDeviceEntryVec(&deviceEntryVec);
            return HC_SUCCESS;
        }
    }
    ClearDeviceEntryVec(&deviceEntryVec);
    return HC_ERR_DEVICE_NOT_EXIST;
}

bool GaIsDeviceInGroup(int32_t groupType, int32_t osAccountId, const char *peerUdid, const char *peerAuthId,
    const char *groupId)
{
    int32_t res;
    int32_t authForm = GroupTypeToAuthForm(groupType);
    if ((authForm == AUTH_FORM_ACROSS_ACCOUNT) || (authForm == AUTH_FORM_IDENTICAL_ACCOUNT)) {
        LOGD("Auth for account related type.");
        return true; /* Do not check  whether account related devices is in account. */
    }
    TrustedDeviceEntry *deviceEntry = CreateDeviceEntry();
    if (deviceEntry == NULL) {
        LOGE("Failed to allocate memory for deviceEntry!");
        return false;
    }
    if (peerUdid != NULL) {
        res = GaGetTrustedDeviceEntryById(osAccountId, peerUdid, true, groupId, deviceEntry);
    } else if (peerAuthId != NULL) {
        res = GaGetTrustedDeviceEntryById(osAccountId, peerAuthId, false, groupId, deviceEntry);
    } else {
        LOGE("Both the input udid and authId is null!");
        res = HC_ERROR;
    }
    DestroyDeviceEntry(deviceEntry);
    if (res != HC_SUCCESS) {
        return false;
    }
    return true;
}