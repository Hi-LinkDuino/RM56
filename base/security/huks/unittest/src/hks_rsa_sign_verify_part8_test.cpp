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

#include "hks_rsa_sign_verify_part8_test.h"
#include "hks_rsa_sign_verify_test_common.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::RsaSignVerify {
class HksRsaSignVerifyPart8Test : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksRsaSignVerifyPart8Test::SetUpTestCase(void)
{
}

void HksRsaSignVerifyPart8Test::TearDownTestCase(void)
{
}

void HksRsaSignVerifyPart8Test::SetUp()
{
}

void HksRsaSignVerifyPart8Test::TearDown()
{
}

static struct HksParam g_genParamsTest071[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    },
};
static struct HksParam g_signParamsTest071[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_verifyParamsTest071[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }
};
static struct HksParam g_genParamsTest072[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    },
};
static struct HksParam g_signParamsTest072[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_verifyParamsTest072[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_genParamsTest073[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    },
};
static struct HksParam g_signParamsTest073[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_verifyParamsTest073[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};
static struct HksParam g_genParamsTest074[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    },
};
static struct HksParam g_signParamsTest074[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};

static struct HksParam g_genParamsTest075[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    },
};
static struct HksParam g_signParamsTest075[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_SIGN
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_PSS
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }
};

