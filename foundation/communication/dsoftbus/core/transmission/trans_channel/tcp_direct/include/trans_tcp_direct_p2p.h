/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SOFTBUS_TCP_DIRECT_P2P_H
#define SOFTBUS_TCP_DIRECT_P2P_H

#include <stdint.h>

#include "softbus_app_info.h"
#include "softbus_conn_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define CODE_VERIFY_P2P 3

int32_t P2pDirectChannelInit(void);

int32_t OpenP2pDirectChannel(const AppInfo *appInfo, const ConnectOption *connInfo, int32_t *channelId);

void StopP2pSessionListener(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
