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
#include "avctp_gap.h"
#include "avctp_br_l2cap.h"
#include "avctp_ctrl.h"
#include "avctp_ctrl_l2cap.h"
#include "avctp_st.h"
#include "log.h"
#include "securec.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/*****************************************************************************
 * Function
 ****************************************************************************/
/*
 * Function     AvctSecurityResultCallback
 * Description  This function is the callback from gap to check service security .
 * Param[in]    result  The check result.
 * Param[in]    channelId  The service info.
 * Param[in]    context  The connection control block.
 * Return       void
 */
void AvctSecurityResultCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_INFO("[AVCT] %{public}s:result(%hu) ", __func__, result);
    AvctSecurityResultCallbackTskParam *param = malloc(sizeof(AvctSecurityResultCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvctSecurityResultCallbackTskParam), 0, sizeof(AvctSecurityResultCallbackTskParam));
    param->result = result;
    (void)memcpy_s(&param->serviceInfo, sizeof(GapServiceSecurityInfo), &serviceInfo, sizeof(GapServiceSecurityInfo));
    param->context = context;
    if (AvctAsyncProcess(AvctSecurityResultCallbackTsk, param)) {
        free(param);
    }
    return;
}

void AvctSecurityResultCallbackTsk(void *context)
{
    AvctSecurityResultCallbackTskParam *param = (AvctSecurityResultCallbackTskParam *)context;
    AvctSecurityResult(param->result, param->serviceInfo, param->context);
    free(param);
    return;
}

void AvctSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_DEBUG("[AVCT] %{public}s:result(%hu) ", __func__, result);
    AvctCbDev *cbDev = ((AvctCbDev *)context);
    AvctCbCh *cbCh = NULL;
    if (result != GAP_SUCCESS) {
        /* Security check fail,send close event */
        AvctEvtData evtData;
        evtData.result = AVCT_FAILED;
        AvctCbCtrlEvt(cbDev, AVCT_REV_DISCONN_EVT, &evtData);
        return;
    }
    LOG_DEBUG("[AVCT] %{public}s: channelId.l2capPsm is (%x),serviceId(%{public}d),chlink(%hhu) ",
        __func__,
        serviceInfo.channelId.l2capPsm,
        serviceInfo.serviceId,
        cbDev->chLink);
    if ((serviceInfo.serviceId == AVRCP_CT) || (serviceInfo.serviceId == AVRCP_TG)) {
        if (cbDev->chLink & AVCT_ALLOC_CTRL) {
            cbCh = cbDev->cbCtrl;
            cbCh->chState = AVCT_CH_CONN;
            if (L2CIF_ConnectReq(&(cbDev->peerAddr), AVCT_PSM, AVCT_PSM, NULL, AvctCtrlL2CIFConnectReqCallback)) {
                LOG_ERROR("[AVCT] %{public}s:L2CIF_ConnectReq failed.", __func__);
            }
        } else {
            LOG_ERROR("[AVCT] %{public}s: Ctrl channel not allocated, Can't connect!", __func__);
        }
    } else if ((serviceInfo.serviceId == AVRCP_CT_BROWSING) || (serviceInfo.serviceId == AVRCP_TG_BROWSING)) {
        /* Browser channel connect */
        if (cbDev->chLink & AVCT_ALLOC_BR) {
            cbCh = cbDev->cbBr;
            cbCh->chState = AVCT_CH_CONN;
            if (L2CIF_ConnectReq(&(cbDev->peerAddr), AVCT_BR_PSM, AVCT_BR_PSM, NULL, AvctBrwL2CIFConnectReqCallback)) {
                LOG_ERROR("[AVCT] %{public}s:L2CIF_ConnectReq failed.", __func__);
            }
        } else {
            LOG_ERROR("[AVCT] %{public}s: Br channel not allocated, Can't connect!", __func__);
        }
    } else {
        LOG_WARN("[AVCT] %{public}s: ServiceID(%{public}d) from GAP is error!", __func__, serviceInfo.serviceId);
    }
    return;
}

/*
 * Function     AvctRevConnSecurityResultCallback
 * Description  GAP security request check callback func.
 * Param[in]    result Check result.
 * Param[in]    channelId   Channel Id.
 * Param[out]   context
 * Return       void
 */
