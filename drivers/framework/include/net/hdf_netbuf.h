/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
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

/**
 * @file hdf_netbuf.h
 *
 * @brief Declares network data buffers and provides APIs for operating buffer queues.
 *
 * This file describes the following network data operations for network device driver development: \n
 *
 * Applying for, releasing, and moving a network data buffer
 * Initializing a network data buffer queue, placing a network data buffer to a queue, and removing a network data
 * buffer from a queue
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDF_NETBUF_H
#define HDF_NETBUF_H

#include "netbuf_adapter.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Initializes a network data buffer queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @since 1.0
 * @version 1.0
 */
void NetBufQueueInit(NetBufQueue *q);

/**
 * @brief Obtains the size of a network data buffer queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the size of the network data buffer queue.
 *
 * @since 1.0
 * @version 1.0
 */
uint32_t NetBufQueueSize(const NetBufQueue *q);

/**
 * @brief Checks whether the network data buffer queue is empty.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns <b>true</b> if the queue is empty; returns <b>false</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
bool NetBufQueueIsEmpty(const NetBufQueue *q);

/**
 * @brief Adds a network data buffer to the tail of a queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 * @param nb Indicates the pointer to the network data buffer.
 *
 * @since 1.0
 * @version 1.0
 */
void NetBufQueueEnqueue(NetBufQueue *q, NetBuf *nb);

/**
 * @brief Adds a network data buffer to the header of a queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 * @param nb Indicates the pointer to the network data buffer.
 *
 * @since 1.0
 * @version 1.0
 */
void NetBufQueueEnqueueHead(NetBufQueue *q, NetBuf *nb);

/**
 * @brief Obtains a network data buffer from the header of a queue and deletes it from the queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the pointer to the first network data buffer if the queue is not empty;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
NetBuf *NetBufQueueDequeue(NetBufQueue *q);

/**
 * @brief Obtains a network data buffer from the tail of a queue and deletes it from the queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the pointer to the last network data buffer if the queue is not empty;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
NetBuf *NetBufQueueDequeueTail(NetBufQueue *q);

/**
 * @brief Obtains the network data buffer from the header of a queue, without deleting it from the queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the pointer to the first network data buffer if the queue is not empty;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
NetBuf *NetBufQueueAtHead(const NetBufQueue *q);

/**
 * @brief Obtains the network data buffer from the tail of a queue, without deleting it from the queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @return Returns the pointer to the last network data buffer if the queue is not empty;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
NetBuf *NetBufQueueAtTail(const NetBufQueue *q);

/**
 * @brief Clears a network data buffer queue and releases the network data buffer in the queue.
 *
 * @param q Indicates the pointer to the network data buffer queue.
 *
 * @since 1.0
 * @version 1.0
 */
void NetBufQueueClear(NetBufQueue *q);

/**
 * @brief Moves all network data buffers from one queue to another and clears the source queue.
 *
 * @param q Indicates the pointer to the target network data buffer queue.
 * @param add Indicates the pointer to the source network data buffer queue.
 *
 * @since 1.0
 * @version 1.0
 */
void NetBufQueueConcat(NetBufQueue *q, NetBufQueue *add);

/**
 * @brief Applies for a network data buffer.
 *
 * @param size Indicates the size of the network data buffer.
 *
 * @return Returns the pointer to the network data buffer if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
NetBuf *NetBufAlloc(uint32_t size);

/**
 * @brief Releases a network data buffer.
 *
 * @param nb Indicates the pointer to the network data buffer.
 *
 * @since 1.0
 * @version 1.0
 */
void NetBufFree(NetBuf *nb);

struct NetDevice;
/**
 * @brief Applies for a network data buffer based on the reserved space and requested size set by a network device.
 *
 * @param dev Indicates the pointer to the network device.
 * @param size Indicates the size of the network data buffer applied.
 *
 * @return Returns the pointer to the network data buffer if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
NetBuf *NetBufDevAlloc(const struct NetDevice *dev, uint32_t size);

/**
 * @brief Performs operations based on the segment ID of a network data buffer.
 * The function is opposite to that of {@link NetBufPop}.
 *
 * Description:
 * ID Type | Result
 * -------|---------
 * E_HEAD_BUF | The length of the header buffer segment is increased and the length of the data segment is reduced.
 * E_DATA_BUF | The length of the data segment is increased and the length of the tail buffer segment is reduced.
 * E_TAIL_BUF | The length of the tail buffer segment is increased and the length of the data segment is reduced.
 *
 * @param nb Indicates the pointer to the network data buffer.
 * @param id Indicates the buffer segment ID.
 * @param len Indicates the operation length.
 *
 * @return Returns the start address of the data segment if the operation is successful;
 * returns <b>NULL</b> if the operation length exceeds the space of a specified buffer segment.
 *
 * @since 1.0
 * @version 1.0
 */
void *NetBufPush(NetBuf *nb, uint32_t id, uint32_t len);

/**
 * @brief Performs operations based on the segment ID of a network data buffer.
 * The function is opposite to that of {@link NetBufPush}.
 *
 * Description:
 * ID Type | Result
 * -------|---------
 * E_HEAD_BUF | The length of the header buffer segment is reduced and the length of the data segment is increased.
 * E_DATA_BUF| The length of the data segment is reduced and the length of the tail buffer segment is increased.
 * E_TAIL_BUF | The length of the tail buffer segment is reduced and the length of the data segment is increased.
 *
 * @param nb Indicates the pointer to the network data buffer.
 * @param id Indicates the buffer segment ID.
 * @param len Indicates the operation length.
 *
 * @return Returns the start address of the data segment if the operation is successful;
 * returns <b>NULL</b> if the operation length exceeds the space of a specified buffer segment.
 *
 * @since 1.0
 * @version 1.0
 */
void *NetBufPop(NetBuf *nb, uint32_t id, uint32_t len);

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
 * @version 1.0
 */
uint8_t *NetBufGetAddress(const NetBuf *nb, uint32_t id);

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
 * @version 1.0
 */
uint32_t NetBufGetRoom(const NetBuf *nb, uint32_t id);

/**
 * @brief Obtains the actual data length of the data segment of a network data buffer.
 *
 * @param nb Indicates the pointer to the network data buffer.
 *
 * @return Returns the actual data length of the data segment.
 *
 * @since 1.0
 * @version 1.0
 */
uint32_t NetBufGetDataLen(const NetBuf *nb);

/**
 * @brief Adjusts the size of a network data buffer space.
 *
 * This function is used to apply for a new network data buffer based on the configured reserved space and
 * the size of the source network data buffer, and copy the actual data to the new network data buffer.
 *
 * @param nb Indicates the pointer to the network data buffer.
 * @param head Indicates the size of the header buffer segment reserved.
 * @param tail Indicates the size of the tail buffer segment reserved.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetBufResizeRoom(NetBuf *nb, uint32_t head, uint32_t tail);

/**
 * @brief Copies data in a network data buffer to another network data buffer.
 *
 * @param nb Indicates the pointer to the network data buffer.
 * @param cnb Indicates the pointer to the target network data buffer.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetBufConcat(NetBuf *nb, NetBuf *cnb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDF_NETBUF_H */
/** @} */
