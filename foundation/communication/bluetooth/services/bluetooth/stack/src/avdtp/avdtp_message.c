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

#include "avdtp_message.h"
#include "avdtp_ctrl.h"
#include "l2cap_if.h"
#include "log.h"
#include "securec.h"

/**
 *
 * @brief        AvdtSendSig
 *
 * @details      Signaling message fragmented and built.
 *               Call L2CAP API to send the message to peer.
 *
 * @return       void
 *
 */
void AvdtSendSig(AvdtSigCtrl *sigCtrl)
{
    if (sigCtrl == NULL) {
        return;
    }
    uint16_t Len = PacketSize(sigCtrl->currentMsg);
    LOG_DEBUG("[AVDT]%{public}s:len(%hu)", __func__, Len);
    /* pkt data debug print */
    AvdtPktDataPrint(sigCtrl->currentMsg);
    AvdtTransChannel *transTbl = AvdtGetTransChTabByHandle(AVDT_CH_TYPE_SIG, sigCtrl->handle);
    if (transTbl == NULL) {
        /* Trace no resources parameter */
        LOG_ERROR("[AVDT]%{public}s: AvdtGetTransChTabByHandle(%hu) Failedl", __func__, sigCtrl->handle);
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s: packetLen(%hu) peermtu(%hu) lcid (%hu)", __func__, Len, transTbl->peerMtu, transTbl->lcid);
    if (Len <= transTbl->peerMtu) {
        if (L2CIF_SendData(transTbl->lcid, sigCtrl->currentMsg, NULL)) {
            LOG_WARN("[AVDT]%{public}s: L2CIF_SendData() failed", __func__);
        }
    } else {
        /* fragment */
        AvdtSendFragPacket(transTbl->lcid, sigCtrl->currentMsg, transTbl->peerMtu);
    }
    PacketFree(sigCtrl->currentMsg);
    sigCtrl->currentMsg = NULL;
    return;
}

/**
 *
 * @brief        AvdtSendFragPacket
 *
 * @details      send fragment message
 *
 * @return       void
 *
 */
void AvdtSendFragPacket(uint16_t lcid, Packet *pkt, uint16_t peerMtu)
{
    LOG_DEBUG("[AVDT]%{public}s: lcid(0x%x) peerMtu(%hu)", __func__, lcid, peerMtu);
    uint8_t pktType = AVDT_PKT_TYPE_START;
    uint16_t PacketLen = PacketSize(pkt);
    uint16_t Ret;
    Packet *fragmentPacket = PacketMalloc(0, 0, 0);
    Buffer *fragmentData = PacketContinuousPayload(pkt);
    PacketPayloadAddLast(fragmentPacket, fragmentData);
    uint8_t *p = (uint8_t *)BufferPtr(PacketHead(pkt));
    uint8_t label = AvdtParseTransLabel(*p);
    uint8_t msgType = AvdtParseMessageType(*p);
    uint8_t signal = *(p + 1);
    if (peerMtu <= AVDT_BUFFER_END) {
        LOG_ERROR("[AVDT]%{public}s: The MTU is too small ", __func__);
        PacketFree(fragmentPacket);
        return;
    }
    uint8_t number = (PacketLen - (peerMtu - AVDT_BUFFER_START)) / (peerMtu - AVDT_BUFFER_END) + 1;
    if ((PacketLen - (peerMtu - AVDT_BUFFER_START)) % (peerMtu - AVDT_BUFFER_END) != 0) {
        number++;
    }
    do {
        if (pktType == AVDT_PKT_TYPE_START) {
            Ret = AvdtSendFragStart(lcid, fragmentPacket, signal, number, msgType, label, peerMtu);
            PacketLen -= (peerMtu - AVDT_BUFFER_START);
            LOG_DEBUG("[AVDT]%{public}s: The length of start fragment sent(%hu) ", __func__, PacketLen);
        } else {
            Ret = AvdtSendFragContinueEnd(lcid, fragmentPacket, pktType, msgType, label, peerMtu);
            if (PacketLen > (peerMtu - AVDT_BUFFER_END)) {
                PacketLen -= (peerMtu - AVDT_BUFFER_END);
            } else {
                PacketLen = 0;
            }
            LOG_DEBUG("[AVDT]%{public}s: The length of fragment sent(%hu) ", __func__, PacketLen);
        }
        if (PacketLen > (peerMtu - AVDT_BUFFER_END)) {
            pktType = AVDT_PKT_TYPE_CONTINUE;
        } else {
            pktType = AVDT_PKT_TYPE_END;
        }
    } while ((PacketLen > 0) && (Ret == AVDT_SUCCESS));
    return;
}

/**
 *
 * @brief        AvdtSendFragStart
 *
 * @details      send fragment start type message
 *
 * @return       void
 *
 */
uint16_t AvdtSendFragStart(uint16_t lcid, Packet *fragmentPacket, uint8_t signal, uint8_t number, uint8_t msgType,
    uint8_t label, uint16_t peerMtu)
{
    LOG_DEBUG(
        "[AVDT]%{public}s: number(%hhu) signal(0x%x) msgType(0x%x) label(0x%x)", __func__, number, signal, msgType, label);
    Packet *sendMsg = PacketMalloc(AVDT_BUFFER_START, 0, 0);
    Buffer *packetHeader = PacketHead(sendMsg);
    uint8_t *p = BufferPtr(packetHeader);
    AvdtBuildSingleHeader(p, label, AVDT_PKT_TYPE_START, msgType);
    p++;
    *p++ = number;
    *p = signal;
    PacketFragment(fragmentPacket, sendMsg, (peerMtu - AVDT_BUFFER_START));
    uint16_t Ret = L2CIF_SendData(lcid, sendMsg, NULL);
    PacketFree(sendMsg);
    return Ret;
}

/**
 *
 * @brief        AvdtSendFragContinueEnd
 *
 * @details      send fragment contitune/end type message
 *
 * @return       void
 *
 */
uint16_t AvdtSendFragContinueEnd(
    uint16_t lcid, Packet *fragmentPacket, uint8_t pktType, uint8_t msgType, uint8_t label, uint16_t peerMtu)
{
    LOG_DEBUG("[AVDT]%{public}s: pktType(0x%x) msgType(0x%x) label(0x%x)", __func__, pktType, msgType, label);
    Packet *sendMsg = PacketMalloc(AVDT_BUFFER_END, 0, 0);
    Buffer *packetHeader = PacketHead(sendMsg);
    uint8_t *p = BufferPtr(packetHeader);
    AvdtBuildSingleHeader(p, label, pktType, msgType);
    PacketFragment(fragmentPacket, sendMsg, (peerMtu - AVDT_BUFFER_END));
    int16_t Ret = L2CIF_SendData(lcid, sendMsg, NULL);
    PacketFree(sendMsg);
    return Ret;
}

/**
 *
 * @brief        AvdtMsgSendCmd
 *
 * @details      Int send signaling command.
 *
 * @return       void
 *
 */
void AvdtMsgSendCmd(AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            AvdtBuildDiscoverCmd(sigCtrl, sigId, avdtMsgData);
            break;
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_GETCONFIG:
        case AVDT_SIG_ABORT:
        case AVDT_SIG_START:
        case AVDT_SIG_SUSPEND:
            AvdtBuildSingleCmd(sigCtrl, sigId, avdtMsgData);
            break;
        case AVDT_SIG_RECONFIG:
            AvdtBuildReconfigureCmd(sigCtrl, streamCtrl, sigId, avdtMsgData);
            break;
        case AVDT_SIG_SETCONFIG:
            AvdtBuildSetConfigureCmd(sigCtrl, streamCtrl, sigId, avdtMsgData);
            break;
        case AVDT_SIG_DELAY_RPT:
            AvdtBuildDelayCmd(sigCtrl, sigId, avdtMsgData);
            break;
        default:
            break;
    }
    AvdtSigProcEvent(sigCtrl, AVDT_SND_MSG_EVENT, NULL);
    return;
}
/**
 *
 * @brief        AvdtMsgSendRsp
 *
 * @details      Acp send respond.
 *
 * @return       void
 *
 */
