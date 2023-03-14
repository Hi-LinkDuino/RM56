/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "hks_others_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_type.h"

#include "securec.h"

using namespace testing::ext;
namespace {
#ifndef _CUT_AUTHENTICATE_
class HksOthersTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksOthersTest::SetUpTestCase(void)
{
}

void HksOthersTest::TearDownTestCase(void)
{
}

void HksOthersTest::SetUp()
{
    EXPECT_EQ(HksInitialize(), 0);
}

void HksOthersTest::TearDown()
{
}

/**
 * @tc.name: HksOthersTest.HksOthersTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest001, TestSize.Level1)
{
    int32_t ret = HksGetKeyParamSet(NULL, NULL, NULL);
    ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest002, TestSize.Level1)
{
    int32_t ret = HksGetKeyInfoList(NULL, NULL, NULL);
    ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest003, TestSize.Level1)
{
    int32_t ret = HksAttestKey(NULL, NULL, NULL);
    ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest004
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest004, TestSize.Level1)
{
    int32_t ret = HksGetCertificateChain(NULL, NULL, NULL);
    ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest005
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest005, TestSize.Level1)
{
    int32_t ret = HksWrapKey(NULL, NULL, NULL, NULL);
    ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest006
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest006, TestSize.Level1)
{
    int32_t ret = HksUnwrapKey(NULL, NULL, NULL, NULL);
    ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest006
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest007, TestSize.Level1)
{
    int32_t ret = HksGetSdkVersion(NULL);
    ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest008
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksOthersTest, HksOthersTest008, TestSize.Level1)
{
    int32_t ret = HksInitialize();
    ASSERT_TRUE(ret == HKS_SUCCESS);
}
#endif /* _CUT_AUTHENTICATE_ */
}