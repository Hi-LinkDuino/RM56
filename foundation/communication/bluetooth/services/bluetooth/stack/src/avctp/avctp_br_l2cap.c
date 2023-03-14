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
#include "avctp_br_l2cap.h"
#include "avctp_br.h"
#include "avctp_ctrl.h"
#include "avctp_dev.h"
#include "avctp_gap.h"
#include "avctp_st.h"
#include "log.h"
#include "securec.h"

/*************************************************************************************************
 * Functions Declare
 *************************************************************************************************/

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/* L2CAP callback function structure */
const L2capService G_AVCT_BR_L2C_SVR = {
    AvctBrRecvConnectionReqCallback,
    AvctBrRecvConnectionRspCallback,
    AvctBrRecvConfigReqCallback,
    AvctBrRecvConfigRspCallback,
    AvctBrRecvDisconnectionReqCallback,
    AvctBrRecvDisconnectionRspCallback,
    AvctBrDisconnectAbnormalCBack,
    AvctBrRecvDataCBack,
    AvctBrRemoteBusyCBack
    };

/*****************************************************************************
 * Function
 ****************************************************************************/
/*
 * Function     AvctBrRecvConnectionReqCallback
 * Description  This function is called back when reveive connect req from peer.
 * Param[in]    aclHandle  ACL handle.
 * Param[in]    lcid   The link channel id.
 * Param[in]    id
 * Param[in]    psm
 * Return       void
 */

void AvctBrRecvConnectionReqCallback(
    uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x),lpsm(0x%x)", __func__, lcid, lpsm);
    AvctBrRecvConnectionReqCallbackTskParam *param = malloc(sizeof(AvctBrRecvConnectionReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctBrRecvConnectionReqCallbackTskParam), 0, sizeof(AvctBrRecvConnectionReqCallbackTskParam));

    param->lcid = lcid;
    param->id = id;
    (void)memcpy_s(&param->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    param->lpsm = lpsm;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRecvConnectionReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrRecvConnectionReqCallbackTsk(void *context)
{
    AvctBrRecvConnectionReqCallbackTskParam *param = (AvctBrRecvConnectionReqCallbackTskParam *)context;
    AvctBrRecvConnectionReqCBack(param->lcid, param->id, &param->info, param->lpsm, param->ctx);
    free(param);
    return;
}

void AvctBrRecvConnectionReqCBack(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s:lcid(0x%x),lpsm(0x%x)", __func__, lcid, lpsm);
    if (lpsm == AVCT_BR_PSM) {
        AvctRequestSecurityBy(info->handle, info->addr, lcid, id, lpsm);
    } else {
        L2CIF_ConnectRsp(lcid, id, L2CAP_PSM_NOT_SUPPORTED, 0, NULL);
    }
    return;
}

/*
 * Function     AvctBrRecvConnectionReqAct
 * Description  Receive Connection Request action func
 * Param[in]    addr    address.
 * Param[in]    aclHandle   acl handle.
 * Param[in]    lcid    link channel id
 * Param[in]    id      id
 * Param[in]    psm     psm
 * Return       void
 */
void AvctBrRecvConnectionReqAct(const BtAddr *addr, uint16_t aclHandle, uint16_t lcid, uint8_t id, uint16_t psm)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x)", __func__, lcid);
    AvctCbCh *cbBr = NULL;
    AvctCbDev *cbDev = NULL;
    uint16_t result = L2CAP_CONNECTION_SUCCESSFUL;
    if (psm != AVCT_BR_PSM) {
        LOG_ERROR("[AVCT]The Psm is not Avct");
        result = L2CAP_PSM_NOT_SUPPORTED;
    } else {
        /* Check the devcie connection is exist */
        cbDev = AvctGetCbDevByAddress(addr);
        if (cbDev == NULL) {
            /* No resource,can't connect */
            result = L2CAP_NO_RESOURCES_AVAILABLE;
        } else if (!(cbDev->chLink & AVCT_ALLOC_CTRL)) {
            /* The control channel is not connect,can't connect */
            result = L2CAP_NO_RESOURCES_AVAILABLE;
        } else if (cbDev->chLink & AVCT_ALLOC_BR) {
            /* The br channel is exist */
            result = L2CAP_SOURCE_CID_ALREADY_ALLOCATED;
        } else {
            /* Alloc br channel resource */
            cbBr = AvctCbChAlloc();
            if (cbBr == NULL) {
                result = L2CAP_NO_RESOURCES_AVAILABLE;
            }
        }
    }
    /* send L2CAP connection response  */
    L2CIF_ConnectRsp(lcid, id, result, 0, NULL);

    /* if connect success,send config requeset */
    if (result == L2CAP_CONNECTION_SUCCESSFUL) {
        cbBr->chId = lcid;
        cbBr->chState = AVCT_CH_CFG;
        cbDev->cbBr = cbBr;
        cbDev->chLink |= AVCT_ALLOC_BR;
        cbDev->cbBr->ia = AVCT_ACPT;
        L2capConfigInfo cfg = {0};
        cfg.mtu = g_avctMng.rxMtuBr;
        cfg.flushTimeout = 0xFFFF;
        cfg.fcs = 0x01;
        cfg.rfc.mode = L2CAP_ENHANCED_RETRANSMISSION_MODE;
        if (L2CIF_ConfigReq(lcid, &cfg, NULL)) {
            LOG_ERROR("[AVCT] %{public}s:L2CIF_ConfigReq failed.", __func__);
        }
    }
    return;
}

