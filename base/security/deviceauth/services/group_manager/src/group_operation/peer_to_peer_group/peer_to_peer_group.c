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

#include "peer_to_peer_group.h"

#include "alg_defs.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "data_manager.h"
#include "dev_auth_module_manager.h"
#include "group_operation_common.h"
#include "hc_dev_info.h"
#include "hc_log.h"
#include "session_manager.h"
#include "string_util.h"

static bool IsSameNameGroupExist(int32_t osAccountId, const char *ownerName, const char *groupName)
{
    if ((ownerName == NULL) || (groupName == NULL)) {
        LOGE("The input ownerName or groupName is NULL!");
        return false;
    }
    QueryGroupParams queryParams = InitQueryGroupParams();
    queryParams.ownerName = ownerName;
    queryParams.groupName = groupName;
    GroupEntryVec groupEntryVec = CreateGroupEntryVec();
    int32_t result = QueryGroups(osAccountId, &queryParams, &groupEntryVec);
    if (result != HC_SUCCESS) {
        ClearGroupEntryVec(&groupEntryVec);
        return result;
    }
    if (HC_VECTOR_SIZE(&groupEntryVec) > 0) {
        ClearGroupEntryVec(&groupEntryVec);
        return true;
    }
    ClearGroupEntryVec(&groupEntryVec);
    return false;
}

