/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "avdtp_l2cap.h"
#include "avdtp_ctrl.h"
#include "avdtp_message.h"
#include "btm.h"
#include "btm/btm_interop.h"
#include "log.h"
#include "securec.h"

/**
 *  Function implement
 */
const L2capService G_AVDT_L2C_APPL = {
    AVDT_L2capConnectIndCallback,
    AVDT_L2capConnectCfmCallback,
    AVDT_L2capConfigIndCallback,
    AVDT_L2capConfigCfmCallback,
    AVDT_L2capDisconnectIndCallback,
    AVDT_L2capDisconnectCfmCallback,
    AVDT_L2capDisconnectAbnormalCallback,
    AVDT_L2capReadDataIndCallback,
    AVDT_L2capRemoteBusyCallback,
};

/**
 *
 * @brief        AVDT_L2capConnectIndCallback
 *
 * @details      Acceptor receive the connect inication.
 *
 * @param[in]    addr: Address of peer device
 *               lcid: L2CAP channel id
 *               id: not used
 *               psm: not used
 *
 * @return       void
 *
 */
void AVDT_L2capConnectIndCallback(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t psm, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s: aclHandle(%hu), lcid(0x%x),addr(%02x:%02x:%02x:%02x:%02x:%02x)",
        __func__,
        info->handle,
        lcid,
        BT_ADDR_FMT_DSC(info->addr.addr));
    AvdtL2capConnectIndCallbackTskParam *param = malloc(sizeof(AvdtL2capConnectIndCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtL2capConnectIndCallbackTskParam), 0, sizeof(AvdtL2capConnectIndCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    (void)memcpy_s(&param->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    param->psm = psm;
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capConnectIndCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvdtL2capConnectIndCallbackTsk(void *context)
{
    AvdtL2capConnectIndCallbackTskParam *param = (AvdtL2capConnectIndCallbackTskParam *)context;
    AvdtL2capConnectIndCallback(param->lcid, param->id, &param->info, param->psm, param->ctx);
    free(context);
    return;
}

void AvdtL2capConnectIndCallback(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t psm, void *ctx)
{
    LOG_DEBUG("[AVDT]%{public}s: aclHandle(%hu), lcid(0x%x),addr(%02x:%02x:%02x:%02x:%02x:%02x)",
        __func__,
        info->handle,
        lcid,
        BT_ADDR_FMT_DSC(info->addr.addr));
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr((BtAddr *)&(info->addr));
    if (sigCtrl == NULL) {
        AvdtConnectSignalingIndication(lcid, id, info);
    } else {
        AvdtConnectStreamIndication(lcid, id, info, sigCtrl);
    }
    return;
}

/**
 *
 * @brief        AvdtConnectSignalingIndication
 *
 * @details      Process the signaling connect req.
 *
 * @param[in]    lcid: L2CAP channel id
 *               id: not used
 *               info: include address
 *
 * @return       void
 *
 */
void AvdtConnectSignalingIndication(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info)
{
    LOG_INFO("[AVDT]%{public}s", __func__);
    GapSecChannel chlId = {0};
    /* signaling channel process */
    AvdtCreateSEP((BtAddr *)&(info->addr));
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr((BtAddr *)&(info->addr));
    if (sigCtrl == NULL) {
        /* Trace: No resources */
        LOG_ERROR("[AVDT]%{public}s: There is no resource for alloce channel control block ", __func__);
    } else {
        sigCtrl->ia = AVDT_ACP;
        sigCtrl->lcid = lcid;
        /* security process:GAP authority */
        chlId.l2capPsm = AVDT_PSM;
        /* create transport table */
        AvdtTransChannel *transTable = AvdtTransChTabAllocate(AVDT_CH_TYPE_SIG, sigCtrl, NULL);
        if (transTable == NULL) {
            /* Trace error: No transcation table resources */
            LOG_ERROR("[AVDT]%{public}s: no resources for transcation channel ctrol block ", __func__);
            return;
        }
        /* Interop check */
        if (BtmInteropIsMatchedAddr(INTEROP_2MBPS_LINK_ONLY, (const BtAddr *)&(info->addr))) {
            BTM_ChangeConnectionPacketType((const BtAddr *)&(info->addr),
                BTM_ACL_PACKET_TYPE_DEFAULT | BTM_ACL_PACKET_TYPE_NO_3_DH1 | BTM_ACL_PACKET_TYPE_NO_3_DH3 |
                    BTM_ACL_PACKET_TYPE_NO_3_DH5);
        }
        /* save the information of table */
        transTable->lcid = lcid;
        transTable->id = id;
        transTable->state = AVDT_TRANS_ST_SECURITY_ACP;
        /* set security */
        GapRequestSecurityParam param = {0};
        param.callback = AvdtRevSecurityCheckCallback;
        param.info.channelId = chlId;
        param.info.protocolId = SEC_PROTOCOL_L2CAP;
        param.info.direction = INCOMING;
        param.info.serviceId = GAVDP_ACP;
        param.context = transTable;
        if (GAPIF_RequestSecurityAsync(&sigCtrl->peerAddress, &param)) {
            LOG_ERROR("[AVCT] %{public}s:GAP_RequestSecurity failed!", __func__);
        }
    }
    return;
}

/**
 *
 * @brief        AvdtConnectStreamIndication
 *
 * @details      Process the meida connect req.
 *
 * @param[in]    lcid: L2CAP channel id
 *               id: not used
 *               info: include address
 *               sigCtrl: control block
 *
 * @return       void
 *
 */
void AvdtConnectStreamIndication(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, AvdtSigCtrl *sigCtrl)
{
    LOG_INFO("[AVDT]%{public}s", __func__);
    AvdtTransChannel *transTable = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_SIG, sigCtrl->handle);
    if (transTable != NULL && transTable->state == AVDT_TRANS_ST_CONNECTION) {
        L2CIF_ConnectRsp(lcid, id, L2CAP_SOURCE_CID_ALREADY_ALLOCATED, 0, NULL);
        return;
    }
    if (AvdtGetTransChTabByLcid(lcid)) {
        L2CIF_ConnectRsp(lcid, id, L2CAP_NO_RESOURCES_AVAILABLE, 0, NULL);
        return;
    }
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(sigCtrl->streamHandle);
    if (streamCtrl == NULL) {
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, sigCtrl->streamHandle);
        return;
    }
    transTable = AvdtTransChTabAllocate(AVDT_CH_TYPE_STREAM, sigCtrl, streamCtrl);
    if (transTable != NULL) {
        L2CIF_ConnectRsp(lcid, id, L2CAP_CONNECTION_SUCCESSFUL, 0, NULL);
        transTable->lcid = lcid;
        sigCtrl->ia = AVDT_ACP;
        L2capConfigInfo cfg = {0};
        cfg.mtu = AvdtGetMtu();
        cfg.flushTimeout = 0xFFFF;
        L2CIF_ConfigReq(transTable->lcid, &cfg, NULL);
    } else {
        LOG_ERROR("[AVDT]%{public}s: There is no resource for transTable ", __func__);
        L2CIF_ConnectRsp(lcid, id, L2CAP_NO_RESOURCES_AVAILABLE, 0, NULL);
    }
    return;
}

/**
 *
 * @brief         AVDT_L2capConnectCfmCallback
 *
 * @details       Initator receive the L2CAP connect respond.
 *
 * @param[in]     lcid:L2CAP channel id
 *                result:Connect restult
 *                status:Connect status
 *
 * @return        void
 *
 */
void AVDT_L2capConnectCfmCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s: aclHandle(%hu), lcid(0x%x),addr(%02x:%02x:%02x:%02x:%02x:%02x)",
        __func__,
        info->handle,
        lcid,
        BT_ADDR_FMT_DSC(info->addr.addr));
    AvdtL2capConnectCfmCallbackTskParam *param = malloc(sizeof(AvdtL2capConnectCfmCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtL2capConnectCfmCallbackTskParam), 0, sizeof(AvdtL2capConnectCfmCallbackTskParam));
    param->lcid = lcid;
    (void)memcpy_s(&param->info, sizeof(L2capConnectionInfo), info, sizeof(L2capConnectionInfo));
    param->result = result;
    param->status = status;
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capConnectCfmCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvdtL2capConnectCfmCallbackTsk(void *context)
{
    AvdtL2capConnectCfmCallbackTskParam *param = (AvdtL2capConnectCfmCallbackTskParam *)context;
    AvdtL2capConnectCfmCallback(param->lcid, &param->info, param->result, param->status, param->ctx);
    free(context);
    return;
}

