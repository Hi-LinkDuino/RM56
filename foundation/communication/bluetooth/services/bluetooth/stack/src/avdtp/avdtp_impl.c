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

#include "avdtp_impl.h"
#include "avdtp_ctrl.h"
#include "avdtp_message.h"
#include "btm.h"
#include "btm/btm_interop.h"
#include "l2cap_if.h"
#include "log.h"
#include "securec.h"

/**
 * Stream  action implement functions
 */

/**
 *
 * @brief         AvdtActDiscoverReq
 *
 * @details       Int send discover command
 *
 * @return        void
 *
 */
uint16_t AvdtActDiscoverReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    /* disable call discover seps API continually */
    sigCtrl->isProcBusy = true;
    sigCtrl->procParam = data->discover.numSeps;
    AvdtMsgSendCmd(sigCtrl, NULL, AVDT_SIG_DISCOVER, NULL);
    return Ret;
}

/**
 *
 * @brief         AvdtActDiscoverRsp
 *
 * @details       Avp send discover respond
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActDiscoverRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    if (data->msg.discoverRsp.hdr.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_DISCOVER, &(data->msg));
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_DISCOVER, &(data->msg));
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActDiscoverInd
 *
 * @details       Acp receive the discover command and indicate to the upper application.
 *                Get the seps information of local device.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActDiscoverInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    if (sigCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    LOG_DEBUG("[AVDT]%{public}s: sigCtrl->state(%hhu)", __func__, sigCtrl->state);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtEventData event = {0};
    event.msg.discoverRsp.numSeps = 0;
    event.msg.discoverRsp.hdr.errCode = AVDT_SUCCESS;
    for (int i = 0; i < AVDT_NUM_SEPS; i++) {
        if (sigCtrl->streamCtrl[i].isAllocated) {
            AvdtStreamConfig *streamConfig = AvdtGetSepConfigByCodecIndex(sigCtrl->streamCtrl[i].codecIndex);
            if (streamConfig != NULL) {
                event.msg.discoverRsp.seps[event.msg.discoverRsp.numSeps].mediaType =
                    streamConfig->cfg.mediaType; /* audio or video */
                event.msg.discoverRsp.seps[event.msg.discoverRsp.numSeps].sepType =
                    streamConfig->sepType; /* Source or sink */
                event.msg.discoverRsp.seps[event.msg.discoverRsp.numSeps].isUsed = sigCtrl->streamCtrl[i].isUsed;
                event.msg.discoverRsp.seps[event.msg.discoverRsp.numSeps].seid =
                    (uint8_t)(sigCtrl->streamCtrl[i].handle);
                event.msg.discoverRsp.numSeps++;
            } else {
                LOG_DEBUG("[AVDT]%{public}s: Failed to get SEP: index(%hu)", __func__, sigCtrl->streamCtrl[i].codecIndex);
            }
        }
    }
    AvdtSigProcEvent(sigCtrl, AVDT_DISCOVER_CMD_RSP_EVENT, &event);
    return Ret;
}

/**
 *
 * @brief         AvdtActDiscoverCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActDiscoverCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    sigCtrl->isProcBusy = false;
    AvdtCtrlEvtCallback(
        sigCtrl, sigCtrl->handle, &(sigCtrl->peerAddress), AVDT_DISCOVER_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief         AvdtActGetCapReq
 *
 * @details       Int send the GetCapability command to get the capability matched acpid.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetCapReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtMsg message = {0};
    /* disable call discover seps API continually */
    sigCtrl->isProcBusy = true;
    message.single.seid = data->getcap.single.seid;
    AvdtMsgSendCmd(sigCtrl, NULL, AVDT_SIG_GETCAP, &message);
    return Ret;
}

/**
 *
 * @brief         AvdtActGetCapRsp
 *
 * @details       Acp send the respond of GetCapability.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetCapRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    if (data->msg.getcap.single.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_GETCAP, &(data->msg));
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_GETCAP, &(data->msg));
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActGetCapInd
 *
 * @details       Acp receive the GetCapability command and indicate to the
 *                upper application and get the capability of local device.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetCapInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    AvdtEventData event = {0};
    AvdtSepConfig capability = {0};
    if (AVDT_SEID_INVLID <= data->msg.single.seid || data->msg.single.seid <= AVDT_SEID_FORBID) {
        event.getcap.single.errCode = AVDT_ERR_BAD_ACP_SEID;
    } else {
        capability = AvdtGetSepConfig(sigCtrl, (uint16_t)data->getcap.single.seid);
        event.getcap.single.errCode = AVDT_SUCCESS;
    }
    event.getcap.cfg = &capability;
    AvdtSigProcEvent(sigCtrl, AVDT_GETCAP_CMD_RSP_EVENT, &event);
    return AVDT_SUCCESS;
}

/**
 *
 * @brief         AvdtGetSepConfig
 *
 * @details       Get sep config by handle
 *
 * @return        AvdtSepConfig
 *
 */
