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

#include "l2cap_core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btm.h"
#include "log.h"

#include "l2cap_cmn.h"
#include "l2cap_crc.h"

static uint8_t L2capGetNewIdentifier(L2capConnection *conn)
{
    uint8_t ident;

    ident = conn->nextIdentifier;
    if (ident == L2CAP_MAX_IDENTIFIER) {
        conn->nextIdentifier = L2CAP_MIN_IDENTIFIER;
    } else {
        conn->nextIdentifier += 1;
    }

    return ident;
}

static void L2capDisconnectAbnormal(const L2capChannel *chan, uint8_t reason)
{
    L2capPsm *psm = NULL;

    psm = L2capGetPsm(chan->lpsm);
    if (psm != NULL) {
        LOG_DEBUG(
            "L2capCallback disconnectAbnormal:%{public}d begin, cid = 0x%04X, reason = %hhu", __LINE__, chan->lcid, reason);
        psm->service.disconnectAbnormal(chan->lcid, reason, psm->ctx);
        LOG_DEBUG("L2capCallback disconnectAbnormal:%{public}d end", __LINE__);
    }

    return;
}

static void L2capResponseTimeout(const void *parameter)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;
    L2capPendingRequest *req = NULL;
    ListNode *node = NULL;

    if (inst->connList == NULL) {
        return;
    }

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        req = L2capGetPendingRequest2(conn->pendingList, parameter);
        if (req != NULL) {
            ListRemoveNode(conn->pendingList, req);
            break;
        }

        node = ListGetNextNode(node);
    }

    if (req == NULL) {
        return;
    }

    if (req->code == L2CAP_INFORMATION_REQUEST) {
        L2capDisconnect(conn->aclHandle, 0x13);
    } else if (req->lcid > 0) {
        L2capChannel *chan = NULL;

        chan = L2capGetChannel(conn, req->lcid);
        if (chan != NULL) {
            L2capDisconnectAbnormal(chan, 0);
            L2capDeleteChannel(conn, chan, 1);
        }
    }

    AlarmDelete(req->timer);
    L2capFree(req);
    return;
}

static void L2capResponseTimeoutCallback(void *parameter)
{
    L2capAsynchronousProcess(L2capResponseTimeout, NULL, parameter);
    return;
}

int L2capSendEchoReq(L2capConnection *conn, const uint8_t *data, uint16_t dataLen)
{
    Packet *pkt = NULL;
    L2capSignalHeader signal = {0};

    signal.code = L2CAP_ECHO_REQUEST;
    signal.identifier = L2capGetNewIdentifier(conn);
    signal.length = dataLen;

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, data);

    L2capCreatePendingRequest(conn->pendingList, 0, &signal, L2CAP_DEFAULT_RTX, L2capResponseTimeoutCallback);
    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendEchoRsp(L2capConnection *conn, uint8_t ident, const uint8_t *data, uint16_t dataLen)
{
    Packet *pkt = NULL;
    L2capSignalHeader signal = {0};

    signal.code = L2CAP_ECHO_RESPONSE;
    signal.identifier = ident;
    signal.length = dataLen;

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, data);
    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendConnectionReq(L2capConnection *conn, L2capChannel *chan)
{
    Packet *pkt = NULL;
    uint8_t buff[4] = {0};
    L2capSignalHeader signal = {0};

    BTM_ExitSniffMode(&(conn->addr));

    L2capCpuToLe16(buff + 0, chan->rpsm);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->lcid);

    signal.code = L2CAP_CONNECTION_REQUEST;
    signal.identifier = L2capGetNewIdentifier(conn);
    signal.length = sizeof(buff);
    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);

    chan->state = L2CAP_CHANNEL_CONNECT_OUT_REQ;

    L2capCreatePendingRequest(conn->pendingList, chan->lcid, &signal, L2CAP_DEFAULT_RTX, L2capResponseTimeoutCallback);
    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendConnectionRsp(L2capConnection *conn, L2capChannel *chan, uint8_t ident, uint16_t result, uint16_t status)
{
    Packet *pkt = NULL;
    uint8_t buff[8] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->lcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, result);
    L2capCpuToLe16(buff + L2CAP_OFFSET_6, status);

    signal.code = L2CAP_CONNECTION_RESPONSE;
    signal.identifier = ident;
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);

    if (result == L2CAP_CONNECTION_SUCCESSFUL) {
        chan->state = L2CAP_CHANNEL_CONFIGING;
    }

    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendConfigurationReq(L2capConnection *conn, const L2capChannel *chan)
{
    Packet *pkt = NULL;
    uint8_t buff[48] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, 0);

    buff[L2CAP_OFFSET_4] = L2CAP_OPTION_MAXIMUM_TRANSMISSION_UNIT;
    buff[L2CAP_OFFSET_5] = L2CAP_SIZE_2;
    L2capCpuToLe16(buff + L2CAP_OFFSET_6, chan->lcfg.mtu);

    signal.length = L2CAP_SIZE_8;

    if (chan->lcfg.flushTimeout != 0xFFFF) {
        buff[signal.length + 0] = L2CAP_OPTION_FLUSH_TIMEOUT;
        buff[signal.length + L2CAP_OFFSET_1] = L2CAP_SIZE_2;
        L2capCpuToLe16(buff + signal.length + L2CAP_OFFSET_2, chan->lcfg.flushTimeout);
        signal.length += L2CAP_SIZE_4;
    }

    if (chan->lcfg.rfc.mode != L2CAP_BASIC_MODE) {
        buff[signal.length + 0] = L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL;
        buff[signal.length + L2CAP_OFFSET_1] = L2CAP_SIZE_9;
        buff[signal.length + L2CAP_OFFSET_2] = chan->lcfg.rfc.mode;
        buff[signal.length + L2CAP_OFFSET_3] = chan->lcfg.rfc.rxWindowSize;
        buff[signal.length + L2CAP_OFFSET_4] = chan->lcfg.rfc.maxTransmit;
        L2capCpuToLe16(buff + signal.length + L2CAP_OFFSET_5, 0);
        L2capCpuToLe16(buff + signal.length + L2CAP_OFFSET_7, 0);
        L2capCpuToLe16(buff + signal.length + L2CAP_OFFSET_9, chan->lcfg.rfc.mps);
        signal.length += L2CAP_SIZE_11;
    }

    if (chan->lcfg.fcs != 0x01) {
        buff[signal.length + 0] = L2CAP_OPTION_FRAME_CHECK_SEQUENCE;
        buff[signal.length + L2CAP_OFFSET_1] = L2CAP_SIZE_1;
        buff[signal.length + L2CAP_OFFSET_2] = 0;
        signal.length += L2CAP_SIZE_3;
    }

    signal.code = L2CAP_CONFIGURATION_REQUEST;
    signal.identifier = L2capGetNewIdentifier(conn);

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);
    L2capCreatePendingRequest(conn->pendingList, chan->lcid, &signal, L2CAP_DEFAULT_RTX, L2capResponseTimeoutCallback);
    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

static void L2capChannelConnected(L2capChannel *chan)
{
    if ((chan->cfgState & L2CAP_CONFIG_STATE_OUT_DONE) && (chan->cfgState & L2CAP_CONFIG_STATE_IN_DONE)) {
        chan->state = L2CAP_CHANNEL_CONNECTED;
        if ((chan->lcfg.fcs == 0x01) || (chan->rcfg.fcs == 0x01)) {
            chan->lcfg.fcs = 0x01;
            chan->rcfg.fcs = 0x01;
        }
    }

    return;
}

static void L2capSendConfigurationRspRfcProcess(
    L2capChannel *chan, L2capSignalHeader *signal, uint8_t *buff, uint16_t result, const L2capConfigInfo *cfg)
{
    if (chan->rcfg.rfc.mode != cfg->rfc.mode) {
        if (result == L2CAP_UNACCEPTABLE_PARAMETERS) {
            chan->rcfg.rfc.mode = cfg->rfc.mode;
        }

        buff[signal->length + 0] = L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL;
        buff[signal->length + L2CAP_OFFSET_1] = L2CAP_SIZE_9;
        buff[signal->length + L2CAP_OFFSET_2] = cfg->rfc.mode;
        buff[signal->length + L2CAP_OFFSET_3] = cfg->rfc.txWindowSize;
        buff[signal->length + L2CAP_OFFSET_4] = cfg->rfc.maxTransmit;
        L2capCpuToLe16(buff + signal->length + L2CAP_OFFSET_5, cfg->rfc.retransmissionTimeout);
        L2capCpuToLe16(buff + signal->length + L2CAP_OFFSET_7, cfg->rfc.monitorTimeout);
        L2capCpuToLe16(buff + signal->length + L2CAP_OFFSET_9, cfg->rfc.mps);
        signal->length += L2CAP_SIZE_11;
    } else {
        if (cfg->rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
            buff[signal->length + 0] = L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL;
            buff[signal->length + L2CAP_OFFSET_1] = L2CAP_SIZE_9;
            buff[signal->length + L2CAP_OFFSET_2] = cfg->rfc.mode;
            buff[signal->length + L2CAP_OFFSET_3] = chan->lcfg.rfc.txWindowSize;
            buff[signal->length + L2CAP_OFFSET_4] = 0;
            L2capCpuToLe16(buff + signal->length + L2CAP_OFFSET_5, chan->lcfg.rfc.retransmissionTimeout);
            L2capCpuToLe16(buff + signal->length + L2CAP_OFFSET_7, chan->lcfg.rfc.monitorTimeout);
            L2capCpuToLe16(buff + signal->length + L2CAP_OFFSET_9, chan->rcfg.rfc.mps);
            signal->length += L2CAP_SIZE_11;
        }
    }

    return;
}