void AvdtL2capConnectCfmCallback(
    uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx)
{
    LOG_DEBUG("[AVDT]%{public}s:lcid(0x%x), result(%hu),status(%hu),addr(%02x:%02x:%02x:%02x:%02x:%02x)",
        __func__,
        lcid,
        result,
        status,
        BT_ADDR_FMT_DSC(info->addr.addr));
    if (result == L2CAP_CONNECTION_PENDING) {
        LOG_INFO("[AVDT]%{public}s: Connect RSP result is pending, do nothing!", __func__);
        return;
    }
    /* Judge the result and status according to L2CAP API */
    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable != NULL) {
        if (result == AVDT_SUCCESS) {
            transTable->state = AVDT_TRANS_ST_CFG;
            L2capConfigInfo cfg = {0};
            cfg.mtu = AvdtGetMtu();
            cfg.flushTimeout = 0xFFFF;
            L2CIF_ConfigReq(lcid, &cfg, NULL);
            LOG_DEBUG("[AVDT]%{public}s:L2CIF_ConfigReq: signalling", __func__);
        } else {
            AvdtConnectConfirmError(transTable);
        }
    }
    return;
}

/**
 *
 * @brief         AvdtConnectConfirmError
 *
 * @details       Process the failure of L2CAP connect respond.
 *
 * @param[in]     transTable:Transport table
 *
 * @return        void
 *
 */