void AvdtMsgSendRsp(AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            AvdtBuildDiscoverRsp(sigCtrl, sigId, avdtMsgData);
            break;
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
            AvdtBuildGetCapRsp(sigCtrl, sigId, avdtMsgData);
            break;
        case AVDT_SIG_GETCONFIG:
            AvdtBuildGetConfigureRsp(sigCtrl, streamCtrl, sigId, avdtMsgData);
            break;
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_ABORT:
        case AVDT_SIG_START:
        case AVDT_SIG_SUSPEND:
        case AVDT_SIG_RECONFIG:
        case AVDT_SIG_SETCONFIG:
        case AVDT_SIG_DELAY_RPT:
            AvdtBuildSingleRsp(sigCtrl, sigId, avdtMsgData);
            break;
        default:
            break;
    }
    AvdtSigProcEvent(sigCtrl, AVDT_SND_MSG_EVENT, NULL);
    return;
}

/**
 *
 * @brief        AvdtMsgSendRej
 *
 * @details      Acp reject the  signaling.
 *
 * @return       void
 *
 */
void AvdtMsgSendRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
        case AVDT_SIG_GETCONFIG:
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_DELAY_RPT:
            AvdtBuildCommonRej(sigCtrl, sigId, avdtMsgData->single.errCode);
            break;
        case AVDT_SIG_SUSPEND:
        case AVDT_SIG_START:
            AvdtBuildConfigureRej(sigCtrl, sigId, avdtMsgData->single.errParam, avdtMsgData->single.errCode);
            break;
        case AVDT_SIG_SETCONFIG:
            AvdtBuildConfigureRej(
                sigCtrl, sigId, avdtMsgData->configureCmd.hdr.errParam, avdtMsgData->configureCmd.hdr.errCode);
            break;
        case AVDT_SIG_RECONFIG:
            AvdtBuildConfigureRej(
                sigCtrl, sigId, avdtMsgData->reconfigureCmd.hdr.errParam, avdtMsgData->reconfigureCmd.hdr.errCode);
            break;
        default:
            break;
    }
    AvdtSigProcEvent(sigCtrl, AVDT_SND_MSG_EVENT, NULL);
    return;
}

/**
 *
 * @brief        AvdtMsgSendGeneralRej
 *
 * @details      Acp send the general rejection.
 *
 * @return       void
 *
 */
void AvdtMsgSendGeneralRej(AvdtSigCtrl *sigCtrl, uint8_t sigId)
{
    if (sigCtrl == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    Packet *pkt = PacketMalloc(AVDT_BUFFER_GENERAL_REJ, 0, 0);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_GRJ);
    AvdtBuildSignal(p + 1, sigId);
    sigCtrl->currentMsg = pkt;
    AvdtSigProcEvent(sigCtrl, AVDT_SND_MSG_EVENT, NULL);
    return;
}

/**
 *
 * @brief        AvdtBuildDelayCmd
 *
 * @details      build delay command to message
 *
 * @return       void
 *
 */
void AvdtBuildDelayCmd(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    if (sigCtrl == NULL || avdtMsgData == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    uint8_t data = 0;
    uint8_t offset = 0;
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, (AVDT_BUFFER_SEID + AVDT_BUFFER_SINGLE));
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->label, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_CMD);
    AvdtBuildSignal(p + 1, sigId);
    AvdtBuildSignalSeid(&data, avdtMsgData->delayRptCmd.hdr.seid);
    PacketPayloadWrite(pkt, &data, offset, AVDT_1BYTE);
    offset++;
    /* delay value */
    data = (uint8_t)(avdtMsgData->delayRptCmd.delay >> AVDT_OFFSET_8BIT);
    PacketPayloadWrite(pkt, &data, offset, AVDT_1BYTE);
    offset++;
    data = (uint8_t)(avdtMsgData->delayRptCmd.delay);
    PacketPayloadWrite(pkt, &data, offset, AVDT_1BYTE);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildDiscoverCmd
 *
 * @details      Build discover command to message.
 *
 * @return       void
 *
 */
void AvdtBuildDiscoverCmd(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    if (sigCtrl == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, 0);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = (uint8_t *)BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->label, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_CMD);
    AvdtBuildSignal(p + 1, sigId);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildDiscoverRsp
 *
 * @details      Build discover respond to message.
 *
 * @return       void
 *
 */
void AvdtBuildDiscoverRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    if (sigCtrl == NULL || avdtMsgData == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s: signal(0x%x),sepnumber(%hhu)", __func__, sigId, avdtMsgData->discoverRsp.numSeps);
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, AVDT_BUFFER_SINGLE * avdtMsgData->discoverRsp.numSeps);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_RSP);
    AvdtBuildSignal(p + 1, sigId);
    /* build seps */
    AvdtBuildSeps(pkt, avdtMsgData);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildSeps
 *
 * @details      Build seps content to message.
 *
 * @return       void
 *
 */
void AvdtBuildSeps(const Packet *pkt, const AvdtMsg *avdtMsgData)
{
    if (avdtMsgData == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint8_t count = avdtMsgData->discoverRsp.numSeps;
    uint8_t data;
    uint8_t offset = 0;
    for (int i = 0; i < count; i++) {
        LOG_DEBUG("[AVDT]%{public}s: discoverRsp acid(%hhu)", __func__, avdtMsgData->discoverRsp.seps[i].seid);
        data = ((avdtMsgData->discoverRsp.seps[i].seid << AVDT_OFFSET_2BIT) |
                (avdtMsgData->discoverRsp.seps[i].isUsed << AVDT_OFFSET_1BIT));
        PacketPayloadWrite(pkt, &data, offset, AVDT_1BYTE);
        LOG_DEBUG("[AVDT]%{public}s: discoverRsp write acid(%hhu) offset(%hhu)", __func__, data, offset);
        offset++;
        data = ((avdtMsgData->discoverRsp.seps[i].mediaType << AVDT_OFFSET_4BIT) |
                (avdtMsgData->discoverRsp.seps[i].sepType << AVDT_OFFSET_3BIT));
        PacketPayloadWrite(pkt, &data, offset, AVDT_1BYTE);
        offset++;
    }
    return;
}

/**
 *
 * @brief        AvdtBuildSingleCmd
 *
 * @details      Only header(include ACPSeid) of cmd built to message.
 *
 * @return       void
 *
 */
void AvdtBuildSingleCmd(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    if (sigCtrl == NULL || avdtMsgData == NULL) {
        return;
    }
    uint8_t data = 0;
    uint8_t transLabel;
    if (sigId == AVDT_SIG_CLOSE) {
        transLabel = AvdtCreateTransLabel(sigCtrl);
    } else {
        transLabel = sigCtrl->label;
    }
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, AVDT_BUFFER_SEID);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, transLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_CMD);
    AvdtBuildSignal(p + 1, sigId);
    AvdtBuildSignalSeid(&data, avdtMsgData->single.seid);
    PacketPayloadWrite(pkt, &data, 0, AVDT_1BYTE);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildSingleRsp
 *
 * @details      Only header of respond built to message.
 *
 * @return       void
 *
 */
void AvdtBuildSingleRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    if (sigCtrl == NULL) {
        return;
    }
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, 0);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_RSP);
    AvdtBuildSignal(p + 1, sigId);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildCommonRej
 *
 * @details      Only header of reject built to message.
 *
 * @return       void
 *
 */
void AvdtBuildCommonRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    if (sigCtrl == NULL) {
        return;
    }
    Packet *pkt = PacketMalloc(AVDT_BUFFER_REJECT, 0, 0);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_REJ);
    p++;
    AvdtBuildSignal(p, sigId);
    p++;
    *p = errCode;
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildConfigureRej
 *
 * @details      Only header of reject built to message.
 *
 * @return       void
 *
 */
void AvdtBuildConfigureRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t category, uint8_t errCode)
{
    LOG_DEBUG("[AVDT]%{public}s:signal(0x%x)", __func__, sigId);
    if (sigCtrl == NULL) {
        return;
    }
    Packet *pkt = PacketMalloc(AVDT_BUFFER_REJECT_4, 0, 0);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_REJ);
    p++;
    AvdtBuildSignal(p, sigId);
    p++;
    *p = category;
    p++;
    *p = errCode;
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildSetConfigureCmd
 *
 * @details      Build setconfigure cmd to message.
 *
 * @return       void
 *
 */
