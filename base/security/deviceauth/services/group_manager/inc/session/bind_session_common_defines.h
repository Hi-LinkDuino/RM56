/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BIND_SESSION_COMMON_DEFINES_H
#define BIND_SESSION_COMMON_DEFINES_H

#include "base_session.h"

typedef void (*OnChannelOpenedFunc)(Session *, int64_t, int64_t);
typedef void (*OnConfirmedFunc)(Session *, CJson *);
typedef int32_t (*ProcessSessionFunc)(Session *, CJson *);

typedef struct {
    Session base;
    bool isWaiting;
    char *appId;
    CJson *params;
    OnChannelOpenedFunc onChannelOpened;
    OnConfirmedFunc onConfirmed;
    int32_t curTaskId;
    int32_t opCode;
    int32_t channelType;
    int32_t moduleType;
    int32_t osAccountId;
    int64_t reqId;
    int64_t channelId;
} BindSession;

#endif
