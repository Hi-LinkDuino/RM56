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

#include "hks_ed25519_sign_verify_test.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::Ed25519 {
class HksEd25519SignVerifyTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksEd25519SignVerifyTest::SetUpTestCase(void)
{
}

void HksEd25519SignVerifyTest::TearDownTestCase(void)
{
}

void HksEd25519SignVerifyTest::SetUp()
{
}

void HksEd25519SignVerifyTest::TearDown()
{
}

void HksTestFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3)
{
    HksFreeParamSet(&paramSet1);
    HksFreeParamSet(&paramSet2);
    HksFreeParamSet(&paramSet3);
}

int32_t HksTestSignVerify(struct HksBlob *keyAlias, struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData, bool isSign)
{
    uint8_t tmpHandle[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), tmpHandle };
    int32_t ret = HksInit(keyAlias, paramSet, &handle);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    ret = TestUpdateFinish(&handle, paramSet, inData, outData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateFinish failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    if (isSign) {
        uint8_t tmpOut[Unittest::Ed25519::ED25519_COMMON_SIZE] = {0};
        struct HksBlob outData1 = { Unittest::Ed25519::ED25519_COMMON_SIZE, tmpOut };
        ret = HksSign(keyAlias, paramSet, inData, &outData1);
        EXPECT_EQ(ret, HKS_SUCCESS) << "HksSign failed.";
    } else {
        ret = HksVerify(keyAlias, paramSet, inData, outData);
        EXPECT_EQ(ret, HKS_SUCCESS) << "HksVerify failed.";
    }

    return ret;
}

/**
 * @tc.name: HksEd25519SignVerifyTest.HksEd25519SignVerifyTest001
 * @tc.desc: alg-ED25519 pur-Sign.
 * @tc.type: FUNC
 */
