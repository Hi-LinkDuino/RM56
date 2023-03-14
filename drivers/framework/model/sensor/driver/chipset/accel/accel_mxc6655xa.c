/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "accel_mxc6655xa.h"
#include <securec.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_accel_driver.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"

#define HDF_LOG_TAG    hdf_sensor_accel
#define MXC6655_RANGE    (16384 * 2)
#define MXC6655_PRECISION    12
#define MXC6655_BOUNDARY    (0x1 << (MXC6655_PRECISION - 1))
#define MXC6655_GRAVITY_STEP    (MXC6655_RANGE / MXC6655_BOUNDARY)
#define MXC6655_MASK    0x7fff
#define MXC6655_ACCEL_OUTPUT_16BIT    16
#define MXC6655_ACCEL_OUTPUT_MSB      8

static struct Mxc6655xaDrvData *g_Mxc6655xaDrvData = NULL;

struct Mxc6655xaDrvData *Mxc6655xaGetDrvData(void)
{
    return g_Mxc6655xaDrvData;
}

static int sensor_convert_data(char high_byte, char low_byte)
{
    int32_t result;

    result = ((uint32_t)high_byte << (MXC6655_PRECISION - MXC6655_ACCEL_OUTPUT_MSB)) |
        ((uint32_t)low_byte >> (MXC6655_ACCEL_OUTPUT_16BIT - MXC6655_PRECISION));

    if (result < MXC6655_BOUNDARY) {
        result = result * MXC6655_GRAVITY_STEP;
    } else {
        result = ~(((~result & (MXC6655_MASK >> (MXC6655_ACCEL_OUTPUT_16BIT - MXC6655_PRECISION))) + 1) *
            MXC6655_GRAVITY_STEP) + 1;
    }

    return result;
}

static int32_t ReadMxc6655xaRawData(struct SensorCfgData *data, struct AccelData *rawData, int64_t *timestamp)
{
    int32_t status = 0;
    int32_t reg[ACCEL_AXIS_BUTT];
    OsalTimespec time;
    int32_t x;
    int32_t y;
    int32_t z;

    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)memset_s(reg, sizeof(reg), 0, sizeof(reg));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return HDF_FAILURE;
    }
    *timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT; /* unit nanosecond */

    int32_t ret = ReadSensor(&data->busCfg, MXC6655XA_STATUS_ADDR, &status, sizeof(int32_t));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: data status [%u] ret [%d]", __func__, status, ret);
        return HDF_FAILURE;
    }

    ret = ReadSensor(&data->busCfg, MXC6655XA_ACCEL_X_LSB_ADDR, &reg[ACCEL_X_AXIS_LSB], sizeof(int32_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, MXC6655XA_ACCEL_X_MSB_ADDR, &reg[ACCEL_X_AXIS_MSB], sizeof(int32_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, MXC6655XA_ACCEL_Y_LSB_ADDR, &reg[ACCEL_Y_AXIS_LSB], sizeof(int32_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, MXC6655XA_ACCEL_Y_MSB_ADDR, &reg[ACCEL_Y_AXIS_MSB], sizeof(int32_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, MXC6655XA_ACCEL_Z_LSB_ADDR, &reg[ACCEL_Z_AXIS_LSB], sizeof(int32_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, MXC6655XA_ACCEL_Z_MSB_ADDR, &reg[ACCEL_Z_AXIS_MSB], sizeof(int32_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    x = sensor_convert_data(reg[ACCEL_X_AXIS_MSB], reg[ACCEL_X_AXIS_LSB]);
    y = sensor_convert_data(reg[ACCEL_Y_AXIS_MSB], reg[ACCEL_Y_AXIS_LSB]);
    z = sensor_convert_data(reg[ACCEL_Z_AXIS_MSB], reg[ACCEL_Z_AXIS_LSB]);
    rawData->x = x;
    rawData->y = y;
    rawData->z = z;

    return HDF_SUCCESS;
}

int32_t ReadMxc6655xaData(struct SensorCfgData *cfg, struct SensorReportEvent *event)
{
    int32_t ret;
    struct AccelData rawData = { 0, 0, 0 };
    static int32_t tmp[ACCEL_AXIS_NUM];

    CHECK_NULL_PTR_RETURN_VALUE(cfg, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(event, HDF_ERR_INVALID_PARAM);

    ret = ReadMxc6655xaRawData(cfg, &rawData, &event->timestamp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MXC6655XA read raw data failed", __func__);
        return HDF_FAILURE;
    }

    event->sensorId = SENSOR_TAG_ACCELEROMETER;
    event->option = 0;
    event->mode = SENSOR_WORK_MODE_REALTIME;

    rawData.x = rawData.x * MXC6655XA_ACC_SENSITIVITY_2G;
    rawData.y = rawData.y * MXC6655XA_ACC_SENSITIVITY_2G;
    rawData.z = rawData.z * MXC6655XA_ACC_SENSITIVITY_2G;

    tmp[ACCEL_X_AXIS] = (rawData.x * SENSOR_CONVERT_UNIT) / SENSOR_CONVERT_UNIT;
    tmp[ACCEL_Y_AXIS] = (rawData.y * SENSOR_CONVERT_UNIT) / SENSOR_CONVERT_UNIT;
    tmp[ACCEL_Z_AXIS] = (rawData.z * SENSOR_CONVERT_UNIT) / SENSOR_CONVERT_UNIT;

    ret = SensorRawDataToRemapData(cfg->direction, tmp, sizeof(tmp) / sizeof(tmp[0]));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MXC6655XA convert raw data failed", __func__);
        return HDF_FAILURE;
    }

    event->dataLen = sizeof(tmp);
    event->data = (uint8_t *)&tmp;

    return ret;
}

static int32_t InitMxc6655xa(struct SensorCfgData *data)
{
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    ret = SetSensorRegCfgArray(&data->busCfg, data->regCfgGroup[SENSOR_INIT_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: MXC6655XA sensor init config failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t DispatchMXC6655xa(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t Mxc6655xaBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct Mxc6655xaDrvData *drvData = (struct Mxc6655xaDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc MXC6655XA drv data fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchMXC6655xa;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_Mxc6655xaDrvData = drvData;

    return HDF_SUCCESS;
}

int32_t Mxc6655xaInitDriver(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct AccelOpsCall ops;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct Mxc6655xaDrvData *drvData = (struct Mxc6655xaDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->sensorCfg = AccelCreateCfgData(device->property);
    if (drvData->sensorCfg == NULL || drvData->sensorCfg->root == NULL) {
        HDF_LOGD("%s: Creating accelcfg failed because detection failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ops.Init = NULL;
    ops.ReadData = ReadMxc6655xaData;
    ret = AccelRegisterChipOps(&ops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register MXC6655XA accel failed", __func__);
        return HDF_FAILURE;
    }

    ret = InitMxc6655xa(drvData->sensorCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init MXC6655XA accel failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void Mxc6655xaReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct Mxc6655xaDrvData *drvData = (struct Mxc6655xaDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->sensorCfg != NULL) {
        AccelReleaseCfgData(drvData->sensorCfg);
        drvData->sensorCfg = NULL;
    }
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_accelMxc6655xaDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_ACCEL_MXC6655XA",
    .Bind = Mxc6655xaBindDriver,
    .Init = Mxc6655xaInitDriver,
    .Release = Mxc6655xaReleaseDriver,
};

HDF_INIT(g_accelMxc6655xaDevEntry);
