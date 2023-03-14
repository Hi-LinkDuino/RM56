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

#ifndef _CUT_AUTHENTICATE_

#include "hks_others_test.h"

#include <hctest.h>
#include "hi_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_type.h"

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(husk, huks_lite, HksOthersTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksOthersTestSetUp()
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
static BOOL HksOthersTestTearDown()
{
    LiteTestPrint("tearDown\n");
    hi_watchdog_enable();
    return TRUE;
}

/**
 * @tc.name: HksOthersTest.HksOthersTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest001, Level1)
{
    int32_t ret = HksGetKeyParamSet(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest002
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest002, Level1)
{
    int32_t ret = HksGetKeyInfoList(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest003
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest003, Level1)
{
    int32_t ret = HksAttestKey(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest004
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest004, Level1)
{
    int32_t ret = HksGetCertificateChain(NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest005
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest005, Level1)
{
    int32_t ret = HksWrapKey(NULL, NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest006
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest006, Level1)
{
    int32_t ret = HksUnwrapKey(NULL, NULL, NULL, NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NOT_SUPPORTED);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest006
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest007, Level1)
{
    int32_t ret = HksGetSdkVersion(NULL);
    TEST_ASSERT_TRUE(ret == HKS_ERROR_NULL_POINTER);
}

/**
 * @tc.name: HksOthersTest.HksOthersTest008
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksOthersTest, HksOthersTest008, Level1)
{
    int32_t ret = HksInitialize();
    TEST_ASSERT_TRUE(ret == HKS_SUCCESS);
}
RUN_TEST_SUITE(HksOthersTest);

#endif /* _CUT_AUTHENTICATE_ */
