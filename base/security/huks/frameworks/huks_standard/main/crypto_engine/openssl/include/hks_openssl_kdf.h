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

#ifndef HKS_OPENSSL_KDF_H
#define HKS_OPENSSL_KDF_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HKDF_EXPAND_MAX 0xFF

int32_t HksOpensslHkdf(const struct HksBlob *masterKey, const struct HksKeySpec *derivationSpec,
    struct HksBlob *derivedKey);

int32_t HksOpensslPbkdf2(const struct HksBlob *mainKey, const struct HksKeySpec *derivationSpec,
    struct HksBlob *derivedKey);

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_KDF_H */
