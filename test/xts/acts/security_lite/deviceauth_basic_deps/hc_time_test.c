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

#include "hc_time_test.h"

// math.h must be included before hctest.h -> unity.h -> unity_internals.h
// otherwise isinf and isnan macro will be redefined
#include <math.h>

#include <hctest.h>
#include <securec.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SLEEP_SECONDS_MIN = 3,
    SLEEP_SECONDS_MAX = 5,
    TEST_TIMES = 5,
};

#define MILLISECONDS_DEVIATION 1.e2

static void TestTime(void)
{
    struct timespec start, end;
    int res, sleepSeconds = rand() % (SLEEP_SECONDS_MAX - SLEEP_SECONDS_MIN) + SLEEP_SECONDS_MIN;
    res = memset_s(&start, sizeof(start), 0, sizeof(start));
    TEST_ASSERT_EQUAL(0, res);
    res = memset_s(&end, sizeof(end), 0, sizeof(end));
    TEST_ASSERT_EQUAL(0, res);

    LOGI("test correctness");

    RUN_AND_PRINT_ELAPSED_TIME(res, clock_gettime(CLOCK_MONOTONIC, &start));
    TEST_ASSERT_EQUAL(0, res);

    LOGI("begin sleeping for %d seconds", sleepSeconds);
    sleep(sleepSeconds);

    RUN_AND_PRINT_ELAPSED_TIME(res, clock_gettime(CLOCK_MONOTONIC, &end));
    TEST_ASSERT_EQUAL(0, res);

    TEST_ASSERT_LESS_THAN(MILLISECONDS_DEVIATION, fabs(CalcTimeSpecDiff(start, end) - sleepSeconds * 1000.0f));

    LOGI("test precision");

    res = memset_s(&start, sizeof(start), 0, sizeof(start));
    TEST_ASSERT_EQUAL(0, res);
    res = memset_s(&end, sizeof(end), 0, sizeof(end));
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, clock_gettime(CLOCK_MONOTONIC, &start));
    TEST_ASSERT_EQUAL(0, res);

    LOGI("in testing precision");

    RUN_AND_PRINT_ELAPSED_TIME(res, clock_gettime(CLOCK_MONOTONIC, &end));
    TEST_ASSERT_EQUAL(0, res);

    TEST_ASSERT_LESS_THAN(MILLISECONDS_DEVIATION, fabs(CalcTimeSpecDiff(start, end)));
}

static void TestPrintTime(void)
{
    for (int i = 0; i < TEST_TIMES; ++i) {
        struct timespec start;
        int res, sleepSeconds;
        res = memset_s(&start, sizeof(start), 0, sizeof(start));
        TEST_ASSERT_EQUAL(0, res);

        sleepSeconds = rand() % (SLEEP_SECONDS_MAX - SLEEP_SECONDS_MIN) + SLEEP_SECONDS_MIN;
        res = clock_gettime(CLOCK_MONOTONIC, &start);
        TEST_ASSERT_EQUAL(0, res);

        double rawTime = (start.tv_nsec / 1000.0 / 1000.0 / 1000.0) + start.tv_sec;
        time_t timeTime = (time_t)rawTime;
        struct tm *pts = gmtime(&timeTime);
        TEST_ASSERT_NOT_NULL(pts);
        LOGI("time = %d-%d-%d %d:%d:%d, %d day of week, %d days in year, isdst = %d",
            pts->tm_year,
            pts->tm_mon + 1,
            pts->tm_mday,
            pts->tm_hour,
            pts->tm_min,
            pts->tm_sec,
            pts->tm_wday + 1,
            pts->tm_yday + 1,
            pts->tm_isdst);
        LOGI("begin sleeping for %d seconds", sleepSeconds);
        sleep(sleepSeconds);
    }
}

void TestHcTime(void)
{
    for (int i = 0; i < TEST_TIMES; ++i) {
        TestTime();
    }

    TestPrintTime();
}

#ifdef __cplusplus
}
#endif
