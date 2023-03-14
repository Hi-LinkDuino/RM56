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

#ifndef HKS_TEST_COMMON_H
#define HKS_TEST_COMMON_H

#include "hks_type.h"
#include "securec.h"

#define GOTO_ERROR_IF_FAIL(ret, err) \
    if ((ret) != 0) { \
        goto err; \
    }

#define GOTO_ERROR_IF_SUCCESS(ret, err) \
    if ((ret) == 0) { \
        goto err; \
    }

#define HKS_SLICE_MAX_LEN  64 * 1024

#define DEFAULT_TEST_CASE_ID 0xFFFFFFFF
#define DEFAULT_EXECUTION_TIMES 0xFFFFFFFF
#define DEFAULT_PERFORM_TIMES 0xFFFFFFFF

#define MAX_ARGS_LEN 20
#define DEFAULT_KEY_ALIAS_SIZE 64
#define DEFAULT_SLICE_SIZE (2 * 128 + 1)

#define DEFAULT_PARAM_SET_OUT_SIZE 4096
#define HKS_MAX_RANDOM_LEN 1024
#define MAX_OUT_BLOB_SIZE (5 * 1024 * 1024)

#define TEST_MAX_FILE_NAME_LEN 512

#define DEFAULT_LOCAL_KEY_SIZE 256

/* inputparams: struct HksBlob *blob */
struct HksTestBlobParams {
    bool blobExist;              /* blob null or not */
    uint32_t blobSize;           /* blob->size value */
    bool blobDataExist;          /* blob->data null or not */
    uint32_t blobDataSize;       /* blob->data malloc size */
};

/* inputparams: struct HksParamSet *paramSet */
struct HksTestGenKeyParamsParamSet {
    bool paramSetExist;          /* paramSet null or not */
    bool setAlg;
    uint32_t alg;
    bool setKeySize;
    uint32_t keySize;
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
    bool setPadding;
    uint32_t padding;
    bool setBlockMode;
    uint32_t mode;
    bool setKeyStorageFlag;
    uint32_t keyStorageFlag;
};

struct HksTestGenKeyParamsParamSetOut {
    bool paramSetExist;          /* paramSet null or not */
    uint32_t paramSetSize;
};

enum HksTestCipherType {
    HKS_TEST_CIPHER_TYPE_AES,
    HKS_TEST_CIPHER_TYPE_RSA,
};

struct TestRsaCipherParamSet {
    struct HksParamSet **outParamSet;
    bool paramSetExist;
    bool setAlg;
    uint32_t alg;
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
    bool setPadding;
    uint32_t padding;
};

struct HksTestCipherParamsParamSet {
    enum HksTestCipherType cipherType;
    bool paramSetExist;          /* paramSet null or not */
    bool setAlg;
    uint32_t alg;
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
    bool setPadding;
    uint32_t padding;
    bool setBlockMode;
    uint32_t mode;
    bool setIv;
    uint32_t ivSize;
    bool setNonce;
    uint32_t nonceSize;
    bool setAad;
    uint32_t aadSize;
    bool setIsKeyAlias;
    bool isKeyAlias;
};

struct HksTestMacParamSet {
    bool paramSetExist;          /* paramSet null or not */
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
};

struct HksTestDeriveParamSet {
    bool paramSetExist;          /* paramSet null or not */
    bool setAlg;
    uint32_t alg;
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
    bool setIteration;
    uint32_t iteration;
    bool setSalt;
    uint32_t saltSize;
    bool setInfo;
    uint32_t infoSize;
    bool setIsKeyAlias;
    bool isKeyAlias;
};

struct HksTestAgreeParamSet {
    bool paramSetExist;          /* paramSet null or not */
    bool setAlg;
    uint32_t alg;
    bool setKeySize;
    uint32_t keySize;
    bool setIsKeyAlias;
    bool isKeyAlias;
};

enum HksTestMacType {
    HKS_TEST_MAC_TYPE_REE,
    HKS_TEST_MAC_TYPE_TEE,
};

struct HksTestHashParamsParamSet {
    bool paramSetExist;          /* paramSet null or not */
    bool setDigest;
    uint32_t digest;
};

