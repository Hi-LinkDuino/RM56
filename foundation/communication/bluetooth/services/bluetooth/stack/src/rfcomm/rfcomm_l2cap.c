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

/**
 * @brief L2CAP calls this function to notify RFCOMM connected requirements.
 *        After receiving the request, RFCOMM creates session information and calls the state machine.
 *
 * @param aclHandle ACL handle.
 * @param lcid      L2CAP channel id.
 * @param id        L2CAP's id information.
 * @param psm       psm value.
 */
void RfcommRecvConnectReqCback(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm)
{
    LOG_INFO("%{public}s lcid:%hu, lpsm:%hu", __func__, lcid, lpsm);

    RfcommSessionInfo *session = NULL;
    RfcommRecvConnectedInfo connectedInfo;
    BtAddr addr;
    (void)memset_s(&addr, sizeof(addr), 0x00, sizeof(addr));

    if (info == NULL) {
        LOG_ERROR("%{public}s info is NULL.", __func__);
        return;
    }

    (void)memcpy_s(&addr, sizeof(BtAddr), &(info->addr), sizeof(BtAddr));
    session = RfcommGetSessionByAddr(&addr);
    if (session == NULL) {
        session = RfcommCreateSession(&addr, lcid, id, false);
    }

    if (session != NULL) {
        connectedInfo.id = id;
        connectedInfo.lcid = lcid;
        // Established session Successfully.
        RfcommSessionEvtFsm(session, EV_SESSION_RECV_CONNECT_REQ, (void*)&connectedInfo);
    } else {
        LOG_ERROR("%{public}s No session resources.", __func__);
        RfcommSendConnectRsp(lcid, id, L2CAP_NO_RESOURCES_AVAILABLE, 0);
    }
}

/**
 * @brief L2CAP calls this function to notify RFCOMM the connection response from peer.
 *        After receiving the response, calls the state machine.
 *
 * @param aclHandle ACL handle.
 * @param lcid      L2CAP channel id.
 * @param result    The result of the response.
 * @param status    status value.
 */
void RfcommRecvConnectRspCback(uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status)
{
    LOG_INFO("%{public}s lcid:%hu, result:%hu", __func__, lcid, result);

    RfcommSessionInfo *session = NULL;

    if (result == L2CAP_CONNECTION_PENDING) {
        LOG_INFO("%{public}s:L2CAP pending.", __func__);
        return;
    }

    session = RfcommGetSessionByLcid(lcid);
    if (session != NULL) {
        // Session exists.
        RfcommSessionEvtFsm(session, EV_SESSION_RECV_CONNECT_RSP, &result);
    }
}

/**
 * @brief L2CAP calls this function to notify RFCOMM config requirements.
 *        After receiving the request, RFCOMM calls the state machine.
 *
 * @param lcid    L2CAP channel id.
 * @param id      L2CAP's id information.
 * @param cfg     Config information.
 */
void RfcommRecvConfigReqCback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommSessionInfo *session = RfcommGetSessionByLcid(lcid);
    RfcommConfigReqInfo reqInfo;
    (void)memset_s(&reqInfo, sizeof(RfcommConfigReqInfo), 0x00, sizeof(RfcommConfigReqInfo));

    reqInfo.id = id;
    (void)memcpy_s(&reqInfo.cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));

    if (session != NULL) {
        // Session exists.
        RfcommSessionEvtFsm(session, EV_SESSION_RECV_CONFIG_REQ, &reqInfo);
    }
}

/**
 * @brief L2CAP calls this function to notify RFCOMM config response from peer.
 *        After receiving the response, RFCOMM calls the state machine.
 *
 * @param lcid    L2CAP channel id.
 * @param cfg     Config information.
 * @param result  The result of the response.
 */
void RfcommRecvConfigRspCback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result)
{
    LOG_INFO("%{public}s lcid:%hu, result:%hu", __func__, lcid, result);

    RfcommSessionInfo *session = NULL;
    RfcommConfigRspInfo configResult;

    if (result == L2CAP_PENDING) {
        LOG_INFO("%{public}s:L2CAP pending.", __func__);
        return;
    }

    if (cfg == NULL) {
        return;
    }

    (void)memset_s(&configResult, sizeof(RfcommConfigRspInfo), 0x00, sizeof(RfcommConfigRspInfo));
    (void)memcpy_s(&configResult.cfg, sizeof(L2capConfigInfo), cfg, sizeof(L2capConfigInfo));
    configResult.result = result;

    session = RfcommGetSessionByLcid(lcid);
    if (session != NULL) {
        // Session exists.
        RfcommSessionEvtFsm(session, EV_SESSION_RECV_CONFIG_RSP, &configResult);
    }
}

