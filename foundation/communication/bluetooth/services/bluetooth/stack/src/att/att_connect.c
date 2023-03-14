/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file att_connect.c
 *
 * @brief implement connect function to be called.
 *
 */

#include "att_connect.h"

#include <string.h>

#include "log.h"

#include "platform/include/allocator.h"

typedef struct AttDisconnectReqAsync {
    uint16_t connectHandle;
} AttDisconnectReqAsync;

typedef struct AttConnectRspAsync {
    uint16_t connectHandle;
    uint16_t result;
    uint16_t status;
    AttConnect *g_attConnectParaObj;
} AttConnectRspAsync;

typedef struct AttConnectReqAsync {
    uint8_t transportType;
    AttConnect *connParaPtr;
    BtAddr *btAddress;
    uint16_t *bredrconnectHandle;
} AttConnectReqAsync;

typedef struct AttLeConnectedAsyncContext {
    BtAddr *addr;
    uint16_t aclHandle;
    uint8_t role;
    uint8_t status;
} AttLeConnectedAsyncContext;

typedef struct AttLeDisconnectedAsyncContext {
    uint16_t aclHandle;
    uint8_t status;
    uint8_t reason;
} AttLeDisconnectedAsyncContext;

typedef struct DisconnectRespCallbackContext {
    uint16_t lcid;
    int result;
} DisconnectRespCallbackContext;

typedef struct ConnectCallBackContext {
    BtAddr *addr;
    int result;
} ConnectCallBackContext;

typedef struct ConnectReqCallbackContext {
    BtAddr *addr;
    uint16_t lcid;
    int result;
} ConnectReqCallbackContext;

typedef struct ConfigReqCallbackContext {
    uint16_t lcid;
    int result;
} ConfigReqCallbackContext;

typedef struct ConnectRspResultCallbackContext {
    uint16_t lcid;
    int result;
} ConnectRspResultCallbackContext;

typedef struct BREDRDisconnectReqCallBackContext {
    uint16_t lcid;
    int result;
    void *context;
} BREDRDisconnectReqCallBackContext;

typedef struct LeDisconnectReqCallBackContext {
    uint16_t aclHandle;
    int result;
} LeDisconnectReqCallBackContext;

typedef struct ConnectRspPendingContext {
    uint16_t lcid;
    int result;
} ConnectRspPendingContext;

typedef struct AttReceiveConnectReqContext {
    uint16_t lcid;
    uint8_t id;
    L2capConnectionInfo *info;
    uint16_t lpsm;
    void *ctx;
} AttReceiveConnectReqContext;

typedef struct AttReceiveConnectionRspContext {
    uint16_t lcid;
    L2capConnectionInfo *info;
    uint16_t result;
    uint16_t status;
    void *ctx;
} AttReceiveConnectionRspContext;

typedef struct AttReceiveConfigRspContext {
    uint16_t lcid;
    L2capConfigInfo *info;
    uint16_t result;
    void *ctx;
} AttReceiveConfigRspContext;

typedef struct AttReceiveDisconnectReqContext {
    uint16_t lcid;
    uint8_t id;
    void *ctx;
} AttReceiveDisconnectReqContext;

typedef struct AttDisconnectAbnormalContext {
    uint16_t lcid;
    uint8_t reason;
    void *ctx;
} AttDisconnectAbnormalContext;

typedef struct AttRecvDisconnectionRspContext {
    uint16_t lcid;
    void *ctx;
} AttRecvDisconnectionRspContext;

typedef struct ConfigRspCallbackContext {
    uint16_t lcid;
    int result;
} ConfigRspCallbackContext;

typedef struct AttReceiveConfigReqContext {
    uint16_t lcid;
    uint8_t id;
    L2capConfigInfo *cfg;
    void *ctx;
} AttReceiveConfigReqContext;

typedef struct ConnectTimeOutContext {
    uint16_t connectHandle;
} ConnectTimeOutContext;

typedef struct ConnectCancelContext {
    BtAddr addr;
} ConnectCancelContext;

typedef struct GAPSecCallbackContext {
    uint16_t result;
    GapServiceSecurityInfo serviceInfo;
    uint16_t connectHandle;
} GAPSecCallbackContext;

static void AttRecvConfigReqAssignBredrConnect(const L2capConfigInfo *cfg, AttConnectingInfo *connecting);
static void AttClearConnectingInfo(AttConnectingInfo *connecting);
static void AttClearConnectInfo(AttConnectInfo *connect);
static int AttBredrConnectStatusChange(uint8_t inputStatus, uint8_t *outStatus, uint8_t flag);
static void AttCopyToConnectingInfo(
    const BtAddr *addr, uint16_t aclHandle, uint16_t lcid, uint8_t id, AttConnectingInfo *connecting);

static void AttAssignGAPRequestSecurity(
    GapRequestSecurityParam *gapSecurity, uint8_t direction, const AttConnectingInfo *connecting);

static void AttConnectReqAssignBredrConnect(AttConnectingInfo **connecting, const BtAddr *btAddress,
    const AttConnect *connParaPtr, uint16_t *bredrconnectHandle);
static void AttConnectRequestLEParaAssign(
    L2capLeConnectionParameter *l2capObjptr, const AttConnectReqAsync *attConnectReqAsyncPtr);

static void GAPSecCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, const void *context);

static int AttBredrConnectProcess(AttConnectingInfo *connecting);
static void AttBredrConnectTimeOut(void *parameter);
static void AttConnectCompletedCallback(AttConnectingInfo *connecting, uint16_t result);
static void AttCopyToConnectInfo(const AttConnectingInfo *connecting, AttConnectInfo **connect);
static void AttConnectInfoAddLe(
    const BtAddr *addr, uint16_t aclHandle, AttConnectInfo **connect, uint8_t initPassConnFlag);

static void L2cifConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, const void *context);
static void L2cifConnectRspPendingCallback(uint16_t lcid, int result);
static void L2cifConnectRspResultCallback(uint16_t lcid, int result);
static void L2cifConfigReqCallback(uint16_t lcid, int result);
static void L2cifConfigRspCallback(uint16_t lcid, int result);

static int AttLeConnectProcess(const BtAddr *addr, L2capLeConnectionParameter var, const AttConnectingInfo *connecting);
static void DisconnectRespCallback(uint16_t lcid, int result);

static void AttConnectRequestAsync(const void *context);
static void AttConnectRequestAsyncDestroy(const void *context);
static void AttConnectRsponseAsync(const void *context);
static void AttConnectRsponseAsyncDestroy(const void *context);
static void AttDisconnectRequestAsync(const void *context);
static void AttDisconnectRequestAsyncDestroy(const void *context);
static void AttLeConnectedAsync(const void *context);
static void AttLeConnectedAsyncDestroy(const void *context);
static void AttLeDisconnectedAsync(const void *context);
static void AttLeDisconnectedAsyncDestroy(const void *context);
static void DisconnectRespCallbackAsync(const void *context);
static void DisconnectRespCallbackAsyncDestroy(const void *context);
static void L2cifLeConnectCallBackAsync(const void *context);
static void L2cifLeConnectCallBackAsyncDestroy(const void *context);
static void L2cifConnectReqCallbackAsync(const void *context);
static void L2cifConnectReqCallbackAsyncDestroy(const void *context);
static void L2cifConfigReqCallbackAsync(const void *context);
static void L2cifConfigReqCallbackAsyncDestroy(const void *context);
static void L2cifConnectRspResultCallbackAsync(const void *context);
static void L2cifConnectRspResultCallbackAsyncDestroy(const void *context);
static void L2cifConnectRspPendingCallbackAsync(const void *context);
static void L2cifConnectRspPendingCallbackAsyncDestroy(const void *context);
static void AttReceiveConnectionReqAsync(const void *context);
static void AttReceiveConnectionReqAsyncDestroy(const void *context);
static void AttReceiveConnectionRspAsync(const void *context);
static void AttReceiveConnectionRspAsyncDestroy(const void *context);
static void AttReceiveConfigRspAsync(const void *context);
static void AttReceiveConfigRspAsyncDestroy(const void *context);
static void AttReceiveDisconnectionReqAsync(const void *context);
static void AttReceiveDisconnectionReqAsyncDestroy(const void *context);
static void AttDisconnectAbnormalAsync(const void *context);
static void AttDisconnectAbnormalAsyncDestroy(const void *context);
static void AttRecvDisconnectionRspAsync(const void *context);
static void AttRecvDisconnectionRspAsyncDestroy(const void *context);
static void L2cifConfigRspCallbackAsync(const void *context);
static void L2cifConfigRspCallbackAsyncDestroy(const void *context);
static void AttReceiveConfigReqAsync(const void *context);
static void AttReceiveConfigReqAsyncDestroy(const void *context);
static void AttBredrConnectTimeOutAsync(const void *context);
static void AttBredrConnectTimeOutAsyncDestroy(const void *context);
static void GAPSecCallbackAsync(const void *context);
static void GAPSecCallbackAsyncDestroy(const void *context);

/**
 * @brief disconnect response call back async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void DisconnectRespCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    DisconnectRespCallbackContext *disConnectRspPtr = (DisconnectRespCallbackContext *)context;
    AttBredrDisconnectCallback bredrDisConnObj;
    int listSize;
    AttConnectedCallback *attConnectCallback = NULL;
    AttConnectInfo *connect = NULL;

    AttGetConnectInfoIndexByCid(disConnectRspPtr->lcid, &connect);

    if (connect == NULL) {
        goto DISCONNECTRESPCALLBACK_END;
    }

    AlarmCancel(connect->alarm);

    attConnectCallback = AttGetATTConnectCallback();
    if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attBREDRDisconnectCompleted == NULL)) {
        LOG_WARN("%{public}s attConnectCallback or attBREDRDisconnectCompleted is NULL", __FUNCTION__);
    } else {
        if (disConnectRspPtr->result == BT_NO_ERROR) {
            bredrDisConnObj.reason = PASSIVECONNECT_DISCONNECT_SUCCESS;
        } else {
            bredrDisConnObj.reason = PASSIVECONNECT_DISCONNECT_FAIL;
        }
        attConnectCallback->attConnect.attBREDRDisconnectCompleted(
            connect->retGattConnectHandle, &bredrDisConnObj, attConnectCallback->context);
    }

    listSize = ListGetSize(connect->instruct);
    for (; listSize > 0; --listSize) {
        ListRemoveLast(connect->instruct);
    }
    AttClearConnectInfo(connect);

DISCONNECTRESPCALLBACK_END:
    MEM_MALLOC.free(disConnectRspPtr);
    return;
}

/**
 * @brief disconnect response call back async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void DisconnectRespCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    DisconnectRespCallbackContext *disConnectRspPtr = (DisconnectRespCallbackContext *)context;

    MEM_MALLOC.free(disConnectRspPtr);

    return;
}

/**
 * @brief disconnect response call back.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 result Indicates the result.
 */
