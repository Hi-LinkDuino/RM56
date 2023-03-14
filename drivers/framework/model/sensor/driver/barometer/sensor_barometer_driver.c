/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_barometer_driver.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_math.h"
#include "osal_mem.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_barometer_driver

#define HDF_BAROMETER_WORK_QUEUE_NAME    "hdf_barometer_work_queue"

static struct BarometerDrvData *g_barometerDrvData = NULL;

static struct BarometerDrvData *BarometerGetDrvData(void)
{
    return g_barometerDrvData;
}

static struct SensorRegCfgGroupNode *g_regCfgGroup[SENSOR_GROUP_MAX] = { NULL };

int32_t BarometerRegisterChipOps(const struct BarometerOpsCall *ops)
{
    struct BarometerDrvData *drvData = BarometerGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(ops, HDF_ERR_INVALID_PARAM);

    drvData->ops.Init = ops->Init;
    drvData->ops.ReadData = ops->ReadData;
    return HDF_SUCCESS;
}

static void BarometerDataWorkEntry(void *arg)
{
    struct BarometerDrvData *drvData = NULL;

    drvData = (struct BarometerDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->ops.ReadData == NULL) {
        HDF_LOGI("%s: Barometer ReadData function NULl", __func__);
        return;
    }
    if (drvData->ops.ReadData(drvData->barometerCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Barometer read data failed", __func__);
    }
}

static void BarometerTimerEntry(uintptr_t arg)
{
    int64_t interval;
    int32_t ret;
    struct BarometerDrvData *drvData = (struct BarometerDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->barometerWorkQueue, &drvData->barometerWork)) {
        HDF_LOGE("%s: barometer add work queue failed", __func__);
    }

    interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
    interval = (interval < SENSOR_TIMER_MIN_TIME) ? SENSOR_TIMER_MIN_TIME : interval;
    ret = OsalTimerSetTimeout(&drvData->barometerTimer, interval);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer modify time failed", __func__);
    }
}

