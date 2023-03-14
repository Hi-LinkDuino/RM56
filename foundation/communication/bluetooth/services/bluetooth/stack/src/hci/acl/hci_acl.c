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

#include "hci_acl.h"

#include <stdbool.h>

#include "btstack.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "platform/include/queue.h"

#include "hci/hci.h"
#include "hci/hci_internal.h"

#define PACKET_BOUNDARY_FIRST_NON_FLUSHABLE 0x00
#define PACKET_BOUNDARY_CONTINUING 0x01
#define PACKET_BOUNDARY_FIRST_FLUSHABLE 0x02

#define BROADCAST_POINT_TO_POINT 0x00
#define BROADCAST_ACTIVE_SLAVE 0x01

#define MAX_ACL_DATA_CACHE UINT32_MAX

#pragma pack(1)
typedef struct {
    uint16_t handle : 12;
    uint16_t pbFlag : 2;
    uint16_t bcFlag : 2;
    uint16_t dataTotalLength;
} HciAclDataHeader;

typedef struct {
    uint16_t connectionHandle;
    uint8_t transport;
} HciConnectionHandleBlock;
#pragma pack()

typedef struct {
    uint16_t connectionHandle;
    uint16_t count;
} HciTxPackets;

static uint16_t g_aclDataPacketLength = 0;
static uint16_t g_totalNumAclDataPackets = 0;

static uint16_t g_numOfAclDataPackets = 0;
static List *g_txAclPackets = NULL;
static Mutex *g_numOfAclDataPacketsLock = NULL;

static List *g_aclDataCache = NULL;
static Mutex *g_aclDataCacheLock = NULL;

static bool g_sharedDataBuffers = false;
static uint16_t g_leAclDataPacketLength = 0;
static uint8_t g_totalNumLeDataPackets = 0;

static uint8_t g_numOfLeDataPackets = 0;
static List *g_txLePackets = NULL;
static Mutex *g_numOfLeDataPacketsLock = NULL;

static List *g_leAclDataCache = NULL;
static Mutex *g_leAclDataCacheLock = NULL;

static List *g_hciAclCallbackList = NULL;
static Mutex *g_hciAclCallbackListLock = NULL;

static List *g_connectionHandleList = NULL;
static Mutex *g_connectionHandleListLock = NULL;

static HciConnectionHandleBlock *HciAllocConnectionHandleBlock(uint16_t connectionHandle, uint8_t transport)
{
    HciConnectionHandleBlock *block = MEM_MALLOC.alloc(sizeof(HciConnectionHandleBlock));
    if (block != NULL) {
        block->connectionHandle = connectionHandle;
        block->transport = transport;
    }
    return block;
}

static void HciFreeConnectionHandleBlock(void *block)
{
    MEM_MALLOC.free(block);
}

static HciConnectionHandleBlock *HciFindConnectionHandleBlock(uint16_t connectionHandle)
{
    HciConnectionHandleBlock *block = NULL;

    ListNode *node = ListGetFirstNode(g_connectionHandleList);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (block->connectionHandle == connectionHandle) {
            break;
        } else {
            block = NULL;
        }
        node = ListGetNextNode(node);
    }

    return block;
}

static uint8_t HciAclGetTransport(uint16_t connectionHandle)
{
    uint8_t transport = -1;

    HciConnectionHandleBlock *block = NULL;

    MutexLock(g_connectionHandleListLock);

    block = HciFindConnectionHandleBlock(connectionHandle);
    if (block != NULL) {
        transport = block->transport;
    }

    MutexUnlock(g_connectionHandleListLock);

    return transport;
}

static HciTxPackets *AllocTxPacketsEntity(uint16_t connectionHandle)
{
    HciTxPackets *entity = MEM_CALLOC.alloc(sizeof(HciTxPackets));
    if (entity != NULL) {
        entity->connectionHandle = connectionHandle;
    }
    return entity;
}

static void FreeTxPacketsEntity(void *packet)
{
    MEM_CALLOC.free(packet);
}

