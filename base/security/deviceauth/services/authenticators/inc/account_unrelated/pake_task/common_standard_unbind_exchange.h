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

#ifndef COMMON_STANDARD_UNBIND_EXCHANGE_H
#define COMMON_STANDARD_UNBIND_EXCHANGE_H

#include <stdint.h>
#include "common_defs.h"
#include "pake_base_cur_task.h"

#define HICHAIN_REMOVE_INFO_REQUEST "hichain_remove_info_request"
#define HICHAIN_REMOVE_INFO_RESPONSE "hichain_remove_info_response"

#define STANDARD_UNBIND_EXCHANGE_NONCE_LEN 12

typedef struct StandardUnbindExchangeParamsT {
    Uint8Buff rmvInfo;
    Uint8Buff exRmvCipher;
    Uint8Buff resultCipher;
    Uint8Buff nonce;
} StandardUnbindExchangeParams;

int32_t InitStandardUnbindExchangeParams(StandardUnbindExchangeParams *params);
void DestroyStandardUnbindExchangeParams(StandardUnbindExchangeParams *params);

int32_t ClientRequestStandardUnbindExchange(const PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams);
int32_t ServerResponseStandardUnbindExchange(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams);
int32_t ClientConfirmStandardUnbindExchange(PakeParams *pakeParams, StandardUnbindExchangeParams *exchangeParams);

#endif
