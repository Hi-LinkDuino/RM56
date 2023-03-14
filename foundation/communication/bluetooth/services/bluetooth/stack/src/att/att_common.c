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
 * @file att_common.c
 *
 * @brief implement common function to be called.
 *
 */

#include "att_common.h"
#include "att_connect.h"

#include <stdlib.h>

#include "alarm.h"
#include "log.h"

#include "platform/include/allocator.h"

#include "../btm/btm_thread.h"

static AttConnectInfo g_connectInfo[MAXCONNECT] = {0};
static AttConnectingInfo g_connecting[MAXCONNECT] = {0};
static AttClientDataCallback g_attClientCallback;
static AttServerDataCallback g_attServerCallback;
static AttClientSendDataCallback g_attClientSendDataCB;
static AttServerSendDataCallback g_attServerSendDataCB;
static AttConnectedCallback g_attConnect;
#define FUNCTIONLIST 256
recvDataFunction g_functionList[FUNCTIONLIST];

typedef struct LeRecvSendDataCallbackAsyncContext {
    uint16_t aclHandle;
    int result;
} LeRecvSendDataCallbackAsyncContext;

typedef struct BREDRRecvSendDataCallbackAsyncContext {
    uint16_t lcid;
    int result;
} BREDRRecvSendDataCallbackAsyncContext;

typedef struct TransactionTimeOutContext {
    uint16_t connectHandle;
} TransactionTimeOutContext;

typedef struct AttRecvDataAsyncContext {
    uint16_t lcid;
    Packet *packet;
    void *ctx;
} AttRecvDataAsyncContext;

typedef struct AttRecvLeDataAsyncContext {
    uint16_t aclHandle;
    Packet *packet;
} AttRecvLeDataAsyncContext;

typedef struct AttConnectRegisterContext {
    AttConnectCallback connectBack;
    void *context;
} AttConnectRegisterContext;

static void AttTransactionTimeOut(const void *parameter);

static void AttClientDataRegisterAsync(const void *context);
static void AttClientDataRegisterAsyncDestroy(const void *context);
static void AttClientDataDeregisterAsync(const void *context);
static void AttClientDataDeregisterAsyncDestroy(const void *context);

static void AttServerDataRegisterAsync(const void *context);
static void AttServerDataRegisterAsyncDestroy(const void *context);
static void AttServerDataDeregisterAsync(const void *context);
static void AttServerDataDeregisterAsyncDestroy(const void *context);

static void LeRecvSendDataCallbackAsync(const void *context);
static void LeRecvSendDataCallbackAsyncDestroy(const void *context);

static void BREDRRecvSendDataCallbackAsync(const void *context);
static void BREDRRecvSendDataCallbackAsyncDestroy(const void *context);

static void AttTransactionTimeOutAsync(const void *context);
static void AttTransactionTimeOutAsyncDestroy(const void *context);

static recvDataFunction GetFunction(uint8_t opcode);

static void AttRecvDataAsync(const void *context);
static void AttRecvDataAsyncDestroy(const void *context);
static void AttRecvLeDataAsync(const void *context);
static void AttRecvLeDataAsyncDestroy(const void *context);

static void AttConnectRegisterAsync(const void *context);
static void AttConnectRegisterAsyncDestroy(const void *context);
static void AttConnectDeregisterAsync(const void *context);
static void AttConnectDeregisterAsyncDestroy(const void *context);

static void AttBREDRSendRespCallbackAsync(const void *context);
static void AttBREDRSendRespCallbackAsyncDestroy(const void *context);
static void AttBREDRSendRespCallback(uint16_t lcid, int result);

static void AttLeSendRespCallbackAsync(const void *context);
static void AttLeSendRespCallbackAsyncDestroy(const void *context);
static void AttLeSendRespCallback(uint16_t aclHandle, int result);

static void AttTransactionTimeOutAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    TransactionTimeOutContext *transTimeOutPtr = (TransactionTimeOutContext *)context;
    AttConnectInfo *connect = NULL;
    int listSize;
    AttClientDataCallback *attClientDataCallback = NULL;
    AttServerDataCallback *attServerDataCallback = NULL;
    uint16_t index = 0;

    AttGetConnectInfoIndexByConnectHandle(transTimeOutPtr->connectHandle, &index, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL and goto ATTTRANSACTIONTIMEOUT_END", __FUNCTION__);
        goto ATTTRANSACTIONTIMEOUT_END;
    }

    if (!connect->serverSendFlag) {
        attClientDataCallback = AttGetATTClientCallback();
        if ((attClientDataCallback == NULL) || (attClientDataCallback->attClientCallback == NULL)) {
            LOG_WARN("%{public}s attClientDataCallback or attClientDataCallback->attClientCallback is NULL", __FUNCTION__);
        } else {
            attClientDataCallback->attClientCallback(
                connect->retGattConnectHandle, ATT_TRANSACTION_TIME_OUT_ID, NULL, NULL, attClientDataCallback->context);
        }
    } else {
        connect->serverSendFlag = false;
        attServerDataCallback = AttGetATTServerCallback();
        if ((attServerDataCallback == NULL) || (attServerDataCallback->attServerCallback == NULL)) {
            LOG_WARN("%{public}s attServerDataCallback or attServerDataCallback->attServerCallback is NULL", __FUNCTION__);
        } else {
            attServerDataCallback->attServerCallback(
                connect->retGattConnectHandle, ATT_TRANSACTION_TIME_OUT_ID, NULL, NULL, attServerDataCallback->context);
        }
    }

    InitiativeDisconnect(transTimeOutPtr->connectHandle);
    listSize = ListGetSize(connect->instruct);
    for (; listSize > 0; --listSize) {
        ListRemoveLast(connect->instruct);
    }

ATTTRANSACTIONTIMEOUT_END:
    MEM_MALLOC.free(transTimeOutPtr);
    return;
}

static void AttTransactionTimeOutAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    TransactionTimeOutContext *transTimeOutPtr = (TransactionTimeOutContext *)context;

    MEM_MALLOC.free(transTimeOutPtr);

    return;
}

/**
 * @brief att transaction timeout.
 *
 * @param parameter Indicates the pointer to parameter.
 */