void HciInitAcl()
{
    g_hciAclCallbackList = ListCreate(NULL);
    g_hciAclCallbackListLock = MutexCreate();

    g_numOfAclDataPacketsLock = MutexCreate();
    g_aclDataCache = ListCreate(NULL);
    g_aclDataCacheLock = MutexCreate();

    g_numOfLeDataPacketsLock = MutexCreate();
    g_leAclDataCache = ListCreate(NULL);
    g_leAclDataCacheLock = MutexCreate();

    g_connectionHandleList = ListCreate(HciFreeConnectionHandleBlock);
    g_connectionHandleListLock = MutexCreate();

    g_txAclPackets = ListCreate(FreeTxPacketsEntity);
    g_txLePackets = ListCreate(FreeTxPacketsEntity);
}

static void HciCleanupCallback()
{
    if (g_hciAclCallbackList != NULL) {
        ListDelete(g_hciAclCallbackList);
        g_hciAclCallbackList = NULL;
    }
    if (g_hciAclCallbackListLock != NULL) {
        MutexDelete(g_hciAclCallbackListLock);
        g_hciAclCallbackListLock = NULL;
    }
}

static void HciCleanupAclData()
{
    if (g_numOfAclDataPacketsLock != NULL) {
        MutexDelete(g_numOfAclDataPacketsLock);
        g_numOfAclDataPacketsLock = NULL;
    }
    if (g_aclDataCache != NULL) {
        ListNode *node = ListGetFirstNode(g_aclDataCache);
        while (node != NULL) {
            PacketFree(ListGetNodeData(node));
            node = ListGetNextNode(node);
        }
        ListDelete(g_aclDataCache);
        g_aclDataCache = NULL;
    }
    if (g_aclDataCacheLock != NULL) {
        MutexDelete(g_aclDataCacheLock);
        g_aclDataCacheLock = NULL;
    }
}

static void HciCleanupLeAclData()
{
    if (g_numOfLeDataPacketsLock != NULL) {
        MutexDelete(g_numOfLeDataPacketsLock);
        g_numOfLeDataPacketsLock = NULL;
    }
    if (g_leAclDataCache != NULL) {
        ListNode *node = ListGetFirstNode(g_leAclDataCache);
        while (node != NULL) {
            PacketFree(ListGetNodeData(node));
            node = ListGetNextNode(node);
        }
        ListDelete(g_leAclDataCache);
        g_leAclDataCache = NULL;
    }
    if (g_leAclDataCacheLock != NULL) {
        MutexDelete(g_leAclDataCacheLock);
        g_leAclDataCacheLock = NULL;
    }
}

static void HciCleanupAclHandle()
{
    if (g_connectionHandleList != NULL) {
        ListDelete(g_connectionHandleList);
        g_connectionHandleList = NULL;
    }
    if (g_connectionHandleListLock != NULL) {
        MutexDelete(g_connectionHandleListLock);
        g_connectionHandleListLock = NULL;
    }
}

static HciTxPackets *FindTxPacketsEntityByConnectionHandle(uint16_t connectionHandle)
{
    HciTxPackets *entity = NULL;
    ListNode *node = ListGetFirstNode(g_txAclPackets);
    while (node != NULL) {
        entity = (HciTxPackets *)ListGetNodeData(node);
        if (entity->connectionHandle == connectionHandle) {
            break;
        } else {
            entity = NULL;
        }
        node = ListGetNextNode(node);
    }
    return entity;
}

static HciTxPackets *FindLeTxPacketsEntityByConnectionHandle(uint16_t connectionHandle)
{
    HciTxPackets *entity = NULL;
    ListNode *node = ListGetFirstNode(g_txLePackets);
    while (node != NULL) {
        entity = (HciTxPackets *)ListGetNodeData(node);
        if (entity->connectionHandle == connectionHandle) {
            break;
        } else {
            entity = NULL;
        }
        node = ListGetNextNode(node);
    }
    return entity;
}

