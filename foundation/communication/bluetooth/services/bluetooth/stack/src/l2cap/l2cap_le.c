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

#include "l2cap_le.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "l2cap_cmn.h"
#include "list.h"
#include "log.h"

#define L2CAP_LE_DEFAULT_CREDIT 0x08

#define L2CAP_LE_CHANNEL_CREDIT_NOT_FULL 0x00
#define L2CAP_LE_CHANNEL_CREDIT_FULL 0x01

typedef struct {
    uint16_t lpsm;
    L2capLeService service;

    void *ctx;
} L2capLePsm;

typedef struct {
    uint16_t lcid;
    uint16_t rcid;

    uint16_t lpsm;
    uint16_t rpsm;

    uint8_t connIdentifier;
    uint8_t state;

    uint8_t busyState;

    L2capLeConfigInfo lcfg;
    L2capLeConfigInfo rcfg;

    List *txList;
    Packet *rxSarPacket;
} L2capLeChannel;

typedef struct {
    uint16_t aclHandle;
    BtAddr addr;

    uint8_t nextIdentifier;
    uint8_t role;

    List *chanList;  // Pack struct L2capLeChannel

    List *pendingList;  // Pack struct L2capPendingRequest
} L2capLeConnection;

typedef struct {
    L2capLeConnectionParameterUpdate cb;
    void *ctx;
} L2capLeConnectionParameterUpdateContext;

typedef struct {
    uint16_t nextLcid;

    L2capLeFixChannel chanAtt;
    L2capLeFixChannel chanSm;

    L2capLeConnectionParameterUpdateContext connParamUpdate;

    List *psmList;   // Pack struct L2capLePsm
    List *connList;  // Pack struct L2capLeConnection
} L2capLeInstance;

L2capLeInstance g_l2capLeInst;

static int L2capLeInitialized()
{
    L2capLeInstance *inst = &g_l2capLeInst;

    if (inst->connList != NULL) {
        return BT_NO_ERROR;
    }

    return BT_BAD_STATUS;
}

