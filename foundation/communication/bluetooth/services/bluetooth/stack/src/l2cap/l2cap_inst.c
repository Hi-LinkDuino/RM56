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

#include "l2cap_inst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "l2cap_cmn.h"

static L2capInstance g_l2capInst;

L2capInstance *L2capGetInstance()
{
    return &g_l2capInst;
}

int L2capInitialized()
{
    L2capInstance *inst = L2capGetInstance();

    if (inst->connList != NULL) {
        return BT_NO_ERROR;
    }

    return BT_BAD_STATUS;
}

L2capPsm *L2capGetPsm(uint16_t lpsm)
{
    L2capInstance *inst = L2capGetInstance();
    L2capPsm *psm = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->psmList);
    while (node != NULL) {
        psm = ListGetNodeData(node);
        if (psm->lpsm == lpsm) {
            return psm;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

L2capConnection *L2capGetConnection(uint16_t aclHandle)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        if (conn->aclHandle == aclHandle) {
            return conn;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

L2capConnection *L2capGetConnection2(const BtAddr *addr)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        if (memcmp(conn->addr.addr, addr->addr, sizeof(addr->addr)) == 0) {
            return conn;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

L2capConnection *L2capGetConnection3(const L2capChannel *chan)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;
    ListNode *node = NULL;
    ListNode *nodeChan = NULL;

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);

        nodeChan = ListGetFirstNode(conn->chanList);
        while (nodeChan != NULL) {
            if (ListGetNodeData(nodeChan) == chan) {
                return conn;
            }

            nodeChan = ListGetNextNode(nodeChan);
        }

        node = ListGetNextNode(node);
    }

    return conn;
}

L2capChannel *L2capGetChannel(const L2capConnection *conn, int16_t lcid)
{
    L2capChannel *chan = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(conn->chanList);
    while (node != NULL) {
        chan = ListGetNodeData(node);
        if (chan->lcid == lcid) {
            return chan;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

void L2capGetChannel2(uint16_t lcid, L2capConnection **conn, L2capChannel **chan)
{
    L2capInstance *inst = L2capGetInstance();
    ListNode *node = NULL;

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        *conn = ListGetNodeData(node);
        *chan = L2capGetChannel(*conn, lcid);
        if ((*chan) != NULL) {
            break;
        }

        node = ListGetNextNode(node);
    }

    return;
}

void L2capGetChannel3(uint16_t aclHandle, uint16_t lcid, L2capConnection **conn, L2capChannel **chan)
{
    *conn = L2capGetConnection(aclHandle);
    if ((*conn) == NULL) {
        return;
    }

    *chan = L2capGetChannel(*conn, lcid);
    return;
}

static uint16_t L2capGetNewLcid()
{
    L2capInstance *inst = L2capGetInstance();
    uint16_t lcid = L2CAP_MIN_CID;

    if (inst->nextLcid == 0) {
        L2capConnection *conn = NULL;
        L2capChannel *chan = NULL;

        while (1) {
            L2capGetChannel2(lcid, &conn, &chan);
            if (chan == NULL) {
                break;
            }

            lcid += 1;
        }
    } else {
        lcid = inst->nextLcid;

        if (lcid == L2CAP_MAX_CID) {
            inst->nextLcid = 0;
        } else {
            inst->nextLcid += 1;
        }
    }

    return lcid;
}

static void L2capSetDefaultConfigOptions(L2capConfigInfo *cfg)
{
    cfg->mtu = L2CAP_DEFAULT_MTU;
    cfg->flushTimeout = L2CAP_NONE_FLUSH_PACKET;
    cfg->rfc.mode = L2CAP_BASIC_MODE;
    cfg->rfc.maxTransmit = L2CAP_DEFAULT_MAX_TRANSMIT;
    cfg->rfc.txWindowSize = L2CAP_DEFAULT_TX_WINDOW;
    cfg->rfc.rxWindowSize = L2CAP_DEFAULT_TX_WINDOW;
    cfg->rfc.retransmissionTimeout = 0;
    cfg->rfc.monitorTimeout = 0;
    cfg->rfc.mps = L2capGetRxBufferSize() - L2CAP_SIZE_10;
    cfg->fcs = 0x01;

    return;
}

L2capChannel *L2capNewChannel(L2capConnection *conn, uint16_t lpsm, uint16_t rpsm)
{
    L2capChannel *chan = NULL;

    chan = L2capAlloc(sizeof(L2capChannel));
    if (chan == NULL) {
        return NULL;
    }

    if (conn->discTimer != NULL) {
        AlarmCancel(conn->discTimer);
        AlarmDelete(conn->discTimer);
        conn->discTimer = NULL;
    } else {
        if ((conn->state == L2CAP_CONNECTION_CONNECTED) && (ListGetFirstNode(conn->chanList) == NULL)) {
            if (L2capAddConnectionRef(conn->aclHandle) != BT_NO_ERROR) {
                conn->state = L2CAP_CONNECTION_DISCONNECTING;
            }
        }
    }

    chan->lcid = L2capGetNewLcid();
    chan->lpsm = lpsm;
    chan->rpsm = rpsm;
    chan->state = L2CAP_CHANNEL_IDLE;
    chan->cfgState = 0;

    chan->part.options = NULL;
    chan->part.length = 0;

    L2capSetDefaultConfigOptions(&(chan->lcfg));
    L2capSetDefaultConfigOptions(&(chan->rcfg));

    ListAddLast(conn->chanList, chan);
    return chan;
}

static void L2capDestroyChannelTx(L2capChannel *chan)
{
    ListNode *node = NULL;

    if (chan->lcfg.rfc.mode == L2CAP_STREAM_MODE) {
        Packet *pkt = NULL;

        while (1) {
            node = ListGetFirstNode(chan->erfc.txList);
            if (node == NULL) {
                break;
            }

            pkt = ListGetNodeData(node);
            ListRemoveNode(chan->erfc.txList, pkt);
            PacketFree(pkt);
        }
    } else {
        L2capErfcTxPacket *tx = NULL;

        while (1) {
            node = ListGetFirstNode(chan->erfc.txList);
            if (node == NULL) {
                break;
            }

            tx = ListGetNodeData(node);
            ListRemoveNode(chan->erfc.txList, tx);
            PacketFree(tx->pkt);
            L2capFree(tx);
        }
    }

    ListDelete(chan->erfc.txList);
    return;
}

void L2capDestroyChannel(L2capChannel *chan)
{
    if (chan->erfc.monitorTimer != NULL) {
        AlarmCancel(chan->erfc.monitorTimer);
        AlarmDelete(chan->erfc.monitorTimer);
    }

    if (chan->erfc.retransmissionTimer != NULL) {
        AlarmCancel(chan->erfc.retransmissionTimer);
        AlarmDelete(chan->erfc.retransmissionTimer);
    }

    if (chan->part.options != NULL) {
        L2capFree(chan->part.options);
    }

    if (chan->erfc.txList != NULL) {
        L2capDestroyChannelTx(chan);
    }

    if (chan->erfc.rxSarPacket != NULL) {
        PacketFree(chan->erfc.rxSarPacket);
    }

    L2capFree(chan);
    return;
}

static void L2capDisconnectTimeout(const void *parameter)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;
    ListNode *node = NULL;

    if (inst->connList == NULL) {
        return;
    }

    node = ListGetFirstNode(inst->connList);
    while (node != NULL) {
        conn = ListGetNodeData(node);
        if (conn == parameter) {
            if (ListGetFirstNode(conn->chanList) != NULL) {
                break;
            }

            AlarmDelete(conn->discTimer);
            conn->discTimer = NULL;
            // Reason: REMOTE USER TERMINATED CONNECTION
            L2capDisconnect(conn->aclHandle, 0x13);
            break;
        }

        node = ListGetNextNode(node);
    }

    return;
}

static void L2capDisconnectTimeoutCallback(void *parameter)
{
    L2capAsynchronousProcess(L2capDisconnectTimeout, NULL, parameter);
    return;
}

void L2capDeleteChannel(L2capConnection *conn, L2capChannel *chan, uint16_t removeAcl)
{
    ListRemoveNode(conn->chanList, chan);
    L2capDestroyChannel(chan);

    if (removeAcl) {
        if (ListGetFirstNode(conn->chanList) == NULL) {
            // Reason: REMOTE USER TERMINATED CONNECTION
            L2capDisconnect(conn->aclHandle, 0x13);
        }
    } else {
        if (ListGetFirstNode(conn->chanList) == NULL) {
            if (conn->discTimer != NULL) {
                AlarmCancel(conn->discTimer);
            } else {
                conn->discTimer = AlarmCreate("", false);
            }

            AlarmSet(conn->discTimer, L2CAP_DISCONNECTION_TIME_OUT, L2capDisconnectTimeoutCallback, conn);
        }
    }

    return;
}

L2capConnection *L2capNewConnection(const BtAddr *addr, uint16_t aclHandle)
{
    L2capInstance *inst = L2capGetInstance();
    L2capConnection *conn = NULL;

    conn = L2capAlloc(sizeof(L2capConnection));
    if (conn == NULL) {
        return NULL;
    }

    (void)memcpy_s(&(conn->addr), sizeof(BtAddr), addr, sizeof(BtAddr));
    conn->aclHandle = aclHandle;
    conn->state = L2CAP_CONNECTION_IDLE;
    conn->info.state = L2CAP_INFO_STATE_NONE;
    conn->nextIdentifier = L2CAP_MIN_IDENTIFIER;

    conn->chanList = ListCreate(NULL);
    conn->pendingList = ListCreate(NULL);
    conn->discTimer = NULL;

    ListAddFirst(inst->connList, conn);
    return conn;
}

void L2capDeleteConnection(L2capConnection *conn)
{
    L2capInstance *inst = L2capGetInstance();
    ListNode *node = NULL;

    if (conn->chanList != NULL) {
        L2capChannel *chan = NULL;

        while (1) {
            node = ListGetFirstNode(conn->chanList);
            if (node == NULL) {
                break;
            }

            chan = ListGetNodeData(node);
            ListRemoveNode(conn->chanList, chan);

            L2capDestroyChannel(chan);
        }

        ListDelete(conn->chanList);
    }

    if (conn->pendingList != NULL) {
        L2capClearPendingRequest(conn->pendingList);
        ListDelete(conn->pendingList);
    }

    if (conn->discTimer != NULL) {
        AlarmCancel(conn->discTimer);
        AlarmDelete(conn->discTimer);
        conn->discTimer = NULL;
    }

    ListRemoveNode(inst->connList, conn);
    L2capFree(conn);

    // if no connection exists, reset nextLcid value
    if (ListGetFirstNode(inst->connList) == NULL) {
        inst->nextLcid = L2CAP_MIN_CID;
    }

    return;
}