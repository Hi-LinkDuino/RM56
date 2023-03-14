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

#include "rfcomm_defs.h"

static void RfcommRecvConnectReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *context);
static void RfcommRecvConnectRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *context);
static void RfcommRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *context);
static void RfcommRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *context);
static void RfcommRecvDisconnectReqCallback(uint16_t lcid, uint8_t id, void *context);
static void RfcommRecvDisconnectRspCallback(uint16_t lcid, void *context);
static void RfcommDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *context);
static void RfcommRecvDataCallback(uint16_t lcid, Packet *pkt, void *context);

/**
 * @brief Register callback function to L2CAP when RFCOMM initialize.
 *
 */
void RfcommRegisterL2cap()
{
    LOG_INFO("%{public}s", __func__);

    static const L2capService svc = {
        RfcommRecvConnectReqCallback,
        RfcommRecvConnectRspCallback,
        RfcommRecvConfigReqCallback,
        RfcommRecvConfigRspCallback,
        RfcommRecvDisconnectReqCallback,
        RfcommRecvDisconnectRspCallback,
        RfcommDisconnectAbnormalCallback,
        RfcommRecvDataCallback,
        NULL
    };
    L2CIF_RegisterService(BT_PSM_RFCOMM, &svc, NULL, NULL);
}

/**
 * @brief Deregister L2CAP when RFCOMM finalize.
 *
 */
void RfcommDeregisterL2cap()
{
    LOG_INFO("%{public}s", __func__);

    L2CIF_DeregisterService(BT_PSM_RFCOMM, NULL);
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConnectionInfo info;
    uint16_t lpsm;
    void *context;
} RfcommRecvConnectReqInfo;

static void RfcommRecvConnectReqTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRecvConnectReqInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvConnectReqCback(ctx->lcid, ctx->id, &ctx->info, ctx->lpsm);
    free(ctx);
}

void RfcommRecvConnectReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommRecvConnectReqInfo *ctx = malloc(sizeof(RfcommRecvConnectReqInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommRecvConnectReqInfo), 0x00, sizeof(RfcommRecvConnectReqInfo));
    ctx->lcid = lcid;
    ctx->id = id;
    ctx->lpsm = lpsm;
    ctx->context = context;
    (void)memcpy_s(&ctx->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvConnectReqTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

typedef struct {
    uint16_t lcid;
    L2capConnectionInfo info;
    uint16_t result;
    uint16_t status;
    void *context;
} RfcommRecvConnectRspInfo;

static void RfcommRecvConnectRspTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRecvConnectRspInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvConnectRspCback(ctx->lcid, &ctx->info, ctx->result, ctx->status);
    free(ctx);
}

void RfcommRecvConnectRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommRecvConnectRspInfo *ctx = malloc(sizeof(RfcommRecvConnectRspInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommRecvConnectRspInfo), 0x00, sizeof(RfcommRecvConnectRspInfo));
    ctx->lcid = lcid;
    ctx->result = result;
    ctx->status = status;
    ctx->context = context;
    (void)memcpy_s(&ctx->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvConnectRspTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConfigInfo cfg;
    void *context;
} RfcommRecvConfigReqInfo;

static void RfcommRecvConfigReqTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRecvConfigReqInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvConfigReqCback(ctx->lcid, ctx->id, &ctx->cfg);
    free(ctx);
}

void RfcommRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommRecvConfigReqInfo *ctx = malloc(sizeof(RfcommRecvConfigReqInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommRecvConfigReqInfo), 0x00, sizeof(RfcommRecvConfigReqInfo));
    ctx->lcid = lcid;
    ctx->id = id;
    ctx->context = context;
    (void)memcpy_s(&ctx->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvConfigReqTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

typedef struct {
    uint16_t lcid;
    L2capConfigInfo cfg;
    uint16_t result;
    void *context;
} RfcommRecvConfigRspInfo;

static void RfcommRecvConfigRspTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRecvConfigRspInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvConfigRspCback(ctx->lcid, &ctx->cfg, ctx->result);
    free(ctx);
}

void RfcommRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommRecvConfigRspInfo *ctx = malloc(sizeof(RfcommRecvConfigRspInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommRecvConfigRspInfo), 0x00, sizeof(RfcommRecvConfigRspInfo));
    ctx->lcid = lcid;
    ctx->result = result;
    ctx->context = context;
    (void)memcpy_s(&ctx->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvConfigRspTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    void *context;
} RfcommRecvDisconnectReqInfo;

static void RfcommRecvDisconnectReqTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRecvDisconnectReqInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvDisconnectReqCback(ctx->lcid, ctx->id);
    free(ctx);
}

void RfcommRecvDisconnectReqCallback(uint16_t lcid, uint8_t id, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommRecvDisconnectReqInfo *ctx = malloc(sizeof(RfcommRecvDisconnectReqInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommRecvDisconnectReqInfo), 0x00, sizeof(RfcommRecvDisconnectReqInfo));
    ctx->lcid = lcid;
    ctx->id = id;
    ctx->context = context;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvDisconnectReqTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

/**
 * @brief L2CAP calls this function to notify RFCOMM disconnect response from peer.
 *        After receiving the response, RFCOMM calls the state machine.
 *
 * @param lcid    L2CAP channel id.
 * @param context Context registered by RFCOMM to L2CAP.
 */
void RfcommRecvDisconnectRspCallback(uint16_t lcid, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);
}

typedef struct {
    uint16_t lcid;
    uint8_t reason;
    void *context;
} RfcommDisconnectAbnormalInfo;

static void RfcommDisconnectAbnormalTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommDisconnectAbnormalInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommDisconnectAbnormalCback(ctx->lcid, ctx->reason);
    free(ctx);
}

void RfcommDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommDisconnectAbnormalInfo *ctx = malloc(sizeof(RfcommDisconnectAbnormalInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommDisconnectAbnormalInfo), 0x00, sizeof(RfcommDisconnectAbnormalInfo));
    ctx->lcid = lcid;
    ctx->reason = reason;
    ctx->context = context;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommDisconnectAbnormalTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

typedef struct {
    uint16_t lcid;
    Packet *pkt;
    void *context;
} RfcommRecvDataInfo;

static void RfcommRecvDataTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRecvDataInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvDataCback(ctx->lcid, ctx->pkt);
    PacketFree(ctx->pkt);
    free(ctx);
}

void RfcommRecvDataCallback(uint16_t lcid, Packet *pkt, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommRecvDataInfo *ctx = malloc(sizeof(RfcommRecvDataInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommRecvDataInfo), 0x00, sizeof(RfcommRecvDataInfo));
    ctx->lcid = lcid;
    ctx->pkt = PacketRefMalloc(pkt);
    ctx->context = context;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvDataTsk, ctx);
    if (ret != BT_NO_ERROR) {
        PacketFree(ctx->pkt);
        free(ctx);
    }
}

typedef struct {
    BtAddr addr;
    uint16_t lcid;
    int result;
    void *context;
} RfcommSendConnectReqInfo;

static void RfcommSendConnectReqCbackTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSendConnectReqInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommSendConnectReqCback(&ctx->addr, ctx->lcid, ctx->result);
    free(ctx);
}

static void RfcommSendConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommSendConnectReqInfo *ctx = malloc(sizeof(RfcommSendConnectReqInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommSendConnectReqInfo), 0x00, sizeof(RfcommSendConnectReqInfo));
    ctx->lcid = lcid;
    ctx->result = result;
    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    ctx->context = context;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommSendConnectReqCbackTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

