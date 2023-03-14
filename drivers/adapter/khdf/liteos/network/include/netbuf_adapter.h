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

/**
 * @addtogroup WLAN
 * @{
 *
 * @brief Provides cross-OS migration, component adaptation, and modular assembly and compilation.
 *
 * Based on the unified APIs provided by the WLAN module, developers of the Hardware Driver Interface
 * (HDI) are capable of creating, disabling, scanning for, and connecting to WLAN hotspots, managing WLAN chips,
 * network devices, and power, and applying for, releasing, and moving network data buffers.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef _HDF_NETBUF_ADAPTER_H
#define _HDF_NETBUF_ADAPTER_H

#include "los_spinlock.h"
#include "hdf_dlist.h"
#include "lwip/pbuf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates the segments of a network data buffer.
 *
 * The entire network data buffer is divided into three segments: a header, data, and a tail.
 * The header and tail are used to extend both ends of the data segment.
 *
 * @since 1.0
 */
enum {
    E_HEAD_BUF, /**< Header buffer segment */
    E_DATA_BUF, /**< Data segment */
    E_TAIL_BUF, /**< Tail buffer segment */
    MAX_BUF_NUM /**< Maximum number of buffer segments */
};

/**
 * @brief Defines the data buffer field.
 *
 * The data buffer field records the offset (based on the memory buffer address) and length of each buffer segment.
 *
 * @since 1.0
 */
struct BufField {
    uint32_t offset;      /**< Offset of the buffer segment */
    uint32_t len;         /**< Length of the buffer segment */
};

/**
 * @brief Defines the reserved field of a network data buffer used to store private information.
 *
 * The length of the reserved field is <b>68</b> bytes.
 */
#define MAX_NETBUF_RESEVER_SIZE     68

/**
 * @brief Records and saves a network data buffer.
 *
 * This structure is used to transmit network data between the protocol stack and network driver.
 *
 * @since 1.0
 */
typedef struct NetBuf {
    struct DListHead dlist;             /**< Doubly linked list. Generally, multiple network data buffers are
                                             linked by using a doubly linked list. */
    struct BufField bufs[MAX_BUF_NUM];  /**< Defines buffer segments used to record the offset address
                                             (based on the memory buffer address) and length of each buffer segment,
                                             including the header buffer segment, data segment, and tail buffer segment.
                                             For details, see {@link MAX_BUF_NUM}. */
    uint8_t     *mem;                   /**< Memory buffer address */
    uint32_t    len;                    /**< Length of the memory buffer */
    uint32_t    dataLen;                /**< Actual data length of the network data buffer */
    void        *dev;                   /**< Network device that receives the network data */
    uint32_t    qmap;                   /**< Queue mappings of the network data buffer */
    uint8_t     rsv[MAX_NETBUF_RESEVER_SIZE]; /**< Reserved field. For details, see {@link MAX_NETBUF_RESEVER_SIZE}. */
} NetBuf;

/**
 * @brief Indicates the queues for storing network data.
 *
 * Queues can be used to process multiple pieces of network data in a centralized manner, improving efficiency.
 *
 * @since 1.0
 */
typedef struct NetBufQueue {
    struct DListHead dlist; /**< Doubly linked list. Generally, multiple network data buffers are linked
                                 by using a doubly linked list. */
    uint32_t     size;      /**< Number of network data buffers in the queue */
    struct Spinlock lock;      /**< Queue operation lock */
} NetBufQueue;

struct NetDevice;

/**
 * @brief Converts the <b>pbuf</b> structure of Lightweight TCP/IP Stack (lwIP) to a network data buffer.
 *
 * When a network device is specified, the reserved space of the network device will be added to
 * the size of the converted network data buffer.
 *
 * @param netdev Indicates the pointer to the network device.
 * @param lwip_buf Indicates the pointer to the data buffer of lwIP.
 *
 * @return Returns the pointer to the network data buffer if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
NetBuf *Pbuf2NetBuf(const struct NetDevice *netdev, struct pbuf *lwipBuf);

/**
 * @brief Converts a network data buffer to the <b>pbuf</b> structure of Lightweight TCP/IP Stack (lwIP).
 *
 * @param nb Indicates the pointer to the network data buffer.
 *
 * @return Returns the pointer to the <b>pbuf</b> structure if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
struct pbuf *NetBuf2Pbuf(const NetBuf *nb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
/** @} */
