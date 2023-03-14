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

#ifndef HKS_ABILITY_H
#define HKS_ABILITY_H

#include "hks_type_inner.h"

enum HksAbilityType {
    HKS_ABILITY_CRYPTO = 1,
    HKS_ABILITY_AUTH,
};

enum HksOperationType {
    HKS_OPERATION_GENERATE_KEY = 0x1,    /* generate key */
    HKS_OPERATION_GET_PUBLIC_KEY = 0x2,  /* get public key */
    HKS_OPERATION_DERIVE_KEY = 0x3,      /* derive key */
    HKS_OPERATION_AGREE_KEY = 0x4,       /* agree key */
    HKS_OPERATION_SIGN = 0x5,            /* sign */
    HKS_OPERATION_VERIFY = 0x6,          /* verify */
    HKS_OPERATION_HMAC_INIT = 0x7,       /* hmac init */
    HKS_OPERATION_HMAC_UPDATE = 0x8,     /* hmac update */
    HKS_OPERATION_HMAC_FINAL = 0x9,      /* hmac final */
    HKS_OPERATION_HASH_INIT = 0xa,       /* hash init */
    HKS_OPERATION_HASH_UPDATE = 0xb,     /* hash update */
    HKS_OPERATION_HASH_FINAL = 0xc,      /* hash final */
    HKS_OPERATION_ENCRYPT_INIT = 0xd,    /* encrypt init */
    HKS_OPERATION_ENCRYPT_UPDATE = 0xe,  /* encrypt update */
    HKS_OPERATION_ENCRYPT_FINAL = 0xf,   /* encrypt final */
    HKS_OPERATION_DECRYPT_INIT = 0x10,   /* decrypt init */
    HKS_OPERATION_DECRYPT_UPDATE = 0x11, /* decrypt update */
    HKS_OPERATION_DECRYPT_FINAL = 0x12,  /* decrypt final */
    HKS_OPERATION_GET_MAIN_KEY = 0x13,   /* get root main key */
    HKS_OPERATION_FILL_RANDOM = 0x14,    /* fill random */
    HKS_OPERATION_HMAC = 0x15,
    HKS_OPERATION_HASH = 0x16,
    HKS_OPERATION_ENCRYPT = 0x17,
    HKS_OPERATION_DECRYPT = 0x18,
    HKS_OPERATION_BN_EXP_MOD = 0x19,
    HKS_OPERATION_HASH_FREE_CTX = 0x1a,      /* hash free ctx */
    HKS_OPERATION_HMAC_FREE_CTX = 0x1b,      /* hmac free ctx */
    HKS_OPERATION_ENCRYPT_FREE_CTX = 0x1c,   /* encrypt free ctx */
    HKS_OPERATION_DECRYPT_FREE_CTX = 0x1d,   /* decrypt free ctx */
};

struct HksAbility {
    uint32_t id;
    void *func;
};

#define HKS_ABILITY_MAX_SIZE 128

#define HKS_ABILITY_SHIFT 24
#define HKS_CRYPTO_OPERATION_SHIFT 16

#define HKS_CRYPTO_ABILITY(type, alg) \
    ((HKS_ABILITY_CRYPTO << HKS_ABILITY_SHIFT) | ((type) << HKS_CRYPTO_OPERATION_SHIFT) | (alg))

#define HKS_CRYPTO_ABILITY_GENERATE_KEY(alg)    HKS_CRYPTO_ABILITY(HKS_OPERATION_GENERATE_KEY, alg)
#define HKS_CRYPTO_ABILITY_GET_PUBLIC_KEY(alg)  HKS_CRYPTO_ABILITY(HKS_OPERATION_GET_PUBLIC_KEY, alg)
#define HKS_CRYPTO_ABILITY_DERIVE_KEY(alg)      HKS_CRYPTO_ABILITY(HKS_OPERATION_DERIVE_KEY, alg)
#define HKS_CRYPTO_ABILITY_AGREE_KEY(alg)       HKS_CRYPTO_ABILITY(HKS_OPERATION_AGREE_KEY, alg)
#define HKS_CRYPTO_ABILITY_SIGN(alg)            HKS_CRYPTO_ABILITY(HKS_OPERATION_SIGN, alg)
#define HKS_CRYPTO_ABILITY_VERIFY(alg)          HKS_CRYPTO_ABILITY(HKS_OPERATION_VERIFY, alg)

