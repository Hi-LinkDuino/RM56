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

#include "bind_session_common.h"
#include "bind_session_common_util.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "data_manager.h"
#include "dev_auth_module_manager.h"
#include "group_operation_common.h"
#include "hc_dev_info.h"
#include "hc_log.h"
#include "hc_types.h"
#include "session_common.h"

static int32_t AddGroupInfoToSendData(const BindSession *session, CJson *data)
{
    const char *groupId = GetStringFromJson(session->params, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from params!");
        return HC_ERR_JSON_GET;
    }
    const char *groupName = GetStringFromJson(session->params, FIELD_GROUP_NAME);
    if (groupName == NULL) {
        LOGE("Failed to get groupName from params!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(data, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId to data!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(data, FIELD_GROUP_NAME, groupName) != HC_SUCCESS) {
        LOGE("Failed to add groupName to data!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(data, FIELD_GROUP_OP, session->opCode) != HC_SUCCESS) {
        LOGE("Failed to add groupOp to data!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(data, FIELD_GROUP_TYPE, PEER_TO_PEER_GROUP) != HC_SUCCESS) {
        LOGE("Failed to add groupType to data!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddDevInfoToSendData(const BindSession *session, CJson *data)
{
    const char *authId = GetStringFromJson(session->params, FIELD_AUTH_ID);
    if (authId == NULL) {
        LOGE("Failed to get authId from params!");
        return HC_ERR_JSON_GET;
    }
    const char *udid = GetStringFromJson(session->params, FIELD_CONN_DEVICE_ID);
    if (udid == NULL) {
        LOGE("Failed to get udid from params!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(data, FIELD_PEER_DEVICE_ID, authId) != HC_SUCCESS) {
        LOGE("Failed to add peerDeviceId to data!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(data, FIELD_CONN_DEVICE_ID, udid) != HC_SUCCESS) {
        LOGE("Failed to add connDeviceId to data!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddRequestInfoToSendData(const BindSession *session, CJson *data)
{
    if (AddStringToJson(data, FIELD_APP_ID, session->appId) != HC_SUCCESS) {
        LOGE("Failed to add appId to data!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddInt64StringToJson(data, FIELD_REQUEST_ID, session->reqId) != HC_SUCCESS) {
        LOGE("Failed to add requestId to data!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(data, FIELD_OWNER_NAME, "") != HC_SUCCESS) {
        LOGE("Failed to add ownerName to data!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t GenerateCompatibleInfo(CJson *groupInfo)
{
    char udid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        return HC_ERR_DB;
    }
    if (AddStringToJson(groupInfo, FIELD_DEVICE_ID, udid) != HC_SUCCESS) {
        LOGE("Failed to add deviceId to groupInfo!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddBoolToJson(groupInfo, FIELD_IS_UUID, true) != HC_SUCCESS) {
        LOGE("Failed to add uuIdAsDeviceId to groupInfo!");
        return HC_ERR_JSON_FAIL;
    }
    /* To be compatible with packets of earlier versions. */
    CJson *managers = CreateJsonArray();
    if (managers == NULL) {
        LOGE("Failed to allocate managers memory!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddObjToJson(groupInfo, FIELD_GROUP_MANAGERS, managers) != HC_SUCCESS) {
        LOGE("Failed to add groupManagers to groupInfo!");
        FreeJson(managers);
        return HC_ERR_JSON_FAIL;
    }
    FreeJson(managers);
    /* Currently, only the public group can be created. */
    if (AddIntToJson(groupInfo, FIELD_GROUP_VISIBILITY, GROUP_VISIBILITY_PUBLIC) != HC_SUCCESS) {
        LOGE("Failed to add groupVisibility to groupInfo!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddCompatibleInfoToSendData(bool isNeedCompatibleInfo, CJson *data)
{
    if (!isNeedCompatibleInfo) {
        return HC_SUCCESS;
    }
    CJson *groupInfo = CreateJson();
    if (groupInfo == NULL) {
        LOGE("Failed to allocate groupInfo memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (GenerateCompatibleInfo(groupInfo) != HC_SUCCESS) {
        FreeJson(groupInfo);
        return HC_ERR_JSON_FAIL;
    }
    if (AddObjToJson(data, FIELD_GROUP_INFO, groupInfo) != HC_SUCCESS) {
        LOGE("Failed to add groupInfo to sendData!");
        FreeJson(groupInfo);
        return HC_ERR_JSON_FAIL;
    }
    FreeJson(groupInfo);
    return HC_SUCCESS;
}

static int32_t AddGroupAndDevInfoToParams(const BindSession *session, CJson *moduleParams)
{
    const char *groupId = GetStringFromJson(session->params, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from params!");
        return HC_ERR_JSON_GET;
    }
    const char *authId = GetStringFromJson(session->params, FIELD_AUTH_ID);
    if (authId == NULL) {
        LOGE("Failed to get authId from params!");
        return HC_ERR_JSON_GET;
    }
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    if (GetIntFromJson(session->params, FIELD_USER_TYPE, &userType) != HC_SUCCESS) {
        LOGE("Failed to get userType from params!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(moduleParams, FIELD_SERVICE_TYPE, groupId) != HC_SUCCESS) {
        LOGE("Failed to add serviceType to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(moduleParams, FIELD_SELF_AUTH_ID, authId) != HC_SUCCESS) {
        LOGE("Failed to add serviceType to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(moduleParams, FIELD_SELF_TYPE, userType) != HC_SUCCESS) {
        LOGE("Failed to add userType to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddRequestInfoToParams(bool isClient, const BindSession *session, CJson *moduleParams)
{
    if (AddByteToJson(moduleParams, FIELD_REQUEST_ID,
        (const uint8_t *)&session->reqId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to add requestId to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(moduleParams, FIELD_KEY_LENGTH, DEFAULT_RETURN_KEY_LENGTH) != HC_SUCCESS) {
        LOGE("Failed to add sessionKeyLength to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddBoolToJson(moduleParams, FIELD_IS_CLIENT, isClient) != HC_SUCCESS) {
        LOGE("Failed to add isClient to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    /* Use the GroupManager package name. */
    if (AddStringToJson(moduleParams, FIELD_PKG_NAME, GROUP_MANAGER_PACKAGE_NAME) != HC_SUCCESS) {
        LOGE("Failed to add pkgName to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddPinCodeToParamsIfNeed(BindSession *session, CJson *moduleParams)
{
    if (session->opCode == MEMBER_DELETE) {
        return HC_SUCCESS;
    }
    const char *pinCode = GetStringFromJson(session->params, FIELD_PIN_CODE);
    if (pinCode == NULL) {
        LOGE("Failed to get pinCode from params!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(moduleParams, FIELD_PIN_CODE, pinCode) != HC_SUCCESS) {
        LOGE("Failed to add pinCode to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    /* Release the memory in advance to reduce the memory usage. */
    (void)DeleteItemFromJson(session->params, FIELD_PIN_CODE);
    return HC_SUCCESS;
}

static int32_t AddPeerAuthIdToParams(BindSession *session, CJson *moduleParams)
{
    const char *peerAuthId = GetStringFromJson(session->params, FIELD_PEER_AUTH_ID);
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from params!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(moduleParams, FIELD_PEER_AUTH_ID, peerAuthId) != HC_SUCCESS) {
        LOGE("Failed to add peerAuthId to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddPeerUserTypeToParams(BindSession *session, CJson *moduleParams)
{
    int32_t peerUserType = DEVICE_TYPE_ACCESSORY;
    if (GetIntFromJson(session->params, FIELD_PEER_USER_TYPE, &peerUserType) != HC_SUCCESS) {
        LOGE("Failed to get peerUserType from params!");
        return HC_ERR_JSON_GET;
    }
    if (AddIntToJson(moduleParams, FIELD_PEER_USER_TYPE, peerUserType) != HC_SUCCESS) {
        LOGE("Failed to add peerUserType to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddPeerAuthIdAndUserTypeToParamsIfNeed(BindSession *session, CJson *moduleParams)
{
    if (session->opCode != MEMBER_DELETE) {
        return HC_SUCCESS;
    }
    int32_t result = AddPeerAuthIdToParams(session, moduleParams);
    if (result != HC_SUCCESS) {
        return result;
    }
    return AddPeerUserTypeToParams(session, moduleParams);
}

static int32_t TryAddPeerUserTypeToParams(const CJson *jsonParams, BindSession *session)
{
    int32_t peerUserType = DEVICE_TYPE_ACCESSORY;
    if (GetIntFromJson(jsonParams, FIELD_PEER_USER_TYPE, &peerUserType) == HC_SUCCESS) {
        if (AddIntToJson(session->params, FIELD_PEER_USER_TYPE, peerUserType) != HC_SUCCESS) {
            LOGE("Failed to add peerUserType to params!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddGroupInfoToParams(const TrustedGroupEntry *entry, CJson *params)
{
    if (AddStringToJson(params, FIELD_GROUP_ID, StringGet(&entry->id)) != HC_SUCCESS) {
        LOGE("Failed to add groupId to json!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(params, FIELD_GROUP_TYPE, entry->type) != HC_SUCCESS) {
        LOGE("Failed to add groupType to json!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(params, FIELD_GROUP_NAME, StringGet(&entry->name)) != HC_SUCCESS) {
        LOGE("Failed to add groupName to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddDevInfoToParams(const TrustedDeviceEntry *devAuthParams, CJson *params)
{
    if (AddStringToJson(params, FIELD_AUTH_ID, StringGet(&devAuthParams->authId)) != HC_SUCCESS) {
        LOGE("Failed to add authId to params!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(params, FIELD_CONN_DEVICE_ID, StringGet(&devAuthParams->udid)) != HC_SUCCESS) {
        LOGE("Failed to add udid to params!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(params, FIELD_USER_TYPE, devAuthParams->devType) != HC_SUCCESS) {
        LOGE("Failed to add userType to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupInfoByDatabase(int32_t osAccountId, const char *groupId, CJson *params)
{
    TrustedGroupEntry *entry = CreateGroupEntry();
    if (entry == NULL) {
        LOGE("Failed to allocate groupEntry memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (GetGroupInfoById(osAccountId, groupId, entry) != HC_SUCCESS) {
        LOGE("Failed to obtain the group information from the database!");
        DestroyGroupEntry(entry);
        return HC_ERR_DB;
    }
    if (AddGroupInfoToParams(entry, params) != HC_SUCCESS) {
        DestroyGroupEntry(entry);
        return HC_ERR_JSON_FAIL;
    }
    DestroyGroupEntry(entry);
    return HC_SUCCESS;
}

static int32_t AddDevInfoByDatabase(int32_t osAccountId, const char *groupId, CJson *params)
{
    char udid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        return HC_ERR_DB;
    }
    TrustedDeviceEntry *devAuthParams = CreateDeviceEntry();
    if (devAuthParams == NULL) {
        LOGE("Failed to allocate devEntry memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (GetTrustedDevInfoById(osAccountId, udid, true, groupId, devAuthParams) != HC_SUCCESS) {
        LOGE("Failed to obtain the device information from the database!");
        DestroyDeviceEntry(devAuthParams);
        return HC_ERR_DB;
    }
    if (AddDevInfoToParams(devAuthParams, params) != HC_SUCCESS) {
        DestroyDeviceEntry(devAuthParams);
        return HC_ERR_JSON_FAIL;
    }
    DestroyDeviceEntry(devAuthParams);
    return HC_SUCCESS;
}

static int32_t AddGroupId(const char *groupId, CJson *params)
{
    if (AddStringToJson(params, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupName(const CJson *jsonParams, CJson *params)
{
    const char *groupName = GetStringFromJson(jsonParams, FIELD_GROUP_NAME);
    if (groupName == NULL) {
        LOGE("Failed to get groupName from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(params, FIELD_GROUP_NAME, groupName) != HC_SUCCESS) {
        LOGE("Failed to add groupName to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupOwnerIfExist(const CJson *jsonParams, CJson *params)
{
    const char *groupOwner = GetStringFromJson(jsonParams, FIELD_GROUP_OWNER);
    if (groupOwner != NULL) {
        if (AddStringToJson(params, FIELD_GROUP_OWNER, groupOwner) != HC_SUCCESS) {
            LOGE("Failed to add groupOwner to params!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddGroupTypeIfValid(const CJson *jsonParams, CJson *params)
{
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetIntFromJson(jsonParams, FIELD_GROUP_TYPE, &groupType) != HC_SUCCESS) {
        LOGE("Failed to get groupType from in!");
        return HC_ERR_JSON_GET;
    }
    if (groupType != PEER_TO_PEER_GROUP) {
        LOGE("The input groupType is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
    if (AddIntToJson(params, FIELD_GROUP_TYPE, groupType) != HC_SUCCESS) {
        LOGE("Failed to add groupType to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupVisibilityIfValidOrDefault(const CJson *jsonParams, CJson *params)
{
    int32_t groupVisibility = GROUP_VISIBILITY_PUBLIC;
    (void)GetIntFromJson(jsonParams, FIELD_GROUP_VISIBILITY, &groupVisibility);
    if (!IsGroupVisibilityValid(groupVisibility)) {
        LOGE("The input groupVisibility is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
    if (AddIntToJson(params, FIELD_GROUP_VISIBILITY, groupVisibility) != HC_SUCCESS) {
        LOGE("Failed to add groupVisibility to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddExpireTimeIfValidOrDefault(const CJson *jsonParams, CJson *params)
{
    int32_t expireTime = DEFAULT_EXPIRE_TIME;
    (void)GetIntFromJson(jsonParams, FIELD_EXPIRE_TIME, &expireTime);
    if (!IsExpireTimeValid(expireTime)) {
        LOGE("The input expireTime is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
    if (AddIntToJson(params, FIELD_EXPIRE_TIME, expireTime) != HC_SUCCESS) {
        LOGE("Failed to add expireTime to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t CheckAuthIdAndUserTypeValid(int32_t osAccountId, int userType, const char *groupId, const char *authId)
{
    if (!IsGroupExistByGroupId(osAccountId, groupId)) {
        return HC_SUCCESS;
    }
    char udid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        return HC_ERR_DB;
    }
    TrustedDeviceEntry *deviceInfo = CreateDeviceEntry();
    if (deviceInfo == NULL) {
        LOGE("Failed to allocate deviceInfo memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t result = GetTrustedDevInfoById(osAccountId, udid, true, groupId, deviceInfo);
    if (result != HC_SUCCESS) {
        LOGE("Failed to obtain the local device information from the database!");
        DestroyDeviceEntry(deviceInfo);
        return result;
    }
    const char *oriAuthId = StringGet(&deviceInfo->authId);
    if ((deviceInfo->devType != userType) || ((oriAuthId != NULL) && (strcmp(oriAuthId, authId) != 0))) {
        LOGE("Once a group is created, the service cannot change the local authId and userType used in the group!");
        DestroyDeviceEntry(deviceInfo);
        return HC_ERR_INVALID_PARAMS;
    }
    DestroyDeviceEntry(deviceInfo);
    return HC_SUCCESS;
}

static int32_t AddAuthIdAndUserTypeIfValidOrDefault(int32_t osAccountId, const char *groupId, const CJson *jsonParams,
    CJson *params)
{
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(jsonParams, FIELD_USER_TYPE, &userType);
    if (!IsUserTypeValid(userType)) {
        LOGE("The input userType is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
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
    int32_t result = CheckAuthIdAndUserTypeValid(osAccountId, userType, groupId, authId);
    if (result != HC_SUCCESS) {
        return result;
    }
    if (AddIntToJson(params, FIELD_USER_TYPE, userType) != HC_SUCCESS) {
        LOGE("Failed to add userType to params!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(params, FIELD_AUTH_ID, authId) != HC_SUCCESS) {
        LOGE("Failed to add authId to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddUdid(CJson *params)
{
    char udid[INPUT_UDID_LEN] = { 0 };
    int32_t res = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", res);
        return HC_ERR_DB;
    }
    if (AddStringToJson(params, FIELD_CONN_DEVICE_ID, udid) != HC_SUCCESS) {
        LOGE("Failed to add udid to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddUserTypeIfValidOrDefault(const CJson *jsonParams, CJson *params)
{
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(jsonParams, FIELD_USER_TYPE, &userType);
    if (!IsUserTypeValid(userType)) {
        LOGE("The input userType is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
    if (AddIntToJson(params, FIELD_USER_TYPE, userType) != HC_SUCCESS) {
        LOGE("Failed to add userType to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupInfoToSessionParams(const char *groupId, const CJson *jsonParams, CJson *params)
{
    int32_t result;
    if (((result = AddGroupId(groupId, params)) != HC_SUCCESS) ||
        ((result = AddGroupName(jsonParams, params)) != HC_SUCCESS) ||
        ((result = AddGroupOwnerIfExist(jsonParams, params)) != HC_SUCCESS) ||
        ((result = AddGroupTypeIfValid(jsonParams, params)) != HC_SUCCESS) ||
        ((result = AddGroupVisibilityIfValidOrDefault(jsonParams, params)) != HC_SUCCESS) ||
        ((result = AddExpireTimeIfValidOrDefault(jsonParams, params)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t AddDevInfoToSessionParams(int32_t osAccountId, const char *groupId, const CJson *jsonParams,
    CJson *params)
{
    int32_t result;
    if (((result = AddAuthIdAndUserTypeIfValidOrDefault(osAccountId, groupId, jsonParams, params)) != HC_SUCCESS) ||
        ((result = AddUdid(params)) != HC_SUCCESS) ||
        ((result = AddUserTypeIfValidOrDefault(jsonParams, params)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t GenerateParamsByInput(int32_t osAccountId, const char *groupId, const CJson *jsonParams, CJson *params)
{
    int32_t result = AddGroupInfoToSessionParams(groupId, jsonParams, params);
    if (result != HC_SUCCESS) {
        return result;
    }
    return AddDevInfoToSessionParams(osAccountId, groupId, jsonParams, params);
}

static int32_t GenerateParamsByDatabase(int32_t osAccountId, const char *groupId, CJson *params)
{
    int32_t result = AddGroupInfoByDatabase(osAccountId, groupId, params);
    if (result != HC_SUCCESS) {
        return result;
    }
    return AddDevInfoByDatabase(osAccountId, groupId, params);
}

static int32_t AddIsForceDeleteIfNeed(int isClient, const CJson *jsonParams, BindSession *session)
{
    if ((isClient == CLIENT) && (session->opCode == MEMBER_DELETE)) {
        bool isForceDelete = false;
        (void)GetBoolFromJson(jsonParams, FIELD_IS_FORCE_DELETE, &isForceDelete);
        if (AddBoolToJson(session->params, FIELD_IS_FORCE_DELETE, isForceDelete) != HC_SUCCESS) {
            LOGE("Failed to add isForceDelete to params!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddChannelIdIfNeed(int isClient, const CJson *jsonParams, BindSession *session)
{
    if ((isClient == SERVER) && (session->channelType == SOFT_BUS)) {
        int64_t channelId = DEFAULT_CHANNEL_ID;
        if (GetByteFromJson(jsonParams, FIELD_CHANNEL_ID, (uint8_t *)&channelId, sizeof(int64_t)) != HC_SUCCESS) {
            LOGE("Failed to get channelId from jsonParams!");
            return HC_ERR_JSON_GET;
        }
        session->channelId = channelId;
    }
    return HC_SUCCESS;
}

static int32_t AddPinCodeIfNeed(const CJson *jsonParams, BindSession *session)
{
    if (session->opCode != MEMBER_DELETE) {
        const char *pinCode = GetStringFromJson(jsonParams, FIELD_PIN_CODE);
        if (pinCode == NULL) {
            LOGE("Failed to get pinCode from jsonParams!");
            return HC_ERR_JSON_GET;
        }
        if (AddStringToJson(session->params, FIELD_PIN_CODE, pinCode) != HC_SUCCESS) {
            LOGE("Failed to add pinCode to params!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddPeerAuthIdAndUdidIfExist(const CJson *jsonParams, BindSession *session)
{
    const char *peerAuthId = GetStringFromJson(jsonParams, FIELD_PEER_DEVICE_ID);
    const char *peerUdid = GetStringFromJson(jsonParams, FIELD_PEER_UDID);
    if (peerAuthId != NULL) {
        if (AddStringToJson(session->params, FIELD_PEER_AUTH_ID, peerAuthId) != HC_SUCCESS) {
            LOGE("Failed to add peerAuthId to params!");
            return HC_ERR_JSON_FAIL;
        }
    }
    if (peerUdid != NULL) {
        if (AddStringToJson(session->params, FIELD_PEER_UDID, peerUdid) != HC_SUCCESS) {
            LOGE("Failed to add peerUdid to params!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddPeerAuthIdIfDelete(bool isClient, const CJson *jsonParams, BindSession *session)
{
    const char *peerAuthId = NULL;
    if (isClient) {
        peerAuthId = GetStringFromJson(jsonParams, FIELD_DELETE_ID);
    } else {
        peerAuthId = GetStringFromJson(jsonParams, FIELD_PEER_DEVICE_ID);
    }
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(session->params, FIELD_PEER_AUTH_ID, peerAuthId) != HC_SUCCESS) {
        LOGE("Failed to add peerAuthId to params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddPeerUserTypeIfDelete(BindSession *session)
{
    const char *peerAuthId = GetStringFromJson(session->params, FIELD_PEER_AUTH_ID);
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from params!");
        return HC_ERR_JSON_GET;
    }
    const char *groupId = GetStringFromJson(session->params, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from params!");
        return HC_ERR_JSON_GET;
    }
    TrustedDeviceEntry *devAuthParams = CreateDeviceEntry();
    if (devAuthParams == NULL) {
        LOGE("Failed to allocate devEntry memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (GetTrustedDevInfoById(session->osAccountId, peerAuthId, false, groupId, devAuthParams) != HC_SUCCESS) {
        LOGE("Failed to obtain the device information from the database!");
        DestroyDeviceEntry(devAuthParams);
        return HC_ERR_DB;
    }
    if (AddIntToJson(session->params, FIELD_PEER_USER_TYPE, devAuthParams->devType) != HC_SUCCESS) {
        DestroyDeviceEntry(devAuthParams);
        return HC_ERR_JSON_FAIL;
    }
    DestroyDeviceEntry(devAuthParams);
    return HC_SUCCESS;
}

static int32_t AddPeerDevInfoIfNeed(bool isClient, const CJson *jsonParams, BindSession *session)
{
    if (session->opCode == MEMBER_DELETE) {
        int32_t result = AddPeerAuthIdIfDelete(isClient, jsonParams, session);
        if (result != HC_SUCCESS) {
            return result;
        }
        return AddPeerUserTypeIfDelete(session);
    }
    return AddPeerAuthIdAndUdidIfExist(jsonParams, session);
}

static int32_t AddGroupAndDevInfo(int32_t osAccountId, int isClient, const CJson *jsonParams, BindSession *session)
{
    const char *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    if (NeedCreateGroup(isClient, session->opCode)) {
        return GenerateParamsByInput(osAccountId, groupId, jsonParams, session->params);
    } else {
        return GenerateParamsByDatabase(session->osAccountId, groupId, session->params);
    }
}

static int32_t InteractWithPeer(const BindSession *session, CJson *sendData)
{
    int32_t result = AddInfoToSendData(false, session, sendData);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate sendData!");
        return result;
    }
    return SendBindSessionData(session, sendData);
}

static int32_t InformSelfBindSuccess(const char *peerAuthId, const char *groupId, const BindSession *session,
    CJson *out)
{
    uint8_t sessionKey[DEFAULT_RETURN_KEY_LENGTH] = { 0 };
    if (GetByteFromJson(out, FIELD_SESSION_KEY, sessionKey, DEFAULT_RETURN_KEY_LENGTH) == HC_SUCCESS) {
        ProcessSessionKeyCallback(session->reqId, sessionKey, DEFAULT_RETURN_KEY_LENGTH, session->base.callback);
        (void)memset_s(sessionKey, DEFAULT_RETURN_KEY_LENGTH, 0, DEFAULT_RETURN_KEY_LENGTH);
        ClearSensitiveStringInJson(out, FIELD_SESSION_KEY);
    }

    char *jsonDataStr = NULL;
    int32_t result = GenerateBindSuccessData(peerAuthId, groupId, &jsonDataStr);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate the data to be sent to the service!");
        return result;
    }
    ProcessFinishCallback(session->reqId, session->opCode, jsonDataStr, session->base.callback);
    FreeJsonString(jsonDataStr);
    return HC_SUCCESS;
}

static int32_t InformSelfUnbindSuccess(const char *peerAuthId, const char *groupId, const BindSession *session)
{
    char *jsonDataStr = NULL;
    int32_t result = GenerateUnbindSuccessData(peerAuthId, groupId, &jsonDataStr);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate the data to be sent to the service!");
        return result;
    }
    ProcessFinishCallback(session->reqId, session->opCode, jsonDataStr, session->base.callback);
    FreeJsonString(jsonDataStr);
    return HC_SUCCESS;
}

static int32_t SetGroupId(const CJson *params, TrustedGroupEntry *groupParams)
{
    const char *groupId = GetStringFromJson(params, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from params!");
        return HC_ERR_JSON_GET;
    }
    if (!StringSetPointer(&groupParams->id, groupId)) {
        LOGE("Failed to copy groupId!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

static int32_t SetGroupName(const CJson *params, TrustedGroupEntry *groupParams)
{
    const char *groupName = GetStringFromJson(params, FIELD_GROUP_NAME);
    if (groupName == NULL) {
        LOGE("Failed to get groupName from params!");
        return HC_ERR_JSON_GET;
    }
    if (!StringSetPointer(&groupParams->name, groupName)) {
        LOGE("Failed to copy groupName!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

static int32_t SetGroupOwner(const char *ownerAppId, TrustedGroupEntry *groupParams)
{
    HcString ownerName = CreateString();
    if (!StringSetPointer(&ownerName, ownerAppId)) {
        LOGE("Failed to copy groupOwner!");
        return HC_ERR_MEMORY_COPY;
    }
    if (groupParams->managers.pushBackT(&groupParams->managers, ownerName) == NULL) {
        LOGE("Failed to push owner to vec!");
        return HC_ERR_MEMORY_COPY;
    }
    return HC_SUCCESS;
}

static int32_t SetGroupType(const CJson *params, TrustedGroupEntry *groupParams)
{
    /* Currently, only peer to peer group is supported. */
    (void)params;
    groupParams->type = PEER_TO_PEER_GROUP;
    return HC_SUCCESS;
}

static int32_t SetGroupVisibility(const CJson *params, TrustedGroupEntry *groupParams)
{
    int32_t groupVisibility = GROUP_VISIBILITY_PUBLIC;
    (void)GetIntFromJson(params, FIELD_GROUP_VISIBILITY, &groupVisibility);
    groupParams->visibility = groupVisibility;
    return HC_SUCCESS;
}

static int32_t SetGroupExpireTime(const CJson *params, TrustedGroupEntry *groupParams)
{
    int32_t expireTime = DEFAULT_EXPIRE_TIME;
    (void)GetIntFromJson(params, FIELD_EXPIRE_TIME, &expireTime);
    groupParams->expireTime = expireTime;
    return HC_SUCCESS;
}

static int32_t ForceDeletePeerKey(CJson *params)
{
    /* Use the DeviceGroupManager package name. */
    const char *appId = GROUP_MANAGER_PACKAGE_NAME;
    const char *peerAuthId = GetStringFromJson(params, FIELD_PEER_AUTH_ID);
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from params!");
        return HC_ERR_JSON_GET;
    }
    const char *groupId = GetStringFromJson(params, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from params!");
        return HC_ERR_JSON_GET;
    }
    int32_t peerUserType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(params, FIELD_PEER_USER_TYPE, &peerUserType);
    Uint8Buff peerAuthIdBuff = {
        .val = (uint8_t *)peerAuthId,
        .length = HcStrlen(peerAuthId)
    };
    return DeletePeerAuthInfo(appId, groupId, &peerAuthIdBuff, peerUserType, DAS_MODULE);
}

static int32_t GenerateGroupParams(const BindSession *session, TrustedGroupEntry *groupParams)
{
    int32_t result;
    if (((result = SetGroupId(session->params, groupParams)) != HC_SUCCESS) ||
        ((result = SetGroupName(session->params, groupParams)) != HC_SUCCESS) ||
        ((result = SetGroupOwner(session->appId, groupParams)) != HC_SUCCESS) ||
        ((result = SetGroupType(session->params, groupParams)) != HC_SUCCESS) ||
        ((result = SetGroupVisibility(session->params, groupParams)) != HC_SUCCESS) ||
        ((result = SetGroupExpireTime(session->params, groupParams)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupToDatabase(const BindSession *session)
{
    TrustedGroupEntry *groupParams = CreateGroupEntry();
    if (groupParams == NULL) {
        LOGE("Failed to allocate groupParams memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t result = GenerateGroupParams(session, groupParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate groupParams!");
        DestroyGroupEntry(groupParams);
        return HC_ERR_DB;
    }
    result = AddGroup(session->osAccountId, groupParams);
    DestroyGroupEntry(groupParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to add the group to the database!");
        return HC_ERR_DB;
    }
    return HC_SUCCESS;
}

static int32_t GenerateDevAuthParams(const char *authId, const char *udid, const char *groupId,
    int userType, TrustedDeviceEntry *devAuthParams)
{
    devAuthParams->devType = userType;
    StringSetPointer(&(devAuthParams->authId), authId);
    StringSetPointer(&(devAuthParams->udid), udid);
    StringSetPointer(&(devAuthParams->groupId), groupId);
    StringSetPointer(&(devAuthParams->serviceType), groupId);
    return HC_SUCCESS;
}

static int32_t AddTrustDevToDatabase(int32_t osAccountId, const char *authId, const char *udid, const char *groupId,
    int userType)
{
    TrustedDeviceEntry *devAuthParams = CreateDeviceEntry();
    if (devAuthParams == NULL) {
        LOGE("Failed to allocate devAuthParams memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t result = GenerateDevAuthParams(authId, udid, groupId, userType, devAuthParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate devAuthParams!");
        DestroyDeviceEntry(devAuthParams);
        return result;
    }
    result = AddTrustedDevice(osAccountId, devAuthParams);
    DestroyDeviceEntry(devAuthParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to add the trusted devices to the database!");
        return HC_ERR_DB;
    }
    return HC_SUCCESS;
}

static int32_t AddGroupAndLocalDevIfNotExist(const char *groupId, const BindSession *session)
{
    if (IsGroupExistByGroupId(session->osAccountId, groupId)) {
        return HC_SUCCESS;
    }
    int32_t result = HC_SUCCESS;
    char udid[INPUT_UDID_LEN] = { 0 };
    result = HcGetUdid((uint8_t *)udid, INPUT_UDID_LEN);
    if (result != HC_SUCCESS) {
        LOGE("Failed to get local udid! res: %d", result);
        return HC_ERR_DB;
    }
    result = AddGroupToDatabase(session);
    if (result != HC_SUCCESS) {
        return result;
    }
    const char *authId = GetStringFromJson(session->params, FIELD_AUTH_ID);
    if (authId == NULL) {
        LOGI("No authId is found. The default value is udid!");
        authId = udid;
    }
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(session->params, FIELD_USER_TYPE, &userType);
    return AddTrustDevToDatabase(session->osAccountId, authId, udid, groupId, userType);
}

static int32_t AddPeerDevToGroup(const char *peerAuthId, const char *peerUdid,
    const char *groupId, const BindSession *session)
{
    if (IsTrustedDeviceInGroup(session->osAccountId, groupId, peerUdid, true)) {
        LOGI("The peer device already exists in the group! RequestId: %" PRId64, session->reqId);
        QueryDeviceParams params = InitQueryDeviceParams();
        params.groupId = groupId;
        params.udid = peerUdid;
        if (DelTrustedDevice(session->osAccountId, &params) != HC_SUCCESS) {
            LOGE("Failed to delete the original data! RequestId: %" PRId64, session->reqId);
            return HC_ERR_DB;
        }
        LOGI("Delete the original data successfully! RequestId: %" PRId64, session->reqId);
    }
    int32_t peerUserType = DEVICE_TYPE_ACCESSORY;
    (void)GetIntFromJson(session->params, FIELD_PEER_USER_TYPE, &peerUserType);
    int32_t result = AddTrustDevToDatabase(session->osAccountId, peerAuthId, peerUdid, groupId, peerUserType);
    if (result != HC_SUCCESS) {
        LOGE("Failed to update the peer trusted device information! RequestId: %" PRId64, session->reqId);
        return result;
    }
    LOGI("The peer trusted device is added to the database successfully! RequestId: %" PRId64, session->reqId);
    return HC_SUCCESS;
}

static int32_t AddGroupAndDev(const char *peerAuthId, const char *peerUdid,
    const char *groupId, const BindSession *session)
{
    int32_t result = AddGroupAndLocalDevIfNotExist(groupId, session);
    if (result != HC_SUCCESS) {
        return result;
    }
    result = AddPeerDevToGroup(peerAuthId, peerUdid, groupId, session);
    if (result != HC_SUCCESS) {
        return result;
    }
    return SaveOsAccountDb(session->osAccountId);
}

static int32_t HandleBindSuccess(const char *peerAuthId, const char *peerUdid,
    const char *groupId, const BindSession *session, CJson *out)
{
    int32_t result = AddGroupAndDev(peerAuthId, peerUdid, groupId, session);
    if (result != HC_SUCCESS) {
        return result;
    }
    return InformSelfBindSuccess(peerAuthId, groupId, session, out);
}

static int32_t HandleUnbindSuccess(const char *peerAuthId, const char *groupId, const BindSession *session)
{
    if (IsGroupExistByGroupId(session->osAccountId, groupId)) {
        QueryDeviceParams params = InitQueryDeviceParams();
        params.groupId = groupId;
        params.authId = peerAuthId;
        if (DelTrustedDevice(session->osAccountId, &params) != HC_SUCCESS ||
            SaveOsAccountDb(session->osAccountId) != HC_SUCCESS) {
            LOGE("Failed to unbind device from database!");
            return HC_ERR_DB;
        }
        LOGI("The device is successfully unbound from the database!");
    }
    return InformSelfUnbindSuccess(peerAuthId, groupId, session);
}

static int32_t OnBindOrUnbindFinish(const BindSession *session, const CJson *jsonParams, CJson *out)
{
    const char *peerAuthId = GetStringFromJson(jsonParams, FIELD_PEER_DEVICE_ID);
    if (peerAuthId == NULL) {
        peerAuthId = GetStringFromJson(session->params, FIELD_PEER_AUTH_ID);
    }
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from jsonParams and params!");
        return HC_ERR_JSON_GET;
    }
    const char *peerUdid = GetStringFromJson(jsonParams, FIELD_CONN_DEVICE_ID);
    if (peerUdid == NULL) {
        peerUdid = GetStringFromJson(session->params, FIELD_PEER_UDID);
    }
    if (peerUdid == NULL) {
        LOGE("Failed to get peerUdid from jsonParams and params!");
        return HC_ERR_JSON_GET;
    }
    const char *groupId = GetStringFromJson(session->params, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from session params!");
        return HC_ERR_JSON_GET;
    }
    if (session->opCode == MEMBER_DELETE) {
        return HandleUnbindSuccess(peerAuthId, groupId, session);
    } else {
        return HandleBindSuccess(peerAuthId, peerUdid, groupId, session, out);
    }
}

static int32_t OnSessionFinish(const BindSession *session, CJson *jsonParams, CJson *out)
{
    int32_t result;
    CJson *sendData = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    /* The last packet may need to be sent. */
    if (sendData != NULL) {
        result = InteractWithPeer(session, sendData);
        if (result != HC_SUCCESS) {
            return result;
        }
    }
    result = OnBindOrUnbindFinish(session, jsonParams, out);
    if (result != HC_SUCCESS) {
        LOGE("An error occurred when processing different end operations!");
        return result;
    }
    LOGI("The session completed successfully! [ReqId]: %" PRId64, session->reqId);
    NotifyBindResult(session->channelType, session->channelId);
    CloseChannel(session->channelType, session->channelId);
    return HC_SUCCESS;
}

static int32_t ProcessBindSessionInner(BindSession *session, CJson *jsonParams, int32_t *status, bool *isNeedInform)
{
    int32_t result;
    if (((result = CheckPeerStatus(jsonParams, isNeedInform)) != HC_SUCCESS) ||
        ((result = TryAddPeerUserTypeToParams(jsonParams, session))) != HC_SUCCESS) {
        return result;
    }

    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to allocate out memory!");
        return HC_ERR_JSON_FAIL;
    }
    result = ProcessModule(session, jsonParams, out, status);
    if (result != HC_SUCCESS) {
        *isNeedInform = false;
        InformPeerModuleError(out, session);
        FreeJson(out);
        return result;
    }
    if (*status == IGNORE_MSG) {
        FreeJson(out);
        return HC_SUCCESS;
    } else if (*status == CONTINUE) {
        DeleteAllItem(jsonParams);
        CJson *sendData = DetachItemFromJson(out, FIELD_SEND_TO_PEER);
        FreeJson(out);
        if (sendData == NULL) {
            LOGE("Failed to get sendToPeer from out!");
            return HC_ERR_JSON_GET;
        }
        result = InteractWithPeer(session, sendData);
        FreeJson(sendData);
        return result;
    }
    result = OnSessionFinish(session, jsonParams, out);
    FreeJson(out);
    return result;
}

int32_t ProcessBindSession(Session *session, CJson *jsonParams)
{
    if ((session == NULL) || (jsonParams == NULL)) {
        LOGE("The input session or jsonParams is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    BindSession *realSession = (BindSession *)session;
    LOGI("Start to process bind session successfully! [ReqId]: %" PRId64, realSession->reqId);

    bool isNeedInform = true;
    int32_t status = CONTINUE;
    int32_t result = ProcessBindSessionInner(realSession, jsonParams, &status, &isNeedInform);
    if (result != HC_SUCCESS) {
        LOGE("An error occurs during processing bind session. We need to notify the service!");
        InformPeerGroupErrorIfNeed(isNeedInform, result, realSession);
        if ((!NeedForceDelete(realSession)) || (ForceUnbindDevice(realSession) != HC_SUCCESS)) {
            ProcessErrorCallback(realSession->reqId, realSession->opCode, result, NULL,
                realSession->base.callback);
        }
        CloseChannel(realSession->channelType, realSession->channelId);
        return result;
    }
    LOGI("Process bind session successfully! [ReqId]: %" PRId64, realSession->reqId);
    if (status == FINISH) {
        return status;
    }
    return result;
}

bool NeedCreateGroup(int isClient, int operationCode)
{
    if (((isClient == CLIENT) && (operationCode == MEMBER_JOIN)) ||
        ((isClient == SERVER) && (operationCode == MEMBER_INVITE))) {
        return true;
    } else {
        return false;
    }
}

bool NeedForceDelete(const BindSession *session)
{
    bool needForceDelete = false;
    (void)GetBoolFromJson(session->params, FIELD_IS_FORCE_DELETE, &needForceDelete);
    return needForceDelete;
}

int32_t ForceUnbindDevice(const BindSession *session)
{
    const char *peerAuthId = GetStringFromJson(session->params, FIELD_PEER_AUTH_ID);
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *groupId = GetStringFromJson(session->params, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    QueryDeviceParams queryDeviceParams = InitQueryDeviceParams();
    queryDeviceParams.groupId = groupId;
    queryDeviceParams.authId = peerAuthId;
    if (DelTrustedDevice(session->osAccountId, &queryDeviceParams) != HC_SUCCESS ||
        SaveOsAccountDb(session->osAccountId) != HC_SUCCESS) {
        LOGE("Failed to delete trust device from database!");
        return HC_ERR_DB;
    }
    /*
     * If the trusted device has been deleted from the database but the peer key fails to be deleted,
     * the forcible unbinding is still considered successful. Only logs need to be printed.
     */
    int32_t result = ForceDeletePeerKey(session->params);
    if (result != HC_SUCCESS) {
        LOGE("Failed to delete peer key!");
    }
    LOGI("An error occurs during the online unbinding. Therefore, we forcibly unbind the peer device!");
    char *returnDataStr = NULL;
    result = GenerateUnbindSuccessData(peerAuthId, groupId, &returnDataStr);
    if (result != HC_SUCCESS) {
        return result;
    }
    ProcessFinishCallback(session->reqId, MEMBER_DELETE, returnDataStr, session->base.callback);
    FreeJsonString(returnDataStr);
    return HC_SUCCESS;
}

int32_t GenerateBindParams(int32_t osAccountId, int isClient, const CJson *jsonParams, BindSession *session)
{
    if (session->params == NULL) {
        session->params = CreateJson();
        if (session->params == NULL) {
            LOGE("Failed to allocate session params memory!");
            return HC_ERR_ALLOC_MEMORY;
        }
    }

    int32_t result;
    if (((result = AddIsForceDeleteIfNeed(isClient, jsonParams, session)) != HC_SUCCESS) ||
        ((result = AddChannelIdIfNeed(isClient, jsonParams, session)) != HC_SUCCESS) ||
        ((result = AddPinCodeIfNeed(jsonParams, session)) != HC_SUCCESS) ||
        ((result = AddGroupAndDevInfo(osAccountId, isClient, jsonParams, session)) != HC_SUCCESS) ||
        ((result = AddPeerDevInfoIfNeed(isClient, jsonParams, session)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

int32_t AddInfoToSendData(bool isNeedCompatibleInfo, const BindSession *session, CJson *data)
{
    int32_t result;
    if (((result = AddGroupInfoToSendData(session, data)) != HC_SUCCESS) ||
        ((result = AddDevInfoToSendData(session, data)) != HC_SUCCESS) ||
        ((result = AddRequestInfoToSendData(session, data)) != HC_SUCCESS) ||
        ((result = AddCompatibleInfoToSendData(isNeedCompatibleInfo, data)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

int32_t GenerateBasicModuleParams(bool isClient, BindSession *session, CJson *moduleParams)
{
    int32_t result;
    if (((result = AddGroupAndDevInfoToParams(session, moduleParams)) != HC_SUCCESS) ||
        ((result = AddRequestInfoToParams(isClient, session, moduleParams)) != HC_SUCCESS) ||
        ((result = AddPinCodeToParamsIfNeed(session, moduleParams)) != HC_SUCCESS) ||
        ((result = AddPeerAuthIdAndUserTypeToParamsIfNeed(session, moduleParams)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}
