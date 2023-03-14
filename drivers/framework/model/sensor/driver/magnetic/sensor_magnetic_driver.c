/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_magnetic_driver.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_math.h"
#include "osal_mem.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    sensor_magnetic_driver_c

#define HDF_MAGNETIC_WORK_QUEUE_NAME    "hdf_magnetic_work_queue"

static struct MagneticDrvData *g_magneticDrvData = NULL;

static struct MagneticDrvData *MagneticGetDrvData(void)
{
    return g_magneticDrvData;
}

static struct SensorRegCfgGroupNode *g_regCfgGroup[SENSOR_GROUP_MAX] = { NULL };

int32_t MagneticRegisterChipOps(const struct MagneticOpsCall *ops)
{
    struct MagneticDrvData *drvData = MagneticGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(ops, HDF_ERR_INVALID_PARAM);

    drvData->ops.Init = ops->Init;
    drvData->ops.ReadData = ops->ReadData;
    return HDF_SUCCESS;
}

static void MagneticDataWorkEntry(void *arg)
{
    struct MagneticDrvData *drvData = NULL;

    drvData = (struct MagneticDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->ops.ReadData == NULL) {
        HDF_LOGE("%s: Magnetic readdata function NULL", __func__);
        return;
    }
    if (drvData->ops.ReadData(drvData->magneticCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic read data failed", __func__);
    }
}

static void MagneticTimerEntry(uintptr_t arg)
{
    int64_t interval;
    int32_t ret;
    struct MagneticDrvData *drvData = (struct MagneticDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->magneticWorkQueue, &drvData->magneticWork)) {
        HDF_LOGE("%s: Magnetic add work queue failed", __func__);
    }

    interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
    interval = (interval < SENSOR_TIMER_MIN_TIME) ? SENSOR_TIMER_MIN_TIME : interval;
    ret = OsalTimerSetTimeout(&drvData->magneticTimer, interval);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic modify time failed", __func__);
    }
}

