/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HKS_OPENSSL_BN_H
#define HKS_OPENSSL_BN_H

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include <openssl/bn.h>

#include "hks_type_inner.h"

struct HksBnExpModParams {
    BIGNUM *bnX;
    BIGNUM *bnA;
    BIGNUM *bnE;
    BIGNUM *bnN;
    BN_CTX *ctx;
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksOpensslBnExpMod(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n);

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_BN_H */
