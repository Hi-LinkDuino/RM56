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

#include "group_operation_common.h"

#include "alg_loader.h"
#include "string_util.h"
#include "common_defs.h"
#include "data_manager.h"
#include "dev_auth_module_manager.h"
#include "device_auth_defines.h"
#include "group_operation.h"
#include "hal_error.h"
#include "hc_dev_info.h"
#include "hc_log.h"

static bool IsGroupManager(const char *appId, const TrustedGroupEntry *entry)
{
    uint32_t index;
    HcString *manager = NULL;
    FOR_EACH_HC_VECTOR(entry->managers, index, manager) {
        if (strcmp(StringGet(manager), appId) == 0) {
            return true;
        }
    }
    return false;
}

static bool IsGroupFriend(const char *appId, const TrustedGroupEntry *entry)
{
    uint32_t index;
    HcString *trustedFriend = NULL;
    FOR_EACH_HC_VECTOR(entry->friends, index, trustedFriend) {
        if (strcmp(StringGet(trustedFriend), appId) == 0) {
            return true;
        }
    }
    return false;
}

static int32_t GetGroupNumByOwner(int32_t osAccountId, const char *ownerName)
{
    if (ownerName == NULL) {
        LOGE("The input ownerName is NULL!");
        return 0;
    }
    int count = 0;
    QueryGroupParams queryParams = InitQueryGroupParams();
    queryParams.ownerName = ownerName;
    GroupEntryVec groupEntryVec = CreateGroupEntryVec();
    int32_t result = QueryGroups(osAccountId, &queryParams, &groupEntryVec);
    if (result != HC_SUCCESS) {
        LOGE("Failed to query groups!");
        ClearGroupEntryVec(&groupEntryVec);
        return count;
    }
    count = HC_VECTOR_SIZE(&groupEntryVec);
    ClearGroupEntryVec(&groupEntryVec);
    return count;
}

TrustedDeviceEntry *GetTrustedDeviceEntryById(int32_t osAccountId, const char *deviceId, bool isUdid,
    const char *groupId)
{
    uint32_t index;
    TrustedDeviceEntry **deviceEntry = NULL;
    DeviceEntryVec deviceEntryVec = CreateDeviceEntryVec();
    QueryDeviceParams params = InitQueryDeviceParams();
    params.groupId = groupId;
    if (isUdid) {
        params.udid = deviceId;
    } else {
        params.authId = deviceId;
    }
    if (QueryDevices(osAccountId, &params, &deviceEntryVec) != HC_SUCCESS) {
        LOGE("Failed to query trusted devices!");
        ClearDeviceEntryVec(&deviceEntryVec);
        return NULL;
    }
    FOR_EACH_HC_VECTOR(deviceEntryVec, index, deviceEntry) {
        if ((deviceEntry != NULL) && (*deviceEntry != NULL)) {
            TrustedDeviceEntry *returnEntry = DeepCopyDeviceEntry(*deviceEntry);
            ClearDeviceEntryVec(&deviceEntryVec);
            return returnEntry;
        }
    }
    ClearDeviceEntryVec(&deviceEntryVec);
    return NULL;
}

TrustedGroupEntry *GetGroupEntryById(int32_t osAccountId, const char *groupId)
{
    if (groupId == NULL) {
        LOGE("The input groupId is NULL!");
        return NULL;
    }
    uint32_t index;
    TrustedGroupEntry **entry = NULL;
    GroupEntryVec groupEntryVec = CreateGroupEntryVec();
    QueryGroupParams params = InitQueryGroupParams();
    params.groupId = groupId;
    if (QueryGroups(osAccountId, &params, &groupEntryVec) != HC_SUCCESS) {
        LOGE("Failed to query groups!");
        ClearGroupEntryVec(&groupEntryVec);
        return NULL;
    }
    FOR_EACH_HC_VECTOR(groupEntryVec, index, entry) {
        if ((entry != NULL) && (*entry != NULL)) {
            TrustedGroupEntry *returnEntry = DeepCopyGroupEntry(*entry);
            ClearGroupEntryVec(&groupEntryVec);
            return returnEntry;
        }
    }
    ClearGroupEntryVec(&groupEntryVec);
    return NULL;
}

bool IsTrustedDeviceInGroup(int32_t osAccountId, const char *groupId, const char *deviceId, bool isUdid)
{
    if ((groupId == NULL) || (deviceId == NULL)) {
        LOGE("The input groupId or deviceId is NULL!");
        return false;
    }
    TrustedDeviceEntry *entry = GetTrustedDeviceEntryById(osAccountId, deviceId, isUdid, groupId);
    if (entry == NULL) {
        return false;
    }
    DestroyDeviceEntry(entry);
    return true;
}

