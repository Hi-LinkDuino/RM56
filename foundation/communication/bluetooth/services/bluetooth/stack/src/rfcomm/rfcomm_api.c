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

#include "platform/include/module.h"

#define WAIT_TIME (-1)

static void RfcommInitializeTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    Event *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommInitialize();

    EventSet(ctx);
}

/**
 * @brief This function is used to initialize rfcomm resources and
 *        register the callback function to L2CAP.
 *
 * @since 6
 */
static void RFCOMM_Startup()
{
    LOG_INFO("%{public}s", __func__);

    Event *ctx = EventCreate(true);

    int ret = BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, BTM_PROCESSING_QUEUE_SIZE_DEFAULT);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx);
        return;
    }

    ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommInitializeTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx);
        return;
    }

    EventWait(ctx, WAIT_TIME);
    EventDelete(ctx);
}

static void RfcommFinalizeTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    Event *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommFinalize();

    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM);

    EventSet(ctx);
}

/**
 * @brief This function is used to release rfcomm internal resources and
 *        deregister the callback function to L2CAP.
 *
 * @since 6
 */
static void RFCOMM_Shutdown()
{
    LOG_INFO("%{public}s", __func__);

    Event *ctx = EventCreate(true);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommFinalizeTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx);
        return;
    }

    EventWait(ctx, WAIT_TIME);
    EventDelete(ctx);
}

typedef struct {
    uint8_t scn;
    Event *event;
} RfcommAssignScnInfo;

static void RfcommAssignServerNumTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommAssignScnInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    ctx->scn = RfcommAssignServerNum();

    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

/**
 * @brief The function is used to assign server numbers to individual servers.
 *        Server number is used to register with the RFCOMM service interface, range is 1~30.
 *        When the return value is 0, it means that there is no available server number.
 *        This Server number shall be registered in the Service Discovery Database;
 *        RFCOMM_SPEC_V12 #5.4
 *
 * @return Server number.0(unavailable number),1~30(available number)
 * @since 6
 */
uint8_t RFCOMM_AssignServerNum()
{
    LOG_INFO("%{public}s", __func__);

    uint8_t scn = 0;
    RfcommAssignScnInfo *ctx = malloc(sizeof(RfcommAssignScnInfo));
    if (ctx == NULL) {
        return scn;
    }

    (void)memset_s(ctx, sizeof(RfcommAssignScnInfo), 0x00, sizeof(RfcommAssignScnInfo));
    ctx->event = EventCreate(true);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommAssignServerNumTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        free(ctx);
        return scn;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);
    scn = ctx->scn;

    free(ctx);
    return scn;
}

typedef struct {
    uint8_t scn;
} RfcommFreeScnInfo;

static void RfcommFreeServerNumTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommFreeScnInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommFreeServerNum(ctx->scn);

    free(ctx);
}

/**
 * @brief After close the server, free the server number.
 *
 * @param scn Server number.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_FreeServerNum(uint8_t scn)
{
    LOG_INFO("%{public}s scn:%hhu", __func__, scn);

    RfcommFreeScnInfo *ctx = malloc(sizeof(RfcommFreeScnInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->scn = scn;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommFreeServerNumTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    RfcommConnectReqInfo reqInfo;
    uint16_t handle;
    Event *event;
    int result;
} RfcommConnectDlcInfo;

static void RfcommConnectChannelTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    uint16_t handle = 0;
    RfcommConnectDlcInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    int ret = RfcommConnectChannel(&ctx->reqInfo, &handle);
    ctx->handle = handle;
    ctx->result = ret;

    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

/**
 * @brief The function is used by the client to establish a connection of the channel
 *
 * @param reqInfo   Connection request information.
 * @param handle    The handle of the channel created by rfcomm.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_ConnectChannel(const RfcommConnectReqInfo *reqInfo, uint16_t *handle)
{
    LOG_INFO("%{public}s", __func__);

    RfcommConnectDlcInfo *ctx = malloc(sizeof(RfcommConnectDlcInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommConnectDlcInfo), 0x00, sizeof(RfcommConnectDlcInfo));
    (void)memcpy_s(&ctx->reqInfo, sizeof(RfcommConnectReqInfo), reqInfo, sizeof(RfcommConnectReqInfo));
    ctx->event = EventCreate(true);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommConnectChannelTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        free(ctx);
        return ret;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);
    *handle = ctx->handle;
    ret = ctx->result;

    free(ctx);
    return ret;
}

typedef struct {
    uint8_t scn;
    uint16_t mtu;
    uint32_t eventMask;
    RFCOMM_EventCallback callback;
    void *context;
    Event *event;
    int retValue;
} RfcommRegistServerInfo;

static void RfcommRegisterServerTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRegistServerInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    int ret = RfcommRegisterServer(ctx->scn, ctx->mtu, ctx->eventMask, ctx->callback, ctx->context);
    ctx->retValue = ret;

    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
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
 * @since 6
 */