void AvctRevConnSecurityResultCallback(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_INFO("[AVCT] %{public}s:result(%hu)", __func__, result);
    AvctRevConnSecurityResultCallbackTskParam *param = malloc(sizeof(AvctRevConnSecurityResultCallbackTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(
        param, sizeof(AvctRevConnSecurityResultCallbackTskParam), 0, sizeof(AvctRevConnSecurityResultCallbackTskParam));
    param->result = result;
    (void)memcpy_s(&param->serviceInfo, sizeof(GapServiceSecurityInfo), &serviceInfo, sizeof(GapServiceSecurityInfo));
    param->context = context;
    if (AvctAsyncProcess(AvctRevConnSecurityResultCallbackTsk, param)) {
        free((AvctRevConnParam *)param->context);
        free(param);
    }
    return;
}

void AvctRevConnSecurityResultCallbackTsk(void *context)
{
    AvctRevConnSecurityResultCallbackTskParam *param = (AvctRevConnSecurityResultCallbackTskParam *)context;
    AvctRevConnSecurityResult(param->result, param->serviceInfo, param->context);
    free(param);
    return;
}

void AvctRevConnSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context)
{
    LOG_DEBUG("[AVCT] %{public}s:result(%hu)", __func__, result);
    AvctRevConnParam *param = (AvctRevConnParam *)context;
    if (result != GAP_SUCCESS) {
        /* Security check fail,send L2CAP_SECURITY_BLOCK rsp */
        result = L2CAP_SECURITY_BLOCK;
        L2CIF_ConnectRsp(param->lcid, param->id, result, 0, NULL);
    } else {
        if ((serviceInfo.serviceId == AVRCP_CT) || (serviceInfo.serviceId == AVRCP_TG)) {
            AvctRecvConnectionReqAct(&(param->addr), param->aclHandle, param->lcid, param->id, param->psm);
        } else if ((serviceInfo.serviceId == AVRCP_CT_BROWSING) || (serviceInfo.serviceId == AVRCP_TG_BROWSING)) {
            AvctBrRecvConnectionReqAct(&(param->addr), param->aclHandle, param->lcid, param->id, param->psm);
        } else {
            LOG_WARN("[AVCT] %{public}s: ServiceID(%{public}d) from GAP is error!", __func__, serviceInfo.serviceId);
        }
    }
    if (param != NULL) {
        free(param);
        param = NULL;
    }
    return;
}

/*
 * Function     AvctRequestSecurity
 * Description  This function is called to Open control channel link.
 * Param[in]    *cbDev      The point to the device block.
 * Param[in]    serviceId   seviced id defined by GAP
 * Param[in]    psm
 * Return       AVCT_SUCCESS. Others: Fail
 */
void AvctRequestSecurity(const AvctCbDev *cbDev, GAP_Service serviceId, uint16_t psm)
{
    LOG_DEBUG("[AVCT] %{public}s:Sevcieid(0x%x),psm(0x%x)", __func__, serviceId, psm);
    GapRequestSecurityParam gapParam = {0};
    /* Request servcie security */
    gapParam.info.protocolId = SEC_PROTOCOL_L2CAP;
    gapParam.callback = AvctSecurityResultCallback;
    gapParam.context = (AvctCbDev *)cbDev;
    gapParam.info.direction = OUTGOING;
    gapParam.info.serviceId = serviceId;
    gapParam.info.channelId.l2capPsm = psm;
    if (GAPIF_RequestSecurityAsync(&(cbDev->peerAddr), &gapParam)) {
        LOG_ERROR("[AVCT] %{public}s:GAP_RequestSecurity failed!", __func__);
    }
    return;
}
/*
 * Function     AvctRequestSecurityBy
 * Description  This function is the callback from gap to check service security .
 * Param[in]    aclHandle  ACL handle.
 * Param[in]    lcid   The link channel id.
 * Param[in]    id
 * Param[in]    lpsm
 * Return       void
 */
void AvctRequestSecurityBy(
    const uint16_t aclHandle, const BtAddr addr, const uint16_t lcid, const uint8_t id, const uint16_t lpsm)
{
    LOG_DEBUG("[AVCT] %{public}s:Addr(%x:%x:%x:%x:%x:%x)", __func__, BT_ADDR_FMT_ASC(addr.addr));
    AvctRevConnParam *connParam = malloc(sizeof(AvctRevConnParam));
    if (connParam == NULL) {
        LOG_ERROR("[AVCT] %{public}s:memory alloc failed!", __func__);
        return;
    }
    connParam->addr = addr;
    connParam->aclHandle = aclHandle;
    connParam->lcid = lcid;
    connParam->id = id;
    connParam->psm = lpsm;
    /* Request servcie security */
    GapRequestSecurityParam gapParam;
    /* Request servcie security */
    gapParam.info.protocolId = SEC_PROTOCOL_L2CAP;
    gapParam.callback = (GapSecurityResultCallback)AvctRevConnSecurityResultCallback;
    gapParam.context = connParam;
    gapParam.info.channelId.l2capPsm = lpsm;
    if (lpsm == AVCT_PSM) {
        if (g_avctMng.role == AVCT_CT) {
            gapParam.info.serviceId = AVRCP_CT;
        } else {
            gapParam.info.serviceId = AVRCP_TG;
        }
    } else {
        if (g_avctMng.role == AVCT_CT) {
            gapParam.info.serviceId = AVRCP_CT_BROWSING;
        } else {
            gapParam.info.serviceId = AVRCP_TG_BROWSING;
        }
    }
    gapParam.info.direction = INCOMING;
    if (GAPIF_RequestSecurityAsync(&addr, &gapParam)) {
        LOG_ERROR("[AVCT] %{public}s:GAP_RequestSecurity failed!", __func__);
        free(connParam);
    }
    return;
}