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

#ifndef PAKE_DEFS_H
#define PAKE_DEFS_H

#include "alg_defs.h"
#include "string_util.h"

#define HICHAIN_SPEKE_BASE_INFO "hichain_speke_base_info"
#define HICHAIN_SPEKE_SESSIONKEY_INFO "hichain_speke_sessionkey_info"
#define SHARED_SECRET_DERIVED_FACTOR "hichain_speke_shared_secret_info"

#define PAKE_SALT_LEN 16
#define PAKE_CHALLENGE_LEN 16
#define PAKE_SECRET_LEN 32
#define PAKE_HMAC_KEY_LEN 32
#define PAKE_EC_KEY_LEN 32
#define PAKE_DL_EXP_LEN 1
#define PAKE_DL_ESK_SMALL_LEN 28
#define PAKE_DL_ESK_LEN 32
#define PAKE_DL_PRIME_SMALL_LEN 256
#define PAKE_DL_PRIME_LEN 384

typedef enum {
    PAKE_ALG_NONE = 0x0000,
    PAKE_ALG_DL = 0x0001,
    PAKE_ALG_EC = 0x0002,
} PakeAlgType;

typedef enum {
    DL_PRIME_MOD_NONE = 0x0000,
    DL_PRIME_MOD_256 = 0x0001,
    DL_PRIME_MOD_384 = 0x0002,
} PakeDlPrimeMod;

typedef struct PakeBaseParamsT {
    Uint8Buff salt;
    Uint8Buff psk;
    Uint8Buff challengeSelf;
    Uint8Buff challengePeer;
    Uint8Buff base;
    Uint8Buff eskSelf;
    Uint8Buff epkSelf;
    Uint8Buff epkPeer;
    Uint8Buff idSelf;
    Uint8Buff idPeer;
    Uint8Buff sessionKey;
    Uint8Buff sharedSecret;
    Uint8Buff hmacKey;
    Uint8Buff kcfData;
    Uint8Buff kcfDataPeer;
    uint32_t innerKeyLen;
    const char *largePrimeNumHex;
    PakeDlPrimeMod supportedDlPrimeMod; // default: DL_PRIME_MOD_NONE
    CurveType curveType; // default: CURVE_NONE
    PakeAlgType supportedPakeAlg;
    bool isClient;

    const AlgLoader *loader;
} PakeBaseParams;

void CleanPakeSensitiveKeys(PakeBaseParams *params);

#endif