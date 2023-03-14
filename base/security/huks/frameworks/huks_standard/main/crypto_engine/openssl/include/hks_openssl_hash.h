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

#ifndef HKS_OPENSSL_HASH_H
#define HKS_OPENSSL_HASH_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksOpensslHash(uint32_t alg, const struct HksBlob *msg, struct HksBlob *hash);

int32_t HksOpensslHashInit(void **cryptoCtx, uint32_t alg);

int32_t HksOpensslHashUpdate(void *cryptoCtx, const struct HksBlob *msg);

int32_t HksOpensslHashFinal(void **cryptoCtx, const struct HksBlob *msg, struct HksBlob *hash);

void HksOpensslHashFreeCtx(void **cryptoCtx);
#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_HASH_H */