/*
 * Function     AvctBrRecvConnectionRspCBack
 * Description  This function is called back when reveived connection response.
 *              from peer.
 * Param[in]    lcid The link channel id.
 * Param[in]    result  the connection request result
 * Param[in]    status
 * Return       void
 */
void AvctBrRecvConnectionRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x), result(%{public}d)", __func__, lcid, result);
    AvctBrRecvConnectionRspCallbackTskParam *param = malloc(sizeof(AvctBrRecvConnectionRspCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctBrRecvConnectionRspCallbackTskParam), 0, sizeof(AvctBrRecvConnectionRspCallbackTskParam));
    param->lcid = lcid;
    (void)memcpy_s(&param->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    param->result = result;
    param->status = status;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRecvConnectionRspCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrRecvConnectionRspCallbackTsk(void *context)
{
    AvctBrRecvConnectionRspCallbackTskParam *param = (AvctBrRecvConnectionRspCallbackTskParam *)context;
    AvctBrRecvConnectionRspCBack(param->lcid, &param->info, param->result, param->status, param->ctx);
    free(param);
    return;
}

void AvctBrRecvConnectionRspCBack(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s: lcid(0x%x), result(%{public}d)", __func__, lcid, result);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev == NULL) {
        LOG_ERROR("[AVCT] %{public}s:AvctGetCbDevByChId(0x%x) failed.", __func__, lcid);
        return;
    }
    if (cbDev->cbBr->chState == AVCT_CH_CONN) {
        if (result == L2CAP_CONNECTION_PENDING) {
            LOG_INFO("[AVCT] %{public}s: Connect req result is pending, do nothing", __func__);
            return;
        }
        if (result == L2CAP_CONNECTION_SUCCESSFUL) {
            /* connect success ,send config request */
            cbDev->cbBr->chState = AVCT_CH_CFG;
            L2capConfigInfo cfg = {0};
            (void)memset_s(&cfg, sizeof(L2capConfigInfo), 0, sizeof(L2capConfigInfo));
            cfg.mtu = g_avctMng.rxMtuBr;
            cfg.flushTimeout = 0xFFFF;
            cfg.fcs = 0x01;
            cfg.rfc.mode = L2CAP_ENHANCED_RETRANSMISSION_MODE;
            if (L2CIF_ConfigReq(lcid, &cfg, NULL)) {
                LOG_ERROR("[AVCT] %{public}s:L2CIF_ConfigReq failed.", __func__);
            }
        } else {
            /* connect failed,send event to up app */
            AvctEvtData evtData;
            evtData.result = result;
            AvctCbBrEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
        }
    }
    return;
}

/*
 * Function     AvctBrRecvConfigReqCBack
 * Description  This function is called back when reveived config request
 *              from peer.
 * Param[in]    *addr  The address of the peer device.
 * Param[in]    lcid The link channel id.
 * Param[in]    id
 * Param[in]    psm
 * Return       void
 */
void AvctBrRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x), mtu(%hu)", __func__, lcid, cfg->mtu);
    AvctBrRecvConfigReqCallbackTskParam *param = malloc(sizeof(AvctBrRecvConfigReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctBrRecvConfigReqCallbackTskParam), 0, sizeof(AvctBrRecvConfigReqCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    (void)memcpy_s(&param->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRecvConfigReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrRecvConfigReqCallbackTsk(void *context)
{
    AvctBrRecvConfigReqCallbackTskParam *param = (AvctBrRecvConfigReqCallbackTskParam *)context;
    AvctBrRecvConfigReqCBack(param->lcid, param->id, &param->cfg, param->ctx);
    free(param);
    return;
}

void AvctBrRecvConfigReqCBack(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s: lcid(0x%x), mtu(%hu)", __func__, lcid, cfg->mtu);
    uint16_t result = L2CAP_SUCCESS;
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        /* send L2CAP configure response */
        if (L2CIF_ConfigRsp(lcid, id, cfg, result, NULL)) {
            LOG_ERROR("[AVCT] %{public}s:L2CIF_ConfigReq failed.", __func__);
        }
        cbDev->cbBr->peerMtu = cfg->mtu;
        cbDev->cbBr->chCfgSt |= AVCT_L2C_CFG_REQ;
        /* There are two step of config process */
        if (cbDev->cbBr->chCfgSt & AVCT_L2C_CFG_RSP) {
            /* config req/rsp reveiced */
            if (cbDev->cbBr->chResult == L2CAP_SUCCESS) {
                cbDev->cbBr->chState = AVCT_CH_OPENED;
                AvctEvtData evtData;
                evtData.result = AVCT_SUCCESS;
                AvctCbBrEvt(cbDev, AVCT_REV_CONN_EVT, &evtData);
            }
        }
    } else {
        result = L2CAP_REJECTED;
        if (L2CIF_ConfigRsp(lcid, id, cfg, result, NULL)) {
            LOG_ERROR("[AVCT] %{public}s:L2CIF_ConfigReq failed.", __func__);
        }
    }
    return;
}

/*
 * Function     AvctBrRecvConfigRspCBack
 * Description  This function is called back when reveived config response
 *              from peer.
 * Param[in]    *addr   The address of the peer device.
 * Param[in]    lcid    The link channel id.
 * Param[in]    cfg     config info
 * Param[in]    result  the result of config request.
 * Return       void
 */
void AvctBrRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x),result(%{public}d)", __func__, lcid, result);
    AvctBrRecvConfigRspCallbackTskParam *param = malloc(sizeof(AvctBrRecvConfigRspCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctBrRecvConfigRspCallbackTskParam), 0, sizeof(AvctBrRecvConfigRspCallbackTskParam));
    param->lcid = lcid;
    (void)memcpy_s(&param->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    param->result = result;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRecvConfigRspCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrRecvConfigRspCallbackTsk(void *context)
{
    AvctBrRecvConfigRspCallbackTskParam *param = (AvctBrRecvConfigRspCallbackTskParam *)context;
    AvctBrRecvConfigRspCBack(param->lcid, &param->cfg, param->result, param->ctx);
    free(param);
    return;
}

void AvctBrRecvConfigRspCBack(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s: lcid(0x%x),result(%{public}d)", __func__, lcid, result);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev == NULL) {
        LOG_ERROR("[AVCT] %{public}s:AvctGetCbDevByChId(0x%x) failed.", __func__, lcid);
        return;
    }
    if (cbDev->cbBr->chState == AVCT_CH_CFG) {
        if (result == L2CAP_PENDING) {
            LOG_INFO("[AVCT] %{public}s: Config RSP result is pending, do nothing!", __func__);
            return;
        }
        if (result == L2CAP_SUCCESS) {
            cbDev->cbBr->chCfgSt |= AVCT_L2C_CFG_RSP;
            if (cbDev->cbBr->chCfgSt & AVCT_L2C_CFG_REQ) {
                /* config req/rsp reveiced */
                cbDev->cbBr->chState = AVCT_CH_OPENED;
                AvctEvtData evtData;
                evtData.result = AVCT_SUCCESS;
                AvctCbBrEvt(cbDev, AVCT_REV_CONN_EVT, &evtData);
            }
        } else {
            /* result is Failed ,send disconnect req */
            cbDev->cbBr->chResult = result;
            L2CIF_DisconnectionReq(lcid, NULL);
        }
    }
    return;
}

/*
 * Function     AvctBrRecvDisconnectionReqCBack
 * Description  This function is called back when reveived disconnection request
 *              from peer.
 * Param[in]    lcid    The link channel id.
 * Param[in]    id
 * Return       void
 */
void AvctBrRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x)", __func__, lcid);
    AvctBrRecvDisconnectionReqCallbackTskParam *param = malloc(sizeof(AvctBrRecvDisconnectionReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param,
        sizeof(AvctBrRecvDisconnectionReqCallbackTskParam),
        0,
        sizeof(AvctBrRecvDisconnectionReqCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRecvDisconnectionReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrRecvDisconnectionReqCallbackTsk(void *context)
{
    AvctBrRecvDisconnectionReqCallbackTskParam *param = (AvctBrRecvDisconnectionReqCallbackTskParam *)context;
    AvctBrRecvDisconnectionReqCBack(param->lcid, param->id, param->ctx);
    free(param);
    return;
}

void AvctBrRecvDisconnectionReqCBack(uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s: lcid(0x%x)", __func__, lcid);
    /* Send disconnect rsp */
    L2CIF_DisconnectionRsp(lcid, id, NULL);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        /* Send Close Event */
        AvctEvtData evtData;
        evtData.result = AVCT_SUCCESS;
        AvctCbBrEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
    }
    return;
}

/*
 * Function     AvctBrRecvDisconnectionRspCBack
 * Description  This function is called back when reveived disconnection
 *              response from peer.
 * Param[in]    lcid    The link channel id.
 * Return       void
 */
void AvctBrRecvDisconnectionRspCallback(uint16_t lcid, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x)", __func__, lcid);
    AvctBrRecvDisconnectionRspCallbackTskParam *param = malloc(sizeof(AvctBrRecvDisconnectionRspCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param,
        sizeof(AvctBrRecvDisconnectionRspCallbackTskParam),
        0,
        sizeof(AvctBrRecvDisconnectionRspCallbackTskParam));
    param->lcid = lcid;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRecvDisconnectionRspCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrRecvDisconnectionRspCallbackTsk(void *context)
{
    AvctBrRecvDisconnectionRspCallbackTskParam *param = (AvctBrRecvDisconnectionRspCallbackTskParam *)context;
    AvctBrRecvDisconnectionRspCBack(param->lcid, param->ctx);
    free(param);
    return;
}

void AvctBrRecvDisconnectionRspCBack(uint16_t lcid, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s:lcid(0x%x)", __func__, lcid);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        /* Send Close Event */
        AvctEvtData evtData;
        evtData.result = cbDev->cbBr->chResult;
        cbDev->cbBr->chResult = 0;
        AvctCbBrEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
    }
    return;
}

/*
 * Function     AvctBrDisconnectAbnormalCBack
 * Description  This function is called back when Disconnected abnormal, such as
 *              acl disconnected or link loss response from peer.
 * Param[in]    lcid    The link channel id.
 * Param[in]    reason  abnormal reason
 * Return       void
 */
void AvctBrDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x)", __func__, lcid);
    AvctBrDisconnectAbnormalCallbackTskParam *param = malloc(sizeof(AvctBrDisconnectAbnormalCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctBrDisconnectAbnormalCallbackTskParam), 0, sizeof(AvctBrDisconnectAbnormalCallbackTskParam));
    param->lcid = lcid;
    param->reason = reason;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrDisconnectAbnormalCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrDisconnectAbnormalCallbackTsk(void *context)
{
    AvctBrDisconnectAbnormalCallbackTskParam *param = (AvctBrDisconnectAbnormalCallbackTskParam *)context;
    AvctBrDisconnectAbnormalCBack(param->lcid, param->reason, param->ctx);
    free(param);
    return;
}

void AvctBrDisconnectAbnormalCBack(uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s: lcid(0x%x)", __func__, lcid);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        if (reason == L2CAP_STATE_COLLISION) {
            LOG_WARN("[AVCT] %{public}s:Connect Req failed! reason:L2CAP_STATE_COLLISION ", __func__);
            /* Connect Req failed, Need connect retry */
            cbDev->cbBr->chCfgSt = 0;
            cbDev->cbBr->chState = AVCT_CH_IDLE;
            cbDev->cbBr->stState = AVCT_IDLE_ST;
            AvctCbBrEvt(cbDev, AVCT_BIND_EVT, NULL);
        } else {
            AvctEvtData evtData;
            evtData.result = AVCT_FAILED;
            AvctCbBrEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
        }
    }
    return;
}

