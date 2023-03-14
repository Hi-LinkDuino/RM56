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

#include "l2cap_cmn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "allocator.h"
#include "bt_endian.h"

#include "btm.h"
#include "hci/hci.h"

#include "../btm/btm_thread.h"

typedef struct {
    uint16_t handle;
    uint16_t length;
    uint16_t cid;
    Packet *pkt;
} L2capFragmentationPacket;

static L2capBdrCallback g_l2capBdr;
static L2capLeCallback g_l2capLe;

static List *g_fragmentationList;

static void L2capProcessPacket(uint16_t handle, uint16_t cid, Packet *pkt)
{
    if (cid >= L2CAP_MIN_CID) {
        if (g_l2capBdr.recvL2capPacket != NULL) {
            g_l2capBdr.recvL2capPacket(handle, cid, pkt);
        }
        return;
    } else if ((cid >= L2CAP_LE_MIN_CID) && (cid <= L2CAP_LE_MAX_CID)) {
        if (g_l2capLe.recvL2capPacket != NULL) {
            g_l2capLe.recvL2capPacket(handle, cid, pkt);
        }
        return;
    }

    switch (cid) {
        case L2CAP_SIGNALING_CHANNEL:
            if (g_l2capBdr.recvL2capPacket != NULL) {
                g_l2capBdr.recvL2capPacket(handle, cid, pkt);
            }
            return;
        case L2CAP_LE_ATTRIBUTE_PROTOCOL:  // dummy
        case L2CAP_LE_SIGNALING_CHANNEL:   // dummy
        case L2CAP_LE_SECURITY_MANAGER_PROTOCOL:
            if (g_l2capLe.recvL2capPacket != NULL) {
                g_l2capLe.recvL2capPacket(handle, cid, pkt);
            }
            return;
        default:
            break;
    }

    return;
}

