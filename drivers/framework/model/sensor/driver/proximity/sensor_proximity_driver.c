/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_proximity_driver.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_math.h"
#include "osal_mem.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    sensor_proximity_driver_c

#define HDF_PROXIMITY_WORK_QUEUE_NAME    "hdf_proximity_work_queue"

static struct ProximityDrvData *g_proximityDrvData = NULL;

static struct ProximityDrvData *ProximityGetDrvData(void)
{
    return g_proximityDrvData;
}

static struct SensorRegCfgGroupNode *g_regCfgGroup[SENSOR_GROUP_MAX] = { NULL };

int32_t ProximityRegisterChipOps(const struct ProximityOpsCall *ops)
{
    struct ProximityDrvData *drvData = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(ops, HDF_ERR_INVALID_PARAM);

    drvData = ProximityGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    drvData->ops.Init = ops->Init;
    drvData->ops.ReadData = ops->ReadData;
    return HDF_SUCCESS;
}

static void ProximityDataWorkEntry(void *arg)
{
    int32_t ret;
    struct ProximityDrvData *drvData = (struct ProximityDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);
    CHECK_NULL_PTR_RETURN(drvData->ops.ReadData);

    ret = drvData->ops.ReadData(drvData->proximityCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity read data failed", __func__);
        return;
    }
}

static void ProximityTimerEntry(uintptr_t arg)
{
    int64_t interval;
    int32_t ret;
    struct ProximityDrvData *drvData = (struct ProximityDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->proximityWorkQueue, &drvData->proximityWork)) {
        HDF_LOGE("%s: proximity add work queue failed", __func__);
    }

    interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
    interval = (interval < SENSOR_TIMER_MIN_TIME) ? SENSOR_TIMER_MIN_TIME : interval;
    ret = OsalTimerSetTimeout(&drvData->proximityTimer, interval);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity modify time failed", __func__);
    }
}

