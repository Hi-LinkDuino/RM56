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

#include "sdp_connect.h"

#include "sdp_client_parse.h"
#include "sdp_server.h"
#include "sdp_util.h"

#include "l2cap_if.h"

#include "bt_endian.h"

#include "alarm.h"
#include "list.h"
#include "packet.h"

#include "../btm/btm_thread.h"

#include "allocator.h"
#include "log.h"

typedef struct {
    BtAddr addr;
    uint16_t lcid;
    uint16_t mtu;
    uint8_t outConnState;
    uint8_t inConnState;
    Alarm *timer;
    bool flag;       /// 0-server 1-client
    Packet *packet;  /// Fragment packet
    uint16_t totalCount;
    bool wait;
} SdpConnectInfo;

static void SdpSendConnectRequest(const BtAddr *addr);
static void SdpSendDisconnectRequest(uint16_t lcid, bool wait);
static void SdpReceiveConnectRequest(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *context);
static void SdpReceiveConnectResponse(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *context);
static void SdpReceiveConfigRequest(uint16_t lcid, uint8_t id, const L2capConfigInfo *config, void *context);
static void SdpReceiveConfigResponse(uint16_t lcid, const L2capConfigInfo *config, uint16_t result, void *context);
static void SdpReceiveDisconnectRequest(uint16_t lcid, uint8_t id, void *context);
static void SdpReceiveDisconnectResponse(uint16_t lcid, void *context);
static void SdpDisconnectAbnormal(uint16_t lcid, uint8_t reason, void *context);
static void SdpReceiveData(uint16_t lcid, Packet *packet, void *context);

/// Connect List for server and client
static List *g_connectList = NULL;
static L2capService g_registerInfo;

void SdpRegisterToL2cap()
{
    /// Register with L2CAP
    (void)memset_s(&g_registerInfo, sizeof(L2capService), 0, sizeof(L2capService));
    g_registerInfo.recvConnectionReq = SdpReceiveConnectRequest;
    g_registerInfo.recvConnectionRsp = SdpReceiveConnectResponse;
    g_registerInfo.recvConfigReq = SdpReceiveConfigRequest;
    g_registerInfo.recvConfigRsp = SdpReceiveConfigResponse;
    g_registerInfo.recvDisconnectionReq = SdpReceiveDisconnectRequest;
    g_registerInfo.recvDisconnectionRsp = SdpReceiveDisconnectResponse;
    g_registerInfo.disconnectAbnormal = SdpDisconnectAbnormal;
    g_registerInfo.recvData = SdpReceiveData;
    g_registerInfo.remoteBusy = NULL;

    L2CIF_RegisterService(SDP_PSM, &g_registerInfo, NULL, NULL);
}

void SdpDeregisterToL2cap()
{
    /// Deregister with L2CAP
    L2CIF_DeregisterService(SDP_PSM, NULL);
}

static void SdpFreeConnectInfo(const void *data)
{
    if (data == NULL) {
        return;
    }
    SdpConnectInfo *connect = (SdpConnectInfo *)data;
    if (connect->timer != NULL) {
        AlarmDelete(connect->timer);
        connect->timer = NULL;
    }
    if (connect->packet != NULL) {
        PacketFree(connect->packet);
        connect->packet = NULL;
    }
    MEM_MALLOC.free(connect);
    connect = NULL;
}

void SdpCreateConnectList()
{
    g_connectList = ListCreate((void (*)(void *))SdpFreeConnectInfo);
}

void SdpDestroyConnectList()
{
    if (g_connectList != NULL) {
        ListDelete(g_connectList);
        g_connectList = NULL;
    }
}