/*
 * Function     AvctBrRecvDataCBack
 * Description  This function is called back when receive message from peer.
 * Param[in]    lcid    The link channel id.
 * Param[in]    *packet message data
 * Return       void
 */
void AvctBrRecvDataCallback(uint16_t lcid, const Packet *pkt, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x),PacketSize(%u)", __func__, lcid, PacketSize(pkt));
    AvctBrRecvDataCallbackTskParam *param = malloc(sizeof(AvctBrRecvDataCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctBrRecvDataCallbackTskParam), 0, sizeof(AvctBrRecvDataCallbackTskParam));
    param->lcid = lcid;
    param->pkt = PacketRefMalloc(pkt);
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRecvDataCallbackTsk, param)) {
        PacketFree(param->pkt);
        free(param);
    }
    return;
}

void AvctBrRecvDataCallbackTsk(void *context)
{
    AvctBrRecvDataCallbackTskParam *param = (AvctBrRecvDataCallbackTskParam *)context;
    AvctBrRecvDataCBack(param->lcid, param->pkt, param->ctx);
    PacketFree(param->pkt);
    free(param);
    return;
}

void AvctBrRecvDataCBack(uint16_t lcid, Packet *pkt, void *ctx)
{
    LOG_DEBUG("[AVCT] %{public}s:lcid(0x%x),PacketSize(%u)", __func__, lcid, PacketSize(pkt));
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        AvctEvtData evtData;
        evtData.buf = pkt;
        AvctCbBrEvt(cbDev, AVCT_REV_MSG_EVT, &evtData);
    } else {
        /* Free pkt memory */
        LOG_WARN("[AVCT] %{public}s:Can't find device by lcid(0x%x)", __func__, lcid);
    }
    return;
}