AvdtSepConfig AvdtGetSepConfig(AvdtSigCtrl *sigCtrl, uint16_t handle)
{
    AvdtSepConfig capability = {0};
    if (sigCtrl == NULL) {
        return capability;
    }
    for (int i = 0; i < AVDT_NUM_SEPS; i++) {
        if (sigCtrl->streamCtrl[i].isAllocated && sigCtrl->streamCtrl[i].handle == handle) {
            AvdtStreamConfig *streamConfig = AvdtGetSepConfigByCodecIndex(sigCtrl->streamCtrl[i].codecIndex);
            if (streamConfig == NULL) {
                continue;
            }
            capability.pscMask = streamConfig->cfg.pscMask;
            capability.mediaType = streamConfig->cfg.mediaType;
            capability.numCodec = streamConfig->cfg.numCodec;
            for (int j = 0; j < (capability.numCodec + 1); j++) {
                capability.codecInfo[j] = streamConfig->cfg.codecInfo[j];
            }

            LOG_DEBUG("[AVDT]%{public}s: GetCapdata acpid(%hu),capability.pscMask(%hu),capability.numCodec(%hhu) ",
                __func__,
                sigCtrl->streamCtrl[i].handle,
                capability.pscMask,
                capability.numCodec);
            break;
        }
    }
    return capability;
}
/**
 *
 * @brief         AvdtActGetCapCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetCapCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    sigCtrl->isProcBusy = false;
    AvdtCtrlEvtCallback(
        sigCtrl, sigCtrl->handle, &(sigCtrl->peerAddress), AVDT_GETCAP_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief         AvdtActGetAllCapReq
 *
 * @details       Int send the GetAllCapability to get all capability matched acp seid.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetAllCapReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtMsg message = {0};
    /* disable call discover seps API continually */
    sigCtrl->isProcBusy = true;
    message.single.seid = data->getcap.single.seid;
    LOG_DEBUG("[AVDT]%{public}s: Reqacpid(%hhu) ", __func__, data->getcap.single.seid);
    AvdtMsgSendCmd(sigCtrl, NULL, AVDT_SIG_GET_ALLCAP, &message);
    return Ret;
}

/**
 *
 * @brief         AvdtActGetAllCapRsp
 *
 * @details       Acl send the respond of get all capability
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetAllCapRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    if (data->msg.getcap.single.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_GET_ALLCAP, &(data->msg));
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_GET_ALLCAP, &(data->msg));
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActGetAllCapInd
 *
 * @details       Acp receive the command of GetAllCapability and indicate to
 *                the upper application and get all capability of local device.
 *
 * @return        uint16_t
 *
 ******************************************************************************/
uint16_t AvdtActGetAllCapInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    LOG_DEBUG("[AVDT]%{public}s: seid(0x%x)", __func__, data->msg.single.seid);
    AvdtSepConfig capability = {0};
    AvdtEventData event = {0};
    if (AVDT_SEID_INVLID <= data->msg.single.seid || data->msg.single.seid <= AVDT_SEID_FORBID) {
        event.getcap.single.errCode = AVDT_ERR_BAD_ACP_SEID;
    } else {
        capability = AvdtGetSepConfig(sigCtrl, (uint16_t)data->getcap.single.seid);
        event.getcap.single.errCode = AVDT_SUCCESS;
    }
    event.getcap.cfg = &capability;
    AvdtSigProcEvent(sigCtrl, AVDT_GETALLCAP_CMD_RSP_EVENT, &event);
    return AVDT_SUCCESS;
}

/**
 *
 * @brief         AvdtActGetAllCapCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetAllCapCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    sigCtrl->isProcBusy = false;
    AvdtCtrlEvtCallback(
        sigCtrl, sigCtrl->handle, &(sigCtrl->peerAddress), AVDT_GET_ALLCAP_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief         AvdtActSetConfigReq
 *
 * @details       Int send the command of SetConfig to configure the stream.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActSetConfigReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    /* disable call discover seps API continually */
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtTransChannel *transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_SIG, sigCtrl->handle);
    if (transTbl == NULL) {
        /* Trace no resources parameter */
        LOG_ERROR("[AVDT]%{public}s: Not find the transTbl", __func__);
        return Ret;
    }
    transTbl->streamHandle = data->msg.configureCmd.intSeid;
    sigCtrl->streamHandle = data->msg.configureCmd.intSeid;
    /* set init role */
    LOG_DEBUG("[AVDT]%{public}s: transTbl->streamHandle(%hu)", __func__, transTbl->streamHandle);
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_SETCONFIG, &data->msg);
    return Ret;
}

/**
 *
 * @brief         AvdtActSetConfigRsp
 *
 * @details       Acceptor send respond of configure successfully.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActSetConfigRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    if (data == NULL || streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    LOG_DEBUG("[AVDT]%{public}s: errorcode(%hhu)", __func__, data->msg.configureCmd.hdr.errCode);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.configureCmd.hdr.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_SETCONFIG, NULL);
        (void)memcpy_s(&streamCtrl->currCfg, sizeof(AvdtSepConfig), &streamCtrl->reqCfg, sizeof(AvdtSepConfig));
        streamCtrl->isUsed = true;
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_SETCONFIG, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActSetConfigInd
 *
 * @details       Acp receive the command ConfigInd from remote device
 *                and indicate to the upper application.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActSetConfigInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtCtrlData confirmData = {0};
    AvdtTransChannel *transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_SIG, streamCtrl->sigHandle);
    if (transTbl == NULL) {
        /* Trace no resources parameter */
        LOG_ERROR("[AVDT]%{public}s: Not fined the transTbl", __func__);
        return Ret;
    }
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    transTbl->streamHandle = streamCtrl->handle;
    sigCtrl->streamHandle = streamCtrl->handle;
    LOG_DEBUG("[AVDT]%{public}s: transTbl->streamHandle(%hu)", __func__, transTbl->streamHandle);
    /* Acceptor set role */
    if (!streamCtrl->isUsed) {
        AvdtIsEdr2MbMode(&sigCtrl->peerAddress);
        AvdtIsEdr3MbMode(&sigCtrl->peerAddress);
        AvdtStreamConfig *streamConfig = AvdtGetSepConfigByCodecIndex(streamCtrl->codecIndex);
        if (streamConfig != NULL) {
            sigCtrl->role = streamConfig->sepType;
        }
        streamCtrl->peerSeid = data->msg.configureCmd.intSeid;
        confirmData.configInd.acpSeid = data->msg.configureCmd.acpSeid;
        confirmData.configInd.intSeid = data->msg.configureCmd.intSeid;
        confirmData.configInd.hdr.mtu = transTbl->peerMtu;
        (void)memcpy_s(&confirmData.configInd.cfg, sizeof(AvdtSepConfig), &(streamCtrl->reqCfg), sizeof(AvdtSepConfig));
        AvdtCtrlEvtCallback(
            sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_CONFIG_IND_EVT, &confirmData, sigCtrl->role);
    } else {
        AvdtIndReject(sigCtrl, AVDT_SIG_SETCONFIG, AVDT_ERR_SEP_IN_USE, 0);
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActSetConfigCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActSetConfigCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    streamCtrl->isUsed = true;
    (void)memcpy_s(&streamCtrl->currCfg, sizeof(AvdtSepConfig), &streamCtrl->reqCfg, sizeof(AvdtSepConfig));
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_CONFIG_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief         AvdtActGetConfigReq
 *
 * @details       Int sent the command of GetConfigReq to get current configure of stream.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetConfigReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsg message = {0};
    /* disable call discover seps API continually */
    message.configureCmd.acpSeid = data->msg.single.seid;
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_GETCONFIG, &message);
    return Ret;
}