static void HciAddTxAclPacket(uint16_t connectionHandle)
{
    HciTxPackets *entity = FindTxPacketsEntityByConnectionHandle(connectionHandle);
    if (entity == NULL) {
        entity = AllocTxPacketsEntity(connectionHandle);
        ListAddLast(g_txAclPackets, entity);
    }

    if (entity != NULL) {
        entity->count++;
    }
}

static void HciOnAclPacketComplete(uint16_t connectionHandle, uint16_t count)
{
    HciTxPackets *entity = FindTxPacketsEntityByConnectionHandle(connectionHandle);
    if (entity != NULL) {
        entity->count -= count;
    }
}

static void HciAddTxLePacket(uint16_t connectionHandle)
{
    HciTxPackets *entity = FindLeTxPacketsEntityByConnectionHandle(connectionHandle);
    if (entity == NULL) {
        entity = AllocTxPacketsEntity(connectionHandle);
        ListAddLast(g_txLePackets, entity);
    }

    if (entity != NULL) {
        entity->count++;
    }
}

static void HciOnLePacketComplete(uint16_t connectionHandle, uint16_t count)
{
    HciTxPackets *entity = FindLeTxPacketsEntityByConnectionHandle(connectionHandle);
    if (entity != NULL) {
        entity->count -= count;
    }
}

static uint16_t HciGetAclHandleFromPacket(const Packet *packet)
{
    uint16_t handle = 0;
    Buffer *headerBuffer = PacketHead(packet);
    HciAclDataHeader *header = BufferPtr(headerBuffer);
    if (header != NULL) {
        handle = header->handle;
    }
    return handle;
}

void HciCloseAcl()
{
    if (g_txAclPackets != NULL) {
        ListDelete(g_txAclPackets);
        g_txAclPackets = NULL;
    }
    if (g_txLePackets != NULL) {
        ListDelete(g_txLePackets);
        g_txLePackets = NULL;
    }

    HciCleanupCallback();
    HciCleanupAclData();
    HciCleanupLeAclData();
    HciCleanupAclHandle();
}

void HCI_SetBufferSize(uint16_t packetLength, uint16_t totalPackets)
{
    g_aclDataPacketLength = packetLength;
    g_totalNumAclDataPackets = totalPackets;

    MutexLock(g_numOfAclDataPacketsLock);
    g_numOfAclDataPackets = g_totalNumAclDataPackets;
    MutexUnlock(g_numOfAclDataPacketsLock);
}

void HCI_SetLeBufferSize(uint16_t packetLength, uint8_t totalPackets)
{
    if (totalPackets == 0) {
        g_sharedDataBuffers = true;
    } else {
        g_sharedDataBuffers = false;
    }

    g_leAclDataPacketLength = packetLength;
    g_totalNumLeDataPackets = totalPackets;

    MutexLock(g_numOfLeDataPacketsLock);
    g_numOfLeDataPackets = g_totalNumLeDataPackets;
    MutexUnlock(g_numOfLeDataPacketsLock);
}

int HCI_RegisterAclCallbacks(const HciAclCallbacks *callbacks)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }
    MutexLock(g_hciAclCallbackListLock);
    ListAddLast(g_hciAclCallbackList, (void *)callbacks);
    MutexUnlock(g_hciAclCallbackListLock);

    return BT_NO_ERROR;
}

int HCI_DeregisterAclCallbacks(const HciAclCallbacks *callbacks)
{
    if (callbacks == NULL) {
        return BT_BAD_PARAM;
    }
    MutexLock(g_hciAclCallbackListLock);
    ListRemoveNode(g_hciAclCallbackList, (void *)callbacks);
    MutexUnlock(g_hciAclCallbackListLock);

    return BT_NO_ERROR;
}

static int HciAclPushToTxQueue(Packet *packet)
{
    int reuslt = BT_NO_ERROR;
    HciPacket *hciPacket = MEM_MALLOC.alloc(sizeof(HciPacket));
    if (hciPacket != NULL) {
        hciPacket->type = H2C_ACLDATA;
        hciPacket->packet = packet;
        HciPushToTxQueue(hciPacket);
    } else {
        reuslt = BT_NO_MEMORY;
    }
    return reuslt;
}

