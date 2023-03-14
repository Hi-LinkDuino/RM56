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

#include "packet.h"
#include "buffer.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "platform/include/platform_def.h"
#include "securec.h"
#include "log.h"

typedef struct Payload {
    struct Payload *next;
    struct Payload *prev;
    Buffer *buf;
} Payload;

typedef struct Packet {
    Payload *head;
    Payload *tail;
    Payload *payload;
} PacketInternal;

static inline Payload *PayloadNew(uint32_t size)
{
    Payload *payload = (Payload *)calloc(1, sizeof(Payload));
    if (payload == NULL) {
        return NULL;
    }
    payload->buf = BufferMalloc(size);
    return payload;
}

static inline Payload *PayloadNewRef(const Buffer *buf)
{
    Payload *payload = (Payload *)calloc(1, sizeof(Payload));
    if (payload == NULL) {
        return NULL;
    }
    payload->buf = BufferRefMalloc(buf);
    return payload;
}

static inline void PayloadFree(Payload *payload)
{
    if (payload == NULL) {
        return;
    }
    BufferFree(payload->buf);
    free(payload);
}

Packet *PacketMalloc(uint16_t headSize, uint16_t tailSize, uint32_t payloadSize)
{
    Packet *packet = (Packet *)calloc(1, sizeof(Packet));
    if (packet == NULL) {
        return NULL;
    }
    packet->head = PayloadNew(headSize);
    packet->tail = PayloadNew(tailSize);
    packet->payload = PayloadNew(payloadSize);

    if ((packet->head != NULL) && (packet->tail != NULL) && (packet->payload != NULL)) {
        packet->head->next = packet->payload;
        packet->payload->prev = packet->head;

        packet->payload->next = packet->tail;
        packet->tail->prev = packet->payload;
    }
    return packet;
}

Packet *PacketRefMalloc(const Packet *pkt)
{
    Packet *refPacket = (Packet *)calloc(1, sizeof(Packet));
    if (refPacket == NULL) {
        return NULL;
    }
    refPacket->head = PayloadNewRef(pkt->head->buf);
    refPacket->tail = PayloadNewRef(pkt->tail->buf);

    if ((refPacket->head != NULL) && (refPacket->tail != NULL)) {
        Payload *srcPayload = pkt->payload;
        Payload *refPayload = refPacket->head;

        while (srcPayload != pkt->tail) {
            Payload *new = PayloadNewRef(srcPayload->buf);
            if (new != NULL) {
                refPayload->next = new;
                new->prev = refPayload;
                refPayload = new;
                srcPayload = srcPayload->next;
            }
        }

        refPayload->next = refPacket->tail;
        refPacket->tail->prev = refPayload;

        refPacket->payload = refPacket->head->next;
        refPacket->payload->prev = refPacket->head;
    }
    return refPacket;
}

Packet *PacketInheritMalloc(const Packet *pkt, uint16_t headSize, uint16_t tailSize)
{
    Packet *inheritPacket = PacketRefMalloc(pkt);
    if (inheritPacket != NULL) {
        Payload *tempTail = inheritPacket->tail;

        inheritPacket->payload = inheritPacket->head;
        inheritPacket->head = PayloadNew(headSize);
        if (inheritPacket->head != NULL) {
            inheritPacket->head->next = inheritPacket->payload;
            inheritPacket->payload->prev = inheritPacket->head;
        }
        inheritPacket->tail = PayloadNew(tailSize);
        if (inheritPacket->tail != NULL) {
            tempTail->next = inheritPacket->tail;
            inheritPacket->tail->prev = tempTail;
        }
    }
    return inheritPacket;
}

void PacketPayloadAddLast(const Packet *pkt, const Buffer *buf)
{
    Payload *last = pkt->payload;
    while (last != NULL && last->next != pkt->tail) {
        last = last->next;
    }

    if (last != NULL) {
        if (last->buf == NULL) {
            last->buf = BufferRefMalloc(buf);
        } else {
            Payload *newLast = PayloadNewRef(buf);
            if (newLast != NULL) {
                newLast->next = pkt->tail;
                pkt->tail->prev = newLast;
                last->next = newLast;
                newLast->prev = last;
            }
        }
    }
}