/**
 *
 * @brief         AvdtActGetConfigRsp
 *
 * @details       Acp send the repond of GetConfig.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetConfigRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.configureCmd.hdr.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, streamCtrl, AVDT_SIG_GETCONFIG, &(data->msg));
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_GETCONFIG, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActGetConfigInd
 *
 * @details       Acp receive the command of GetConfig and indicate to the upper
 *                application and get the configure information of stream.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetConfigInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtSepConfig capability = {0};
    AvdtEventData event = {0};
    uint8_t seid = data->msg.single.seid;
    if ((seid >= AVDT_SEID_INVLID) || (seid == AVDT_SEID_FORBID)) {
        event.getcap.single.errCode = AVDT_ERR_BAD_ACP_SEID;
        event.getcap.cfg = &capability;
    } else {
        event.getcap.single.errCode = AVDT_SUCCESS;
        event.getcap.cfg = &capability;
        capability = AvdtGetConfig(sigCtrl, (uint16_t)data->msg.single.seid);
    }
    AvdtStreamProcEvent(streamCtrl, AVDT_GETCONFIGURE_CMD_RSP_EVENT, &event);
    return Ret;
}
AvdtSepConfig AvdtGetConfig(AvdtSigCtrl *sigCtrl, uint16_t handle)
{
    AvdtSepConfig capability = {0};
    if (sigCtrl == NULL) {
        return capability;
    }
    for (int i = 0; i < AVDT_NUM_SEPS; i++) {
        if (sigCtrl->streamCtrl[i].isAllocated && sigCtrl->streamCtrl[i].handle == handle) {
            capability.pscMask = sigCtrl->streamCtrl[i].currCfg.cfg.pscMask;
            capability.numCodec = sigCtrl->streamCtrl[i].currCfg.cfg.numCodec;
            capability.mediaType = sigCtrl->streamCtrl[i].currCfg.cfg.mediaType;
            for (int j = 0; j < capability.numCodec; j++) {
                capability.codecInfo[j] = sigCtrl->streamCtrl[i].currCfg.cfg.codecInfo[j];
            }
            LOG_DEBUG("[AVDT]%{public}s: sepNumber(%{public}d),seid(%hu),capability.pscMask(%hu),capability.numCodec(%hhu)",
                __func__,
                i,
                handle,
                capability.pscMask,
                capability.numCodec);
            break;
        }
    }
    return capability;
}

/**
 *
 * @brief         AvdtActGetConfigCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActGetConfigCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_GETCONFIG_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief         AvdtActReconfigReq
 *
 * @details       Int send the command of Reconfigure to change the codec parameters
 *                of stream.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActReconfigReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    /* disable call discover seps API continually */
    (void)memcpy_s(&streamCtrl->currCfg, sizeof(AvdtSepConfig), &data->msg.reconfigureCmd.cfg, sizeof(AvdtSepConfig));
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_RECONFIG, &data->msg);
    return Ret;
}