static L2capLePsm *L2capLeGetPsm(uint16_t lpsm)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLePsm *lepsm = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->psmList);
    while (node != NULL) {
        lepsm = ListGetNodeData(node);
        if (lepsm->lpsm == lpsm) {
            return lepsm;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

static L2capLeConnection *L2capLeGetConnection(uint16_t aclHandle)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *leconn = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        leconn = ListGetNodeData(node);
        if (leconn->aclHandle == aclHandle) {
            return leconn;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

static L2capLeConnection *L2capLeGetConnection2(const BtAddr *addr)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *conn = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        if (memcmp(&(conn->addr), addr, sizeof(BtAddr)) == 0) {
            return conn;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

static L2capLeChannel *L2capLeGetChannel(L2capLeConnection *conn, int16_t lcid)
{
    L2capLeChannel *lechan = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(conn->chanList);
    while (node != NULL) {
        lechan = ListGetNodeData(node);
        if (lechan->lcid == lcid) {
            return lechan;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

static void L2capLeGetChannel2(uint16_t lcid, L2capLeConnection **conn, L2capLeChannel **chan)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        *conn = ListGetNodeData(node);
        *chan = L2capLeGetChannel(*conn, lcid);
        if ((*chan) != NULL) {
            break;
        }

        node = ListGetNextNode(node);
    }

    return;
}

static void L2capLeGetChannel3(uint16_t aclHandle, uint16_t lcid, L2capLeConnection **conn, L2capLeChannel **chan)
{
    *conn = L2capLeGetConnection(aclHandle);
    if ((*conn) == NULL) {
        return;
    }

    *chan = L2capLeGetChannel(*conn, lcid);
    return;
}

static L2capLeChannel *L2capLeGetChannel4(L2capLeConnection *conn, int16_t ident)
{
    L2capLeChannel *chan = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(conn->chanList);
    while (node != NULL) {
        chan = ListGetNodeData(node);
        if (chan->connIdentifier == ident) {
            return chan;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

static uint16_t L2capLeGetNewLcid()
{
    L2capLeInstance *inst = &g_l2capLeInst;
    uint16_t lcid = L2CAP_LE_MIN_CID;

    if (inst->nextLcid == 0) {
        L2capLeConnection *conn = NULL;
        L2capLeChannel *chan = NULL;

        while (1) {
            L2capLeGetChannel2(lcid, &conn, &chan);
            if (chan == NULL) {
                break;
            }

            lcid += 1;
        }
    } else {
        lcid = inst->nextLcid;

        if (lcid == L2CAP_LE_MAX_CID) {
            inst->nextLcid = 0;
        } else {
            inst->nextLcid += 1;
        }
    }

    return lcid;
}

static L2capLeChannel *L2capLeNewChannel(L2capLeConnection *conn, uint16_t lpsm, uint16_t rpsm)
{
    L2capLeChannel *chan = NULL;

    chan = L2capAlloc(sizeof(L2capLeChannel));
    if (chan == NULL) {
        return NULL;
    }

    chan->lcid = L2capLeGetNewLcid();
    chan->lpsm = lpsm;
    chan->rpsm = rpsm;
    chan->lcfg.mps = L2capGetRxBufferSize() - L2CAP_SIZE_6;
    chan->lcfg.credit = L2CAP_LE_DEFAULT_CREDIT;
    chan->state = L2CAP_CHANNEL_IDLE;
    chan->rxSarPacket = NULL;

    ListAddLast(conn->chanList, chan);
    return chan;
}

static void L2capLeDestroyChannel(L2capLeChannel *chan)
{
    if (chan->txList != NULL) {
        ListNode *node = NULL;
        Packet *pkt = NULL;

        while (1) {
            node = ListGetFirstNode(chan->txList);
            if (node == NULL) {
                break;
            }

            pkt = ListGetNodeData(node);
            ListRemoveNode(chan->txList, pkt);
            PacketFree(pkt);
        }

        ListDelete(chan->txList);
    }

    if (chan->rxSarPacket != NULL) {
        PacketFree(chan->rxSarPacket);
        chan->rxSarPacket = NULL;
    }

    L2capFree(chan);
    return;
}

static void L2capLeDeleteChannel(L2capLeConnection *conn, L2capLeChannel *chan, uint16_t removeAcl)
{
    ListRemoveNode(conn->chanList, chan);
    L2capLeDestroyChannel(chan);

    if (removeAcl) {
        if (ListGetFirstNode(conn->chanList) == NULL) {
            // Reason: REMOTE USER TERMINATED CONNECTION
            L2capDisconnect(conn->aclHandle, 0x13);
        }
    }

    return;
}

static L2capLeConnection *L2capLeNewConnection(const BtAddr *addr, uint16_t aclHandle, uint8_t role)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *conn = NULL;

    conn = L2capAlloc(sizeof(L2capLeConnection));
    if (conn == NULL) {
        return NULL;
    }

    (void)memcpy_s(&(conn->addr), sizeof(BtAddr), addr, sizeof(BtAddr));
    conn->aclHandle = aclHandle;
    conn->nextIdentifier = L2CAP_MIN_IDENTIFIER;
    conn->role = role;

    conn->chanList = ListCreate(NULL);
    conn->pendingList = ListCreate(NULL);
    ListAddFirst(inst->connList, conn);
    return conn;
}

static void L2capLeDeleteConnection(L2capLeConnection *conn)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    ListNode *node = NULL;

    if (conn->chanList != NULL) {
        L2capLeChannel *chan = NULL;

        while (1) {
            node = ListGetFirstNode(conn->chanList);
            if (node == NULL) {
                break;
            }

            chan = ListGetNodeData(node);
            ListRemoveNode(conn->chanList, chan);
            L2capLeDestroyChannel(chan);
        }

        ListDelete(conn->chanList);
    }

    if (conn->pendingList != NULL) {
        L2capClearPendingRequest(conn->pendingList);
        ListDelete(conn->pendingList);
    }

    ListRemoveNode(inst->connList, conn);
    L2capFree(conn);

    // if no connection exists, reset nextLcid value
    if (ListGetFirstNode(inst->connList) == NULL) {
        inst->nextLcid = L2CAP_LE_MIN_CID;
    }

    return;
}

static uint8_t L2capLeGetNewIdentifier(L2capLeConnection *leconn)
{
    uint8_t ident;

    ident = leconn->nextIdentifier;
    if (ident == L2CAP_MAX_IDENTIFIER) {
        leconn->nextIdentifier = L2CAP_MIN_IDENTIFIER;
    } else {
        leconn->nextIdentifier += 1;
    }

    return ident;
}

static void L2capLeResponseTimeout(const void *parameter)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *conn = NULL;
    L2capPendingRequest *lereq = NULL;
    ListNode *node = NULL;

    if (inst->connList == NULL) {
        return;
    }

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        lereq = L2capGetPendingRequest2(conn->pendingList, parameter);
        if (lereq != NULL) {
            ListRemoveNode(conn->pendingList, lereq);
            break;
        }

        node = ListGetNextNode(node);
    }

    if (lereq == NULL) {
        return;
    }

    if (lereq->lcid > 0) {
        L2capLeChannel *chan = NULL;
        L2capLePsm *psm = NULL;

        chan = L2capLeGetChannel(conn, lereq->lcid);
        if (chan != NULL) {
            psm = L2capLeGetPsm(chan->lpsm);
            if (psm != NULL) {
                LOG_DEBUG(
                    "L2capCallback leDisconnectAbnormal:%{public}d begin, cid = 0x%04X, reason = 0", __LINE__, lereq->lcid);
                psm->service.leDisconnectAbnormal(lereq->lcid, 0, psm->ctx);
                LOG_DEBUG("L2capCallback leDisconnectAbnormal:%{public}d end", __LINE__);
            }

            L2capLeDeleteChannel(conn, chan, 1);
        }
    }

    AlarmDelete(lereq->timer);
    L2capFree(lereq);

    return;
}

static void L2capLeResponseTimeoutCallback(void *parameter)
{
    L2capAsynchronousProcess(L2capLeResponseTimeout, NULL, parameter);
    return;
}

static int L2capSendCreditBasedConnectionReq(L2capLeConnection *conn, L2capLeChannel *chan)
{
    Packet *pkt = NULL;
    uint8_t buff[10] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->rpsm);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->lcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, chan->lcfg.mtu);
    L2capCpuToLe16(buff + L2CAP_OFFSET_6, chan->lcfg.mps);
    L2capCpuToLe16(buff + L2CAP_OFFSET_8, chan->lcfg.credit);

    signal.code = L2CAP_LE_CREDIT_BASED_CONNECTION_REQUEST;
    signal.identifier = L2capLeGetNewIdentifier(conn);
    signal.length = sizeof(buff);

    chan->connIdentifier = signal.identifier;
    pkt = L2capBuildSignalPacket(L2CAP_LE_SIGNALING_CHANNEL, &signal, buff);
    L2capCreatePendingRequest(
        conn->pendingList, chan->lcid, &signal, L2CAP_DEFAULT_RTX, L2capLeResponseTimeoutCallback);
    return L2capLeSendPacket(conn->aclHandle, pkt);
}

static int L2capSendCreditBasedConnectionRsp(
    L2capLeConnection *conn, L2capLeChannel *chan, uint8_t ident, uint16_t result)
{
    Packet *pkt = NULL;
    uint8_t buff[10] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->lcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->lcfg.mtu);
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, chan->lcfg.mps);
    L2capCpuToLe16(buff + L2CAP_OFFSET_6, chan->lcfg.credit);
    L2capCpuToLe16(buff + L2CAP_OFFSET_8, result);

    signal.code = L2CAP_LE_CREDIT_BASED_CONNECTION_RESPONSE;
    signal.identifier = ident;
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_LE_SIGNALING_CHANNEL, &signal, buff);
    return L2capLeSendPacket(conn->aclHandle, pkt);
}

static int L2capLeSendDisconnectionReq(L2capLeConnection *conn, const L2capLeChannel *chan)
{
    Packet *pkt = NULL;
    uint8_t buff[4] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->rcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->lcid);

    signal.code = L2CAP_DISCONNECTION_REQUEST;
    signal.identifier = L2capLeGetNewIdentifier(conn);
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_LE_SIGNALING_CHANNEL, &signal, buff);
    L2capCreatePendingRequest(
        conn->pendingList, chan->lcid, &signal, L2CAP_DEFAULT_RTX, L2capLeResponseTimeoutCallback);
    return L2capLeSendPacket(conn->aclHandle, pkt);
}

static int L2capLeSendDisconnectionRsp(const L2capLeConnection *conn, const L2capLeChannel *chan, uint8_t ident)
{
    Packet *pkt = NULL;
    uint8_t buff[4] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->lcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, chan->rcid);

    signal.code = L2CAP_DISCONNECTION_RESPONSE;
    signal.identifier = ident;
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_LE_SIGNALING_CHANNEL, &signal, buff);
    return L2capLeSendPacket(conn->aclHandle, pkt);
}

static int L2capLeSendFlowControlCredit(L2capLeConnection *conn, const L2capLeChannel *chan, uint16_t credit)
{
    Packet *pkt = NULL;
    uint8_t buff[4] = {0};
    L2capSignalHeader signal = {0};

    L2capCpuToLe16(buff + 0, chan->lcid);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, credit);

    signal.code = L2CAP_LE_FLOW_CONTROL_CREDIT;
    signal.identifier = L2capLeGetNewIdentifier(conn);
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_LE_SIGNALING_CHANNEL, &signal, buff);
    return L2capLeSendPacket(conn->aclHandle, pkt);
}

static void L2capLeTxWithCredit(const L2capLeConnection *conn, L2capLeChannel *chan)
{
    ListNode *node = NULL;
    Packet *pkt = NULL;

    while (1) {
        if (chan->rcfg.credit == 0) {
            break;
        }

        node = ListGetFirstNode(chan->txList);
        if (node == NULL) {
            break;
        }

        pkt = ListGetNodeData(node);

        ListRemoveNode(chan->txList, pkt);
        L2capLeSendPacket(conn->aclHandle, pkt);

        chan->rcfg.credit -= 1;
    }

    if (chan->rcfg.credit == 0) {
        if (chan->busyState == L2CAP_LE_CHANNEL_CREDIT_NOT_FULL) {
            L2capLePsm *psm = NULL;

            chan->busyState = L2CAP_LE_CHANNEL_CREDIT_FULL;
            psm = L2capLeGetPsm(chan->lpsm);
            if (psm != NULL) {
                LOG_DEBUG("L2capCallback leRemoteBusy:%{public}d begin, cid = 0x%04X, busyState = %hhu",
                    __LINE__,
                    chan->lcid,
                    chan->busyState);
                psm->service.leRemoteBusy(chan->lcid, chan->busyState, psm->ctx);
                LOG_DEBUG("L2capCallback leRemoteBusy:%{public}d end", __LINE__);
            }
        }
    } else {
        if (chan->busyState == L2CAP_LE_CHANNEL_CREDIT_FULL) {
            L2capLePsm *psm = NULL;

            chan->busyState = L2CAP_LE_CHANNEL_CREDIT_NOT_FULL;
            psm = L2capLeGetPsm(chan->lpsm);
            if (psm != NULL) {
                LOG_DEBUG("L2capCallback leRemoteBusy:%{public}d begin, cid = 0x%04X, busyState = %hhu",
                    __LINE__,
                    chan->lcid,
                    chan->busyState);
                psm->service.leRemoteBusy(chan->lcid, chan->busyState, psm->ctx);
                LOG_DEBUG("L2capCallback leRemoteBusy:%{public}d end", __LINE__);
            }
        }
    }

    return;
}

static void L2capLeSegmentPacketWithCredit(const L2capLeChannel *chan, Packet *pkt)
{
    Packet *frag = NULL;
    Packet *fragPkt = NULL;
    uint16_t totalLength;
    uint8_t *header = NULL;
    uint16_t headerLength;

    totalLength = PacketSize(pkt);
    headerLength = L2CAP_HEADER_LENGTH + L2CAP_SIZE_2;
    while (1) {
        frag = PacketMalloc(0, 0, 0);
        int remainLength = PacketFragment(pkt, frag, chan->rcfg.mps);

        fragPkt = PacketInheritMalloc(frag, headerLength, 0);
        PacketFree(frag);

        header = BufferPtr(PacketHead(fragPkt));
        L2capCpuToLe16(header + 0, PacketSize(fragPkt) - L2CAP_HEADER_LENGTH);
        L2capCpuToLe16(header + L2CAP_OFFSET_2, chan->rcid);

        if (headerLength > L2CAP_HEADER_LENGTH) {
            L2capCpuToLe16(header + L2CAP_OFFSET_4, totalLength);
        }

        ListAddLast(chan->txList, fragPkt);
        if (remainLength == 0) {
            break;
        }

        headerLength = L2CAP_HEADER_LENGTH;
    }

    return;
}

static void L2capLeProcessConnectionParameterUpdateReq(
    uint16_t aclHandle, L2capSignalHeader *signal, const uint8_t *data)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *conn = NULL;
    L2capLeConnectionParameter param = {0};

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    if (conn->role == L2CAP_LE_ROLE_SLAVE) {
        L2capSendCommandReject(
            aclHandle, L2CAP_LE_SIGNALING_CHANNEL, signal->identifier, L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);
        return;
    }

    param.connIntervalMin = L2capLe16ToCpu(data + 0);
    param.connIntervalMax = L2capLe16ToCpu(data + L2CAP_OFFSET_2);
    param.connLatency = L2capLe16ToCpu(data + L2CAP_OFFSET_4);
    param.supervisionTimeout = L2capLe16ToCpu(data + L2CAP_OFFSET_6);

    if (inst->connParamUpdate.cb.recvLeConnectionParameterUpdateReq != NULL) {
        LOG_DEBUG("L2capCallback recvLeConnectionParameterUpdateReq:%{public}d begin", __LINE__);
        inst->connParamUpdate.cb.recvLeConnectionParameterUpdateReq(
            aclHandle, signal->identifier, &param, inst->connParamUpdate.ctx);
        LOG_DEBUG("L2capCallback recvLeConnectionParameterUpdateReq:%{public}d end", __LINE__);
    }

    return;
}

static void L2capLeProcessConnectionParameterUpdateRsp(
    uint16_t aclHandle, L2capSignalHeader *signal, const uint8_t *data)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *conn = NULL;
    uint16_t result;

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);
    result = L2capLe16ToCpu(data + 0);

    if (inst->connParamUpdate.cb.recvLeConnectionParameterUpdateRsp != NULL) {
        LOG_DEBUG("L2capCallback recvLeConnectionParameterUpdateRsp:%{public}d begin", __LINE__);
        inst->connParamUpdate.cb.recvLeConnectionParameterUpdateRsp(aclHandle, result, inst->connParamUpdate.ctx);
        LOG_DEBUG("L2capCallback recvLeConnectionParameterUpdateRsp:%{public}d end", __LINE__);
    }

    return;
}