int L2capSendConfigurationRsp(
    const L2capConnection *conn, L2capChannel *chan, uint8_t ident, uint16_t result, const L2capConfigInfo *cfg)
{
    Packet *pkt = NULL;
    uint8_t buff[48] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, 0);
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, result);
    signal.length = L2CAP_SIZE_6;

    if (chan->rcfg.mtu != cfg->mtu) {
        if (result == L2CAP_UNACCEPTABLE_PARAMETERS) {
            chan->rcfg.mtu = cfg->mtu;
        }

        buff[signal.length + 0] = L2CAP_OPTION_MAXIMUM_TRANSMISSION_UNIT;
        buff[signal.length + L2CAP_OFFSET_1] = L2CAP_SIZE_2;
        L2capCpuToLe16(buff + signal.length + L2CAP_OFFSET_2, cfg->mtu);
        signal.length += L2CAP_SIZE_4;
    }

    if (chan->rcfg.flushTimeout != cfg->flushTimeout) {
        if (result == L2CAP_UNACCEPTABLE_PARAMETERS) {
            chan->rcfg.flushTimeout = cfg->flushTimeout;
        }

        buff[signal.length + 0] = L2CAP_OPTION_FLUSH_TIMEOUT;
        buff[signal.length + L2CAP_OFFSET_1] = L2CAP_SIZE_2;
        L2capCpuToLe16(buff + signal.length + L2CAP_OFFSET_2, cfg->flushTimeout);
        signal.length += L2CAP_SIZE_4;
    }

    L2capSendConfigurationRspRfcProcess(chan, &signal, buff, result, cfg);

    signal.code = L2CAP_CONFIGURATION_RESPONSE;
    signal.identifier = ident;

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);

    if (result == L2CAP_SUCCESS) {
        chan->cfgState |= L2CAP_CONFIG_STATE_IN_DONE;
        L2capChannelConnected(chan);
    }

    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendDisconnectionReq(L2capConnection *conn, L2capChannel *chan)
{
    Packet *pkt = NULL;
    uint8_t buff[4] = {0};
    L2capSignalHeader signal = {0};

    BTM_ExitSniffMode(&(conn->addr));

    L2capCpuToLe16(buff + 0, chan->rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->lcid);

    signal.code = L2CAP_DISCONNECTION_REQUEST;
    signal.identifier = L2capGetNewIdentifier(conn);
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);

    chan->state = L2CAP_CHANNEL_DISCONNECT_OUT_REQ;
    L2capCreatePendingRequest(conn->pendingList, chan->lcid, &signal, L2CAP_DEFAULT_RTX, L2capResponseTimeoutCallback);
    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendDisconnectionRsp(const L2capConnection *conn, const L2capChannel *chan, uint8_t ident)
{
    Packet *pkt = NULL;
    uint8_t buff[4] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->lcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->rcid);

    signal.code = L2CAP_DISCONNECTION_RESPONSE;
    signal.identifier = ident;
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);

    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendInformationReq(L2capConnection *conn, uint16_t type)
{
    Packet *pkt = NULL;
    uint8_t buff[2] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, type);

    signal.code = L2CAP_INFORMATION_REQUEST;
    signal.identifier = L2capGetNewIdentifier(conn);
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);

    conn->info.state = L2CAP_INFO_STATE_PROCESSING;
    L2capCreatePendingRequest(conn->pendingList, 0, &signal, L2CAP_DEFAULT_RTX, L2capResponseTimeoutCallback);
    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

int L2capSendInformationRsp(const L2capConnection *conn, uint8_t ident, uint16_t infoType)
{
    Packet *pkt = NULL;
    uint8_t buff[L2CAP_OFFSET_12] = {0};
    L2capSignalHeader signal = {0};

    (void)memset_s(buff, sizeof(buff), 0, sizeof(buff));

    L2capCpuToLe16(buff + 0, infoType);

    if (infoType == L2CAP_INFORMATION_TYPE_EXTENDED_FEATURE) {
        L2capCpuToLe16(buff + L2CAP_OFFSET_2, 0);  // Success

        buff[L2CAP_OFFSET_4] = L2CAP_FEATURE_ENHANCED_RETRANSMISSION_MODE | L2CAP_FEATURE_STREAMING_MODE |
                               L2CAP_FEATURE_FCS_OPTION | L2CAP_FEATURE_FIXED_CHANNELS;
        signal.length = L2CAP_SIZE_8;
    } else if (infoType == L2CAP_INFORMATION_TYPE_FIXED_CHANNEL) {
        L2capCpuToLe16(buff + L2CAP_OFFSET_2, 0);  // Success

        buff[L2CAP_OFFSET_4] = L2CAP_FEATURE_SIGNALING_CHANNEl;
        signal.length = L2CAP_SIZE_12;
    } else {
        L2capCpuToLe16(buff + L2CAP_OFFSET_2, 1);  // Not supported
        signal.length = L2CAP_SIZE_4;
    }

    signal.code = L2CAP_INFORMATION_RESPONSE;
    signal.identifier = ident;

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);
    return L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

static uint16_t L2capGetNextSeq(uint16_t seq)
{
    return ((seq + 1) % L2CAP_MAX_TX_WINDOW);
}

static uint16_t L2capGetSeqWindow(uint16_t endSeq, uint16_t startSeq)
{
    uint16_t seqWindow;

    if (startSeq > endSeq) {
        seqWindow = endSeq + L2CAP_MAX_TX_WINDOW - startSeq;
    } else {
        seqWindow = endSeq - startSeq;
    }

    return seqWindow;
}

static uint16_t L2capGetTxWindow(const L2capErfc *erfc)
{
    return L2capGetSeqWindow(erfc->nextTxSeq, erfc->expectedAckSeq);
}

static uint16_t L2capGetRxWindow(const L2capErfc *erfc)
{
    return L2capGetSeqWindow(erfc->expectedTxSeq, erfc->bufferSeq);
}

static void L2capAddCrc(Packet *pkt)
{
    uint16_t crc;
    uint8_t *tail = NULL;

    crc = PacketCalCrc16(pkt, CalCrc16WithPrev);
    tail = BufferPtr(PacketTail(pkt));
    L2capCpuToLe16(tail, crc);

    return;
}

static int L2capCheckCrc(Packet *pkt)
{
    uint8_t tail[2] = {0};
    uint8_t *tailPtr = NULL;
    uint16_t fcs;
    uint16_t fcsCalc;

    PacketExtractTail(pkt, tail, sizeof(tail));

    tailPtr = tail;
    fcs = L2capLe16ToCpu(tailPtr);

    fcsCalc = PacketCalCrc16(pkt, CalCrc16WithPrev);
    if (fcs != fcsCalc) {
        LOG_ERROR("L2cap CRC Error, %{public}s:%{public}d", __FUNCTION__, __LINE__);
        return BT_BAD_PARAM;
    }

    return BT_NO_ERROR;
}

static Packet *L2capBuildSFrame(const L2capChannel *chan, const L2capErfcSControl *sCtrl)
{
    Packet *spkt = NULL;
    uint8_t *header = NULL;
    uint8_t tailLength = 0;

    if (chan->lcfg.fcs == 0x01) {
        tailLength = L2CAP_SIZE_2;
    }

    spkt = PacketMalloc(L2CAP_HEADER_LENGTH + L2CAP_SIZE_2, tailLength, 0);
    header = BufferPtr(PacketHead(spkt));

    L2capCpuToLe16(header + 0, tailLength + L2CAP_SIZE_2);
    L2capCpuToLe16(header + L2CAP_OFFSET_2, chan->rcid);
    if (memcpy_s(header + L2CAP_HEADER_LENGTH, sizeof(L2capErfcSControl), sCtrl, sizeof(L2capErfcSControl)) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        PacketFree(spkt);
        return NULL;
    }

    if (chan->lcfg.fcs == 0x01) {
        L2capAddCrc(spkt);
    }

    return spkt;
}

int L2capSendSFrame(const L2capConnection *conn, L2capChannel *chan, uint8_t pBit, uint8_t fBit, uint8_t sBit)
{
    L2capErfc *erfc = NULL;
    L2capErfcSControl sCtrl = {0};
    Packet *spkt = NULL;

    erfc = &(chan->erfc);

    sCtrl.type = L2CAP_SFRAME;
    sCtrl.reqSeq = erfc->expectedTxSeq;
    sCtrl.fBit = fBit;

    sCtrl.pBit = pBit;
    sCtrl.sBit = sBit;

    erfc->bufferSeq = erfc->expectedTxSeq;
    spkt = L2capBuildSFrame(chan, &sCtrl);

    L2capSendPacket(conn->aclHandle, chan->lcfg.flushTimeout, spkt);
    return BT_NO_ERROR;
}

static void L2capProcessRxReqSeq(L2capChannel *chan, uint16_t reqSeq)
{
    L2capErfc *erfc = NULL;
    ListNode *node = NULL;
    L2capErfcTxPacket *tx = NULL;
    uint16_t finishedPacketNum = 0;

    erfc = &(chan->erfc);
    if (erfc->nextTxSeq == reqSeq) {
        finishedPacketNum = L2capGetTxWindow(erfc);
    } else {
        uint8_t *header = NULL;
        L2capErfcIControl *ctrl = NULL;

        node = ListGetFirstNode(erfc->txList);
        while (node != NULL) {
            tx = ListGetNodeData(node);
            header = BufferPtr(PacketHead(tx->pkt));

            ctrl = (L2capErfcIControl *)(header + L2CAP_HEADER_LENGTH);
            if (ctrl->txSeq == erfc->nextTxSeq) {
                return;  // wrong ReqSeq, ignore
            }

            if (ctrl->txSeq == reqSeq) {
                break;
            }

            finishedPacketNum += 1;
            node = ListGetNextNode(node);
        }
    }

    erfc->expectedAckSeq = reqSeq;

    if (finishedPacketNum > 0) {
        AlarmCancel(chan->erfc.retransmissionTimer);

        if (L2capGetTxWindow(erfc) > 0) {
            L2capErfcStartRetransmissionTimer(chan);
        }
    }

    while (finishedPacketNum) {
        node = ListGetFirstNode(erfc->txList);
        tx = ListGetNodeData(node);

        ListRemoveNode(erfc->txList, tx);
        PacketFree(tx->pkt);
        L2capFree(tx);

        finishedPacketNum -= 1;
    }

    return;
}

