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

static bool g_serverNum[MAX_SERVER_COUNT] = {false};

/**
 * @brief This function is used to initialize rfcomm resources and
 *        register the callback function to L2CAP.
 *
 */
void RfcommInitialize()
{
    LOG_INFO("%{public}s", __func__);

    for (uint8_t cnt = 0; cnt < MAX_SERVER_COUNT; cnt++) {
        g_serverNum[cnt] = false;
    }

    // Create list.
    RfcommCreateSessionList();
    RfcommCreateChannelList();
    RfcommCreateServerList();

    // Register security to GAP.
    RfcommRegisterSecurity();

    // Register to L2CAP.
    RfcommRegisterL2cap();
}

/**
 * @brief This function is used to release rfcomm internal resources and
 *        deregister the callback function to L2CAP.
 *
 */
void RfcommFinalize()
{
    LOG_INFO("%{public}s", __func__);

    for (uint8_t cnt = 0; cnt < MAX_SERVER_COUNT; cnt++) {
        g_serverNum[cnt] = false;
    }

    // Deregister from L2CAP.
    RfcommDeregisterL2cap();

    // Deregister security from GAP.
    RfcommDeregisterSecurity();

    // Free list.
    RfcommDestroySessionList();
    RfcommDestroyChannelList();
    RfcommDestroyServerList();
}

/**
 * @brief The function is used to assign server numbers to individual servers.
 *        Server number is used to register with the RFCOMM service interface, range is 1~30.
 *        When the return value is 0, it means that there is no available server number.
 *        This Server number shall be registered in the Service Discovery Database;
 *        RFCOMM_SPEC_V12 #5.4
 *
 * @return Server number.0(unavailable number),1~30(available number)
 */
uint8_t RfcommAssignServerNum()
{
    LOG_INFO("%{public}s", __func__);

    uint8_t scn = 0;

    for (uint8_t index = 0; index < MAX_SERVER_COUNT; index++) {
        if (g_serverNum[index] == true) {
            continue;
        }
        g_serverNum[index] = true;
        scn = index + 1;
        break;
    }

    return scn;
}

