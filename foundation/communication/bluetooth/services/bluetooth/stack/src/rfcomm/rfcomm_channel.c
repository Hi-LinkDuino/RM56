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

static List *g_channelList;
static bool g_channelHandle[MAX_DLC_COUNT] = {false};
static Mutex *g_readLock = NULL;

void RfcommReadLock()
{
    LOG_INFO("%{public}s", __func__);

    if (g_readLock != NULL) {
        MutexLock(g_readLock);
    }
}

void RfcommReadUnlock()
{
    LOG_INFO("%{public}s", __func__);

    if (g_readLock != NULL) {
        MutexUnlock(g_readLock);
    }
}

/**
 * @brief Create channel list when RFCOMM initialize.
 *
 */
void RfcommCreateChannelList()
{
    LOG_INFO("%{public}s", __func__);

    for (uint8_t cnt = 0; cnt < MAX_DLC_COUNT; cnt++) {
        g_channelHandle[cnt] = false;
    }

    g_channelList = ListCreate(NULL);

    g_readLock = MutexCreate();
}

/**
 * @brief Free channel list when RFCOMM finalize.
 */
void RfcommDestroyChannelList()
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    for (uint8_t cnt = 0; cnt < MAX_DLC_COUNT; cnt++) {
        g_channelHandle[cnt] = false;
    }

    if (g_channelList == NULL) {
        LOG_DEBUG("%{public}s Channel list is NULL.", __func__);
        return;
    }

    // Release channel information.
    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        node = ListGetNextNode(node);
        RfcommRemoveChannel(channel);
    }

    // Free channel list.
    ListDelete(g_channelList);
    g_channelList = NULL;

    // Destroy read lock.
    if (g_readLock != NULL) {
        MutexDelete(g_readLock);
        g_readLock = NULL;
    }
}

/**
 * @brief The function is used to assign handle to individual DLC.
 *
 * @return Handle number.0(unavailable handle),1~36(available handle)
 */
static uint16_t RfcommAssignHandle()
{
    LOG_INFO("%{public}s", __func__);

    uint16_t handle = 0;

    for (uint8_t index = 0; index < MAX_DLC_COUNT; index++) {
        if (g_channelHandle[index]) {
            continue;
        }
        g_channelHandle[index] = true;
        handle = index + 1;
        LOG_DEBUG("%{public}s DLC handle is %hu.", __func__, handle);
        break;
    }

    return handle;
}

/**
 * @brief After disconnect the DLC, free the handle.
 *
 * @param handle Handle.
 */
static void RfcommFreeHandle(uint16_t handle)
{
    if ((handle < 1) || (handle > MAX_DLC_COUNT)) {
        LOG_DEBUG("%{public}s handle(%hu) is overrun.", __func__, handle);
        return;
    }

    g_channelHandle[handle - 1] = false;
}

/**
 * @brief Initialize channel information.
 *
 * @param channel The pointer of the channel in the channel list.
 */
static void RfcommInitChannelInfo(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRemotePortConfig defaultCfg = {
        BAUDRATE_9600, DATA_BIT_8, STOP_BIT_1, NO_PARITY, ODD_PARITY, NO_FLC, XON_DC1, XOFF_DC3, 0x00, 0x00
    };

    // Init channel information.
    (void)memset_s(channel, sizeof(RfcommChannelInfo), 0x00, sizeof(RfcommChannelInfo));
    channel->channelState = ST_CHANNEL_CLOSED;
    channel->peerMtu = RFCOMM_PEER_DEFAULT_MTU;
    channel->sendQueue = ListCreate(NULL);
    channel->recvQueue = ListCreate(NULL);
    channel->timer = AlarmCreate(NULL, false);
    channel->localCreditMax = MAX_CREDIT_COUNT;
    channel->peerChannelFc = false;
    // Set remote port default value.
    channel->portConfig = defaultCfg;
}

/**
 * @brief The timeout's processing.
 *
 * @param parameter The parametr info registered to alarm.
 */
