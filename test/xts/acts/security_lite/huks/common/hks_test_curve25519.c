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

#include "hks_test_curve25519.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_log.h"
#include "hks_type.h"
#include "securec.h"

#define TEST_ALIAS_ED25519 "test_ed25519"
#define TEST_PLAIN_TEST_ED25519 "This is a plain text! Hello world and thanks for watching ED25519~"
#define TEST_CURVE_256 256
#define TEST_CURVE_512 512

static uint8_t g_buffer[TEST_CURVE_256];
static uint32_t g_bufferSize = TEST_CURVE_256;
static uint8_t g_pubKey[TEST_CURVE_512] = {0};
static uint32_t g_pubKeyLen = TEST_CURVE_512;

int32_t TestGenerateEd25519Key(struct HksBlob alias)
{
    HKS_TEST_LOG_I("Test_GenerateEd25519!\n");
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam keySizeParam = {0};
    keySizeParam.tag = HKS_TAG_KEY_SIZE;
    keySizeParam.uint32Param = TEST_CURVE_256;
    ret = HksAddParams(paramSet, &keySizeParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY;
    ret = HksAddParams(paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA512;
    ret = HksAddParams(paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam paddingParam = {0};
    paddingParam.tag = HKS_TAG_PADDING;
    paddingParam.uint32Param = HKS_PADDING_NONE;
    ret = HksAddParams(paramSet, &paddingParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);
    ret = HksGenerateKey(&alias, paramSet, NULL);
    HKS_TEST_ASSERT(ret == 0);
    HksFreeParamSet(&paramSet);
    return ret;
}

static int32_t TestSignEd25519(struct HksBlob alias)
{
    struct HksBlob msg = {strlen(TEST_PLAIN_TEST_ED25519), (uint8_t *)TEST_PLAIN_TEST_ED25519};

    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_SIGN;
    ret = HksAddParams(paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA512;
    ret = HksAddParams(paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam paddingParam = {0};
    paddingParam.tag = HKS_TAG_PADDING;
    paddingParam.uint32Param = HKS_PADDING_NONE;
    ret = HksAddParams(paramSet, &paddingParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob signature = { TEST_CURVE_256, g_buffer };
    ret = HksSign(&alias, paramSet, &msg, &signature);
    HKS_TEST_ASSERT(ret == 0);
    g_bufferSize = signature.size;
    HKS_TEST_LOG_I("TestSignEd25519 signature size is %u", signature.size);
    HksFreeParamSet(&paramSet);
    return ret;
}

static int32_t TestVerifyEd25519(struct HksBlob alias)
{
    struct HksBlob msg = {strlen(TEST_PLAIN_TEST_ED25519), (uint8_t *)TEST_PLAIN_TEST_ED25519};

    HKS_TEST_LOG_I("TestVerifyEd25519!\n");
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_VERIFY;
    ret = HksAddParams(paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA512;
    ret = HksAddParams(paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam paddingParam = {0};
    paddingParam.tag = HKS_TAG_PADDING;
    paddingParam.uint32Param = HKS_PADDING_NONE;
    ret = HksAddParams(paramSet, &paddingParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    struct HksBlob signature = { g_bufferSize, g_buffer };
    ret = HksVerify(&alias, paramSet, &msg, &signature);
    HKS_TEST_ASSERT(ret == 0);
    HksFreeParamSet(&paramSet);
    return ret;
}

int32_t TestImportEd25519(struct HksBlob alias, struct HksBlob *pubKeyInfo)
{
    HKS_TEST_LOG_I("TestImportEd25519!\n");
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam keySizeParam = {0};
    keySizeParam.tag = HKS_TAG_KEY_SIZE;
    keySizeParam.uint32Param = TEST_CURVE_256;
    ret = HksAddParams(paramSet, &keySizeParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_VERIFY;
    ret = HksAddParams(paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA512;
    ret = HksAddParams(paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam paddingParam = {0};
    paddingParam.tag = HKS_TAG_PADDING;
    paddingParam.uint32Param = HKS_PADDING_NONE;
    ret = HksAddParams(paramSet, &paddingParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksImportKey(&alias, paramSet, pubKeyInfo);
    HKS_TEST_ASSERT(ret == 0);
    HksFreeParamSet(&paramSet);
    return ret;
}

static int32_t TestExportImportEd25519SignVerify(struct HksBlob alias)
{
    uint8_t pubKey[32] = {0};
    uint32_t pubKeyLen = 32;
    struct HksBlob pubKeyInfo = { pubKeyLen, pubKey };
    int32_t ret = TestGenerateEd25519Key(alias);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksExportPublicKey(&alias, NULL, &pubKeyInfo);
    HKS_TEST_ASSERT(ret == 0);

    ret = TestSignEd25519(alias);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksDeleteKey(&alias, NULL);
    HKS_TEST_ASSERT(ret == 0);

    struct HksBlob newAlias = { strlen("test_ed25519_2"), (uint8_t *)"test_ed25519_2" };
    ret = TestImportEd25519(newAlias, &pubKeyInfo);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestVerifyEd25519(newAlias);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksDeleteKey(&newAlias, NULL);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

int32_t TestCurve25519All()
{
    struct HksBlob ed25519Alias = { strlen(TEST_ALIAS_ED25519), (uint8_t *)TEST_ALIAS_ED25519 };
    int32_t ret = TestGenerateEd25519Key(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestSignEd25519(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestVerifyEd25519(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);
    ret = HksDeleteKey(&ed25519Alias, NULL);
    HKS_TEST_ASSERT(ret == 0);

    ret = TestExportImportEd25519SignVerify(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

static int32_t BuildTeeSignParamSet(struct HksParamSet **paramSet)
{
    int32_t ret = HksInitParamSet(paramSet);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(*paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_SIGN;
    ret = HksAddParams(*paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA512;
    ret = HksAddParams(*paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam paddingParam = {0};
    paddingParam.tag = HKS_TAG_PADDING;
    paddingParam.uint32Param = HKS_PADDING_NONE;
    ret = HksAddParams(*paramSet, &paddingParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(paramSet);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

static int32_t BuildLocalVerifyParamSet(struct HksParamSet **paramSet)
{
    int32_t ret = HksInitParamSet(paramSet);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(*paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam isKeyAlias = {0};
    isKeyAlias.tag = HKS_TAG_IS_KEY_ALIAS;
    isKeyAlias.boolParam = false;
    ret = HksAddParams(*paramSet, &isKeyAlias, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam keySizeParam = {0};
    keySizeParam.tag = HKS_TAG_KEY_SIZE;
    keySizeParam.uint32Param = TEST_CURVE_256;
    ret = HksAddParams(*paramSet, &keySizeParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_VERIFY;
    ret = HksAddParams(*paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA512;
    ret = HksAddParams(*paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(paramSet);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

int32_t TestEd25519SignTeeVerifyLocal()
{
    HKS_TEST_LOG_D("TestEd25519SignTeeVerifyLocal enter!");

    struct HksBlob ed25519Alias = { strlen(TEST_ALIAS_ED25519), (uint8_t *)TEST_ALIAS_ED25519 };
    struct HksBlob msg = {strlen(TEST_PLAIN_TEST_ED25519), (uint8_t *)TEST_PLAIN_TEST_ED25519};
    struct HksBlob signature = { TEST_CURVE_256, g_buffer };
    struct HksParamSet *paramSetSign = NULL;
    struct HksParamSet *paramSetVerify = NULL;
    struct HksBlob pubKeyInfo = { g_pubKeyLen, g_pubKey };

    int32_t ret = TestGenerateEd25519Key(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksExportPublicKey(&ed25519Alias, NULL, &pubKeyInfo);
    HKS_TEST_ASSERT(ret == 0);
    HKS_TEST_LOG_I("HksExportPublicKey puKey size is %u", pubKeyInfo.size);

    ret = BuildTeeSignParamSet(&paramSetSign);
    HKS_TEST_ASSERT(ret == 0);
    ret = HksSign(&ed25519Alias, paramSetSign, &msg, &signature);
    HKS_TEST_ASSERT(ret == 0);
    HKS_TEST_LOG_I("Test_Ed25519_Sign_TEE signature size is %u", signature.size);

    ret = BuildLocalVerifyParamSet(&paramSetVerify);
    HKS_TEST_ASSERT(ret == 0);
    ret = HksVerify(&pubKeyInfo, paramSetVerify, &msg, &signature);
    HKS_TEST_ASSERT(ret == 0);
    HKS_TEST_LOG_I("Test_Ed25519_Verify_Local Success");

    ret = HksDeleteKey(&ed25519Alias, NULL);
    HKS_TEST_ASSERT(ret == 0);

    HksFreeParamSet(&paramSetSign);
    HksFreeParamSet(&paramSetVerify);
    HKS_TEST_LOG_D("TestEd25519SignTeeVerifyLocal End!\n");
    return ret;
}

static int32_t TestSignEd25519Wrong(struct HksBlob alias)
{
    struct HksBlob msg = {strlen(TEST_PLAIN_TEST_ED25519), (uint8_t *)TEST_PLAIN_TEST_ED25519};

    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_VERIFY;
    ret = HksAddParams(paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA256;
    ret = HksAddParams(paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    struct HksParam paddingParam = {0};
    paddingParam.tag = HKS_TAG_PADDING;
    paddingParam.uint32Param = HKS_PADDING_PKCS5;
    ret = HksAddParams(paramSet, &paddingParam, 1);
    HKS_TEST_ASSERT(ret == 0);
    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob signature = { TEST_CURVE_256, g_buffer };
    ret = HksSign(&alias, paramSet, &msg, &signature);
    HKS_TEST_ASSERT(ret != 0);
    g_bufferSize = signature.size;
    HKS_TEST_LOG_I("TestSignEd25519 signature size is %u", signature.size);
    HksFreeParamSet(&paramSet);
    return ret;
}

int32_t TestCurve25519SignWrong()
{
    struct HksBlob ed25519Alias = { strlen(TEST_ALIAS_ED25519), (uint8_t *)TEST_ALIAS_ED25519 };
    int32_t ret = TestGenerateEd25519Key(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestSignEd25519Wrong(ed25519Alias);
    HKS_TEST_ASSERT(ret != 0);
    int32_t retTwo = HksDeleteKey(&ed25519Alias, NULL);
    HKS_TEST_ASSERT(retTwo == 0);
    if ((ret != 0) && (retTwo == 0)) {
        return 0;
    }
    return 1;
}

static int32_t TestVerifyEd25519Wrong(struct HksBlob alias)
{
    struct HksBlob msg = {strlen(TEST_PLAIN_TEST_ED25519), (uint8_t *)TEST_PLAIN_TEST_ED25519};

    HKS_TEST_LOG_I("TestVerifyEd25519!\n");
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam algParam = {0};
    algParam.tag = HKS_TAG_ALGORITHM;
    algParam.uint32Param = HKS_ALG_ED25519;
    ret = HksAddParams(paramSet, &algParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam purposeParam = {0};
    purposeParam.tag = HKS_TAG_PURPOSE;
    purposeParam.uint32Param = HKS_KEY_PURPOSE_SIGN;
    ret = HksAddParams(paramSet, &purposeParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam digestParam = {0};
    digestParam.tag = HKS_TAG_DIGEST;
    digestParam.uint32Param = HKS_DIGEST_SHA256;
    ret = HksAddParams(paramSet, &digestParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    struct HksParam paddingParam = {0};
    paddingParam.tag = HKS_TAG_PADDING;
    paddingParam.uint32Param = HKS_PADDING_PKCS5;
    ret = HksAddParams(paramSet, &paddingParam, 1);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksBuildParamSet(&paramSet);
    HKS_TEST_ASSERT(ret == 0);

    struct HksBlob signature = { g_bufferSize, g_buffer };
    ret = HksVerify(&alias, paramSet, &msg, &signature);
    HKS_TEST_ASSERT(ret != 0);
    HksFreeParamSet(&paramSet);
    return ret;
}

int32_t TestCurve25519verifyWrong()
{
    struct HksBlob ed25519Alias = { strlen(TEST_ALIAS_ED25519), (uint8_t *)TEST_ALIAS_ED25519 };
    int32_t ret = TestGenerateEd25519Key(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestSignEd25519(ed25519Alias);
    HKS_TEST_ASSERT(ret == 0);
    ret = TestVerifyEd25519Wrong(ed25519Alias);
    HKS_TEST_ASSERT(ret != 0);
    int32_t retTwo = HksDeleteKey(&ed25519Alias, NULL);
    HKS_TEST_ASSERT(retTwo == 0);
    if ((ret != 0) && (retTwo == 0)) {
        return 0;
    }
    return 1;
}