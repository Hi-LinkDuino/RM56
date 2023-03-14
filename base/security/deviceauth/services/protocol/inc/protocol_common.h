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

#ifndef PROTOCOL_COMMON_H
#define PROTOCOL_COMMON_H

#include "string_util.h"

typedef enum {
    PROTOCOL_TYPE_NONE,
    ISO,
    PAKE_V1,
    PAKE_V2,
    STS,
} ProtocolType;

void FreeAndCleanKey(Uint8Buff *key);
int32_t InitSingleParam(Uint8Buff *param, uint32_t len);

#endif
