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

#include "pake_protocol_ec_common.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "pake_defs.h"
#include "protocol_common.h"

#define PAKE_PRIVATE_KEY_AND_MASK_HIGH 0xF8
#define PAKE_PRIVATE_KEY_AND_MASK_LOW  0x7F
#define PAKE_PRIVATE_KEY_OR_MASK_LOW   0x40

uint32_t GetPakeEcAlg(void)
{
    return PAKE_ALG_EC;
}

static int32_t GenerateEsk(PakeBaseParams *params)
{
    int32_t res;
    if (params->curveType == CURVE_256) {
        LOGE("CURVE_256 is not supported.");
        return HC_ERR_NOT_SUPPORT;
    } else { // CURVE_25519
        res = params->loader->generateRandom(&(params->eskSelf));
        if (res != HC_SUCCESS) {
            LOGE("CURVE_25519: GenerateRandom for eskSelf failed, res: %x.", res);
            return res;
        }
        params->eskSelf.val[PAKE_EC_KEY_LEN - 1] &= PAKE_PRIVATE_KEY_AND_MASK_HIGH;
        params->eskSelf.val[0] &= PAKE_PRIVATE_KEY_AND_MASK_LOW;
        params->eskSelf.val[0] |= PAKE_PRIVATE_KEY_OR_MASK_LOW;
    }
    return res;
}

static int32_t InitEcPakeParams(PakeBaseParams *params)
{
    params->eskSelf.length = PAKE_EC_KEY_LEN;
    params->innerKeyLen = PAKE_EC_KEY_LEN;
    /* P256 requires buffer for both X and Y coordinates. */
    uint32_t keyBufferLen = (params->curveType == CURVE_256) ? (params->innerKeyLen * 2) : (params->innerKeyLen);
    int32_t res = InitSingleParam(&(params->eskSelf), params->eskSelf.length);
    if (res !=  HC_SUCCESS) {
        LOGE("InitSingleParam for eskSelf failed, res: %x.", res);
        return res;
    }
    res = InitSingleParam(&(params->epkSelf), keyBufferLen);
    if (res !=  HC_SUCCESS) {
        LOGE("InitSingleParam for epkSelf failed, res: %x.", res);
        return res;
    }
    res = InitSingleParam(&(params->base), keyBufferLen);
    if (res !=  HC_SUCCESS) {
        LOGE("InitSingleParam for base failed, res: %x.", res);
        return res;
    }
    return res;
}

int32_t GenerateEcPakeParams(PakeBaseParams *params, Uint8Buff *secret)
{
    int32_t res = InitEcPakeParams(params);
    if (res != HC_SUCCESS) {
        LOGE("InitEcPakeParams failed, res: %x.", res);
        goto CLEAN_UP;
    }

    res = GenerateEsk(params);
    if (res != HC_SUCCESS) {
        LOGE("GenerateEsk failed, res: %x.", res);
        goto CLEAN_UP;
    }

    Algorithm alg = (params->curveType == CURVE_256) ? P256 : X25519;
    res = params->loader->hashToPoint(secret, alg, &params->base);
    if (res != HC_SUCCESS) {
        LOGE("HashToPoint from secret to base failed, res: %x.", res);
        goto CLEAN_UP;
    }
    KeyBuff eskSelfBuff = { params->eskSelf.val, params->eskSelf.length, false };
    KeyBuff baseBuff = { params->base.val, params->base.length, false };
    res = params->loader->agreeSharedSecret(&eskSelfBuff, &baseBuff, alg, &params->epkSelf);
    if (res != HC_SUCCESS) {
        LOGE("AgreeSharedSecret failed, res: %x.", res);
        goto CLEAN_UP;
    }
    return res;
CLEAN_UP:
    CleanPakeSensitiveKeys(params);
    return res;
}

int32_t AgreeEcSharedSecret(PakeBaseParams *params, Uint8Buff *sharedSecret)
{
    if (params->curveType == CURVE_256) {
        LOGE("CURVE_256 is not supported.");
        CleanPakeSensitiveKeys(params);
        return HC_ERR_NOT_SUPPORT;
    }
    int32_t res;
    /* P256 requires buffer for both X and Y coordinates. */
    uint32_t validKeyBufferLen = (params->curveType == CURVE_256) ? (PAKE_EC_KEY_LEN * 2) : (PAKE_EC_KEY_LEN);
    if (params->epkPeer.length != validKeyBufferLen) {
        LOGE("Invalid epkPeer length: %u.", params->epkPeer.length);
        res = HC_ERR_INVALID_LEN;
        goto CLEAN_UP;
    }

    Algorithm alg = (params->curveType == CURVE_256) ? P256 : X25519;
    KeyBuff eskSelfBuff = { params->eskSelf.val, params->eskSelf.length, false };
    KeyBuff epkPeerBuff = { params->epkPeer.val, params->epkPeer.length, false };
    res = params->loader->agreeSharedSecret(&eskSelfBuff, &epkPeerBuff, alg, sharedSecret);
    if (res != HC_SUCCESS) {
        LOGE("AgreeSharedSecret failed, res: %x.", res);
        goto CLEAN_UP;
    }
    return res;
CLEAN_UP:
    CleanPakeSensitiveKeys(params);
    return res;
}