static void AttTransactionTimeOut(const void *parameter)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectInfo *connect = (AttConnectInfo *)parameter;
    TransactionTimeOutContext *transTimeOutPtr = MEM_MALLOC.alloc(sizeof(TransactionTimeOutContext));
    if (transTimeOutPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    transTimeOutPtr->connectHandle = connect->retGattConnectHandle;

    AttAsyncProcess(AttTransactionTimeOutAsync, AttTransactionTimeOutAsyncDestroy, transTimeOutPtr);

    return;
}

/**
 * @brief get AttConnectInfo information.
 *
 * @return Returns the pointer to AttConnectInfo.
 */
AttConnectInfo *AttGetConnectStart()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return g_connectInfo;
}

/**
 * @brief lookup AttConnectInfo info by aclHandle.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 connect Indicates the second rank pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByAclHandle(uint16_t aclHandle, AttConnectInfo **connect)
{
    LOG_INFO("%{public}s enter, aclHandle = %hu", __FUNCTION__, aclHandle);

    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        if (g_connectInfo[index].aclHandle == aclHandle) {
            break;
        }
    }

    if (index != MAXCONNECT) {
        *connect = &g_connectInfo[index];
    } else {
        *connect = NULL;
    }

    LOG_INFO("%{public}s return: index = %hu", __FUNCTION__, index);
    return;
}

/**
 * @brief lookup AttConnectInfo info by cid.
 *
 * @param1 cid Indicates the cid.
 * @param2 connect Indicates the second rank pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByCid(uint16_t cid, AttConnectInfo **connect)
{
    LOG_INFO("%{public}s enter, cid = %hu", __FUNCTION__, cid);

    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        if (g_connectInfo[index].transportType == BT_TRANSPORT_BR_EDR) {
            if (g_connectInfo[index].AttConnectID.bredrcid == cid) {
                break;
            }
        }
        if (g_connectInfo[index].transportType == BT_TRANSPORT_LE) {
            if (g_connectInfo[index].aclHandle == cid) {
                break;
            }
        }
    }

    if (index != MAXCONNECT) {
        *connect = &g_connectInfo[index];
    } else {
        *connect = NULL;
    }

    LOG_INFO("%{public}s return: index = %hu", __FUNCTION__, index);
    return;
}

/**
 * @brief lookup AttConnectInfo info by cid and output parameter index.
 *
 * @param1 cid Indicates the cid.
 * @param2 index Indicates the pointer to index.
 * @param3 connect Indicates second rank pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByCidOutIndex(uint16_t cid, uint16_t *index, AttConnectInfo **connect)
{
    LOG_INFO("%{public}s enter,cid = %hu", __FUNCTION__, cid);

    uint16_t indexNumber = 0;

    for (; indexNumber < MAXCONNECT; ++indexNumber) {
        if (g_connectInfo[indexNumber].AttConnectID.bredrcid == cid) {
            break;
        }
    }

    *index = indexNumber;

    if (indexNumber != MAXCONNECT) {
        *connect = &g_connectInfo[indexNumber];
    } else {
        *connect = NULL;
    }

    LOG_INFO("%{public}s return: *index = %hu", __FUNCTION__, *index);
    return;
}

/**
 * @brief lookup AttConnectInfo info by connectHandle and output parameter index.
 *
 * @param1 connectHandle Indicates the connectHandle.
 * @param2 index Indicates the pointer to index.
 * @param3 connect Indicates the Secondary pointer to AttConnectInfo.
 */
void AttGetConnectInfoIndexByConnectHandle(uint16_t connectHandle, uint16_t *index, AttConnectInfo **connect)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu", __FUNCTION__, connectHandle);

    uint16_t inindex = 0;

    for (; inindex < MAXCONNECT; ++inindex) {
        if (g_connectInfo[inindex].retGattConnectHandle == connectHandle) {
            break;
        }
    }

    *index = inindex;

    if (inindex != MAXCONNECT) {
        *connect = &g_connectInfo[inindex];

        goto ATTGETCONNECTINFOINDEXBYCONNECTHANDLE_END;
    } else {
        *connect = NULL;
        *index = MAXCONNECT;
        goto ATTGETCONNECTINFOINDEXBYCONNECTHANDLE_END;
    }

ATTGETCONNECTINFOINDEXBYCONNECTHANDLE_END:
    LOG_INFO("%{public}s return: *index = %hu", __FUNCTION__, *index);
    return;
}

/**
 * @brief gatt register client data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttClientDataRegisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttClientDataCallback *attClientDataCallbackAsyncPtr = (AttClientDataCallback *)context;

    g_attClientCallback.attClientCallback = attClientDataCallbackAsyncPtr->attClientCallback;
    g_attClientCallback.context = attClientDataCallbackAsyncPtr->context;

    MEM_MALLOC.free(attClientDataCallbackAsyncPtr);

    return;
}

/**
 * @brief destroy gatt register client data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttClientDataRegisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttClientDataCallback *attClientDataCallbackAsyncPtr = (AttClientDataCallback *)context;

    MEM_MALLOC.free(attClientDataCallbackAsyncPtr);

    return;
}

/**
 * @brief gatt register client data to att.
 *
 * @param1 dataCallback Indicates the pointer to callback.
 * @param2 context Indicates the pointer to context.
 */
void ATT_ClientDataRegister(attCallback dataCallback, void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttClientDataCallback *attClientDataCallbackAsyncPtr = MEM_MALLOC.alloc(sizeof(AttClientDataCallback));
    if (attClientDataCallbackAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attClientDataCallbackAsyncPtr->attClientCallback = dataCallback;
    attClientDataCallbackAsyncPtr->context = context;

    AttAsyncProcess(AttClientDataRegisterAsync, AttClientDataRegisterAsyncDestroy, attClientDataCallbackAsyncPtr);

    return;
}

/**
 * @brief gatt deregister client data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttClientDataDeregisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    g_attClientCallback.attClientCallback = NULL;
    g_attClientCallback.context = NULL;

    return;
}

/**
 * @brief destroy gatt deregister client data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttClientDataDeregisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return;
}

/**
 * @brief gatt deregister client data to att.
 *
 */
void ATT_ClientDataDeregister()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttAsyncProcess(AttClientDataDeregisterAsync, AttClientDataDeregisterAsyncDestroy, NULL);

    return;
}