static void L2capErfcTxOneFrame(L2capConnection *conn, L2capChannel *chan, uint8_t txSeq)
{
    ListNode *node = NULL;
    L2capErfcTxPacket *tx = NULL;
    uint8_t *pktHeader = NULL;
    L2capErfcIControl *ctrl = NULL;

    L2capErfc *erfc = &(chan->erfc);

    if (erfc->txList == NULL) {
        return;
    }

    node = ListGetFirstNode(erfc->txList);
    while (node != NULL) {
        tx = ListGetNodeData(node);
        pktHeader = BufferPtr(PacketHead(tx->pkt));

        ctrl = (L2capErfcIControl *)(pktHeader + L2CAP_HEADER_LENGTH);
        if (ctrl->txSeq == erfc->nextTxSeq) {
            break;
        }

        if (ctrl->txSeq == txSeq) {
            if (tx->retryCount == chan->lcfg.rfc.maxTransmit) {
                L2capSendDisconnectionReq(conn, chan);
                return;
            }

            ctrl->reqSeq = erfc->expectedTxSeq;
            if (chan->lcfg.fcs == 0x01) {
                L2capAddCrc(tx->pkt);
            }

            L2capSendPacketNoFree(conn->aclHandle, chan->lcfg.flushTimeout, tx->pkt);
            break;
        }

        node = ListGetNextNode(node);
    }

    return;
}

static void L2capErfcRemoteBusyProcess(L2capChannel *chan, uint8_t isBusy)
{
    L2capErfc *erfc = NULL;
    L2capPsm *psm = NULL;

    erfc = &(chan->erfc);

    if (isBusy) {
        if (erfc->busyState & L2CAP_BUSY_REMOTE_BUSY) {
            return;
        }

        erfc->busyState |= L2CAP_BUSY_REMOTE_BUSY;
    } else {
        if (!(erfc->busyState & L2CAP_BUSY_REMOTE_BUSY)) {
            return;
        }

        erfc->busyState &= (~L2CAP_BUSY_REMOTE_BUSY);
    }

    psm = L2capGetPsm(chan->lpsm);
    if (psm != NULL) {
        LOG_DEBUG("L2capCallback remoteBusy:%{public}d begin, cid = 0x%04X, busyState = %{public}d", __LINE__, chan->lcid, isBusy);
        psm->service.remoteBusy(chan->lcid, isBusy, psm->ctx);
        LOG_DEBUG("L2capCallback remoteBusy:%{public}d end", __LINE__);
    }

    return;
}

static void L2capErfcMonitorTimeout(const void *parameter)
{
    L2capInstance *inst = L2capGetInstance();
    L2capChannel *chan = NULL;
    L2capConnection *conn = NULL;

    if (inst->connList == NULL) {
        return;
    }

    chan = (L2capChannel *)parameter;
    conn = L2capGetConnection3(chan);
    if (conn == NULL) {
        return;
    }

    if (chan->erfc.retryCount == chan->lcfg.rfc.maxTransmit) {
        L2capSendDisconnectionReq(conn, chan);
        return;
    }

    if (chan->erfc.busyState & L2CAP_BUSY_LOCAL_BUSY) {
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_ON, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RNR);
    } else {
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_ON, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RR);
    }

    chan->erfc.retryCount += 1;
    L2capErfcStartMonitorTimer(chan);
    return;
}

static void L2capErfcMonitorTimeoutCallback(void *parameter)
{
    L2capAsynchronousProcess(L2capErfcMonitorTimeout, NULL, parameter);
    return;
}

void L2capErfcStartMonitorTimer(L2capChannel *chan)
{
    if (chan->erfc.monitorTimer == NULL) {
        chan->erfc.monitorTimer = AlarmCreate("", false);
    } else {
        AlarmCancel(chan->erfc.monitorTimer);
    }

    AlarmSet(chan->erfc.monitorTimer, chan->lcfg.rfc.monitorTimeout, L2capErfcMonitorTimeoutCallback, chan);
    return;
}

static void L2capErfcRetransmissionTimeout(const void *parameter)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;

    if (inst->connList == NULL) {
        return;
    }

    chan = (L2capChannel *)parameter;
    conn = L2capGetConnection3(chan);
    if (conn == NULL) {
        return;
    }

    if (chan->erfc.busyState & L2CAP_BUSY_LOCAL_BUSY) {
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_ON, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RNR);
    } else {
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_ON, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RR);
    }

    chan->erfc.retryCount = 1;
    L2capErfcStartMonitorTimer(chan);

    chan->erfc.busyState |= L2CAP_BUSY_WAIT_F;
    L2capErfcRemoteBusyProcess(chan, 1);
    return;
}

static void L2capErfcRetransmissionTimeoutCallback(void *parameter)
{
    L2capAsynchronousProcess(L2capErfcRetransmissionTimeout, NULL, parameter);
    return;
}

void L2capErfcStartRetransmissionTimer(L2capChannel *chan)
{
    if (chan->erfc.retransmissionTimer == NULL) {
        chan->erfc.retransmissionTimer = AlarmCreate("", false);
    } else {
        AlarmCancel(chan->erfc.retransmissionTimer);
    }

    AlarmSet(chan->erfc.retransmissionTimer,
        chan->lcfg.rfc.retransmissionTimeout,
        L2capErfcRetransmissionTimeoutCallback,
        chan);
    return;
}

static void L2capErfcDoTx(L2capConnection *conn, L2capChannel *chan, uint16_t txWindow, const ListNode *node)
{
    uint8_t *header = NULL;
    L2capErfcTxPacket *tx = NULL;
    L2capErfcIControl *ctrl = NULL;

    L2capErfc *erfc = &(chan->erfc);

    while ((node != NULL) && txWindow) {
        tx = ListGetNodeData(node);
        if (tx->retryCount == chan->lcfg.rfc.maxTransmit) {
            L2capSendDisconnectionReq(conn, chan);
            return;
        }

        header = BufferPtr(PacketHead(tx->pkt));

        ctrl = (L2capErfcIControl *)(header + L2CAP_HEADER_LENGTH);
        ctrl->reqSeq = erfc->expectedTxSeq;

        if (chan->lcfg.fcs == 0x01) {
            L2capAddCrc(tx->pkt);
        }

        tx->retryCount += 1;
        L2capSendPacketNoFree(conn->aclHandle, chan->lcfg.flushTimeout, tx->pkt);

        if (txWindow == chan->lcfg.rfc.txWindowSize) {
            L2capErfcStartRetransmissionTimer(chan);
        }

        erfc->bufferSeq = erfc->expectedTxSeq;
        erfc->nextTxSeq = L2capGetNextSeq(erfc->nextTxSeq);

        txWindow -= 1;
        node = ListGetNextNode(node);
    }

    if (txWindow == 0) {
        L2capErfcRemoteBusyProcess(chan, 1);
    } else {
        L2capErfcRemoteBusyProcess(chan, 0);
    }

    return;
}

static void L2capErfcTx(L2capConnection *conn, L2capChannel *chan)
{
    uint16_t txWindow;
    ListNode *node = NULL;
    L2capErfcTxPacket *tx = NULL;
    uint8_t *header = NULL;
    L2capErfcIControl *ctrl = NULL;

    L2capErfc *erfc = &(chan->erfc);

    if (erfc->txList == NULL) {
        return;
    }

    if ((erfc->busyState & L2CAP_BUSY_REMOTE_RNR) || (erfc->busyState & L2CAP_BUSY_WAIT_F)) {
        return;
    }

    txWindow = chan->lcfg.rfc.txWindowSize - L2capGetTxWindow(erfc);
    if (txWindow == 0) {
        return;
    }

    node = ListGetFirstNode(erfc->txList);
    while (node != NULL) {
        tx = ListGetNodeData(node);
        header = BufferPtr(PacketHead(tx->pkt));

        ctrl = (L2capErfcIControl *)(header + L2CAP_HEADER_LENGTH);
        if (ctrl->txSeq == erfc->nextTxSeq) {
            break;
        }

        node = ListGetNextNode(node);
    }

    L2capErfcDoTx(conn, chan, txWindow, node);
    return;
}

static void L2capStreamTx(const L2capConnection *conn, L2capChannel *chan)
{
    ListNode *node = NULL;
    Packet *pkt = NULL;

    L2capErfc *erfc = &(chan->erfc);

    if (erfc->txList == NULL) {
        return;
    }

    while (1) {
        node = ListGetFirstNode(erfc->txList);
        if (node == NULL) {
            break;
        }

        pkt = ListGetNodeData(node);
        ListRemoveNode(erfc->txList, pkt);

        if (chan->lcfg.fcs == 0x01) {
            L2capAddCrc(pkt);
        }

        L2capSendPacket(conn->aclHandle, chan->lcfg.flushTimeout, pkt);
    }

    return;
}

