/*
 * Copyright (c) 2021 xu
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "proximity_apds9960.h"
#include <securec.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_proximity_driver.h"

#define HDF_LOG_TAG    hdf_sensor_proximity

#define PROXIMITY_STATE_FAR    5
#define PROXIMITY_STATE_NEAR   0

static struct Apds9960DrvData *g_apds9960DrvData = NULL;

struct Apds9960DrvData *Apds9960GetDrvData(void)
{
    return g_apds9960DrvData;
}

/* IO config for int-pin and I2C-pin */
#define SENSOR_I2C6_DATA_REG_ADDR 0x114f004c
#define SENSOR_I2C6_CLK_REG_ADDR  0x114f0048
#define SENSOR_I2C_REG_CFG        0x403

static int32_t ReadApds9960RawData(struct SensorCfgData *data, struct ProximityData *rawData, int64_t *timestamp)
{
    OsalTimespec time;

    (void)memset_s(&time, sizeof(time), 0, sizeof(time));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return HDF_FAILURE;
    }
    *timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT; /* unit nanosecond */

    int32_t ret = ReadSensor(&data->busCfg, APDS9960_PROXIMITY_DATA_ADDR, &rawData->stateFlag, sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    return ret;
}

int32_t ReadApds9960Data(struct SensorCfgData *data)
{
    int32_t ret;
    int32_t tmp;
    struct ProximityData rawData = { 5 };
    struct SensorReportEvent event;

    (void)memset_s(&event, sizeof(event), 0, sizeof(event));

    ret = ReadApds9960RawData(data, &rawData, &event.timestamp);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    event.sensorId = SENSOR_TAG_PROXIMITY;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_ON_CHANGE;

    if (rawData.stateFlag <= APDS9960_PROXIMITY_THRESHOLD) {
        tmp = PROXIMITY_STATE_FAR;
    } else {
        tmp = PROXIMITY_STATE_NEAR;
    }

    event.dataLen = sizeof(tmp);
    event.data = (uint8_t *)&tmp;
    ret = ReportSensorEvent(&event);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: APDS9960 report data failed", __func__);
    }

    return ret;
}

static int32_t InitApda9960(struct SensorCfgData *data)
{
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    ret = SetSensorRegCfgArray(&data->busCfg, data->regCfgGroup[SENSOR_INIT_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: APDS9960 sensor init config failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t InitProximityPreConfig(void)
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

static int32_t DispatchApds9960(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t Apds9960BindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct Apds9960DrvData *drvData = (struct Apds9960DrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc Apds9960 drv data fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchApds9960;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_apds9960DrvData = drvData;

    return HDF_SUCCESS;
}

int32_t Apds996InitDriver(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct ProximityOpsCall ops;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct Apds9960DrvData *drvData = (struct Apds9960DrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    ret = InitProximityPreConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init  APDS9960 bus mux config", __func__);
        return HDF_FAILURE;
    }

    drvData->sensorCfg = ProximityCreateCfgData(device->property);
    if (drvData->sensorCfg == NULL || drvData->sensorCfg->root == NULL) {
        HDF_LOGD("%s: Creating proximitycfg failed because detection failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ops.Init = NULL;
    ops.ReadData = ReadApds9960Data;
    ret = ProximityRegisterChipOps(&ops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register APDS9960 proximity failed", __func__);
        return HDF_FAILURE;
    }

    ret = InitApda9960(drvData->sensorCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init APDS9960 proximity failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}


void Apds996ReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct Apds9960DrvData *drvData = (struct Apds9960DrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->sensorCfg != NULL) {
        ProximityReleaseCfgData(drvData->sensorCfg);
        drvData->sensorCfg = NULL;
    }
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_proximityApds9960DevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_PROXIMITY_APDS9960",
    .Bind = Apds9960BindDriver,
    .Init = Apds996InitDriver,
    .Release = Apds996ReleaseDriver,
};

HDF_INIT(g_proximityApds9960DevEntry);