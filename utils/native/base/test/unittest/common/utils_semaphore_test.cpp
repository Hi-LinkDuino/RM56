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
#include <gtest/gtest.h>
#include "thread_pool.h"
#include <cstdio>

using namespace testing::ext;
using namespace OHOS;

class UtilsSemaphoreTest : public testing::Test
{
public :
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UtilsSemaphoreTest::SetUpTestCase(void)
{
    // input testsuit setup step
}

void UtilsSemaphoreTest::TearDownTestCase(void)
{
    // input testsuit teardown step
}

void UtilsSemaphoreTest::SetUp(void)
{
    // recover times
}

void UtilsSemaphoreTest::TearDown(void)
{
    // recover times
}

HWTEST_F(UtilsSemaphoreTest, test_01, TestSize.Level0)
{
    EXPECT_EQ(0, 0);
}

