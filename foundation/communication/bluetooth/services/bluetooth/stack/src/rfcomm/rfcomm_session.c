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

static List *g_sessionList;

/**
 * @brief Create session list when RFCOMM initialize.
 *
 */
void RfcommCreateSessionList()
{
    LOG_INFO("%{public}s", __func__);

    g_sessionList = ListCreate(NULL);
}

/**
 * @brief Free session list when RFCOMM finalize.
 *
 */
void RfcommDestroySessionList()
{
    LOG_INFO("%{public}s", __func__);

    ListNode *node = NULL;
    RfcommSessionInfo *session = NULL;

    if (g_sessionList == NULL) {
        LOG_DEBUG("%{public}s Session list is NULL.", __func__);
        return;
    }

    // Release server information.
    node = ListGetFirstNode(g_sessionList);
    while (node != NULL) {
        session = ListGetNodeData(node);
        if (session->timer != NULL) {
            AlarmDelete(session->timer);
            session->timer = NULL;
        }
        free(session);
        node = ListGetNextNode(node);
    }

    // Free server list.
    ListDelete(g_sessionList);
    g_sessionList = NULL;
}

/**
 * @brief The timeout callback function registered to alarm.
 *
 * @param parameter The parametr info registered to alarm.
 */
static void RfcommSessionTimeoutCallback(void *context)
{
    LOG_INFO("%{public}s", __func__);

    if (context == NULL) {
        LOG_ERROR("%{public}s:Session is closed.", __func__);
        return;
    }

    BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommSessionTimeout, context);

    return;
}

void RfcommSessionTimeout(void *parameter)
{
    LOG_INFO("%{public}s", __func__);

    bool isSessionValid = RfcommIsSessionValid((RfcommSessionInfo *)parameter);
    if (!isSessionValid) {
        LOG_ERROR("%{public}s:Session is closed.", __func__);
        return;
    }

    RfcommSessionEvtFsm((RfcommSessionInfo *)parameter, EV_SESSION_TIMEOUT, NULL);
}

/**
 * @brief Start session's timer.
 *
 * @param session The pointer of the session in the session list.
 * @param timeout Timeout value.
 */
void RfcommStartSessionTimer(RfcommSessionInfo *session, uint8_t timeout)
{
    LOG_INFO("%{public}s", __func__);

    if (session->timer == NULL) {
        LOG_ERROR("%{public}s Session's timer is NULL.", __func__);
        return;
    }

    AlarmSet(session->timer, timeout * RFCOMM_PER_SEC, RfcommSessionTimeoutCallback, session);
}

/**
 * @brief Stop session's timer.
 *
 * @param session The pointer of the session in the session list.
 */
void RfcommStopSessionTimer(RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    if (session->timer == NULL) {
        LOG_ERROR("%{public}s Session's timer is NULL.", __func__);
        return;
    }

    AlarmCancel(session->timer);
}

/**
 * @brief This function is used to register session information.
 *
 * @param addr        The peer's BT address.
 * @param lcid        L2CAP channel id.
 * @param id          sequence id(Only used in L2CIF_ConnectRsp).
 * @param isInitiator Whether it is the initiator. true: initiator, false: non-initiator.
 * @return The pointer of the new session in the session list.
 */
RfcommSessionInfo *RfcommCreateSession(const BtAddr *addr, uint16_t lcid, uint8_t id, bool isInitiator)
{
    LOG_INFO("%{public}s", __func__);

    if (g_sessionList == NULL) {
        LOG_ERROR("%{public}s Session list is NULL.", __func__);
        return NULL;
    }

    int32_t listCount = ListGetSize(g_sessionList);
    // If sessionlist's size exceeds 6, there is no resource to establish a new session connection,
    // and NULL is returned
    if (listCount >= MAX_SESSION_COUNT) {
        LOG_ERROR("%{public}s Session is over Max count.", __func__);
        return NULL;
    }
    // Malloc a new session resource.
    RfcommSessionInfo *session = malloc(sizeof(RfcommSessionInfo));
    if (session == NULL) {
        return NULL;
    }
    session->isInitiator = isInitiator;
    (void)memcpy_s(&(session->btAddr), sizeof(BtAddr), addr, sizeof(BtAddr));
    session->l2capId = lcid;
    session->id = id;
    session->pendingL2capId = 0;
    session->pendingId = 0;
    session->fcType = FC_TYPE_UNKNOWN;
    session->sessionState = ST_SESSION_CLOSED;
    session->l2capLocalMtu = RFCOMM_L2CAP_MTU;
    session->l2capPeerMtu = RFCOMM_L2CAP_DEFAUT_MTU;
    session->timer = AlarmCreate(NULL, false);
    session->peerSessionFc = false;
    // Add the new session info to the session list.
    ListAddLast(g_sessionList, session);

    // Return a session pointer.
    return session;
}

