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

#include "l2cap_if.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "module.h"

#include "semaphore.h"

#include "../btm/btm_thread.h"
#include "l2cap.h"
#include "l2cap_cmn.h"

typedef struct {
    uint16_t lpsm;
    L2capService service;
    void *context;
    void (*cb)(uint16_t lpsm, int result);
} L2cifRegisterServiceContext;

static void L2cifRegisterService(const void *context)
{
    L2cifRegisterServiceContext *ctx = NULL;
    int result;

    ctx = (L2cifRegisterServiceContext *)context;

    result = L2CAP_RegisterService(ctx->lpsm, &(ctx->service), ctx->context);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lpsm, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_RegisterService(uint16_t lpsm, const L2capService *svc, void *context, void (*cb)(uint16_t lpsm, int result))
{
    L2cifRegisterServiceContext *ctx = NULL;

    if (svc == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifRegisterServiceContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->lpsm = lpsm;
    (void)memcpy_s(&(ctx->service), sizeof(L2capService), svc, sizeof(L2capService));
    ctx->context = context;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifRegisterService, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t lpsm;
    void (*cb)(uint16_t lpsm, int result);
} L2cifDeregisterServiceContext;

static void L2cifDeregisterService(const void *context)
{
    L2cifDeregisterServiceContext *ctx = NULL;
    int result;

    ctx = (L2cifDeregisterServiceContext *)context;

    result = L2CAP_DeregisterService(ctx->lpsm);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lpsm, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_DeregisterService(uint16_t lpsm, void (*cb)(uint16_t lpsm, int result))
{
    L2cifDeregisterServiceContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifDeregisterServiceContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lpsm = lpsm;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifDeregisterService, L2capFree, ctx);
    return;
}

typedef struct {
    BtAddr addr;
    uint16_t lpsm;
    uint16_t rpsm;
    void *context;
    void (*cb)(const BtAddr *addr, uint16_t lcid, int result, void *context);
} L2cifConnectReqContext;

static void L2cifConnectReq(const void *context)
{
    L2cifConnectReqContext *ctx = NULL;
    uint16_t lcid = 0;
    int result;

    ctx = (L2cifConnectReqContext *)context;

    result = L2CAP_ConnectReq(&(ctx->addr), ctx->lpsm, ctx->rpsm, &lcid);
    if (ctx->cb != NULL) {
        ctx->cb(&(ctx->addr), lcid, result, ctx->context);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_ConnectReq(const BtAddr *addr, uint16_t lpsm, uint16_t rpsm, void *context,
    void (*cb)(const BtAddr *addr, uint16_t lcid, int result, void *context))
{
    L2cifConnectReqContext *ctx = NULL;

    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifConnectReqContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memcpy_s(&(ctx->addr), sizeof(BtAddr), addr, sizeof(BtAddr));
    ctx->lpsm = lpsm;
    ctx->rpsm = rpsm;
    ctx->context = context;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifConnectReq, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    uint16_t result;
    uint16_t status;
    void (*cb)(uint16_t lcid, int result);
} L2cifConnectRspContext;

static void L2cifConnectRsp(const void *context)
{
    L2cifConnectRspContext *ctx = NULL;
    int result;

    ctx = (L2cifConnectRspContext *)context;

    result = L2CAP_ConnectRsp(ctx->lcid, ctx->id, ctx->result, ctx->status);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_ConnectRsp(
    uint16_t lcid, uint8_t id, uint16_t result, uint16_t status, void (*cb)(uint16_t lcid, int result))
{
    L2cifConnectRspContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifConnectRspContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lcid = lcid;
    ctx->id = id;
    ctx->result = result;
    ctx->status = status;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifConnectRsp, L2capFree, ctx);
    return;
}

typedef struct {
    uint16_t lcid;
    L2capConfigInfo cfg;
    void (*cb)(uint16_t lcid, int result);
} L2cifConfigReqContext;

static void L2cifConfigReq(const void *context)
{
    L2cifConfigReqContext *ctx = NULL;
    int result;

    ctx = (L2cifConfigReqContext *)context;

    result = L2CAP_ConfigReq(ctx->lcid, &(ctx->cfg));
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_ConfigReq(uint16_t lcid, const L2capConfigInfo *cfg, void (*cb)(uint16_t lcid, int result))
{
    L2cifConfigReqContext *ctx = NULL;

    if (cfg == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifConfigReqContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->lcid = lcid;
    (void)memcpy_s(&(ctx->cfg), sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifConfigReq, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConfigInfo cfg;
    uint16_t result;
    void (*cb)(uint16_t lcid, int result);
} L2cifConfigRspContext;

static void L2cifConfigRsp(const void *context)
{
    L2cifConfigRspContext *ctx = NULL;
    int result;

    ctx = (L2cifConfigRspContext *)context;

    result = L2CAP_ConfigRsp(ctx->lcid, ctx->id, &(ctx->cfg), ctx->result);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

int L2CIF_ConfigRsp(
    uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, uint16_t result, void (*cb)(uint16_t lcid, int result))
{
    L2cifConfigRspContext *ctx = NULL;

    if (cfg == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifConfigRspContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->lcid = lcid;
    ctx->id = id;
    (void)memcpy_s(&(ctx->cfg), sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    ctx->result = result;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifConfigRsp, L2capFree, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    uint16_t lcid;
    void (*cb)(uint16_t lcid, int result);
} L2cifDisconnectionReqContext;

static void L2cifDisconnectionReq(const void *context)
{
    L2cifDisconnectionReqContext *ctx = NULL;
    int result;

    ctx = (L2cifDisconnectionReqContext *)context;

    result = L2CAP_DisconnectionReq(ctx->lcid);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_DisconnectionReq(uint16_t lcid, void (*cb)(uint16_t lcid, int result))
{
    L2cifDisconnectionReqContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifDisconnectionReqContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lcid = lcid;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifDisconnectionReq, L2capFree, ctx);
    return;
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    void (*cb)(uint16_t lcid, int result);
} L2cifDisconnectionRspContext;

static void L2cifDisconnectionRsp(const void *context)
{
    L2cifDisconnectionRspContext *ctx = NULL;
    int result;

    ctx = (L2cifDisconnectionRspContext *)context;

    result = L2CAP_DisconnectionRsp(ctx->lcid, ctx->id);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_DisconnectionRsp(uint16_t lcid, uint8_t id, void (*cb)(uint16_t lcid, int result))
{
    L2cifDisconnectionRspContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifDisconnectionRspContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lcid = lcid;
    ctx->id = id;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifDisconnectionRsp, L2capFree, ctx);
    return;
}

typedef struct {
    uint16_t lcid;
    uint8_t isBusy;
    void (*cb)(uint16_t lcid, int result);
} L2cifLocalBusyContext;

static void L2cifLocalBusy(const void *context)
{
    L2cifLocalBusyContext *ctx = NULL;
    int result;

    ctx = (L2cifLocalBusyContext *)context;

    result = L2CAP_LocalBusy(ctx->lcid, ctx->isBusy);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2capFree(ctx);
    return;
}

void L2CIF_LocalBusy(uint16_t lcid, uint8_t isBusy, void (*cb)(uint16_t lcid, int result))
{
    L2cifLocalBusyContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifLocalBusyContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->lcid = lcid;
    ctx->isBusy = isBusy;
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifLocalBusy, L2capFree, ctx);
    return;
}

typedef struct {
    uint16_t lcid;
    Packet *pkt;
    void (*cb)(uint16_t lcid, int result);
} L2cifSendDataContext;

static void L2cifSendDataContextDestroy(void *context)
{
    L2cifSendDataContext *ctx = NULL;

    ctx = context;

    PacketFree(ctx->pkt);
    L2capFree(ctx);
    return;
}

static void L2cifSendData(const void *context)
{
    L2cifSendDataContext *ctx = NULL;
    int result;

    ctx = (L2cifSendDataContext *)context;

    result = L2CAP_SendData(ctx->lcid, ctx->pkt);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->lcid, result);
    }

    L2cifSendDataContextDestroy(ctx);
    return;
}

int L2CIF_SendData(uint16_t lcid, const Packet *pkt, void (*cb)(uint16_t lcid, int result))
{
    L2cifSendDataContext *ctx = NULL;

    if (pkt == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifSendDataContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->lcid = lcid;
    ctx->pkt = PacketRefMalloc((Packet *)pkt);
    ctx->cb = cb;

    L2capAsynchronousProcess(L2cifSendData, L2cifSendDataContextDestroy, ctx);
    return BT_NO_ERROR;
}

typedef struct {
    L2capEcho echoCallback;
    void *context;
} L2cifRegisterEchoContext;

static void L2cifRegisterEcho(const void *context)
{
    L2cifRegisterEchoContext *ctx = NULL;

    ctx = (L2cifRegisterEchoContext *)context;

    L2CAP_RegisterEcho(&(ctx->echoCallback), ctx->context);

    L2capFree(ctx);
    return;
}

int L2CIF_RegisterEcho(const L2capEcho *echoCallback, void *context)
{
    L2cifRegisterEchoContext *ctx = NULL;

    if (echoCallback == NULL) {
        return BT_BAD_PARAM;
    }

    ctx = L2capAlloc(sizeof(L2cifRegisterEchoContext));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memcpy_s(&(ctx->echoCallback), sizeof(L2capEcho), echoCallback, sizeof(L2capEcho));
    ctx->context = context;

    L2capAsynchronousProcess(L2cifRegisterEcho, L2capFree, ctx);
    return BT_NO_ERROR;
}

static void L2cifDeregisterEcho(const void *context)
{
    L2CAP_DeregisterEcho();
    return;
}

void L2CIF_DeregisterEcho()
{
    L2capAsynchronousProcess(L2cifDeregisterEcho, NULL, NULL);
    return;
}

typedef struct {
    uint16_t aclHandle;
    uint8_t *data;
    uint16_t dataLen;
    void (*cb)(uint16_t aclHandle, int result);
} L2cifEchoReqContext;

static void L2cifEchoReqContextDestroy(void *context)
{
    L2cifEchoReqContext *ctx = NULL;

    ctx = context;
    if (ctx->data != NULL) {
        L2capFree(ctx->data);
    }

    L2capFree(ctx);
    return;
}

static void L2cifEchoReq(const void *context)
{
    L2cifEchoReqContext *ctx = NULL;
    int result;

    ctx = (L2cifEchoReqContext *)context;

    result = L2CAP_EchoReq(ctx->aclHandle, ctx->data, ctx->dataLen);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->aclHandle, result);
    }

    L2cifEchoReqContextDestroy(ctx);
    return;
}

void L2CIF_EchoReq(
    uint16_t aclHandle, const uint8_t *data, uint16_t dataLen, void (*cb)(uint16_t aclHandle, int result))
{
    L2cifEchoReqContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifEchoReqContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->aclHandle = aclHandle;
    ctx->data = NULL;
    ctx->dataLen = 0;
    ctx->cb = cb;

    if (data != NULL) {
        ctx->data = L2capAlloc(dataLen);
        if (ctx->data == NULL) {
            LOG_WARN("malloc failed");
            return;
        }
        (void)memcpy_s(ctx->data, dataLen, data, dataLen);
        ctx->dataLen = dataLen;
    }

    L2capAsynchronousProcess(L2cifEchoReq, L2cifEchoReqContextDestroy, ctx);
    return;
}

typedef struct {
    uint16_t aclHandle;
    uint8_t id;
    uint8_t *data;
    uint16_t dataLen;
    void (*cb)(uint16_t aclHandle, int result);
} L2cifEchoRspContext;

static void L2cifEchoRspContextDestroy(void *context)
{
    L2cifEchoRspContext *ctx = NULL;

    ctx = context;
    if (ctx->data != NULL) {
        L2capFree(ctx->data);
    }

    L2capFree(ctx);
    return;
}

static void L2cifEchoRsp(const void *context)
{
    L2cifEchoRspContext *ctx = NULL;
    int result;

    ctx = (L2cifEchoRspContext *)context;

    result = L2CAP_EchoRsp(ctx->aclHandle, ctx->id, ctx->data, ctx->dataLen);
    if (ctx->cb != NULL) {
        ctx->cb(ctx->aclHandle, result);
    }

    L2cifEchoRspContextDestroy(ctx);
    return;
}

void L2CIF_EchoRsp(
    uint16_t aclHandle, uint8_t id, const uint8_t *data, uint16_t dataLen, void (*cb)(uint16_t aclHandle, int result))
{
    L2cifEchoRspContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2cifEchoRspContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->aclHandle = aclHandle;
    ctx->id = id;
    ctx->data = NULL;
    ctx->dataLen = 0;
    ctx->cb = cb;

    if (data != NULL) {
        ctx->data = L2capAlloc(dataLen);
        if (ctx->data == NULL) {
            LOG_WARN("malloc failed");
            return;
        }

        (void)memcpy_s(ctx->data, dataLen, data, dataLen);
        ctx->dataLen = dataLen;
    }

    L2capAsynchronousProcess(L2cifEchoRsp, L2cifEchoRspContextDestroy, ctx);
    return;
}

typedef struct {
    Semaphore *sem;
    int traceLevel;
} L2cifContext;

static void L2cifStartup(void *context)
{
    L2cifContext *ctx = NULL;

    ctx = context;

    L2CAP_Initialize(ctx->traceLevel);

    SemaphorePost(ctx->sem);
    L2capFree(ctx);
    return;
}

void L2CIF_Startup()
{
    int result;
    L2cifContext *ctx = NULL;
    Semaphore *sem = NULL;

    result = BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_LA2CAP, BTM_PROCESSING_QUEUE_SIZE_DEFAULT);
    if (result != BT_NO_ERROR) {
        return;
    }

    sem = SemaphoreCreate(0);
    ctx = L2capAlloc(sizeof(L2cifContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        SemaphoreDelete(sem);
        return;
    }

    ctx->sem = sem;
    ctx->traceLevel = 0;

    result = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_LA2CAP, L2cifStartup, ctx);
    if (result != BT_NO_ERROR) {
        L2capFree(ctx);
        SemaphoreDelete(sem);
        return;
    }

    SemaphoreWait(sem);
    SemaphoreDelete(sem);
    return;
}

static void L2cifShutdown(void *context)
{
    L2cifContext *ctx = NULL;

    ctx = context;

    L2CAP_Finalize();

    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_LA2CAP);

    SemaphorePost(ctx->sem);
    L2capFree(ctx);
    return;
}

void L2CIF_Shutdown()
{
    int result;
    L2cifContext *ctx = NULL;
    Semaphore *sem = NULL;

    sem = SemaphoreCreate(0);
    ctx = L2capAlloc(sizeof(L2cifContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        SemaphoreDelete(sem);
        return;
    }
    ctx->sem = sem;
    ctx->traceLevel = 0;

    result = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_LA2CAP, L2cifShutdown, ctx);
    if (result != BT_NO_ERROR) {
        L2capFree(ctx);
        SemaphoreDelete(sem);
        return;
    }

    SemaphoreWait(sem);
    SemaphoreDelete(sem);

    return;
}

void L2CIF_Initialize(int traceLevel)
{
    // passthrough
    return;
}

void L2CIF_Finalize()
{
    // passthrough
    return;
}

static Module g_l2cap = {
    .name = MODULE_NAME_L2CAP,
    .init = L2CIF_Initialize,
    .startup = L2CIF_Startup,
    .shutdown = L2CIF_Shutdown,
    .cleanup = L2CIF_Finalize,
    .dependencies = {MODULE_NAME_HCI},
};

MODULE_DECL(g_l2cap);