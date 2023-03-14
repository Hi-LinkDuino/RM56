/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_pedometer_driver.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_math.h"
#include "osal_mem.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_pedometer_driver

#define HDF_PEDOMETER_WORK_QUEUE_NAME    "hdf_pedometer_work_queue"

static struct PedometerDrvData *g_pedometerDrvData = NULL;

static struct PedometerDrvData *PedometerGetDrvData(void)
{
    return g_pedometerDrvData;
}

static struct SensorRegCfgGroupNode *g_regCfgGroup[SENSOR_GROUP_MAX] = { NULL };

int32_t PedometerRegisterChipOps(const struct PedometerOpsCall *ops)
{
    struct PedometerDrvData *drvData = PedometerGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(ops, HDF_ERR_INVALID_PARAM);

    drvData->ops.Init = ops->Init;
    drvData->ops.ReadData = ops->ReadData;
    return HDF_SUCCESS;
}

static void PedometerDataWorkEntry(void *arg)
{
    struct PedometerDrvData *drvData = NULL;

    drvData = (struct PedometerDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->ops.ReadData == NULL) {
        HDF_LOGI("%s: Pedometer ReadData function NULl", __func__);
        return;
    }

    if (drvData->ops.ReadData(drvData->pedometerCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer read data failed", __func__);
        return;
    }
}

static void PedometerTimerEntry(uintptr_t arg)
{
    int64_t interval;
    int32_t ret;
    struct PedometerDrvData *drvData = (struct PedometerDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->pedometerWorkQueue, &drvData->pedometerWork)) {
        HDF_LOGE("%s: Pedometer add work queue failed", __func__);
    }

    interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
    interval = (interval < SENSOR_TIMER_MIN_TIME) ? SENSOR_TIMER_MIN_TIME : interval;
    ret = OsalTimerSetTimeout(&drvData->pedometerTimer, interval);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer modify time failed", __func__);
    }
}