static int32_t CheckGroupName(int32_t osAccountId, const char *appId, const CJson *jsonParams)
{
    const char *groupName = GetStringFromJson(jsonParams, FIELD_GROUP_NAME);
    if (groupName == NULL) {
        LOGE("Failed to get groupName from jsonParams!");
        return HC_ERR_JSON_GET;
    }

    if (IsSameNameGroupExist(osAccountId, appId, groupName)) {
        LOGE("A group with the same group name has been created! [AppId]: %s, [GroupName]: %s", appId, groupName);
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

static int32_t GenerateGroupId(const char *groupName, const char *appId, char **returnGroupId)
{
    /* peer to peer group: groupId = sha256(groupName | appId) */
    uint8_t *hashMessage = NULL;
    uint32_t messageSize = 0;
    Uint8Buff groupNameBuff = {(uint8_t *)groupName, HcStrlen(groupName)};
    Uint8Buff appIdBuff = {(uint8_t *)appId, HcStrlen(appId)};
    int32_t result = GetHashMessage(&groupNameBuff, &appIdBuff, &hashMessage, &messageSize);
    if (result != HC_SUCCESS) {
        return result;
    }
    int hashStrLen = SHA256_LEN * BYTE_TO_HEX_OPER_LENGTH + 1;
    *returnGroupId = (char *)HcMalloc(hashStrLen, 0);
    if (*returnGroupId == NULL) {
        LOGE("Failed to allocate returnGroupId memory!");
        HcFree(hashMessage);
        return HC_ERR_ALLOC_MEMORY;
    }
    result = GetHashResult(hashMessage, messageSize, *returnGroupId, hashStrLen);
    HcFree(hashMessage);
    if (result != HC_SUCCESS) {
        LOGE("Failed to get hash for groupId! [AppId]: %s, [GroupName]: %s", appId, groupName);
        HcFree(*returnGroupId);
        *returnGroupId = NULL;
        return HC_ERR_HASH_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t GeneratePeerToPeerGroupId(const CJson *jsonParams, char **returnGroupId)
{
    const char *groupName = GetStringFromJson(jsonParams, FIELD_GROUP_NAME);
    if (groupName == NULL) {
        LOGE("Failed to get groupName from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t result = GenerateGroupId(groupName, appId, returnGroupId);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate groupId! [GroupName]: %s, [AppId]: %s", groupName, appId);
        return result;
    }
    return HC_SUCCESS;
}

static int32_t CheckCreateParams(int32_t osAccountId, const CJson *jsonParams)
{
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t result;
    if (((result = CheckGroupName(osAccountId, appId, jsonParams)) != HC_SUCCESS) ||
        ((result = CheckUserTypeIfExist(jsonParams)) != HC_SUCCESS) ||
        ((result = CheckGroupVisibilityIfExist(jsonParams)) != HC_SUCCESS) ||
        ((result = CheckExpireTimeIfExist(jsonParams)) != HC_SUCCESS) ||
        ((result = CheckGroupNumLimit(osAccountId, PEER_TO_PEER_GROUP, appId)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t GenerateGroupParams(const CJson *jsonParams, const char *groupId, TrustedGroupEntry *groupParams)
{
    const char *groupName = GetStringFromJson(jsonParams, FIELD_GROUP_NAME);
    if (groupName == NULL) {
        LOGE("Failed to get groupName from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t result;
    if (((result = AddGroupTypeToParams(PEER_TO_PEER_GROUP, groupParams)) != HC_SUCCESS) ||
        ((result = AddGroupNameToParams(groupName, groupParams)) != HC_SUCCESS) ||
        ((result = AddGroupIdToParams(groupId, groupParams)) != HC_SUCCESS) ||
        ((result = AddGroupOwnerToParams(appId, groupParams)) != HC_SUCCESS) ||
        ((result = AddGroupVisibilityOrDefault(jsonParams, groupParams)) != HC_SUCCESS) ||
        ((result = AddExpireTimeOrDefault(jsonParams, groupParams)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t GenerateDevParams(const CJson *jsonParams, const char *groupId, TrustedDeviceEntry *devParams)
{
    int32_t result;
    if (((result = AddUdidToParams(devParams)) != HC_SUCCESS) ||
        ((result = AddAuthIdToParamsOrDefault(jsonParams, devParams))) ||
        ((result = AddUserTypeToParamsOrDefault(jsonParams, devParams)) != HC_SUCCESS) ||
        ((result = AddGroupIdToDevParams(groupId, devParams)) != HC_SUCCESS) ||
        ((result = AddServiceTypeToParams(groupId, devParams)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t CreateGroupInner(int32_t osAccountId, const CJson *jsonParams, char **returnGroupId)
{
    char *groupId = NULL;
    int32_t result;
    if (((result = CheckCreateParams(osAccountId, jsonParams)) != HC_SUCCESS) ||
        ((result = GeneratePeerToPeerGroupId(jsonParams, &groupId)) != HC_SUCCESS) ||
        ((result = ProcessKeyPair(CREATE_KEY_PAIR, jsonParams, groupId)) != HC_SUCCESS) ||
        ((result = AddGroupToDatabaseByJson(osAccountId, GenerateGroupParams, jsonParams, groupId)) != HC_SUCCESS) ||
        ((result = AddDeviceToDatabaseByJson(osAccountId, GenerateDevParams, jsonParams, groupId)) != HC_SUCCESS) ||
        ((result = SaveOsAccountDb(osAccountId)) != HC_SUCCESS)) {
        HcFree(groupId);
        return result;
    }
    *returnGroupId = groupId;
    return HC_SUCCESS;
}

static int32_t GetPeerDevUserTypeFromDb(int32_t osAccountId, const char *groupId, const char *peerAuthId)
{
    int peerUserType = DEVICE_TYPE_ACCESSORY;
    TrustedDeviceEntry *devAuthParams = CreateDeviceEntry();
    if (devAuthParams == NULL) {
        LOGE("Failed to allocate devEntry memory!");
        return peerUserType;
    }
    if (GetTrustedDevInfoById(osAccountId, peerAuthId, false, groupId, devAuthParams) != HC_SUCCESS) {
        LOGE("Failed to obtain the device information from the database!");
        DestroyDeviceEntry(devAuthParams);
        return peerUserType;
    }
    peerUserType = devAuthParams->devType;
    DestroyDeviceEntry(devAuthParams);
    return peerUserType;
}

static int32_t DelPeerDevAndKeyInfo(int32_t osAccountId, const char *groupId, const char *peerAuthId)
{
    int32_t peerUserType = GetPeerDevUserTypeFromDb(osAccountId, groupId, peerAuthId);
    QueryDeviceParams queryDeviceParams = InitQueryDeviceParams();
    queryDeviceParams.groupId = groupId;
    queryDeviceParams.authId = peerAuthId;
    int32_t result = DelTrustedDevice(osAccountId, &queryDeviceParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to delete peer device from database!");
        return result;
    }
    /* Use the DeviceGroupManager package name. */
    const char *appId = GROUP_MANAGER_PACKAGE_NAME;
    Uint8Buff peerAuthIdBuff = {
        .val = (uint8_t *)peerAuthId,
        .length = HcStrlen(peerAuthId)
    };
    /*
     * If the trusted device has been deleted from the database but the peer key fails to be deleted,
     * the forcible unbinding is still considered successful. Only logs need to be printed.
     */
    result = DeletePeerAuthInfo(appId, groupId, &peerAuthIdBuff, peerUserType, DAS_MODULE);
    if (result != HC_SUCCESS) {
        LOGD("delete peer key fail! res: %d", result);
    } else {
        LOGD("delete peer key success!");
    }
    return HC_SUCCESS;
}

static bool IsLocalDevice(const char *udid)
{
    char localUdid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)localUdid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        return HC_ERR_DB;
    }
    return (strcmp(localUdid, udid) == 0);
}

static int32_t DelAllPeerDevAndKeyInfo(int32_t osAccountId, const char *groupId)
{
    QueryDeviceParams queryParams = InitQueryDeviceParams();
    queryParams.groupId = groupId;
    DeviceEntryVec deviceEntryVec = CreateDeviceEntryVec();
    int32_t result = QueryDevices(osAccountId, &queryParams, &deviceEntryVec);
    if (result != HC_SUCCESS) {
        ClearDeviceEntryVec(&deviceEntryVec);
        return result;
    }
    uint32_t index;
    TrustedDeviceEntry **entryPtr = NULL;
    FOR_EACH_HC_VECTOR(deviceEntryVec, index, entryPtr) {
        if ((entryPtr == NULL) || (*entryPtr == NULL)) {
            continue;
        }
        TrustedDeviceEntry *entry = (TrustedDeviceEntry *)(*entryPtr);
        if (IsLocalDevice(StringGet(&entry->udid))) {
            continue;
        }
        result = DelPeerDevAndKeyInfo(osAccountId, groupId, StringGet(&entry->authId));
        if (result != HC_SUCCESS) {
            ClearDeviceEntryVec(&deviceEntryVec);
            return result;
        }
    }
    ClearDeviceEntryVec(&deviceEntryVec);
    return HC_SUCCESS;
}

static int32_t AddAuthIdAndUserTypeToParams(int32_t osAccountId, const char *groupId, CJson *jsonParams)
{
    TrustedDeviceEntry *deviceInfo = CreateDeviceEntry();
    if (deviceInfo == NULL) {
        LOGE("Failed to allocate deviceInfo memory!");
        return HC_ERR_ALLOC_MEMORY;
    }

    char localUdid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)localUdid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        DestroyDeviceEntry(deviceInfo);
        return HC_ERR_DB;
    }

    if (GetTrustedDevInfoById(osAccountId, localUdid, true, groupId, deviceInfo) != HC_SUCCESS) {
        LOGE("Failed to obtain the device information from the database!");
        DestroyDeviceEntry(deviceInfo);
        return HC_ERR_DB;
    }

    if (AddStringToJson(jsonParams, FIELD_DEVICE_ID, StringGet(&deviceInfo->authId)) != HC_SUCCESS) {
        LOGE("Failed to add authId to params!");
        DestroyDeviceEntry(deviceInfo);
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(jsonParams, FIELD_USER_TYPE, deviceInfo->devType) != HC_SUCCESS) {
        LOGE("Failed to add userType to params!");
        DestroyDeviceEntry(deviceInfo);
        return HC_ERR_JSON_FAIL;
    }
    DestroyDeviceEntry(deviceInfo);
    return HC_SUCCESS;
}

static int32_t DelGroupAndSelfKeyInfo(int32_t osAccountId, const char *groupId, CJson *jsonParams)
{
    int32_t result = AddAuthIdAndUserTypeToParams(osAccountId, groupId, jsonParams);
    if (result != HC_SUCCESS) {
        return result;
    }
    result = DelGroupFromDb(osAccountId, groupId);
    if (result != HC_SUCCESS) {
        return result;
    }
    /*
     * If the group has been disbanded from the database but the key pair fails to be deleted,
     * we still believe we succeeded in disbanding the group. Only logs need to be printed.
     */
    result = ProcessKeyPair(DELETE_KEY_PAIR, jsonParams, groupId);
    if (result != HC_SUCCESS) {
        LOGD("delete self key fail! res: %d", result);
    } else {
        LOGD("delete self key success!");
    }
    return HC_SUCCESS;
}

static int32_t HandleLocalUnbind(int64_t requestId, const CJson *jsonParams,
    const DeviceAuthCallback *callback)
{
    const char *peerAuthId = GetStringFromJson(jsonParams, FIELD_DELETE_ID);
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t osAccountId;
    if (GetIntFromJson(jsonParams, FIELD_OS_ACCOUNT_ID, &osAccountId) != HC_SUCCESS) {
        LOGE("Failed to get osAccountId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int result = DelPeerDevAndKeyInfo(osAccountId, groupId, peerAuthId);
    if (result != HC_SUCCESS) {
        return result;
    }
    result = SaveOsAccountDb(osAccountId);
    if (result != HC_SUCCESS) {
        LOGE("Failed to save osAccountDb!");
        return result;
    }
    char *returnDataStr = NULL;
    result = GenerateUnbindSuccessData(peerAuthId, groupId, &returnDataStr);
    if (result != HC_SUCCESS) {
        return result;
    }
    ProcessFinishCallback(requestId, MEMBER_DELETE, returnDataStr, callback);
    FreeJsonString(returnDataStr);
    return HC_SUCCESS;
}

static int32_t AssertPeerToPeerGroupType(int32_t groupType)
{
    if (groupType != PEER_TO_PEER_GROUP) {
        LOGE("Invalid group type! [GroupType]: %d", groupType);
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

static int32_t CheckInputGroupTypeValid(const CJson *jsonParams)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetIntFromJson(jsonParams, FIELD_GROUP_TYPE, &groupType) != HC_SUCCESS) {
        LOGE("Failed to get groupType from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    return AssertPeerToPeerGroupType(groupType);
}

static int32_t IsPeerDeviceNotSelf(const char *peerUdid)
{
    if (peerUdid == NULL) {
        LOGE("The input peerUdid is NULL!");
        return HC_ERR_NULL_PTR;
    }
    char udid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        return HC_ERR_DB;
    }
    if (strcmp(peerUdid, udid) == 0) {
        LOGE("You are not allowed to delete yourself!");
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

static int32_t CheckPeerDeviceStatus(int32_t osAccountId, const char *groupId, const CJson *jsonParams)
{
    const char *peerAuthId = GetStringFromJson(jsonParams, FIELD_DELETE_ID);
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerUdid from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    TrustedDeviceEntry *deviceInfo = CreateDeviceEntry();
    if (deviceInfo == NULL) {
        LOGE("Failed to allocate deviceInfo memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t result = GetTrustedDevInfoById(osAccountId, peerAuthId, false, groupId, deviceInfo);
    if (result != HC_SUCCESS) {
        LOGE("Failed to obtain the peer device information from the database!");
        DestroyDeviceEntry(deviceInfo);
        return result;
    }
    result = IsPeerDeviceNotSelf(StringGet(&deviceInfo->udid));
    DestroyDeviceEntry(deviceInfo);
    return result;
}

static int32_t CheckInvitePeer(const CJson *jsonParams)
{
    const char *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t osAccountId;
    if (GetIntFromJson(jsonParams, FIELD_OS_ACCOUNT_ID, &osAccountId) != HC_SUCCESS) {
        LOGE("Failed to get osAccountId from jsonParams!");
        return HC_ERR_JSON_GET;
    }

    int32_t groupType = PEER_TO_PEER_GROUP;
    int32_t result;
    if (((result = CheckGroupExist(osAccountId, groupId)) != HC_SUCCESS) ||
        ((result = GetGroupTypeFromDb(osAccountId, groupId, &groupType)) != HC_SUCCESS) ||
        ((result = AssertPeerToPeerGroupType(groupType)) != HC_SUCCESS) ||
        ((result = CheckPermForGroup(osAccountId, MEMBER_INVITE, appId, groupId)) != HC_SUCCESS) ||
        ((result = CheckDeviceNumLimit(osAccountId, groupId, NULL)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t CheckJoinPeer(const CJson *jsonParams)
{
    return CheckInputGroupTypeValid(jsonParams);
}

static int32_t CheckDeletePeer(const CJson *jsonParams)
{
    const char *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t osAccountId;
    if (GetIntFromJson(jsonParams, FIELD_OS_ACCOUNT_ID, &osAccountId) != HC_SUCCESS) {
        LOGE("Failed to get osAccountId from jsonParams!");
        return HC_ERR_JSON_GET;
    }

    int32_t groupType = PEER_TO_PEER_GROUP;
    int32_t result;
    if (((result = CheckGroupExist(osAccountId, groupId)) != HC_SUCCESS) ||
        ((result = GetGroupTypeFromDb(osAccountId, groupId, &groupType)) != HC_SUCCESS) ||
        ((result = AssertPeerToPeerGroupType(groupType)) != HC_SUCCESS) ||
        ((result = CheckPermForGroup(osAccountId, MEMBER_DELETE, appId, groupId)) != HC_SUCCESS) ||
        ((result = CheckPeerDeviceStatus(osAccountId, groupId, jsonParams)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t CheckClientStatus(int operationCode, const CJson *jsonParams)
{
    switch (operationCode) {
        case MEMBER_INVITE:
            return CheckInvitePeer(jsonParams);
        case MEMBER_JOIN:
            return CheckJoinPeer(jsonParams);
        case MEMBER_DELETE:
            return CheckDeletePeer(jsonParams);
        default:
            LOGE("Enter the exception case!");
            return HC_ERR_CASE;
    }
}

static CJson *GenerateGroupErrorMsg(int32_t errorCode, int64_t requestId, const CJson *jsonParams)
{
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return NULL;
    }
    CJson *errorData = CreateJson();
    if (errorData == NULL) {
        LOGE("Failed to allocate errorData memory!");
        return NULL;
    }
    if (AddIntToJson(errorData, FIELD_GROUP_ERROR_MSG, errorCode) != HC_SUCCESS) {
        LOGE("Failed to add errorCode to errorData!");
        FreeJson(errorData);
        return NULL;
    }
    if (AddStringToJson(errorData, FIELD_APP_ID, appId) != HC_SUCCESS) {
        LOGE("Failed to add appId to errorData!");
        FreeJson(errorData);
        return NULL;
    }
    if (AddInt64StringToJson(errorData, FIELD_REQUEST_ID, requestId) != HC_SUCCESS) {
        LOGE("Failed to add requestId to errorData!");
        FreeJson(errorData);
        return NULL;
    }
    return errorData;
}

static void InformPeerProcessError(int64_t requestId, const CJson *jsonParams, const DeviceAuthCallback *callback,
    int32_t errorCode)
{
    int64_t channelId = DEFAULT_CHANNEL_ID;
    ChannelType channelType = SOFT_BUS;
    if (GetByteFromJson(jsonParams, FIELD_CHANNEL_ID, (uint8_t *)&channelId, sizeof(int64_t)) != HC_SUCCESS) {
        channelType = SERVICE_CHANNEL;
    }
    CJson *errorData = GenerateGroupErrorMsg(errorCode, requestId, jsonParams);
    if (errorData == NULL) {
        return;
    }
    char *errorDataStr = PackJsonToString(errorData);
    FreeJson(errorData);
    if (errorDataStr == NULL) {
        LOGE("An error occurred when converting json to string!");
        return;
    }
    (void)HcSendMsg(channelType, requestId, channelId, callback, errorDataStr);
    FreeJsonString(errorDataStr);
}

static int32_t ShouldForceUnbind(bool isForceDelete, const CJson *jsonParams)
{
    bool isIgnoreChannel = false;
    (void)GetBoolFromJson(jsonParams, FIELD_IS_IGNORE_CHANNEL, &isIgnoreChannel);
    return (isForceDelete && isIgnoreChannel);
}

static int32_t CreateClientSession(int64_t requestId, int32_t operationCode, ChannelType channelType,
    CJson *jsonParams, const DeviceAuthCallback *callback)
{
    int32_t result = CreateSession(requestId, TYPE_CLIENT_BIND_SESSION, jsonParams, callback);
    if (result != HC_SUCCESS) {
        if (result != HC_ERR_CREATE_SESSION_FAIL) {
            ProcessErrorCallback(requestId, operationCode, result, NULL, callback);
        }
        return result;
    }
    /**
     * If service open the channel by itself,
     * a channel opened message needs to be triggered to unify the channel usage policy.
     */
    if (channelType == SERVICE_CHANNEL) {
        /* Release the memory in advance to reduce the memory usage. */
        DeleteAllItem(jsonParams);
        OnChannelOpened(requestId, DEFAULT_CHANNEL_ID);
    }
    return HC_SUCCESS;
}

static int32_t CreateServerSession(int64_t requestId, int32_t operationCode, CJson *jsonParams,
    const DeviceAuthCallback *callback)
{
    int32_t result = CreateSession(requestId, TYPE_SERVER_BIND_SESSION, jsonParams, callback);
    if (result != HC_SUCCESS) {
        if (result != HC_ERR_CREATE_SESSION_FAIL) {
            InformPeerProcessError(requestId, jsonParams, callback, result);
            ProcessErrorCallback(requestId, operationCode, result, NULL, callback);
        }
        return result;
    }
    return HC_SUCCESS;
}

static int32_t CreateGroup(int32_t osAccountId, CJson *jsonParams, char **returnJsonStr)
{
    LOGI("[Start]: Start to create a peer to peer group!");
    if ((jsonParams == NULL) || (returnJsonStr == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    char *groupId = NULL;
    int32_t result = CreateGroupInner(osAccountId, jsonParams, &groupId);
    if (result != HC_SUCCESS) {
        return result;
    }
    result = ConvertGroupIdToJsonStr(groupId, returnJsonStr);
    HcFree(groupId);
    if (result != HC_SUCCESS) {
        return result;
    }
    LOGI("[End]: Create a peer to peer group successfully!");
    return HC_SUCCESS;
}

static int32_t DeleteGroup(int32_t osAccountId, CJson *jsonParams, char **returnJsonStr)
{
    LOGI("[Start]: Start to delete a peer to peer group!");
    if ((jsonParams == NULL) || (returnJsonStr == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    int32_t result;
    const char *groupId = NULL;
    if (((result = GetGroupIdFromJson(jsonParams, &groupId)) != HC_SUCCESS) ||
        ((result = DelAllPeerDevAndKeyInfo(osAccountId, groupId)) != HC_SUCCESS) ||
        ((result = DelGroupAndSelfKeyInfo(osAccountId, groupId, jsonParams)) != HC_SUCCESS) ||
        ((result = ConvertGroupIdToJsonStr(groupId, returnJsonStr)) != HC_SUCCESS)) {
        return result;
    }
    LOGI("[End]: Delete a peer to peer group successfully!");
    return HC_SUCCESS;
}

static int32_t AddMemberToGroup(int32_t osAccountId, int64_t requestId, CJson *jsonParams,
    const DeviceAuthCallback *callback)
{
    LOGI("[Start]: Start to add member to a peer to peer group!");
    if ((jsonParams == NULL) || (callback == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    int32_t result;
    int32_t operationCode = MEMBER_INVITE;
    AddIntToJson(jsonParams, FIELD_OS_ACCOUNT_ID, osAccountId);
    (void)GetIntFromJson(jsonParams, FIELD_OPERATION_CODE, &operationCode);
    result = CheckClientStatus(operationCode, jsonParams);
    if (result != HC_SUCCESS) {
        ProcessErrorCallback(requestId, operationCode, result, NULL, callback);
        return result;
    }
    return CreateClientSession(requestId, operationCode, GetChannelType(callback, jsonParams), jsonParams, callback);
}

static int32_t DeleteMemberFromGroup(int32_t osAccountId, int64_t requestId, CJson *jsonParams,
    const DeviceAuthCallback *callback)
{
    LOGI("[Start]: Start to delete member from a peer to peer group!");
    if ((jsonParams == NULL) || (callback == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    AddIntToJson(jsonParams, FIELD_OS_ACCOUNT_ID, osAccountId);
    int32_t result = CheckClientStatus(MEMBER_DELETE, jsonParams);
    if (result != HC_SUCCESS) {
        ProcessErrorCallback(requestId, MEMBER_DELETE, result, NULL, callback);
        return result;
    }
    bool isForceDelete = false;
    (void)(GetBoolFromJson(jsonParams, FIELD_IS_FORCE_DELETE, &isForceDelete));
    if (ShouldForceUnbind(isForceDelete, jsonParams)) {
        result = HandleLocalUnbind(requestId, jsonParams, callback);
        if (result != HC_SUCCESS) {
            ProcessErrorCallback(requestId, MEMBER_DELETE, result, NULL, callback);
        }
        return result;
    }
    return CreateClientSession(requestId, MEMBER_DELETE, GetChannelType(callback, jsonParams), jsonParams, callback);
}

static int32_t ProcessData(int64_t requestId, CJson *jsonParams, const DeviceAuthCallback *callback)
{
    LOGI("[Start]: Start to process binding data!");
    if ((jsonParams == NULL) || (callback == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    int32_t operationCode = MEMBER_INVITE;
    (void)(GetIntFromJson(jsonParams, FIELD_GROUP_OP, &operationCode));
    return CreateServerSession(requestId, operationCode, jsonParams, callback);
}

static int32_t AddManagerWithCheck(int32_t osAccountId, const char *appId, const char *groupId,
    const char *managerAppId)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetGroupTypeFromDb(osAccountId, groupId, &groupType) != HC_SUCCESS) {
        LOGE("No group is found based on the query parameters!");
        return HC_ERR_GROUP_NOT_EXIST;
    }
    if (AssertPeerToPeerGroupType(groupType) != HC_SUCCESS) {
        return HC_ERR_NOT_SUPPORT;
    }
    if (!IsGroupOwner(osAccountId, groupId, appId)) {
        LOGE("You do not have the permission to add a manager to the group!");
        return HC_ERR_ACCESS_DENIED;
    }
    (void)managerAppId;
    return HC_SUCCESS;
}

static int32_t AddFriendWithCheck(int32_t osAccountId, const char *appId, const char *groupId, const char *friendAppId)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetGroupTypeFromDb(osAccountId, groupId, &groupType) != HC_SUCCESS) {
        LOGE("No group is found based on the query parameters!");
        return HC_ERR_GROUP_NOT_EXIST;
    }
    if (AssertPeerToPeerGroupType(groupType) != HC_SUCCESS) {
        return HC_ERR_NOT_SUPPORT;
    }
    if (!IsGroupEditAllowed(osAccountId, groupId, appId)) {
        LOGE("You do not have the permission to add a friend to the group!");
        return HC_ERR_ACCESS_DENIED;
    }
    (void)friendAppId;
    return HC_SUCCESS;
}

static int32_t DeleteManagerWithCheck(int32_t osAccountId, const char *appId, const char *groupId,
    const char *managerAppId)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetGroupTypeFromDb(osAccountId, groupId, &groupType) != HC_SUCCESS) {
        LOGE("No group is found based on the query parameters!");
        return HC_ERR_GROUP_NOT_EXIST;
    }
    if (AssertPeerToPeerGroupType(groupType) != HC_SUCCESS) {
        return HC_ERR_NOT_SUPPORT;
    }
    if (!IsGroupOwner(osAccountId, groupId, appId)) {
        LOGE("You do not have the permission to delete a manager from the group!");
        return HC_ERR_ACCESS_DENIED;
    }
    (void)managerAppId;
    return HC_SUCCESS;
}

static int32_t DeleteFriendWithCheck(int32_t osAccountId, const char *appId, const char *groupId,
    const char *friendAppId)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetGroupTypeFromDb(osAccountId, groupId, &groupType) != HC_SUCCESS) {
        LOGE("No group is found based on the query parameters!");
        return HC_ERR_GROUP_NOT_EXIST;
    }
    if (AssertPeerToPeerGroupType(groupType) != HC_SUCCESS) {
        return HC_ERR_NOT_SUPPORT;
    }
    if (!IsGroupEditAllowed(osAccountId, groupId, appId)) {
        LOGE("You do not have the permission to add a friend to the group!");
        return HC_ERR_ACCESS_DENIED;
    }
    (void)friendAppId;
    return HC_SUCCESS;
}

static int32_t GetManagersWithCheck(int32_t osAccountId, const char *appId, const char *groupId, char **returnManagers,
    uint32_t *returnSize)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetGroupTypeFromDb(osAccountId, groupId, &groupType) != HC_SUCCESS) {
        LOGE("No group is found based on the query parameters!");
        return HC_ERR_GROUP_NOT_EXIST;
    }
    if (AssertPeerToPeerGroupType(groupType) != HC_SUCCESS) {
        return HC_ERR_NOT_SUPPORT;
    }
    if (!IsGroupOwner(osAccountId, groupId, appId)) {
        LOGE("You do not have the permission to query the group managers information!");
        return HC_ERR_ACCESS_DENIED;
    }
    CJson *managers = CreateJsonArray();
    if (managers == NULL) {
        LOGE("Failed to allocate managers memory!");
        return HC_ERR_JSON_FAIL;
    }
    (void)returnManagers;
    (void)returnSize;
    return HC_SUCCESS;
}

static int32_t GetFriendsWithCheck(int32_t osAccountId, const char *appId, const char *groupId, char **returnFriends,
    uint32_t *returnSize)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetGroupTypeFromDb(osAccountId, groupId, &groupType) != HC_SUCCESS) {
        LOGE("No group is found based on the query parameters!");
        return HC_ERR_GROUP_NOT_EXIST;
    }
    if (AssertPeerToPeerGroupType(groupType) != HC_SUCCESS) {
        return HC_ERR_NOT_SUPPORT;
    }
    if (!IsGroupEditAllowed(osAccountId, groupId, appId)) {
        LOGE("You do not have the permission to query the group friends information!");
        return HC_ERR_ACCESS_DENIED;
    }
    (void)returnFriends;
    (void)returnSize;
    return HC_SUCCESS;
}

static int32_t AddGroupRoleWithCheck(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
    const char *roleAppId)
{
    if (isManager) {
        return AddManagerWithCheck(osAccountId, appId, groupId, roleAppId);
    }
    return AddFriendWithCheck(osAccountId, appId, groupId, roleAppId);
}

static int32_t DeleteGroupRoleWithCheck(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
    const char *roleAppId)
{
    if (isManager) {
        return DeleteManagerWithCheck(osAccountId, appId, groupId, roleAppId);
    }
    return DeleteFriendWithCheck(osAccountId, appId, groupId, roleAppId);
}

static int32_t GetGroupRolesWithCheck(int32_t osAccountId, bool isManager, const char *appId, const char *groupId,
    char **returnJsonStr, uint32_t *returnSize)
{
    if (isManager) {
        return GetManagersWithCheck(osAccountId, appId, groupId, returnJsonStr, returnSize);
    }
    return GetFriendsWithCheck(osAccountId, appId, groupId, returnJsonStr, returnSize);
}

static PeerToPeerGroup g_peerToPeerGroup = {
    .base.type = PEER_TO_PEER_GROUP,
    .base.createGroup = CreateGroup,
    .base.deleteGroup = DeleteGroup,
    .addMember = AddMemberToGroup,
    .deleteMember = DeleteMemberFromGroup,
    .processData = ProcessData,
    .addGroupRole = AddGroupRoleWithCheck,
    .deleteGroupRole = DeleteGroupRoleWithCheck,
    .getGroupRoles = GetGroupRolesWithCheck,
};

BaseGroup *GetPeerToPeerGroupInstance(void)
{
    return (BaseGroup *)&g_peerToPeerGroup;
}

bool IsPeerToPeerGroupSupported(void)
{
    return true;
}