void RfcommChannelTimeout(void *parameter)
{
    LOG_INFO("%{public}s", __func__);

    bool isChannelValid = RfcommIsChannelValid((RfcommChannelInfo *)parameter);
    if (!isChannelValid) {
        LOG_ERROR("%{public}s:Channel is closed.", __func__);
        return;
    }

    RfcommChannelEvtFsm((RfcommChannelInfo *)parameter, EV_CHANNEL_TIMEOUT, NULL);
}

/**
 * @brief The timeout callback function registered to alarm.
 *
 * @param parameter The parametr info registered to alarm.
 */
static void RfcommChannelTimeoutCallback(void *context)
{
    LOG_INFO("%{public}s", __func__);

    if (context == NULL) {
        LOG_ERROR("%{public}s:DLC is closed.", __func__);
        return;
    }

    BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommChannelTimeout, context);

    return;
}

/**
 * @brief Start channel's timer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param timeout Timeout value.
 */
void RfcommStartChannelTimer(RfcommChannelInfo *channel, uint8_t timeout)
{
    LOG_INFO("%{public}s", __func__);

    if (channel->timer == NULL) {
        LOG_ERROR("%{public}s Channel's timer is NULL.", __func__);
        return;
    }

    AlarmSet(channel->timer, timeout * RFCOMM_PER_SEC, RfcommChannelTimeoutCallback, channel);
}

/**
 * @brief Stop channel's timer.
 *
 * @param  channel The pointer of the channel in the channel list.
 */
void RfcommStopChannelTimer(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    if (channel->timer == NULL) {
        LOG_ERROR("%{public}s Channel's timer is NULL.", __func__);
        return;
    }

    AlarmCancel(channel->timer);
}

/**
 * @brief This function is used to register data link related information and
 *        generate the handle of the data link connection
 *
 * @param createChannelInfo Channel creation information.
 * @return The pointer of the new channel in the channel list.
 */
RfcommChannelInfo *RfcommCreateChannel(const RfcommCreateChannelInfo *createChannelInfo)
{
    LOG_INFO("%{public}s", __func__);

    if (g_channelList == NULL) {
        LOG_ERROR("%{public}s Channel list is NULL.", __func__);
        return NULL;
    }

    uint16_t count = (uint16_t)ListGetSize(g_channelList);
    // If the max number of links has been exceeded, no more links will be created.
    if (count >= MAX_DLC_COUNT) {
        LOG_ERROR("%{public}s Channel count is over 36.", __func__);
        return NULL;
    }

    RfcommChannelInfo *channel = malloc(sizeof(RfcommChannelInfo));
    if (channel == NULL) {
        return NULL;
    }
    // Init channel information.
    RfcommInitChannelInfo(channel);
    channel->session = createChannelInfo->session;
    channel->isServer = createChannelInfo->isServer;
    channel->scn = createChannelInfo->dlci >> RFCOMM_DLCI_SHIFT_SCN;
    if (createChannelInfo->mtu > 0) {
        channel->localMtu = (createChannelInfo->mtu > RFCOMM_LOCAL_DEFAULT_MTU) ?
                            RFCOMM_LOCAL_DEFAULT_MTU : createChannelInfo->mtu;
    } else {
        channel->localMtu = RFCOMM_LOCAL_DEFAULT_MTU;
    }
    channel->context = createChannelInfo->context;
    channel->callBack = createChannelInfo->callback;
    channel->eventMask = createChannelInfo->eventMask;
    channel->dlci = createChannelInfo->dlci;
    // Set handle.
    channel->handle = RfcommAssignHandle();

    // Add the new channel into channel list.
    RfcommReadLock();
    ListAddLast(g_channelList, channel);
    RfcommReadUnlock();

    // return channel.
    return channel;
}

/**
 * @brief The server is currently connected, create a channel associated with the server.
 *
 * @param session The pointer of the session in the session list.
 * @param dlci    The dlci value.
 * @param event   The event id.
 * @return The pointer of the new channel in the channel list.
 */
