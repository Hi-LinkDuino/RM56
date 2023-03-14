/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "detector.h"
#include <gtest/gtest.h>

using namespace testing::ext;

class DetectorPrimeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DetectorPrimeTest::SetUpTestCase(void)
{
    // step 2: input testsuit setup step
}

void DetectorPrimeTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void DetectorPrimeTest::SetUp(void)
{
    // step 3: input testcase setup step
}

void DetectorPrimeTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

/**
 * @tc.name: check_prime_number_01
 * @tc.desc: Check if the specified element is prime.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorPrimeTest, check_prime_number_01, TestSize.Level1)
{
    EXPECT_FALSE(IsPrime(6));
}

/**
 * @tc.name: check_prime_number_02
 * @tc.desc: Check if the specified element is prime.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorPrimeTest, check_prime_number_02, TestSize.Level3)
{
    EXPECT_TRUE(IsPrime(3)) << "This is the test information.";
}

/**
 * @tc.name: check_prime_number_03
 * @tc.desc: Check if the specified element is prime.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorPrimeTest, check_prime_number_03, TestSize.Level3)
{
    printf("check_prime_number_03 start.\n");
    EXPECT_FALSE(IsPrime(0));
    printf("check_prime_number_03 finish.\n");
}

/**
 * @tc.name: check_prime_number_04
 * @tc.desc: Check if the specified element is prime.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorPrimeTest, check_prime_number_04, TestSize.Level1)
{
    EXPECT_FALSE(IsPrime(-5));
}

/**
 * @tc.name: check_prime_number_05
 * @tc.desc: Check if the specified element is prime.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorPrimeTest, check_prime_number_05, TestSize.Level2)
{
    EXPECT_FALSE(IsPrime(-100));
}

/**
 * @tc.name: check_prime_number_06
 * @tc.desc: Check if the specified element is prime.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorPrimeTest, check_prime_number_06, TestSize.Level2)
{
    EXPECT_FALSE(IsPrime(INT_MIN));
}
