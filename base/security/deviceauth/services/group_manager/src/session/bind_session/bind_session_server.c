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

#include "bind_session_server.h"
#include "bind_session_common_util.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "das_module_defines.h"
#include "group_operation_common.h"
#include "hc_log.h"
#include "os_account_adapter.h"
#include "session_manager.h"

static int32_t AddRecvModuleDataToParams(CJson *jsonParams, CJson *moduleParams)
{
    int32_t message = ERR_MESSAGE;
    if (GetIntFromJson(jsonParams, FIELD_MESSAGE, &message) != HC_SUCCESS) {
        LOGE("Failed to get message from in!");
        return HC_ERR_JSON_GET;
    }
    int32_t authForm = ERR_AUTH_FORM;
    (void)GetIntFromJson(jsonParams, FIELD_AUTH_FORM, &authForm);
    CJson *payload = GetObjFromJson(jsonParams, FIELD_PAYLOAD);
    if (payload == NULL) {
        LOGE("Failed to get payload from in!");
        return HC_ERR_JSON_GET;
    }
    if (AddIntToJson(moduleParams, FIELD_MESSAGE, message) != HC_SUCCESS) {
        LOGE("Failed to add message to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(moduleParams, FIELD_AUTH_FORM, authForm) != HC_SUCCESS) {
        LOGE("Failed to add authForm to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddObjToJson(moduleParams, FIELD_PAYLOAD, payload) != HC_SUCCESS) {
        LOGE("Failed to add payload to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t AddConfirmationToParams(CJson *moduleParams)
{
    if (AddIntToJson(moduleParams, FIELD_CONFIRMATION, REQUEST_ACCEPTED) != HC_SUCCESS) {
        LOGE("Failed to add confirmation to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t GenerateServerModuleParams(BindSession *session, CJson *jsonParams, CJson *moduleParams)
{
    int32_t result;
    if (((result = GenerateBasicModuleParams(false, session, moduleParams)) != HC_SUCCESS) ||
        ((result = AddConfirmationToParams(moduleParams)) != HC_SUCCESS) ||
        ((result = AddRecvModuleDataToParams(jsonParams, moduleParams)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t GetServerModuleReturnData(BindSession *session, CJson *jsonParams, CJson *out, bool *isNeedInform)
{
    CJson *moduleParams = CreateJson();
    if (moduleParams == NULL) {
        LOGE("Failed to allocate moduleParams memory!");
        return HC_ERR_JSON_FAIL;
    }

    int32_t result = GenerateServerModuleParams(session, jsonParams, moduleParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate all params sent to the module!");
        FreeJson(moduleParams);
        return result;
    }
    /* Release the memory in advance to reduce the memory usage. */
    DeleteAllItem(jsonParams);

    result = CreateAndProcessModule(session, moduleParams, out);
    FreeJson(moduleParams);
    if (result != HC_SUCCESS) {
        *isNeedInform = false;
        InformPeerModuleError(out, session);
        return result;
    }
    return HC_SUCCESS;
}

static int32_t PrepareData(BindSession *session, CJson *jsonParams, CJson **sendData, bool *isNeedInform)
{
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to allocate out memory!");
        return HC_ERR_JSON_FAIL;
    }

    int32_t result = GetServerModuleReturnData(session, jsonParams, out, isNeedInform);
    if (result != HC_SUCCESS) {
        FreeJson(out);
        return result;
    }

    *sendData = DetachItemFromJson(out, FIELD_SEND_TO_PEER);
    FreeJson(out);
    if (*sendData == NULL) {
        LOGE("Failed to get sendToPeer from out!");
        return HC_ERR_JSON_GET;
    }

    result = AddInfoToSendData((session->opCode == MEMBER_JOIN), session, *sendData);
    if (result != HC_SUCCESS) {
        LOGE("Failed to add information to sendData!");
        FreeJson(*sendData);
        *sendData = NULL;
        return result;
    }
    return HC_SUCCESS;
}

static int32_t PrepareAndSendDataServer(BindSession *session, CJson *jsonParams, bool *isNeedInform)
{
    CJson *sendData = NULL;
    int32_t result = PrepareData(session, jsonParams, &sendData, isNeedInform);
    if (result != HC_SUCCESS) {
        return result;
    }

    result = SendBindSessionData(session, sendData);
    FreeJson(sendData);
    return result;
}

static int32_t GenerateRequestParams(const CJson *jsonParams, CJson *requestParams)
{
    const char *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
    if (groupId == NULL) {
        LOGE("Failed to get groupId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *peerAuthId = GetStringFromJson(jsonParams, FIELD_PEER_DEVICE_ID);
    if (peerAuthId == NULL) {
        LOGE("Failed to get peerAuthId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t groupType = PEER_TO_PEER_GROUP;
    if (GetIntFromJson(jsonParams, FIELD_GROUP_TYPE, &groupType) != HC_SUCCESS) {
        LOGE("Failed to get groupType from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(requestParams, FIELD_GROUP_ID, groupId) != HC_SUCCESS) {
        LOGE("Failed to add groupId to requestParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddIntToJson(requestParams, FIELD_GROUP_TYPE, groupType) != HC_SUCCESS) {
        LOGE("Failed to add groupType to requestParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(requestParams, FIELD_PEER_DEVICE_ID, peerAuthId) != HC_SUCCESS) {
        LOGE("Failed to add peerDeviceId to requestParams!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(requestParams, FIELD_APP_ID, appId) != HC_SUCCESS) {
        LOGE("Failed to add appId to requestParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t RequestConfirmation(const CJson *jsonParams, const BindSession *session, char **returnStr)
{
    CJson *requestParams = CreateJson();
    if (requestParams == NULL) {
        LOGE("Failed to create json object!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t result = GenerateRequestParams(jsonParams, requestParams);
    if (result != HC_SUCCESS) {
        LOGE("An error occurs when the request parameters are generated!");
        FreeJson(requestParams);
        return result;
    }
    char *requestParamsStr = PackJsonToString(requestParams);
    FreeJson(requestParams);
    if (requestParamsStr == NULL) {
        LOGE("An error occurred when converting JSON data to String data.!");
        return HC_ERR_JSON_FAIL;
    }
    char *returnDataStr = ProcessRequestCallback(session->reqId, session->opCode,
        requestParamsStr, session->base.callback);
    FreeJsonString(requestParamsStr);
    if (returnDataStr == NULL) {
        LOGE("The OnRequest callback is fail!");
        return HC_ERR_REQ_REJECTED;
    }
    *returnStr = returnDataStr;
    return HC_SUCCESS;
}

static int32_t AddAuthIdIfExist(const CJson *returnData, CJson *jsonParams)
{
    const char *authId = GetStringFromJson(returnData, FIELD_DEVICE_ID);
    if (authId != NULL) {
        if (AddStringToJson(jsonParams, FIELD_DEVICE_ID, authId) != HC_SUCCESS) {
            LOGE("Failed to add authId to jsonParams!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddUserTypeIfExistAndValid(const CJson *returnData, CJson *jsonParams)
{
    int32_t userType = DEVICE_TYPE_ACCESSORY;
    if (GetIntFromJson(returnData, FIELD_USER_TYPE, &userType) == HC_SUCCESS) {
        if (!IsUserTypeValid(userType)) {
            LOGE("The input userType is invalid!");
            return HC_ERR_INVALID_PARAMS;
        }
        if (AddIntToJson(jsonParams, FIELD_USER_TYPE, userType) != HC_SUCCESS) {
            LOGE("Failed to add userType to jsonParams!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddGroupVisibilityIfExistAndValid(const CJson *returnData, CJson *jsonParams)
{
    int32_t groupVisibility = GROUP_VISIBILITY_PUBLIC;
    if (GetIntFromJson(returnData, FIELD_GROUP_VISIBILITY, &groupVisibility) == HC_SUCCESS) {
        if (!IsGroupVisibilityValid(groupVisibility)) {
            LOGE("The input groupVisibility invalid!");
            return HC_ERR_INVALID_PARAMS;
        }
        if (AddIntToJson(jsonParams, FIELD_GROUP_VISIBILITY, groupVisibility) != HC_SUCCESS) {
            LOGE("Failed to add groupVisibility to jsonParams!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddExpireTimeIfExistAndValid(const CJson *returnData, CJson *jsonParams)
{
    int32_t expireTime = DEFAULT_EXPIRE_TIME;
    if (GetIntFromJson(returnData, FIELD_EXPIRE_TIME, &expireTime) == HC_SUCCESS) {
        if (!IsExpireTimeValid(expireTime)) {
            LOGE("The input expireTime invalid!");
            return HC_ERR_INVALID_PARAMS;
        }
        if (AddIntToJson(jsonParams, FIELD_EXPIRE_TIME, expireTime) != HC_SUCCESS) {
            LOGE("Failed to add expireTime to jsonParams!");
            return HC_ERR_JSON_FAIL;
        }
    }
    return HC_SUCCESS;
}

static int32_t AddPinCode(const CJson *returnData, CJson *jsonParams)
{
    const char *pinCode = GetStringFromJson(returnData, FIELD_PIN_CODE);
    if (pinCode == NULL) {
        LOGE("Failed to get pinCode from returnData!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(jsonParams, FIELD_PIN_CODE, pinCode) != HC_SUCCESS) {
        LOGE("Failed to add pinCode to jsonParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t CombineInfoWhenInvite(const CJson *returnData, CJson *jsonParams)
{
    int32_t result;
    if (((result = AddPinCode(returnData, jsonParams)) != HC_SUCCESS) ||
        ((result = AddAuthIdIfExist(returnData, jsonParams)) != HC_SUCCESS) ||
        ((result = AddUserTypeIfExistAndValid(returnData, jsonParams)) != HC_SUCCESS) ||
        ((result = AddGroupVisibilityIfExistAndValid(returnData, jsonParams)) != HC_SUCCESS) ||
        ((result = AddExpireTimeIfExistAndValid(returnData, jsonParams)) != HC_SUCCESS)) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t CombineInputData(int operationCode, const CJson *returnData, CJson *jsonParams)
{
    if (operationCode == MEMBER_DELETE) {
        return HC_SUCCESS;
    } else if (operationCode == MEMBER_JOIN) {
        return AddPinCode(returnData, jsonParams);
    } else {
        return CombineInfoWhenInvite(returnData, jsonParams);
    }
}

static int32_t CheckServerStatusIfNotInvite(int32_t osAccountId, int operationCode, const CJson *jsonParams)
{
    if (operationCode == MEMBER_INVITE) {
        return HC_SUCCESS;
    }
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
    const char *peerUdid = GetStringFromJson(jsonParams, FIELD_CONN_DEVICE_ID);
    if (peerUdid == NULL) {
        LOGE("Failed to get peerUdid from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    int32_t result = CheckGroupExist(osAccountId, groupId);
    if (result != HC_SUCCESS) {
        return result;
    }
    if (operationCode == MEMBER_JOIN) {
        /* The client sends a join request, which is equivalent to the server performing an invitation operation. */
        result = CheckPermForGroup(osAccountId, MEMBER_INVITE, appId, groupId);
        if (result != HC_SUCCESS) {
            return result;
        }
        result = CheckDeviceNumLimit(osAccountId, groupId, peerUdid);
    } else if (operationCode == MEMBER_DELETE) {
        result = CheckPermForGroup(osAccountId, MEMBER_DELETE, appId, groupId);
        if (result != HC_SUCCESS) {
            return result;
        }
        if (!IsTrustedDeviceInGroup(osAccountId, groupId, peerUdid, true)) {
            result = HC_ERR_DEVICE_NOT_EXIST;
        }
    }
    return result;
}

static int32_t PrepareServer(BindSession *session, CJson *returnData, bool *isNeedInform)
{
    if ((session->isWaiting) && (!IsAcceptRequest(returnData))) {
        LOGE("The service rejects the request!");
        return HC_ERR_REQ_REJECTED;
    }
    CJson *jsonParams = DetachItemFromJson(session->params, FIELD_RECEIVED_DATA);
    if (jsonParams == NULL) {
        LOGE("Received data before request confirmation are lost!");
        return HC_ERR_LOST_DATA;
    }
    int32_t osAccountId = ANY_OS_ACCOUNT;
    (void)GetIntFromJson(returnData, FIELD_OS_ACCOUNT_ID, &osAccountId);
    osAccountId = DevAuthGetRealOsAccountLocalId(osAccountId);
    if (osAccountId == INVALID_OS_ACCOUNT) {
        FreeJson(jsonParams);
        return HC_ERR_INVALID_PARAMS;
    }
    int32_t result = CheckServerStatusIfNotInvite(osAccountId, session->opCode, jsonParams);
    if (result != HC_SUCCESS) {
        FreeJson(jsonParams);
        return result;
    }
    session->osAccountId = osAccountId;
    result = CombineInputData(session->opCode, returnData, jsonParams);
    /* Release the memory in advance to reduce the memory usage. */
    DeleteAllItem(returnData);
    session->isWaiting = false;
    if (result != HC_SUCCESS) {
        FreeJson(jsonParams);
        return result;
    }
    result = GenerateBindParams(osAccountId, SERVER, jsonParams, session);
    if (result != HC_SUCCESS) {
        FreeJson(jsonParams);
        return result;
    }
    /*
     * If the service is invited to join the peer group,
     * the identity key pair of the corresponding group needs to be generated here.
     */
    if (NeedCreateGroup(SERVER, session->opCode)) {
        const char *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
        if (groupId == NULL) {
            LOGE("Failed to get groupId from jsonParams!");
            FreeJson(jsonParams);
            return HC_ERR_JSON_GET;
        }
        result = ProcessKeyPair(CREATE_KEY_PAIR, jsonParams, groupId);
        if (result != HC_SUCCESS) {
            FreeJson(jsonParams);
            return result;
        }
    }
    result = PrepareAndSendDataServer(session, jsonParams, isNeedInform);
    FreeJson(jsonParams);
    return result;
}

static void OnBindConfirmationReceived(Session *session, CJson *returnData)
{
    if ((session == NULL) || (returnData == NULL)) {
        LOGE("The input session or returnData is NULL!");
        return;
    }
    BindSession *realSession = (BindSession *)session;

    bool isNeedInform = true;
    int32_t result = PrepareServer(realSession, returnData, &isNeedInform);
    if (result != HC_SUCCESS) {
        InformPeerGroupErrorIfNeed(isNeedInform, result, realSession);
        ProcessErrorCallback(realSession->reqId, realSession->opCode, result, NULL, realSession->base.callback);
        CloseChannel(realSession->channelType, realSession->channelId);
        DestroySession(realSession->reqId);
    }
}

static int32_t BindSaveReceivedData(BindSession *session, const CJson *jsonParams)
{
    if (session->params == NULL) {
        session->params = CreateJson();
        if (session->params == NULL) {
            LOGE("Failed to allocate session params memory!");
            return HC_ERR_ALLOC_MEMORY;
        }
    }
    if (AddObjToJson(session->params, FIELD_RECEIVED_DATA, jsonParams) != HC_SUCCESS) {
        LOGE("Failed to add received data to session params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t JudgeConfirmation(CJson *returnData, CJson *jsonParams, BindSession *session, bool *isNeedInform)
{
    uint32_t confirmation = REQUEST_ACCEPTED;
    if (GetUnsignedIntFromJson(returnData, FIELD_CONFIRMATION, &confirmation) != HC_SUCCESS) {
        LOGE("Failed to get confirmation from returnData!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t result;
    switch (confirmation) {
        case REQUEST_WAITING:
            LOGI("The service wants us to wait for its signal!");
            result = BindSaveReceivedData(session, jsonParams);
            session->isWaiting = true;
            return result;
        case REQUEST_ACCEPTED:
            LOGI("The service accepts the request!");
            result = BindSaveReceivedData(session, jsonParams);
            if (result != HC_SUCCESS) {
                return result;
            }
            /* Release the memory in advance to reduce the memory usage. */
            DeleteAllItem(jsonParams);
            return PrepareServer(session, returnData, isNeedInform);
        case REQUEST_REJECTED:
            LOGE("The service rejects the request!");
            return HC_ERR_REQ_REJECTED;
        default:
            LOGE("Enter the exception case!");
            return HC_ERR_CASE;
    }
}

static int32_t HandleRequest(CJson *jsonParams, BindSession *session, bool *isNeedInform)
{
    char *returnDataStr = NULL;
    int32_t result = RequestConfirmation(jsonParams, session, &returnDataStr);
    if (result != HC_SUCCESS) {
        return result;
    }

    CJson *returnData = CreateJsonFromString(returnDataStr);
    FreeJsonString(returnDataStr);
    if (returnData == NULL) {
        LOGE("Failed to create returnData from string!");
        return HC_ERR_JSON_FAIL;
    }
    result = JudgeConfirmation(returnData, jsonParams, session, isNeedInform);
    FreeJson(returnData);
    return result;
}

Session *CreateServerBindSession(CJson *jsonParams, const DeviceAuthCallback *callback)
{
    int32_t opCode = MEMBER_INVITE;
    if (GetIntFromJson(jsonParams, FIELD_GROUP_OP, &opCode) != HC_SUCCESS) {
        LOGE("Failed to get opCode from jsonParams!");
        return NULL;
    }

    BindSession *session = CreateBaseBindSession(TYPE_SERVER_BIND_SESSION, opCode,
        jsonParams, callback, ProcessBindSession);
    if (session == NULL) {
        return NULL;
    }
    InitServerChannel(jsonParams, session);
    /* The server may receive the confirm request message. */
    session->onConfirmed = OnBindConfirmationReceived;

    bool isNeedInform = true;
    int32_t result = HandleRequest(jsonParams, session, &isNeedInform);
    if (result != HC_SUCCESS) {
        InformPeerGroupErrorIfNeed(isNeedInform, result, session);
        ProcessErrorCallback(session->reqId, session->opCode, result, NULL, session->base.callback);
        CloseChannel(session->channelType, session->channelId);
        DestroyBindSession((Session *)session);
        return NULL;
    }
    return (Session *)session;
}
