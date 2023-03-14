/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include <string>

#include "hks_storage.h"

using namespace testing::ext;

namespace {
namespace {
const std::string TEST_PROCESS_NAME = "test_process";
const std::string TEST_USER_ID = "123465";
const std::string TEST_KEY_ALIAS = "key_alias";
constexpr uint32_t TEST_BLOB_SIZE = 16;
constexpr uint8_t TEST_BLOB[TEST_BLOB_SIZE] = {0};
}  // namespace

class HksStorageTest : public testing::Test {
public:
    void TearDown() override
    {
        HksBlob processName = {
            .size = TEST_PROCESS_NAME.size() + 1,
            .data = (uint8_t *)&TEST_PROCESS_NAME[0],
        };
        HksBlob userId = {
            .size = TEST_USER_ID.size() + 1,
            .data = (uint8_t *)&TEST_USER_ID[0]
        };
        HksBlob keyAlias = {
            .size = TEST_KEY_ALIAS.size() + 1,
            .data = (uint8_t *)&TEST_KEY_ALIAS[0],
        };
        HksProcessInfo hksProcessInfo = {
            .userId = userId,
            .processName = processName
        };
        HksStoreDeleteKeyBlob(&hksProcessInfo, &keyAlias, HksStorageType::HKS_STORAGE_TYPE_KEY);
    };
};

static void PrepareBlob()
{
    HksBlob processName = {
        .size = TEST_PROCESS_NAME.size() + 1,
        .data = (uint8_t *)&TEST_PROCESS_NAME[0],
    };
    HksBlob userId = {
        .size = TEST_USER_ID.size() + 1,
        .data = (uint8_t *)&TEST_USER_ID[0]
    };
    HksBlob keyAlias = {
        .size = TEST_KEY_ALIAS.size() + 1,
        .data = (uint8_t *)&TEST_KEY_ALIAS[0],
    };
    HksBlob keyBlob = {
        .size = TEST_BLOB_SIZE,
        .data = (uint8_t *)TEST_BLOB,
    };
    HksProcessInfo hksProcessInfo = {
        .userId = userId,
        .processName = processName
    };

    int32_t result = HksStoreKeyBlob(&hksProcessInfo, &keyAlias, HksStorageType::HKS_STORAGE_TYPE_KEY, &keyBlob);
    EXPECT_EQ(result, (int32_t)0);
}

HWTEST_F(HksStorageTest, HksStorageTest_00100, Function | SmallTest | Level1)
{
    HksBlob processName = {
        .size = TEST_PROCESS_NAME.size() + 1,
        .data = (uint8_t *)&TEST_PROCESS_NAME[0],
    };
    HksBlob userId = {
        .size = TEST_USER_ID.size() + 1,
        .data = (uint8_t *)&TEST_USER_ID[0]
    };
    HksBlob keyAlias = {
        .size = TEST_KEY_ALIAS.size() + 1,
        .data = (uint8_t *)&TEST_KEY_ALIAS[0],
    };
    HksBlob keyBlob = {
        .size = TEST_BLOB_SIZE,
        .data = (uint8_t *)TEST_BLOB,
    };
    HksProcessInfo hksProcessInfo = {
        .userId = userId,
        .processName = processName
    };

    int32_t result = HksStoreKeyBlob(&hksProcessInfo, &keyAlias, HksStorageType::HKS_STORAGE_TYPE_KEY, &keyBlob);
    EXPECT_EQ(result, (int32_t)0);
}

HWTEST_F(HksStorageTest, HksStorageTest_00200, Function | SmallTest | Level1)
{
    PrepareBlob();

    HksBlob processName = {
        .size = TEST_PROCESS_NAME.size() + 1,
        .data = (uint8_t *)&TEST_PROCESS_NAME[0],
    };
    HksBlob userId = {
        .size = TEST_USER_ID.size() + 1,
        .data = (uint8_t *)&TEST_USER_ID[0]
    };
    HksBlob keyAlias = {
        .size = TEST_KEY_ALIAS.size() + 1,
        .data = (uint8_t *)&TEST_KEY_ALIAS[0],
    };
    uint8_t buff[TEST_BLOB_SIZE] = {0};
    HksBlob keyBlob = {
        .size = TEST_BLOB_SIZE,
        .data = buff,
    };
    HksProcessInfo hksProcessInfo = {
        .userId = userId,
        .processName = processName
    };

    int32_t result = HksStoreGetKeyBlob(&hksProcessInfo, &keyAlias, HksStorageType::HKS_STORAGE_TYPE_KEY, &keyBlob);
    EXPECT_EQ(result, (int32_t)0);
}

HWTEST_F(HksStorageTest, HksStorageTest_00300, Function | SmallTest | Level1)
{
    PrepareBlob();

    HksBlob processName = {
        .size = TEST_PROCESS_NAME.size() + 1,
        .data = (uint8_t *)&TEST_PROCESS_NAME[0],
    };
    HksBlob userId = {
        .size = TEST_USER_ID.size() + 1,
        .data = (uint8_t *)&TEST_USER_ID[0]
    };
    HksBlob keyAlias = {
        .size = TEST_KEY_ALIAS.size() + 1,
        .data = (uint8_t *)&TEST_KEY_ALIAS[0],
    };
    HksProcessInfo hksProcessInfo = {
        .userId = userId,
        .processName = processName
    };

    int32_t result = HksStoreDeleteKeyBlob(&hksProcessInfo, &keyAlias, HksStorageType::HKS_STORAGE_TYPE_KEY);
    EXPECT_EQ(result, (int32_t)0);
}
}  // namespace
