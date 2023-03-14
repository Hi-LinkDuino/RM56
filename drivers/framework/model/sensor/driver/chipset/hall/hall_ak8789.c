/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hall_ak8789.h"
#include "osal_irq.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_hall_driver.h"

#define HDF_LOG_TAG    hdf_sensor_hall

static struct Ak8789DrvData *g_ak8789DrvData = NULL;

struct Ak8789DrvData *Ak8789GetDrvData(void)
{
    return g_ak8789DrvData;
}

/* IO config for int-pin and Gpio-pin */
#define SENSOR_HALL_DATA_REG_ADDR 0x114f0040
#define SENSOR_HALL_CLK_REG_ADDR  0x114f0044
#define SENSOR_HALL_REG_CFG       0x400

static int32_t InitHallPreConfig(void)
{
    if (SetSensorPinMux(SENSOR_HALL_DATA_REG_ADDR, SENSOR_ADDR_WIDTH_4_BYTE, SENSOR_HALL_REG_CFG) != HDF_SUCCESS) {
        HDF_LOGE("%s: Data write mux pin failed", __func__);
        return HDF_FAILURE;
    }
    if (SetSensorPinMux(SENSOR_HALL_CLK_REG_ADDR, SENSOR_ADDR_WIDTH_4_BYTE, SENSOR_HALL_REG_CFG) != HDF_SUCCESS) {
        HDF_LOGE("%s: Clk write mux pin failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t DispatchAK8789(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t Ak8789BindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct Ak8789DrvData *drvData = (struct Ak8789DrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc Ak8789 drv data fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchAK8789;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_ak8789DrvData = drvData;

    return HDF_SUCCESS;
}

int32_t AK8789InitDriver(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct HallOpsCall ops;
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct Ak8789DrvData *drvData = (struct Ak8789DrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    ret = InitHallPreConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init  AK8789 bus mux config", __func__);
        return HDF_FAILURE;
    }

    drvData->sensorCfg = HallCreateCfgData(device->property);
    if (drvData->sensorCfg == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ops.Init = NULL;
    ops.ReadData = NULL;
    ret = HallRegisterChipOps(&ops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register AK8789 hall failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void Ak8789ReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct Ak8789DrvData *drvData = (struct Ak8789DrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    HallReleaseCfgData(drvData->sensorCfg);
    drvData->sensorCfg = NULL;
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_hallAk8789DevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_HALL_AK8789",
    .Bind = Ak8789BindDriver,
    .Init = AK8789InitDriver,
    .Release = Ak8789ReleaseDriver,
};

HDF_INIT(g_hallAk8789DevEntry);