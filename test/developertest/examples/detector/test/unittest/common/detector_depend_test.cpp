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

class DetectorDependTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DetectorDependTest::SetUpTestCase(void)
{
    // step 2: input testsuit setup step
}

void DetectorDependTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void DetectorDependTest::SetUp(void)
{
    // step 3: input testcase setup step
}

void DetectorDependTest::TearDown(void)
{
    // step 3: input testcase teardown step
}

/**
 * @tc.name: TestPoint_001
 * @tc.desc: detection prime number
 * @tc.type: FUNC
 * @tc.require: AR00000000 SR00000000
 */
HWTEST_F(DetectorDependTest, TestPoint_001, TestSize.Level1)
{
    // step 1: testcase step 1 description
    EXPECT_TRUE(IsPrime(7));

    // step 2: testcase step 2 description
    EXPECT_TRUE(IsPrime(3));
}
