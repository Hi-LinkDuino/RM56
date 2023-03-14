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
#include "v1_0/light_interface_proxy.h"

using namespace OHOS::HDI::Light::V1_0;
using namespace testing::ext;

namespace {
    constexpr int32_t LIGHT_COMMON_TIME = 500;
    constexpr int32_t g_minLightId = HDF_LIGHT_TYPE_BATTERY;
    constexpr int32_t g_maxLightId = HDF_LIGHT_TYPE_ATTENTION;
    constexpr int32_t LIGHT_USEC_TIME = 1000000;
    constexpr int32_t LIGHT_MSEC_TIME = 1000;
    constexpr int32_t g_onTime = 500;
    constexpr int32_t g_offTime = 500;
    sptr<ILightInterface> g_lightInterface = nullptr;
}

class HdfLightHdiPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLightHdiPerformanceTest::SetUpTestCase()
{
    g_lightInterface = ILightInterface::Get();
    if (g_lightInterface == nullptr) {
        printf("test lightHdi get Module insttace failed\n\r");
    }
}

void HdfLightHdiPerformanceTest::TearDownTestCase()
{
}

void HdfLightHdiPerformanceTest::SetUp()
{
}

void HdfLightHdiPerformanceTest::TearDown()
{
}

/**
  * @tc.name: GetLightInfo
  * @tc.desc:
  * @tc.type: FUNC
  * @tc.require: #I4NN4Z
  */
HWTEST_F(HdfLightHdiPerformanceTest, GetLightInfo001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec) {0};
    struct timespec tv2 = (struct timespec) {0};

    std::vector<HdfLightInfo> info;
    clock_gettime(CLOCK_REALTIME, &tv1);
    int32_t ret = g_lightInterface->GetLightInfo(info);
    clock_gettime(CLOCK_REALTIME, &tv2);

    timeUsed = ((tv2.tv_sec * LIGHT_USEC_TIME + tv2.tv_nsec / LIGHT_MSEC_TIME) -
        (tv1.tv_sec * LIGHT_USEC_TIME + tv1.tv_nsec / LIGHT_MSEC_TIME));
    EXPECT_GT(LIGHT_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: TurnOnLight001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: #I4NN4Z
  */
HWTEST_F(HdfLightHdiPerformanceTest, TurnOnLight001, TestSize.Level1)
{
    std::vector<HdfLightInfo> info;
    int32_t ret = g_lightInterface->GetLightInfo(info);
    EXPECT_EQ(0, ret);
    printf("get light list num[%zu]\n\r", info.size());

    for (auto iter : info)
    {
        EXPECT_GE(iter.lightId, g_minLightId);
        EXPECT_LE(iter.lightId, g_maxLightId);

        int timeUsed = 0;
        struct timespec tv1 = (struct timespec) {0};
        struct timespec tv2 = (struct timespec) {0};
        HdfLightEffect effect;
        effect.flashEffect.flashMode = HDF_LIGHT_FLASH_NONE;
        clock_gettime(CLOCK_REALTIME, &tv1);
        int32_t ret = g_lightInterface->TurnOnLight(iter.lightId, effect);
        clock_gettime(CLOCK_REALTIME, &tv2);

        timeUsed = ((tv2.tv_sec * LIGHT_USEC_TIME + tv2.tv_nsec / LIGHT_MSEC_TIME) -
            (tv1.tv_sec * LIGHT_USEC_TIME + tv1.tv_nsec / LIGHT_MSEC_TIME));
        EXPECT_GT(LIGHT_COMMON_TIME, timeUsed);
        EXPECT_EQ(ret, HDF_SUCCESS);

        clock_gettime(CLOCK_REALTIME, &tv1);
        ret = g_lightInterface->TurnOffLight(iter.lightId);
        clock_gettime(CLOCK_REALTIME, &tv2);
        timeUsed = ((tv2.tv_sec * LIGHT_USEC_TIME + tv2.tv_nsec / LIGHT_MSEC_TIME) -
            (tv1.tv_sec * LIGHT_USEC_TIME + tv1.tv_nsec / LIGHT_MSEC_TIME));
        EXPECT_GT(LIGHT_COMMON_TIME, timeUsed);
        EXPECT_EQ(ret, HDF_SUCCESS);
    }
}

/**
  * @tc.name: TurnOnLight002
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: #I4NN4Z
  */
HWTEST_F(HdfLightHdiPerformanceTest, TurnOnLight002, TestSize.Level1)
{
    std::vector<HdfLightInfo> info;
    int32_t ret = g_lightInterface->GetLightInfo(info);
    EXPECT_EQ(0, ret);
    printf("get light list num[%zu]\n\r", info.size());

    for (auto iter : info)
    {
        EXPECT_GE(iter.lightId, g_minLightId);
        EXPECT_LE(iter.lightId, g_maxLightId);

        int timeUsed = 0;
        struct timespec tv1 = (struct timespec) {0};
        struct timespec tv2 = (struct timespec) {0};
        HdfLightEffect effect;
        effect.lightBrightness = 0xFFFF0000;
        effect.flashEffect.flashMode = HDF_LIGHT_FLASH_TIMED;
        effect.flashEffect.onTime = g_onTime;
        effect.flashEffect.offTime = g_offTime;
        clock_gettime(CLOCK_REALTIME, &tv1);
        int32_t ret = g_lightInterface->TurnOnLight(iter.lightId, effect);
        clock_gettime(CLOCK_REALTIME, &tv2);

        timeUsed = ((tv2.tv_sec * LIGHT_USEC_TIME + tv2.tv_nsec / LIGHT_MSEC_TIME) -
            (tv1.tv_sec * LIGHT_USEC_TIME + tv1.tv_nsec / LIGHT_MSEC_TIME));
        EXPECT_GT(LIGHT_COMMON_TIME, timeUsed);
        EXPECT_EQ(ret, HDF_SUCCESS);

        clock_gettime(CLOCK_REALTIME, &tv1);
        ret = g_lightInterface->TurnOffLight(iter.lightId);
        clock_gettime(CLOCK_REALTIME, &tv2);
        timeUsed = ((tv2.tv_sec * LIGHT_USEC_TIME + tv2.tv_nsec / LIGHT_MSEC_TIME) -
            (tv1.tv_sec * LIGHT_USEC_TIME + tv1.tv_nsec / LIGHT_MSEC_TIME));
        EXPECT_GT(LIGHT_COMMON_TIME, timeUsed);
        EXPECT_EQ(ret, HDF_SUCCESS);
    }
}
