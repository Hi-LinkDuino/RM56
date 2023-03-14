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

#include "hks_ecdh_agree_part2_test.h"
#include "hks_ecdh_agree_test_common.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::EcdhAgree {
class HksEcdhAgreePart2Test : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksEcdhAgreePart2Test::SetUpTestCase(void)
{
}

void HksEcdhAgreePart2Test::TearDownTestCase(void)
{
}

void HksEcdhAgreePart2Test::SetUp()
{
}

void HksEcdhAgreePart2Test::TearDown()
{
}

static struct HksBlob g_keyAlias01006 = {
    strlen("HksECDHAgreeKeyAliasTest006_1"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest006_1"
};
static struct HksBlob g_keyAlias02006 = {
    strlen("HksECDHAgreeKeyAliasTest006_2"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest006_2"
};
static struct HksParam g_genParams006[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_256
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_NONE
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};
static struct HksParam g_agreeParams01Init006[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams01Finish006[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};
static struct HksParam g_agreeParams02Init006[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_256
    }
};
static struct HksParam g_agreeParams02Finish006[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};

static struct HksBlob g_keyAlias01007 = {
    strlen("HksECDHAgreeKeyAliasTest007_1"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest007_1"
};
static struct HksBlob g_keyAlias02007 = {
    strlen("HksECDHAgreeKeyAliasTest007_2"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest007_2"
};
static struct HksParam g_genParams007[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_384
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_NONE
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};
static struct HksParam g_agreeParams01Init007[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_384
    }
};
static struct HksParam g_agreeParams01Finish007[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};
static struct HksParam g_agreeParams02Init007[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_384
    }
};
static struct HksParam g_agreeParams02Finish007[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};

static struct HksBlob g_keyAlias01008 = {
    strlen("HksECDHAgreeKeyAliasTest008_1"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest008_1"
};
static struct HksBlob g_keyAlias02008 = {
    strlen("HksECDHAgreeKeyAliasTest008_2"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest008_2"
};
static struct HksParam g_genParams008[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_NONE
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};
static struct HksParam g_agreeParams01Init008[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }
};
static struct HksParam g_agreeParams01Finish008[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};
static struct HksParam g_agreeParams02Init008[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }
};
static struct HksParam g_agreeParams02Finish008[] = {
    {
        .tag = HKS_TAG_KEY_STORAGE_FLAG,
        .uint32Param = HKS_STORAGE_TEMP
    }
};

static struct HksBlob g_keyAlias01009 = {
    strlen("HksECDHAgreeKeyAliasTest009_1"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest009_1"
};
static struct HksBlob g_keyAlias02009 = {
    strlen("HksECDHAgreeKeyAliasTest009_2"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest009_2"
};
static struct HksParam g_genParams009[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_NONE
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};
static struct HksParam g_agreeParams01Init009[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }
};
static struct HksParam g_agreeParams02Init009[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }
};

static struct HksBlob g_keyAlias01010 = {
    strlen("HksECDHAgreeKeyAliasTest0010_1"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest0010_1"
};
static struct HksBlob g_keyAlias02010 = {
    strlen("HksECDHAgreeKeyAliasTest0010_2"),
    (uint8_t *)"HksECDHAgreeKeyAliasTest0010_2"
};
static struct HksParam g_genParams010[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_NONE
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};
static struct HksParam g_agreeParams01Init010[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }
};
static struct HksParam g_agreeParams02Init010[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_ECDH
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_AGREE
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_ECC_KEY_SIZE_521
    }
};

/**
 * @tc.name: HksEcdhAgreePart2Test.HksEcdhAgree006
 * @tc.desc: alg-EDCH, pur-AGREE, size-256
 * @tc.type: FUNC
 */
HWTEST_F(HksEcdhAgreePart2Test, HksEcdhAgree006, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams006, sizeof(g_genParams006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init006, sizeof(g_agreeParams01Init006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_agreeParams01Finish006, sizeof(g_agreeParams01Finish006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init006, sizeof(g_agreeParams02Init006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_agreeParams02Finish006, sizeof(g_agreeParams02Finish006)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01006, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02006, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    struct HksBlob publicKey01 = { .size = HKS_ECC_KEY_SIZE_256, .data = nullptr };
    struct HksBlob publicKey02 = { .size = HKS_ECC_KEY_SIZE_256, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey01, publicKey01.size), HKS_SUCCESS) << "Malloc publicKey01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey02, publicKey02.size), HKS_SUCCESS) << "Malloc publicKey02 failed.";
    ret = HksEcdhAgreeExport(&g_keyAlias01006, &g_keyAlias02006, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportKey failed.";

    struct HksBlob outData01 = { .size = Unittest::EcdhAgree::ECDH_COMMON_SIZE, .data = nullptr };
    struct HksBlob outData02 = { .size = Unittest::EcdhAgree::ECDH_COMMON_SIZE, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&outData01, outData01.size), HKS_SUCCESS) << "Malloc outData01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&outData02, outData02.size), HKS_SUCCESS) << "Malloc outData02 failed.";
    ret = HksEcdhAgreeFinish(&g_keyAlias01006, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEcdhAgreeFinish01 failed.";
    ret = HksAgreeKey(initParamSet02, &g_keyAlias02006, &publicKey01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAgreeKey02 failed.";
    EXPECT_EQ(TestCmpKeyAliasHash(&outData01, &outData02), HKS_SUCCESS) << "outData01 not equals outData02";

    HksDeleteKey(&g_keyAlias01006, genParamSet);
    HksDeleteKey(&g_keyAlias02006, genParamSet);
    HksEcdhAgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
    HksEcdhAgreeFreeBlob(&publicKey01, &publicKey02, &outData01, &outData02);
}

/**
 * @tc.name: HksEcdhAgreePart2Test.HksEcdhAgree007
 * @tc.desc: alg-EDCH, pur-AGREE, size-384
 * @tc.type: FUNC
 */
HWTEST_F(HksEcdhAgreePart2Test, HksEcdhAgree007, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams007, sizeof(g_genParams007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init007, sizeof(g_agreeParams01Init007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_agreeParams01Finish007, sizeof(g_agreeParams01Finish007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init007, sizeof(g_agreeParams02Init007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_agreeParams02Finish007, sizeof(g_agreeParams02Finish007)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01007, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02007, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    struct HksBlob publicKey01 = { .size = HKS_ECC_KEY_SIZE_384, .data = nullptr };
    struct HksBlob publicKey02 = { .size = HKS_ECC_KEY_SIZE_384, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey01, publicKey01.size), HKS_SUCCESS) << "Malloc publicKey01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey02, publicKey02.size), HKS_SUCCESS) << "Malloc publicKey02 failed.";
    ret = HksEcdhAgreeExport(&g_keyAlias01007, &g_keyAlias02007, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportKey failed.";

    struct HksBlob outData01 = { .size = Unittest::EcdhAgree::ECDH_COMMON_SIZE, .data = nullptr };
    struct HksBlob outData02 = { .size = Unittest::EcdhAgree::ECDH_COMMON_SIZE, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&outData01, outData01.size), HKS_SUCCESS) << "Malloc outData01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&outData02, outData02.size), HKS_SUCCESS) << "Malloc outData02 failed.";
    ret = HksEcdhAgreeFinish(&g_keyAlias01007, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEcdhAgreeFinish01 failed.";
    ret = HksAgreeKey(initParamSet02, &g_keyAlias02007, &publicKey01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAgreeKey02 failed.";
    EXPECT_EQ(TestCmpKeyAliasHash(&outData01, &outData02), HKS_SUCCESS) << "outData01 not equals outData02";

    HksDeleteKey(&g_keyAlias01007, genParamSet);
    HksDeleteKey(&g_keyAlias02007, genParamSet);
    HksEcdhAgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
    HksEcdhAgreeFreeBlob(&publicKey01, &publicKey02, &outData01, &outData02);
}

/**
 * @tc.name: HksEcdhAgreePart2Test.HksEcdhAgree008
 * @tc.desc: alg-EDCH, pur-AGREE, size-521
 * @tc.type: FUNC
 */
HWTEST_F(HksEcdhAgreePart2Test, HksEcdhAgree008, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *finishParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    struct HksParamSet *finishParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams008, sizeof(g_genParams008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init008, sizeof(g_agreeParams01Init008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&finishParamSet01, g_agreeParams01Finish008, sizeof(g_agreeParams01Finish008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init008, sizeof(g_agreeParams02Init008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";
    ret = InitParamSet(&finishParamSet02, g_agreeParams02Finish008, sizeof(g_agreeParams02Finish008)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(finish)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01008, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02008, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    struct HksBlob publicKey01 = { .size = HKS_ECC_KEY_SIZE_521, .data = nullptr };
    struct HksBlob publicKey02 = { .size = HKS_ECC_KEY_SIZE_521, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey01, publicKey01.size), HKS_SUCCESS) << "Malloc publicKey01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey02, publicKey02.size), HKS_SUCCESS) << "Malloc publicKey02 failed.";
    ret = HksEcdhAgreeExport(&g_keyAlias01008, &g_keyAlias02008, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportKey failed.";

    struct HksBlob outData01 = { .size = Unittest::EcdhAgree::ECDH_COMMON_SIZE, .data = nullptr };
    struct HksBlob outData02 = { .size = Unittest::EcdhAgree::ECDH_COMMON_SIZE, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&outData01, outData01.size), HKS_SUCCESS) << "Malloc outData01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&outData02, outData02.size), HKS_SUCCESS) << "Malloc outData02 failed.";
    ret = HksEcdhAgreeFinish(&g_keyAlias01008, &publicKey02, initParamSet01, finishParamSet01, &outData01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEcdhAgreeFinish01 failed.";
    ret = HksAgreeKey(initParamSet02, &g_keyAlias02008, &publicKey01, &outData02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAgreeKey02 failed.";
    EXPECT_EQ(TestCmpKeyAliasHash(&outData01, &outData02), HKS_SUCCESS) << "outData01 not equals outData02";

    HksDeleteKey(&g_keyAlias01008, genParamSet);
    HksDeleteKey(&g_keyAlias02008, genParamSet);
    HksEcdhAgreeFreeParamSet(genParamSet, initParamSet01, finishParamSet01, initParamSet02, finishParamSet02);
    HksEcdhAgreeFreeBlob(&publicKey01, &publicKey02, &outData01, &outData02);
}

/**
 * @tc.name: HksEcdhAgreePart2Test.HksEcdhAgree009
 * @tc.desc: alg-EDCH, pur-AGREE, size-521
 * @tc.type: FUNC
 */
HWTEST_F(HksEcdhAgreePart2Test, HksEcdhAgree009, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams009, sizeof(g_genParams009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init009, sizeof(g_agreeParams01Init009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init009, sizeof(g_agreeParams02Init009)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01009, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02009, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    struct HksBlob publicKey01 = { .size = HKS_ECC_KEY_SIZE_521, .data = nullptr };
    struct HksBlob publicKey02 = { .size = HKS_ECC_KEY_SIZE_521, .data = nullptr };
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey01, publicKey01.size), HKS_SUCCESS) << "Malloc publicKey01 failed.";
    EXPECT_EQ(MallocAndCheckBlobData(&publicKey02, publicKey02.size), HKS_SUCCESS) << "Malloc publicKey02 failed.";
    ret = HksEcdhAgreeExport(&g_keyAlias01009, &g_keyAlias02009, &publicKey01, &publicKey02, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportKey failed.";

    ret = HksEcdhAgreeAbort(&g_keyAlias01009, &publicKey02, initParamSet01);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEcdhAgreeAbort01 failed.";
    ret = HksEcdhAgreeAbort(&g_keyAlias02009, &publicKey01, initParamSet02);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEcdhAgreeAbort02 failed.";

    HksDeleteKey(&g_keyAlias01009, genParamSet);
    HksDeleteKey(&g_keyAlias02009, genParamSet);
    HksEcdhAgreeFreeParamSet(genParamSet, initParamSet01, initParamSet02);
    HksEcdhAgreeFreeBlob(&publicKey01, &publicKey02);
}

/**
 * @tc.name: HksEcdhAgreePart2Test.HksEcdhAgree0010
 * @tc.desc: alg-EDCH, pur-AGREE, size-521
 * @tc.type: FUNC    abnormal
 */
HWTEST_F(HksEcdhAgreePart2Test, HksEcdhAgree0010, TestSize.Level0)
{
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *initParamSet01 = nullptr;
    struct HksParamSet *initParamSet02 = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams010, sizeof(g_genParams010)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = InitParamSet(&initParamSet01, g_agreeParams01Init010, sizeof(g_agreeParams01Init010)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)01 failed.";
    ret = InitParamSet(&initParamSet02, g_agreeParams02Init010, sizeof(g_agreeParams02Init010)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(init & update)02 failed.";

    ret = HksGenerateKey(&g_keyAlias01010, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey01 failed.";
    ret = HksGenerateKey(&g_keyAlias02010, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey02 failed.";

    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    ret = HksInit(NULL, initParamSet01, &handle);
    EXPECT_NE(ret, HKS_SUCCESS) << "HksInit01 should failed.";
    ret = HksInit(NULL, initParamSet02, &handle);
    EXPECT_NE(ret, HKS_SUCCESS) << "HksInit02 should failed.";

    HksDeleteKey(&g_keyAlias01010, genParamSet);
    HksDeleteKey(&g_keyAlias02010, genParamSet);
    HksEcdhAgreeFreeParamSet(genParamSet, initParamSet01, initParamSet02);
}
} // namespace Unittest::EcdhAgree