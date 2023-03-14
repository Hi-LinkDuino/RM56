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
#include <unistd.h>
#include <gtest/gtest.h>
#include <securec.h>
#include "hdf_base.h"
#include "osal_time.h"
#include "sensor_if.h"
#include "sensor_type.h"

using namespace testing::ext;

namespace {
    struct SensorValueRange {
        float highThreshold;
        float lowThreshold;
    };

    struct SensorDevelopmentList {
        int32_t sensorTypeId;
        char sensorName[SENSOR_NAME_MAX_LEN];
        int32_t dataForm;    // 0: fixed, 1: range
        int32_t dataDimension;
        struct SensorValueRange *valueRange;
    };

    struct SensorValueRange g_testRange[] = {{1e5, 0.0}};
    struct SensorValueRange g_accelRange[] = {{78.0, -78.0}, {78.0, -78.0}, {78.0, -78.0}};
    struct SensorValueRange g_alsRange[] = {{10000.0, 0.0}};
    struct SensorValueRange g_pedometerRange[] = {{10000.0, 0.0}};
    struct SensorValueRange g_proximityRange[] = {{5.0, 0.0}};
    struct SensorValueRange g_hallRange[] = {{1.0, 0.0}};
    struct SensorValueRange g_barometerRange[] = {{1100.0, -1100.0}, {1100.0, -1100.0}};
    struct SensorValueRange g_magneticRange[] = {{2000.0, -2000.0}, {2000.0, -2000.0}, {2000.0, -2000.0}};
    struct SensorValueRange g_gyroscopeRange[] = {{35.0, -35.0}, {35.0, -35.0}, {35.0, -35.0}};
    struct SensorValueRange g_gravityRange[] = {{78.0, -78.0}, {78.0, -78.0}, {78.0, -78.0}};

    struct SensorDevelopmentList g_sensorList[] = {
        {SENSOR_TYPE_NONE, "sensor_test", 1, 1, g_testRange},
        {SENSOR_TYPE_ACCELEROMETER, "accelerometer", 1, 3, g_accelRange},
        {SENSOR_TYPE_PEDOMETER, "pedometer", 1, 1, g_pedometerRange},
        {SENSOR_TYPE_PROXIMITY, "proximity", 0, 1, g_proximityRange},
        {SENSOR_TYPE_HALL, "hallrometer", 0, 1, g_hallRange},
        {SENSOR_TYPE_BAROMETER, "barometer", 1, 2, g_barometerRange},
        {SENSOR_TYPE_AMBIENT_LIGHT, "als", 1, 1, g_alsRange},
        {SENSOR_TYPE_MAGNETIC_FIELD, "magnetometer", 1, 3, g_magneticRange},
        {SENSOR_TYPE_GYROSCOPE, "gyroscope", 1, 3, g_gyroscopeRange},
        {SENSOR_TYPE_GRAVITY, "gravity", 1, 3, g_gravityRange}
    };

    constexpr int g_listNum = sizeof(g_sensorList) / sizeof(g_sensorList[0]);
    uint32_t g_sensorDataFlag = 1;
    constexpr int32_t SENSOR_INTERVAL1 = 200000000;
    constexpr int32_t SENSOR_INTERVAL2 = 20000000;
    constexpr int32_t SENSOR_POLL_TIME = 1;
    constexpr int32_t SENSOR_WAIT_TIME = 100;
    constexpr float EPSINON = 1e-6;
    constexpr int32_t ABNORMAL_SENSORID = -1;
    const struct SensorInterface *g_sensorDev = nullptr;
    int32_t g_count = 0;
    struct SensorInformation *g_sensorInfo = nullptr;