static Packet *L2capBuildIFrame(L2capChannel *chan, const Packet *pkt, uint8_t sar, uint16_t sduLength)
{
    L2capErfc *erfc = NULL;
    L2capErfcIControl *iCtrl = NULL;
    Packet *ipkt = NULL;
    uint8_t *header = NULL;
    uint16_t headerLength;
    uint16_t tailLength = 0;

    erfc = &(chan->erfc);
    headerLength = L2CAP_HEADER_LENGTH + L2CAP_SIZE_2;
    if (sar == L2CAP_ERFC_START_SDU) {
        headerLength = L2CAP_HEADER_LENGTH + L2CAP_SIZE_4;
    }

    if (chan->lcfg.fcs == 0x01) {
        tailLength = L2CAP_SIZE_2;
    }

    ipkt = PacketInheritMalloc(pkt, headerLength, tailLength);
    header = BufferPtr(PacketHead(ipkt));

    L2capCpuToLe16(header + 0, PacketSize(ipkt) - L2CAP_HEADER_LENGTH);
    L2capCpuToLe16(header + L2CAP_OFFSET_2, chan->rcid);
    iCtrl = (L2capErfcIControl *)(header + L2CAP_HEADER_LENGTH);

    iCtrl->type = L2CAP_IFRAME;
    iCtrl->txSeq = erfc->txSeq;
    iCtrl->reqSeq = 0;
    iCtrl->fBit = L2CAP_ERFC_FBIT_OFF;
    iCtrl->sar = sar;

    if (sar == L2CAP_ERFC_START_SDU) {
        L2capCpuToLe16(header + L2CAP_OFFSET_6, sduLength);
    }

    return ipkt;
}

static void L2capAddNewPacket(L2capChannel *chan, Packet *pkt)
{
    if (chan->lcfg.rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
        L2capErfcTxPacket *tx = NULL;

        tx = L2capAlloc(sizeof(L2capErfcTxPacket));
        if (tx == NULL) {
            return;
        }
        tx->pkt = pkt;
        tx->retryCount = 0;

        ListAddLast(chan->erfc.txList, tx);
    } else if (chan->lcfg.rfc.mode == L2CAP_STREAM_MODE) {
        ListAddLast(chan->erfc.txList, pkt);
    }

    return;
}

int L2capSendIFrame(L2capConnection *conn, L2capChannel *chan, Packet *pkt)
{
    L2capErfc *erfc = NULL;
    Packet *ipkt = NULL;
    uint16_t length;
    uint8_t sar;

    erfc = &(chan->erfc);
    if (erfc->txList == NULL) {
        erfc->txList = ListCreate(NULL);
    }

    length = PacketSize(pkt);
    if (length > chan->rcfg.rfc.mps) {
        Packet *frag = NULL;
        sar = L2CAP_ERFC_START_SDU;
        while (1) {
            frag = PacketMalloc(0, 0, 0);
            int remainLength = PacketFragment(pkt, frag, chan->rcfg.rfc.mps);

            ipkt = L2capBuildIFrame(chan, frag, sar, length);
            PacketFree(frag);

            L2capAddNewPacket(chan, ipkt);
            erfc->txSeq = L2capGetNextSeq(erfc->txSeq);

            if (remainLength == 0) {
                break;
            }

            sar = L2CAP_ERFC_END_SDU;
            if (remainLength > chan->rcfg.rfc.mps) {
                sar = L2CAP_ERFC_CONTINUATION_SDU;
            }
        }
    } else {
        sar = L2CAP_ERFC_UNSEGMENTED_SDU;
        ipkt = L2capBuildIFrame(chan, pkt, sar, 0);

        L2capAddNewPacket(chan, ipkt);
        erfc->txSeq = L2capGetNextSeq(erfc->txSeq);
    }

    if (chan->lcfg.rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
        L2capErfcTx(conn, chan);
    } else if (chan->lcfg.rfc.mode == L2CAP_STREAM_MODE) {
        L2capStreamTx(conn, chan);
    }

    return BT_NO_ERROR;
}

static void L2capErfcProcessFBit(L2capChannel *chan, uint8_t fBit, uint8_t reqSeq)
{
    if (chan->erfc.busyState & L2CAP_BUSY_WAIT_F) {
        if (fBit == L2CAP_ERFC_FBIT_ON) {
            AlarmCancel(chan->erfc.monitorTimer);
            chan->erfc.busyState &= (~L2CAP_BUSY_WAIT_F);
            chan->erfc.retryCount = 0;
            chan->erfc.nextTxSeq = reqSeq;
        }
    }

    return;
}

static void L2capErfcProcessSFrame(L2capConnection *conn, L2capChannel *chan, uint8_t ctrl[2])
{
    L2capErfc *erfc = NULL;
    L2capErfcSControl *sCtrl = NULL;

    erfc = &(chan->erfc);
    sCtrl = (L2capErfcSControl *)ctrl;

    if ((sCtrl->sBit != L2CAP_ERFC_SREJ) || (sCtrl->pBit != L2CAP_ERFC_PBIT_OFF)) {
        L2capProcessRxReqSeq(chan, sCtrl->reqSeq);
    }

    L2capErfcProcessFBit(chan, sCtrl->fBit, sCtrl->reqSeq);

    if (sCtrl->pBit == L2CAP_ERFC_PBIT_ON) {
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_OFF, L2CAP_ERFC_FBIT_ON, L2CAP_ERFC_RR);
    }

    if (sCtrl->sBit == L2CAP_ERFC_RR) {
        if (erfc->busyState & L2CAP_BUSY_REMOTE_RNR) {
            if (sCtrl->pBit == L2CAP_ERFC_PBIT_ON) {
                erfc->busyState &= (~L2CAP_BUSY_REMOTE_RNR);
                L2capErfcTx(conn, chan);
            }
        } else {
            L2capErfcTx(conn, chan);
        }
    } else if (sCtrl->sBit == L2CAP_ERFC_REJ) {
        erfc->nextTxSeq = sCtrl->reqSeq;
        L2capErfcTx(conn, chan);
    } else if (sCtrl->sBit == L2CAP_ERFC_RNR) {
        erfc->busyState |= L2CAP_BUSY_REMOTE_RNR;
        L2capErfcRemoteBusyProcess(chan, 1);
    } else if (sCtrl->sBit == L2CAP_ERFC_SREJ) {
        L2capErfcTxOneFrame(conn, chan, sCtrl->reqSeq);
    }

    return;
}

static Packet *L2capReassembleIFrame(L2capErfc *erfc, const Packet *pkt, L2capErfcIControl *iCtrl)
{
    Packet *ipkt = NULL;

    if (iCtrl->sar == L2CAP_ERFC_START_SDU) {
        if (erfc->rxSarPacket != NULL) {
            PacketFree(erfc->rxSarPacket);
        }

        erfc->rxSarPacket = PacketRefMalloc(pkt);
    } else if (iCtrl->sar == L2CAP_ERFC_CONTINUATION_SDU) {
        if (erfc->rxSarPacket == NULL) {
            return NULL;
        }

        PacketAssemble(erfc->rxSarPacket, pkt);
    } else if (iCtrl->sar == L2CAP_ERFC_END_SDU) {
        uint8_t header[2] = {0};
        uint8_t *headerPtr = NULL;
        uint16_t sduLength;

        if (erfc->rxSarPacket == NULL) {
            return NULL;
        }

        PacketAssemble(erfc->rxSarPacket, pkt);

        PacketExtractHead(erfc->rxSarPacket, header, sizeof(header));

        headerPtr = header;
        sduLength = L2capLe16ToCpu(headerPtr);
        if (sduLength != PacketSize(erfc->rxSarPacket)) {
            PacketFree(erfc->rxSarPacket);
            erfc->rxSarPacket = NULL;
            return NULL;
        }

        ipkt = erfc->rxSarPacket;
        erfc->rxSarPacket = NULL;
    } else if (iCtrl->sar == L2CAP_ERFC_UNSEGMENTED_SDU) {
        ipkt = PacketRefMalloc(pkt);
    }

    return ipkt;
}

static Packet *L2capErfcProcessIFrame(L2capConnection *conn, L2capChannel *chan, const Packet *pkt, uint8_t ctrl[2])
{
    L2capErfc *erfc = NULL;
    L2capErfcIControl *iCtrl = NULL;
    Packet *ipkt = NULL;

    erfc = &(chan->erfc);
    iCtrl = (L2capErfcIControl *)ctrl;

    L2capProcessRxReqSeq(chan, iCtrl->reqSeq);

    L2capErfcProcessFBit(chan, iCtrl->fBit, iCtrl->reqSeq);

    if (erfc->expectedTxSeq != iCtrl->txSeq) {
        if (erfc->rejState == 0) {
            L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_OFF, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_REJ);
            erfc->rejState = 1;
        }

        return NULL;
    }

    erfc->rejState = 0;
    erfc->expectedTxSeq = L2capGetNextSeq(erfc->expectedTxSeq);

    ipkt = L2capReassembleIFrame(erfc, pkt, iCtrl);

    if (L2capGetRxWindow(erfc) == (chan->rcfg.rfc.txWindowSize - 1)) {
        L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_OFF, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RR);
    } else {
        if (ipkt != NULL) {                        // full packet received
            if (erfc->nextTxSeq == erfc->txSeq) {  // no pending packet in list
                L2capSendSFrame(conn, chan, L2CAP_ERFC_PBIT_OFF, L2CAP_ERFC_FBIT_OFF, L2CAP_ERFC_RR);
            }
        }
    }

    L2capErfcTx(conn, chan);
    return ipkt;
}

