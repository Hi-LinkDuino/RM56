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

#ifndef TRANS_SERVER_PROXY_H
#define TRANS_SERVER_PROXY_H

#include "stdint.h"
#include "softbus_common.h"
#include "softbus_trans_def.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t TransServerProxyInit(void);
void TransServerProxyDeInit(void);
int32_t ServerIpcCreateSessionServer(const char *pkgName, const char *sessionName);
int32_t ServerIpcRemoveSessionServer(const char *pkgName, const char *sessionName);
int32_t ServerIpcOpenSession(const SessionParam *param, TransInfo *info);
int32_t ServerIpcOpenAuthSession(const char *sessionName, const ConnectionAddr *addrInfo);
int32_t ServerIpcNotifyAuthSuccess(int channelId);
int32_t ServerIpcCloseChannel(int32_t channelId, int32_t channelType);
int32_t ServerIpcSendMessage(int32_t channelId, int32_t channelType, const void *data, uint32_t len, int32_t msgType);
int32_t ServerIpcQosReport(int32_t channelId, int32_t chanType, int32_t appType, int32_t quality);

#ifdef __cplusplus
}
#endif
#endif