static void L2capLeProcessCreditBasedConnectionReq(
    uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    L2capLePsm *psm = NULL;
    uint16_t lpsm;
    uint16_t rcid;
    L2capLeConfigInfo cfg = {0};
    L2capConnectionInfo connInfo = {0};

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    lpsm = L2capLe16ToCpu(data + 0);
    rcid = L2capLe16ToCpu(data + L2CAP_OFFSET_2);
    cfg.mtu = L2capLe16ToCpu(data + L2CAP_OFFSET_4);
    cfg.mps = L2capLe16ToCpu(data + L2CAP_OFFSET_6);
    cfg.credit = L2capLe16ToCpu(data + L2CAP_OFFSET_8);

    psm = L2capLeGetPsm(lpsm);
    if (psm == NULL) {
        L2capLeChannel tchan = {0};

        tchan.lcid = 0;
        (void)memcpy_s(&(tchan.lcfg), sizeof(L2capLeConfigInfo), &cfg, sizeof(L2capLeConfigInfo));
        L2capSendCreditBasedConnectionRsp(conn, &tchan, signal->identifier, L2CAP_LE_PSM_NOT_SUPPORTED);
        return;
    }

    chan = L2capLeNewChannel(conn, lpsm, lpsm);
    chan->rcid = rcid;
    chan->connIdentifier = signal->identifier;

    chan->rcfg.mtu = cfg.mtu;
    chan->rcfg.mps = cfg.mps;
    chan->rcfg.credit = cfg.credit;
    if (chan->rcfg.mps > (L2capLeGetTxBufferSize() - L2CAP_SIZE_6)) {
        chan->rcfg.mps = L2capLeGetTxBufferSize() - L2CAP_SIZE_6;
    }

    chan->state = L2CAP_CHANNEL_CONNECT_IN_REQ;

    LOG_DEBUG(
        "L2capCallback recvLeCreditBasedConnectionReq:%{public}d begin, aclHandle = %hu, cid = 0x%04X, id = %hhu, lpsm = %hu",
        __LINE__,
        aclHandle,
        chan->lcid,
        signal->identifier,
        psm->lpsm);

    connInfo.handle = aclHandle;
    (void)memcpy_s(&(connInfo.addr), sizeof(BtAddr), &(conn->addr), sizeof(BtAddr));
    psm->service.recvLeCreditBasedConnectionReq(chan->lcid, signal->identifier, &connInfo, &cfg, psm->ctx);
    LOG_DEBUG("L2capCallback recvLeCreditBasedConnectionReq:%{public}d end", __LINE__);

    return;
}

static void L2capLeProcessCreditBasedConnectionRsp(
    uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    L2capLePsm *psm = NULL;
    uint16_t lcid = 0;
    uint16_t rcid;
    uint16_t result;
    L2capLeConfigInfo cfg = {0};
    L2capConnectionInfo connInfo = {0};

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);

    rcid = L2capLe16ToCpu(data + 0);
    cfg.mtu = L2capLe16ToCpu(data + L2CAP_OFFSET_2);
    cfg.mps = L2capLe16ToCpu(data + L2CAP_OFFSET_4);
    cfg.credit = L2capLe16ToCpu(data + L2CAP_OFFSET_6);
    result = L2capLe16ToCpu(data + L2CAP_OFFSET_8);

    chan = L2capLeGetChannel4(conn, signal->identifier);
    if (chan == NULL) {
        return;
    }

    chan->rcid = rcid;
    lcid = chan->lcid;

    chan->rcfg.mtu = cfg.mtu;
    chan->rcfg.mps = cfg.mps;
    chan->rcfg.credit = cfg.credit;
    if (chan->rcfg.mps > (L2capLeGetTxBufferSize() - L2CAP_SIZE_6)) {
        chan->rcfg.mps = L2capLeGetTxBufferSize() - L2CAP_SIZE_6;
    }

    psm = L2capLeGetPsm(chan->lpsm);

    if (result == L2CAP_LE_CONNECTION_SUCCESSFUL) {
        chan->state = L2CAP_CHANNEL_CONNECTED;
    } else {
        L2capLeDeleteChannel(conn, chan, 1);
    }

    if (psm != NULL) {
        LOG_DEBUG("L2capCallback recvLeCreditBasedConnectionRsp:begin, cid = 0x%04X, result = %hu", lcid, result);

        connInfo.handle = aclHandle;
        (void)memcpy_s(&(connInfo.addr), sizeof(BtAddr), &(conn->addr), sizeof(BtAddr));
        psm->service.recvLeCreditBasedConnectionRsp(lcid, &connInfo, &cfg, result, psm->ctx);
        LOG_DEBUG("L2capCallback recvLeCreditBasedConnectionRsp:%{public}d end", __LINE__);
    }

    return;
}

static void L2capLeProcessDisconnectionReq(uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    uint16_t lcid;
    uint16_t rcid;
    L2capLePsm *psm = NULL;

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    lcid = L2capLe16ToCpu(data + 0);
    rcid = L2capLe16ToCpu(data + L2CAP_OFFSET_2);

    chan = L2capLeGetChannel(conn, lcid);
    if (chan == NULL) {
        uint16_t rejCid[L2CAP_SIZE_2] = {lcid, rcid};

        L2capSendCommandReject(
            aclHandle, L2CAP_LE_SIGNALING_CHANNEL, signal->identifier, L2CAP_INVALID_CID_IN_REQUEST, rejCid);
        return;
    }

    // this case is for both side call disconnect at same time
    if (chan->state == L2CAP_CHANNEL_DISCONNECT_OUT_REQ) {
        L2capLeSendDisconnectionRsp(conn, chan, signal->identifier);
        return;
    }

    chan->state = L2CAP_CHANNEL_DISCONNECT_IN_REQ;

    psm = L2capLeGetPsm(chan->lpsm);
    if (psm != NULL) {
        LOG_DEBUG("L2capCallback recvLeDisconnectionReq:%{public}d begin, cid = 0x%04X, id = %hhu",
            __LINE__,
            lcid,
            signal->identifier);
        psm->service.recvLeDisconnectionReq(lcid, signal->identifier, psm->ctx);
        LOG_DEBUG("L2capCallback recvLeDisconnectionReq:%{public}d end", __LINE__);
    }

    return;
}

static void L2capLeProcessDisconnectionRsp(uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    L2capLePsm *psm = NULL;
    uint16_t lcid;
    uint16_t rcid;

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    L2capDestroyPendingRequest(conn->pendingList, signal->identifier);

    rcid = L2capLe16ToCpu(data + 0);
    lcid = L2capLe16ToCpu(data + L2CAP_OFFSET_2);

    chan = L2capLeGetChannel(conn, lcid);
    if (chan == NULL) {
        return;
    }

    if (chan->rcid != rcid) {
        return;
    }

    psm = L2capLeGetPsm(chan->lpsm);

    L2capLeDeleteChannel(conn, chan, 1);

    if (psm != NULL) {
        LOG_DEBUG("L2capCallback recvLeDisconnectionRsp:%{public}d begin, cid = 0x%04X", __LINE__, lcid);
        psm->service.recvLeDisconnectionRsp(lcid, psm->ctx);
        LOG_DEBUG("L2capCallback recvLeDisconnectionRsp:%{public}d end", __LINE__);
    }

    return;
}

static void L2capLeProcessFlowControlCredit(uint16_t aclHandle, const L2capSignalHeader *signal, const uint8_t *data)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    uint16_t lcid;
    uint16_t credit;

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return;
    }

    lcid = L2capLe16ToCpu(data + 0);
    credit = L2capLe16ToCpu(data + L2CAP_OFFSET_2);

    chan = L2capLeGetChannel(conn, lcid);
    if (chan == NULL) {
        return;
    }

    if (chan->rcfg.credit > (UINT16_MAX - credit)) {
        L2capLeSendDisconnectionReq(conn, chan);
        return;
    }

    chan->rcfg.credit += credit;

    L2capLeTxWithCredit(conn, chan);
    return;
}