static Packet *L2capStreamProcessIFrame(L2capChannel *chan, const Packet *pkt, L2capErfcIControl *iCtrl)
{
    L2capErfc *erfc = NULL;
    Packet *ipkt = NULL;

    erfc = &(chan->erfc);
    if (erfc->expectedTxSeq != iCtrl->txSeq) {
        if (erfc->rxSarPacket != NULL) {
            PacketFree(erfc->rxSarPacket);
            erfc->rxSarPacket = NULL;
        }
    }

    erfc->expectedTxSeq = L2capGetNextSeq(iCtrl->txSeq);
    ipkt = L2capReassembleIFrame(erfc, pkt, iCtrl);

    return ipkt;
}

static int L2capCheckConfigurationOptionLength(uint8_t optType, uint8_t optLength)
{
    switch (optType) {
        case L2CAP_OPTION_MAXIMUM_TRANSMISSION_UNIT:
            if (optLength != L2CAP_SIZE_2) {
                return BT_BAD_PARAM;
            }

            break;
        case L2CAP_OPTION_FLUSH_TIMEOUT:
            if (optLength != L2CAP_SIZE_2) {
                return BT_BAD_PARAM;
            }

            break;
        case L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL:
            if (optLength != L2CAP_SIZE_9) {
                return BT_BAD_PARAM;
            }

            break;
        case L2CAP_OPTION_FRAME_CHECK_SEQUENCE:
            if (optLength != L2CAP_SIZE_1) {
                return BT_BAD_PARAM;
            }

            break;
        case L2CAP_OPTION_QUALITY_OF_SERVICE:
            if (optLength != L2CAP_SIZE_22) {
                return BT_BAD_PARAM;
            }

            break;
        default:
            break;
    }

    return BT_NO_ERROR;
}

static void L2capProcessErfcOption(const uint8_t *data, uint16_t offset, L2capConfigInfo *cfg)
{
    cfg->rfc.mode = data[offset + L2CAP_OFFSET_2];
    cfg->rfc.txWindowSize = data[offset + L2CAP_OFFSET_3];
    cfg->rfc.maxTransmit = data[offset + L2CAP_OFFSET_4];

    cfg->rfc.retransmissionTimeout = L2capLe16ToCpu(data + offset + L2CAP_OFFSET_5);
    cfg->rfc.monitorTimeout = L2capLe16ToCpu(data + offset + L2CAP_OFFSET_7);
    cfg->rfc.mps = L2capLe16ToCpu(data + offset + L2CAP_OFFSET_9);

    return;
}

static int L2capParseConfiguration(const uint8_t *data, uint16_t length, L2capConfigInfo *cfg, L2capOptions *unknown)
{
    uint16_t offset = 0;

    while (length > (offset + L2CAP_SIZE_2)) {
        uint8_t optType = data[offset + 0];
        uint8_t optLength = data[offset + L2CAP_OFFSET_1];

        if (length < (offset + optLength + L2CAP_SIZE_2)) {
            return BT_BAD_PARAM;
        }

        if (L2capCheckConfigurationOptionLength(optType, optLength) != BT_NO_ERROR) {
            return BT_BAD_PARAM;
        }

        switch (optType) {
            case L2CAP_OPTION_MAXIMUM_TRANSMISSION_UNIT:
                cfg->mtu = L2capLe16ToCpu(data + offset + L2CAP_OFFSET_2);
                break;
            case L2CAP_OPTION_FLUSH_TIMEOUT:
                cfg->flushTimeout = L2capLe16ToCpu(data + offset + L2CAP_OFFSET_2);
                break;
            case L2CAP_OPTION_RETRANSMISSION_AND_FLOW_CONTROL:
                L2capProcessErfcOption(data, offset, cfg);
                break;
            case L2CAP_OPTION_FRAME_CHECK_SEQUENCE:
                cfg->fcs = data[offset + L2CAP_OFFSET_2];
                break;
            case L2CAP_OPTION_QUALITY_OF_SERVICE:
                if ((data[offset + L2CAP_OFFSET_3] != L2CAP_QOS_SERVICE_TYPE_BEST_EFFORT) &&
                    (data[offset + L2CAP_OFFSET_3] != L2CAP_QOS_SERVICE_TYPE_GUARANTEED)) {
                    return BT_BAD_PARAM;
                }
                break;
            case L2CAP_OPTION_EXTENDED_FLOW_SPECIFICATION:  // dummy
            case L2CAP_OPTION_EXTENDED_WINDOW_SIZE:
                return BT_BAD_PARAM;
            default:
                if ((optType & L2CAP_OPTION_HINT) || (unknown == NULL)) {
                    break;
                }

                if (unknown->options == NULL) {
                    unknown->options = L2capAlloc(length);
                }

                (void)memcpy_s(unknown->options + unknown->length,
                    optLength + L2CAP_SIZE_2,
                    data + offset,
                    optLength + L2CAP_SIZE_2);
                unknown->length += optLength + L2CAP_SIZE_2;
                break;
        }

        offset += (optLength + L2CAP_SIZE_2);
    }

    return BT_NO_ERROR;
}

static void L2capConfigurationRspReject(const L2capConnection *conn, uint8_t ident, uint16_t rcid)
{
    Packet *pkt = NULL;
    uint8_t buff[6] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, (uint16_t)0);  // continuation flag
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, (uint16_t)L2CAP_REJECTED);

    signal.length = L2CAP_SIZE_6;
    signal.code = L2CAP_CONFIGURATION_RESPONSE;
    signal.identifier = ident;

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);
    L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
    return;
}

static void L2capConfigurationRspUnknownOptions(
    const L2capConnection *conn, uint8_t ident, uint16_t rcid, L2capOptions *unknown)
{
    Packet *pkt = NULL;
    uint8_t buff[L2CAP_SIGNAL_MTU] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, (uint16_t)0);  // continuation flag
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, (uint16_t)L2CAP_UNKNOWN_OPTIONS);

    if (memcpy_s(buff + L2CAP_OFFSET_6, L2CAP_SIGNAL_MTU - L2CAP_OFFSET_6, unknown->options, unknown->length) != EOK) {
        LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
        return;
    }

    signal.length = L2CAP_SIZE_6 + unknown->length;
    signal.code = L2CAP_CONFIGURATION_RESPONSE;
    signal.identifier = ident;

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);
    L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
    return;
}

static void L2capConfigurationRspContinuation(const L2capConnection *conn, uint8_t ident, uint16_t rcid)
{
    Packet *pkt = NULL;
    uint8_t buff[6] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, (uint16_t)1);  // continuation flag
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, (uint16_t)L2CAP_SUCCESS);

    signal.length = L2CAP_SIZE_6;
    signal.code = L2CAP_CONFIGURATION_RESPONSE;
    signal.identifier = ident;

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);
    L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
    return;
}

static void L2capDestroyPartOption(L2capChannel *chan)
{
    if (chan->part.options != NULL) {
        L2capFree(chan->part.options);
        chan->part.options = NULL;
        chan->part.length = 0;
    }

    return;
}

static void L2capProcessConfigurationReqWithContinue(
    L2capConnection *conn, L2capChannel *chan, L2capSignalHeader *signal, const uint8_t *data, uint16_t optLength)
{
    if (optLength > 0) {
        if (chan->part.options == NULL) {
            chan->part.options = L2capAlloc(L2CAP_SIGNAL_MTU);
            if (chan->part.options == NULL) {
                LOG_WARN("malloc failed");
                return;
            }
            chan->part.length = 0;
        }

        if ((chan->part.length + optLength) > L2CAP_SIGNAL_MTU) {
            L2capSendCommandReject(
                conn->aclHandle, L2CAP_SIGNALING_CHANNEL, signal->identifier, L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);

            L2capDestroyPartOption(chan);
            return;
        }

        if (memcpy_s(chan->part.options + chan->part.length, optLength, data, optLength) != EOK) {
            LOG_ERROR("L2capProcessConfigurationReqWithContinue memcpy_s failed");
            return;
        }
        chan->part.length += optLength;
    }

    L2capConfigurationRspContinuation(conn, signal->identifier, chan->rcid);
    return;
}

static int L2capProcessConfigurationReqContinueOption(
    L2capConnection *conn, L2capChannel *chan, L2capSignalHeader *signal, const uint8_t *data, uint16_t optLength)
{
    if ((chan->part.length + optLength) > L2CAP_SIGNAL_MTU) {
        L2capSendCommandReject(
            conn->aclHandle, L2CAP_SIGNALING_CHANNEL, signal->identifier, L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);

        L2capDestroyPartOption(chan);
        return BT_BAD_PARAM;
    }

    if (optLength > 0) {
        if (memcpy_s(chan->part.options + chan->part.length, optLength, data, optLength) != EOK) {
            LOG_ERROR("[%{public}s][%{public}d] memcpy_s fail.", __FUNCTION__, __LINE__);
            return BT_NO_MEMORY;
        }
        chan->part.length += optLength;
    }

    return BT_NO_ERROR;
}

