/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "magnetic_lsm303.h"
#include <securec.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_magnetic_driver.h"

#define HDF_LOG_TAG    hdf_sensor_magnetic

static struct Lsm303DrvData *g_lsm303DrvData = NULL;

struct Lsm303DrvData *Lsm303GetDrvData(void)
{
    return g_lsm303DrvData;
}

/* IO config for int-pin and I2C-pin */
#define SENSOR_I2C6_DATA_REG_ADDR 0x114f004c
#define SENSOR_I2C6_CLK_REG_ADDR  0x114f0048
#define SENSOR_I2C_REG_CFG        0x403

static int32_t ReadLsm303RawData(struct SensorCfgData *data, struct MagneticData *rawData, int64_t *timestamp)
{
    uint8_t status = 0;
    uint8_t reg[MAGNETIC_AXIS_BUTT];
    OsalTimespec time;

    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)memset_s(reg, sizeof(reg), 0, sizeof(reg));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return HDF_FAILURE;
    }
    *timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT; /* unit nanosecond */

    int32_t ret = ReadSensor(&data->busCfg, LSM303_STATUS_ADDR, &status, sizeof(uint8_t));
    if (!(status & LSM303_DATA_READY_MASK) || (ret != HDF_SUCCESS)) {
        HDF_LOGE("%s: data status [%u] ret [%d]", __func__, status, ret);
        return HDF_FAILURE;
    }

    ret = ReadSensor(&data->busCfg, LSM303_MAGNETIC_X_MSB_ADDR, &reg[MAGNETIC_X_AXIS_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, LSM303_MAGNETIC_X_LSB_ADDR, &reg[MAGNETIC_X_AXIS_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, LSM303_MAGNETIC_Y_MSB_ADDR, &reg[MAGNETIC_Y_AXIS_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, LSM303_MAGNETIC_Y_LSB_ADDR, &reg[MAGNETIC_Y_AXIS_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, LSM303_MAGNETIC_Z_MSB_ADDR, &reg[MAGNETIC_Z_AXIS_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, LSM303_MAGNETIC_Z_LSB_ADDR, &reg[MAGNETIC_Z_AXIS_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    rawData->x = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[MAGNETIC_X_AXIS_MSB], SENSOR_DATA_WIDTH_8_BIT) |
        reg[MAGNETIC_X_AXIS_LSB]);
    rawData->y = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[MAGNETIC_Y_AXIS_MSB], SENSOR_DATA_WIDTH_8_BIT) |
        reg[MAGNETIC_Y_AXIS_LSB]);
    rawData->z = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[MAGNETIC_Z_AXIS_MSB], SENSOR_DATA_WIDTH_8_BIT) |
        reg[MAGNETIC_Z_AXIS_LSB]);

    return HDF_SUCCESS;
}

int32_t ReadLsm303Data(struct SensorCfgData *data)
{
    struct MagneticData rawData = { 0, 0, 0 };
    int32_t tmp[MAGNETIC_AXIS_NUM];
    struct SensorReportEvent event;

    (void)memset_s(&event, sizeof(event), 0, sizeof(event));
    (void)memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    int32_t ret = ReadLsm303RawData(data, &rawData, &event.timestamp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LSM303 read raw data failed", __func__);
        return HDF_FAILURE;
    }

    event.sensorId = SENSOR_TAG_MAGNETIC_FIELD;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_REALTIME;

    tmp[MAGNETIC_X_AXIS] = rawData.x * LSM303_MAGNETIC_GIN / LSM303DLHC_SENSITIVITY_XY47GA;
    tmp[MAGNETIC_Y_AXIS] = rawData.y * LSM303_MAGNETIC_GIN / LSM303DLHC_SENSITIVITY_XY47GA;
    tmp[MAGNETIC_Z_AXIS] = rawData.z * LSM303_MAGNETIC_GIN / LSM303DLHC_SENSITIVITY_Z47GA;

    ret = SensorRawDataToRemapData(data->direction, tmp, sizeof(tmp) / sizeof(tmp[0]));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LSM303 convert raw data failed", __func__);
        return HDF_FAILURE;
    }

    event.dataLen = sizeof(tmp);
    event.data = (uint8_t *)&tmp;
    ret = ReportSensorEvent(&event);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LSM303 report data failed", __func__);
    }

    return ret;
}

static int32_t InitLsm303(struct SensorCfgData *data)
{
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    ret = SetSensorRegCfgArray(&data->busCfg, data->regCfgGroup[SENSOR_INIT_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Lsm303 sensor init config failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitMagneticPreConfig(void)
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

static int32_t DispatchLsm303(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t Lsm303BindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct Lsm303DrvData *drvData = (struct Lsm303DrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc Lsm303 drv data fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchLsm303;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_lsm303DrvData = drvData;

    return HDF_SUCCESS;
}

int32_t Lsm303InitDriver(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct MagneticOpsCall ops;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct Lsm303DrvData *drvData = (struct Lsm303DrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    ret = InitMagneticPreConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init Lsm303 bus mux config", __func__);
        return HDF_FAILURE;
    }

    drvData->sensorCfg = MagneticCreateCfgData(device->property);
    if (drvData->sensorCfg == NULL || drvData->sensorCfg->root == NULL) {
        HDF_LOGD("%s: Creating magneticcfg failed because detection failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ops.Init = NULL;
    ops.ReadData = ReadLsm303Data;
    ret = MagneticRegisterChipOps(&ops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register lsm303 magnetic failed", __func__);
        return HDF_FAILURE;
    }

    ret = InitLsm303(drvData->sensorCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init lsm303 magnetic failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void Lsm303ReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct Lsm303DrvData *drvData = (struct Lsm303DrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->sensorCfg != NULL) {
        MagneticReleaseCfgData(drvData->sensorCfg);
        drvData->sensorCfg = NULL;
    }
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_magneticLsm303DevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_MAGNETIC_LSM303",
    .Bind = Lsm303BindDriver,
    .Init = Lsm303InitDriver,
    .Release = Lsm303ReleaseDriver,
};

HDF_INIT(g_magneticLsm303DevEntry);