/**
 * @brief During the session disconnection process, the upper layer sends a DLC connection request,
 *        reset part of session data.
 *
 * @param session The pointer of the session in the session list.
 */
void RfcommResetSessionInfo(RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommStopSessionTimer(session);
    session->isInitiator = false;
    session->l2capId = 0;
    session->id = 0;
    session->pendingL2capId = 0;
    session->pendingId = 0;
    session->peerSessionFc = false;
    session->fcType = FC_TYPE_UNKNOWN;
    session->sessionState = ST_SESSION_CLOSED;
    session->l2capLocalMtu = RFCOMM_L2CAP_MTU;
    session->l2capPeerMtu = RFCOMM_L2CAP_DEFAUT_MTU;
}

/**
 * @brief Set flown control type of the session.
 *
 * @param session The pointer of the session in the session list.
 * @param isPnReq true:Pn request, false:Pn response.
 * @param cl      cl value.
 */
void RfcommSetFlowControlType(RfcommSessionInfo *session, bool isPnReq, uint8_t cl)
{
    LOG_INFO("%{public}s", __func__);

    if (session->fcType != FC_TYPE_UNKNOWN) {
        LOG_INFO("%{public}s flow control type is confirmed.", __func__);
        return;
    }

    // A responding implementation shall set this field in the PN response to 14 (0xE),
    // if (and only if) the value in the PN request was 15.Ref-RFCOMM_SPEC_V12(5.5.3).
    if (isPnReq) {
        if (cl == CL_REQ_SUPPORT_CREDIT) {
            session->fcType = FC_TYPE_CREDIT;
        } else {
            session->fcType = FC_TYPE_NORMAL;
        }
    } else {
        if (cl == CL_RSP_SUPPORT_CREDIT) {
            session->fcType = FC_TYPE_CREDIT;
        } else {
            session->fcType = FC_TYPE_NORMAL;
        }
    }
}

/**
 * @brief Remove session node from session list and free the session resources.
 *
 * @param session The pointer of the session in the session list.
 */
void RfcommRemoveSession(RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    ListRemoveNode(g_sessionList, session);

    if (session->timer != NULL) {
        AlarmDelete(session->timer);
        session->timer = NULL;
    }

    free(session);
}

/**
 * @brief This function is used to find the specified session using lcid.
 *
 * @param lcid The lcid value.
 * @return The pointer of the session in the session list.
 */
RfcommSessionInfo *RfcommGetSessionByLcid(uint16_t lcid)
{
    LOG_INFO("%{public}s lcid:%hu", __func__, lcid);

    RfcommSessionInfo *session = NULL;
    ListNode *node = NULL;

    if (g_sessionList == NULL) {
        LOG_ERROR("%{public}s Session list is NULL.", __func__);
        return NULL;
    }

    node = ListGetFirstNode(g_sessionList);
    while (node != NULL) {
        session = ListGetNodeData(node);
        if (session->l2capId == lcid) {
            return session;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

/**
 * @brief This function is used to find the specified session using BT address.
 *
 * @param addr BT address.
 * @return The pointer of the session in the session list.
 */
RfcommSessionInfo *RfcommGetSessionByAddr(const BtAddr *addr)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSessionInfo *session = NULL;
    ListNode *node = NULL;

    if (g_sessionList == NULL) {
        LOG_ERROR("%{public}s Session list is NULL.", __func__);
        return NULL;
    }

    node = ListGetFirstNode(g_sessionList);
    while (node != NULL) {
        session = ListGetNodeData(node);
        if (!memcmp(&session->btAddr, addr, sizeof(BtAddr))) {
            return session;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

/**
 * @brief Check if the session is valid.
 *
 * @param session The pointer of the session.
 * @return True if the session exists,false if the session doesn't exist.
 */
bool RfcommIsSessionValid(RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSessionInfo *sessionInfo = NULL;
    ListNode *node = NULL;

    if (g_sessionList == NULL) {
        LOG_ERROR("%{public}s Session list is NULL.", __func__);
        return NULL;
    }

    // Check if there is a channel on the session.
    node = ListGetFirstNode(g_sessionList);
    while (node != NULL) {
        sessionInfo = ListGetNodeData(node);
        if (sessionInfo == session) {
            LOG_DEBUG("%{public}s The session is valid.", __func__);
            return true;
        }
        node = ListGetNextNode(node);
    }

    return false;
}