/**
 * @brief L2CAP calls this function to notify RFCOMM disconnect requirements.
 *        After receiving the request, RFCOMM calls the state machine.
 *
 * @param lcid    L2CAP channel id.
 * @param id      L2CAP's id information.
 */
void RfcommRecvDisconnectReqCback(uint16_t lcid, uint8_t id)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommSessionInfo *session = RfcommGetSessionByLcid(lcid);

    RfcommSendDisconnectRsp(lcid, id);

    if (session != NULL) {
        RfcommSessionEvtFsm(session, EV_SESSION_RECV_DISCONNECT_REQ, NULL);
    }
}

/**
 * @brief When an abnormal disconnection event occurs,
 *        such as link loss, connection timeout, etc., L2CAP calls this function to notify RFCOMM.
 *        After receiving the notification, RFCOMM calls the state machine.
 *
 * @param lcid    L2CAP channel id.
 * @param reason  Abnormal disconnection's reason.
 */
void RfcommDisconnectAbnormalCback(uint16_t lcid, uint8_t reason)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommSessionInfo *session = RfcommGetSessionByLcid(lcid);

    if (session != NULL) {
        // Session exists.
        RfcommSessionEvtFsm(session, EV_SESSION_RECV_LINK_LOSS, &reason);
    }
}

/**
 * @brief When receiving RFCOMM Frame, L2CAP calls this function to notify RFCOMM.
 *        RFCOMM analyzes the data and determines whether it is a session event or a channel event,
 *        and calls the corresponding state machine to continue processing.
 *
 * @param lcid    L2CAP channel id.
 * @param pkt     The RFCOMM frame information.
 */
void RfcommRecvDataCback(uint16_t lcid, Packet *pkt)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    int event = -1;
    uint8_t dlci = 0;
    RfcommUihInfo info;
    RfcommSessionInfo *session = RfcommGetSessionByLcid(lcid);
    RfcommChannelInfo *channel = NULL;
    RfcommParseFrameResult parseRslt;
    (void)memset_s(&info, sizeof(RfcommUihInfo), 0x00, sizeof(RfcommUihInfo));

    if (session == NULL) {
        LOG_ERROR("%{public}s:Session does not exist.", __func__);
        return;
    }

    parseRslt.dlci = &dlci;
    parseRslt.event = &event;
    parseRslt.info = &info;

    RfcommEventType eventType = RfcommParseFrames(session, pkt, parseRslt);
    switch (eventType) {
        case EVENT_SESSION:
            RfcommSessionEvtFsm(session, event, &info);
            break;
        case EVENT_CHANNEL:
            channel = RfcommGetChannelByDlci(session, dlci);
            if (channel == NULL) {
                // If the channel does not exist, it means that the server is currently connected.
                // In this case, find the server and create a channel associated with the server.
                channel = RfcommCreateChannelOfServer(session, dlci, event);
            }
            if (channel != NULL) {
                RfcommChannelEvtFsm(channel, event, &info);
            }
            break;
        case EVENT_UNSUPPORTED_CMD:
            RfcommSendUihNsc(session, info.invalidCmd.ea, info.invalidCmd.cr, info.invalidCmd.type);
            break;
        case EVENT_FRAME_ERR:
            LOG_ERROR("%{public}s:RFCOMM frame error.", __func__);
            break;
        default:
            break;
    }
}

void RfcommSendConnectReqCback(const BtAddr *addr, uint16_t lcid, int result)
{
    LOG_INFO("%{public}s lcid is %hu, result is %{public}d.", __func__, lcid, result);

    RfcommReqConnectRslt connectRslt;
    RfcommSessionInfo *session = RfcommGetSessionByAddr(addr);

    (void)memset_s(&connectRslt, sizeof(RfcommReqConnectRslt), 0x00, sizeof(RfcommReqConnectRslt));
    connectRslt.lcid = lcid;
    connectRslt.result = result;

    if (session != NULL) {
        // Session exists.
        RfcommSessionEvtFsm(session, EV_SESSION_RECV_REQ_CONNECT_RESULT, &connectRslt);
    }
}