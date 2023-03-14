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

#include "auth_session_common.h"
#include "account_related_group_auth.h"
#include "account_unrelated_group_auth.h"
#include "alg_defs.h"
#include "auth_session_util.h"
#include "common_defs.h"
#include "string_util.h"
#include "data_manager.h"
#include "dev_auth_module_manager.h"
#include "group_auth_data_operation.h"
#include "hc_dev_info.h"
#include "hc_log.h"
#include "json_utils.h"

#define MIN_PROTOCOL_VERSION "1.0.0"
IMPLEMENT_HC_VECTOR(ParamsVec, void *, 1)

static bool IsOldFormatParams(const CJson *param)
{
    int32_t authForm = AUTH_FORM_INVALID_TYPE;
    (void)GetIntFromJson(param, FIELD_AUTH_FORM, &authForm);
    int64_t uid = 0;
    (void)GetByteFromJson(param, FIELD_USER_ID, (uint8_t *)&uid, sizeof(int64_t));
    bool isOldFormat = ((GetStringFromJson(param, FIELD_SELF_AUTH_ID) != NULL) ||
        (uid != 0L) || (authForm == AUTH_FORM_IDENTICAL_ACCOUNT));
    return isOldFormat;
}

static int32_t UnifyOldFormatParams(const CJson *param, ParamsVec *paramsVec)
{
    LOGI("Unify old format parameters.");
    CJson *oldFormatParams = DuplicateJson(param);
    if (oldFormatParams == NULL) {
        LOGE("Failed to create json for old form params!");
        return HC_ERR_JSON_FAIL;
    }
    int64_t uid = 0;
    uint32_t groupIdLen = SHA256_LEN * BYTE_TO_HEX_OPER_LENGTH + 1;
    char *groupId = (char *)HcMalloc(groupIdLen, 0);
    if (groupId == NULL) {
        LOGE("Failed to allocate memory for groupId!");
        FreeJson(oldFormatParams);
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t res = HC_SUCCESS;
    do {
        if (GetByteFromJson(param, FIELD_USER_ID, (uint8_t *)&uid, sizeof(int64_t)) != HC_SUCCESS) {
            LOGI("No uid in auth param!");
            res = HC_ERR_JSON_GET;
            break;
        }
        if (GetInfoHash((const uint8_t *)&uid, sizeof(int64_t), groupId, groupIdLen) != HC_SUCCESS) {
            LOGE("Failed to get hash for groupId!");
            res = HC_ERR_HASH_FAIL;
            break;
        }
        if (AddStringToJson(oldFormatParams, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
            LOGE("Failed to add groupId to json params!");
            res = HC_ERR_JSON_FAIL;
            break;
        }
    } while (0);
    HcFree(groupId);
    groupId = NULL;
    if (res != HC_SUCCESS) {
        FreeJson(oldFormatParams);
        return res;
    }
    paramsVec->pushBack(paramsVec, (const void **)&oldFormatParams);
    return res;
}

static bool IsGroupAvailable(int32_t osAccountId, const char *groupId, const char *pkgName)
{
    if (GaIsGroupAccessible(osAccountId, groupId, pkgName)) {
        return true;
    }
    LOGD("%s don't have enough right in group: %s, os account:%d!", pkgName, groupId, osAccountId);
    return false;
}

static int32_t AddGeneralParams(const char *groupId, int32_t groupType, const TrustedDeviceEntry *localAuthInfo,
    CJson *paramsData)
{
    int32_t authForm = GroupTypeToAuthForm(groupType);
    if (AddStringToJson(paramsData, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId for client auth!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(paramsData, FIELD_AUTH_FORM, authForm) != HC_SUCCESS) {
        LOGE("Failed to add authFrom for client auth!");
        return HC_ERR_JSON_FAIL;
    }
    const char *serviceType = StringGet(&(localAuthInfo->serviceType));
    if ((groupType == COMPATIBLE_GROUP) && (serviceType != NULL)) {
        if (AddStringToJson(paramsData, FIELD_SERVICE_TYPE, serviceType) != HC_SUCCESS) {
            LOGE("Failed to add serviceType for client compatible group auth!");
            return HC_ERR_JSON_FAIL;
        }
    } else if (AddStringToJson(paramsData, FIELD_SERVICE_TYPE, groupId) != HC_SUCCESS) {
        LOGE("Failed to add serviceType with groupId for client auth!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t GetLocalDeviceInfoFromDatabase(int32_t osAccountId, const char *groupId,
    TrustedDeviceEntry *localAuthInfo)
{
    char *localUdid = (char *)HcMalloc(INPUT_UDID_LEN, 0);
    if (localUdid == NULL) {
        LOGE("Failed to malloc for local udid!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t res = HcGetUdid((uint8_t *)localUdid, INPUT_UDID_LEN);
    if (res != HC_SUCCESS) {
        LOGE("[DB]: Failed to get local udid! res: %d", res);
        HcFree(localUdid);
        return res;
    }

    res = GaGetTrustedDeviceEntryById(osAccountId, localUdid, true, groupId, localAuthInfo);
    HcFree(localUdid);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get local device info from database!");
    }
    return res;
}

static int32_t ExtractAndAddParams(int32_t osAccountId, const char *groupId,
    const TrustedGroupEntry *groupInfo, CJson *paramsData)
{
    int32_t res;
    TrustedDeviceEntry *localAuthInfo = CreateDeviceEntry();
    if (localAuthInfo == NULL) {
        LOGE("Failed to allocate memory for localAuthInfo!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t groupType = groupInfo->type;
    int32_t authForm = GroupTypeToAuthForm(groupType);
    do {
        res = GetLocalDeviceInfoFromDatabase(osAccountId, groupId, localAuthInfo);
        if (res != HC_SUCCESS) {
            break;
        }
        res = AddGeneralParams(groupId, groupType, localAuthInfo, paramsData);
        if (res != HC_SUCCESS) {
            LOGE("Failed to add general params!");
            break;
        }
        BaseGroupAuth *groupAuth = GetGroupAuth(GetGroupAuthType(authForm));
        if (groupAuth == NULL) {
            LOGE("Failed to get group auth handle!");
            break;
        }
        res = groupAuth->fillDeviceAuthInfo(osAccountId, groupInfo, localAuthInfo, paramsData);
        if (res != HC_SUCCESS) {
            LOGE("Failed to fill device auth info!");
            break;
        }
    } while (0);
    DestroyDeviceEntry(localAuthInfo);
    return res;
}

static int32_t FillAuthParams(int32_t osAccountId, const CJson *param,
    const GroupEntryVec *vec, ParamsVec *paramsVec)
{
    const char *peerUdid = GetStringFromJson(param, FIELD_PEER_CONN_DEVICE_ID);
    const char *peerAuthId = GetStringFromJson(param, FIELD_PEER_AUTH_ID);
    const char *pkgName = GetStringFromJson(param, FIELD_SERVICE_PKG_NAME);
    if (pkgName == NULL) {
        LOGE("Pkg name is null, can't extract params from db!");
        return HC_ERR_NULL_PTR;
    }
    uint32_t index;
    TrustedGroupEntry **ptr = NULL;
    FOR_EACH_HC_VECTOR(*vec, index, ptr) {
        if ((ptr == NULL) || (*ptr == NULL)) {
            continue;
        }
        const TrustedGroupEntry *groupInfo = (TrustedGroupEntry *)(*ptr);
        const char *groupId = StringGet(&(groupInfo->id));
        if (groupId == NULL) {
            continue;
        }
        if (!IsGroupAvailable(osAccountId, groupId, pkgName)) {
            continue;
        }
        if (!GaIsDeviceInGroup(groupInfo->type, osAccountId, peerUdid, peerAuthId, groupId)) {
            continue;
        }
        CJson *paramsData = DuplicateJson(param);
        if (paramsData == NULL) {
            LOGE("Failed to duplicate auth param data!");
            return HC_ERR_JSON_FAIL;
        }
        int32_t res = ExtractAndAddParams(osAccountId, groupId, groupInfo, paramsData);
        if (res != HC_SUCCESS) {
            LOGE("Failed to extract and add param!");
            FreeJson(paramsData);
            continue;
        }
        paramsVec->pushBack(paramsVec, (const void **)&paramsData);
    }
    return HC_SUCCESS;
}

static void GetCandidateGroupByOrder(int32_t osAccountId, const CJson *param,
    QueryGroupParams *queryParams, GroupEntryVec *vec)
{
    BaseGroupAuth *groupAuth = GetGroupAuth(ACCOUNT_RELATED_GROUP_AUTH_TYPE);
    if (groupAuth != NULL) {
        AccountRelatedGroupAuth *realGroupAuth = (AccountRelatedGroupAuth *)groupAuth;
        realGroupAuth->getAccountCandidateGroup(param, queryParams, vec);
    }
    queryParams->groupType = PEER_TO_PEER_GROUP;
    if (QueryGroups(osAccountId, queryParams, vec) != HC_SUCCESS) {
        LOGD("No peer to peer group in db.");
    }
    queryParams->groupType = COMPATIBLE_GROUP;
    if (QueryGroups(osAccountId, queryParams, vec) != HC_SUCCESS) {
        LOGD("No compatible group in db.");
    }
}

static void GetCandidateGroupInfo(int32_t osAccountId, const CJson *param, GroupEntryVec *vec)
{
    LOGI("No input of groupId, extract group info without groupId.");
    bool deviceLevelFlag = false;
    bool isClient = true;
    (void)GetBoolFromJson(param, FIELD_IS_DEVICE_LEVEL, &deviceLevelFlag);
    if (GetBoolFromJson(param, FIELD_IS_CLIENT, &isClient) != HC_SUCCESS) {
        LOGE("Failed to get the value: isClient!");
        return;
    }
    QueryGroupParams queryParams = InitQueryGroupParams();
    if (deviceLevelFlag && isClient) {
        LOGI("Try to get device-level candidate groups for auth.");
    } else {
        queryParams.groupVisibility = GROUP_VISIBILITY_PUBLIC;
    }
    GetCandidateGroupByOrder(osAccountId, param, &queryParams, vec);
}

static void GetGroupInfoByGroupId(int32_t osAccountId, const char *groupId,
    GroupEntryVec *groupEntryVec)
{
    if (groupId == NULL) {
        LOGE("GroupId is null!");
        return;
    }
    QueryGroupParams queryParams = InitQueryGroupParams();
    queryParams.groupId = groupId;
    if (QueryGroups(osAccountId, &queryParams, groupEntryVec) != HC_SUCCESS) {
        LOGE("Failed to query groups for groupId %s!", groupId);
        return;
    }
}

static int32_t GetBleGroupInfoAndAuthParams(const CJson *param, ParamsVec *paramsVec)
{
    CJson *bleAuthParams = DuplicateJson(param);
    if (bleAuthParams == NULL) {
        LOGE("Failed to create json for ble auth params!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(bleAuthParams, FIELD_AUTH_FORM, AUTH_FORM_ACROSS_ACCOUNT) != HC_SUCCESS) {
        LOGE("Failed to add ble authForm!");
        FreeJson(bleAuthParams);
        return HC_ERR_JSON_FAIL;
    }
    paramsVec->pushBack(paramsVec, (const void **)&bleAuthParams);
    return HC_SUCCESS;
}

static int32_t GetCandidateAuthInfo(int32_t osAccountId, const char *groupId,
    const CJson *param, ParamsVec *authParamsVec)
{
    GroupEntryVec vec = CreateGroupEntryVec();
    if (groupId == NULL) {
        GetCandidateGroupInfo(osAccountId, param, &vec);
    } else {
        GetGroupInfoByGroupId(osAccountId, groupId, &vec);
    }
    if (vec.size(&vec) == 0) {
        LOGE("No satisfied candidate group!");
        ClearGroupEntryVec(&vec);
        return HC_ERR_NO_CANDIDATE_GROUP;
    }
    int32_t res = FillAuthParams(osAccountId, param, &vec, authParamsVec);
    ClearGroupEntryVec(&vec);
    return res;
}

static int32_t AddGroupAuthTransmitData(const AuthSession *session, CJson *sendToPeer)
{
    ParamsVec list = session->paramsList;
    CJson *authParam = list.get(&list, session->currentIndex);
    if (authParam == NULL) {
        LOGE("The json data in session is null!");
        return HC_ERR_NULL_PTR;
    }
    bool isDeviceLevel = false;
    /* Disable device-level auth. */
    if (AddBoolToJson(sendToPeer, FIELD_IS_DEVICE_LEVEL, isDeviceLevel) != HC_SUCCESS) {
        LOGE("Failed to add device level!");
        return HC_ERR_JSON_FAIL;
    }
    bool isClient = true;
    if (GetBoolFromJson(authParam, FIELD_IS_CLIENT, &isClient)) {
        LOGE("Failed to get isClient!");
        return HC_ERR_JSON_GET;
    }
    if (isClient && (session->currentIndex < (list.size(&list) - 1))) {
        CJson *nextParam = list.get(&list, session->currentIndex + 1);
        if (nextParam == NULL) {
            LOGE("Failed to get next auth params!");
            return HC_ERR_JSON_FAIL;
        }
        const char *altGroup = GetStringFromJson(nextParam, FIELD_SERVICE_TYPE);
        if ((altGroup != NULL) && (AddStringToJson(sendToPeer, FIELD_ALTERNATIVE, altGroup) != HC_SUCCESS)) {
            LOGE("Failed to add alternative group!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t ReturnTransmitData(const AuthSession *session, CJson *out)
{
    CJson *sendToPeer = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (sendToPeer == NULL) {
        LOGI("The transmit data to peer is null!");
        return HC_ERR_JSON_GET;
    }
    CJson *authParam = (session->paramsList).get(&(session->paramsList), session->currentIndex);
    if (authParam == NULL) {
        LOGE("The json data in session is null!");
        return HC_ERR_NULL_PTR;
    }
    int64_t requestId = 0;
    if (GetByteFromJson(authParam, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return HC_ERR_JSON_GET;
    }

    int32_t ret = AddGroupAuthTransmitData(session, sendToPeer);
    if (ret != HC_SUCCESS) {
        LOGE("Failed to add extra data!");
        return ret;
    }
    char *outStr = PackJsonToString(sendToPeer);
    if (outStr == NULL) {
        LOGE("Failed to pack outStr for onTransmit!");
        return HC_ERR_ALLOC_MEMORY;
    }

    const DeviceAuthCallback *callback = session->base.callback;
    do {
        if ((callback == NULL) || (callback->onTransmit == NULL)) {
            LOGE("The callback for transmit is null!");
            ret = HC_ERR_TRANSMIT_FAIL;
            break;
        }
        LOGI("Start to transmit data to peer for auth!");
        if (!callback->onTransmit(requestId, (uint8_t *)outStr, HcStrlen(outStr) + 1)) {
            LOGE("Failed to transmit data to peer!");
            ret = HC_ERR_TRANSMIT_FAIL;
        }
        LOGI("End transmit data to peer for auth!");
    } while (0);
    FreeJsonString(outStr);
    return ret;
}

int32_t GetAuthParamsList(int32_t osAccountId, const CJson *param, ParamsVec *authParamsVec)
{
    int32_t ret;
    const char *groupId = GetStringFromJson(param, FIELD_GROUP_ID);
    if (groupId == NULL) {
        groupId = GetStringFromJson(param, FIELD_SERVICE_TYPE);
    }
    if (IsOldFormatParams(param)) {
        LOGI("The input params' type is in old format!");
        ret = UnifyOldFormatParams(param, authParamsVec);
    } else if (IsBleAuthForAcrossAccount(param)) {
        LOGD("This is across-account auth for ble device.");
        ret = GetBleGroupInfoAndAuthParams(param, authParamsVec);
    } else {
        ret = GetCandidateAuthInfo(osAccountId, groupId, param, authParamsVec);
    }
    return ret;
}

static void ReturnFinishData(const AuthSession *session, const CJson *out)
{
    ParamsVec list = session->paramsList;
    const CJson *authParam = list.get(&list, session->currentIndex);
    if (authParam == NULL) {
        LOGE("The json data in session is null!");
        return;
    }
    int64_t requestId = 0;
    if (GetByteFromJson(authParam, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return;
    }
    int32_t authForm = AUTH_FORM_INVALID_TYPE;
    if (GetIntFromJson(authParam, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
        LOGE("Failed to get auth type!");
        return;
    }
    BaseGroupAuth *groupAuth = GetGroupAuth(GetGroupAuthType(authForm));
    if (groupAuth != NULL) {
        groupAuth->onFinish(requestId, authParam, out, session->base.callback);
    }
}

static int32_t ReturnErrorToLocalBySession(const AuthSession *session, int errorCode)
{
    ParamsVec list = session->paramsList;
    CJson *authParam = list.get(&list, session->currentIndex);
    int64_t requestId = 0;
    int32_t authForm = 0;
    if (authParam == NULL) {
        LOGE("The json data in session is null!");
        return HC_ERR_NULL_PTR;
    }
    if (GetByteFromJson(authParam, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to add request id!");
        return HC_ERR_JSON_GET;
    }
    if (GetIntFromJson(authParam, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
        LOGE("Failed to add auth form!");
        return HC_ERR_JSON_GET;
    }

    BaseGroupAuth *groupAuth = GetGroupAuth(GetGroupAuthType(authForm));
    if (groupAuth != NULL) {
        LOGE("Invoke ReturnErrorToLocalBySession for authForm:%d!", authForm);
        groupAuth->onError(requestId, session, errorCode);
    }
    return HC_SUCCESS;
}

static int32_t AddVersionMsgToPeer(CJson *errorToPeer)
{
    CJson *version = CreateJson();
    if (version == NULL) {
        LOGE("Failed to create json for version!");
        return HC_ERR_JSON_CREATE;
    }
    CJson *payload = CreateJson();
    if (payload == NULL) {
        LOGE("Failed to create json for payload!");
        FreeJson(version);
        return HC_ERR_JSON_CREATE;
    }
    int32_t res = HC_SUCCESS;
    do {
        if (AddStringToJson(version, FIELD_MIN_VERSION, MIN_PROTOCOL_VERSION) != HC_SUCCESS) {
            LOGE("Failed to add min version to json!");
            res = HC_ERR_JSON_ADD;
            break;
        }
        if (AddStringToJson(version, FIELD_CURRENT_VERSION, MIN_PROTOCOL_VERSION) != HC_SUCCESS) {
            LOGE("Failed to add max version to json!");
            res = HC_ERR_JSON_ADD;
            break;
        }
        if (AddObjToJson(payload, FIELD_VERSION, version) != HC_SUCCESS) {
            LOGE("Add version object to errorToPeer failed.");
            res = HC_ERR_JSON_ADD;
            break;
        }
        if (AddIntToJson(payload, FIELD_ERROR_CODE, -1) != HC_SUCCESS) {
            LOGE("Failed to add errorCode for peer!");
            res = HC_ERR_JSON_ADD;
            break;
        }
        if (AddObjToJson(errorToPeer, FIELD_PAYLOAD, payload) != HC_SUCCESS) {
            res = HC_ERR_JSON_ADD;
            break;
        }
    } while (0);
    FreeJson(version);
    FreeJson(payload);
    return res;
}

static int32_t PrepareErrorMsgToPeer(CJson *errorToPeer)
{
    if (AddIntToJson(errorToPeer, FIELD_GROUP_ERROR_MSG, GROUP_ERR_MSG) != HC_SUCCESS) {
        LOGE("Failed to add groupErrorMsg for peer!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(errorToPeer, FIELD_MESSAGE, GROUP_ERR_MSG) != HC_SUCCESS) {
        LOGE("Failed to add message for peer!");
        return HC_ERR_JSON_FAIL;
    }
    return AddVersionMsgToPeer(errorToPeer);
}

static int32_t ReturnErrorToPeerBySession(const CJson *authParam, const DeviceAuthCallback *callback)
{
    int64_t requestId = 0;
    if (GetByteFromJson(authParam, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request ID!");
        return HC_ERR_JSON_GET;
    }
    CJson *errorToPeer = CreateJson();
    if (errorToPeer == NULL) {
        LOGE("Failed to allocate memory for errorToPeer!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t res = PrepareErrorMsgToPeer(errorToPeer);
    if (res != HC_SUCCESS) {
        FreeJson(errorToPeer);
        return res;
    }
    char *errorToPeerStr = PackJsonToString(errorToPeer);
    FreeJson(errorToPeer);
    if (errorToPeerStr == NULL) {
        LOGE("Failed to pack errorToPeer to string!");
        return HC_ERR_ALLOC_MEMORY;
    }

    do {
        if ((callback == NULL) || (callback->onTransmit == NULL)) {
            LOGE("The callback of onTransmit is null!");
            res = HC_ERR_NULL_PTR;
            break;
        }
        LOGD("Begin transmit error msg to peer by session!");
        if (!callback->onTransmit(requestId, (uint8_t *)errorToPeerStr, HcStrlen(errorToPeerStr) + 1)) {
            LOGE("Failed to invoke onTransmit by session!");
            res = HC_ERR_TRANSMIT_FAIL;
            break;
        }
        LOGD("End transmit error msg to peer by session!");
    } while (0);
    FreeJsonString(errorToPeerStr);
    return res;
}

static int32_t ReturnErrorToPeerByTask(const CJson *sendToPeer, const CJson *authParam,
    const DeviceAuthCallback *callback)
{
    int64_t requestId = 0;
    if (GetByteFromJson(authParam, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return HC_ERR_JSON_FAIL;
    }
    char *sendToPeerStr = PackJsonToString(sendToPeer);
    if (sendToPeerStr == NULL) {
        LOGE("Failed to pack json to string!");
        return HC_ERR_ALLOC_MEMORY;
    }

    int32_t res = HC_SUCCESS;
    do {
        if ((callback == NULL) || (callback->onTransmit == NULL)) {
            LOGE("The callback of onTransmit is null!");
            res = HC_ERR_NULL_PTR;
            break;
        }
        LOGD("Begin transmit error msg to peer by task!");
        if (!callback->onTransmit(requestId, (uint8_t *)sendToPeerStr, HcStrlen(sendToPeerStr) + 1)) {
            LOGE("Failed to invoke onTransmit by task!");
            res = HC_ERR_TRANSMIT_FAIL;
            break;
        }
        LOGD("End transmit error msg to peer by task!");
    } while (0);
    FreeJsonString(sendToPeerStr);
    return res;
}

bool HasAlternativeAuthGroup(const AuthSession *session)
{
    if (session->currentIndex >= session->paramsList.size(&session->paramsList) - 1) {
        return false;
    }
    return true;
}

static int32_t ProcessNextGroupIfPossible(AuthSession *session)
{
    if (!HasAlternativeAuthGroup(session)) {
        LOGD("There is no alternative auth group.");
        return HC_ERR_NO_CANDIDATE_GROUP;
    }
    int32_t res;
    session->currentIndex++;
    CJson *paramInNextSession = (session->paramsList).get(&(session->paramsList), session->currentIndex);
    if (paramInNextSession == NULL) {
        LOGE("The json data in session is null!");
        return HC_ERR_NULL_PTR;
    }
    CJson *outNext = CreateJson();
    if (outNext == NULL) {
        LOGE("Failed to create json for outNext!");
        return HC_ERR_ALLOC_MEMORY;
    }
    do {
        int32_t status = 0;
        res = CreateAndProcessTask(session, paramInNextSession, outNext, &status);
        if (res != HC_SUCCESS) {
            break;
        }
        res = ProcessTaskStatusForAuth(session, paramInNextSession, outNext, status);
    } while (0);
    if (res != HC_SUCCESS) {
        DestroyTask(session->curTaskId, GetAuthModuleType(paramInNextSession));
        res = InformAuthError(session, outNext, res);
    }
    FreeJson(outNext);
    return res;
}

int32_t ReturnSessionKey(int64_t requestId, const CJson *authParam,
    const CJson *out, const DeviceAuthCallback *callback)
{
    int32_t keyLen = DEFAULT_RETURN_KEY_LENGTH;
    (void)GetIntFromJson(authParam, FIELD_KEY_LENGTH, &keyLen);
    uint8_t *sessionKey = (uint8_t *)HcMalloc(keyLen, 0);
    if (sessionKey == NULL) {
        LOGE("Failed to allocate memory for sessionKey!");
        return HC_ERR_ALLOC_MEMORY;
    }

    int32_t res = HC_SUCCESS;
    do {
        if (GetByteFromJson(out, FIELD_SESSION_KEY, sessionKey, keyLen) != HC_SUCCESS) {
            LOGE("Failed to get sessionKey!");
            res = HC_ERR_JSON_GET;
            break;
        }
        if ((callback == NULL) || (callback->onSessionKeyReturned == NULL)) {
            LOGE("The callback of onSessionKeyReturned is null!");
            res = HC_ERR_INVALID_PARAMS;
            break;
        }
        LOGI("Begin invoke onSessionKeyReturned.");
        callback->onSessionKeyReturned(requestId, sessionKey, keyLen);
        LOGI("End invoke onSessionKeyReturned, res = %d.", res);
    } while (0);
    (void)memset_s(sessionKey, keyLen, 0, keyLen);
    HcFree(sessionKey);
    sessionKey = NULL;
    return res;
}

void DeleteCachedData(CJson *paramInSession)
{
    DeleteItemFromJson(paramInSession, FIELD_PAYLOAD);
    DeleteItemFromJson(paramInSession, FIELD_SELF_AUTH_ID);
    DeleteItemFromJson(paramInSession, FIELD_OPERATION_CODE);
}

int32_t GetGroupAuthType(int32_t authForm)
{
    switch (authForm) {
        case AUTH_FORM_ACCOUNT_UNRELATED:
            return ACCOUNT_UNRELATED_GROUP_AUTH_TYPE;
        case AUTH_FORM_IDENTICAL_ACCOUNT:
            return ACCOUNT_RELATED_GROUP_AUTH_TYPE;
        case AUTH_FORM_ACROSS_ACCOUNT:
            return ACCOUNT_RELATED_GROUP_AUTH_TYPE;
        default:
            LOGE("Invalid authForm!");
            return INVALID_GROUP_AUTH_TYPE;
    }
}

int32_t InformAuthError(AuthSession *session, const CJson *out, int errorCode)
{
    ParamsVec list = session->paramsList;
    CJson *paramInSession = list.get(&list, session->currentIndex);
    if (paramInSession == NULL) {
        LOGE("The json data in session is null!");
        return HC_ERR_NULL_PTR;
    }
    int32_t res;
    if (out == NULL) {
        res = ReturnErrorToPeerBySession(paramInSession, session->base.callback);
        LOGI("Out data is null, so assemble error msg to peer by auth session.");
        return res;
    }
    const CJson *sendToPeer = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (sendToPeer != NULL) {
        res = ReturnErrorToPeerByTask(sendToPeer, paramInSession, session->base.callback);
        if (res != HC_SUCCESS) {
            LOGE("Failed to return task's error msg to peer!");
            return res;
        }
    }

    res = ProcessNextGroupIfPossible(session);
    if (res == HC_SUCCESS) {
        return HC_SUCCESS;
    }

    const char *altGroup = GetStringFromJson(paramInSession, FIELD_ALTERNATIVE);
    if (altGroup == NULL) {
        res = ReturnErrorToLocalBySession(session, errorCode);
    }
    return res;
}

void ProcessDeviceLevel(const CJson *receiveData, CJson *authParam)
{
    bool receiveLevel = false;
    bool authLevel = false;
    (void)GetBoolFromJson(receiveData, FIELD_IS_DEVICE_LEVEL, &receiveLevel);
    (void)GetBoolFromJson(authParam, FIELD_IS_DEVICE_LEVEL, &authLevel);
    authLevel = receiveLevel && authLevel;
    if (AddBoolToJson(authParam, FIELD_IS_DEVICE_LEVEL, authLevel) != HC_SUCCESS) {
        LOGE("Failed to add device level to auth param!");
        return;
    }
}

int32_t ProcessTaskStatusForAuth(const AuthSession *session, const CJson *param, CJson *out, int32_t status)
{
    int32_t res = HC_SUCCESS;
    switch (status) {
        case IGNORE_MSG:
            LOGD("Ignore this msg.");
            break;
        case CONTINUE:
            res = ReturnTransmitData(session, out);
            if (res != HC_SUCCESS) {
                LOGE("Failed to transmit data to peer!");
                InformLocalAuthError(param, session->base.callback);
            }
            break;
        case FINISH:
            ReturnFinishData(session, out);
            ClearSensitiveStringInJson(out, FIELD_SESSION_KEY);
            res = FINISH;
            break;
        default:
            LOGE("Invalid status after process task!");
            res = HC_ERR_INVALID_PARAMS;
            break;
    }
    return res;
}

int32_t CreateAndProcessTask(AuthSession *session, CJson *paramInSession, CJson *out, int32_t *status)
{
    int32_t moduleType = GetAuthModuleType(paramInSession);
    const char *pkgName = GetStringFromJson(paramInSession, FIELD_SERVICE_PKG_NAME);
    if (pkgName == NULL) {
        LOGE("Pkg name is null!");
        return HC_ERR_NULL_PTR;
    }
    if (AddStringToJson(paramInSession, FIELD_PKG_NAME, pkgName) != HC_SUCCESS) {
        LOGE("Failed to add pkg name to json!");
        return HC_ERR_JSON_FAIL;
    }
    session->curTaskId = 0;
    int32_t res = CreateTask(&(session->curTaskId), paramInSession, out, moduleType);
    if (res != HC_SUCCESS) {
        LOGE("Failed to create task for auth!");
        return res;
    }
    res = ProcessTask(session->curTaskId, paramInSession, out, status, moduleType);
    DeleteCachedData(paramInSession);
    if (res != HC_SUCCESS) {
        DestroyTask(session->curTaskId, GetAuthModuleType(paramInSession));
        LOGE("Failed to process task for auth!");
        return res;
    }
    return HC_SUCCESS;
}

void InformLocalAuthError(const CJson *authParam, const DeviceAuthCallback *callback)
{
    bool isClient = false;
    (void)GetBoolFromJson(authParam, FIELD_IS_CLIENT, &isClient);
    const char *altGroup = GetStringFromJson(authParam, FIELD_ALTERNATIVE);
    if (!isClient && (altGroup != NULL)) { /* Server wait for next group auth. */
        LOGI("Server wait for next group auth.");
        return;
    }
    int64_t requestId = 0;
    if (GetByteFromJson(authParam, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return;
    }
    if ((callback != NULL) && (callback->onError != NULL)) {
        LOGE("Invoke InformLocalAuthError!");
        callback->onError(requestId, AUTH_FORM_INVALID_TYPE, HC_ERR_CREATE_SESSION_FAIL, NULL);
    }
}

void InformPeerAuthError(const CJson *authParam, const DeviceAuthCallback *callback)
{
    if (ReturnErrorToPeerBySession(authParam, callback) != HC_SUCCESS) {
        LOGE("Failed to return error to peer by session!");
    }
}

void DestroyAuthSession(Session *session)
{
    if (session == NULL) {
        return;
    }
    AuthSession *realSession = (AuthSession *)session;
    CJson *paramInSession = (realSession->paramsList).get(&(realSession->paramsList), realSession->currentIndex);
    if (paramInSession == NULL) {
        LOGE("The json data in session is null!");
        return;
    }
    DestroyTask(realSession->curTaskId, GetAuthModuleType(paramInSession));

    uint32_t index;
    void **paramsData = NULL;
    FOR_EACH_HC_VECTOR(realSession->paramsList, index, paramsData) {
        if (paramsData != NULL) {
            FreeJson((CJson *)*paramsData);
        }
    }
    DESTROY_HC_VECTOR(ParamsVec, &(realSession->paramsList));
    HcFree(realSession);
    realSession = NULL;
}

void CreateAuthParamsVec(ParamsVec *vec)
{
    *vec = CREATE_HC_VECTOR(ParamsVec);
}

void DestroyAuthParamsVec(ParamsVec *vec)
{
    DESTROY_HC_VECTOR(ParamsVec, vec);
}