NO_SANITIZE("cfi") static void L2capProcessConfigurationReqParse(
    L2capConnection *conn, L2capChannel *chan, L2capSignalHeader *signal, const uint8_t *data, uint16_t optLength)
{
    L2capConfigInfo cfg = {0};
    L2capPsm *psm = NULL;

    (void)memcpy_s(&cfg, sizeof(L2capConfigInfo), &(chan->rcfg), sizeof(L2capConfigInfo));
    if (optLength > 0) {
        L2capOptions unknown = {0};

        if (L2capParseConfiguration(data, optLength, &cfg, &unknown) != BT_NO_ERROR) {
            if (unknown.options != NULL) {
                L2capFree(unknown.options);
            }

            L2capConfigurationRspReject(conn, signal->identifier, chan->rcid);
            L2capDestroyPartOption(chan);
            return;
        }

        if (unknown.options != NULL) {
            L2capConfigurationRspUnknownOptions(conn, signal->identifier, chan->rcid, &unknown);
            L2capFree(unknown.options);
            L2capDestroyPartOption(chan);
            return;
        }

        chan->rcfg.mtu = cfg.mtu;
        chan->rcfg.fcs = cfg.fcs;
        chan->rcfg.flushTimeout = cfg.flushTimeout;
        chan->rcfg.rfc.mode = cfg.rfc.mode;
        chan->rcfg.rfc.maxTransmit = cfg.rfc.maxTransmit;
        chan->rcfg.rfc.rxWindowSize = cfg.rfc.txWindowSize;
        chan->rcfg.rfc.mps = cfg.rfc.mps;
    }

    L2capDestroyPartOption(chan);

    psm = L2capGetPsm(chan->lpsm);
    if (psm != NULL) {
        LOG_DEBUG(
            "L2capCallback recvConfigReq:%{public}d begin, cid = 0x%04X, id = %hhu", __LINE__, chan->lcid, signal->identifier);
        psm->service.recvConfigReq(chan->lcid, signal->identifier, &cfg, psm->ctx);
        LOG_DEBUG("L2capCallback recvConfigReq:%{public}d end", __LINE__);
    }

    return;
}

static void L2capProcessConfigurationReq(uint16_t aclHandle, L2capSignalHeader *signal, const uint8_t *data)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    uint16_t cid;
    uint16_t flag;
    uint16_t optLength;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    cid = L2capLe16ToCpu(data + 0);
    flag = L2capLe16ToCpu(data + L2CAP_OFFSET_2);

    optLength = signal->length - L2CAP_SIZE_4;
    data += L2CAP_SIZE_4;

    chan = L2capGetChannel(conn, cid);
    if (chan == NULL) {
        uint16_t rejCid[2] = {0, cid};

        L2capSendCommandReject(
            aclHandle, L2CAP_SIGNALING_CHANNEL, signal->identifier, L2CAP_INVALID_CID_IN_REQUEST, rejCid);
        return;
    }

    if (flag & 0x0001) {  // continuation flag is set
        L2capProcessConfigurationReqWithContinue(conn, chan, signal, data, optLength);
        return;
    }

    if (chan->part.options != NULL) {
        if (L2capProcessConfigurationReqContinueOption(conn, chan, signal, data, optLength) != BT_NO_ERROR) {
            return;
        }

        data = chan->part.options;
        optLength = chan->part.length;
    }

    L2capProcessConfigurationReqParse(conn, chan, signal, data, optLength);
    return;
}

static void L2capConfigurationReqContinuation(L2capConnection *conn, const L2capChannel *chan)
{
    Packet *pkt = NULL;
    uint8_t buff[4] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, 0);
    signal.length = sizeof(buff);

    signal.code = L2CAP_CONFIGURATION_REQUEST;
    signal.identifier = L2capGetNewIdentifier(conn);

    pkt = L2capBuildSignalPacket(L2CAP_SIGNALING_CHANNEL, &signal, buff);

    L2capCreatePendingRequest(conn->pendingList, chan->lcid, &signal, L2CAP_DEFAULT_RTX, L2capResponseTimeoutCallback);
    L2capSendPacket(conn->aclHandle, L2CAP_NONE_FLUSH_PACKET, pkt);
    return;
}

NO_SANITIZE("cfi")
static void L2capProcessConfigurationRsp(uint16_t aclHandle, L2capSignalHeader *signal, uint8_t *data)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    uint16_t cid;
    uint16_t flag;
    uint16_t result;
    L2capConfigInfo cfg = {0};
    L2capPsm *psm = NULL;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);

    cid = L2capLe16ToCpu(data + 0);
    flag = L2capLe16ToCpu(data + L2CAP_OFFSET_2);
    result = L2capLe16ToCpu(data + L2CAP_OFFSET_4);

    chan = L2capGetChannel(conn, cid);
    if (chan == NULL) {
        return;
    }

    (void)memcpy_s(&cfg, sizeof(L2capConfigInfo), &(chan->lcfg), sizeof(L2capConfigInfo));
    if (signal->length > L2CAP_SIZE_6) {
        L2capParseConfiguration(data + L2CAP_OFFSET_6, signal->length - L2CAP_SIZE_6, &cfg, NULL);

        if (cfg.rfc.mode == L2CAP_ENHANCED_RETRANSMISSION_MODE) {
            chan->rcfg.rfc.retransmissionTimeout = cfg.rfc.retransmissionTimeout;
            chan->rcfg.rfc.monitorTimeout = cfg.rfc.monitorTimeout;
            chan->rcfg.rfc.txWindowSize = cfg.rfc.txWindowSize;
        }
    }

    if (flag & 0x0001) {  // continuation flag is set
        L2capConfigurationReqContinuation(conn, chan);
        return;
    }

    if (result == L2CAP_SUCCESS) {
        chan->cfgState |= L2CAP_CONFIG_STATE_OUT_DONE;
        L2capChannelConnected(chan);
    }

    if (result == L2CAP_PENDING) {
        signal->code = L2CAP_CONFIGURATION_REQUEST;
        L2capCreatePendingRequest(
            conn->pendingList, chan->lcid, signal, L2CAP_DEFAULT_ERTX, L2capResponseTimeoutCallback);
    }

    psm = L2capGetPsm(chan->lpsm);
    if (psm != NULL) {
        LOG_DEBUG("L2capCallback recvConfigRsp:%{public}d begin, cid = 0x%04X, result = %hu", __LINE__, cid, result);
        psm->service.recvConfigRsp(cid, &cfg, result, psm->ctx);
        LOG_DEBUG("L2capCallback recvConfigRsp:%{public}d end", __LINE__);
    }

    return;
}

NO_SANITIZE("cfi")
static void L2capProcessConnectionReq(uint16_t aclHandle, const L2capSignalHeader *signal, uint8_t *data)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    L2capPsm *psm = NULL;
    uint16_t lpsm;
    uint16_t cid;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    BTM_ExitSniffMode(&(conn->addr));

    lpsm = L2capLe16ToCpu(data + 0);
    cid = L2capLe16ToCpu(data + L2CAP_OFFSET_2);

    psm = L2capGetPsm(lpsm);
    if (psm == NULL) {
        L2capChannel tchan;

        tchan.lcid = 0;
        tchan.rcid = cid;
        L2capSendConnectionRsp(
            conn, &tchan, signal->identifier, L2CAP_PSM_NOT_SUPPORTED, L2CAP_NO_FURTHER_INFORMATION_AVAILABLE);
        return;
    }

    chan = L2capNewChannel(conn, lpsm, lpsm);
    chan->rcid = cid;
    chan->state = L2CAP_CHANNEL_CONNECT_IN_REQ;
    chan->connIdentifier = signal->identifier;

    if (conn->info.state == L2CAP_INFO_STATE_NONE) {
        L2capSendConnectionRsp(
            conn, chan, signal->identifier, L2CAP_CONNECTION_PENDING, L2CAP_NO_FURTHER_INFORMATION_AVAILABLE);
        L2capSendInformationReq(conn, L2CAP_INFORMATION_TYPE_EXTENDED_FEATURE);
    } else if (conn->info.state == L2CAP_INFO_STATE_DONE) {
        L2capConnectionInfo connInfo = {0};

        LOG_DEBUG("L2capCallback recvConnectionReq:%{public}d begin, aclHandle = %hu, cid = 0x%04X, id = %hhu, lpsm = %hu",
            __LINE__,
            aclHandle,
            chan->lcid,
            signal->identifier,
            psm->lpsm);

        connInfo.handle = aclHandle;
        (void)memcpy_s(&(connInfo.addr), sizeof(BtAddr), &(conn->addr), sizeof(BtAddr));
        psm->service.recvConnectionReq(chan->lcid, signal->identifier, &connInfo, psm->lpsm, psm->ctx);
        LOG_DEBUG("L2capCallback recvConnectionReq:%{public}d end", __LINE__);
    } else {
        L2capSendConnectionRsp(
            conn, chan, signal->identifier, L2CAP_CONNECTION_PENDING, L2CAP_NO_FURTHER_INFORMATION_AVAILABLE);
    }

    return;
}

static void L2capProcessConnectionRsp(uint16_t aclHandle, L2capSignalHeader *signal, uint8_t *data)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    L2capPsm *psm = NULL;
    uint16_t lcid;
    uint16_t rcid;
    uint16_t result;
    uint16_t status;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);

    rcid = L2capLe16ToCpu(data + 0);
    lcid = L2capLe16ToCpu(data + L2CAP_OFFSET_2);
    result = L2capLe16ToCpu(data + L2CAP_OFFSET_4);
    status = L2capLe16ToCpu(data + L2CAP_OFFSET_6);

    chan = L2capGetChannel(conn, lcid);
    if (chan == NULL) {
        return;
    }

    psm = L2capGetPsm(chan->lpsm);

    chan->rcid = rcid;

    if (result == L2CAP_CONNECTION_SUCCESSFUL) {
        chan->state = L2CAP_CHANNEL_CONFIGING;
    }

    if (result == L2CAP_CONNECTION_PENDING) {
        signal->code = L2CAP_CONNECTION_REQUEST;
        L2capCreatePendingRequest(
            conn->pendingList, chan->lcid, signal, L2CAP_DEFAULT_ERTX, L2capResponseTimeoutCallback);
    }

    if (result != L2CAP_CONNECTION_SUCCESSFUL && result != L2CAP_CONNECTION_PENDING) {
        L2capDeleteChannel(conn, chan, 1);
    }

    if (psm != NULL) {
        L2capConnectionInfo connInfo = {0};

        LOG_DEBUG("L2capCallback recvConnectionRsp:%{public}d begin, aclHandle = %hu, cid = 0x%04X, result = %{public}d, status = %hu",
            __LINE__,
            aclHandle,
            lcid,
            result,
            status);

        connInfo.handle = aclHandle;
        (void)memcpy_s(&(connInfo.addr), sizeof(BtAddr), &(conn->addr), sizeof(BtAddr));
        psm->service.recvConnectionRsp(lcid, &connInfo, result, status, psm->ctx);
        LOG_DEBUG("L2capCallback recvConnectionRsp:%{public}d end", __LINE__);
    }

    return;
}

