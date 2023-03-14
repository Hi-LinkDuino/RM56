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
#include "avctp_br_act.h"
#include "avctp_st.h"
#include "avctp_conn.h"
#include "avctp_gap.h"
#include "avctp_ctrl.h"
#include "avctp_ctrl_act.h"
#include "avctp_br.h"
#include "l2cap_if.h"
#include "log.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/*
 * Function     AvctCbBrChConn
 * Description  This function is called to Open browser channel link.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChConn(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    GAP_Service serviceId;
    /* Request servcie security */
    if (g_avctMng.role == AVCT_CT) {
        serviceId = AVRCP_CT_BROWSING;
    } else {
        serviceId = AVRCP_TG_BROWSING;
    }
    AvctRequestSecurity(cbDev, serviceId, AVCT_BR_PSM);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChConnFail
 * Description  This function is called when Open browser channel link failed.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChConnFail(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
            if (cbDev->chLink & AVCT_ALLOC_BR) {
                AvctCbConnEvtCallback(cbConn, AVCT_BR_CONNECT_CFM_EVT, data->result, &(cbConn->cbDev->peerAddr));
            }
        }
    }
    AvctCbChDealloc(&(cbDev->cbBr));
    cbDev->chLink &= ~AVCT_ALLOC_BR;
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChBind
 * Description  This function is called when the browser channel had been
 *              created, only bind it.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChBind(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* Send callback event to app */
    AvctCbConnEvtCallback(data->cbConn, AVCT_BR_CONNECT_CFM_EVT, AVCT_SUCCESS, &(data->cbConn->cbDev->peerAddr));
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChBindFail
 * Description  This function is called when the browser channel had been
 *              created, try to bind it failed.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChBindFail(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* send connect cfm event ,result failed */
    AvctCbConnEvtCallback(data->cbConn, AVCT_BR_CONNECT_CFM_EVT, AVCT_FAILED, &(data->cbConn->cbDev->peerAddr));
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChDisconn
 * Description  This function is called to disconnect the browser channel .
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChDisconn(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    L2CIF_DisconnectionReq(cbDev->cbBr->chId, NULL);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChUnbind
 * Description  This function is called to unbind and disconnect the browser
 *              channel.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChUnbind(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* send disconnect cfm event ,result failed */
    AvctCbConnEvtCallback(data->cbConn, AVCT_BR_DISCONNECT_CFM_EVT, AVCT_SUCCESS, &(data->cbConn->cbDev->peerAddr));
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChCheckDisconn
 * Description  This function is called to check if the connect is the last one which is using the br channel.
 *              If it is the last one, disconnect the channel.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChCheckDisconn(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* check if the connect is the last one which is using the br channel */
    if (cbDev->bindCnt > 1) {
        /* unbind */
        AvctCbBrChUnbind(cbDev, data);
    } else {
        /* if the browser channel is exit, disconnect too */
        if (cbDev->cbBr != NULL) {
            AvctCbBrEvt(cbDev, AVCT_DISCONN_EVT, data);
        }
    }
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChConnInd
 * Description  This function is called when receive connect ind.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChConnInd(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    bool bind = false;
    /* find the conn for the connection */
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
            /* find the connection to the device and send cfm event to app */
            bind = true;
            if (cbDev->cbBr->ia == AVCT_ACPT) {
                AvctCbConnEvtCallback(cbConn, AVCT_BR_CONNECT_IND_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
            } else {
                AvctCbConnEvtCallback(cbConn, AVCT_BR_CONNECT_CFM_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
            }
        }
    }
    /* can not bind to conn ,send disconnect */
    if (!bind) {
        AvctCbBrEvt(cbDev, AVCT_DISCONN_EVT, data);
    }
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChCloseInd
 * Description  This function is called when receive close ind.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChCloseInd(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    /* find conn which connected to the device, send callback event,relase memory */
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        if (g_avctMng.cbConn[i].cbDev == cbDev) {
            cbConn = &g_avctMng.cbConn[i];
            /* Send AVCT_DISCONNECT_IND_EVT */
            AvctCbConnEvtCallback(cbConn, AVCT_BR_DISCONNECT_IND_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
        }
    }
    AvctCbChDealloc(&(cbDev->cbBr));
    cbDev->chLink &= ~AVCT_ALLOC_BR;
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChCloseCfm
 * Description  This function is called when receive close confirm.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChCloseCfm(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
            /* Send AVCT_BR_DISCONNECT_CFM_EVT */
            AvctCbConnEvtCallback(cbConn, AVCT_BR_DISCONNECT_CFM_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
        }
    }
    /* Free resource */
    AvctCbChDealloc(&(cbDev->cbBr));
    cbDev->chLink &= ~AVCT_ALLOC_BR;
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrChBusyInd
 * Description  This function is called when receive channel busy.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrChBusyInd(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    uint8_t event = AVCT_BR_CHANNEL_UNBUSY_EVT;
    if (data->result == AVCT_BUSY_ST) {
        event = AVCT_BR_CHANNEL_BUSY_EVT;
    }
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
            /* Send AVCT_BR_CHANNEL_BUSY_EVT */
            AvctCbConnEvtCallback(cbConn, event, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
        }
    }
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbBrDiscardMsg
 * Description  This function is called when discard received message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrDiscardMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    return AVCT_FAILED;
}

/*
 * Function     AvctCbBrSendMsg
 * Description  This function is called to send message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrSendMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    uint16_t ret = AVCT_SUCCESS;
    AvctTxMsg txMsg = data->txMsg;
    AvctCbCh *cbBr = cbDev->cbBr;
    /* Malloc Head */
    Packet *pkt = PacketInheritMalloc(txMsg.buf, AVCT_PKG_HDR_LEN_SINGLE, 0);
    if (pkt == NULL) {
        LOG_ERROR("[AVCT] %{public}s: PacketInheritMalloc failed!", __func__);
        ret = AVCT_ERR_NO_RESOURCES;
    } else {
        /* Make Packet Head */
        AvctMakeSignleMsgHead(pkt, txMsg.label, txMsg.cr, txMsg.cbConn->connParam.pid);
        /* Send Packet */
        int lRet = L2CIF_SendData(cbBr->chId, pkt, NULL);
        if (lRet) {
            LOG_WARN("[AVCT] %{public}s:L2CIF_SendData failed.error is %{public}d", __func__, lRet);
        }
        PacketFree(pkt);
    }
    return ret;
}

/*
 * Function     AvctCbBrRevMsg
 * Description  This function is called when received message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbBrRevMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    uint16_t ret = AVCT_SUCCESS;
    uint8_t sHead[AVCT_PKG_HDR_LEN_SINGLE] = {0};
    Packet *pkt = AvctMsgAsmbl(cbDev->cbBr, data->buf);
    if (pkt == NULL) {
        return ret;
    }
    /* Extract signle pkt head info */
    PacketExtractHead(pkt, sHead, AVCT_PKG_HDR_LEN_SINGLE);
    uint8_t label;
    uint8_t cr;
    uint8_t ipid;
    uint16_t pid;
    AVCT_PARSE_SIGNLE_HDR(sHead, label, cr, ipid, pid);
    if (ipid == 1) {
        /* an invalid Profile Identifier received */
        LOG_WARN("[AVCT] %{public}s: Invalid ipid!", __func__);
        return ret;
    }
    /* Get the conn by pid */
    AvctCbConn *cbConn = AvctGetCbConnByPid(cbDev, pid);
    if (cbConn != NULL) {
        /* Send msg to app */
        if (cbConn->connParam.msgCallback != NULL) {
            (*cbConn->connParam.msgCallback)(cbConn->connId, label, cr, AVCT_DATA_BR, pkt, cbConn->connParam.context);
        }
        return ret;
    }
    /* Don't find pid connection,send reject */
    if (cr == AVCT_CMD) {
        Packet *sndPkg = PacketMalloc(AVCT_PKG_HDR_LEN_SINGLE, 0, 0);
        if (sndPkg == NULL) {
            LOG_ERROR("[AVCT] %{public}s: PacketMalloc failed!", __func__);
            ret = AVCT_ERR_NO_RESOURCES;
        } else {
            AvctMakeRejMsgHead(sndPkg, label, pid);
            int lRet = L2CIF_SendData(cbDev->cbBr->chId, sndPkg, NULL);
            if (lRet) {
                LOG_WARN("[AVCT] %{public}s:L2CIF_SendData failed.error is %{public}d", __func__, lRet);
            }
            PacketFree(sndPkg);
        }
    }
    return ret;
}

/*
 * Function     AvctCbBrDiscardRevMsg
 * Description  This function is called to discard received message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */

uint16_t AvctCbBrDiscardRevMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    return AVCT_SUCCESS;
}