static void L2capLeSignal(uint16_t aclHandle, L2capSignalHeader *signal, const uint8_t *data)
{
    switch (signal->code) {
        case L2CAP_LE_CREDIT_BASED_CONNECTION_REQUEST:
            L2capLeProcessCreditBasedConnectionReq(aclHandle, signal, data);
            break;
        case L2CAP_LE_CREDIT_BASED_CONNECTION_RESPONSE:
            L2capLeProcessCreditBasedConnectionRsp(aclHandle, signal, data);
            break;
        case L2CAP_DISCONNECTION_REQUEST:
            L2capLeProcessDisconnectionReq(aclHandle, signal, data);
            break;
        case L2CAP_DISCONNECTION_RESPONSE:
            L2capLeProcessDisconnectionRsp(aclHandle, signal, data);
            break;
        case L2CAP_LE_FLOW_CONTROL_CREDIT:
            L2capLeProcessFlowControlCredit(aclHandle, signal, data);
            break;
        case L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST:
            L2capLeProcessConnectionParameterUpdateReq(aclHandle, signal, data);
            break;
        case L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE:
            L2capLeProcessConnectionParameterUpdateRsp(aclHandle, signal, data);
            break;
        case L2CAP_COMMAND_REJECT:
            break;
        default:
            L2capSendCommandReject(
                aclHandle, L2CAP_LE_SIGNALING_CHANNEL, signal->identifier, L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);
            break;
    }
    return;
}

static void L2capLeProcessSignal(uint16_t aclHandle, const Packet *pkt)
{
    uint8_t buff[L2CAP_SIGNAL_MTU] = {0};
    uint16_t length;
    L2capSignalHeader signal = {0};

    length = PacketSize(pkt);
    if (length > L2CAP_SIGNAL_MTU) {
        PacketRead(pkt, buff, 0, L2CAP_SIGNAL_HEADER_LENGTH);
        L2capSendCommandReject(aclHandle, L2CAP_LE_SIGNALING_CHANNEL, buff[1], L2CAP_SIGNAL_MTU_EXCEEDED, NULL);
        return;
    }

    if (length < L2CAP_SIGNAL_HEADER_LENGTH) {
        PacketRead(pkt, buff, 0, L2CAP_SIGNAL_HEADER_LENGTH);
        L2capSendCommandReject(aclHandle, L2CAP_LE_SIGNALING_CHANNEL, buff[1], L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);
        return;
    }

    PacketRead(pkt, buff, 0, length);
    signal.code = buff[0];
    signal.identifier = buff[1];
    signal.length = L2capLe16ToCpu(buff + L2CAP_OFFSET_2);

    if (signal.length != (length - L2CAP_SIGNAL_HEADER_LENGTH)) {
        L2capSendCommandReject(
            aclHandle, L2CAP_LE_SIGNALING_CHANNEL, signal.identifier, L2CAP_COMMAND_NOT_UNDERSTOOD, NULL);
        return;
    }

    L2capLeSignal(aclHandle, &signal, buff + L2CAP_SIGNAL_HEADER_LENGTH);
    return;
}

static void L2capLeProcessFixChannelData(uint16_t aclHandle, uint16_t cid, const Packet *pkt)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *conn = NULL;

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        LOG_ERROR("L2cap fix channel recvData but connection not found");
        return;
    }

    LOG_DEBUG("L2capCallback fix channel recvData:%{public}d begin, cid = 0x%04X", __LINE__, cid);
    if (cid == L2CAP_LE_ATTRIBUTE_PROTOCOL) {
        if (inst->chanAtt.recvLeData != NULL) {
            inst->chanAtt.recvLeData(aclHandle, pkt);
        }
    } else if (cid == L2CAP_LE_SECURITY_MANAGER_PROTOCOL) {
        if (inst->chanSm.recvLeData != NULL) {
            inst->chanSm.recvLeData(aclHandle, pkt);
        }
    }
    LOG_DEBUG("L2capCallback fix channel recvData:%{public}d end", __LINE__);

    return;
}

static void L2capLeDataCallback(const L2capLeChannel *chan, Packet *pkt)
{
    L2capLePsm *psm = NULL;

    psm = L2capLeGetPsm(chan->lpsm);
    if (psm == NULL) {
        return;
    }

    LOG_DEBUG("L2capCallback recvLeData:%{public}d begin, cid = 0x%04X", __LINE__, chan->lcid);
    psm->service.recvLeData(chan->lcid, pkt, psm->ctx);
    LOG_DEBUG("L2capCallback recvLeData:%{public}d end", __LINE__);
    return;
}

static void L2capLeProcessLeData(uint16_t aclHandle, uint16_t cid, Packet *pkt)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    uint8_t header[2] = {0};
    uint16_t length;

    L2capLeGetChannel3(aclHandle, cid, &conn, &chan);
    if (chan == NULL) {
        return;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECTED) {
        return;
    }

    if (chan->rxSarPacket != NULL) {
        PacketAssemble(chan->rxSarPacket, pkt);

        PacketRead(chan->rxSarPacket, header, 0, sizeof(header));
        length = L2capLe16ToCpu(header + 0);
        if (PacketSize(chan->rxSarPacket) == (size_t)(length + L2CAP_SIZE_2)) {
            PacketExtractHead(chan->rxSarPacket, header, sizeof(header));

            L2capLeDataCallback(chan, chan->rxSarPacket);
            PacketFree(chan->rxSarPacket);
            chan->rxSarPacket = NULL;
        }
    } else {
        PacketRead(pkt, header, 0, sizeof(header));
        length = L2capLe16ToCpu(header + 0);
        if (PacketSize(pkt) == (size_t)(length + L2CAP_SIZE_2)) {
            PacketExtractHead(pkt, header, sizeof(header));

            L2capLeDataCallback(chan, pkt);
        } else {
            chan->rxSarPacket = PacketRefMalloc(pkt);
        }
    }

    L2capLeSendFlowControlCredit(conn, chan, 1);
    return;
}

static int L2capLeReceivePacket(uint16_t handle, uint16_t cid, Packet *pkt)
{
    uint8_t header[4] = {0};

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    PacketExtractHead(pkt, header, sizeof(header));
    switch (cid) {
        case L2CAP_LE_SIGNALING_CHANNEL:
            L2capLeProcessSignal(handle, pkt);
            break;
        case L2CAP_LE_ATTRIBUTE_PROTOCOL:  // dummy
        case L2CAP_LE_SECURITY_MANAGER_PROTOCOL:
            L2capLeProcessFixChannelData(handle, cid, pkt);
            break;
        default:
            L2capLeProcessLeData(handle, cid, pkt);
            break;
    }

    return 0;
}