void AvdtBuildSetConfigureCmd(
    AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || streamCtrl == NULL || avdtMsgData == NULL) {
        return;
    }
    uint8_t data = 0;
    uint8_t offset = 0;
    uint8_t payloadLen = 2;
    if (streamCtrl->reqCfg.pscMask & AVDT_PSC_MSK_TRANS) {
        payloadLen += AVDT_TRANS_LENGTH;
    }
    if (streamCtrl->reqCfg.pscMask & AVDT_PSC_MSK_DELAY_RPT) {
        payloadLen += AVDT_DELAY_LENGTH;
    }
    payloadLen += streamCtrl->reqCfg.numCodec + AVDT_2BYTE;
    LOG_DEBUG("[AVDT]%{public}s: payloadLen(%hhu), numcodec(%hhu)", __func__, payloadLen, streamCtrl->reqCfg.numCodec);
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, payloadLen);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = (uint8_t *)BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->label, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_CMD);
    AvdtBuildSignal(p + 1, sigId);
    AvdtBuildSignalSeid(&data, avdtMsgData->configureCmd.acpSeid);
    PacketPayloadWrite(pkt, &data, offset, AVDT_BUFFER_SEID);
    offset++;
    AvdtBuildSignalSeid(&data, avdtMsgData->configureCmd.intSeid);
    PacketPayloadWrite(pkt, &data, offset, AVDT_BUFFER_SEID);
    offset++;
    AvdtBuildCapability(pkt, &(streamCtrl->reqCfg), offset, sigId);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildGetCapRsp
 *
 * @details      Build getcapability respond to message.
 *
 * @return       void
 *
 */
void AvdtBuildGetCapRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || avdtMsgData == NULL) {
        return;
    }
    uint8_t payloadLen = 0;
    if (avdtMsgData->getcap.cfg->pscMask & AVDT_PSC_MSK_TRANS) {
        payloadLen += AVDT_TRANS_LENGTH;
    }
    if ((avdtMsgData->getcap.cfg->pscMask & AVDT_PSC_MSK_DELAY_RPT) && (sigId != AVDT_SIG_GETCAP)) {
        payloadLen += AVDT_DELAY_LENGTH;
    }
    payloadLen += avdtMsgData->getcap.cfg->numCodec + AVDT_2BYTE;
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, payloadLen);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_RSP);
    AvdtBuildSignal(p + 1, sigId);
    /* buld seps */
    AvdtBuildCapability(pkt, avdtMsgData->getcap.cfg, 0, sigId);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildGetConfigureRsp
 *
 * @details      Build getconfigure respond to message.
 *
 * @return       void
 *
 */
void AvdtBuildGetConfigureRsp(
    AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || streamCtrl == NULL) {
        return;
    }
    uint8_t payloadLen = 0;
    if (streamCtrl->currCfg.cfg.pscMask & AVDT_PSC_MSK_TRANS) {
        payloadLen += AVDT_TRANS_LENGTH;
    }
    if (streamCtrl->currCfg.cfg.pscMask & AVDT_PSC_MSK_DELAY_RPT) {
        payloadLen += AVDT_DELAY_LENGTH;
    }
    payloadLen += streamCtrl->currCfg.cfg.numCodec + AVDT_2BYTE;
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, payloadLen);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_RSP);
    AvdtBuildSignal(p + 1, sigId);
    /* buld seps */
    AvdtBuildCapability(pkt, &streamCtrl->currCfg.cfg, 0, sigId);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief         AvdtBuildReconfigureCmd
 *
 * @details      Build reconfigure cmd to message.
 *
 * @return          void
 *
 */
void AvdtBuildReconfigureCmd(
    AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (sigCtrl == NULL || streamCtrl == NULL || avdtMsgData == NULL) {
        return;
    }
    uint8_t data = 0;
    uint8_t offset = 0;
    uint8_t payloadLen = 1;
    if (avdtMsgData->reconfigureCmd.cfg.pscMask & AVDT_PSC_MSK_TRANS) {
        payloadLen += AVDT_TRANS_LENGTH;
    }
    if (avdtMsgData->reconfigureCmd.cfg.pscMask & AVDT_PSC_MSK_DELAY_RPT) {
        payloadLen += AVDT_DELAY_LENGTH;
    }
    payloadLen += avdtMsgData->reconfigureCmd.cfg.numCodec + AVDT_2BYTE;
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, payloadLen);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->label, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_CMD);
    AvdtBuildSignal(p + 1, sigId);
    AvdtBuildSignalSeid(&data, avdtMsgData->reconfigureCmd.hdr.seid);
    PacketPayloadWrite(pkt, &data, offset, AVDT_BUFFER_SEID);
    offset++;
    streamCtrl->reqCfg.pscMask = AVDT_PSC_MSK_CODEC; /* reconfigure is modify codec only */
    AvdtBuildCapability(pkt, &avdtMsgData->reconfigureCmd.cfg, offset, sigId);
    sigCtrl->currentMsg = pkt;
    return;
}

/**
 *
 * @brief        AvdtBuildSingleHeader
 *
 * @details      Build the first octet of message.
 *
 * @return       void
 *
 */
void AvdtBuildSingleHeader(uint8_t *data, uint8_t label, uint8_t pktType, uint8_t msgType)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    *(data) = (label << MOVE_4BIT) | (pktType << MOVE_2BIT) | (msgType);
    return;
}

/**
 *
 * @brief        AvdtBuildSignal
 *
 * @details      Build the signalling of message.
 *
 * @return       void
 *
 */
void AvdtBuildSignal(uint8_t *data, uint8_t signal)
{
    LOG_DEBUG("[AVDT]%{public}s: signal(0x%x)", __func__, signal);
    *data = signal;
    return;
}

/**
 *
 * @brief        AvdtBuildSignalSeid
 *
 * @details      Build the seid of message.
 *
 * @return       void
 *
 */
void AvdtBuildSignalSeid(uint8_t *data, uint8_t seid)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    *data = seid << MOVE_2BIT;
    return;
}

/**
 *
 * @brief        AvdtBuildCapability
 *
 * @details      Build the service capability to message.
 *
 * @return       The length of data.
 *
 */
uint16_t AvdtBuildCapability(const Packet *pkt, const AvdtSepConfig *cfg, uint8_t off, uint8_t sigId)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Offset = off;
    uint8_t data;
    if (cfg == NULL) {
        return 0;
    }
    /* for now, just build media transport, codec and delayreport */
    /* media transport */
    if (cfg->pscMask & AVDT_PSC_MSK_TRANS) {
        data = AVDT_PSC_TRANS;
        PacketPayloadWrite(pkt, &data, Offset, AVDT_1BYTE);
        Offset++;
        data = 0;
        PacketPayloadWrite(pkt, &data, Offset, AVDT_1BYTE); /* media transport length */
        Offset++;
    }
    /* codec */
    if (cfg->numCodec != 0) {
        data = AVDT_PSC_CODEC;
        PacketPayloadWrite(pkt, &data, Offset, AVDT_1BYTE);
        Offset++;
        PacketPayloadWrite(pkt, cfg->codecInfo, Offset, (cfg->numCodec + 1));
        Offset += (cfg->numCodec + 1);
    }
    if (sigId != AVDT_SIG_GETCAP) {
        /* delay report */
        if (cfg->pscMask & AVDT_PSC_MSK_DELAY_RPT) {
            data = AVDT_PSC_DELAY_RPT;
            PacketPayloadWrite(pkt, &data, Offset, AVDT_1BYTE);
            Offset++;
            data = 0;
            PacketPayloadWrite(pkt, &data, Offset, AVDT_1BYTE);
            Offset++;
        }
    }
    return Offset;
}

/**
 *
 * @brief        AvdtParseMsg
 *
 * @details      Parse the signalling data from L2CAP data read indication.
 *
 *
 * @return       void
 *
 */
void AvdtParseMsg(AvdtSigCtrl *sigCtrl, Packet *pkt)
{
    if (sigCtrl == NULL) {
        return;
    }
    uint8_t data = 0;
    PacketExtractHead(pkt, &data, AVDT_1BYTE); /* Read first octet message */
    uint8_t transLabel = AvdtParseTransLabel(data);
    uint8_t pktType = AvdtParsePacketType(data);
    uint8_t messageType = AvdtParseMessageType(data);
    LOG_DEBUG("[AVDT]%{public}s: messageType(%hhu) transLabel(%hhu) pktType(%hhu)", __func__, messageType, transLabel, pktType);
    if (pktType == AVDT_PKT_TYPE_SINGLE) {
        AvdtParseSingleMsg(sigCtrl, pkt, transLabel, messageType);
    } else {
        if (pktType == AVDT_PKT_TYPE_START) {
            sigCtrl->rcvLabel = transLabel;
            AvdtAssembleMsg(sigCtrl, messageType, pktType, pkt);
        } else {
            if (transLabel == sigCtrl->rcvLabel) {
                AvdtAssembleMsg(sigCtrl, messageType, pktType, pkt);
            } else {
                /* Trace Error */
                PacketFree(sigCtrl->rxMsg);
                sigCtrl->rxMsg = NULL;
                sigCtrl->rcvLabel = 0;
                sigCtrl->numbPakcket = 0;
            }
            if (pktType == AVDT_PKT_TYPE_END && sigCtrl->numbPakcket == 0) {
                AvdtParseMsgAssembled(sigCtrl, sigCtrl->rxMsg);
            }
        }
    }
    return;
}