/**
 * @brief gatt register server data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttServerDataRegisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttServerDataCallback *attServerDataCallbackAsyncPtr = (AttServerDataCallback *)context;

    g_attServerCallback.attServerCallback = attServerDataCallbackAsyncPtr->attServerCallback;
    g_attServerCallback.context = attServerDataCallbackAsyncPtr->context;

    MEM_MALLOC.free(attServerDataCallbackAsyncPtr);

    return;
}

/**
 * @brief destroy gatt register server data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttServerDataRegisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttServerDataCallback *attServerDataCallbackAsyncPtr = (AttServerDataCallback *)context;

    MEM_MALLOC.free(attServerDataCallbackAsyncPtr);

    return;
}

/**
 * @brief gatt register server data to att.
 *
 * @param1 dataCallback Indicates the pointer to callback.
 * @param2 context Indicates the pointer to context.
 */
void ATT_ServerDataRegister(attCallback dataCallback, void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttServerDataCallback *attServerDataCallbackAsyncPtr = MEM_MALLOC.alloc(sizeof(AttServerDataCallback));
    if (attServerDataCallbackAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attServerDataCallbackAsyncPtr->attServerCallback = dataCallback;
    attServerDataCallbackAsyncPtr->context = context;

    AttAsyncProcess(AttServerDataRegisterAsync, AttServerDataRegisterAsyncDestroy, attServerDataCallbackAsyncPtr);

    return;
}

/**
 * @brief destroy gatt deregister server data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttServerDataDeregisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    g_attServerCallback.attServerCallback = NULL;
    g_attServerCallback.context = NULL;

    return;
}

/**
 * @brief destroy gatt deregister server data to att in self thread..
 *
 * @param context Indicates the pointer to context.
 */
static void AttServerDataDeregisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return;
}

/**
 * @brief gatt deregister server data to att.
 *
 */
void ATT_ServerDataDeregister()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttAsyncProcess(AttServerDataDeregisterAsync, AttServerDataDeregisterAsyncDestroy, NULL);

    return;
}

/**
 * @brief get AttClientDataCallback information.
 *
 * @return Returns the pointer to AttClientDataCallback.
 */
AttClientDataCallback *AttGetATTClientCallback()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return &g_attClientCallback;
}

/**
 * @brief get AttServerDataCallback information.
 *
 * @return Returns the pointer to AttServerDataCallback.
 */
AttServerDataCallback *AttGetATTServerCallback()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return &g_attServerCallback;
}

/**
 * @brief initiative execut instructions by Scheduling.
 *
 * @param connect Indicates the pointer to AttConnectInfo.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
int AttSendSequenceScheduling(const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter, listsize = %u", __FUNCTION__, ListGetSize(connect->instruct));

    int ret = BT_NO_ERROR;

    if (ListGetSize(connect->instruct) == 1) {
        ListNode *listNodePtr = ListGetFirstNode(connect->instruct);
        if (listNodePtr == NULL) {
            LOG_INFO("%{public}s listNodePtr == NULL", __FUNCTION__);
            ret = BT_OPERATION_FAILED;
            goto ATTSENDSEQUENCESCHEDULING_END;
        }
        Packet *packet = ListGetNodeData(listNodePtr);
        if (connect->transportType == BT_TRANSPORT_LE) {
            ret = L2CIF_LeSendFixChannelData(connect->aclHandle, (uint16_t)LE_CID, packet, LeRecvSendDataCallback);
        }
        if (connect->transportType == BT_TRANSPORT_BR_EDR) {
            ret = L2CIF_SendData(connect->AttConnectID.bredrcid, packet, BREDRRecvSendDataCallback);
        }
        if (ret != BT_NO_ERROR) {
            LOG_INFO("%{public}s call l2cap interface return not success", __FUNCTION__);
        } else {
            AlarmSet(
                connect->alarm, (uint64_t)INSTRUCTIONTIMEOUT, (void (*)(void *))AttTransactionTimeOut, (void *)connect);
        }
    }

ATTSENDSEQUENCESCHEDULING_END:
    return ret;
}

/**
 * @brief execut instructions by Scheduling after receiving response.
 *
 * @param connect Indicates the pointer to AttConnectInfo.
 */
void AttReceiveSequenceScheduling(const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter, listsize = %u, transportType = %hhu",
        __FUNCTION__,
        ListGetSize(connect->instruct),
        connect->transportType);

    int ret = BT_OPERATION_FAILED;

    if (ListGetSize(connect->instruct) > 0) {
        ListNode *listNodePtr = ListGetFirstNode(connect->instruct);
        if (listNodePtr == NULL) {
            LOG_INFO("%{public}s listNodePtr == NULL", __FUNCTION__);
            goto ATTRECEIVESEQUENCESCHEDULING_END;
        }
        Packet *PacketPtr = ListGetNodeData(listNodePtr);
        if (connect->transportType == BT_TRANSPORT_LE) {
            ret = L2CIF_LeSendFixChannelData(connect->aclHandle, (uint16_t)LE_CID, PacketPtr, LeRecvSendDataCallback);
        }
        if (connect->transportType == BT_TRANSPORT_BR_EDR) {
            ret = L2CIF_SendData(connect->AttConnectID.bredrcid, PacketPtr, BREDRRecvSendDataCallback);
        }

        if (ret != BT_NO_ERROR) {
            LOG_INFO("%{public}s call l2cap interface return not success", __FUNCTION__);
        } else {
            AlarmSet(connect->alarm,
                (uint64_t)INSTRUCTIONTIMEOUT,
                (void (*)(void *))AttTransactionTimeOut,
                (AttConnectInfo *)connect);
        }
    }

ATTRECEIVESEQUENCESCHEDULING_END:
    return;
}

/**
 * @brief get AttConnectingInfo information.
 *
 * @return Returns the pointer to AttConnectingInfo.
 */
AttConnectingInfo *AttGetConnectingStart()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return g_connecting;
}