    void SensorDataVerification(const float &data, const struct SensorDevelopmentList &sensorNode)
    {
        for (int32_t j = 0; j < sensorNode.dataDimension; ++j) {
            printf("sensor id :[%d], data[%d]: %f\n\r", sensorNode.sensorTypeId, j + 1, *(&data + j));
            if (sensorNode.dataForm == 0) {
                if (abs(*(&data + j) - sensorNode.valueRange[j].highThreshold) < EPSINON ||
                    abs(*(&data + j) - sensorNode.valueRange[j].lowThreshold) < EPSINON) {
                    g_sensorDataFlag &= 1;
                } else {
                    g_sensorDataFlag = 0;
                    printf("%s: %s Not expected\n\r", __func__, sensorNode.sensorName);
                }
            }

            if (sensorNode.dataForm == 1) {
                if (*(&data + j) > sensorNode.valueRange[j].lowThreshold &&
                    *(&data + j) < sensorNode.valueRange[j].highThreshold) {
                    g_sensorDataFlag &= 1;
                } else {
                    g_sensorDataFlag = 0;
                    printf("%s: %s Not expected\n\r", __func__, sensorNode.sensorName);
                }
            }
        }
    }

    int32_t TraditionalSensorTestDataCallback(const struct SensorEvents *event)
    {
        if (event == nullptr || event->data == nullptr) {
            return SENSOR_FAILURE;
        }

        for (int32_t i = 0; i < g_listNum; ++i) {
            if (event->sensorId == g_sensorList[i].sensorTypeId) {
                float *data = (float*)event->data;
                SensorDataVerification(*data, g_sensorList[i]);
            }
        }
        return SENSOR_SUCCESS;
    }

    int32_t MedicalSensorTestDataCallback(const struct SensorEvents *event)
    {
        (void)event;

        return SENSOR_SUCCESS;
    }
}

class HdfSensorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfSensorTest::SetUpTestCase()
{
    g_sensorDev = NewSensorInterfaceInstance();
    if (g_sensorDev == nullptr) {
        printf("test sensorHdi get Module instance failed\n\r");
        return;
    }
    int32_t ret = g_sensorDev->GetAllSensors(&g_sensorInfo, &g_count);
    if (ret == -1) {
        printf("get sensor information failed\n\r");
    }
}

void HdfSensorTest::TearDownTestCase()
{
    if (g_sensorDev != nullptr) {
        FreeSensorInterfaceInstance();
        g_sensorDev = nullptr;
    }
}

void HdfSensorTest::SetUp()
{
}

void HdfSensorTest::TearDown()
{
}

/**
  * @tc.name: GetSensorInstance001
  * @tc.desc: Create a sensor instance and check whether the instance is empty.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869N, AR000F8QNL
  */
HWTEST_F(HdfSensorTest, GetSensorInstance001, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        printf("test sensorHdi get Module instance failed\n\r");
        return;
    }

    const struct SensorInterface *sensorDev = NewSensorInterfaceInstance();
    EXPECT_EQ(sensorDev, g_sensorDev);
}

/**
  * @tc.name: RemoveSensorInstance001
  * @tc.desc: The sensor instance is successfully removed.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869O, AR000F8QNL
  */
HWTEST_F(HdfSensorTest, RemoveSensorInstance001, TestSize.Level1)
{
    int32_t ret = FreeSensorInterfaceInstance();
    ASSERT_EQ(SENSOR_SUCCESS, ret);
    ret = FreeSensorInterfaceInstance();
    EXPECT_EQ(SENSOR_SUCCESS, ret);
    g_sensorDev = NewSensorInterfaceInstance();
    if (g_sensorDev == nullptr) {
        ASSERT_EQ(SENSOR_SUCCESS, ret);
        return;
    }
    ret = g_sensorDev->GetAllSensors(&g_sensorInfo, &g_count);
    EXPECT_EQ(SENSOR_SUCCESS, ret);
}

/**
  * @tc.name: RegisterSensorDataCb001
  * @tc.desc: Returns 0 if the callback is successfully registered; returns a negative value otherwise.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869P, AR000F8QNL
  */
HWTEST_F(HdfSensorTest, RegisterSensorDataCb001, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->Register(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);
    ret = g_sensorDev->Unregister(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);
}

