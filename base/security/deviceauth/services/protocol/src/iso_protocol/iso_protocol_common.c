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

#include "iso_protocol_common.h"
#include "alg_loader.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"

int32_t InitIsoBaseParams(IsoBaseParams *params)
{
    if (params == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }

    int32_t res;
    params->randSelf.length = RAND_BYTE_LEN;
    params->randSelf.val = (uint8_t *)HcMalloc(params->randSelf.length, 0);
    if (params->randSelf.val == NULL) {
        LOGE("Malloc randSelf failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto CLEAN_UP;
    }
    params->randPeer.length = RAND_BYTE_LEN;
    params->randPeer.val = (uint8_t *)HcMalloc(params->randPeer.length, 0);
    if (params->randPeer.val == NULL) {
        LOGE("Malloc randPeer failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto CLEAN_UP;
    }

    params->sessionKey.length = ISO_SESSION_KEY_LEN;
    params->sessionKey.val = (uint8_t *)HcMalloc(params->sessionKey.length, 0);
    if (params->sessionKey.val == NULL) {
        LOGE("Malloc sessionKey failed.");
        res = HC_ERR_ALLOC_MEMORY;
        goto CLEAN_UP;
    }

    params->loader = GetLoaderInstance();
    if (params->loader == NULL) {
        res = HC_ERROR;
        goto CLEAN_UP;
    }

    return HC_SUCCESS;
CLEAN_UP:
    DestroyIsoBaseParams(params);
    return res;
}

void DestroyIsoBaseParams(IsoBaseParams *params)
{
    if (params == NULL) {
        return;
    }

    FreeAndCleanKey(&params->sessionKey);
    (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);

    HcFree(params->randSelf.val);
    params->randSelf.val = NULL;

    HcFree(params->randPeer.val);
    params->randPeer.val = NULL;

    HcFree(params->authIdSelf.val);
    params->authIdSelf.val = NULL;

    HcFree(params->authIdPeer.val);
    params->authIdPeer.val = NULL;
}

static int IsoCalSelfToken(const IsoBaseParams *params, Uint8Buff *outHmac)
{
    int res;
    int length = params->randSelf.length + params->randPeer.length + params->authIdSelf.length +
        params->authIdPeer.length;
    uint8_t *messagePeer = (uint8_t *)HcMalloc(length, 0);
    if (messagePeer == NULL) {
        LOGE("Malloc for messagePeer failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    int usedLen = 0;
    if (memcpy_s(messagePeer, length, params->randPeer.val, params->randPeer.length) != EOK) {
        LOGE("Memcpy randPeer failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    usedLen += params->randPeer.length;
    if (memcpy_s(messagePeer + usedLen, length - usedLen, params->randSelf.val, params->randSelf.length) != EOK) {
        LOGE("Memcpy randSelf failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    usedLen += params->randSelf.length;
    if (memcpy_s(messagePeer + usedLen, length - usedLen, params->authIdSelf.val, params->authIdSelf.length) != EOK) {
        LOGE("Memcpy authIdSelf failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    usedLen += params->authIdSelf.length;
    if (memcpy_s(messagePeer + usedLen, length - usedLen, params->authIdPeer.val, params->authIdPeer.length) != EOK) {
        LOGE("Memcpy authIdPeer failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    Uint8Buff messageBuf = { messagePeer, length };
    Uint8Buff pskBuf = { (uint8_t *)params->psk, sizeof(params->psk) };
    res = params->loader->computeHmac(&pskBuf, &messageBuf, outHmac, false);
    if (res != HC_SUCCESS) {
        LOGE("ComputeHmac failed, res: %x.", res);
        goto CLEAN_UP;
    }
CLEAN_UP:
    HcFree(messagePeer);
    return res;
}

static int IsoCalPeerToken(const IsoBaseParams *params, Uint8Buff *selfToken)
{
    int length = params->randSelf.length + params->randPeer.length + params->authIdPeer.length +
        params->authIdSelf.length;
    uint8_t *messageSelf = (uint8_t *)HcMalloc(length, 0);
    if (messageSelf == NULL) {
        LOGE("Malloc for messageSelf failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    int res;
    int usedLen = 0;
    if (memcpy_s(messageSelf, length, params->randSelf.val, params->randSelf.length) != EOK) {
        LOGE("Memcpy randSelf failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    usedLen += params->randSelf.length;
    if (memcpy_s(messageSelf + usedLen, length - usedLen, params->randPeer.val, params->randPeer.length) != EOK) {
        LOGE("Memcpy randPeer failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    usedLen += params->randPeer.length;
    if (memcpy_s(messageSelf + usedLen, length - usedLen, params->authIdPeer.val, params->authIdPeer.length) != EOK) {
        LOGE("Memcpy authIdPeer failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    usedLen += params->authIdPeer.length;
    if (memcpy_s(messageSelf + usedLen, length - usedLen, params->authIdSelf.val, params->authIdSelf.length) != EOK) {
        LOGE("Memcpy authIdSelf failed.");
        res = HC_ERR_MEMORY_COPY;
        goto CLEAN_UP;
    }
    Uint8Buff messageBufSelf = { messageSelf, length };
    Uint8Buff pskBuf = { (uint8_t *)params->psk, sizeof(params->psk) };
    res = params->loader->computeHmac(&pskBuf, &messageBufSelf, selfToken, false);
    if (res != HC_SUCCESS) {
        LOGE("ComputeHmac for selfToken failed, res: %x.", res);
        goto CLEAN_UP;
    }
CLEAN_UP:
    HcFree(messageSelf);
    return res;
}

static int IsoCombineHkdfSalt(IsoBaseParams *params, Uint8Buff *hkdfSaltBuf, bool isClient)
{
    if (isClient) {
        if (memcpy_s(hkdfSaltBuf->val, hkdfSaltBuf->length, params->randSelf.val, params->randSelf.length) != EOK) {
            LOGE("Memcpy randSelf failed.");
            return HC_ERR_MEMORY_COPY;
        }
        if (memcpy_s(hkdfSaltBuf->val + params->randSelf.length, hkdfSaltBuf->length - params->randSelf.length,
            params->randPeer.val, params->randPeer.length) != EOK) {
            LOGE("Memcpy randPeer failed.");
            return HC_ERR_MEMORY_COPY;
        }
    } else {
        if (memcpy_s(hkdfSaltBuf->val, hkdfSaltBuf->length, params->randPeer.val, params->randPeer.length) != EOK) {
            LOGE("Memcpy randPeer failed.");
            return HC_ERR_MEMORY_COPY;
        }
        if (memcpy_s(hkdfSaltBuf->val + params->randPeer.length, hkdfSaltBuf->length - params->randPeer.length,
            params->randSelf.val, params->randSelf.length) != EOK) {
            LOGE("Memcpy randSelf failed.");
            return HC_ERR_MEMORY_COPY;
        }
    }
    return HC_SUCCESS;
}

static int IsoGenSessionKey(IsoBaseParams *params, Uint8Buff *pskBuf, bool isClient)
{
    int hkdfSaltLen = params->randPeer.length + params->randSelf.length;
    uint8_t *hkdfSalt = (uint8_t *)HcMalloc(hkdfSaltLen, 0);
    if (hkdfSalt == NULL) {
        LOGE("Malloc for hkdfSalt failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    Uint8Buff hkdfSaltBuf = { hkdfSalt, hkdfSaltLen };
    int res = IsoCombineHkdfSalt(params, &hkdfSaltBuf, isClient);
    if (res != HC_SUCCESS) {
        LOGE("IsoCombineHkdfSalt failed, res: %x.", res);
        HcFree(hkdfSalt);
        return res;
    }

    Uint8Buff keyInfoBuf = { (uint8_t *)GENERATE_SESSION_KEY_STR, HcStrlen(GENERATE_SESSION_KEY_STR) };
    res = params->loader->computeHkdf(pskBuf, &hkdfSaltBuf, &keyInfoBuf, &params->sessionKey, false);
    if (res != HC_SUCCESS) {
        LOGE("ComputeHkdf for sessionKey failed, res: %x.", res);
        FreeAndCleanKey(&params->sessionKey);
    }
    HcFree(hkdfSalt);
    return res;
}

int IsoClientGenRandom(IsoBaseParams *params)
{
    if (params == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    int32_t res = params->loader->generateRandom(&params->randSelf);
    if (res != HC_SUCCESS) {
        LOGE("Generate randSelf failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
    }
    return res;
}

int IsoClientCheckAndGenToken(IsoBaseParams *params, const Uint8Buff *peerToken, Uint8Buff *selfToken)
{
    if (params == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    if (peerToken == NULL || selfToken == NULL) {
        LOGE("Params is null.");
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return HC_ERR_NULL_PTR;
    }

    uint8_t hmacPeer[SHA256_LEN] = { 0 };
    Uint8Buff outHmac = { hmacPeer, sizeof(hmacPeer) };
    int res = IsoCalSelfToken(params, &outHmac);
    if (res != HC_SUCCESS) {
        LOGE("IsoCalSelfToken failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return res;
    }
    if (memcmp(peerToken->val, outHmac.val, outHmac.length) != 0) {
        LOGE("Compare hmac token failed.");
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return HC_ERR_PROOF_NOT_MATCH;
    }
    res = IsoCalPeerToken(params, selfToken);
    if (res != HC_SUCCESS) {
        LOGE("IsoCalPeerToken failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
    }
    return res;
}

int IsoClientGenSessionKey(IsoBaseParams *params, int returnResult, const uint8_t *hmac, uint32_t hmacLen)
{
    if (params == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    if (hmac == NULL) {
        LOGE("Params is null.");
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return HC_ERR_NULL_PTR;
    }

    Uint8Buff pskBuf = { params->psk, sizeof(params->psk) };
    Uint8Buff hmacMessage = { (uint8_t *)&returnResult, sizeof(int) };
    uint8_t hmacSelf[SHA256_LEN] = { 0 };
    Uint8Buff outHmacBuf = { hmacSelf, sizeof(hmacSelf) };
    int res = params->loader->computeHmac(&pskBuf, &hmacMessage, &outHmacBuf, false);
    if (res != HC_SUCCESS) {
        LOGE("ComputeHmac for returnResult failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return res;
    }
    if (memcmp(outHmacBuf.val, hmac, hmacLen) != 0) {
        LOGE("Compare hmac result failed.");
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return HC_ERR_PROOF_NOT_MATCH;
    }

    res = IsoGenSessionKey(params, &pskBuf, true);
    if (res != HC_SUCCESS) {
        LOGE("IsoGenSessionKey failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
    }

    return res;
}

int IsoServerGenRandomAndToken(IsoBaseParams *params, Uint8Buff *selfTokenBuf)
{
    if (params == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    if (selfTokenBuf == NULL) {
        LOGE("Params is null.");
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return HC_ERR_NULL_PTR;
    }
    int res = params->loader->generateRandom(&params->randSelf);
    if (res != HC_SUCCESS) {
        LOGE("Generate randSelf failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return res;
    }
    res = IsoCalPeerToken(params, selfTokenBuf);
    if (res != HC_SUCCESS) {
        LOGE("IsoCalPeerToken failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
    }
    return res;
}

int IsoServerGenSessionKeyAndCalToken(IsoBaseParams *params, const Uint8Buff *tokenFromPeer, Uint8Buff *tokenToPeer)
{
    if (params == NULL) {
        LOGE("Params is null.");
        return HC_ERR_NULL_PTR;
    }
    if (tokenFromPeer == NULL || tokenToPeer == NULL) {
        LOGE("Params is null.");
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return HC_ERR_NULL_PTR;
    }

    uint8_t hmacPeer[SHA256_LEN] = { 0 };
    Uint8Buff outHmac = { hmacPeer, sizeof(hmacPeer) };
    int res = IsoCalSelfToken(params, &outHmac);
    if (res != HC_SUCCESS) {
        LOGE("IsoCalSelfToken failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return res;
    }
    if (memcmp(tokenFromPeer->val, outHmac.val, outHmac.length) != 0) {
        LOGE("Compare hmac token failed.");
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return HC_ERR_PROOF_NOT_MATCH;
    }

    Uint8Buff pskBuf = { params->psk, sizeof(params->psk) };
    res = IsoGenSessionKey(params, &pskBuf, false);
    if (res != HC_SUCCESS) {
        LOGE("IsoGenSessionKey failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        return res;
    }

    int returnCode = 0;
    Uint8Buff messageBuf = { (uint8_t *)&returnCode, sizeof(int) };
    res = params->loader->computeHmac(&pskBuf, &messageBuf, tokenToPeer, false);
    if (res != HC_SUCCESS) {
        LOGE("Compute hmac for returnCode failed, res: %x.", res);
        (void)memset_s(params->psk, sizeof(params->psk), 0, PSK_LEN);
        FreeAndCleanKey(&params->sessionKey);
    }
    return res;
}