static void L2capRecombineStart(uint16_t handle, uint16_t length, uint16_t cid, const Packet *pkt)
{
    L2capFragmentationPacket *frag = NULL;
    ListNode *node = NULL;
    Packet *tpkt = NULL;

    if (g_fragmentationList == NULL) {
        return;
    }

    // if there are already packet with the same handle, the old packet will be discard
    node = ListGetFirstNode(g_fragmentationList);
    while (node != NULL) {
        frag = ListGetNodeData(node);
        if (frag->handle == handle) {
            ListRemoveNode(g_fragmentationList, frag);
            PacketFree(frag->pkt);
            L2capFree(frag);
            break;
        }

        node = ListGetNextNode(node);
    }

    tpkt = PacketRefMalloc(pkt);
    frag = L2capAlloc(sizeof(L2capFragmentationPacket));
    if (frag == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    frag->handle = handle;
    frag->length = length;
    frag->cid = cid;
    frag->pkt = tpkt;

    ListAddLast(g_fragmentationList, frag);
    return;
}

static void L2capRecombineContinue(uint16_t handle, const Packet *pkt)
{
    L2capFragmentationPacket *frag = NULL;
    ListNode *node = NULL;
    uint16_t pktLength;

    if (g_fragmentationList == NULL) {
        return;
    }

    node = ListGetFirstNode(g_fragmentationList);
    while (node != NULL) {
        frag = ListGetNodeData(node);
        if (frag->handle == handle) {
            PacketAssemble(frag->pkt, pkt);
            pktLength = (uint16_t) PacketSize(frag->pkt);
            if (frag->length == (pktLength - L2CAP_HEADER_LENGTH)) {
                L2capProcessPacket(handle, frag->cid, frag->pkt);

                ListRemoveNode(g_fragmentationList, frag);
                PacketFree(frag->pkt);
                L2capFree(frag);
            } else if (frag->length < (pktLength - L2CAP_HEADER_LENGTH)) {  // invalid packet length
                ListRemoveNode(g_fragmentationList, frag);
                PacketFree(frag->pkt);
                L2capFree(frag);
            }

            break;
        }

        node = ListGetNextNode(node);
    }

    return;
}

static void L2capAclDataReceived(uint16_t handle, uint8_t pb, uint8_t bc, Packet *pkt)
{
    if ((pb == L2CAP_FIRST_NON_AUTOMATICALLY_FLUSHABLE_PACKET) ||
        (pb == L2CAP_FIRST_AUTOMATICALLY_FLUSHABLE_PACKET)) {  // start packet
        uint16_t pktLength = PacketSize(pkt);
        if (pktLength <= L2CAP_HEADER_LENGTH) {
            return;
        }

        uint8_t header[L2CAP_HEADER_LENGTH] = {0};
        PacketRead(pkt, header, 0, sizeof(header));
        uint16_t length = L2capLe16ToCpu(header + 0);
        uint16_t cid = L2capLe16ToCpu(header + L2CAP_OFFSET_2);

        if (length == (pktLength - L2CAP_HEADER_LENGTH)) {
            L2capProcessPacket(handle, cid, pkt);
        } else {
            // invalid packet length
            if (length < (pktLength - L2CAP_HEADER_LENGTH)) {
                return;
            }
            L2capRecombineStart(handle, length, cid, pkt);
        }
    } else {  // continue packet
        L2capRecombineContinue(handle, pkt);
    }

    return;
}

typedef struct {
    uint16_t handle;
    uint8_t pb;
    uint8_t bc;
    Packet *pkt;
} L2capAclDataReceivedContext;

static void L2capAclDataReceivedContextDestroy(void *context)
{
    L2capAclDataReceivedContext *ctx = NULL;

    ctx = context;

    PacketFree(ctx->pkt);
    L2capFree(ctx);
    return;
}

static void L2capAclDataReceivedCallbackProcess(const void *context)
{
    L2capAclDataReceivedContext *ctx = NULL;

    ctx = (L2capAclDataReceivedContext *)context;

    L2capAclDataReceived(ctx->handle, ctx->pb, ctx->bc, ctx->pkt);

    L2capAclDataReceivedContextDestroy(ctx);
    return;
}

static void L2capAclDataReceivedCallback(uint16_t handle, uint8_t pb, uint8_t bc, Packet *pkt)
{
    L2capAclDataReceivedContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2capAclDataReceivedContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->handle = handle;
    ctx->pb = pb;
    ctx->bc = bc;
    ctx->pkt = PacketRefMalloc(pkt);

    L2capAsynchronousProcess(L2capAclDataReceivedCallbackProcess, L2capAclDataReceivedContextDestroy, ctx);
    return;
}

static void L2capAclDisconnected(uint8_t status, uint16_t handle, uint8_t reason, void *context)
{
    L2capFragmentationPacket *frag = NULL;
    ListNode *node = NULL;

    if (g_fragmentationList != NULL) {
        node = ListGetFirstNode(g_fragmentationList);
        while (node != NULL) {
            frag = ListGetNodeData(node);
            if (frag->handle == handle) {
                ListRemoveNode(g_fragmentationList, frag);
                PacketFree(frag->pkt);
                L2capFree(frag);

                node = ListGetFirstNode(g_fragmentationList);
                continue;
            }

            node = ListGetNextNode(node);
        }
    }

    if (g_l2capBdr.aclDisconnected != NULL) {
        g_l2capBdr.aclDisconnected(handle, status, reason);
    }

    if (g_l2capLe.aclDisconnected != NULL) {
        g_l2capLe.aclDisconnected(handle, status, reason);
    }
    return;
}

typedef struct {
    uint8_t status;
    uint16_t handle;
    uint8_t reason;
    void *context;
} L2capAclDisconnectedContext;

static void L2capAclDisconnectedCallbackProcess(const void *context)
{
    L2capAclDisconnectedContext *ctx = NULL;

    ctx = (L2capAclDisconnectedContext *)context;

    L2capAclDisconnected(ctx->status, ctx->handle, ctx->reason, ctx->context);

    L2capFree(ctx);
    return;
}

static void L2capAclDisconnectedCallback(uint8_t status, uint16_t handle, uint8_t reason, void *context)
{
    L2capAclDisconnectedContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2capAclDisconnectedContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->status = status;
    ctx->handle = handle;
    ctx->reason = reason;
    ctx->context = context;

    L2capAsynchronousProcess(L2capAclDisconnectedCallbackProcess, L2capFree, ctx);
    return;
}

static void L2capBdrAclConnected(uint8_t status, uint16_t handle, const BtAddr *addr)
{
    if (g_l2capBdr.aclConnected != NULL) {
        g_l2capBdr.aclConnected((BtAddr *)addr, handle, status);
    }
    return;
}

typedef struct {
    uint8_t status;
    uint16_t handle;
    BtAddr addr;
} L2capBdrAclConnectedContext;

static void L2capBdrAclConnectedCallbackProcess(const void *context)
{
    L2capBdrAclConnectedContext *ctx = NULL;

    ctx = (L2capBdrAclConnectedContext *)context;

    L2capBdrAclConnected(ctx->status, ctx->handle, &(ctx->addr));

    L2capFree(ctx);
    return;
}

static void L2capBdrAclConnectedCallback(const BtmAclConnectCompleteParam *param, void *context)
{
    L2capBdrAclConnectedContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2capBdrAclConnectedContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }
    ctx->status = param->status;
    ctx->handle = param->connectionHandle;
    (void)memcpy_s(&(ctx->addr), sizeof(BtAddr), param->addr, sizeof(BtAddr));

    L2capAsynchronousProcess(L2capBdrAclConnectedCallbackProcess, L2capFree, ctx);
    return;
}