struct HksTestParamSetOut {
    bool paramSetExist;
    uint32_t paramSetSize;
    uint32_t realParamSetSize;
};

struct HksTestGenKeyParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestBlobParams keyAliasParams;
    struct HksTestGenKeyParamsParamSet paramSetParams;
    struct HksTestGenKeyParamsParamSetOut paramSetParamsOut;
};

struct HksTestCipherParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestBlobParams keyAliasParams;
    struct HksTestGenKeyParamsParamSet genKeyParamSetParams;
    struct HksTestGenKeyParamsParamSetOut genKeyParamSetParamsOut;
    struct HksTestCipherParamsParamSet encryptParamSetParams;
    struct HksTestCipherParamsParamSet decryptParamSetParams;
    struct HksTestBlobParams plainTextParams;
    struct HksTestBlobParams cipherTextParams;
    struct HksTestBlobParams decryptedTextParams;
    struct HksTestBlobParams localKeyParams;
};

struct HksTestEncryptParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestBlobParams keyAliasParams;
    struct HksTestGenKeyParamsParamSet genKeyParamSetParams;
    struct HksTestBlobParams encryptAliasParams;
    struct HksTestCipherParamsParamSet encryptParamSetParams;
    struct HksTestBlobParams inDataParams;
    struct HksTestBlobParams outDataParams;
    struct HksTestBlobParams localKeyParams;
};

struct HksTestDecryptParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestBlobParams keyAliasParams;
    struct HksTestGenKeyParamsParamSet genKeyParamSetParams;
    struct HksTestBlobParams decryptAliasParams;
    struct HksTestCipherParamsParamSet decryptParamSetParams;
    struct HksTestBlobParams inDataParams;
    struct HksTestBlobParams outDataParams;
    struct HksTestBlobParams localKeyParams;
};

struct HksTestMacParams {
    uint32_t testId;
    int32_t expectResult;
    enum HksTestMacType macType;
    struct HksTestBlobParams keyAliasParams;
    struct HksTestGenKeyParamsParamSet genKeyParamSetParams;
    struct HksTestBlobParams keyParams;
    struct HksTestMacParamSet macParamSetParams;
    struct HksTestBlobParams srcDataParams;
    struct HksTestBlobParams macParams;
};

struct HksTestDeriveParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestBlobParams keyAliasParams;
    struct HksTestGenKeyParamsParamSet genKeyParamSetParams;
    struct HksTestBlobParams masterKeyParams;
    struct HksTestDeriveParamSet deriveParamSetParams;
    struct HksTestBlobParams derivedKeyParams;
    struct HksTestBlobParams localKeyParams;
};

struct HksTestKeyExistParams {
    uint32_t testId;
    int32_t expectResult;
    bool isGenKey;
    struct HksTestBlobParams keyAliasParams;
};

struct HksTestGetKeyParamSetParams {
    uint32_t testId;
    int32_t expectResult;
    bool isGenKey;
    struct HksTestBlobParams keyAliasParams;
    struct HksTestParamSetOut paramSetOutParams;
};

struct HksTestGenRandomParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestBlobParams randomParams;
};

struct HksTestHashParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestHashParamsParamSet paramSetParams;
    struct HksTestBlobParams srcDataParams;
    struct HksTestBlobParams hashParams;
};

struct HksTestBnExpModParams {
    uint32_t testId;
    int32_t expectResult;
    bool isTestValue;
    struct HksTestBlobParams xParams;
    struct HksTestBlobParams aParams;
    struct HksTestBlobParams eParams;
    struct HksTestBlobParams nParams;
};

struct HksTestAgreeParams {
    uint32_t testId;
    int32_t expectResult;
    struct HksTestBlobParams keyAlias1Params;
    struct HksTestBlobParams keyAlias2Params;
    struct HksTestGenKeyParamsParamSet genKeyParamSetParams;
    struct HksTestBlobParams privateKeyParams;
    struct HksTestAgreeParamSet agreeParamSetParams;
    struct HksTestBlobParams peerPublicParams;
    struct HksTestBlobParams agreedKeyParams;
    struct HksTestBlobParams localPrivateKeyParams;
    struct HksTestBlobParams localPublicKeyParams;
};