/**
 * @brief lookup AttConnectingInfo info by cid.
 *
 * @param1 cid Indicates the cid.
 * @param2 connecting Indicates the Secondary pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByCid(uint16_t cid, AttConnectingInfo **connecting)
{
    LOG_INFO("%{public}s enter, cid = %hu", __FUNCTION__, cid);

    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        if (g_connecting[index].cid == cid) {
            break;
        }
    }

    if (index != MAXCONNECT) {
        *connecting = &g_connecting[index];
        goto ATTGETCONNECTINGINDEXBYCID_END;
    } else {
        *connecting = NULL;
        goto ATTGETCONNECTINGINDEXBYCID_END;
    }

ATTGETCONNECTINGINDEXBYCID_END:
    LOG_INFO("%{public}s return: index = %hu", __FUNCTION__, index);
    return;
}

/**
 * @brief lookup AttConnectingInfo info by cid and output parameter index.
 *
 * @param1 cid Indicates the cid.
 * @param2 index Indicates the pointer to index.
 * @param3 connecting Indicates the Secondary pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByCidOutIndex(uint16_t cid, uint16_t *index, AttConnectingInfo **connecting)
{
    LOG_INFO("%{public}s enter,cid = %hu", __FUNCTION__, cid);

    uint16_t indexNumber = 0;

    for (; indexNumber < MAXCONNECT; ++indexNumber) {
        if (g_connecting[indexNumber].cid == cid) {
            break;
        }
    }

    if (indexNumber != MAXCONNECT) {
        *connecting = &g_connecting[indexNumber];
        *index = indexNumber;
        goto ATTGETCONNECTINGINDEXBYCIDOUTINDEX_END;
    } else {
        *connecting = NULL;
        *index = MAXCONNECT;
        goto ATTGETCONNECTINGINDEXBYCIDOUTINDEX_END;
    }

ATTGETCONNECTINGINDEXBYCIDOUTINDEX_END:
    LOG_INFO("%{public}s return: *index = %hu", __FUNCTION__, *index);
    return;
}

/**
 * @brief lookup AttConnectingInfo info by connectHandle.
 *
 * @param1 connectHandle Indicates the connectHandle.
 * @param2 connecting Indicates the Secondary pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByConnectHandle(uint16_t connectHandle, AttConnectingInfo **connecting)
{
    LOG_INFO("%{public}s enter, connectHandle = %hu", __FUNCTION__, connectHandle);

    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        if (g_connecting[index].connectHandle == connectHandle) {
            break;
        }
    }

    if (index != MAXCONNECT) {
        *connecting = &g_connecting[index];
        goto ATTGETCONNECTINGINDEXBYCONNECTHANDLE_END;
    } else {
        *connecting = NULL;
        goto ATTGETCONNECTINGINDEXBYCONNECTHANDLE_END;
    }

ATTGETCONNECTINGINDEXBYCONNECTHANDLE_END:
    LOG_INFO("%{public}s return: index = %hu", __FUNCTION__, index);
    return;
}

/**
 * @brief lookup AttConnectingInfo info by cid and connectHandle, result to output parameter index.
 *
 * @param1 cid Indicates the cid.
 * @param2 connectHandle Indicates the connectHandle.
 * @param3 index Indicates the pointer to index.
 * @param4 connecting Indicates the Secondary pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByCidConnectHandle(
    uint16_t cid, uint16_t connectHandle, uint16_t *index, AttConnectingInfo **connecting)
{
    LOG_INFO("%{public}s enter, cid = %hu,connectHandle = %hu", __FUNCTION__, cid, connectHandle);

    uint16_t inindex = 0;

    for (; inindex < MAXCONNECT; ++inindex) {
        if ((g_connecting[inindex].cid == cid) && (g_connecting[inindex].connectHandle == connectHandle)) {
            break;
        }
    }

    *index = inindex;

    if (inindex != MAXCONNECT) {
        *connecting = &g_connecting[inindex];
        goto ATTGETCONNECTINGINDEXBYCIDCONNECTHANDLE_END;
    } else {
        *connecting = NULL;
        goto ATTGETCONNECTINGINDEXBYCIDCONNECTHANDLE_END;
    }

ATTGETCONNECTINGINDEXBYCIDCONNECTHANDLE_END:
    LOG_INFO("%{public}s return: *index = %hu", __FUNCTION__, *index);
    return;
}

/**
 * @brief lookup AttConnectingInfo info by addr.
 *
 * @param1 addr Indicates the pointer to const BtAddr.
 * @param2 connecting Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByAddr(const BtAddr *addr, AttConnectingInfo **connecting)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        if (!memcmp(g_connecting[index].addr.addr, addr->addr, ADDRESSLEN)) {
            break;
        }
    }

    if (index != MAXCONNECT) {
        *connecting = &g_connecting[index];
        goto ATTGETCONNECTINGINDEXBYADDR_END;
    } else {
        *connecting = NULL;
        goto ATTGETCONNECTINGINDEXBYADDR_END;
    }

ATTGETCONNECTINGINDEXBYADDR_END:
    LOG_INFO("%{public}s return: index = %hu", __FUNCTION__, index);
    return;
}

/**
 * @brief lookup AttConnectingInfo info by addr cid.
 *
 * @param1 addr Indicates the pointer to addr.
 * @param2 addr Indicates the cid.
 * @param3 connect Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByAddrUninitializedCid(const BtAddr *addr, AttConnectingInfo **connecting)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        if ((!memcmp(g_connecting[index].addr.addr, addr->addr, ADDRESSLEN)) && (g_connecting[index].cid == 0)) {
            break;
        }
    }

    LOG_INFO("%{public}s,index = %hu", __FUNCTION__, index);

    if (index != MAXCONNECT) {
        *connecting = &g_connecting[index];
        goto ATTGETCONNECTINGINDEXBYADDRUNINITIALIZEDCID_END;
    } else {
        *connecting = NULL;
        goto ATTGETCONNECTINGINDEXBYADDRUNINITIALIZEDCID_END;
    }

ATTGETCONNECTINGINDEXBYADDRUNINITIALIZEDCID_END:
    LOG_INFO("%{public}s return: index = %hu", __FUNCTION__, index);
    return;
}

/**
 * @brief lookup AttConnectingInfo info by addr aclhandle cid.
 *
 * @param1 addr Indicates pointer to addr.
 * @param2 addr Indicates the aclHandle.
 * @param3 addr Indicates the cid.
 * @param2 connect Indicates the second rank pointer to AttConnectingInfo.
 */
