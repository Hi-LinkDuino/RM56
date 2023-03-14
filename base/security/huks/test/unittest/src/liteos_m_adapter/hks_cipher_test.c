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

#include "hks_cipher_test.h"

#include "hctest.h"
#include "hi_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_cipher.h"
#include "securec.h"

#define TEST_INDEX_0    0
#define TEST_INDEX_1    1
#define TEST_INDEX_2    2
#define TEST_INDEX_3    3
#define TEST_INDEX_4    4

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(husk, huks_lite, HksCipherTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksCipherTestSetUp()
{
    LiteTestPrint("setup\n");
    hi_watchdog_disable();
    TEST_ASSERT_TRUE(HksInitialize() == 0);
    return TRUE;
}

/**
 * @tc.teardown: define a setup for test suit, format:"CalcMultiTest + TearDown"
 * @return: true——teardown success
 */
static BOOL HksCipherTestTearDown()
{
    LiteTestPrint("tearDown\n");
    hi_watchdog_enable();
    return TRUE;
}


/**
 * @tc.name: HksCipherTest.HksCipherTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksCipherTest, HksCipherTest001, Level1)
{
    TEST_ASSERT_TRUE(BaseTestCipher(1, TEST_INDEX_0, 1) == 0);
}

#ifndef _CUT_AUTHENTICATE_

LITE_TEST_CASE(HksCipherTest, HksCipherTest002, Level1)
{
    TEST_ASSERT_TRUE(BaseTestCipher(1, TEST_INDEX_1, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest003, Level1)
{
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_0, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest004, Level1)
{
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_1, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest005, Level1)
{
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_2, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest006, Level1)
{
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_3, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest007, Level1)
{
    TEST_ASSERT_TRUE(BaseTestEncrypt(1, TEST_INDEX_4, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest008, Level1)
{
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_0, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest009, Level1)
{
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_1, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest010, Level1)
{
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_2, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest011, Level1)
{
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_3, 1) == 0);
}

LITE_TEST_CASE(HksCipherTest, HksCipherTest012, Level1)
{
    TEST_ASSERT_TRUE(BaseTestDecrypt(1, TEST_INDEX_4, 1) == 0);
}
#endif /* _CUT_AUTHENTICATE_ */

RUN_TEST_SUITE(HksCipherTest);