static int32_t InitMagneticData(struct MagneticDrvData *drvData)
{
    if (HdfWorkQueueInit(&drvData->magneticWorkQueue, HDF_MAGNETIC_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->magneticWork, MagneticDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->interval = SENSOR_TIMER_MIN_TIME;
    drvData->enable = false;
    drvData->detectFlag = false;

    return HDF_SUCCESS;
}

static int32_t SetMagneticEnable(void)
{
    int32_t ret;
    struct MagneticDrvData *drvData = MagneticGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->magneticCfg, HDF_ERR_INVALID_PARAM);

    if (drvData->enable) {
        HDF_LOGE("%s: Magnetic sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->magneticCfg->busCfg, drvData->magneticCfg->regCfgGroup[SENSOR_ENABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic sensor enable config failed", __func__);
        return ret;
    }

    ret = OsalTimerCreate(&drvData->magneticTimer, SENSOR_TIMER_MIN_TIME, MagneticTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->magneticTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SetMagneticDisable(void)
{
    int32_t ret;
    struct MagneticDrvData *drvData = MagneticGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->magneticCfg, HDF_ERR_INVALID_PARAM);

    if (!drvData->enable) {
        HDF_LOGE("%s: Magnetic sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->magneticCfg->busCfg, drvData->magneticCfg->regCfgGroup[SENSOR_DISABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic sensor disable config failed", __func__);
        return ret;
    }

    ret = OsalTimerDelete(&drvData->magneticTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Magnetic delete timer failed", __func__);
        return ret;
    }
    drvData->enable = false;

    return HDF_SUCCESS;
}

static int32_t SetMagneticBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;

    struct MagneticDrvData *drvData = NULL;

    drvData = MagneticGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->interval = samplingInterval;

    return HDF_SUCCESS;
}

static int32_t SetMagneticMode(int32_t mode)
{
    if (mode <= SENSOR_WORK_MODE_DEFAULT || mode >= SENSOR_WORK_MODE_MAX) {
        HDF_LOGE("%s: The current mode is not supported", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetMagneticOption(uint32_t option)
{
    (void)option;

    return HDF_SUCCESS;
}

static int32_t DispatchMagnetic(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t MagneticBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct MagneticDrvData *drvData = (struct MagneticDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc magnetic drv data fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchMagnetic;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_magneticDrvData = drvData;

    return HDF_SUCCESS;
}

static int32_t InitMagneticOps(struct SensorCfgData *config, struct SensorDeviceInfo *deviceInfo)
{
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    deviceInfo->ops.Enable = SetMagneticEnable;
    deviceInfo->ops.Disable = SetMagneticDisable;
    deviceInfo->ops.SetBatch = SetMagneticBatch;
    deviceInfo->ops.SetMode = SetMagneticMode;
    deviceInfo->ops.SetOption = SetMagneticOption;

    if (memcpy_s(&deviceInfo->sensorInfo, sizeof(deviceInfo->sensorInfo),
        &config->sensorInfo, sizeof(config->sensorInfo)) != EOK) {
        HDF_LOGE("%s: Copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitMagneticAfterDetected(struct SensorCfgData *config)
{
    struct SensorDeviceInfo deviceInfo;
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    if (InitMagneticOps(config, &deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init magnetic ops failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add magnetic device failed", __func__);
        return HDF_FAILURE;
    }

    if (ParseSensorDirection(config) != HDF_SUCCESS) {
        HDF_LOGE("%s: Parse magnetic direction failed", __func__);
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

struct SensorCfgData *MagneticCreateCfgData(const struct DeviceResourceNode *node)
{
    struct MagneticDrvData *drvData = MagneticGetDrvData();

    if (drvData == NULL || node == NULL) {
        HDF_LOGE("%s: Magnetic node pointer NULL", __func__);
        return NULL;
    }

    if (drvData->detectFlag) {
        HDF_LOGE("%s: Magnetic sensor have detected", __func__);
        return NULL;
    }

    if (drvData->magneticCfg == NULL) {
        HDF_LOGE("%s: Magnetic magneticCfg pointer NULL", __func__);
        return NULL;
    }

    if (GetSensorBaseConfigData(node, drvData->magneticCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get sensor base config failed", __func__);
        goto BASE_CONFIG_EXIT;
    }

    if (DetectSensorDevice(drvData->magneticCfg) != HDF_SUCCESS) {
        HDF_LOGI("%s: Magnetic sensor detect device no exist", __func__);
        drvData->detectFlag = false;
        goto BASE_CONFIG_EXIT;
    }

    drvData->detectFlag = true;
    if (InitMagneticAfterDetected(drvData->magneticCfg) != HDF_SUCCESS) {
        HDF_LOGI("%s: Magnetic sensor detect device no exist", __func__);
        goto INIT_EXIT;
    }
    return drvData->magneticCfg;

INIT_EXIT:
    (void)ReleaseSensorBusHandle(&drvData->magneticCfg->busCfg);
BASE_CONFIG_EXIT:
    drvData->magneticCfg->root = NULL;
    (void)memset_s(&drvData->magneticCfg->sensorInfo,
        sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&drvData->magneticCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&drvData->magneticCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));

    return drvData->magneticCfg;
}

void MagneticReleaseCfgData(struct SensorCfgData *magneticCfg)
{
    CHECK_NULL_PTR_RETURN(magneticCfg);

    (void)DeleteSensorDevice(&magneticCfg->sensorInfo);
    ReleaseSensorAllRegConfig(magneticCfg);
    (void)ReleaseSensorBusHandle(&magneticCfg->busCfg);
    ReleaseSensorDirectionConfig(magneticCfg);

    magneticCfg->root = NULL;
    (void)memset_s(&magneticCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&magneticCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&magneticCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
}

int32_t MagneticInitDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct MagneticDrvData *drvData = (struct MagneticDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (InitMagneticData(drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init magnetic config failed", __func__);
        return HDF_FAILURE;
    }

    drvData->magneticCfg = (struct SensorCfgData *)OsalMemCalloc(sizeof(*drvData->magneticCfg));
    if (drvData->magneticCfg == NULL) {
        HDF_LOGE("%s: Malloc magnetic config data failed", __func__);
        return HDF_FAILURE;
    }

    drvData->magneticCfg->regCfgGroup = &g_regCfgGroup[0];

    HDF_LOGI("%s: Init magnetic driver success", __func__);
    return HDF_SUCCESS;
}

void MagneticReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct MagneticDrvData *drvData = (struct MagneticDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->detectFlag && drvData->magneticCfg != NULL) {
        MagneticReleaseCfgData(drvData->magneticCfg);
    }

    OsalMemFree(drvData->magneticCfg);
    drvData->magneticCfg = NULL;

    HdfWorkDestroy(&drvData->magneticWork);
    HdfWorkQueueDestroy(&drvData->magneticWorkQueue);
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorMagneticDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_MAGNETIC",
    .Bind = MagneticBindDriver,
    .Init = MagneticInitDriver,
    .Release = MagneticReleaseDriver,
};

HDF_INIT(g_sensorMagneticDevEntry);