void AvdtConnectConfirmError(const AvdtTransChannel *transTable)
{
    LOG_INFO("[AVDT]%{public}s ", __func__);
    AvdtCtrlData confirmData = {0};
    uint8_t event;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(transTable->sigHandle);
    if (sigCtrl == NULL) {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetSigCtrlByHandle(%hu) Failed", __func__, transTable->sigHandle);
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:Get sigCtrl info:transTable->sigHandle(%hu),transTable->type(%hhu),sigCtrl->role(%hhu)",
        __func__,
        transTable->sigHandle,
        transTable->type,
        sigCtrl->role);
    confirmData.connectCfm.errCode = AVDT_FAILED;
    switch (transTable->type) {
        case AVDT_CH_TYPE_SIG:
            event = AVDT_CONNECT_CFM_EVT;
            AvdtCtrlEvtCallback(sigCtrl, sigCtrl->handle, &(sigCtrl->peerAddress), event, &confirmData, sigCtrl->role);
            AvdtTransChDealloc(transTable->lcid);
            AvdtFreeSigCtrlByHandle(sigCtrl->handle);
            break;
        case AVDT_CH_TYPE_STREAM: {
            event = AVDT_OPEN_CFM_EVT;
            AvdtCtrlEvtCallback(
                sigCtrl, transTable->streamHandle, &(sigCtrl->peerAddress), event, &confirmData, sigCtrl->role);
            /* clear the local stream is used information */
            AvdtStreamCtrlDeallocByHandle(sigCtrl, transTable->streamHandle);
            AvdtTransChDealloc(transTable->lcid);
            break;
        }
        default:
            break;
    }
}

/**
 *
 * @brief         AVDT_L2capConfigIndCallback
 *
 * @details       Receive the l2cap configure req.
 *
 * @param[in]     lcid: L2CAP channel id
 *                cfg:  L2CAP configure information
 *                id:   L2CAP id
 *
 * @return        void
 *
 */
