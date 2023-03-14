/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pedometer_bmi160.h"
#include <securec.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_pedometer_driver.h"

#define HDF_LOG_TAG    hdf_sensor_pedometer

static struct Bmi160DrvData *g_bmi160DrvData = NULL;

struct Bmi160DrvData *PedometerBmi160GetDrvData(void)
{
    return g_bmi160DrvData;
}

/* IO config for int-pin and I2C-pin */
#define SENSOR_I2C6_DATA_REG_ADDR 0x114f004c
#define SENSOR_I2C6_CLK_REG_ADDR  0x114f0048
#define SENSOR_I2C_REG_CFG        0x403

static int32_t ReadBmi160PedometerRawData(struct SensorCfgData *data, struct PedometerData *rawData, int64_t *timestamp)
{
    uint8_t reg[PEDOMETER_BUTT];
    OsalTimespec time;

    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)memset_s(reg, sizeof(reg), 0, sizeof(reg));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return HDF_FAILURE;
    }
    *timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT;

    int32_t ret = ReadSensor(&data->busCfg, BMI160_PEDOMETER_LSB_ADDR, &reg[PEDOMETER_NU_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMI160_PEDOMETER_MSB_ADDR, &reg[PEDOMETER_NU_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    rawData->pedometer = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[PEDOMETER_NU_MSB], SENSOR_DATA_WIDTH_8_BIT) |
        reg[PEDOMETER_NU_LSB]);

    return HDF_SUCCESS;
}

int32_t ReadBmi160PedometerData(struct SensorCfgData *data)
{
    int32_t ret;
    struct PedometerData rawData = { 0 };
    static int32_t tmp;
    struct SensorReportEvent event;

    ret = memset_s(&event, sizeof(event), 0, sizeof(event));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "memset_s");

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    ret = ReadBmi160PedometerRawData(data, &rawData, &event.timestamp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: BMI160 read raw data failed", __func__);
        return HDF_FAILURE;
    }

    event.sensorId = SENSOR_TAG_PEDOMETER;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_REALTIME;

    tmp = rawData.pedometer;

    event.dataLen = sizeof(tmp);
    event.data = (uint8_t *)&tmp;
    ret = ReportSensorEvent(&event);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: BMI160 report data failed", __func__);
    }

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

static int32_t InitPedometerPreConfig(void)
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

int32_t PedometerBmi160BindDriver(struct HdfDeviceObject *device)
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

int32_t PedometerBmi160InitDriver(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct PedometerOpsCall ops;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct Bmi160DrvData *drvData = (struct Bmi160DrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    ret = InitPedometerPreConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init  BMI160 bus mux config", __func__);
        return HDF_FAILURE;
    }

    drvData->sensorCfg = PedometerCreateCfgData(device->property);
    if (drvData->sensorCfg == NULL || drvData->sensorCfg->root == NULL) {
        HDF_LOGD("%s: Creating pedometercfg failed because detection failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ops.Init = NULL;
    ops.ReadData = ReadBmi160PedometerData;
    ret = PedometerRegisterChipOps(&ops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register BMI160 pedometer failed", __func__);
        return HDF_FAILURE;
    }

    ret = InitBmi160(drvData->sensorCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init BMI160 pedometer failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void PedometerBmi160ReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct Bmi160DrvData *drvData = (struct Bmi160DrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->sensorCfg != NULL) {
        PedometerReleaseCfgData(drvData->sensorCfg);
        drvData->sensorCfg = NULL;
    }
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_pedometerBmi160DevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_PEDOMETER_BMI160",
    .Bind = PedometerBmi160BindDriver,
    .Init = PedometerBmi160InitDriver,
    .Release = PedometerBmi160ReleaseDriver,
};

HDF_INIT(g_pedometerBmi160DevEntry);
