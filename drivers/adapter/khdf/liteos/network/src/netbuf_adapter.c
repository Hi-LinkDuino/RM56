/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hdf_netbuf.h"
#include "los_memory.h"
#include "lwip/prot/ethernet.h"
#include "net_device.h"
#include "osal.h"

#define HDF_LOG_TAG             NetBuf
#define MAX_CONVERSION_LEN      65535

/**
 * @brief Initializes a network data buffer queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @since 1.0
 */
void NetBufQueueInit(NetBufQueue *q)
{
    if (q == NULL) {
        return;
    }

    DListHeadInit(&q->dlist);
    LOS_SpinInit(&q->lock);
    q->size = 0;
}

/**
 * @brief Obtains the size of a network data buffer queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the size of the network data buffer queue.
 *
 * @since 1.0
 */
uint32_t NetBufQueueSize(const NetBufQueue *q)
{
    if (q == NULL) {
        return 0;
    }

    return q->size;
}

/**
 * @brief Checks whether the network data buffer queue is empty.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns <b>true</b> if the queue is empty; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool NetBufQueueIsEmpty(const NetBufQueue *q)
{
    if (q == NULL) {
        return true;
    }

    return DListIsEmpty(&q->dlist);
}

/*
 * Net buffer is a continuous memory which consists of three parts: head buffer, data buffer and tail buffer.
 *
 *  -----------------------------------------------------
 *  | | | | | | | | | | | | | | | | | | | | | | | | | | |
 *  ^               ^                   ^               ^
 *  |               |                   |               |
 *  |<--head buff-->|<----data buff---->|<--tail buff-->|
 *                  |<----data len----->|
 *  |<---------------------len------------------------->|
 */

/*
 * Enqueue the new net buffer to the tail of buffer queue.
 *
 * @param  : q The pointer to the net buffer queue
 *           nb The new net buffer
 * @return : void
 */
void NetBufQueueEnqueue(NetBufQueue *q, NetBuf *nb)
{
    if (q == NULL || nb == NULL) {
        return;
    }

    uint32_t intSave;
    LOS_SpinLockSave(&q->lock, &intSave);
    DListInsertTail(&nb->dlist, &q->dlist);
    q->size++;
    LOS_SpinUnlockRestore(&q->lock, intSave);
}

/*
 * Enqueue the new net buffer to the head of buffer queue.
 *
 * @param  : q The pointer to the net buffer queue
 *           nb The new net buffer
 * @return : void
 */
void NetBufQueueEnqueueHead(NetBufQueue *q, NetBuf *nb)
{
    if (q == NULL || nb == NULL) {
        return;
    }

    uint32_t intSave;
    LOS_SpinLockSave(&q->lock, &intSave);
    DListInsertHead(&nb->dlist, &q->dlist);
    q->size++;
    LOS_SpinUnlockRestore(&q->lock, intSave);
}

/*
 * Dequeue the first net buffer from net buffer queue.
 *
 * @param  : q The pointer to the net buffer queue
 * @return : The first net buffer in net buffer queue
 */
NetBuf *NetBufQueueDequeue(NetBufQueue *q)
{
    NetBuf *nb = NULL;

    if (q == NULL) {
        return nb;
    }

    uint32_t intSave;
    LOS_SpinLockSave(&q->lock, &intSave);
    if (!DListIsEmpty(&q->dlist)) {
        nb = DLIST_FIRST_ENTRY(&q->dlist, NetBuf, dlist);
        DListRemove(&nb->dlist);
        q->size--;
    }
    LOS_SpinUnlockRestore(&q->lock, intSave);

    return nb;
}

/*
 * Dequeue the last net buffer from net buffer queue.
 *
 * @param  : q The pointer to the net buffer queue
 * @return : The last net buffer in net buffer queue
 */
