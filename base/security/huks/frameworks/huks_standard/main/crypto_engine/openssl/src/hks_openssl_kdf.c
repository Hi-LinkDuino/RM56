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

#include "hks_openssl_kdf.h"

#include <openssl/evp.h>
#include <openssl/kdf.h>

#include "hks_crypto_hal.h"
#include "hks_log.h"
#include "hks_openssl_engine.h"

static const EVP_MD *GetDeriveDigestType(uint32_t digestAlg)
{
    switch (digestAlg) {
        case (HKS_DIGEST_SHA256):
            return EVP_sha256();
        case (HKS_DIGEST_SHA384):
            return EVP_sha384();
        case (HKS_DIGEST_SHA512):
            return EVP_sha512();
        default:
            return NULL;
    }
}

int32_t HksOpensslPbkdf2(const struct HksBlob *mainKey, const struct HksKeySpec *derivationSpec,
    struct HksBlob *derivedKey)
{
    struct HksKeyDerivationParam *deriveParam = (struct HksKeyDerivationParam *)derivationSpec->algParam;
    const EVP_MD *md = GetDeriveDigestType(deriveParam->digestAlg);
    if (PKCS5_PBKDF2_HMAC((char *)mainKey->data, mainKey->size, deriveParam->salt.data, deriveParam->salt.size,
        deriveParam->iterations, md, derivedKey->size, derivedKey->data) != 1) {
        HKS_LOG_E("derive pbkdf2 key using openssl interface failed");
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

int32_t HksOpensslHkdf(const struct HksBlob *mainKey, const struct HksKeySpec *derivationSpec,
    struct HksBlob *derivedKey)
{
    struct HksKeyDerivationParam *deriveParam = (struct HksKeyDerivationParam *)derivationSpec->algParam;
    const EVP_MD *md = GetDeriveDigestType(deriveParam->digestAlg);
    EVP_PKEY_CTX *pctx;
    pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
    int32_t ret = HKS_FAILURE;
    do {
        if (EVP_PKEY_derive_init(pctx) <= 0) {
            HksLogOpensslError();
            break;
        }
        if (EVP_PKEY_CTX_set_hkdf_md(pctx, md) <= 0) {
            HksLogOpensslError();
            break;
        }
        if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, deriveParam->salt.data, deriveParam->salt.size) <= 0) {
            HksLogOpensslError();
            break;
        }
        if (EVP_PKEY_CTX_set1_hkdf_key(pctx, mainKey->data, mainKey->size) <= 0) {
            HksLogOpensslError();
            break;
        }
        if (EVP_PKEY_CTX_add1_hkdf_info(pctx, deriveParam->info.data, deriveParam->info.size) <= 0) {
            HksLogOpensslError();
            break;
        }
        if (EVP_PKEY_derive(pctx, derivedKey->data, (size_t *)&derivedKey->size) <= 0) {
            HksLogOpensslError();
            break;
        }
        ret = HKS_SUCCESS;
    } while (0);
    EVP_PKEY_CTX_free(pctx);
    return ret;
}
