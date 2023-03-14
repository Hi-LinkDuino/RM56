/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HKS_CMD_ID_H
#define HKS_CMD_ID_H

#include "hks_type_inner.h"

enum HksCmdId {
    HKS_CMD_ID_INVALID = 0x0,
    HKS_CMD_ID_GENERATE_KEY,
    HKS_CMD_ID_GET_KEY_CHARACTER,
    HKS_CMD_ID_BEGIN,
    HKS_CMD_ID_UPDATE,
    HKS_CMD_ID_FINISH,
    HKS_CMD_ID_ABORT,
    HKS_CMD_ID_SET,
    HKS_CMD_ID_GET,
    HKS_CMD_ID_ATTEST_KEY,
    HKS_CMD_ID_EXPORT_KEY,
    HKS_CMD_ID_DELETE,
    HKS_CMD_ID_EXPORT_TRUST_CERT,
    HKS_CMD_ID_ECDH,
    HKS_CMD_ID_ATTEST_TA_KEY,
    HKS_CMD_ID_SET_KEY_PROTECTION,
    HKS_CMD_ID_CONFIGURE,

    HKS_CMD_ID_IMPORT_KEY = 0x100,
    HKS_CMD_ID_GET_CERTCHAIN = 0x101,
    HKS_CMD_ID_SIGN = 0x102,
    HKS_CMD_ID_SIGN_INIT = 0x103,
    HKS_CMD_ID_SIGN_UPDATE = 0x104,
    HKS_CMD_ID_SIGN_FINAL = 0x105,
    HKS_CMD_ID_VERIFY = 0x106,
    HKS_CMD_ID_VERIFY_INIT = 0x107,
    HKS_CMD_ID_VERIFY_UPDATE = 0x108,
    HKS_CMD_ID_VERIFY_FINAL = 0x109,
    HKS_CMD_ID_MAC = 0x10A,
    HKS_CMD_ID_MAC_INIT = 0x10B,
    HKS_CMD_ID_MAC_UPDATE = 0x10C,
    HKS_CMD_ID_MAC_FINAL = 0x10D,
    HKS_CMD_ID_ENCRYPT = 0x10E,
    HKS_CMD_ID_ENCRYPT_INIT = 0x10F,
    HKS_CMD_ID_ENCRYPT_UPDATE = 0x110,
    HKS_CMD_ID_ENCRYPT_FINAL = 0x111,
    HKS_CMD_ID_DECRYPT = 0x112,
    HKS_CMD_ID_DECRYPT_INIT = 0x113,
    HKS_CMD_ID_DECRYPT_UPDATE = 0x114,
    HKS_CMD_ID_DECRYPT_FINAL = 0x115,
    HKS_CMD_ID_CHECK_KEY_LEGALITY = 0x116,
    HKS_CMD_ID_GENERATE_RANDOM = 0x117,
    HKS_CMD_ID_AGREE_KEY = 0x118,
    HKS_CMD_ID_DERIVE_KEY = 0x119,

    HKS_CMD_ID_WRAP = 0x1000,
    HKS_CMD_ID_UNWRAP = 0x1001,

    HKS_CMD_ID_SIGN_WITH_DEV_KEY = 0x00010103,

    HKS_CMD_ID_GET_FP_AUTH_RESULT_QCOM = 0x00020500,

    HKS_CMD_ID_INJECT_KEY = 0x00010015,
    HKS_CMD_ID_INJECT_KEY_VERIFY = 0x00010016,
};

#endif
