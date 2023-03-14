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

#include "avdtp_api.h"
#include "avdtp_ctrl.h"
#include "log.h"
#include "securec.h"

/**
 *
 * @brief        AVDT_Register
 *
 * @details      This function initializes AVDTP and prepares the protocol
 *               stack for its use.  It must be called once by the system
 *               or platform using AVDT before the other functions of the
 *               API an be used.
 *
 * @return       void
 *
 */
void AVDT_Register(const AvdtRegisterParam *reg)
{
    LOG_INFO("[AVDT]%{public}s:", __func__);
    Event *event = EventCreate(true);
    AvdtRegisterTskParam *param = malloc(sizeof(AvdtRegisterTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return;
    }
    (void)memset_s(param, sizeof(AvdtRegisterTskParam), 0, sizeof(AvdtRegisterTskParam));
    param->reg = reg;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtRegisterTsk, param)) {
        EventWait(event, WAIT_TIME);
    }
    free(param);
    EventDelete(event);
    return;
}

void AvdtRegisterTsk(void *context)
{
    AvdtRegisterTskParam *param = (AvdtRegisterTskParam *)context;
    AvdtRegister(param->reg);
    EventSet(param->event);
    return;
}

/**
 *
 * @brief        AVDT_Deregister
 *
 * @details      This function is called to deregister AVDTP protocol.
 *               This function can be called after all streams must be
 *               removed.
 *
 * @return       void
 *
 */
void AVDT_Deregister(void)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    return;
}

