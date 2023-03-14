/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_WLAN_PRIORITY_QUEUE_H
#define HDF_WLAN_PRIORITY_QUEUE_H

#include "hdf_base.h"
#include "osal/osal_sem.h"

#define HIGHEST_PRIORITY 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
} PriorityQueue;

PriorityQueue *CreatePriorityQueue(uint16_t queueSize, uint8_t priorityLevelCount);
void DestroyPriorityQueue(PriorityQueue *);
void *PopPriorityQueue(PriorityQueue *queue, uint32_t waitInMS);
int32_t PushPriorityQueue(PriorityQueue *queue, const uint8_t priority, void *context);

#ifdef __cplusplus
}
#endif

#endif