/**
 * @brief After close the server, free the server number.
 *
 * @param scn Server number.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommFreeServerNum(uint8_t scn)
{
    LOG_INFO("%{public}s scn:%hhu", __func__, scn);

    if ((scn < 1) || (scn > MAX_SERVER_COUNT)) {
        return RFCOMM_ERR_PARAM;
    }

    g_serverNum[scn - 1] = false;

    return RFCOMM_SUCCESS;
}

/**
 * @brief The function is used by the client to establish a connection of the channel.
 *
 * @param reqInfo   Connection request information.
 * @param handle    The handle of the channel created by rfcomm
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommConnectChannel(const RfcommConnectReqInfo *reqInfo, uint16_t *handle)
{
    LOG_INFO("%{public}s", __func__);

    // The valid range of server num is 1~30.
    // If the incoming parameter scn is greater than 30, an exception will be returned.
    if ((reqInfo->scn == 0) || (reqInfo->scn > MAX_SERVER_COUNT)) {
        return RFCOMM_ERR_PARAM;
    }

    // Find session by BT address.If the session does not exist, create the session.
    RfcommSessionInfo *session = RfcommGetSessionByAddr(&(reqInfo->addr));
    if (session == NULL) {
        // If the session does not exist, create a session.
        session = RfcommCreateSession(&(reqInfo->addr), 0, 0, true);
        if (session == NULL) {
            return RFCOMM_ERR_NO_RESOURCES;
        }
    }

    // Server applications on the noninitiating device are reachable on DLCIs 2,4,6,…,60;
    // and server applications on the initiating device are reachable on DLCIs 3,5,7,…,61.
    uint8_t dlci = session->isInitiator ? (reqInfo->scn << 1) : ((reqInfo->scn << 1) + 1);

    // Determine whether the channel has been created.
    RfcommChannelInfo *channel = RfcommGetChannelByDlci(session, dlci);
    if (channel != NULL) {
        // Channel already exists.
        *handle = channel->handle;
        return RFCOMM_ALREADY_EXIST;
    }

    RfcommCreateChannelInfo createChannelInfo;
    createChannelInfo.session = session;
    createChannelInfo.isServer = false;
    createChannelInfo.dlci = dlci;
    createChannelInfo.mtu = reqInfo->mtu;
    createChannelInfo.eventMask = reqInfo->eventMask;
    createChannelInfo.callback = reqInfo->callback;
    createChannelInfo.context = reqInfo->context;

    // If the channel does not exist, create a channel.
    channel = RfcommCreateChannel(&createChannelInfo);
    if (channel == NULL) {
        return RFCOMM_ERR_NO_RESOURCES;
    }
    *handle = channel->handle;

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_OPEN_REQ, NULL);
}

/**
 * @brief The function is used for the server to register with RFCOMM and
 *        wait for the client to connect.
 *
 * @param scn       The server's number.
 * @param mtu       The maximum size of data received at a time.
 * @param eventMask The collection of events followed by upper layers.
 * @param callback  The callback function used by rfcomm to notify uppers of data or events.
 * @param context   The content passed in from the upper layer.
 *                  It will be brought back to the upper layer when callback is called.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRegisterServer(uint8_t scn, uint16_t mtu, uint32_t eventMask,
                         RFCOMM_EventCallback callback, void *context)
{
    LOG_INFO("%{public}s scn:%hhu", __func__, scn);

    RfcommServerInfo *server = NULL;

    // The valid range of server num is 1~30.
    // If the incoming parameter scn is greater than 30, an exception will be returned.
    if ((scn == 0) || (scn > MAX_SERVER_COUNT)) {
        return RFCOMM_ERR_PARAM;
    }

    // Determine whether the server has been registered.
    server = RfcommGetServerByScn(scn);
    if (server != NULL) {
        return RFCOMM_ALREADY_EXIST;
    }
    // If it is not registered, proceed with the registration process.
    server = RfcommCreateServer(scn, mtu, eventMask, callback, context);
    if (server == NULL) {
        return RFCOMM_ERR_NO_RESOURCES;
    }

    return RFCOMM_SUCCESS;
}

/**
 * @brief The function is used to tell RFCOMM to accept the connection request when the server
 *        receives the connection notification(eventId is RFCOMM_EVENT_CONNECT_INCOMING).
 *        After receiving the response from the upper layer, RFCOMM notifies the client
 *        of the peer device to accept the connection request.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommAcceptConnection(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_ACCEPT, NULL);
}

/**
 * @brief This function is used to tell RFCOMM to reject the connection request when the server
 *        receives the connection notification(eventId is RFCOMM_EVENT_CONNECT_INCOMING).
 *        After receiving the response from the upper layer, RFCOMM notifies the client
 *        of the peer device to reject the connection request.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRejectConnection(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_REJECT, NULL);
}

/**
 * @brief The function is used to disconnect the specified channel.
 *        RFCOMM_SPEC_V12 #5.2
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommDisconnectChannel(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_CLOSE_REQ, NULL);
}

/**
 * @brief When the server is shut down, call this function to
 *        release the resources about the server held in RFCOMM.
 *
 * @param scn The server's number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommDeregisterServer(uint8_t scn, bool isRemoveCallback)
{
    LOG_INFO("%{public}s scn:%hhu", __func__, scn);

    RfcommServerInfo *server = RfcommGetServerByScn(scn);

    if (server != NULL) {
        RfcommRemoveServer(server);
    }

    // Remove callback from channel list and disconnect the channel.
    if (isRemoveCallback) {
        RfcommRemoveChannelCallback(scn);
    }

    return RFCOMM_SUCCESS;
}

/**
 * @brief The function is used for set the remote port communication settings.
 *        The command may be used before a new DLC is opened and shall be used
 *        whenever the port settings change.
 *        RFCOMM_SPEC_V12 #5.5.1
 *
 * @param handle The channel(DLC)'s handle number
 * @param config Remote port negotiation parameters
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSetPortConfig(uint16_t handle, const RfcommRemotePortConfig *config)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_RPN_CMD, (void *)config);
}

/**
 * @brief The function is used to obtain the remote port negotiation information
 *        of the peer device.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommReqPortConfig(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_RPN_REQ, NULL);
}

/**
 * @brief The function is used for indication of remote port line status.
 *        RFCOMM_SPEC_V12 #5.5.2
 *
 * @param handle     The channel(DLC)'s handle number
 * @param lineStatus Remote line status
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendRemoteLineStatus(uint16_t handle, const RfcommRemoteLineStatus *lineStatus)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_RLS_REQ, (void *)lineStatus);
}

/**
 * @brief The function is used to convey the RS-232 control signals and the break signal.
 *        RFCOMM_SPEC_V12 #2.2
 *
 * @param handle      The channel(DLC)'s handle number
 * @param modemStatus Control signals and the break signal
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSetModemStatus(uint16_t handle, const RfcommModemStatus *modemStatus)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_SEND_MSC_REQ, (void *)modemStatus);
}

/**
 * @brief This function is used to obtain port related information.
 *        Currently, the amount of data sent and received by the port can be obtained.
 *
 * @param handle The channel(DLC)'s handle number
 * @param state  The port's information
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommGetPortState(uint16_t handle, RfcommPortState *state)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }

    state->receivedBytes = channel->receivedBytes;
    state->transmittedBytes = channel->transmittedBytes;

    return RFCOMM_SUCCESS;
}

/**
 * @brief This function is used to get the payload packet sent by the peer from RFCOMM.
 *        After the caller finishes using this interface, it creates a packet reference or
 *        reads the payload buffer as needed, and must free the packet obtained from RFCOMM.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet point for receiving data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommRead(uint16_t handle, Packet **pkt)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    ListNode *node = NULL;
    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        return RFCOMM_ERR_PARAM;
    }
    node = ListGetFirstNode(channel->recvQueue);
    if (node == NULL) {
        *pkt = NULL;
        return RFCOMM_NO_DATA;
    }

    *pkt = (Packet *)ListGetNodeData(node);
    // Add received data bytes value.
    channel->receivedBytes += PacketPayloadSize(*pkt);
    // Remove first node.
    ListRemoveFirst(channel->recvQueue);

    // Local can receive more data, send flow control to peer.
    RfcommSetFlcToPeer(channel, true);

    return RFCOMM_SUCCESS;
}

/**
 * @brief This function is used to write the data to be transmitted to the opposite end to RFCOMM.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet for sending data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommWrite(uint16_t handle, Packet *pkt)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    size_t len = PacketPayloadSize(pkt);
    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        LOG_ERROR("%{public}s:Channel does not exist.", __func__);
        return RFCOMM_ERR_PARAM;
    }

    if (len > channel->peerMtu) {
        LOG_ERROR("%{public}s:Packet length(%zu) is over MTU.", __func__, len);
        return RFCOMM_OVERRUN;
    }

    return RfcommChannelEvtFsm(channel, EV_CHANNEL_WRITE_DATA, pkt);
}

/**
 * @brief This function is used to send Test Command to the peer.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet for sending data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendTestCmd(uint16_t handle, Packet *pkt)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    size_t len = PacketPayloadSize(pkt);
    RfcommChannelInfo *channel = RfcommGetChannelByHandle(handle);

    if (channel == NULL) {
        LOG_ERROR("%{public}s:The channel does not exist.", __func__);
        return RFCOMM_ERR_PARAM;
    }

    if (len > channel->peerMtu) {
        LOG_ERROR("%{public}s:Test cmd packet length(%zu) is over MTU.", __func__, len);
        return RFCOMM_OVERRUN;
    }

    return RfcommSendUihTest(channel->session, true, pkt);
}