void AVDT_L2capConfigIndCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s: lcid(0x%x),id(0x%x)", __func__, lcid, id);
    AvdtL2capConfigIndCallbackTskParam *param = malloc(sizeof(AvdtL2capConfigIndCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtL2capConfigIndCallbackTskParam), 0, sizeof(AvdtL2capConfigIndCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    (void)memcpy_s(&param->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capConfigIndCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvdtL2capConfigIndCallbackTsk(void *context)
{
    AvdtL2capConfigIndCallbackTskParam *param = (AvdtL2capConfigIndCallbackTskParam *)context;
    AvdtL2capConfigIndCallback(param->lcid, param->id, &param->cfg, param->ctx);
    free(context);
    return;
}

void AvdtL2capConfigIndCallback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx)
{
    LOG_DEBUG("[AVDT]%{public}s: lcid(0x%x),id(0x%x)", __func__, lcid, id);
    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable != NULL) {
        AvdtConfigureIndication(lcid, id, cfg, transTable);
    } else {
        /* Trace: No resources */
        LOG_INFO("[AVDT]%{public}s: Config REJECTED! By can't find the transTable by lcid(0x%x)", __func__, lcid);
        L2CIF_ConfigRsp(lcid, id, cfg, L2CAP_REJECTED, NULL);
    }
    return;
}

/**
 *
 * @brief         AvdtConfigureIndication
 *
 * @details       Receive the l2cap configure req.
 *
 * @param[in]     lcid: L2CAP channel id
 *                cfg:  L2CAP configure information
 *                id:   L2CAP id
 *                transTable:Transport table
 *
 * @return        void
 *
 */
void AvdtConfigureIndication(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, AvdtTransChannel *transTable)
{
    LOG_INFO("[AVDT]%{public}s ", __func__);
    if (cfg->rfc.mode == L2CAP_BASIC_MODE) {
        L2capConfigInfo l2capCfg = {0};
        (void)memcpy_s(&l2capCfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
        if (cfg->mtu > AvdtGetMtu()) {
            transTable->peerMtu = AvdtGetMtu();
            l2capCfg.mtu = AvdtGetMtu();
        } else {
            transTable->peerMtu = cfg->mtu;
        }
        transTable->peerFlushTo = cfg->flushTimeout;
        transTable->state = AVDT_TRANS_ST_CFG;
        L2CIF_ConfigRsp(lcid, id, &l2capCfg, L2CAP_SUCCESS, NULL);
        transTable->cfgFlags |= AVDT_CFG_IND;
        if (transTable->cfgFlags == AVDT_CFG_END) {
            AvdtConfigComplete(transTable);
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

/**
 *
 * @brief         AvdtConfigComplete
 *
 * @details       Config complete,nofify state to up app.
 *
 * @param[in]     transTable: transport table
 *
 * @return        void
 *
 */
void AvdtConfigComplete(AvdtTransChannel *transTable)
{
    uint8_t event = 0;
    uint8_t stEvent;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(transTable->sigHandle);
    if (sigCtrl == NULL) {
        /* Trace: No resources */
        LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hu) Failed!", __func__, transTable->sigHandle);
        return;
    }
    switch (transTable->type) {
        case AVDT_CH_TYPE_SIG:
            if (sigCtrl->ia == AVDT_ACP) {
                event = AVDT_CONNECT_IND_EVT;
                stEvent = AVDT_CONNECT_CMD_IND_EVENT;
            } else {
                event = AVDT_CONNECT_CFM_EVT;
                stEvent = AVDT_CONNECT_CMD_CFM_EVENT;
            }
            AvdtSigProcEvent(sigCtrl, stEvent, NULL);
            break;
        case AVDT_CH_TYPE_STREAM:
            if (sigCtrl->ia == AVDT_ACP) {
                event = AVDT_OPEN_IND_EVT;
            } else {
                event = AVDT_OPEN_CFM_EVT;
            }
            AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(transTable->streamHandle);
            if (streamCtrl != NULL) {
                AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_OPEN_CMD_CFM_EVENT, NULL);
            }
            break;
        default:
            break;
    }
    transTable->state = AVDT_TRANS_ST_COMPLETE;
    AvdtCtrlData confirmData = {0};
    confirmData.connectCfm.errCode = AVDT_SUCCESS;
    confirmData.connectCfm.mtu = transTable->peerMtu;
    AvdtCtrlEvtCallback(sigCtrl, transTable->streamHandle, &sigCtrl->peerAddress, event, &confirmData, sigCtrl->role);
    return;
}
/**
 *
 * @brief         AVDT_L2capConfigCfmCallback
 *
 * @details       Receive the l2cap configure respond.
 *
 * @param[in]     lcid: L2CAP channel id
 *                cfg:  L2CAP configure information
 *                id:   L2CAP id
 *
 * @return        void
 *
 */
void AVDT_L2capConfigCfmCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s: lcid(0x%x),result(%hu)", __func__, lcid, result);
    AvdtL2capConfigCfmCallbackTskParam *param = malloc(sizeof(AvdtL2capConfigCfmCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtL2capConfigCfmCallbackTskParam), 0, sizeof(AvdtL2capConfigCfmCallbackTskParam));
    param->lcid = lcid;
    (void)memcpy_s(&param->cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    param->result = result;
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capConfigCfmCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvdtL2capConfigCfmCallbackTsk(void *context)
{
    AvdtL2capConfigCfmCallbackTskParam *param = (AvdtL2capConfigCfmCallbackTskParam *)context;
    AvdtL2capConfigCfmCallback(param->lcid, &param->cfg, param->result, param->ctx);
    free(context);
    return;
}

void AvdtL2capConfigCfmCallback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx)
{
    LOG_DEBUG("[AVDT]%{public}s: lcid(0x%x),result(%hu)", __func__, lcid, result);
    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable != NULL) {
        if (result == AVDT_SUCCESS) {
            AvdtConfigureConfirm(transTable);
        } else {
            AvdtConnectConfirmError(transTable);
        }
    } else {
        LOG_ERROR("[AVDT]%{public}s: can't find the transtable by lcid(0x%x)", __func__, lcid);
    }
    return;
}

/**
 *
 * @brief         AvdtConfigureConfirm
 *
 * @details       Process the l2cap configure respond.
 *
 * @param[in]     transTable:   transport table
 *
 * @return        void
 *
 */
void AvdtConfigureConfirm(AvdtTransChannel *transTable)
{
    LOG_INFO("[AVDT]%{public}s: ", __func__);
    transTable->state = AVDT_TRANS_ST_CFG;
    transTable->cfgFlags |= AVDT_CFG_CFM;
    if (transTable->cfgFlags == AVDT_CFG_END) {
        AvdtConfigComplete(transTable);
    }
}

/**
 *
 * @brief         AVDT_L2capDisconnectIndCallback
 *
 * @details       Acceptor receive the l2cap disconnect indication.
 *
 * @param[in]     lcid: L2CAP channel id
 *                cfg:  L2CAP configure information
 *                id:   L2CAP id
 *
 * @return        void
 *
 */
void AVDT_L2capDisconnectIndCallback(uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s: lcid(0x%x),id(0x%x)", __func__, lcid, id);
    AvdtL2capDisconnectIndCallbackTskParam *param = malloc(sizeof(AvdtL2capDisconnectIndCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvdtL2capDisconnectIndCallbackTskParam), 0, sizeof(AvdtL2capDisconnectIndCallbackTskParam));
    param->lcid = lcid;
    param->id = id;
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capDisconnectIndCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvdtL2capDisconnectIndCallbackTsk(void *context)
{
    AvdtL2capDisconnectIndCallbackTskParam *param = (AvdtL2capDisconnectIndCallbackTskParam *)context;
    AvdtL2capDisconnectIndCallback(param->lcid, param->id, param->ctx);
    free(context);
    return;
}

void AvdtL2capDisconnectIndCallback(uint16_t lcid, uint8_t id, void *ctx)
{
    LOG_DEBUG("[AVDT]%{public}s: lcid(0x%x),id(0x%x)", __func__, lcid, id);
    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable == NULL) {
        L2CIF_DisconnectionRsp(lcid, id, NULL);
        /* Trace no resources */
        return;
    }
    switch (transTable->type) {
        case AVDT_CH_TYPE_SIG: {
            AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(transTable->sigHandle);
            if (sigCtrl != NULL) {
                AvdtSigProcEvent(sigCtrl, AVDT_DISCONNECT_CMD_IND_EVENT, NULL);
            }
            break;
        }
        case AVDT_CH_TYPE_STREAM: {
            AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(transTable->streamHandle);
            if (streamCtrl != NULL) {
                AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_CLOSE_CMD_IND_EVENT, NULL);
            }
            break;
        }
        default:
            break;
    }
    /* clear the tabel information */
    L2CIF_DisconnectionRsp(lcid, id, NULL);
    AvdtTransChDealloc(lcid);
    return;
}

/**
 *
 * @brief         AVDT_L2capDisconnectCfmCallback
 *
 * @details       Initatior receive the l2cap disconnect respond.
 *
 * @param[in]     lcid: L2CAP channel id
 *                cfg:  L2CAP configure information
 *                id:   L2CAP id
 *
 * @return        void
 *
 */
void AVDT_L2capDisconnectCfmCallback(uint16_t lcid, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s: lcid(0x%x)", __func__, lcid);
    AvdtL2capDisconnectCfmCallbackTskParam *param = malloc(sizeof(AvdtL2capDisconnectCfmCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvdtL2capDisconnectCfmCallbackTskParam), 0, sizeof(AvdtL2capDisconnectCfmCallbackTskParam));
    param->lcid = lcid;
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capDisconnectCfmCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvdtL2capDisconnectCfmCallbackTsk(void *context)
{
    AvdtL2capDisconnectCfmCallbackTskParam *param = (AvdtL2capDisconnectCfmCallbackTskParam *)context;
    AvdtL2capDisconnectCfmCallback(param->lcid, param->ctx);
    free(context);
    return;
}

void AvdtL2capDisconnectCfmCallback(uint16_t lcid, void *ctx)
{
    LOG_DEBUG("[AVDT]%{public}s: lcid(0x%x)", __func__, lcid);
    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable == NULL) {
        /* Trace no resources */
        LOG_ERROR("[AVDT]%{public}s:AvdtGetTransChTabByLcid(0x%x) Failed!!", __func__, lcid);
        return;
    }
    switch (transTable->type) {
        case AVDT_CH_TYPE_SIG: {
            AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(transTable->sigHandle);
            if (sigCtrl != NULL) {
                AvdtSigProcEvent(sigCtrl, AVDT_DISCONNECT_CMD_CFM_EVENT, NULL);
            }
            break;
        }
        case AVDT_CH_TYPE_STREAM: {
            AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(transTable->streamHandle);
            if (streamCtrl != NULL) {
                AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_CLOSE_CMD_CFM_EVENT, NULL);
            }
            break;
        }
        default:
            break;
    }
    AvdtTransChDealloc(transTable->lcid);
    return;
}

