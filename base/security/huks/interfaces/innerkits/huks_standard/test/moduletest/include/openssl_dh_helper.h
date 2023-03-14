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

#ifndef OPENSSL_DH_HELPER_H
#define OPENSSL_DH_HELPER_H

#include "hks_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DH_FAILED 1
#define DH_SUCCESS 0

int32_t DhGenerateKey(const int keyLen, struct HksBlob *key);

int32_t DhAgreeKey(
    const int keyLen, const struct HksBlob *nativeKey, const struct HksBlob *pubKey, struct HksBlob *sharedKey);

int32_t DhGetDhPubKey(const struct HksBlob *input, struct HksBlob *output);

int32_t DhX509ToHksBlob(const struct HksBlob *x509Key, struct HksBlob *publicKey);

int32_t DhHksBlobToX509(const struct HksBlob *key, struct HksBlob *x509Key);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // CIPHER_H