void PacketFree(Packet *pkt)
{
    if (pkt == NULL) {
        return;
    }

    Payload *node = pkt->head;
    while (node != NULL) {
        Payload *tempNode = node;
        node = node->next;
        PayloadFree(tempNode);
    }
    free(pkt);
}

Buffer *PacketHead(const Packet *pkt)
{
    ASSERT(pkt);
    return pkt->head->buf;
}

Buffer *PacketTail(const Packet *pkt)
{
    ASSERT(pkt);
    return pkt->tail->buf;
}

Buffer *PacketContinuousPayload(Packet *pkt)
{
    ASSERT(pkt);
    if (pkt->payload == NULL) {
        return NULL;
    }
    if (pkt->payload->next == pkt->tail) {
        return pkt->payload->buf;
    }

    uint32_t payloadSize = PacketPayloadSize(pkt);
    if (payloadSize == 0) {
        return NULL;
    }

    Payload *newPayload = PayloadNew(payloadSize);
    if (newPayload == NULL) {
        return NULL;
    }
    Payload *payloadIter = pkt->payload;
    uint8_t *bufPtr = BufferPtr(newPayload->buf);

    while (payloadIter != pkt->tail) {
        uint32_t bufSize = BufferGetSize(payloadIter->buf);
        if (bufSize > 0) {
            (void)memcpy_s(bufPtr, bufSize, BufferPtr(payloadIter->buf), bufSize);
            bufPtr += bufSize;
        }
        Payload *payloadTemp = payloadIter;
        payloadIter = payloadIter->next;
        PayloadFree(payloadTemp);
    }

    newPayload->next = pkt->tail;
    pkt->tail->prev = newPayload;

    pkt->head->next = newPayload;
    newPayload->prev = pkt->head;

    pkt->payload = newPayload;

    return newPayload->buf;
}

uint32_t PacketPayloadSize(const Packet *pkt)
{
    ASSERT(pkt);
    uint32_t size = 0;
    Payload *node = pkt->payload;
    while (node != pkt->tail) {
        size += BufferGetSize(node->buf);
        node = node->next;
    }
    return size;
}

static Payload *PacketMoveToOffset(const Packet *pkt, Payload *start, const Payload *end, uint32_t *offset)
{
    uint32_t pktSize = PacketSize(pkt);
    if (*offset > pktSize) {
        return NULL;
    }
    uint32_t currOffset = 0;
    while (BufferGetSize(start->buf) + currOffset <= *offset) {
        if (start == end) {
            return 0;
        } else {
            currOffset += BufferGetSize(start->buf);
            start = start->next;
        }
    }

    *offset = *offset - currOffset;
    return start;
}

static uint32_t PacketCopyToBuffer(Payload *start, const Payload *end, uint8_t *buffer, uint32_t offset, uint32_t size)
{
    uint32_t retSize;
    uint32_t remain = BufferGetSize(start->buf) - offset;
    if (remain >= size) {
        if (memcpy_s(buffer, size, (uint8_t *)BufferPtr(start->buf) + offset, size) != EOK) {
            LOG_ERROR("PacketCopyToBuffer, memcpy_s fail");
            return 0;
        }
        return size;
    } else {
        retSize = remain;
        (void)memcpy_s(buffer, remain, (uint8_t *)BufferPtr(start->buf) + offset, remain);

        buffer += remain;
        remain = size - remain;
        start = start->next;

        for (; start != end; start = start->next) {
            if (remain == 0) {
                break;
            }
            uint32_t buffSize = BufferGetSize(start->buf);
            uint32_t copySize = ((remain <= buffSize) ? remain : buffSize);
            (void)memcpy_s(buffer, copySize, (uint8_t *)BufferPtr(start->buf), copySize);
            retSize += copySize;
            buffer += copySize;
            remain -= copySize;
        }
    }
    return retSize;
}