static void SdpConnectWaitTimeTask(void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpConnectInfo *ctx = context;

    if (!SdpGetEnableState()) {
        return;
    }

    SdpSendDisconnectRequest(ctx->lcid, true);
    MEM_MALLOC.free(ctx);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpConnectWaitTime(void *parameter)
{
    int ret;
    SdpConnectInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpConnectInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    (void)memset_s(ctx, sizeof(SdpConnectInfo), 0x00, sizeof(SdpConnectInfo));
    (void)memcpy_s(ctx, sizeof(SdpConnectInfo), parameter, sizeof(SdpConnectInfo));

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpConnectWaitTimeTask, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SdpConnectTimeoutTask(void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpConnectInfo *ctx = context;

    if (!SdpGetEnableState()) {
        return;
    }

    if (ctx->flag) {
        SdpRemoveAllRequestByAddress(&ctx->addr);
    }
    ListRemoveNode(g_connectList, ctx);
    MEM_MALLOC.free(ctx);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpConnectTimeout(void *parameter)
{
    int ret;
    SdpConnectInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpConnectInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpConnectInfo), 0x00, sizeof(SdpConnectInfo));
    (void)memcpy_s(ctx, sizeof(SdpConnectInfo), parameter, sizeof(SdpConnectInfo));

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpConnectTimeoutTask, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static SdpConnectInfo *SdpFindConnectByCid(uint16_t lcid)
{
    ListNode *node = NULL;
    SdpConnectInfo *connect = NULL;

    if (g_connectList == NULL) {
        return NULL;
    }
    node = ListGetFirstNode(g_connectList);
    while (node != NULL) {
        connect = (SdpConnectInfo *)ListGetNodeData(node);
        /// Find connect by local channel ID as a server or a client.
        if ((connect->lcid == lcid) && (connect->outConnState != SDP_STATE_IDLE)) {
            return connect;
        }
        node = ListGetNextNode(node);
    }
    return NULL;
}

static SdpConnectInfo *SdpFindConnectByAddress(const BtAddr *addr)
{
    ListNode *node = NULL;
    SdpConnectInfo *connect = NULL;

    if (g_connectList == NULL) {
        return NULL;
    }
    node = ListGetFirstNode(g_connectList);
    while (node != NULL) {
        connect = (SdpConnectInfo *)ListGetNodeData(node);
        if ((connect->flag) && (memcmp(&connect->addr, addr, sizeof(BtAddr)) == 0)) {
            return connect;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

static SdpConnectInfo *SdpFindIdleConnectByAddress(const BtAddr *addr)
{
    ListNode *node = NULL;
    SdpConnectInfo *connect = NULL;

    if (g_connectList == NULL) {
        return NULL;
    }
    node = ListGetFirstNode(g_connectList);
    while (node != NULL) {
        connect = (SdpConnectInfo *)ListGetNodeData(node);
        if ((connect->flag) && (memcmp(&connect->addr, addr, sizeof(BtAddr)) == 0) &&
            (connect->inConnState == SDP_STATE_IDLE) && (connect->outConnState == SDP_STATE_IDLE)) {
            return connect;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

static void SdpNextConnect(const BtAddr *addr)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpClientRequest *request = NULL;

    SdpRemoveRequestByAddress(addr);
    request = SdpFindRequestByAddress(addr);
    if (request != NULL) {
        SdpSendConnectRequest(addr);
    }

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

typedef struct {
    BtAddr addr;
    uint16_t lcid;
    int result;
} SdpConnectCallbackInfo;

static void SdpConnectReqCallbackTask(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpConnectInfo *connect = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    /// Check if L2CAP started the connection process
    if ((result != BT_NO_ERROR) || (lcid == 0)) {
        LOG_ERROR("[%{public}s][%{public}d] Send connect request failed ", __FUNCTION__, __LINE__);
        SdpNextConnect(addr);
        return;
    }

    connect = SdpFindIdleConnectByAddress(addr);
    /// Save channel id
    connect->lcid = lcid;
    /// Set connection state
    connect->outConnState = SDP_STATE_CONN_SETUP;
    connect->inConnState = SDP_STATE_CONN_SETUP;
    /// Set timer
    connect->timer = AlarmCreate(NULL, false);

    /// Start timer
    AlarmSet(connect->timer, SDP_CONNECT_TIMEOUT, SdpConnectTimeout, connect);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpConnectReqCallbackTsk(void *context)
{
    SdpConnectCallbackInfo *ctx = context;
    SdpConnectReqCallbackTask(&ctx->addr, ctx->lcid, ctx->result, NULL);
    MEM_MALLOC.free(ctx);
}

static void SdpConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    int ret;
    SdpConnectCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpConnectCallbackInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpConnectCallbackInfo), 0x00, sizeof(SdpConnectCallbackInfo));

    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    ctx->lcid = lcid;
    ctx->result = result;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpConnectReqCallbackTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SdpConfigReqCallbackTask(uint16_t lcid, int result)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpConnectInfo *connect = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    if (result != BT_NO_ERROR) {
        LOG_ERROR("[%{public}s][%{public}d] Send config request failed ", __FUNCTION__, __LINE__);
        connect = SdpFindConnectByCid(lcid);
        if ((connect != NULL) && (connect->flag)) {
            SdpNextConnect(&connect->addr);
        }
        return;
    }

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpConfigReqCallbackTsk(void *context)
{
    SdpConnectCallbackInfo *ctx = context;
    SdpConfigReqCallbackTask(ctx->lcid, ctx->result);
    MEM_MALLOC.free(ctx);
}

void SdpConfigReqCallback(uint16_t lcid, int result)
{
    int ret;
    SdpConnectCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpConnectCallbackInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpConnectCallbackInfo), 0x00, sizeof(SdpConnectCallbackInfo));

    ctx->lcid = lcid;
    ctx->result = result;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpConfigReqCallbackTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

void SdpConnectRspCallbackTask(uint16_t lcid, int result)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpConnectInfo *connect = NULL;
    L2capConfigInfo config;

    if (!SdpGetEnableState()) {
        return;
    }

    if (result != BT_NO_ERROR) {
        LOG_ERROR("[%{public}s][%{public}d] Send connect response failed ", __FUNCTION__, __LINE__);
        return;
    }

    connect = MEM_MALLOC.alloc(sizeof(SdpConnectInfo));
    if (connect == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(connect, sizeof(SdpConnectInfo), 0, sizeof(SdpConnectInfo));
    /// Save channel id
    connect->lcid = lcid;
    /// Set connection state, waiting for config
    connect->outConnState = SDP_STATE_CONN_SETUP;
    connect->inConnState = SDP_STATE_CONN_SETUP;
    connect->flag = false;
    connect->timer = NULL;
    connect->wait = false;
    ListAddLast(g_connectList, connect);

    /// L2CAP default configuration. SDP only care about mtu.
    (void)memset_s(&config, sizeof(L2capConfigInfo), 0, sizeof(L2capConfigInfo));
    config.mtu = SDP_MTU_SIZE;
    config.flushTimeout = 0xFFFF;
    config.fcs = 0x01;
    L2CIF_ConfigReq(lcid, &config, SdpConfigReqCallback);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpConnectRspCallbackTsk(void *context)
{
    SdpConnectCallbackInfo *ctx = context;
    SdpConnectRspCallbackTask(ctx->lcid, ctx->result);
    MEM_MALLOC.free(ctx);
}

static void SdpConnectRspCallback(uint16_t lcid, int result)
{
    int ret;
    SdpConnectCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpConnectCallbackInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpConnectCallbackInfo), 0x00, sizeof(SdpConnectCallbackInfo));

    ctx->lcid = lcid;
    ctx->result = result;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpConnectRspCallbackTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SdpConfigRspCallbackTask(uint16_t lcid, int result)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpConnectInfo *connect = NULL;
    SdpClientRequest *request = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] Config response callback failed", __FUNCTION__, __LINE__);
        return;
    }

    if (result != BT_NO_ERROR) {
        LOG_ERROR("[%{public}s][%{public}d] Send config response failed ", __FUNCTION__, __LINE__);
        if ((connect != NULL) && (connect->flag)) {
            SdpNextConnect(&connect->addr);
        }
        return;
    }

    connect->inConnState = SDP_STATE_CFG_REQ_SETUP;
    if ((connect->outConnState == SDP_STATE_CFG_RSP_SETUP) && (connect->inConnState == SDP_STATE_CFG_REQ_SETUP)) {
        connect->outConnState = SDP_STATE_CONNECTED;
        connect->inConnState = SDP_STATE_CONNECTED;
        if (connect->flag) {
            // Send client packet
            request = SdpFindRequestByAddress(&connect->addr);
            request->packetState = SDP_PACKET_SEND;
            SdpSendRequest(lcid, request->transactionId, 0, NULL, request->packet);
        }
    }
    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpConfigRspCallbackTsk(void *context)
{
    SdpConnectCallbackInfo *ctx = context;
    SdpConfigRspCallbackTask(ctx->lcid, ctx->result);
    MEM_MALLOC.free(ctx);
}

static void SdpConfigRspCallback(uint16_t lcid, int result)
{
    int ret;
    SdpConnectCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpConnectCallbackInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpConnectCallbackInfo), 0x00, sizeof(SdpConnectCallbackInfo));

    ctx->lcid = lcid;
    ctx->result = result;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpConfigRspCallbackTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

static void SdpDisconnectionRspCallbackTask(uint16_t lcid, int result)
{
    LOG_DEBUG("[%{public}s][%{public}d] lcid = 0x%04x start", __FUNCTION__, __LINE__, lcid);

    SdpConnectInfo *connect = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    if (result != BT_NO_ERROR) {
        LOG_ERROR("[%{public}s][%{public}d] Disconnect response callback failed", __FUNCTION__, __LINE__);
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] Disconnect response callback failed", __FUNCTION__, __LINE__);
        return;
    }
    ListRemoveNode(g_connectList, connect);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpDisconnectionRspCallbackTsk(void *context)
{
    SdpConnectCallbackInfo *ctx = context;
    SdpDisconnectionRspCallbackTask(ctx->lcid, ctx->result);
    MEM_MALLOC.free(ctx);
}

static void SdpDisconnectionRspCallback(uint16_t lcid, int result)
{
    int ret;
    SdpConnectCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpConnectCallbackInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpConnectCallbackInfo), 0x00, sizeof(SdpConnectCallbackInfo));

    ctx->lcid = lcid;
    ctx->result = result;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpDisconnectionRspCallbackTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

/**
 * @brief   Send connection request from L2CAP as a client.
 *
 * @param addr   The Bluetooth address of the peer.
 * @param packet The packet point for sending data.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
static void SdpSendConnectRequest(const BtAddr *addr)
{
    SdpConnectInfo *connect = NULL;

    connect = MEM_MALLOC.alloc(sizeof(SdpConnectInfo));
    if (connect == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(connect, sizeof(SdpConnectInfo), 0, sizeof(SdpConnectInfo));

    /// Save address
    (void)memcpy_s(&connect->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    /// Set client connect
    connect->inConnState = SDP_STATE_IDLE;
    connect->outConnState = SDP_STATE_IDLE;
    connect->flag = true;
    connect->wait = false;
    ListAddLast(g_connectList, connect);

    /// Send request to L2CAP
    L2CIF_ConnectReq(addr, SDP_PSM, SDP_PSM, NULL, SdpConnectReqCallback);
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConnectionInfo info;
    uint16_t lpsm;
    void *context;
} SdpReceiveConnectRequestInfo;
/**
 * @brief Receive connection request from L2CAP as a server.
 *
 * @param lcid    Local channel identifier.
 * @param id      Identifier.
 * @param info    L2CAP connection info.
 * @param lpsm    SDP_PSM (0x0001).
 * @param context The context from upper layer.
 */
static void SdpReceiveConnectRequestTask(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    if (!SdpGetEnableState()) {
        return;
    }

    /// Send connect response to L2CAP
    L2CIF_ConnectRsp(
        lcid, id, L2CAP_CONNECTION_SUCCESSFUL, L2CAP_NO_FURTHER_INFORMATION_AVAILABLE, SdpConnectRspCallback);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpReceiveConnectRequestTsk(void *context)
{
    SdpReceiveConnectRequestInfo *ctx = context;
    SdpReceiveConnectRequestTask(ctx->lcid, ctx->id, &ctx->info, ctx->lpsm, ctx->context);
    MEM_MALLOC.free(ctx);
}

static void SdpReceiveConnectRequest(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *context)
{
    int ret;
    SdpReceiveConnectRequestInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpReceiveConnectRequestInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpReceiveConnectRequestInfo), 0x00, sizeof(SdpReceiveConnectRequestInfo));

    ctx->lcid = lcid;
    ctx->id = id;
    (void)memcpy_s(&ctx->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    ctx->lpsm = lpsm;
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpReceiveConnectRequestTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

typedef struct {
    uint16_t lcid;
    L2capConnectionInfo info;
    uint16_t result;
    uint16_t status;
    void *context;
} SdpReceiveConnectResponseInfo;
/**
 * @brief Receive connection response from L2CAP as a client.
 *
 * @param lcid    Local channel identifier
 * @param info    L2CAP connection info.
 * @param result  L2CAP result
 * @param status  L2CAP status
 * @param context The context from upper layer.
 */
static void SdpReceiveConnectResponseTask(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    L2capConfigInfo config;
    SdpConnectInfo *connect = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_WARN("[%{public}s][%{public}d] Recv connect failed with unknown cid [0x%02x]", __FUNCTION__, __LINE__, lcid);
        SdpSendDisconnectRequest(lcid, false);
        return;
    }
    if (result == L2CAP_CONNECTION_PENDING) {
        LOG_WARN("[%{public}s][%{public}d] Recv connect response pending [0x%02x]", __FUNCTION__, __LINE__, lcid);
        return;
    } else if ((result != L2CAP_CONNECTION_SUCCESSFUL) || (connect->outConnState != SDP_STATE_CONN_SETUP) ||
               (connect->inConnState != SDP_STATE_CONN_SETUP)) {
        LOG_ERROR("[%{public}s][%{public}d] Recv connect response failed with cid [0x%02x]", __FUNCTION__, __LINE__, lcid);
        return;
    }

    /// stop timer when received a connect response
    if (connect->timer != NULL) {
        /// Cancel timeout
        AlarmCancel(connect->timer);
    }

    /// L2CAP default configuration. SDP only care about mtu.
    (void)memset_s(&config, sizeof(L2capConfigInfo), 0, sizeof(L2capConfigInfo));
    config.mtu = SDP_MTU_SIZE;
    config.flushTimeout = 0xFFFF;
    config.fcs = 0x01;
    L2CIF_ConfigReq(lcid, &config, SdpConfigReqCallback);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpReceiveConnectResponseTsk(void *context)
{
    SdpReceiveConnectResponseInfo *ctx = context;
    SdpReceiveConnectResponseTask(ctx->lcid, &ctx->info, ctx->result, ctx->status, ctx->context);
    MEM_MALLOC.free(ctx);
}

static void SdpReceiveConnectResponse(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *context)
{
    int ret;
    SdpReceiveConnectResponseInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpReceiveConnectResponseInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpReceiveConnectResponseInfo), 0x00, sizeof(SdpReceiveConnectResponseInfo));

    ctx->lcid = lcid;
    (void)memcpy_s(&ctx->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    ctx->result = result;
    ctx->status = status;
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpReceiveConnectResponseTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    L2capConfigInfo config;
    void *context;
} SdpReceiveConfigRequestInfo;
/**
 * @brief Receive Configuration response from L2CAP as a server or a client.
 *
 * @param lcid    Local channel identifier.
 * @param id      Identifier.
 * @param config  L2CAP configuration info.
 * @param context The context from upper layer.
 */
static void SdpReceiveConfigRequestTask(uint16_t lcid, uint8_t id, const L2capConfigInfo *config, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);

    SdpConnectInfo *connect = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] Recv config request failed with unknown cid [0x%02x]", __FUNCTION__, __LINE__, lcid);
        return;
    }
    connect->mtu = config->mtu;
    L2CIF_ConfigRsp(lcid, id, config, L2CAP_CONNECTION_SUCCESSFUL, SdpConfigRspCallback);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpReceiveConfigRequestTsk(void *context)
{
    SdpReceiveConfigRequestInfo *ctx = context;
    SdpReceiveConfigRequestTask(ctx->lcid, ctx->id, &ctx->config, ctx->context);
    MEM_MALLOC.free(ctx);
}