static void DisconnectRespCallback(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    DisconnectRespCallbackContext *disConnectRspPtr = MEM_MALLOC.alloc(sizeof(DisconnectRespCallbackContext));
    if (disConnectRspPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    disConnectRspPtr->lcid = lcid;
    disConnectRspPtr->result = result;

    AttAsyncProcess(DisconnectRespCallbackAsync, DisconnectRespCallbackAsyncDestroy, disConnectRspPtr);

    return;
}

/**
 * @brief att assign bredr connect receive config request parameter.
 *
 * @param1 cfg Indicates the pointer to const L2capConfigInfo.
 * @param2 connecting Indicates the pointer to AttConnectingInfo.
 */
static void AttRecvConfigReqAssignBredrConnect(const L2capConfigInfo *cfg, AttConnectingInfo *connecting)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    connecting->remotel2capConfigInfoObj.mtu = cfg->mtu;
    connecting->remotel2capConfigInfoObj.flushTimeout = cfg->flushTimeout;
    connecting->remotel2capConfigInfoObj.rfc.mode = cfg->rfc.mode;
    connecting->mtu = Min(connecting->locall2capConfigInfoObj.mtu, connecting->remotel2capConfigInfoObj.mtu);

    return;
}

/**
 * @brief att clear connecting information.
 *
 * @param connecting Indicates the pointer to AttConnectingInfo.
 */
static void AttClearConnectingInfo(AttConnectingInfo *connecting)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    connecting->aclHandle = 0;
    connecting->cid = 0;
    connecting->id = 0;
    connecting->mtu = 0;
    connecting->connectHandle = 0;
    connecting->initiativeConnectStatus = 0;
    connecting->passiveConnectSatatus = 0;
    connecting->initPassConnFlag = 0;
    (void)memset_s(
        &connecting->locall2capConfigInfoObj, sizeof(connecting->locall2capConfigInfoObj), 0, sizeof(L2capConfigInfo));
    (void)memset_s(&connecting->remotel2capConfigInfoObj,
        sizeof(connecting->remotel2capConfigInfoObj),
        0,
        sizeof(L2capConfigInfo));
    (void)memset_s(&connecting->addr, sizeof(connecting->addr), 0, sizeof(BtAddr));

    if (connecting->transportType == BT_TRANSPORT_BR_EDR) {
        if (connecting->bredrAlarm != NULL) {
            AlarmCancel(connecting->bredrAlarm);
        }
    }

    return;
}

/**
 * @brief att clear connected information.
 *
 * @param connect Indicates the pointer to AttConnectInfo.
 */
static void AttClearConnectInfo(AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    connect->aclHandle = 0;
    connect->AttConnectID.bredrcid = 0;
    connect->AttConnectID.lecid = 0;
    connect->retGattConnectHandle = 0;
    connect->transportType = 0;
    connect->mtu = 0;
    connect->mtuFlag = false;
    connect->initPassConnFlag = 0;
    connect->sendMtu = 0;
    connect->receiveMtu = 0;
    connect->serverSendFlag = false;
    (void)memset_s(&connect->addr, sizeof(connect->addr), 0, sizeof(BtAddr));

    if (connect->alarm != NULL) {
        AlarmCancel(connect->alarm);
    }

    return;
}

/**
 * @brief att bredr connect statuc change.
 *
 * @param1 inputStatus Indicates the input parameter.
 * @param2 outStatus Indicates the pointer to output parameter.
 * @param3 flag Indicates the flag of connect mode.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
static int AttBredrConnectStatusChange(uint8_t inputStatus, uint8_t *outStatus, uint8_t flag)
{
    LOG_INFO("%{public}s enter,inputStatus = %hhu,flag=%hhu", __FUNCTION__, inputStatus, flag);

    int ret = BT_OPERATION_FAILED;

    if (flag == INITIATIVECONNECT) {
        if (inputStatus == DISCONNECTED) {
            ret = BT_OPERATION_FAILED;
            goto ATTBREDRCONNECTSTATUSCHANGE_END;
        }
        if (inputStatus == CONNECTING) {
            *outStatus = CONFIGED;
        } else if (inputStatus == CONFIGED) {
            *outStatus = CONNECTED;
        }
    } else if (flag == PASSIVECONNECT) {
        if (inputStatus == DISCONNECTED) {
            ret = BT_OPERATION_FAILED;
            goto ATTBREDRCONNECTSTATUSCHANGE_END;
        }
        if (inputStatus == CONNECTIND) {
            *outStatus = CONFIGED;
        } else if (inputStatus == CONFIGED) {
            *outStatus = CONNECTED;
        }
    }

    ret = BT_NO_ERROR;

ATTBREDRCONNECTSTATUSCHANGE_END:
    return ret;
}

/**
 * @brief att connecting information storage.
 *
 * @param1 addr Indicates the pointer to BtAddr.
 * @param2 aclHandle Indicates the aclHandle.
 * @param3 lcid Indicates the lcid.
 * @param4 id Indicates the id.
 * @param5 connecting Indicates the pointer to AttConnectingInfo.
 */
static void AttCopyToConnectingInfo(
    const BtAddr *addr, uint16_t aclHandle, uint16_t lcid, uint8_t id, AttConnectingInfo *connecting)
{
    LOG_INFO("%{public}s enter,aclHandle=%hu,lcid=%hu,id=%hhu", __FUNCTION__, aclHandle, lcid, id);

    uint16_t index = 0;

    AttGetConnectingIndexByCidOutIndex(0, &index, &connecting);

    if (connecting != NULL) {
        connecting->cid = lcid;
        (void)memcpy_s(connecting->addr.addr, ADDRESSLEN, addr->addr, ADDRESSLEN);
        connecting->addr.type = addr->type;
        connecting->connectHandle = index + 1;
        connecting->initPassConnFlag = PASSIVECONNECT;
        connecting->passiveConnectSatatus = DISCONNECTED;
        connecting->id = id;
        connecting->aclHandle = aclHandle;
        connecting->mtu = DEFAULTBREDRMTU;
    }

    return;
}

/**
 * @brief gap security callback async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void GAPSecCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    GAPSecCallbackContext *gapSecCallbackPtr = (GAPSecCallbackContext *)context;
    AttConnectingInfo *connecting = NULL;
    AttConnectedCallback *attConnectCallback = NULL;
    AttBredrConnectCallback bredrConnectCallback;

    AttGetConnectingIndexByConnectHandle(gapSecCallbackPtr->connectHandle, &connecting);
    attConnectCallback = AttGetATTConnectCallback();

    if (gapSecCallbackPtr->result != 0) {
        LOG_INFO("%{public}s gap security authen callback error", __FUNCTION__);
        if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attBREDRConnectCompleted == NULL)) {
            LOG_WARN("%{public}s attConnectCallback or attBREDRConnectCompleted is NULL", __FUNCTION__);
            goto GAPSECCALLBACK_END;
        }
        bredrConnectCallback.addr.type = connecting->addr.type;
        (void)memcpy_s(bredrConnectCallback.addr.addr, ADDRESSLEN, connecting->addr.addr, ADDRESSLEN);
        bredrConnectCallback.status = SECURITY_AUTHENTICATION_FAIL;
        bredrConnectCallback.mtu = DEFAULTBREDRMTU;
        attConnectCallback->attConnect.attBREDRConnectCompleted(
            connecting->connectHandle, &bredrConnectCallback, attConnectCallback->context);
        AttClearConnectingInfo(connecting);
        goto GAPSECCALLBACK_END;
    }

    if (connecting->initPassConnFlag == INITIATIVECONNECT) {
        AttBredrConnectProcess(connecting);
    } else if (connecting->initPassConnFlag == PASSIVECONNECT) {
        if (connecting->passiveConnectSatatus == DISCONNECTED) {
            connecting->passiveConnectSatatus = CONNECTIND;
        }
        if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attBREDRConnectInd == NULL)) {
            LOG_WARN(
                "%{public}s attConnectCallback or attConnectCallback->attConnect.attBREDRConnectInd is NULL", __FUNCTION__);
            goto GAPSECCALLBACK_END;
        }

        attConnectCallback->attConnect.attBREDRConnectInd(connecting->connectHandle, attConnectCallback->context);
    }

GAPSECCALLBACK_END:
    MEM_MALLOC.free(gapSecCallbackPtr);
    return;
}

/**
 * @brief gap security callback async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void GAPSecCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    GAPSecCallbackContext *gapSecCallbackPtr = (GAPSecCallbackContext *)context;

    MEM_MALLOC.free(gapSecCallbackPtr);

    return;
}

/**
 * @brief gap security callback.
 *
 * @param1 result Indicates the result.
 * @param2 serviceInfo Indicates the struct GapServiceSecurityInfo.
 * @param3 context Indicates the pointer to context.
 */
static void GAPSecCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, const void *context)
{
    LOG_INFO("%{public}s enter,result = %hu", __FUNCTION__, result);

    uint16_t connectHandle = *(uint16_t *)context;
    GAPSecCallbackContext *gapSecCallbackPtr = MEM_MALLOC.alloc(sizeof(GAPSecCallbackContext));
    if (gapSecCallbackPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    gapSecCallbackPtr->result = result;
    (void)memcpy_s(&(gapSecCallbackPtr->serviceInfo),
        sizeof(gapSecCallbackPtr->serviceInfo),
        &serviceInfo,
        sizeof(GapServiceSecurityInfo));
    gapSecCallbackPtr->connectHandle = connectHandle;

    AttAsyncProcess(GAPSecCallbackAsync, GAPSecCallbackAsyncDestroy, gapSecCallbackPtr);

    return;
}

/**
 * @brief assign gap request security parameter.
 *
 * @param1 gapSecurity Indicates the pointer to GapRequestSecurityParam.
 * @param2 direction Indicates the direction.
 * @param3 connecting Indicates the pointer to AttConnectingInfo.
 */
static void AttAssignGAPRequestSecurity(
    GapRequestSecurityParam *gapSecurity, uint8_t direction, const AttConnectingInfo *connecting)
{
    LOG_INFO("%{public}s enter,direction = %hhu", __FUNCTION__, direction);

    if (direction == OUTGOING) {
        gapSecurity->info.direction = OUTGOING;
        gapSecurity->info.serviceId = GATT_CLIENT;
    } else if (direction == INCOMING) {
        gapSecurity->info.direction = INCOMING;
        gapSecurity->info.serviceId = GATT_SERVER;
    }
    gapSecurity->info.protocolId = SEC_PROTOCOL_L2CAP;
    gapSecurity->info.channelId.l2capPsm = 0x001F;
    gapSecurity->callback = (void (*)(uint16_t, GapServiceSecurityInfo, void *))GAPSecCallback;
    gapSecurity->context = &(((AttConnectingInfo *)connecting)->connectHandle);

    return;
}

/**
 * @brief assign bredr connecting request parameter.
 *
 * @param1 connecting Indicates the pointer to AttConnectingInfo.
 * @param2 btAddress Indicates the pointer to const BtAddr.
 * @param3 connParaPtr Indicates the pointer to const AttConnect.
 * @param4 bredrconnectHandle Indicates the pointer to bredr connecthandle.
 */
static void AttConnectReqAssignBredrConnect(AttConnectingInfo **connecting, const BtAddr *btAddress,
    const AttConnect *connParaPtr, uint16_t *bredrconnectHandle)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;

    AttGetConnectingIndexByCidConnectHandle(0, 0, &index, connecting);

    if (*connecting == NULL) {
        goto ATTCONNECTREQASSIGNBREDRCONNECT_END;
    }

    (void)memcpy_s((*connecting)->addr.addr, ADDRESSLEN, btAddress->addr, ADDRESSLEN);
    (*connecting)->addr.type = btAddress->type;
    (*connecting)->connectHandle = index + 1;
    *bredrconnectHandle = index + 1;
    (*connecting)->initPassConnFlag = INITIATIVECONNECT;
    (*connecting)->initiativeConnectStatus = CONNECTING;
    (*connecting)->locall2capConfigInfoObj.mtu = connParaPtr->bredrConnParaVar.mtu;
    (*connecting)->locall2capConfigInfoObj.flushTimeout = connParaPtr->bredrConnParaVar.flushTimeout;
    (*connecting)->locall2capConfigInfoObj.rfc.mode = connParaPtr->bredrConnParaVar.mode;
    (*connecting)->mtu = DEFAULTBREDRMTU;
    (*connecting)->transportType = BT_TRANSPORT_BR_EDR;

ATTCONNECTREQASSIGNBREDRCONNECT_END:
    return;
}