/**
 *
 * @brief        AvdtParseSingleMsg
 *
 * @details      Parse the single type signalling data from L2CAP data read indication.
 *
 *
 * @return       void
 *
 */
void AvdtParseSingleMsg(AvdtSigCtrl *sigCtrl, Packet *pkt, uint8_t transLabel, uint8_t messageType)
{
    if (sigCtrl == NULL) {
        return;
    }
    uint8_t data = 0;
    sigCtrl->rcvLabel = transLabel;
    PacketExtractHead(pkt, &data, AVDT_1BYTE);
    sigCtrl->rcvSignal = AvdtParseSignal(data);
    switch (messageType) {
        case AVDT_MSG_TYPE_CMD:
            AvdtParseSignalInd(sigCtrl, sigCtrl->rcvSignal, transLabel, pkt);
            break;
        case AVDT_MSG_TYPE_RSP:
            if (sigCtrl->label != sigCtrl->rcvLabel) {
                LOG_ERROR("[AVDT]%{public}s: Response label not pair: sendLabel(%hhu)/ReceiveLabel(%hhu) ",
                    __func__,
                    sigCtrl->label,
                    sigCtrl->rcvLabel);
            }
            AvdtParseSignalRsp(sigCtrl, sigCtrl->rcvSignal, pkt);
            break;
        case AVDT_MSG_TYPE_REJ:
            AvdtParseSignalRej(sigCtrl, sigCtrl->rcvSignal, transLabel, pkt);
            break;
        case AVDT_MSG_TYPE_GRJ:
            LOG_ERROR("[AVDT]%{public}s: Invalid signaling(%hhu) sendLabel(%hhu), receiveLabel(%hhu)",
                __func__,
                sigCtrl->rcvSignal,
                sigCtrl->label,
                sigCtrl->rcvLabel);
            break;
        default:
            LOG_ERROR("[AVDT]%{public}s: unsupported messageType(%hhu) ", __func__, messageType);
            break;
    }
    return;
}
/**
 *
 * @brief        AvdtAssembleMsg
 *
 * @details      Assemble the fragment message.
 *
 *
 * @return       void
 *
 */
void AvdtAssembleMsg(AvdtSigCtrl *sigCtrl, uint8_t msgType, uint8_t pktType, Packet *pkt)
{
    if (sigCtrl == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:sendLabel(%hhu) ReceiveLabel(%hhu),msgType(%hhu),pktType(%hhu)",
        __func__,
        sigCtrl->label,
        sigCtrl->rcvLabel,
        msgType,
        pktType);
    uint8_t data = 0;
    if (pktType == AVDT_PKT_TYPE_START) {
        uint8_t sigId;
        LOG_DEBUG("[AVDT]%{public}s: pktType(%hhu) msgType(%hhu)", __func__, pktType, msgType);
        PacketExtractHead(pkt, &data, 1);
        sigCtrl->numbPakcket = data;
        sigCtrl->msgType = msgType;
        PacketExtractHead(pkt, &data, 1);
        sigId = AvdtParseSignal(data);
        LOG_DEBUG("[AVDT]%{public}s: sigId(%hhu)", __func__, sigId);
        if (sigCtrl->rxMsg != NULL) {
            PacketFree(sigCtrl->rxMsg);
            sigCtrl->rxMsg = NULL;
        }
        sigCtrl->rxMsg = PacketMalloc(AVDT_BUFFER_SINGLE, 0, 0);
        if (sigCtrl->rxMsg != NULL) {
            Buffer *packetHeader = PacketHead(sigCtrl->rxMsg);
            uint8_t *p = BufferPtr(packetHeader);
            AvdtBuildSingleHeader(p, sigCtrl->rcvLabel, AVDT_PKT_TYPE_SINGLE, msgType);
            AvdtBuildSignal(p + 1, sigId);
            sigCtrl->rcvSignal = sigId;
            PacketAssemble(sigCtrl->rxMsg, pkt);
            sigCtrl->numbPakcket--;
        } else {
            LOG_ERROR("[AVDT]%{public}s: Memory alloc for rxMsg failed", __func__);
        }
    } else {
        PacketExtractHead(pkt, &data, 1);
        PacketAssemble(sigCtrl->rxMsg, pkt);
        sigCtrl->numbPakcket--;
    }
    LOG_DEBUG("[AVDT]%{public}s: sigCtrl->numbPakcket(%hhu)", __func__, sigCtrl->numbPakcket);
    return;
}

/**
 *
 * @brief        AvdtParseMsgAssembled
 *
 * @details      Parse the message assembled.
 *
 *
 * @return       void
 *
 */
void AvdtParseMsgAssembled(AvdtSigCtrl *sigCtrl, Packet *pkt)
{
    if (sigCtrl == NULL || pkt == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s: sigCtrl->rcvSignal(0x%x) sigCtrl->rcvLabel(0x%x) sigCtrl->msgType(0x%x)",
        __func__,
        sigCtrl->rcvSignal,
        sigCtrl->rcvLabel,
        sigCtrl->msgType);
    uint8_t header[2] = {0};
    PacketExtractHead(pkt, &header[0], AVDT_2BYTE);
    switch (sigCtrl->msgType) {
        case AVDT_MSG_TYPE_CMD:
            if (AvdtCheckSignal(sigCtrl->rcvSignal) != AVDT_SUCCESS) {
                /* Trace: invalid signal  */
                AvdtSignalGeneralRej(sigCtrl, sigCtrl->rcvSignal, sigCtrl->rcvLabel);
                return;
            }
            AvdtParseSignalInd(sigCtrl, sigCtrl->rcvSignal, sigCtrl->rcvLabel, pkt);
            break;
        case AVDT_MSG_TYPE_RSP:
            AvdtParseSignalRsp(sigCtrl, sigCtrl->rcvSignal, pkt);
            break;
        case AVDT_MSG_TYPE_REJ:
            break;
        case AVDT_MSG_TYPE_GRJ:
            break;
        default:
            LOG_ERROR("[AVDT]%{public}s: unsupported messageType(%hhu) ", __func__, sigCtrl->msgType);
            break;
    }
    return;
}

/**
 *
 * @brief        AvdtParseTransLabel
 *
 * @details      Parse the transport label from the first octet of message.
 *
 *
 * @return       Transport label
 *
 */
uint8_t AvdtParseTransLabel(uint8_t header)
{
    return (uint8_t)(header >> AVDT_OFFSET_4BIT);
}

/**
 *
 * @brief        AvdtParsePacketType
 *
 * @details      Parse the packet type from the first octet of message.
 *
 *
 * @return       packet type
 *
 */
uint8_t AvdtParsePacketType(uint8_t header)
{
    return (header & 0x0D) >> AVDT_OFFSET_2BIT;
}

/**
 *
 * @brief        AvdtParseMessageType
 *
 * @details      Parse the message type from the first octet of message.
 *
 *
 * @return       message type
 *
 */
uint8_t AvdtParseMessageType(uint8_t header)
{
    return (uint8_t)(header & 0x03);
}

/**
 *
 * @brief        AvdtParseSignal
 *
 * @details      Parse the signalling from message.
 *
 * @return       Signalling id
 *
 */
uint8_t AvdtParseSignal(uint8_t header)
{
    return (header & 0x3F);
}

/**
 *
 * @brief        AvdtCheckSignal
 *
 * @details      Checkt the signalling is valid.
 *
 * @return       valid:0 / invalid:1
 *
 */
uint8_t AvdtCheckSignal(uint8_t sigId)
{
    uint8_t Ret = AVDT_SUCCESS;
    if ((sigId > AVDT_SIG_MAX) || (sigId < AVDT_SIG_DISCOVER)) {
        Ret = AVDT_BAD_PARAMS;
    }
    return Ret;
}