NetBuf *NetBufQueueDequeueTail(NetBufQueue *q)
{
    NetBuf *nb = NULL;

    if (q == NULL) {
        return nb;
    }

    uint32_t intSave;
    LOS_SpinLockSave(&q->lock, &intSave);
    if (!DListIsEmpty(&q->dlist)) {
        nb = DLIST_LAST_ENTRY(&q->dlist, NetBuf, dlist);
        DListRemove(&nb->dlist);
        q->size--;
    }
    LOS_SpinUnlockRestore(&q->lock, intSave);

    return nb;
}

/**
 * @brief Obtains the network data buffer from the header of a queue, without deleting it from the queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the pointer to the first network data buffer if the queue is not empty;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
NetBuf *NetBufQueueAtHead(const NetBufQueue *q)
{
    if (q == NULL) {
        return NULL;
    }

    return (DListIsEmpty(&q->dlist)) ? NULL : DLIST_FIRST_ENTRY(&q->dlist, NetBuf, dlist);
}

/**
 * @brief Obtains the network data buffer from the tail of a queue, without deleting it from the queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the pointer to the last network data buffer if the queue is not empty;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
NetBuf *NetBufQueueAtTail(const NetBufQueue *q)
{
    if (q == NULL) {
        return NULL;
    }

    return (DListIsEmpty(&q->dlist)) ? NULL : DLIST_LAST_ENTRY(&q->dlist, NetBuf, dlist);
}

/*
 * Clear the net buffer queue and free all net buffer in net buffer queue.
 *
 * @param  : q The pointer to the net buffer queue
 * @return : void
 */
void NetBufQueueClear(NetBufQueue *q)
{
    NetBuf *nb = NULL;

    if (q == NULL) {
        return;
    }

    while ((nb = NetBufQueueDequeue(q)) != NULL) {
        NetBufFree(nb);
    }
}

/*
 * Merge two net buffer queues.
 *
 * @param  : q The pointer to the net buffer queue for added.
 *           add The pointer to the net buffer queue to add.
 * @return : void
 */
void NetBufQueueConcat(NetBufQueue *q, NetBufQueue *add)
{
    if (q == NULL) {
        return;
    }

    if (!NetBufQueueIsEmpty(add)) {
        DListMerge(&add->dlist, &q->dlist);
        q->size += add->size;
        add->size = 0;
    }
}

/*
 * Alloc a net buffer.
 *
 * @param  : size The net buffer size
 * @return : A new net buffer or NULL on fail
 */
NetBuf *NetBufAlloc(uint32_t size)
{
    NetBuf *nb = NULL;
    uint8_t *data = NULL;

#if CACHE_ALIGNED_SIZE
    nb = (NetBuf *)LOS_MemAllocAlign(m_aucSysMem0, sizeof(*nb), CACHE_ALIGNED_SIZE);
#else
    nb = (NetBuf *)LOS_MemAlloc(m_aucSysMem0, sizeof(*nb));
#endif

    if (nb == NULL) {
        HDF_LOGE("%s alloc net buf fail", __func__);
        return NULL;
    }

#if CACHE_ALIGNED_SIZE
    data = (uint8_t *)LOS_MemAllocAlign(m_aucSysMem0, size, CACHE_ALIGNED_SIZE);
#else
    data = (uint8_t *)LOS_MemAlloc(m_aucSysMem0, size);
#endif

    if (data == NULL) {
        HDF_LOGE("%s alloc data fail, size:%u", __func__, size);
        LOS_MemFree(m_aucSysMem0, nb);
        return NULL;
    }

    (void)memset_s(nb, sizeof(NetBuf), 0, sizeof(NetBuf));
    (void)memset_s(data, sizeof(uint8_t), 0, sizeof(uint8_t));

    nb->mem = data;
    nb->len = size;
    nb->dataLen = 0;
    nb->bufs[E_HEAD_BUF].offset = 0;
    nb->bufs[E_HEAD_BUF].len    = 0;
    nb->bufs[E_DATA_BUF].offset = 0;
    nb->bufs[E_DATA_BUF].len    = 0;
    nb->bufs[E_TAIL_BUF].offset = 0;
    nb->bufs[E_TAIL_BUF].len    = size;

    return nb;
}

