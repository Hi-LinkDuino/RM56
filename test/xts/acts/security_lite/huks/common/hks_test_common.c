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

#include "hks_test_common.h"

#include <limits.h>
#include <stdbool.h>

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_log.h"
#include "hks_test_mem.h"

#include "securec.h"

#define HKS_TEST_1024 1024
#define HKS_TEST_COMMON_8 8
#define HKS_TEST_COMMON_128 128

void TestFreeBlob(struct HksBlob **key)
{
    if (key == NULL || *key == NULL) {
        return;
    }
    HksTestFree((*key)->data);
    HksTestFree(*key);
}

int32_t TestConstuctBlob(struct HksBlob **blob, bool blobExist, uint32_t blobSize, bool blobDataExist,
    uint32_t realBlobDataSize)
{
    if (!blobExist) { // blob not exist
        return 0; // do nothing, return null
    }

    *blob = (struct HksBlob *)HksTestMalloc(sizeof(struct HksBlob));
    if (*blob == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(*blob, sizeof(struct HksBlob), 0, sizeof(struct HksBlob));

    if (blobDataExist) {
        (*blob)->size = realBlobDataSize;
        (*blob)->data = (uint8_t *)HksTestMalloc(realBlobDataSize);
        if ((*blob)->data == NULL) {
            return HKS_ERROR_MALLOC_FAIL;
        }
        (void)memset_s((*blob)->data, realBlobDataSize, 0, realBlobDataSize);
        uint32_t offset = 0;
        for (uint32_t i = 0; i < (realBlobDataSize - 1) / HKS_TEST_1024; i++) {
            struct HksBlob tmp;
            tmp.size = HKS_TEST_1024;
            tmp.data = (uint8_t *)HksTestMalloc(tmp.size);
            HKS_TEST_ASSERT(tmp.data != NULL);
            (void)memset_s(tmp.data, tmp.size, 0, tmp.size);

            HKS_TEST_ASSERT(HksGenerateRandom(NULL, &tmp) == 0);

            HKS_TEST_ASSERT(memcpy_s((*blob)->data + offset, realBlobDataSize - offset, tmp.data, tmp.size) == EOK);
            offset += tmp.size;
            HksTestFree(tmp.data);
        }

        uint32_t remainSize = realBlobDataSize - offset;
        struct HksBlob tmp;
        tmp.size = remainSize;
        tmp.data = (uint8_t *)HksTestMalloc(tmp.size);
        HKS_TEST_ASSERT(tmp.data != NULL);
        (void)memset_s(tmp.data, tmp.size, 0, tmp.size);

        HKS_TEST_ASSERT(HksGenerateRandom(NULL, &tmp) == 0);
        HKS_TEST_ASSERT(memcpy_s((*blob)->data + offset, remainSize, tmp.data, tmp.size) == EOK);
        offset += tmp.size;
        HksTestFree(tmp.data);
    } else {
        (*blob)->data = NULL;
    }

    (*blob)->size = blobSize;
    return 0;
}

static int32_t TestGenerateKeyParamSetPre(struct GenerateKeyParamSetStructure *paramStruct,
    struct HksParamSet *paramSet)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    int32_t ret = 1;

    if (paramStruct->setAlg) {
        struct HksParam algParam = {0};
        algParam.tag = HKS_TAG_ALGORITHM;
        algParam.uint32Param = paramStruct->alg;
        ret = HksAddParams(paramSet, (const struct HksParam *)&algParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setKeySize) {
        struct HksParam keySizeParam = {0};
        keySizeParam.tag = HKS_TAG_KEY_SIZE;
        keySizeParam.uint32Param = paramStruct->keySize;
        ret = HksAddParams(paramSet, (const struct HksParam *)&keySizeParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setPurpose) {
        struct HksParam purposeParam = {0};
        purposeParam.tag = HKS_TAG_PURPOSE;
        purposeParam.uint32Param = paramStruct->purpose;
        ret = HksAddParams(paramSet, (const struct HksParam *)&purposeParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setDigest) {
        struct HksParam digestParam = {0};
        digestParam.tag = HKS_TAG_DIGEST;
        digestParam.uint32Param = paramStruct->digest;
        ret = HksAddParams(paramSet, (const struct HksParam *)&digestParam, 1);
        HKS_TEST_LOG_I("HksAddParams ret = 0x%X", ret);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setPadding) {
        struct HksParam paddingParam = {0};
        paddingParam.tag = HKS_TAG_PADDING;
        paddingParam.uint32Param = paramStruct->padding;
        ret = HksAddParams(paramSet, (const struct HksParam *)&paddingParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }
    return ret;
}

static int32_t TestGenerateKeyParamSetPost(struct GenerateKeyParamSetStructure *paramStruct,
    struct HksParamSet *paramSet)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    int32_t ret;
    if (paramStruct->setBlockMode) {
        struct HksParam keyMode = {0};
        keyMode.tag = HKS_TAG_BLOCK_MODE;
        keyMode.uint32Param = paramStruct->mode;
        ret = HksAddParams(paramSet, (const struct HksParam *)&keyMode, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setKeyStorageFlag) {
        struct HksParam keyStorageFlagParam = {0};
        keyStorageFlagParam.tag = HKS_TAG_KEY_STORAGE_FLAG;
        keyStorageFlagParam.uint32Param = paramStruct->keyStorageFlag;
        ret = HksAddParams(paramSet, (const struct HksParam *)&keyStorageFlagParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    struct HksParam tmpParam = {0};
    tmpParam.tag = HKS_TAG_KEY_ROLE;
    tmpParam.uint32Param = 0xFFFFFFFF;
    ret = HksAddParams(paramSet, (const struct HksParam *)&tmpParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    *(paramStruct->outParamSet) = paramSet;
    return ret;
}

int32_t TestConstructGenerateKeyParamSet(struct GenerateKeyParamSetStructure *paramStruct)
{
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    ret = TestGenerateKeyParamSetPre(paramStruct, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestGenerateKeyParamSetPost(paramStruct, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

int32_t TestConstructGenerateKeyParamSetOut(
    struct HksParamSet **outParamSet,
    bool paramSetExist, uint32_t paramSetSize)
{
    if (!paramSetExist) {
        return 0; // do nothing, return null
    }
    struct HksParamSet *tempParamSet = (struct HksParamSet *)HksTestMalloc(paramSetSize);
    if (tempParamSet == NULL) {
        return -1; // -1 is error code
    }
    (void)memset_s(tempParamSet, paramSetSize, 0, paramSetSize);
    tempParamSet->paramSetSize = paramSetSize;

    *outParamSet = tempParamSet;
    return 0;
}

int32_t TestConstructRsaCipherParamSet(struct TestRsaCipherParamSet *paramStruct)
{
    struct GenerateKeyParamSetStructure genParamStruct = {
        paramStruct->outParamSet, paramStruct->paramSetExist,
        paramStruct->setAlg, paramStruct->alg,
        false, 0,
        paramStruct->setPurpose, paramStruct->purpose,
        paramStruct->setDigest, paramStruct->digest,
        paramStruct->setPadding, paramStruct->padding,
        false, 0,
        false, 0
    };
    return TestConstructGenerateKeyParamSet(&genParamStruct);
}

int32_t TestAesCipherParamSetPre(struct AesCipherParamSetStructure *paramStruct, struct HksParamSet *paramSet)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    int32_t ret = 1;
    if (paramStruct->setAlg) {
        struct HksParam algParam = {0};
        algParam.tag = HKS_TAG_ALGORITHM;
        algParam.uint32Param = paramStruct->alg;
        ret = HksAddParams(paramSet, (const struct HksParam *)&algParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setPurpose) {
        struct HksParam purposeParam = {0};
        purposeParam.tag = HKS_TAG_PURPOSE;
        purposeParam.uint32Param = paramStruct->purpose;
        ret = HksAddParams(paramSet, (const struct HksParam *)&purposeParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setPadding) {
        struct HksParam paddingParam = {0};
        paddingParam.tag = HKS_TAG_PADDING;
        paddingParam.uint32Param = paramStruct->padding;
        ret = HksAddParams(paramSet, (const struct HksParam *)&paddingParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setBlockMode) {
        struct HksParam keyMode = {0};
        keyMode.tag = HKS_TAG_BLOCK_MODE;
        keyMode.uint32Param = paramStruct->mode;
        ret = HksAddParams(paramSet, (const struct HksParam *)&keyMode, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setIv) {
        struct HksParam ivParam = {0};
        ivParam.tag = HKS_TAG_IV;
        ivParam.blob = *(paramStruct->ivBlob);
        ret = HksAddParams(paramSet, (const struct HksParam *)&ivParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }
    return ret;
}

int32_t TestAesCipherParamSetPost(struct AesCipherParamSetStructure *paramStruct, struct HksParamSet *paramSet)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    int32_t ret;
    if (paramStruct->setNonce) {
        struct HksParam nonceParam = {0};
        nonceParam.tag = HKS_TAG_NONCE;
        nonceParam.blob = *(paramStruct->nonceBlob);
        ret = HksAddParams(paramSet, (const struct HksParam *)&nonceParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setAad) {
        struct HksParam aadParam = {0};
        aadParam.tag = HKS_TAG_ASSOCIATED_DATA;
        aadParam.blob = *(paramStruct->aadBlob);
        ret = HksAddParams(paramSet, (const struct HksParam *)&aadParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setIsKeyAlias) {
        struct HksParam isKeyAilasParam = {0};
        isKeyAilasParam.tag = HKS_TAG_IS_KEY_ALIAS;
        isKeyAilasParam.boolParam = paramStruct->isKeyAlias;
        ret = HksAddParams(paramSet, (const struct HksParam *)&isKeyAilasParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    struct HksParam tmpParam = {0};
    tmpParam.tag = HKS_TAG_KEY_ROLE;
    tmpParam.uint32Param = 0xFFFFFFFF;
    ret = HksAddParams(paramSet, (const struct HksParam *)&tmpParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    *(paramStruct->outParamSet) = paramSet;
    return ret;
}

int32_t TestConstructAesCipherParamSet(struct AesCipherParamSetStructure *paramStruct)
{
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    ret = TestAesCipherParamSetPre(paramStruct, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestAesCipherParamSetPost(paramStruct, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

int32_t TestConstructMacParamSet(struct TestMacParamSetStructure *paramStruct)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    if (paramStruct->setPurpose) {
        struct HksParam purposeParam = {0};
        purposeParam.tag = HKS_TAG_PURPOSE;
        purposeParam.uint32Param = paramStruct->purpose;
        ret = HksAddParams(paramSet, (const struct HksParam *)&purposeParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setDigest) {
        struct HksParam digestParam = {0};
        digestParam.tag = HKS_TAG_DIGEST;
        digestParam.uint32Param = paramStruct->digest;
        ret = HksAddParams(paramSet, (const struct HksParam *)&digestParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setKeyAlias) {
        struct HksParam keyIsKeyAlias = {0};
        keyIsKeyAlias.tag = HKS_TAG_IS_KEY_ALIAS;
        keyIsKeyAlias.boolParam = paramStruct->isKeyAlias;
        ret = HksAddParams(paramSet, (const struct HksParam *)&keyIsKeyAlias, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    struct HksParam tmpParam = {0};
    tmpParam.tag = HKS_TAG_KEY_ROLE;
    tmpParam.uint32Param = 0xFFFFFFFF;
    ret = HksAddParams(paramSet, (const struct HksParam *)&tmpParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    *(paramStruct->outParamSet) = paramSet;
    return ret;
}

int32_t TestConstructAgreeParamSet(struct TestAgreeParamSetStructure *paramStruct)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    if (paramStruct->setAlg) {
        struct HksParam algParam = {0};
        algParam.tag = HKS_TAG_ALGORITHM;
        algParam.uint32Param = paramStruct->alg;
        ret = HksAddParams(paramSet, (const struct HksParam *)&algParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setKeySize) {
        struct HksParam keySizeParam = {0};
        keySizeParam.tag = HKS_TAG_KEY_SIZE;
        keySizeParam.uint32Param = paramStruct->keySize;
        ret = HksAddParams(paramSet, (const struct HksParam *)&keySizeParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setIsKeyAlias) {
        struct HksParam isKeyAliasParam = {0};
        isKeyAliasParam.tag = HKS_TAG_IS_KEY_ALIAS;
        isKeyAliasParam.uint32Param = paramStruct->isKeyAlias;
        ret = HksAddParams(paramSet, (const struct HksParam *)&isKeyAliasParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    struct HksParam tmpParam = {0};
    tmpParam.tag = HKS_TAG_KEY_ROLE;
    tmpParam.uint32Param = 0xFFFFFFFF;
    ret = HksAddParams(paramSet, (const struct HksParam *)&tmpParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    *(paramStruct->outParamSet) = paramSet;
    return ret;
}


int32_t TestDeriveParamSetPre(struct TestDeriveParamSetStructure *paramStruct, struct HksParamSet *paramSet)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    int32_t ret = 1;
    if (paramStruct->setAlg) {
        struct HksParam algParam = {0};
        algParam.tag = HKS_TAG_ALGORITHM;
        algParam.uint32Param = paramStruct->alg;
        ret = HksAddParams(paramSet, (const struct HksParam *)&algParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setPurpose) {
        struct HksParam purposeParam = {0};
        purposeParam.tag = HKS_TAG_PURPOSE;
        purposeParam.uint32Param = paramStruct->purpose;
        ret = HksAddParams(paramSet, (const struct HksParam *)&purposeParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setDigest) {
        struct HksParam digestParam = {0};
        digestParam.tag = HKS_TAG_DIGEST;
        digestParam.uint32Param = paramStruct->digest;
        ret = HksAddParams(paramSet, (const struct HksParam *)&digestParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setIteration) {
        struct HksParam iterationParam = {0};
        iterationParam.tag = HKS_TAG_ITERATION;
        iterationParam.uint32Param = paramStruct->iteration;
        ret = HksAddParams(paramSet, (const struct HksParam *)&iterationParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }
    return ret;
}

int32_t TestDeriveParamSetPost(struct TestDeriveParamSetStructure *paramStruct, struct HksParamSet *paramSet)
{
    if (!paramStruct->paramSetExist) {
        return 0; // do nothing, return null
    }
    int32_t ret;
    if (paramStruct->setSalt) {
        struct HksParam saltParam = {0};
        saltParam.tag = HKS_TAG_SALT;
        saltParam.blob = *(paramStruct->saltBlob);
        ret = HksAddParams(paramSet, (const struct HksParam *)&saltParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setInfo) {
        struct HksParam infoParam = {0};
        infoParam.tag = HKS_TAG_INFO;
        infoParam.blob = *(paramStruct->infoBlob);
        ret = HksAddParams(paramSet, (const struct HksParam *)&infoParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    if (paramStruct->setIsKeyAlias) {
        struct HksParam isKeyAliasParam = {0};
        isKeyAliasParam.tag = HKS_TAG_IS_KEY_ALIAS;
        isKeyAliasParam.boolParam = paramStruct->isKeyAlias;
        ret = HksAddParams(paramSet, (const struct HksParam *)&isKeyAliasParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    struct HksParam tmpParam = {0};
    tmpParam.tag = HKS_TAG_KEY_ROLE;
    tmpParam.uint32Param = 0xFFFFFFFF;
    ret = HksAddParams(paramSet, (const struct HksParam *)&tmpParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    *(paramStruct->outParamSet) = paramSet;
    return ret;
}

int32_t TestConstructDeriveParamSet(struct TestDeriveParamSetStructure *paramStruct)
{
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    ret = TestDeriveParamSetPre(paramStruct, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestDeriveParamSetPost(paramStruct, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

int32_t TestConstructHashParamSet(
    struct HksParamSet **outParamSet,
    bool paramSetExist,
    bool setDigest, uint32_t digest)
{
    if (!paramSetExist) {
        return 0; // do nothing, return null
    }
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    if (setDigest) {
        struct HksParam digestParam = {0};
        digestParam.tag = HKS_TAG_DIGEST;
        digestParam.uint32Param = digest;
        ret = HksAddParams(paramSet, (const struct HksParam *)&digestParam, 1);
        HKS_TEST_ASSERT(ret == 0);
    }

    struct HksParam tmpParam = {0};
    tmpParam.tag = HKS_TAG_KEY_ROLE;
    tmpParam.uint32Param = 0xFFFFFFFF;
    ret = HksAddParams(paramSet, (const struct HksParam *)&tmpParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    *outParamSet = paramSet;
    return ret;
}

int32_t GenerateKey(struct HksBlob **keyAlias, const struct HksTestBlobParams *keyAliasParams,
    const struct HksTestGenKeyParamsParamSet *genKeyParamSetParams,
    const struct HksTestGenKeyParamsParamSetOut *genKeyParamSetParamsOut)
{
    int32_t ret = TestConstuctBlob(keyAlias,
        keyAliasParams->blobExist,
        keyAliasParams->blobSize,
        keyAliasParams->blobDataExist,
        keyAliasParams->blobDataSize);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *paramSet = NULL;
    struct GenerateKeyParamSetStructure paramStruct = {
        &paramSet,
        genKeyParamSetParams->paramSetExist,
        genKeyParamSetParams->setAlg, genKeyParamSetParams->alg,
        genKeyParamSetParams->setKeySize, genKeyParamSetParams->keySize,
        genKeyParamSetParams->setPurpose, genKeyParamSetParams->purpose,
        genKeyParamSetParams->setDigest, genKeyParamSetParams->digest,
        genKeyParamSetParams->setPadding, genKeyParamSetParams->padding,
        genKeyParamSetParams->setBlockMode, genKeyParamSetParams->mode,
        genKeyParamSetParams->setKeyStorageFlag, genKeyParamSetParams->keyStorageFlag
    };
    ret = TestConstructGenerateKeyParamSet(&paramStruct);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *paramSetOut = NULL;
    if (genKeyParamSetParamsOut != NULL) {
        ret = TestConstructGenerateKeyParamSetOut(&paramSet,
            genKeyParamSetParamsOut->paramSetExist, genKeyParamSetParamsOut->paramSetSize);
        HKS_TEST_ASSERT(ret == 0);
    }

    ret = HksGenerateKey(*keyAlias, paramSet, paramSetOut);
    HKS_TEST_ASSERT(ret == 0);

    HksFreeParamSet(&paramSet);
    return ret;
}

int32_t GenerateLocalRandomKey(struct HksBlob **keyAlias, const struct HksTestBlobParams *localKeyParams)
{
    int32_t ret = TestConstuctBlob(keyAlias,
        localKeyParams->blobExist,
        localKeyParams->blobSize,
        localKeyParams->blobDataExist,
        localKeyParams->blobDataSize);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

int32_t TestConstructBlobOut(struct HksBlob **blob,
    bool blobExist,
    uint32_t blobSize,
    bool blobDataExist,
    uint32_t realBlobDataSize)
{
    if (!blobExist) { // blob not exist
        return 0; // do nothing, return null
    }

    *blob = (struct HksBlob *)HksTestMalloc(sizeof(struct HksBlob));
    if (*blob == NULL) {
        HKS_TEST_LOG_E("malloc fail");
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(*blob, sizeof(struct HksBlob), 0, sizeof(struct HksBlob));

    if (blobDataExist) {
        (*blob)->size = realBlobDataSize;
        (*blob)->data = (uint8_t *)HksTestMalloc(realBlobDataSize);
        if ((*blob)->data == NULL) {
            HKS_TEST_LOG_E("malloc fail");
            return HKS_ERROR_MALLOC_FAIL;
        }
        (void)memset_s((*blob)->data, realBlobDataSize, 0, realBlobDataSize);
    } else {
        (*blob)->data = NULL;
    }

    (*blob)->size = blobSize;
    return 0;
}

int32_t GenerateLocalX25519Key(struct HksBlob **privateKey, struct HksBlob **publicKey,
    const struct HksTestBlobParams *localPrivateKeyParams, const struct HksTestBlobParams *localPublicKeyParams)
{
    int32_t ret;
    if ((privateKey != NULL) && (localPrivateKeyParams != NULL)) {
        ret = TestConstuctBlob(privateKey,
            localPrivateKeyParams->blobExist,
            localPrivateKeyParams->blobSize,
            localPrivateKeyParams->blobDataExist,
            localPrivateKeyParams->blobDataSize);
        HKS_TEST_ASSERT(ret == 0);
    }

    if ((publicKey != NULL) && (localPublicKeyParams != NULL)) {
        ret = TestConstuctBlob(publicKey,
            localPublicKeyParams->blobExist,
            localPublicKeyParams->blobSize,
            localPublicKeyParams->blobDataExist,
            localPublicKeyParams->blobDataSize);
        HKS_TEST_ASSERT(ret == 0);
    }

    struct HksParamSet *paramSet = NULL;
    struct GenerateKeyParamSetStructure paramStruct = {
        &paramSet, true, true, HKS_ALG_X25519,
        true, HKS_CURVE25519_KEY_SIZE_256, true, HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY, false, 0,
        false, 0, false, 0, true, HKS_STORAGE_TEMP
    };
    ret = TestConstructGenerateKeyParamSet(&paramStruct);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *paramSetOut = NULL;
    ret = TestConstructGenerateKeyParamSetOut(&paramSetOut, true, HKS_TEST_COMMON_128);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksGenerateKey(NULL, paramSet, paramSetOut);
    HKS_TEST_ASSERT(ret == 0);

    if ((publicKey != NULL) && ((*publicKey) != NULL) &&
        (localPublicKeyParams->blobDataExist) &&
        (localPublicKeyParams->blobDataSize == (HKS_CURVE25519_KEY_SIZE_256 / HKS_TEST_COMMON_8))) {
        (void)memcpy_s((*publicKey)->data, (*publicKey)->size,
            paramSetOut->params[0].blob.data, paramSetOut->params[0].blob.size);
    }

    if ((privateKey != NULL) && ((*privateKey) != NULL) &&
        (localPrivateKeyParams->blobDataExist) &&
        (localPrivateKeyParams->blobDataSize == (HKS_CURVE25519_KEY_SIZE_256 / HKS_TEST_COMMON_8))) {
        (void)memcpy_s((*privateKey)->data, (*privateKey)->size,
            paramSetOut->params[1].blob.data, paramSetOut->params[1].blob.size);
    }

    HksFreeParamSet(&paramSet);
    HksFreeParamSet(&paramSetOut);
    return ret;
}

int32_t TestGenDefaultKeyAndGetAlias(struct HksBlob **keyAlias)
{
    struct HksTestGenKeyParams genKeyParam = {
        0xffffffff, HKS_SUCCESS,
        { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE }, {
            true,
            true, HKS_ALG_AES,
            true, HKS_AES_KEY_SIZE_128,
            true, HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT,
            true, HKS_DIGEST_SHA256,
            true, HKS_PADDING_NONE,
            true, HKS_MODE_CBC },
        { false, 0 }
    };

    int32_t ret = TestConstuctBlob(keyAlias,
        genKeyParam.keyAliasParams.blobExist,
        genKeyParam.keyAliasParams.blobSize,
        genKeyParam.keyAliasParams.blobDataExist,
        genKeyParam.keyAliasParams.blobDataSize);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParamSet *paramSet = NULL;
    struct GenerateKeyParamSetStructure paramStruct = {
        &paramSet, genKeyParam.paramSetParams.paramSetExist,
        genKeyParam.paramSetParams.setAlg, genKeyParam.paramSetParams.alg,
        genKeyParam.paramSetParams.setKeySize, genKeyParam.paramSetParams.keySize,
        genKeyParam.paramSetParams.setPurpose, genKeyParam.paramSetParams.purpose,
        genKeyParam.paramSetParams.setDigest, genKeyParam.paramSetParams.digest,
        genKeyParam.paramSetParams.setPadding, genKeyParam.paramSetParams.padding,
        genKeyParam.paramSetParams.setBlockMode, genKeyParam.paramSetParams.mode,
        false, 0
    };
    ret = TestConstructGenerateKeyParamSet(&paramStruct);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksGenerateKey(*keyAlias, paramSet, NULL);
    HKS_TEST_ASSERT(ret == 0);

    HksFreeParamSet(&paramSet);
    return ret;
}