/**
 *
 * @brief        AVDT_L2capReadDataIndCallback
 *
 * @details      Receive the l2cap indication to read data .
 *
 * @param[in]    lcid:      L2CAP channel id
 *               packet:    data of signalling or media
 *
 * @return       void
 *
 */
void AVDT_L2capReadDataIndCallback(uint16_t lcid, Packet *packet, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s:lcid(0x%x) ,PacketSize(%u)", __func__, lcid, PacketSize(packet));
    AvdtL2capReadDataIndCallbackTskParam *param = malloc(sizeof(AvdtL2capReadDataIndCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvdtL2capReadDataIndCallbackTskParam), 0, sizeof(AvdtL2capReadDataIndCallbackTskParam));
    param->lcid = lcid;
    param->packet = PacketRefMalloc(packet);
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capReadDataIndCallbackTsk, param)) {
        PacketFree(param->packet);
        free(param);
    }
    return;
}

void AvdtL2capReadDataIndCallbackTsk(void *context)
{
    AvdtL2capReadDataIndCallbackTskParam *param = (AvdtL2capReadDataIndCallbackTskParam *)context;
    AvdtL2capReadDataIndCallback(param->lcid, param->packet, param->ctx);
    PacketFree(param->packet);
    free(context);
    return;
}

void AvdtL2capReadDataIndCallback(uint16_t lcid, Packet *packet, void *ctx)
{
    if (packet == NULL) {
        LOG_ERROR("[AVDT]%{public}s:packet is null!!", __func__);
        return;
    }
    LOG_INFO("[AVDT]%{public}s:lcid(0x%x) ,PacketSize(%u)", __func__, lcid, PacketSize(packet));
    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable == NULL) {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetTransChTabByLcid(0x%x) Failed!!", __func__, lcid);
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:transTable->type(%hhu), transTable->streamHandle(%hu),transTable->sigHandle(%hu) ",
        __func__,
        transTable->type,
        transTable->streamHandle,
        transTable->sigHandle);
    switch (transTable->type) {
        case AVDT_CH_TYPE_SIG: {
            AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(transTable->sigHandle);
            if (sigCtrl != NULL) {
                /* debug log */
                AvdtPktDataPrint(packet);
                AvdtParseMsg(sigCtrl, packet);
            } else {
                /* Trace: No resources */
                LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hu) Failed!", __func__, transTable->sigHandle);
            }
            break;
        }
        case AVDT_CH_TYPE_STREAM: {
            AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(transTable->streamHandle);
            if (streamCtrl != NULL) {
                AvdtStreamDataProc(streamCtrl, packet);
            } else {
                /* Trace: No resources */
                LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, transTable->streamHandle);
            }
            break;
        }
        default:
            break;
    }
    return;
}

