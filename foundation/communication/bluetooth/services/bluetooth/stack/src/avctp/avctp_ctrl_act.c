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

#include "avctp_ctrl_act.h"
#include "avctp_br.h"
#include "avctp_conn.h"
#include "avctp_ctrl.h"
#include "avctp_dev.h"
#include "avctp_gap.h"
#include "avctp_st.h"
#include "l2cap_if.h"
#include "log.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/*****************************************************************************
 * Function
 ****************************************************************************/
Packet *AvctMakeFrgMsgHeadStart(uint8_t num, uint8_t label, uint8_t cr, uint16_t pid);
Packet *AvctMakeFrgMsgHeadContinueEnd(uint8_t label, uint8_t type, uint8_t cr);

/*
 * Function     AvctCbCtrlChConn
 * Description  This function is called to Open control channel link.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChConn(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    GAP_Service serviceId;
    /* Request servcie security */
    if (g_avctMng.role == AVCT_CT) {
        serviceId = AVRCP_CT;
    } else {
        serviceId = AVRCP_TG;
    }
    AvctRequestSecurity(cbDev, serviceId, AVCT_PSM);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChConnFail
 * Description  This function is called to Open control channel link failed.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChConnFail(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
            AvctCbConnEvtCallback(cbConn, AVCT_CONNECT_CFM_EVT, data->result, &(cbConn->cbDev->peerAddr));
            AvctCbConnDealloc(cbConn);
        }
    }
    AvctCbDevDealloc(cbDev);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChBind
 * Description  This function is called when the control channel has been
 *              created,and try to bind it.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChBind(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    data->cbConn->cbDev = cbDev;
    cbDev->bindCnt++;
    /* Send callback event to app */
    AvctCbConnEvtCallback(data->cbConn, AVCT_CONNECT_CFM_EVT, AVCT_SUCCESS, &(data->cbConn->cbDev->peerAddr));
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChBindFail
 * Description  This function is called when the control bind failed.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChBindFail(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* send connect cfm event ,result failed */
    AvctCbConnEvtCallback(data->cbConn, AVCT_CONNECT_CFM_EVT, AVCT_FAILED, &(data->cbConn->cbDev->peerAddr));
    /* dealloc memory */
    AvctCbConnDealloc(data->cbConn);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChDisconn
 * Description  This function is called to disconnect the control channel.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChDisconn(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    L2CIF_DisconnectionReq(cbDev->cbCtrl->chId, NULL);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChUnbind
 * Description  This function is called to unbind and disconnect the control
 *              channel.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChUnbind(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* send disconnect cfm event ,result failed */
    AvctCbConnEvtCallback(data->cbConn, AVCT_DISCONNECT_CFM_EVT, AVCT_SUCCESS, &(data->cbConn->cbDev->peerAddr));
    /* dealloc memory */
    AvctCbConnDealloc(data->cbConn);
    cbDev->bindCnt--;
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChCheckDisconn
 * Description  This function is called to check if the connect is the last one which is using the ctrl channel.
 *              If it is the last one, disconnect the channel.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChCheckDisconn(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* check if the connect is the last one which is using the ctrl channel */
    if (cbDev->bindCnt > 1) {
        /* unbind */
        AvctCbCtrlChUnbind(cbDev, data);
    } else {
        /* ctrl channel disconnect */
        AvctCbCtrlEvt(cbDev, AVCT_DISCONN_EVT, data);
        /* if the browser channel is exit, disconnect too */
        if (cbDev->cbBr != NULL) {
            AvctCbBrEvt(cbDev, AVCT_DISCONN_EVT, data);
        }
    }
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChConnInd
 * Description  This function is called when receive the connect ind.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChConnInd(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    bool bind = false;
    /* find the conn for the connection */
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if (cbConn->status) {
            if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
                /* find the connection to the device and send cfm event to app */
                bind = true;
                AvctCbConnEvtCallback(cbConn, AVCT_CONNECT_CFM_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
            } else if ((cbConn->cbDev == NULL) && (cbConn->connParam.role == AVCT_ACPT) &&
                       (AvctGetCbConnByPid(cbDev, cbConn->connParam.pid) == NULL)) {
                /* bind the connection(ACCEPT) to the device */
                bind = true;
                cbConn->cbDev = cbDev;
                cbConn->status = AVCT_CONN_BIND;
                AvctCbConnEvtCallback(cbConn, AVCT_CONNECT_IND_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
            } else {
                LOG_DEBUG("[AVCT] %{public}s:i(%hhu)Can't find the cbConn! "
                          "cbConn->connId(%hhu),cbConn->status(%hhu)",
                    __func__,
                    i,
                    cbConn->connId,
                    cbConn->status);
            }
        }
    }
    /* can not bind to conn,send disconnect */
    if (!bind) {
        AvctCbCtrlEvt(cbDev, AVCT_DISCONN_EVT, data);
    }
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChCloseInd
 * Description  This function is called when receive the close ind.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChCloseInd(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    /* find conn which connected to the device, send callback event, relase memory */
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
            /* Send AVCT_DISCONNECT_IND_EVT */
            AvctCbConnEvtCallback(cbConn, AVCT_DISCONNECT_IND_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
            /* If Br channel is connected, send AVCT_DISCONN_EVT to close channel */
            if (cbConn->cbDev->cbBr != NULL) {
                AvctCbBrEvt(cbDev, AVCT_DISCONN_EVT, (AvctEvtData *)&cbConn);
            }
            AvctCbConnDealloc(cbConn);
        }
    }
    AvctCbChDealloc(&(cbDev->cbCtrl));
    AvctCbDevDealloc(cbDev);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChCloseCfm
 * Description  This function is called when receive the close confirm.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChCloseCfm(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        cbConn = &g_avctMng.cbConn[i];
        if ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev == cbDev)) {
            /* Send AVCT_DISCONNECT_CFM_EVT */
            AvctCbConnEvtCallback(cbConn, AVCT_DISCONNECT_CFM_EVT, AVCT_SUCCESS, &(cbConn->cbDev->peerAddr));
            AvctCbConnDealloc(cbConn);
        }
    }
    /* Free resource */
    AvctCbChDealloc(&(cbDev->cbCtrl));
    AvctCbDevDealloc(cbDev);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctCbCtrlChBusyInd
 * Description  This function is called when receive the channel busy ind.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlChBusyInd(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    uint8_t event = AVCT_CHANNEL_UNBUSY_EVT;
    if (data->result == AVCT_BUSY_ST) {
        event = AVCT_CHANNEL_BUSY_EVT;
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
 * Function     AvctCbCtrlDiscardMsg
 * Description  This function is called to discard message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlDiscardMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    return AVCT_FAILED;
}

/*
 * Function     AvctCbCtrlSendMsg
 * Description  This function is called to send message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */

uint16_t AvctCbCtrlSendMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    uint16_t ret;
    Packet *pkt = data->txMsg.buf;
    uint16_t msgLen = (uint16_t)PacketSize(pkt);
    LOG_DEBUG("[AVCT] %{public}s:PKT length %hu", __func__, msgLen);
    if (msgLen > (cbDev->cbCtrl->peerMtu - AVCT_PKG_HDR_LEN_SINGLE)) {
        /* Need Message Fragmentation */
        ret = AvctSendFraMsg((const AvctCbCh *)cbDev->cbCtrl, (const AvctTxMsg)data->txMsg);
    } else {
        /* Send signle Message */
        ret = AvctSendSignleMsg((const AvctCbCh *)cbDev->cbCtrl, (const AvctTxMsg)data->txMsg);
    }
    return ret;
}

/*
 * Function     AvctCbCtrlRevMsg
 * Description  This function is called when receive message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
NO_SANITIZE("cfi") uint16_t AvctCbCtrlRevMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    uint16_t ret = AVCT_SUCCESS;
    uint8_t sHead[AVCT_PKG_HDR_LEN_SINGLE] = {0};
    Packet *pkt = AvctMsgAsmbl(cbDev->cbCtrl, data->buf);
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
            (*cbConn->connParam.msgCallback)(cbConn->connId, label, cr, AVCT_DATA_CTRL, pkt, cbConn->connParam.context);
        }
        return ret;
    }
    LOG_DEBUG("[AVCT] %{public}s: ---------cbConn is null--------!", __func__);
    /* Don't find pid connection,send reject */
    if (cr == AVCT_CMD) {
        Packet *sndPkg = PacketMalloc(AVCT_PKG_HDR_LEN_SINGLE, 0, 0);
        if (sndPkg == NULL) {
            LOG_ERROR("[AVCT] %{public}s: PacketMalloc failed!", __func__);
            ret = AVCT_ERR_NO_RESOURCES;
        } else {
            AvctMakeRejMsgHead(sndPkg, label, pid);
            int lRet = L2CIF_SendData(cbDev->cbCtrl->chId, sndPkg, NULL);
            if (lRet) {
                LOG_WARN("[AVCT] %{public}s:L2CIF_SendData failed.error is %{public}d", __func__, lRet);
            }
            PacketFree(sndPkg);
        }
    }
    return ret;
}

/*
 * Function     AvctCbCtrlDiscardRevMsg
 * Description  This function is called to discard received message.
 * Param[in]    *cbDev  The point to the device block.
 * Param[in]    *data  Event data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctCbCtrlDiscardRevMsg(AvctCbDev *cbDev, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    return AVCT_SUCCESS;
}

/*
 * Function     AvctSendSignleMsg
 * Description  This function is called to send single type package.
 * Param[in]    *cbCh  The point to the send link.
 * Param[in]    txMsg  Package data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctSendSignleMsg(const AvctCbCh *cbCh, const AvctTxMsg txMsg)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    uint16_t ret;
    /* Malloc Head */
    Packet *pkt = PacketInheritMalloc(txMsg.buf, AVCT_PKG_HDR_LEN_SINGLE, 0);
    if (pkt == NULL) {
        LOG_ERROR("[AVCT] %{public}s: PacketInheritMalloc failed!", __func__);
        ret = AVCT_ERR_NO_RESOURCES;
    } else {
        /* Make Packet Head */
        AvctMakeSignleMsgHead(pkt, txMsg.label, txMsg.cr, txMsg.cbConn->connParam.pid);
        /* Send Packet */
        ret = L2CIF_SendData(cbCh->chId, pkt, NULL);
        /* pkt data debug print */
        AvctPktDataPrint(pkt);
        PacketFree(pkt);
    }
    return ret;
}

/*
 * Function     AvctSendFraMsg
 * Description  This function is called to send fragmentation package.
 * Param[in]    *cbCh  The point to the send link.
 * Param[in]    txMsg  Package data
 * Return       AVCT_SUCCESS. Others: Fail
 */
uint16_t AvctSendFraMsg(const AvctCbCh *cbCh, const AvctTxMsg txMsg)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    uint16_t ret;
    uint8_t pktType = AVCT_PKT_TYPE_START;
    Packet *pkt = txMsg.buf;
    Packet *sndPkg = NULL;
    uint16_t msgLen = (uint16_t)PacketSize(pkt);
    size_t size;
    /* calculate Number of AVCTP Packets */
    uint8_t num = (msgLen - (cbCh->peerMtu - AVCT_PKG_HDR_LEN_START)) / (cbCh->peerMtu - AVCT_PKG_HDR_LEN_CONTINUE) + 1;
    if ((msgLen - (cbCh->peerMtu - AVCT_PKG_HDR_LEN_START)) % (cbCh->peerMtu - AVCT_PKG_HDR_LEN_CONTINUE) != 0) {
        num++;
    }
    /* Fragement Message */
    do {
        /* Make Packet Head data */
        if (pktType == AVCT_PKT_TYPE_START) {
            sndPkg = AvctMakeFrgMsgHeadStart(num, txMsg.label, txMsg.cr, txMsg.cbConn->connParam.pid);
            if (sndPkg == NULL) {
                LOG_ERROR("[AVCT] %{public}s: PacketMalloc failed!", __func__);
                ret = AVCT_ERR_NO_RESOURCES;
                break;
            }
        } else {
            sndPkg = AvctMakeFrgMsgHeadContinueEnd(txMsg.label, pktType, txMsg.cr);
            if (sndPkg == NULL) {
                LOG_ERROR("[AVCT] %{public}s: Packet memory Malloc failed!", __func__);
                ret = AVCT_ERR_NO_RESOURCES;
                break;
            }
        }
        /* Make Fragment packet */
        if (pktType == AVCT_PKT_TYPE_START) {
            size = cbCh->peerMtu - AVCT_PKG_HDR_LEN_START;
        } else {
            size = cbCh->peerMtu - AVCT_PKG_HDR_LEN_CONTINUE;
        }
        msgLen = PacketFragment(pkt, sndPkg, size);
        /* Send packet */
        ret = L2CIF_SendData(cbCh->chId, sndPkg, NULL);
        /* Free pkt memmory */
        PacketFree(sndPkg);
        /* Change packet type */
        if (msgLen > (cbCh->peerMtu - AVCT_PKG_HDR_LEN_END)) {
            pktType = AVCT_PKT_TYPE_CONTINUE;
        } else {
            pktType = AVCT_PKT_TYPE_END;
        }
    } while ((msgLen > 0) && (ret == L2CAP_SUCCESS));
    return ret;
}

/*
 * Function     AvctParsePktType
 * Description  This function is parase pkt type.
 * Param[in]    data  pkg first byte.
 * Param[out]   type  Package type
 * Return       void
 */
void AvctParsePktType(uint8_t data, uint8_t *type)
{
    *type = (data >> MOVE_2BIT) & PKG_TYPE_MASK;
}

/*
 * Function     AvctMsgAsmbl
 * Description  This function is called to asbmle fragmentation package.
 * Param[in]    *cbCh  The point to the send link.
 * Param[in]    pkt  Package data buffer
 * Return       Packet. Package data buffer or NULL
 */
Packet *AvctMsgAsmbl(AvctCbCh *cbCh, Packet *pkt)
{
    Packet *ret = NULL;
    uint8_t fistByte = 0;
    uint8_t pktType = 0;
    uint8_t sHead[4] = {0};
    /* Read first byte to check packet type */
    PacketPayloadRead(pkt, &fistByte, 0, 1);
    AvctParsePktType(fistByte, &pktType);
    LOG_DEBUG("[AVCT] %{public}s:packet type is %hhu", __func__, pktType);
    if (pktType == AVCT_PKT_TYPE_SINGLE) {
        ret = pkt;
    } else if (pktType == AVCT_PKT_TYPE_START) {
        if (cbCh->rxMsg != NULL) {
            /* Free pkt memmory */
            PacketFree(cbCh->rxMsg);
            cbCh->rxMsg = NULL;
        }
        /* Make single packet head at payload */
        cbCh->rxMsg = PacketMalloc(0, 0, AVCT_PKG_HDR_LEN_SINGLE);
        sHead[0] = fistByte;
        PacketPayloadRead(pkt, &sHead[1], AVCT_START_PKT_PID_OFFSET, AVCT_START_PKT_PID_SIZE);
        PacketPayloadWrite(cbCh->rxMsg, sHead, 0, AVCT_PKG_HDR_LEN_SINGLE);
        /* Extrect start head */
        PacketExtractHead(pkt, sHead, AVCT_PKG_HDR_LEN_START);
        /* Add packet playload */
        PacketAssemble(cbCh->rxMsg, pkt);
        LOG_DEBUG("[AVCT] %{public}s:cbCh->rxMsg PacketSize(%u)", __func__, PacketSize(cbCh->rxMsg));
    } else {
        /* continue or end */
        /* Extrect continue/end head */
        PacketExtractHead(pkt, sHead, AVCT_PKG_HDR_LEN_CONTINUE);
        /* Add packet playload */
        PacketAssemble(cbCh->rxMsg, pkt);
        if (pktType == AVCT_PKT_TYPE_END) {
            ret = cbCh->rxMsg;
        }
        LOG_DEBUG("[AVCT] %{public}s:cbCh->rxMsg PacketSize(%u)", __func__, PacketSize(cbCh->rxMsg));
    }
    return ret;
}
/*
 * Function     AvctMakeSignleMsgHead
 * Description  Make signle packet head.
 * Param[out]   *pkt    The point to the packet.
 * Param[in]    label   stransport label
 * Param[in]    cr      scommand / response
 * Param[in]    pid     profile id
 * Return       void
 */
void AvctMakeSignleMsgHead(const Packet *pkt, uint8_t label, uint8_t cr, uint16_t pid)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    Buffer *buf = PacketHead(pkt);
    uint8_t *p = (uint8_t *)BufferPtr(buf);
    do {
        *(p)++ = (((label) << MOVE_4BIT) | (AVCT_PKT_TYPE_SINGLE << MOVE_2BIT) | ((cr) << MOVE_1BIT));
        *(p)++ = (uint8_t)((pid) >> MOVE_8BIT);
        *(p) = (uint8_t)((pid)&0x00FF);
    } while (0);
    return;
}
/*
 * Function     AvctMakeRejMsgHead
 * Description  Make reject pakect head.
 * Param[out]   *pkt    The point to the packet.
 * Param[in]    label   stransport label
 * Param[in]    pid     profile id
 * Return       void
 */
