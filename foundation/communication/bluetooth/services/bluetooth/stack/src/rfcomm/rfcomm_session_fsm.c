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

#include "rfcomm_defs.h"

typedef int (*SessionActionFunc)(RfcommSessionInfo *session, const void *data);
typedef struct {
    RfcommSessionEvent eventId;
    SessionActionFunc fn;
} RfcommSessionEvtAction;

static int RfcommOpenSession(RfcommSessionInfo *session, const void *data);
static int RfcommCloseSession(RfcommSessionInfo *session, const void *data);
static int RfcommRecvSessionSecurityRslt(RfcommSessionInfo *session, const void *data);
static int RfcommRecvConnectReq(RfcommSessionInfo *session, const void *data);
static int RfcommRecvReqConnectRslt(RfcommSessionInfo *session, const void *data);
static int RfcommRecvConnectRsp(RfcommSessionInfo *session, const void *data);
static int RfcommRecvConfigReq(RfcommSessionInfo *session, const void *data);
static int RfcommRecvConfigRsp(RfcommSessionInfo *session, const void *data);
static int RfcommRecvDisconnectReq(RfcommSessionInfo *session, const void *data);
static int RfcommRecvSabm0(RfcommSessionInfo *session, const void *data);
static int RfcommRecvDisc0(RfcommSessionInfo *session, const void *data);
static int RfcommRecvUa0(RfcommSessionInfo *session, const void *data);
static int RfcommRecvDm0(RfcommSessionInfo *session, const void *data);
static int RfcommRecvFconReq(RfcommSessionInfo *session, const void *data);
static int RfcommRecvFconRsp(RfcommSessionInfo *session, const void *data);
static int RfcommRecvFcoffReq(RfcommSessionInfo *session, const void *data);
static int RfcommRecvFcoffRsp(RfcommSessionInfo *session, const void *data);
static int RfcommRecvTestReq(RfcommSessionInfo *session, const void *data);
static int RfcommRecvTestRsp(RfcommSessionInfo *session, const void *data);
static int RfcommRecvNsc(RfcommSessionInfo *session, const void *data);
static int RfcommRecvLinkLoss(RfcommSessionInfo *session, const void *data);
static int RfcommRecvTimeOut(RfcommSessionInfo *session, const void *data);
static int RfcommRestartSession(RfcommSessionInfo *session);
static int RfcommAcceptPeerConnectReq(RfcommSessionInfo *session);

static RfcommSessionEvtAction g_sessionEvtActTbl[EV_SESSION_EV_MAX] = {
    {EV_SESSION_SEND_OPEN_REQ, RfcommOpenSession},
    {EV_SESSION_SEND_CLOSE_REQ, RfcommCloseSession},
    {EV_SESSION_RECV_SECURITY_RESULT, RfcommRecvSessionSecurityRslt},
    {EV_SESSION_RECV_REQ_CONNECT_RESULT, RfcommRecvReqConnectRslt},
    {EV_SESSION_RECV_CONNECT_REQ, RfcommRecvConnectReq},
    {EV_SESSION_RECV_CONNECT_RSP, RfcommRecvConnectRsp},
    {EV_SESSION_RECV_CONFIG_REQ, RfcommRecvConfigReq},
    {EV_SESSION_RECV_CONFIG_RSP, RfcommRecvConfigRsp},
    {EV_SESSION_RECV_DISCONNECT_REQ, RfcommRecvDisconnectReq},
    {EV_SESSION_RECV_SABM0, RfcommRecvSabm0},
    {EV_SESSION_RECV_DISC0, RfcommRecvDisc0},
    {EV_SESSION_RECV_UA0, RfcommRecvUa0},
    {EV_SESSION_RECV_DM0, RfcommRecvDm0},
    {EV_SESSION_RECV_FCON_REQ, RfcommRecvFconReq},
    {EV_SESSION_RECV_FCON_RSP, RfcommRecvFconRsp},
    {EV_SESSION_RECV_FCOFF_REQ, RfcommRecvFcoffReq},
    {EV_SESSION_RECV_FCOFF_RSP, RfcommRecvFcoffRsp},
    {EV_SESSION_RECV_TEST_REQ, RfcommRecvTestReq},
    {EV_SESSION_RECV_TEST_RSP, RfcommRecvTestRsp},
    {EV_SESSION_RECV_NSC, RfcommRecvNsc},
    {EV_SESSION_RECV_LINK_LOSS, RfcommRecvLinkLoss},
    {EV_SESSION_TIMEOUT, RfcommRecvTimeOut}
};

/**
 * @brief State machine for handling events related to session.
 *
 * @param session The pointer of the session in the session list.
 * @param event   The event id.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSessionEvtFsm(RfcommSessionInfo *session, RfcommSessionEvent event, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    int ret = 0;

    for (int cnt = 0; cnt < EV_SESSION_EV_MAX; cnt++) {
        if (g_sessionEvtActTbl[cnt].eventId == event) {
            ret = g_sessionEvtActTbl[cnt].fn(session, data);
            break;
        }
    }

    return ret;
}

/**
 * @brief Processing after receiving connect session request.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommOpenSession(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    int ret = 0;
    uint8_t scn = *(uint8_t *)data;

    switch (session->sessionState) {
        case ST_SESSION_CLOSED:
            session->sessionState = ST_SESSION_INITIATOR_WAIT_SECURITY_RESULT;
            ret = RfcommCheckSessionSecurity(session, scn, false);
            break;
        case ST_SESSION_DISC0_REQ_WAIT_UA0:
            // fall-through
        case ST_SESSION_CLOSING:
            // If a connection request is received during the closing process,
            // the status is set to ST_SESSION_WAIT_RESTART, and after the UA response is received,
            // the reconnection process is performed.
            session->sessionState = ST_SESSION_WAIT_RESTART;
            break;
        default:
            break;
    }

    return ret;
}

int RfcommRecvReqConnectRslt(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommReqConnectRslt connectRslt;
    (void)memcpy_s(&connectRslt, sizeof(RfcommReqConnectRslt), data, sizeof(RfcommReqConnectRslt));

    if (session->sessionState != ST_SESSION_WAIT_CONNECT_REQ_RESULT) {
        return RFCOMM_SUCCESS;
    }

    LOG_INFO("%{public}s pendingL2capId :%hu.", __func__, session->pendingL2capId);
    if (session->pendingL2capId != 0) {
        RfcommStopSessionTimer(session);
        if ((connectRslt.result == RFCOMM_SUCCESS) && (connectRslt.lcid != 0)) {
            RfcommSendDisconnectReq(connectRslt.lcid);
        }
        // Accept peer's connection request.
        return RfcommAcceptPeerConnectReq(session);
    }

    if ((connectRslt.result != RFCOMM_SUCCESS) || (connectRslt.lcid == 0)) {
        // Notify all channels on the session that connection failed.
        RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
        // Remove all channels on session.
        RfcommRemoveAllChannelOnSession(session);
        // Destroy session from session list.
        RfcommRemoveSession(session);
    } else {
        session->l2capId = connectRslt.lcid;
        session->sessionState = ST_SESSION_WAIT_CONNECT_RSP;
    }

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving connect response from L2CAP.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvConnectRsp(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    uint16_t result = *(uint16_t *)data;
    L2capConfigInfo config;
    (void)memset_s(&config, sizeof(config), 0, sizeof(config));

    if (session->sessionState != ST_SESSION_WAIT_CONNECT_RSP) {
        LOG_DEBUG("%{public}s:Session state is not WAIT_CONNECT_RSP.", __func__);
        return RFCOMM_FAILED;
    }

    LOG_INFO("%{public}s pendingL2capId:%hu.", __func__, session->pendingL2capId);
    if (result != L2CAP_CONNECTION_SUCCESSFUL) {
        if (session->pendingL2capId != 0) {
            RfcommStopSessionTimer(session);
            // Accept peer's connection request.
            return RfcommAcceptPeerConnectReq(session);
        }
        // Notify all channels on the session that connection failed.
        RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
        // Remove all channels on session.
        RfcommRemoveAllChannelOnSession(session);
        // Destroy session.
        RfcommRemoveSession(session);

        return RFCOMM_SUCCESS;
    }

    if (session->pendingL2capId != 0) {
        RfcommStopSessionTimer(session);
        RfcommSendConnectRsp(session->pendingL2capId, session->pendingId, L2CAP_NO_RESOURCES_AVAILABLE, 0);
        session->pendingL2capId = 0;
        session->pendingId = 0;
    }

    config.rfc.mode = 0;  // Basic mode
    config.flushTimeout = 0xFFFF;
    config.mtu = session->l2capLocalMtu;
    config.fcs = 0x01;

    session->sessionState = ST_SESSION_INITIATOR_WAIT_CONFIG_REQ_AND_RSP;

    return RfcommSendConfigReq(session->l2capId, &config);
}

/**
 * @brief Processing after receiving config response from L2CAP.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvConfigRsp(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    int ret = 0;
    RfcommConfigRspInfo *configResult = (RfcommConfigRspInfo *)data;

    if (configResult->result != L2CAP_SUCCESS) {
        RfcommSendDisconnectReq(session->l2capId);
        // Notify all channels on the session that connection failed.
        RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
        // Remove all channels on session.
        RfcommRemoveAllChannelOnSession(session);
        // Destroy session.
        RfcommRemoveSession(session);

        return RFCOMM_SUCCESS;
    }

    switch (session->sessionState) {
        case ST_SESSION_INITIATOR_WAIT_CONFIG_REQ_AND_RSP:
            session->sessionState = ST_SESSION_INITIATOR_WAIT_CONFIG_REQ;
            break;
        case ST_SESSION_INITIATOR_WAIT_CONFIG_RSP:
            ret = RfcommSendSabm(session, CONTROL_DLCI);
            session->sessionState = ST_SESSION_SABM0_REQ_WAIT_UA0;
            RfcommStartSessionTimer(session, T1_SABM_DISC);
            break;
        case ST_SESSION_RESPONDER_WAIT_CONFIG_REQ_AND_RSP:
            session->sessionState = ST_SESSION_RESPONDER_WAIT_CONFIG_REQ;
            break;
        case ST_SESSION_RESPONDER_WAIT_CONFIG_RSP:
            session->sessionState = ST_SESSION_WAIT_SABM0;
            break;
        default:
            break;
    }

    return ret;
}

/**
 * @brief Processing after receiving config request from L2CAP.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvConfigReq(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommConfigReqInfo reqInfo;
    (void)memcpy_s(&reqInfo, sizeof(RfcommConfigReqInfo), (RfcommConfigReqInfo *)data, sizeof(RfcommConfigReqInfo));

    session->l2capPeerMtu = reqInfo.cfg.mtu;

    if (reqInfo.cfg.rfc.mode != 0) {
        LOG_DEBUG("%{public}s mode is not basic mode, reconfig.", __func__);

        (void)memset_s(&reqInfo.cfg, sizeof(L2capConfigInfo), 0x00, sizeof(L2capConfigInfo));
        reqInfo.cfg.rfc.mode = 0;
        reqInfo.cfg.flushTimeout = 0xFFFF;
        reqInfo.cfg.fcs = 0x01;
        reqInfo.cfg.mtu = session->l2capPeerMtu;

        return RfcommSendConfigRsp(session->l2capId, reqInfo.id, &reqInfo.cfg, L2CAP_UNACCEPTABLE_PARAMETERS);
    }

    int ret = RfcommSendConfigRsp(session->l2capId, reqInfo.id, &reqInfo.cfg, 0);

    switch (session->sessionState) {
        case ST_SESSION_INITIATOR_WAIT_CONFIG_REQ_AND_RSP:
            session->sessionState = ST_SESSION_INITIATOR_WAIT_CONFIG_RSP;
            break;
        case ST_SESSION_INITIATOR_WAIT_CONFIG_REQ:
            ret = RfcommSendSabm(session, CONTROL_DLCI);
            session->sessionState = ST_SESSION_SABM0_REQ_WAIT_UA0;
            RfcommStartSessionTimer(session, T1_SABM_DISC);
            break;
        case ST_SESSION_RESPONDER_WAIT_CONFIG_REQ_AND_RSP:
            session->sessionState = ST_SESSION_RESPONDER_WAIT_CONFIG_RSP;
            break;
        case ST_SESSION_RESPONDER_WAIT_CONFIG_REQ:
            session->sessionState = ST_SESSION_WAIT_SABM0;
            break;
        default:
            break;
    }
    return ret;
}

/**
 * @brief Processing after receiving UA(DLCI0) response from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvUa0(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;
    int ret = 0;

    switch (session->sessionState) {
        case ST_SESSION_SABM0_REQ_WAIT_UA0:
            session->sessionState = ST_SESSION_CONNECTED;
            // Stop timer.
            RfcommStopSessionTimer(session);
            // After the session is successfully connected,
            // connect all links under the session.
            RfcommOpenAllChannelOnSession(session);
            break;
        case ST_SESSION_DISC0_REQ_WAIT_UA0:
            // Stop timer.
            RfcommStopSessionTimer(session);
            // L2CAP disconnect.
            ret = RfcommSendDisconnectReq(session->l2capId);
            // Remove all channels on session.
            RfcommRemoveAllChannelOnSession(session);
            // Destroy session.
            RfcommRemoveSession(session);
            break;
        case ST_SESSION_WAIT_RESTART:
            // Stop timer.
            RfcommStopSessionTimer(session);
            // L2CAP disconnect.
            RfcommSendDisconnectReq(session->l2capId);
            // Restart the session, init the session information first.
            ret = RfcommRestartSession(session);
            break;
        default:
            break;
    }
    return ret;
}

/**
 * @brief Processing after receiving connect request from L2CAP.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvConnectReq(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    int ret = 0;
    RfcommRecvConnectedInfo connectedInfo;
    (void)memcpy_s(&connectedInfo, sizeof(RfcommRecvConnectedInfo), data, sizeof(RfcommRecvConnectedInfo));

    switch (session->sessionState) {
        case ST_SESSION_INITIATOR_WAIT_SECURITY_RESULT:
            session->id = connectedInfo.id;
            session->l2capId = connectedInfo.lcid;
            session->isInitiator = false;
            RfcommUpdateChannelDirectionBit(session, false);
            // fall-through
        case ST_SESSION_CLOSED:
            session->sessionState = ST_SESSION_RESPONDER_WAIT_SECURITY_RESULT;
            ret = RfcommCheckSessionSecurity(session, 0, true);
            break;
        case ST_SESSION_WAIT_CONNECT_REQ_RESULT:
            // fall-through
        case ST_SESSION_WAIT_CONNECT_RSP:
            session->pendingL2capId = connectedInfo.lcid;
            session->pendingId = connectedInfo.id;
            uint32_t randomNum = RandomGenerate();
            uint8_t timer = randomNum % RFCOMM_NUM_TEN;
            RfcommStartSessionTimer(session, timer);
            break;
        default:
            ret = RfcommSendConnectRsp(session->l2capId, session->id, L2CAP_NO_RESOURCES_AVAILABLE, 0);
            break;
    }

    return ret;
}

/**
 * @brief Processing after receiving SABM(DLCI0) request from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvSabm0(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;

    if (session->sessionState != ST_SESSION_WAIT_SABM0) {
        LOG_DEBUG("%{public}s:Session state is not WAIT_SABM0.", __func__);
        return RFCOMM_FAILED;
    }

    int ret = RfcommSendUa(session, CONTROL_DLCI);
    session->sessionState = ST_SESSION_CONNECTED;
    // After the session is successfully connected,
    // connect all links under the session.
    RfcommOpenAllChannelOnSession(session);
    return ret;
}

/**
 * @brief Processing after receiving DISC(DLCI0) request from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvDisc0(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;
    int ret;
    bool sessionValid = false;

    switch (session->sessionState) {
        case ST_SESSION_SABM0_REQ_WAIT_UA0:
            session->sessionState = ST_SESSION_CLOSING;
            ret = RfcommSendUa(session, CONTROL_DLCI);
            // Notify all channels on the session that connection failed.
            RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
            // Remove all channels on session.
            RfcommRemoveAllChannelOnSession(session);
            break;
        case ST_SESSION_DISC0_REQ_WAIT_UA0:
            ret = RfcommSendUa(session, CONTROL_DLCI);
            break;
        case ST_SESSION_CONNECTED:
            session->sessionState = ST_SESSION_CLOSING;
            ret = RfcommSendUa(session, CONTROL_DLCI);
            // Delete all channels on the session(except the channel to be connected).
            RfcommRemoveInvalidChannelOnSession(session);
            // Check if the session is valid.
            sessionValid = RfcommCheckSessionValid(session);
            if (sessionValid) {
                // If there are channels to be connected, set RESTART of session's state.
                session->sessionState = ST_SESSION_WAIT_RESTART;
                RfcommResetAllChannelOnSession(session);
            }
            break;
        default:
            ret = RfcommSendDm(session, CONTROL_DLCI, true);
            break;
    }

    return ret;
}

/**
 * @brief Processing after receiving link loss notification from L2CAP.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvLinkLoss(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    uint8_t reason = *(uint8_t *)data;

    if (reason == L2CAP_STATE_COLLISION) {
        LOG_DEBUG("%{public}s Connect request error.Retry!", __func__);
        return RfcommRestartSession(session);
    }

    switch (session->sessionState) {
        case ST_SESSION_CONNECTED:
            // Notify all channels on the session that disconnection.
            RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_DISCONNECTED);
            break;
        default:
            // Stop timer.
            RfcommStopSessionTimer(session);
            // Notify all channels on the session that connection failed.
            RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
            break;
    }
    // Remove all channels on session.
    RfcommRemoveAllChannelOnSession(session);
    // Destroy session.
    RfcommRemoveSession(session);

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving the request to disconnect session from L2CAP.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvDisconnectReq(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;

    if (session->sessionState == ST_SESSION_WAIT_RESTART) {
        // Restart the session, init the session information first.
        return RfcommRestartSession(session);
    }

    // Notify all channels on the session that disconnection.
    RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_DISCONNECTED);
    // Remove all channels on session.
    RfcommRemoveAllChannelOnSession(session);
    // Destroy session.
    RfcommRemoveSession(session);

    return RFCOMM_SUCCESS;
}

/**
 * @brief The processing after receiving the request to close the session.
 *        When all channels on the session are disconnected, perform this process.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommCloseSession(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;
    int ret = 0;

    switch (session->sessionState) {
        case ST_SESSION_WAIT_CONNECT_RSP:
        // fall-through
        case ST_SESSION_INITIATOR_WAIT_CONFIG_REQ_AND_RSP:
        // fall-through
        case ST_SESSION_INITIATOR_WAIT_CONFIG_REQ:
        // fall-through
        case ST_SESSION_INITIATOR_WAIT_CONFIG_RSP:
        // fall-through
        case ST_SESSION_SABM0_REQ_WAIT_UA0:
            // Stop timer.
            RfcommStopSessionTimer(session);
            ret = RfcommSendDisconnectReq(session->l2capId);
            // Remove all channels on session.
            RfcommRemoveAllChannelOnSession(session);
            // Destroy session.
            RfcommRemoveSession(session);
            break;
        case ST_SESSION_CONNECTED:
            ret = RfcommSendDisc(session, CONTROL_DLCI);
            session->sessionState = ST_SESSION_DISC0_REQ_WAIT_UA0;
            RfcommStartSessionTimer(session, T1_SABM_DISC);
            break;
        default:
            break;
    }
    return ret;
}

/**
 * @brief The processing after receiving the DM(DLCI0) response from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvDm0(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;
    int ret = 0;

    switch (session->sessionState) {
        case ST_SESSION_SABM0_REQ_WAIT_UA0:
            // Stop timer.
            RfcommStopSessionTimer(session);
            ret = RfcommSendDisconnectReq(session->l2capId);
            // Notify all channels on the session that connection failed.
            RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
            break;
        case ST_SESSION_DISC0_REQ_WAIT_UA0:
            // Stop timer.
            RfcommStopSessionTimer(session);
            ret = RfcommSendDisconnectReq(session->l2capId);
            break;
        case ST_SESSION_WAIT_RESTART:
            // Stop timer.
            RfcommStopSessionTimer(session);
            // L2CAP disconnect.
            RfcommSendDisconnectReq(session->l2capId);
            // Restart the session, init the session information first.
            return RfcommRestartSession(session);
        default:
            break;
    }

    // Remove all channels on session.
    RfcommRemoveAllChannelOnSession(session);
    // Destroy session.
    RfcommRemoveSession(session);

    return ret;
}

/**
 * @brief The processing after receiving timeout notification.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvTimeOut(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;
    int ret = 0;

    switch (session->sessionState) {
        case ST_SESSION_SABM0_REQ_WAIT_UA0:
            ret = RfcommSendDisconnectReq(session->l2capId);
            // Notify all channels on the session that connection failed.
            RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
            // Remove all channels on session.
            RfcommRemoveAllChannelOnSession(session);
            // Destroy session.
            RfcommRemoveSession(session);
            break;
        case ST_SESSION_WAIT_SABM0:
            break;
        case ST_SESSION_DISC0_REQ_WAIT_UA0:
            ret = RfcommSendDisconnectReq(session->l2capId);
            // Remove all channels on session.
            RfcommRemoveAllChannelOnSession(session);
            // Destroy session.
            RfcommRemoveSession(session);
            break;
        case ST_SESSION_WAIT_RESTART:
            // L2CAP disconnect.
            RfcommSendDisconnectReq(session->l2capId);
            // Restart the session, init the session information first.
            ret = RfcommRestartSession(session);
            break;
        case ST_SESSION_CONNECTED:
            // Notify all channels that DLC is disconnected.
            RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_DISCONNECTED);
            // Remove all channels on session.
            RfcommRemoveAllChannelOnSession(session);
            // The default action performed on timeouts is to close down the multiplexer session.
            ret = RfcommSendDisc(session, CONTROL_DLCI);
            session->sessionState = ST_SESSION_DISC0_REQ_WAIT_UA0;
            RfcommStartSessionTimer(session, T1_SABM_DISC);
            break;
        case ST_SESSION_WAIT_CONNECT_RSP:
            if (session->pendingL2capId != 0) {
                // L2CAP disconnect.
                RfcommSendDisconnectReq(session->l2capId);
                // Accept peer's connection request.
                ret = RfcommAcceptPeerConnectReq(session);
            }
            break;
        default:
            break;
    }

    return ret;
}

/**
 * @brief The processing after receiving FCON request from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvFconReq(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;

    if (session->sessionState != ST_SESSION_CONNECTED) {
        LOG_DEBUG("%{public}s:Session state is not connected.", __func__);
        return RfcommSendDm(session, CONTROL_DLCI, false);
    }
    // Send FCon response.
    int ret = RfcommSendUihFcon(session, false);

    // When credit based flow control is being used on a session,
    // the FCon and FCoff multiplexer control commands shall not be used.
    if (session->fcType == FC_TYPE_CREDIT) {
        LOG_DEBUG("%{public}s:Credit based fc is used, ignore FC-bit.", __func__);
        return ret;
    }
    // Set the flow control status, and send buffered data on all channels.
    if (session->peerSessionFc) {
        session->peerSessionFc = false;
        RfcommSendAllCachePktOnSession(session);
    }

    return ret;
}

/**
 * @brief The processing after receiving FCON response from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvFconRsp(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    (void)session;
    (void)data;

    return RFCOMM_SUCCESS;
}

/**
 * @brief The processing after receiving FCOFF request from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvFcoffReq(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    (void)data;

    if (session->sessionState != ST_SESSION_CONNECTED) {
        LOG_DEBUG("%{public}s:Session state is not connected.", __func__);
        return RfcommSendDm(session, CONTROL_DLCI, false);
    }

    // Send FCon response.
    int ret = RfcommSendUihFcoff(session, false);

    // When credit based flow control is being used on a session,
    // the FCon and FCoff multiplexer control commands shall not be used.
    if (session->fcType == FC_TYPE_CREDIT) {
        LOG_DEBUG("%{public}s:Credit based fc is used, ignore FC-bit.", __func__);
        return ret;
    }
    // Set the flow control status.
    session->peerSessionFc = true;

    return ret;
}

/**
 * @brief The processing after receiving FCOFF response from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvFcoffRsp(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    (void)session;
    (void)data;

    return RFCOMM_SUCCESS;
}

/**
 * @brief The processing after receiving test request from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvTestReq(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommUihInfo info;
    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));

    return RfcommSendUihTest(session, false, info.test.pkt);
}

/**
 * @brief The processing after receiving test response from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvTestRsp(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    (void)session;
    (void)data;

    return RFCOMM_SUCCESS;
}

/**
 * @brief The processing after receiving NSC response from peer.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvNsc(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommUihInfo info;
    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));

    LOG_DEBUG("%{public}s:ea is %hhu.", __func__, info.nsc.ea);
    LOG_DEBUG("%{public}s:cr is %hhu.", __func__, info.nsc.cr);
    LOG_DEBUG("%{public}s:type is %hhu.", __func__, info.nsc.type);

    return RFCOMM_SUCCESS;
}

/**
 * @brief The processing after receiving the result of session's security.
 *
 * @param session The pointer of the session in the session list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvSessionSecurityRslt(RfcommSessionInfo *session, const void *data)
{
    LOG_INFO("%{public}s session state is:%{public}d.", __func__, session->sessionState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    int ret = 0;
    RfcommSessionSecurityRslt securityRslt;
    L2capConfigInfo config;
    (void)memset_s(&config, sizeof(config), 0, sizeof(config));
    (void)memcpy_s(&securityRslt, sizeof(RfcommSessionSecurityRslt), data, sizeof(RfcommSessionSecurityRslt));

    switch (session->sessionState) {
        case ST_SESSION_INITIATOR_WAIT_SECURITY_RESULT:
            if (securityRslt.direction != OUTGOING) {
                LOG_ERROR("%{public}s Direction is error:%{public}d.", __func__, securityRslt.direction);
                break;
            }
            if (securityRslt.result != GAP_SUCCESS) {
                // Notify all channels on the session that connection failed.
                RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
                // Remove all channels on session.
                RfcommRemoveAllChannelOnSession(session);
                // Destroy session from session list.
                RfcommRemoveSession(session);
                break;
            }
            session->sessionState = ST_SESSION_WAIT_CONNECT_REQ_RESULT;
            ret = RfcommSendConnectReq(&(session->btAddr));
            break;
        case ST_SESSION_RESPONDER_WAIT_SECURITY_RESULT:
            if (securityRslt.direction != INCOMING) {
                LOG_ERROR("%{public}s Direction is error:%{public}d.", __func__, securityRslt.direction);
                break;
            }
            if (securityRslt.result != GAP_SUCCESS) {
                RfcommSendConnectRsp(session->l2capId, session->id, L2CAP_NO_RESOURCES_AVAILABLE, 0);
                // Notify all channels on the session that connection failed.
                RfcommNotifyAllChannelEvtOnSession(session, RFCOMM_CHANNEL_EV_CONNECT_FAIL);
                // Remove all channels on session.
                RfcommRemoveAllChannelOnSession(session);
                // Destroy session.
                RfcommRemoveSession(session);
                break;
            }
            RfcommSendConnectRsp(session->l2capId, session->id, 0, 0);

            config.rfc.mode = 0;  // Basic mode
            config.flushTimeout = 0xFFFF;
            config.mtu = session->l2capLocalMtu;
            config.fcs = 0x01;

            session->sessionState = ST_SESSION_RESPONDER_WAIT_CONFIG_REQ_AND_RSP;
            ret = RfcommSendConfigReq(session->l2capId, &config);
            break;
        default:
            break;
    }
    return ret;
}

/**
 * @brief Restart the session.When a connection request is received during the disconnection process,
 *        the connection is recreated after disconnection.
 *
 * @param session The pointer of the session in the session list.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRestartSession(RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommResetSessionInfo(session);
    session->isInitiator = true;
    RfcommUpdateChannelDirectionBit(session, true);
    // Start to reopen the session.
    session->sessionState = ST_SESSION_INITIATOR_WAIT_SECURITY_RESULT;
    RfcommChannelInfo *channel = RfcommGetFirstChannelOnSession(session);
    if (channel == NULL) {
        LOG_ERROR("%{public}s No channel on the session.", __func__);
        return RFCOMM_SUCCESS;
    }

    return RfcommCheckSessionSecurity(session, channel->scn, false);
}

/**
 * @brief Accept the connection request from the peer device.
 *
 * @param session The pointer of the session in the session list.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommAcceptPeerConnectReq(RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    session->l2capId = session->pendingL2capId;
    session->id = session->pendingId;
    session->isInitiator = false;
    RfcommUpdateChannelDirectionBit(session, false);
    session->pendingL2capId = 0;
    session->pendingId = 0;

    session->sessionState = ST_SESSION_RESPONDER_WAIT_SECURITY_RESULT;
    return RfcommCheckSessionSecurity(session, 0, true);
}