#define HKS_CRYPTO_ABILITY_HMAC                 HKS_CRYPTO_ABILITY(HKS_OPERATION_HMAC, 0)
#define HKS_CRYPTO_ABILITY_HMAC_INIT            HKS_CRYPTO_ABILITY(HKS_OPERATION_HMAC_INIT, 0)
#define HKS_CRYPTO_ABILITY_HMAC_UPDATE          HKS_CRYPTO_ABILITY(HKS_OPERATION_HMAC_UPDATE, 0)
#define HKS_CRYPTO_ABILITY_HMAC_FINAL           HKS_CRYPTO_ABILITY(HKS_OPERATION_HMAC_FINAL, 0)
#define HKS_CRYPTO_ABILITY_HMAC_FREE_CTX        HKS_CRYPTO_ABILITY(HKS_OPERATION_HMAC_FREE_CTX, 0)

#define HKS_CRYPTO_ABILITY_HASH                 HKS_CRYPTO_ABILITY(HKS_OPERATION_HASH, 0)
#define HKS_CRYPTO_ABILITY_HASH_INIT            HKS_CRYPTO_ABILITY(HKS_OPERATION_HASH_INIT, 0)
#define HKS_CRYPTO_ABILITY_HASH_UPDATE          HKS_CRYPTO_ABILITY(HKS_OPERATION_HASH_UPDATE, 0)
#define HKS_CRYPTO_ABILITY_HASH_FINAL           HKS_CRYPTO_ABILITY(HKS_OPERATION_HASH_FINAL, 0)
#define HKS_CRYPTO_ABILITY_HASH_FREE_CTX        HKS_CRYPTO_ABILITY(HKS_OPERATION_HASH_FREE_CTX, 0)

#define HKS_CRYPTO_ABILITY_ENCRYPT(alg)         HKS_CRYPTO_ABILITY(HKS_OPERATION_ENCRYPT, alg)
#define HKS_CRYPTO_ABILITY_ENCRYPT_INIT(alg)    HKS_CRYPTO_ABILITY(HKS_OPERATION_ENCRYPT_INIT, alg)
#define HKS_CRYPTO_ABILITY_ENCRYPT_UPDATE(alg)  HKS_CRYPTO_ABILITY(HKS_OPERATION_ENCRYPT_UPDATE, alg)
#define HKS_CRYPTO_ABILITY_ENCRYPT_FINAL(alg)   HKS_CRYPTO_ABILITY(HKS_OPERATION_ENCRYPT_FINAL, alg)
#define HKS_CRYPTO_ABILITY_ENCRYPT_FREE_CTX(alg)   HKS_CRYPTO_ABILITY(HKS_OPERATION_ENCRYPT_FREE_CTX, alg)

#define HKS_CRYPTO_ABILITY_DECRYPT(alg)         HKS_CRYPTO_ABILITY(HKS_OPERATION_DECRYPT, alg)
#define HKS_CRYPTO_ABILITY_DECRYPT_INIT(alg)    HKS_CRYPTO_ABILITY(HKS_OPERATION_DECRYPT_INIT, alg)
#define HKS_CRYPTO_ABILITY_DECRYPT_UPDATE(alg)  HKS_CRYPTO_ABILITY(HKS_OPERATION_DECRYPT_UPDATE, alg)
#define HKS_CRYPTO_ABILITY_DECRYPT_FINAL(alg)   HKS_CRYPTO_ABILITY(HKS_OPERATION_DECRYPT_FINAL, alg)
#define HKS_CRYPTO_ABILITY_DECRYPT_FREE_CTX(alg)   HKS_CRYPTO_ABILITY(HKS_OPERATION_DECRYPT_FREE_CTX, alg)

#define HKS_CRYPTO_ABILITY_GET_MAIN_KEY         HKS_CRYPTO_ABILITY(HKS_OPERATION_GET_MAIN_KEY, 0)

#define HKS_CRYPTO_ABILITY_FILL_RANDOM          HKS_CRYPTO_ABILITY(HKS_OPERATION_FILL_RANDOM, 0)
#define HKS_CRYPTO_ABILITY_BN_EXP_MOD           HKS_CRYPTO_ABILITY(HKS_OPERATION_BN_EXP_MOD, 0)

#ifdef __cplusplus
extern "C" {
#endif

int32_t RegisterAbility(uint32_t id, void *func);

void *GetAbility(uint32_t id);

int32_t HksCryptoAbilityInit(void);

#ifdef __cplusplus
}
#endif

#endif /* HKS_ABILITY_H */
