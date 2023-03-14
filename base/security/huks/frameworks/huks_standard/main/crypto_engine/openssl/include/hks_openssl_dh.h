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

#ifndef HKS_OPENSSL_DH_H
#define HKS_OPENSSL_DH_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HKS_SUPPORT_DH_C
#ifdef HKS_SUPPORT_DH_GENERATE_KEY
int32_t HksOpensslDhGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif /* HKS_SUPPORT_DH_GENERATE_KEY */

#ifdef HKS_SUPPORT_DH_GET_PUBLIC_KEY
int32_t HksOpensslGetDhPubKey(const struct HksBlob *input, struct HksBlob *output);
#endif /* HKS_SUPPORT_DH_GET_PUBLIC_KEY */

#ifdef HKS_SUPPORT_DH_AGREE_KEY
int32_t HksOpensslDhAgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey);
#endif /* HKS_SUPPORT_DH_AGREE_KEY */
#endif /* HKS_SUPPORT_DH_C */

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_DH_H */