/**
 * @brief att bredr connect timeout async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttBredrConnectTimeOutAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectTimeOutContext *attBredrTimeOutPtr = (ConnectTimeOutContext *)context;
    AttConnectingInfo *connecting = NULL;

    AttGetConnectingIndexByConnectHandle(attBredrTimeOutPtr->connectHandle, &connecting);

    if (connecting == NULL) {
        LOG_INFO("%{public}s connecting == NULL and goto ATTBREDRCONNECTTIMEOUT_END", __FUNCTION__);
        goto ATTBREDRCONNECTTIMEOUT_END;
    }

    AttConnectCompletedCallback(connecting, ATT_CONNECT_TIMEOUT);

ATTBREDRCONNECTTIMEOUT_END:
    MEM_MALLOC.free(attBredrTimeOutPtr);
    return;
}

/**
 * @brief att bredr connect timeout async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttBredrConnectTimeOutAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectTimeOutContext *attBredrTimeOutPtr = (ConnectTimeOutContext *)context;

    MEM_MALLOC.free(attBredrTimeOutPtr);

    return;
}

/**
 * @brief att bredr connect timeout.
 *
 * @param parameter Indicates the pointer to parameter.
 */
static void AttBredrConnectTimeOut(void *parameter)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectingInfo *connecting = (AttConnectingInfo *)parameter;
    ConnectTimeOutContext *attBredrTimeOutPtr = MEM_MALLOC.alloc(sizeof(ConnectTimeOutContext));
    if (attBredrTimeOutPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attBredrTimeOutPtr->connectHandle = connecting->connectHandle;

    AttAsyncProcess(AttBredrConnectTimeOutAsync, AttBredrConnectTimeOutAsyncDestroy, attBredrTimeOutPtr);

    return;
}

/**
 * @brief att connect completed call back.
 *
 * @param1 connecting Indicates the pointer to AttConnectingInfo.
 * @param2 result Indicates the result.
 */
static void AttConnectCompletedCallback(AttConnectingInfo *connecting, uint16_t result)
{
    LOG_INFO("%{public}s enter, result = %hu", __FUNCTION__, result);

    AttConnectInfo *connect = NULL;

    if (connecting == NULL) {
        LOG_INFO("%{public}s enter, result = %hu", __FUNCTION__, result);
        return;
    }

    if (result != ATT_CONNECT_TIMEOUT) {
        AlarmCancel(connecting->bredrAlarm);
    }

    AttBredrConnectCallback bredrConnectCallback;
    AttConnectedCallback *attConnectCallback = AttGetATTConnectCallback();
    if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attBREDRConnectCompleted == NULL)) {
        LOG_WARN("%{public}s attConnectCallback or attBREDRConnectCompleted is NULL", __FUNCTION__);
        goto ATTCONNECTCOMPLETEDCALLBACK_END;
    }

    (void)memcpy_s(bredrConnectCallback.addr.addr, ADDRESSLEN, connecting->addr.addr, ADDRESSLEN);
    bredrConnectCallback.addr.type = connecting->addr.type;
    bredrConnectCallback.status = result;
    bredrConnectCallback.mtu = connecting->mtu;

    if (result == BREDR_CONNECT_SUCCESS) {
        AttCopyToConnectInfo(connecting, &connect);
        if (connect == NULL) {
            goto ATTCONNECTCOMPLETEDCALLBACK_END;
        }
        attConnectCallback->attConnect.attBREDRConnectCompleted(
            connect->retGattConnectHandle, &bredrConnectCallback, attConnectCallback->context);
        goto ATTCONNECTCOMPLETEDCALLBACK_END;
    }
    if (connecting->initPassConnFlag == INITIATIVECONNECT) {
        if (connecting->initiativeConnectStatus == DISCONNECTED) {
            goto ATTCONNECTCOMPLETEDCALLBACK_END;
        } else {
            attConnectCallback->attConnect.attBREDRConnectCompleted(
                connecting->connectHandle, &bredrConnectCallback, attConnectCallback->context);
        }
    }
    if (connecting->initPassConnFlag == PASSIVECONNECT) {
        if (connecting->passiveConnectSatatus == DISCONNECTED) {
            goto ATTCONNECTCOMPLETEDCALLBACK_END;
        } else {
            attConnectCallback->attConnect.attBREDRConnectCompleted(
                connecting->connectHandle, &bredrConnectCallback, attConnectCallback->context);
        }
    }

ATTCONNECTCOMPLETEDCALLBACK_END:
    AttClearConnectingInfo(connecting);
    return;
}

/**
 * @brief att copy to connected.
 *
 * @param connecting Indicates the pointer to AttConnectingInfo.
 */
static void AttCopyToConnectInfo(const AttConnectingInfo *connecting, AttConnectInfo **connect)
{
    LOG_INFO("%{public}s enter ", __FUNCTION__);

    uint16_t index = 0;

    AttGetConnectInfoIndexByCidOutIndex(0, &index, connect);

    if (*connect == NULL) {
        LOG_INFO("%{public}s connect == NULL", __FUNCTION__);
        goto ATTCOPYTOCONNECTINFO_END;
    }

    (*connect)->aclHandle = connecting->aclHandle;
    (*connect)->AttConnectID.bredrcid = connecting->cid;
    (*connect)->retGattConnectHandle = index + 1;
    (*connect)->transportType = BT_TRANSPORT_BR_EDR;
    (*connect)->initPassConnFlag = connecting->initPassConnFlag;
    (*connect)->addr.type = connecting->addr.type;
    (void)memcpy_s((*connect)->addr.addr, ADDRESSLEN, connecting->addr.addr, ADDRESSLEN);
    (*connect)->mtu = connecting->mtu;

ATTCOPYTOCONNECTINFO_END:
    return;
}

/**
 * @brief add le connect information to AttConnectInfo.
 *
 * @param1 addr Indicates the pointer to const BtAddr.
 * @param2 aclHandle Indicates the aclHandle.
 * @param3 connect Indicates the secondary pointer to  AttConnectInfo.
 */
static void AttConnectInfoAddLe(
    const BtAddr *addr, uint16_t aclHandle, AttConnectInfo **connect, uint8_t initPassConnFlag)
{
    LOG_INFO("%{public}s enter,aclHandle = %hu", __FUNCTION__, aclHandle);

    uint16_t index = 0;
    AttGetConnectInfoIndexByConnectHandle(0, &index, connect);

    if ((*connect) == NULL) {
        LOG_INFO("%{public}s *connect == NULL", __FUNCTION__);
        goto ATTCONNECTINFOADDLE_END;
    }

    (*connect)->aclHandle = aclHandle;
    (*connect)->AttConnectID.lecid = LE_CID;
    (*connect)->retGattConnectHandle = index + 1;
    (*connect)->transportType = BT_TRANSPORT_LE;
    (*connect)->addr.type = addr->type;
    (*connect)->mtu = DEFAULTLEATTMTU;
    (*connect)->initPassConnFlag = initPassConnFlag;
    (void)memcpy_s((*connect)->addr.addr, ADDRESSLEN, addr->addr, ADDRESSLEN);

ATTCONNECTINFOADDLE_END:
    return;
}

/**
 * @brief call l2cap interface for le connect call back async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifLeConnectCallBackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectCallBackContext *connectCallContextPtr = (ConnectCallBackContext *)context;
    AttConnectingInfo *connecting = NULL;
    AttLeConnectCallback data;
    AttConnectedCallback *attConnectCallback = NULL;

    AttGetConnectingIndexByAddr(connectCallContextPtr->addr, &connecting);

    if (connecting == NULL) {
        goto L2CIFLECONNECTCALLBACK_END;
    }

    attConnectCallback = AttGetATTConnectCallback();
    if (connectCallContextPtr->result == BT_NO_ERROR) {
        connecting->transportType = BT_TRANSPORT_LE;
    } else {
        LOG_WARN("%{public}s:L2CIF_ConnectReq error", __FUNCTION__);
        (void)memcpy_s(&data.addr, sizeof(BtAddr), connectCallContextPtr->addr, sizeof(BtAddr));
        data.status = LE_CONNECT_FAIL;
        attConnectCallback->attConnect.attLEConnectCompleted(0, &data, attConnectCallback->context);
        AttClearConnectingInfo(connecting);
    }

L2CIFLECONNECTCALLBACK_END:
    MEM_MALLOC.free(connectCallContextPtr->addr);
    MEM_MALLOC.free(connectCallContextPtr);
    return;
}

/**
 * @brief call l2cap interface for le connect call back async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifLeConnectCallBackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectCallBackContext *connectCallContextPtr = (ConnectCallBackContext *)context;

    MEM_MALLOC.free(connectCallContextPtr->addr);
    MEM_MALLOC.free(connectCallContextPtr);

    return;
}

/**
 * @brief call l2cap interface for le connect call back.
 *
 * @param1 addr Indicates the pointer to const BtAddr.
 * @param2 result Indicates the result.
 */