/**
 *
 * @brief         AvdtActReconfigRsp
 *
 * @details       Acp send respond of Reconfigure.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActReconfigRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.configureCmd.hdr.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_RECONFIG, NULL);
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_RECONFIG, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActReconfigInd
 *
 * @details       Acp receive the command of Reconfigure and indicate to the
 *                upper application.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActReconfigInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlData confirmData = {0};
    confirmData.reconfigInd.hdr.seid = data->msg.reconfigureCmd.hdr.seid;
    (void)memcpy_s(&confirmData.reconfigInd.cfg, sizeof(AvdtSepConfig), &(streamCtrl->reqCfg), sizeof(AvdtSepConfig));
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_RECONFIG_IND_EVT, &confirmData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief         AvdtActReconfigCfm
 *
 * @details       Test suit only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActReconfigCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    (void)memcpy_s(&streamCtrl->currCfg, sizeof(AvdtSepConfig), &streamCtrl->reqCfg, sizeof(AvdtSepConfig));
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_RECONFIG_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActOpenReq
 *
 * @details      Initator send open command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActOpenReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsg message = {0};
    sigCtrl->ia = AVDT_INT;
    message.single.seid = data->msg.single.seid;
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_OPEN, &message);
    return Ret;
}

/**
 *
 * @brief         AvdtActOpenRsp
 *
 * @details       Acceptor send respond of open requirement .
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActOpenRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.single.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, streamCtrl, AVDT_SIG_OPEN, &data->msg);
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_OPEN, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActOpenInd
 *
 * @details       Acp receive the open requirement from remote peer and
 *                check and send the respnd.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActOpenInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    uint8_t seid = data->msg.single.seid;
    if (AVDT_SEID_INVLID <= seid || seid == AVDT_SEID_FORBID) {
        data->msg.single.errCode = AVDT_ERR_BAD_ACP_SEID;
    } else {
        sigCtrl->ia = AVDT_ACP;
        data->msg.single.errCode = AVDT_SUCCESS;
        sigCtrl->streamHandle = streamCtrl->handle;
    }
    AvdtStreamProcEvent(streamCtrl, AVDT_OPEN_CMD_RSP_EVENT, data);
    return Ret;
}

/**
 *
 * @brief         AvdtActOpenCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActOpenCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtStreamProcEvent(streamCtrl, AVDT_STREAM_OPEN_CMD_REQ_EVENT, data);
    sigCtrl->streamHandle = streamCtrl->handle;
    return Ret;
}

/**
 *
 * @brief        AvdtActSuspendReq
 *
 * @details      Initator send suspend command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActSuspendReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsg message = {0};
    message.single.seid = data->msg.single.seid;
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_SUSPEND, &message);
    return Ret;
}

/**
 *
 * @brief        AvdtActSuspendRsp
 *
 * @details      Acp send  the respond of suspend command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActSuspendRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.single.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_SUSPEND, NULL);
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_SUSPEND, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief        AvdtActSuspendInd
 *
 * @details      Acp receive the command of suspend command and indicate to
 *               the upper application.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActSuspendInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlData confirmData = {0};
    uint8_t seid = data->msg.single.seid;
    if (AVDT_SEID_INVLID <= seid || seid == AVDT_SEID_FORBID) {
        AvdtMsg msg = {0};
        msg.single.errCode = AVDT_ERR_BAD_ACP_SEID;
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_SUSPEND, &msg);
        return Ret;
    }
    confirmData.suspendInd.seid = (uint16_t)data->msg.single.seid;
    confirmData.suspendInd.label = data->msg.single.label;
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_SUSPEND_IND_EVT, &confirmData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActSuspendCfm
 *
 * @details      Test suite only.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActSuspendCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_SUSPEND_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActStartReq
 *
 * @details      Initator send suspend command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActStartReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsg message = {0};
    message.single.seid = data->msg.single.seid;
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_START, &message);
    return Ret;
}

/**
 *
 * @brief        AvdtActStartRsp
 *
 * @details      Acp send  the respond of start command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActStartRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.single.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_START, NULL);
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_START, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief        AvdtActStartInd
 *
 * @details      Acp receive the command of start command and indicate to
 *               the upper application.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActStartInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlData confirmData = {0};
    confirmData.startInd.seid = (uint16_t)data->msg.single.seid;
    confirmData.startInd.label = data->msg.single.label;
    confirmData.startInd.mtu = sigCtrl->edr;
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_START_IND_EVT, &confirmData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActStartCfm
 *
 * @details      Test suite only.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActStartCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    data->ctrlData.startCfm.mtu = sigCtrl->edr;
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_START_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActCloseReq
 *
 * @details      Initator send close stream command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActCloseReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsg message = {0};
    message.single.seid = data->msg.single.seid;
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_CLOSE, &message);
    return Ret;
}

/**
 *
 * @brief        AvdtActCloseRsp
 *
 * @details      Acp send  the respond of close stream command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActCloseRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.single.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_CLOSE, NULL);
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_CLOSE, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief        AvdtActCloseInd
 *
 * @details      Acp receive the command of close command and indicate to
 *               the upper application.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActCloseInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    LOG_DEBUG("[AVDT]%{public}s: streamCtrl->state(%hhu)", __func__, streamCtrl->state);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlData confirmData = {0};
    confirmData.closeInd.seid = (uint16_t)data->msg.single.seid;
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_CLOSE_IND_EVT, &confirmData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActCloseCfm
 *
 * @details      Test suite only.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActCloseCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtTransChannel *transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_STREAM, streamCtrl->handle);
    if (transTbl != NULL) {
        L2CIF_DisconnectionReq(transTbl->lcid, NULL);
        LOG_DEBUG("[AVDT]%{public}s: mediachannel lcid(0x%x) ", __func__, transTbl->lcid);
    } else {
        LOG_ERROR("[AVDT]%{public}s: Not fine the transTbl", __func__);
    }
    return Ret;
}

/**
 *
 * @brief        AvdtActDelayRptReq
 *
 * @details      Initator send delay command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActDelayRptReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsg message = {0};
    message.delayRptCmd.delay = data->apiDelay.delay;
    message.delayRptCmd.hdr.seid = data->apiDelay.hdr.seid;
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_DELAY_RPT, &message);
    return Ret;
}

/**
 *
 * @brief        AvdtActDelayRptRsp
 *
 * @details      Acp send  the respond of delay command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActDelayRptRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_INFO("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    if (streamCtrl == NULL || data == NULL) {
        LOG_ERROR("[AVDT]%{public}s: streamCtrl or data is null", __func__);
        return AVDT_SUCCESS;
    }
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    if (data->msg.single.errCode == AVDT_SUCCESS) {
        AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_DELAY_RPT, &data->msg);
    } else {
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_DELAY_RPT, &data->msg);
    }
    return Ret;
}

/**
 *
 * @brief        AvdtActDelayRptInd
 *
 * @details      Acp receive the command of delay command and indicate to
 *               the upper application.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActDelayRptInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    LOG_INFO("[AVDT]%{public}s: delay value is %{public}d ", __func__, data->msg.delayRptCmd.delay);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlData confirmData = {0};
    confirmData.delayRptInd.delay = data->msg.delayRptCmd.delay;
    confirmData.delayRptInd.hdr.seid = data->msg.delayRptCmd.hdr.seid;
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_DELAY_REPORT_IND_EVT, &confirmData, sigCtrl->role);
    AvdtStreamProcEvent(streamCtrl, AVDT_DELAY_CMD_RSP_EVENT, data);
    return Ret;
}

/**
 *
 * @brief        AvdtActDelayRptCfm
 *
 * @details      Test suite only.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActDelayRptCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlEvtCallback(sigCtrl,
        streamCtrl->handle,
        &(sigCtrl->peerAddress),
        AVDT_DELAY_REPORT_CFM_EVT,
        &data->ctrlData,
        sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActAbortReq
 *
 * @details      Initator send the command of abort.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActAbortReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsg message = {0};
    message.single.seid = data->msg.single.seid;
    AvdtMsgSendCmd(sigCtrl, streamCtrl, AVDT_SIG_ABORT, &message);
    return Ret;
}

/**
 *
 * @brief        AvdtActAbortRsp
 *
 * @details      Acp send the respond of abort command.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActAbortRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtMsgSendRsp(sigCtrl, NULL, AVDT_SIG_ABORT, NULL);
    return Ret;
}

/**
 *
 * @brief        AvdtActAbortInd
 *
 * @details      Acp receive the command of abort command and indicate to
 *               the upper application.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActAbortInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    /* notify the application indication */
    AvdtCtrlEvtCallback(sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_ABORT_IND_EVT, NULL, sigCtrl->role);
    AvdtStreamProcEvent(streamCtrl, AVDT_ABORT_CMD_RSP_EVENT, NULL);
    return Ret;
}

