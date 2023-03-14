/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_accel_driver.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_math.h"
#include "osal_mem.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_accel_driver

#define HDF_ACCEL_WORK_QUEUE_NAME    "hdf_accel_work_queue"

static struct AccelDrvData *g_accelDrvData = NULL;

static struct AccelDrvData *AccelGetDrvData(void)
{
    return g_accelDrvData;
}

static struct SensorRegCfgGroupNode *g_regCfgGroup[SENSOR_GROUP_MAX] = { NULL };

int32_t SubscribeAccelDataCallbackFunc(GravitySubscribeAccelCallback cb)
{
    CHECK_NULL_PTR_RETURN_VALUE(cb, HDF_ERR_INVALID_PARAM);

    struct AccelDrvData *drvData = AccelGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->cb = cb;

    return HDF_SUCCESS;
}

int32_t AccelRegisterChipOps(const struct AccelOpsCall *ops)
{
    struct AccelDrvData *drvData = AccelGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(ops, HDF_ERR_INVALID_PARAM);

    drvData->ops.Init = ops->Init;
    drvData->ops.ReadData = ops->ReadData;
    return HDF_SUCCESS;
}

static void AccelDataWorkEntry(void *arg)
{
    struct AccelDrvData *drvData = NULL;
    struct SensorReportEvent event;

    drvData = (struct AccelDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->ops.ReadData == NULL) {
        HDF_LOGI("%s: Accel ReadData function NULl", __func__);
        return;
    }

    if (drvData->ops.ReadData(drvData->accelCfg, &event) != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel read data failed", __func__);
        return;
    }

    if (ReportSensorEvent(&event) != HDF_SUCCESS) {
        HDF_LOGE("%s: report accel data failed", __func__);
        return;
    }

    if (drvData->cb != NULL) {
        drvData->cb((int32_t*)event.data, event.dataLen);
    }
}

static void AccelTimerEntry(uintptr_t arg)
{
    int64_t interval;
    int32_t ret;
    struct AccelDrvData *drvData = (struct AccelDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->accelWorkQueue, &drvData->accelWork)) {
        HDF_LOGE("%s: Accel add work queue failed", __func__);
    }

    interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
    interval = (interval < SENSOR_TIMER_MIN_TIME) ? SENSOR_TIMER_MIN_TIME : interval;
    ret = OsalTimerSetTimeout(&drvData->accelTimer, interval);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel modify time failed", __func__);
    }
}

