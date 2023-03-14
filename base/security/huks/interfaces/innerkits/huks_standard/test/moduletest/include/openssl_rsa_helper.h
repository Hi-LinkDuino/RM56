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

#ifndef OPENSSL_RSA_MT_HELPER_H
#define OPENSSL_RSA_MT_HELPER_H

#include <openssl/evp.h>

#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BIT_NUM_OF_UINT8 8
#define RSA_FAILED (-1)
#define RSA_SUCCESS 0

int32_t SaveRsaKeyToHksBlob(EVP_PKEY *pkey, const uint32_t keySize, struct HksBlob *key);

EVP_PKEY *GenerateRsaKey(const uint32_t keySize);

bool OpensslGetx509PubKey(EVP_PKEY *pkey, struct HksBlob *x509Key);

int32_t X509ToRsaPublicKey(struct HksBlob *x509Key, struct HksBlob *publicKey);

int32_t EncryptRsa(const struct HksBlob *inData, struct HksBlob *outData, struct HksBlob *key, int padding,
    enum HksKeyDigest digestType);

int32_t DecryptRsa(const struct HksBlob *inData, struct HksBlob *outData, struct HksBlob *key, int padding,
    enum HksKeyDigest digestType);

int32_t OpensslSignRsa(const struct HksBlob *plainText, struct HksBlob *signData, struct HksBlob *key, int padding,
    enum HksKeyDigest digestType);

int32_t OpensslVerifyRsa(const struct HksBlob *plainText, struct HksBlob *signData, struct HksBlob *key, int padding,
    enum HksKeyDigest digestType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CIPHER_H