/**
 *
 * @brief        AvdtActAbortCfm
 *
 * @details      Test suite only.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActAbortCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &(sigCtrl->peerAddress), AVDT_ABORT_CFM_EVT, &data->ctrlData, sigCtrl->role);
    return Ret;
}

/**
 *
 * @brief        AvdtActWriteReq
 *
 * @details      Source send mediapacket.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActWriteReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL || data == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    uint32_t ssrc = 0;
    AvdtStreamConfig *streamConfig = AvdtGetSepConfigByCodecIndex(streamCtrl->codecIndex);
    if (streamConfig != NULL) {
        ssrc = (uint32_t)(streamConfig->cfg.codecInfo[AVDT_CODEC_OFFSET_MEDIA_TYPE] |
                          streamConfig->cfg.codecInfo[AVDT_CODEC_OFFSET_CODEC_TYPE]);
        LOG_DEBUG("[AVDT]%{public}s: ssrc(%u), mediatype(%hhu), codectype(%hhu)",
            __func__,
            ssrc,
            streamConfig->cfg.codecInfo[AVDT_CODEC_OFFSET_MEDIA_TYPE],
            streamConfig->cfg.codecInfo[AVDT_CODEC_OFFSET_CODEC_TYPE]);
    }
    Packet *mediaPacket = PacketInheritMalloc(data->apiWrite.pkt, AVDT_BUFFER_MEDIA_HEADER, 0);
    Buffer *header = PacketHead(mediaPacket);
    uint8_t *p = BufferPtr(header);
    *p++ = AVDT_MEDIA_OCTET1;
    *p++ = data->apiWrite.payloadType;
    uint16_t Sequence = AvdtCreateSequenceNo(streamCtrl);
    AVDT_MAKE_SEQUENCE_NO(p, Sequence);
    AVDT_MAKE_TIME_STAMP(p, data->apiWrite.timeStamp);
    /* ssrc */
    AVDT_MAKE_SSRC(p, ssrc);

    AvdtTransChannel *transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_STREAM, streamCtrl->handle);
    if (transTbl == NULL) {
        LOG_ERROR("[AVDT]%{public}s: Not fine the transTbl", __func__);
        return Ret;
    }
    /* pkt data debug print */
    AvdtPktDataPrint(mediaPacket);
    L2CIF_SendData(transTbl->lcid, mediaPacket, AvdtStreamSendDataCallback);
    PacketFree(mediaPacket);
    return Ret;
}

/**
 *
 * @brief        AvdtActWriteCfm
 *
 * @details      Test suite only.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActWriteCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief        AvdtActReadInd
 *
 * @details      Test suite only.
 *
 * @return       uint16_t
 *
 */
uint16_t AvdtActReadInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamOpenReq
 *
 * @details       Open the Media channel
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamOpenReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtTransChannel *transTbl = AvdtTransChTabAllocate(AVDT_CH_TYPE_STREAM, sigCtrl, streamCtrl);
    if (transTbl == NULL) {
        /* Trace no resources parameter */
        LOG_ERROR("[AVDT]%{public}s: AvdtTransChTabAllocate failed!", __func__);
        return Ret;
    }
    transTbl->state = AVDT_TRANS_ST_CONNECTION;
    /* L2CAP connect req */
    L2CIF_ConnectReq(&sigCtrl->peerAddress, AVDT_PSM, AVDT_PSM, NULL, AvdtStreamConnectReqCallback);
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamOpenRsp
 *
 * @details       retain
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamOpenRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamOpenInd
 *
 * @details       retain
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamOpenInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamOpenCfm
 *
 * @details       retain
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamOpenCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamCloseReq
 *
 * @details       Retain.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamCloseReq(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtTransChannel *transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_STREAM, streamCtrl->handle);
    if (transTbl == NULL) {
        /* Trace no resources parameter */
        LOG_ERROR("[AVDT]%{public}s: AvdtGetTransChTabByHandle(%hu) Failed!", __func__, streamCtrl->handle);
        return Ret;
    }
    /* L2CAP disconnect req */
    L2CIF_DisconnectionReq(transTbl->lcid, NULL);
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamCloseRsp
 *
 * @details       Retain.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamCloseRsp(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamCloseInd
 *
 * @details       Free stream resources.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamCloseInd(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    LOG_DEBUG("[AVDT]%{public}s:Current state(%hhu)", __func__, streamCtrl->state);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlData confirmData = {0};
    confirmData.hdr.errCode = AVDT_SUCCESS;
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &sigCtrl->peerAddress, AVDT_CLOSE_TRANS_IND_EVT, &confirmData, sigCtrl->role);
    AvdtStreamCtrlDeallocByHandle(sigCtrl, streamCtrl->handle);
    sigCtrl->streamHandle = 0;
    return Ret;
}

