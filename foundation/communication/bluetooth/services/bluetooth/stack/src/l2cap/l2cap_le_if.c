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

#include "l2cap_le_if.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../btm/btm_thread.h"
#include "l2cap_cmn.h"
#include "l2cap_le.h"

typedef struct {
    uint16_t lpsm;
    L2capLeService svc;
    void *context;
    void (*cb)(uint16_t lpsm, int result);
} L2cifLeRegisterServiceContext;

static void L2cifLeRegisterService(const void *context)
{
    L2cifLeRegisterServiceContext *ctx = NULL;
    int result;

    ctx = (L2cifLeRegisterServiceContext *)context;

    result = L2CAP_LeRegisterService(ctx->lpsm, &(ctx->svc), ctx->context);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lpsm, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_LeRegisterService(
    uint16_t lpsm, const L2capLeService *svc, void *context, void (*cb)(uint16_t lpsm, int result))
{
    L2cifLeRegisterServiceContext *ctx = NULL;

    if (svc == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeRegisterServiceContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->lpsm = lpsm;
    (void)memcpy_s(&(ctx->svc), sizeof(L2capLeService), svc, sizeof(L2capLeService));
    ctx->context = context;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeRegisterService, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t lpsm;
    void (*cb)(uint16_t lpsm, int result);
} L2cifLeDeregisterServiceContext;

static void L2cifLeDeregisterService(const void *context)
{
    L2cifLeDeregisterServiceContext *ctx = NULL;
    int result;

    ctx = (L2cifLeDeregisterServiceContext *)context;

    result = L2CAP_LeDeregisterService(ctx->lpsm);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lpsm, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_LeDeregisterService(uint16_t lpsm, void (*cb)(uint16_t lpsm, int result))
{
    L2cifLeDeregisterServiceContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifLeDeregisterServiceContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lpsm = lpsm;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeDeregisterService, L2capFree, ctx);
    return;
}

typedef struct {
    BtAddr addr;
    uint16_t lpsm;
    uint16_t rpsm;
    L2capLeConfigInfo cfg;
    void (*cb)(const BtAddr *addr, uint16_t lcid, int result);
} L2cifLeCreditBasedConnectionReqContext;

static void L2cifLeCreditBasedConnectionReq(const void *context)
{
    L2cifLeCreditBasedConnectionReqContext *ctx = NULL;
    uint16_t lcid = 0;
    int result;

    ctx = (L2cifLeCreditBasedConnectionReqContext *)context;

    result = L2CAP_LeCreditBasedConnectionReq(&(ctx->addr), ctx->lpsm, ctx->rpsm, &(ctx->cfg), &lcid);
    if (ctx->cb != NULL) {
        ctx->cb(&(ctx->addr), lcid, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_LeCreditBasedConnectionReq(const BtAddr *addr, uint16_t lpsm, uint16_t rpsm, const L2capLeConfigInfo *cfg,
    void (*cb)(const BtAddr *addr, uint16_t lcid, int result))
{
    L2cifLeCreditBasedConnectionReqContext *ctx = NULL;

    if ((addr == NULL) || (cfg == NULL)) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeCreditBasedConnectionReqContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memcpy_s(&(ctx->addr), sizeof(BtAddr), addr, sizeof(BtAddr));
    ctx->lpsm = lpsm;
    ctx->rpsm = rpsm;
    (void)memcpy_s(&(ctx->cfg), sizeof(L2capLeConfigInfo), cfg, sizeof(L2capLeConfigInfo));
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeCreditBasedConnectionReq, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capLeConfigInfo cfg;
    uint16_t result;
    void (*cb)(uint16_t lcid, int result);
} L2cifLeCreditBasedConnectionRspContext;

static void L2cifLeCreditBasedConnectionRsp(const void *context)
{
    L2cifLeCreditBasedConnectionRspContext *ctx = NULL;
    int result;

    ctx = (L2cifLeCreditBasedConnectionRspContext *)context;

    result = L2CAP_LeCreditBasedConnectionRsp(ctx->lcid, ctx->id, &(ctx->cfg), ctx->result);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_LeCreditBasedConnectionRsp(
    uint16_t lcid, uint8_t id, const L2capLeConfigInfo *cfg, uint16_t result, void (*cb)(uint16_t lcid, int result))
{
    L2cifLeCreditBasedConnectionRspContext *ctx = NULL;

    if (cfg == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeCreditBasedConnectionRspContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->lcid = lcid;
    ctx->id = id;
    (void)memcpy_s(&(ctx->cfg), sizeof(L2capLeConfigInfo), cfg, sizeof(L2capLeConfigInfo));
    ctx->result = result;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeCreditBasedConnectionRsp, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t lcid;
    void (*cb)(uint16_t lcid, int result);
} L2cifLeDisconnectionReqContext;

static void L2cifLeDisconnectionReq(const void *context)
{
    L2cifLeDisconnectionReqContext *ctx = NULL;
    int result;

    ctx = (L2cifLeDisconnectionReqContext *)context;

    result = L2CAP_LeDisconnectionReq(ctx->lcid);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_LeDisconnectionReq(uint16_t lcid, void (*cb)(uint16_t lcid, int result))
{
    L2cifLeDisconnectionReqContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifLeDisconnectionReqContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lcid = lcid;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeDisconnectionReq, L2capFree, ctx);
    return;
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    void (*cb)(uint16_t lcid, int result);
} L2cifLeDisconnectionRspContext;

static void L2cifLeDisconnectionRsp(const void *context)
{
    L2cifLeDisconnectionRspContext *ctx = NULL;
    int result;

    ctx = (L2cifLeDisconnectionRspContext *)context;

    result = L2CAP_LeDisconnectionRsp(ctx->lcid, ctx->id);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_LeDisconnectionRsp(uint16_t lcid, uint8_t id, void (*cb)(uint16_t lcid, int result))
{
    L2cifLeDisconnectionRspContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifLeDisconnectionRspContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lcid = lcid;
    ctx->id = id;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeDisconnectionRsp, L2capFree, ctx);
    return;
}

typedef struct {
    uint16_t lcid;
    Packet *pkt;
    void (*cb)(uint16_t lcid, int result);
} L2cifLeSendDataContext;

static void L2cifLeSendDataContextDestroy(void *context)
{
    L2cifLeSendDataContext *ctx = NULL;

    ctx = context;

    PacketFree(ctx->pkt);
    L2capFree(ctx);
    return;
}

static void L2cifLeSendData(const void *context)
{
    L2cifLeSendDataContext *ctx = NULL;
    int result;

    ctx = (L2cifLeSendDataContext *)context;

    result = L2CAP_LeSendData(ctx->lcid, ctx->pkt);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2cifLeSendDataContextDestroy(ctx);
    return;
}

int L2CIF_LeSendData(uint16_t lcid, const Packet *pkt, void (*cb)(uint16_t lcid, int result))
{
    L2cifLeSendDataContext *ctx = NULL;

    if (pkt == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeSendDataContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->lcid = lcid;
    ctx->pkt = PacketRefMalloc(pkt);
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeSendData, L2cifLeSendDataContextDestroy, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t cid;
    L2capLeFixChannel chan;
    void (*cb)(uint16_t cid, int result);
} L2cifLeRegisterFixChannelContext;

static void L2cifLeRegisterFixChannel(const void *context)
{
    L2cifLeRegisterFixChannelContext *ctx = NULL;
    int result;

    ctx = (L2cifLeRegisterFixChannelContext *)context;

    result = L2CAP_LeRegisterFixChannel(ctx->cid, &(ctx->chan));
    if (ctx->cb != NULL) {
        ctx->cb(ctx->cid, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_LeRegisterFixChannel(uint16_t cid, const L2capLeFixChannel *chan, void (*cb)(uint16_t cid, int result))
{
    L2cifLeRegisterFixChannelContext *ctx = NULL;

    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeRegisterFixChannelContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->cid = cid;
    (void)memcpy_s(&(ctx->chan), sizeof(L2capLeFixChannel), chan, sizeof(L2capLeFixChannel));
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeRegisterFixChannel, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t cid;
    void (*cb)(uint16_t cid, int result);
} L2cifLeDeregisterFixChannelContext;

static void L2cifLeDeregisterFixChannel(const void *context)
{
    L2cifLeDeregisterFixChannelContext *ctx = NULL;
    int result;

    ctx = (L2cifLeDeregisterFixChannelContext *)context;

    result = L2CAP_LeDeregisterFixChannel(ctx->cid);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->cid, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_LeDeregisterFixChannel(uint16_t cid, void (*cb)(uint16_t cid, int result))
{
    L2cifLeDeregisterFixChannelContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifLeDeregisterFixChannelContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->cid = cid;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeDeregisterFixChannel, L2capFree, ctx);
    return;
}

typedef struct {
    BtAddr addr;
    L2capLeConnectionParameter param;
    void (*cb)(const BtAddr *addr, int result);
} L2cifLeConnectContext;

static void L2cifLeConnect(const void *context)
{
    L2cifLeConnectContext *ctx = NULL;
    int result;

    ctx = (L2cifLeConnectContext *)context;

    result = L2CAP_LeConnect(&(ctx->addr), &(ctx->param));
    if (ctx->cb != NULL) {
        ctx->cb(&(ctx->addr), result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_LeConnect(
    const BtAddr *addr, const L2capLeConnectionParameter *param, void (*cb)(const BtAddr *addr, int result))
{
    L2cifLeConnectContext *ctx = NULL;

    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeConnectContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memcpy_s(&(ctx->addr), sizeof(BtAddr), addr, sizeof(BtAddr));
    if (param != NULL) {
        (void)memcpy_s(&(ctx->param), sizeof(L2capLeConnectionParameter), param, sizeof(L2capLeConnectionParameter));
    }

    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeConnect, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    BtAddr addr;
} L2cifLeConnectCancelContext;

static void L2cifLeConnectCancel(const void *context)
{
    L2cifLeConnectCancelContext *ctx = NULL;

    ctx = (L2cifLeConnectCancelContext *)context;

    L2CAP_LeConnectCancel(&(ctx->addr));

    L2capFree(ctx);
    return;
}

int L2CIF_LeConnectCancel(const BtAddr *addr)
{
    L2cifLeConnectCancelContext *ctx = NULL;

    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeConnectCancelContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memcpy_s(&(ctx->addr), sizeof(BtAddr), addr, sizeof(BtAddr));
    L2capAsynchronousProcess(L2cifLeConnectCancel, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t aclHandle;
    void (*cb)(uint16_t aclHandle, int result);
} L2cifLeDisconnectContext;

static void L2cifLeDisconnect(const void *context)
{
    L2cifLeDisconnectContext *ctx = NULL;
    int result;

    ctx = (L2cifLeDisconnectContext *)context;

    result = L2CAP_LeDisconnect(ctx->aclHandle);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->aclHandle, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_LeDisconnect(uint16_t aclHandle, void (*cb)(uint16_t aclHandle, int result))
{
    L2cifLeDisconnectContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifLeDisconnectContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->aclHandle = aclHandle;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeDisconnect, L2capFree, ctx);
    return;
}

typedef struct {
    uint16_t aclHandle;
    uint16_t cid;
    Packet *pkt;
    void (*cb)(uint16_t aclHandle, int result);
} L2cifLeSendFixChannelDataContext;

static void L2cifLeSendFixChannelDataContextDestroy(void *context)
{
    L2cifLeSendFixChannelDataContext *ctx = NULL;

    ctx = context;
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    PacketFree(ctx->pkt);
    L2capFree(ctx);
    return;
}

static void L2cifLeSendFixChannelData(const void *context)
{
    L2cifLeSendFixChannelDataContext *ctx = NULL;
    int result;

    ctx = (L2cifLeSendFixChannelDataContext *)context;

    result = L2CAP_LeSendFixChannelData(ctx->aclHandle, ctx->cid, ctx->pkt);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->aclHandle, result);
    }

    L2cifLeSendFixChannelDataContextDestroy(ctx);
    return;
}

int L2CIF_LeSendFixChannelData(
    uint16_t aclHandle, uint16_t cid, Packet *pkt, void (*cb)(uint16_t aclHandle, int result))
{
    L2cifLeSendFixChannelDataContext *ctx = NULL;

    if (pkt == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeSendFixChannelDataContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->aclHandle = aclHandle;
    ctx->cid = cid;
    ctx->pkt = PacketRefMalloc((Packet *)pkt);
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLeSendFixChannelData, L2cifLeSendFixChannelDataContextDestroy, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    L2capLeConnectionParameterUpdate cb;
    void *context;
} L2cifLeRegisterConnectionParameterUpdateContext;

static void L2cifLeRegisterConnectionParameterUpdate(const void *context)
{
    L2cifLeRegisterConnectionParameterUpdateContext *ctx = NULL;

    ctx = (L2cifLeRegisterConnectionParameterUpdateContext *)context;

    L2CAP_LeRegisterConnectionParameterUpdate(&(ctx->cb), ctx->context);

    L2capFree(ctx);
    return;
}

int L2CIF_LeRegisterConnectionParameterUpdate(const L2capLeConnectionParameterUpdate *cb, void *context)
{
    L2cifLeRegisterConnectionParameterUpdateContext *ctx = NULL;

    if (cb == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeRegisterConnectionParameterUpdateContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memcpy_s(&(ctx->cb), sizeof(L2capLeConnectionParameterUpdate), cb, sizeof(L2capLeConnectionParameterUpdate));
    ctx->context = context;

    L2capAsynchronousProcess(L2cifLeRegisterConnectionParameterUpdate, L2capFree, ctx);
    return BT_NO_ERROR;
}

static void L2cifLeDeregisterConnectionParameterUpdate(const void *context)
{
    L2CAP_LeDeregisterConnectionParameterUpdate();
    return;
}

void L2CIF_LeDeregisterConnectionParameterUpdate()
{
    L2capAsynchronousProcess(L2cifLeDeregisterConnectionParameterUpdate, NULL, NULL);
    return;
}

typedef struct {
    uint16_t aclHandle;
    L2capLeConnectionParameter param;
    void (*cb)(uint16_t aclHandle, int result);
} L2cifLeConnectionParameterUpdateReqContext;

static void L2cifLeConnectionParameterUpdateReq(const void *context)
{
    L2cifLeConnectionParameterUpdateReqContext *ctx = NULL;
    int result;

    ctx = (L2cifLeConnectionParameterUpdateReqContext *)context;

    result = L2CAP_LeConnectionParameterUpdateReq(ctx->aclHandle, &(ctx->param));
    if (ctx->cb != NULL) {
        ctx->cb(ctx->aclHandle, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_LeConnectionParameterUpdateReq(
    uint16_t aclHandle, const L2capLeConnectionParameter *param, void (*const cb)(uint16_t aclHandle, int result))
{
    L2cifLeConnectionParameterUpdateReqContext *ctx = NULL;

    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifLeConnectionParameterUpdateReqContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->aclHandle = aclHandle;
    (void)memcpy_s(&(ctx->param), sizeof(L2capLeConnectionParameter), param, sizeof(L2capLeConnectionParameter));

    L2capAsynchronousProcess(L2cifLeConnectionParameterUpdateReq, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t aclHandle;
    uint8_t id;
    uint16_t result;
    void (*cb)(uint16_t aclHandle, int result);
} L2cifLeConnectionParameterUpdateRspContext;

static void L2cifLeConnectionParameterUpdateRsp(const void *context)
{
    L2cifLeConnectionParameterUpdateRspContext *ctx = NULL;
    int result;

    ctx = (L2cifLeConnectionParameterUpdateRspContext *)context;

    result = L2CAP_LeConnectionParameterUpdateRsp(ctx->aclHandle, ctx->id, ctx->result);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->aclHandle, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_LeConnectionParameterUpdateRsp(
    uint16_t aclHandle, uint8_t id, uint16_t result, void (*const cb)(uint16_t aclHandle, int result))
{
    L2cifLeConnectionParameterUpdateRspContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifLeConnectionParameterUpdateRspContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }
    ctx->aclHandle = aclHandle;
    ctx->id = id;
    ctx->result = result;

    L2capAsynchronousProcess(L2cifLeConnectionParameterUpdateRsp, L2capFree, ctx);
    return;
}