static Packet **HciFargment(uint16_t handle, uint8_t flushable, Packet *packet, uint16_t frameLength, int *frameCount)
{
    if (frameLength == 0) {
        return NULL;
    }

    size_t totalLength = PacketSize(packet);

    int count = (totalLength / frameLength) + ((totalLength % frameLength) ? 1 : 0);
    Packet *fargmented = NULL;
    Buffer *headerBuffer = NULL;
    HciAclDataHeader *header = NULL;
    Packet **fargmentedPackets = MEM_MALLOC.alloc(sizeof(Packet *) * count);
    if (fargmentedPackets == NULL) {
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        fargmented = PacketMalloc(sizeof(HciAclDataHeader), 0, 0);
        PacketFragment(packet, fargmented, frameLength);
        headerBuffer = PacketHead(fargmented);
        header = BufferPtr(headerBuffer);
        if (header != NULL) {
            header->handle = handle;
            if (i == 0) {
                header->pbFlag = (flushable == NON_FLUSHABLE_PACKET) ?
                    PACKET_BOUNDARY_FIRST_NON_FLUSHABLE : PACKET_BOUNDARY_FIRST_FLUSHABLE;
            } else {
                header->pbFlag = PACKET_BOUNDARY_CONTINUING;
            }
            header->bcFlag = BROADCAST_POINT_TO_POINT;
            header->dataTotalLength = PacketPayloadSize(fargmented);
            header = NULL;
        }
        headerBuffer = NULL;
        fargmentedPackets[i] = fargmented;
        fargmented = NULL;
    }
    *frameCount = count;

    return fargmentedPackets;
}

static int HciSendSinglePacket(uint16_t connectionHandle, Packet *packet)
{
    int result = BT_NO_ERROR;
    if (g_numOfAclDataPackets > 0) {
        result = HciAclPushToTxQueue(packet);
        if (result == BT_NO_ERROR) {
            g_numOfAclDataPackets--;
            HciAddTxAclPacket(connectionHandle);
        }
    } else {
        MutexLock(g_aclDataCacheLock);
        ListAddLast(g_aclDataCache, packet);
        MutexUnlock(g_aclDataCacheLock);
    }
    return result;
}

static int HciFargmentAndSendData(uint16_t handle, uint8_t flushable, Packet *packet)
{
    int result = BT_NO_ERROR;

    size_t totalLength = PacketSize(packet);
    if (totalLength > g_aclDataPacketLength) {
        int count = 0;
        Packet **fargmentedPackets = HciFargment(handle, flushable, packet, g_aclDataPacketLength, &count);
        if (fargmentedPackets != NULL) {
            MutexLock(g_numOfAclDataPacketsLock);
            for (int i = 0; i < count; i++) {
                HciSendSinglePacket(handle, fargmentedPackets[i]);
            }
            MutexUnlock(g_numOfAclDataPacketsLock);

            MEM_MALLOC.free(fargmentedPackets);
        } else {
            result = BT_NO_MEMORY;
        }
    } else {
        Packet *aclPacket = PacketInheritMalloc(packet, sizeof(HciAclDataHeader), 0);
        Buffer *headerBuffer = PacketHead(aclPacket);
        HciAclDataHeader *header = BufferPtr(headerBuffer);
        if (header != NULL) {
            header->handle = handle;
            header->pbFlag = (flushable == NON_FLUSHABLE_PACKET) ?
                PACKET_BOUNDARY_FIRST_NON_FLUSHABLE : PACKET_BOUNDARY_FIRST_FLUSHABLE;
            header->bcFlag = BROADCAST_POINT_TO_POINT;
            header->dataTotalLength = PacketPayloadSize(aclPacket);
        }
        MutexLock(g_numOfAclDataPacketsLock);
        result = HciSendSinglePacket(handle, aclPacket);
        MutexUnlock(g_numOfAclDataPacketsLock);
    }

    return result;
}

