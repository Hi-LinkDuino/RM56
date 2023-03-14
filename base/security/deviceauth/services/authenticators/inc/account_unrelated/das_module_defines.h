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

#ifndef DAS_MODULE_DEFINES_H
#define DAS_MODULE_DEFINES_H

#include "common_defs.h"
#include "device_auth_defines.h"
#include "hc_types.h"

#define STEP_ONE 1
#define STEP_TWO 2
#define STEP_THREE 3
#define MIN_PIN_LEN 4
#define MAX_PIN_LEN 1024
#define MIN_OUTPUT_KEY_LEN 16
#define MAX_OUTPUT_KEY_LEN 32

typedef enum {
    PAKE_REQUEST = 0x0001,
    PAKE_RESPONSE = 0x8001,
    PAKE_CLIENT_CONFIRM = 0x0002,
    PAKE_SERVER_CONFIRM = 0x8002,

    PAKE_BIND_EXCHANGE_REQUEST = 0x0003,
    PAKE_BIND_EXCHANGE_RESPONSE = 0x8003,

    PAKE_UNBIND_EXCHANGE_REQUEST = 0x0033,
    PAKE_UNBIND_EXCHANGE_RESPONSE = 0x8033,

    ISO_CLIENT_BIND_START = 0x0001,
    ISO_SERVER_BIND_START = 0x8001,
    ISO_CLIENT_BIND_CAL_TOKEN = 0x0002,
    ISO_SERVER_BIND_CAL_TOKEN = 0x8002,

    ISO_CLIENT_AUTH_OR_UNBIND_START = 0x0011,
    ISO_SERVER_AUTH_OR_UNBIND_START = 0x8011,
    ISO_CLIENT_AUTH_OR_UNBIND_CAL_TAOKEN = 0x0012,
    ISO_SERVER_AUTH_OR_UNBIND_CAL_TAOKEN = 0x8012,

    ISO_CLIENT_BIND_EXCHANGE_CMD = 0x0003,
    ISO_SERVER_BIND_EXCHANGE_RET = 0x8003,
    ISO_CLIENT_BIND_EXCHANGE_CONFIRM = 0x0004,

    ISO_CLIENT_UNBIND_EXCHANGE_CMD = 0x0013,
    ISO_SERVER_UNBIND_EXCHANGE_RET = 0x8013,
    ISO_CLIENT_UNBIND_EXCHANGE_CONFIRM = 0x0014,

    ISO_RESULT_CONFIRM_CMD = 0x0013,

    ERR_MESSAGE = 0x8080,
    INVALID_MESSAGE = 0xFFFF,
} DasMessageCode;

typedef enum CurTaskTypeT {
    TASK_TYPE_ISO_PROTOCOL = 0,
    TASK_TYPE_PAKE_V1_PROTOCOL = 1,
    TASK_TYPE_PAKE_V2_PROTOCOL = 2,
    TASK_TYPE_BIND_LITE_EXCHANGE = 3,
    TASK_TYPE_UNBIND_LITE_EXCHANGE = 4,
    TASK_TYPE_BIND_STANDARD_EXCHANGE = 5,
    TASK_TYPE_UNBIND_STANDARD_EXCHANGE = 6,
    TASK_TYPE_NONE,
} CurTaskType;

typedef enum {
    KEY_ALIAS_ACCESSOR_PK = DEVICE_TYPE_ACCESSORY,
    KEY_ALIAS_CONTROLLER_PK = DEVICE_TYPE_CONTROLLER,
    KEY_ALIAS_LT_KEY_PAIR = 2,
    KEY_ALIAS_KEK = 3,
    KEY_ALIAS_DEK = 4,
    KEY_ALIAS_TMP = 5,
    KEY_ALIAS_PSK = 6,
    KEY_ALIAS_AUTH_TOKEN = 7,

    KEY_ALIAS_TYPE_END
} KeyAliasType;  // 0 ~ 2^8-1, don't change the order

#endif