RfcommChannelInfo *RfcommCreateChannelOfServer(RfcommSessionInfo *session, uint8_t dlci, int event)
{
    LOG_INFO("%{public}s", __func__);

    // If the event is SABM cmd, PN req, RPN req, RPN cmd, find the server and
    // create a channel associated with the server.
    if (!IS_FIRST_CMD(event)) {
        LOG_DEBUG("%{public}s:Event(%{public}d) is not first cmd.", __func__, event);
        return NULL;
    }

    uint8_t scn = dlci >> RFCOMM_DLCI_SHIFT_SCN;
    RfcommServerInfo *server = RfcommGetServerByScn(scn);
    if (server == NULL) {
        LOG_ERROR("%{public}s:Server(scn:%hhu) does not exist.", __func__, scn);
        return NULL;
    }

    RfcommCreateChannelInfo createChannelInfo;
    createChannelInfo.session = session;
    createChannelInfo.isServer = true;
    createChannelInfo.dlci = dlci;
    createChannelInfo.mtu = server->maxRevSize;
    createChannelInfo.eventMask = server->eventMask;
    createChannelInfo.callback = server->callBack;
    createChannelInfo.context = server->context;

    RfcommChannelInfo *channel = RfcommCreateChannel(&createChannelInfo);

    return channel;
}

/**
 * @brief Remove channel node from channel list and free the channel resources.
 *
 * @param channel The pointer of the channel in the channel list.
 */
void RfcommRemoveChannel(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    RfcommReadLock();

    // Release cache data.
    RfcommReleaseCachePkt(channel);

    // Release alarm resource.
    if (channel->timer != NULL) {
        AlarmDelete(channel->timer);
        channel->timer = NULL;
    }

    // Free handle.
    RfcommFreeHandle(channel->handle);

    // Remove channel node.
    ListRemoveNode(g_channelList, channel);

    // Release channel resource.
    free(channel);

    RfcommReadUnlock();
}

/**
 * @brief Remove packet from send queue and free the queue resources.
 *
 * @param channel The pointer of the channel in the channel list.
 */
void RfcommReleaseCachePkt(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    Packet *pkt = NULL;
    ListNode *node = NULL;

    if (channel->sendQueue != NULL) {
        // Release send queue's cache data.
        node = ListGetFirstNode(channel->sendQueue);
        while (node != NULL) {
            pkt = ListGetNodeData(node);
            PacketFree(pkt);
            node = ListGetNextNode(node);
        }
        // Release send queue resource.
        ListDelete(channel->sendQueue);
        channel->sendQueue = NULL;
    }

    if (channel->recvQueue != NULL) {
        // Release receive queue's cache data.
        node = ListGetFirstNode(channel->recvQueue);
        while (node != NULL) {
            pkt = ListGetNodeData(node);
            PacketFree(pkt);
            node = ListGetNextNode(node);
        }
        // Release receive queue resource.
        ListDelete(channel->recvQueue);
        channel->recvQueue = NULL;
    }
}

/**
 * @brief During the DLC disconnection process, the upper layer sends a DLC connection request,
 *        reset part of DLC data.
 *
 * @param channel The pointer of the channel in the channel list.
 */
void RfcommResetChannelInfo(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRemotePortConfig defaultCfg = {
        BAUDRATE_9600, DATA_BIT_8, STOP_BIT_1, NO_PARITY, ODD_PARITY, NO_FLC, XON_DC1, XOFF_DC3, 0x00, 0x00
    };

    RfcommReadLock();

    RfcommStopChannelTimer(channel);
    RfcommReleaseCachePkt(channel);
    channel->sendQueue = ListCreate(NULL);
    channel->recvQueue = ListCreate(NULL);
    channel->peerMtu = RFCOMM_PEER_DEFAULT_MTU;
    channel->channelState = ST_CHANNEL_CLOSED;
    channel->localCredit = 0;
    channel->peerCredit = 0;
    channel->transferReady = 0;
    channel->localFcToPeer = false;
    channel->localFcToUpper = false;
    channel->peerChannelFc = false;
    channel->receivedBytes = 0;
    channel->transmittedBytes = 0;
    // Set remote port default value.
    channel->portConfig = defaultCfg;

    (void)memset_s(&channel->lineStatus, sizeof(RfcommRemoteLineStatus), 0x00, sizeof(RfcommRemoteLineStatus));
    (void)memset_s(&channel->peerModemSt, sizeof(RfcommModemStatusInfo), 0x00, sizeof(RfcommModemStatusInfo));

    RfcommReadUnlock();
}