static int32_t InitPedometerData(struct PedometerDrvData *drvData)
{
    if (HdfWorkQueueInit(&drvData->pedometerWorkQueue, HDF_PEDOMETER_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->pedometerWork, PedometerDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->interval = SENSOR_TIMER_MIN_TIME;
    drvData->enable = false;
    drvData->detectFlag = false;

    return HDF_SUCCESS;
}

static int32_t SetPedometerEnable(void)
{
    int32_t ret;
    struct PedometerDrvData *drvData = PedometerGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->pedometerCfg, HDF_ERR_INVALID_PARAM);

    if (drvData->enable) {
        HDF_LOGE("%s: Pedometer sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->pedometerCfg->busCfg, drvData->pedometerCfg->regCfgGroup[SENSOR_ENABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer sensor enable config failed", __func__);
        return ret;
    }

    ret = OsalTimerCreate(&drvData->pedometerTimer, SENSOR_TIMER_MIN_TIME, PedometerTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->pedometerTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SetPedometerDisable(void)
{
    int32_t ret;
    struct PedometerDrvData *drvData = PedometerGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->pedometerCfg, HDF_ERR_INVALID_PARAM);

    if (!drvData->enable) {
        HDF_LOGE("%s: Pedometer sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->pedometerCfg->busCfg,
        drvData->pedometerCfg->regCfgGroup[SENSOR_DISABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer sensor disable config failed", __func__);
        return ret;
    }

    ret = OsalTimerDelete(&drvData->pedometerTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer delete timer failed", __func__);
        return ret;
    }
    drvData->enable = false;

    return HDF_SUCCESS;
}

static int32_t SetPedometerBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;

    struct PedometerDrvData *drvData = NULL;

    drvData = PedometerGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->interval = samplingInterval;

    return HDF_SUCCESS;
}

static int32_t SetPedometerMode(int32_t mode)
{
    if (mode <= SENSOR_WORK_MODE_DEFAULT || mode >= SENSOR_WORK_MODE_MAX) {
        HDF_LOGE("%s: The current mode is not supported", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetPedometerOption(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t DispatchPedometer(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t PedometerBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct PedometerDrvData *drvData = (struct PedometerDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc pedometer drv data fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchPedometer;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_pedometerDrvData = drvData;
    return HDF_SUCCESS;
}

static int32_t InitPedometerOps(struct SensorCfgData *config, struct SensorDeviceInfo *deviceInfo)
{
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    deviceInfo->ops.Enable = SetPedometerEnable;
    deviceInfo->ops.Disable = SetPedometerDisable;
    deviceInfo->ops.SetBatch = SetPedometerBatch;
    deviceInfo->ops.SetMode = SetPedometerMode;
    deviceInfo->ops.SetOption = SetPedometerOption;

    if (memcpy_s(&deviceInfo->sensorInfo, sizeof(deviceInfo->sensorInfo),
        &config->sensorInfo, sizeof(config->sensorInfo)) != EOK) {
        HDF_LOGE("%s: Copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitPedometerAfterDetected(struct SensorCfgData *config)
{
    struct SensorDeviceInfo deviceInfo;
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    if (InitPedometerOps(config, &deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init pedometer ops failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add pedometer device failed", __func__);
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

struct SensorCfgData *PedometerCreateCfgData(const struct DeviceResourceNode *node)
{
    struct PedometerDrvData *drvData = PedometerGetDrvData();

    if (drvData == NULL || node == NULL) {
        HDF_LOGE("%s: Pedometer node pointer NULL", __func__);
        return NULL;
    }

    if (drvData->detectFlag) {
        HDF_LOGE("%s: Pedometer sensor have detected", __func__);
        return NULL;
    }

    if (drvData->pedometerCfg == NULL) {
        HDF_LOGE("%s: Pedometer pedometerCfg pointer NULL", __func__);
        return NULL;
    }

    if (GetSensorBaseConfigData(node, drvData->pedometerCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get sensor base config failed", __func__);
        goto BASE_CONFIG_EXIT;
    }

    if (DetectSensorDevice(drvData->pedometerCfg) != HDF_SUCCESS) {
        HDF_LOGI("%s: Pedometer sensor detect device no exist", __func__);
        drvData->detectFlag = false;
        goto BASE_CONFIG_EXIT;
    }

    drvData->detectFlag = true;
    if (InitPedometerAfterDetected(drvData->pedometerCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Pedometer sensor detect device no exist", __func__);
        goto INIT_EXIT;
    }
    return drvData->pedometerCfg;

INIT_EXIT:
    (void)ReleaseSensorBusHandle(&drvData->pedometerCfg->busCfg);
BASE_CONFIG_EXIT:
    drvData->pedometerCfg->root = NULL;
    (void)memset_s(&drvData->pedometerCfg->sensorInfo,
        sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&drvData->pedometerCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&drvData->pedometerCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
    return drvData->pedometerCfg;
}

void PedometerReleaseCfgData(struct SensorCfgData *pedometerCfg)
{
    CHECK_NULL_PTR_RETURN(pedometerCfg);

    (void)DeleteSensorDevice(&pedometerCfg->sensorInfo);
    ReleaseSensorAllRegConfig(pedometerCfg);
    (void)ReleaseSensorBusHandle(&pedometerCfg->busCfg);
    ReleaseSensorDirectionConfig(pedometerCfg);

    pedometerCfg->root = NULL;
    (void)memset_s(&pedometerCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&pedometerCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&pedometerCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
}

int32_t PedometerInitDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct PedometerDrvData *drvData = (struct PedometerDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (InitPedometerData(drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init pedometer config failed", __func__);
        return HDF_FAILURE;
    }

    drvData->pedometerCfg = (struct SensorCfgData *)OsalMemCalloc(sizeof(*drvData->pedometerCfg));
    if (drvData->pedometerCfg == NULL) {
        HDF_LOGE("%s: Malloc pedometer config data failed", __func__);
        return HDF_FAILURE;
    }

    drvData->pedometerCfg->regCfgGroup = &g_regCfgGroup[0];

    HDF_LOGI("%s: Init pedometer driver success", __func__);
    return HDF_SUCCESS;
}

void PedometerReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct PedometerDrvData *drvData = (struct PedometerDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->detectFlag && drvData->pedometerCfg != NULL) {
        PedometerReleaseCfgData(drvData->pedometerCfg);
    }

    OsalMemFree(drvData->pedometerCfg);
    drvData->pedometerCfg = NULL;

    HdfWorkDestroy(&drvData->pedometerWork);
    HdfWorkQueueDestroy(&drvData->pedometerWorkQueue);
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorPedometerDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_PEDOMETER",
    .Bind = PedometerBindDriver,
    .Init = PedometerInitDriver,
    .Release = PedometerReleaseDriver,
};

HDF_INIT(g_sensorPedometerDevEntry);
