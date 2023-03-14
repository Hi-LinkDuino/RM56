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

#ifndef AUTH_SESSION_COMMON_DEFINES_H
#define AUTH_SESSION_COMMON_DEFINES_H

#include "base_session.h"
#include "device_auth.h"
#include "hc_vector.h"

DECLARE_HC_VECTOR(ParamsVec, void*)

typedef struct {
    Session base;
    int32_t curTaskId;
    ParamsVec paramsList;
    uint32_t currentIndex;
} AuthSession;

typedef enum {
    INVALID_GROUP_AUTH_TYPE = -1,
    ACCOUNT_UNRELATED_GROUP_AUTH_TYPE,
    ACCOUNT_RELATED_GROUP_AUTH_TYPE,
} GroupAuthType;

#define MIN_KEY_LENGTH 16
#define MAX_KEY_LENGTH 1024

#endif