/**
 * @tc.name: HksRsaSignVerifyPart8Test.HksRsaSignVerifyPart8Test071
 * @tc.desc: alg-RSA pur-Sign pad-PKCS1_V1_5 digest-MD5.
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaSignVerifyPart8Test, HksRsaSignVerifyPart8Test071, TestSize.Level1)
{
    int32_t ret = HKS_FAILURE;
    const char *keyAliasString = "HksRSASignVerifyKeyAliasTest071";
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;

    ret = InitParamSet(&genParamSet, g_genParamsTest071, sizeof(g_genParamsTest071)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&signParamSet, g_signParamsTest071, sizeof(g_signParamsTest071)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&verifyParamSet, g_verifyParamsTest071, sizeof(g_verifyParamsTest071)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* Generate Key */
    struct HksBlob keyAlias = {strlen(keyAliasString), (uint8_t *)keyAliasString};

    if ((genParamSet != nullptr) || (signParamSet != nullptr) || (verifyParamSet != nullptr)) {
        ret = HksRsaSignVerifyTestNormalCase(keyAlias, genParamSet, signParamSet, verifyParamSet);
    }

    /* Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksRsaSignVerifyPart8Test.HksRsaSignVerifyPart8Test072
 * @tc.desc: alg-RSA pur-Sign pad-PKCS1_V1_5 digest-MD5.
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaSignVerifyPart8Test, HksRsaSignVerifyPart8Test072, TestSize.Level1)
{
    int32_t ret = HKS_FAILURE;
    const char *keyAliasString = "HksRSASignVerifyKeyAliasTest072";
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;

    ret = InitParamSet(&genParamSet, g_genParamsTest072, sizeof(g_genParamsTest072)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&signParamSet, g_signParamsTest072, sizeof(g_signParamsTest072)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&verifyParamSet, g_verifyParamsTest072, sizeof(g_verifyParamsTest072)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* Generate Key */
    struct HksBlob keyAlias = {strlen(keyAliasString), (uint8_t *)keyAliasString};

    if ((genParamSet != nullptr) || (signParamSet != nullptr) || (verifyParamSet != nullptr)) {
        ret = HksRsaSignVerifyTestNormalCase(keyAlias, genParamSet, signParamSet, verifyParamSet);
    }

    /* Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksRsaSignVerifyPart8Test.HksRsaSignVerifyPart8Test073
 * @tc.desc: alg-RSA pur-Sign pad-PKCS1_V1_5 digest-MD5. Verify-Abort
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaSignVerifyPart8Test, HksRsaSignVerifyPart8Test073, TestSize.Level1)
{
    struct HksBlob keyAlias = { strlen("HksRSASignVerifyKeyAliasTest073"),
        (uint8_t *)"HksRSASignVerifyKeyAliasTest073" };
    struct HksBlob inData = { Unittest::RsaSignVerify::g_inData.length(),
                              (uint8_t *)Unittest::RsaSignVerify::g_inData.c_str() };

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParamsTest073, sizeof(g_genParamsTest073)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    struct HksParamSet *signParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_signParamsTest073, sizeof(g_signParamsTest073)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(&keyAlias, signParamSet, &handleSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleSign, signParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    uint8_t outDataS[Unittest::RsaSignVerify::RSA_COMMON_SIZE] = {0};
    struct HksBlob outDataSign = { Unittest::RsaSignVerify::RSA_COMMON_SIZE, outDataS };
    ret = HksFinish(&handleSign, signParamSet, &inData, &outDataSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";

    /* 3. Export Public Key */
    uint8_t pubKey[HKS_RSA_KEY_SIZE_4096] = {0};
    struct HksBlob publicKey = { HKS_RSA_KEY_SIZE_4096, pubKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey failed.";

    /* 4. Verify Three Stage (Abort) */
    struct HksParamSet *verifyParamSet = nullptr;
    ret = InitParamSet(&verifyParamSet, g_verifyParamsTest073, sizeof(g_verifyParamsTest073)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    /* Import Key */
    char newKey[] = "RSA_Sign_Verify_Import_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(newKey), .data = (uint8_t *)newKey };
    ret = HksImportKey(&newKeyAlias, verifyParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ImportKey failed";
    // Init
    uint8_t handleV[sizeof(uint64_t)] = {0};
    struct HksBlob handleVerify = { sizeof(uint64_t), handleV };
    ret = HksInit(&newKeyAlias, verifyParamSet, &handleVerify);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleVerify, verifyParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Abort
    ret = HksAbort(&handleVerify, verifyParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Abort failed.";

    /* 5. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    ret = HksDeleteKey(&newKeyAlias, verifyParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete ImportKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
    HksFreeParamSet(&verifyParamSet);
}

/**
 * @tc.name: HksRsaSignVerifyPart8Test.HksRsaSignVerifyPart8Test074
 * @tc.desc: alg-RSA pur-Sign pad-PKCS1_V1_5 digest-MD5. Sign-Abort
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaSignVerifyPart8Test, HksRsaSignVerifyPart8Test074, TestSize.Level1)
{
    struct HksBlob keyAlias = { strlen("HksRSASignVerifyKeyAliasTest074"),
        (uint8_t *)"HksRSASignVerifyKeyAliasTest074" };
    struct HksBlob inData = { Unittest::RsaSignVerify::g_inData.length(),
                              (uint8_t *)Unittest::RsaSignVerify::g_inData.c_str() };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParamsTest074, sizeof(g_genParamsTest074)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    struct HksParamSet *signParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_signParamsTest074, sizeof(g_signParamsTest074)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(&keyAlias, signParamSet, &handleSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleSign, signParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Abort
    ret = HksAbort(&handleSign, signParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Abort failed.";

    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
}

/**
 * @tc.name: HksRsaSignVerifyPart8Test.HksRsaSignVerifyPart8Test075
 * @tc.desc: alg-RSA pur-Sign pad-PKCS1_V1_5 digest-MD5. abnormal
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaSignVerifyPart8Test, HksRsaSignVerifyPart8Test075, TestSize.Level1)
{
    struct HksBlob keyAlias = { strlen("HksRSASignVerifyKeyAliasTest075"),
        (uint8_t *)"HksRSASignVerifyKeyAliasTest075" };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParamsTest075, sizeof(g_genParamsTest075)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    struct HksParamSet *signParamSet = nullptr;
    ret = InitParamSet(&signParamSet, g_signParamsTest075, sizeof(g_signParamsTest075)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(NULL, signParamSet, &handleSign);
    EXPECT_NE(ret, HKS_SUCCESS) << "Init failed.";

    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
}
} // namespace Unittest::RsaSignVerify