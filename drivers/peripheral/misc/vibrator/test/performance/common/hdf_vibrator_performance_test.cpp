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

#include <cmath>
#include <cstdio>
#include <gtest/gtest.h>
#include <securec.h>
#include "hdf_base.h"
#include "osal_time.h"
#include "vibrator_if.h"
#include "vibrator_type.h"

using namespace testing::ext;

namespace {
    const int32_t VIBRATOR_USEC_TIME = 1000000;
    const int32_t VIBRATOR_MSEC_TIME = 1000;
    const int32_t VIBRATOR_DURATION  = 2000;
    const int32_t VIBRATOR_COMMON_TIME = 500;
    const char *g_timeSequence = "haptic.clock.timer";
    const char *g_builtIn = "haptic.default.effect";
    const struct VibratorInterface *g_vibratorPerformanceDev = nullptr;
    }

class HdfVibratorPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfVibratorPerformanceTest::SetUpTestCase()
{
    g_vibratorPerformanceDev = NewVibratorInterfaceInstance();
    if(g_vibratorPerformanceDev == nullptr) {
        printf("test vibratorHdi get Module insttace failed\n\r");
    }
}

void HdfVibratorPerformanceTest::TearDownTestCase()
{
    if(g_vibratorPerformanceDev != nullptr){
        FreeVibratorInterfaceInstance();
        g_vibratorPerformanceDev = nullptr;
    }
}

void HdfVibratorPerformanceTest::SetUp()
{
}

void HdfVibratorPerformanceTest::TearDown()
{
}

/**
  *@tc.name: VibratorStartOnce001
  *@tc.desc: Interface performance test.
  *@tc.type: FUNC
  *@tc.require: AR000FK1J0,AR000FK1JP
  */
HWTEST_F(HdfVibratorPerformanceTest, VibratorStartOnce001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_vibratorPerformanceDev->StartOnce(VIBRATOR_DURATION);
    int endRet = g_vibratorPerformanceDev->Stop(VIBRATOR_MODE_ONCE);
    clock_gettime(CLOCK_REALTIME, &tv2);
    
    timeUsed = ((tv2.tv_sec * VIBRATOR_USEC_TIME + tv2.tv_nsec / VIBRATOR_MSEC_TIME) - 
        (tv1.tv_sec * VIBRATOR_USEC_TIME + tv1.tv_nsec / VIBRATOR_MSEC_TIME));
    EXPECT_GT(VIBRATOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, endRet);
}

/**
  *@tc.name: VibratorHapticDefaultTime001
  *@tc.desc: Interface performance test.
  *@tc.type: FUNC
  *@tc.require: AR000FK1JN,AR000FK1JP
  */
HWTEST_F(HdfVibratorPerformanceTest, VibratorHapticDefaultTime001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_vibratorPerformanceDev->Start(g_timeSequence);
    int endRet = g_vibratorPerformanceDev->Stop(VIBRATOR_MODE_PRESET);
    clock_gettime(CLOCK_REALTIME, &tv2);
    
    timeUsed = ((tv2.tv_sec * VIBRATOR_USEC_TIME + tv2.tv_nsec / VIBRATOR_MSEC_TIME) - 
        (tv1.tv_sec * VIBRATOR_USEC_TIME + tv1.tv_nsec / VIBRATOR_MSEC_TIME));
    EXPECT_GT(VIBRATOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, endRet);
}

/**
  *@tc.name: VibratorHapticDefaultEffect001
  *@tc.desc: Interface performance test.
  *@tc.type: FUNC
  *@tc.require: AR000FK1J0,AR000FK1JP
  */
HWTEST_F(HdfVibratorPerformanceTest, VibratorHapticDefaultEffect001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_vibratorPerformanceDev->Start(g_builtIn);
    int endRet = g_vibratorPerformanceDev->Stop(VIBRATOR_MODE_PRESET);
    clock_gettime(CLOCK_REALTIME, &tv2);
    
    timeUsed = ((tv2.tv_sec * VIBRATOR_USEC_TIME + tv2.tv_nsec / VIBRATOR_MSEC_TIME) - 
        (tv1.tv_sec * VIBRATOR_USEC_TIME + tv1.tv_nsec / VIBRATOR_MSEC_TIME));
    EXPECT_GT(VIBRATOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, endRet);
}
