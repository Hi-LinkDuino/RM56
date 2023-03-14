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

#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <stdint.h>
#include "base_session.h"
#include "device_auth.h"

#define BIND_TYPE 0
#define AUTH_TYPE 1

void InitSessionManager(void);
void DestroySessionManager(void);

bool IsRequestExist(int64_t requestId);

int32_t CreateSession(int64_t requestId, SessionTypeValue sessionType, CJson *param,
    const DeviceAuthCallback *callback);
int32_t ProcessSession(int64_t requestId, int32_t type, CJson *in);
void DestroySession(int64_t requestId);
void OnChannelOpened(int64_t requestId, int64_t channelId);
void OnConfirmed(int64_t requestId, CJson *returnData);

#endif
