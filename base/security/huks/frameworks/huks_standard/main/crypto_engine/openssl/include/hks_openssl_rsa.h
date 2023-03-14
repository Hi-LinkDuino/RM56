/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef HKS_OPENSSL_RSA_H
#define HKS_OPENSSL_RSA_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HKS_RSA_KEYPAIR_CNT 3

#ifdef HKS_SUPPORT_RSA_C
#ifdef HKS_SUPPORT_RSA_GENERATE_KEY
int32_t HksOpensslRsaGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif /* HKS_SUPPORT_RSA_GENERATE_KEY */

#ifdef HKS_SUPPORT_RSA_GET_PUBLIC_KEY
int32_t HksOpensslGetRsaPubKey(const struct HksBlob *input, struct HksBlob *output);
#endif /* HKS_SUPPORT_RSA_GET_PUBLIC_KEY */

#ifdef HKS_SUPPORT_RSA_CRYPT
int32_t HksOpensslRsaEncrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText, struct HksBlob *tagAead);

int32_t HksOpensslRsaDecrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText);
#endif /* HKS_SUPPORT_RSA_CRYPT */

#ifdef HKS_SUPPORT_RSA_SIGN_VERIFY
int32_t HksOpensslRsaSign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature);

int32_t HksOpensslRsaVerify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature);
#endif /* HKS_SUPPORT_RSA_SIGN_VERIFY */

#endif /* HKS_SUPPORT_RSA_C */

#ifdef __cplusplus
}
#endif
#endif