void AttGetConnectingIndexByAddrAclhandleCid(
    const BtAddr *addr, uint16_t aclHandle, uint16_t cid, AttConnectingInfo **connecting)
{
    LOG_INFO("%{public}s enter, aclHandle = %hu, cid = %hu", __FUNCTION__, aclHandle, cid);
    uint16_t index = 0;

    for (; index < MAXCONNECT; ++index) {
        if ((!memcmp(g_connecting[index].addr.addr, addr->addr, ADDRESSLEN)) &&
            (g_connecting[index].aclHandle == aclHandle) && (g_connecting[index].cid == cid)) {
            break;
        }
    }

    if (index != MAXCONNECT) {
        *connecting = &g_connecting[index];
        goto ATTGETCONNECTINGINDEXBYADDRACLHANDLECID_END;
    } else {
        *connecting = NULL;
        goto ATTGETCONNECTINGINDEXBYADDRACLHANDLECID_END;
    }

ATTGETCONNECTINGINDEXBYADDRACLHANDLECID_END:
    LOG_INFO("%{public}s return: index = %hu", __FUNCTION__, index);
    return;
}

/**
 * @brief client call back copy.
 *
 * @param1 attSendDataCb Indicates the pointer to attSendDataCallback.
 * @param2 context Indicates the pointer to context.
 */
void AttClientCallBackCopyToCommon(attSendDataCallback attSendDataCB, const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    g_attClientSendDataCB.attSendDataCB = attSendDataCB;
    g_attClientSendDataCB.context = (void *)context;

    return;
}
/**
 * @brief server call back copy.
 *
 * @param1 attSendDataCb Indicates the pointer to attSendDataCallback.
 * @param2 context Indicates the pointer to context.
 */
void AttServerCallBackCopyToCommon(attSendDataCallback attSendDataCB, const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    g_attServerSendDataCB.attSendDataCB = attSendDataCB;
    g_attServerSendDataCB.context = (void *)context;

    return;
}

/**
 * @brief le receive senddata callback async.
 *
 * @param context Indicates the pointer to context.
 */
static void LeRecvSendDataCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    LeRecvSendDataCallbackAsyncContext *leRecvSendDataCallPtr = (LeRecvSendDataCallbackAsyncContext *)context;
    AttConnectInfo *connect = NULL;

    AttGetConnectInfoIndexByCid(leRecvSendDataCallPtr->aclHandle, &connect);

    if (connect == NULL) {
        goto RECVSENDDATACALLBACK_END;
    }

    if (g_attClientSendDataCB.attSendDataCB != NULL) {
        g_attClientSendDataCB.attSendDataCB(
            connect->retGattConnectHandle, leRecvSendDataCallPtr->result, g_attClientSendDataCB.context);
    }
    if (leRecvSendDataCallPtr->result == BT_NO_ERROR) {
        AlarmSet(connect->alarm, (uint64_t)INSTRUCTIONTIMEOUT, (void (*)(void *))AttTransactionTimeOut, connect);
    } else {
        LOG_WARN("L2CAP error code = %{public}d", leRecvSendDataCallPtr->result);
        AlarmCancel(connect->alarm);
    }

RECVSENDDATACALLBACK_END:
    MEM_MALLOC.free(leRecvSendDataCallPtr);
    return;
}

/**
 * @brief le receive senddata callback async destroy.
 *
 * @param context Indicates the pointer to context.
 */
static void LeRecvSendDataCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    LeRecvSendDataCallbackAsyncContext *leRecvSendDataCallPtr = (LeRecvSendDataCallbackAsyncContext *)context;

    MEM_MALLOC.free(leRecvSendDataCallPtr);

    return;
}

/**
 * @brief receive senddata callback.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 result Indicates the result.
 */
void LeRecvSendDataCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    LeRecvSendDataCallbackAsyncContext *leRecvSendDataCallPtr =
        MEM_MALLOC.alloc(sizeof(LeRecvSendDataCallbackAsyncContext));
    if (leRecvSendDataCallPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    leRecvSendDataCallPtr->aclHandle = aclHandle;
    leRecvSendDataCallPtr->result = result;

    AttAsyncProcess(LeRecvSendDataCallbackAsync, LeRecvSendDataCallbackAsyncDestroy, leRecvSendDataCallPtr);

    return;
}

/**
 * @brief BREDR receive senddata callback async.
 *
 * @param context Indicates the pointer to context.
 */
static void BREDRRecvSendDataCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BREDRRecvSendDataCallbackAsyncContext *bredrRecvSendDataCallPtr = (BREDRRecvSendDataCallbackAsyncContext *)context;
    AttConnectInfo *connect = NULL;

    AttGetConnectInfoIndexByCid(bredrRecvSendDataCallPtr->lcid, &connect);

    if (connect == NULL) {
        goto BREDRRECVSENDDATACALLBACK_END;
    }

    if (g_attClientSendDataCB.attSendDataCB != NULL) {
        g_attClientSendDataCB.attSendDataCB(
            connect->retGattConnectHandle, bredrRecvSendDataCallPtr->result, g_attClientSendDataCB.context);
    }
    if (bredrRecvSendDataCallPtr->result == BT_NO_ERROR) {
        AlarmSet(connect->alarm, (uint64_t)INSTRUCTIONTIMEOUT, (void (*)(void *))AttTransactionTimeOut, connect);
    } else {
        LOG_WARN("L2CAP error code = %{public}d", bredrRecvSendDataCallPtr->result);
        AlarmCancel(connect->alarm);
    }

BREDRRECVSENDDATACALLBACK_END:
    MEM_MALLOC.free(bredrRecvSendDataCallPtr);
    return;
}

/**
 * @brief BREDR receive senddata callback async destroy.
 *
 * @param context Indicates the pointer to context.
 */