static uint32_t PacketCopyFromBuffer(
    Payload *start, const Payload *end, const uint8_t *buffer, uint32_t offset, uint32_t size)
{
    uint32_t retSize;
    uint32_t remain = BufferGetSize(start->buf) - offset;
    if (remain >= size) {
        if (memcpy_s((uint8_t *)BufferPtr(start->buf) + offset, size, buffer, size) != EOK) {
            LOG_ERROR("PacketCopyFromBuffer, memcpy_s fail");
            return 0;
        }
        return size;
    } else {
        retSize = remain;
        (void)memcpy_s((uint8_t *)BufferPtr(start->buf) + offset, size, buffer, remain);

        buffer += remain;
        remain = size - remain;
        start = start->next;

        for (; start != end; start = start->next) {
            if (remain == 0) {
                break;
            }
            uint32_t buffSize = BufferGetSize(start->buf);
            uint32_t copySize = ((remain <= buffSize) ? remain : buffSize);
            (void)memcpy_s((uint8_t *)BufferPtr(start->buf), copySize, buffer, copySize);
            retSize += copySize;
            buffer += copySize;
            remain -= copySize;
        }
    }
    return retSize;
}

uint32_t PacketPayloadRead(const Packet *pkt, uint8_t *dst, uint32_t offset, uint32_t size)
{
    ASSERT(pkt);
    ASSERT(dst);
    if (size == 0) {
        return 0;
    }
    Payload *start = pkt->payload;
    Payload *end = pkt->tail;
    start = PacketMoveToOffset(pkt, start, end, &offset);
    if (start == NULL) {
        return 0;
    }

    return PacketCopyToBuffer(start, end, dst, offset, size);
}

uint32_t PacketPayloadWrite(const Packet *pkt, const uint8_t *src, uint32_t offset, uint32_t size)
{
    ASSERT(pkt);
    ASSERT(src);
    if (size == 0) {
        return 0;
    }
    Payload *start = pkt->payload;
    Payload *end = pkt->tail;
    start = PacketMoveToOffset(pkt, start, end, &offset);
    if (start == NULL) {
        return 0;
    }

    return PacketCopyFromBuffer(start, end, src, offset, size);
}

uint32_t PacketSize(const Packet *pkt)
{
    ASSERT(pkt);
    uint32_t size = 0;
    Payload *node = pkt->head;
    while (node != NULL) {
        size += BufferGetSize(node->buf);
        node = node->next;
    }
    return size;
}

uint32_t PacketRead(const Packet *pkt, uint8_t *dst, uint32_t offset, uint32_t size)
{
    ASSERT(pkt);
    ASSERT(dst);
    if (size == 0) {
        return 0;
    }
    Payload *start = pkt->head;
    Payload *end = NULL;
    start = PacketMoveToOffset(pkt, start, end, &offset);
    if (start == NULL) {
        return 0;
    }

    return PacketCopyToBuffer(start, end, dst, offset, size);
}

void PacketExtractHead(Packet *pkt, uint8_t *data, uint32_t size)
{
    ASSERT(pkt);
    ASSERT(data);
    if (size > PacketPayloadSize(pkt)) {
        return;
    }

    Payload *first = pkt->payload;
    while (BufferGetSize(first->buf) < size) {
        uint32_t buffSize = BufferGetSize(first->buf);
        (void)memcpy_s(data, buffSize, (uint8_t *)BufferPtr(first->buf), buffSize);
        size -= buffSize;
        data += buffSize;
        Payload *tempFirst = first;
        first = first->next;
        PayloadFree(tempFirst);
    }
    (void)memcpy_s(data, size, (uint8_t *)BufferPtr(first->buf), size);
    first->buf = BufferResize(first->buf, size, BufferGetSize(first->buf) - size);
    pkt->payload = first;
    pkt->head->next = pkt->payload;
    pkt->payload->prev = pkt->head;
}

void PacketExtractTail(const Packet *pkt, uint8_t *data, uint32_t size)
{
    ASSERT(pkt);
    ASSERT(data);
    if (size > PacketPayloadSize(pkt)) {
        return;
    }

    Payload *last = pkt->tail->prev;
    while (BufferGetSize(last->buf) < size) {
        size_t buffSize = BufferGetSize(last->buf);
        size -= buffSize;
        (void)memcpy_s(data + size, buffSize, (uint8_t *)BufferPtr(last->buf), buffSize);

        struct Payload *tempLast = last;
        last = last->prev;
        PayloadFree(tempLast);
    }
    uint32_t buffSize = BufferGetSize(last->buf);
    (void)memcpy_s(data, size, (uint8_t *)BufferPtr(last->buf) + (buffSize - size), size);
    last->buf = BufferResize(last->buf, 0, buffSize - size);
    last->next = pkt->tail;
    pkt->tail->prev = last;
}