/**
 * @brief Initialize channel information.
 *
 * @param channel   The pointer of the channel in the channel list.
 * @param eventId   The eventId to be notified to the upper layer.
 * @param eventData The data to be notified to the upper layer.
 */
NO_SANITIZE("cfi")
void RfcommNotifyEvtToUpper(const RfcommChannelInfo *channel, uint32_t eventId, const void *eventData)
{
    LOG_INFO("%{public}s event is %u", __func__, eventId);

    uint32_t event = eventId & channel->eventMask;

    if (!event) {
        LOG_DEBUG("%{public}s:No event that the upper layer cares.", __func__);
        return;
    }

    if ((channel->channelState == ST_CHANNEL_CLOSED) && (!IS_DISCONNECTED(eventId))) {
        LOG_DEBUG("%{public}s:Channel is disconnected.", __func__);
        return;
    }

    if (channel->callBack) {
        LOG_DEBUG("%{public}s:eventId(%u), handle(%hu)", __func__, eventId, channel->handle);
        channel->callBack(channel->handle, event, eventData, channel->context);
        LOG_DEBUG("%{public}s:End.", __func__);
    }
}

/**
 * @brief This function is used to find the specified channel using dlci and session.
 *
 * @param session The pointer of the session in the session list.
 * @param dlci    The dlci value.
 * @return The pointer of the channel in the channel list.
 */
RfcommChannelInfo *RfcommGetChannelByDlci(const RfcommSessionInfo *session, uint8_t dlci)
{
    LOG_INFO("%{public}s dlci:%hhu", __func__, dlci);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    if (g_channelList == NULL) {
        LOG_ERROR("%{public}s Channel list is NULL.", __func__);
        return NULL;
    }

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if ((channel->session == session) && (channel->dlci == dlci)) {
            return channel;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

/**
 * @brief This function is used to get the first channel on the session.
 *
 * @param session The pointer of the session in the session list.
 * @return The pointer of the channel in the channel list.
 */
RfcommChannelInfo *RfcommGetFirstChannelOnSession(const RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    if (g_channelList == NULL) {
        LOG_ERROR("%{public}s Channel list is NULL.", __func__);
        return NULL;
    }

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if (channel->session == session) {
            return channel;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

/**
 * @brief This function is used to find the specified channel using handle.
 *
 * @param handle The channel's handle.
 * @return The pointer of the channel in the channel list.
 */
RfcommChannelInfo *RfcommGetChannelByHandle(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    if (g_channelList == NULL) {
        LOG_ERROR("%{public}s Channel list is NULL.", __func__);
        return NULL;
    }

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if (channel->handle == handle) {
            return channel;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}

/**
 * @brief If there is no channel on the particular session, then close the session.
 *
 * @param session The pointer of the session in the session list.
 */
bool RfcommCheckSessionValid(const RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    // Check if there is a channel on the session.
    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if (channel->session == session) {
            LOG_DEBUG("%{public}s There are channels on the session.", __func__);
            return true;
        }
        node = ListGetNextNode(node);
    }

    return false;
}

/**
 * @brief Check if the channel is valid.
 *
 * @param channel The pointer of the channel.
 * @return True if the channel exists,false if the channel doesn't exist.
 */
bool RfcommIsChannelValid(const RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channelInfo = NULL;
    ListNode *node = NULL;

    if (g_channelList == NULL) {
        LOG_ERROR("%{public}s Channel list is NULL.", __func__);
        return NULL;
    }

    // Check if there is a channel on the session.
    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channelInfo = ListGetNodeData(node);
        if (channelInfo == channel) {
            LOG_DEBUG("%{public}s The channel is valid.", __func__);
            return true;
        }
        node = ListGetNextNode(node);
    }

    return false;
}

/**
 * @brief Create all data link connections on the session.
 *
 * @param session The pointer of the session in the session list.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommOpenAllChannelOnSession(const RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    int ret = 0;
    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if (channel->session == session) {
            ret = RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_OPEN_REQ, NULL);
        }
        node = ListGetNextNode(node);
    }

    return ret;
}

/**
 * @brief Reset all channel's information on the session.
 *
 * @param session The pointer of the session in the session list.
 */
void RfcommResetAllChannelOnSession(const RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if (channel->session == session) {
            RfcommResetChannelInfo(channel);
        }
        node = ListGetNextNode(node);
    }
}

/**
 * @brief Send the buffered data to be sent on all channels.
 *
 * @param session The pointer of the session in the session list.
 */
void RfcommSendAllCachePktOnSession(const RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if (channel->session == session) {
            RfcommSendCachePkt(channel);
            RfcommSetFlcToUpper(channel);
        }
        node = ListGetNextNode(node);
    }
}

/**
 * @brief Notify all channels on the session event.
 *
 * @param session The pointer of the session in the session list.
 * @param eventId The eventId to be notified to the upper layer.
 */
void RfcommNotifyAllChannelEvtOnSession(const RfcommSessionInfo *session, uint32_t eventId)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        if (channel->session == session) {
            if (IS_DISCONNECTED(eventId)) {
                channel->channelState = ST_CHANNEL_CLOSED;
            }
            RfcommNotifyEvtToUpper(channel, eventId, NULL);
        }
        node = ListGetNextNode(node);
    }
}

