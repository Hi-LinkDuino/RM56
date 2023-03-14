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
#include "v1_0/sensor_interface_proxy.h"
#include "sensor_callback_impl.h"

using namespace testing::ext;
using namespace OHOS::HDI::Sensor::V1_0;

namespace {
    const int32_t SENSOR_ID = 0;
    const int32_t SENSOR_INTERVAL = 1000000000;
    const int32_t SENSOR_POLL_TIME = 5;
    const int32_t SENSOR_USEC_TIME = 1000000;
    const int32_t SENSOR_MSEC_TIME = 1000;
    const int32_t SENSOR_COMMON_TIME = 2000;
    sptr<ISensorInterface>  g_sensorInterface = nullptr;
    sptr<ISensorCallback> g_callback = new SensorCallbackImpl();
}

class HdfSensorHdiPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfSensorHdiPerformanceTest::SetUpTestCase()
{
    g_sensorInterface = ISensorInterface::Get();
    if (g_sensorInterface == nullptr) {
        printf("test sensorHdi get Module instance failed\n\r");
    }
}

void HdfSensorHdiPerformanceTest::TearDownTestCase()
{
}

void HdfSensorHdiPerformanceTest::SetUp()
{
}

void HdfSensorHdiPerformanceTest::TearDown()
{
}

/**
  * @tc.name: SensorHdiRegister001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869N
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiRegister001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->Register(0, g_callback);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SensorHdiGetAllSensors001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869O
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiGetAllSensors001, TestSize.Level1)
{
    std::vector<HdfSensorInformation> info;
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->GetAllSensorInfo(info);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SensorHdiEnable001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869P
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiEnable001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->Enable(SENSOR_ID);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SensorHdiSetBatch001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869Q
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiSetBatch001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->SetBatch(SENSOR_ID, SENSOR_INTERVAL, SENSOR_POLL_TIME);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));

    OsalSleep(SENSOR_POLL_TIME);

    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SensorHdiDisable001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869U
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiDisable001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->Disable(SENSOR_ID);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SensorHdiSetMode001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869R
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiSetMode001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->SetMode(SENSOR_ID, 1);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SensorHdiSetOption001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869S
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiSetOption001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->SetOption(SENSOR_ID, 1);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SensorHdiRegister001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869T
  */
HWTEST_F(HdfSensorHdiPerformanceTest, SensorHdiUnregister001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorInterface->Unregister(0, g_callback);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}
