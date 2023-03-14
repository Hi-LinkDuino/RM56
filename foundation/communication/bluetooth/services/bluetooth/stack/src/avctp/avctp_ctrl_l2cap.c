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
#include "avctp_ctrl_l2cap.h"
#include "avctp_ctrl.h"
#include "avctp_ctrl_act.h"
#include "avctp_dev.h"
#include "avctp_gap.h"
#include "avctp_st.h"
#include "log.h"
#include "securec.h"

/*************************************************************************************************
 *  Functions Declare
 *************************************************************************************************/

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/* L2CAP callback function structure */
const L2capService G_AVCT_L2C_SVR = {
    AvctRecvConnectionReqCallback,
    AvctRecvConnectionRspCallback,
    AvctRecvConfigReqCallback,
    AvctRecvConfigRspCallback,
    AvctRecvDisconnectionReqCallback,
    AvctRecvDisconnectionRspCallback,
    AvctDisconnectAbnormalCallback,
    AvctRecvDataCallback,
    AvctRemoteBusyCallback
    };

/*****************************************************************************
 * Function
 ****************************************************************************/
/*
 * Function     AvctRecvConnectionReqCallback
 * Description  This function is called back when reveive connect req from peer. Only one
 *              channel connection per PSM is used for this purpose between peer devices. Between two
 *              devices, multiple AVCTP connections may exist. There shall be only one AVCTP
 *              connection per PSM per ACL.
 * Param[in]    aclHandle acl handle
 * Param[in]    lcid   The link channel id.
 * Param[in]    id
 * Param[in]    lpsm
 * Return       void
 */