static void SdpReceiveConfigRequest(uint16_t lcid, uint8_t id, const L2capConfigInfo *config, void *context)
{
    int ret;
    SdpReceiveConfigRequestInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpReceiveConfigRequestInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpReceiveConfigRequestInfo), 0x00, sizeof(SdpReceiveConfigRequestInfo));

    ctx->lcid = lcid;
    ctx->id = id;
    (void)memcpy_s(&ctx->config, sizeof(L2capConfigInfo), config, sizeof(L2capConfigInfo));
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpReceiveConfigRequestTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

typedef struct {
    uint16_t lcid;
    L2capConfigInfo config;
    uint16_t result;
    void *context;
} SdpReceiveConfigResponseInfo;
/**
 * @brief Receive configuration response from L2CAP as a server or a client.
 *
 * @param lcid    Local channel identifier.
 * @param config  L2CAP configuration info.
 * @param result  Connect result.
 * @param context The context from upper layer.
 */
static void SdpReceiveConfigResponseTask(uint16_t lcid, const L2capConfigInfo *config, uint16_t result, void *context)
{
    SdpClientRequest *request = NULL;
    SdpConnectInfo *connect = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] Recv connect with unknown cid [0x%02x]", __FUNCTION__, __LINE__, lcid);
        return;
    }
    if ((result != L2CAP_CONNECTION_SUCCESSFUL) && (connect->outConnState != SDP_STATE_CONN_SETUP)) {
        return;
    }

    connect->outConnState = SDP_STATE_CFG_RSP_SETUP;
    if ((connect->outConnState == SDP_STATE_CFG_RSP_SETUP) && (connect->inConnState == SDP_STATE_CFG_REQ_SETUP)) {
        connect->outConnState = SDP_STATE_CONNECTED;
        connect->inConnState = SDP_STATE_CONNECTED;
        if (connect->flag) {
            // Send client packet
            request = SdpFindRequestByAddress(&connect->addr);
            request->packetState = SDP_PACKET_SEND;
            SdpSendRequest(connect->lcid, request->transactionId, 0, NULL, request->packet);
        }
    }
}

