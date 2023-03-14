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

#ifndef COMMON_STANDARD_BIND_EXCHANGE_H
#define COMMON_STANDARD_BIND_EXCHANGE_H

#include <stdint.h>
#include "common_defs.h"
#include "pake_base_cur_task.h"

#define HICHAIN_EXCHANGE_REQUEST  "hichain_exchange_request"
#define HICHAIN_EXCHANGE_RESPONSE  "hichain_exchange_response"
#define STANDARD_BIND_EXCHANGE_NONCE_LEN 12

typedef struct StandardBindExchangeParamsT {
    Uint8Buff pubKeySelf;
    Uint8Buff pubKeyPeer;
    Uint8Buff authInfo;
    Uint8Buff exInfoCipher;
    Uint8Buff nonce;
} StandardBindExchangeParams;

int32_t InitStandardBindExchangeParams(StandardBindExchangeParams *params);
void DestroyStandardBindExchangeParams(StandardBindExchangeParams *params);

int32_t ClientRequestStandardBindExchange(const PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams);
int32_t ServerResponseStandardBindExchange(PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams);
int32_t ClientConfirmStandardBindExchange(PakeParams *pakeParams, StandardBindExchangeParams *exchangeParams);

#endif