/**
  * @tc.name: RegisterSensorDataCb002
  * @tc.desc: Returns 0 if the callback is successfully registered; returns a negative value otherwise.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869P
  */
HWTEST_F(HdfSensorTest, RegisterSensorDataCb002, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->Register(TRADITIONAL_SENSOR_TYPE, nullptr);
    EXPECT_EQ(SENSOR_NULL_PTR, ret);
    ret = g_sensorDev->Unregister(0, nullptr);
    EXPECT_EQ(SENSOR_NULL_PTR, ret);
}

/**
  * @tc.name: RegisterSensorDataCb003
  * @tc.desc: Returns 0 if the callback is successfully registered; returns a negative value otherwise.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869P, AR000F8QNL
  */
HWTEST_F(HdfSensorTest, RegisterSensorDataCb003, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->Register(MEDICAL_SENSOR_TYPE, MedicalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);
    ret = g_sensorDev->Unregister(MEDICAL_SENSOR_TYPE, MedicalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);
}

/**
  * @tc.name: RegisterSensorDataCb004
  * @tc.desc: Returns 0 if the callback is successfully registered; returns a negative value otherwise.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869P
  */
HWTEST_F(HdfSensorTest, RegisterSensorDataCb004, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->Register(MEDICAL_SENSOR_TYPE, nullptr);
    EXPECT_EQ(SENSOR_NULL_PTR, ret);
    ret = g_sensorDev->Unregister(MEDICAL_SENSOR_TYPE, nullptr);
    EXPECT_EQ(SENSOR_NULL_PTR, ret);
}

/**
  * @tc.name: RegisterSensorDataCb005
  * @tc.desc: Returns 0 if the callback is successfully registered; returns a negative value otherwise.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869P
  */
HWTEST_F(HdfSensorTest, RegisterSensorDataCb005, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->Register(SENSOR_TYPE_MAX, nullptr);
    EXPECT_EQ(SENSOR_INVALID_PARAM, ret);
    ret = g_sensorDev->Unregister(SENSOR_TYPE_MAX, nullptr);
    EXPECT_EQ(SENSOR_INVALID_PARAM, ret);
}

/**
  * @tc.name: GetSensorList001
  * @tc.desc: Obtains information about all sensors in the system. Validity check of input parameters.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869Q
  */
HWTEST_F(HdfSensorTest, GetSensorList001, TestSize.Level1)
{
    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }
    EXPECT_GT(g_count, 0);
}

/**
  * @tc.name: GetSensorList002
  * @tc.desc: Obtains information about all sensors in the system. Validity check of input parameters.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869Q
  */
HWTEST_F(HdfSensorTest, GetSensorList002, TestSize.Level1)
{
    struct SensorInformation *info = nullptr;
    int j;

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }
    EXPECT_GT(g_count, 0);

    printf("get sensor list num[%d]\n\r", g_count);
    info = g_sensorInfo;

    for (int32_t i = 0; i < g_count; ++i) {
        printf("get sensoriId[%d], info name[%s], power[%f]\n\r", info->sensorId, info->sensorName, info->power);
        for (j = 0; j < g_listNum; ++j) {
            if (info->sensorId == g_sensorList[j].sensorTypeId) {
                EXPECT_STRNE("", info->sensorName);
                break;
            }
        }
        info++;
    }
}

/**
  * @tc.name: GetSensorList003
  * @tc.desc: Obtains information about all sensors in the system. The operations include obtaining sensor information,
  * subscribing to or unsubscribing from sensor data, enabling or disabling a sensor,
  * setting the sensor data reporting mode, and setting sensor options such as the accuracy and measurement range.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869Q
  */
HWTEST_F(HdfSensorTest, GetSensorList003, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->GetAllSensors(nullptr, &g_count);
    EXPECT_EQ(SENSOR_NULL_PTR, ret);
    ret = g_sensorDev->GetAllSensors(&g_sensorInfo, nullptr);
    EXPECT_EQ(SENSOR_NULL_PTR, ret);
    ret = g_sensorDev->GetAllSensors(nullptr, nullptr);
    EXPECT_EQ(SENSOR_NULL_PTR, ret);
}

