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

#include "calculator.h"
#include <gtest/gtest.h>

using namespace testing::ext;

class CalculatorSubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void CalculatorSubTest::SetUpTestCase(void)
{
    // step 2: input testsuit setup step
}

void CalculatorSubTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void CalculatorSubTest::SetUp(void)
{
    // step 3: input testcase setup step
}

void CalculatorSubTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

/**
 * @tc.name: integer_sub_001
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(CalculatorSubTest, integer_sub_001, TestSize.Level1)
{
    EXPECT_EQ(4, Sub(4, 0));
}

/**
 * @tc.name: integer_sub_002
 * @tc.desc: Verify the sub function.
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(CalculatorSubTest, integer_sub_002, TestSize.Level3)
{
    EXPECT_EQ(-2, Sub(5, 7));
}

