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

#ifndef STANDARD_EXCHANGE_MESSAGE_UTIL_H
#define STANDARD_EXCHANGE_MESSAGE_UTIL_H

#include "common_defs.h"
#include "json_utils.h"
#include "pake_base_cur_task.h"

int32_t PackageNonceAndCipherToJson(const Uint8Buff *nonce, const Uint8Buff *cipher, CJson *data, const char *key);
int32_t ParseNonceAndCipherFromJson(Uint8Buff *nonce, Uint8Buff *cipher, const CJson *in, const char *key);

int32_t GenerateSelfChallenge(PakeParams *params);
int32_t GetPeerChallenge(PakeParams *params, const CJson *in);

#endif