HWTEST_F(HksEd25519SignVerifyTest, HksEd25519SignVerifyTest001, TestSize.Level0)
{
    const char *keyAliasString = "HksED25519SignVerifyKeyAliasTest001";
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    int32_t ret = HKS_FAILURE;

    ret = InitParamSet(&genParamSet, Unittest::Ed25519::g_genParamsTest001,
        sizeof(Unittest::Ed25519::g_genParamsTest001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&signParamSet, Unittest::Ed25519::g_signParamsTest001,
        sizeof(Unittest::Ed25519::g_signParamsTest001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&verifyParamSet, Unittest::Ed25519::g_verifyParamsTest001,
        sizeof(Unittest::Ed25519::g_verifyParamsTest001)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    struct HksBlob keyAlias = { strlen(keyAliasString), (uint8_t *)keyAliasString };

    /* 1. Generate Key */
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    uint8_t outDataS[Unittest::Ed25519::ED25519_COMMON_SIZE] = {0};
    struct HksBlob outDataSign = { Unittest::Ed25519::ED25519_COMMON_SIZE, outDataS };
    ret = HksTestSignVerify(&keyAlias, signParamSet, &g_inData, &outDataSign, true);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Sign failed.";

    /* 3. Export Public Key */
    uint8_t pubKey[HKS_CURVE25519_KEY_SIZE_256] = {0};
    struct HksBlob publicKey = { HKS_CURVE25519_KEY_SIZE_256, pubKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey failed.";

    /* 4. Import Key */
    char newKey[] = "ECC_Sign_Verify_Import_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(newKey), .data = (uint8_t *)newKey };
    ret = HksImportKey(&newKeyAlias, verifyParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ImportKey failed";

    /* 5. Verify Three Stage */
    ret = HksTestSignVerify(&newKeyAlias, verifyParamSet, &g_inData, &outDataSign, false);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Verify failed.";

    /* 5. Delete Key */
    EXPECT_EQ(HksDeleteKey(&keyAlias, genParamSet), HKS_SUCCESS) << "DeleteKey failed.";
    EXPECT_EQ(HksDeleteKey(&newKeyAlias, verifyParamSet), HKS_SUCCESS) << "Delete ImportKey failed.";

    HksTestFreeParamSet(genParamSet, signParamSet, verifyParamSet);
}

/**
 * @tc.name: HksEd25519SignVerifyTest.HksEd25519SignVerifyTest002
 * @tc.desc: alg-ED25519 pur-Sign. Verify-Abort
 * @tc.type: FUNC
 */
HWTEST_F(HksEd25519SignVerifyTest, HksEd25519SignVerifyTest002, TestSize.Level0)
{
    const char *keyAliasString = "HksED25519SignVerifyKeyAliasTest002";
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *signParamSet = nullptr;
    struct HksParamSet *verifyParamSet = nullptr;
    struct HksBlob keyAlias = { strlen(keyAliasString), (uint8_t *)keyAliasString };

    int32_t ret = InitParamSet(&genParamSet, Unittest::Ed25519::g_genParamsTest002,
        sizeof(Unittest::Ed25519::g_genParamsTest002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&signParamSet, Unittest::Ed25519::g_signParamsTest002,
        sizeof(Unittest::Ed25519::g_signParamsTest002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    ret = InitParamSet(&verifyParamSet, Unittest::Ed25519::g_verifyParamsTest002,
        sizeof(Unittest::Ed25519::g_verifyParamsTest002)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* 1. Generate Key */
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(&keyAlias, signParamSet, &handleSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleSign, signParamSet, &Unittest::Ed25519::g_inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    uint8_t outDataS[Unittest::Ed25519::ED25519_COMMON_SIZE] = {0};
    struct HksBlob outDataSign = { Unittest::Ed25519::ED25519_COMMON_SIZE, outDataS };
    ret = HksFinish(&handleSign, signParamSet, &Unittest::Ed25519::g_inData, &outDataSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";

    /* 3. Export Public Key */
    uint8_t pubKey[HKS_CURVE25519_KEY_SIZE_256] = {0};
    struct HksBlob publicKey = { HKS_CURVE25519_KEY_SIZE_256, pubKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey failed.";

    /* 4. Import Key */
    char newKey[] = "ED25519_Sign_Verify_Import_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(newKey), .data = (uint8_t *)newKey };
    ret = HksImportKey(&newKeyAlias, verifyParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ImportKey failed";

    /* 5. Verify Three Stage */
    // Init
    uint8_t handleV[sizeof(uint64_t)] = {0};
    struct HksBlob handleVerify = { sizeof(uint64_t), handleV };
    ret = HksInit(&newKeyAlias, verifyParamSet, &handleVerify);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleVerify, verifyParamSet, &Unittest::Ed25519::g_inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Abort
    ret = HksAbort(&handleVerify, verifyParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Abort failed.";

    /* 5. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    ret = HksDeleteKey(&newKeyAlias, verifyParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete ImportKey failed.";

    HksTestFreeParamSet(genParamSet, signParamSet, verifyParamSet);
}

/**
 * @tc.name: HksEd25519SignVerifyTest.HksEd25519SignVerifyTest003
 * @tc.desc: alg-ED25519 pur-Sign. Sign-Abort
 * @tc.type: FUNC
 */
HWTEST_F(HksEd25519SignVerifyTest, HksEd25519SignVerifyTest003, TestSize.Level0)
{
    const char *keyAliasString = "HksED25519SignVerifyKeyAliasTest003";
    struct HksBlob keyAlias = { strlen(keyAliasString), (uint8_t *)keyAliasString };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, Unittest::Ed25519::g_genParamsTest003,
        sizeof(Unittest::Ed25519::g_genParamsTest003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage (Abort) */
    struct HksParamSet *signParamSet = nullptr;
    ret = InitParamSet(&signParamSet, Unittest::Ed25519::g_signParamsTest003,
        sizeof(Unittest::Ed25519::g_signParamsTest003)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(&keyAlias, signParamSet, &handleSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleSign, signParamSet, &Unittest::Ed25519::g_inData);
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
 * @tc.name: HksEd25519SignVerifyTest.HksEd25519SignVerifyTest004
 * @tc.desc: alg-ED25519 pur-Sign. abnormal
 * @tc.type: FUNC
 */
HWTEST_F(HksEd25519SignVerifyTest, HksEd25519SignVerifyTest004, TestSize.Level0)
{
    const char *keyAliasString = "HksED25519SignVerifyKeyAliasTest004";
    struct HksBlob keyAlias = { strlen(keyAliasString), (uint8_t *)keyAliasString };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, Unittest::Ed25519::g_genParamsTest004,
        sizeof(Unittest::Ed25519::g_genParamsTest004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    struct HksParamSet *signParamSet = nullptr;
    ret = InitParamSet(&signParamSet, Unittest::Ed25519::g_signParamsTest004,
        sizeof(Unittest::Ed25519::g_signParamsTest004)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(NULL, signParamSet, &handleSign);
    EXPECT_NE(ret, HKS_SUCCESS) << "Init should failed.";

    /* 3. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&signParamSet);
}
} // namespace Unittest::Ed25519