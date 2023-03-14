/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SOFT_BUS_CHANNEL_H
#define SOFT_BUS_CHANNEL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t (*openChannel)(const char *connectParams, int64_t requestId, int64_t *returnChannelId);
    void (*closeChannel)(int64_t channelId);
    int32_t (*sendMsg)(int64_t channelId, const uint8_t *data, uint32_t dataLen);
    void (*notifyResult)(int64_t channelId);
} SoftBus;

int32_t InitSoftBusChannelModule(void);
void DestroySoftBusChannelModule(void);
SoftBus *GetSoftBusInstance(void);
bool IsSoftBusChannelSupported(void);

#ifdef __cplusplus
}
#endif
#endif
