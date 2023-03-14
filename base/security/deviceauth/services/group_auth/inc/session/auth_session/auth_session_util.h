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

#ifndef AUTH_SESSION_UTIL_H
#define AUTH_SESSION_UTIL_H

#include <stdint.h>
#include "base_group_auth.h"
#include "device_auth.h"
#include "json_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

BaseGroupAuth *GetGroupAuth(int32_t groupAuthType);
int32_t GetAuthModuleType(const CJson *in);
int32_t GetInfoHash(const uint8_t *info, uint32_t infoLen, char *str, uint32_t strLen);
bool IsBleAuthForAcrossAccount(const CJson *authParam);
int32_t GroupTypeToAuthForm(int32_t groupType);

#ifdef __cplusplus
}
#endif
#endif
