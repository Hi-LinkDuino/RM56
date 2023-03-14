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

typedef struct {
    uint16_t result;
    GapServiceSecurityInfo serviceInfo;
    void *context;
} RfcommSecurityResultInfo;

static void RfcommRecvChannelSecurityResultTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSecurityResultInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvChannelSecurityResult(ctx->result, ctx->serviceInfo, ctx->context);

    free(ctx);
}

/**
 * @brief This is the callback function registered when GAPIF_RequestSecurityAsync is called
 *        to receive the result of the security check.
 *
 * @param result      The result of the security check.
 * @param serviceInfo server information.
 * @param context     The context passed in when calling GAPIF_RequestSecurityAsync.
 */
static void RfcommRecvChannelSecurityResultCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_INFO("%{public}s result:%hu", __func__, result);

    if (context == NULL) {
        LOG_ERROR("%{public}s:DLC is closed.", __func__);
        return;
    }

    RfcommSecurityResultInfo *ctx = malloc(sizeof(RfcommSecurityResultInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommSecurityResultInfo), 0x00, sizeof(RfcommSecurityResultInfo));
    ctx->result = result;
    ctx->context = context;
    (void)memcpy_s(&ctx->serviceInfo, sizeof(GapServiceSecurityInfo), &serviceInfo, sizeof(GapServiceSecurityInfo));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvChannelSecurityResultTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

static void RfcommRecvSessionSecurityResultTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSecurityResultInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRecvSessionSecurityResult(ctx->result, ctx->serviceInfo, ctx->context);

    free(ctx);
}

static void RfcommRecvSessionSecurityResultCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_INFO("%{public}s result:%hu", __func__, result);

    if (context == NULL) {
        LOG_ERROR("%{public}s:Session is closed.", __func__);
        return;
    }

    RfcommSecurityResultInfo *ctx = malloc(sizeof(RfcommSecurityResultInfo));
    if (ctx == NULL) {
        return;
    }

    (void)memset_s(ctx, sizeof(RfcommSecurityResultInfo), 0x00, sizeof(RfcommSecurityResultInfo));
    ctx->result = result;
    ctx->context = context;
    (void)memcpy_s(&ctx->serviceInfo, sizeof(GapServiceSecurityInfo), &serviceInfo, sizeof(GapServiceSecurityInfo));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRecvSessionSecurityResultTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }
}

/**
 * @brief Before initiating a connection or accepting a connection,
 *        the GAP interface is called to perform a security check.
 *
 * @param channel  The pointer of the channel in the channel list.
 * @param isServer Server side:true, client side:false.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommCheckChannelSecurity(RfcommChannelInfo *channel, bool isServer)
{
    LOG_INFO("%{public}s isServer:%{public}d", __func__, isServer);

    GapRequestSecurityParam param;
    (void)memset_s(&param, sizeof(GapRequestSecurityParam), 0x00, sizeof(GapRequestSecurityParam));

    param.callback = RfcommRecvChannelSecurityResultCallback;
    param.info.channelId.rfcommChannel = channel->dlci >> RFCOMM_DLCI_SHIFT_SCN;
    param.info.direction = isServer ? INCOMING : OUTGOING;
    param.info.protocolId = SEC_PROTOCOL_RFCOMM;
    param.info.serviceId = UNKNOWN_SERVICE;
    param.context = channel;

    int ret = GAPIF_RequestSecurityAsync(&(channel->session->btAddr), &param);
    if (ret != BT_NO_ERROR) {
        LOG_DEBUG("%{public}s return error.", __func__);
    }

    return ret;
}

/**
 * @brief Before initiating a connection or accepting a L2CAP connection,
 *        the GAP interface is called to perform a security check.
 *
 * @param session  The pointer of the session in the session list.
 * @param scn      The rfcomm server channel number(Valid if this is initiator).
 * @param isServer Server side:true, client side:false.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommCheckSessionSecurity(RfcommSessionInfo *session, uint8_t scn, bool isServer)
{
    LOG_INFO("%{public}s isServer:%{public}d", __func__, isServer);

    GapRequestSecurityParam param;
    (void)memset_s(&param, sizeof(GapRequestSecurityParam), 0x00, sizeof(GapRequestSecurityParam));

    param.callback = RfcommRecvSessionSecurityResultCallback;
    param.context = (void*)session;
    if (isServer) {
        param.info.direction = INCOMING;
        param.info.serviceId = RFCOMM;
        param.info.protocolId = SEC_PROTOCOL_L2CAP;
        param.info.channelId.l2capPsm = BT_PSM_RFCOMM;
    } else {
        param.info.direction = OUTGOING;
        param.info.serviceId = UNKNOWN_SERVICE;
        param.info.protocolId = SEC_PROTOCOL_RFCOMM;
        param.info.channelId.rfcommChannel = scn;
    }

    int ret = GAPIF_RequestSecurityAsync(&(session->btAddr), &param);
    if (ret != BT_NO_ERROR) {
        LOG_DEBUG("%{public}s return error.", __func__);
    }

    return ret;
}

/**
 * @brief Register security information to GAP.
 *
 */
void RfcommRegisterSecurity()
{
    LOG_INFO("%{public}s", __func__);

    GapServiceSecurityInfo serviceInfo = {INCOMING, RFCOMM, SEC_PROTOCOL_L2CAP, {BT_PSM_RFCOMM}};

    int ret = GAPIF_RegisterServiceSecurityAsync(NULL, &serviceInfo, 0);
    if (ret != BT_NO_ERROR) {
        LOG_DEBUG("%{public}s return error.", __func__);
    }
}

/**
 * @brief Deregister security information to GAP.
 *
 */
void RfcommDeregisterSecurity()
{
    LOG_INFO("%{public}s", __func__);

    GapServiceSecurityInfo serviceInfo = {INCOMING, RFCOMM, SEC_PROTOCOL_L2CAP, {BT_PSM_RFCOMM}};

    int ret = GAPIF_DeregisterServiceSecurityAsync(NULL, &serviceInfo);
    if (ret != BT_NO_ERROR) {
        LOG_DEBUG("%{public}s return error.", __func__);
    }
}