static void L2capLeCleanAllChannels(L2capLeConnection *conn, uint8_t status)
{
    L2capLeChannel *chan = NULL;
    L2capLePsm *psm = NULL;
    ListNode *node = NULL;

    while (1) {
        node = ListGetFirstNode(conn->chanList);
        if (node == NULL) {
            break;
        }

        chan = ListGetNodeData(node);

        psm = L2capLeGetPsm(chan->lpsm);
        if (psm != NULL) {
            LOG_DEBUG("L2capCallback leDisconnectAbnormal:%{public}d begin, cid = 0x%04X, reason = %hhu",
                __LINE__,
                chan->lcid,
                status);
            psm->service.leDisconnectAbnormal(chan->lcid, status, psm->ctx);
            LOG_DEBUG("L2capCallback leDisconnectAbnormal:%{public}d end", __LINE__);
        }

        ListRemoveNode(conn->chanList, chan);
        L2capLeDestroyChannel(chan);
    }

    return;
}

static void L2capLeAclDisconnectProcess(L2capLeConnection *conn, uint8_t status, uint8_t reason)
{
    L2capLeInstance *inst = &g_l2capLeInst;

    LOG_DEBUG("L2capCallback leDisconnected:%{public}d begin, aclHandle = 0x%04X, status = %hhu, reason = %hhu",
        __LINE__,
        conn->aclHandle,
        status,
        reason);
    if (inst->chanAtt.leDisconnected != NULL) {
        inst->chanAtt.leDisconnected(conn->aclHandle, status, reason);
    }

    if (inst->chanSm.leDisconnected != NULL) {
        inst->chanSm.leDisconnected(conn->aclHandle, status, reason);
    }
    LOG_DEBUG("L2capCallback leDisconnected:%{public}d end", __LINE__);

    if (status == 0) {
        L2capLeCleanAllChannels(conn, status);
    }
    return;
}

static void L2capLeAclConnectProcess(L2capLeConnection *conn)
{
    L2capLeChannel *chan = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(conn->chanList);
    while (node != NULL) {
        chan = ListGetNodeData(node);
        if (chan->state == L2CAP_CHANNEL_CONNECT_OUT_REQ) {
            L2capSendCreditBasedConnectionReq(conn, chan);
        }

        node = ListGetNextNode(node);
    }

    return;
}

static int L2capLeConnectComplete(const BtAddr *addr, uint16_t handle, uint8_t role, uint8_t status)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeConnection *conn = NULL;

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    LOG_DEBUG("L2capCallback leConnected:%{public}d begin, aclHandle = 0x%04X, role = %hhu, status = %hhu",
        __LINE__,
        handle,
        role,
        status);

    if (inst->chanAtt.leConnected != NULL) {
        inst->chanAtt.leConnected(addr, handle, role, status);
    }

    if (inst->chanSm.leConnected != NULL) {
        inst->chanSm.leConnected(addr, handle, role, status);
    }
    LOG_DEBUG("L2capCallback leConnected:%{public}d end", __LINE__);

    if (status != 0) {
        conn = L2capLeGetConnection2(addr);
        if (conn != NULL) {
            L2capLeCleanAllChannels(conn, status);
            L2capLeDeleteConnection(conn);
        }

        return BT_BAD_STATUS;
    }

    conn = L2capLeGetConnection2(addr);
    if (conn == NULL) {
        conn = L2capLeNewConnection(addr, handle, role);
    }

    conn->aclHandle = handle;
    conn->role = role;
    L2capAddConnectionRef(handle);
    L2capLeAclConnectProcess(conn);

    return BT_NO_ERROR;
}

static int L2capLeDisconnectComplete(uint16_t handle, uint8_t status, uint8_t reason)
{
    L2capLeConnection *conn = NULL;

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    conn = L2capLeGetConnection(handle);
    if (conn == NULL) {
        LOG_ERROR("L2capLeDisconnectComplete but connection not found");
        return BT_BAD_PARAM;
    }

    L2capLeAclDisconnectProcess(conn, status, reason);

    if (status == 0) {
        L2capLeDeleteConnection(conn);
    }

    return BT_NO_ERROR;
}

int L2CAP_LeCreditBasedConnectionReq(
    const BtAddr *addr, uint16_t lpsm, uint16_t rpsm, const L2capLeConfigInfo *cfg, uint16_t *lcid)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    L2capLePsm *psm = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lpsm = 0x%04X, rpsm = 0x%04X", __FUNCTION__, __LINE__, lpsm, rpsm);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if ((addr == NULL) || (lcid == NULL) || (cfg == NULL)) {
        return BT_BAD_PARAM;
    }

    if (cfg->mtu < L2CAP_LE_MIN_MTU) {
        return BT_BAD_PARAM;
    }

    psm = L2capLeGetPsm(lpsm);
    if (psm == NULL) {
        return BT_BAD_PARAM;
    }

    conn = L2capLeGetConnection2(addr);
    if (conn == NULL) {
        conn = L2capLeNewConnection(addr, 0, 0);
    }

    chan = L2capLeNewChannel(conn, lpsm, rpsm);
    chan->state = L2CAP_CHANNEL_CONNECT_OUT_REQ;

    chan->lcfg.mtu = cfg->mtu;
    if (cfg->credit != 0) {
        chan->lcfg.credit = cfg->credit;
    }
    if (chan->lcfg.mps > cfg->mtu) {
        chan->lcfg.mps = cfg->mtu;
    }

    *lcid = chan->lcid;

    if (conn->aclHandle == 0) {
        int result;

        result = L2capConnectLe(addr);
        if (result != BT_NO_ERROR) {
            L2capLeDeleteConnection(conn);
        }

        return result;
    }

    L2capSendCreditBasedConnectionReq(conn, chan);
    return BT_NO_ERROR;
}

int L2CAP_LeCreditBasedConnectionRsp(uint16_t lcid, uint8_t id, const L2capLeConfigInfo *cfg, uint16_t result)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;

    LOG_INFO("%{public}s:%{public}d enter, lcid = 0x%04X, id = %hhu, result = %hu", __FUNCTION__, __LINE__, lcid, id, result);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (cfg == NULL) {
        return BT_BAD_PARAM;
    }

    if (cfg->mtu < L2CAP_LE_MIN_MTU) {
        return BT_BAD_PARAM;
    }

    L2capLeGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECT_IN_REQ) {
        return BT_BAD_STATUS;
    }

    chan->lcfg.mtu = cfg->mtu;
    if (cfg->credit != 0) {
        chan->lcfg.credit = cfg->credit;
    }
    if (chan->lcfg.mps > cfg->mtu) {
        chan->lcfg.mps = cfg->mtu;
    }

    chan->state = L2CAP_CHANNEL_CONNECTED;
    L2capSendCreditBasedConnectionRsp(conn, chan, id, result);

    if (result != L2CAP_LE_CONNECTION_SUCCESSFUL) {
        L2capLeDeleteChannel(conn, chan, 0);
    }

    return BT_NO_ERROR;
}