/**
  * @tc.name: EnableSensor001
  * @tc.desc: Enables the sensor unavailable in the sensor list based on the specified sensor ID.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869R, AR000F8QNL
  */
HWTEST_F(HdfSensorTest, EnableSensor001, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->Register(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);

    struct SensorInformation *info = nullptr;

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }

    info = g_sensorInfo;
    for (int32_t i = 0; i < g_count; i++) {
        ret = g_sensorDev->SetBatch(info->sensorId, SENSOR_INTERVAL1, SENSOR_POLL_TIME);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        ret = g_sensorDev->Enable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        OsalSleep(SENSOR_POLL_TIME);
        ret = g_sensorDev->Disable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        info++;
    }

    ret = g_sensorDev->Unregister(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);

    EXPECT_EQ(g_sensorDataFlag, 1);
    g_sensorDataFlag = 1;
}

/**
  * @tc.name: EnableSensor002
  * @tc.desc: Enables the sensor available in the sensor list based on the specified sensor ID.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869R
  */
HWTEST_F(HdfSensorTest, EnableSensor002, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }

    int32_t ret = g_sensorDev->Enable(ABNORMAL_SENSORID);
    EXPECT_EQ(SENSOR_NOT_SUPPORT, ret);
    ret = g_sensorDev->Disable(ABNORMAL_SENSORID);
    EXPECT_EQ(SENSOR_NOT_SUPPORT, ret);
}

/**
  * @tc.name: SetSensorBatch001
  * @tc.desc: Sets the sampling time and data report interval for sensors in batches.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869T
  */
HWTEST_F(HdfSensorTest, SetSensorBatch001, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    struct SensorInformation *info = nullptr;

    int32_t ret = g_sensorDev->Register(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }

    info = g_sensorInfo;
    for (int32_t i = 0; i < g_count; i++) {
        ret = g_sensorDev->SetBatch(info->sensorId, SENSOR_INTERVAL2, SENSOR_POLL_TIME);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        ret = g_sensorDev->Enable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        OsalMSleep(SENSOR_WAIT_TIME);
        ret = g_sensorDev->Disable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        info++;
    }

    ret = g_sensorDev->Unregister(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);
    EXPECT_EQ(g_sensorDataFlag, 1);
    g_sensorDataFlag = 1;
}

/** @tc.name: SetSensorBatch002
    @tc.desc: Sets the sampling time and data report interval for sensors in batches.
    @tc.type: FUNC
    @tc.requrire: SR000F869M, AR000F869U
    */
HWTEST_F(HdfSensorTest, SetSensorBatch002, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }
    int32_t ret = g_sensorDev->SetBatch(ABNORMAL_SENSORID, 0, 0);
    EXPECT_EQ(SENSOR_NOT_SUPPORT, ret);
}

/** @tc.name: SetSensorBatch003
    @tc.desc: Sets the sampling time and data report interval for sensors in batches.
    @tc.type: FUNC
    @tc.requrire: SR000F869M, AR000F869U
    */
HWTEST_F(HdfSensorTest, SetSensorBatch003, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    struct SensorInformation *info = nullptr;

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }

    info = g_sensorInfo;
    for (int32_t i = 0; i < g_count; i++) {
        int32_t ret = g_sensorDev->SetBatch(info->sensorId, -1, SENSOR_POLL_TIME);
        EXPECT_EQ(SENSOR_INVALID_PARAM, ret);
        info++;
    }
}
/**
  * @tc.name: SetSensorMode001
  * @tc.desc: Sets the data reporting mode for the specified sensor.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869U, AR000F8QNL
  */
HWTEST_F(HdfSensorTest, SetSensorMode001, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    struct SensorInformation *info = nullptr;

    int32_t ret = g_sensorDev->Register(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }

    info = g_sensorInfo;
    for (int32_t i = 0; i < g_count; i++) {
        ret = g_sensorDev->SetBatch(info->sensorId, SENSOR_INTERVAL1, SENSOR_POLL_TIME);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        if (info->sensorId == SENSOR_TYPE_HALL) {
            ret = g_sensorDev->SetMode(info->sensorId, SENSOR_MODE_ON_CHANGE);
            EXPECT_EQ(SENSOR_SUCCESS, ret);
        } else {
            ret = g_sensorDev->SetMode(info->sensorId, SENSOR_MODE_REALTIME);
            EXPECT_EQ(SENSOR_SUCCESS, ret);
        }

        ret = g_sensorDev->Enable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        OsalMSleep(SENSOR_WAIT_TIME);
        ret = g_sensorDev->Disable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        info++;
    }

    ret = g_sensorDev->Unregister(TRADITIONAL_SENSOR_TYPE, TraditionalSensorTestDataCallback);
    EXPECT_EQ(SENSOR_SUCCESS, ret);
    EXPECT_EQ(1, g_sensorDataFlag);
    g_sensorDataFlag = 1;
}

/**
  * @tc.name: SetSensorMode002
  * @tc.desc: Sets the data reporting mode for the specified sensor.The current real-time polling mode is valid.
  * Other values are invalid.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869U
  */
HWTEST_F(HdfSensorTest, SetSensorMode002, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    int32_t ret = g_sensorDev->SetBatch(ABNORMAL_SENSORID, SENSOR_INTERVAL1, SENSOR_POLL_TIME);
    EXPECT_EQ(SENSOR_NOT_SUPPORT, ret);
}

/**
  * @tc.name: SetSensorMode002
  * @tc.desc: Sets the data reporting mode for the specified sensor.The current real-time polling mode is valid.
  * Other values are invalid.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869U
  */
HWTEST_F(HdfSensorTest, SetSensorMode003, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    struct SensorInformation *info = nullptr;

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }

    info = g_sensorInfo;
    for (int32_t i = 0; i < g_count; i++) {
        int32_t ret = g_sensorDev->SetBatch(info->sensorId, SENSOR_INTERVAL1, SENSOR_POLL_TIME);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        ret = g_sensorDev->SetMode(info->sensorId, SENSOR_MODE_DEFAULT);
        EXPECT_EQ(SENSOR_FAILURE, ret);
        ret = g_sensorDev->Enable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        OsalMSleep(SENSOR_WAIT_TIME);
        ret = g_sensorDev->Disable(info->sensorId);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        info++;
    }
}

/**
  * @tc.name: SetSensorOption001
  * @tc.desc: Sets options for the specified sensor, including its measurement range and accuracy.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869U
  */
HWTEST_F(HdfSensorTest, SetSensorOption001, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    struct SensorInformation *info = nullptr;

    if (g_sensorInfo == nullptr) {
        EXPECT_NE(nullptr, g_sensorInfo);
        return;
    }

    info = g_sensorInfo;
    for (int32_t i = 0; i < g_count; i++) {
        int32_t ret = g_sensorDev->SetOption(info->sensorId, 0);
        EXPECT_EQ(SENSOR_SUCCESS, ret);
        info++;
    }
}

/**
  * @tc.name: SetSensorOption001
  * @tc.desc: Sets options for the specified sensor, including its measurement range and accuracy.
  * @tc.type: FUNC
  * @tc.require: SR000F869M, AR000F869U
  */
HWTEST_F(HdfSensorTest, SetSensorOption002, TestSize.Level1)
{
    if (g_sensorDev == nullptr) {
        EXPECT_NE(nullptr, g_sensorDev);
        return;
    }

    int32_t ret = g_sensorDev->SetOption(ABNORMAL_SENSORID, 0);
    EXPECT_EQ(SENSOR_NOT_SUPPORT, ret);
}