static void BREDRRecvSendDataCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BREDRRecvSendDataCallbackAsyncContext *bredrRecvSendDataCallPtr = (BREDRRecvSendDataCallbackAsyncContext *)context;

    MEM_MALLOC.free(bredrRecvSendDataCallPtr);

    return;
}

/**
 * @brief receive senddata callback.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 result Indicates the result.
 * @param3 context Indicates the pointer to context.
 */
void BREDRRecvSendDataCallback(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BREDRRecvSendDataCallbackAsyncContext *bredrSendDataCallPtr =
        MEM_MALLOC.alloc(sizeof(BREDRRecvSendDataCallbackAsyncContext));
    if (bredrSendDataCallPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    bredrSendDataCallPtr->lcid = lcid;
    bredrSendDataCallPtr->result = result;

    AttAsyncProcess(BREDRRecvSendDataCallbackAsync, BREDRRecvSendDataCallbackAsyncDestroy, bredrSendDataCallPtr);

    return;
}

/**
 * @brief receive delect callback.
 *
 */
void AttCallBackDelectCopyToCommon()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    g_attServerSendDataCB.attSendDataCB = NULL;
    g_attServerSendDataCB.context = NULL;

    return;
}

/**
 * @brief switch thread.
 *
 * @param1 callback Indicates the pointer to function pointer.
 * @param2 destroyCallback Indicates the pointer to function pointer.
 * @param3 context Indicates the pointer to context.
 */
void AttAsyncProcess(
    void (*callback)(const void *context), void (*destroyCallback)(const void *context), const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_ATT, (void (*)(void *))callback, (void *)context);
    if (ret != BT_NO_ERROR) {
        if (destroyCallback != NULL) {
            destroyCallback(context);
        }
    }

    return;
}

/**
 * @brief shut down clear connect information.
 *
 * @param connectInfo Indicates the pointer to AttConnectInfo.
 */
void AttShutDownClearConnectInfo(AttConnectInfo *connectInfo)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    connectInfo->aclHandle = 0;
    (void)memset_s(&connectInfo->AttConnectID, sizeof(connectInfo->AttConnectID), 0, sizeof(connectInfo->AttConnectID));
    connectInfo->retGattConnectHandle = 0;
    connectInfo->transportType = 0;
    (void)memset_s(&connectInfo->addr, sizeof(connectInfo->addr), 0, sizeof(BtAddr));
    connectInfo->mtu = 0;
    connectInfo->sendMtu = 0;
    connectInfo->receiveMtu = 0;
    connectInfo->mtuFlag = false;
    connectInfo->initPassConnFlag = 0;
    connectInfo->serverSendFlag = false;

    g_attClientCallback.attClientCallback = NULL;
    g_attServerCallback.attServerCallback = NULL;
    g_attClientSendDataCB.attSendDataCB = NULL;
    g_attServerSendDataCB.attSendDataCB = NULL;
    g_attConnect.attConnect.attBREDRConnectCompleted = NULL;
    g_attConnect.attConnect.attBREDRConnectInd = NULL;
    g_attConnect.attConnect.attBREDRDisconnectCompleted = NULL;
    g_attConnect.attConnect.attLEConnectCompleted = NULL;
    g_attConnect.attConnect.attLEDisconnectCompleted = NULL;

    return;
}

/**
 * @brief client callback btbadparam.
 *
 */
void ClientCallbackBTBADPARAM(const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    if (connect == NULL) {
        return;
    }

    if (g_attClientSendDataCB.attSendDataCB != NULL) {
        g_attClientSendDataCB.attSendDataCB(connect->retGattConnectHandle, BT_BAD_PARAM, g_attClientSendDataCB.context);
    }

    return;
}

/**
 * @brief server callback btbadparam.
 *
 */
void ServerCallbackBTBADPARAM(const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    if (g_attServerSendDataCB.attSendDataCB != NULL) {
        g_attServerSendDataCB.attSendDataCB(connect->retGattConnectHandle, BT_BAD_PARAM, g_attServerSendDataCB.context);
    }

    return;
}

/**
 * @brief client callback return value.
 *
 * @param1 ret Indicates the ret.
 * @param2 connect Indicates the pointer of AttConnectInfo.
 */
void ClientCallbackReturnValue(int ret, const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    if (ret != BT_NO_ERROR) {
        if (g_attClientSendDataCB.attSendDataCB != NULL) {
            g_attClientSendDataCB.attSendDataCB(connect->retGattConnectHandle, ret, g_attClientSendDataCB.context);
        }
    }

    return;
}

/**
 * @brief server callback return value.
 *
 * @param1 ret Indicates the ret.
 * @param2 connect Indicates the pointer of AttConnectInfo.
 */
void ServerCallbackReturnValue(int ret, const AttConnectInfo *connect)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    if (ret != BT_NO_ERROR) {
        if (g_attServerSendDataCB.attSendDataCB != NULL) {
            g_attServerSendDataCB.attSendDataCB(connect->retGattConnectHandle, ret, g_attServerSendDataCB.context);
        }
    }

    return;
}

/**
 * @brief receive  bredr connect instructions data in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttRecvDataAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint8_t opcode = 0;
    AttConnectInfo *connect = NULL;
    AttRecvDataAsyncContext *attRecvDataAsyncPtr = (AttRecvDataAsyncContext *)context;
    AttGetConnectInfoIndexByCid(attRecvDataAsyncPtr->lcid, &connect);
    if (connect != NULL) {
        PacketExtractHead(attRecvDataAsyncPtr->packet, &opcode, sizeof(uint8_t));
        Buffer *buffer = PacketContinuousPayload(attRecvDataAsyncPtr->packet);
        recvDataFunction functionPtr = GetFunction(opcode);
        if (functionPtr != NULL) {
            functionPtr(connect, buffer);
        } else {
            LOG_WARN("%{public}s UnKnow OpCode : %hhu", __FUNCTION__, opcode);
            if ((opcode & 0b01000000) == 0) {
                AttErrorCode(connect, opcode);
            }
        }
    }

    PacketFree(attRecvDataAsyncPtr->packet);
    MEM_MALLOC.free(attRecvDataAsyncPtr);

    return;
}

/**
 * @brief destroy receive bredr connect instructions data in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttRecvDataAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttRecvDataAsyncContext *attRecvDataAsyncPtr = (AttRecvDataAsyncContext *)context;

    PacketFree(attRecvDataAsyncPtr->packet);
    MEM_MALLOC.free(attRecvDataAsyncPtr);

    return;
}

/**
 * @brief receive  bredr connect instructions data.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 packet Indicates the pointer to Packet.
 * @param3 ctx Indicates the pointer to  context.
 */
