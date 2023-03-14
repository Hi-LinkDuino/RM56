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

#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "device_auth.h"
#include "common_defs.h"
#include "json_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t InitChannelManager(void);
void DestroyChannelManager(void);

/* Channel operation interfaces */
ChannelType GetChannelType(const DeviceAuthCallback *callback, const CJson *jsonParams);
int32_t OpenChannel(ChannelType channelType, const CJson *jsonParams, int64_t requestId, int64_t *returnChannelId);
void CloseChannel(ChannelType channelType, int64_t channelId);
int32_t HcSendMsg(ChannelType channelType, int64_t requestId, int64_t channelId,
    const DeviceAuthCallback *callback, const char *data);
void NotifyBindResult(ChannelType channelType, int64_t channelId);
int32_t GetLocalConnectInfo(char *jsonAddrInfo, int32_t bufLen);

#ifdef __cplusplus
}
#endif
#endif
