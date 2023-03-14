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

#include "hctest.h"
#include "hi_watchdog.h"
#include "hks_delete_test.h"

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
LITE_TEST_SUIT(husk, huks_lite, HksDeleteTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksDeleteTestSetUp()
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
static BOOL HksDeleteTestTearDown()
{
    LiteTestPrint("tearDown\n");
    hi_watchdog_enable();
    return TRUE;
}


static const struct HksTestKeyExistParams g_testKeyExistParams[] = {
    /* normal case */
    { 0, HKS_SUCCESS, true, { true, DEFAULT_KEY_ALIAS_SIZE, true, DEFAULT_KEY_ALIAS_SIZE } },
};

/**
 * @tc.name: HksDeleteTest.HksDeleteTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksDeleteTest, HksDeleteTest001, Level1)
{
    int32_t ret;
    struct HksBlob *keyAlias = NULL;
    if (g_testKeyExistParams[0].isGenKey) {
        HKS_TEST_ASSERT(TestGenDefaultKeyAndGetAlias(&keyAlias) == 0);
    } else {
        ret = TestConstuctBlob(&keyAlias,
                               g_testKeyExistParams[0].keyAliasParams.blobExist,
                               g_testKeyExistParams[0].keyAliasParams.blobSize,
                               g_testKeyExistParams[0].keyAliasParams.blobDataExist,
                               g_testKeyExistParams[0].keyAliasParams.blobDataSize);
        HKS_TEST_ASSERT(ret == 0);
    }

    ret = HksDeleteKeyRun(keyAlias, 1);
    HKS_TEST_ASSERT(ret == g_testKeyExistParams[0].expectResult);

    TestFreeBlob(&keyAlias);
    TEST_ASSERT_TRUE(ret == 0);
}

RUN_TEST_SUITE(HksDeleteTest);
#endif /* _CUT_AUTHENTICATE_ */