int L2CAP_LeDisconnectionReq(uint16_t lcid)
{
    L2capLeChannel *chan = NULL;
    L2capLeConnection *conn = NULL;

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capLeGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECTED) {
        return BT_BAD_STATUS;
    }

    chan->state = L2CAP_CHANNEL_DISCONNECT_OUT_REQ;
    L2capLeSendDisconnectionReq(conn, chan);
    return BT_NO_ERROR;
}

int L2CAP_LeDisconnectionRsp(uint16_t lcid, uint8_t id)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capLeGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_DISCONNECT_IN_REQ) {
        return BT_BAD_STATUS;
    }

    L2capLeSendDisconnectionRsp(conn, chan, id);
    return BT_NO_ERROR;
}

int L2CAP_LeSendData(uint16_t lcid, Packet *pkt)
{
    L2capLeConnection *conn = NULL;
    L2capLeChannel *chan = NULL;
    uint16_t length;

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    L2capLeGetChannel2(lcid, &conn, &chan);
    if (chan == NULL) {
        return BT_BAD_PARAM;
    }

    if (pkt == NULL) {
        return BT_BAD_PARAM;
    }

    if (chan->state != L2CAP_CHANNEL_CONNECTED) {
        return BT_BAD_STATUS;
    }

    length = PacketSize(pkt);
    if (length > chan->rcfg.mtu) {
        return BT_BAD_PARAM;
    }

    if (chan->txList == NULL) {
        chan->txList = ListCreate(NULL);
    }

    if (length > chan->rcfg.mps) {
        L2capLeSegmentPacketWithCredit(chan, pkt);
    } else {
        Packet *tpkt = NULL;
        uint8_t *header = NULL;

        tpkt = PacketInheritMalloc(pkt, L2CAP_SIZE_6, 0);
        header = BufferPtr(PacketHead(tpkt));

        L2capCpuToLe16(header + 0, length + L2CAP_SIZE_2);
        L2capCpuToLe16(header + L2CAP_OFFSET_2, chan->rcid);
        L2capCpuToLe16(header + L2CAP_OFFSET_4, length);

        ListAddLast(chan->txList, tpkt);
    }

    L2capLeTxWithCredit(conn, chan);
    return BT_NO_ERROR;
}

int L2CAP_LeRegisterService(uint16_t lpsm, const L2capLeService *svc, void *context)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLePsm *psm = NULL;

    LOG_INFO("%{public}s:%{public}d enter, psm = 0x%04X", __FUNCTION__, __LINE__, lpsm);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (svc == NULL) {
        return BT_BAD_PARAM;
    }

    // check whether the psm is valid
    if (!(lpsm & 0x0001) || (lpsm & 0x0100)) {
        return BT_BAD_PARAM;
    }

    psm = L2capLeGetPsm(lpsm);
    if (psm != NULL) {
        return BT_BAD_STATUS;
    }

    psm = L2capAlloc(sizeof(L2capLePsm));
    if (psm == NULL) {
        return BT_NO_MEMORY;
    }

    psm->lpsm = lpsm;
    psm->ctx = context;
    (void)memcpy_s(&(psm->service), sizeof(L2capLeService), svc, sizeof(L2capLeService));
    ListAddFirst(inst->psmList, psm);

    return BT_NO_ERROR;
}

int L2CAP_LeDeregisterService(uint16_t lpsm)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLePsm *psm = NULL;
    L2capLeConnection *conn = NULL;
    ListNode *nodeConnection = NULL;

    LOG_INFO("%{public}s:%{public}d enter, psm = 0x%04X", __FUNCTION__, __LINE__, lpsm);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    psm = L2capLeGetPsm(lpsm);
    if (psm == NULL) {
        return BT_BAD_PARAM;
    }

    nodeConnection = ListGetFirstNode(inst->connList);
    while (nodeConnection != NULL) {
        L2capLeChannel *chan = NULL;
        ListNode *nodeChannel = NULL;

        conn = ListGetNodeData(nodeConnection);
        nodeChannel = ListGetFirstNode(conn->chanList);
        while (nodeChannel != NULL) {
            chan = ListGetNodeData(nodeChannel);
            // if any channel used the psm, return error
            if (chan->lpsm == lpsm) {
                return BT_BAD_STATUS;
            }

            nodeChannel = ListGetNextNode(nodeChannel);
        }

        nodeConnection = ListGetNextNode(nodeConnection);
    }

    ListRemoveNode(inst->psmList, psm);
    L2capFree(psm);
    return BT_NO_ERROR;
}

int L2CAP_LeRegisterFixChannel(uint16_t cid, const L2capLeFixChannel *chan)
{
    L2capLeInstance *inst = &g_l2capLeInst;

    LOG_INFO("%{public}s:%{public}d enter, cid = 0x%04X", __FUNCTION__, __LINE__, cid);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (cid == L2CAP_LE_ATT_CHANNEL) {
        inst->chanAtt.leConnected = chan->leConnected;
        inst->chanAtt.leDisconnected = chan->leDisconnected;
        inst->chanAtt.recvLeData = chan->recvLeData;
    } else if (cid == L2CAP_LE_SMP_CHANNEL) {
        inst->chanSm.leConnected = chan->leConnected;
        inst->chanSm.leDisconnected = chan->leDisconnected;
        inst->chanSm.recvLeData = chan->recvLeData;
    } else {
        return BT_BAD_PARAM;
    }

    return BT_NO_ERROR;
}

int L2CAP_LeDeregisterFixChannel(uint16_t cid)
{
    L2capLeInstance *inst = &g_l2capLeInst;

    LOG_INFO("%{public}s:%{public}d enter, cid = 0x%04X", __FUNCTION__, __LINE__, cid);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (cid == L2CAP_LE_ATT_CHANNEL) {
        inst->chanAtt.leConnected = NULL;
        inst->chanAtt.leDisconnected = NULL;
        inst->chanAtt.recvLeData = NULL;
    } else if (cid == L2CAP_LE_SMP_CHANNEL) {
        inst->chanSm.leConnected = NULL;
        inst->chanSm.leDisconnected = NULL;
        inst->chanSm.recvLeData = NULL;
    } else {
        return BT_BAD_PARAM;
    }

    return BT_NO_ERROR;
}

int L2CAP_LeConnect(const BtAddr *addr, const L2capLeConnectionParameter *param)
{
    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    return L2capConnectLe(addr);
}

