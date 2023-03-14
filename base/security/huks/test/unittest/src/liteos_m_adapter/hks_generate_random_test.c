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

#include "hks_generate_random_test.h"

#include "hctest.h"
#include "hi_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "hks_type.h"

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(husk, huks_lite, HksGenerateRandomTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksGenerateRandomTestSetUp()
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
static BOOL HksGenerateRandomTestTearDown()
{
    LiteTestPrint("tearDown\n");
    hi_watchdog_enable();
    return TRUE;
}

static const struct HksTestGenRandomParams g_testGenRandomParams[] = {
    /* normal case */
    { 0, HKS_SUCCESS, { true, HKS_MAX_RANDOM_LEN, true, HKS_MAX_RANDOM_LEN } },
};
/**
 * @tc.name: HksGenerateRandomTest.HksGenerateRandomTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksGenerateRandomTest, HksGenerateRandomTest001, Level1)
{
    int32_t ret;
    struct HksBlob *random = NULL;

    ret = TestConstructBlobOut(&random,
        g_testGenRandomParams[0].randomParams.blobExist,
        g_testGenRandomParams[0].randomParams.blobSize,
        g_testGenRandomParams[0].randomParams.blobDataExist,
        g_testGenRandomParams[0].randomParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = HksGenerateRandomRun(random, 1);
    if (ret != g_testGenRandomParams[0].expectResult) {
        HKS_TEST_LOG_I("HksGenerateRandomRun failed, ret[%u] = %d", g_testGenRandomParams[0].testId, ret);
    }
    TEST_ASSERT_TRUE(ret == g_testGenRandomParams[0].expectResult);

    TestFreeBlob(&random);
    TEST_ASSERT_TRUE(ret == 0);
}

RUN_TEST_SUITE(HksGenerateRandomTest);
