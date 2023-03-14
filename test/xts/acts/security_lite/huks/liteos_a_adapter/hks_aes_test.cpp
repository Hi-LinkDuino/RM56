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

#include "hks_aes_test.h"

#include "securec.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_aes.h"

using namespace testing::ext;
namespace {
class HksAesTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksAesTest::SetUpTestCase(void)
{
}

void HksAesTest::TearDownTestCase(void)
{
}

void HksAesTest::SetUp()
{
    EXPECT_EQ(HksInitialize(), 0);
}

void HksAesTest::TearDown()
{
}

#ifndef _CUT_AUTHENTICATE_
/**
 * @tc.name: HksCipherTest.HksCipherTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksAesTest, HksAesTest001, TestSize.Level1)
{
    ASSERT_TRUE(TestAes256ByAgree() == 0);
}

/**
 * @tc.name: HksCipherTest.HksCipherTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksAesTest, HksAesTest002, TestSize.Level1)
{
    ASSERT_TRUE(TestAes256ByAgree1() == 0);
}

/**
 * @tc.name: HksCipherTest.HksCipherTest003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksAesTest, HksAesTest003, TestSize.Level1)
{
    ASSERT_TRUE(TestAes256ByAgree2() == 0);
}
#endif /* _CUT_AUTHENTICATE_ */

/**
 * @tc.name: HksCipherTest.HksCipherTest003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksAesTest, HksAesTest004, TestSize.Level1)
{
    ASSERT_EQ(TestAes256ByLocal(), 0);
}
}