/*
 * Free a net buffer.
 *
 * @param  : nb A net buffer
 * @return : void
 */
void NetBufFree(NetBuf *nb)
{
    if (nb == NULL) {
        return;
    }

    if (nb->mem != NULL) {
        LOS_MemFree(m_aucSysMem0, nb->mem);
        nb->mem = NULL;
    }

    LOS_MemFree(m_aucSysMem0, nb);
}

/*
 * Pop head room and add to data buffer
 *
 * @param  : nb A net buffer
 *           len The length of remove data
 * @return : The new start address of net buffer data on success or NULL on fail.
 */
void *NetBufPush(NetBuf *nb, uint32_t id, uint32_t len)
{
    struct BufField *headBuf = NULL;
    struct BufField *dataBuf = NULL;
    struct BufField *tailBuf = NULL;

    if (nb == NULL) {
        return NULL;
    }

    headBuf = &nb->bufs[E_HEAD_BUF];
    dataBuf = &nb->bufs[E_DATA_BUF];
    tailBuf = &nb->bufs[E_TAIL_BUF];

    switch (id) {
        case E_HEAD_BUF:
            if (dataBuf->len < len || nb->dataLen < len) {
                HDF_LOGE("%s fail, datasize[%u], dataroom[%u], len[%u]", __func__, nb->dataLen, dataBuf->len, len);
                return NULL;
            }

            headBuf->len += len;
            dataBuf->offset += len;
            dataBuf->len -= len;
            nb->dataLen -= len;
            break;
        case E_DATA_BUF:
            if (tailBuf->len < len) {
                HDF_LOGE("%s fail, tailroom[%u], len[%u]", __func__, tailBuf->len, len);
                return NULL;
            }

            nb->dataLen += len;
            dataBuf->len += len;
            tailBuf->offset += len;
            tailBuf->len -= len;
            break;
        case E_TAIL_BUF:
            if (dataBuf->len < len || nb->dataLen < len || tailBuf->offset < len) {
                HDF_LOGE("%s fail, datasize[%u], netbuflen[%u], tailbufOffset[%u], len[%u]", __func__,
                    dataBuf->len, nb->dataLen, tailBuf->offset, len);
                return NULL;
            }

            dataBuf->len -= len;
            nb->dataLen -= len;
            tailBuf->offset -= len;
            tailBuf->len += len;
            break;
        default:
            break;
    }

    return (nb->mem + dataBuf->offset);
}

/*
 * reduce data buffer and push to head room.
 *
 * @param  : nb A net buffer
 *           len The length of add data
 * @return : The new start address of net buffer data on success or NULL on fail.
 */
void *NetBufPop(NetBuf *nb, uint32_t id, uint32_t len)
{
    struct BufField *headBuf = NULL;
    struct BufField *dataBuf = NULL;
    struct BufField *tailBuf = NULL;

    if (nb == NULL) {
        return NULL;
    }

    headBuf = &nb->bufs[E_HEAD_BUF];
    dataBuf = &nb->bufs[E_DATA_BUF];
    tailBuf = &nb->bufs[E_TAIL_BUF];

    switch (id) {
        case E_HEAD_BUF:
            if (headBuf->len < len || dataBuf->offset < len) {
                HDF_LOGE("%s fail, headroom[%u], len[%u]", __func__, headBuf->len, len);
                return NULL;
            }

            headBuf->len -= len;
            dataBuf->offset -= len;
            dataBuf->len += len;
            nb->dataLen += len;
            break;
        case E_DATA_BUF:
            if (nb->dataLen < len || dataBuf->len < len) {
                HDF_LOGE("%s fail, datasize[%u], dataroom[%u], len[%u]",
                    __func__, nb->dataLen, dataBuf->len, len);
                return NULL;
            }
            headBuf->len += len;
            dataBuf->offset += len;
            dataBuf->len -= len;
            nb->dataLen -= len;
            break;
        case E_TAIL_BUF:
            if (tailBuf->len < len) {
                HDF_LOGE("%s fail, tailroom[%u], len[%u]", __func__, tailBuf->len, len);
                return NULL;
            }
            tailBuf->len -= len;
            tailBuf->offset += len;
            dataBuf->len += len;
            nb->dataLen += len;
            break;
        default:
            break;
    }

    return (nb->mem + dataBuf->offset);
}