/**
 *
 * @brief         AvdtActStreamCloseCfm
 *
 * @details       Free the resources of stream.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActStreamCloseCfm(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    AvdtCtrlData confirmData = {0};
    confirmData.hdr.errCode = AVDT_SUCCESS;
    AvdtCtrlEvtCallback(
        sigCtrl, streamCtrl->handle, &sigCtrl->peerAddress, AVDT_CLOSE_CFM_EVT, &confirmData, sigCtrl->role);
    /* release the stream */
    AvdtStreamCtrlDeallocByHandle(sigCtrl, streamCtrl->handle);
    sigCtrl->streamHandle = 0;
    return Ret;
}

uint16_t AvdtActSndMsg(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    AvdtSendSig(sigCtrl);
    return Ret;
}

/**
 *
 * @brief         AvdtActConnReq
 *
 * @details       INT send connect command
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActConnReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    /* Interop check */
    if (BtmInteropIsMatchedAddr(INTEROP_2MBPS_LINK_ONLY, (const BtAddr *)&(sigCtrl->peerAddress))) {
        BTM_ChangeConnectionPacketType((const BtAddr *)&(sigCtrl->peerAddress),
            BTM_ACL_PACKET_TYPE_DEFAULT | BTM_ACL_PACKET_TYPE_NO_3_DH1 | BTM_ACL_PACKET_TYPE_NO_3_DH3 |
                BTM_ACL_PACKET_TYPE_NO_3_DH5);
    }
    GapSecChannel chlId = {0};
    GapRequestSecurityParam param = {0};
    chlId.l2capPsm = AVDT_PSM;
    /* set security */
    param.callback = AvdtSecurityCheckCallback;
    param.info.channelId = chlId;
    param.info.protocolId = SEC_PROTOCOL_L2CAP;
    param.context = &sigCtrl->peerAddress;
    param.info.direction = OUTGOING;
    param.info.serviceId = GAVDP_INT;
    if (GAPIF_RequestSecurityAsync((const BtAddr *)&sigCtrl->peerAddress, &param)) {
        LOG_ERROR("[AVCT] %{public}s:GAP_RequestSecurity failed!", __func__);
        Ret = AVDT_FAILED;
    }
    return Ret;
}

/**
 *
 * @brief         AvdtActConnRsp
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActConnRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActConnInd
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActConnInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActConnCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActConnCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActDisconnReq
 *
 * @details       INT send disconnect command
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActDisconnReq(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtTransChannel *transTbl = NULL;
    for (int i = 0; i < AVDT_NUM_SEPS; i++) {
        if ((sigCtrl->streamCtrl[i].isAllocated) && (sigCtrl->streamCtrl[i].isUsed)) {
            transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_STREAM, sigCtrl->streamCtrl[i].handle);
            if (transTbl != NULL) {
                L2CIF_DisconnectionReq(transTbl->lcid, NULL);
                LOG_DEBUG("[AVDT]%{public}s: mediachannel lcid(0x%x)", __func__, transTbl->lcid);
            } else {
                LOG_ERROR("[AVDT]%{public}s: Not fine the transTbl", __func__);
            }
        }
    }
    transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_SIG, sigCtrl->handle);
    if (transTbl == NULL) {
        /* Trace no resources parameter */
        LOG_ERROR("[AVDT]%{public}s: AvdtGetTransChTabByHandle(%hu) Failed!", __func__, sigCtrl->handle);
        return Ret;
    }
    L2CIF_DisconnectionReq(transTbl->lcid, NULL);
    LOG_DEBUG("[AVDT]%{public}s: signaling lcid(0x%x) ", __func__, transTbl->lcid);
    return Ret;
}

/**
 *
 * @brief         AvdtActDisconnRsp
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActDisconnRsp(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Ret = AVDT_SUCCESS;
    return Ret;
}

/**
 *
 * @brief         AvdtActDisconnInd
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActDisconnInd(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtCtrlData confirmData = {0};
    AvdtStreamCtrlDeallocAll(sigCtrl);
    confirmData.hdr.errCode = AVDT_SUCCESS;
    if (sigCtrl->state != AVDT_SIG_IDLE_ST) {
        AvdtCtrlEvtCallback(
            sigCtrl, sigCtrl->handle, &sigCtrl->peerAddress, AVDT_DISCONNECT_IND_EVT, &confirmData, sigCtrl->role);
    }
    AvdtFreeSigCtrlByHandle(sigCtrl->handle);
    return Ret;
}

/**
 *
 * @brief         AvdtActDisconnCfm
 *
 * @details       Test suite only.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActDisconnCfm(AvdtSigCtrl *sigCtrl, const AvdtEventData *data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    uint16_t Ret = AVDT_SUCCESS;
    AvdtCtrlData confirmData = {0};
    AvdtStreamCtrlDeallocAll(sigCtrl);
    confirmData.hdr.errCode = AVDT_SUCCESS;
    AvdtCtrlEvtCallback(
        sigCtrl, sigCtrl->handle, &sigCtrl->peerAddress, AVDT_DISCONNECT_CFM_EVT, &confirmData, sigCtrl->role);
    AvdtFreeSigCtrlByHandle(sigCtrl->handle);
    return Ret;
}

/**
 *
 * @brief         AvdtActBadStateRej
 *
 * @details       Current State not surport this command, Send Reject signal cmd.
 *
 * @return        uint16_t
 *
 */