static int32_t InitBarometerData(struct BarometerDrvData *drvData)
{
    if (HdfWorkQueueInit(&drvData->barometerWorkQueue, HDF_BAROMETER_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->barometerWork, BarometerDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->interval = SENSOR_TIMER_MIN_TIME;
    drvData->enable = false;
    drvData->detectFlag = false;

    return HDF_SUCCESS;
}

static int32_t SetBarometerEnable(void)
{
    int32_t ret;
    struct BarometerDrvData *drvData = BarometerGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->barometerCfg, HDF_ERR_INVALID_PARAM);

    if (drvData->enable) {
        HDF_LOGE("%s: barometer sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->barometerCfg->busCfg, drvData->barometerCfg->regCfgGroup[SENSOR_ENABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer sensor enable config failed", __func__);
        return ret;
    }

    ret = OsalTimerCreate(&drvData->barometerTimer, SENSOR_TIMER_MIN_TIME, BarometerTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->barometerTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SetBarometerDisable(void)
{
    int32_t ret;
    struct BarometerDrvData *drvData = BarometerGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->barometerCfg, HDF_ERR_INVALID_PARAM);

    if (!drvData->enable) {
        HDF_LOGE("%s: barometer sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->barometerCfg->busCfg,
        drvData->barometerCfg->regCfgGroup[SENSOR_DISABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer sensor disable config failed", __func__);
        return ret;
    }

    ret = OsalTimerDelete(&drvData->barometerTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: barometer delete timer failed", __func__);
        return ret;
    }
    drvData->enable = false;
    return HDF_SUCCESS;
}

static int32_t SetBarometerBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;

    struct BarometerDrvData *drvData = NULL;

    drvData = BarometerGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->interval = samplingInterval;

    return HDF_SUCCESS;
}

static int32_t SetBarometerMode(int32_t mode)
{
    if (mode <= SENSOR_WORK_MODE_DEFAULT || mode >= SENSOR_WORK_MODE_MAX) {
        HDF_LOGE("%s: The current mode is not supported", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetBarometerOption(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t DispatchBarometer(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t BarometerBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct BarometerDrvData *drvData = (struct BarometerDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: malloc barometer drv data fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchBarometer;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_barometerDrvData = drvData;
    return HDF_SUCCESS;
}

static int32_t InitBarometerOps(struct SensorCfgData *config, struct SensorDeviceInfo *deviceInfo)
{
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    deviceInfo->ops.Enable = SetBarometerEnable;
    deviceInfo->ops.Disable = SetBarometerDisable;
    deviceInfo->ops.SetBatch = SetBarometerBatch;
    deviceInfo->ops.SetMode = SetBarometerMode;
    deviceInfo->ops.SetOption = SetBarometerOption;

    if (memcpy_s(&deviceInfo->sensorInfo, sizeof(deviceInfo->sensorInfo),
        &config->sensorInfo, sizeof(config->sensorInfo)) != EOK) {
        HDF_LOGE("%s: copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitBarometerAfterDetected(struct SensorCfgData *config)
{
    struct SensorDeviceInfo deviceInfo;
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    if (InitBarometerOps(config, &deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init barometer ops failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add barometer device failed", __func__);
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

struct SensorCfgData *BarometerCreateCfgData(const struct DeviceResourceNode *node)
{
    struct BarometerDrvData *drvData = BarometerGetDrvData();

    if (drvData == NULL || node == NULL) {
        HDF_LOGE("%s: Barometer node pointer NULL", __func__);
        return NULL;
    }

    if (drvData->detectFlag) {
        HDF_LOGE("%s: Barometer sensor have detected", __func__);
        return NULL;
    }

    if (drvData->barometerCfg == NULL) {
        HDF_LOGE("%s: Barometer barometerCfg pointer NULL", __func__);
        return NULL;
    }

    if (GetSensorBaseConfigData(node, drvData->barometerCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get sensor base config failed", __func__);
        goto BASE_CONFIG_EXIT;
    }

    if (DetectSensorDevice(drvData->barometerCfg) != HDF_SUCCESS) {
        HDF_LOGI("%s: Barometer sensor detect device no exist", __func__);
        drvData->detectFlag = false;
        goto BASE_CONFIG_EXIT;
    }

    drvData->detectFlag = true;
    if (InitBarometerAfterDetected(drvData->barometerCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Barometer sensor detect device no exist", __func__);
        goto INIT_EXIT;
    }
    return drvData->barometerCfg;

INIT_EXIT:
    (void)ReleaseSensorBusHandle(&drvData->barometerCfg->busCfg);
BASE_CONFIG_EXIT:
    drvData->barometerCfg->root = NULL;
    (void)memset_s(&drvData->barometerCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0,
        sizeof(struct SensorBasicInfo));
    (void)memset_s(&drvData->barometerCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&drvData->barometerCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
    return drvData->barometerCfg;
}

void BarometerReleaseCfgData(struct SensorCfgData *barometerCfg)
{
    CHECK_NULL_PTR_RETURN(barometerCfg);

    (void)DeleteSensorDevice(&barometerCfg->sensorInfo);
    ReleaseSensorAllRegConfig(barometerCfg);
    (void)ReleaseSensorBusHandle(&barometerCfg->busCfg);

    barometerCfg->root = NULL;
    (void)memset_s(&barometerCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&barometerCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&barometerCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
}

int32_t BarometerInitDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct BarometerDrvData *drvData = (struct BarometerDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (InitBarometerData(drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init barometer config failed", __func__);
        return HDF_FAILURE;
    }

    drvData->barometerCfg = (struct SensorCfgData *)OsalMemCalloc(sizeof(*drvData->barometerCfg));
    if (drvData->barometerCfg == NULL) {
        HDF_LOGE("%s: Malloc barometer config data failed", __func__);
        return HDF_FAILURE;
    }

    drvData->barometerCfg->regCfgGroup = &g_regCfgGroup[0];

    HDF_LOGI("%s: Init barometer driver success", __func__);
    return HDF_SUCCESS;
}

void BarometerReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct BarometerDrvData *drvData = (struct BarometerDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->detectFlag && drvData->barometerCfg != NULL) {
        BarometerReleaseCfgData(drvData->barometerCfg);
    }

    OsalMemFree(drvData->barometerCfg);
    drvData->barometerCfg = NULL;

    HdfWorkDestroy(&drvData->barometerWork);
    HdfWorkQueueDestroy(&drvData->barometerWorkQueue);
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorBarometerDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_BAROMETER",
    .Bind = BarometerBindDriver,
    .Init = BarometerInitDriver,
    .Release = BarometerReleaseDriver,
};

HDF_INIT(g_sensorBarometerDevEntry);