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

#include "hks_hash_test.h"

#include <hctest.h>
#include "hi_watchdog.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_api_performance.h"
#include "hks_test_common.h"
#include "hks_test_log.h"
#include "hks_type.h"


#define DEFAULT_SRC_DATA_SIZE 200

#define DIGEST_SHA256_HASH_SIZE 32

/*
 * @tc.register: register a test suit named "CalcMultiTest"
 * @param: test subsystem name
 * @param: c_example module name
 * @param: CalcMultiTest test suit name
 */
LITE_TEST_SUIT(husk, huks_lite, HksHashTest);

/**
 * @tc.setup: define a setup for test suit, format:"CalcMultiTest + SetUp"
 * @return: true——setup success
 */
static BOOL HksHashTestSetUp()
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
static BOOL HksHashTestTearDown()
{
    LiteTestPrint("tearDown\n");
    hi_watchdog_enable();
    return TRUE;
}


static const struct HksTestHashParams g_testHashParams[] = {
    /* normal case */
    { 0, HKS_SUCCESS,
        { true, true, HKS_DIGEST_SHA256},
        { true, DEFAULT_SRC_DATA_SIZE, true, DEFAULT_SRC_DATA_SIZE },
        { true, DIGEST_SHA256_HASH_SIZE, true, DIGEST_SHA256_HASH_SIZE }
    },
};

/**
 * @tc.name: HksHashTest.HksHashTest001
 * @tc.desc: The static function will return true;
 * @tc.type: FUNC
 */
LITE_TEST_CASE(HksHashTest, HksHashTest001, Level1)
{
    struct HksParamSet *paramSet = NULL;
    struct HksBlob *srcData = NULL;
    struct HksBlob *hash = NULL;

    int32_t ret = TestConstructHashParamSet(&paramSet,
        g_testHashParams[0].paramSetParams.paramSetExist,
        g_testHashParams[0].paramSetParams.setDigest, g_testHashParams[0].paramSetParams.digest);
    TEST_ASSERT_TRUE(ret == 0);

    ret = TestConstuctBlob(&srcData,
        g_testHashParams[0].srcDataParams.blobExist,
        g_testHashParams[0].srcDataParams.blobSize,
        g_testHashParams[0].srcDataParams.blobDataExist,
        g_testHashParams[0].srcDataParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = TestConstructBlobOut(&hash,
        g_testHashParams[0].hashParams.blobExist,
        g_testHashParams[0].hashParams.blobSize,
        g_testHashParams[0].hashParams.blobDataExist,
        g_testHashParams[0].hashParams.blobDataSize);
    TEST_ASSERT_TRUE(ret == 0);

    ret = HksHashRun(paramSet, srcData, hash, 1);
    if (ret != g_testHashParams[0].expectResult) {
        HKS_TEST_LOG_I("HksHashRun failed, ret[%u] = %d", g_testHashParams[0].testId, ret);
    }
    TEST_ASSERT_TRUE(ret == g_testHashParams[0].expectResult);

    HksFreeParamSet(&paramSet);
    TestFreeBlob(&srcData);
    TestFreeBlob(&hash);
    HKS_TEST_LOG_I("[%u]TestHash, Testcase_Hash_[%03u] pass!", 1, g_testHashParams[0].testId);
    TEST_ASSERT_TRUE(ret == 0);
}
RUN_TEST_SUITE(HksHashTest);

#endif /* _CUT_AUTHENTICATE_ */

