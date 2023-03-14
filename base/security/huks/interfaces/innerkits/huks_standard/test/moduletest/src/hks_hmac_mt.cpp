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

#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <securec.h>

#include "hks_api.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "openssl_hmac_helper.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
namespace {
struct TestCaseParams {
    std::vector<HksParam> params;
    std::string hexData;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode hmacResult = HksErrorCode::HKS_SUCCESS;
};

const char HMAC_KEY[] = "This is a HMAC key";

const TestCaseParams HUKS_HMAC_MT_00100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00600_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00700_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00800_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_00900_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_01000_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_TEMP },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = false },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_01100_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA1 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_01200_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA224 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_01300_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA256 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_01400_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA384 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};

const TestCaseParams HUKS_HMAC_MT_01500_PARAMS = {
    .params = {
        { .tag = HKS_TAG_KEY_STORAGE_FLAG, .uint32Param = HKS_STORAGE_PERSISTENT },
        { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_HMAC },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HMAC_KEY_SIZE },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_MAC },
        { .tag = HKS_TAG_DIGEST, .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_IS_KEY_ALIAS, .boolParam = true },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE, .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT },
    },
    .hexData = "0123456789abcdef",
    .generateKeyResult = HKS_SUCCESS,
    .hmacResult = HKS_SUCCESS,
};
}  // namespace

class HksHmacMt : public testing::Test {
protected:
    uint32_t ReadValueByTag(std::vector<HksParam> params, uint32_t needTag)
    {
        for (auto testParams : params) {
            if (testParams.tag == needTag) {
                return testParams.uint32Param;
            }
        }
        return 0;
    }

    void LocalHmacScenario(int32_t scenario, struct HksBlob *authId, const TestCaseParams &testCaseParams,
        struct HksBlob *macForHuks, struct HksBlob *message)
    {
        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size());
        HksBuildParamSet(&paramInSet);
        uint32_t digest = ReadValueByTag(testCaseParams.params, HKS_TAG_DIGEST);

        struct HksParamSet *paramOutSet = nullptr;
        HksInitParamSet(&paramOutSet);
        struct HksParam localKey = {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { .size = HMAC_KEY_SIZE, .data = (uint8_t *)HksMalloc(HMAC_KEY_SIZE) }
        };
        ASSERT_NE(localKey.blob.data, nullptr);
        HksAddParams(paramOutSet, &localKey, 1);
        HksBuildParamSet(&paramOutSet);
        if (scenario == 0) {
            EXPECT_EQ(HksGenerateKey(authId, paramInSet, paramOutSet), testCaseParams.generateKeyResult);
        } else if (scenario == 1) {
            EXPECT_EQ(HmacGenerateKey(HMAC_KEY_SIZE, authId), testCaseParams.generateKeyResult);
        }

        HksParam *outParam = nullptr;
        HksGetParam(paramOutSet, HKS_TAG_SYMMETRIC_KEY_DATA, &outParam);
        HksBlob key = { .size = outParam->blob.size, .data = (uint8_t *)HksMalloc(outParam->blob.size) };
        ASSERT_NE(key.data, nullptr);
        (void)memcpy_s(key.data, outParam->blob.size, outParam->blob.data, outParam->blob.size);
        HksBlob macMessage = { .size = HMAC_MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(HMAC_MESSAGE_SIZE) };
        ASSERT_NE(macMessage.data, nullptr);

            EXPECT_EQ(HmacHmac(&key, digest, message, &macMessage), testCaseParams.hmacResult);
            EXPECT_EQ(HksMac(&key, paramInSet, message, macForHuks), testCaseParams.hmacResult);

