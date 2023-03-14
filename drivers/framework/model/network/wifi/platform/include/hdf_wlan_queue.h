/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef HDF_WLAN_QUEUE_H
#define HDF_WLAN_QUEUE_H
#include "hdf_base.h"

typedef struct {
} HdfWlanQueue;

HdfWlanQueue *CreateQueue(uint16_t maxQueueSize);
void DestroyQueue(HdfWlanQueue* queue);

void *PopQueue(HdfWlanQueue *queue);
int32_t PushQueue(HdfWlanQueue *queue, void *context);


#endif