int32_t GetGroupInfoById(int32_t osAccountId, const char *groupId, TrustedGroupEntry *returnGroupEntry)
{
    if ((groupId == NULL) || (returnGroupEntry == NULL)) {
        LOGE("The input groupId or returnGroupEntry is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    int32_t result;
    uint32_t index;
    TrustedGroupEntry **entry = NULL;
    GroupEntryVec groupEntryVec = CreateGroupEntryVec();
    QueryGroupParams params = InitQueryGroupParams();
    params.groupId = groupId;
    result = QueryGroups(osAccountId, &params, &groupEntryVec);
    if (result != HC_SUCCESS) {
        LOGE("Failed to query groups!");
        ClearGroupEntryVec(&groupEntryVec);
        return result;
    }
    FOR_EACH_HC_VECTOR(groupEntryVec, index, entry) {
        if ((entry == NULL) || (*entry == NULL)) {
            continue;
        }
        result = GenerateGroupEntryFromEntry(*entry, returnGroupEntry) ? HC_SUCCESS : HC_ERR_MEMORY_COPY;
        ClearGroupEntryVec(&groupEntryVec);
        return result;
    }
    ClearGroupEntryVec(&groupEntryVec);
    return HC_ERR_GROUP_NOT_EXIST;
}

bool IsGroupAccessible(int32_t osAccountId, const char *groupId, const char *appId)
{
    if ((groupId == NULL) || (appId == NULL)) {
        LOGE("The input groupId or appId is NULL!");
        return false;
    }
    TrustedGroupEntry *entry = GetGroupEntryById(osAccountId, groupId);
    if (entry == NULL) {
        LOGE("The group cannot be found!");
        return false;
    }
    if ((entry->visibility == GROUP_VISIBILITY_PUBLIC) ||
        (IsGroupManager(appId, entry)) ||
        (IsGroupFriend(appId, entry))) {
        DestroyGroupEntry(entry);
        return true;
    }
    DestroyGroupEntry(entry);
    return false;
}

int32_t CheckGroupNumLimit(int32_t osAccountId, int32_t groupType, const char *appId)
{
    /* Currently, only peer to peer group is supported. */
    (void)groupType;
    if (GetGroupNumByOwner(osAccountId, appId) >= HC_TRUST_GROUP_ENTRY_MAX_NUM) {
        LOGE("The number of groups created by the service exceeds the maximum!");
        return HC_ERR_BEYOND_LIMIT;
    }
    return HC_SUCCESS;
}

bool IsGroupOwner(int32_t osAccountId, const char *groupId, const char *appId)
{
    if ((groupId == NULL) || (appId == NULL)) {
        LOGE("The input groupId or appId is NULL!");
        return false;
    }
    TrustedGroupEntry *entry = GetGroupEntryById(osAccountId, groupId);
    if (entry == NULL) {
        LOGE("The group cannot be found!");
        return false;
    }
    if (HC_VECTOR_SIZE(&entry->managers) <= 0) {
        LOGE("The group does not have manager and owner!");
        DestroyGroupEntry(entry);
        return false;
    }
    HcString entryManager = HC_VECTOR_GET(&entry->managers, 0);
    const char *groupOwner = StringGet(&entryManager);
    if ((groupOwner != NULL) && strcmp(groupOwner, appId) == 0) {
        DestroyGroupEntry(entry);
        return true;
    }
    DestroyGroupEntry(entry);
    return false;
}

bool IsGroupExistByGroupId(int32_t osAccountId, const char *groupId)
{
    if (groupId == NULL) {
        LOGE("The input groupId is NULL!");
        return false;
    }
    TrustedGroupEntry *entry = GetGroupEntryById(osAccountId, groupId);
    if (entry == NULL) {
        LOGE("The group cannot be found!");
        return false;
    }
    DestroyGroupEntry(entry);
    return true;
}

bool IsGroupEditAllowed(int32_t osAccountId, const char *groupId, const char *appId)
{
    if ((groupId == NULL) || (appId == NULL)) {
        LOGE("The input groupId or appId is NULL!");
        return false;
    }
    TrustedGroupEntry *entry = GetGroupEntryById(osAccountId, groupId);
    if (entry == NULL) {
        LOGE("The group cannot be found!");
        return false;
    }
    if (IsGroupManager(appId, entry)) {
        DestroyGroupEntry(entry);
        return true;
    }
    DestroyGroupEntry(entry);
    return false;
}

int32_t GetGroupInfo(int32_t osAccountId, int groupType, const char *groupId, const char *groupName,
    const char *groupOwner, GroupEntryVec *returnGroupEntryVec)
{
    /* Fuzzy query interfaces, so some parameters can be NULL. */
    if (returnGroupEntryVec == NULL) {
        LOGE("The input returnGroupEntryVec is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    QueryGroupParams params = InitQueryGroupParams();
    params.groupId = groupId;
    params.groupName = groupName;
    params.ownerName = groupOwner;
    params.groupType = groupType;
    return QueryGroups(osAccountId, &params, returnGroupEntryVec);
}

int32_t GetJoinedGroups(int32_t osAccountId, int groupType, GroupEntryVec *returnGroupEntryVec)
{
    QueryGroupParams params = InitQueryGroupParams();
    params.groupType = groupType;
    return QueryGroups(osAccountId, &params, returnGroupEntryVec);
}

int32_t GetRelatedGroups(int32_t osAccountId, const char *peerDeviceId, bool isUdid, GroupEntryVec *returnGroupEntryVec)
{
    uint32_t index;
    TrustedDeviceEntry **entry = NULL;
    DeviceEntryVec deviceEntryVec = CreateDeviceEntryVec();
    QueryDeviceParams params = InitQueryDeviceParams();
    params.groupId = NULL;
    if (isUdid) {
        params.udid = peerDeviceId;
    } else {
        params.authId = peerDeviceId;
    }
    int32_t result = QueryDevices(osAccountId, &params, &deviceEntryVec);
    if (result != HC_SUCCESS) {
        LOGE("Failed to query trusted devices!");
        ClearDeviceEntryVec(&deviceEntryVec);
        return result;
    }
    FOR_EACH_HC_VECTOR(deviceEntryVec, index, entry) {
        if ((entry == NULL) || (*entry == NULL)) {
            continue;
        }
        TrustedGroupEntry *groupEntry = GetGroupEntryById(osAccountId, StringGet(&(*entry)->groupId));
        if (groupEntry == NULL) {
            LOGE("Failed to get group entry by id!");
            ClearDeviceEntryVec(&deviceEntryVec);
            return HC_ERR_GROUP_NOT_EXIST;
        }
        if (returnGroupEntryVec->pushBackT(returnGroupEntryVec, groupEntry) == NULL) {
            DestroyGroupEntry(groupEntry);
            ClearDeviceEntryVec(&deviceEntryVec);
            return HC_ERR_MEMORY_COPY;
        }
    }
    ClearDeviceEntryVec(&deviceEntryVec);
    return HC_SUCCESS;
}

int32_t GetTrustedDevInfoById(int32_t osAccountId, const char *deviceId, bool isUdid, const char *groupId,
    TrustedDeviceEntry *returnDeviceEntry)
{
    if ((deviceId == NULL) || (groupId == NULL) || (returnDeviceEntry == NULL)) {
        LOGE("The input parameters contain NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    LOGI("Start to get device information of a specified group!");
    TrustedDeviceEntry *deviceEntry = GetTrustedDeviceEntryById(osAccountId, deviceId, isUdid, groupId);
    if (deviceEntry == NULL) {
        LOGE("The trusted device is not found!");
        return HC_ERR_DEVICE_NOT_EXIST;
    }
    int32_t result = GenerateDeviceEntryFromEntry(deviceEntry, returnDeviceEntry) ? HC_SUCCESS : HC_ERR_MEMORY_COPY;
    DestroyDeviceEntry(deviceEntry);
    return result;
}

int32_t GetTrustedDevices(int32_t osAccountId, const char *groupId, DeviceEntryVec *returnDeviceEntryVec)
{
    QueryDeviceParams params = InitQueryDeviceParams();
    params.groupId = groupId;
    return QueryDevices(osAccountId, &params, returnDeviceEntryVec);
}

static int32_t AddGroupNameToReturn(const TrustedGroupEntry *groupInfo, CJson *json)
{
    const char *groupName = StringGet(&groupInfo->name);
    if (groupName == NULL) {
        LOGE("Failed to get groupName from groupInfo!");
        return HC_ERR_NULL_PTR;
    }
    if (AddStringToJson(json, FIELD_GROUP_NAME, groupName) != HC_SUCCESS) {
        LOGE("Failed to add groupName to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupIdToReturn(const TrustedGroupEntry *groupInfo, CJson *json)
{
    const char *groupId = StringGet(&groupInfo->id);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from groupInfo!");
        return HC_ERR_NULL_PTR;
    }
    if (AddStringToJson(json, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupOwnerToReturn(const TrustedGroupEntry *groupInfo, CJson *json)
{
    HcString entryManager = HC_VECTOR_GET(&groupInfo->managers, 0);
    const char *groupOwner = StringGet(&entryManager);
    if (groupOwner == NULL) {
        LOGE("Failed to get groupOwner from groupInfo!");
        return HC_ERR_NULL_PTR;
    }
    if (AddStringToJson(json, FIELD_GROUP_OWNER, groupOwner) != HC_SUCCESS) {
        LOGE("Failed to add groupOwner to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupTypeToReturn(const TrustedGroupEntry *groupInfo, CJson *json)
{
    int32_t groupType = groupInfo->type;
    if (AddIntToJson(json, FIELD_GROUP_TYPE, groupType) != HC_SUCCESS) {
        LOGE("Failed to add groupType to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupVisibilityToReturn(const TrustedGroupEntry *groupInfo, CJson *json)
{
    int groupVisibility = groupInfo->visibility;
    if (AddIntToJson(json, FIELD_GROUP_VISIBILITY, groupVisibility) != HC_SUCCESS) {
        LOGE("Failed to add groupType to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddAuthIdToReturn(const TrustedDeviceEntry *deviceInfo, CJson *json)
{
    const char *authId = StringGet(&deviceInfo->authId);
    if (authId == NULL) {
        LOGE("Failed to get authId from deviceInfo!");
        return HC_ERR_NULL_PTR;
    }
    if (AddStringToJson(json, FIELD_AUTH_ID, authId) != HC_SUCCESS) {
        LOGE("Failed to add authId to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddCredentialTypeToReturn(const TrustedDeviceEntry *deviceInfo, CJson *json)
{
    int credentialType = deviceInfo->credential;
    if (AddIntToJson(json, FIELD_CREDENTIAL_TYPE, credentialType) != HC_SUCCESS) {
        LOGE("Failed to add credentialType to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddUserTypeToReturn(const TrustedDeviceEntry *deviceInfo, CJson *json)
{
    int userType = deviceInfo->devType;
    if (AddIntToJson(json, FIELD_USER_TYPE, userType) != HC_SUCCESS) {
        LOGE("Failed to add userType to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

bool IsAccountRelatedGroup(int groupType)
{
    return ((groupType == IDENTICAL_ACCOUNT_GROUP) || (groupType == ACROSS_ACCOUNT_AUTHORIZE_GROUP));
}

int32_t GenerateReturnGroupInfo(const TrustedGroupEntry *groupInfo, CJson *returnJson)
{
    int32_t result;
    if (((result = AddGroupNameToReturn(groupInfo, returnJson)) != HC_SUCCESS) ||
        ((result = AddGroupIdToReturn(groupInfo, returnJson)) != HC_SUCCESS) ||
        ((result = AddGroupOwnerToReturn(groupInfo, returnJson)) != HC_SUCCESS) ||
        ((result = AddGroupTypeToReturn(groupInfo, returnJson)) != HC_SUCCESS) ||
        ((result = AddGroupVisibilityToReturn(groupInfo, returnJson)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

int32_t GenerateReturnDevInfo(const TrustedDeviceEntry *devInfo, CJson *returnJson)
{
    int32_t result;
    if (((result = AddAuthIdToReturn(devInfo, returnJson)) != HC_SUCCESS) ||
        ((result = AddCredentialTypeToReturn(devInfo, returnJson)) != HC_SUCCESS) ||
        ((result = AddUserTypeToReturn(devInfo, returnJson)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

int32_t GetHashMessage(const Uint8Buff *first, const Uint8Buff *second, uint8_t **hashMessage, uint32_t *messageSize)
{
    if ((first == NULL) || (second == NULL) || (hashMessage == NULL) || (messageSize == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_NULL_PTR;
    }
    const char *separator = "|";
    uint32_t firstSize = first->length;
    uint32_t secondSize = second->length;
    uint32_t separatorSize = HcStrlen(separator);
    uint32_t totalSize = firstSize + secondSize + separatorSize;
    *hashMessage = (uint8_t *)HcMalloc(totalSize, 0);
    if (*hashMessage == NULL) {
        LOGE("Failed to allocate hashMessage memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t result = HC_SUCCESS;
    do {
        if (memcpy_s((*hashMessage), totalSize, first->val, firstSize) != HC_SUCCESS) {
            LOGE("Failed to copy first!");
            result = HC_ERR_MEMORY_COPY;
            break;
        }
        if (memcpy_s((*hashMessage) + firstSize, totalSize - firstSize, separator, separatorSize) != HC_SUCCESS) {
            LOGE("Failed to copy separator!");
            result = HC_ERR_MEMORY_COPY;
            break;
        }
        if (memcpy_s((*hashMessage) + firstSize + separatorSize, secondSize, second->val, secondSize) != HC_SUCCESS) {
            LOGE("Failed to copy second!");
            result = HC_ERR_MEMORY_COPY;
        }
    } while (0);
    if (result != HC_SUCCESS) {
        HcFree(*hashMessage);
        *hashMessage = NULL;
        return result;
    }
    *messageSize = totalSize;
    return HC_SUCCESS;
}

int32_t GetCurDeviceNumByGroupId(int32_t osAccountId, const char *groupId)
{
    if (groupId == NULL) {
        LOGE("The input groupId is NULL!");
        return 0;
    }
    int count = 0;
    QueryDeviceParams queryDeviceParams = InitQueryDeviceParams();
    queryDeviceParams.groupId = groupId;
    DeviceEntryVec deviceEntryVec = CreateDeviceEntryVec();
    int32_t result = QueryDevices(osAccountId, &queryDeviceParams, &deviceEntryVec);
    if (result != HC_SUCCESS) {
        LOGE("Failed to query trusted devices!");
        ClearDeviceEntryVec(&deviceEntryVec);
        return result;
    }
    count = HC_VECTOR_SIZE(&deviceEntryVec);
    ClearDeviceEntryVec(&deviceEntryVec);
    return count;
}

int32_t CheckDeviceNumLimit(int32_t osAccountId, const char *groupId, const char *peerUdid)
{
    /*
     * If the peer device does not exist in the group and needs to be added,
     * check whether the number of trusted devices exceeds the upper limit.
     */

    if ((peerUdid != NULL) && (IsTrustedDeviceInGroup(osAccountId, groupId, peerUdid, true))) {
        return HC_SUCCESS;
    }
    if (GetCurDeviceNumByGroupId(osAccountId, groupId) >= HC_TRUST_DEV_ENTRY_MAX_NUM) {
        LOGE("The number of devices in the group has reached the upper limit!");
        return HC_ERR_BEYOND_LIMIT;
    }
    return HC_SUCCESS;
}

bool IsUserTypeValid(int userType)
{
    if ((userType == DEVICE_TYPE_ACCESSORY) ||
        (userType == DEVICE_TYPE_CONTROLLER) ||
        (userType == DEVICE_TYPE_PROXY)) {
        return true;
    }
    return false;
}

bool IsExpireTimeValid(int expireTime)
{
    if ((expireTime < -1) || (expireTime == 0) || (expireTime > MAX_EXPIRE_TIME)) {
        return false;
    }
    return true;
}

bool IsGroupVisibilityValid(int groupVisibility)
{
    /* Currently, only the public group and private group can be created. */
    if ((groupVisibility == GROUP_VISIBILITY_PUBLIC) ||
        ((groupVisibility == GROUP_VISIBILITY_PRIVATE))) {
        return true;
    }
    return false;
}

int32_t CheckUserTypeIfExist(const CJson *jsonParams)
{
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(jsonParams, FIELD_USER_TYPE, &userType);
    if (!IsUserTypeValid(userType)) {
        LOGE("The input userType is invalid! [UserType]: %d", userType);
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

int32_t CheckGroupVisibilityIfExist(const CJson *jsonParams)
{
    int32_t groupVisibility = GROUP_VISIBILITY_PUBLIC;
    (void)GetIntFromJson(jsonParams, FIELD_GROUP_VISIBILITY, &groupVisibility);
    if (!IsGroupVisibilityValid(groupVisibility)) {
        LOGE("The input groupVisibility is invalid! [GroupVisibility]: %d", groupVisibility);
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

int32_t CheckExpireTimeIfExist(const CJson *jsonParams)
{
    int32_t expireTime = DEFAULT_EXPIRE_TIME;
    (void)GetIntFromJson(jsonParams, FIELD_EXPIRE_TIME, &expireTime);
    if (!IsExpireTimeValid(expireTime)) {
        LOGE("Invalid group expire time! [ExpireTime]: %d", expireTime);
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

int32_t AddGroupNameToParams(const char *groupName, TrustedGroupEntry *groupParams)
{
    if (!StringSetPointer(&groupParams->name, groupName)) {
        LOGE("Failed to copy groupName!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

int32_t AddGroupIdToParams(const char *groupId, TrustedGroupEntry *groupParams)
{
    if (!StringSetPointer(&groupParams->id, groupId)) {
        LOGE("Failed to copy groupId!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

int32_t AddGroupOwnerToParams(const char *owner, TrustedGroupEntry *groupParams)
{
    HcString ownerName = CreateString();
    if (!StringSetPointer(&ownerName, owner)) {
        LOGE("Failed to copy groupOwner!");
        return HC_ERR_MEMORY_COPY;
    }
    if (groupParams->managers.pushBackT(&groupParams->managers, ownerName) == NULL) {
        LOGE("Failed to push owner to vec!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

int32_t AddGroupTypeToParams(int groupType, TrustedGroupEntry *groupParams)
{
    groupParams->type = groupType;
    return HC_SUCCESS;
}

int32_t AddGroupVisibilityOrDefault(const CJson *jsonParams, TrustedGroupEntry *groupParams)
{
    /* Currently, only the public group and private group can be created. */
    int32_t groupVisibility = GROUP_VISIBILITY_PUBLIC;
    (void)GetIntFromJson(jsonParams, FIELD_GROUP_VISIBILITY, &groupVisibility);
    groupParams->visibility = groupVisibility;
    return HC_SUCCESS;
}

int32_t AddExpireTimeOrDefault(const CJson *jsonParams, TrustedGroupEntry *groupParams)
{
    int32_t expireTime = DEFAULT_EXPIRE_TIME;
    (void)GetIntFromJson(jsonParams, FIELD_EXPIRE_TIME, &expireTime);
    groupParams->expireTime = expireTime;
    return HC_SUCCESS;
}

int32_t AddUserIdHashToGroupParams(const CJson *jsonParams, TrustedGroupEntry *groupParams)
{
    char *userIdHash = NULL;
    int32_t result = GetUserIdHashFromJson(jsonParams, &userIdHash);
    if (result != HC_SUCCESS) {
        return result;
    }
    if (!StringSetPointer(&groupParams->userIdHash, userIdHash)) {
        LOGE("Failed to copy userIdHash!");
        HcFree(userIdHash);
        return HC_ERR_MEMORY_COPY;
    }
    HcFree(userIdHash);
    return HC_SUCCESS;
}

int32_t AddUdidToParams(TrustedDeviceEntry *devParams)
{
    char udid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        return HC_ERR_DB;
    }
    if (!StringSetPointer(&devParams->udid, udid)) {
        LOGE("Failed to copy udid!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

int32_t AddAuthIdToParamsOrDefault(const CJson *jsonParams, TrustedDeviceEntry *devParams)
{
    const char *authId = GetStringFromJson(jsonParams, FIELD_DEVICE_ID);
    char udid[INPUT_UDID_LEN] = { 0 };
    if (authId == NULL) {
        LOGD("No authId is found. The default value is udid!");
        int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
        if (res != HC_SUCCESS) {
            LOGE("Failed to get local udid! res: %d", res);
            return HC_ERR_DB;
        }
        authId = udid;
    }
    if (!StringSetPointer(&devParams->authId, authId)) {
        LOGE("Failed to copy authId!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

int32_t AddUserTypeToParamsOrDefault(const CJson *jsonParams, TrustedDeviceEntry *devParams)
{
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(jsonParams, FIELD_USER_TYPE, &userType);
    devParams->devType = userType;
    return HC_SUCCESS;
}

int32_t AddServiceTypeToParams(const char *groupId, TrustedDeviceEntry *devParams)
{
    if (!StringSetPointer(&devParams->serviceType, groupId)) {
        LOGE("Failed to copy serviceType!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

int32_t AddGroupIdToDevParams(const char *groupId, TrustedDeviceEntry *devParams)
{
    if (!StringSetPointer(&devParams->groupId, groupId)) {
        LOGE("Failed to copy groupId!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

int32_t AddUserIdHashToDevParams(const CJson *jsonParams, TrustedDeviceEntry *devParams)
{
    char *userIdHash = NULL;
    int32_t result = GetUserIdHashFromJson(jsonParams, &userIdHash);
    if (result != HC_SUCCESS) {
        return result;
    }
    if (!StringSetPointer(&devParams->userIdHash, userIdHash)) {
        LOGE("Failed to copy userIdHash!");
        HcFree(userIdHash);
        return HC_ERR_MEMORY_COPY;
    }
    HcFree(userIdHash);
    return HC_SUCCESS;
}

int32_t AssertUserIdHashExist(const CJson *jsonParams)
{
    const char *userIdHash = GetStringFromJson(jsonParams, FIELD_USER_ID);
    if (userIdHash == NULL) {
        LOGE("Failed to get userIdHash from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    return HC_SUCCESS;
}

int32_t CheckGroupExist(int32_t osAccountId, const char *groupId)
{
    if (groupId == NULL) {
        LOGE("The input groupId is NULL!");
        return HC_ERR_NULL_PTR;
    }
    if (!IsGroupExistByGroupId(osAccountId, groupId)) {
        LOGE("The group does not exist!");
        return HC_ERR_GROUP_NOT_EXIST;
    }
    return HC_SUCCESS;
}

int32_t AddGroupToDatabaseByJson(int32_t osAccountId, int32_t (*generateGroupParams)(const CJson*, const char *,
    TrustedGroupEntry*), const CJson *jsonParams, const char *groupId)
{
    if ((generateGroupParams == NULL) || (jsonParams == NULL) || (groupId == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    TrustedGroupEntry *groupParams = CreateGroupEntry();
    if (groupParams == NULL) {
        LOGE("Failed to allocate groupParams memory!");
        return HC_ERR_ALLOC_MEMORY;
    }

    int32_t result = (*generateGroupParams)(jsonParams, groupId, groupParams);
    if (result != HC_SUCCESS) {
        DestroyGroupEntry(groupParams);
        return result;
    }

    result = AddGroup(osAccountId, groupParams);
    DestroyGroupEntry(groupParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to add the group to the database!");
    }
    return result;
}

int32_t AddDeviceToDatabaseByJson(int32_t osAccountId, int32_t (*generateDevParams)(const CJson*, const char*,
    TrustedDeviceEntry*), const CJson *jsonParams, const char *groupId)
{
    if ((generateDevParams == NULL) || (jsonParams == NULL) || (groupId == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    TrustedDeviceEntry *devParams = CreateDeviceEntry();
    if (devParams == NULL) {
        LOGE("Failed to allocate devParams memory!");
        return HC_ERR_ALLOC_MEMORY;
    }

    int32_t result = (*generateDevParams)(jsonParams, groupId, devParams);
    if (result != HC_SUCCESS) {
        DestroyDeviceEntry(devParams);
        return result;
    }

    result = AddTrustedDevice(osAccountId, devParams);
    DestroyDeviceEntry(devParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to add the trust device to the database!");
    }
    return result;
}

int32_t DelGroupFromDb(int32_t osAccountId, const char *groupId)
{
    if (groupId == NULL) {
        LOGE("The input groupId is NULL!");
        return HC_ERR_NULL_PTR;
    }
    int32_t result;
    QueryGroupParams queryGroupParams = InitQueryGroupParams();
    queryGroupParams.groupId = groupId;
    QueryDeviceParams queryDeviceParams = InitQueryDeviceParams();
    queryDeviceParams.groupId = groupId;
    if (((result = DelTrustedDevice(osAccountId, &queryDeviceParams)) != HC_SUCCESS) ||
        ((result = DelGroup(osAccountId, &queryGroupParams)) != HC_SUCCESS) ||
        ((result = SaveOsAccountDb(osAccountId)) != HC_SUCCESS)) {
        LOGE("Failed to delete group from database!");
        return result;
    }
    return HC_SUCCESS;
}

int32_t ConvertGroupIdToJsonStr(const char *groupId, char **returnJsonStr)
{
    if ((groupId == NULL) || (returnJsonStr == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    CJson *json = CreateJson();
    if (json == NULL) {
        LOGE("Failed to allocate json memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (AddStringToJson(json, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId to json!");
        FreeJson(json);
        return HC_ERR_JSON_FAIL;
    }
    *returnJsonStr = PackJsonToString(json);
    FreeJson(json);
    if (*returnJsonStr == NULL) {
        LOGE("Failed to convert json to string!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

int32_t GenerateBindSuccessData(const char *peerAuthId, const char *groupId, char **returnDataStr)
{
    if ((peerAuthId == NULL) || (groupId == NULL) || (returnDataStr == NULL)) {
        LOGE("The input params contains NULL value!");
        return HC_ERR_NULL_PTR;
    }
    char *tempGroupId = NULL;
    char *tempAuthId = NULL;
    ConvertToAnonymousStr(groupId, &tempGroupId);
    ConvertToAnonymousStr(peerAuthId, &tempAuthId);
    LOGI("Bind successfully! [GroupId]: %s, [AddId]: %s",
        tempGroupId == NULL ? "NULL" : tempGroupId,
        tempAuthId == NULL ? "NULL" : tempAuthId);
    HcFree(tempGroupId);
    HcFree(tempAuthId);
    CJson *jsonData = CreateJson();
    if (jsonData == NULL) {
        LOGE("Failed to allocate jsonData memory!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(jsonData, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId to jsonData!");
        FreeJson(jsonData);
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(jsonData, FIELD_ADD_ID, peerAuthId) != HC_SUCCESS) {
        LOGE("Failed to add addId to jsonData!");
        FreeJson(jsonData);
        return HC_ERR_JSON_FAIL;
    }
    char *jsonDataStr = PackJsonToString(jsonData);
    FreeJson(jsonData);
    if (jsonDataStr == NULL) {
        LOGE("An error occurred when converting JSON data to String data!");
        return HC_ERR_JSON_FAIL;
    }
    *returnDataStr = jsonDataStr;
    return HC_SUCCESS;
}

int32_t GenerateUnbindSuccessData(const char *peerAuthId, const char *groupId, char **returnDataStr)
{
    if ((peerAuthId == NULL) || (groupId == NULL) || (returnDataStr == NULL)) {
        LOGE("The input params contains NULL value!");
        return HC_ERR_NULL_PTR;
    }
    char *tempGroupId = NULL;
    char *tempAuthId = NULL;
    ConvertToAnonymousStr(groupId, &tempGroupId);
    ConvertToAnonymousStr(peerAuthId, &tempAuthId);
    LOGI("Unbind successfully! [GroupId]: %s, [DeleteId]: %s",
        tempGroupId == NULL ? "NULL" : tempGroupId,
        tempAuthId == NULL ? "NULL" : tempAuthId);
    HcFree(tempGroupId);
    HcFree(tempAuthId);
    CJson *jsonData = CreateJson();
    if (jsonData == NULL) {
        LOGE("Failed to allocate jsonData memory!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(jsonData, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId to jsonData!");
        FreeJson(jsonData);
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(jsonData, FIELD_DELETE_ID, peerAuthId) != HC_SUCCESS) {
        LOGE("Failed to add deleteId to jsonData!");
        FreeJson(jsonData);
        return HC_ERR_JSON_FAIL;
    }
    char *jsonDataStr = PackJsonToString(jsonData);
    FreeJson(jsonData);
    if (jsonDataStr == NULL) {
        LOGE("An error occurred when converting JSON data to String data!");
        return HC_ERR_JSON_FAIL;
    }
    *returnDataStr = jsonDataStr;
    return HC_SUCCESS;
}

int32_t ProcessKeyPair(int action, const CJson *jsonParams, const char *groupId)
{
    if ((jsonParams == NULL) || (groupId == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    /* Use the DeviceGroupManager package name. */
    const char *appId = GROUP_MANAGER_PACKAGE_NAME;
    const char *authId = GetStringFromJson(jsonParams, FIELD_DEVICE_ID);
    char udid[INPUT_UDID_LEN] = { 0 };
    if (authId == NULL) {
        LOGD("No authId is found. The default value is udid!");
        int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
        if (res != HC_SUCCESS) {
            LOGE("Failed to get local udid! res: %d", res);
            return HC_ERR_DB;
        }
        authId = udid;
    }
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(jsonParams, FIELD_USER_TYPE, &userType);
    Uint8Buff authIdBuff = { 0, 0 };
    authIdBuff.length = HcStrlen(authId);
    if (authIdBuff.length > MAX_DATA_BUFFER_SIZE) {
        LOGE("The length of authId is too long!");
        return HC_ERR_INVALID_PARAMS;
    }
    authIdBuff.val = (uint8_t *)HcMalloc(authIdBuff.length, 0);
    if (authIdBuff.val == NULL) {
        LOGE("Failed to allocate authIdBuff memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (memcpy_s(authIdBuff.val, authIdBuff.length, authId, authIdBuff.length) != EOK) {
        LOGE("Failed to copy authId!");
        HcFree(authIdBuff.val);
        return HC_ERR_MEMORY_COPY;
    }
    int32_t result;
    if (action == CREATE_KEY_PAIR) {
        result = RegisterLocalIdentity(appId, groupId, &authIdBuff, userType, DAS_MODULE);
    } else {
        result = UnregisterLocalIdentity(appId, groupId, &authIdBuff, userType, DAS_MODULE);
    }
    HcFree(authIdBuff.val);
    return result;
}

int32_t GetGroupTypeFromDb(int32_t osAccountId, const char *groupId, int32_t *returnGroupType)
{
    if ((groupId == NULL) || (returnGroupType == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    TrustedGroupEntry *groupEntry = CreateGroupEntry();
    if (groupEntry == NULL) {
        LOGE("Failed to allocate groupEntry memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (GetGroupInfoById(osAccountId, groupId, groupEntry) != HC_SUCCESS) {
        LOGE("Failed to get groupEntry from database!");
        DestroyGroupEntry(groupEntry);
        return HC_ERR_DB;
    }
    *returnGroupType = groupEntry->type;
    DestroyGroupEntry(groupEntry);
    return HC_SUCCESS;
}

int32_t GetUserIdHashFromJson(const CJson *jsonParams, char **userIdHash)
{
    if ((jsonParams == NULL) || (userIdHash == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    const char *oriUserIdHash = GetStringFromJson(jsonParams, FIELD_USER_ID);
    if (oriUserIdHash == NULL) {
        LOGE("Failed to get userIdHash from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    return ToUpperCase(oriUserIdHash, userIdHash);
}

int32_t GetGroupIdFromJson(const CJson *jsonParams, const char **groupId)
{
    if ((jsonParams == NULL) || (groupId == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
    if (*groupId == NULL) {
        LOGE("Failed to get groupId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    return HC_SUCCESS;
}

int32_t GetAppIdFromJson(const CJson *jsonParams, const char **appId)
{
    if ((jsonParams == NULL) || (appId == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (*appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    return HC_SUCCESS;
}

int32_t CheckPermForGroup(int32_t osAccountId, int actionType, const char *callerPkgName, const char *groupId)
{
    if (((actionType == GROUP_DISBAND) && (IsGroupOwner(osAccountId, groupId, callerPkgName))) ||
        ((actionType == MEMBER_INVITE) && (IsGroupEditAllowed(osAccountId, groupId, callerPkgName))) ||
        ((actionType == MEMBER_DELETE) && (IsGroupEditAllowed(osAccountId, groupId, callerPkgName)))) {
        return HC_SUCCESS;
    }
    LOGE("You do not have the right to execute the command!");
    return HC_ERR_ACCESS_DENIED;
}

int32_t GetHashResult(const uint8_t *info, uint32_t infoLen, char *hash, uint32_t hashLen)
{
    if ((info == NULL) || (hash == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HAL_ERR_NULL_PTR;
    }
    Uint8Buff infoHash = { NULL, SHA256_LEN };
    Uint8Buff message = { NULL, infoLen };
    infoHash.val = (uint8_t *)HcMalloc(SHA256_LEN, 0);
    if (infoHash.val == NULL) {
        LOGE("Failed to allocate infoHash.val memory!");
        return HAL_ERR_BAD_ALLOC;
    }
    message.val = (uint8_t *)HcMalloc(infoLen, 0);
    if (message.val == NULL) {
        LOGE("Failed to allocate message.val memory!");
        HcFree(infoHash.val);
        return HAL_ERR_BAD_ALLOC;
    }
    if (memcpy_s(message.val, infoLen, info, infoLen) != EOK) {
        LOGE("Failed to copy info!");
        HcFree(infoHash.val);
        HcFree(message.val);
        return HAL_ERR_MEMORY_COPY;
    }
    int32_t result = GetLoaderInstance()->sha256(&message, &infoHash);
    if (result == HAL_SUCCESS) {
        if (ByteToHexString(infoHash.val, infoHash.length, hash, hashLen) != HAL_SUCCESS) {
            LOGE("Failed to convert bytes to string!");
            result = HAL_ERR_BUILD_PARAM_SET_FAILED;
        }
    }
    HcFree(infoHash.val);
    HcFree(message.val);
    return result;
}