static int HciSendLeSinglePacket(uint16_t connectionHandle, Packet *packet)
{
    int result = BT_NO_ERROR;
    if (g_numOfLeDataPackets > 0) {
        result = HciAclPushToTxQueue(packet);
        if (result == BT_NO_ERROR) {
            g_numOfLeDataPackets--;
            HciAddTxLePacket(connectionHandle);
        }
    } else {
        MutexLock(g_leAclDataCacheLock);
        ListAddLast(g_leAclDataCache, packet);
        MutexUnlock(g_leAclDataCacheLock);
    }
    return result;
}

static int HciFargmentAndSendLeData(uint16_t handle, uint8_t flushable, Packet *packet)
{
    int result = BT_NO_ERROR;

    size_t totalLength = PacketSize(packet);
    if (totalLength > g_leAclDataPacketLength) {
        int count = 0;
        Packet **fargmentedPackets = HciFargment(handle, flushable, packet, g_leAclDataPacketLength, &count);
        if (fargmentedPackets != NULL) {
            MutexLock(g_numOfLeDataPacketsLock);
            for (int i = 0; i < count; i++) {
                HciSendLeSinglePacket(handle, fargmentedPackets[i]);
            }
            MutexUnlock(g_numOfLeDataPacketsLock);

            MEM_MALLOC.free(fargmentedPackets);
        } else {
            result = BT_NO_MEMORY;
        }
    } else {
        Packet *aclPacket = PacketInheritMalloc(packet, sizeof(HciAclDataHeader), 0);
        Buffer *headerBuffer = PacketHead(aclPacket);
        HciAclDataHeader *header = BufferPtr(headerBuffer);
        if (header != NULL) {
            header->handle = handle;
            header->pbFlag = (flushable == NON_FLUSHABLE_PACKET) ?
                PACKET_BOUNDARY_FIRST_NON_FLUSHABLE : PACKET_BOUNDARY_FIRST_FLUSHABLE;
            header->bcFlag = BROADCAST_POINT_TO_POINT;
            header->dataTotalLength = PacketPayloadSize(aclPacket);
        }
        MutexLock(g_numOfLeDataPacketsLock);
        result = HciSendLeSinglePacket(handle, aclPacket);
        MutexUnlock(g_numOfLeDataPacketsLock);
    }

    return result;
}

int HCI_SendAclData(uint16_t handle, uint8_t flushable, Packet *packet)
{
    HciConnectionHandleBlock *block = NULL;

    uint8_t transport = 0;

    MutexLock(g_connectionHandleListLock);

    block = HciFindConnectionHandleBlock(handle);
    if (block != NULL) {
        transport = block->transport;
    }

    int result;

    switch (transport) {
        case TRANSPORT_BREDR:
            result = HciFargmentAndSendData(handle, flushable, packet);
            break;
        case TRANSPORT_LE:
            if (g_sharedDataBuffers) {
                result = HciFargmentAndSendData(handle, flushable, packet);
            } else {
                result = HciFargmentAndSendLeData(handle, flushable, packet);
            }
            break;
        default:
            result = BT_OPERATION_FAILED;
            break;
    }

    MutexUnlock(g_connectionHandleListLock);

    return result;
}

void HciOnAclData(Packet *packet)
{
    HciAclDataHeader header;
    PacketExtractHead(packet, (uint8_t *)&header, sizeof(header));

    MutexLock(g_hciAclCallbackListLock);

    HciAclCallbacks *callback = NULL;
    ListNode *node = ListGetFirstNode(g_hciAclCallbackList);
    while (node != NULL) {
        callback = ListGetNodeData(node);
        if (callback != NULL) {
            if (callback->onAclData != NULL) {
                callback->onAclData(header.handle, header.pbFlag & 0x3, header.bcFlag & 0x3, packet);
            }
        }
        node = ListGetNextNode(node);
    }

    MutexUnlock(g_hciAclCallbackListLock);
}

