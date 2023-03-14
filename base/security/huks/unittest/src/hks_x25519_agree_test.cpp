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

#include "hks_x25519_agree_test.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::X25519Agree {
class HksX25519AgreeTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksX25519AgreeTest::SetUpTestCase(void)
{
}

void HksX25519AgreeTest::TearDownTestCase(void)
{
}

void HksX25519AgreeTest::SetUp()
{
}

void HksX25519AgreeTest::TearDown()
{
}

static struct HksBlob g_keyAlias01001 = {
    strlen("HksX25519AgreeKeyAliasTest001_1"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest001_1"
};
static struct HksBlob g_keyAlias02001 = {
    strlen("HksX25519AgreeKeyAliasTest001_2"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest001_2"
};
static struct HksBlob g_keyAliasFinal1001 = {
    strlen("HksX25519AgreeKeyAliasTest001_final"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest001_final"
};
static struct HksBlob g_keyAliasFinal2001 = {
    strlen("HksX25519AgreeKeyAliasTest002_final"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest002_final"
};
#ifdef _USE_MBEDTLS_
static struct HksParam g_genParams001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#else
static struct HksParam g_genParams001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ED25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#endif // _USE_MBEDTLS_
static struct HksParam g_agreeParams01Init001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams01Finish001[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_ALIAS, .blob = g_keyAliasFinal1001
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};
static struct HksParam g_agreeParams02Init001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams02Finish001[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_PERSISTENT
    }, {
        .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true
    }, {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DERIVE
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_ALIAS, .blob = g_keyAliasFinal2001
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};

static struct HksBlob g_keyAlias01002 = {
    strlen("HksX25519AgreeKeyAliasTest002_1"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest002_1"
};
static struct HksBlob g_keyAlias02002 = {
    strlen("HksX25519AgreeKeyAliasTest002_2"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest002_2"
};
#ifdef _USE_MBEDTLS_
static struct HksParam g_genParams002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#else
static struct HksParam g_genParams002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ED25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#endif // _USE_MBEDTLS_
static struct HksParam g_agreeParams01Init002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams01Finish002[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};
static struct HksParam g_agreeParams02Init002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams02Finish002[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};

static struct HksBlob g_keyAlias01003 = {
    strlen("HksX25519AgreeKeyAliasTest003_1"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest003_1"
};
static struct HksBlob g_keyAlias02003 = {
    strlen("HksX25519AgreeKeyAliasTest003_2"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest003_2"
};
#ifdef _USE_MBEDTLS_
static struct HksParam g_genParams003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#else
static struct HksParam g_genParams003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ED25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#endif // _USE_MBEDTLS_
static struct HksParam g_agreeParams01Init003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams02Init003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};

static struct HksBlob g_keyAlias01004 = {
    strlen("HksX25519AgreeKeyAliasTest004_1"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest004_1"
};
static struct HksBlob g_keyAlias02004 = {
    strlen("HksX25519AgreeKeyAliasTest004_2"),
    (uint8_t *)"HksX25519AgreeKeyAliasTest004_2"
};
#ifdef _USE_MBEDTLS_
static struct HksParam g_genParams004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#else
static struct HksParam g_genParams004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ED25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
#endif // _USE_MBEDTLS_
static struct HksParam g_agreeParams01Init004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams02Init004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_X25519
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_CURVE25519_KEY_SIZE_256
    }
};

int32_t HksX25519AgreeFinish(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet, const struct HksParamSet *finishParamSet, struct HksBlob *outData)
{
    struct HksBlob inData = {
        Unittest::X25519Agree::g_inData.length(),
        (uint8_t *)Unittest::X25519Agree::g_inData.c_str()
    };

    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    int32_t ret = HksInit(keyAlias, initParamSet, &handle);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    uint8_t outDataU[Unittest::X25519Agree::X25519_COMMON_SIZE] = {0};
    struct HksBlob outDataUpdate = { Unittest::X25519Agree::X25519_COMMON_SIZE, outDataU };
    ret = HksUpdate(&handle, initParamSet, publicKey, &outDataUpdate);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    ret = HksFinish(&handle, finishParamSet, &inData, outData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

int32_t HksX25519AgreeAbort(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet)
{
    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    int32_t ret = HksInit(keyAlias, initParamSet, &handle);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    uint8_t outDataU[Unittest::X25519Agree::X25519_COMMON_SIZE] = {0};
    struct HksBlob outDataUpdate = { Unittest::X25519Agree::X25519_COMMON_SIZE, outDataU };
    ret = HksUpdate(&handle, initParamSet, publicKey, &outDataUpdate);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    ret = HksAbort(&handle, initParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

int32_t HksX25519AgreeExport(const struct HksBlob *keyAlias1, const struct HksBlob *keyAlias2,
    struct HksBlob *publicKey1, struct HksBlob *publicKey2, const struct HksParamSet *genParamSet)
{
    int32_t ret = HksExportPublicKey(keyAlias1, genParamSet, publicKey1);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey01 failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    ret = HksExportPublicKey(keyAlias2, genParamSet, publicKey2);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey02 failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

void HksX25519AgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3)
{
    HksFreeParamSet(&paramSet1);
    HksFreeParamSet(&paramSet2);
    HksFreeParamSet(&paramSet3);
}

void HksX25519AgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3, struct HksParamSet *paramSet4, struct HksParamSet *paramSet5)
{
    HksFreeParamSet(&paramSet1);
    HksFreeParamSet(&paramSet2);
    HksFreeParamSet(&paramSet3);
    HksFreeParamSet(&paramSet4);
    HksFreeParamSet(&paramSet5);
}

void HksX25519AgreeFreeBlob(struct HksBlob *blob1, struct HksBlob *blob2)
{
    HksFree(blob1->data);
    HksFree(blob2->data);
}

void HksX25519AgreeFreeBlob(struct HksBlob *blob1, struct HksBlob *blob2,
    struct HksBlob *blob3, struct HksBlob *blob4)
{
    HksFree(blob1->data);
    HksFree(blob2->data);
    HksFree(blob3->data);
    HksFree(blob4->data);
}

/**
 * @tc.name: HksX25519AgreeTest.HksX25519Agree001
 * @tc.desc: alg-x25519, pur-AGREE
 * @tc.type: FUNC
 */
HWTEST_F(HksX25519AgreeTest, HksX25519Agree001, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams001, sizeof(g_genParams001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init001, sizeof(g_agreeParams01Init001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_agreeParams01Finish001, sizeof(g_agreeParams01Finish001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init001, sizeof(g_agreeParams02Init001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_agreeParams02Finish001, sizeof(g_agreeParams02Finish001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01001, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02001, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    struct HksBlob publicKey01 = { .size = HKS_CURVE25519_KEY_SIZE_256, .data = nullptr };
    struct HksBlob publicKey02 = { .size = HKS_CURVE25519_KEY_SIZE_256, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey01, publicKey01.size), HKS_SUCCESS) << "Malloc publicKey01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey02, publicKey02.size), HKS_SUCCESS) << "Malloc publicKey02 failed.";
    ret = HksX25519AgreeExport(&g_keyAlias01001, &g_keyAlias02001, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportKey failed.";

    struct HksBlob outData01 = { .size = Unittest::X25519Agree::X25519_COMMON_SIZE, .data = nullptr };
    struct HksBlob outData02 = { .size = Unittest::X25519Agree::X25519_COMMON_SIZE, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&outData01, outData01.size), HKS_SUCCESS) << "Malloc outData01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&outData02, outData02.size), HKS_SUCCESS) << "Malloc outData02 failed.";
    ret = HksX25519AgreeFinish(&g_keyAlias01001, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksX25519AgreeFinish01 failed.";
    ret = HksX25519AgreeFinish(&g_keyAlias02001, &publicKey01, initParamSet02, finishParamSet02, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksX25519AgreeFinish02 failed.";
    EXPECT_EQ(TestCmpKeyAliasHash(&outData01, &outData02), HKS_SUCCESS) << "outData01 not equals outData02";

    HksDeleteKey(&g_keyAlias01001, genParamSet);
    HksDeleteKey(&g_keyAlias02001, genParamSet);
    HksDeleteKey(&g_keyAliasFinal1001, NULL);
    HksDeleteKey(&g_keyAliasFinal2001, NULL);
    HksX25519AgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
    HksX25519AgreeFreeBlob(&publicKey01, &publicKey02, &outData01, &outData02);
}

/**
 * @tc.name: HksX25519AgreeTest.HksX25519Agree002
 * @tc.desc: alg-x25519, pur-AGREE
 * @tc.type: FUNC
 */
HWTEST_F(HksX25519AgreeTest, HksX25519Agree002, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams002, sizeof(g_genParams002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init002, sizeof(g_agreeParams01Init002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_agreeParams01Finish002, sizeof(g_agreeParams01Finish002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init002, sizeof(g_agreeParams02Init002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_agreeParams02Finish002, sizeof(g_agreeParams02Finish002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01002, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02002, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    struct HksBlob publicKey01 = { .size = HKS_CURVE25519_KEY_SIZE_256, .data = nullptr };
    struct HksBlob publicKey02 = { .size = HKS_CURVE25519_KEY_SIZE_256, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey01, publicKey01.size), HKS_SUCCESS) << "Malloc publicKey01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey02, publicKey02.size), HKS_SUCCESS) << "Malloc publicKey02 failed.";
    ret = HksX25519AgreeExport(&g_keyAlias01002, &g_keyAlias02002, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportKey failed.";

    struct HksBlob outData01 = { .size = Unittest::X25519Agree::X25519_COMMON_SIZE, .data = nullptr };
    struct HksBlob outData02 = { .size = Unittest::X25519Agree::X25519_COMMON_SIZE, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&outData01, outData01.size), HKS_SUCCESS) << "Malloc outData01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&outData02, outData02.size), HKS_SUCCESS) << "Malloc outData02 failed.";
    ret = HksX25519AgreeFinish(&g_keyAlias01002, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksX25519AgreeFinish01 failed.";
    ret = HksAgreeKey(initParamSet02, &g_keyAlias02002, &publicKey01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAgreeKey02 failed.";
    EXPECT_EQ(TestCmpKeyAliasHash(&outData01, &outData02), HKS_SUCCESS) << "outData01 not equals outData02";

    HksDeleteKey(&g_keyAlias01002, genParamSet);
    HksDeleteKey(&g_keyAlias02002, genParamSet);
    HksX25519AgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
    HksX25519AgreeFreeBlob(&publicKey01, &publicKey02, &outData01, &outData02);
}

/**
 * @tc.name: HksX25519AgreeTest.HksX25519Agree003
 * @tc.desc: alg-x25519, pur-AGREE
 * @tc.type: FUNC   abort
 */
HWTEST_F(HksX25519AgreeTest, HksX25519Agree003, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams003, sizeof(g_genParams003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init003, sizeof(g_agreeParams01Init003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init003, sizeof(g_agreeParams02Init003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01003, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02003, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    struct HksBlob publicKey01 = { .size = HKS_CURVE25519_KEY_SIZE_256, .data = nullptr };
    struct HksBlob publicKey02 = { .size = HKS_CURVE25519_KEY_SIZE_256, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey01, publicKey01.size), HKS_SUCCESS) << "Malloc publicKey01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey02, publicKey02.size), HKS_SUCCESS) << "Malloc publicKey02 failed.";
    ret = HksX25519AgreeExport(&g_keyAlias01003, &g_keyAlias02003, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportKey failed.";

    ret = HksX25519AgreeAbort(&g_keyAlias01003, &publicKey02, initParamSet01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksX25519AgreeAbort01 failed.";
    ret = HksX25519AgreeAbort(&g_keyAlias02003, &publicKey01, initParamSet02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksX25519AgreeAbort02 failed.";

    HksDeleteKey(&g_keyAlias01003, genParamSet);
    HksDeleteKey(&g_keyAlias02003, genParamSet);
    HksX25519AgreeFreeParamSet(genParamSet, initParamSet01, initParamSet02);
    HksX25519AgreeFreeBlob(&publicKey01, &publicKey02);
}

/**
 * @tc.name: HksX25519AgreeTest.HksX25519Agree004
 * @tc.desc: alg-x25519, pur-AGREE
 * @tc.type: FUNC   abnormal
 */
HWTEST_F(HksX25519AgreeTest, HksX25519Agree004, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams004, sizeof(g_genParams004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init004, sizeof(g_agreeParams01Init004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init004, sizeof(g_agreeParams02Init004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01004, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02004, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    ret = HksInit(NULL, initParamSet01, &handle);
    EXPECT_NE(ret, HKS_SUCCESS) << "HksInit01 should failed.";
    ret = HksInit(NULL, initParamSet02, &handle);
    EXPECT_NE(ret, HKS_SUCCESS) << "HksInit02 should failed.";

    HksDeleteKey(&g_keyAlias01004, genParamSet);
    HksDeleteKey(&g_keyAlias02004, genParamSet);
    HksX25519AgreeFreeParamSet(genParamSet, initParamSet01, initParamSet02);
}
} // namespace Unittest::X25519Agree