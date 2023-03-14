/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "v1_0/vibrator_interface_proxy.h"

using namespace OHOS::HDI::Vibrator::V1_0;
using namespace testing::ext;

namespace {
    const int32_t VIBRATOR_USEC_TIME = 1000000;
    const int32_t VIBRATOR_MSEC_TIME = 1000;
    const int32_t VIBRATOR_DURATION  = 2000;
    const int32_t VIBRATOR_COMMON_TIME = 500;
    const char *g_timeSequence = "haptic.clock.timer";
    const char *g_builtIn = "haptic.default.effect";
    sptr<IVibratorInterface> g_vibratorInterface = nullptr;
}

class HdfVibratorHdiPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfVibratorHdiPerformanceTest::SetUpTestCase()
{
    g_vibratorInterface = IVibratorInterface::Get();
    if (g_vibratorInterface == nullptr) {
        printf("test vibratorHdi get Module insttace failed\n\r");
    }
}

void HdfVibratorHdiPerformanceTest::TearDownTestCase()
{
}

void HdfVibratorHdiPerformanceTest::SetUp()
{
}

void HdfVibratorHdiPerformanceTest::TearDown()
{
}

/**
  * @tc.name: VibratorStartOnce001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: #I4NN4Z
  */
HWTEST_F(HdfVibratorHdiPerformanceTest, VibratorStartOnce001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_vibratorInterface->StartOnce(VIBRATOR_DURATION);
    int endRet = g_vibratorInterface->Stop(HDF_VIBRATOR_MODE_ONCE);
    clock_gettime(CLOCK_REALTIME, &tv2);

    timeUsed = ((tv2.tv_sec * VIBRATOR_USEC_TIME + tv2.tv_nsec / VIBRATOR_MSEC_TIME) -
        (tv1.tv_sec * VIBRATOR_USEC_TIME + tv1.tv_nsec / VIBRATOR_MSEC_TIME));
    EXPECT_GT(VIBRATOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, endRet);
}

/**
  * @tc.name: VibratorHapticDefaultTime001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: #I4NN4Z
  */
HWTEST_F(HdfVibratorHdiPerformanceTest, VibratorHapticDefaultTime001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_vibratorInterface->Start(g_timeSequence);
    int endRet = g_vibratorInterface->Stop(HDF_VIBRATOR_MODE_PRESET);
    clock_gettime(CLOCK_REALTIME, &tv2);

    timeUsed = ((tv2.tv_sec * VIBRATOR_USEC_TIME + tv2.tv_nsec / VIBRATOR_MSEC_TIME) -
        (tv1.tv_sec * VIBRATOR_USEC_TIME + tv1.tv_nsec / VIBRATOR_MSEC_TIME));
    EXPECT_GT(VIBRATOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, endRet);
}

/**
  * @tc.name: VibratorHapticDefaultEffect001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: #I4NN4Z
  */
HWTEST_F(HdfVibratorHdiPerformanceTest, VibratorHapticDefaultEffect001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_vibratorInterface->Start(g_builtIn);
    int endRet = g_vibratorInterface->Stop(HDF_VIBRATOR_MODE_PRESET);
    clock_gettime(CLOCK_REALTIME, &tv2);

    timeUsed = ((tv2.tv_sec * VIBRATOR_USEC_TIME + tv2.tv_nsec / VIBRATOR_MSEC_TIME) -
        (tv1.tv_sec * VIBRATOR_USEC_TIME + tv1.tv_nsec / VIBRATOR_MSEC_TIME));
    EXPECT_GT(VIBRATOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(0, endRet);
}