static void SdpReceiveConfigResponseTsk(void *context)
{
    SdpReceiveConfigResponseInfo *ctx = context;
    SdpReceiveConfigResponseTask(ctx->lcid, &ctx->config, ctx->result, ctx->context);
    MEM_MALLOC.free(ctx);
}

static void SdpReceiveConfigResponse(uint16_t lcid, const L2capConfigInfo *config, uint16_t result, void *context)
{
    int ret;
    SdpReceiveConfigResponseInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpReceiveConfigResponseInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }

    (void)memset_s(ctx, sizeof(SdpReceiveConfigResponseInfo), 0x00, sizeof(SdpReceiveConfigResponseInfo));

    ctx->lcid = lcid;
    (void)memcpy_s(&ctx->config, sizeof(L2capConfigInfo), config, sizeof(L2capConfigInfo));
    ctx->result = result;
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpReceiveConfigResponseTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

/**
 * @brief Send disconnection request from L2CAP as a client.
 *
 * @param lcid Local channel identifier.
 */
static void SdpSendDisconnectRequest(uint16_t lcid, bool wait)
{
    LOG_DEBUG("[%{public}s][%{public}d] lcid = 0x%04x start", __FUNCTION__, __LINE__, lcid);

    SdpConnectInfo *connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_WARN("[%{public}s][%{public}d] This connection has been disconnected.", __FUNCTION__, __LINE__);
        return;
    }
    connect->inConnState = SDP_STATE_DISCONNECT;
    connect->outConnState = SDP_STATE_DISCONNECT;
    connect->wait = wait;

    L2CIF_DisconnectionReq(lcid, NULL);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

typedef struct {
    uint16_t lcid;
    uint8_t id;
    void *context;
} SdpReceiveDisconnectRequestInfo;
/**
 * @brief Receive disconnection request from L2CAP as a server.
 *
 * @param lcid    Local channel identifier.
 * @param id      Identifier.
 * @param context The context from upper layer.
 */
static void SdpReceiveDisconnectRequestTask(uint16_t lcid, uint8_t id, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] lcid = 0x%04x start", __FUNCTION__, __LINE__, lcid);

    if (!SdpGetEnableState()) {
        return;
    }

    L2CIF_DisconnectionRsp(lcid, id, SdpDisconnectionRspCallback);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpReceiveDisconnectRequestTsk(void *context)
{
    SdpReceiveDisconnectRequestInfo *ctx = context;
    SdpReceiveDisconnectRequestTask(ctx->lcid, ctx->id, ctx->context);
    MEM_MALLOC.free(ctx);
}