/**
 *
 * @brief        AvdtParseSignalInd
 *
 * @details      Parse and dispatch the command from remote peer.
 *
 * @return       void
 *
 */
void AvdtParseSignalInd(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label, const Packet *pkt)
{
    if (sigCtrl == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:sendLabel(%hhu) ReceiveLabel(%hhu),sigId(0x%x),PaketSize(%u)",
        __func__,
        sigCtrl->label,
        sigCtrl->rcvLabel,
        sigId,
        PacketSize(pkt));
    uint8_t event = AvdtGetEventBySigidInd(sigId);
    if (AvdtIndPktLenCheck(sigCtrl, sigId, pkt) != AVDT_SUCCESS) {
        return;
    }
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            AvdtSigProcEvent(sigCtrl, event, NULL);
            break;
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_GETCONFIG:
        case AVDT_SIG_ABORT:
        case AVDT_SIG_START:
        case AVDT_SIG_SUSPEND:
            AvdtParseCommonInd(sigCtrl, sigId, label, pkt, event);
            break;
        case AVDT_SIG_RECONFIG:
            AvdtParseReconfigInd(sigCtrl, label, pkt, event);
            break;
        case AVDT_SIG_SETCONFIG:
            AvdtParseSetconfigInd(sigCtrl, label, pkt, event);
            break;
        case AVDT_SIG_DELAY_RPT:
            AvdtParseDelayRptInd(sigCtrl, label, pkt, event);
            break;
        default:
            AvdtMsgSendGeneralRej(sigCtrl, sigId);
            break;
    }
    return;
}
void AvdtParseCommonInd(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label, const Packet *pkt, uint8_t event)
{
    if (sigCtrl == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint8_t data = 0;
    AvdtEventData msg = {0};
    PacketPayloadRead(pkt, &data, 0, AVDT_1BYTE);
    uint8_t seid = AvdtIndSeidCheck(sigCtrl, sigId, data);
    if (seid == AVDT_SEID_INVLID) {
        return;
    }
    msg.msg.single.seid = seid;
    msg.msg.single.label = label;
    if (sigId == AVDT_SIG_GET_ALLCAP || sigId == AVDT_SIG_GETCAP) {
        AvdtSigProcEvent(sigCtrl, event, &msg);
    } else {
        AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle((uint16_t)seid);
        if (streamCtrl != NULL) {
            AvdtStreamProcEvent(streamCtrl, event, &msg);
        } else {
            LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle failed(%hhu)!", __func__, seid);
            AvdtMsg avdtMsgData = {0};
            avdtMsgData.single.errCode = AVDT_ERR_BAD_ACP_SEID;
            AvdtMsgSendRej(sigCtrl, sigId, &avdtMsgData);
        }
    }
    return;
}
void AvdtParseReconfigInd(AvdtSigCtrl *sigCtrl, uint8_t label, const Packet *pkt, uint8_t event)
{
    uint8_t data = 0;
    uint16_t Offset = 0;
    AvdtEventData msg = {0};
    PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
    uint8_t seid = AvdtIndSeidCheck(sigCtrl, AVDT_SIG_RECONFIG, data);
    if (seid == AVDT_SEID_INVLID) {
        return;
    }
    Offset++;
    PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
    if (AvdtServiceCapCheck(sigCtrl, AVDT_SIG_RECONFIG, data) != AVDT_SUCCESS) {
        return;
    }
    AvdtParseReconfig(
        (uint16_t)seid, pkt, Offset, &msg.msg.reconfigureCmd.hdr.errCode, &msg.msg.reconfigureCmd.hdr.errParam);

    if (msg.msg.reconfigureCmd.hdr.errCode != AVDT_SUCCESS) {
        AvdtIndReject(
            sigCtrl, AVDT_SIG_RECONFIG, msg.msg.reconfigureCmd.hdr.errCode, msg.msg.reconfigureCmd.hdr.errParam);
        return;
    }
    msg.msg.reconfigureCmd.hdr.seid = seid;
    msg.msg.reconfigureCmd.hdr.label = label;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle((uint16_t)seid);
    if (streamCtrl != NULL) {
        AvdtStreamProcEvent(streamCtrl, event, &msg);
    } else {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle failed(%hhu)!", __func__, seid);
        AvdtMsg avdtMsgData = {0};
        avdtMsgData.reconfigureCmd.hdr.errCode = AVDT_ERR_BAD_ACP_SEID;
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_RECONFIG, &avdtMsgData);
    }
    return;
}

void AvdtParseSetconfigInd(AvdtSigCtrl *sigCtrl, uint8_t label, const Packet *pkt, uint8_t event)
{
    uint8_t data = 0;
    uint16_t Offset = 0;
    AvdtEventData msg = {0};
    PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
    uint8_t seid = AvdtIndSeidCheck(sigCtrl, AVDT_SIG_SETCONFIG, data);
    if (seid == AVDT_SEID_INVLID) {
        return;
    }
    Offset++;
    PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
    uint8_t intSeid = AvdtParseSeid(data);
    Offset++;
    PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
    if (AvdtServiceCapCheck(sigCtrl, AVDT_SIG_SETCONFIG, data) != AVDT_SUCCESS) {
        return;
    }
    AvdtParseSetconfig(
        (uint16_t)seid, pkt, Offset, &msg.msg.configureCmd.hdr.errCode, &msg.msg.configureCmd.hdr.errParam);
    if (msg.msg.configureCmd.hdr.errCode != AVDT_SUCCESS) {
        AvdtIndReject(sigCtrl, AVDT_SIG_SETCONFIG, msg.msg.configureCmd.hdr.errCode, msg.msg.configureCmd.hdr.errParam);
        return;
    }
    msg.msg.configureCmd.acpSeid = seid;
    msg.msg.configureCmd.intSeid = intSeid;
    msg.msg.configureCmd.hdr.label = label;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle((uint16_t)seid);
    if (streamCtrl != NULL) {
        AvdtStreamProcEvent(streamCtrl, event, &msg);
    } else {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle failed(%hhu)!", __func__, seid);
        AvdtMsg avdtMsgData = {0};
        avdtMsgData.configureCmd.hdr.errCode = AVDT_ERR_BAD_ACP_SEID;
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_SETCONFIG, &avdtMsgData);
    }
    return;
}
void AvdtParseDelayRptInd(AvdtSigCtrl *sigCtrl, uint8_t label, const Packet *pkt, uint8_t event)
{
    uint8_t data = 0;
    uint16_t Offset = 0;
    uint8_t maxSB = 0;
    uint8_t lowSB = 0;
    AvdtEventData msg = {0};
    PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
    Offset++;
    uint8_t seid = AvdtIndSeidCheck(sigCtrl, AVDT_SIG_DELAY_RPT, data);
    if (seid == AVDT_SEID_INVLID) {
        return;
    }
    PacketPayloadRead(pkt, &maxSB, Offset, AVDT_1BYTE);
    Offset++;
    PacketPayloadRead(pkt, &lowSB, Offset, AVDT_1BYTE);
    Offset++;
    PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
    if (AvdtServiceCapCheck(sigCtrl, AVDT_SIG_DELAY_RPT, data) != AVDT_SUCCESS) {
        return;
    }
    msg.msg.delayRptCmd.delay = AvdtParseDelayValue(maxSB, lowSB);
    msg.msg.delayRptCmd.hdr.seid = seid;
    msg.msg.delayRptCmd.hdr.label = label;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle((uint16_t)seid);
    if (streamCtrl != NULL) {
        AvdtStreamProcEvent(streamCtrl, event, &msg);
    } else {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle failed(%hhu)!", __func__, seid);
        AvdtMsg avdtMsgData = {0};
        avdtMsgData.discoverRsp.hdr.errCode = AVDT_ERR_BAD_ACP_SEID;
        AvdtMsgSendRej(sigCtrl, AVDT_SIG_DELAY_RPT, &avdtMsgData);
    }
    return;
}

/**
 *
 * @brief        AvdtGetEventBySigidInd
 *
 * @details      Get event by signalling ID.
 *
 * @return       void
 *
 */