int RFCOMM_RegisterServer(uint8_t scn, uint16_t mtu, uint32_t eventMask,
                          RFCOMM_EventCallback callback, void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRegistServerInfo *ctx = malloc(sizeof(RfcommRegistServerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommRegistServerInfo), 0x00, sizeof(RfcommRegistServerInfo));
    ctx->scn = scn;
    ctx->mtu = mtu;
    ctx->eventMask = eventMask;
    ctx->callback = callback;
    ctx->context = context;
    ctx->event = EventCreate(true);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRegisterServerTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        free(ctx);
        return ret;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);
    ret = ctx->retValue;

    free(ctx);
    return ret;
}

typedef struct {
    uint16_t handle;
} RfcommAcceptConnectionInfo;

static void RfcommAcceptConnectionTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommAcceptConnectionInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommAcceptConnection(ctx->handle);

    free(ctx);
}

/**
 * @brief The function is used to tell RFCOMM to accept the connection request when the server
 *        receives the connection notification(eventId is RFCOMM_EVENT_CONNECT_INCOMING).
 *        After receiving the response from the upper layer, RFCOMM notifies the client
 *        of the peer device to accept the connection request.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_AcceptConnection(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommAcceptConnectionInfo *ctx = malloc(sizeof(RfcommAcceptConnectionInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->handle = handle;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommAcceptConnectionTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    uint16_t handle;
} RfcommRejectConnectionInfo;

static void RfcommRejectConnectionTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRejectConnectionInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommRejectConnection(ctx->handle);

    free(ctx);
}

/**
 * @brief This function is used to tell RFCOMM to reject the connection request when the server
 *        receives the connection notification(eventId is RFCOMM_EVENT_CONNECT_INCOMING).
 *        After receiving the response from the upper layer, RFCOMM notifies the client
 *        of the peer device to reject the connection request.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_RejectConnection(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommRejectConnectionInfo *ctx = malloc(sizeof(RfcommRejectConnectionInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->handle = handle;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommRejectConnectionTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    uint16_t handle;
} RfcommDisconnectChannelInfo;

static void RfcommDisconnectChannelTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommRejectConnectionInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommDisconnectChannel(ctx->handle);

    free(ctx);
}

/**
 * @brief The function is used to disconnect the specified channel.
 *        RFCOMM_SPEC_V12 #5.2
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_DisconnectChannel(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommDisconnectChannelInfo *ctx = malloc(sizeof(RfcommDisconnectChannelInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->handle = handle;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommDisconnectChannelTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    uint8_t scn;
    bool isRemoveCallback;
    Event *event;
} RfcommDeregisterServerInfo;

static void RfcommDeregisterServerTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommDeregisterServerInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommDeregisterServer(ctx->scn, ctx->isRemoveCallback);

    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

/**
 * @brief When the server is shut down, call this function to
 *        release the resources about the server held in RFCOMM.
 *
 * @param scn The server's number
 * @param isRemoveCallback true:remove callback,RFCOMM will not notify event to upper layer.otherwise:false.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_DeregisterServer(uint8_t scn, bool isRemoveCallback)
{
    LOG_INFO("%{public}s scn:%hhu, isRemoveCallback:%{public}d.", __func__, scn, isRemoveCallback);

    RfcommDeregisterServerInfo *ctx = malloc(sizeof(RfcommDeregisterServerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommDeregisterServerInfo), 0x00, sizeof(RfcommDeregisterServerInfo));
    ctx->scn = scn;
    ctx->isRemoveCallback = isRemoveCallback;
    ctx->event = EventCreate(true);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommDeregisterServerTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        free(ctx);
        return ret;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);

    free(ctx);
    return ret;
}

typedef struct {
    uint16_t handle;
    RfcommRemotePortConfig config;
} RfcommSetRpnInfo;

static void RfcommSetPortConfigTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSetRpnInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommSetPortConfig(ctx->handle, &ctx->config);

    free(ctx);
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
 * @since 6
 */
