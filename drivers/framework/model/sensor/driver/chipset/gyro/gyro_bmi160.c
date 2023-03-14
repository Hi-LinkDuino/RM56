/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gyro_bmi160.h"
#include <securec.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_gyro_driver.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"

#define HDF_LOG_TAG    hdf_sensor_gyro

static struct Bmi160DrvData *g_bmi160DrvData = NULL;

struct Bmi160DrvData *GyroBmi160GetDrvData(void)
{
    return g_bmi160DrvData;
}

/* IO config for int-pin and I2C-pin */
#define SENSOR_I2C6_DATA_REG_ADDR 0x114f004c
#define SENSOR_I2C6_CLK_REG_ADDR  0x114f0048
#define SENSOR_I2C_REG_CFG        0x403

static int32_t ReadBmi160GyroRawData(struct SensorCfgData *data, struct GyroData *rawData, int64_t *timestamp)
{
    uint8_t status = 0;
    uint8_t reg[GYRO_AXIS_BUTT];
    OsalTimespec time;

    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)memset_s(reg, sizeof(reg), 0, sizeof(reg));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return HDF_FAILURE;
    }
    *timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT; /* unit nanosecond */

    int32_t ret = ReadSensor(&data->busCfg, BMI160_STATUS_ADDR, &status, sizeof(uint8_t));
    if (!(status & BMI160_GYRO_DATA_READY_MASK) || (ret != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }

    ret = ReadSensor(&data->busCfg, BMI160_GYRO_X_LSB_ADDR, &reg[GYRO_X_AXIS_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMI160_GYRO_X_MSB_ADDR, &reg[GYRO_X_AXIS_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMI160_GYRO_Y_LSB_ADDR, &reg[GYRO_Y_AXIS_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMI160_GYRO_Y_MSB_ADDR, &reg[GYRO_Y_AXIS_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMI160_GYRO_Z_LSB_ADDR, &reg[GYRO_Z_AXIS_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMI160_GYRO_Z_MSB_ADDR, &reg[GYRO_Z_AXIS_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    rawData->x = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[GYRO_X_AXIS_MSB], SENSOR_DATA_WIDTH_8_BIT) |
        reg[GYRO_X_AXIS_LSB]);
    rawData->y = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[GYRO_Y_AXIS_MSB], SENSOR_DATA_WIDTH_8_BIT) |
        reg[GYRO_Y_AXIS_LSB]);
    rawData->z = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[GYRO_Z_AXIS_MSB], SENSOR_DATA_WIDTH_8_BIT) |
        reg[GYRO_Z_AXIS_LSB]);

    return ret;
}

int32_t ReadBmi160GyroData(struct SensorCfgData *data)
{
    int32_t ret;
    struct GyroData rawData = { 0, 0, 0 };
    int32_t tmp[GYRO_AXIS_NUM];
    struct SensorReportEvent event;

    (void)memset_s(&event, sizeof(event), 0, sizeof(event));

    ret = ReadBmi160GyroRawData(data, &rawData, &event.timestamp);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    event.sensorId = SENSOR_TAG_GYROSCOPE;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_REALTIME;

    tmp[GYRO_X_AXIS] = rawData.x * BMI160_GYRO_SENSITIVITY_2000DPS;
    tmp[GYRO_Y_AXIS] = rawData.y * BMI160_GYRO_SENSITIVITY_2000DPS;
    tmp[GYRO_Z_AXIS] = rawData.z * BMI160_GYRO_SENSITIVITY_2000DPS;

    ret = SensorRawDataToRemapData(data->direction, tmp, sizeof(tmp) / sizeof(tmp[0]));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: BMI160 convert raw data failed", __func__);
        return HDF_FAILURE;
    }

    event.dataLen = sizeof(tmp);
    event.data = (uint8_t *)&tmp;
    ret = ReportSensorEvent(&event);
    return ret;
}
static int32_t InitBmi160(struct SensorCfgData *data)
{
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    ret = SetSensorRegCfgArray(&data->busCfg, data->regCfgGroup[SENSOR_INIT_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: BMI160 sensor init config failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t InitGyroPreConfig(void)
{
    if (SetSensorPinMux(SENSOR_I2C6_DATA_REG_ADDR, SENSOR_ADDR_WIDTH_4_BYTE, SENSOR_I2C_REG_CFG) != HDF_SUCCESS) {
        HDF_LOGE("%s: Data write mux pin failed", __func__);
        return HDF_FAILURE;
    }
    if (SetSensorPinMux(SENSOR_I2C6_CLK_REG_ADDR, SENSOR_ADDR_WIDTH_4_BYTE, SENSOR_I2C_REG_CFG) != HDF_SUCCESS) {
        HDF_LOGE("%s: Clk write mux pin failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t DispatchBMI160(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t GyroBmi160BindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct Bmi160DrvData *drvData = (struct Bmi160DrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc Bmi160 drv data fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchBMI160;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_bmi160DrvData = drvData;

    return HDF_SUCCESS;
}

int32_t GyroBmi160InitDriver(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct GyroOpsCall ops;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct Bmi160DrvData *drvData = (struct Bmi160DrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    ret = InitGyroPreConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init  BMI160 bus mux config", __func__);
        return HDF_FAILURE;
    }

    drvData->sensorCfg = GyroCreateCfgData(device->property);
    if (drvData->sensorCfg == NULL || drvData->sensorCfg->root == NULL) {
        HDF_LOGD("%s: Creating gyrocfg failed because detection failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ops.Init = NULL;
    ops.ReadData = ReadBmi160GyroData;
    ret = GyroRegisterChipOps(&ops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register BMI160 gyro failed", __func__);
        return HDF_FAILURE;
    }

    ret = InitBmi160(drvData->sensorCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init BMI160 gyro failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void GyroBmi160ReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct Bmi160DrvData *drvData = (struct Bmi160DrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->sensorCfg != NULL) {
        GyroReleaseCfgData(drvData->sensorCfg);
        drvData->sensorCfg = NULL;
    }

    OsalMemFree(drvData);
}

struct HdfDriverEntry g_gyroBmi160DevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_GYRO_BMI160",
    .Bind = GyroBmi160BindDriver,
    .Init = GyroBmi160InitDriver,
    .Release = GyroBmi160ReleaseDriver,
};

HDF_INIT(g_gyroBmi160DevEntry);
