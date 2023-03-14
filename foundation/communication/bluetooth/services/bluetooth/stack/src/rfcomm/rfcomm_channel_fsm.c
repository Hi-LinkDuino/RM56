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

typedef int (*ChannelActionFunc)(RfcommChannelInfo *channel, const void *data);
typedef struct {
    RfcommChannelEvent eventId;
    ChannelActionFunc fn;
} RfcommChannelEvtAction;

static int RfcommOpenChannel(RfcommChannelInfo *channel, const void *data);
static int RfcommCloseChannel(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvSecurityRslt(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvSabm(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvDisc(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvUa(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvDm(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvPnReq(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvPnRsp(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvRpnCmd(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvRpnReq(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvRpnRsp(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvMscReq(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvMscRsp(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvRlsReq(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvRlsRsp(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvAcceptRsp(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvRejectRsp(RfcommChannelInfo *channel, const void *data);
static int RfcommRecvRevData(RfcommChannelInfo *channel, const void *data);
static int RfcommSendRpnCmd(RfcommChannelInfo *channel, const void *data);
static int RfcommSendRpnReq(RfcommChannelInfo *channel, const void *data);
static int RfcommSendRlsReq(RfcommChannelInfo *channel, const void *data);
static int RfcommSendMscReq(RfcommChannelInfo *channel, const void *data);
static int RfcommWriteData(RfcommChannelInfo *channel, const void *data);
static int RfcommTimeOut(RfcommChannelInfo *channel, const void *data);
static void RfcommSetUihPnParameter(
    RfcommFlowControlType fcType, uint8_t reqCl, RfcommPnCmdKind cmdKind, uint8_t *cl, uint8_t *credits);

static RfcommChannelEvtAction g_channelEvtActTbl[EV_CHANNEL_EV_MAX] = {
    {EV_CHANNEL_SEND_OPEN_REQ, RfcommOpenChannel},
    {EV_CHANNEL_SEND_CLOSE_REQ, RfcommCloseChannel},
    {EV_CHANNEL_RECV_SECURITY_RESULT, RfcommRecvSecurityRslt},
    {EV_CHANNEL_RECV_SABM, RfcommRecvSabm},
    {EV_CHANNEL_RECV_DISC, RfcommRecvDisc},
    {EV_CHANNEL_RECV_UA, RfcommRecvUa},
    {EV_CHANNEL_RECV_DM, RfcommRecvDm},
    {EV_CHANNEL_RECV_PN_REQ, RfcommRecvPnReq},
    {EV_CHANNEL_RECV_PN_RSP, RfcommRecvPnRsp},
    {EV_CHANNEL_RECV_MSC_REQ, RfcommRecvMscReq},
    {EV_CHANNEL_RECV_MSC_RSP, RfcommRecvMscRsp},
    {EV_CHANNEL_RECV_RPN_CMD, RfcommRecvRpnCmd},
    {EV_CHANNEL_RECV_RPN_REQ, RfcommRecvRpnReq},
    {EV_CHANNEL_RECV_RPN_RSP, RfcommRecvRpnRsp},
    {EV_CHANNEL_RECV_RLS_REQ, RfcommRecvRlsReq},
    {EV_CHANNEL_RECV_RLS_RSP, RfcommRecvRlsRsp},
    {EV_CHANNEL_ACCEPT, RfcommRecvAcceptRsp},
    {EV_CHANNEL_REJECT, RfcommRecvRejectRsp},
    {EV_CHANNEL_RECV_DATA, RfcommRecvRevData},
    {EV_CHANNEL_WRITE_DATA, RfcommWriteData},
    {EV_CHANNEL_SEND_RPN_CMD, RfcommSendRpnCmd},
    {EV_CHANNEL_SEND_RPN_REQ, RfcommSendRpnReq},
    {EV_CHANNEL_SEND_RLS_REQ, RfcommSendRlsReq},
    {EV_CHANNEL_SEND_MSC_REQ, RfcommSendMscReq},
    {EV_CHANNEL_TIMEOUT, RfcommTimeOut}
};

/**
 * @brief State machine for handling events related to data link connection.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param event   The event id.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommChannelEvtFsm(RfcommChannelInfo *channel, RfcommChannelEvent event, const void *data)
{
    LOG_INFO("%{public}s event:%{public}d", __func__, event);

    int ret = 0;

    for (int cnt = 0; cnt < EV_CHANNEL_EV_MAX; cnt++) {
        if (g_channelEvtActTbl[cnt].eventId == event) {
            ret = g_channelEvtActTbl[cnt].fn(channel, data);
            break;
        }
    }

    return ret;
}

/**
 * @brief Processing after receiving connect req event from upper.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommOpenChannel(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    int ret = 0;
    uint8_t credits = 0;
    uint8_t cl = 0;
    uint8_t scn = channel->scn;
    RfcommSessionInfo *session = channel->session;

    switch (channel->channelState) {
        case ST_CHANNEL_CLOSED:
            // If session is not connected, connect the session.
            if (session->sessionState != ST_SESSION_CONNECTED) {
                ret = RfcommSessionEvtFsm(session, EV_SESSION_SEND_OPEN_REQ, &scn);
                break;
            }
            // If session is connected, prepare to connect the DLC.Process PN negotiation firstly.
            RfcommSetUihPnParameter(session->fcType, 0, PN_REQ, &cl, &credits);
            channel->localCredit = credits;

            RfcommSendPnInfo pnInfo;
            pnInfo.cl = cl;
            pnInfo.credits = credits;
            pnInfo.mtu = channel->localMtu;
            pnInfo.priority = 0;
            ret = RfcommSendUihPn(session, channel->dlci, true, &pnInfo);
            channel->channelState = ST_CHANNEL_WAIT_PN_RSP;
            RfcommStartChannelTimer(channel, T2_UIH_DLCI0);
            break;
        case ST_CHANNEL_DISC_REQ_WAIT_UA:
            channel->channelState = ST_CHANNEL_WAIT_RESTART;
            break;
        case ST_CHANNEL_CONNECTED:
            LOG_DEBUG("%{public}s:DLC is connected.", __func__);
            break;
        default:
            break;
    }

    return ret;
}

/**
 * @brief Processing after receiving the security check result from GAP.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvSecurityRslt(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    int ret = 0;
    uint16_t result = *(uint16_t *)data;
    RfcommSessionInfo *session = channel->session;
    RfcommConnectedInfo connectedInfo;

    switch (channel->channelState) {
        case ST_CHANNEL_CLIENT_WAIT_SECURITY_RESULT:
            if (result != GAP_SUCCESS) {
                channel->channelState = ST_CHANNEL_CLOSED;
                RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_FAIL, NULL);
                // Release channelinfo.
                RfcommRemoveChannel(channel);
                // If the last channel on the session is closed, close the session.
                RfcommCloseInvalidSession(session);
                break;
            }
            ret = RfcommSendSabm(session, channel->dlci);
            channel->channelState = ST_CHANNEL_SABM_REQ_WAIT_UA;
            RfcommStartChannelTimer(channel, T1_SABM_OPEN_DLC);
            break;
        case ST_CHANNEL_SERVER_WAIT_SECURITY_RESULT:
            // If the security check result is not successful,
            // then return a connection rejection response to the peer.
            if (result != GAP_SUCCESS) {
                ret = RfcommSendDm(session, channel->dlci, true);
                channel->channelState = ST_CHANNEL_CLOSED;
                RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_FAIL, NULL);
                // Release channelinfo.
                RfcommRemoveChannel(channel);
                // If the last channel on the session is closed, close the session.
                RfcommCloseInvalidSession(session);
                break;
            }
            // If the result of the security check is successful,
            // the connection acceptance response is returned to the peer.
            ret = RfcommSendUa(session, channel->dlci);
            channel->channelState = ST_CHANNEL_CONNECTED;
            // After DLC is established, determine the mtu size of the opposite end.
            RfcommDeterminePeerMtu(channel);
            (void)memset_s(&connectedInfo, sizeof(RfcommConnectedInfo), 0x00, sizeof(RfcommConnectedInfo));
            connectedInfo.sendMTU = channel->peerMtu;
            connectedInfo.recvMTU = channel->localMtu;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_SUCCESS, &connectedInfo);
            break;
        default:
            break;
    }

    return ret;
}

/**
 * @brief Processing after receiving the UA response from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvUa(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    int ret = 0;
    uint8_t credits = 0;
    uint8_t cl = 0;
    RfcommSessionInfo *session = channel->session;
    RfcommConnectedInfo connectedInfo;
    RfcommModemStatusInfo modemStatus;
    (void)memset_s(&modemStatus, sizeof(modemStatus), 0x00, sizeof(modemStatus));

    switch (channel->channelState) {
        case ST_CHANNEL_SABM_REQ_WAIT_UA:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            channel->channelState = ST_CHANNEL_CONNECTED;
            // After DLC is established, determine the mtu size of the opposite end.
            RfcommDeterminePeerMtu(channel);
            // Notify the upper layer that the connection is successful.
            (void)memset_s(&connectedInfo, sizeof(RfcommConnectedInfo), 0x00, sizeof(RfcommConnectedInfo));
            connectedInfo.sendMTU = channel->peerMtu;
            connectedInfo.recvMTU = channel->localMtu;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_SUCCESS, &connectedInfo);
            // Send MSC command.
            // In any case MSC commands and responses must be exchanged before
            // the data transfer may start.Ref-RFCOMM_SPEC_V12(6.3)
            modemStatus.signals = MSC_RTC | MSC_RTR | MSC_DV;
            ret = RfcommSendUihMsc(session, channel->dlci, true, &modemStatus);
            channel->transferReady |= MSC_CMD_SEND;
            // Start timer.
            RfcommStartChannelTimer(channel, T2_UIH_DLCI0);
            break;
        case ST_CHANNEL_DISC_REQ_WAIT_UA:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            channel->channelState = ST_CHANNEL_CLOSED;
            // Notify the upper layer that the disconnection is successful.
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS, NULL);
            // Release channelinfo.
            RfcommRemoveChannel(channel);
            // If the last channel on the session is closed, close the session.
            RfcommCloseInvalidSession(session);
            break;
        case ST_CHANNEL_WAIT_RESTART:
            RfcommResetChannelInfo(channel);
            // If session is connected, prepare to connect the DLC.Process PN negotiation first.
            RfcommSetUihPnParameter(session->fcType, 0, PN_REQ, &cl, &credits);
            channel->localCredit = credits;

            RfcommSendPnInfo pnInfo;
            pnInfo.cl = cl;
            pnInfo.credits = credits;
            pnInfo.mtu = channel->localMtu;
            pnInfo.priority = 0;
            ret = RfcommSendUihPn(session, channel->dlci, true, &pnInfo);
            channel->channelState = ST_CHANNEL_WAIT_PN_RSP;
            RfcommStartChannelTimer(channel, T2_UIH_DLCI0);
            break;
        default:
            break;
    }

    return ret;
}

/**
 * @brief Processing after receiving the DM response from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvDm(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    RfcommSessionInfo *session = channel->session;

    if (channel->channelState == ST_CHANNEL_CLOSED) {
        LOG_DEBUG("%{public}s Channel is restarting.", __func__);
        return RFCOMM_SUCCESS;
    }

    switch (channel->channelState) {
        case ST_CHANNEL_WAIT_PN_RSP:
        // fall-through
        case ST_CHANNEL_SABM_REQ_WAIT_UA:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            channel->channelState = ST_CHANNEL_CLOSED;
            // Notify the upper layer that the connection is fail.
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_FAIL, NULL);
            break;
        case ST_CHANNEL_DISC_REQ_WAIT_UA:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            channel->channelState = ST_CHANNEL_CLOSED;
            // Notify the upper layer that the disconnection is successful.
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS, NULL);
            break;
        case ST_CHANNEL_CONNECTED:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            channel->channelState = ST_CHANNEL_CLOSED;
            // Notify the upper layer that the channel is disconnected.
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECTED, NULL);
            break;
        default:
            break;
    }

    // Release channelinfo.
    RfcommRemoveChannel(channel);
    // If the last channel on the session is closed, close the session.
    RfcommCloseInvalidSession(session);

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving the RPN commad from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvRpnCmd(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommUihInfo info;
    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));

    channel->portConfig.baudrate = info.rpn.rpnInfo.baudrate;
    channel->portConfig.data_bits = info.rpn.rpnInfo.data_bits;
    channel->portConfig.fc = info.rpn.rpnInfo.fc;
    channel->portConfig.parity = info.rpn.rpnInfo.parity;
    channel->portConfig.parity_type = info.rpn.rpnInfo.parity_type;
    channel->portConfig.stop_bit = info.rpn.rpnInfo.stop_bit;
    channel->portConfig.xoff_char = info.rpn.rpnInfo.xoff_char;
    channel->portConfig.xon_char = info.rpn.rpnInfo.xon_char;
    channel->portConfig.parameter_mask1 = info.rpn.rpnInfo.parameter_mask1;
    channel->portConfig.parameter_mask2 = info.rpn.rpnInfo.parameter_mask2;

    RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_REMOTE_PORT_CONFIG, &channel->portConfig);

    return RfcommSendUihRpn(channel->session, channel->dlci, false, &channel->portConfig);
}

/**
 * @brief Processing after receiving the RPN request from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvRpnReq(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    RfcommRemotePortConfig portConfig;

    (void)memcpy_s(&portConfig, sizeof(RfcommRemotePortConfig), &channel->portConfig, sizeof(RfcommRemotePortConfig));
    portConfig.parameter_mask1 = 0;
    portConfig.parameter_mask2 = 0;

    return RfcommSendUihRpn(channel->session, channel->dlci, false, &portConfig);
}

/**
 * @brief Processing after receiving the PN request from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvPnReq(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    int ret;
    uint8_t credits = 0;
    uint8_t cl = 0;
    RfcommSessionInfo *session = channel->session;
    RfcommUihInfo info;
    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));
    RfcommSendPnInfo pnInfo;
    (void)memset_s(&pnInfo, sizeof(RfcommSendPnInfo), 0x00, sizeof(RfcommSendPnInfo));

    switch (channel->channelState) {
        case ST_CHANNEL_CLOSED:
            // Set flow control type.
            RfcommSetFlowControlType(session, true, info.pn.cl);
            // If it is credit-based flow control, then set the initial credits of the peer.
            if ((session->fcType == FC_TYPE_CREDIT) && (info.pn.cl == CL_REQ_SUPPORT_CREDIT)) {
                channel->peerCredit = info.pn.k;
            }
            RfcommSetUihPnParameter(session->fcType, info.pn.cl, PN_RSP_BEFORE_CREATE, &cl, &credits);
            // For a PN command with N1 value of N1c (c for command),
            // a PN response shall have an N1 value N1r (r for response) where N1r <= N1c.
            channel->localMtu = (channel->localMtu < info.pn.mtu) ? channel->localMtu : info.pn.mtu;
            channel->peerMtu = channel->localMtu;
            // Send PN response to peer.
            channel->localCredit = credits;

            pnInfo.cl = cl;
            pnInfo.credits = credits;
            pnInfo.mtu = channel->localMtu;
            pnInfo.priority = info.pn.priority;
            ret = RfcommSendUihPn(session, channel->dlci, false, &pnInfo);
            // Waiting peer to send sabm request.
            channel->channelState = ST_CHANNEL_WAIT_SABM;
            break;
        case ST_CHANNEL_DISC_REQ_WAIT_UA:
            ret = RfcommSendDm(session, channel->dlci, false);
            break;
        default:
            // When DLC is connected or connecting, PN req is received, only return Pn rsp to Peer.
            RfcommSetUihPnParameter(session->fcType, info.pn.cl, PN_RSP_AFTER_CREATE, &cl, &credits);

            pnInfo.cl = cl;
            pnInfo.credits = credits;
            pnInfo.mtu = channel->localMtu;
            pnInfo.priority = info.pn.priority;
            // Send PN response to peer.
            ret = RfcommSendUihPn(session, channel->dlci, false, &pnInfo);
            break;
    }

    return ret;
}

/**
 * @brief Processing after receiving PN response from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvPnRsp(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommUihInfo info;
    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));

    if (channel->channelState != ST_CHANNEL_WAIT_PN_RSP) {
        LOG_DEBUG("%{public}s:State is not WAIT_RSP.", __func__);
        return RFCOMM_FAILED;
    }

    // Stop timer.
    RfcommStopChannelTimer(channel);

    // Set flow control type.
    RfcommSetFlowControlType(channel->session, false, info.pn.cl);
    // If flow control type is based credit, then set amount of credits issued to the peer.
    if (channel->session->fcType == FC_TYPE_CREDIT) {
        channel->peerCredit = info.pn.k;
    }
    // A device receiving a PN response may accept N1r and use this value as the
    // maximum frame data size.If this connection is established,
    // neither side may send a frame with more than N1r bytes of data
    channel->localMtu = (channel->localMtu < info.pn.mtu) ? channel->localMtu : info.pn.mtu;
    channel->peerMtu = channel->localMtu;

    // Perform security checks before initiating a connection request.
    channel->channelState = ST_CHANNEL_CLIENT_WAIT_SECURITY_RESULT;

    return RfcommCheckChannelSecurity(channel, false);
}

/**
 * @brief Processing after receiving the SABM request from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvSabm(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    RfcommIncomingInfo eventInfo;
    (void)memset_s(&eventInfo, sizeof(eventInfo), 0x00, sizeof(eventInfo));

    switch (channel->channelState) {
        case ST_CHANNEL_CLOSED:
        // fall-through
        case ST_CHANNEL_WAIT_SABM:
            (void)memcpy_s(&eventInfo.addr, sizeof(BtAddr), &(channel->session->btAddr), sizeof(BtAddr));
            eventInfo.scn = channel->dlci >> RFCOMM_DLCI_SHIFT_SCN;
            // After receiving the connection request from the peer, notify the request to the upper layer,
            // and wait for the upper layer to accept or reject the connection request response.
            channel->channelState = ST_CHANNEL_WAIT_UPPER_RESPONSE;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_INCOMING, &eventInfo);
            break;
        default:
            break;
    }

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving the DISC request from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvDisc(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    int ret = 0;
    RfcommSessionInfo *session = channel->session;

    switch (channel->channelState) {
        case ST_CHANNEL_CLOSED:
            ret = RfcommSendDm(session, channel->dlci, true);
            break;
        case ST_CHANNEL_WAIT_PN_RSP:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            ret = RfcommSendDm(session, channel->dlci, true);
            channel->channelState = ST_CHANNEL_CLOSED;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_FAIL, NULL);
            break;
        case ST_CHANNEL_SABM_REQ_WAIT_UA:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            ret = RfcommSendUa(session, channel->dlci);
            channel->channelState = ST_CHANNEL_CLOSED;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_FAIL, NULL);
            break;
        case ST_CHANNEL_DISC_REQ_WAIT_UA:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            ret = RfcommSendDm(session, channel->dlci, true);
            channel->channelState = ST_CHANNEL_CLOSED;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS, NULL);
            // Release channelinfo.
            RfcommRemoveChannel(channel);
            // If the last channel on the session is closed, close the session.
            RfcommCloseInvalidSession(session);
            return ret;
        case ST_CHANNEL_WAIT_UPPER_RESPONSE:
        // fall-through
        case ST_CHANNEL_SERVER_WAIT_SECURITY_RESULT:
        // fall-through
        case ST_CHANNEL_CONNECTED:
            ret = RfcommSendUa(session, channel->dlci);
            channel->channelState = ST_CHANNEL_CLOSED;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECTED, NULL);
            break;
        default:
            break;
    }

    // Release channel resource.
    RfcommRemoveChannel(channel);

    return ret;
}

/**
 * @brief Processing after receiving the acception response of the connection request from upper.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvAcceptRsp(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;

    if (channel->channelState != ST_CHANNEL_WAIT_UPPER_RESPONSE) {
        LOG_DEBUG("%{public}s:State is not WAIT_UPPER_RSP.", __func__);
        return RFCOMM_FAILED;
    }

    // Before returning a response to the connection request, perform security check processing.
    channel->channelState = ST_CHANNEL_SERVER_WAIT_SECURITY_RESULT;

    return RfcommCheckChannelSecurity(channel, true);
}

/**
 * @brief Processing after receiving the rejection response of the connection request from upper.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvRejectRsp(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    int ret = 0;
    RfcommSessionInfo *session = channel->session;

    if (channel->channelState != ST_CHANNEL_WAIT_UPPER_RESPONSE) {
        LOG_DEBUG("%{public}s:State is not WAIT_UPPER_RSP.", __func__);
        return ret;
    }
    ret = RfcommSendDm(session, channel->dlci, true);
    // Release channelinfo.
    RfcommRemoveChannel(channel);
    // If the last channel on the session is closed, close the session.
    RfcommCloseInvalidSession(session);

    return ret;
}

/**
 * @brief Processing after receiving the disconnect channel request from upper.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommCloseChannel(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    int ret = 0;
    RfcommSessionInfo *session = channel->session;

    switch (channel->channelState) {
        case ST_CHANNEL_WAIT_PN_RSP:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            // fall-through
        case ST_CHANNEL_CLOSED:
            // fall-through
        case ST_CHANNEL_CLIENT_WAIT_SECURITY_RESULT:
            channel->channelState = ST_CHANNEL_CLOSED;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS, NULL);
            // Release channelinfo.
            RfcommRemoveChannel(channel);
            // If the last channel on the session is closed, close the session.
            RfcommCloseInvalidSession(session);
            break;
        case ST_CHANNEL_WAIT_UPPER_RESPONSE:
        // fall-through
        case ST_CHANNEL_SERVER_WAIT_SECURITY_RESULT:
            ret = RfcommSendDm(session, channel->dlci, true);
            channel->channelState = ST_CHANNEL_CLOSED;
            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS, NULL);
            // Release channelinfo.
            RfcommRemoveChannel(channel);
            // If the last channel on the session is closed, close the session.
            RfcommCloseInvalidSession(session);
            break;
        case ST_CHANNEL_DISC_REQ_WAIT_UA:
            LOG_DEBUG("%{public}s:DLCI(%hhu) is Disconnecting.", __func__, channel->dlci);
            break;
        case ST_CHANNEL_SABM_REQ_WAIT_UA:
            // Stop timer.
            RfcommStopChannelTimer(channel);
            // fall-through
        case ST_CHANNEL_CONNECTED:
            ret = RfcommSendDisc(session, channel->dlci);
            channel->channelState = ST_CHANNEL_DISC_REQ_WAIT_UA;
            RfcommStartChannelTimer(channel, T1_SABM_DISC);
            break;
        default:
            break;
    }
    return ret;
}

/**
 * @brief Processing after receiving the timeout notification.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommTimeOut(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;
    uint32_t eventId = 0;
    RfcommSessionInfo *session = channel->session;

    switch (channel->channelState) {
        case ST_CHANNEL_WAIT_PN_RSP:
        // fall-through
        case ST_CHANNEL_SABM_REQ_WAIT_UA:
            eventId = RFCOMM_CHANNEL_EV_CONNECT_FAIL;
            break;
        case ST_CHANNEL_DISC_REQ_WAIT_UA:
            eventId = RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS;
            break;
        case ST_CHANNEL_CONNECTED:
            eventId = RFCOMM_CHANNEL_EV_DISCONNECTED;
            break;
        default:
            return RFCOMM_SUCCESS;
    }

    channel->channelState = ST_CHANNEL_CLOSED;
    RfcommNotifyEvtToUpper(channel, eventId, NULL);
    RfcommRemoveChannel(channel);

    return RfcommSessionEvtFsm(session, EV_SESSION_TIMEOUT, NULL);
}

/**
 * @brief Processing after receiving the MSC request from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvMscReq(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommUihInfo info;
    RfcommModemStatusInfo modemSts;
    (void)memset_s(&modemSts, sizeof(modemSts), 0x00, sizeof(modemSts));

    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));
    modemSts.signals = info.msc.signal;
    modemSts.breakSignal = info.msc.breakSignal;
    // Send MSC response to the peer.
    int ret = RfcommSendUihMsc(channel->session, channel->dlci, false, &modemSts);

    uint8_t oldSt = channel->transferReady;
    channel->transferReady |= MSC_CMD_RECV;
    LOG_DEBUG("%{public}s:transferReady State is (%hhu).", __func__, channel->transferReady);

    // Set peer's modem status.
    RfcommSetPeerModemStatus(channel, &modemSts);
    // If MSC cmd has not been sent, send it.
    // In any case MSC commands and responses must be exchanged before
    // the data transfer may start.Ref-RFCOMM_SPEC_V12(6.3)
    if (!(channel->transferReady & MSC_CMD_SEND)) {
        (void)memset_s(&modemSts, sizeof(modemSts), 0x00, sizeof(modemSts));
        modemSts.signals = MSC_RTC | MSC_RTR | MSC_DV;
        ret = RfcommSendUihMsc(channel->session, channel->dlci, true, &modemSts);
        channel->transferReady |= MSC_CMD_SEND;
        // Start timer.
        RfcommStartChannelTimer(channel, T2_UIH_DLCI0);
    } else if (oldSt == (MSC_CMD_SEND | MSC_RSP_RECV)) {
        // If the MSC commnd and response have been exchanged,
        // check whether there is data in the sending buffer queue,
        // and if there is buffered data, send the data.
        RfcommSendCachePkt(channel);
        RfcommSetFlcToUpper(channel);
    }

    return ret;
}

/**
 * @brief Processing after receiving the MSC response from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvMscRsp(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;

    if (channel->channelState != ST_CHANNEL_CONNECTED) {
        LOG_DEBUG("%{public}s:State is not connected.", __func__);
        return RFCOMM_ERR_NOT_CONNECTED;
    }

    // Stop timer.
    RfcommStopChannelTimer(channel);

    uint8_t oldSt = channel->transferReady;
    channel->transferReady |= MSC_RSP_RECV;

    LOG_DEBUG("%{public}s:transferReady State is (%hhu).", __func__, channel->transferReady);

    // If the MSC commnd and response have been exchanged,
    // check whether there is data in the sending buffer queue,
    // and if there is buffered data, send the data.
    if (oldSt == (MSC_CMD_SEND | MSC_CMD_RECV)) {
        RfcommSendCachePkt(channel);
        RfcommSetFlcToUpper(channel);
    }

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving the RPN response from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvRpnRsp(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;

    RfcommStopChannelTimer(channel);

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving the RLS request from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvRlsReq(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommUihInfo info;
    RfcommRemoteLineStatus lineStatus;
    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));
    (void)memset_s(&lineStatus, sizeof(RfcommRemoteLineStatus), 0x00, sizeof(RfcommRemoteLineStatus));

    lineStatus.overrunErr = info.rls.lineStatus >> RFCOMM_RLS_SHIFT_OVERRUN;
    lineStatus.parityErr = info.rls.lineStatus >> RFCOMM_RLS_SHIFT_PARITY;
    lineStatus.frameErr = info.rls.lineStatus >> RFCOMM_RLS_SHIFT_FRAMING;

    RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_REMOTE_LINE_STATUS, &lineStatus);

    return RfcommSendUihRls(channel->session, channel->dlci, false, info.rls.lineStatus);
}

/**
 * @brief Processing after receiving the RLS response from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvRlsRsp(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;

    RfcommStopChannelTimer(channel);

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving the transmit information from peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRecvRevData(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    Packet *pkt = NULL;
    RfcommUihInfo info;
    (void)memcpy_s(&info, sizeof(RfcommUihInfo), data, sizeof(RfcommUihInfo));

    if (channel->channelState != ST_CHANNEL_CONNECTED) {
        LOG_DEBUG("%{public}s Channel is not connected.", __func__);
        return RfcommSendDm(channel->session, channel->dlci, false);
    }

    if (info.data.size > 0) {
        RfcommReadLock();
        uint8_t count = (uint8_t)ListGetSize(channel->recvQueue);
        if (count < MAX_QUEUE_COUNT) {
            pkt = PacketRefMalloc(info.data.payload);
            ListAddLast(channel->recvQueue, pkt);
            RfcommReadUnlock();

            RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_REV_DATA, NULL);
            RfcommSetFlcToPeer(channel, false);
        } else {
            RfcommReadUnlock();
        }
    }

    if (info.data.credits > 0) {
        channel->peerCredit += info.data.credits;
        RfcommSendCachePkt(channel);
        RfcommSetFlcToUpper(channel);
    }

    return RFCOMM_SUCCESS;
}

/**
 * @brief Processing after receiving the write transmit information from upper.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommWriteData(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    uint8_t newCredits = 0;
    Packet *refpkt = NULL;
    RfcommSessionInfo *session = channel->session;

    // If it is currently in the process of disconnection, no data will be sent.
    if (channel->channelState == ST_CHANNEL_DISC_REQ_WAIT_UA || channel->channelState == ST_CHANNEL_CLOSED) {
        return RFCOMM_ERR_NOT_CONNECTED;
    }
    // Determine whether the peer can receive the data. If the peer cannot receive the data,
    // save the data in the queue to be sent.
    if (((session->fcType == FC_TYPE_CREDIT) && (channel->peerCredit == 0)) ||
        (channel->transferReady != TRANSFER_READY) || channel->peerChannelFc || (session->peerSessionFc)) {
        // Get send list's count
        uint8_t count = (uint8_t)ListGetSize(channel->sendQueue);
        if (count < MAX_QUEUE_COUNT) {
            refpkt = PacketRefMalloc((Packet *)data);
            ListAddLast(channel->sendQueue, (void *)refpkt);
            return RFCOMM_SUCCESS;
        }
        channel->localFcToUpper = true;
        return RFCOMM_QUEUE_FULL;
    }
    if (session->fcType == FC_TYPE_CREDIT) {
        // The value of the credit octet (0 - 255) signifies a number of frames,
        // for which the sender now has buffer space available to receive on the DLC.
        newCredits = channel->localCreditMax - channel->localCredit;
        channel->localCredit += newCredits;
    }

    // Add transmite data bytes value.
    channel->transmittedBytes += PacketPayloadSize((Packet *)data);
    // Send data to peer.
    int ret = RfcommSendUihData(session, channel->dlci, newCredits, (Packet *)data);
    // Decrease credit count.
    if ((session->fcType == FC_TYPE_CREDIT) && (ret == RFCOMM_SUCCESS)) {
        if (channel->peerCredit > 0) {
            channel->peerCredit--;
        }
    }
    return ret;
}

/**
 * @brief Send rpn command to peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendRpnCmd(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    int ret = RfcommSendUihRpn(channel->session, channel->dlci, true, (RfcommRemotePortConfig *)data);

    // Start timer.
    RfcommStartChannelTimer(channel, T2_UIH_DLCI0);

    return ret;
}

/**
 * @brief Send rpn request to peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendRpnReq(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s channel state is:%{public}d.", __func__, channel->channelState);

    (void)data;

    int ret = RfcommSendUihRpn(channel->session, channel->dlci, true, NULL);

    // Start timer.
    RfcommStartChannelTimer(channel, T2_UIH_DLCI0);

    return ret;
}

/**
 * @brief Send rls request to peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendRlsReq(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommRemoteLineStatus *lineStatus = (RfcommRemoteLineStatus *)data;

    uint8_t status = lineStatus->overrunErr ? OVERRUN_ERROR : 0;
    status |= lineStatus->parityErr ? PARITY_ERROR : 0;
    status |= lineStatus->frameErr ? FRAMING_ERROR : 0;

    if (status) {
        status = (status << 1) | 0x01;
    }

    int ret = RfcommSendUihRls(channel->session, channel->dlci, true, status);

    // Start timer.
    RfcommStartChannelTimer(channel, T2_UIH_DLCI0);

    return ret;
}

/**
 * @brief Send modem status request.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param data    Data related to the event.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendMscReq(RfcommChannelInfo *channel, const void *data)
{
    LOG_INFO("%{public}s", __func__);

    if (data == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    RfcommModemStatus *modemSts = (RfcommModemStatus *)data;
    RfcommModemStatusInfo status;
    (void)memset_s(&status, sizeof(RfcommModemStatusInfo), 0x00, sizeof(RfcommModemStatusInfo));

    status.signals = modemSts->fc ? MSC_FC : 0;
    status.signals |= modemSts->rtc ? MSC_RTC : 0;
    status.signals |= modemSts->rtr ? MSC_RTR : 0;
    status.signals |= modemSts->ic ? MSC_IC : 0;
    status.signals |= modemSts->dv ? MSC_DV : 0;

    status.breakSignal = modemSts->break_signal ? MSC_BREAK : 0;
    status.breakSignal |= modemSts->break_length << RFCOMM_MSC_SHIFT_BREAKLEN;

    int ret = RfcommSendUihMsc(channel->session, channel->dlci, true, &status);
    channel->transferReady |= MSC_CMD_SEND;

    // Start timer.
    RfcommStartChannelTimer(channel, T2_UIH_DLCI0);

    return ret;
}

/**
 * @brief Set UIH PN request or response parameter.
 *
 * @param fcType  Flow control type.
 * @param reqCl   The cl value of the pn request.
 * @param cmdKind Pn command kind.
 * @param cl      The cl value that will be sent to peer.
 * @param credits The initial credits that will be sent to peer.
 */
void RfcommSetUihPnParameter(
    RfcommFlowControlType fcType, uint8_t reqCl, RfcommPnCmdKind cmdKind, uint8_t *cl, uint8_t *credits)
{
    LOG_INFO("%{public}s fcType:%{public}d, req_cl:%hhu, cmdKind:%{public}d.", __func__, fcType, reqCl, cmdKind);

    switch (cmdKind) {
        case PN_REQ:
            // If the flow control type is not set, the credit-based flow control type is supported by default.
            // The initiator has to try to turn on the use of credit based flow control.
            if ((fcType == FC_TYPE_UNKNOWN) || (fcType == FC_TYPE_CREDIT)) {
                *cl = CL_REQ_SUPPORT_CREDIT;
                *credits = DEFAULT_CREDITS_VALUE;
            } else {
                *cl = CL_REQ_UNSUPPORTED_CREDIT;
                *credits = 0;
            }
            break;
        case PN_RSP_BEFORE_CREATE:
            // A responding implementation shall set this field in the PN response to 14 (0xE),
            // if (and only if) the value in the PN request was 15.
            if ((fcType == FC_TYPE_CREDIT) && (reqCl == CL_REQ_SUPPORT_CREDIT)) {
                *cl = CL_RSP_SUPPORT_CREDIT;
                *credits = DEFAULT_CREDITS_VALUE;
            } else {
                *cl = CL_RSP_UNSUPPORTED_CREDIT;
                *credits = 0;
            }
            break;
        case PN_RSP_AFTER_CREATE:
            // After the DLC is established, the responder of a PN request may
            // refuse to change any parameters.
            // it is not possible to “set initial credits” more than once per DLC activation.
            *credits = 0;
            if (reqCl == CL_REQ_SUPPORT_CREDIT) {
                LOG_DEBUG("%{public}s:Peer's cl value is wrong.", __func__);
                *cl = CL_RSP_SUPPORT_CREDIT;
            } else {
                *cl = CL_RSP_UNSUPPORTED_CREDIT;
            }
            break;
        default:
            LOG_ERROR("%{public}s:Error cmd kind.", __func__);
            break;
    }
}