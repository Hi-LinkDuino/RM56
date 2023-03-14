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

#ifndef TRANS_UDP_NEGOTIATION_H
#define TRANS_UDP_NEGOTIATION_H

#include <stdint.h>
#include "softbus_app_info.h"
#include "softbus_conn_interface.h"
#include "trans_channel_callback.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t TransUdpChannelInit(IServerChannelCallBack *callback);
void TransUdpChannelDeinit(void);

int32_t TransOpenUdpChannel(AppInfo* appInfo, const ConnectOption *connOpt, int32_t *channelId);
int32_t TransCloseUdpChannel(int32_t channelId);

int32_t NotifyUdpChannelOpenFailed(const AppInfo *info);
int32_t NotifyUdpChannelClosed(const AppInfo *info);
int32_t NotifyUdpQosEvent(const AppInfo *info, int eventId, int tvCount, const QosTv *tvList);

void ReleaseUdpChannelId(int32_t channelId);
#ifdef __cplusplus
}
#endif
#endif