static void SdpReceiveDisconnectRequest(uint16_t lcid, uint8_t id, void *context)
{
    int ret;
    SdpReceiveDisconnectRequestInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpReceiveDisconnectRequestInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpReceiveDisconnectRequestInfo), 0x00, sizeof(SdpReceiveDisconnectRequestInfo));

    ctx->lcid = lcid;
    ctx->id = id;
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpReceiveDisconnectRequestTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

typedef struct {
    uint16_t lcid;
    void *context;
} SdpReceiveDisconnectResponseInfo;
/**
 * @brief Receive disconnection response from L2CAP as a client.
 *
 * @param lcid    Local channel identifier.
 * @param context The context from upper layer.
 */
static void SdpReceiveDisconnectResponseTask(uint16_t lcid, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] lcid = 0x%04x start", __FUNCTION__, __LINE__, lcid);

    SdpConnectInfo *connect = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_WARN("[%{public}s][%{public}d] This connection has been disconnected.", __FUNCTION__, __LINE__);
        return;
    }
    if (!connect->wait) {
        SdpRemoveRequestByAddress(&connect->addr);
    }
    ListRemoveNode(g_connectList, connect);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpReceiveDisconnectResponseTsk(void *context)
{
    SdpReceiveDisconnectResponseInfo *ctx = context;
    SdpReceiveDisconnectResponseTask(ctx->lcid, ctx->context);
    MEM_MALLOC.free(ctx);
}

static void SdpReceiveDisconnectResponse(uint16_t lcid, void *context)
{
    int ret;
    SdpReceiveDisconnectResponseInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpReceiveDisconnectResponseInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpReceiveDisconnectResponseInfo), 0x00, sizeof(SdpReceiveDisconnectResponseInfo));

    ctx->lcid = lcid;
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpReceiveDisconnectResponseTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

typedef struct {
    uint16_t lcid;
    uint8_t reason;
    void *context;
} SdpDisconnectAbnormalInfo;
/**
 * @brief Receive abnormal disconnection response from L2CAP as a server or a client.
 *
 * @param lcid    Local channel identifier.
 * @param reason  The reason of abnormal disconnection.
 * @param context The context from upper layer.
 */
static void SdpDisconnectAbnormalTask(uint16_t lcid, uint8_t reason, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] lcid = 0x%04x reason = 0x%02x start", __FUNCTION__, __LINE__, lcid, reason);

    SdpConnectInfo *connect = NULL;
    SdpClientRequest *request = NULL;

    if (!SdpGetEnableState()) {
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_WARN("[%{public}s][%{public}d] This connection has been disconnected.", __FUNCTION__, __LINE__);
        return;
    }
#define HCI_COMMAND_DISALLOWED 0x0c
    if (connect->flag) {
        if (reason == HCI_COMMAND_DISALLOWED) {
            request = SdpFindRequestByAddress(&connect->addr);
            if (!request->resentFlag) {
                ListRemoveNode(g_connectList, connect);
                SdpSendConnectRequest(&request->addr);
                request->resentFlag = true;
                return;
            }
        } else if (reason == L2CAP_STATE_COLLISION) {
            request = SdpFindRequestByAddress(&connect->addr);
            ListRemoveNode(g_connectList, connect);
            SdpSendConnectRequest(&request->addr);
            return;
        }
        SdpRemoveAllRequestByAddress(&connect->addr);
    }
    ListRemoveNode(g_connectList, connect);

    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpDisconnectAbnormalTsk(void *context)
{
    SdpDisconnectAbnormalInfo *ctx = context;
    SdpDisconnectAbnormalTask(ctx->lcid, ctx->reason, ctx->context);
    MEM_MALLOC.free(ctx);
}
static void SdpDisconnectAbnormal(uint16_t lcid, uint8_t reason, void *context)
{
    int ret;
    SdpDisconnectAbnormalInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpDisconnectAbnormalInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpDisconnectAbnormalInfo), 0x00, sizeof(SdpDisconnectAbnormalInfo));

    ctx->lcid = lcid;
    ctx->reason = reason;
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpDisconnectAbnormalTsk, ctx);
    if (ret != BT_NO_ERROR) {
        MEM_MALLOC.free(ctx);
        return;
    }
}

typedef struct {
    uint16_t lcid;
    Packet *packet;
    void *context;
} SdpReceiveDataInfo;
/**
 * @brief Receive packet from L2CAP as a server or client.
 *
 * @param lcid    Local channel identifier
 * @param packet  The packet point for receiving data
 * @param context The context from upper layer.
 * @return    void
 */
static void SdpReceiveDataTask(uint16_t lcid, const Packet *packet, void *context)
{
    LOG_DEBUG("[%{public}s][%{public}d] start", __FUNCTION__, __LINE__);
    SdpConnectInfo *connect = NULL;
    SdpClientRequest *request = NULL;
    bool flag = false;

    if (!SdpGetEnableState()) {
        return;
    }

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] Recv connect with unknown cid [0x%02x]", __FUNCTION__, __LINE__, lcid);
        return;
    }
    if ((connect->outConnState == SDP_STATE_CONNECTED) && (connect->inConnState == SDP_STATE_CONNECTED)) {
        if (connect->flag) {
            SdpParseServerResponse(&connect->addr, lcid, packet);
            request = SdpFindRemainRequestByAddress(&connect->addr, &flag);
            if (request != NULL) {
                request->packetState = SDP_PACKET_SEND;
                SdpSendRequest(connect->lcid, request->transactionId, 0, NULL, request->packet);
            } else if (!flag) {
                AlarmSet(connect->timer, SDP_CONNECT_WAIT_TIME, SdpConnectWaitTime, connect);
            }
        } else {
            SdpParseClientRequest(lcid, packet);
        }
    }
    LOG_DEBUG("[%{public}s][%{public}d] end", __FUNCTION__, __LINE__);
}

static void SdpReceiveDataTsk(void *context)
{
    SdpReceiveDataInfo *ctx = context;
    SdpReceiveDataTask(ctx->lcid, ctx->packet, ctx->context);
    PacketFree(ctx->packet);
    ctx->packet = NULL;
    MEM_MALLOC.free(ctx);
}