void AvctRecvConnectionReqCallback(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x),lpsm(0x%x)", __func__, lcid, lpsm);
    AvctRecvConnectionReqCallbackTskParam *param = malloc(sizeof(AvctRecvConnectionReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctRecvConnectionReqCallbackTskParam), 0, sizeof(AvctRecvConnectionReqCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    (void)memcpy_s(&param->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    param->lpsm = lpsm;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRecvConnectionReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctRecvConnectionReqCallbackTsk(void *context)
{
    AvctRecvConnectionReqCallbackTskParam *param = (AvctRecvConnectionReqCallbackTskParam *)context;
    AvctRecvConnectionReqCBack(param->lcid, param->id, &param->info, param->lpsm, param->ctx);
    free(param);
    return;
}

void AvctRecvConnectionReqCBack(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx)
{
    if (lpsm == AVCT_PSM) {
        AvctRequestSecurityBy(info->handle, info->addr, lcid, id, lpsm);
    } else {
        L2CIF_ConnectRsp(lcid, id, L2CAP_PSM_NOT_SUPPORTED, 0, NULL);
    }
    return;
}

/*
 * Function     AvctRecvConnectionReqAct
 * Description  Receive Connection Request action func
 * Param[in]    addr    address.
 * Param[in]    aclHandle   acl handle.
 * Param[in]    lcid    link channel id
 * Param[in]    id      id
 * Param[in]    psm     psm
 * Return       void
 */
void AvctRecvConnectionReqAct(const BtAddr *addr, uint16_t aclHandle, uint16_t lcid, uint8_t id, uint16_t psm)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x) psm(0x%x)", __func__, lcid, psm);
    AvctCbCh *cbCtrl = NULL;
    uint16_t result = L2CAP_CONNECTION_SUCCESSFUL;
    if (psm != AVCT_PSM) {
        LOG_ERROR("[AVCT]The Psm is not Avct");
        result = L2CAP_PSM_NOT_SUPPORTED;
        /* send L2CAP connection response  */
        L2CIF_ConnectRsp(lcid, id, result, 0, NULL);
        return;
    }
    /* check the devcie connection is exist */
    AvctCbDev *cbDev = AvctGetCbDevByAddress(addr);
    if (cbDev == NULL) {
        /* alloc ctrl resource */
        cbDev = AvctCbDevAlloc(addr);
        if (cbDev == NULL) {
            result = L2CAP_NO_RESOURCES_AVAILABLE;
        } else {
            cbCtrl = AvctCbChAlloc();
            if (cbCtrl == NULL) {
                result = L2CAP_NO_RESOURCES_AVAILABLE;
            }
        }
    } else {
        /* the connect is exist,return failed */
        result = L2CAP_SOURCE_CID_ALREADY_ALLOCATED;
    }
    /* send L2CAP connection response  */
    L2CIF_ConnectRsp(lcid, id, result, 0, NULL);
    /* if connect success,send config requeset */
    if (result == L2CAP_CONNECTION_SUCCESSFUL) {
        cbCtrl->chId = lcid;
        cbCtrl->chState = AVCT_CH_CFG;
        cbDev->cbCtrl = cbCtrl;
        cbDev->chLink |= AVCT_ALLOC_CTRL;
        L2capConfigInfo cfg = {0};
        cfg.mtu = g_avctMng.rxMtu;
        cfg.flushTimeout = 0xFFFF;
        if (L2CIF_ConfigReq(lcid, &cfg, NULL)) {
            LOG_ERROR("[AVCT] %{public}s:L2CIF_ConfigReq failed.", __func__);
        }
    }
    return;
}

/*
 * Function     AvctRecvConnectionRspCallback
 * Description  This function is called back when reveived connection response. from peer.
 * Param[in]    lcid The link channel id.
 * Param[in]    result  the connection request result
 * Param[in]    status
 * Return       void
 */
void AvctRecvConnectionRspCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x), result(%{public}d)", __func__, lcid, result);
    AvctRecvConnectionRspCallbackTskParam *param = malloc(sizeof(AvctRecvConnectionRspCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctRecvConnectionRspCallbackTskParam), 0, sizeof(AvctRecvConnectionRspCallbackTskParam));
    param->lcid = lcid;
    (void)memcpy_s(&param->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    param->result = result;
    param->status = status;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRecvConnectionRspCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctRecvConnectionRspCallbackTsk(void *context)
{
    AvctRecvConnectionRspCallbackTskParam *param = (AvctRecvConnectionRspCallbackTskParam *)context;
    AvctRecvConnectionRspCBack(param->lcid, &param->info, param->result, param->status, param->ctx);
    free(param);
    return;
}

void AvctRecvConnectionRspCBack(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev == NULL) {
        LOG_ERROR("[AVCT] %{public}s:AvctGetCbDevByChId(0x%x) failed.", __func__, lcid);
        return;
    }
    if (cbDev->cbCtrl->stState == AVCT_CH_CONN) {
        if (result == L2CAP_CONNECTION_PENDING) {
            LOG_INFO("[AVCT] %{public}s: Connect RSP result is pending, do nothing!", __func__);
            return;
        }
        if (result == L2CAP_CONNECTION_SUCCESSFUL) {
            cbDev->cbCtrl->chState = AVCT_CH_CFG;
            /* connect success ,send config request */
            L2capConfigInfo cfg = {0};
            cfg.mtu = g_avctMng.rxMtu;
            cfg.flushTimeout = 0xFFFF;
            if (L2CIF_ConfigReq(lcid, &cfg, NULL)) {
                LOG_ERROR("[AVCT] %{public}s:L2CIF_ConfigReq failed.", __func__);
            }
        } else {
            /* connect failed,send event to up app */
            AvctEvtData evtData;
            evtData.result = result;
            AvctCbCtrlEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
        }
    }
    return;
}

/*
 * Function     AvctRecvConfigReqCallback
 * Description  This function is called back when reveived config request from peer.
 * Param[in]    lcid The link channel id.
 * Param[in]    id
 * Param[in]    cfg the point of config
 * Return       void
 */
void AvctRecvConfigReqCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x), mtu(%x)", __func__, lcid, cfg->mtu);
    AvctRecvConfigReqCallbackTskParam *param = malloc(sizeof(AvctRecvConfigReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctRecvConfigReqCallbackTskParam), 0, sizeof(AvctRecvConfigReqCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    (void)memcpy_s(&param->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRecvConfigReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctRecvConfigReqCallbackTsk(void *context)
{
    AvctRecvConfigReqCallbackTskParam *param = (AvctRecvConfigReqCallbackTskParam *)context;
    AvctRecvConfigReqCBack(param->lcid, param->id, &param->cfg, param->ctx);
    free(param);
    return;
}

void AvctRecvConfigReqCBack(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    uint16_t result = L2CAP_SUCCESS;
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev == NULL) {
        LOG_ERROR("[AVCT] %{public}s: Config REJECTED! By can't fine device by lcid(0x%x)", __func__, lcid);
        L2CIF_ConfigRsp(lcid, id, cfg, L2CAP_REJECTED, NULL);
        return;
    }
    if (cfg->rfc.mode == L2CAP_BASIC_MODE) {
        /* send L2CAP configure response */
        if (L2CIF_ConfigRsp(lcid, id, cfg, result, NULL)) {
            LOG_ERROR("[AVCT] %{public}s:L2CIF_ConfigRsp failed.", __func__);
        }
        cbDev->cbCtrl->peerMtu = cfg->mtu;
        cbDev->cbCtrl->chCfgSt |= AVCT_L2C_CFG_REQ;
        /* There are two step of config process */
        if (cbDev->cbCtrl->chCfgSt & AVCT_L2C_CFG_RSP) {
            /* config req/rsp reveiced */
            if (cbDev->cbCtrl->chResult == L2CAP_SUCCESS) {
                cbDev->cbCtrl->chState = AVCT_CH_OPENED;
                AvctEvtData evtData;
                evtData.result = AVCT_SUCCESS;
                AvctCbCtrlEvt(cbDev, AVCT_REV_CONN_EVT, &evtData);
            }
        }
    } else {
        LOG_WARN("[AVCT] %{public}s: Config UNACCEPT! By mode is not basemode", __func__);
        L2capConfigInfo rspCfg = {0};
        rspCfg.mtu = cfg->mtu;
        rspCfg.flushTimeout = 0xFFFF;
        rspCfg.fcs = 0x01;
        rspCfg.rfc.mode = L2CAP_BASIC_MODE;
        L2CIF_ConfigRsp(lcid, id, &rspCfg, L2CAP_UNACCEPTABLE_PARAMETERS, NULL);
    }
    return;
}

/*
 * Function     AvctRecvConfigRspCallback
 * Description  This function is called back when reveived config response from peer.
 * Param[in]    *addr   The address of the peer device.
 * Param[in]    lcid    The link channel id.
 * Param[in]    cfg   The point of config info
 * Param[in]    result  the result of config request.
 * Return       void
 */
void AvctRecvConfigRspCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x),result(%hu)", __func__, lcid, result);
    AvctRecvConfigRspCallbackTskParam *param = malloc(sizeof(AvctRecvConfigRspCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctRecvConfigRspCallbackTskParam), 0, sizeof(AvctRecvConfigRspCallbackTskParam));
    param->lcid = lcid;
    (void)memcpy_s(&param->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    param->result = result;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRecvConfigRspCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctRecvConfigRspCallbackTsk(void *context)
{
    AvctRecvConfigRspCallbackTskParam *param = (AvctRecvConfigRspCallbackTskParam *)context;
    AvctRecvConfigRspCBack(param->lcid, &param->cfg, param->result, param->ctx);
    free(param);
    return;
}

void AvctRecvConfigRspCBack(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev == NULL) {
        LOG_ERROR("[AVCT] %{public}s:AvctGetCbDevByChId(0x%x) failed.", __func__, lcid);
        return;
    }
    if (cbDev->cbCtrl->chState == AVCT_CH_CFG) {
        if (result == L2CAP_PENDING) {
            LOG_INFO("[AVCT] %{public}s: Config rsp result is pending, do nothing", __func__);
            return;
        }
        if (result == L2CAP_SUCCESS) {
            cbDev->cbCtrl->chCfgSt |= AVCT_L2C_CFG_RSP;
            if (cbDev->cbCtrl->chCfgSt & AVCT_L2C_CFG_REQ) {
                /* config req/rsp reveiced */
                cbDev->cbCtrl->chState = AVCT_CH_OPENED;
                AvctEvtData evtData;
                evtData.result = AVCT_SUCCESS;
                AvctCbCtrlEvt(cbDev, AVCT_REV_CONN_EVT, &evtData);
            }
        } else {
            /* result is Failed ,send disconnect req */
            cbDev->cbCtrl->chResult = result;
            L2CIF_DisconnectionReq(lcid, NULL);
        }
    }
    return;
}

/*
 * Function     AvctRecvDisconnectionReqCallback
 * Description  This function is called back when reveived disconnection request from peer.
 * Param[in]    lcid    The link channel id.
 * Param[in]    id
 * Return       void
 */
void AvctRecvDisconnectionReqCallback(uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x)", __func__, lcid);
    AvctRecvDisconnectionReqCallbackTskParam *param = malloc(sizeof(AvctRecvDisconnectionReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctRecvDisconnectionReqCallbackTskParam), 0, sizeof(AvctRecvDisconnectionReqCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRecvDisconnectionReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctRecvDisconnectionReqCallbackTsk(void *context)
{
    AvctRecvDisconnectionReqCallbackTskParam *param = (AvctRecvDisconnectionReqCallbackTskParam *)context;
    AvctRecvDisconnectionReqCBack(param->lcid, param->id, param->ctx);
    free(param);
    return;
}

void AvctRecvDisconnectionReqCBack(uint16_t lcid, uint8_t id, void *ctx)
{
    /* Send disconnect rep */
    L2CIF_DisconnectionRsp(lcid, id, NULL);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        /* Send Close Event */
        AvctEvtData evtData;
        evtData.result = AVCT_SUCCESS;
        AvctCbCtrlEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
    }
    return;
}

/*
 * Function     AvctRecvDisconnectionRspCallback
 * Description  This function is called back when reveived disconnection response from peer.
 * Param[in]    lcid    The link channel id.
 * Return       void
 */
void AvctRecvDisconnectionRspCallback(uint16_t lcid, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x)", __func__, lcid);
    AvctRecvDisconnectionRspCallbackTskParam *param = malloc(sizeof(AvctRecvDisconnectionRspCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctRecvDisconnectionRspCallbackTskParam), 0, sizeof(AvctRecvDisconnectionRspCallbackTskParam));
    param->lcid = lcid;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRecvDisconnectionRspCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctRecvDisconnectionRspCallbackTsk(void *context)
{
    AvctRecvDisconnectionRspCallbackTskParam *param = (AvctRecvDisconnectionRspCallbackTskParam *)context;
    AvctRecvDisconnectionRspCBack(param->lcid, param->ctx);
    free(param);
    return;
}

void AvctRecvDisconnectionRspCBack(uint16_t lcid, void *ctx)
{
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        /* Send Close Event */
        AvctEvtData evtData;
        evtData.result = cbDev->cbCtrl->chResult;
        cbDev->cbCtrl->chResult = 0;
        AvctCbCtrlEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
    }
    return;
}

/*
 * Function     AvctDisconnectAbnormalCallback
 * Description  This function is called back when Disconnected abnormal, such as acl disconnected or link loss response
 *              from peer.
 * Param[in]    lcid    The link channel id.
 * Param[in]    reason  abnormal reason
 * Return       void
 */
void AvctDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x) reason(%hhu)", __func__, lcid, reason);
    AvctDisconnectAbnormalCallbackTskParam *param = malloc(sizeof(AvctDisconnectAbnormalCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctDisconnectAbnormalCallbackTskParam), 0, sizeof(AvctDisconnectAbnormalCallbackTskParam));
    param->lcid = lcid;
    param->reason = reason;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctDisconnectAbnormalCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctDisconnectAbnormalCallbackTsk(void *context)
{
    AvctDisconnectAbnormalCallbackTskParam *param = (AvctDisconnectAbnormalCallbackTskParam *)context;
    AvctDisconnectAbnormalCBack(param->lcid, param->reason, param->ctx);
    free(param);
    return;
}

