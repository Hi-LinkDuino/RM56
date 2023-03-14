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

#include "js_async_work_tdd_test.h"
#include "js_async_work.h"

namespace OHOS {
namespace ACELite {
static int8_t g_statusCode = 0;

void JsAsyncWorkTddTest::SetUp()
{
    g_statusCode = -1;
}

static void Execute(void *data)
{
    (void)data;
}

static void ExecuteWithCode(void *data, int8_t statusCode)
{
    (void)data;
    g_statusCode = statusCode;
}

void JsAsyncWorkTddTest::JsAsyncWorkTest001()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call DispatchAsyncWork with valid parameters
     */
    bool res = JsAsyncWork::DispatchAsyncWork(Execute, nullptr);
    /**
     * @tc.expected: step1. res is false due to null AceAbility instance
     */
    EXPECT_FALSE(res);
    /**
     * @tc.steps: step2. call overloaded DispatchAsyncWork with valid parameters
     */
    res = JsAsyncWork::DispatchAsyncWork(ExecuteWithCode, nullptr);
    /**
     * @tc.expected: step2. res is false due to null AceAbility instance
     */
    EXPECT_FALSE(res);
    TDD_CASE_END();
}

void JsAsyncWorkTddTest::JsAsyncWorkTest002()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call DispatchAsyncWork with invalid parameters
     */
    AsyncWorkHandler handler1 = nullptr;
    bool res = JsAsyncWork::DispatchAsyncWork(handler1, nullptr);
    /**
     * @tc.expected: step1. res is false due to invalid parameters, and error log can be checked
     */
    EXPECT_FALSE(res);
    /**
     * @tc.steps: step2. call overloaded DispatchAsyncWork with invalid parameters
     */
    AsyncHandler handler2 = nullptr;
    res = JsAsyncWork::DispatchAsyncWork(handler2, nullptr);
    /**
     * @tc.expected: step2. res is false due to invalid parameters, and error log can be checked
     */
    EXPECT_FALSE(res);
    TDD_CASE_END();
}

void JsAsyncWorkTddTest::JsAsyncWorkTest003()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call ExecuteAsyncWork with valid parameters
     */
    AsyncWork *asyncWork = new AsyncWork();
    EXPECT_TRUE(asyncWork != nullptr);

    asyncWork->workHandler = Execute;
    asyncWork->handler = ExecuteWithCode;
    JsAsyncWork::ExecuteAsyncWork(asyncWork);
    JsAsyncWork::ExecuteAsyncWork(asyncWork, JsAsyncWork::ERR_OK);
    /**
     * @tc.expected: step1. log can be checked and g_statusCode = JsAsyncWork::ERR_OK
     */
    EXPECT_TRUE(g_statusCode == JsAsyncWork::ERR_OK);
    if (asyncWork != nullptr) {
        delete asyncWork;
        asyncWork = nullptr;
    }
    TDD_CASE_END();
}

void JsAsyncWorkTddTest::JsAsyncWorkTest004()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call ExecuteAsyncWork with null asyncWork
     */
    AsyncWork *asyncWork = nullptr;
    JsAsyncWork::ExecuteAsyncWork(asyncWork);
    JsAsyncWork::ExecuteAsyncWork(asyncWork, JsAsyncWork::ERR_OK);
    /**
     * @tc.expected: step1. error log can be checked and g_statusCode != JsAsyncWork::ERR_OK
     */
    EXPECT_TRUE(g_statusCode != JsAsyncWork::ERR_OK);
    TDD_CASE_END();
}

void JsAsyncWorkTddTest::JsAsyncWorkTest005()
{
    TDD_CASE_BEGIN();
    /**
     * @tc.steps: step1. call ExecuteAsyncWork with null workHandler
     */
    AsyncWork *asyncWork = new AsyncWork();
    EXPECT_TRUE(asyncWork != nullptr);
    JsAsyncWork::ExecuteAsyncWork(asyncWork);
    JsAsyncWork::ExecuteAsyncWork(asyncWork, JsAsyncWork::ERR_OK);
    /**
     * @tc.expected: step1. error log can be checked and g_statusCode != JsAsyncWork::ERR_OK
     */
    EXPECT_TRUE(g_statusCode != JsAsyncWork::ERR_OK);
    if (asyncWork != nullptr) {
        delete asyncWork;
        asyncWork = nullptr;
    }
    TDD_CASE_END();
}

void JsAsyncWorkTddTest::RunTests()
{
    JsAsyncWorkTest001();
    JsAsyncWorkTest002();
    JsAsyncWorkTest003();
    JsAsyncWorkTest004();
    JsAsyncWorkTest005();
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: JsAsyncWorkTest001
 * @tc.desc: Verify DispatchAsyncWork with valid parameters.
 */
HWTEST_F(JsAsyncWorkTddTest, test001, TestSize.Level1)
{
    JsAsyncWorkTddTest::JsAsyncWorkTest001();
}

/**
 * @tc.name: JsAsyncWorkTest002
 * @tc.desc: Verify DispatchAsyncWork with invalid parameters.
 */
HWTEST_F(JsAsyncWorkTddTest, test002, TestSize.Level1)
{
    JsAsyncWorkTddTest::JsAsyncWorkTest002();
}

/**
 * @tc.name: JsAsyncWorkTest003
 * @tc.desc: Verify ExcuteAsyncWork with valid parameters.
 */
HWTEST_F(JsAsyncWorkTddTest, test003, TestSize.Level1)
{
    JsAsyncWorkTddTest::JsAsyncWorkTest003();
}

/**
 * @tc.name: JsAsyncWorkTest004
 * @tc.desc: Verify ExcuteAsyncWork with invalid parameters.
 */
HWTEST_F(JsAsyncWorkTddTest, test004, TestSize.Level1)
{
    JsAsyncWorkTddTest::JsAsyncWorkTest004();
}

/**
 * @tc.name: JsAsyncWorkTest005
 * @tc.desc: Verify ExcuteAsyncWork with invalid parameters.
 */
HWTEST_F(JsAsyncWorkTddTest, test005, TestSize.Level1)
{
    JsAsyncWorkTddTest::JsAsyncWorkTest005();
}
#endif
} // namespace ACELite
} // namespace OHOS