int RFCOMM_SetPortConfig(uint16_t handle, const RfcommRemotePortConfig *config)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommSetRpnInfo *ctx = malloc(sizeof(RfcommSetRpnInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommSetRpnInfo), 0x00, sizeof(RfcommSetRpnInfo));
    ctx->handle = handle;
    (void)memcpy_s(&ctx->config, sizeof(RfcommRemotePortConfig), config, sizeof(RfcommRemotePortConfig));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommSetPortConfigTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    uint16_t handle;
} RfcommReqPortConfigInfo;

static void RfcommReqPortConfigTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommReqPortConfigInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommReqPortConfig(ctx->handle);

    free(ctx);
}

/**
 * @brief The function is used to obtain the remote port negotiation information
 *        of the peer device.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_ReqPortConfig(uint16_t handle)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommReqPortConfigInfo *ctx = malloc(sizeof(RfcommReqPortConfigInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    ctx->handle = handle;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommReqPortConfigTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    uint16_t handle;
    RfcommRemoteLineStatus lineStatus;
} RfcommSendRlsInfo;

static void RfcommSendRemoteLineStatusTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSendRlsInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommSendRemoteLineStatus(ctx->handle, &ctx->lineStatus);

    free(ctx);
}

/**
 * @brief The function is used for indication of remote port line status.
 *        RFCOMM_SPEC_V12 #5.5.2
 *
 * @param handle     The channel(DLC)'s handle number
 * @param lineStatus Remote line status
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_SendRemoteLineStatus(uint16_t handle, const RfcommRemoteLineStatus *lineStatus)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommSendRlsInfo *ctx = malloc(sizeof(RfcommSendRlsInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommSendRlsInfo), 0x00, sizeof(RfcommSendRlsInfo));
    ctx->handle = handle;
    (void)memcpy_s(&ctx->lineStatus, sizeof(RfcommRemoteLineStatus), lineStatus, sizeof(RfcommRemoteLineStatus));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommSendRemoteLineStatusTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    uint16_t handle;
    RfcommModemStatus modemStatus;
} RfcommSetMscInfo;

static void RfcommSetModemStatusTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSetMscInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommSetModemStatus(ctx->handle, &ctx->modemStatus);

    free(ctx);
}

/**
 * @brief The function is used to convey the RS-232 control signals and the break signal.
 *        RFCOMM_SPEC_V12 #2.2
 *
 * @param handle      The channel(DLC)'s handle number
 * @param modemStatus Control signals and the break signal
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_SetModemStatus(uint16_t handle, const RfcommModemStatus *modemStatus)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommSetMscInfo *ctx = malloc(sizeof(RfcommSetMscInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommSetMscInfo), 0x00, sizeof(RfcommSetMscInfo));
    ctx->handle = handle;
    (void)memcpy_s(&ctx->modemStatus, sizeof(RfcommModemStatus), modemStatus, sizeof(RfcommModemStatus));

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommSetModemStatusTsk, ctx);
    if (ret != BT_NO_ERROR) {
        free(ctx);
    }

    return ret;
}

typedef struct {
    uint16_t handle;
    RfcommPortState state;
    Event *event;
    int result;
} RfcommGetPortStInfo;

static void RfcommGetPortStateTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommGetPortStInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    int ret = RfcommGetPortState(ctx->handle, &ctx->state);
    ctx->result = ret;

    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

/**
 * @brief This function is used to obtain port related information.
 *        Currently, the amount of data sent and received by the port can be obtained.
 *
 * @param handle The channel(DLC)'s handle number
 * @param state  The port's information
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_GetPortState(uint16_t handle, RfcommPortState *state)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommGetPortStInfo *ctx = malloc(sizeof(RfcommGetPortStInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommGetPortStInfo), 0x00, sizeof(RfcommGetPortStInfo));
    ctx->handle = handle;
    ctx->event = EventCreate(true);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommGetPortStateTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        free(ctx);
        return ret;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);
    (void)memcpy_s(state, sizeof(RfcommPortState), &ctx->state, sizeof(RfcommPortState));
    ret = ctx->result;

    free(ctx);
    return ret;
}

/**
 * @brief This function is used to get the payload packet sent by the peer from RFCOMM.
 *        After the caller finishes using this interface, it creates a packet reference or
 *        reads the payload buffer as needed, and must free the packet obtained from RFCOMM.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet point for receiving data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_Read(uint16_t handle, Packet **pkt)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommReadLock();
    int ret = RfcommRead(handle, pkt);
    RfcommReadUnlock();
    return ret;
}

typedef struct {
    uint16_t handle;
    Packet *pkt;
    Event *event;
    int result;
} RfcommWriteInfo;

static void RfcommWriteTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommWriteInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    int ret = RfcommWrite(ctx->handle, ctx->pkt);
    ctx->result = ret;

    if (ctx->event != NULL) {
        EventSet(ctx->event);
    }
}

/**
 * @brief This function is used to write the data to be transmitted to the opposite end to RFCOMM.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet for sending data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_Write(uint16_t handle, Packet *pkt)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommWriteInfo *ctx = malloc(sizeof(RfcommWriteInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommWriteInfo), 0x00, sizeof(RfcommWriteInfo));
    ctx->handle = handle;
    ctx->pkt = pkt;
    ctx->event = EventCreate(true);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommWriteTsk, ctx);
    if (ret != BT_NO_ERROR) {
        EventDelete(ctx->event);
        free(ctx);
        return ret;
    }

    EventWait(ctx->event, WAIT_TIME);
    EventDelete(ctx->event);
    ret = ctx->result;

    free(ctx);
    return ret;
}

typedef struct {
    uint16_t handle;
    Packet *pkt;
} RfcommSendTestCmdInfo;

static void RfcommSendTestCmdTsk(void *context)
{
    LOG_INFO("%{public}s", __func__);

    RfcommSendTestCmdInfo *ctx = context;

    if (ctx == NULL) {
        LOG_ERROR("%{public}s context is NULL.", __func__);
        return;
    }

    RfcommSendTestCmd(ctx->handle, ctx->pkt);

    PacketFree(ctx->pkt);
    free(ctx);
}

/**
 * @brief This function is used to send Test Command to the peer.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet for sending data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int RFCOMM_SendTestCmd(uint16_t handle, Packet *pkt)
{
    LOG_INFO("%{public}s handle:%hu", __func__, handle);

    RfcommSendTestCmdInfo *ctx = malloc(sizeof(RfcommSendTestCmdInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(RfcommSendTestCmdInfo), 0x00, sizeof(RfcommSendTestCmdInfo));
    ctx->handle = handle;
    ctx->pkt = PacketRefMalloc(pkt);

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_RFCOMM, RfcommSendTestCmdTsk, ctx);
    if (ret != BT_NO_ERROR) {
        PacketFree(ctx->pkt);
        free(ctx);
    }

    return ret;
}

static void RFCOMM_Init(int traceLevel)
{
    LOG_INFO("%{public}s", __func__);

    (void)traceLevel;

    return;
}

static void RFCOMM_Cleanup()
{
    LOG_INFO("%{public}s", __func__);

    return;
}

static Module g_rfcomm = {
    .name = MODULE_NAME_RFCOMM,
    .init = RFCOMM_Init,
    .startup = RFCOMM_Startup,
    .shutdown = RFCOMM_Shutdown,
    .cleanup = RFCOMM_Cleanup,
    .dependencies = {MODULE_NAME_L2CAP, MODULE_NAME_GAP},
};

MODULE_DECL(g_rfcomm)