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
#include "avctp_api.h"
#include "avctp_br.h"
#include "avctp_conn.h"
#include "avctp_ctrl.h"
#include "avctp_dev.h"
#include "avctp_st.h"
#include "btm/btm_thread.h"
#include "log.h"
#include "securec.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/*
 * Function:    AVCT_Register
 * Description: This is the system level registration function for the AVCTP
 *              protocol. This function initializes AVCTP protocal and prepares
 *              the protocol stack for its use.  This function must be called
 *              once by the system before the other functions of the API can be
 *              used.
 * param[in]    mtu        max transcation unit for control
 * param[in]    mtuBr      max transcation unit for browser
 * param[in]    role       CT/TG
 * Return:      void
 */
void AVCT_Register(uint16_t mtu, uint16_t mtuBr, uint16_t role)
{
    LOG_INFO("[AVCT] %{public}s: Regist mut(%hu) mtuBr(%hu) role(%hu)", __func__, mtu, mtuBr, role);
    Event *event = EventCreate(true);
    AvctRegisterTskParam *param = malloc(sizeof(AvctRegisterTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return;
    }
    (void)memset_s(param, sizeof(AvctRegisterTskParam), 0, sizeof(AvctRegisterTskParam));
    param->mtu = mtu;
    param->mtuBr = mtuBr;
    param->role = role;
    param->event = event;
    if (!AvctAsyncProcess(AvctRegisterTsk, param)) {
        EventWait(event, WAIT_TIME);
    }
    free(param);
    EventDelete(event);
    return;
}

void AvctRegisterTsk(void *context)
{
    AvctRegisterTskParam *param = (AvctRegisterTskParam *)context;
    AvctRegister(param->mtu, param->mtuBr, param->role);
    EventSet(param->event);
    return;
}

void AvctRegister(uint16_t mtu, uint16_t mtuBr, uint16_t role)
{
    LOG_DEBUG("[AVCT] %{public}s: Regist mut(%hu) mtuBr(%hu) role(%hu)", __func__, mtu, mtuBr, role);
    /* set receive MTU */
    if (!g_avctMng.registered) {
        g_avctMng.rxMtu = mtu;
        g_avctMng.rxMtuBr = mtuBr;
        g_avctMng.role = role;
        g_avctMng.registered = true;
    }
    return;
}

/*
 * Function     AVCT_Deregister
 * Description  This function is called to deregister AVCTP protocol.
 *              It is called when AVCTP is no longer being used by any
 *              application in the system. All connections must be
 *              disconned in advance.
 * Param[in]    void
 * Return       void
 */
void AVCT_Deregister(void)
{
    LOG_INFO("[AVCT] %{public}s:", __func__);
    Event *event = EventCreate(true);
    AvctDeregisterTskParam *param = malloc(sizeof(AvctDeregisterTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return;
    }
    (void)memset_s(param, sizeof(AvctDeregisterTskParam), 0, sizeof(AvctDeregisterTskParam));
    param->event = event;
    if (!AvctAsyncProcess(AvctDeregisterTsk, param)) {
        EventWait(event, WAIT_TIME);
    }
    free(param);
    EventDelete(event);
    return;
}

void AvctDeregisterTsk(void *context)
{
    AvctDeregisterTskParam *param = (AvctDeregisterTskParam *)context;
    AvctDeregister();
    EventSet(param->event);
    return;
}

void AvctDeregister(void)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    for (int i = 0; i < AVCT_MAX_CONNECTS; i++) {
        if (g_avctMng.cbConn[i].status != AVCT_CONN_UNALLOC) {
            AvctDisconnectReq(g_avctMng.cbConn[i].connId);
        }
    }
    return;
}

/*
 * Function     AVCT_ConnectReq
 * Description  This function is called to create an avctp control connection
 *              for peer device.
 *              AVCT.ICS #Table 2/11 3/11 Connect Request
 *              AVCT.ICS #Table 3/9 Event register for connection request.
 *              AVCT Profile #11.2.1 #11.1
 * Param[in]   *connParam  oint to the info of the connection request.
 *              Such as the Role、Profile ID、Callback func point.
 * Param[in]   peerAddr  The peer address to be connected.
 * Param[out]  *connId    Poit to the handle of the connected channel
 * Return      AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AVCT_ConnectReq(uint8_t *connId, const AvctConnectParam *connParam, const BtAddr *peerAddr)
{
    LOG_INFO("[AVCT] %{public}s: peerAddr is (%x:%x:%x:%x:%x:%x) role(%hhu)",
        __func__,
        BT_ADDR_FMT_ASC(peerAddr->addr),
        connParam->role);
    uint16_t ret = AVCT_FAILED;
    Event *event = EventCreate(true);
    AvctConnectReqTskParam *param = malloc(sizeof(AvctConnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return ret;
    }
    (void)memset_s(param, sizeof(AvctConnectReqTskParam), 0, sizeof(AvctConnectReqTskParam));
    param->event = event;
    param->connId = *connId;
    (void)memcpy_s(&param->connParam, sizeof(AvctConnectParam), connParam, sizeof(AvctConnectParam));
    (void)memcpy_s(&param->peerAddr, sizeof(BtAddr), peerAddr, sizeof(BtAddr));
    if (!AvctAsyncProcess(AvctConnectReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return ret;
}

void AvctConnectReqTsk(void *context)
{
    AvctConnectReqTskParam *param = (AvctConnectReqTskParam *)context;
    param->ret = AvctConnectReq(&param->connId, &param->connParam, &param->peerAddr);
    EventSet(param->event);
    return;
}
uint16_t AvctConnectReq(uint8_t *connId, const AvctConnectParam *connParam, const BtAddr *peerAddr)
{
    uint16_t ret = AVCT_SUCCESS;
    if (connParam == NULL || peerAddr == NULL || connParam->chEvtCallback == NULL || connParam->msgCallback == NULL) {
        ret = AVCT_ERR_PARAM;
        return ret;
    }
    LOG_DEBUG("[AVCT] %{public}s: peerAddr is (%x:%x:%x:%x:%x:%x) role(%hhu)",
        __func__,
        BT_ADDR_FMT_ASC(peerAddr->addr),
        connParam->role);
    AvctCbConn *cbConn = AvctCbConnAlloc(connParam);
    if (cbConn == NULL) {
        LOG_ERROR("[AVCT]There is no connection resource for the connID(%hhu)!", *connId);
        ret = AVCT_ERR_NO_RESOURCES;
        return ret;
    }
    *connId = cbConn->connId;
    if (cbConn->connParam.role == AVCT_INIT) {
        ret = AvctConnectInitiate(cbConn, connParam, peerAddr);
    }
    return ret;
}

/*
 * Function     AVCT_DisconnectReq
 * Description  This function is called to disconnect the avctp control channel.
 *              AVCT.ICS #Table 2/12 3/12 Disconnect Request AVCT.ICS #Table
 *              3/10 Event register for disconnection request. AVCT Profile
 *              #11.2.2 #11.2.3 #11.1
 * Param[in]    connId   The id of the connection channel to be disconnected.
 * Return       AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AVCT_DisconnectReq(uint8_t connId)
{
    LOG_INFO("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t ret = AVCT_FAILED;
    Event *event = EventCreate(true);
    AvctDisconnectReqTskParam *param = malloc(sizeof(AvctDisconnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return ret;
    }
    (void)memset_s(param, sizeof(AvctDisconnectReqTskParam), 0, sizeof(AvctDisconnectReqTskParam));
    param->event = event;
    param->connId = connId;
    if (!AvctAsyncProcess(AvctDisconnectReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return ret;
}

void AvctDisconnectReqTsk(void *context)
{
    AvctDisconnectReqTskParam *param = (AvctDisconnectReqTskParam *)context;
    param->ret = AvctDisconnectReq(param->connId);
    EventSet(param->event);
    return;
}

uint16_t AvctDisconnectReq(uint8_t connId)
{
    LOG_DEBUG("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t ret = AvctDisconnect(connId, AVCT_CH_CTRL);
    return ret;
}

/*
 * Function     AVCT_SendMsgReq
 *
 * Description  This function is called to send an  message to the channel.
 *              AVCT.ICS #Table 2/13 3/13 Send Message
 *              AVCT.ICS #Table 3/10 Event register for message  reception.
 *              AVCT Profile #11.2.3 #11.2.4 #11.1
 * Pram[in]     connId The id of the connection channel which is the message to
 *              be send.
 * Param[in]    label   Message label from application
 * Param[in]    cr      command/response
 * Param[in]    msg   Message data point
 * Return      AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AVCT_SendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg)
{
    LOG_INFO("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t ret = AVCT_FAILED;
    Event *event = EventCreate(true);
    AvctSendMsgReqTskParam *param = malloc(sizeof(AvctSendMsgReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return ret;
    }
    (void)memset_s(param, sizeof(AvctSendMsgReqTskParam), 0, sizeof(AvctSendMsgReqTskParam));
    param->event = event;
    param->connId = connId;
    param->label = label;
    param->cr = cr;
    param->msg = PacketRefMalloc((Packet *)msg);
    if (!AvctAsyncProcess(AvctSendMsgReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        ret = param->ret;
    }
    PacketFree(param->msg);
    free(param);
    EventDelete(event);
    return ret;
}

void AvctSendMsgReqTsk(void *context)
{
    AvctSendMsgReqTskParam *param = (AvctSendMsgReqTskParam *)context;
    param->ret = AvctSendMsgReq(param->connId, param->label, param->cr, param->msg);
    EventSet(param->event);
    return;
}

uint16_t AvctSendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg)
{
    LOG_DEBUG("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t ret;
    AvctTxMsg txMsg = {0};
    AvctCbConn *cbConn = AvctGetCbConnByConnId(connId);
    if (msg == NULL) {
        LOG_ERROR("[AVCT]The message point is NULL!");
        ret = AVCT_ERR_PARAM;
        return ret;
    }
    if (cbConn == NULL) {
        LOG_ERROR("[AVCT]There is no connection for the ID(%hhu)!", connId);
        ret = AVCT_ERR_CONN_BAD;
    } else if ((cbConn->status == AVCT_CONN_ALLOC) ||
               ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev->cbCtrl == NULL))) {
        LOG_ERROR("[AVCT]The connection(%hhu) is not open!", connId);
        ret = AVCT_ERR_CONN_NOT_OPEN;
    } else {
        /* send message */
        txMsg.cr = cr;
        txMsg.label = label;
        txMsg.buf = (Packet *)msg;
        txMsg.cbConn = cbConn;
        AvctEvtData evtData;
        evtData.txMsg = txMsg;
        ret = AvctCbCtrlEvt(cbConn->cbDev, AVCT_SED_MSG_EVT, &evtData);
    }
    return ret;
}