int L2CAP_LeConnectCancel(const BtAddr *addr)
{
    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    return L2capConnectLeCancel(addr);
}

int L2CAP_LeDisconnect(uint16_t aclHandle)
{
    LOG_INFO("%{public}s:%{public}d enter, handle = 0x%04X", __FUNCTION__, __LINE__, aclHandle);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    // Reason: REMOTE USER TERMINATED CONNECTION
    return L2capDisconnect(aclHandle, 0x13);
}

int L2CAP_LeSendFixChannelData(uint16_t aclHandle, uint16_t cid, const Packet *pkt)
{
    L2capLeConnection *conn = NULL;
    uint16_t length;
    Packet *tpkt = NULL;
    uint8_t *header = NULL;

    LOG_INFO("%{public}s:%{public}d enter, cid = 0x%04X, pktLength = %u", __FUNCTION__, __LINE__, cid, PacketSize(pkt));

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return BT_BAD_PARAM;
    }

    length = PacketSize(pkt);

    tpkt = PacketInheritMalloc(pkt, L2CAP_HEADER_LENGTH, 0);
    header = BufferPtr(PacketHead(tpkt));

    L2capCpuToLe16(header + 0, length);
    L2capCpuToLe16(header + L2CAP_OFFSET_2, cid);

    L2capLeSendPacket(aclHandle, tpkt);

    return BT_NO_ERROR;
}

int L2CAP_LeRegisterConnectionParameterUpdate(const L2capLeConnectionParameterUpdate *cb, void *context)
{
    L2capLeInstance *inst = &g_l2capLeInst;

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if (cb == NULL) {
        return BT_BAD_PARAM;
    }

    inst->connParamUpdate.ctx = context;
    (void)memcpy_s(&(inst->connParamUpdate.cb),
        sizeof(L2capLeConnectionParameterUpdate),
        cb,
        sizeof(L2capLeConnectionParameterUpdate));

    return BT_NO_ERROR;
}

int L2CAP_LeDeregisterConnectionParameterUpdate()
{
    L2capLeInstance *inst = &g_l2capLeInst;

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    (void)memset_s(&(inst->connParamUpdate),
        sizeof(L2capLeConnectionParameterUpdate),
        0,
        sizeof(L2capLeConnectionParameterUpdate));
    return BT_NO_ERROR;
}

int L2CAP_LeConnectionParameterUpdateReq(uint16_t aclHandle, const L2capLeConnectionParameter *param)
{
    L2capLeConnection *conn = NULL;
    uint8_t buff[8] = {0};
    L2capSignalHeader signal = {0};
    Packet *pkt = NULL;

    LOG_INFO("%{public}s:%{public}d enter, handle = 0x%04X", __FUNCTION__, __LINE__, aclHandle);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    if ((param->connIntervalMin < 0x0006) || (param->connIntervalMin > 0x0C80)) {
        return BT_BAD_PARAM;
    }

    if ((param->connIntervalMax < 0x0006) || (param->connIntervalMax > 0x0C80)) {
        return BT_BAD_PARAM;
    }

    if ((param->supervisionTimeout < 0x000A) || (param->supervisionTimeout > 0x0C80)) {
        return BT_BAD_PARAM;
    }

    if (param->connLatency > 0x01F3) {
        return BT_BAD_PARAM;
    }

    if (param->connLatency > ((param->supervisionTimeout / (param->connIntervalMax * L2CAP_SIZE_2)) - 1)) {
        return BT_BAD_PARAM;
    }

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return BT_BAD_PARAM;
    }

    if (conn->role != L2CAP_LE_ROLE_SLAVE) {
        return BT_BAD_PARAM;
    }

    L2capCpuToLe16(buff + 0, param->connIntervalMin);
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, param->connIntervalMax);
    L2capCpuToLe16(buff + L2CAP_OFFSET_4, param->connLatency);
    L2capCpuToLe16(buff + L2CAP_OFFSET_6, param->supervisionTimeout);

    signal.code = L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST;
    signal.identifier = L2capLeGetNewIdentifier(conn);
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_LE_SIGNALING_CHANNEL, &signal, buff);
    L2capCreatePendingRequest(conn->pendingList, 0, &signal, L2CAP_DEFAULT_RTX, L2capLeResponseTimeoutCallback);
    return L2capLeSendPacket(aclHandle, pkt);
}

int L2CAP_LeConnectionParameterUpdateRsp(uint16_t aclHandle, uint8_t id, uint16_t result)
{
    L2capLeConnection *conn = NULL;
    uint8_t buff[2] = {0};
    L2capSignalHeader signal = {0};
    Packet *pkt = NULL;

    LOG_INFO("%{public}s:%{public}d enter, handle = 0x%04X", __FUNCTION__, __LINE__, aclHandle);

    if (L2capLeInitialized() != BT_NO_ERROR) {
        return BT_BAD_STATUS;
    }

    conn = L2capLeGetConnection(aclHandle);
    if (conn == NULL) {
        return BT_BAD_PARAM;
    }

    L2capCpuToLe16(buff + 0, result);

    signal.code = L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE;
    signal.identifier = id;
    signal.length = sizeof(buff);

    pkt = L2capBuildSignalPacket(L2CAP_LE_SIGNALING_CHANNEL, &signal, buff);

    return L2capLeSendPacket(aclHandle, pkt);
}

void L2CAP_LeInitialize(int traceLevel)
{
    L2capLeInstance *inst = &g_l2capLeInst;
    L2capLeCallback cmnCallback = {0};

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    cmnCallback.aclConnected = L2capLeConnectComplete;
    cmnCallback.aclDisconnected = L2capLeDisconnectComplete;
    cmnCallback.recvL2capPacket = L2capLeReceivePacket;
    L2capRegisterLe(&cmnCallback);

    inst->psmList = ListCreate(NULL);
    inst->connList = ListCreate(NULL);
    inst->nextLcid = L2CAP_LE_MIN_CID;
    return;
}

void L2CAP_LeFinalize()
{
    L2capLeInstance *leinst = &g_l2capLeInst;
    ListNode *node = NULL;
    L2capLeConnection *conn = NULL;
    void *psm = NULL;

    LOG_INFO("%{public}s:%{public}d enter", __FUNCTION__, __LINE__);

    node = ListGetFirstNode(leinst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        L2capLeDeleteConnection(conn);

        node = ListGetFirstNode(leinst->connList);
    }

    ListDelete(leinst->connList);
    leinst->connList = NULL;

    node = ListGetFirstNode(leinst->psmList);
    while (node != NULL) {
        psm = ListGetNodeData(node);
        ListRemoveNode(leinst->psmList, psm);
        L2capFree(psm);

        node = ListGetFirstNode(leinst->psmList);
    }

    ListDelete(leinst->psmList);
    leinst->psmList = NULL;

    return;
}