/**
 *
 * @brief        AvdtStreamDataProc
 *
 * @details      Stream Media packet data process.
 *
 * @param[in]    streamCtrl:    Stream Media channel control
 *               packet:        data of media
 *
 * @return       void
 *
 */
void AvdtStreamDataProc(AvdtStreamCtrl *streamCtrl, Packet *packet)
{
    uint8_t data = 0;
    uint8_t ssrc[4] = {0};
    PacketExtractHead(packet, &data, 1);
    PacketExtractHead(packet, &data, 1);
    uint8_t pktType = (data & 0xF7);
    PacketExtractHead(packet, &data, 1);
    PacketExtractHead(packet, &data, 1);
    PacketExtractHead(packet, &data, 1);
    uint32_t timeStamp = (data << AVDT_OFFSET_24BIT);
    PacketExtractHead(packet, &data, 1);
    timeStamp = (timeStamp | (data << AVDT_OFFSET_16BIT));
    PacketExtractHead(packet, &data, 1);
    timeStamp = (timeStamp | (data << AVDT_OFFSET_8BIT));
    PacketExtractHead(packet, &data, 1);
    timeStamp = (timeStamp | data);
    /* extract ssrc data */
    PacketExtractHead(packet, &ssrc[0], AVDT_4BYTE);
    AvdtStreamConfig *streamConfig = AvdtGetSepConfigByCodecIndex(streamCtrl->codecIndex);
    if ((streamConfig != NULL) && (streamConfig->sinkDataCback != NULL)) {
        streamConfig->sinkDataCback(streamCtrl->handle, packet, timeStamp, pktType, streamCtrl->codecIndex);
    }
}