static void SdpReceiveData(uint16_t lcid, Packet *packet, void *context)
{
    int ret;
    SdpReceiveDataInfo *ctx = MEM_MALLOC.alloc(sizeof(SdpReceiveDataInfo));
    if (ctx == NULL) {
        LOG_ERROR("point to NULL");
        return;
    }
    (void)memset_s(ctx, sizeof(SdpReceiveDataInfo), 0x00, sizeof(SdpReceiveDataInfo));

    ctx->lcid = lcid;
    ctx->packet = PacketRefMalloc(packet);
    ctx->context = context;

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_SDP, SdpReceiveDataTsk, ctx);
    if (ret != BT_NO_ERROR) {
        PacketFree(ctx->packet);
        ctx->packet = NULL;
        MEM_MALLOC.free(ctx);
        return;
    }
}

void SdpSendErrorResponse(uint16_t lcid, uint16_t transactionId, uint16_t errorCode)
{
    Packet *packet = NULL;
    uint8_t buffer[2] = {0};
    uint8_t *header = NULL;
    uint16_t offset = 0;

    packet = PacketMalloc(SDP_PDU_HEADER_LENGTH, 0, SDP_UINT16_LENGTH);
    *(uint16_t *)buffer = H2BE_16(errorCode);
    PacketPayloadWrite(packet, buffer, 0, SDP_UINT16_LENGTH);

    header = (uint8_t *)BufferPtr(PacketHead(packet));
    /// PduID
    header[0] = SDP_ERROR_RESPONSE;
    offset++;
    /// Transaction ID
    *(uint16_t *)(header + offset) = H2BE_16(transactionId);
    offset += SDP_UINT16_LENGTH;
    /// ParameterLength
    *(uint16_t *)(header + offset) = H2BE_16(SDP_UINT16_LENGTH);

    L2CIF_SendData(lcid, packet, NULL);
    PacketFree(packet);
    packet = NULL;
}

static uint16_t SdpGetCurrentServiceRecordCount(uint16_t mtu, uint16_t maxCount, uint16_t totalServiceRecordCount)
{
    uint16_t number = (mtu - SDP_PACKET_HEADER_AND_TAIL_LENGTH) / SDP_SERVICE_RECORD_HANDLE_BYTE;
    if (maxCount > number) {
        return number;
    }
    return maxCount;
}

static Packet *SdpBuildSearchFragmentResponse(uint16_t transactionId, size_t size, uint16_t totalServiceRecordCount,
    uint16_t currentServiceRecordCount, Packet *fragmentPacket)
{
    Packet *sendPacket = NULL;
    uint8_t *header = NULL;
    uint8_t *tail = NULL;
    uint16_t length = 0;
    uint16_t offset = 0;

    /// ContinuationState
    if (size == 0) {
        sendPacket =
            PacketInheritMalloc(fragmentPacket, SDP_PDU_HEADER_LENGTH + SDP_UINT32_LENGTH, SDP_PACKET_TAIL_ONE_BYTE);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x00;
        length = currentServiceRecordCount * SDP_UINT32_LENGTH + SDP_UINT32_LENGTH + SDP_PACKET_TAIL_ONE_BYTE;
    } else if (size <= 0xFF) {
        sendPacket =
            PacketInheritMalloc(fragmentPacket, SDP_PDU_HEADER_LENGTH + SDP_UINT32_LENGTH, SDP_PACKET_TAIL_TWO_BYTE);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x01;
        tail[1] = size & 0xFF;
        length = currentServiceRecordCount * SDP_UINT32_LENGTH + SDP_UINT32_LENGTH + SDP_PACKET_TAIL_TWO_BYTE;
    } else if (size <= 0xFFFF) {
        sendPacket =
            PacketInheritMalloc(fragmentPacket, SDP_PDU_HEADER_LENGTH + SDP_UINT32_LENGTH, SDP_PACKET_TAIL_THREE_BYTE);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x02;
        *(uint16_t *)(tail + 1) = H2BE_16(size);
        length = currentServiceRecordCount * SDP_UINT32_LENGTH + SDP_UINT32_LENGTH + SDP_PACKET_TAIL_THREE_BYTE;
    } else {
        LOG_ERROR("[%{public}s][%{public}d] Invalid continuation length [%zu]", __FUNCTION__, __LINE__, size);
        return NULL;
    }
    header = BufferPtr(PacketHead(sendPacket));
    /// PduID
    header[offset] = SDP_SERVICE_SEARCH_RESPONSE;
    offset++;
    /// Transaction ID
    *(uint16_t *)(header + offset) = H2BE_16(transactionId);
    offset += SDP_UINT16_LENGTH;
    /// ParameterLength
    *(uint16_t *)(header + offset) = H2BE_16(length);
    offset += SDP_UINT16_LENGTH;
    /// TotalServiceRecordCount
    *(uint16_t *)(header + offset) = H2BE_16(totalServiceRecordCount);
    offset += SDP_UINT16_LENGTH;
    /// CurrentServiceRecordCount
    *(uint16_t *)(header + offset) = H2BE_16(currentServiceRecordCount);

    return sendPacket;
}

void SdpSendSearchFragmentResponse(uint16_t lcid, uint16_t transactionId, uint16_t maxCount, const Packet *searchPacket)
{
    SdpConnectInfo *connect = NULL;
    uint16_t totalServiceRecordCount;
    uint16_t currentServiceRecordCount;
    Packet *packet = NULL;
    Packet *fragmentPacket = NULL;
    Packet *sendPacket = NULL;
    size_t size;

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        return;
    }

    if (searchPacket == NULL) {
        if (connect->packet == NULL) {
            return;
        }
        packet = connect->packet;
        totalServiceRecordCount = connect->totalCount;
    } else {
        packet = PacketRefMalloc(searchPacket);
        totalServiceRecordCount = PacketSize(packet) / SDP_SERVICE_RECORD_HANDLE_BYTE;
    }
    currentServiceRecordCount = SdpGetCurrentServiceRecordCount(connect->mtu, maxCount, totalServiceRecordCount);

    fragmentPacket = PacketMalloc(0, 0, 0);
    size = PacketFragment(packet, fragmentPacket, currentServiceRecordCount * SDP_SERVICE_RECORD_HANDLE_BYTE);
    currentServiceRecordCount = PacketSize(fragmentPacket) / SDP_SERVICE_RECORD_HANDLE_BYTE;

    sendPacket = SdpBuildSearchFragmentResponse(
        transactionId, size, totalServiceRecordCount, currentServiceRecordCount, fragmentPacket);
    if (sendPacket == NULL) {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_CONT_STATE);
        PacketFree(fragmentPacket);
        fragmentPacket = NULL;
        return;
    }

    /// Send data
    L2CIF_SendData(lcid, sendPacket, NULL);

    if (size != 0) {
        connect->packet = packet;
        connect->totalCount = totalServiceRecordCount;
    } else {
        PacketFree(packet);
        connect->packet = NULL;
        packet = NULL;
    }

    PacketFree(fragmentPacket);
    fragmentPacket = NULL;
    PacketFree(sendPacket);
    sendPacket = NULL;
}