static void L2capLeAclConnected(uint8_t status, uint16_t handle, const BtAddr *addr, uint8_t role, void *context)
{
    if (g_l2capLe.aclConnected != NULL) {
        g_l2capLe.aclConnected((BtAddr *)addr, handle, role, status);
    }
    return;
}

typedef struct {
    uint8_t status;
    uint16_t handle;
    BtAddr addr;
    uint8_t role;
    void *context;
} L2capLeAclConnectedContext;

static void L2capLeAclConnectedCallbackProcess(const void *context)
{
    L2capLeAclConnectedContext *ctx = NULL;

    ctx = (L2capLeAclConnectedContext *)context;

    L2capLeAclConnected(ctx->status, ctx->handle, &(ctx->addr), ctx->role, ctx->context);

    L2capFree(ctx);
    return;
}

static void L2capLeAclConnectedCallback(
    uint8_t status, uint16_t handle, const BtAddr *addr, uint8_t role, void *context)
{
    L2capLeAclConnectedContext *ctx = NULL;

    ctx = L2capAlloc(sizeof(L2capLeAclConnectedContext));
    if (ctx == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    ctx->status = status;
    ctx->handle = handle;
    (void)memcpy_s(&(ctx->addr), sizeof(BtAddr), addr, sizeof(BtAddr));
    ctx->role = role;
    ctx->context = context;

    L2capAsynchronousProcess(L2capLeAclConnectedCallbackProcess, L2capFree, ctx);
    return;
}

void L2capCpuToLe16(uint8_t *dst, uint16_t src)
{
    uint16_t tmp;

    tmp = H2LE_16(src);
    (void)memcpy_s(dst, sizeof(uint16_t), &tmp, sizeof(uint16_t));
    return;
}

uint16_t L2capLe16ToCpu(const uint8_t *src)
{
    uint16_t tmp = 0;

    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        tmp += (src[i] << (L2CAP_SIZE_8 * i));
    }

    return LE2H_16(tmp);
}

void *L2capAlloc(int size)
{
    void *p;

    p = MEM_MALLOC.alloc(size);
    if (p != NULL) {
        (void)memset_s(p, size, 0, size);
    }

    return p;
}

void L2capFree(void *p)
{
    MEM_MALLOC.free(p);
    return;
}

int L2capAsynchronousProcess(void (*task)(const void *context), void (*destroy)(void *context), void *context)
{
    int result;

    result = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_LA2CAP, (void (*)(void *))task, context);
    if (result != BT_NO_ERROR) {
        if (destroy != NULL) {
            destroy(context);
        }
    }

    return result;
}

Packet *L2capBuildSignalPacket(uint16_t cid, const L2capSignalHeader *signal, const uint8_t *data)
{
    Packet *pkt = NULL;
    uint8_t *buff = NULL;

    pkt = PacketMalloc(0, 0, L2CAP_HEADER_LENGTH + L2CAP_SIGNAL_HEADER_LENGTH + signal->length);
    if (pkt == NULL) {
        return NULL;
    }

    buff = BufferPtr(PacketContinuousPayload(pkt));

    // build Length field
    L2capCpuToLe16(buff + 0, L2CAP_SIGNAL_HEADER_LENGTH + signal->length);

    // build cid field
    L2capCpuToLe16(buff + L2CAP_OFFSET_2, cid);

    buff[L2CAP_OFFSET_4] = signal->code;
    buff[L2CAP_OFFSET_5] = signal->identifier;

    L2capCpuToLe16(buff + L2CAP_OFFSET_6, signal->length);

    if (signal->length > 0) {
        if (memcpy_s(
            buff + L2CAP_HEADER_LENGTH + L2CAP_SIGNAL_HEADER_LENGTH, signal->length, data, signal->length) != EOK) {
            LOG_ERROR("L2capBuildSignalPacket memcpy_s failed");
        }
    }

    return pkt;
}