void AttRecvData(uint16_t lcid, const Packet *packet, const void *ctx)
{
    LOG_INFO("%{public}s enter,lcid = %hu", __FUNCTION__, lcid);

    Packet *packetPtr = PacketRefMalloc((Packet *)packet);
    AttRecvDataAsyncContext *attRecvDataAsyncPtr = MEM_MALLOC.alloc(sizeof(AttRecvDataAsyncContext));
    if (attRecvDataAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attRecvDataAsyncPtr->lcid = lcid;
    attRecvDataAsyncPtr->packet = packetPtr;
    attRecvDataAsyncPtr->ctx = (void *)ctx;

    AttAsyncProcess(AttRecvDataAsync, AttRecvDataAsyncDestroy, attRecvDataAsyncPtr);

    return;
}

/**
 * @brief receive le connect instructions data in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttRecvLeDataAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    uint8_t opcode = 0;
    AttConnectInfo *connect = NULL;
    AttRecvLeDataAsyncContext *attRecvLeDataAsyncPtr = (AttRecvLeDataAsyncContext *)context;
    AttGetConnectInfoIndexByAclHandle(attRecvLeDataAsyncPtr->aclHandle, &connect);
    if (connect != NULL) {
        PacketExtractHead(attRecvLeDataAsyncPtr->packet, &opcode, sizeof(uint8_t));
        Buffer *buffer = PacketContinuousPayload(attRecvLeDataAsyncPtr->packet);
        recvDataFunction functionPtr = GetFunction(opcode);
        if (functionPtr != NULL) {
            functionPtr(connect, buffer);
        } else {
            LOG_WARN("UnKnow OpCode : %hhu", opcode);
            if ((opcode & 0b01000000) == 0) {
                AttErrorCode(connect, opcode);
            }
        }
    }

    PacketFree(attRecvLeDataAsyncPtr->packet);
    MEM_MALLOC.free(attRecvLeDataAsyncPtr);

    return;
}

/**
 * @brief destroy le bredr connect instructions data in self thread.
 *
 * @param context Indicates the pointer to context.
 */
static void AttRecvLeDataAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttRecvLeDataAsyncContext *attRecvLeDataAsyncPtr = (AttRecvLeDataAsyncContext *)context;

    PacketFree(attRecvLeDataAsyncPtr->packet);
    MEM_MALLOC.free(attRecvLeDataAsyncPtr);

    return;
}

/**
 * @brief receive  le connect instructions data.
 *
 * @param1 aclHandle Indicates the aclHandle.
 * @param2 packet Indicates the pointer to Packet.
 */
void AttRecvLeData(uint16_t aclHandle, const Packet *packet)
{
    LOG_INFO("%{public}s enter, aclHandle = %hu", __FUNCTION__, aclHandle);

    Packet *packetPtr = PacketRefMalloc((Packet *)packet);
    AttRecvLeDataAsyncContext *attRecvLeDataAsyncPtr = MEM_MALLOC.alloc(sizeof(AttRecvLeDataAsyncContext));
    if (attRecvLeDataAsyncPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    attRecvLeDataAsyncPtr->aclHandle = aclHandle;
    attRecvLeDataAsyncPtr->packet = packetPtr;

    AttAsyncProcess(AttRecvLeDataAsync, AttRecvLeDataAsyncDestroy, attRecvLeDataAsyncPtr);

    return;
}

/**
 * @brief get function.
 *
 * @param1 opcode Indicates the opcode.
 * @return Returns the recvDataFunction.
 */
static recvDataFunction GetFunction(uint8_t opcode)
{
    return g_functionList[opcode];
}

/**
 * @brief get function array dress.
 *
 * @return Returns the pointer to recvDataFunction.
 */
recvDataFunction *GetFunctionArrayDress()
{
    return g_functionList;
}

/**
 * @brief get att connect callback.
 *
 * @return Returns the pointer to AttConnectedCallback.
 */
AttConnectedCallback *AttGetATTConnectCallback()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return &g_attConnect;
}

static void AttConnectRegisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectRegisterContext *attConnRegPtr = (AttConnectRegisterContext *)context;
    AttConnectedCallback *attConnectCallback = AttGetATTConnectCallback();

    attConnectCallback->attConnect = attConnRegPtr->connectBack;
    attConnectCallback->context = attConnRegPtr->context;

    MEM_MALLOC.free(attConnRegPtr);

    return;
}

static void AttConnectRegisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectRegisterContext *attConnRegPtr = (AttConnectRegisterContext *)context;

    MEM_MALLOC.free(attConnRegPtr);

    return;
}

/**
 * @brief gatt register connect to att.
 *
 * @param1 connectBack Indicates the struct to callback.
 * @param2 context Indicates the pointer to context.
 */
void ATT_ConnectRegister(AttConnectCallback connectBack, void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectRegisterContext *attConnRegPtr = MEM_MALLOC.alloc(sizeof(AttConnectRegisterContext));
    if (attConnRegPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    (void)memcpy_s(
        &(attConnRegPtr->connectBack), sizeof(attConnRegPtr->connectBack), &connectBack, sizeof(AttConnectCallback));
    attConnRegPtr->context = context;

    AttAsyncProcess(AttConnectRegisterAsync, AttConnectRegisterAsyncDestroy, attConnRegPtr);

    return;
}

static void AttConnectDeregisterAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttConnectedCallback *attConnectCallback = AttGetATTConnectCallback();

    attConnectCallback->attConnect.attLEConnectCompleted = NULL;
    attConnectCallback->attConnect.attLEDisconnectCompleted = NULL;
    attConnectCallback->attConnect.attBREDRConnectCompleted = NULL;
    attConnectCallback->attConnect.attBREDRDisconnectCompleted = NULL;
    attConnectCallback->attConnect.attBREDRConnectInd = NULL;
    attConnectCallback->context = NULL;

    return;
}