void L2cifLeConnectCallBack(const BtAddr *addr, int result)
{
    LOG_INFO("%{public}s enter result = %{public}d", __FUNCTION__, result);

    BtAddr *btaddr = NULL;
    ConnectCallBackContext *connectCallContextPtr = NULL;

    btaddr = MEM_MALLOC.alloc(sizeof(BtAddr));
    if (btaddr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memcpy_s(btaddr->addr, ADDRESSLEN, addr, ADDRESSLEN);
    btaddr->type = addr->type;

    connectCallContextPtr = MEM_MALLOC.alloc(sizeof(ConnectCallBackContext));
    if (connectCallContextPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    connectCallContextPtr->addr = btaddr;
    connectCallContextPtr->result = result;

    AttAsyncProcess(L2cifLeConnectCallBackAsync, L2cifLeConnectCallBackAsyncDestroy, connectCallContextPtr);

    return;
}

/**
 * @brief call l2cap interface for le connect.
 *
 * @param1 addr Indicates the pointer to const BtAddr.
 * @param2 var Indicates the struct L2capLeConnectionParameter.
 * @param3 connecting Indicates the pointer to  AttConnectingInfo.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
static int AttLeConnectProcess(const BtAddr *addr, L2capLeConnectionParameter var, const AttConnectingInfo *connecting)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;

    ret = L2CIF_LeConnect(addr, &var, L2cifLeConnectCallBack);

    return ret;
}

/**
 * @brief callback of bredr connect request async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConnectReqCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectReqCallbackContext *connectReqContextPtr = NULL;
    AttConnectingInfo *connecting = NULL;

    connectReqContextPtr = (ConnectReqCallbackContext *)context;
    AttGetConnectingIndexByAddrUninitializedCid(connectReqContextPtr->addr, &connecting);

    if (connecting == NULL) {
        goto L2CIFCONNECTREQCALLBACK_END;
    }

    if (connectReqContextPtr->result == BT_NO_ERROR) {
        connecting->cid = connectReqContextPtr->lcid;
        connecting->transportType = BT_TRANSPORT_BR_EDR;
        connecting->mtu = DEFAULTBREDRMTU;
    } else {
        LOG_WARN("%{public}s:L2CIF_ConnectReq error", __FUNCTION__);
        AlarmCancel(connecting->bredrAlarm);
        AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
        AttClearConnectingInfo(connecting);
    }

L2CIFCONNECTREQCALLBACK_END:
    MEM_MALLOC.free(connectReqContextPtr->addr);
    MEM_MALLOC.free(connectReqContextPtr);
    return;
}

/**
 * @brief callback of bredr connect request async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConnectReqCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectReqCallbackContext *connectReqContextPtr = (ConnectReqCallbackContext *)context;

    MEM_MALLOC.free(connectReqContextPtr->addr);
    MEM_MALLOC.free(connectReqContextPtr);

    return;
}

/**
 * @brief callback of bredr connect request.
 *
 * @param addr Indicates the pointer to  BtAddr.
 * @param lcid Indicates the lcid.
 * @param result Indicates the result.
 */
static void L2cifConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BtAddr *btaddr = NULL;
    ConnectReqCallbackContext *connectReqContextPtr = NULL;

    btaddr = MEM_MALLOC.alloc(sizeof(BtAddr));
    if (btaddr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memcpy_s(btaddr->addr, ADDRESSLEN, addr, ADDRESSLEN);
    btaddr->type = addr->type;

    connectReqContextPtr = MEM_MALLOC.alloc(sizeof(ConnectReqCallbackContext));
    if (connectReqContextPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    connectReqContextPtr->addr = btaddr;
    connectReqContextPtr->lcid = lcid;
    connectReqContextPtr->result = result;

    AttAsyncProcess(L2cifConnectReqCallbackAsync, L2cifConnectReqCallbackAsyncDestroy, connectReqContextPtr);

    return;
}

/**
 * @brief call l2cap interface for bredr connect.
 *
 * @param connecting Indicates the pointer to  AttConnectingInfo.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
static int AttBredrConnectProcess(AttConnectingInfo *connecting)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;

    AlarmSet(connecting->bredrAlarm, (uint64_t)CONNECTTIMEOUT, AttBredrConnectTimeOut, connecting);

    ret = L2CIF_ConnectReq(&(connecting->addr),
        BT_PSM_ATT,
        BT_PSM_ATT,
        NULL,
        (void (*)(const BtAddr *, uint16_t, int, void *))L2cifConnectReqCallback);

    return ret;
}

/**
 * @brief gatt send connect request para assign.
 *
 * @param1 l2capObjptr Indicates the pointer to L2capLeConnectionParameter.
 * @param2 attConnectReqAsyncPtr Indicates the pointer to AttConnectReqAsync.
 */
static void AttConnectRequestLEParaAssign(
    L2capLeConnectionParameter *l2capObjptr, const AttConnectReqAsync *attConnectReqAsyncPtr)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    l2capObjptr->connIntervalMin = attConnectReqAsyncPtr->connParaPtr->leConnParaVar.connIntervalMin;
    l2capObjptr->connIntervalMax = attConnectReqAsyncPtr->connParaPtr->leConnParaVar.connIntervalMax;
    l2capObjptr->connLatency = attConnectReqAsyncPtr->connParaPtr->leConnParaVar.connLatency;
    l2capObjptr->supervisionTimeout = attConnectReqAsyncPtr->connParaPtr->leConnParaVar.supervisionTimeout;

    return;
}

/**
 * @brief send connect request in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttConnectRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index;
    L2capLeConnectionParameter l2capObj;
    GapRequestSecurityParam gapReqSecurity;
    AttConnectingInfo *connecting = NULL;
    AttConnectReqAsync *attConnectReqAsyncPtr = (AttConnectReqAsync *)context;

    if (attConnectReqAsyncPtr->btAddress == NULL) {
        LOG_WARN("%{public}s:btAddress == NULL and goto ATT_CONNECTREQ_END", __FUNCTION__);
        goto ATT_CONNECTREQ_END;
    }

    if (attConnectReqAsyncPtr->transportType == BT_TRANSPORT_BR_EDR) {
        AttConnectReqAssignBredrConnect(&connecting,
            attConnectReqAsyncPtr->btAddress,
            attConnectReqAsyncPtr->connParaPtr,
            attConnectReqAsyncPtr->bredrconnectHandle);
        AttAssignGAPRequestSecurity(&gapReqSecurity, OUTGOING, connecting);
        GAPIF_RequestSecurityAsync(&(connecting->addr), &gapReqSecurity);
    } else if (attConnectReqAsyncPtr->transportType == BT_TRANSPORT_LE) {
        AttConnectRequestLEParaAssign(&l2capObj, attConnectReqAsyncPtr);
        connecting = AttGetConnectingStart();
        for (index = 0; index < MAXCONNECT; ++index, connecting++) {
            if (connecting->connectHandle == 0) {
                (void)memcpy_s(connecting->addr.addr, ADDRESSLEN, attConnectReqAsyncPtr->btAddress->addr, ADDRESSLEN);
                connecting->addr.type = attConnectReqAsyncPtr->btAddress->type;
                connecting->connectHandle = index + 1;
                connecting->transportType = BT_TRANSPORT_LE;
                connecting->cid = LE_CID;
                connecting->initPassConnFlag = INITIATIVECONNECT;
                break;
            }
        }
        AttLeConnectProcess(attConnectReqAsyncPtr->btAddress, l2capObj, connecting);
    }

ATT_CONNECTREQ_END:

    MEM_MALLOC.free(attConnectReqAsyncPtr->connParaPtr);
    MEM_MALLOC.free(attConnectReqAsyncPtr->btAddress);
    MEM_MALLOC.free(attConnectReqAsyncPtr->bredrconnectHandle);
    MEM_MALLOC.free(attConnectReqAsyncPtr);

    return;
}

/**
 * @brief destroy connect request data in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttConnectRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectReqAsync *attConnectReqAsyncPtr = (AttConnectReqAsync *)context;

    MEM_MALLOC.free(attConnectReqAsyncPtr->connParaPtr);
    MEM_MALLOC.free(attConnectReqAsyncPtr->btAddress);
    MEM_MALLOC.free(attConnectReqAsyncPtr->bredrconnectHandle);
    MEM_MALLOC.free(attConnectReqAsyncPtr);

    return;
}

/**
 * @brief gatt send connect request to att.
 *
 * @param1 transportType Indicates the connect type.
 * @param2 connParaPtr Indicates the pointer to connect parameter.
 * @param3 btAddress Indicates the pointer to address.
 * @param4 bredrconnectHandle Indicates the pointer to connecthandle be outputted.
 * @return Returns the result of connect request.
 */
void ATT_ConnectReq(
    uint8_t transportType, const AttConnect *connParaPtr, const BtAddr *btAddress, const uint16_t *bredrconnectHandle)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnect *connectAsyncPtr = MEM_MALLOC.alloc(sizeof(AttConnect));
    BtAddr *btAddrAsyncPtr = MEM_MALLOC.alloc(sizeof(BtAddr));
    uint16_t *bredrConnectHandleAsyncPtr = MEM_MALLOC.alloc(sizeof(uint16_t));
    AttConnectReqAsync *attConnectReqAsyncPtr = MEM_MALLOC.alloc(sizeof(AttConnectReqAsync));
    if (connectAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    if (btAddrAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    if (bredrConnectHandleAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    if (attConnectReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    if (transportType == BT_TRANSPORT_BR_EDR) {
        connectAsyncPtr->bredrConnParaVar = connParaPtr->bredrConnParaVar;
    } else if (transportType == BT_TRANSPORT_LE) {
        connectAsyncPtr->leConnParaVar = connParaPtr->leConnParaVar;
    }

    (void)memcpy_s(btAddrAsyncPtr, sizeof(BtAddr), btAddress, sizeof(BtAddr));

    attConnectReqAsyncPtr->transportType = transportType;
    attConnectReqAsyncPtr->connParaPtr = connectAsyncPtr;
    attConnectReqAsyncPtr->btAddress = btAddrAsyncPtr;
    attConnectReqAsyncPtr->bredrconnectHandle = bredrConnectHandleAsyncPtr;

    AttAsyncProcess(AttConnectRequestAsync, AttConnectRequestAsyncDestroy, attConnectReqAsyncPtr);

    return;
}

/**
 * @brief gatt send connect request to att async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConfigReqCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectingInfo *connecting = NULL;
    ConfigReqCallbackContext *configReqPtr = (ConfigReqCallbackContext *)context;

    AttGetConnectingIndexByCid(configReqPtr->lcid, &connecting);

    if (connecting == NULL) {
        goto L2CIFCONFIGREQCALLBACK_END;
    }

    if (configReqPtr->result != BT_NO_ERROR) {
        AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
        AttClearConnectingInfo(connecting);
    }

L2CIFCONFIGREQCALLBACK_END:
    MEM_MALLOC.free(configReqPtr);
    return;
}

/**
 * @brief gatt send connect request to att async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConfigReqCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConfigReqCallbackContext *configReqPtr = (ConfigReqCallbackContext *)context;

    MEM_MALLOC.free(configReqPtr);

    return;
}

/**
 * @brief gatt send connect request to att.
 *
 * @param1 lcid Indicates the connect lcid.
 * @param2 result Indicates the variable of result.
 */
static void L2cifConfigReqCallback(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter, lcid = %hu, result = %{public}d", __FUNCTION__, lcid, result);

    ConfigReqCallbackContext *configReqPtr = MEM_MALLOC.alloc(sizeof(ConfigReqCallbackContext));
    if (configReqPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    configReqPtr->lcid = lcid;
    configReqPtr->result = result;

    AttAsyncProcess(L2cifConfigReqCallbackAsync, L2cifConfigReqCallbackAsyncDestroy, configReqPtr);

    return;
}

/**
 * @brief gatt send connect response to att async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConnectRspResultCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectingInfo *connecting = NULL;
    ConnectRspResultCallbackContext *connectRspPtr = (ConnectRspResultCallbackContext *)context;

    AttGetConnectingIndexByCid(connectRspPtr->lcid, &connecting);

    if (connecting == NULL) {
        goto L2CIFCONNECTRSPRESULTCALLBACK_END;
    }

    if (connectRspPtr->result == L2CAP_SUCCESS) {
        L2CIF_ConfigReq(connecting->cid, &(connecting->locall2capConfigInfoObj), L2cifConfigReqCallback);
    }

L2CIFCONNECTRSPRESULTCALLBACK_END:
    MEM_MALLOC.free(connectRspPtr);
    return;
}

/**
 * @brief gatt send connect response to att async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConnectRspResultCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectRspResultCallbackContext *connectRspPtr = (ConnectRspResultCallbackContext *)context;

    MEM_MALLOC.free(connectRspPtr);

    return;
}

/**
 * @brief gatt send connect response to att.
 *
 * @param1 lcid Indicates the connect lcid.
 * @param2 result Indicates the variable of result.
 */
static void L2cifConnectRspResultCallback(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter, lcid = %hu, result = %{public}d", __FUNCTION__, lcid, result);

    ConnectRspResultCallbackContext *connectRspPtr = MEM_MALLOC.alloc(sizeof(ConnectRspResultCallbackContext));
    if (connectRspPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    connectRspPtr->lcid = lcid;
    connectRspPtr->result = result;

    AttAsyncProcess(L2cifConnectRspResultCallbackAsync, L2cifConnectRspResultCallbackAsyncDestroy, connectRspPtr);

    return;
}

/**
 * @brief perform the connect rsp operation in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttConnectRsponseAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectingInfo *connecting = NULL;
    AttConnectRspAsync *attConnectRspAsyncPtr = (AttConnectRspAsync *)context;

    AttGetConnectingIndexByConnectHandle(attConnectRspAsyncPtr->connectHandle, &connecting);

    if (connecting == NULL) {
        LOG_INFO("connecting == NULL and goto ATTCONNECTRSP_END");
        goto ATTCONNECTRSP_END;
    }

    connecting->locall2capConfigInfoObj.mtu = attConnectRspAsyncPtr->g_attConnectParaObj->bredrConnParaVar.mtu;
    connecting->locall2capConfigInfoObj.flushTimeout =
        attConnectRspAsyncPtr->g_attConnectParaObj->bredrConnParaVar.flushTimeout;
    connecting->locall2capConfigInfoObj.rfc.mode = attConnectRspAsyncPtr->g_attConnectParaObj->bredrConnParaVar.mode;

    L2CIF_ConnectRsp(connecting->cid,
        connecting->id,
        attConnectRspAsyncPtr->result,
        attConnectRspAsyncPtr->status,
        L2cifConnectRspResultCallback);

ATTCONNECTRSP_END:

    MEM_MALLOC.free(attConnectRspAsyncPtr->g_attConnectParaObj);
    MEM_MALLOC.free(attConnectRspAsyncPtr);
    return;
}

/**
 * @brief destroy connect response data in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttConnectRsponseAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectRspAsync *attConnectRspAsyncPtr = (AttConnectRspAsync *)context;

    MEM_MALLOC.free(attConnectRspAsyncPtr->g_attConnectParaObj);
    MEM_MALLOC.free(attConnectRspAsyncPtr);

    return;
}

/**
 * @brief gatt send connect response to att.
 *
 * @param1 connectHandle Indicates the connect handle.
 * @param2 result Indicates the variable of result.
 * @param3 status Indicates the result of status.
 * @param4 g_attConnectParaObj Indicates the pointer to connect parameter.
 * @return Returns the result of connect response.
 */
void ATT_ConnectRsp(uint16_t connectHandle, uint16_t result, uint16_t status, const AttConnect *cfg)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu,result = %hu,status=%hu, mtu = %{public}d",
        __FUNCTION__,
        connectHandle,
        result,
        status,
        cfg->bredrConnParaVar.mtu);

    AttConnect *attConnectBredrConnPtr = NULL;
    AttConnectRspAsync *attConnectRspAsyncPtr = NULL;

    attConnectBredrConnPtr = MEM_MALLOC.alloc(sizeof(AttConnect));
    attConnectRspAsyncPtr = MEM_MALLOC.alloc(sizeof(AttConnectRspAsync));
    if (attConnectBredrConnPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    if (attConnectRspAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attConnectBredrConnPtr->bredrConnParaVar.mtu = cfg->bredrConnParaVar.mtu;
    attConnectBredrConnPtr->bredrConnParaVar.flushTimeout = cfg->bredrConnParaVar.flushTimeout;
    attConnectBredrConnPtr->bredrConnParaVar.mode = cfg->bredrConnParaVar.mode;

    attConnectRspAsyncPtr->connectHandle = connectHandle;
    attConnectRspAsyncPtr->result = result;
    attConnectRspAsyncPtr->status = status;
    attConnectRspAsyncPtr->g_attConnectParaObj = attConnectBredrConnPtr;

    AttAsyncProcess(AttConnectRsponseAsync, AttConnectRsponseAsyncDestroy, attConnectRspAsyncPtr);

    return;
}

/**
 * @brief gatt send disconnect request to att async.
 *
 * @param ctx Indicates the pointer to context.
 */
void L2cifBREDRDisconnectReqCallBackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectInfo *connect = NULL;
    BREDRDisconnectReqCallBackContext *disBredrConnectReqPtr = (BREDRDisconnectReqCallBackContext *)context;
    AttBredrDisconnectCallback BredrData;
    AttConnectedCallback *attConnectCallback = NULL;

    AttGetConnectInfoIndexByCid(disBredrConnectReqPtr->lcid, &connect);

    if (connect == NULL) {
        goto L2CIFDISCONNECTREQCALLBACK_END;
    }

    attConnectCallback = AttGetATTConnectCallback();
    if (disBredrConnectReqPtr->result != BT_NO_ERROR) {
        if (connect->transportType == BT_TRANSPORT_BR_EDR) {
            BredrData.reason = INITIATIVECONNECT_DISCONNECT_FAIL;
            attConnectCallback->attConnect.attBREDRDisconnectCompleted(
                connect->retGattConnectHandle, &BredrData, attConnectCallback->context);
        }
    }

L2CIFDISCONNECTREQCALLBACK_END:
    MEM_MALLOC.free(disBredrConnectReqPtr);
    return;
}

/**
 * @brief gatt send disconnect request to att async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
void L2cifBREDRDisconnectReqCallBackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BREDRDisconnectReqCallBackContext *disBredrConnectReqPtr = (BREDRDisconnectReqCallBackContext *)context;

    MEM_MALLOC.free(disBredrConnectReqPtr);

    return;
}

/**
 * @brief gatt send disconnect request to att.
 *
 * @param lcid Indicates the connect lcid.
 * @return Returns the result of disconnect request.
 */
void L2cifBREDRDisconnectReqCallBack(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter, result = %{public}d", __FUNCTION__, result);

    BREDRDisconnectReqCallBackContext *disBredrConnectReqPtr =
        MEM_MALLOC.alloc(sizeof(BREDRDisconnectReqCallBackContext));
    if (disBredrConnectReqPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    disBredrConnectReqPtr->lcid = lcid;
    disBredrConnectReqPtr->result = result;

    AttAsyncProcess(
        L2cifBREDRDisconnectReqCallBackAsync, L2cifBREDRDisconnectReqCallBackAsyncDestroy, disBredrConnectReqPtr);

    return;
}

/**
 * @brief le disconnect request callback async.
 *
 * @param ctx Indicates the pointer to context.
 */
void LeDisconnectReqCallBackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectInfo *connect = NULL;
    LeDisconnectReqCallBackContext *disLeConnectReqPtr = (LeDisconnectReqCallBackContext *)context;
    AttLeDisconnectCallback leData;
    AttConnectedCallback *attConnectCallback = NULL;

    AttGetConnectInfoIndexByCid(disLeConnectReqPtr->aclHandle, &connect);

    if (connect == NULL) {
        goto LEDISCONNECTREQCALLBACK_END;
    }

    attConnectCallback = AttGetATTConnectCallback();
    if (disLeConnectReqPtr->result != BT_NO_ERROR) {
        if (connect->transportType == BT_TRANSPORT_LE) {
            leData.status = LE_DISCONNECT_FAIL;
            if (attConnectCallback->attConnect.attLEDisconnectCompleted != NULL) {
                attConnectCallback->attConnect.attLEDisconnectCompleted(
                    connect->retGattConnectHandle, &leData, attConnectCallback->context);
            }
        }
    }

LEDISCONNECTREQCALLBACK_END:
    MEM_MALLOC.free(disLeConnectReqPtr);
    return;
}

/**
 * @brief le disconnect request callback async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
void LeDisconnectReqCallBackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    LeDisconnectReqCallBackContext *disLeConnectReqPtr = (LeDisconnectReqCallBackContext *)context;

    MEM_MALLOC.free(disLeConnectReqPtr);

    return;
}

/**
 * @brief le disconnect request callback.
 *
 * @param aclHandle Indicates the aclHandle.
 * @param result Indicates the result.
 */
void LeDisconnectReqCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s enter, result = %{public}d", __FUNCTION__, result);

    LeDisconnectReqCallBackContext *disLeConnectReqPtr = MEM_MALLOC.alloc(sizeof(LeDisconnectReqCallBackContext));
    if (disLeConnectReqPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    disLeConnectReqPtr->aclHandle = aclHandle;
    disLeConnectReqPtr->result = result;

    AttAsyncProcess(LeDisconnectReqCallBackAsync, LeDisconnectReqCallBackAsyncDestroy, disLeConnectReqPtr);

    return;
}

/**
 * @brief send disconnect request in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttDisconnectRequestAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttDisconnectReqAsync *attDisconnectReqAsyncPtr = (AttDisconnectReqAsync *)context;

    InitiativeDisconnect(attDisconnectReqAsyncPtr->connectHandle);

    MEM_MALLOC.free(attDisconnectReqAsyncPtr);
    return;
}

/**
 * @brief destroy disconnect request async destroy.
 *
 * @param context Indicates the pointer to context.
 */
static void AttDisconnectRequestAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttDisconnectReqAsync *attDisconnectReqAsyncPtr = (AttDisconnectReqAsync *)context;

    MEM_MALLOC.free(attDisconnectReqAsyncPtr);

    return;
}

/**
 * @brief gatt send disconnect request to att.
 *
 * @param connectHandle Indicates the connect handle.
 * @return Returns the result of disconnect request.
 */
void ATT_DisconnectReq(uint16_t connectHandle)
{
    LOG_INFO("%{public}s enter,connectHandle = %hu", __FUNCTION__, connectHandle);

    AttDisconnectReqAsync *attDisconnectReqAsyncPtr = MEM_MALLOC.alloc(sizeof(AttDisconnectReqAsync));
    if (attDisconnectReqAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attDisconnectReqAsyncPtr->connectHandle = connectHandle;

    AttAsyncProcess(AttDisconnectRequestAsync, AttDisconnectRequestAsyncDestroy, attDisconnectReqAsyncPtr);

    return;
}

/**
 * @brief receive le connected async.
 *
 * @param ctx Indicates the pointer to context.
 */
NO_SANITIZE("cfi") static void AttLeConnectedAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttLeConnectedAsyncContext *attLeConnectedAsyncPtr = (AttLeConnectedAsyncContext *)context;
    AttLeConnectCallback leConnectCallbackParaObj;
    AttConnectInfo *connect = NULL;
    AttConnectingInfo *connecting = NULL;
    uint8_t initPassConnFlag = 0;
    AttConnectedCallback *attConnectCallback = AttGetATTConnectCallback();

    if (attLeConnectedAsyncPtr->role == LEROLEMASTER) {
        AttGetConnectingIndexByAddrAclhandleCid(attLeConnectedAsyncPtr->addr, 0, LE_CID, &connecting);

        if (connecting != NULL) {
            AttClearConnectingInfo(connecting);
        }

        initPassConnFlag = INITIATIVECONNECT;
    } else if (attLeConnectedAsyncPtr->role == LEROLESLAVE) {
        initPassConnFlag = PASSIVECONNECT;
    }

    if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attLEConnectCompleted == NULL)) {
        LOG_WARN("%{public}s attConnectCallback or attLEConnectCompleted is NULL", __FUNCTION__);
        goto ATTLECONNECTED_END;
    }

    leConnectCallbackParaObj.addr.type = attLeConnectedAsyncPtr->addr->type;
    leConnectCallbackParaObj.role = attLeConnectedAsyncPtr->role;
    (void)memcpy_s(leConnectCallbackParaObj.addr.addr, ADDRESSLEN, attLeConnectedAsyncPtr->addr->addr, ADDRESSLEN);

    if (attLeConnectedAsyncPtr->status == BT_NO_ERROR) {
        leConnectCallbackParaObj.status = LE_CONNECT_SUCCESS;
        AttConnectInfoAddLe(
            attLeConnectedAsyncPtr->addr, attLeConnectedAsyncPtr->aclHandle, &connect, initPassConnFlag);
        if (connect != NULL) {
            attConnectCallback->attConnect.attLEConnectCompleted(
                connect->retGattConnectHandle, &leConnectCallbackParaObj, attConnectCallback->context);
        }
    } else {
        leConnectCallbackParaObj.status = LE_CONNECT_FAIL;
        attConnectCallback->attConnect.attLEConnectCompleted(0, &leConnectCallbackParaObj, attConnectCallback->context);
    }

ATTLECONNECTED_END:
    MEM_MALLOC.free(attLeConnectedAsyncPtr->addr);
    MEM_MALLOC.free(attLeConnectedAsyncPtr);
    return;
}

/**
 * @brief receive le connected async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttLeConnectedAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttLeConnectedAsyncContext *attLeConnectedAsyncPtr = (AttLeConnectedAsyncContext *)context;

    MEM_MALLOC.free(attLeConnectedAsyncPtr->addr);
    MEM_MALLOC.free(attLeConnectedAsyncPtr);

    return;
}

/**
 * @brief receive  le connected.
 *
 * @param1 addr Indicates the pointer to BtAddr.
 * @param2 aclHandle Indicates the aclHandle.
 * @param3 role Indicates the role.
 * @param4 status Indicates the status.
 */
void AttLeConnected(const BtAddr *addr, uint16_t aclHandle, uint8_t role, uint8_t status)
{
    LOG_INFO("%{public}s enter, aclHandle = %hu, role = %hhu", __FUNCTION__, aclHandle, role);

    BtAddr *btAddrPtr = MEM_MALLOC.alloc(sizeof(BtAddr));
    if (btAddrPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memcpy_s(btAddrPtr->addr, ADDRESSLEN, addr, ADDRESSLEN);
    btAddrPtr->type = addr->type;

    AttLeConnectedAsyncContext *attLeConnectedAsyncPtr = MEM_MALLOC.alloc(sizeof(AttLeConnectedAsyncContext));
    if (attLeConnectedAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attLeConnectedAsyncPtr->addr = btAddrPtr;
    attLeConnectedAsyncPtr->aclHandle = aclHandle;
    attLeConnectedAsyncPtr->role = role;
    attLeConnectedAsyncPtr->status = status;

    AttAsyncProcess(AttLeConnectedAsync, AttLeConnectedAsyncDestroy, attLeConnectedAsyncPtr);

    return;
}

/**
 * @brief receive  le disconnected async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttLeDisconnectedAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttLeDisconnectedAsyncContext *attLeDisconnectedPtr = (AttLeDisconnectedAsyncContext *)context;
    AttLeDisconnectCallback leDisConnCallbackObj;
    int listSize;
    AttConnectInfo *connect = NULL;
    AttConnectedCallback *attConnectCallback = AttGetATTConnectCallback();

    leDisConnCallbackObj.reason = attLeDisconnectedPtr->reason;
    if (attLeDisconnectedPtr->status == BT_NO_ERROR) {
        leDisConnCallbackObj.status = LE_DISCONNECT_SUCCESS;
    } else {
        leDisConnCallbackObj.status = LE_DISCONNECT_FAIL;
    }

    AttGetConnectInfoIndexByAclHandle(attLeDisconnectedPtr->aclHandle, &connect);

    if (connect == NULL) {
        goto ATTLEDISCONNECTED_END;
    }

    if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attLEDisconnectCompleted == NULL)) {
        LOG_WARN("%{public}s attConnectCallback or attLEDisconnectCompleted is NULL", __FUNCTION__);
    } else {
        attConnectCallback->attConnect.attLEDisconnectCompleted(
            connect->retGattConnectHandle, &leDisConnCallbackObj, attConnectCallback->context);
    }

    if (leDisConnCallbackObj.status == LE_DISCONNECT_SUCCESS) {
        listSize = ListGetSize(connect->instruct);
        LOG_INFO("%{public}s listSize = %u", __FUNCTION__, listSize);
        for (; listSize > 0; --listSize) {
            ListRemoveLast(connect->instruct);
        }
        AttClearConnectInfo(connect);
    }

ATTLEDISCONNECTED_END:
    MEM_MALLOC.free(attLeDisconnectedPtr);
    return;
}

/**
 * @brief receive  le disconnected async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttLeDisconnectedAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttLeDisconnectedAsyncContext *attLeDisconnectedPtr = (AttLeDisconnectedAsyncContext *)context;

    MEM_MALLOC.free(attLeDisconnectedPtr);

    return;
}

/**
 * @brief receive  le disconnected.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 status Indicates the status.
 * @param3 reason Indicates the reason.
 */
void AttLeDisconnected(uint16_t aclHandle, uint8_t status, uint8_t reason)
{
    LOG_INFO("%{public}s enter, aclHandle = %hu,status = %hhu,reason = %hhu", __FUNCTION__, aclHandle, status, reason);

    AttLeDisconnectedAsyncContext *attLeDisconnectedPtr = MEM_MALLOC.alloc(sizeof(AttLeDisconnectedAsyncContext));
    if (attLeDisconnectedPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attLeDisconnectedPtr->aclHandle = aclHandle;
    attLeDisconnectedPtr->status = status;
    attLeDisconnectedPtr->reason = reason;

    AttAsyncProcess(AttLeDisconnectedAsync, AttLeDisconnectedAsyncDestroy, attLeDisconnectedPtr);

    return;
}

/**
 * @brief  bredr connect response pending call backt async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConnectRspPendingCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectingInfo *connecting = NULL;
    ConnectRspPendingContext *connectRspPendingPtr = (ConnectRspPendingContext *)context;
    GapRequestSecurityParam gapReqSecurity;

    AttGetConnectingIndexByCid(connectRspPendingPtr->lcid, &connecting);

    if (connecting == NULL) {
        goto L2CIFCONNECTRSPPENDINGCALLBACK_END;
    }

    if (connectRspPendingPtr->result == BT_NO_ERROR) {
        AttAssignGAPRequestSecurity(&gapReqSecurity, INCOMING, connecting);
        GAPIF_RequestSecurityAsync(&(connecting->addr), &gapReqSecurity);
    } else {
        LOG_WARN("%{public}s:L2CIF_ConnectRsp error", __FUNCTION__);
        AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
        AttClearConnectingInfo(connecting);
    }

L2CIFCONNECTRSPPENDINGCALLBACK_END:
    MEM_MALLOC.free(connectRspPendingPtr);
    return;
}

/**
 * @brief  bredr connect response pending call backt async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConnectRspPendingCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectRspPendingContext *connectRspPendingPtr = (ConnectRspPendingContext *)context;

    MEM_MALLOC.free(connectRspPendingPtr);

    return;
}

/**
 * @brief  bredr connect response pending call backt.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 result Indicates the result.
 */
static void L2cifConnectRspPendingCallback(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter, lcid = %hu, result = %{public}d", __FUNCTION__, lcid, result);

    ConnectRspPendingContext *connectRspPendingPtr = MEM_MALLOC.alloc(sizeof(ConnectRspPendingContext));
    if (connectRspPendingPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    connectRspPendingPtr->lcid = lcid;
    connectRspPendingPtr->result = result;

    AttAsyncProcess(
        L2cifConnectRspPendingCallbackAsync, L2cifConnectRspPendingCallbackAsyncDestroy, connectRspPendingPtr);

    return;
}

/**
 * @brief received bredr connect request async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConnectionReqAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConnectReqContext *attRecevieConnectPtr = (AttReceiveConnectReqContext *)context;

    AttConnectingInfo *connecting = AttGetConnectingStart();

    if (connecting == NULL) {
        LOG_WARN("%{public}s connecting == NULL", __FUNCTION__);
        L2CIF_ConnectRsp(attRecevieConnectPtr->lcid,
            attRecevieConnectPtr->id,
            L2CAP_NO_RESOURCES_AVAILABLE,
            L2CAP_NO_FURTHER_INFORMATION_AVAILABLE,
            NULL);
        goto ATTRECEIVECONNECTIONREQ_END;
    }

    AttCopyToConnectingInfo(&(attRecevieConnectPtr->info->addr),
        attRecevieConnectPtr->info->handle,
        attRecevieConnectPtr->lcid,
        attRecevieConnectPtr->id,
        connecting);

    L2CIF_ConnectRsp(attRecevieConnectPtr->lcid,
        attRecevieConnectPtr->id,
        CONNECTIONPENDING,
        AUTHENTICATIONPENDING,
        L2cifConnectRspPendingCallback);

ATTRECEIVECONNECTIONREQ_END:
    MEM_MALLOC.free(attRecevieConnectPtr->info);
    MEM_MALLOC.free(attRecevieConnectPtr);
    return;
}

/**
 * @brief received bredr connect request async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConnectionReqAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConnectReqContext *attRecevieConnectPtr = (AttReceiveConnectReqContext *)context;

    MEM_MALLOC.free(attRecevieConnectPtr->info);
    MEM_MALLOC.free(attRecevieConnectPtr);

    return;
}

/**
 * @brief received bredr connect request.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 id Indicates the id.
 * @param3 info Indicates the pointer to L2capConnectionInfo.
 * @param4 lpsm Indicates the lpsm.
 * @param5 ctx Indicates the pointer to context.
 */
void AttReceiveConnectionReq(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, const void *ctx)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    L2capConnectionInfo *connectInfoPtr = NULL;
    AttReceiveConnectReqContext *attRecevieConnectPtr = NULL;

    connectInfoPtr = MEM_MALLOC.alloc(sizeof(L2capConnectionInfo));
    if (connectInfoPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memcpy_s(&(connectInfoPtr->addr), sizeof(BtAddr), &(info->addr), sizeof(BtAddr));
    connectInfoPtr->handle = info->handle;

    attRecevieConnectPtr = MEM_MALLOC.alloc(sizeof(AttReceiveConnectReqContext));
    if (attRecevieConnectPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attRecevieConnectPtr->lcid = lcid;
    attRecevieConnectPtr->id = id;
    attRecevieConnectPtr->info = connectInfoPtr;
    attRecevieConnectPtr->lpsm = lpsm;
    attRecevieConnectPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttReceiveConnectionReqAsync, AttReceiveConnectionReqAsyncDestroy, attRecevieConnectPtr);

    return;
}

/**
 * @brief received bredr connect response async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConnectionRspAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConnectionRspContext *attRecevieRspConnectPtr = (AttReceiveConnectionRspContext *)context;
    AttConnectingInfo *connecting = NULL;
    L2capConfigInfo l2capConfigInfoPtr;

    AttGetConnectingIndexByCid(attRecevieRspConnectPtr->lcid, &connecting);

    if (connecting == NULL) {
        goto ATTRECVCONNECTIONRSP_END;
    }

    connecting->aclHandle = attRecevieRspConnectPtr->info->handle;

    if (attRecevieRspConnectPtr->result == L2CAP_CONNECTION_SUCCESSFUL) {
        if (connecting->locall2capConfigInfoObj.rfc.mode == BR_EDR_BASIC_MODE) {
            l2capConfigInfoPtr.mtu = connecting->locall2capConfigInfoObj.mtu;
            l2capConfigInfoPtr.flushTimeout = connecting->locall2capConfigInfoObj.flushTimeout;
            l2capConfigInfoPtr.rfc.mode = connecting->locall2capConfigInfoObj.rfc.mode;
        }
        L2CIF_ConfigReq(attRecevieRspConnectPtr->lcid, &l2capConfigInfoPtr, L2cifConfigReqCallback);
    } else if (attRecevieRspConnectPtr->result == L2CAP_CONNECTION_PENDING) {
        goto ATTRECVCONNECTIONRSP_END;
    } else {
        attRecevieRspConnectPtr->result = BREDR_CONNECT_FAIL;
        AttConnectCompletedCallback(connecting, attRecevieRspConnectPtr->result);
    }

ATTRECVCONNECTIONRSP_END:
    MEM_MALLOC.free(attRecevieRspConnectPtr->info);
    MEM_MALLOC.free(attRecevieRspConnectPtr);
    return;
}

/**
 * @brief received bredr connect response async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConnectionRspAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConnectionRspContext *attRecevieRspConnectPtr = (AttReceiveConnectionRspContext *)context;

    MEM_MALLOC.free(attRecevieRspConnectPtr->info);
    MEM_MALLOC.free(attRecevieRspConnectPtr);

    return;
}

/**
 * @brief received bredr connect response.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 info Indicates the pointer to L2capConnectionInfo.
 * @param3 result Indicates the result.
 * @param4 status Indicates the status.
 * @param5 ctx Indicates the pointer to context.
 */
void AttReceiveConnectionRsp(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, const void *ctx)
{
    LOG_INFO("%{public}s enter, result = %hu, status = %hu", __FUNCTION__, result, status);

    L2capConnectionInfo *connectInfoPtr = NULL;
    AttReceiveConnectionRspContext *attRecevieRspConnectPtr = NULL;

    connectInfoPtr = MEM_MALLOC.alloc(sizeof(L2capConnectionInfo));
    if (connectInfoPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memcpy_s(&(connectInfoPtr->addr), sizeof(BtAddr), &(info->addr), sizeof(BtAddr));
    connectInfoPtr->handle = info->handle;

    attRecevieRspConnectPtr = MEM_MALLOC.alloc(sizeof(AttReceiveConnectionRspContext));
    if (attRecevieRspConnectPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attRecevieRspConnectPtr->lcid = lcid;
    attRecevieRspConnectPtr->info = connectInfoPtr;
    attRecevieRspConnectPtr->result = result;
    attRecevieRspConnectPtr->status = status;
    attRecevieRspConnectPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttReceiveConnectionRspAsync, AttReceiveConnectionRspAsyncDestroy, attRecevieRspConnectPtr);

    return;
}

/**
 * @brief receive config response async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConfigRspAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConfigRspContext *attRecevieConfigRspPtr = (AttReceiveConfigRspContext *)context;
    uint8_t outPara = 0;
    AttConnectingInfo *connecting = NULL;

    AttGetConnectingIndexByCid(attRecevieConfigRspPtr->lcid, &connecting);

    if (connecting == NULL) {
        goto ATTRECVCONFIGRSP_END;
    }

    if (attRecevieConfigRspPtr->result == L2CAP_SUCCESS) {
        if (connecting->initPassConnFlag == INITIATIVECONNECT) {
            AttBredrConnectStatusChange(connecting->initiativeConnectStatus, &outPara, INITIATIVECONNECT);
            connecting->initiativeConnectStatus = outPara;
            if (connecting->initiativeConnectStatus == CONNECTED) {
                AttConnectCompletedCallback(connecting, BREDR_CONNECT_SUCCESS);
            }
        } else if (connecting->initPassConnFlag == PASSIVECONNECT) {
            AttBredrConnectStatusChange(connecting->passiveConnectSatatus, &outPara, PASSIVECONNECT);
            connecting->passiveConnectSatatus = outPara;
            if (connecting->passiveConnectSatatus == CONNECTED) {
                AttConnectCompletedCallback(connecting, BREDR_CONNECT_SUCCESS);
            }
        }
    } else if (attRecevieConfigRspPtr->result == L2CAP_PENDING) {
        goto ATTRECVCONFIGRSP_END;
    } else {
        if (connecting->initPassConnFlag == INITIATIVECONNECT) {
            connecting->initiativeConnectStatus = DISCONNECTED;
        } else if (connecting->initPassConnFlag == PASSIVECONNECT) {
            connecting->passiveConnectSatatus = DISCONNECTED;
        }
        AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
    }

ATTRECVCONFIGRSP_END:
    MEM_MALLOC.free(attRecevieConfigRspPtr->info);
    MEM_MALLOC.free(attRecevieConfigRspPtr);
    return;
}

/**
 * @brief receive config response async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConfigRspAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConfigRspContext *attRecevieConfigRspPtr = (AttReceiveConfigRspContext *)context;

    MEM_MALLOC.free(attRecevieConfigRspPtr->info);
    MEM_MALLOC.free(attRecevieConfigRspPtr);

    return;
}

/**
 * @brief receive config response.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 cfg Indicates the pointer to const L2capConfigInfo.
 * @param3 result Indicates the result.
 * @param4 ctx Indicates the pointer to context.
 */
void AttReceiveConfigRsp(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, const void *ctx)
{
    LOG_INFO("%{public}s enter, lcid = %hu, mtu = %hu, flushTimeout = %hu, mode = %hhu, result = %hu",
        __FUNCTION__,
        lcid,
        cfg->mtu,
        cfg->flushTimeout,
        cfg->rfc.mode,
        result);

    L2capConfigInfo *configInfoPtr = NULL;
    AttReceiveConfigRspContext *attRecevieConfigRspPtr = NULL;

    configInfoPtr = MEM_MALLOC.alloc(sizeof(L2capConfigInfo));
    if (configInfoPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memcpy_s(configInfoPtr, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));

    attRecevieConfigRspPtr = MEM_MALLOC.alloc(sizeof(AttReceiveConfigRspContext));
    if (attRecevieConfigRspPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attRecevieConfigRspPtr->lcid = lcid;
    attRecevieConfigRspPtr->info = configInfoPtr;
    attRecevieConfigRspPtr->result = result;
    attRecevieConfigRspPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttReceiveConfigRspAsync, AttReceiveConfigRspAsyncDestroy, attRecevieConfigRspPtr);

    return;
}

/**
 * @brief receive disconnect req async.
 *
 * @param1 ctx Indicates the pointer to context.
 */
static void AttReceiveDisconnectionReqAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveDisconnectReqContext *attRecDisReqPtr = NULL;

    attRecDisReqPtr = (AttReceiveDisconnectReqContext *)context;

    L2CIF_DisconnectionRsp(attRecDisReqPtr->lcid, attRecDisReqPtr->id, DisconnectRespCallback);

    MEM_MALLOC.free(attRecDisReqPtr);

    return;
}

/**
 * @brief receive disconnect req async destroy.
 *
 * @param1 ctx Indicates the pointer to context.
 */
static void AttReceiveDisconnectionReqAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveDisconnectReqContext *attRecDisReqPtr = NULL;

    attRecDisReqPtr = (AttReceiveDisconnectReqContext *)context;

    MEM_MALLOC.free(attRecDisReqPtr);

    return;
}

/**
 * @brief receive disconnect request.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 id Indicates the id.
 * @param3 ctx Indicates the pointer to context.
 */
void AttReceiveDisconnectionReq(uint16_t lcid, uint8_t id, const void *ctx)
{
    LOG_INFO("%{public}s enter, lcid = %hu, id = %hhu", __FUNCTION__, lcid, id);

    AttReceiveDisconnectReqContext *attRecDisReqPtr = NULL;

    attRecDisReqPtr = MEM_MALLOC.alloc(sizeof(AttReceiveDisconnectReqContext));
    if (attRecDisReqPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attRecDisReqPtr->lcid = lcid;
    attRecDisReqPtr->id = id;
    attRecDisReqPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttReceiveDisconnectionReqAsync, AttReceiveDisconnectionReqAsyncDestroy, attRecDisReqPtr);

    return;
}

/**
 * @brief receive disconnect abnormal async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttDisconnectAbnormalAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttDisconnectAbnormalContext *attDisconnectAbnormalPtr = (AttDisconnectAbnormalContext *)context;
    AttBredrDisconnectCallback bredrDisConnObj;
    int listSize;
    AttConnectedCallback *attConnectCallback = NULL;
    AttConnectInfo *connect = NULL;

    if (attDisconnectAbnormalPtr->reason == L2CAP_STATE_COLLISION) {
        AttConnectingInfo *connecting = NULL;
        GapRequestSecurityParam gapReqSecurity;
        AttGetConnectingIndexByCid(attDisconnectAbnormalPtr->lcid, &connecting);
        connecting->cid = 0;
        AttAssignGAPRequestSecurity(&gapReqSecurity, OUTGOING, connecting);
        GAPIF_RequestSecurityAsync(&(connecting->addr), &gapReqSecurity);
    }

    AttGetConnectInfoIndexByCid(attDisconnectAbnormalPtr->lcid, &connect);
    if (connect == NULL) {
        goto ATTDISCONNECTABNORMAL_END;
    }

    AlarmCancel(connect->alarm);

    attConnectCallback = AttGetATTConnectCallback();
    if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attBREDRDisconnectCompleted == NULL)) {
        LOG_WARN("%{public}s attConnectCallback or attBREDRDisconnectCompleted is NULL", __FUNCTION__);
    } else {
        bredrDisConnObj.reason = DISCONNECT_ABNORMAL;
        attConnectCallback->attConnect.attBREDRDisconnectCompleted(
            connect->retGattConnectHandle, &bredrDisConnObj, attConnectCallback->context);
    }

    listSize = ListGetSize(connect->instruct);
    for (; listSize > 0; --listSize) {
        ListRemoveLast(connect->instruct);
    }
    AttClearConnectInfo(connect);

ATTDISCONNECTABNORMAL_END:
    MEM_MALLOC.free(attDisconnectAbnormalPtr);
    return;
}

/**
 * @brief receive disconnect abnormal async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttDisconnectAbnormalAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttDisconnectAbnormalContext *attDisconnectAbnormalPtr = (AttDisconnectAbnormalContext *)context;

    MEM_MALLOC.free(attDisconnectAbnormalPtr);

    return;
}

/**
 * @brief receive disconnect abnormal.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 reason Indicates the reason.
 * @param3 ctx Indicates the pointer to context.
 */
void AttDisconnectAbnormal(uint16_t lcid, uint8_t reason, const void *ctx)
{
    LOG_INFO("%{public}s enter,lcid = %hu, reason = %hhu", __FUNCTION__, lcid, reason);

    AttDisconnectAbnormalContext *attDisconnectAbnormalPtr = MEM_MALLOC.alloc(sizeof(AttDisconnectAbnormalContext));
    if (attDisconnectAbnormalPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attDisconnectAbnormalPtr->lcid = lcid;
    attDisconnectAbnormalPtr->reason = reason;
    attDisconnectAbnormalPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttDisconnectAbnormalAsync, AttDisconnectAbnormalAsyncDestroy, attDisconnectAbnormalPtr);

    return;
}

/**
 * @brief receive disconnect response async.
 *
 * @param2 ctx Indicates the pointer to context.
 */
static void AttRecvDisconnectionRspAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttRecvDisconnectionRspContext *attRecvDisconnRspPtr = (AttRecvDisconnectionRspContext *)context;
    AttBredrDisconnectCallback bredrDisConnObj;
    int listSize;
    AttConnectInfo *connect = NULL;
    AttConnectedCallback *attConnectCallback = NULL;

    AttGetConnectInfoIndexByCid(attRecvDisconnRspPtr->lcid, &connect);

    if (connect == NULL) {
        goto ATTRECVDISCONNECTIONRSP_END;
    }

    attConnectCallback = AttGetATTConnectCallback();

    AlarmCancel(connect->alarm);

    if ((attConnectCallback == NULL) || (attConnectCallback->attConnect.attBREDRDisconnectCompleted == NULL)) {
        LOG_WARN("%{public}s attConnectCallback or attBREDRDisconnectCompleted is NULL", __FUNCTION__);
    } else {
        bredrDisConnObj.reason = INITIATIVECONNECT_DISCONNECT_SUCCESS;
        attConnectCallback->attConnect.attBREDRDisconnectCompleted(
            connect->retGattConnectHandle, &bredrDisConnObj, attConnectCallback->context);
    }

    listSize = ListGetSize(connect->instruct);
    for (; listSize > 0; --listSize) {
        ListRemoveLast(connect->instruct);
    }

    AttClearConnectInfo(connect);

ATTRECVDISCONNECTIONRSP_END:
    MEM_MALLOC.free(attRecvDisconnRspPtr);
    return;
}

/**
 * @brief receive disconnect response async destroy.
 *
 * @param2 ctx Indicates the pointer to context.
 */
static void AttRecvDisconnectionRspAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttRecvDisconnectionRspContext *attRecvDisconnRspPtr = (AttRecvDisconnectionRspContext *)context;

    MEM_MALLOC.free(attRecvDisconnRspPtr);

    return;
}

/**
 * @brief receive disconnect response.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 ctx Indicates the pointer to context.
 */
void AttRecvDisconnectionRsp(uint16_t lcid, const void *ctx)
{
    LOG_INFO("%{public}s enter,lcid = %hu", __FUNCTION__, lcid);

    AttRecvDisconnectionRspContext *attRecvDisconnRspPtr = MEM_MALLOC.alloc(sizeof(AttRecvDisconnectionRspContext));
    if (attRecvDisconnRspPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attRecvDisconnRspPtr->lcid = lcid;
    attRecvDisconnRspPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttRecvDisconnectionRspAsync, AttRecvDisconnectionRspAsyncDestroy, attRecvDisconnRspPtr);

    return;
}

/**
 * @brief receive configrsp callback async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConfigRspCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConfigRspCallbackContext *configRspPtr = (ConfigRspCallbackContext *)context;
    AttConnectingInfo *connecting = NULL;
    uint8_t outPara = 0;
    int ret;

    AttGetConnectingIndexByCid(configRspPtr->lcid, &connecting);

    if (configRspPtr->result == BT_NO_ERROR) {
        if (connecting->initPassConnFlag == INITIATIVECONNECT) {
            ret = AttBredrConnectStatusChange(connecting->initiativeConnectStatus, &outPara, INITIATIVECONNECT);
            if (ret == BT_OPERATION_FAILED) {
                AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
                goto L2CIFCONFIGRSPCALLBACK_END;
            }
            connecting->initiativeConnectStatus = outPara;
            if (connecting->initiativeConnectStatus == CONNECTED) {
                AttConnectCompletedCallback(connecting, BREDR_CONNECT_SUCCESS);
            }
        } else if (connecting->initPassConnFlag == PASSIVECONNECT) {
            ret = AttBredrConnectStatusChange(connecting->passiveConnectSatatus, &outPara, PASSIVECONNECT);
            if (ret == BT_OPERATION_FAILED) {
                AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
                goto L2CIFCONFIGRSPCALLBACK_END;
            }
            connecting->passiveConnectSatatus = outPara;
            if (connecting->passiveConnectSatatus == CONNECTED) {
                AttConnectCompletedCallback(connecting, BREDR_CONNECT_SUCCESS);
            }
        }
    } else {
        AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
    }
L2CIFCONFIGRSPCALLBACK_END:
    MEM_MALLOC.free(configRspPtr);
    return;
}

/**
 * @brief receive configrsp callback async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void L2cifConfigRspCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConfigRspCallbackContext *configRspPtr = (ConfigRspCallbackContext *)context;

    MEM_MALLOC.free(configRspPtr);

    return;
}

/**
 * @brief receive configrsp callback.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 result Indicates the result.
 */
static void L2cifConfigRspCallback(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter,lcid = %hu, result = %{public}d", __FUNCTION__, lcid, result);

    ConfigRspCallbackContext *configRspPtr = MEM_MALLOC.alloc(sizeof(ConfigRspCallbackContext));
    if (configRspPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    configRspPtr->lcid = lcid;
    configRspPtr->result = result;

    AttAsyncProcess(L2cifConfigRspCallbackAsync, L2cifConfigRspCallbackAsyncDestroy, configRspPtr);

    return;
}

/**
 * @brief receive Config request async.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConfigReqAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConfigReqContext *attRecevieConfigReqPtr = (AttReceiveConfigReqContext *)context;
    AttConnectingInfo *connecting = NULL;

    AttGetConnectingIndexByCid(attRecevieConfigReqPtr->lcid, &connecting);

    if (connecting == NULL) {
        L2CIF_ConfigRsp(attRecevieConfigReqPtr->lcid,
            attRecevieConfigReqPtr->id,
            attRecevieConfigReqPtr->cfg,
            L2CAP_REJECTED,
            NULL);
        AttConnectCompletedCallback(connecting, BREDR_CONNECT_FAIL);
        goto ATTRECVCONFIGREQ_END;
    }

    AttRecvConfigReqAssignBredrConnect(attRecevieConfigReqPtr->cfg, connecting);

    if (attRecevieConfigReqPtr->cfg->rfc.mode != L2CAP_BASIC_MODE) {
        L2CIF_ConfigRsp(
            connecting->cid, attRecevieConfigReqPtr->id, &(connecting->remotel2capConfigInfoObj), L2CAP_REJECTED, NULL);
        goto ATTRECVCONFIGREQ_END;
    }

    L2CIF_ConfigRsp(connecting->cid,
        attRecevieConfigReqPtr->id,
        &(connecting->remotel2capConfigInfoObj),
        L2CAP_SUCCESS,
        L2cifConfigRspCallback);

ATTRECVCONFIGREQ_END:
    MEM_MALLOC.free(attRecevieConfigReqPtr->cfg);
    MEM_MALLOC.free(attRecevieConfigReqPtr);
    return;
}

/**
 * @brief receive Config request async destroy.
 *
 * @param ctx Indicates the pointer to context.
 */
static void AttReceiveConfigReqAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttReceiveConfigReqContext *attRecevieConfigReqPtr = (AttReceiveConfigReqContext *)context;

    MEM_MALLOC.free(attRecevieConfigReqPtr->cfg);
    MEM_MALLOC.free(attRecevieConfigReqPtr);

    return;
}

/**
 * @brief receive disconnect response.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 id Indicates the id.
 * @param3 cfg Indicates the pointer to const L2capConfigInfo.
 * @param4 ctx Indicates the pointer to context.
 */
void AttReceiveConfigReq(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, const void *ctx)
{
    LOG_INFO("%{public}s enter,lcid = %hu, id = %hhu, mtu = %hu, flushTimeout = %hu, mode = %hhu",
        __FUNCTION__,
        lcid,
        id,
        cfg->mtu,
        cfg->flushTimeout,
        cfg->rfc.mode);

    L2capConfigInfo *configInfoPtr = NULL;
    AttReceiveConfigReqContext *attRecevieConfigReqPtr = NULL;

    configInfoPtr = MEM_MALLOC.alloc(sizeof(L2capConfigInfo));
    if (configInfoPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memcpy_s(configInfoPtr, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));

    attRecevieConfigReqPtr = MEM_MALLOC.alloc(sizeof(AttReceiveConfigReqContext));
    attRecevieConfigReqPtr->lcid = lcid;
    attRecevieConfigReqPtr->id = id;
    attRecevieConfigReqPtr->cfg = configInfoPtr;
    attRecevieConfigReqPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttReceiveConfigReqAsync, AttReceiveConfigReqAsyncDestroy, attRecevieConfigReqPtr);

    return;
}

/**
 * @brief Initiative disconnect.
 *
 * @param1 connectHandle Att connectinfo handle.
 */
void InitiativeDisconnect(uint16_t connectHandle)
{
    LOG_DEBUG("%{public}s enter", __FUNCTION__);

    uint16_t transportType;
    uint16_t index = 0;
    AttConnectInfo *connect = NULL;
    AttConnectingInfo *connecting = NULL;
    AttGetConnectInfoIndexByConnectHandle(connectHandle, &index, &connect);
    if (connect == NULL) {
        LOG_WARN("InitiativeDisconnect connect == NULL");
        return;
    }

    transportType = connect->transportType;
    if (transportType == BT_TRANSPORT_BR_EDR) {
        AttGetConnectingIndexByAddrAclhandleCid(
            &(connect->addr), connect->aclHandle, connect->AttConnectID.bredrcid, &connecting);
    } else {
        AttGetConnectingIndexByAddrAclhandleCid(
            &(connect->addr), connect->aclHandle, connect->AttConnectID.lecid, &connecting);
    }

    if (connecting != NULL) {
        if (transportType == BT_TRANSPORT_BR_EDR) {
            AlarmCancel(connecting->bredrAlarm);
        }
    }

    if (transportType == BT_TRANSPORT_BR_EDR) {
        L2CIF_DisconnectionReq(connect->AttConnectID.bredrcid, L2cifBREDRDisconnectReqCallBack);
    } else if (transportType == BT_TRANSPORT_LE) {
        L2CIF_LeDisconnect(connect->aclHandle, LeDisconnectReqCallback);
    }

    return;
}

static void AttLeConnectCancelAsyn(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectingInfo *connecting = NULL;
    ConnectCancelContext *connectCancelPtr = (ConnectCancelContext *)context;
    AttGetConnectingIndexByAddr(&connectCancelPtr->addr, &connecting);

    if (connecting != NULL) {
        AttClearConnectingInfo(connecting);
    }

    L2CIF_LeConnectCancel(&connectCancelPtr->addr);
    MEM_MALLOC.free(connectCancelPtr);
    return;
}

static void AttLeConnectCancelAsyndestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    ConnectCancelContext *connectCancelPtr = (ConnectCancelContext *)context;
    MEM_MALLOC.free(connectCancelPtr);
}

int ATT_LeConnectCancel(const BtAddr *addr)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    ConnectCancelContext *context = MEM_MALLOC.alloc(sizeof(ConnectCancelContext));
    if (context == NULL) {
        return BT_NO_MEMORY;
    }

    context->addr.type = addr->type;
    (void)memcpy_s(context->addr.addr, ADDRESSLEN, addr->addr, ADDRESSLEN);
    AttAsyncProcess(AttLeConnectCancelAsyn, AttLeConnectCancelAsyndestroy, context);
    return BT_NO_ERROR;
}