static void L2capProcessDisconnectionReq(uint16_t aclHandle, L2capSignalHeader *signal, uint8_t *data)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    uint16_t lcid;
    uint16_t rcid;
    L2capPsm *psm = NULL;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    BTM_ExitSniffMode(&(conn->addr));

    lcid = L2capLe16ToCpu(data + 0);
    rcid = L2capLe16ToCpu(data + L2CAP_OFFSET_2);

    chan = L2capGetChannel(conn, lcid);
    if (chan == NULL) {
        uint16_t rejCid[2] = {lcid, rcid};

        L2capSendCommandReject(
            aclHandle, L2CAP_SIGNALING_CHANNEL, signal->identifier, L2CAP_INVALID_CID_IN_REQUEST, rejCid);
        return;
    }

    // this case is for both side call disconnect at same time
    if (chan->state == L2CAP_CHANNEL_DISCONNECT_OUT_REQ) {
        L2capSendDisconnectionRsp(conn, chan, signal->identifier);
        return;
    }

    chan->state = L2CAP_CHANNEL_DISCONNECT_IN_REQ;

    psm = L2capGetPsm(chan->lpsm);
    if (psm != NULL) {
        LOG_DEBUG(
            "L2capCallback recvDisconnectionReq:%{public}d begin, cid = 0x%04X, id = %hhu", __LINE__, lcid, signal->identifier);
        psm->service.recvDisconnectionReq(lcid, signal->identifier, psm->ctx);
        LOG_DEBUG("L2capCallback recvDisconnectionReq:%{public}d end", __LINE__);
    }

    return;
}

NO_SANITIZE("cfi")
static void L2capProcessDisconnectionRsp(uint16_t aclHandle, const L2capSignalHeader *signal, uint8_t *data)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    L2capPsm *psm = NULL;
    uint16_t lcid;
    uint16_t rcid;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);

    rcid = L2capLe16ToCpu(data + 0);
    lcid = L2capLe16ToCpu(data + L2CAP_OFFSET_2);

    chan = L2capGetChannel(conn, lcid);
    if (chan == NULL) {
        return;
    }

    if (chan->rcid != rcid) {
        return;
    }

    psm = L2capGetPsm(chan->lpsm);

    L2capDeleteChannel(conn, chan, 1);

    if (psm != NULL) {
        LOG_DEBUG("L2capCallback recvDisconnectionRsp:%{public}d begin, cid = 0x%04X", __LINE__, lcid);
        psm->service.recvDisconnectionRsp(lcid, psm->ctx);
        LOG_DEBUG("L2capCallback recvDisconnectionRsp:%{public}d end", __LINE__);
    }

    return;
}

static void L2capProcessInformationReq(uint16_t aclHandle, const L2capSignalHeader *signal, uint8_t *data)
{
    L2capConnection *conn = NULL;
    uint16_t infoType;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    infoType = L2capLe16ToCpu(data + 0);

    L2capSendInformationRsp(conn, signal->identifier, infoType);
    return;
}

static void L2capIncomingConnection(const L2capConnection *conn, const L2capChannel *chan)
{
    L2capPsm *psm = NULL;
    L2capConnectionInfo connInfo = {0};

    psm = L2capGetPsm(chan->lpsm);
    if (psm != NULL) {
        LOG_DEBUG("L2capCallback recvConnectionReq:%{public}d begin, aclHandle = %hu, cid = 0x%04X, id = %hhu, lpsm = %hu",
            __LINE__,
            conn->aclHandle,
            chan->lcid,
            chan->connIdentifier,
            chan->lpsm);

        connInfo.handle = conn->aclHandle;
        (void)memcpy_s(&(connInfo.addr), sizeof(BtAddr), &(conn->addr), sizeof(BtAddr));
        psm->service.recvConnectionReq(chan->lcid, chan->connIdentifier, &connInfo, chan->lpsm, psm->ctx);
        LOG_DEBUG("L2capCallback recvConnectionReq:%{public}d end", __LINE__);
    }

    return;
}

static void L2capProcessInformationRsp(uint16_t aclHandle, const L2capSignalHeader *signal, uint8_t *data)
{
    L2capConnection *conn = NULL;
    uint16_t infoType;
    uint16_t result;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);

    infoType = L2capLe16ToCpu(data + 0);
    result = L2capLe16ToCpu(data + L2CAP_OFFSET_2);
    if (infoType == L2CAP_INFORMATION_TYPE_EXTENDED_FEATURE) {
        if (result == 0) {
            (void)memcpy_s(conn->info.extendedFeature, L2CAP_SIZE_4, data + L2CAP_OFFSET_4, L2CAP_SIZE_4);
        }

        if (conn->info.extendedFeature[0] & L2CAP_FEATURE_FIXED_CHANNELS) {
            L2capSendInformationReq(conn, L2CAP_INFORMATION_TYPE_FIXED_CHANNEL);
        } else {
            conn->info.state = L2CAP_INFO_STATE_DONE;
        }
    } else {  // L2CAP_INFORMATION_TYPE_FIXED_CHANNEL
        if (result == 0) {
            (void)memcpy_s(conn->info.fixedChannel, L2CAP_SIZE_8, data + L2CAP_OFFSET_4, L2CAP_SIZE_8);
        }

        conn->info.state = L2CAP_INFO_STATE_DONE;
    }

    if (conn->info.state == L2CAP_INFO_STATE_DONE) {
        L2capChannel *chan = NULL;
        ListNode *node = NULL;

        node = ListGetFirstNode(conn->chanList);
        while (node != NULL) {
            chan = ListGetNodeData(node);
            if (chan->state == L2CAP_CHANNEL_IDLE) {
                L2capSendConnectionReq(conn, chan);
            } else {
                L2capIncomingConnection(conn, chan);
            }

            node = ListGetNextNode(node);
        }
    }

    return;
}

static void L2capProcessEchoReq(uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    if (inst->echo.cb.recvEchoReq != NULL) {
        inst->echo.cb.recvEchoReq(aclHandle, signal->identifier, data, signal->length, inst->echo.ctx);
    } else {
        L2capSendEchoRsp(conn, signal->identifier, NULL, 0);
    }

    return;
}

static void L2capProcessEchoRsp(uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);

    if (inst->echo.cb.recvEchoRsp != NULL) {
        inst->echo.cb.recvEchoRsp(aclHandle, data, signal->length, inst->echo.ctx);
    }

    return;
}

static void L2capProcessCommandReject(uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    uint16_t reason;

    conn = L2capGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    reason = L2capLe16ToCpu(data + 0);
    if (reason == L2CAP_INVALID_CID_IN_REQUEST) {
        L2capPsm *psm = NULL;
        uint16_t lcid;

        lcid = L2capLe16ToCpu(data + L2CAP_OFFSET_4);
        chan = L2capGetChannel(conn, lcid);
        if (chan != NULL) {
            psm = L2capGetPsm(chan->lpsm);
            if (psm != NULL) {
                LOG_DEBUG("L2capCallback disconnectAbnormal:%{public}d begin, cid = 0x%04X, reason = 0", __LINE__, lcid);
                psm->service.disconnectAbnormal(lcid, 0, psm->ctx);
                LOG_DEBUG("L2capCallback disconnectAbnormal:%{public}d end", __LINE__);
            }

            L2capDeleteChannel(conn, chan, 1);
        }
    } else if ((reason == L2CAP_COMMAND_NOT_UNDERSTOOD) || (reason == L2CAP_SIGNAL_MTU_EXCEEDED)) {
        L2capPendingRequest *req = NULL;

        req = L2capGetPendingRequest(conn->pendingList, signal->identifier);
        if (req != NULL) {
            chan = L2capGetChannel(conn, req->lcid);
            if (chan != NULL) {
                L2capSendDisconnectionReq(conn, chan);
            }
        }
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);
    return;
}

static void L2capSignal(uint16_t aclHandle, L2capSignalHeader *signal, uint8_t *data)
{
    switch (signal->code) {
        case L2CAP_COMMAND_REJECT:
            L2capProcessCommandReject(aclHandle, signal, data);
            break;
        case L2CAP_CONNECTION_REQUEST:
            L2capProcessConnectionReq(aclHandle, signal, data);
            break;
        case L2CAP_CONNECTION_RESPONSE:
            L2capProcessConnectionRsp(aclHandle, signal, data);
            break;
        case L2CAP_CONFIGURATION_REQUEST:
            L2capProcessConfigurationReq(aclHandle, signal, data);
            break;
        case L2CAP_CONFIGURATION_RESPONSE:
            L2capProcessConfigurationRsp(aclHandle, signal, data);
            break;
        case L2CAP_DISCONNECTION_REQUEST:
            L2capProcessDisconnectionReq(aclHandle, signal, data);
            break;
        case L2CAP_DISCONNECTION_RESPONSE:
            L2capProcessDisconnectionRsp(aclHandle, signal, data);
            break;
        case L2CAP_ECHO_REQUEST:
            L2capProcessEchoReq(aclHandle, signal, data);
            break;
        case L2CAP_ECHO_RESPONSE:
            L2capProcessEchoRsp(aclHandle, signal, data);
            break;
        case L2CAP_INFORMATION_REQUEST:
            L2capProcessInformationReq(aclHandle, signal, data);
            break;
        case L2CAP_INFORMATION_RESPONSE:
            L2capProcessInformationRsp(aclHandle, signal, data);
            break;
        default:
            L2capSendCommandReject(
                aclHandle, L2CAP_SIGNALING_CHANNEL, signal->identifier, L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);
            break;
    }

    return;
}