uint16_t AvdtActBadStateRej(AvdtStreamCtrl *streamCtrl, AvdtEventData *data)
{
    uint16_t Ret = AVDT_SUCCESS;
    if (streamCtrl == NULL) {
        return AVDT_SUCCESS;
    }
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(streamCtrl->sigHandle);
    LOG_DEBUG("[AVDT]%{public}s:singalId(0x%x)", __func__, sigCtrl->rcvSignal);
    AvdtIndReject(sigCtrl, sigCtrl->rcvSignal, AVDT_ERR_BAD_STATE, 0);
    return Ret;
}

/**
 * inner functions
 */

/**
 *
 * @brief         AvdtSecurityCheckCallback
 *
 * @details       INT receive the security check result
 *
 * @return        uint16_t
 *
 */
void AvdtSecurityCheckCallback(uint16_t result, GapServiceSecurityInfo security, void *context)
{
    LOG_INFO("[AVDT]%{public}s:result(%hu), l2capPsm(0x%x)", __func__, result, security.channelId.l2capPsm);
    AvdtSecurityCheckTskParam *param = malloc(sizeof(AvdtSecurityCheckTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtSecurityCheckTskParam), 0, sizeof(AvdtSecurityCheckTskParam));
    param->result = result;
    (void)memcpy_s(&param->security, sizeof(GapServiceSecurityInfo), &security, sizeof(GapServiceSecurityInfo));
    param->context = context;
    if (AvdtAsyncProcess(AvdtSecurityCheckTsk, param)) {
        free(param);
    }
    return;
}

void AvdtSecurityCheckTsk(void *context)
{
    AvdtSecurityCheckTskParam *param = (AvdtSecurityCheckTskParam *)context;
    AvdtSecurityCheck(param->result, param->security, param->context);
    free(context);
    return;
}

void AvdtSecurityCheck(uint16_t result, GapServiceSecurityInfo security, void *context)
{
    LOG_DEBUG("[AVDT]%{public}s:result(%hu), l2capPsm(0x%x)", __func__, result, security.channelId.l2capPsm);
    AvdtTransChannel *transTbl = NULL;
    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByAddr((BtAddr *)context);
    if (sigCtrl == NULL) {
        LOG_DEBUG("[AVDT]%{public}s:AvdtGetSigCtrlByAddr failed!", __func__);
        return;
    }
    if (result == AVDT_SUCCESS) {
        sigCtrl->isFailReport = false;
        /* open the signaling channel req */
        transTbl = AvdtTransChTabAllocate(AVDT_CH_TYPE_SIG, sigCtrl, NULL);
        if (transTbl == NULL) {
            /* Trace no resources parameter */
            LOG_ERROR("[AVDT]%{public}s: AvdtTransChTabAllocate failed!", __func__);
            return;
        }
        transTbl->state = AVDT_TRANS_ST_CONNECTION;
        transTbl->sigHandle = sigCtrl->handle;
        /* L2CAP connect req */
        LOG_DEBUG("[AVDT]%{public}s: [psm: 0x%x] Addr(%02x:%02x:%02x:%02x:%2x:%02x)",
            __func__,
            AVDT_PSM,
            BT_ADDR_FMT_DSC(sigCtrl->peerAddress.addr));
        if (L2CIF_ConnectReq(&sigCtrl->peerAddress, AVDT_PSM, AVDT_PSM, NULL, AvdtSignalConnectReqCallback)) {
            LOG_DEBUG("[AVDT]%{public}s:L2CIF_ConnectReq failed)", __func__);
        }
    } else {
        if (!sigCtrl->isFailReport) {
            AvdtCtrlData confirmData = {0};
            confirmData.connectCfm.errCode = AVDT_FAILED;
            sigCtrl->isFailReport = true;
            AvdtCtrlEvtCallback(
                sigCtrl, sigCtrl->handle, &(sigCtrl->peerAddress), AVDT_CONNECT_CFM_EVT, &confirmData, sigCtrl->role);
        }
    }
    return;
}

void AvdtSignalConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[AVDT]%{public}s:result(%{public}d), lcid(0x%x)", __func__, result, lcid);
    AvdtSignalConnectReqTskParam *param = malloc(sizeof(AvdtSignalConnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtSignalConnectReqTskParam), 0, sizeof(AvdtSignalConnectReqTskParam));
    (void)memcpy_s(&param->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param->lcid = lcid;
    param->result = result;
    param->context = context;
    if (AvdtAsyncProcess(AvdtSignalConnectReqTsk, param)) {
        free(param);
    }
    return;
}

void AvdtSignalConnectReqTsk(void *context)
{
    AvdtSignalConnectReqTskParam *param = (AvdtSignalConnectReqTskParam *)context;
    AvdtSignalConnectReq(&param->addr, param->lcid, param->result, param->context);
    free(context);
    return;
}