void AvctDisconnectAbnormalCBack(uint16_t lcid, uint8_t reason, void *ctx)
{
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        if (reason == L2CAP_STATE_COLLISION) {
            LOG_WARN("[AVCT] %{public}s:Connect Req failed! reason:L2CAP_STATE_COLLISION ", __func__);
            /* Connect Req failed, Need connect retry */
            cbDev->cbCtrl->chCfgSt = 0;
            cbDev->cbCtrl->chState = AVCT_CH_IDLE;
            cbDev->cbCtrl->stState = AVCT_IDLE_ST;
            AvctCbCtrlEvt(cbDev, AVCT_BIND_EVT, NULL);
        } else {
            AvctEvtData evtData;
            evtData.result = AVCT_FAILED;
            AvctCbCtrlEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
        }
    }
    return;
}

/*
 * Function     AvctRecvDataCallback
 * Description  This function is called back when receive message from peer.
 * Param[in]    lcid    The link channel id.
 * Param[in]    *pkt  message data
 * Return       void
 */
void AvctRecvDataCallback(uint16_t lcid, Packet *pkt, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x),PacketSize(%u)", __func__, lcid, PacketSize(pkt));
    AvctRecvDataCallbackTskParam *param = malloc(sizeof(AvctRecvDataCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctRecvDataCallbackTskParam), 0, sizeof(AvctRecvDataCallbackTskParam));
    param->lcid = lcid;
    param->pkt = PacketRefMalloc(pkt);
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRecvDataCallbackTsk, param)) {
        PacketFree(param->pkt);
        free(param);
    }
    return;
}

void AvctRecvDataCallbackTsk(void *context)
{
    AvctRecvDataCallbackTskParam *param = (AvctRecvDataCallbackTskParam *)context;
    AvctRecvDataCBack(param->lcid, param->pkt, param->ctx);
    PacketFree(param->pkt);
    free(param);
    return;
}

