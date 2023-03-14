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

#include "hctest.h"
#include <hi_watchdog.h>
#include "hks_api.h"
#include "hks_param.h"
#include "securec.h"

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(husk, huks_lite, HksAesTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksAesTestSetUp()
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
static BOOL HksAesTestTearDown()
{
    LiteTestPrint("tearDown\n");
    hi_watchdog_enable();
    return TRUE;
}

#ifndef _CUT_AUTHENTICATE_
#ifndef _CUT_ED25519_
/**
 * @tc.name: HksCipherTest.HksCipherTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest001, Level1)
{
    TEST_ASSERT_TRUE(TestAes256ByAgree() == 0);
}

/**
 * @tc.name: HksCipherTest.HksCipherTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest002, Level1)
{
    TEST_ASSERT_TRUE(TestAes256ByAgree1() == 0);
}

/**
 * @tc.name: HksCipherTest.HksCipherTest003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest003, Level1)
{
    TEST_ASSERT_TRUE(TestAes256ByAgree2() == 0);
}
#endif
#endif /* _CUT_AUTHENTICATE_ */

/**
 * @tc.name: HksCipherTest.HksCipherTest004
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksAesTest, HksAesTest004, Level1)
{
    TEST_ASSERT_EQUAL(TestAes256ByLocal(), 0);
}

RUN_TEST_SUITE(HksAesTest);
