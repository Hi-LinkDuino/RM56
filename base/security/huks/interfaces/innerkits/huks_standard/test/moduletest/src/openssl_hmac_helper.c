/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "openssl_hmac_helper.h"

#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

int32_t HmacGenerateKey(int keyLen, struct HksBlob *key)
{
    uint32_t keySizeByte = (uint32_t)keyLen / BIT_NUM_OF_UINT8;

    uint8_t *tmpKey = (uint8_t *)malloc(keySizeByte);
    if (tmpKey == NULL) {
        return HMAC_FAILED;
    }

    if (RAND_bytes(tmpKey, keySizeByte) != 1) {
        free(tmpKey);
        return HMAC_FAILED;
    }

    key->data = tmpKey;
    key->size = keySizeByte;

    return HMAC_SUCCESS;
}

static const EVP_MD *GetAlg(uint32_t alg)
{
    switch (alg) {
        case HKS_DIGEST_SHA1:
            return EVP_sha1();
        case HKS_DIGEST_SHA224:
            return EVP_sha224();
        case HKS_DIGEST_SHA256:
            return EVP_sha256();
        case HKS_DIGEST_SHA384:
            return EVP_sha384();
        case HKS_DIGEST_SHA512:
            return EVP_sha512();
        default:
            return NULL;
    }
}

int32_t HmacHmac(const struct HksBlob *key, uint32_t digestAlg, const struct HksBlob *msg, struct HksBlob *mac)
{
    const EVP_MD *getDigestAlg = GetAlg(digestAlg);
    if (getDigestAlg == NULL) {
        return HMAC_FAILED;
    }
    if (HMAC(getDigestAlg, key->data, (int32_t)key->size, msg->data, msg->size, mac->data, &mac->size) == NULL) {
        return HMAC_FAILED;
    }
    return HMAC_SUCCESS;
}