static int32_t InitProximityData(struct ProximityDrvData *drvData)
{
    if (HdfWorkQueueInit(&drvData->proximityWorkQueue, HDF_PROXIMITY_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->proximityWork, ProximityDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->interval = SENSOR_TIMER_MIN_TIME;
    drvData->enable = false;
    drvData->detectFlag = false;

    return HDF_SUCCESS;
}

static int32_t SetProximityEnable(void)
{
    int32_t ret;
    struct ProximityDrvData *drvData = ProximityGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->proximityCfg, HDF_ERR_INVALID_PARAM);

    if (drvData->enable) {
        HDF_LOGE("%s: proximity sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->proximityCfg->busCfg, drvData->proximityCfg->regCfgGroup[SENSOR_ENABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity sensor enable config failed", __func__);
        return ret;
    }

    ret = OsalTimerCreate(&drvData->proximityTimer, SENSOR_TIMER_MIN_TIME, ProximityTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->proximityTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SetProximityDisable(void)
{
    int32_t ret;
    struct ProximityDrvData *drvData = ProximityGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->proximityCfg, HDF_ERR_INVALID_PARAM);

    if (!drvData->enable) {
        HDF_LOGE("%s: proximity sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->proximityCfg->busCfg,
        drvData->proximityCfg->regCfgGroup[SENSOR_DISABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity sensor disable config failed", __func__);
        return ret;
    }

    ret = OsalTimerDelete(&drvData->proximityTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: proximity delete timer failed", __func__);
        return ret;
    }
    drvData->enable = false;
    return HDF_SUCCESS;
}

static int32_t SetProximityBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;

    struct ProximityDrvData *drvData = NULL;

    drvData = ProximityGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->interval = samplingInterval;

    return HDF_SUCCESS;
}

static int32_t SetProximityMode(int32_t mode)
{
    if (mode <= SENSOR_WORK_MODE_DEFAULT || mode >= SENSOR_WORK_MODE_MAX) {
        HDF_LOGE("%s: The current mode is not supported", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetProximityOption(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t DispatchProximity(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t ProximityBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    HDF_LOGI("%s: enter", __func__);
    struct ProximityDrvData *drvData = (struct ProximityDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGI("%s: malloc proximity drv data fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchProximity;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_proximityDrvData = drvData;
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t InitProximityOps(struct SensorCfgData *config, struct SensorDeviceInfo *deviceInfo)
{
    struct ProximityDrvData *drvData = ProximityGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    deviceInfo->ops.Enable = SetProximityEnable;
    deviceInfo->ops.Disable = SetProximityDisable;
    deviceInfo->ops.SetBatch = SetProximityBatch;
    deviceInfo->ops.SetMode = SetProximityMode;
    deviceInfo->ops.SetOption = SetProximityOption;

    if (memcpy_s(&deviceInfo->sensorInfo, sizeof(deviceInfo->sensorInfo),
        &config->sensorInfo, sizeof(config->sensorInfo)) != EOK) {
        HDF_LOGE("%s: Copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitProximityAfterDetected(struct SensorCfgData *config)
{
    struct SensorDeviceInfo deviceInfo;
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    if (InitProximityOps(config, &deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init proximity ops failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add proximity device failed", __func__);
        return HDF_FAILURE;
    }

    if (ParseSensorRegConfig(config) != HDF_SUCCESS) {
        HDF_LOGE("%s: Parse sensor register failed", __func__);
        (void)DeleteSensorDevice(&config->sensorInfo);
        ReleaseSensorAllRegConfig(config);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

struct SensorCfgData *ProximityCreateCfgData(const struct DeviceResourceNode *node)
{
    struct ProximityDrvData *drvData = ProximityGetDrvData();

    if (drvData == NULL || node == NULL) {
        HDF_LOGE("%s: Proximity node pointer NULL", __func__);
        return NULL;
    }

    if (drvData->detectFlag) {
        HDF_LOGE("%s: Proximity sensor have detected", __func__);
        return NULL;
    }

    if (drvData->proximityCfg == NULL) {
        HDF_LOGE("%s: Proximity proximityCfg pointer NULL", __func__);
        return NULL;
    }

    if (GetSensorBaseConfigData(node, drvData->proximityCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get sensor base config failed", __func__);
        goto BASE_CONFIG_EXIT;
    }

    if (DetectSensorDevice(drvData->proximityCfg) != HDF_SUCCESS) {
        HDF_LOGI("%s: Proximity sensor detect device no exist", __func__);
        drvData->detectFlag = false;
        goto BASE_CONFIG_EXIT;
    }

    drvData->detectFlag = true;
    if (InitProximityAfterDetected(drvData->proximityCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Proximity sensor detect device no exist", __func__);
        goto INIT_EXIT;
    }
    return drvData->proximityCfg;

INIT_EXIT:
    (void)ReleaseSensorBusHandle(&drvData->proximityCfg->busCfg);
BASE_CONFIG_EXIT:
    drvData->proximityCfg->root = NULL;
    (void)memset_s(&drvData->proximityCfg->sensorInfo, sizeof(struct SensorBasicInfo),
        0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&drvData->proximityCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&drvData->proximityCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
    return drvData->proximityCfg;
}

void ProximityReleaseCfgData(struct SensorCfgData *proximityCfg)
{
    CHECK_NULL_PTR_RETURN(proximityCfg);

    (void)DeleteSensorDevice(&proximityCfg->sensorInfo);
    ReleaseSensorAllRegConfig(proximityCfg);
    (void)ReleaseSensorBusHandle(&proximityCfg->busCfg);

    proximityCfg->root = NULL;
    (void)memset_s(&proximityCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&proximityCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&proximityCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
}

int32_t ProximityInitDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct ProximityDrvData *drvData = (struct ProximityDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (InitProximityData(drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init accel config failed", __func__);
        return HDF_FAILURE;
    }

    drvData->proximityCfg = (struct SensorCfgData *)OsalMemCalloc(sizeof(*drvData->proximityCfg));
    if (drvData->proximityCfg == NULL) {
        HDF_LOGE("%s: Malloc proximity config data failed", __func__);
        return HDF_FAILURE;
    }

    drvData->proximityCfg->regCfgGroup = &g_regCfgGroup[0];

    HDF_LOGI("%s: Init proximity driver success", __func__);
    return HDF_SUCCESS;
}

void ProximityReleaseDriver(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: enter", __func__);
    CHECK_NULL_PTR_RETURN(device);

    struct ProximityDrvData *drvData = (struct ProximityDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->detectFlag && drvData->proximityCfg != NULL) {
        ProximityReleaseCfgData(drvData->proximityCfg);
    }

    OsalMemFree(drvData->proximityCfg);
    drvData->proximityCfg = NULL;

    HdfWorkDestroy(&drvData->proximityWork);
    HdfWorkQueueDestroy(&drvData->proximityWorkQueue);
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorProximityDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_PROXIMITY",
    .Bind = ProximityBindDriver,
    .Init = ProximityInitDriver,
    .Release = ProximityReleaseDriver,
};

HDF_INIT(g_sensorProximityDevEntry);