static void AttConnectDeregisterAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    return;
}

/**
 * @brief gatt deregister connect to att.
 *
 */
void ATT_ConnectDeregister()
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    AttAsyncProcess(AttConnectDeregisterAsync, AttConnectDeregisterAsyncDestroy, NULL);

    return;
}

static void AttBREDRSendRespCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BREDRRecvSendDataCallbackAsyncContext *attBredrSendRspPtr = (BREDRRecvSendDataCallbackAsyncContext *)context;
    AttConnectInfo *connect = NULL;

    AttGetConnectInfoIndexByCid(attBredrSendRspPtr->lcid, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL", __FUNCTION__);
        goto ATTBREDRSENDRESPCALLBACK_END;
    }

    if (attBredrSendRspPtr->result != BT_NO_ERROR) {
        LOG_WARN("L2CAP Send Resp error ,error code = %{public}d", attBredrSendRspPtr->result);
    }

    if (g_attServerSendDataCB.attSendDataCB != NULL) {
        g_attServerSendDataCB.attSendDataCB(
            connect->retGattConnectHandle, attBredrSendRspPtr->result, g_attServerSendDataCB.context);
    }

ATTBREDRSENDRESPCALLBACK_END:
    MEM_MALLOC.free(attBredrSendRspPtr);
    return;
}

static void AttBREDRSendRespCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BREDRRecvSendDataCallbackAsyncContext *attBredrSendRspPtr = (BREDRRecvSendDataCallbackAsyncContext *)context;

    MEM_MALLOC.free(attBredrSendRspPtr);

    return;
}

/**
 * @brief callback of send response.
 *
 * @param1 lcid Indicates the lcid.
 * @param2 result Indicates the result.
 */
static void AttBREDRSendRespCallback(uint16_t lcid, int result)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    BREDRRecvSendDataCallbackAsyncContext *attBredrSendRspPtr =
        MEM_MALLOC.alloc(sizeof(BREDRRecvSendDataCallbackAsyncContext));
    if (attBredrSendRspPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attBredrSendRspPtr->lcid = lcid;
    attBredrSendRspPtr->result = result;

    AttAsyncProcess(AttBREDRSendRespCallbackAsync, AttBREDRSendRespCallbackAsyncDestroy, attBredrSendRspPtr);

    return;
}

static void AttLeSendRespCallbackAsync(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    LeRecvSendDataCallbackAsyncContext *attLeSendRspPtr = (LeRecvSendDataCallbackAsyncContext *)context;
    AttConnectInfo *connect = NULL;

    AttGetConnectInfoIndexByCid(attLeSendRspPtr->aclHandle, &connect);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL", __FUNCTION__);
        goto ATTLESENDRESPCALLBACK_END;
    }

    if (attLeSendRspPtr->result != BT_NO_ERROR) {
        LOG_WARN("L2CAP Send Resp error ,error code = %{public}d", attLeSendRspPtr->result);
    }

    if (g_attServerSendDataCB.attSendDataCB != NULL) {
        g_attServerSendDataCB.attSendDataCB(
            connect->retGattConnectHandle, attLeSendRspPtr->result, g_attServerSendDataCB.context);
    }

ATTLESENDRESPCALLBACK_END:
    MEM_MALLOC.free(attLeSendRspPtr);
    return;
}

static void AttLeSendRespCallbackAsyncDestroy(const void *context)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    LeRecvSendDataCallbackAsyncContext *attLeSendRspPtr = (LeRecvSendDataCallbackAsyncContext *)context;

    MEM_MALLOC.free(attLeSendRspPtr);

    return;
}

static void AttLeSendRespCallback(uint16_t aclHandle, int result)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    LeRecvSendDataCallbackAsyncContext *attLeSendRspPtr = MEM_MALLOC.alloc(sizeof(LeRecvSendDataCallbackAsyncContext));
    if (attLeSendRspPtr == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    attLeSendRspPtr->aclHandle = aclHandle;
    attLeSendRspPtr->result = result;

    AttAsyncProcess(AttLeSendRespCallbackAsync, AttLeSendRespCallbackAsyncDestroy, attLeSendRspPtr);

    return;
}

/**
 * @brief call l2cap interface to send data.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 packet Indicates the pointer to Packet.
 * @return Returns <b>0</b> if the operation is successful; returns <b>!0</b> if the operation fails.
 */
int AttResponseSendData(const AttConnectInfo *connect, const Packet *packet)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    int ret = BT_OPERATION_FAILED;

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL", __FUNCTION__);
        ret = BT_BAD_PARAM;
        return ret;
    }

    if (connect->transportType == BT_TRANSPORT_BR_EDR) {
        ret = L2CIF_SendData(connect->AttConnectID.bredrcid, (Packet *)packet, AttBREDRSendRespCallback);
    }
    if (connect->transportType == BT_TRANSPORT_LE) {
        ret = L2CIF_LeSendFixChannelData(connect->aclHandle, (uint16_t)LE_CID, (Packet *)packet, AttLeSendRespCallback);
    }

    return ret;
}

/**
 * @brief callback error code.
 *
 * @param1 connect Indicates the pointer to const AttConnectInfo.
 * @param2 opcode Indicates the opcode.
 */
void AttErrorCode(const AttConnectInfo *connect, uint8_t opcode)
{
    LOG_INFO("%{public}s enter", __FUNCTION__);

    if (connect == NULL) {
        LOG_INFO("%{public}s connect == NULL", __FUNCTION__);
        goto ATTERRORCODE_END;
    }

    AttServerDataCallback *attServerDataCallback = NULL;
    uint8_t errorCode = opcode;
    attServerDataCallback = AttGetATTServerCallback();
    attServerDataCallback->attServerCallback(
        connect->retGattConnectHandle, ATT_UNKNOW_OPCODE_ID, &errorCode, NULL, attServerDataCallback->context);

ATTERRORCODE_END:
    return;
}

uint16_t Min(uint16_t param1, uint16_t param2)
{
    if (param1 < param2) {
        return param1;
    } else {
        return param2;
    }
}
