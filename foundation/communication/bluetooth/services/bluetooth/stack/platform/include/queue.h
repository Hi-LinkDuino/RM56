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

#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Queue Queue;
typedef void (*NodeDataFreeCb)(void *data);

/**
 * @brief Perform instantiation of the Queue.
 *
 * @param capacity Queue's capacity.
 * @return Succeed return Queue instantiation, failed return NULL.
 * @since 6
 */
Queue *QueueCreate(uint32_t capacity);

/**
 * @brief Delete instantiation of the Queue.
 *
 * @param queue Queue's pointer.
 * @param cb Free queue node callback.
 * @since 6
 */
void QueueDelete(Queue *queue, NodeDataFreeCb cb);

/**
 * @brief Enqueue data into Queue.
 *
 * @param queue Queue's pointer.
 * @param data Enqueue data.
 * @since 6
 */
void QueueEnqueue(Queue *queue, void *data);

/**
 * @brief TryEnqueue data into NonblockQueue.
 *
 * @param queue Queue's pointer.
 * @param data Enqueue data.
 * @return Success enqueue data return true, else return false.
 * @since 6
 */
bool QueueTryEnqueue(Queue *queue, void *data);

/**
 * @brief Dequeue data from Queue.
 * @param queue Queue's pointer.
 * @return Dequeue data.
 * @since 6
 */
void *QueueDequeue(Queue *queue);

/**
 * @brief TryDequeue data from NonblockQueue.
 *
 * @param queue Queue's pointer.
 * @return Succeed return data, failed return NULL.
 * @since 6
 */
void *QueueTryDequeue(Queue *queue);

/**
 * @brief Get Queue EnqueueFd.
 *
 * @param queue Queue pointer
 * @return Succeed return Queue EnqueueFd, failed return -1.
 * @since 6
 */
int32_t QueueGetEnqueueFd(const Queue *queue);

/**
 * @brief Get Queue DequeueFd.
 *
 * @param queue Queue pointer.
 * @return Succeed return Queue DequeueFd, failed return -1.
 * @since 6
 */
int32_t QueueGetDequeueFd(const Queue *queue);

/**
 * @brief Flush all data of the queue.
 *
 * @param queue Queue pointer.
 * @param cb Free queue node callback.
 * @since 6
 */
void QueueFlush(Queue *queue, NodeDataFreeCb cb);

/**
 * @brief Check if the queue is empty.
 *
 * @param list Queue pointer.
 * @return true: The queue is empty. false: The queue is not empty.
 * @since 6
 */
bool QueueIsEmpty(Queue *queue);

/**
 * @brief Get the size of the queue.
 *
 * @param list Queue pointer.
 * @return The size of the queue.
 * @since 6
 */
int32_t QueueGetSize(Queue *queue);

#ifdef __cplusplus
}
#endif

#endif  // QUEUE_H