uint8_t AvdtGetEventBySigidInd(uint8_t sigId)
{
    uint8_t event = 0;
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            event = AVDT_DISCOVER_CMD_IND_EVENT;
            break;
        case AVDT_SIG_GET_ALLCAP:
            event = AVDT_GETALLCAP_CMD_IND_EVENT;
            break;
        case AVDT_SIG_GETCAP:
            event = AVDT_GETCAP_CMD_IND_EVENT;
            break;
        case AVDT_SIG_OPEN:
            event = AVDT_OPEN_CMD_IND_EVENT;
            break;
        case AVDT_SIG_CLOSE:
            event = AVDT_CLOSE_CMD_IND_EVENT;
            break;
        case AVDT_SIG_GETCONFIG:
            event = AVDT_GETCONFIGURE_CMD_IND_EVENT;
            break;
        case AVDT_SIG_ABORT:
            event = AVDT_ABORT_CMD_IND_EVENT;
            break;
        case AVDT_SIG_START:
            event = AVDT_START_CMD_IND_EVENT;
            break;
        case AVDT_SIG_SUSPEND:
            event = AVDT_SUSPEND_CMD_IND_EVENT;
            break;
        case AVDT_SIG_RECONFIG:
            event = AVDT_RECONFIGURE_CMD_IND_EVENT;
            break;
        case AVDT_SIG_SETCONFIG:
            event = AVDT_SETCONFIGURE_CMD_IND_EVENT;
            break;
        case AVDT_SIG_DELAY_RPT:
            event = AVDT_DELAY_CMD_IND_EVENT;
            break;
        default:
            break;
    }
    return event;
}

/**
 *
 * @brief        AvdtGetEventBySigidCfm
 *
 * @details      Get event by signalling ID.
 *
 * @return       void
 *
 */
uint8_t AvdtGetEventBySigidCfm(uint8_t sigId)
{
    uint8_t event = 0;
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            event = AVDT_DISCOVER_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_GET_ALLCAP:
            event = AVDT_GETALLCAP_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_GETCAP:
            event = AVDT_GETCAP_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_OPEN:
            event = AVDT_OPEN_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_CLOSE:
            event = AVDT_CLOSE_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_GETCONFIG:
            event = AVDT_GETCONFIGURE_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_ABORT:
            event = AVDT_ABORT_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_START:
            event = AVDT_START_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_SUSPEND:
            event = AVDT_SUSPEND_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_RECONFIG:
            event = AVDT_RECONFIGURE_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_SETCONFIG:
            event = AVDT_SETCONFIGURE_CMD_CFM_EVENT;
            break;
        case AVDT_SIG_DELAY_RPT:
            event = AVDT_DELAY_CMD_CFM_EVENT;
            break;
        default:
            break;
    }
    return event;
}

/**
 *
 * @brief        AvdtParseSignalRsp
 *
 * @details      Parse the respond content from the message of remote peer.
 *               Trigger the callback matched of A2DP profile.
 *
 * @return       void
 *
 ******************************************************************************/
void AvdtParseSignalRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const Packet *pkt)
{
    if (sigCtrl == NULL) {
        return;
    }
    LOG_DEBUG(
        "[AVDT]%{public}s:sendLabel(%hhu) ReceiveLabel(%hhu),sigId(%hhu)", __func__, sigCtrl->label, sigCtrl->rcvLabel, sigId);
    bool isSigType = false;
    uint8_t stEvent = AvdtGetEventBySigidCfm(sigId);
    AvdtEventData data = {0};
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            AvdtParseDiscoverRsp(sigCtrl, &data.ctrlData, pkt);
            isSigType = true;
            break;
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
            AvdtParseGetCapRsp(&data.ctrlData, pkt);
            isSigType = true;
            break;
        case AVDT_SIG_GETCONFIG:
            AvdtParseGetConfigRsp(&data.ctrlData, pkt);
            break;
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_ABORT:
        case AVDT_SIG_START:
        case AVDT_SIG_SUSPEND:
        case AVDT_SIG_RECONFIG:
        case AVDT_SIG_SETCONFIG:
        case AVDT_SIG_DELAY_RPT:
            break;
        default:
            LOG_ERROR("[AVDT]%{public}s:The Bad signal id(0x%x)", __func__, sigId);
            return;
    }
    if (isSigType) {
        AvdtSigProcEvent(sigCtrl, stEvent, &data);
    } else {
        AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(sigCtrl->streamHandle);
        if (streamCtrl != NULL) {
            AvdtStreamProcEvent(streamCtrl, stEvent, &data);
        } else {
            LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, sigCtrl->streamHandle);
        }
    }
    return;
}

/**
 *
 * @brief        AvdtParseSignalRej
 *
 * @details      Parse and dispatch the reject respond .
 *
 * @return       void
 *
 ******************************************************************************/
void AvdtParseSignalRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label, Packet *pkt)
{
    uint8_t data = 0;
    uint8_t errCode = 0;
    uint8_t stEvent = AvdtGetEventBySigidCfm(sigId);
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
        case AVDT_SIG_GETCONFIG:
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
            PacketExtractHead(pkt, &errCode, AVDT_1BYTE); /* Read first octet message */
            break;
        case AVDT_SIG_SUSPEND:
        case AVDT_SIG_START:
        case AVDT_SIG_RECONFIG:
        case AVDT_SIG_SETCONFIG:
            PacketExtractHead(pkt, &data, AVDT_1BYTE);    /* Read first octet message */
            PacketExtractHead(pkt, &errCode, AVDT_1BYTE); /* Read second octet message */
            break;
        default:
            break;
    }
    LOG_DEBUG("[AVDT]%{public}s: sigId(%hhu) errCode(0x%x) categoryOrSeid(%hhu)", __func__, sigId, errCode, data);
    /* Do nothing, the timer of a2dp will be triggered */
    if (stEvent == AVDT_RECONFIGURE_CMD_CFM_EVENT) {
        AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(sigCtrl->streamHandle);
        if (streamCtrl != NULL) {
            AvdtEventData eventData = {0};
            eventData.errCode = errCode;
            AvdtStreamProcEvent(streamCtrl, stEvent, &eventData);
        } else {
            LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle(%hu) Failed!", __func__, sigCtrl->streamHandle);
        }
    }
    return;
}

/**
 *
 * @brief        AvdtSignalGeneralRej
 *
 * @details      ACP check the signalling is out of scope.
 *               Then call this @brief to reject the require of remote peer.
 *
 * @return       void
 *
 */
void AvdtSignalGeneralRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label)
{
    if (sigCtrl == NULL) {
        return;
    }
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    Packet *pkt = PacketMalloc(AVDT_BUFFER_SINGLE, 0, AVDT_BUFFER_CMD_MAX);
    Buffer *msgHeader = PacketHead(pkt);
    uint8_t *p = BufferPtr(msgHeader);
    AvdtBuildSingleHeader(p, sigCtrl->label, AVDT_PKT_TYPE_SINGLE, AVDT_MSG_TYPE_GRJ);
    AvdtBuildSignal(p + 1, sigId);
    sigCtrl->currentMsg = pkt;
    AvdtSigProcEvent(sigCtrl, AVDT_SND_MSG_EVENT, NULL);
    return;
}

/**
 *
 * @brief        AvdtParseGetCapRsp
 *
 * @details      Parse the respond of get capability.
 *
 * @return       void
 *
 */
void AvdtParseGetCapRsp(AvdtCtrlData *data, const Packet *pkt)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    AvdtParseCategory(data, pkt);
    return;
}

/**
 *
 * @brief        AvdtParseGetConfigRsp
 *
 * @details      Parse the respond of getconfigure.
 *
 * @return       void
 *
 */
void AvdtParseGetConfigRsp(AvdtCtrlData *data, const Packet *pkt)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    AvdtParseCategory(data, pkt);
    return;
}

/**
 *
 * @brief        AvdtParseCategory
 *
 * @details      Parse the category.
 *
 * @return       void
 *
 */