/**
 *
 * @brief         AvdtRevSecurityCheckCallback
 *
 * @details       Acp receive security check result.
 *
 * @return        void
 *
 */
void AvdtRevSecurityCheckCallback(uint16_t result, GapServiceSecurityInfo security, void *context)
{
    LOG_INFO("[AVDT]%{public}s:result(%hu), l2capPsm(0x%x)", __func__, result, security.channelId.l2capPsm);
    AvdtRevSecurityCheckTskParam *param = malloc(sizeof(AvdtRevSecurityCheckTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtRevSecurityCheckTskParam), 0, sizeof(AvdtRevSecurityCheckTskParam));
    param->result = result;
    (void)memcpy_s(&param->security, sizeof(GapServiceSecurityInfo), &security, sizeof(GapServiceSecurityInfo));
    param->context = context;
    if (AvdtAsyncProcess(AvdtRevSecurityCheckTsk, param)) {
        free(param);
    }
    return;
}

void AvdtRevSecurityCheckTsk(void *context)
{
    AvdtRevSecurityCheckTskParam *param = (AvdtRevSecurityCheckTskParam *)context;
    AvdtRevSecurityCheck(param->result, param->security, param->context);
    free(context);
    return;
}

void AvdtRevSecurityCheck(uint16_t result, GapServiceSecurityInfo security, void *context)
{
    AvdtTransChannel *transTable = (AvdtTransChannel *)context;
    LOG_DEBUG(
        "[AVDT]%{public}s:result(%hu),lcid(0x%x), psm(0x%x)", __func__, result, transTable->lcid, security.channelId.l2capPsm);
    if (result == AVDT_SUCCESS) {
        /* Call L2CAP connect respond API */
        L2CIF_ConnectRsp(transTable->lcid, transTable->id, L2CAP_CONNECTION_SUCCESSFUL, 0, NULL);
        L2capConfigInfo cfg = {0};
        cfg.mtu = AvdtGetMtu();
        cfg.flushTimeout = 0xFFFF;
        L2CIF_ConfigReq(transTable->lcid, &cfg, NULL);
    } else {
        L2CIF_ConnectRsp(transTable->lcid, transTable->id, L2CAP_SECURITY_BLOCK, 0, NULL);
    }
    return;
}

/**
 *
 * @brief         AVDT_L2capDisconnectAbnormalCallback
 *
 * @details       receive disconnect abnormal from l2cap.
 *
 * @return        void
 *
 */
