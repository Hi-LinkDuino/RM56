/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef HKS_CRYPTO_ED25519_H
#define HKS_CRYPTO_ED25519_H

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "hks_crypto_hal.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HKS_SUPPORT_ED25519_C) || defined(HKS_SUPPORT_ED25519_GENERATE_KEY)
int32_t HksEd25519GenerateKey(const struct HksKeySpec *spec, struct HksBlob *keyOut);
#endif

#if defined(HKS_SUPPORT_ED25519_C) || defined(HKS_SUPPORT_ED2519_GET_PUBLIC_KEY)
int32_t HksGetEd25519PubKey(const struct HksBlob *input, struct HksBlob *output);
#endif

#if defined(HKS_SUPPORT_ED25519_C) || defined(HKS_SUPPORT_ED25519_SIGN_VERIFY)
int32_t HksEd25519Sign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature);

int32_t HksEd25519Verify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature);
#endif

#ifdef __cplusplus
}
#endif

#endif