int L2capSendCommandReject(uint16_t handle, uint16_t cid, uint8_t ident, uint16_t reason, uint16_t data[2])
{
    Packet *pkt = NULL;
    uint8_t buff[12] = {0};
    L2capSignalHeader siganl = {0};

    (void)memset_s(buff, sizeof(buff), 0, sizeof(buff));

    L2capCpuToLe16(buff + 0, reason);

    if (reason == L2CAP_COMMAND_NOT_UNDERSTOOD) {
        siganl.length = L2CAP_SIZE_2;
    } else if (reason == L2CAP_SIGNAL_MTU_EXCEEDED) {
        L2capCpuToLe16(buff + L2CAP_OFFSET_2, L2CAP_SIGNAL_MTU);
        siganl.length = L2CAP_SIZE_4;
    } else if (reason == L2CAP_INVALID_CID_IN_REQUEST) {
        L2capCpuToLe16(buff + L2CAP_OFFSET_2, data[0]);
        L2capCpuToLe16(buff + L2CAP_OFFSET_4, data[1]);

        siganl.length = L2CAP_SIZE_6;
    } else {
        return BT_BAD_PARAM;
    }

    siganl.code = L2CAP_COMMAND_REJECT;
    siganl.identifier = ident;

    pkt = L2capBuildSignalPacket(cid, &siganl, buff);
    return L2capSendPacket(handle, L2CAP_NONE_FLUSH_PACKET, pkt);
}

void L2capCreatePendingRequest(
    List *pendingList, uint16_t lcid, const L2capSignalHeader *signal, uint64_t timeo, AlarmCallback timerExpired)
{
    L2capPendingRequest *req = NULL;

    req = L2capAlloc(sizeof(L2capPendingRequest));
    if (req == NULL) {
        LOG_WARN("malloc failed");
        return;
    }

    req->lcid = lcid;
    req->code = signal->code;
    req->identifier = signal->identifier;
    req->timer = AlarmCreate("", false);
    AlarmSet(req->timer, timeo, timerExpired, req);

    ListAddLast(pendingList, req);
    return;
}

void L2capDestroyPendingRequest(List *pendingList, uint8_t identifier)
{
    L2capPendingRequest *req = NULL;

    req = L2capGetPendingRequest(pendingList, identifier);
    if (req != NULL) {
        ListRemoveNode(pendingList, req);
        AlarmCancel(req->timer);
        AlarmDelete(req->timer);
        L2capFree(req);
    }

    return;
}