void AvdtParseCategory(AvdtCtrlData *data, const Packet *pkt)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    if (data == NULL) {
        return;
    }
    uint16_t TotalSize = PacketSize(pkt);
    uint16_t Len = TotalSize;
    uint16_t Offset = 0;
    uint8_t context = 0;
    uint8_t tmp;
    uint8_t count;
    do {
        PacketPayloadRead(pkt, &context, Offset, AVDT_1BYTE);
        Offset++;
        Len--;
        /* parse */
        switch (context) {
            case AVDT_PSC_TRANS: {
                count = AvdtParseCategoryTrans(&data->getconfigCfm.cfg, pkt, Offset, &tmp);
                break;
            }
            case AVDT_PSC_CODEC: {
                /* read codec parameter length */
                count = AvdtParseCategoryCodec(&data->getconfigCfm.cfg, pkt, Offset);
                break;
            }
            case AVDT_PSC_DELAY_RPT: {
                count = AvdtParseCategoryDelay(&data->getconfigCfm.cfg, pkt, Offset, &tmp);
                break;
            }
            default: {
                LOG_WARN("[AVDT]%{public}s: The category(0x%x) is not surport now!", __func__, context);
                count = AvdtParseCategorySkip(pkt, Offset);
                break;
            }
        }
        Offset += count;
        Len -= count;
    } while (Len > 0 && TotalSize > Offset);
    LOG_DEBUG("[AVDT]%{public}s:data->getconfigCfm.cfg.pscMask is (0x%x)", __func__, data->getconfigCfm.cfg.pscMask);
    return;
}

/**
 *
 * @brief        AvdtParseDiscoverRsp
 *
 * @details      Parse the respond of discover.
 *
 * @return       void
 *
 */
void AvdtParseDiscoverRsp(AvdtSigCtrl *sigCtrl, AvdtCtrlData *data, const Packet *pkt)
{
    if (sigCtrl == NULL || data == NULL) {
        return;
    }
    uint16_t TotalSize = PacketSize(pkt);
    uint16_t Len = TotalSize;
    uint8_t maxNumSeps = sigCtrl->procParam;
    uint16_t Offset = 0;
    uint8_t context = 0;
    uint8_t index = 0;

    LOG_DEBUG("[AVDT]%{public}s: totalSize(%hu)", __func__, TotalSize);
    if (maxNumSeps == 0) {
        return;
    }
    do {
        (void)memset_s(&data->discoverCfm.seps[index], sizeof(AvdtSepInfo), 0, sizeof(AvdtSepInfo));
        PacketPayloadRead(pkt, &context, Offset, AVDT_1BYTE);
        Offset++;
        Len--;
        data->discoverCfm.seps[index].isUsed = (bool)(context & 0x02);
        data->discoverCfm.seps[index].seid = (context >> AVDT_OFFSET_2BIT);
        context = 0;
        PacketPayloadRead(pkt, &context, Offset, AVDT_1BYTE);
        Offset++;
        Len--;
        data->discoverCfm.seps[index].mediaType = (context >> AVDT_OFFSET_4BIT);
        data->discoverCfm.seps[index].sepType = ((context & 0x08) >> AVDT_OFFSET_3BIT);
        context = 0;
        maxNumSeps--;
        LOG_DEBUG("[AVDT]%{public}s: sigCtrl->role(%hhu) seps[index].sepType(%hhu) acpid(%hhu) mediaType(%hhu) index(%hhu) "
                  "maxNumSeps(%hhu)",
            __func__,
            sigCtrl->role,
            data->discoverCfm.seps[index].sepType,
            data->discoverCfm.seps[index].seid,
            data->discoverCfm.seps[index].mediaType,
            index,
            maxNumSeps);
        if (sigCtrl->role != data->discoverCfm.seps[index].sepType && (data->discoverCfm.seps[index].seid > 0)) {
            index++;
        }
    } while (Len > 0 && maxNumSeps > 0 && TotalSize > Offset);
    data->discoverCfm.numSeps = index;
    return;
}

/**
 *
 * @brief        AvdtParseGetConfigRsp
 *
 * @details      Parse the respond of getconfigure.
 *
 * @return       void
 *
 */
uint8_t AvdtParseSeid(uint8_t data)
{
    uint8_t seid = ((data >> AVDT_OFFSET_2BIT) & 0x3F);
    if (seid < AVDT_SEID_MIN || seid > AVDT_SEID_MAX) {
        seid = AVDT_SEID_INVLID;
    }
    LOG_DEBUG("[AVDT]%{public}s: seid(0x%x)", __func__, seid);
    return seid;
}

/**
 *
 * @brief        AvdtParseDelayValue
 *
 * @details      Parse the delay value from message.
 *
 * @return       delay value
 *
 */
uint16_t AvdtParseDelayValue(uint8_t maxSB, uint8_t lowSB)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t Value = (uint16_t)((maxSB << AVDT_OFFSET_8BIT) | lowSB);
    return Value;
}

/**
 *
 * @brief        AvdtParseSetconfig
 *
 * @details      Parse the content of setconfig. *
 *
 * @return       0:sccusse /other errcode
 *
 */
void AvdtParseSetconfig(uint16_t streamHandle, const Packet *pkt, uint16_t off, uint8_t *errCode, uint8_t *errParam)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t TotalSize = PacketSize(pkt);
    uint16_t Len = TotalSize;
    uint8_t data = 0;
    uint16_t Offset = off;
    uint8_t tmp;
    uint8_t count;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(streamHandle);
    if (streamCtrl == NULL) {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle Failed", __func__);
        return;
    }
    do {
        PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
        Offset++;
        Len--;
        switch (data) {
            case AVDT_PSC_TRANS: {
                count = AvdtParseCategoryTrans(&streamCtrl->reqCfg, pkt, Offset, &tmp);
                if (tmp != 0) {
                    *errCode = AVDT_ERR_BAD_MEDIA_TRANSPORT_FORMAT;
                    *errParam = AVDT_PSC_TRANS;
                    return;
                }
                break;
            }
            case AVDT_PSC_CODEC: {
                count = AvdtParseCategoryCodec(&streamCtrl->reqCfg, pkt, Offset);
                break;
            }
            case AVDT_PSC_DELAY_RPT: {
                count = AvdtParseCategoryDelay(&streamCtrl->reqCfg, pkt, Offset, &tmp);
                break;
            }
            default:
                LOG_ERROR("[AVDT]%{public}s: The category(0x%x) is not surport now!", __func__, data);
                *errCode = AVDT_ERR_INVALID_CAPABILITIES;
                *errParam = data;
                return;
        }
        Offset += count;
        Len -= count;
    } while (Len > 0 && TotalSize > Offset);
    return;
}

/**
 *
 * @brief        AvdtParseCategoryCodec
 *
 * @details      Parse the codec content of setconfig.
 *
 * @return       read data length
 *
 */
uint8_t AvdtParseCategoryCodec(AvdtSepConfig *sepConfig, const Packet *pkt, uint16_t offset)
{
    if (sepConfig == NULL) {
        return 0;
    }
    uint8_t data = 0;
    uint8_t length = 0;
    PacketPayloadRead(pkt, &data, offset, AVDT_1BYTE);

    /* now only surport sbc/aac */
    if (data > AVDT_CODEC_SIZE) {
        data = AVDT_CODEC_SIZE;
    }
    offset++;
    length++;
    sepConfig->codecInfo[0] = data;
    PacketPayloadRead(pkt, &(sepConfig->codecInfo[1]), offset, data);
    sepConfig->numCodec = data;
    offset += data;
    length += data;
    sepConfig->pscMask = (sepConfig->pscMask | AVDT_PSC_MSK_CODEC);
    LOG_DEBUG("[AVDT]%{public}s:offset=%{public}d,length=%{public}d", __func__, offset, length);
    return length;
}
/**
 *
 * @brief        AvdtParseCategoryTrans
 *
 * @details      Parse the trans media content of setconfig.
 *
 * @return       read data length
 *
 */
uint8_t AvdtParseCategoryTrans(AvdtSepConfig *sepConfig, const Packet *pkt, uint16_t offset, uint8_t *data)
{
    if (sepConfig == NULL) {
        return 0;
    }
    uint8_t length = 0;
    PacketPayloadRead(pkt, data, offset, AVDT_1BYTE);
    length++;
    sepConfig->pscMask = (sepConfig->pscMask | AVDT_PSC_MSK_TRANS);
    return length;
}
/**
 *
 * @brief        AvdtParseCategoryDelay
 *
 * @details      Parse the delay report content of setconfig.
 *
 * @return       read data length
 *
 */
uint8_t AvdtParseCategoryDelay(AvdtSepConfig *sepConfig, const Packet *pkt, uint16_t offset, uint8_t *data)
{
    if (sepConfig == NULL) {
        return 0;
    }
    uint8_t length = 0;
    PacketPayloadRead(pkt, data, offset, AVDT_1BYTE);
    length++;
    sepConfig->pscMask = (sepConfig->pscMask | AVDT_PSC_MSK_DELAY_RPT);
    return length;
}
/**
 *
 * @brief        AvdtParseCategorySkip
 *
 * @details      Skip unsurport type content of setconfig.
 *
 * @return       read data length
 *
 */