/*
 * Function     AvctBrRemoteBusyCBack
 * Description  This function is called back when remote peer is busy in
 *              Enhanced mode.
 * Param[in]    lcid    The link channel id.
 * Param[in]    isBusy  busy/unbusy
 * Return       void
 */
void AvctBrRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx)
{
    AvctBrRemoteBusyCallbackTskParam *param = malloc(sizeof(AvctBrRemoteBusyCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctBrRemoteBusyCallbackTskParam), 0, sizeof(AvctBrRemoteBusyCallbackTskParam));
    param->lcid = lcid;
    param->isBusy = isBusy;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctBrRemoteBusyCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrRemoteBusyCallbackTsk(void *context)
{
    AvctBrRemoteBusyCallbackTskParam *param = (AvctBrRemoteBusyCallbackTskParam *)context;
    AvctBrRemoteBusyCBack(param->lcid, param->isBusy, param->ctx);
    free(param);
    return;
}

void AvctBrRemoteBusyCBack(uint16_t lcid, uint8_t isBusy, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x),isBusy(%hhu)", __func__, lcid, isBusy);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        AvctEvtData evtData;
        if (isBusy) {
            cbDev->cbBr->chState = AVCT_CH_BUSY;
            evtData.result = AVCT_BUSY_ST;
            AvctCbBrEvt(cbDev, AVCT_REV_BUSY_EVT, &evtData);
        } else {
            if (cbDev->cbBr->chState == AVCT_CH_BUSY) {
                cbDev->cbBr->chState = AVCT_CH_OPENED;
                evtData.result = AVCT_OPENED_ST;
                AvctCbBrEvt(cbDev, AVCT_REV_BUSY_EVT, &evtData);
            }
        }
    }
    return;
}

/*
 * Function     AvctBrwL2CIFConnectReqCallback
 * Description  L2cap connect request check callback func.
 * Param[in]    result Check result.
 * Param[in]    lcid   lcid.
 * Param[out]   addr   peer address
 * Return       void
 */
void AvctBrwL2CIFConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x), result(%{public}d)", __func__, lcid, result);
    AvctBrwL2CIFConnectReqCallbackTskParam *param = malloc(sizeof(AvctBrwL2CIFConnectReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctBrwL2CIFConnectReqCallbackTskParam), 0, sizeof(AvctBrwL2CIFConnectReqCallbackTskParam));
    (void)memcpy_s(&param->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param->lcid = lcid;
    param->result = result;
    param->context = context;
    if (AvctAsyncProcess(AvctBrwL2CIFConnectReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctBrwL2CIFConnectReqCallbackTsk(void *context)
{
    AvctBrwL2CIFConnectReqCallbackTskParam *param = (AvctBrwL2CIFConnectReqCallbackTskParam *)context;
    AvctBrwL2CIFConnectResult(&param->addr, param->lcid, param->result, param->context);
    free(param);
    return;
}

void AvctBrwL2CIFConnectResult(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_DEBUG("[AVCT] %{public}s: lcid(0x%x), result(%{public}d)", __func__, lcid, result);
    AvctCbDev *cbDev = AvctGetCbDevByAddress(addr);
    if ((cbDev != NULL) && (cbDev->cbBr != NULL)) {
        cbDev->cbBr->chId = lcid;
        if (lcid == 0) {
            /* connect req failed, send close event */
            AvctEvtData evtData;
            evtData.result = AVCT_FAILED;
            AvctCbBrEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
        }
    } else {
        LOG_WARN("[AVCT]%{public}s:Can't find control block by bdAddr((%02x:%02x:%02x:%02x:%02x:%02x)",
            __func__,
            BT_ADDR_FMT_DSC(addr->addr));
    }
    return;
}