/**
 * @brief Delete all channels on the specified Session.
 *
 * @param session The pointer of the session in the session list.
 */
void RfcommRemoveAllChannelOnSession(const RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        node = ListGetNextNode(node);
        if (channel->session == session) {
            // Release cache data.
            RfcommRemoveChannel(channel);
        }
    }
}

/**
 * @brief When receiving DISC0(timeout occurs at the opposite end),
 *        delete all channels on the session(except the channel to be connected).
 *
 * @param session The pointer of the session.
 */
void RfcommRemoveInvalidChannelOnSession(const RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        node = ListGetNextNode(node);
        if (channel->session != session) {
            continue;
        }
        switch (channel->channelState) {
            case ST_CHANNEL_CLOSED:
                // fall-through
            case ST_CHANNEL_WAIT_RESTART:
                // fall-through
            case ST_CHANNEL_WAIT_PN_RSP:
                // fall-through
                LOG_DEBUG("%{public}s The channel is waiting to be opened", __func__);
                break;
            case ST_CHANNEL_CLIENT_WAIT_SECURITY_RESULT:
                // fall-through
            case ST_CHANNEL_SABM_REQ_WAIT_UA:
                // fall-through
            case ST_CHANNEL_SERVER_WAIT_SECURITY_RESULT:
                RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_CONNECT_FAIL, NULL);
                RfcommRemoveChannel(channel);
                break;
            case ST_CHANNEL_WAIT_UPPER_RESPONSE:
                // fall-through
            case ST_CHANNEL_DISC_REQ_WAIT_UA:
                // fall-through
            case ST_CHANNEL_CONNECTED:
                RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_DISCONNECTED, NULL);
                RfcommRemoveChannel(channel);
                break;
            case ST_CHANNEL_WAIT_SABM:
                RfcommRemoveChannel(channel);
                break;
            default:
                break;
        }
    }
}

/**
 * @brief Remove the callback from channel list about the scn and disconnect the channel.
 *
 * @param scn Server number.
 * @param isRemoveCallback true:remove callback,RFCOMM will not notify event to upper layer.otherwise:false.
 */
void RfcommRemoveChannelCallback(uint8_t scn)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        node = ListGetNextNode(node);
        if ((channel->scn == scn) && (channel->isServer)) {
            channel->callBack = NULL;
            RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_CLOSE_REQ, NULL);
        }
    }
}

/**
 * @brief Send data in the buffer queue.
 *
 * @param channel The pointer of the channel in the channel list.
 */