static void L2capProcessSignal(uint16_t aclHandle, const Packet *pkt)
{
    uint8_t buff[L2CAP_SIGNAL_MTU] = {0};
    uint16_t length;
    uint16_t offset = 0;
    L2capSignalHeader signal = {0};

    length = PacketSize(pkt);
    if (length > L2CAP_SIGNAL_MTU) {
        PacketRead(pkt, buff, 0, L2CAP_SIGNAL_HEADER_LENGTH);
        L2capSendCommandReject(aclHandle, L2CAP_SIGNALING_CHANNEL, buff[1], L2CAP_SIGNAL_MTU_EXCEEDED, NULL);
        return;
    }

    if (length < L2CAP_SIGNAL_HEADER_LENGTH) {
        PacketRead(pkt, buff, 0, L2CAP_SIGNAL_HEADER_LENGTH);
        L2capSendCommandReject(aclHandle, L2CAP_SIGNALING_CHANNEL, buff[1], L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);
        return;
    }

    PacketRead(pkt, buff, 0, length);
    while (length >= L2CAP_SIGNAL_HEADER_LENGTH) {
        signal.code = buff[offset + 0];
        signal.identifier = buff[offset + L2CAP_OFFSET_1];
        signal.length = L2capLe16ToCpu(buff + offset + L2CAP_OFFSET_2);

        uint16_t signalLength = signal.length + L2CAP_SIGNAL_HEADER_LENGTH;
        if (length < signalLength || signal.length > L2CAP_SIGNAL_MTU) {
            L2capSendCommandReject(
                aclHandle, L2CAP_SIGNALING_CHANNEL, signal.identifier, L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);
            break;
        }

        L2capSignal(aclHandle, &signal, buff + offset + L2CAP_SIGNAL_HEADER_LENGTH);

        offset += signalLength;
        length -= signalLength;
    }

    return;
}

static Packet *L2capProcessStreamData(L2capChannel *chan, Packet *pkt)
{
    uint8_t header[6] = {0};
    Packet *outPkt = NULL;
    L2capErfcIControl *iCtrl = NULL;

    if (chan->lcfg.fcs == 0x01) {
        if (L2capCheckCrc(pkt) != BT_NO_ERROR) {
            return NULL;
        }
    }

    PacketExtractHead(pkt, header, sizeof(header));

    iCtrl = (L2capErfcIControl *)(header + L2CAP_HEADER_LENGTH);
    if (iCtrl->type == L2CAP_IFRAME) {
        outPkt = L2capStreamProcessIFrame(chan, pkt, iCtrl);
    }

    return outPkt;
}

static Packet *L2capProcessErfcData(L2capConnection *conn, L2capChannel *chan, Packet *pkt)
{
    uint8_t header[6] = {0};
    Packet *outPkt = NULL;

    if (chan->lcfg.fcs == 0x01) {
        if (L2capCheckCrc(pkt) != BT_NO_ERROR) {
            return NULL;
        }
    }

    PacketExtractHead(pkt, header, sizeof(header));
    if (header[L2CAP_HEADER_LENGTH] & L2CAP_SFRAME) {
        L2capErfcProcessSFrame(conn, chan, header + L2CAP_HEADER_LENGTH);
    } else {  // I Frame
        outPkt = L2capErfcProcessIFrame(conn, chan, pkt, header + L2CAP_HEADER_LENGTH);
    }

    return outPkt;
}

static int L2capProcessBasicData(const L2capChannel *chan, Packet *pkt)
{
    uint16_t length;
    uint8_t header[L2CAP_HEADER_LENGTH] = {0};

    PacketExtractHead(pkt, header, sizeof(header));
    length = PacketSize(pkt);
    if (length > chan->lcfg.mtu) {
        return BT_BAD_PARAM;
    }

    return BT_NO_ERROR;
}

NO_SANITIZE("cfi") static void L2capProcessData(uint16_t aclHandle, uint16_t cid, Packet *pkt)
{
    L2capConnection *conn = NULL;
    L2capChannel *chan = NULL;
    L2capPsm *psm = NULL;
    Packet *outPkt = NULL;

    L2capGetChannel3(aclHandle, cid, &conn, &chan);
    if (chan == NULL) {
        return;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECTED) {
        return;
    }

    psm = L2capGetPsm(chan->lpsm);
    if (psm == NULL) {
        return;
    }

    switch (chan->lcfg.rfc.mode) {
        case L2CAP_BASIC_MODE:
            if (L2capProcessBasicData(chan, pkt) == BT_NO_ERROR) {
                LOG_DEBUG("L2capCallback recvData: begin, cid = 0x%04X, pktLen = %u", cid, PacketSize(pkt));
                psm->service.recvData(cid, pkt, psm->ctx);
                LOG_DEBUG("L2capCallback recvData:%{public}d end", __LINE__);
            }
            break;
        case L2CAP_ENHANCED_RETRANSMISSION_MODE:
            outPkt = L2capProcessErfcData(conn, chan, pkt);
            if (outPkt != NULL) {
                LOG_DEBUG("L2capCallback recvData: begin, cid = 0x%04X, pktLen = %u", cid, PacketSize(outPkt));
                psm->service.recvData(cid, outPkt, psm->ctx);
                LOG_DEBUG("L2capCallback recvData:%{public}d end", __LINE__);

                PacketFree(outPkt);
            }
            break;
        case L2CAP_STREAM_MODE:
            outPkt = L2capProcessStreamData(chan, pkt);
            if (outPkt != NULL) {
                LOG_DEBUG("L2capCallback recvData: begin, cid = 0x%04X, pktLen = %{public}d", cid, PacketSize(outPkt));
                psm->service.recvData(cid, outPkt, psm->ctx);
                LOG_DEBUG("L2capCallback recvData:%{public}d end", __LINE__);

                PacketFree(outPkt);
            }
            break;
        default:
            break;
    }

    return;
}

int L2capReceivePacket(uint16_t handle, uint16_t cid, Packet *pkt)
{
    uint8_t header[L2CAP_HEADER_LENGTH] = {0};

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    switch (cid) {
        case L2CAP_SIGNALING_CHANNEL:
            PacketExtractHead(pkt, header, sizeof(header));
            L2capProcessSignal(handle, pkt);
            break;
        default:
            L2capProcessData(handle, cid, pkt);
            break;
    }

    return BT_NO_ERROR;
}

static void L2capAclConnectFailed(List *chanList, uint8_t status)
{
    L2capChannel *chan = NULL;
    ListNode *node = NULL;

    if (chanList == NULL) {
        return;
    }

    while (1) {
        node = ListGetFirstNode(chanList);
        if (node == NULL) {
            break;
        }

        chan = ListGetNodeData(node);
        L2capDisconnectAbnormal(chan, status);

        ListRemoveNode(chanList, chan);
        L2capDestroyChannel(chan);
    }

    ListDelete(chanList);
    return;
}

int L2capConnectComplete(const BtAddr *addr, uint16_t handle, uint8_t status)
{
    L2capConnection *conn = NULL;

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (status != 0) {
        List *chanList = NULL;

        conn = L2capGetConnection2(addr);
        if (conn != NULL) {
            chanList = conn->chanList;
            conn->chanList = NULL;

            L2capDeleteConnection(conn);
        }

        L2capAclConnectFailed(chanList, status);
        return BT_BAD_STATUS;
    }

    conn = L2capGetConnection2(addr);
    if (conn == NULL) {
        conn = L2capNewConnection(addr, handle);
    }

    conn->aclHandle = handle;
    conn->state = L2CAP_CONNECTION_CONNECTED;

    if (ListGetFirstNode(conn->chanList) != NULL) {
        L2capAddConnectionRef(handle);
        L2capSendInformationReq(conn, L2CAP_INFORMATION_TYPE_EXTENDED_FEATURE);
    }

    return BT_NO_ERROR;
}

static void L2capAclDisconnected(L2capConnection *conn, uint8_t status, uint8_t reason)
{
    L2capChannel *chan = NULL;
    ListNode *node = NULL;
    List *chanList = conn->chanList;
    uint8_t infoState = conn->info.state;

    conn->chanList = NULL;
    L2capDeleteConnection(conn);

    while (1) {
        node = ListGetFirstNode(chanList);
        if (node == NULL) {
            break;
        }

        chan = ListGetNodeData(node);
        ListRemoveNode(chanList, chan);

        if (infoState != L2CAP_INFO_STATE_DONE) {
            L2capDisconnectAbnormal(chan, reason);
            L2capDestroyChannel(chan);
            continue;
        }

        if (chan->state == L2CAP_CHANNEL_IDLE) {
            L2capDisconnectAbnormal(chan, L2CAP_STATE_COLLISION);
        } else {
            L2capDisconnectAbnormal(chan, reason);
        }

        L2capDestroyChannel(chan);
    }

    ListDelete(chanList);
    return;
}

int L2capDisconnectComplete(uint16_t handle, uint8_t status, uint8_t reason)
{
    L2capConnection *conn = NULL;

    if (L2capInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    conn = L2capGetConnection(handle);
    if (conn == NULL) {
        return BT_BAD_PARAM;
    }

    L2capAclDisconnected(conn, status, reason);
    return BT_NO_ERROR;
}