/**
 * @brief Obtains the address of a specified buffer segment in a network data buffer.
 *
 * @param nb Indicates the pointer to the network data buffer.
 * @param id Indicates the buffer segment ID.
 *
 * @return Returns the address of the specified buffer segment if the operation is successful;
 * returns <b>NULL</b> if the buffer segment ID is invalid.
 *
 * @since 1.0
 */
uint8_t *NetBufGetAddress(const NetBuf *nb, uint32_t id)
{
    if (nb == NULL) {
        return NULL;
    }

    if (id < MAX_BUF_NUM) {
        return (nb->mem + nb->bufs[id].offset);
    }

    return NULL;
}

/**
 * @brief Obtains the size of a specified buffer segment space in a network data buffer.
 *
 * @param nb Indicates the pointer to the network data buffer.
 * @param id Indicates the buffer segment ID.
 *
 * @return Returns the size of the specified buffer segment space if the operation is successful;
 * returns <b>NULL</b> if the buffer segment ID is invalid.
 *
 * @since 1.0
 */
uint32_t NetBufGetRoom(const NetBuf *nb, uint32_t id)
{
    if (nb == NULL) {
        return 0;
    }

    if (id < MAX_BUF_NUM) {
        return nb->bufs[id].len;
    }

    return 0;
}

/**
 * @brief Obtains the actual data length of the data segment of a network data buffer.
 *
 * @param nb Indicates the pointer to the network data buffer.
 *
 * @return Returns the actual data length of the data segment.
 *
 * @since 1.0
 */
uint32_t NetBufGetDataLen(const NetBuf *nb)
{
    if (nb == NULL) {
        return 0;
    }

    return nb->dataLen;
}

/*
 * Expand a net buffer
 *
 * @param  : nb A net buffer
 *           head The reserved size at head room
 *           tail The reserved size at tail room
 * @return : 0 for success and others for failure
 */