        EXPECT_EQ(macMessage.size, macForHuks->size);
        EXPECT_EQ(HksMemCmp(macMessage.data, macForHuks->data, macForHuks->size), HKS_SUCCESS);
        if (scenario == 1) {
            HksFree(authId->data);
        }
        HksFreeParamSet(&paramInSet);
        HksFreeParamSet(&paramOutSet);
        HksFree(localKey.blob.data);
        HksFree(macMessage.data);
        HksFree(key.data);
    }

    void RunTestCase(const TestCaseParams &testCaseParams, int32_t scenario)
    {
        struct HksBlob authId = { (uint32_t)strlen(HMAC_KEY), (uint8_t *)HMAC_KEY };

        struct HksParamSet *paramInSet = nullptr;
        HksInitParamSet(&paramInSet);
        HksAddParams(paramInSet, testCaseParams.params.data(), testCaseParams.params.size());
        HksBuildParamSet(&paramInSet);

        uint32_t storage = ReadValueByTag(testCaseParams.params, HKS_TAG_KEY_STORAGE_FLAG);

        HksBlob message = {
            .size = (uint32_t)testCaseParams.hexData.length(),
            .data = (uint8_t *)&testCaseParams.hexData[0],
        };
        HksBlob macForHuks = { .size = HMAC_MESSAGE_SIZE, .data = (uint8_t *)HksMalloc(HMAC_MESSAGE_SIZE) };
        ASSERT_NE(macForHuks.data, nullptr);

        if (storage == HKS_STORAGE_TEMP) {
            LocalHmacScenario(scenario, &authId, testCaseParams, &macForHuks, &message);
        } else if (storage == HKS_STORAGE_PERSISTENT) {
            EXPECT_EQ(HksGenerateKey(&authId, paramInSet, NULL), testCaseParams.generateKeyResult);
            EXPECT_EQ(HksMac(&authId, paramInSet, &message, &macForHuks), testCaseParams.hmacResult);
        }

        HksFreeParamSet(&paramInSet);
        HksFree(macForHuks.data);
    }
};

/**
 * @tc.number    : HksHmacMt.HksHmacMt00100
 * @tc.name      : HksHmacMt00100
 * @tc.desc      : huks generates HMAC key, which can be successfully used for OpenSSL using HMAC-SHA1 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00100, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00100_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00200
 * @tc.name      : HksHmacMt00200
 * @tc.desc      : huks generates HMAC key, which can be successfully used for OpenSSL using HMAC-SHA224 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00200, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00200_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00300
 * @tc.name      : HksHmacMt00300
 * @tc.desc      : huks generates HMAC key, which can be successfully used for OpenSSL using HMAC-SHA256 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00300, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00300_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00400
 * @tc.name      : HksHmacMt00400
 * @tc.desc      : huks generates HMAC key, which can be successfully used for OpenSSL using HMAC-SHA384 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00400, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00400_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00500
 * @tc.name      : HksHmacMt00500
 * @tc.desc      : huks generates HMAC key, which can be successfully used for OpenSSL using HMAC-SHA512 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00500, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00500_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00600
 * @tc.name      : HksHmacMt00600
 * @tc.desc      : OpenSSL generates HMAC key, which can be successfully used for huks using HMAC-SHA1 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00600, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00600_PARAMS, 1);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00700
 * @tc.name      : HksHmacMt00700
 * @tc.desc      : OpenSSL generates HMAC key, which can be successfully used for huks using HMAC-SHA224 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00700, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00700_PARAMS, 1);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00800
 * @tc.name      : HksHmacMt00800
 * @tc.desc      : OpenSSL generates HMAC key, which can be successfully used for huks using HMAC-SHA256 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00800, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00800_PARAMS, 1);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt00900
 * @tc.name      : HksHmacMt00900
 * @tc.desc      : OpenSSL generates HMAC key, which can be successfully used for huks using HMAC-SHA384 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt00900, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_00900_PARAMS, 1);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt01000
 * @tc.name      : HksHmacMt01000
 * @tc.desc      : OpenSSL generates HMAC key, which can be successfully used for huks using HMAC-SHA512 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt01000, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_01000_PARAMS, 1);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt01100
 * @tc.name      : HksHmacMt01100
 * @tc.desc      : huks generates HMAC key, which can be successfully used for huks using HMAC-SHA1 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt01100, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_01100_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt01200
 * @tc.name      : HksHmacMt01200
 * @tc.desc      : huks generates HMAC key, which can be successfully used for huks using HMAC-SHA224 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt01200, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_01200_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt01300
 * @tc.name      : HksHmacMt01300
 * @tc.desc      : huks generates HMAC key, which can be successfully used for huks using HMAC-SHA256 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt01300, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_01300_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt01400
 * @tc.name      : HksHmacMt01400
 * @tc.desc      : huks generates HMAC key, which can be successfully used for huks using HMAC-SHA384 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt01400, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_01400_PARAMS, 0);
}

/**
 * @tc.number    : HksHmacMt.HksHmacMt01500
 * @tc.name      : HksHmacMt01500
 * @tc.desc      : huks generates HMAC key, which can be successfully used for huks using HMAC-SHA512 hash operation
 */
HWTEST_F(HksHmacMt, HksHmacMt01500, TestSize.Level0)
{
    RunTestCase(HUKS_HMAC_MT_01500_PARAMS, 0);
}
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS