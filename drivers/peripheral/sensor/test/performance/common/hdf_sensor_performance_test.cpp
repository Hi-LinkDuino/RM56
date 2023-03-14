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
#include "sensor_if.h"
#include "sensor_type.h"

using namespace testing::ext;

namespace {
    int32_t g_sensorDataFlag = 0;
    const int32_t SENSOR_ID = 0;
    const int32_t SENSOR_INTERVAL = 1000000000;
    const int32_t SENSOR_POLL_TIME = 5;
    const int32_t SENSOR_USEC_TIME = 1000000;
    const int32_t SENSOR_MSEC_TIME = 1000;
    const int32_t SENSOR_COMMON_TIME = 2000;
    const struct SensorInterface *g_sensorPerformanceDev = nullptr;

    int SensorTestDataCallback(const struct SensorEvents *event)
    {
        if (event == nullptr || event->data == nullptr) {
            return -1;
        }
        float *data = (float*)event->data;

        if (event->sensorId == SENSOR_ID) {
            printf("time [%lld] sensor id [%d] data [%f]\n\r", event->timestamp, event->sensorId, *(data));
            if (fabs(*data) > 1e-5) {
                g_sensorDataFlag = 1;
            }
        }

        return 0;
    }
}

class HdfSensorPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfSensorPerformanceTest::SetUpTestCase()
{
    g_sensorPerformanceDev = NewSensorInterfaceInstance();
    if (g_sensorPerformanceDev == nullptr) {
        printf("test sensorHdi get Module instance failed\n\r");
    }
}

void HdfSensorPerformanceTest::TearDownTestCase()
{
    if (g_sensorPerformanceDev != nullptr) {
        FreeSensorInterfaceInstance();
        g_sensorPerformanceDev = nullptr;
    }
}

void HdfSensorPerformanceTest::SetUp()
{
}

void HdfSensorPerformanceTest::TearDown()
{
}

/**
  * @tc.name: SensorHdiRegister001
  * @tc.desc: Interface performance test.
  * @tc.type: FUNC
  * @tc.require: AR000F869N
  */
HWTEST_F(HdfSensorPerformanceTest, SensorHdiRegister001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->Register(0, SensorTestDataCallback);
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
HWTEST_F(HdfSensorPerformanceTest, SensorHdiGetAllSensors001, TestSize.Level1)
{
    int32_t count = 0;
    struct SensorInformation *sensorInfo = nullptr;
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->GetAllSensors(&sensorInfo, &count);
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
HWTEST_F(HdfSensorPerformanceTest, SensorHdiEnable001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->Enable(SENSOR_ID);
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
HWTEST_F(HdfSensorPerformanceTest, SensorHdiSetBatch001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->SetBatch(SENSOR_ID, SENSOR_INTERVAL, SENSOR_POLL_TIME);
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
HWTEST_F(HdfSensorPerformanceTest, SensorHdiDisable001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->Disable(SENSOR_ID);
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
HWTEST_F(HdfSensorPerformanceTest, SensorHdiSetMode001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->SetMode(SENSOR_ID, 1);
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
HWTEST_F(HdfSensorPerformanceTest, SensorHdiSetOption001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->SetOption(SENSOR_ID, 1);
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
HWTEST_F(HdfSensorPerformanceTest, SensorHdiUnregister001, TestSize.Level1)
{
    int timeUsed = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};

    clock_gettime(CLOCK_REALTIME, &tv1);
    int ret = g_sensorPerformanceDev->Unregister(0, SensorTestDataCallback);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * SENSOR_USEC_TIME + tv2.tv_nsec / SENSOR_MSEC_TIME) -
        (tv1.tv_sec * SENSOR_USEC_TIME + tv1.tv_nsec / SENSOR_MSEC_TIME));
    EXPECT_GE(SENSOR_COMMON_TIME, timeUsed);
    EXPECT_EQ(0, ret);
}