void AvctMakeRejMsgHead(const Packet *pkt, uint8_t label, uint16_t pid)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    Buffer *buf = PacketHead(pkt);
    uint8_t *p = (uint8_t *)BufferPtr(buf);
    do {
        *(p)++ = (((label) << MOVE_4BIT) | (AVCT_PKT_TYPE_SINGLE << MOVE_2BIT) | AVCT_REJ);
        *(p)++ = (uint8_t)((pid) >> MOVE_8BIT);
        *(p) = (uint8_t)((pid)&0x00FF);
    } while (0);
    return;
}
/*
 * Function     AvctMakeFrgMsgHeadStart
 * Description  Make fragment message type start.
 * Param[in]    num    The num of fragment pakect.
 * Param[in]    label   stransport label
 * Param[in]    pid     profile id
 * Return       void
 */
Packet *AvctMakeFrgMsgHeadStart(uint8_t num, uint8_t label, uint8_t cr, uint16_t pid)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    Packet *pkg = PacketMalloc(AVCT_PKG_HDR_LEN_START, 0, 0);
    if (pkg != NULL) {
        Buffer *buf = PacketHead(pkg);
        uint8_t *p = (uint8_t *)BufferPtr(buf);
        do {
            *(p)++ = (((label) << MOVE_4BIT) | (AVCT_PKT_TYPE_START << MOVE_2BIT) | ((cr) << MOVE_1BIT));
            *(p)++ = num;
            *(p)++ = (uint8_t)((pid) >> MOVE_8BIT);
            *(p) = (uint8_t)((pid)&0x00FF);
        } while (0);
    }
    return pkg;
}
/*
 * Function     AvctMakeFrgMsgHeadContinueEnd
 * Description  Make fragment message type continue/End.
 * Param[in]    label   stransport label
 * Param[in]    type    continue/End
 * Param[in]    cr      command/response
 * Return       void
 */
Packet *AvctMakeFrgMsgHeadContinueEnd(uint8_t label, uint8_t type, uint8_t cr)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    /* Create Continue/End Head */
    Packet *pkg = PacketMalloc(AVCT_PKG_HDR_LEN_CONTINUE, 0, 0);
    if (pkg != NULL) {
        Buffer *buf = PacketHead(pkg);
        uint8_t *p = (uint8_t *)BufferPtr(buf);
        do {
            *(p)++ = (((label) << MOVE_4BIT) | ((type) << MOVE_2BIT) | ((cr) << MOVE_1BIT));
        } while (0);
    }
    return pkg;
}

void AvctPktDataPrint(const Packet *pkt)
{
    uint16_t len = (uint16_t)PacketSize(pkt);
    uint8_t buf[AVCT_32BYTE] = {0};
    if (len > AVCT_32BYTE) {
        len = AVCT_32BYTE;
    }
    PacketRead(pkt, buf, 0, len);
    for (int i = 0; i < len; i++) {
        LOG_DEBUG("%02X ", buf[i]);
    }
}