/**
 * @brief Call L2CAP_ConnectReq to request connection from L2CAP.
 *
 * @param addr The peer's BT address.
 * @param lcid The L2CAP's channel id.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendConnectReq(const BtAddr *addr)
{
    LOG_INFO("%{public}s", __func__);

    int ret = L2CIF_ConnectReq(addr, BT_PSM_RFCOMM, BT_PSM_RFCOMM, NULL, RfcommSendConnectReqCallback);
    if (ret != RFCOMM_SUCCESS) {
        LOG_ERROR("%{public}s L2CIF_ConnectReq return value is error.", __func__);
    }

    return ret;
}

/**
 * @brief Call L2CAP_ConnectRsp to send connection response to peer.
 *
 * @param lcid   The L2CAP's channel id.
 * @param id     L2CAP's id information.
 * @param result The result of the response.
 * @param status The status information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendConnectRsp(uint16_t lcid, uint8_t id, uint16_t result, uint16_t status)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    L2CIF_ConnectRsp(lcid, id, result, status, NULL);

    return RFCOMM_SUCCESS;
}

/**
 * @brief Call L2CAP_ConfigReq to send config request to peer.
 *
 * @param lcid The L2CAP's channel id.
 * @param cfg  Config information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendConfigReq(uint16_t lcid, const L2capConfigInfo *cfg)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    int ret = L2CIF_ConfigReq(lcid, cfg, NULL);
    if (ret != RFCOMM_SUCCESS) {
        LOG_ERROR("%{public}s L2CIF_ConfigReq return value is error.", __func__);
    }

    return ret;
}

/**
 * @brief Call L2CAP_ConfigRsp to send config response to peer.
 *
 * @param lcid   The L2CAP's channel id.
 * @param id     L2CAP's id information.
 * @param cfg    Config information.
 * @param result The result of the response.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendConfigRsp(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, uint16_t result)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    int ret = L2CIF_ConfigRsp(lcid, id, cfg, result, NULL);
    if (ret != RFCOMM_SUCCESS) {
        LOG_ERROR("%{public}s L2CIF_ConfigRsp return value is error.", __func__);
    }

    return ret;
}

/**
 * @brief Call L2CAP_DisconnectionReq to send disconnect request to peer.
 *
 * @param lcid The L2CAP's channel id.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendDisconnectReq(uint16_t lcid)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    L2CIF_DisconnectionReq(lcid, NULL);

    return RFCOMM_SUCCESS;
}

/**
 * @brief Call L2CAP_DisconnectionRsp to send disconnect response to peer.
 *
 * @param lcid The L2CAP's channel id.
 * @param id   L2CAP's id information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendDisconnectRsp(uint16_t lcid, uint8_t id)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    L2CIF_DisconnectionRsp(lcid, id, NULL);

    return RFCOMM_SUCCESS;
}

/**
 * @brief Call L2CAP_SendData to send RFCOMM frame to peer.
 *
 * @param lcid     The L2CAP's channel id.
 * @param header   RFCOMM frame header information.
 * @param headSize RFCOMM frame header length.
 * @param tail     RFCOMM frame tail information.
 * @param pkt      The RFCOMM frame information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendData(uint16_t lcid, const uint8_t *header, uint8_t headSize, uint8_t tail, Packet *pkt)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    Packet *packet = NULL;
    uint8_t *headerBuf = NULL;
    uint8_t *tailBuf = NULL;

    if (pkt == NULL) {
        packet = PacketMalloc(headSize, 1, 0);
    } else {
        packet = PacketInheritMalloc(pkt, headSize, 1);
    }

    headerBuf = (uint8_t *)BufferPtr(PacketHead(packet));
    tailBuf = (uint8_t *)BufferPtr(PacketTail(packet));
    (void)memcpy_s(headerBuf, (sizeof(uint8_t) * headSize), header, (sizeof(uint8_t) * headSize));
    *tailBuf = tail;

    int ret = L2CIF_SendData(lcid, packet, NULL);
    if (ret != RFCOMM_SUCCESS) {
        LOG_ERROR("%{public}s L2CIF_SendData return value is error.", __func__);
    }
    PacketFree(packet);

    return ret;
}