struct TestAgreeParamSetStructure {
    struct HksParamSet **outParamSet;
    bool paramSetExist;
    bool setAlg;
    uint32_t alg;
    bool setKeySize;
    uint32_t keySize;
    bool setIsKeyAlias;
    bool isKeyAlias;
};

struct GenerateKeyParamSetStructure {
    struct HksParamSet **outParamSet;
    bool paramSetExist;
    bool setAlg;
    uint32_t alg;
    bool setKeySize;
    uint32_t keySize;
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
    bool setPadding;
    uint32_t padding;
    bool setBlockMode;
    uint32_t mode;
    bool setKeyStorageFlag;
    uint32_t keyStorageFlag;
};

struct AesCipherParamSetStructure {
    struct HksParamSet **outParamSet;
    bool paramSetExist;
    bool setAlg;
    uint32_t alg;
    bool setPurpose;
    uint32_t purpose;
    bool setPadding;
    uint32_t padding;
    bool setBlockMode;
    uint32_t mode;
    bool setIv;
    const struct HksBlob *ivBlob;
    bool setNonce;
    const struct HksBlob *nonceBlob;
    bool setAad;
    const struct HksBlob *aadBlob;
    bool setIsKeyAlias;
    bool isKeyAlias;
};

struct TestMacParamSetStructure {
    struct HksParamSet **outParamSet;
    bool paramSetExist;
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
    bool setKeyAlias;
    bool isKeyAlias;
};

struct TestDeriveParamSetStructure {
    struct HksParamSet **outParamSet;
    bool paramSetExist;
    bool setAlg;
    uint32_t alg;
    bool setPurpose;
    uint32_t purpose;
    bool setDigest;
    uint32_t digest;
    bool setIteration;
    uint32_t iteration;
    bool setSalt;
    const struct HksBlob *saltBlob;
    bool setInfo;
    const struct HksBlob *infoBlob;
    bool setIsKeyAlias;
    bool isKeyAlias;
};

#ifdef __cplusplus
extern "C" {
#endif

void TestFreeBlob(struct HksBlob **key);

int32_t TestConstuctBlob(struct HksBlob **blob,
    bool blobExist,
    uint32_t blobSize,
    bool blobDataExist,
    uint32_t realBlobDataSize);

int32_t TestConstructGenerateKeyParamSetOut(
    struct HksParamSet **outParamSet,
    bool paramSetExist, uint32_t paramSetSize);

int32_t TestConstructGenerateKeyParamSet(struct GenerateKeyParamSetStructure *paramStruct);

int32_t TestConstructRsaCipherParamSet(struct TestRsaCipherParamSet *paramStruct);

int32_t TestConstructAesCipherParamSet(struct AesCipherParamSetStructure *paramStruct);

int32_t TestConstructMacParamSet(struct TestMacParamSetStructure *paramStruct);

int32_t TestConstructAgreeParamSet(struct TestAgreeParamSetStructure *paramStruct);

int32_t GenerateLocalX25519Key(struct HksBlob **privateKey, struct HksBlob **publicKey,
    const struct HksTestBlobParams *localPrivateKeyParams, const struct HksTestBlobParams *localPublicKeyParams);

int32_t TestConstructDeriveParamSet(struct TestDeriveParamSetStructure *paramStruct);

int32_t TestConstructHashParamSet(
    struct HksParamSet **outParamSet,
    bool paramSetExist,
    bool setDigest, uint32_t digest);

int32_t GenerateKey(struct HksBlob **keyAlias, const struct HksTestBlobParams *keyAliasParams,
    const struct HksTestGenKeyParamsParamSet *genKeyParamSetParams,
    const struct HksTestGenKeyParamsParamSetOut *genKeyParamSetParamsOut);

int32_t GenerateLocalRandomKey(struct HksBlob **keyAlias, const struct HksTestBlobParams *localKeyParams);

int32_t TestConstructBlobOut(struct HksBlob **blob,
    bool blobExist,
    uint32_t blobSize,
    bool blobDataExist,
    uint32_t realBlobDataSize);

int32_t TestGenDefaultKeyAndGetAlias(struct HksBlob **keyAlias);

#ifdef __cplusplus
}
#endif

#endif /* HKS_TEST_COMMON_H */
