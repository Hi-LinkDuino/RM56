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

#include "hks_hmac_test.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::Hmac {
class HksHmacTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksHmacTest::SetUpTestCase(void)
{
}

void HksHmacTest::TearDownTestCase(void)
{
}

void HksHmacTest::SetUp()
{
}

void HksHmacTest::TearDown()
{
}

static struct HksParam g_genParams001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA1
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Hmac::COMMON_SIZE
    }
};
static struct HksParam g_hmacParams001[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA1
    }
};

static struct HksParam g_genParams002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA224
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Hmac::COMMON_SIZE
    }
};
static struct HksParam g_hmacParams002[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA224
    }
};

static struct HksParam g_genParams003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Hmac::COMMON_SIZE
    }
};
static struct HksParam g_hmacParams003[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA256
    }
};

static struct HksParam g_genParams004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Hmac::COMMON_SIZE
    }
};
static struct HksParam g_hmacParams004[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};

static struct HksParam g_genParams005[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = Unittest::Hmac::COMMON_SIZE
    }
};
static struct HksParam g_hmacParams005[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_HMAC
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_MAC
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};

int32_t HksHmacTestCase(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *hmacParamSet)
{
    struct HksBlob inData = {Unittest::Hmac::g_inData.length(), (uint8_t *)Unittest::Hmac::g_inData.c_str()};

    /* 1. Generate Key */
    int32_t ret = HksGenerateKey(keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. HMAC One Stage */
    uint8_t tmpMac[Unittest::Hmac::COMMON_SIZE] = {0};
    struct HksBlob mac = { Unittest::Hmac::COMMON_SIZE, tmpMac };
    ret = HksMac(keyAlias, hmacParamSet, &inData, &mac);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksMac failed.";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }

    /* 2. HMAC Three Stage */
    // Init
    uint8_t handle[sizeof(uint64_t)] = {0};
    struct HksBlob handleHMAC = { sizeof(uint64_t), handle };
    ret = HksInit(keyAlias, hmacParamSet, &handleHMAC);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }
    // Update & Finish
    uint8_t out[Unittest::Hmac::COMMON_SIZE] = {0};
    struct HksBlob outData = { Unittest::Hmac::COMMON_SIZE, out };
    ret = TestUpdateFinish(&handleHMAC, hmacParamSet, &inData, &outData);
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }
    EXPECT_EQ(HksMemCmp(mac.data, outData.data, outData.size), HKS_SUCCESS);

    /* 3. Delete Key */
    ret = HksDeleteKey(keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    return ret;
}

/**
 * @tc.name: HksHmacTest.HksHmacTest001
 * @tc.desc: alg-HMAC pur-MAC dig-SHA1.
 * @tc.type: FUNC
 */
HWTEST_F(HksHmacTest, HksHmacTest001, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksHMACKeyAliasTest001";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };

    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams001, sizeof(g_genParams001)/sizeof(HksParam));

    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams001, sizeof(g_hmacParams001)/sizeof(HksParam));

    ret = HksHmacTestCase(&keyAlias, genParamSet, hmacParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "this case failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}

/**
 * @tc.name: HksHmacTest.HksHmacTest002
 * @tc.desc: alg-HMAC pur-MAC dig-SHA224.
 * @tc.type: FUNC
 */
HWTEST_F(HksHmacTest, HksHmacTest002, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksHMACKeyAliasTest002";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };

    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams002, sizeof(g_genParams002)/sizeof(HksParam));

    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams002, sizeof(g_hmacParams002)/sizeof(HksParam));

    ret = HksHmacTestCase(&keyAlias, genParamSet, hmacParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "this case failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}

/**
 * @tc.name: HksHmacTest.HksHmacTest003
 * @tc.desc: alg-HMAC pur-MAC dig-SHA256.
 * @tc.type: FUNC
 */
HWTEST_F(HksHmacTest, HksHmacTest003, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksHMACKeyAliasTest003";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };

    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams003, sizeof(g_genParams003)/sizeof(HksParam));

    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams003, sizeof(g_hmacParams003)/sizeof(HksParam));

    ret = HksHmacTestCase(&keyAlias, genParamSet, hmacParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "this case failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}

/**
 * @tc.name: HksHmacTest.HksHmacTest004
 * @tc.desc: alg-HMAC pur-MAC dig-SHA384.
 * @tc.type: FUNC
 */
HWTEST_F(HksHmacTest, HksHmacTest004, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksHMACKeyAliasTest004";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };

    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams004, sizeof(g_genParams004)/sizeof(HksParam));

    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams004, sizeof(g_hmacParams004)/sizeof(HksParam));

    ret = HksHmacTestCase(&keyAlias, genParamSet, hmacParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "this case failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}

/**
 * @tc.name: HksHmacTest.HksHmacTest005
 * @tc.desc: alg-HMAC pur-MAC dig-SHA512.
 * @tc.type: FUNC
 */
HWTEST_F(HksHmacTest, HksHmacTest005, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksHMACKeyAliasTest005";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };

    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams005, sizeof(g_genParams005)/sizeof(HksParam));

    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams005, sizeof(g_hmacParams005)/sizeof(HksParam));

    ret = HksHmacTestCase(&keyAlias, genParamSet, hmacParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "this case failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}

/**
 * @tc.name: HksHmacTest.HksHmacTest006
 * @tc.desc: alg-HMAC pur-MAC dig-SHA512. Abort
 * @tc.type: FUNC
 */
HWTEST_F(HksHmacTest, HksHmacTest006, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksHMACKeyAliasTest006";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksBlob inData = { Unittest::Hmac::g_inData.length(), (uint8_t *)Unittest::Hmac::g_inData.c_str() };
    int32_t ret = HKS_FAILURE;

    /* 2. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams005, sizeof(g_genParams005)/sizeof(HksParam));
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. HMAC Three Stage(Abort) */
    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams005, sizeof(g_hmacParams005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    ret = HksInit(&keyAlias, hmacParamSet, &handle);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handle, hmacParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Abort
    ret = HksAbort(&handle, hmacParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Abort failed.";

    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}

/**
 * @tc.name: HksHmacTest.HksHmacTest007
 * @tc.desc: alg-HMAC pur-MAC dig-SHA512. abnormal
 * @tc.type: FUNC
 */
HWTEST_F(HksHmacTest, HksHmacTest007, TestSize.Level0)
{
    char tmpKeyAlias[] = "HksHMACKeyAliasTest007";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    int32_t ret = HKS_FAILURE;

    /* 2. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams005, sizeof(g_genParams005)/sizeof(HksParam));
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, NULL);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. HMAC Three Stage(Abort) */
    struct HksParamSet *hmacParamSet = nullptr;
    ret = InitParamSet(&hmacParamSet, g_hmacParams005, sizeof(g_hmacParams005)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    ret = HksInit(NULL, hmacParamSet, &handle);
    EXPECT_NE(ret, HKS_SUCCESS) << "Init should failed.";

    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&hmacParamSet);
}
} // namespace Unittest::Hmac