void RfcommSendCachePkt(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSessionInfo *session = channel->session;
    Packet *pkt = NULL;
    ListNode *node = NULL;

    if (channel->transferReady != TRANSFER_READY) {
        LOG_DEBUG("%{public}s:Not ready to send data.", __func__);
        return;
    }

    node = ListGetFirstNode(channel->sendQueue);
    if (node == NULL) {
        LOG_DEBUG("%{public}s:There is no cache data.", __func__);
        return;
    }

    if (session->fcType == FC_TYPE_CREDIT) {
        while ((channel->peerCredit) && (node)) {
            pkt = ListGetNodeData(node);
            node = ListGetNextNode(node);
            uint8_t newCredit = channel->localCreditMax - channel->localCredit;
            newCredit = (newCredit < MAX_ONCE_NEWCREDIT) ? newCredit : MAX_ONCE_NEWCREDIT;
            channel->localCredit += newCredit;
            // Add transmite data bytes value.
            channel->transmittedBytes += PacketPayloadSize(pkt);
            // Send data to peer.
            RfcommSendUihData(session, channel->dlci, newCredit, pkt);
            ListRemoveNode(channel->sendQueue, pkt);
            PacketFree(pkt);
            // Decrease credits.
            channel->peerCredit--;
        }
    } else if ((!channel->peerChannelFc) && (!session->peerSessionFc)) {
        while (node != NULL) {
            pkt = ListGetNodeData(node);
            node = ListGetNextNode(node);
            // Add transmite data bytes value.
            channel->transmittedBytes += PacketPayloadSize(pkt);
            // Send data to peer.
            RfcommSendUihData(session, channel->dlci, 0, pkt);
            ListRemoveNode(channel->sendQueue, pkt);
            PacketFree(pkt);
        }
    }
}

/**
 * @brief Notify the upper layer whether RFCOMM can receive data.
 *
 * @param channel The pointer of the channel in the channel list.
 */
void RfcommSetFlcToUpper(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    if (channel->localFcToUpper) {
        RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_FC_ON, NULL);
        channel->localFcToUpper = false;
    }
}

/**
 * @brief Send further credits to peer.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param credits The number of credits that can be received locally.
 */
static void RfcommSendCredits(const RfcommChannelInfo *channel, uint8_t credits)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSendUihData(channel->session, channel->dlci, credits, NULL);
}

/**
 * @brief Notify the peer whether local can receive data.
 *
 * @param channel The pointer of the channel in the channel list.
 * @param enable  Whether local can receive data.true:yes,false:no
 */
void RfcommSetFlcToPeer(RfcommChannelInfo *channel, bool enable)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSessionInfo *session = channel->session;
    RfcommModemStatusInfo modemStatus;
    (void)memset_s(&modemStatus, sizeof(modemStatus), 0x00, sizeof(modemStatus));

    if (session->fcType == FC_TYPE_CREDIT) {
        if (!enable) {
            return;
        }
        // Send new credit.
        uint8_t newCredits = channel->localCreditMax - channel->localCredit;
        newCredits = (newCredits >= MAX_ONCE_NEWCREDIT) ? (MAX_ONCE_NEWCREDIT - 1) : newCredits;
        channel->localCredit += newCredits;
        RfcommSendCredits(channel, newCredits + 1);
    } else {
        RfcommReadLock();
        uint8_t count = ListGetSize(channel->recvQueue);
        RfcommReadUnlock();
        if ((enable) && (channel->localFcToPeer)) {
            modemStatus.signals = MSC_RTC | MSC_RTR | MSC_DV;
            channel->localFcToPeer = false;
        } else if ((!enable) && (count >= MAX_QUEUE_COUNT)) {
            modemStatus.signals = MSC_FC | MSC_RTC | MSC_RTR | MSC_DV;
            channel->localFcToPeer = true;
        } else {
            return;
        }
        RfcommSendUihMsc(channel->session, channel->dlci, true, &modemStatus);
        // Start timer.
        RfcommStartChannelTimer(channel, T2_UIH_DLCI0);
    }
}

/**
 * @brief Set the peer's modem status, and notify the upper layer if the status changes.
 *        And if the current flow control is not based on credit,
 *        set the fc state and send the data in the queue if the peer can receive data.
 *
 * @param channel  The pointer of the channel in the channel list.
 * @param modemSts Peer's modem status.
 */
