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

#ifndef OPENSSL_DSA_SIGN_MT_HELPER_H
#define OPENSSL_DSA_SIGN_MT_HELPER_H

#include <openssl/evp.h>

#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BIT_NUM_OF_UINT8 8
#define DSA_FAILED 1
#define DSA_SUCCESS 0

EVP_PKEY *GenerateDsaKey(const uint32_t keySize);

bool DsaGetx509PubKey(EVP_PKEY *pkey, struct HksBlob *x509Key);

int32_t OpensslSignDsa(
    const struct HksBlob *plainText, struct HksBlob *signData, struct HksBlob *key, enum HksKeyDigest digestType);

int32_t OpensslVerifyDsa(
    const struct HksBlob *plainText, struct HksBlob *signData, struct HksBlob *key, enum HksKeyDigest digestType);

int32_t X509ToDsaPublicKey(struct HksBlob *x509Key, struct HksBlob *publicKey);

int32_t SaveDsaKeyToHksBlob(EVP_PKEY *pkey, const uint32_t keySize, struct HksBlob *key);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CIPHER_H