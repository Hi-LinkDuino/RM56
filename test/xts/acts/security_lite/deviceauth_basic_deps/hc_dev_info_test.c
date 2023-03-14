/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hc_dev_info_test.h"

#include <hctest.h>
#include <parameter.h>
#include <pthread.h>
#include <securec.h>
#include <stdlib.h>

#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INPUT_UDID_LEN 200

enum {
    INPUT_UDID_LEN = 65,
    TEST_UDID_TIMES = 20,
};

static void TestGetDevUdid(char *udid, int len)
{
    int res;
    int zeroCount = 0;
    res = memset_s(udid, MAX_INPUT_UDID_LEN, 0, MAX_INPUT_UDID_LEN);
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, GetDevUdid(udid, len));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < len; ++i) {
        if (udid[i] == 0) {
            ++zeroCount;
        }
    }
    TEST_ASSERT_NOT_EQUAL_MESSAGE(len, zeroCount, "invalid all zero udid");
}

void TestHcGetUdid(void)
{
    int res;
    char udid[MAX_INPUT_UDID_LEN] = {0};
    int udidLen;
    LOGI("check rand size udid");
    for (int i = 0; i < TEST_UDID_TIMES; ++i) {
        udidLen = rand() % (MAX_INPUT_UDID_LEN - INPUT_UDID_LEN) + INPUT_UDID_LEN;
        LOGI("rand udidLen = %d", udidLen);
        res = memset_s(udid, sizeof(udid), 0, sizeof(udid));
        TEST_ASSERT_EQUAL(0, res);
        TestGetDevUdid(udid, udidLen);
        LOGI("udid = \"%s\"", udid);
    }

    LOGI("check min size udid");
    res = memset_s(udid, sizeof(udid), 0, sizeof(udid));
    TEST_ASSERT_EQUAL(0, res);
    TestGetDevUdid(udid, INPUT_UDID_LEN);
    LOGI("udid = \"%s\"", udid);

    LOGI("check max size udid");
    res = memset_s(udid, sizeof(udid), 0, sizeof(udid));
    TEST_ASSERT_EQUAL(0, res);
    TestGetDevUdid(udid, MAX_INPUT_UDID_LEN);
    LOGI("udid = \"%s\"", udid);
}

#ifdef __cplusplus
}
#endif
