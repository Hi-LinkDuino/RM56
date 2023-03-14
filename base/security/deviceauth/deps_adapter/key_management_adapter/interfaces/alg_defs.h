/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef ALG_DEFS_H
#define ALG_DEFS_H

#include "hc_types.h"
#include "string_util.h"

#define SHA256_LEN 32
#define HMAC_LEN 32
#define SIGNATURE_LEN 64
#define AE_TAG_LEN 16
#define BIG_PRIME_LEN_384 384
#define BIG_PRIME_LEN_256 256

typedef enum {
    PAIR_TYPE_BIND = 0,
    PAIR_TYPE_CLONE = 1,
    PAIR_TYPE_END
} PairType; // range: 0 ~ 2^8-1

typedef struct {
    Uint8Buff authId;
    int32_t userType;
    int32_t pairType;
} ExtraInfo;

typedef enum {
    ED25519 = 0,
    X25519 = 1,
    P256 = 2,
} Algorithm;

typedef enum {
    KEY_PURPOSE_MAC = 0,
    KEY_PURPOSE_DERIVE = 1,
} KeyPurpose;

typedef enum {
    CURVE_NONE,
    CURVE_256,
    CURVE_25519,
} CurveType;

typedef struct {
    uint8_t *nonce;
    uint32_t nonceLen;
    uint8_t *aad;
    uint32_t aadLen;
} GcmParam;

typedef struct {
    uint8_t *key;
    uint32_t keyLen;
    bool isAlias;
} KeyBuff;

typedef int32_t (*InitAlgFunc)(void);

typedef int32_t (*Sha256Func)(const Uint8Buff *message, Uint8Buff *hash);

typedef int32_t (*GenerateRandomFunc)(Uint8Buff *rand);

typedef int32_t (*ComputeHmacFunc)(const Uint8Buff *key, const Uint8Buff *message, Uint8Buff *outHmac, bool isAlias);

typedef int32_t (*ComputeHkdfFunc)(const Uint8Buff *baseKey, const Uint8Buff *salt, const Uint8Buff *keyInfo,
    Uint8Buff *outHkdf, bool isAlias);

typedef int32_t (*ImportSymmetricKeyFunc)(const Uint8Buff *keyAlias, const Uint8Buff *authToken, KeyPurpose purpose,
    const ExtraInfo *exInfo);

typedef int32_t (*CheckKeyExistFunc)(const Uint8Buff *keyAlias);
typedef int32_t (*DeleteKeyFunc)(const Uint8Buff *keyAlias);

typedef int32_t (*AesGcmEncryptFunc)(const Uint8Buff *key, const Uint8Buff *plain,
    const GcmParam *encryptInfo, bool isAlias, Uint8Buff *outCipher);
typedef int32_t (*AesGcmDecryptFunc)(const Uint8Buff *key, const Uint8Buff *cipher,
    const GcmParam *decryptInfo, bool isAlias, Uint8Buff *outPlain);

typedef int32_t (*GetTrustAuthIdListFunc)(const Uint8Buff *ownerAuthId, int32_t trustUserType,
    Uint8Buff *outAuthIdList, uint32_t *outCount);

typedef int32_t (*HashToPointFunc)(const Uint8Buff *hash, Algorithm algo, Uint8Buff *outEcPoint);

typedef int32_t (*AgreeSharedSecretWithStorageFunc)(const KeyBuff *priKey, const KeyBuff *pubKey, Algorithm algo,
    uint32_t sharedKeyLen, const Uint8Buff *sharedKeyAlias);

typedef int32_t (*AgreeSharedSecretFunc)(const KeyBuff *priKey, const KeyBuff *pubKey, Algorithm algo,
    Uint8Buff *sharedKey);

typedef int32_t (*BigNumExpModFunc)(const Uint8Buff *base, const Uint8Buff *exp, const char *bigNumHex,
    Uint8Buff *outNum);

typedef int32_t (*GenerateKeyPairWithStorageFunc)(const Uint8Buff *keyAlias, uint32_t keyLen, Algorithm algo,
    const ExtraInfo *exInfo);

typedef int32_t (*GenerateKeyPairFunc)(Algorithm algo, Uint8Buff *outPriKey, Uint8Buff *outPubKey);

typedef int32_t (*ExportPublicKeyFunc)(const Uint8Buff *keyAlias, Uint8Buff *outPubKey);

typedef int32_t (*SignFunc)(const Uint8Buff *keyAlias, const Uint8Buff *message, Algorithm algo,
    Uint8Buff *outSignature, bool isAlias);

typedef int32_t (*VerifyFunc)(const Uint8Buff *key, const Uint8Buff *message, Algorithm algo,
    const Uint8Buff *signature, bool isAlias);

typedef int32_t (*ImportPublicKeyFunc)(const Uint8Buff *keyAlias, const Uint8Buff *pubKey, Algorithm algo,
    const ExtraInfo *exInfo);

typedef bool (*CheckEcPublicKeyFunc)(const Uint8Buff *pubKey, Algorithm algo);

typedef bool (*CheckDlPublicKeyFunc)(const Uint8Buff *key, const char *primeHex);

typedef int32_t (*BigNumCompareFunc)(const Uint8Buff *x, const Uint8Buff *y);

typedef struct {
    InitAlgFunc initAlg;
    Sha256Func sha256;
    GenerateRandomFunc generateRandom;
    ComputeHmacFunc computeHmac;
    ComputeHkdfFunc computeHkdf;
    ImportSymmetricKeyFunc importSymmetricKey;
    CheckKeyExistFunc checkKeyExist;
    DeleteKeyFunc deleteKey;
    AesGcmEncryptFunc aesGcmEncrypt;
    AesGcmDecryptFunc aesGcmDecrypt;
    HashToPointFunc hashToPoint;
    AgreeSharedSecretWithStorageFunc agreeSharedSecretWithStorage;
    AgreeSharedSecretFunc agreeSharedSecret;
    BigNumExpModFunc bigNumExpMod;
    GenerateKeyPairWithStorageFunc generateKeyPairWithStorage;
    GenerateKeyPairFunc generateKeyPair;
    ExportPublicKeyFunc exportPublicKey;
    SignFunc sign;
    VerifyFunc verify;
    ImportPublicKeyFunc importPublicKey;
    CheckDlPublicKeyFunc checkDlPublicKey;
    CheckEcPublicKeyFunc checkEcPublicKey;
    BigNumCompareFunc bigNumCompare;
} AlgLoader;

#endif