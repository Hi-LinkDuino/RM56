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

#ifndef OPENSSL_ECC_HELPER_H
#define OPENSSL_ECC_HELPER_H

#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ECC_KEYPAIR_CNT 3
#define ECC_FAILED 1
#define ECC_SUCCESS 0
#define ECC_KEY_SIZE 528
#define ECC_MESSAGE_SIZE 141

int32_t EccGenerateKey(const int keyLen, struct HksBlob *key);

int32_t EcdsaSign(const struct HksBlob *key, int digest, const struct HksBlob *message, struct HksBlob *signature);

int32_t EcdsaVerify(
    const struct HksBlob *key, int digest, const struct HksBlob *message, const struct HksBlob *signature);

int32_t GetEccPubKey(const struct HksBlob *input, struct HksBlob *output);

int32_t X509ToHksBlob(const struct HksBlob *x509Key, struct HksBlob *publicKey);

int32_t HksBlobToX509(const struct HksBlob *key, struct HksBlob *x509Key);

int32_t EcdhAgreeKey(
    const int keyLen, const struct HksBlob *nativeKey, const struct HksBlob *pubKey, struct HksBlob *sharedKey);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CIPHER_H