static void HciSendCachedAclPackets()
{
    Packet *packet = NULL;
    ListNode *node = NULL;

    while (g_numOfAclDataPackets > 0) {
        MutexLock(g_aclDataCacheLock);
        node = ListGetFirstNode(g_aclDataCache);
        if (node != NULL) {
            packet = ListGetNodeData(node);
            ListRemoveNode(g_aclDataCache, packet);
        } else {
            packet = NULL;
        }
        MutexUnlock(g_aclDataCacheLock);
        if (packet != NULL) {
            uint16_t handle = HciGetAclHandleFromPacket(packet);
            int result = HciAclPushToTxQueue(packet);
            if (result == BT_NO_ERROR) {
                g_numOfAclDataPackets--;
                HciAddTxAclPacket(handle);
            } else {
                PacketFree(packet);
            }
        } else {
            break;
        }
    }
}

static void HciAclOnPacketCompleted(const HciNumberOfCompletedPackets *completedPackets)
{
    MutexLock(g_numOfAclDataPacketsLock);
    g_numOfAclDataPackets += completedPackets->numOfCompletedPackets;
    HciOnAclPacketComplete(completedPackets->connectionHandle, completedPackets->numOfCompletedPackets);

    HciSendCachedAclPackets();

    MutexUnlock(g_numOfAclDataPacketsLock);
}

static void HciSendCachedLePackets()
{
    Packet *packet = NULL;
    ListNode *node = NULL;

    while (g_numOfLeDataPackets > 0) {
        MutexLock(g_leAclDataCacheLock);
        node = ListGetFirstNode(g_leAclDataCache);
        if (node != NULL) {
            packet = ListGetNodeData(node);
            ListRemoveNode(g_leAclDataCache, packet);
        } else {
            packet = NULL;
        }
        MutexUnlock(g_leAclDataCacheLock);
        if (packet != NULL) {
            uint16_t handle = HciGetAclHandleFromPacket(packet);
            int result = HciAclPushToTxQueue(packet);
            if (result == BT_NO_ERROR) {
                g_numOfLeDataPackets--;
                HciAddTxLePacket(handle);
            } else {
                PacketFree(packet);
            }
        } else {
            break;
        }
    }
}

static void HciAclOnLePacketCompleted(const HciNumberOfCompletedPackets *completedPackets)
{
    if (g_sharedDataBuffers) {
        HciAclOnPacketCompleted(completedPackets);
    } else {
        MutexLock(g_numOfLeDataPacketsLock);
        g_numOfLeDataPackets += completedPackets->numOfCompletedPackets;
        HciOnLePacketComplete(completedPackets->connectionHandle, completedPackets->numOfCompletedPackets);

        HciSendCachedLePackets();

        MutexUnlock(g_numOfLeDataPacketsLock);
    }
}

void HciAclOnNumberOfCompletedPacket(uint8_t numberOfHandles, const HciNumberOfCompletedPackets *list)
{
    for (int i = 0; i < numberOfHandles; i++) {
        uint8_t transport = HciAclGetTransport(list[i].connectionHandle);
        switch (transport) {
            case TRANSPORT_BREDR:
                HciAclOnPacketCompleted(&list[i]);
                break;
            case TRANSPORT_LE:
                HciAclOnLePacketCompleted(&list[i]);
                break;
            default:
                break;
        }
    }
}

void HciAclOnConnectionComplete(uint16_t connectionHandle, uint8_t transport)
{
    HciConnectionHandleBlock *block = HciAllocConnectionHandleBlock(connectionHandle, transport);
    if (block != NULL) {
        MutexLock(g_connectionHandleListLock);
        ListAddLast(g_connectionHandleList, block);
        MutexUnlock(g_connectionHandleListLock);
    }
}