void AvdtSignalConnectReq(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_DEBUG("[AVDT]%{public}s:result(%{public}d), lcid(0x%x)", __func__, result, lcid);
    AvdtSigCtrl *sigCtrl = NULL;
    AvdtTransChannel *transTbl = AvdtGetTransChTabByAddr(addr, AVDT_CH_TYPE_SIG);
    if (transTbl != NULL) {
        transTbl->lcid = lcid;
    } else {
        LOG_ERROR("[AVDT]%{public}s:Can't find transport channel by bdAddr((%02x:%02x:%02x:%02x:%02x:%02x)",
            __func__,
            BT_ADDR_FMT_DSC(addr->addr));
    }
    sigCtrl = AvdtGetSigCtrlByAddr(addr);
    if (sigCtrl != NULL) {
        sigCtrl->lcid = lcid;
    } else {
        LOG_ERROR("[AVDT]%{public}s:Can't find control block by bdAddr((%02x:%02x:%02x:%02x:%02x:%02x)",
            __func__,
            BT_ADDR_FMT_DSC(addr->addr));
    }
    return;
}

void AvdtStreamConnectReqCallback(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_INFO("[AVDT]%{public}s:result(%{public}d), lcid(0x%x)", __func__, result, lcid);
    AvdtStreamConnectReqTskParam *param = malloc(sizeof(AvdtStreamConnectReqTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtStreamConnectReqTskParam), 0, sizeof(AvdtStreamConnectReqTskParam));
    (void)memcpy_s(&param->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    param->lcid = lcid;
    param->result = result;
    param->context = context;
    if (AvdtAsyncProcess(AvdtStreamConnectReqTsk, param)) {
        free(param);
    }
    return;
}

void AvdtStreamConnectReqTsk(void *context)
{
    AvdtStreamConnectReqTskParam *param = (AvdtStreamConnectReqTskParam *)context;
    AvdtStreamConnectReq(&param->addr, param->lcid, param->result, param->context);
    free(context);
    return;
}

void AvdtStreamConnectReq(const BtAddr *addr, uint16_t lcid, int result, void *context)
{
    LOG_DEBUG("[AVDT]%{public}s:result(%{public}d), lcid(0x%x)", __func__, result, lcid);
    AvdtSigCtrl *sigCtrl = NULL;
    AvdtTransChannel *transTbl = AvdtGetTransChTabByAddr(addr, AVDT_CH_TYPE_STREAM);
    if (transTbl != NULL) {
        transTbl->lcid = lcid;
    } else {
        LOG_ERROR("[AVDT]%{public}s:Can't find transport channel by bdAddr((%02x:%02x:%02x:%02x:%02x:%02x)",
            __func__,
            BT_ADDR_FMT_DSC(addr->addr));
    }
    sigCtrl = AvdtGetSigCtrlByAddr(addr);
    if (sigCtrl != NULL) {
        sigCtrl->lcid = lcid;
    } else {
        LOG_ERROR("[AVDT]%{public}s:Can't find control block by bdAddr((%02x:%02x:%02x:%02x:%02x:%02x)",
            __func__,
            BT_ADDR_FMT_DSC(addr->addr));
    }
    return;
}

void AvdtPktDataPrint(const Packet *pkt)
{
    int len = PacketSize(pkt);
    uint8_t buf[AVDT_32BYTE] = {0};
    if (len > AVDT_32BYTE) {
        len = AVDT_32BYTE;
    }
    PacketRead(pkt, buf, 0, len);
    for (int i = 0; i < len; i++) {
        LOG_DEBUG("%02X ", buf[i]);
    }
    return;
}

void AvdtStreamSendDataCallback(uint16_t lcid, int result)
{
    LOG_INFO("[AVDT]%{public}s:result(%{public}d), lcid(0x%x)", __func__, result, lcid);
    AvdtStreamSendDataTskParam *param = malloc(sizeof(AvdtStreamSendDataTskParam));
    if (param == NULL) {
        LOG_ERROR("[AVDT] %{public}s: memory malloc failed", __func__);
        return;
    }
    (void)memset_s(param, sizeof(AvdtStreamSendDataTskParam), 0, sizeof(AvdtStreamSendDataTskParam));
    param->lcid = lcid;
    param->result = result;
    if (AvdtAsyncProcess(AvdtStreamSendDataTsk, param)) {
        free(param);
    }
    return;
}

void AvdtStreamSendDataTsk(void *context)
{
    LOG_INFO("[AVDT]%{public}s", __func__);

    AvdtStreamSendDataTskParam *param = (AvdtStreamSendDataTskParam *)context;
    AvdtStreamSendData(param->lcid, param->result);
    free(context);
    return;
}

void AvdtStreamSendData(uint16_t lcid, int result)
{
    LOG_INFO("[AVDT]%{public}s:result(%{public}d), lcid(0x%x)", __func__, result, lcid);

    AvdtTransChannel *transTable = AvdtGetTransChTabByLcid(lcid);
    if (transTable == NULL) {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetTransChTabByLcid(0x%x) Failed!!", __func__, lcid);
        return;
    }

    LOG_DEBUG("[AVDT]%{public}s:transTable->type(%hhu), transTable->streamHandle(%hu),transTable->sigHandle(%hu) ",
        __func__,
        transTable->type,
        transTable->streamHandle,
        transTable->sigHandle);

    AvdtSigCtrl *sigCtrl = AvdtGetSigCtrlByHandle(transTable->sigHandle);
    if (sigCtrl == NULL) {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetSigCtrlByHandle(%hu) Failed", __func__, transTable->sigHandle);
        return;
    }

    AvdtCtrlData confirmData = {0};
    if (result != BT_NO_ERROR) {
        confirmData.hdr.errCode = AVDT_FAILED;
    }

    AvdtCtrlEvtCallback(
        sigCtrl, sigCtrl->streamHandle, &(sigCtrl->peerAddress), AVDT_WRITE_CFM_EVT, &confirmData, sigCtrl->role);
}