void RfcommSetPeerModemStatus(RfcommChannelInfo *channel, const RfcommModemStatusInfo *modemSts)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSessionInfo *session = channel->session;
    RfcommModemStatus status;
    (void)memset_s(&status, sizeof(status), 0x00, sizeof(status));

    // Notify the upper layer the peer's status if the status changes.
    if (memcmp(&channel->peerModemSt, modemSts, sizeof(RfcommModemStatusInfo))) {
        (void)memcpy_s(&channel->peerModemSt, sizeof(RfcommModemStatusInfo), modemSts, sizeof(RfcommModemStatusInfo));
        status.fc = (modemSts->signals & MSC_FC) ? 1 : 0;
        status.rtc = (modemSts->signals & MSC_RTC) ? 1 : 0;
        status.rtr = (modemSts->signals & MSC_RTR) ? 1 : 0;
        status.ic = (modemSts->signals & MSC_IC) ? 1 : 0;
        status.dv = (modemSts->signals & MSC_DV) ? 1 : 0;
        status.break_signal = (modemSts->breakSignal & MSC_BREAK) ? 1 : 0;
        status.break_length = (modemSts->breakSignal & MSC_BREAK_LEN) >> RFCOMM_MSC_SHIFT_BREAKLEN;
        RfcommNotifyEvtToUpper(channel, RFCOMM_CHANNEL_EV_MODEM_STATUS, &status);
    }
    // When credit based flow control is being used on a session,
    // the FC-bit in the MSC-command has no meaning;
    // it shall be set to zero in MSCcommands, and it shall be ignored by a receiver.
    // Ref-RFCOMM_SPEC_V12(6.5.3)
    if (session->fcType == FC_TYPE_CREDIT) {
        LOG_DEBUG("%{public}s:Credit based fc is used, ignore FC-bit.", __func__);
        return;
    }
    channel->peerChannelFc = (modemSts->signals & MSC_FC) ? true : false;
    if (!channel->peerChannelFc) {
        RfcommSendCachePkt(channel);
        RfcommSetFlcToUpper(channel);
    }
}

/**
 * @brief After DLC is established, determine the mtu size of the opposite end.
 *
 * @param channel The pointer of the channel in the channel list.
 */
void RfcommDeterminePeerMtu(RfcommChannelInfo *channel)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSessionInfo *session = channel->session;

    // If PN communication is performed, set the minimum value between PN negotiation and L2CAP configuration.
    // If there is no PN communication, take the minimum value between the default mtu (127) of RFCOMM and
    // the value with L2CAP configuration.
    channel->peerMtu = (channel->peerMtu < (session->l2capPeerMtu - RFCOMM_HEAD_MAX_LEN)) ?
        channel->peerMtu : (session->l2capPeerMtu - RFCOMM_HEAD_MAX_LEN);
}

/**
 * @brief The device closing the last connection (DLC) on a particular session
 *        shall close the multiplexer.
 *
 * @param session The pointer of the session in the session list.
 */
void RfcommCloseInvalidSession(RfcommSessionInfo *session)
{
    LOG_INFO("%{public}s", __func__);

    // Check if the session is valid.
    bool sessionValid = RfcommCheckSessionValid(session);
    if (!sessionValid) {
        // If there is no channel on the session, close the session.
        RfcommSessionEvtFsm(session, EV_SESSION_SEND_CLOSE_REQ, NULL);
    }
}

/**
 * @brief Update the direction bit of all channels on the specified Session.
 *
 * @param session     The pointer of the session in the session list.
 * @param isInitiator Whether it is the initiator. true: initiator, false: non-initiator.
 */
void RfcommUpdateChannelDirectionBit(const RfcommSessionInfo *session, bool isInitiator)
{
    LOG_INFO("%{public}s", __func__);

    RfcommChannelInfo *channel = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(g_channelList);
    while (node != NULL) {
        channel = ListGetNodeData(node);
        node = ListGetNextNode(node);
        if (channel->session == session) {
            channel->dlci = isInitiator ? (channel->scn << 1) : ((channel->scn << 1) + 1);
        }
    }
}
