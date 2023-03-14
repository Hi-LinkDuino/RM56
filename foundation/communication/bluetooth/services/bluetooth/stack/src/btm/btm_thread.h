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

#ifndef BTM_THREAD_H
#define BTM_THREAD_H

#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

void BtmInitThread();
void BtmCloseThread();

Thread *BTM_GetProcessingThread();

#define BTM_PROCESSING_QUEUE_SIZE_DEFAULT 128

#define PROCESSING_QUEUE_ID_BTM 0
#define PROCESSING_QUEUE_ID_HCI 1
#define PROCESSING_QUEUE_ID_LA2CAP 2
#define PROCESSING_QUEUE_ID_GAP 3
#define PROCESSING_QUEUE_ID_RFCOMM 4
#define PROCESSING_QUEUE_ID_ATT 5
#define PROCESSING_QUEUE_ID_AVCTP 6
#define PROCESSING_QUEUE_ID_AVDTP 7
#define PROCESSING_QUEUE_ID_SDP 8
#define PROCESSING_QUEUE_ID_SMP 9

int BTM_CreateProcessingQueue(uint8_t queueId, uint16_t size);

int BTM_DeleteProcessingQueue(uint8_t queueId);

int BTM_RunTaskInProcessingQueue(uint8_t queueId, void (*task)(void *context), void *context);

#ifdef __cplusplus
}
#endif

#endif