void SdpSendSearchResponse(uint16_t lcid, uint16_t transactionId, uint16_t offset, uint8_t *buffer, uint16_t maxCount)
{
    SdpConnectInfo *connect = NULL;
    uint16_t totalServiceRecordCount;
    uint16_t number;
    uint16_t length;
    Packet *packet = NULL;
    Packet *sendPacket = NULL;
    uint8_t *header = NULL;
    uint8_t *tail = NULL;
    uint16_t pos = 0;

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        return;
    }

    /// ServiceSearchPattern
    packet = PacketMalloc(0, 0, offset);
    PacketPayloadWrite(packet, buffer, 0, offset);

    /// TotalServiceRecordCount
    totalServiceRecordCount = offset / SDP_SERVICE_RECORD_HANDLE_BYTE;
    if (totalServiceRecordCount > maxCount) {
        totalServiceRecordCount = maxCount;
    }
    number = (connect->mtu - SDP_PACKET_HEADER_AND_TAIL_LENGTH - 1) / SDP_SERVICE_RECORD_HANDLE_BYTE;
    if (totalServiceRecordCount > number) {
        /// Fragment packet
        SdpSendSearchFragmentResponse(lcid, transactionId, maxCount, packet);
        PacketFree(packet);
        packet = NULL;
        return;
    }

    /// Single packet
    length = totalServiceRecordCount * SDP_SERVICE_RECORD_HANDLE_BYTE + SDP_UINT32_LENGTH + 1;
    sendPacket = PacketInheritMalloc(packet, SDP_PDU_HEADER_LENGTH + SDP_UINT32_LENGTH, 1);
    header = BufferPtr(PacketHead(sendPacket));
    tail = BufferPtr(PacketTail(sendPacket));
    /// PduID
    header[pos] = SDP_SERVICE_SEARCH_RESPONSE;
    pos++;
    /// Transaction ID
    *(uint16_t *)(header + pos) = H2BE_16(transactionId);
    pos += SDP_UINT16_LENGTH;
    /// ParameterLength
    *(uint16_t *)(header + pos) = H2BE_16(length);
    pos += SDP_UINT16_LENGTH;
    /// TotalServiceRecordCount
    *(uint16_t *)(header + pos) = H2BE_16(totalServiceRecordCount);
    pos += SDP_UINT16_LENGTH;
    /// CurrentServiceRecordCount = TotalServiceRecordCount
    *(uint16_t *)(header + pos) = H2BE_16(totalServiceRecordCount);
    /// ContinuationState
    tail[0] = 0;

    /// Send packet
    L2CIF_SendData(lcid, sendPacket, NULL);

    PacketFree(packet);
    packet = NULL;
    PacketFree(sendPacket);
    sendPacket = NULL;
}

static Packet *SdpBuildAttributeFragmentResponse(
    SdpPduId pduId, uint16_t transactionId, size_t size, Packet *fragmentPacket)
{
    Packet *sendPacket = NULL;
    uint16_t length = 0;
    uint16_t offset = 0;
    uint8_t *header = NULL;
    uint8_t *tail = NULL;

    /// AttributeByteCount
    uint16_t attributeByteCount = (uint16_t)PacketSize(fragmentPacket);

    /// ContinuationState
    if (size == 0) {
        sendPacket =
            PacketInheritMalloc(fragmentPacket, SDP_PDU_HEADER_LENGTH + SDP_UINT16_LENGTH, SDP_PACKET_TAIL_ONE_BYTE);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x00;
        length = attributeByteCount + SDP_UINT16_LENGTH + SDP_PACKET_TAIL_ONE_BYTE;
    } else if (size <= 0xFF) {
        sendPacket =
            PacketInheritMalloc(fragmentPacket, SDP_PDU_HEADER_LENGTH + SDP_UINT16_LENGTH, SDP_PACKET_TAIL_TWO_BYTE);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x01;
        tail[1] = size & 0xFF;
        length = attributeByteCount + SDP_UINT16_LENGTH + SDP_PACKET_TAIL_TWO_BYTE;
    } else if (size <= 0xFFFF) {
        sendPacket =
            PacketInheritMalloc(fragmentPacket, SDP_PDU_HEADER_LENGTH + SDP_UINT16_LENGTH, SDP_PACKET_TAIL_THREE_BYTE);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x02;
        *(uint16_t *)(tail + 1) = H2BE_16((uint16_t)size);
        length = attributeByteCount + SDP_UINT16_LENGTH + SDP_PACKET_TAIL_THREE_BYTE;
    } else {
        LOG_ERROR("[%{public}s][%{public}d] Invalid continuation length [%zu]", __FUNCTION__, __LINE__, size);
        return NULL;
    }

    header = BufferPtr(PacketHead(sendPacket));
    /// PduID
    header[offset] = pduId;
    offset++;
    /// Transaction ID
    *(uint16_t *)(header + offset) = H2BE_16(transactionId);
    offset += SDP_UINT16_LENGTH;
    /// ParameterLength
    *(uint16_t *)(header + offset) = H2BE_16(length);
    offset += SDP_UINT16_LENGTH;
    /// AttributeListByteCount
    *(uint16_t *)(header + offset) = H2BE_16(attributeByteCount);

    return sendPacket;
}