static void HciCleanupCacheByAclHandle(uint16_t connectionHandle)
{
    Packet *packet = NULL;
    Buffer *buffer = NULL;
    HciAclDataHeader *header = NULL;
    ListNode *node = NULL;

    MutexLock(g_aclDataCacheLock);

    node = ListGetFirstNode(g_aclDataCache);
    while (node != NULL) {
        packet = ListGetNodeData(node);
        node = ListGetNextNode(node);
        buffer = PacketHead(packet);
        if (buffer != NULL) {
            header = BufferPtr(buffer);
            if ((header != NULL) && (header->handle == connectionHandle)) {
                ListRemoveNode(g_aclDataCache, packet);
                PacketFree(packet);
            }
        }
    }

    MutexUnlock(g_aclDataCacheLock);
}

static void HciCleanupLeCacheByAclHandle(uint16_t connectionHandle)
{
    Packet *packet = NULL;
    Buffer *buffer = NULL;
    HciAclDataHeader *header = NULL;
    ListNode *node = NULL;

    MutexLock(g_leAclDataCacheLock);

    node = ListGetFirstNode(g_leAclDataCache);
    while (node != NULL) {
        packet = ListGetNodeData(node);
        node = ListGetNextNode(node);
        buffer = PacketHead(packet);
        if (buffer != NULL) {
            header = BufferPtr(buffer);
            if (header != NULL && header->handle == connectionHandle) {
                ListRemoveNode(g_leAclDataCache, packet);
                PacketFree(packet);
            }
        }
    }

    MutexUnlock(g_leAclDataCacheLock);
}

static void HciAclCleanCacheOnDisconnect(uint16_t connectionHandle, uint16_t transport)
{
    if (transport == TRANSPORT_BREDR) {
        HciCleanupCacheByAclHandle(connectionHandle);
    } else if (transport == TRANSPORT_LE) {
        if (g_sharedDataBuffers) {
            HciCleanupCacheByAclHandle(connectionHandle);
        } else {
            HciCleanupLeCacheByAclHandle(connectionHandle);
        }
    }
}

static void HciRecoveryNumOfAclDataPackets(uint16_t connectionHandle)
{
    MutexLock(g_numOfAclDataPacketsLock);

    HciTxPackets *entity = FindTxPacketsEntityByConnectionHandle(connectionHandle);
    if (entity != NULL) {
        g_numOfAclDataPackets += entity->count;
        ListRemoveNode(g_txAclPackets, entity);
    }

    MutexUnlock(g_numOfAclDataPacketsLock);
}

static void HciRecoveryNumOfLeDataPackets(uint16_t connectionHandle)
{
    MutexLock(g_numOfLeDataPacketsLock);

    HciTxPackets *entity = FindLeTxPacketsEntityByConnectionHandle(connectionHandle);
    if (entity != NULL) {
        g_numOfLeDataPackets += entity->count;
        ListRemoveNode(g_txAclPackets, entity);
    }

    MutexUnlock(g_numOfLeDataPacketsLock);
}

static void HciAclOnDisconnect(uint16_t connectionHandle)
{
    HciConnectionHandleBlock *block = NULL;

    uint8_t transport = 0;

    MutexLock(g_connectionHandleListLock);

    block = HciFindConnectionHandleBlock(connectionHandle);
    if (block != NULL) {
        transport = block->transport;
        ListRemoveNode(g_connectionHandleList, block);
    }

    MutexUnlock(g_connectionHandleListLock);

    HciAclCleanCacheOnDisconnect(connectionHandle, transport);

    if (transport == TRANSPORT_BREDR) {
        HciRecoveryNumOfAclDataPackets(connectionHandle);
    } else if (transport == TRANSPORT_LE) {
        if (g_sharedDataBuffers) {
            HciRecoveryNumOfAclDataPackets(connectionHandle);
        } else {
            HciRecoveryNumOfLeDataPackets(connectionHandle);
        }
    }
}

void HciAclOnDisconnectStatus(uint16_t connectionHandle)
{
    HciAclOnDisconnect(connectionHandle);
}

void HciAclOnDisconnectComplete(uint16_t connectionHandle)
{
    HciAclOnDisconnect(connectionHandle);
}