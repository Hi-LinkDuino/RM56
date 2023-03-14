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

#ifndef TRANS_CHANNEL_CALLBACK_H
#define TRANS_CHANNEL_CALLBACK_H

#include <stdint.h>

#include "session.h"
#include "softbus_def.h"
#include "softbus_trans_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    int32_t (*OnChannelOpened)(const char *pkgName, const char *sessionName, const ChannelInfo *channel);
    int32_t (*OnChannelClosed)(const char *pkgName, int32_t channelId, int32_t channelType);
    int32_t (*OnChannelOpenFailed)(const char *pkgName, int32_t channelId, int32_t channelType);
    int32_t (*OnDataReceived)(const char *pkgName, int32_t channelId, int32_t channelType,
        const void *data, uint32_t len, int32_t type);
    int32_t (*OnQosEvent)(const char *pkgName, const QosParam *param);
    int32_t (*GetPkgNameBySessionName)(const char *sessionName, char *pkgName, uint16_t len);
    int32_t (*GetUidAndPidBySessionName)(const char *sessionName, int32_t *uid, int32_t *pid);
} IServerChannelCallBack;

IServerChannelCallBack *TransServerGetChannelCb(void);

int32_t TransServerOnChannelLinkDown(const char *pkgName, const char *networkId, int32_t routeType);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif

