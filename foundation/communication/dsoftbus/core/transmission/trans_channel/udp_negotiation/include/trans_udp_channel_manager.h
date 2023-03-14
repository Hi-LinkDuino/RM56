/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef TRANS_UDP_CHANNEL_MANAGER_H
#define TRANS_UDP_CHANNEL_MANAGER_H

#include <stdint.h>
#include "softbus_app_info.h"

typedef enum {
    UDP_CHANNEL_STATUS_INIT = 0,
    UDP_CHANNEL_STATUS_OPEN_AUTH,
    UDP_CHANNEL_STATUS_NEGING,
    UDP_CHANNEL_STATUS_DONE
} UdpChannelStatus;

typedef struct {
    ListNode node;
    int64_t seq;
    AppInfo info;
    uint32_t timeOut;
    UdpChannelStatus status;
    uint32_t requestId;
} UdpChannelInfo;

int32_t TransUdpChannelMgrInit(void);
void TransUdpChannelMgrDeinit(void);

int32_t GetUdpChannelLock(void);
void ReleaseUdpChannelLock(void);

int32_t TransAddUdpChannel(UdpChannelInfo *channel);
int32_t TransDelUdpChannel(int32_t channelId);

int32_t TransGetUdpChannelBySeq(int64_t seq, UdpChannelInfo *channel);
int32_t TransGetUdpChannelById(int32_t channelId, UdpChannelInfo *channel);
int32_t TransGetUdpChannelByRequestId(uint32_t requestId, UdpChannelInfo *channel);

int32_t TransSetUdpChannelStatus(int64_t seq, UdpChannelStatus status);
int32_t TransSetUdpChannelOptType(int32_t channelId, UdpChannelOptType type);

int32_t TransUdpGetNameByChanId(int32_t channelId, char *pkgName, char *sessionName,
    uint16_t pkgNameLen, uint16_t sessionNameLen);

void TransUpdateUdpChannelInfo(int64_t seq, const AppInfo *appInfo);

UdpChannelInfo *TransGetChannelObj(int32_t channelId);
#endif // !TRANS_UDP_CHANNEL_MANAGER_H