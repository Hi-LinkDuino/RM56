/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>
#include <gtest/gtest.h>
#include <securec.h>
#include <sys/time.h>

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"

using namespace testing::ext;
namespace {
namespace {
const char GENERATE_KEY[] = "This is for generate key";
const char IMPORT_KEY[] = "This is for import key";
const uint32_t TEST_FREQUENCY = 1000;
const uint32_t MAX_SDK_VERSION_SIZE = 64;
const uint32_t IV_SIZE = 16;
const uint32_t COMPLEMENT_LEN = 16;
const uint32_t KEY_PARAMSET_SIZE = 1024;
const uint32_t MESSAGE_SIZE = 64;
const uint32_t TEST_KEY_SIZE = 512;
const uint32_t DERIVED_KEY_SIZE = 64;

static const struct HksParam PARAMS_FOR_ENCRYPT[] = {
    { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
    { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
    { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
    { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
    { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
    { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
    { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
    { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
};

static const struct HksParam PARAMS_FOR_DECRYPT[] = {
    { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
    { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
    { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
    { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
    { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
    { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
    { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
    { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
};
}  // namespace
class PressureTest : public testing::Test {
public:
    int32_t LocalHksGenerate(const uint32_t keyLen, const struct HksBlob *authId, const struct HksParamSet *paramSetIn,
        struct HksBlob *priKey, struct HksBlob *pubKey) const;
};

int32_t PressureTest::LocalHksGenerate(const uint32_t keyLen, const struct HksBlob *authId,
    const struct HksParamSet *paramSetIn, struct HksBlob *priKey, struct HksBlob *pubKey) const
{
    struct HksParamSet *paramOutSet = nullptr;
    HksInitParamSet(&paramOutSet);
    struct HksParam localKey = {
        .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
        .blob = { .size = keyLen, .data = (uint8_t *)HksMalloc(keyLen) }
    };
    if (localKey.blob.data == nullptr) {
        return HKS_FAILURE;
    }
    HksAddParams(paramOutSet, &localKey, 1);
    HksBuildParamSet(&paramOutSet);

    if (HksGenerateKey(authId, paramSetIn, paramOutSet) != HKS_SUCCESS) {
        return HKS_SUCCESS;
    }

    HksParam *priParam = nullptr;
    HksGetParam(paramOutSet, HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA, &priParam);
    priKey->size = priParam->blob.size;
    (void)memcpy_s(priKey->data, priParam->blob.size, priParam->blob.data, priParam->blob.size);

    HksParam *pubParam = nullptr;
    HksGetParam(paramOutSet, HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA, &pubParam);
    pubKey->size = pubParam->blob.size;
    (void)memcpy_s(pubKey->data, pubParam->blob.size, pubParam->blob.data, pubParam->blob.size);

    HksFree(localKey.blob.data);
    HksFreeParamSet(&paramOutSet);
    return HKS_SUCCESS;
}

/**
 * @tc.number    : PressureTest.PressureTest00100
 * @tc.name      : PressureTest00100
 * @tc.desc      : HksGetSdkVersion
 */
HWTEST_F(PressureTest, PressureTest00100, TestSize.Level1)
{
    double programTimes = 0;

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksBlob sdkVersion = { .size = MAX_SDK_VERSION_SIZE,
            .data = (uint8_t *)HksMalloc(MAX_SDK_VERSION_SIZE) };
        ASSERT_NE(sdkVersion.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksGetSdkVersion(&sdkVersion);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());
        HksFree(sdkVersion.data);
    }
    HKS_LOG_I("HksGetSdkVersion Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00200
 * @tc.name      : PressureTest00200
 * @tc.desc      : HksInitialize
 */
HWTEST_F(PressureTest, PressureTest00200, TestSize.Level1)
{
    double programTimes = 0;

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksInitialize();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());
    }
    HKS_LOG_I("HksInitialize Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00300
 * @tc.name      : PressureTest00300
 * @tc.desc      : HksRefreshKeyInfo
 */
HWTEST_F(PressureTest, PressureTest00300, TestSize.Level1)
{
    double programTimes = 0;

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksRefreshKeyInfo();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());
    }
    HKS_LOG_I("HksRefreshKeyInfo Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00400
 * @tc.name      : PressureTest00400
 * @tc.desc      : HksGenerateKey
 */
HWTEST_F(PressureTest, PressureTest00400, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        };

        HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        HksBuildParamSet(&paramInSet);

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksGenerateKey(&authId, paramInSet, NULL);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksDeleteKey(&authId, paramInSet);
        HksFreeParamSet(&paramInSet);
    }
    HKS_LOG_I("HksGenerateKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00500
 * @tc.name      : PressureTest00500
 * @tc.desc      : HksImportKey
 */
HWTEST_F(PressureTest, PressureTest00500, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    struct HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(pubKey.data, nullptr);
    HksExportPublicKey(&authId, paramInSet, &pubKey);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksBlob importId = { strlen(IMPORT_KEY), (uint8_t *)IMPORT_KEY };
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksImportKey(&importId, paramInSet, &pubKey);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());
        HksDeleteKey(&importId, paramInSet);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFree(pubKey.data);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksImportKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00600
 * @tc.name      : PressureTest00600
 * @tc.desc      : HksExportPublicKey
 */
HWTEST_F(PressureTest, PressureTest00600, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(pubKey.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksExportPublicKey(&authId, paramInSet, &pubKey);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(pubKey.data);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksExportPublicKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00700
 * @tc.name      : PressureTest00700
 * @tc.desc      : HksDeleteKey
 */
HWTEST_F(PressureTest, PressureTest00700, TestSize.Level1)
{
    double programTimes = 0;

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        };

        HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        HksBuildParamSet(&paramInSet);

        HksGenerateKey(&authId, paramInSet, NULL);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksDeleteKey(&authId, paramInSet);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFreeParamSet(&paramInSet);
    }
    HKS_LOG_I("HksDeleteKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00800
 * @tc.name      : PressureTest00800
 * @tc.desc      : HksGetKeyParamSet
 */
HWTEST_F(PressureTest, PressureTest00800, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksParamSet *paramOutSet = nullptr;
        HksInitParamSet(&paramOutSet);
        struct HksParam localKey = { .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = KEY_PARAMSET_SIZE, .data = (uint8_t *)HksMalloc(KEY_PARAMSET_SIZE) } };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramOutSet, &localKey, 1);

        HksBuildParamSet(&paramOutSet);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksGetKeyParamSet(&authId, paramInSet, paramOutSet);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramOutSet);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksGetKeyParamSet Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest00900
 * @tc.name      : PressureTest00900
 * @tc.desc      : HksKeyExist
 */
HWTEST_F(PressureTest, PressureTest00900, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksKeyExist(&authId, paramInSet);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());
    }
    HksDeleteKey(&authId, paramInSet);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksKeyExist Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01000
 * @tc.name      : PressureTest01000
 * @tc.desc      : HksGenerateRandom
 */
HWTEST_F(PressureTest, PressureTest01000, TestSize.Level1)
{
    double programTimes = 0;

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksBlob authId = { .size = TEST_KEY_SIZE, .data = (uint8_t *)HksMalloc(TEST_KEY_SIZE) };
        ASSERT_NE(authId.data, nullptr);
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        };

        HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        HksBuildParamSet(&paramInSet);

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksGenerateRandom(paramInSet, &authId);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(authId.data);
        HksFreeParamSet(&paramInSet);
    }
    HKS_LOG_I("HksGenerateRandom Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01100
 * @tc.name      : PressureTest01100
 * @tc.desc      : HksSign
 */
HWTEST_F(PressureTest, PressureTest01100, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
        HksBlob signature = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(signature.data, nullptr);

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksSign(&authId, paramInSet, &message, &signature);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(signature.data);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksSign Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01200
 * @tc.name      : PressureTest01200
 * @tc.desc      : HksVerify
 */
HWTEST_F(PressureTest, PressureTest01200, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);
    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
    HksBlob signature = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(signature.data, nullptr);

    HksSign(&authId, paramInSet, &message, &signature);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksVerify(&authId, paramInSet, &message, &signature);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());
    }
    HksDeleteKey(&authId, paramInSet);
    HksFree(signature.data);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksVerify Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01300
 * @tc.name      : PressureTest01300
 * @tc.desc      : HksEncrypt
 */
HWTEST_F(PressureTest, PressureTest01300, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
    };

    uint8_t iv[IV_SIZE] = {0};
    struct HksParam tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
    HksAddParams(paramInSet, &tagIv, 1);

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob plainText = { .size = dataLen, .data = (uint8_t *)hexData };

        uint32_t inLen = dataLen + COMPLEMENT_LEN;
        HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(cipherText.data, nullptr);

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksEncrypt(&authId, paramInSet, &plainText, &cipherText);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(cipherText.data);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksEncrypt Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01400
 * @tc.name      : PressureTest01400
 * @tc.desc      : HksDecrypt
 */
HWTEST_F(PressureTest, PressureTest01400, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_PKCS7 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
    };

    uint8_t iv[IV_SIZE] = {0};
    struct HksParam tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
    HksAddParams(paramInSet, &tagIv, 1);

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob plainText = { .size = dataLen, .data = (uint8_t *)hexData };

    uint32_t inLen = dataLen + COMPLEMENT_LEN;
    HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
    ASSERT_NE(cipherText.data, nullptr);

    HksEncrypt(&authId, paramInSet, &plainText, &cipherText);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        HksBlob plainTextDecrypt = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(plainTextDecrypt.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksDecrypt(&authId, paramInSet, &cipherText, &plainTextDecrypt);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(plainTextDecrypt.data);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFree(cipherText.data);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksDecrypt Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01500
 * @tc.name      : PressureTest01500
 * @tc.desc      : HksAgreeKey
 */
HWTEST_F(PressureTest, PressureTest01500, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSetForKey = nullptr;
    HksInitParamSet(&paramInSetForKey);
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSetForKey, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));

    HksParam algForKey = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC };
    HksParam alg = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH };

    HksAddParams(paramInSetForKey, &algForKey, 1);
    HksAddParams(paramInSet, &alg, 1);
    HksParam degistForKey = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE };
    HksAddParams(paramInSetForKey, &degistForKey, 1);
    HksBuildParamSet(&paramInSetForKey);
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSetForKey, NULL);

    struct HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(pubKey.data, nullptr);
    HksExportPublicKey(&authId, paramInSetForKey, &pubKey);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        HksBlob agreeKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(agreeKey.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksAgreeKey(paramInSet, &authId, &pubKey, &agreeKey);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(agreeKey.data);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFree(pubKey.data);
    HksFreeParamSet(&paramInSetForKey);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksAgreeKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01600
 * @tc.name      : PressureTest01600
 * @tc.desc      : HksDeriveKey
 */
HWTEST_F(PressureTest, PressureTest01600, TestSize.Level1)
{
    double programTimes = 0;

    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_128 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DERIVE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    struct HksParamSet *paramInSetHkdf = nullptr;
    HksInitParamSet(&paramInSetHkdf);
    struct HksParam tmpParamsHkdf[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HKDF },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = DERIVED_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DERIVE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSetHkdf, tmpParamsHkdf, sizeof(tmpParamsHkdf) / sizeof(tmpParamsHkdf[0]));
    HksBuildParamSet(&paramInSetHkdf);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        HksBlob derivedKey = { .size = DERIVED_KEY_SIZE, .data = (uint8_t *)HksMalloc(DERIVED_KEY_SIZE) };
        ASSERT_NE(derivedKey.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksDeriveKey(paramInSetHkdf, &authId, &derivedKey);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(derivedKey.data);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFreeParamSet(&paramInSet);
    HksFreeParamSet(&paramInSetHkdf);
    HKS_LOG_I("HksDeriveKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01700
 * @tc.name      : PressureTest01700
 * @tc.desc      : HksMac
 */
HWTEST_F(PressureTest, PressureTest01700, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = TEST_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateKey(&authId, paramInSet, NULL);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
        HksBlob macMessage = { .size = MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(MESSAGE_SIZE) };
        ASSERT_NE(macMessage.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksMac(&authId, paramInSet, &message, &macMessage);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(macMessage.data);
    }
    HksDeleteKey(&authId, paramInSet);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksMac Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01800
 * @tc.name      : PressureTest01800
 * @tc.desc      : HksHash
 */
HWTEST_F(PressureTest, PressureTest01800, TestSize.Level1)
{
    double programTimes = 0;

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam digest = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 };
    HksAddParams(paramInSet, &digest, 1);
    HksBuildParamSet(&paramInSet);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
        HksBlob shaMessage = { .size = MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(MESSAGE_SIZE) };
        ASSERT_NE(shaMessage.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksHash(paramInSet, &message, &shaMessage);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(shaMessage.data);
    }
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("HksHash Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest01900
 * @tc.name      : PressureTest01900
 * @tc.desc      : HksGenerateKey
 */
HWTEST_F(PressureTest, PressureTest01900, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);

        struct HksParam tmpParams[] = {
            { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
            { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
            { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
            { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_VERIFY },
            { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
            { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
            { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
        };

        HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
        HksBuildParamSet(&paramInSet);

        struct HksParamSet *paramOutSet = nullptr;
        HksInitParamSet(&paramOutSet);
        struct HksParam localKey = { .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) } };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramOutSet, &localKey, 1);

        HksBuildParamSet(&paramOutSet);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksGenerateKey(&authId, paramInSet, paramOutSet);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(localKey.blob.data);
        HksFreeParamSet(&paramInSet);
        HksFreeParamSet(&paramOutSet);
    }
    HKS_LOG_I("Local HksGenerateKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest02000
 * @tc.name      : PressureTest02000
 * @tc.desc      : HksSign
 */
HWTEST_F(PressureTest, PressureTest02000, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksBlob priKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(priKey.data, nullptr);
    HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(pubKey.data, nullptr);

    PressureTest::LocalHksGenerate(HKS_ECC_KEY_SIZE_224, &authId, paramInSet, &priKey, &pubKey);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
        HksBlob signature = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(signature.data, nullptr);

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksSign(&priKey, paramInSet, &message, &signature);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(signature.data);
    }
    HksFree(priKey.data);
    HksFree(pubKey.data);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("Local HksSign Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest02100
 * @tc.name      : PressureTest02100
 * @tc.desc      : HksVerify
 */
HWTEST_F(PressureTest, PressureTest02100, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksBlob priKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(priKey.data, nullptr);
    HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(pubKey.data, nullptr);

    PressureTest::LocalHksGenerate(HKS_ECC_KEY_SIZE_224, &authId, paramInSet, &priKey, &pubKey);

    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
    HksBlob signature = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(signature.data, nullptr);

    HksSign(&priKey, paramInSet, &message, &signature);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksVerify(&pubKey, paramInSet, &message, &signature);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());
    }
    HksFree(priKey.data);
    HksFree(pubKey.data);
    HksFree(signature.data);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("Local HksVerify Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest02200
 * @tc.name      : PressureTest02200
 * @tc.desc      : HksEncrypt
 */
HWTEST_F(PressureTest, PressureTest02200, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    uint8_t iv[IV_SIZE] = {0};
    struct HksParam tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
    HksAddParams(paramInSet, &tagIv, 1);

    HksAddParams(paramInSet, PARAMS_FOR_ENCRYPT, sizeof(PARAMS_FOR_ENCRYPT) / sizeof(PARAMS_FOR_ENCRYPT[0]));
    HksBuildParamSet(&paramInSet);

    struct HksParamSet *paramOutSet = nullptr;
    HksInitParamSet(&paramOutSet);
    struct HksParam localKey = { .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
        .blob = { .size = HKS_AES_KEY_SIZE_128, .data = (uint8_t *)HksMalloc(HKS_AES_KEY_SIZE_128) } };
    ASSERT_NE(localKey.blob.data, nullptr);
    HksAddParams(paramOutSet, &localKey, 1);
    HksBuildParamSet(&paramOutSet);

    HksGenerateKey(&authId, paramInSet, paramOutSet);

    HksParam *paramOut = nullptr;
    HksGetParam(paramOutSet, HKS_TAG_SYMMETRIC_KEY_DATA, &paramOut);
    HksBlob authKey = { .size = paramOut->blob.size, .data = (uint8_t *)HksMalloc(paramOut->blob.size) };
    ASSERT_NE(authKey.data, nullptr);
    (void)memcpy_s(authKey.data, paramOut->blob.size, paramOut->blob.data, paramOut->blob.size);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob plainText = { .size = dataLen, .data = (uint8_t *)hexData };

        uint32_t inLen = dataLen + COMPLEMENT_LEN;
        HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(cipherText.data, nullptr);

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksEncrypt(&authKey, paramInSet, &plainText, &cipherText);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(cipherText.data);
    }
    HksFree(localKey.blob.data);
    HksFree(authKey.data);
    HksFreeParamSet(&paramInSet);
    HksFreeParamSet(&paramOutSet);
    HKS_LOG_I("Local HksEncrypt Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest02300
 * @tc.name      : PressureTest02300
 * @tc.desc      : HksDecrypt
 */
HWTEST_F(PressureTest, PressureTest02300, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    uint8_t iv[IV_SIZE] = {0};
    struct HksParam tagIv = { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv } };
    HksAddParams(paramInSet, &tagIv, 1);

    HksAddParams(paramInSet, PARAMS_FOR_DECRYPT, sizeof(PARAMS_FOR_DECRYPT) / sizeof(PARAMS_FOR_DECRYPT[0]));
    HksBuildParamSet(&paramInSet);

    struct HksParamSet *paramOutSet = nullptr;
    HksInitParamSet(&paramOutSet);
    struct HksParam localKey = { .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
        .blob = { .size = HKS_AES_KEY_SIZE_128, .data = (uint8_t *)HksMalloc(HKS_AES_KEY_SIZE_128) } };
    ASSERT_NE(localKey.blob.data, nullptr);
    HksAddParams(paramOutSet, &localKey, 1);
    HksBuildParamSet(&paramOutSet);

    HksGenerateKey(&authId, paramInSet, paramOutSet);

    HksParam *paramOut = nullptr;
    HksGetParam(paramOutSet, HKS_TAG_SYMMETRIC_KEY_DATA, &paramOut);
    HksBlob authKey = { .size = paramOut->blob.size, .data = (uint8_t *)HksMalloc(paramOut->blob.size) };
    ASSERT_NE(authKey.data, nullptr);
    (void)memcpy_s(authKey.data, paramOut->blob.size, paramOut->blob.data, paramOut->blob.size);

    const char *hexData = "0123456789abcdef";
    uint32_t dataLen = strlen(hexData);
    HksBlob plainText = { .size = dataLen, .data = (uint8_t *)hexData };

    uint32_t inLen = dataLen + COMPLEMENT_LEN;
    HksBlob cipherText = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
    ASSERT_NE(cipherText.data, nullptr);

    HksEncrypt(&authKey, paramInSet, &plainText, &cipherText);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        HksBlob plainTextDecrypt = { .size = inLen, .data = (uint8_t *)HksMalloc(inLen) };
        ASSERT_NE(plainTextDecrypt.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksDecrypt(&authKey, paramInSet, &cipherText, &plainTextDecrypt);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(plainTextDecrypt.data);
    }
    HksFree(localKey.blob.data);
    HksFree(authKey.data);
    HksFree(cipherText.data);
    HksFreeParamSet(&paramInSet);
    HksFreeParamSet(&paramOutSet);
    HKS_LOG_I("Local HksDecrypt Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest02400
 * @tc.name      : PressureTest02400
 * @tc.desc      : HksAgreeKey
 */
HWTEST_F(PressureTest, PressureTest02400, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSetForKey = nullptr;
    HksInitParamSet(&paramInSetForKey);
    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_ECC_KEY_SIZE_224 },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_AGREE },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSetForKey, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));

    HksParam algForKey = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECC };
    HksParam alg = { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_ECDH };

    HksAddParams(paramInSetForKey, &algForKey, 1);
    HksAddParams(paramInSet, &alg, 1);
    HksParam degistForKey = { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_NONE };
    HksAddParams(paramInSetForKey, &degistForKey, 1);
    HksBuildParamSet(&paramInSetForKey);
    HksBuildParamSet(&paramInSet);

    HksBlob priKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(priKey.data, nullptr);
    HksBlob pubKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
    ASSERT_NE(pubKey.data, nullptr);

    PressureTest::LocalHksGenerate(HKS_ECC_KEY_SIZE_224, &authId, paramInSetForKey, &priKey, &pubKey);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        HksBlob agreeKey = { .size = HKS_ECC_KEY_SIZE_224, .data = (uint8_t *)HksMalloc(HKS_ECC_KEY_SIZE_224) };
        ASSERT_NE(agreeKey.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksAgreeKey(paramInSet, &priKey, &pubKey, &agreeKey);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(agreeKey.data);
    }
    HksFree(priKey.data);
    HksFree(pubKey.data);
    HksFreeParamSet(&paramInSetForKey);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("Local HksAgreeKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest02500
 * @tc.name      : PressureTest02500
 * @tc.desc      : HksDeriveKey
 */
HWTEST_F(PressureTest, PressureTest02500, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { .size = TEST_KEY_SIZE, .data = (uint8_t *)HksMalloc(TEST_KEY_SIZE) };
    ASSERT_NE(authId.data, nullptr);

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HKDF },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = DERIVED_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DERIVE },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    HksGenerateRandom(paramInSet, &authId);
    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        HksBlob derivedKey = { .size = DERIVED_KEY_SIZE, .data = (uint8_t *)HksMalloc(DERIVED_KEY_SIZE) };
        ASSERT_NE(derivedKey.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksDeriveKey(paramInSet, &authId, &derivedKey);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(derivedKey.data);
    }
    HksFree(authId.data);
    HksFreeParamSet(&paramInSet);
    HKS_LOG_I("Local HksDeriveKey Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}

/**
 * @tc.number    : PressureTest.PressureTest02600
 * @tc.name      : PressureTest02600
 * @tc.desc      : HksMac
 */
HWTEST_F(PressureTest, PressureTest02600, TestSize.Level1)
{
    double programTimes = 0;
    struct HksBlob authId = { strlen(GENERATE_KEY), (uint8_t *)GENERATE_KEY };

    struct HksParamSet *paramInSet = nullptr;
    HksInitParamSet(&paramInSet);

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = TEST_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    };

    HksAddParams(paramInSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0]));
    HksBuildParamSet(&paramInSet);

    struct HksParamSet *paramOutSet = nullptr;
    HksInitParamSet(&paramOutSet);
    struct HksParam localKey = { .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
        .blob = { .size = HKS_AES_KEY_SIZE_128, .data = (uint8_t *)HksMalloc(HKS_AES_KEY_SIZE_128) } };
    ASSERT_NE(localKey.blob.data, nullptr);
    HksAddParams(paramOutSet, &localKey, 1);
    HksBuildParamSet(&paramOutSet);

    HksGenerateKey(&authId, paramInSet, paramOutSet);

    HksParam *paramOut = nullptr;
    HksGetParam(paramOutSet, HKS_TAG_SYMMETRIC_KEY_DATA, &paramOut);
    HksBlob authKey = { .size = paramOut->blob.size, .data = (uint8_t *)HksMalloc(paramOut->blob.size) };
    ASSERT_NE(authKey.data, nullptr);
    (void)memcpy_s(authKey.data, paramOut->blob.size, paramOut->blob.data, paramOut->blob.size);

    for (uint32_t ii = 0; ii < TEST_FREQUENCY; ii++) {
        const char *hexData = "0123456789abcdef";
        uint32_t dataLen = strlen(hexData);
        HksBlob message = { .size = dataLen, .data = (uint8_t *)hexData };
        HksBlob macMessage = { .size = MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(MESSAGE_SIZE) };
        ASSERT_NE(macMessage.data, nullptr);
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        int32_t ret = HksMac(&authKey, paramInSet, &message, &macMessage);
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
        EXPECT_EQ(ret, HKS_SUCCESS);
        programTimes += (end.time_since_epoch().count() - start.time_since_epoch().count());

        HksFree(macMessage.data);
    }
    HksFree(localKey.blob.data);
    HksFree(authKey.data);
    HksFreeParamSet(&paramInSet);
    HksFreeParamSet(&paramOutSet);
    HKS_LOG_I("Local HksMac Interface Call Duration: %f", (programTimes / TEST_FREQUENCY));
}
}  // namespace