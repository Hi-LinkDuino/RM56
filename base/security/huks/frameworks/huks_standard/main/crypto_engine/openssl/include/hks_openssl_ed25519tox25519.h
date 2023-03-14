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

#ifndef HKS_OPENSSL_ED25519TOX25519_H
#define HKS_OPENSSL_ED25519TOX25519_H

#include <openssl/bn.h>

#include "hks_crypto_hal.h"

struct Curve25519Structure {
    BIGNUM *p; // Curve25519 prime
    BIGNUM *d; // A non-zero element in the finite filed
    BIGNUM *negativeTwo;
    BIGNUM *negativeOne;
    BIGNUM *negativeOneDivideTwo;
    BIGNUM *ed25519Pubkey;
};
struct Curve25519Var {
    BIGNUM *a;
    BIGNUM *b;
    BIGNUM *c;
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t ConvertPubkeyX25519FromED25519(const struct HksBlob *keyIn, struct HksBlob *keyOut);

int32_t ConvertPrivX25519FromED25519(const struct HksBlob *keyIn, struct HksBlob *keyOut);

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_ED25519TOX25519_H */