void AVDT_L2capDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s: lcid(0x%x) reason(%hhu)", __func__, lcid, reason);
    AvdtL2capDisconnectAbnormalCallbackTskParam *param = malloc(sizeof(AvdtL2capDisconnectAbnormalCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param,
        sizeof(AvdtL2capDisconnectAbnormalCallbackTskParam),
        0,
        sizeof(AvdtL2capDisconnectAbnormalCallbackTskParam));
    param->lcid = lcid;
    param->reason = reason;
    param->ctx = ctx;
    if (AvdtAsyncProcess(AvdtL2capDisconnectAbnormalCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvdtL2capDisconnectAbnormalCallbackTsk(void *context)
{
    AvdtL2capDisconnectAbnormalCallbackTskParam *param = (AvdtL2capDisconnectAbnormalCallbackTskParam *)context;
    AvdtL2capDisconnectAbnormalCallback(param->lcid, param->reason, param->ctx);
    free(context);
    return;
}
void AvdtL2capDisconnectAbnormalCallback(uint16_t lcid, uint8_t reason, void *ctx)
{
    LOG_DEBUG("[AVDT]%{public}s: lcid(0x%x) reason(%hhu)", __func__, lcid, reason);
    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable == NULL) {
        /* Trace no resources */
        LOG_ERROR("[AVDT]%{public}s: AvdtGetTransChTabByLcid(0x%x) Failed", __func__, lcid);
        return;
    }
    switch (transTable->type) {
        case AVDT_CH_TYPE_SIG: {
            AvdtL2capDisconnectAbnormalSignle(transTable, reason);
            break;
        }
        case AVDT_CH_TYPE_STREAM: {
            AvdtL2capDisconnectAbnormalStream(transTable, reason);
            break;
        }
        default:
            break;
    }
    AvdtTransChDealloc(lcid);
    return;
}

void AvdtL2capDisconnectAbnormalSignle(const AvdtTransChannel *transTable, uint8_t reason)
{
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(transTable->sigHandle);
    if (sigCtrl == NULL) {
        LOG_ERROR("[AVCT] %{public}s:AvdtGetSigCtrlByHandle(%hu) failed ", __func__, transTable->sigHandle);
        return;
    }

    if (reason == L2CAP_STATE_COLLISION) {
        LOG_WARN("[AVCT] %{public}s:Connect Req failed! reason:L2CAP_STATE_COLLISION ", __func__);
        /* Connect Req failed, Need connect retry */
        sigCtrl->state = AVDT_SIG_IDLE_ST;
        AvdtSigProcEvent(sigCtrl, AVDT_CONNECT_CMD_REQ_EVENT, NULL);
        return;
    }

    AvdtTransChannel *mediaTransTable = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_STREAM, transTable->streamHandle);
    if (mediaTransTable != NULL) {
        LOG_INFO("[AVDT]%{public}s:Delete Media lcid(0x%x)", __func__, mediaTransTable->lcid);
        AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(transTable->streamHandle);
        if (streamCtrl != NULL) {
            AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_CLOSE_CMD_REQ_EVENT, NULL);
        }
    }

    if (sigCtrl->state == AVDT_SIG_CLOSING_ST) {
        AvdtSigProcEvent(sigCtrl, AVDT_DISCONNECT_CMD_CFM_EVENT, NULL);
    } else {
        AvdtSigProcEvent(sigCtrl, AVDT_DISCONNECT_CMD_IND_EVENT, NULL);
    }
    return;
}

void AvdtL2capDisconnectAbnormalStream(const AvdtTransChannel *transTable, uint8_t reason)
{
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(transTable->streamHandle);
    if (streamCtrl == NULL) {
        LOG_ERROR("[AVCT] %{public}s:AvdtGetStreamCtrlByHandle(%hu) failed ", __func__, transTable->sigHandle);
        return;
    }

    if (reason == L2CAP_STATE_COLLISION) {
        LOG_WARN("[AVCT] %{public}s:Connect Req failed! reason:L2CAP_STATE_COLLISION ", __func__);
        /* Connect Req failed, Need connect retry */
        streamCtrl->state = AVDT_OPENING_ST;
        AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_OPEN_CMD_REQ_EVENT, NULL);
    } else {
        if (streamCtrl->state == AVDT_CLOSING_ST) {
            AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_CLOSE_CMD_CFM_EVENT, NULL);
        } else {
            AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_CLOSE_CMD_IND_EVENT, NULL);
        }
    }
    return;
}
void AVDT_L2capRemoteBusyCallback(uint16_t lcid, uint8_t isBusy, void *ctx)
{
    LOG_INFO("[AVDT]%{public}s:lcid(0x%x),isBusy(%hhu)", __func__, lcid, isBusy);
    return;
}