/**
 *
 * @brief          AVDT_CreateStream
 *
 * @details        Create a stream endpoint. An application can initiate a
 *                 connection between this endpoint and an endpoint on a peer
 *                 device after a stream endpoint is created.
 * Pram[in]        bdAddr   Peer device address.
 * Pram[out]       handle   Stream channel handle.Start from 1.
 * Pram[in]        codecIndex   Sep index. Start form 1.
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_CreateStream(const BtAddr *bdAddr, uint16_t *handle, uint16_t codecIndex)
{
    LOG_INFO("[AVDT]%{public}s:bdAddr((%02x:%02x:%02x:%02x:%02x:%02x) codecIndex(%hu)",
        __func__,
        BT_ADDR_FMT_DSC(bdAddr->addr),
        codecIndex);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtCreateStreamTskParam *param = malloc(sizeof(AvdtCreateStreamTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtCreateStreamTskParam), 0, sizeof(AvdtCreateStreamTskParam));
    param->bdAddr = bdAddr;
    param->handle = handle;
    param->codecIndex = codecIndex;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtCreateStreamTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtCreateStreamTsk(void *context)
{
    AvdtCreateStreamTskParam *param = (AvdtCreateStreamTskParam *)context;
    param->ret = AvdtCreateStream(param->bdAddr, param->handle, param->codecIndex);
    EventSet(param->event);
    return;
}

uint16_t AvdtCreateStream(const BtAddr *bdAddr, uint16_t *handle, uint16_t codecIndex)
{
    LOG_DEBUG("[AVDT]%{public}s:bdAddr((%02x:%02x:%02x:%02x:%02x:%02x) codecIndex(%hu)",
        __func__,
        BT_ADDR_FMT_DSC(bdAddr->addr),
        codecIndex);
    uint16_t Ret = AVDT_SUCCESS;
    if (codecIndex < 1) {
        Ret = AVDT_BAD_PARAMS;
        return Ret;
    }
    /* judge avdt Ind,Cfm callback and service capabilities
       Allocate channel ctrl and stream ctrl */
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        sigCtrl = AvdtSigCtrlAllocate(bdAddr, AVDT_ROLE_UNKOWN);
        if (sigCtrl == NULL) {
            Ret = AVDT_NO_RESOURCES;
            return Ret;
        }
    }
    /* check the sep had been registered */
    *handle = AvdtCheckSepIsUsed(sigCtrl, codecIndex);
    if (*handle != 0) {
        LOG_INFO("[AVDT]%{public}s: Index(%hu) registered, registered handle(%hu)", __func__, codecIndex, *handle);
        return Ret;
    }
    /* Create stream control block */
    AvdtStreamCtrl *streamCtrl = AvdtStreamCtrlAllocate(sigCtrl, codecIndex);
    if (streamCtrl == NULL) {
        Ret = AVDT_NO_RESOURCES;
    } else {
        *handle = streamCtrl->handle;
        LOG_INFO("[AVDT]%{public}s: Index(%hu) registered, registered handle(%hu)", __func__, codecIndex, *handle);
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_RegisterLocalSEP
 *
 * @details        Register local host stream endpoint information
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_RegisterLocalSEP(AvdtStreamConfig *avdtStreamConfig, uint8_t number)
{
    LOG_INFO("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtRegisterLocalSEPTskParam *param = malloc(sizeof(AvdtRegisterLocalSEPTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtRegisterLocalSEPTskParam), 0, sizeof(AvdtRegisterLocalSEPTskParam));
    param->avdtStreamConfig = avdtStreamConfig;
    param->number = number;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtRegisterLocalSEPTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtRegisterLocalSEPTsk(void *context)
{
    AvdtRegisterLocalSEPTskParam *param = (AvdtRegisterLocalSEPTskParam *)context;
    param->ret = AvdtRegisterLocalSEP(param->avdtStreamConfig, param->number);
    EventSet(param->event);
    return;
}

/**
 *
 * @brief          AVDT_RemoveStream
 *
 * @details        Remove a stream endpoint.  If this function is called when
 *                 the endpoint is connected the connection is closed and then
 *                 the stream endpoint is removed.
 *
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_RemoveStream(uint16_t handle)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief          AVDT_DiscoverReq
 *
 * @details        This function send AVDT_DISCOVER_CMD from INT to ACP and it
 *                 request to get the overview of all SEP information of the ACP.
 *
 *                 When discovery is complete, an AVDT_DISCOVER_CFM_EVT
 *                 is sent to the application via its callback function.
 *                 The application must not call AVDT_GetCapReq() or
 *                 AVDT_DiscoverReq() again to the same device until
 *                 discovery is complete.
 *                 AVDTP.ICS #Table 4-1, 4b-1 Stream discover command
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_DiscoverReq(const BtAddr *bdAddr, uint8_t maxSeps, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s: type (%hhu)Addr(%x:%x:%x:%x:%x:%x), maxSeps(%hhu)",
        __func__,
        bdAddr->type,
        BT_ADDR_FMT_DSC(bdAddr->addr),
        maxSeps);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtDiscoverReqTskParam *param = malloc(sizeof(AvdtDiscoverReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtDiscoverReqTskParam), 0, sizeof(AvdtDiscoverReqTskParam));
    param->bdAddr = bdAddr;
    param->maxSeps = maxSeps;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtDiscoverReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtDiscoverReqTsk(void *context)
{
    AvdtDiscoverReqTskParam *param = (AvdtDiscoverReqTskParam *)context;
    param->ret = AvdtDiscoverReq(param->bdAddr, param->maxSeps, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtDiscoverReq(const BtAddr *bdAddr, uint8_t maxSeps, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s: type (%hhu)Addr(%x:%x:%x:%x:%x:%x), maxSeps(%hhu)",
        __func__,
        bdAddr->type,
        BT_ADDR_FMT_DSC(bdAddr->addr),
        maxSeps);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        sigCtrl = AvdtSigCtrlAllocate(bdAddr, AVDT_ROLE_UNKOWN);
        if (sigCtrl == NULL) {
            /* Trace:No resources */
            Ret = AVDT_NO_RESOURCES;
            LOG_ERROR("[AVDT]%{public}s: No SigCtrl resource!", __func__);
        }
    }
    if (Ret == AVDT_SUCCESS) {
        if (sigCtrl->isProcBusy) {
            /* Disable call this api again before the respond is feedback */
            Ret = AVDT_BUSY;
            /* Trace: disable call the api */
            LOG_WARN("[AVDT]%{public}s: Proce Busy!", __func__);
        } else {
            AvdtEventData evt = {0};
            evt.discover.numSeps = maxSeps;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtSigProcEvent(sigCtrl, AVDT_DISCOVER_CMD_REQ_EVENT, &evt);
        }
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_DiscoverRsp
 *
 * @details        Respond to a discover request from ACP.
 *                 Test suit only.
 *                 AVDTP.ICS #Table 10-1, 10b-1 Stream discover response
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_DiscoverRsp(
    const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepInfo *sepInfo, uint8_t numSeps)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief          AVDT_GetCapReq
 *
 * @details        This function send AVDT_GETCAP_REQ from INT to ACP and it
 *                 used to get the information of a SEP of ACP.
 *
 *                 When the procedure is complete, an AVDT_GETCAP_CFM_EVT is
 *                 sent to the application via its callback function.  The
 *                 application must not call AVDT_GetCapReq() or
 *                 AVDT_DiscoverReq() again until the procedure is complete.
 *                 AVDTP.ICS #Table 4-2, 4b-2 Stream get capabilities command
 *                 AVDTP.ICS #Table 4-6, 4b-6 Stream get all capabilities
 *                 command.
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_GetCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s: acpSeid(%hhu)", __func__, acpSeid);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtGetCapReqTskParam *param = malloc(sizeof(AvdtGetCapReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtGetCapReqTskParam), 0, sizeof(AvdtGetCapReqTskParam));
    param->bdAddr = bdAddr;
    param->acpSeid = acpSeid;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtGetCapReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtGetCapReqTsk(void *context)
{
    AvdtGetCapReqTskParam *param = (AvdtGetCapReqTskParam *)context;
    param->ret = AvdtGetCapReq(param->bdAddr, param->acpSeid, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtGetCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s: acpSeid(%hhu)", __func__, acpSeid);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        sigCtrl = AvdtSigCtrlAllocate(bdAddr, AVDT_ROLE_UNKOWN);
        if (sigCtrl == NULL) {
            /* Trace:No resources */
            Ret = AVDT_NO_RESOURCES;
            LOG_ERROR("[AVDT]%{public}s: AvdtSigCtrlAllocate failed!", __func__);
        }
    }
    if (Ret == AVDT_SUCCESS) {
        if (sigCtrl->isProcBusy) {
            /* Disable call this api again before the respond is feedback */
            Ret = AVDT_BUSY;
            /* Trace: disable call the api */
            LOG_WARN("[AVDT]%{public}s: Proce Busy!", __func__);
        } else {
            AvdtEventData evt = {0};
            evt.getcap.single.seid = acpSeid;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtSigProcEvent(sigCtrl, AVDT_GETCAP_CMD_REQ_EVENT, &evt);
        }
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_GetCapRsp
 *
 * @details        Respond to a getcap request from INT. Test suit only.
 *                 AVDTP.ICS #Table 10-2, 10b-2 Stream get capabilities
 *                 response.
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_GetCapRsp(const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap)
{
    LOG_INFO("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtGetCapRspTskParam *param = malloc(sizeof(AvdtGetCapRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtGetCapRspTskParam), 0, sizeof(AvdtGetCapRspTskParam));
    param->bdAddr = bdAddr;
    param->transLabel = transLabel;
    param->errCode = errCode;
    param->sepCap = sepCap;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtGetCapRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtGetCapRspTsk(void *context)
{
    AvdtGetCapRspTskParam *param = (AvdtGetCapRspTskParam *)context;
    param->ret = AvdtGetCapRsp(param->bdAddr, param->transLabel, param->errCode, param->sepCap);
    EventSet(param->event);
    return;
}

uint16_t AvdtGetCapRsp(const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        Ret = AVDT_BAD_PARAMS;
    } else {
        AvdtEventData event = {0};
        event.getcap.cfg = sepCap;
        event.getcap.single.errCode = errCode;
        AvdtSigProcEvent(sigCtrl, AVDT_GETCAP_CMD_RSP_EVENT, &event);
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_GetAllCapReq
 *
 * @details        This function send AVDT_GETALLCAP_REQ from INT to ACP and it
 *                 used to get the information of a SEP of ACP.
 *
 *                 When the procedure is complete, an AVDT_GETALLCAP_CFM_EVT is
 *                 sent to the application via its callback function.  The
 *                 application must not call AVDT_GetAllCapReq() or
 *                 AVDT_DiscoverReq() again until the procedure is complete.
 *                 AVDTP.ICS #Table 4-2, 4b-2 Stream get capabilities command
 *                 AVDTP.ICS #Table 4-6, 4b-6 Stream get all capabilities
 *                 command.
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_GetAllCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s: acpSeid(%hhu)", __func__, acpSeid);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtGetAllCapReqTskParam *param = malloc(sizeof(AvdtGetAllCapReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtGetAllCapReqTskParam), 0, sizeof(AvdtGetAllCapReqTskParam));
    param->bdAddr = bdAddr;
    param->acpSeid = acpSeid;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtGetAllCapReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtGetAllCapReqTsk(void *context)
{
    AvdtGetAllCapReqTskParam *param = (AvdtGetAllCapReqTskParam *)context;
    param->ret = AvdtGetAllCapReq(param->bdAddr, param->acpSeid, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtGetAllCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s: acpSeid(%hhu)", __func__, acpSeid);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        sigCtrl = AvdtSigCtrlAllocate(bdAddr, AVDT_ROLE_UNKOWN);
        if (sigCtrl == NULL) {
            /* Trace:No resources */
            Ret = AVDT_NO_RESOURCES;
            LOG_ERROR("[AVDT]%{public}s: AvdtSigCtrlAllocate failed!", __func__);
        }
    }
    if (Ret == AVDT_SUCCESS) {
        if (sigCtrl->isProcBusy) {
            /* Disable call this api again before the respond is feedback */
            Ret = AVDT_BUSY;
            /* Trace: disable call the api */
            LOG_WARN("[AVDT]%{public}s: Proce Busy!", __func__);
        } else {
            AvdtEventData evt = {0};
            evt.getcap.single.seid = acpSeid;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtSigProcEvent(sigCtrl, AVDT_GETALLCAP_CMD_REQ_EVENT, &evt);
        }
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_GetAllCapRsp
 *
 * @details        Respond to a getcap request from INT. Test suit only.
 *                 AVDTP.ICS #Table 10-2, 10b-2 Stream get capabilities
 *                 response.
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_GetAllCapRsp(const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap)
{
    LOG_INFO("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtGetAllCapRspTskParam *param = malloc(sizeof(AvdtGetAllCapRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtGetAllCapRspTskParam), 0, sizeof(AvdtGetAllCapRspTskParam));
    param->bdAddr = bdAddr;
    param->transLabel = transLabel;
    param->errCode = errCode;
    param->sepCap = sepCap;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtGetAllCapRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtGetAllCapRspTsk(void *context)
{
    AvdtGetAllCapRspTskParam *param = (AvdtGetAllCapRspTskParam *)context;
    param->ret = AvdtGetAllCapRsp(param->bdAddr, param->transLabel, param->errCode, param->sepCap);
    EventSet(param->event);
    return;
}

uint16_t AvdtGetAllCapRsp(const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        Ret = AVDT_BAD_PARAMS;
    } else {
        AvdtEventData event = {0};
        event.getcap.cfg = sepCap;
        event.getcap.single.errCode = errCode;
        AvdtSigProcEvent(sigCtrl, AVDT_GETALLCAP_CMD_RSP_EVENT, &event);
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_DelayReq
 *
 * @details        This functions sends a Delay Report to ACP
 *                 that is associated with a particular SEID.
 *                 This function is called by SNK device.
 *                 AVDTP.ICS #Table 15-6 Delay Reporting
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_DelayReq(uint16_t handle, uint8_t *transLabel, uint16_t delayValue)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu),delayValue(%hu)", __func__, handle, delayValue);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtDelayReqTskParam *param = malloc(sizeof(AvdtDelayReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtDelayReqTskParam), 0, sizeof(AvdtDelayReqTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    param->delayValue = delayValue;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtDelayReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtDelayReqTsk(void *context)
{
    AvdtDelayReqTskParam *param = (AvdtDelayReqTskParam *)context;
    param->ret = AvdtDelayReq(param->handle, param->transLabel, param->delayValue);
    EventSet(param->event);
    return;
}

uint16_t AvdtDelayReq(uint16_t handle, uint8_t *transLabel, uint16_t delayValue)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu),delayValue(%hu)", __func__, handle, delayValue);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.apiDelay.delay = delayValue;
            evt.apiDelay.hdr.seid = streamCtrl->peerSeid;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtStreamProcEvent(streamCtrl, AVDT_DELAY_CMD_REQ_EVENT, &evt);
        }
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_DelayRsp
 *
 * @details        Respond to a delay request from the peer device.  This
 *                 function must be called if the application receives an
 *                 AVDT_DELAY_IND_EVT through its control callback.
 *                 AVDTP.ICS #Table 14-6 Delay Reporting.
 *                 The feature is not supported ,only for test suite
 *
 * @return          AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_DelayRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu),transLabel(%hhu),errCode(%hhu)", __func__, handle, transLabel, errCode);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtDelayRspTskParam *param = malloc(sizeof(AvdtDelayRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtDelayRspTskParam), 0, sizeof(AvdtDelayRspTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    param->errCode = errCode;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtDelayRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtDelayRspTsk(void *context)
{
    AvdtDelayRspTskParam *param = (AvdtDelayRspTskParam *)context;
    param->ret = AvdtDelayRsp(param->handle, param->transLabel, param->errCode);
    EventSet(param->event);
    return;
}

uint16_t AvdtDelayRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu),transLabel(%hhu),errCode(%hhu)", __func__, handle, transLabel, errCode);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl != NULL) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.errCode = errCode;
            evt.msg.single.label = transLabel;
            AvdtStreamProcEvent(streamCtrl, AVDT_DELAY_CMD_RSP_EVENT, &evt);
        } else {
            Ret = AVDT_BAD_PARAMS;
            LOG_ERROR("[AVDT]%{public}s:AvdtGetSigCtrlByHandle(%hhu) Failed!", __func__, streamCtrl->sigHandle);
        }
    } else {
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_OpenReq
 *
 * @details        This function send the AVDT_OPEN_REQ from INT to ACP and
 *                 create the stream between INT and ACP.When the connection
 *                 is completed, an AVDT_OPEN_CFM_EVT is sent to the
 *                 application via the control callback function for this
 *                 handle.
 *                 AVDTP.ICS #Table 5-1,5b-1 Open stream command
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_OpenReq(uint16_t handle, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtOpenReqTskParam *param = malloc(sizeof(AvdtOpenReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtOpenReqTskParam), 0, sizeof(AvdtOpenReqTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtOpenReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtOpenReqTsk(void *context)
{
    AvdtOpenReqTskParam *param = (AvdtOpenReqTskParam *)context;
    param->ret = AvdtOpenReq(param->handle, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtOpenReq(uint16_t handle, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.seid = streamCtrl->peerSeid;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtStreamProcEvent(streamCtrl, AVDT_OPEN_CMD_REQ_EVENT, &evt);
        } else {
            Ret = AVDT_BAD_PARAMS;
            LOG_ERROR("[AVDT]%{public}s:AvdtGetSigCtrlByHandle(%hhu) Failed!", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_OpenRsp
 *
 * @details        Respond to an open request from ACP.  This function must be
 *                 called if the application receives an AVDT_OPEN_IND_EVT
 *                 through its control callback.
 *                 AVDTP.ICS #Table 11-1, 11b-1 Open stream response
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_OpenRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief          AVDT_SetConfigReq
 *
 * @details        This function send AVDT_CONFIG_CMD from INT to ACP. When
 *                 the config is completed, an AVDT_CONFIG_CFM_EVT is sent to
 *                 the application via the control callback function for this
 *                 handle.
 *                 AVDTP.ICS #Table 4-3, 4b-3 Set configuration command
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_SetConfigReq(uint16_t handle, uint8_t seid, AvdtSepConfig *sepConfig, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu),seid(%hhu)", __func__, handle, seid);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtSetConfigReqTskParam *param = malloc(sizeof(AvdtSetConfigReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtSetConfigReqTskParam), 0, sizeof(AvdtSetConfigReqTskParam));
    param->handle = handle;
    param->seid = seid;
    param->sepConfig = sepConfig;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtSetConfigReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtSetConfigReqTsk(void *context)
{
    AvdtSetConfigReqTskParam *param = (AvdtSetConfigReqTskParam *)context;
    param->ret = AvdtSetConfigReq(param->handle, param->seid, param->sepConfig, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtSetConfigReq(uint16_t handle, uint8_t seid, AvdtSepConfig *sepConfig, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu),seid(%hhu)", __func__, handle, seid);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = NULL;
    if (seid == 0) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
    }
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    } else {
        sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl == NULL) {
            Ret = AVDT_BAD_PARAMS;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed!", __func__, streamCtrl->sigHandle);
        }
    }
    if (AVDT_SUCCESS == Ret) {
        AvdtIsEdr2MbMode(&sigCtrl->peerAddress);
        AvdtIsEdr3MbMode(&sigCtrl->peerAddress);
        AvdtStreamConfig *streamConfig = AvdtGetSepConfigByCodecIndex(streamCtrl->codecIndex);
        if ((streamConfig != NULL) && (sigCtrl->role != streamConfig->sepType)) {
            sigCtrl->role = streamConfig->sepType;
        }
        AvdtEventData evt = {0};
        evt.msg.configureCmd.acpSeid = seid;
        streamCtrl->peerSeid = seid;
        evt.msg.configureCmd.intSeid = (uint8_t)handle;
        (void)memcpy_s(&streamCtrl->reqCfg, sizeof(AvdtSepConfig), sepConfig, sizeof(AvdtSepConfig));
        *transLabel = AvdtCreateTransLabel(sigCtrl);
        AvdtStreamProcEvent(streamCtrl, AVDT_SETCONFIGURE_CMD_REQ_EVENT, &evt);
    }
    return Ret;
}

/**
 *
 * @brief          AVDT_SetConfigRsp
 *
 * @details        Respond to a configure request from ACP.  This
 *                 function must be called if the application receives an
 *                 AVDT_CONFIG_IND_EVT through its control callback.
 *                 AVDTP.ICS #Table 10-3, 10b-3 Set configuration response
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_SetConfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu),transLabel(%hhu)", __func__, handle, transLabel);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtSetConfigRspTskParam *param = malloc(sizeof(AvdtSetConfigRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtSetConfigRspTskParam), 0, sizeof(AvdtSetConfigRspTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    (void)memcpy_s(&param->category, sizeof(AvdtCatetory), &category, sizeof(AvdtCatetory));
    param->event = event;
    if (!AvdtAsyncProcess(AvdtSetConfigRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtSetConfigRspTsk(void *context)
{
    AvdtSetConfigRspTskParam *param = (AvdtSetConfigRspTskParam *)context;
    param->ret = AvdtSetConfigRsp(param->handle, param->transLabel, param->category);
    EventSet(param->event);
    return;
}

uint16_t AvdtSetConfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu),transLabel(%hhu)", __func__, handle, transLabel);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl != NULL) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.configureCmd.hdr.errCode = category.errCode;
            evt.msg.configureCmd.hdr.label = transLabel;
            evt.msg.configureCmd.hdr.errParam = category.category;
            AvdtStreamProcEvent(streamCtrl, AVDT_SETCONFIGURE_CMD_RSP_EVENT, &evt);
        }
    } else {
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle Failed,input parameter invalid handl(%hu)", __func__, handle);
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_GetConfigReq
 *
 * @details       This function send AVDT_GETCONFIG_CMD from INT to ACP
 *                to get the current configuration of a SEP in ACP. When
 *                the config is completed, an AVDT_GETCONFIG_CFM_EVT is
 *                sent to the application via the control callback function.
 *                AVDTP.ICS #Table 4-4, 4b-4 Get configuration command
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_GetConfigReq(uint16_t handle, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtGetConfigReqTskParam *param = malloc(sizeof(AvdtGetConfigReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtGetConfigReqTskParam), 0, sizeof(AvdtGetConfigReqTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtGetConfigReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtGetConfigReqTsk(void *context)
{
    AvdtGetConfigReqTskParam *param = (AvdtGetConfigReqTskParam *)context;
    param->ret = AvdtGetConfigReq(param->handle, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtGetConfigReq(uint16_t handle, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.seid = streamCtrl->peerSeid;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtStreamProcEvent(streamCtrl, AVDT_GETCONFIGURE_CMD_REQ_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_StartReq
 *
 * @details       Start one or more stream endpoints(audio/video). All stream
 *                endpoints must previously be opened.  When the streams are
 *                started, an AVDT_START_CFM_EVT is sent to the application
 *                via the control callback function for each stream.
 *                AVDTP.ICS #Table 5-2, 5b-2 Start stream command
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_StartReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s:handles[0] is %hu", __func__, handles[0]);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtStartReqTskParam *param = malloc(sizeof(AvdtStartReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtStartReqTskParam), 0, sizeof(AvdtStartReqTskParam));
    param->handles = handles;
    param->numHandles = numHandles;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtStartReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtStartReqTsk(void *context)
{
    AvdtStartReqTskParam *param = (AvdtStartReqTskParam *)context;
    param->ret = AvdtStartReq(param->handles, param->numHandles, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtStartReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s:handles[0] is %hu", __func__, handles[0]);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handles[0]);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handles[0]);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.seid = streamCtrl->peerSeid;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtStreamProcEvent(streamCtrl, AVDT_START_CMD_REQ_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_StartRsp
 *
 * @details       Respond to a start request from ACP.  This function must
 *                be called if the application receives an AVDT_START_IND_EVT
 *                through its control callback.
 *                AVDTP.ICS #Table 11-2, 11b-2 Start stream command
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_StartRsp(uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode)
{
    LOG_INFO("[AVDT]%{public}s:transLabel(%hhu),firstHandle(%hu),firstFailHandle(%hu)",
        __func__,
        transLabel,
        firstHandle,
        firstFailHandle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtStartRspTskParam *param = malloc(sizeof(AvdtStartRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtStartRspTskParam), 0, sizeof(AvdtStartRspTskParam));
    param->firstHandle = firstHandle;
    param->transLabel = transLabel;
    param->firstFailHandle = firstFailHandle;
    param->errCode = errCode;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtStartRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtStartRspTsk(void *context)
{
    AvdtStartRspTskParam *param = (AvdtStartRspTskParam *)context;
    param->ret = AvdtStartRsp(param->firstHandle, param->transLabel, param->firstFailHandle, param->errCode);
    EventSet(param->event);
    return;
}

uint16_t AvdtStartRsp(uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:transLabel(%hhu),firstHandle(%hu),firstFailHandle(%hu)",
        __func__,
        transLabel,
        firstHandle,
        firstFailHandle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(firstHandle);
    if (streamCtrl == NULL || firstHandle == 0) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_WARN("[AVDT]%{public}s: input parameter invalid handle(%hu)", __func__, firstHandle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.errCode = errCode;
            evt.msg.single.label = transLabel;
            AvdtStreamProcEvent(streamCtrl, AVDT_START_CMD_RSP_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_SuspendReq
 *
 * @details       Suspend one or more stream endpoints. This suspends the
 *                transfer of media packets for the streams.  All stream
 *                endpoints must previously be open and started.  When the
 *                streams are suspended, an AVDT_SUSPEND_CFM_EVT is sent to
 *                the application via the control callback function for
 *                each stream.
 *                AVDTP.ICS #Table 5-4, 5b-4 Suspend command
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_SuspendReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtSuspendReqTskParam *param = malloc(sizeof(AvdtSuspendReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtSuspendReqTskParam), 0, sizeof(AvdtSuspendReqTskParam));
    param->handles = handles;
    param->numHandles = numHandles;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtSuspendReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtSuspendReqTsk(void *context)
{
    AvdtSuspendReqTskParam *param = (AvdtSuspendReqTskParam *)context;
    param->ret = AvdtSuspendReq(param->handles, param->numHandles, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtSuspendReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handles[0]);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handles[0]);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.seid = streamCtrl->peerSeid;
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtStreamProcEvent(streamCtrl, AVDT_SUSPEND_CMD_REQ_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_SuspendRsp
 *
 * @details       Respond to a suspend request from ACP.  This function must
 *                be called if the application receives an
 *                AVDT_SUSPEND_IND_EVT through its control callback.
 *                AVDTP.ICS #Table 11-4, 11b-4 Suspend response
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_SuspendRsp(uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode)
{
    LOG_INFO("[AVDT]%{public}s:firstHandle(%hu),firstFailHandle(%hu)", __func__, firstHandle, firstFailHandle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtSuspendRspTskParam *param = malloc(sizeof(AvdtSuspendRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtSuspendRspTskParam), 0, sizeof(AvdtSuspendRspTskParam));
    param->firstHandle = firstHandle;
    param->transLabel = transLabel;
    param->firstFailHandle = firstFailHandle;
    param->errCode = errCode;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtSuspendRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtSuspendRspTsk(void *context)
{
    AvdtSuspendRspTskParam *param = (AvdtSuspendRspTskParam *)context;
    param->ret = AvdtSuspendRsp(param->firstHandle, param->transLabel, param->firstFailHandle, param->errCode);
    EventSet(param->event);
    return;
}

uint16_t AvdtSuspendRsp(uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:firstHandle(%hu),firstFailHandle(%hu)", __func__, firstHandle, firstFailHandle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(firstHandle);
    if (streamCtrl == NULL || firstHandle == 0) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, firstHandle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.errCode = errCode;
            evt.msg.single.label = transLabel;
            AvdtStreamProcEvent(streamCtrl, AVDT_SUSPEND_CMD_RSP_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_CloseReq
 *
 * @details       Close a stream endpoint.  It stops the transfer of media
 *                packets and closes the transport channel associated with
 *                this stream endpoint.  When the stream is closed, an
 *                AVDT_CLOSE_CFM_EVT is sent to the application via the
 *                control callback function for this handle.
 *                AVDTP.ICS #Table 5-3, 5b-3 Close stream command
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_CloseReq(uint16_t handle)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtCloseReqTskParam *param = malloc(sizeof(AvdtCloseReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtCloseReqTskParam), 0, sizeof(AvdtCloseReqTskParam));
    param->handle = handle;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtCloseReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtCloseReqTsk(void *context)
{
    AvdtCloseReqTskParam *param = (AvdtCloseReqTskParam *)context;
    param->ret = AvdtCloseReq(param->handle);
    EventSet(param->event);
    return;
}

uint16_t AvdtCloseReq(uint16_t handle)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.seid = streamCtrl->peerSeid;
            AvdtStreamProcEvent(streamCtrl, AVDT_CLOSE_CMD_REQ_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_CloseRsp
 *
 * @details       Respond to a close request from ACP.  This function must
 *                be called if the application receives an AVDT_CLOSE_IND_EVT
 *                through its control callback.
 *                AVDTP.ICS #Table 11-3, 11b-c Close stream response
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_CloseRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu),transLabel(%hhu),errCode(%hhu)", __func__, handle, transLabel, errCode);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtCloseRspTskParam *param = malloc(sizeof(AvdtCloseRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtCloseRspTskParam), 0, sizeof(AvdtCloseRspTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    param->errCode = errCode;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtCloseRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtCloseRspTsk(void *context)
{
    AvdtCloseRspTskParam *param = (AvdtCloseRspTskParam *)context;
    param->ret = AvdtCloseRsp(param->handle, param->transLabel, param->errCode);
    EventSet(param->event);
    return;
}

uint16_t AvdtCloseRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu),transLabel(%hhu),errCode(%hhu)", __func__, handle, transLabel, errCode);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL || handle == 0) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.errCode = errCode;
            evt.msg.single.label = transLabel;
            AvdtStreamProcEvent(streamCtrl, AVDT_CLOSE_CMD_RSP_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_AbortReq
 *
 * @details       This function is called to request to pass AVDT Abort.
 *                AVDTP.ICS #Table 5-5, 5b-5 Abort stream command
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_AbortReq(uint16_t handle, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtAbortReqTskParam *param = malloc(sizeof(AvdtAbortReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtAbortReqTskParam), 0, sizeof(AvdtAbortReqTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtAbortReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtAbortReqTsk(void *context)
{
    AvdtAbortReqTskParam *param = (AvdtAbortReqTskParam *)context;
    param->ret = AvdtAbortReq(param->handle, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtAbortReq(uint16_t handle, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.seid = streamCtrl->peerSeid;
            AvdtStreamProcEvent(streamCtrl, AVDT_ABORT_CMD_REQ_EVENT, &evt);
            *transLabel = AvdtCreateTransLabel(sigCtrl);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_AbortRsp
 *
 * @details       The response of  Abort request from peer device.
 *                AVDTP.ICS #Table 11-5, 11b-5 Abort stream response
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_AbortRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu),transLabel(%hhu),errCode(%hhu) ", __func__, handle, transLabel, errCode);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtAbortRspTskParam *param = malloc(sizeof(AvdtAbortRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtAbortRspTskParam), 0, sizeof(AvdtAbortRspTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    param->errCode = errCode;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtAbortRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtAbortRspTsk(void *context)
{
    AvdtAbortRspTskParam *param = (AvdtAbortRspTskParam *)context;
    param->ret = AvdtAbortRsp(param->handle, param->transLabel, param->errCode);
    EventSet(param->event);
    return;
}

uint16_t AvdtAbortRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu),transLabel(%hhu),errCode(%hhu) ", __func__, handle, transLabel, errCode);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (handle == 0 || streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.single.errCode = errCode;
            evt.msg.single.label = transLabel;
            AvdtStreamProcEvent(streamCtrl, AVDT_ABORT_CMD_RSP_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_ReconfigReq
 *
 * @details       Reconfigure a stream endpoint.  It allows the application
 *                can only be called if the stream is opened but not started
 *                to change the codec capabilities of a stream endpoint after
 *                it has been opened.  When the procedure is completed, an
 *                AVDT_RECONFIG_CFM_EVT is sent to the application via the
 *                control callback function for this handle.
 *                AVDTP.ICS #Table 4-5, 4b-5 Reconfigure command
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_ReconfigReq(uint16_t handle, AvdtSepConfig *cfg, uint8_t *transLabel)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtReconfigReqTskParam *param = malloc(sizeof(AvdtReconfigReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtReconfigReqTskParam), 0, sizeof(AvdtReconfigReqTskParam));
    param->handle = handle;
    param->cfg = cfg;
    param->transLabel = transLabel;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtReconfigReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtReconfigReqTsk(void *context)
{
    AvdtReconfigReqTskParam *param = (AvdtReconfigReqTskParam *)context;
    param->ret = AvdtReconfigReq(param->handle, param->cfg, param->transLabel);
    EventSet(param->event);
    return;
}

uint16_t AvdtReconfigReq(uint16_t handle, AvdtSepConfig *cfg, uint8_t *transLabel)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (AVDT_SUCCESS == Ret) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.reconfigureCmd.hdr.seid = streamCtrl->peerSeid;
            (void)memcpy_s(&evt.msg.reconfigureCmd.cfg, sizeof(AvdtSepConfig), cfg, sizeof(AvdtSepConfig));
            *transLabel = AvdtCreateTransLabel(sigCtrl);
            AvdtStreamProcEvent(streamCtrl, AVDT_RECONFIGURE_CMD_REQ_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_ReconfigRsp
 *
 * @details       Respond to a reconfigure request from ACP.This functionr
 *                must be called if the application receives an
 *                AVDT_RECONFIG_IND_EVT through its control callback.
 *                AVDTP.ICS #Table 10-5, 10b-5 Reconfigure response
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_ReconfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtReconfigRspTskParam *param = malloc(sizeof(AvdtReconfigRspTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtReconfigRspTskParam), 0, sizeof(AvdtReconfigRspTskParam));
    param->handle = handle;
    param->transLabel = transLabel;
    (void)memcpy_s(&param->category, sizeof(AvdtCatetory), &category, sizeof(AvdtCatetory));
    param->event = event;
    if (!AvdtAsyncProcess(AvdtReconfigRspTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtReconfigRspTsk(void *context)
{
    AvdtReconfigRspTskParam *param = (AvdtReconfigRspTskParam *)context;
    param->ret = AvdtReconfigRsp(param->handle, param->transLabel, param->category);
    EventSet(param->event);
    return;
}

uint16_t AvdtReconfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL || handle == 0) {
        /* Trace input parameter invalid */
        Ret = AVDT_BAD_PARAMS;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    }
    if (Ret == AVDT_SUCCESS) {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.msg.configureCmd.hdr.errCode = category.errCode;
            evt.msg.configureCmd.hdr.label = transLabel;
            evt.msg.configureCmd.hdr.errParam = category.category;
            AvdtStreamProcEvent(streamCtrl, AVDT_RECONFIGURE_CMD_RSP_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_WriteReq
 *
 * @details       Send a media packet from SOURCE to the SINK and the status
 *                of the stream must be streaming.
 *                AVDTP 13.2.1
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_WriteReq(uint16_t handle, const Packet *pkt, uint32_t timeStamp, uint8_t payloadType, uint16_t marker)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu),timeStamp(%u),payloadType(%hhu),marker(%hu),pktlen(%u)",
        __func__,
        handle,
        timeStamp,
        payloadType,
        marker,
        PacketSize(pkt));
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtWriteReqTskParam *param = malloc(sizeof(AvdtWriteReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtWriteReqTskParam), 0, sizeof(AvdtWriteReqTskParam));
    param->handle = handle;
    param->pkt = PacketRefMalloc(pkt);
    param->timeStamp = timeStamp;
    param->payloadType = payloadType;
    param->marker = marker;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtWriteReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    PacketFree(param->pkt);
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtWriteReqTsk(void *context)
{
    AvdtWriteReqTskParam *param = (AvdtWriteReqTskParam *)context;
    param->ret = AvdtWriteReq(param->handle, param->pkt, param->timeStamp, param->payloadType, param->marker);
    EventSet(param->event);
    return;
}

uint16_t AvdtWriteReq(uint16_t handle, Packet *pkt, uint32_t timeStamp, uint8_t payloadType, uint16_t marker)
{
    LOG_DEBUG("[AVDT]%{public}s:handle(%hu),timeStamp(%u),payloadType(%hhu),marker(%hu),pktlen(%u)",
        __func__,
        handle,
        timeStamp,
        payloadType,
        marker,
        PacketSize(pkt));
    uint16_t Ret = AVDT_SUCCESS;
    /* pkt data debug print */
    AvdtPktDataPrint(pkt);
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(handle);
    if (streamCtrl == NULL) {
        /* Trace input bad parameters */
        Ret = AVDT_BAD_HANDLE;
        LOG_ERROR("[AVDT]%{public}s: AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, handle);
    } else {
        AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
        if (sigCtrl != NULL) {
            AvdtEventData evt = {0};
            evt.apiWrite.marker = marker;
            evt.apiWrite.pkt = pkt;
            evt.apiWrite.timeStamp = timeStamp;
            evt.apiWrite.payloadType = payloadType;
            AvdtStreamProcEvent(streamCtrl, AVDT_WRITE_CMD_REQ_EVENT, &evt);
        } else {
            Ret = AVDT_FAILED;
            LOG_ERROR("[AVDT]%{public}s: AvdtGetSigCtrlByHandle(%hhu) Failed", __func__, streamCtrl->sigHandle);
        }
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_ConnectRsp
 *
 * @details       Respond to a connect request from ACP.  This function must
 *                be called if the application receives an
 *                AVDT_CONNECT_IND_EVT through its control callback.
 *                AVDTP.ICS #Table 9-1, 9b-1 Establish signaling channel
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_ConnectRsp(const BtAddr *bdAddr, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief          AVDT_ConnectReq
 *
 * @details        This function send an AVDT_CONNECT_REQ from INT to ACP.
 *                 When the connection is completed, an AVDT_CONNECT_CFM_EVT
 *                 is sent to the application via its control callback
 *                 function.
 *                 AVDTP.ICS #Table 3-1, 3b-1 Establish signaling channel
 *
 * @return         AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_ConnectReq(const BtAddr *bdAddr, uint8_t role)
{
    LOG_INFO(
        "[AVDT]%{public}s: addr(%02x:%02x:%02x:%02x:%02x:%02x),pkrole(%hhu)", __func__, BT_ADDR_FMT_DSC(bdAddr->addr), role);
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtConnectReqTskParam *param = malloc(sizeof(AvdtConnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtConnectReqTskParam), 0, sizeof(AvdtConnectReqTskParam));
    param->bdAddr = bdAddr;
    param->role = role;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtConnectReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtConnectReqTsk(void *context)
{
    AvdtConnectReqTskParam *param = (AvdtConnectReqTskParam *)context;
    param->ret = AvdtConnectReq(param->bdAddr, param->role);
    EventSet(param->event);
    return;
}

uint16_t AvdtConnectReq(const BtAddr *bdAddr, uint8_t role)
{
    LOG_DEBUG(
        "[AVDT]%{public}s: addr(%02x:%02x:%02x:%02x:%02x:%02x),pkrole(%hhu)", __func__, BT_ADDR_FMT_DSC(bdAddr->addr), role);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtTransChannel *transTbl = NULL;
    AvdtCtrlData confirmData = {0};
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        /* Trace:No resources */
        Ret = AVDT_NO_RESOURCES;
        LOG_WARN("[AVDT]%{public}s: Ret(%hu)", __func__, Ret);
    }
    if (Ret == AVDT_SUCCESS) {
        /* judge the signal channel is created or not */
        transTbl = AvdtGetTransChTabByAddr(bdAddr, 0);
        sigCtrl->role = role;
        if (transTbl != NULL) {
            AvdtCtrlEvtCallback(sigCtrl, sigCtrl->handle, bdAddr, AVDT_CONNECT_CFM_EVT, &confirmData, role);
            return Ret;
        }
        sigCtrl->peerAddress = *bdAddr;
        AvdtSigProcEvent(sigCtrl, AVDT_CONNECT_CMD_REQ_EVENT, NULL);
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_DisconnectReq
 *
 * @details       This function send an AVDT_DISCONNECT_REQ from INT to ACP.
 *                When the connection is completed, an AVDT_DISCONNECT_CFM_EVT
 *                is sent to the application via its control callback function.
 *                AVDTP.ICS #Table 3-2, 3b-2 Establish signaling channel
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_DisconnectReq(const BtAddr *bdAddr)
{
    LOG_INFO("[AVDT]%{public}s: addr(%02x:%02x:%02x:%02x:%02x:%02x)", __func__, BT_ADDR_FMT_DSC(bdAddr->addr));
    uint16_t Ret = AVDT_FAILED;
    Event *event = EventCreate(true);
    AvdtDisconnectReqTskParam *param = malloc(sizeof(AvdtDisconnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        EventDelete(event);
        return Ret;
    }
    (void)memset_s(param, sizeof(AvdtDisconnectReqTskParam), 0, sizeof(AvdtDisconnectReqTskParam));
    param->bdAddr = bdAddr;
    param->event = event;
    if (!AvdtAsyncProcess(AvdtDisconnectReqTsk, param)) {
        EventWait(event, WAIT_TIME);
        Ret = param->ret;
    }
    free(param);
    EventDelete(event);
    return Ret;
}

void AvdtDisconnectReqTsk(void *context)
{
    AvdtDisconnectReqTskParam *param = (AvdtDisconnectReqTskParam *)context;
    param->ret = AvdtDisconnectReq(param->bdAddr);
    EventSet(param->event);
    return;
}
uint16_t AvdtDisconnectReq(const BtAddr *bdAddr)
{
    LOG_DEBUG("[AVDT]%{public}s: addr(%02x:%02x:%02x:%02x:%02x:%02x)", __func__, BT_ADDR_FMT_DSC(bdAddr->addr));
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr(bdAddr);
    if (sigCtrl == NULL) {
        /* Trace:No resources */
        Ret = AVDT_NO_RESOURCES;
        LOG_ERROR("[AVDT]%{public}s: The sigCtrl is no exist!", __func__);
    }
    if (Ret == AVDT_SUCCESS) {
        sigCtrl->peerAddress = *bdAddr;
        AvdtSigProcEvent(sigCtrl, AVDT_DISCONNECT_CMD_REQ_EVENT, NULL);
    }
    return Ret;
}

/**
 *
 * @brief         AVDT_DisconnectRsp
 *
 * @details       Respond to a disconnect request from ACP.  This function
 *                must be called if the application receives an
 *                AVDT_DISCONNECT_IND_EVT through its control callback.
 *                AVDTP.ICS #Table 9-2, 9b-2 Establish signaling channel
 *
 * @return        AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t AVDT_DisconnectRsp(const BtAddr *bdAddr, uint8_t errCode)
{
    LOG_INFO("[AVDT]%{public}s:errCode(%hhu)", __func__, errCode);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AVDT_GetL2capChannelByHandle
 *
 * @details       Get the L2CAP CID used by the handle.
 *
 * @return        LCID if successful, otherwise 0.
 *
 */
uint16_t AVDT_GetL2capChannelByHandle(uint16_t handle)
{
    LOG_INFO("[AVDT]%{public}s:handle(%hu)", __func__, handle);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AVDT_Rej
 *
 * @details      Send a reject message from ACP to INT.
 *               AVDTP.ICS #Table 11-6,11b-6,16-3 General Reject Response
 *               Includes Signal ID,General reject message
 *
  @return        void
 *
 */
void AVDT_Rej(uint16_t handle, const BtAddr *bdAddr, uint8_t cmd, uint8_t transLabel, AvdtRejErrInfo errInfo)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    return;
}