uint32_t PacketFragment(Packet *uplayer, const Packet *downlayer, uint32_t fragLength)
{
    ASSERT(uplayer);
    ASSERT(downlayer);
    if (fragLength == 0) {
        return 0;
    }
    Payload *dFirst = downlayer->payload;
    Payload *first = uplayer->head;
    dFirst->next = NULL;
    while (first != NULL && fragLength > 0) {
        uint32_t buffSize = BufferGetSize(first->buf);
        if (fragLength >= buffSize) {
            dFirst->next = PayloadNewRef(first->buf);
            if (dFirst->next != NULL) {
                dFirst->next->prev = dFirst;
                fragLength -= buffSize;
                Payload *temp = first;
                first = first->next;
                dFirst = dFirst->next;
                uplayer->head = first;
                PayloadFree(temp);
            }
        } else {
            dFirst->next = (Payload *)calloc(1, sizeof(Payload));
            if (dFirst->next != NULL) {
                dFirst->next->prev = dFirst;
                dFirst->next->buf = BufferSliceMalloc(first->buf, 0, fragLength);
                dFirst = dFirst->next;
                first->buf = BufferResize(first->buf, fragLength, buffSize - fragLength);
                fragLength = 0;
                uplayer->head = first;
            }
        }
    }
    if (first == NULL) {
        uplayer->head = NULL;
        uplayer->payload = NULL;
        uplayer->tail = NULL;
        dFirst->next = downlayer->tail;
        downlayer->tail->prev = dFirst;
        return 0;
    }
    dFirst->next = downlayer->tail;
    downlayer->tail->prev = dFirst;
    return PacketSize(uplayer);
}

void PacketAssemble(const Packet *uplayer, const Packet *downlayer)
{
    Packet *refPacket = PacketRefMalloc(downlayer);
    if (refPacket == NULL) {
        LOG_ERROR("Failed to execute func PacketRefMalloc in func PacketAssemble");
        return;
    }
    Payload *upLast = uplayer->payload;
    while ((upLast != NULL) && (upLast->next != uplayer->tail)) {
        upLast = upLast->next;
    }

    if (upLast != NULL && refPacket->payload != NULL) {
        upLast->next = refPacket->payload;
        refPacket->payload->prev = upLast;
    }

    Payload *refLast = refPacket->payload;
    while ((refLast != NULL) && (refLast->next != refPacket->tail)) {
        refLast = refLast->next;
    }

    PayloadFree(refPacket->head);
    refPacket->head = NULL;
    PayloadFree(refPacket->tail);
    refPacket->tail = NULL;

    if (refLast != NULL) {
        refLast->next = uplayer->tail;
        uplayer->tail->prev = refLast;
    }

    free(refPacket);
}

uint16_t PacketCalCrc16(const Packet *pkt, CalCrc16 calCrc16)
{
    uint16_t retVal = 0;
    Payload *iter = pkt->head;
    while (iter != pkt->tail) {
        Buffer *buf = iter->buf;
        uint8_t *ptr = BufferPtr(buf);
        uint32_t bufSize = BufferGetSize(buf);

        while (bufSize) {
            retVal = calCrc16(*ptr, retVal);
            ptr++;
            bufSize--;
        }

        iter = iter->next;
    }

    return retVal;
}

int32_t PacketVerCrc16(const Packet *pkt, CalCrc16 calCrc16, uint16_t crcVal)
{
    uint16_t calVal = 0;
    Payload *iter = pkt->head;
    while (iter != pkt->tail) {
        Buffer *buf = iter->buf;
        uint8_t *ptr = BufferPtr(buf);
        uint32_t bufSize = BufferGetSize(buf);

        while (bufSize) {
            calVal = calCrc16(*ptr, calVal);
            ptr++;
            bufSize--;
        }

        iter = iter->next;
    }

    if (calVal != crcVal) {
        return -1;
    }

    return 0;
}