L2capPendingRequest *L2capGetPendingRequest(List *pendingList, uint8_t identifier)
{
    L2capPendingRequest *req = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(pendingList);
    while (node != NULL) {
        req = ListGetNodeData(node);
        if (req->identifier == identifier) {
            return req;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

L2capPendingRequest *L2capGetPendingRequest2(List *pendingList, const void *request)
{
    L2capPendingRequest *req = NULL;
    ListNode *node = NULL;

    node = ListGetFirstNode(pendingList);
    while (node != NULL) {
        req = ListGetNodeData(node);
        if (req == request) {
            return req;
        }

        node = ListGetNextNode(node);
    }

    return NULL;
}

void L2capClearPendingRequest(List *pendingList)
{
    L2capPendingRequest *req = NULL;
    ListNode *node = NULL;

    while (1) {
        node = ListGetFirstNode(pendingList);
        if (node == NULL) {
            break;
        }

        req = ListGetNodeData(node);

        ListRemoveNode(pendingList, req);
        AlarmCancel(req->timer);
        AlarmDelete(req->timer);
        L2capFree(req);
    }

    return;
}

int L2capSendPacket(uint16_t handle, uint16_t flushTimeout, Packet *pkt)
{
    int result;

    result = L2capSendPacketNoFree(handle, flushTimeout, pkt);
    PacketFree(pkt);
    return result;
}

int L2capSendPacketNoFree(uint16_t handle, uint16_t flushTimeout, Packet *pkt)
{
    int result;

    if (BTM_IsControllerSupportNonFlushablePacketBoundaryFlag()) {
        if (flushTimeout == L2CAP_NONE_FLUSH_PACKET) {
            result = HCI_SendAclData(handle, NON_FLUSHABLE_PACKET, pkt);
        } else {
            result = HCI_SendAclData(handle, FLUSHABLE_PACKET, pkt);
        }
    } else {
        result = HCI_SendAclData(handle, FLUSHABLE_PACKET, pkt);
    }

    return result;
}

int L2capLeSendPacket(uint16_t handle, Packet *pkt)
{
    int result;

    result = HCI_SendAclData(handle, NON_FLUSHABLE_PACKET, pkt);
    PacketFree(pkt);
    return result;
}

uint16_t L2capGetTxBufferSize()
{
    uint16_t bufferSize = 0;

    BTM_GetAclDataPacketLength(&bufferSize);
    return bufferSize;
}

uint16_t L2capLeGetTxBufferSize()
{
    uint16_t bufferSize = 0;

    BTM_GetLeAclDataPacketLength(&bufferSize);
    return bufferSize;
}

uint16_t L2capGetRxBufferSize()
{
    return (uint16_t)L2CAP_MTU_SIZE;
}

int L2capConnectBdr(const BtAddr *addr)
{
    return BTM_AclConnect(addr);
}

int L2capConnectLe(const BtAddr *addr)
{
    return BTM_LeConnect(addr);
}

int L2capConnectLeCancel(const BtAddr *addr)
{
    return BTM_LeCancelConnect(addr);
}

int L2capAddConnectionRef(uint16_t handle)
{
    LOG_DEBUG("L2cap Call BTM_AclAddRef, handle = 0x%04X", handle);
    return BTM_AclAddRef(handle);
}

int L2capDisconnect(uint16_t handle, uint8_t reason)
{
    LOG_DEBUG("L2cap Call BTM_AclRelease, handle = 0x%04X", handle);
    BTM_AclRelease(handle);
    return BT_NO_ERROR;
}

static BtmAclCallbacks g_btmAclCallback = {
    .connectionComplete = L2capBdrAclConnectedCallback,
    .disconnectionComplete = L2capAclDisconnectedCallback,
    .leConnectionComplete = L2capLeAclConnectedCallback,
    .leDisconnectionComplete = L2capAclDisconnectedCallback,
};

static HciAclCallbacks g_hciAclCallback = {
    L2capAclDataReceivedCallback,
};

int L2capRegisterBdr(const L2capBdrCallback *cb)
{
    (void)memcpy_s(&g_l2capBdr, sizeof(L2capBdrCallback), cb, sizeof(L2capBdrCallback));
    return BT_NO_ERROR;
}

int L2capRegisterLe(const L2capLeCallback *cb)
{
    (void)memcpy_s(&g_l2capLe, sizeof(L2capLeCallback), cb, sizeof(L2capLeCallback));
    return BT_NO_ERROR;
}

void L2capCommonStartup()
{
    if (g_fragmentationList != NULL) {
        return;
    }

    g_fragmentationList = ListCreate(NULL);

    BTM_RegisterAclCallbacks(&g_btmAclCallback, NULL);
    HCI_RegisterAclCallbacks(&g_hciAclCallback);

    return;
}

void L2capCommonShutdown()
{
    BTM_DeregisterAclCallbacks(&g_btmAclCallback);
    HCI_DeregisterAclCallbacks(&g_hciAclCallback);

    if (g_fragmentationList != NULL) {
        L2capFragmentationPacket *frag = NULL;
        ListNode *node = NULL;

        while (true) {
            node = ListGetFirstNode(g_fragmentationList);
            if (node == NULL) {
                break;
            }

            frag = ListGetNodeData(node);
            ListRemoveNode(g_fragmentationList, frag);
            PacketFree(frag->pkt);
            L2capFree(frag);
        }

        ListDelete(g_fragmentationList);
        g_fragmentationList = NULL;
    }

    return;
}