uint8_t AvdtParseCategorySkip(const Packet *pkt, uint16_t offset)
{
    uint8_t data = 0;
    PacketPayloadRead(pkt, &data, offset, AVDT_1BYTE);
    uint8_t length = data + 1;
    return length;
}
/**
 *
 * @brief        AvdtParseReconfig
 *
 * @details      Parse the content of reconfig.
 *
 * @return       void
 *
 */
void AvdtParseReconfig(uint16_t streamHandle, const Packet *pkt, uint16_t off, uint8_t *errCode, uint8_t *errParam)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint16_t TotalSize = PacketSize(pkt);
    uint16_t Len = TotalSize;
    uint16_t Offset = off;
    uint8_t data = 0;
    uint8_t tmp;
    uint8_t count;
    AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle(streamHandle);
    if (streamCtrl == NULL) {
        LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle Failed", __func__);
        return;
    }
    do {
        PacketPayloadRead(pkt, &data, Offset, AVDT_1BYTE);
        Offset++;
        Len--;
        switch (data) {
            case AVDT_PSC_TRANS: {
                count = AvdtParseCategoryTrans(&streamCtrl->reqCfg, pkt, Offset, &tmp);
                if (tmp != 0) {
                    *errCode = AVDT_ERR_BAD_MEDIA_TRANSPORT_FORMAT;
                    *errParam = AVDT_PSC_TRANS;
                    return;
                }
                break;
            }
            case AVDT_PSC_CODEC: {
                count = AvdtParseCategoryCodec(&streamCtrl->reqCfg, pkt, Offset);
                break;
            }
            default:
                LOG_ERROR("[AVDT]%{public}s: The category(0x%x) is not surport now!", __func__, data);
                *errCode = AVDT_ERR_INVALID_CAPABILITIES;
                *errParam = data;
                return;
        }
        Offset += count;
        Len -= count;
    } while (Len > 0 && TotalSize > Offset);
    LOG_DEBUG("[AVDT]%{public}s:data->getCapCfm.cfg.pscMask is (0x%x)", __func__, streamCtrl->reqCfg.pscMask);
    return;
}

/**
 *
 * @brief        AvdtIndPktLenCheck
 *
 * @details      Check packet length is valid for ind command.
 *
 * @return       valid：0 / invalid: other
 *
 */
uint8_t AvdtIndPktLenCheck(AvdtSigCtrl *sigCtrl, uint8_t sigId, const Packet *pkt)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint8_t Ret = AVDT_SUCCESS;
    uint16_t Len = PacketSize(pkt);
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            break;
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
        case AVDT_SIG_GETCONFIG:
        case AVDT_SIG_RECONFIG:
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_ABORT:
        case AVDT_SIG_START:
        case AVDT_SIG_SUSPEND:
            if (Len < AVDT_SIG_LENGTH_COMM) {
                Ret = AVDT_BAD_PARAMS;
            }
            break;
        case AVDT_SIG_SETCONFIG:
            if (Len < AVDT_SIG_LENGTH_SETCONFIG) {
                Ret = AVDT_BAD_PARAMS;
            }
            break;
        case AVDT_SIG_DELAY_RPT:
            if (Len < AVDT_SIG_LENGTH_DELAY_RPT) {
                Ret = AVDT_BAD_PARAMS;
            }
            break;
        default:
            break;
    }
    if (Ret != AVDT_SUCCESS) {
        LOG_ERROR("[AVDT]%{public}s:BAD LENGTH of the cmd!", __func__);
        AvdtIndReject(sigCtrl, sigId, AVDT_ERR_BAD_LENGTH, 0);
    }
    return Ret;
}

/**
 *
 * @brief        AvdtIndSeidCheck
 *
 * @details      Check se id is valid for ind command.
 *
 * @return       valid：id / invalid: AVDT_SEID_INVLID
 *
 */
uint8_t AvdtIndSeidCheck(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint8_t seid = AVDT_SEID_INVLID;
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            break;
        case AVDT_SIG_GET_ALLCAP:
        case AVDT_SIG_GETCAP:
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_GETCONFIG:
        case AVDT_SIG_ABORT:
        case AVDT_SIG_START:
        case AVDT_SIG_SUSPEND:
        case AVDT_SIG_RECONFIG:
        case AVDT_SIG_SETCONFIG:
        case AVDT_SIG_DELAY_RPT:
            seid = AvdtParseSeid(data);
            if (seid == AVDT_SEID_INVLID) {
                LOG_ERROR("[AVDT]%{public}s:BAD SEID of the cmd!", __func__);
                AvdtIndReject(sigCtrl, sigId, AVDT_ERR_BAD_ACP_SEID, 0);
            } else {
                AvdtStreamCtrl *streamCtrl = AvdtGetStreamCtrlByHandle((uint16_t)seid);
                if (streamCtrl == NULL) {
                    LOG_ERROR("[AVDT]%{public}s:AvdtGetStreamCtrlByHandle failed(%hhu)!", __func__, seid);
                    AvdtIndReject(sigCtrl, sigId, AVDT_ERR_BAD_ACP_SEID, 0);
                    seid = AVDT_SEID_INVLID;
                }
            }
            break;
        default:
            break;
    }
    return seid;
}

/**
 *
 * @brief        AvdtMakeRejMsg
 *
 * @details      Make reject message for received signal.
 *
 * @return       valid：id / invalid: AVDT_SEID_INVLID
 *
 */
void AvdtMakeRejMsg(uint8_t sigId, uint8_t errCode, uint8_t errParam, AvdtMsg *msg)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    switch (sigId) {
        case AVDT_SIG_DISCOVER:
            break;
        case AVDT_SIG_GETCONFIG:
        case AVDT_SIG_OPEN:
        case AVDT_SIG_CLOSE:
        case AVDT_SIG_ABORT:
        case AVDT_SIG_START:
        case AVDT_SIG_SUSPEND:
            msg->single.errCode = errCode;
            msg->single.errParam = errParam;
            break;
        case AVDT_SIG_GETCAP:
        case AVDT_SIG_GET_ALLCAP:
            msg->getcap.single.errCode = errCode;
            msg->getcap.single.errParam = errParam;
            break;
        case AVDT_SIG_SETCONFIG:
            msg->configureCmd.hdr.errCode = errCode;
            msg->configureCmd.hdr.errParam = errParam;
            break;
        case AVDT_SIG_RECONFIG:
            msg->reconfigureCmd.hdr.errCode = errCode;
            msg->reconfigureCmd.hdr.errParam = errParam;
            break;
        case AVDT_SIG_DELAY_RPT:
            msg->delayRptCmd.hdr.errCode = errCode;
            msg->delayRptCmd.hdr.errParam = errParam;
            break;
        default:
            break;
    }
    return;
}

/**
 *
 * @brief        AvdtServiceCapCheck
 *
 * @details      Service capabilities valid check.
 *
 * @return       valid：0 / invalid: other
 *
 */
uint8_t AvdtServiceCapCheck(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t data)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    uint8_t Ret = AVDT_SUCCESS;
    uint8_t err = 0;
    if (data > AVDT_PSC_MAX_CUR) {
        err = AVDT_ERR_BAD_SERV_CATEGORY;
    } else if (sigId == AVDT_SIG_RECONFIG && data != AVDT_PSC_CODEC) {
        err = AVDT_ERR_INVALID_CAPABILITIES;
    } else if ((data != AVDT_PSC_TRANS) && (data != AVDT_PSC_CODEC) && (data != AVDT_PSC_DELAY_RPT)
        && (data != AVDT_PSC_PROTECT)) {
        err = AVDT_ERR_INVALID_CAPABILITIES;
    }
    if (err != 0) {
        LOG_ERROR("[AVDT]%{public}s:BAD Service Category of the cmd(0x%x)!", __func__, sigId);
        AvdtIndReject(sigCtrl, sigId, err, data);
        Ret = AVDT_BAD_PARAMS;
    }
    return Ret;
}

/**
 *
 * @brief        AvdtIndReject
 *
 * @details      Send reject message by signal.
 *
 * @return       void
 *
 */
void AvdtIndReject(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t errCode, uint8_t errParam)
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    AvdtMsg avdtMsgData = {0};
    AvdtMakeRejMsg(sigId, errCode, errParam, &avdtMsgData);
    AvdtMsgSendRej(sigCtrl, sigId, &avdtMsgData);
    return;
}