static int32_t InitAccelData(struct AccelDrvData *drvData)
{
    if (HdfWorkQueueInit(&drvData->accelWorkQueue, HDF_ACCEL_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->accelWork, AccelDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->interval = SENSOR_TIMER_MIN_TIME;
    drvData->enable = false;
    drvData->detectFlag = false;

    return HDF_SUCCESS;
}

static int32_t SetAccelEnable(void)
{
    int32_t ret;
    struct AccelDrvData *drvData = AccelGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->accelCfg, HDF_ERR_INVALID_PARAM);

    if (drvData->enable) {
        HDF_LOGE("%s: Accel sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->accelCfg->busCfg, drvData->accelCfg->regCfgGroup[SENSOR_ENABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel sensor enable config failed", __func__);
        return ret;
    }

    ret = OsalTimerCreate(&drvData->accelTimer, SENSOR_TIMER_MIN_TIME, AccelTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->accelTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SetAccelDisable(void)
{
    int32_t ret;
    struct AccelDrvData *drvData = AccelGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->accelCfg, HDF_ERR_INVALID_PARAM);

    if (!drvData->enable) {
        HDF_LOGE("%s: Accel sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->accelCfg->busCfg, drvData->accelCfg->regCfgGroup[SENSOR_DISABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel sensor disable config failed", __func__);
        return ret;
    }

    ret = OsalTimerDelete(&drvData->accelTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel delete timer failed", __func__);
        return ret;
    }
    drvData->enable = false;

    return HDF_SUCCESS;
}

static int32_t SetAccelBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;

    struct AccelDrvData *drvData = NULL;

    drvData = AccelGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->interval = samplingInterval;

    return HDF_SUCCESS;
}

static int32_t SetAccelMode(int32_t mode)
{
    if (mode <= SENSOR_WORK_MODE_DEFAULT || mode >= SENSOR_WORK_MODE_MAX) {
        HDF_LOGE("%s: The current mode is not supported", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetAccelOption(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t DispatchAccel(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t AccelBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct AccelDrvData *drvData = (struct AccelDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc accel drv data fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchAccel;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_accelDrvData = drvData;
    return HDF_SUCCESS;
}

static int32_t InitAccelOps(struct SensorCfgData *config, struct SensorDeviceInfo *deviceInfo)
{
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    deviceInfo->ops.Enable = SetAccelEnable;
    deviceInfo->ops.Disable = SetAccelDisable;
    deviceInfo->ops.SetBatch = SetAccelBatch;
    deviceInfo->ops.SetMode = SetAccelMode;
    deviceInfo->ops.SetOption = SetAccelOption;

    if (memcpy_s(&deviceInfo->sensorInfo, sizeof(deviceInfo->sensorInfo),
        &config->sensorInfo, sizeof(config->sensorInfo)) != EOK) {
        HDF_LOGE("%s: Copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitAccelAfterDetected(struct SensorCfgData *config)
{
    struct SensorDeviceInfo deviceInfo;
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    if (InitAccelOps(config, &deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init accel ops failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add accel device failed", __func__);
        return HDF_FAILURE;
    }

    if (ParseSensorDirection(config) != HDF_SUCCESS) {
        HDF_LOGE("%s: Parse accel direction failed", __func__);
        (void)DeleteSensorDevice(&config->sensorInfo);
        return HDF_FAILURE;
    }

    if (ParseSensorRegConfig(config) != HDF_SUCCESS) {
        HDF_LOGE("%s: Parse sensor register failed", __func__);
        (void)DeleteSensorDevice(&config->sensorInfo);
        ReleaseSensorAllRegConfig(config);
        ReleaseSensorDirectionConfig(config);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

struct SensorCfgData *AccelCreateCfgData(const struct DeviceResourceNode *node)
{
    struct AccelDrvData *drvData = AccelGetDrvData();

    if (drvData == NULL || node == NULL) {
        HDF_LOGE("%s: Accel node pointer NULL", __func__);
        return NULL;
    }

    if (drvData->detectFlag) {
        HDF_LOGE("%s: Accel sensor have detected", __func__);
        return NULL;
    }

    if (drvData->accelCfg == NULL) {
        HDF_LOGE("%s: Accel accelCfg pointer NULL", __func__);
        return NULL;
    }

    if (GetSensorBaseConfigData(node, drvData->accelCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get sensor base config failed", __func__);
        goto BASE_CONFIG_EXIT;
    }

    if (DetectSensorDevice(drvData->accelCfg) != HDF_SUCCESS) {
        HDF_LOGI("%s: Accel sensor detect device no exist", __func__);
        drvData->detectFlag = false;
        goto BASE_CONFIG_EXIT;
    }

    drvData->detectFlag = true;
    if (InitAccelAfterDetected(drvData->accelCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Accel sensor detect device no exist", __func__);
        goto INIT_EXIT;
    }
    return drvData->accelCfg;

INIT_EXIT:
    (void)ReleaseSensorBusHandle(&drvData->accelCfg->busCfg);
BASE_CONFIG_EXIT:
    drvData->accelCfg->root = NULL;
    (void)memset_s(&drvData->accelCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&drvData->accelCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&drvData->accelCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
    return drvData->accelCfg;
}

void AccelReleaseCfgData(struct SensorCfgData *accelCfg)
{
    CHECK_NULL_PTR_RETURN(accelCfg);

    (void)DeleteSensorDevice(&accelCfg->sensorInfo);
    ReleaseSensorAllRegConfig(accelCfg);
    (void)ReleaseSensorBusHandle(&accelCfg->busCfg);
    ReleaseSensorDirectionConfig(accelCfg);

    accelCfg->root = NULL;
    (void)memset_s(&accelCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&accelCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&accelCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
}

int32_t AccelInitDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct AccelDrvData *drvData = (struct AccelDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (InitAccelData(drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init accel config failed", __func__);
        return HDF_FAILURE;
    }

    drvData->accelCfg = (struct SensorCfgData *)OsalMemCalloc(sizeof(*drvData->accelCfg));
    if (drvData->accelCfg == NULL) {
        HDF_LOGE("%s: Malloc accel config data failed", __func__);
        return HDF_FAILURE;
    }

    drvData->accelCfg->regCfgGroup = &g_regCfgGroup[0];
    drvData->cb = NULL;

    HDF_LOGI("%s: Init accel driver success", __func__);
    return HDF_SUCCESS;
}

void AccelReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct AccelDrvData *drvData = (struct AccelDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->detectFlag && drvData->accelCfg != NULL) {
        AccelReleaseCfgData(drvData->accelCfg);
    }

    OsalMemFree(drvData->accelCfg);
    drvData->accelCfg = NULL;

    HdfWorkDestroy(&drvData->accelWork);
    HdfWorkQueueDestroy(&drvData->accelWorkQueue);
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorAccelDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_ACCEL",
    .Bind = AccelBindDriver,
    .Init = AccelInitDriver,
    .Release = AccelReleaseDriver,
};

HDF_INIT(g_sensorAccelDevEntry);