int32_t NetBufResizeRoom(NetBuf *nb, uint32_t head, uint32_t tail)
{
    uint8_t *data = NULL;
    uint32_t size;
    uint32_t offset;

    if (nb == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    size = head + nb->len + tail;
#if CACHE_ALIGNED_SIZE
    data = (uint8_t *)LOS_MemAllocAlign(m_aucSysMem0, size, CACHE_ALIGNED_SIZE);
#else
    data = (uint8_t *)LOS_MemAlloc(m_aucSysMem0, size, CACHE_ALIGNED_SIZE);
#endif
    if (data == NULL) {
        HDF_LOGE("%s mem alloc fail, size:%u", __func__, size);
        return HDF_FAILURE;
    }

    (void)memset_s(data, size, 0, size);
    if (memcpy_s(data + head, size - head, nb->mem, nb->bufs[E_TAIL_BUF].offset) != EOK) {
        HDF_LOGE("%s memcopy failed", __func__);
        return HDF_FAILURE;
    }

    offset = nb->bufs[E_HEAD_BUF].len;

    LOS_MemFree(m_aucSysMem0, nb->mem);
    nb->mem = data;
    nb->len = size;
    nb->bufs[E_HEAD_BUF].offset = 0;
    nb->bufs[E_HEAD_BUF].len = offset + head;
    nb->bufs[E_DATA_BUF].offset = offset + head;
    nb->bufs[E_DATA_BUF].len = nb->dataLen;
    nb->bufs[E_TAIL_BUF].offset += head;
    nb->bufs[E_TAIL_BUF].len += tail;

    return HDF_SUCCESS;
}

/*
 * Concat a net buffer to another net buffer and free the net buffer for concatted.
 *
 * @param  : nb A net buffer
 *           cnb The concat net buffer
 * @return : 0 for success and others for failure
 */
int32_t NetBufConcat(NetBuf *nb, NetBuf *cnb)
{
    uint32_t tailroom;

    if (nb == NULL || cnb == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    tailroom = NetBufGetRoom(nb, E_TAIL_BUF);
    if (tailroom < cnb->dataLen) {
        HDF_LOGE("%s can not concat, tailroom[%u], len[%u]", __func__, tailroom, cnb->len);
        return HDF_FAILURE;
    }

    if (nb->mem != NULL) {
        uint8_t *src = cnb->mem + cnb->bufs[E_DATA_BUF].offset;
        uint8_t *dst = nb->mem + nb->bufs[E_TAIL_BUF].offset;

        if (memcpy_s(dst, tailroom, src, cnb->dataLen) != EOK) {
            return HDF_FAILURE;
        }
        NetBufPush(nb, E_DATA_BUF, cnb->dataLen);
    }

    NetBufFree(cnb);

    return HDF_SUCCESS;
}

/*
 * Convert net buffer to LWIP pbuf.
 *
 * @param  : nb The net buffer
 * @return : pbuf for lwip
 */
struct pbuf *NetBuf2Pbuf(const NetBuf *nb)
{
    struct pbuf *p  = NULL;
    struct eth_hdr *hdr = NULL;
    uint32_t len = NetBufGetDataLen(nb);

    if ((len + ETH_PAD_SIZE) > MAX_CONVERSION_LEN) {
        HDF_LOGE("%s netbuf len exceeds the maximum length of the pbuf!", __func__);
        return NULL;
    }
    p = pbuf_alloc(PBUF_RAW, (uint16_t)(len + ETH_PAD_SIZE), PBUF_RAM);
    if (p == NULL) {
        HDF_LOGE("%s pbuf_alloc failed! len = %d", __func__, len);
        return NULL;
    }

    hdr = (struct eth_hdr *)p->payload;
    if (memcpy_s(&hdr->dest, len, NetBufGetAddress(nb, E_DATA_BUF), len) != EOK) {
        pbuf_free(p);
        HDF_LOGE("%s memcpy err!", __func__);
        return NULL;
    }

    return p;
}

/*
 * Convert LWIP pbuf to net buffer.
 *
 * @param  : netdev The net device object
 *           lwip_buf The LWIP net buffer
 * @return : The net buffer
 */
NetBuf *Pbuf2NetBuf(const NetDevice *netdev, struct pbuf *lwipBuf)
{
    NetBuf *nb   = NULL;
    struct pbuf   *tmp  = NULL;
    uint32_t index = 0;
    uint32_t offset = 0;

    if (lwipBuf == NULL) {
        return NULL;
    }

    nb = NetBufDevAlloc(netdev, lwipBuf->tot_len);
    if (nb == NULL) {
        HDF_LOGE("%s alloc net buffer failed! len = %d", __func__, lwipBuf->tot_len);
        return NULL;
    }

    offset = nb->bufs[E_DATA_BUF].offset;

    for (tmp = lwipBuf; tmp != NULL; tmp = tmp->next, index++) {
        if (tmp->len == 0 || tmp->payload == NULL) {
            continue;
        }

        if (NetBufGetRoom(nb, E_TAIL_BUF) < tmp->len) {
            HDF_LOGE("%s fail, tailroom[%d], len[%d], idx[%d]",
                __func__, NetBufGetRoom(nb, E_TAIL_BUF), tmp->len, index);
            NetBufFree(nb);
            return NULL;
        }

        if (memcpy_s(nb->mem + offset, tmp->len, tmp->payload, tmp->len) != EOK) {
            HDF_LOGE("%s memcpy_s err!", __func__);
            NetBufFree(nb);
            return NULL;
        }
        NetBufPush(nb, E_DATA_BUF, tmp->len);

        offset += tmp->len;
    }

    return nb;
}
