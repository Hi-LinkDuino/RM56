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

#ifndef ISO_BASE_CUR_TASK_H
#define ISO_BASE_CUR_TASK_H

#include "json_utils.h"
#include "alg_loader.h"
#include "common_defs.h"
#include "das_module_defines.h"
#include "iso_protocol_common.h"

#define CHALLENGE_SIZE 16
#define NONCE_SIZE 12
#define TAG_LEN 16
#define EXCHANGE_AAD "hichain_iso_exchange"
#define RESULT_AAD "hichain_iso_result"
#define UNBIND_ADD_REQUEST "hichain_remove_info_request"
#define UNBIND_ADD_RESPONSE "hichain_remove_info_response"
#define GENERATE_RETURN_KEY_STR "hichain_return_key"
#define ISO_TOKEN_LEN 32
#define RESULT_MAC_LEN 32
#define AUTH_CODE_LEN 32
#define KEY_ALIAS_SIZE 32
#define ENC_CHALLENGE_LEN (CHALLENGE_SIZE + TAG_LEN)
#define SEED_LEN 32
#define ISO_KEY_ALIAS_LEN 32

typedef struct IsoParamsT {
    IsoBaseParams baseParams;

    Uint8Buff seed;
    int32_t selfUserType;
    int32_t peerUserType;
    char *packageName;
    char *serviceType;
    char *pinCodeString;
    int32_t opCode;
    uint32_t keyLen;
    bool isClient;
} IsoParams;

typedef struct SymBaseCurTaskT {
    CurTaskType (*getCurTaskType)(void);
    void (*destroyTask)(struct SymBaseCurTaskT *);
    int(*process)(struct SymBaseCurTaskT *, IsoParams *params, const CJson *in, CJson *out, int32_t *status);
    int taskStatus;
} SymBaseCurTask;

#endif
