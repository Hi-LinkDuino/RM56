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

#ifndef SOFTBUS_PROXYCHANNEL_NETWORK_H
#define SOFTBUS_PROXYCHANNEL_NETWORK_H

#include "stdint.h"

#include "softbus_app_info.h"

int32_t NotifyNetworkingChannelOpened(int32_t channelId, const AppInfo *appInfo, unsigned char isServer);

void NotifyNetworkingChannelOpenFailed(int32_t channelId, const char *NetworkId);

void NotifyNetworkingChannelClosed(int32_t channelId);

void NotifyNetworkingMsgReceived(int32_t channelId, const char *data, uint32_t len);

#endif

