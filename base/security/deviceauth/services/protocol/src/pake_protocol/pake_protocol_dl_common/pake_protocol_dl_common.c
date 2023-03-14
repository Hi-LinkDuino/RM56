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

#include "pake_protocol_dl_common.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "pake_defs.h"
#include "protocol_common.h"

static const char * const g_largePrimeNumberHex384 =
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74"\
    "020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F1437"\
    "4FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"\
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF05"\
    "98DA48361C55D39A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB"\
    "9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"\
    "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"\
    "3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D04507A33"\
    "A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7DB3970F85A6E1E4C7"\
    "ABF5AE8CDB0933D71E8C94E04A25619DCEE3D2261AD2EE6BF12FFA06D98A0864"\
    "D87602733EC86A64521F2B18177B200CBBE117577A615D6C770988C0BAD946E2"\
    "08E24FA074E5AB3143DB5BFCE0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF";

static const char * const g_largePrimeNumberHex256 =
    "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74"\
    "020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F1437"\
    "4FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED"\
    "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF05"\
    "98DA48361C55D39A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB"\
    "9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B"\
    "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"\
    "3995497CEA956AE515D2261898FA051015728E5A8AACAA68FFFFFFFFFFFFFFFF";

uint32_t GetPakeDlAlg(void)
{
    return PAKE_ALG_DL;
}

static int32_t GenerateEsk(PakeBaseParams *params)
{
    int res = params->loader->generateRandom(&(params->eskSelf));
    if (res != HC_SUCCESS) {
        LOGE("GenerateRandom for eskSelf failed, res: %x.", res);
    }
    return res;
}

static int FillDlKeysLenAccordingToEpkPeer(PakeBaseParams *params)
{
    if ((params->epkPeer.length == PAKE_DL_PRIME_LEN) &&
        (((uint32_t)params->supportedDlPrimeMod & DL_PRIME_MOD_384) != 0)) {
        params->eskSelf.length = PAKE_DL_ESK_LEN;
        params->innerKeyLen = PAKE_DL_PRIME_LEN;
        return HC_SUCCESS;
    }
    if ((params->epkPeer.length == PAKE_DL_PRIME_SMALL_LEN) &&
        (((uint32_t)params->supportedDlPrimeMod & DL_PRIME_MOD_256) != 0)) {
        params->eskSelf.length = PAKE_DL_ESK_SMALL_LEN;
        params->innerKeyLen = PAKE_DL_PRIME_SMALL_LEN;
        return HC_SUCCESS;
    }
    LOGE("PAKE DL mod: %x, Invalid epkPeer length: %u.", params->supportedDlPrimeMod, params->epkPeer.length);
    return HC_ERR_INVALID_LEN;
}

static int FillDlKeysLenAccordingToMod(PakeBaseParams *params)
{
    if (((uint32_t)params->supportedDlPrimeMod & DL_PRIME_MOD_384) != 0) {
        params->eskSelf.length = PAKE_DL_ESK_LEN;
        params->innerKeyLen = PAKE_DL_PRIME_LEN;
        return HC_SUCCESS;
    }
    if (((uint32_t)params->supportedDlPrimeMod & DL_PRIME_MOD_256) != 0) {
        params->eskSelf.length = PAKE_DL_ESK_SMALL_LEN;
        params->innerKeyLen = PAKE_DL_PRIME_SMALL_LEN;
        return HC_SUCCESS;
    }
    LOGE("Unsupported PAKE DL mod: %x.", params->supportedDlPrimeMod);
    return HC_ERR_NOT_SUPPORT;
}

static int32_t InitDlPakeParams(PakeBaseParams *params)
{
    int res;
    if (params->isClient) {
        res = FillDlKeysLenAccordingToEpkPeer(params);
    } else {
        res = FillDlKeysLenAccordingToMod(params);
    }
    if (res != HC_SUCCESS) {
        LOGE("FillDlKeysLen failed, res: %x.", res);
        return res;
    }
    res = InitSingleParam(&(params->eskSelf), params->eskSelf.length);
    if (res !=  HC_SUCCESS) {
        LOGE("InitSingleParam for eskSelf failed, res: %x.", res);
        return res;
    }
    res = InitSingleParam(&(params->epkSelf), params->innerKeyLen);
    if (res !=  HC_SUCCESS) {
        LOGE("InitSingleParam for epkSelf failed, res: %x.", res);
        return res;
    }
    res = InitSingleParam(&(params->base), params->innerKeyLen);
    if (res !=  HC_SUCCESS) {
        LOGE("InitSingleParam for base failed, res: %x.", res);
        return res;
    }
    return res;
}

int32_t GenerateDlPakeParams(PakeBaseParams *params, const Uint8Buff *secret)
{
    int32_t res = InitDlPakeParams(params);
    if (res != HC_SUCCESS) {
        LOGE("InitDlPakeParams failed, res: %x.", res);
        goto CLEAN_UP;
    }
    res = GenerateEsk(params);
    if (res != HC_SUCCESS) {
        LOGE("GenerateEsk failed, res: %x.", res);
        goto CLEAN_UP;
    }
    uint8_t expVal[PAKE_DL_EXP_LEN] = { 2 };
    Uint8Buff exp = { expVal, PAKE_DL_EXP_LEN };
    params->largePrimeNumHex = (params->innerKeyLen == PAKE_DL_PRIME_SMALL_LEN) ?
        g_largePrimeNumberHex256 : g_largePrimeNumberHex384;
    res = params->loader->bigNumExpMod(secret, &exp, params->largePrimeNumHex, &params->base);
    if (res != HC_SUCCESS) {
        LOGE("BigNumExpMod for base failed, res: %x.", res);
        goto CLEAN_UP;
    }

    res = params->loader->bigNumExpMod(&params->base, &(params->eskSelf), params->largePrimeNumHex, &(params->epkSelf));
    if (res != HC_SUCCESS) {
        LOGE("BigNumExpMod for epkSelf failed, res: %x.", res);
        goto CLEAN_UP;
    }
    return res;
CLEAN_UP:
    CleanPakeSensitiveKeys(params);
    return res;
}

static bool IsEpkPeerLenInvalid(PakeBaseParams *params)
{
    if ((params->epkPeer.length == PAKE_DL_PRIME_LEN) &&
        (((uint32_t)params->supportedDlPrimeMod & DL_PRIME_MOD_384) != 0)) {
        return false;
    }
    if ((params->epkPeer.length == PAKE_DL_PRIME_SMALL_LEN) &&
        (((uint32_t)params->supportedDlPrimeMod & DL_PRIME_MOD_256) != 0)) {
        return false;
    }
    LOGE("Invalid epkPeer length: %u.", params->epkPeer.length);
    return true;
}

int32_t AgreeDlSharedSecret(PakeBaseParams *params, Uint8Buff *sharedSecret)
{
    int res;
    if (IsEpkPeerLenInvalid(params)) {
        res = HC_ERR_INVALID_LEN;
        goto CLEAN_UP;
    }
    if (!params->loader->checkDlPublicKey(&(params->epkPeer), params->largePrimeNumHex)) {
        LOGE("CheckDlPublicKey failed.");
        res = HC_ERR_INVALID_PUBLIC_KEY;
        goto CLEAN_UP;
    }
    res = params->loader->bigNumExpMod(&(params->epkPeer), &(params->eskSelf), params->largePrimeNumHex, sharedSecret);
    if (res != HC_SUCCESS) {
        LOGE("BigNumExpMod for sharedSecret failed, res: %x.", res);
        goto CLEAN_UP;
    }
    return res;
CLEAN_UP:
    CleanPakeSensitiveKeys(params);
    return res;
}