void AvctRecvDataCBack(uint16_t lcid, Packet *pkt, void *ctx)
{
    AvctPktDataPrint(pkt);
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        AvctEvtData evtData;
        evtData.buf = pkt;
        AvctCbCtrlEvt(cbDev, AVCT_REV_MSG_EVT, &evtData);
    } else {
        LOG_WARN("[AVCT] %{public}s:Can't find device by lcid(0x%x)", __func__, lcid);
    }
    return;
}

/*
 * Function     AvctRemoteBusyCallback
 * Description  This function is called back when remote peer is busy in Enhanced mode.
 * Param[in]    lcid    The link channel id.
 * Param[in]    isBusy  busy/unbusy
 * Return       void
 */
void AvctRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx)
{
    LOG_INFO("[AVCT] %{public}s:lcid(0x%x),isBusy(%hhu)", __func__, lcid, isBusy);
    AvctRemoteBusyCallbackTskParam *param = malloc(sizeof(AvctRemoteBusyCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctRemoteBusyCallbackTskParam), 0, sizeof(AvctRemoteBusyCallbackTskParam));
    param->lcid = lcid;
    param->isBusy = isBusy;
    param->ctx = ctx;
    if (AvctAsyncProcess(AvctRemoteBusyCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctRemoteBusyCallbackTsk(void *context)
{
    AvctRemoteBusyCallbackTskParam *param = (AvctRemoteBusyCallbackTskParam *)context;
    AvctRemoteBusyCBack(param->lcid, param->isBusy, param->ctx);
    free(param);
    return;
}

void AvctRemoteBusyCBack(uint16_t lcid, uint8_t isBusy, void *ctx)
{
    AvctCbDev *cbDev = AvctGetCbDevByChId(lcid);
    if (cbDev != NULL) {
        AvctEvtData evtData;
        if (isBusy) {
            cbDev->cbCtrl->chState = AVCT_CH_BUSY;
            evtData.result = AVCT_BUSY_ST;
            AvctCbCtrlEvt(cbDev, AVCT_REV_BUSY_EVT, &evtData);
        } else {
            if (cbDev->cbCtrl->chState == AVCT_CH_BUSY) {
                cbDev->cbCtrl->chState = AVCT_CH_OPENED;
                evtData.result = AVCT_OPENED_ST;
                AvctCbCtrlEvt(cbDev, AVCT_REV_BUSY_EVT, &evtData);
            }
        }
    }
    return;
}

/*
 * Function     AvctCtrlL2CIFConnectReqCallback
 * Description  L2cap connect request check callback func.
 * Param[in]    result Check result.
 * Param[in]    lcid   lcid.
 * Param[out]   addr   peer address
 * Return       void
 */
void AvctCtrlL2CIFConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[AVCT] %{public}s: lcid(0x%x), result(%{public}d)", __func__, lcid, result);
    AvctCtrlL2CIFConnectReqCallbackTskParam *param = malloc(sizeof(AvctCtrlL2CIFConnectReqCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctCtrlL2CIFConnectReqCallbackTskParam), 0, sizeof(AvctCtrlL2CIFConnectReqCallbackTskParam));
    (void)memcpy_s(&param->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param->lcid = lcid;
    param->result = result;
    param->context = context;
    if (AvctAsyncProcess(AvctCtrlL2CIFConnectReqCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctCtrlL2CIFConnectReqCallbackTsk(void *context)
{
    AvctCtrlL2CIFConnectReqCallbackTskParam *param = (AvctCtrlL2CIFConnectReqCallbackTskParam *)context;
    AvctCtrlConnectResult(&param->addr, param->lcid, param->result, param->context);
    free(param);
    return;
}

void AvctCtrlConnectResult(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    AvctCbDev *cbDev = AvctGetCbDevByAddress(addr);
    if ((cbDev != NULL) && (cbDev->cbCtrl != NULL)) {
        cbDev->cbCtrl->chId = lcid;
        if (lcid == 0) {
            /* connect req failed, send close event */
            AvctEvtData evtData;
            evtData.result = AVCT_FAILED;
            AvctCbCtrlEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
        }
    } else {
        LOG_WARN("[AVCT]%{public}s:Can't find control block by bdAddr((%02x:%02x:%02x:%02x:%02x:%02x)",
            __func__,
            BT_ADDR_FMT_DSC(addr->addr));
    }
    return;
}