void SdpSendAttributeFragmentResponse(
    uint16_t lcid, SdpPduId pduId, uint16_t transactionId, uint16_t maxCount, const Packet *attributePacket)
{
    SdpConnectInfo *connect = NULL;
    Packet *packet = NULL;
    Packet *fragmentPacket = NULL;
    Packet *sendPacket = NULL;
    size_t size;

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        return;
    }

    if (attributePacket == NULL) {
        if (connect->packet == NULL) {
            return;
        }
        packet = connect->packet;
    } else {
        packet = PacketRefMalloc(attributePacket);
    }

    fragmentPacket = PacketMalloc(0, 0, 0);
    size = PacketFragment(packet, fragmentPacket, maxCount);

    sendPacket = SdpBuildAttributeFragmentResponse(pduId, transactionId, size, fragmentPacket);
    if (sendPacket == NULL) {
        SdpSendErrorResponse(lcid, transactionId, SDP_INVALID_CONT_STATE);
        return;
    }

    /// Send data
    L2CIF_SendData(lcid, sendPacket, NULL);

    if (size != 0) {
        connect = SdpFindConnectByCid(lcid);
        if (connect == NULL) {
            LOG_ERROR("point to NULL");
            return;
        }
        /// store remain packet
        connect->packet = packet;
    } else {
        PacketFree(packet);
        packet = NULL;
        connect->packet = NULL;
    }

    PacketFree(fragmentPacket);
    fragmentPacket = NULL;
    PacketFree(sendPacket);
    sendPacket = NULL;
}

void SdpSendAttributeResponse(
    uint16_t lcid, uint16_t transactionId, SdpPduId pduId, uint16_t maxCount, const Packet *packet)
{
    SdpConnectInfo *connect = NULL;
    Packet *sendPacket = NULL;
    uint16_t length;
    uint8_t *header = NULL;
    uint8_t *tail = NULL;

    connect = SdpFindConnectByCid(lcid);
    if (connect == NULL) {
        LOG_ERROR("[%{public}s][%{public}d] There is no connect with cid [%hu]", __FUNCTION__, __LINE__, lcid);
        return;
    }

    if (maxCount > (connect->mtu - SDP_PACKET_HEADER_AND_TAIL_LENGTH)) {
        maxCount = connect->mtu - SDP_PACKET_HEADER_AND_TAIL_LENGTH;
    }

    length = PacketSize(packet);
    if (length > maxCount) {
        /// Fragment packet
        SdpSendAttributeFragmentResponse(lcid, pduId, transactionId, maxCount, packet);
        return;
    } else {
        /// Single packet
        uint16_t offset = 0;
        sendPacket = PacketInheritMalloc(packet, SDP_PDU_HEADER_LENGTH + SDP_UINT16_LENGTH, SDP_PACKET_TAIL_ONE_BYTE);
        header = BufferPtr(PacketHead(sendPacket));
        tail = BufferPtr(PacketTail(sendPacket));
        /// PduID
        header[offset] = pduId;
        offset++;
        /// Transaction ID
        *(uint16_t *)(header + offset) = H2BE_16(transactionId);
        offset += SDP_UINT16_LENGTH;
        /// ParameterLength
        *(uint16_t *)(header + offset) = H2BE_16(length + SDP_UINT16_LENGTH + SDP_PACKET_TAIL_ONE_BYTE);
        offset += SDP_UINT16_LENGTH;
        /// AttributeListByteCount
        *(uint16_t *)(header + offset) = H2BE_16(length);
        /// ContinuationState
        tail[0] = 0;

        /// Send packet
        L2CIF_SendData(lcid, sendPacket, NULL);
        PacketFree(sendPacket);
        sendPacket = NULL;
    }
}

void SdpSendRequest(uint16_t lcid, uint16_t transactionId, uint8_t continuationStateLen,
    const uint8_t *continuationState, Packet *packet)
{
    SdpClientRequest *request = NULL;
    Packet *sendPacket = NULL;
    uint8_t *header = NULL;
    uint8_t *tail = NULL;

    uint16_t length = PacketPayloadSize(packet);
    /// ContinuationState
    if (continuationStateLen == 0) {
        sendPacket = PacketInheritMalloc(packet, SDP_PDU_HEADER_LENGTH, 1);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x00;
        length += 1;
    } else {
        sendPacket = PacketInheritMalloc(packet, SDP_PDU_HEADER_LENGTH, continuationStateLen + 1);
        tail = BufferPtr(PacketTail(sendPacket));
        tail[0] = 0x01;
        (void)memcpy_s(tail, continuationStateLen + 1, continuationState, continuationStateLen + 1);
        length = length + continuationStateLen + 1;
    }

    header = (uint8_t *)BufferPtr(PacketHead(sendPacket));
    /// PduID
    request = SdpFindRequestByTransactionId(transactionId);
    if (request == NULL) {
        return;
    }
    header[0] = request->pduId;
    /// Transaction ID
    *(uint16_t *)(header + 1) = H2BE_16(transactionId);
    /// ParameterLength
    *(uint16_t *)(header + SDP_UINT16_LENGTH + 1) = H2BE_16(length);

    /// Send packet
    L2CIF_SendData(lcid, sendPacket, NULL);

    PacketFree(sendPacket);
    sendPacket = NULL;
}

int SdpClientConnect(SdpClientRequest *request)
{
    SdpConnectInfo *connect = NULL;

    if (g_connectList == NULL) {
        return BT_OPERATION_FAILED;
    }
    connect = SdpFindConnectByAddress(&request->addr);
    if (connect == NULL) {
        /// Create new channel
        SdpAddRequest(request);
        SdpSendConnectRequest(&request->addr);
    } else {
        /// Use existed channel
        if ((connect->inConnState == SDP_STATE_CONNECTED) && (connect->outConnState == SDP_STATE_CONNECTED)) {
            /// Channel idle and send packet
            if (connect->timer != NULL) {
                AlarmCancel(connect->timer);
            }
            request->packetState = SDP_PACKET_SEND;
            SdpAddRequest(request);
            SdpSendRequest(connect->lcid, request->transactionId, 0, NULL, request->packet);
        } else if ((connect->inConnState == SDP_STATE_DISCONNECT) && (connect->outConnState == SDP_STATE_DISCONNECT)) {
            /// Create new channel
            SdpAddRequest(request);
            SdpSendConnectRequest(&request->addr);
        } else {
            SdpAddRequest(request);
        }
    }

    return BT_NO_ERROR;
}
