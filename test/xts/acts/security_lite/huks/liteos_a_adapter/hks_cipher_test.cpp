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

#include "hks_cipher_test.h"

#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_cipher.h"

#include "securec.h"

using namespace testing::ext;
namespace {
class HksCipherTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksCipherTest::SetUpTestCase(void)
{
}

void HksCipherTest::TearDownTestCase(void)
{
}

void HksCipherTest::SetUp()
{
    EXPECT_EQ(HksInitialize(), 0);
}

void HksCipherTest::TearDown()
{
}

/**
 * @tc.name: HksCipherTest.HksCipherTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
HWTEST_F(HksCipherTest, HksCipherTest001, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestCipher(1, 0, 1) == 0);
}

#ifndef _CUT_AUTHENTICATE_
HWTEST_F(HksCipherTest, HksCipherTest002, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestCipher(1, 1, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest003, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestEncrypt(1, 0, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest004, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestEncrypt(1, 1, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest005, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestEncrypt(1, 2, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest006, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestEncrypt(1, 3, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest007, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestEncrypt(1, 4, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest008, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestDecrypt(1, 0, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest009, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestDecrypt(1, 1, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest010, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestDecrypt(1, 2, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest011, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestDecrypt(1, 3, 1) == 0);
}

HWTEST_F(HksCipherTest, HksCipherTest012, TestSize.Level1)
{
    ASSERT_TRUE(BaseTestDecrypt(1, 4, 1) == 0);
}
#endif /* _CUT_AUTHENTICATE_ */
}