/*
 * Function     AVCT_GetPeerMtu
 *
 * Description  This function is called to get the mtu of the control channel.
 * Param[in]    connId   The id of the connection channel.
 * Return       MTU size.
 */
uint16_t AVCT_GetPeerMtu(uint8_t connId)
{
    LOG_DEBUG("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t peerMtu = 0;
    AvctCbConn *cbConn = NULL;
    if (connId < AVCT_MAX_CONNECTS) {
        cbConn = &g_avctMng.cbConn[connId];
    }
    if ((cbConn != NULL) && (cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev->cbCtrl != NULL)) {
        peerMtu = cbConn->cbDev->cbCtrl->peerMtu;
    }
    return peerMtu;
}

/*
 * Function     AVCT_BrConnectReq
 * Description  This function is called to create an avctp browser connection
 *              if the peer has.
 * Param[in]    connId   The control channel id of the peer device.
 * Param[in]    role     The connection role.
 * Return       AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AVCT_BrConnectReq(uint8_t connId, uint8_t role)
{
    LOG_INFO("[AVCT] %{public}s: Connid(0x%x)", __func__, connId);
    uint16_t ret = AVCT_FAILED;
    Event *event = EventCreate(true);
    AvctBrConnectReqTskParam *param = malloc(sizeof(AvctBrConnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return ret;
    }
    (void)memset_s(param, sizeof(AvctBrConnectReqTskParam), 0, sizeof(AvctBrConnectReqTskParam));
    param->event = event;
    param->connId = connId;
    param->role = role;
    if (!AvctAsyncProcess(AvctBrConnectReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return ret;
}

void AvctBrConnectReqTsk(void *context)
{
    AvctBrConnectReqTskParam *param = (AvctBrConnectReqTskParam *)context;
    param->ret = AvctBrConnectReq(param->connId, param->role);
    EventSet(param->event);
    return;
}

uint16_t AvctBrConnectReq(uint8_t connId, uint8_t role)
{
    LOG_DEBUG("[AVCT] %{public}s: Connid(0x%x)", __func__, connId);
    uint16_t ret = AVCT_SUCCESS;
    AvctCbConn *cbConn = AvctGetCbConnByConnId(connId);
    if ((cbConn == NULL) || (cbConn->status != AVCT_CONN_BIND)) {
        LOG_ERROR("[AVCT]There is no connection for the ID(%hhu)!", connId);
        ret = AVCT_ERR_CONN_BAD;
        return ret;
    }
    if (role == AVCT_INIT) {
        ret = AvctBrConnectInitiate(cbConn);
    }
    return ret;
}

/*
 * Function     AVCT_BrDisconnectReq
 * Description  Remove an AVCTP browser connection. If this is the last
 *              connection to a peer the L2CAP channel for AVCTP will be closed.
 * param[in]    connId   The connection channel id
 * Return       AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AVCT_BrDisconnectReq(uint8_t connId)
{
    LOG_INFO("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t ret = AVCT_FAILED;
    Event *event = EventCreate(true);
    AvctBrDisconnectReqTskParam *param = malloc(sizeof(AvctBrDisconnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return ret;
    }
    (void)memset_s(param, sizeof(AvctBrDisconnectReqTskParam), 0, sizeof(AvctBrDisconnectReqTskParam));
    param->event = event;
    param->connId = connId;
    if (!AvctAsyncProcess(AvctBrDisconnectReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return ret;
}

void AvctBrDisconnectReqTsk(void *context)
{
    AvctBrDisconnectReqTskParam *param = (AvctBrDisconnectReqTskParam *)context;
    param->ret = AvctBrDisconnectReq(param->connId);
    EventSet(param->event);
    return;
}

uint16_t AvctBrDisconnectReq(uint8_t connId)
{
    LOG_DEBUG("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t ret = AvctDisconnect(connId, AVCT_CH_BR);
    return ret;
}

/*
 * Function     AVCT_BrSendMsgReq
 * Description  This function is called to send an  message to the channel.
 *              AVCT.ICS #Table 2/13 3/13 Send Message
 *              AVCT.ICS #Table 3/10 Event register for message  reception.
 *              AVCT Profile #11.2.3 #11.2.4 #11.1
 * Param[in]    connId The id of the connection channel which is the message to
 *              be send.
 * Param[in]    label   Message label from application
 * Param[in]    cr      command/response
 * Param[in]    msg   Message data point
 * Return       AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AVCT_BrSendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg)
{
    LOG_INFO("[AVCT] %{public}s: Connid(0x%x)", __func__, connId);
    uint16_t ret = AVCT_FAILED;
    Event *event = EventCreate(true);
    AvctBrSendMsgReqTskParam *param = malloc(sizeof(AvctBrSendMsgReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVCT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return ret;
    }
    (void)memset_s(param, sizeof(AvctBrSendMsgReqTskParam), 0, sizeof(AvctBrSendMsgReqTskParam));
    param->event = event;
    param->connId = connId;
    param->label = label;
    param->cr = cr;
    param->msg = PacketRefMalloc((Packet *)msg);
    if (!AvctAsyncProcess(AvctBrSendMsgReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        ret = param->ret;
    }
    PacketFree(param->msg);
    free(param);
    EventDelete(event);
    return ret;
}

void AvctBrSendMsgReqTsk(void *context)
{
    AvctBrSendMsgReqTskParam *param = (AvctBrSendMsgReqTskParam *)context;
    param->ret = AvctBrSendMsgReq(param->connId, param->label, param->cr, param->msg);
    EventSet(param->event);
    return;
}

uint16_t AvctBrSendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg)
{
    LOG_DEBUG("[AVCT] %{public}s: Connid(0x%x)", __func__, connId);
    uint16_t ret = AVCT_SUCCESS;
    AvctTxMsg txMsg = {0};
    AvctCbConn *cbConn = AvctGetCbConnByConnId(connId);
    if (msg == NULL) {
        ret = AVCT_ERR_PARAM;
        return ret;
    }
    if (cbConn == NULL) {
        LOG_ERROR("[AVCT]There is no connection for the ID(%hhu)!", connId);
        ret = AVCT_ERR_CONN_BAD;
    } else if ((cbConn->status == AVCT_CONN_ALLOC) ||
               ((cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev->cbBr == NULL))) {
        LOG_ERROR("[AVCT]The connection(%hhu) is not open!", connId);
        ret = AVCT_ERR_CONN_NOT_OPEN;
    } else {
        /* send message */
        txMsg.cr = cr;
        txMsg.label = label;
        txMsg.buf = (Packet *)msg;
        txMsg.cbConn = cbConn;
        AvctEvtData evtData;
        evtData.txMsg = txMsg;
        AvctCbBrEvt(cbConn->cbDev, AVCT_SED_MSG_EVT, &evtData);
    }
    return ret;
}

/*
 * Function     AVCT_BrGetPeerMtu
 * Description  This function is called to get the mtu of the browser channel.
 * Pram[in]     connId   The id of the connection channel.
 * Return       MTU size.
 */
uint16_t AVCT_BrGetPeerMtu(uint8_t connId)
{
    LOG_DEBUG("[AVCT] %{public}s: Connid(0x%x)", __func__, connId);
    uint16_t peerMtu = 0;
    AvctCbConn *cbConn = NULL;
    if (connId < AVCT_MAX_CONNECTS) {
        cbConn = &g_avctMng.cbConn[connId];
    }
    if ((cbConn != NULL) && (cbConn->status == AVCT_CONN_BIND) && (cbConn->cbDev->cbBr != NULL)) {
        peerMtu = cbConn->cbDev->cbBr->peerMtu;
    }
    return peerMtu;
}

/*
 * Function     AvctConnectInitiate
 * Description  Connect for initiate role
 * Param[out]  *cbConn    connection
 * Param[in]   *connParam  oint to the info of the connection request.
 *              Such as the Role、Profile ID、Callback func point.
 * Param[in]   peerAddr  The peer address to be connected.
 * Return      AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AvctConnectInitiate(AvctCbConn *cbConn, const AvctConnectParam *connParam, const BtAddr *peerAddr)
{
    uint16_t ret = AVCT_SUCCESS;
    AvctCbCh *cbCtrl = NULL;
    AvctCbDev *cbDev = AvctGetCbDevByAddress(peerAddr);
    if (cbDev == NULL) {
        cbDev = AvctCbDevAlloc(peerAddr);
        if (cbDev == NULL) {
            LOG_ERROR("[AVCT]AvctCbDevAlloc Failed!");
            AvctCbConnDealloc(cbConn);
            ret = AVCT_ERR_NO_RESOURCES;
        } else {
            cbCtrl = AvctCbChAlloc();
            if (cbCtrl == NULL) {
                LOG_ERROR("[AVCT]AvctCbChAlloc Failed!");
                AvctCbConnDealloc(cbConn);
                AvctCbDevDealloc(cbDev);
                ret = AVCT_ERR_NO_RESOURCES;
            }
        }
    } else {
        /* check the connected ctrl channel has the same pid. */
        if (AvctGetCbConnByPid(cbDev, connParam->pid)) {
            LOG_ERROR("[AVCT]pid is in used !");
            ret = AVCT_ERR_PID_USED;
        }
    }
    if (ret == AVCT_SUCCESS) {
        cbDev->cbCtrl = cbCtrl;
        cbDev->chLink |= AVCT_ALLOC_CTRL;
        cbConn->cbDev = cbDev;
        cbConn->status = AVCT_CONN_BIND;
        AvctEvtData evtData;
        evtData.cbConn = cbConn;
        ret = AvctCbCtrlEvt(cbConn->cbDev, AVCT_BIND_EVT, &evtData);
    }
    return ret;
}
/*
 * Function     AvctDisconnect
 * Description  Disconnet request
 * Param[out]   *cbConn  connection
 * Param[in]    connId   connection ID.
 * Param[in]    type     connection type.
 * Return       AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AvctDisconnect(uint8_t connId, uint8_t type)
{
    LOG_INFO("[AVCT] %{public}s: ConnId(%hhu)", __func__, connId);
    uint16_t ret = AVCT_SUCCESS;
    AvctCbConn *cbConn = AvctGetCbConnByConnId(connId);
    if (cbConn == NULL) {
        LOG_ERROR("[AVCT]There is no connection for the ID(%hhu)!", connId);
        ret = AVCT_ERR_CONN_BAD;
    } else if (cbConn->cbDev == NULL) {
        AvctCbConnDealloc(cbConn);
    } else {
        /* send unbind event */
        AvctEvtData evtData;
        evtData.cbConn = cbConn;
        if (type == AVCT_CH_BR) {
            ret = AvctCbBrEvt(cbConn->cbDev, AVCT_UNBIND_EVT, &evtData);
        } else {
            ret = AvctCbCtrlEvt(cbConn->cbDev, AVCT_UNBIND_EVT, &evtData);
        }
    }
    return ret;
}

uint8_t AvctAsyncProcess(void (*callback)(void *context), void *context)
{
    if (BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_AVCTP, callback, context)) {
        return AVCT_FAILED;
    }
    return AVCT_SUCCESS;
}