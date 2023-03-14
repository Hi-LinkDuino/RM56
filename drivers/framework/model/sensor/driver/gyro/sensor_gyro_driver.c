/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_gyro_driver.h"
#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_math.h"
#include "osal_mem.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_gyro_driver_c

#define HDF_GYRO_WORK_QUEUE_NAME    "hdf_gyro_work_queue"

static struct GyroDrvData *g_gyroDrvData = NULL;

static struct GyroDrvData *GyroGetDrvData(void)
{
    return g_gyroDrvData;
}

static struct SensorRegCfgGroupNode *g_regCfgGroup[SENSOR_GROUP_MAX] = { NULL };

int32_t GyroRegisterChipOps(const struct GyroOpsCall *ops)
{
    struct GyroDrvData *drvData = GyroGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(ops, HDF_ERR_INVALID_PARAM);

    drvData->ops.Init = ops->Init;
    drvData->ops.ReadData = ops->ReadData;
    return HDF_SUCCESS;
}

static void GyroDataWorkEntry(void *arg)
{
    struct GyroDrvData *drvData = NULL;

    drvData = (struct GyroDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->ops.ReadData == NULL) {
        HDF_LOGI("%s: Gyro ReadData function NULl", __func__);
        return;
    }
    if (drvData->ops.ReadData(drvData->gyroCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro read data failed", __func__);
    }
}

static void GyroTimerEntry(uintptr_t arg)
{
    int64_t interval;
    int32_t ret;
    struct GyroDrvData *drvData = (struct GyroDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->gyroWorkQueue, &drvData->gyroWork)) {
        HDF_LOGE("%s: Gyro add work queue failed", __func__);
    }

    interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
    interval = (interval < SENSOR_TIMER_MIN_TIME) ? SENSOR_TIMER_MIN_TIME : interval;
    ret = OsalTimerSetTimeout(&drvData->gyroTimer, interval);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro modify time failed", __func__);
    }
}

static int32_t InitGyroData(struct GyroDrvData *drvData)
{
    if (HdfWorkQueueInit(&drvData->gyroWorkQueue, HDF_GYRO_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->gyroWork, GyroDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->interval = SENSOR_TIMER_MIN_TIME;
    drvData->enable = false;
    drvData->detectFlag = false;

    return HDF_SUCCESS;
}

static int32_t SetGyroEnable(void)
{
    int32_t ret;
    struct GyroDrvData *drvData = GyroGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->gyroCfg, HDF_ERR_INVALID_PARAM);

    if (drvData->enable) {
        HDF_LOGE("%s: Gyro sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->gyroCfg->busCfg, drvData->gyroCfg->regCfgGroup[SENSOR_ENABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro sensor enable config failed", __func__);
        return ret;
    }

    ret = OsalTimerCreate(&drvData->gyroTimer, SENSOR_TIMER_MIN_TIME, GyroTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->gyroTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SetGyroDisable(void)
{
    int32_t ret;
    struct GyroDrvData *drvData = GyroGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->gyroCfg, HDF_ERR_INVALID_PARAM);

    if (!drvData->enable) {
        HDF_LOGE("%s: Gyro sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = SetSensorRegCfgArray(&drvData->gyroCfg->busCfg, drvData->gyroCfg->regCfgGroup[SENSOR_DISABLE_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro sensor disable config failed", __func__);
        return ret;
    }

    ret = OsalTimerDelete(&drvData->gyroTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro delete timer failed", __func__);
        return ret;
    }
    drvData->enable = false;

    return HDF_SUCCESS;
}

static int32_t SetGyroBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;

    struct GyroDrvData *drvData = NULL;

    drvData = GyroGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    drvData->interval = samplingInterval;

    return HDF_SUCCESS;
}

static int32_t SetGyroMode(int32_t mode)
{
    if (mode <= SENSOR_WORK_MODE_DEFAULT || mode >= SENSOR_WORK_MODE_MAX) {
        HDF_LOGE("%s: The current mode is not supported", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetGyroOption(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t DispatchGyro(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t GyroBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct GyroDrvData *drvData = (struct GyroDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc gyro drv data fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchGyro;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_gyroDrvData = drvData;
    return HDF_SUCCESS;
}

static int32_t InitGyroOps(struct SensorCfgData *config, struct SensorDeviceInfo *deviceInfo)
{
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    deviceInfo->ops.Enable = SetGyroEnable;
    deviceInfo->ops.Disable = SetGyroDisable;
    deviceInfo->ops.SetBatch = SetGyroBatch;
    deviceInfo->ops.SetMode = SetGyroMode;
    deviceInfo->ops.SetOption = SetGyroOption;

    if (memcpy_s(&deviceInfo->sensorInfo, sizeof(deviceInfo->sensorInfo),
        &config->sensorInfo, sizeof(config->sensorInfo)) != EOK) {
        HDF_LOGE("%s: Copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitGyroAfterDetected(struct SensorCfgData *config)
{
    struct SensorDeviceInfo deviceInfo;
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    if (InitGyroOps(config, &deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init gyro ops failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add gyro device failed", __func__);
        return HDF_FAILURE;
    }

    if (ParseSensorDirection(config) != HDF_SUCCESS) {
        HDF_LOGE("%s: Parse gyro direction failed", __func__);
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

struct SensorCfgData *GyroCreateCfgData(const struct DeviceResourceNode *node)
{
    struct GyroDrvData *drvData = GyroGetDrvData();

    if (drvData == NULL || node == NULL) {
        HDF_LOGE("%s: Gyro node pointer NULL", __func__);
        return NULL;
    }

    if (drvData->detectFlag) {
        HDF_LOGE("%s: Gyro sensor have detected", __func__);
        return NULL;
    }

    if (drvData->gyroCfg == NULL) {
        HDF_LOGE("%s: Gyro gyroCfg pointer NULL", __func__);
        return NULL;
    }

    if (GetSensorBaseConfigData(node, drvData->gyroCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get sensor base config failed", __func__);
        goto BASE_CONFIG_EXIT;
    }

    if (DetectSensorDevice(drvData->gyroCfg) != HDF_SUCCESS) {
        HDF_LOGI("%s: Gyro sensor detect device no exist", __func__);
        drvData->detectFlag = false;
        goto BASE_CONFIG_EXIT;
    }

    drvData->detectFlag = true;
    if (InitGyroAfterDetected(drvData->gyroCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Gyro sensor detect device no exist", __func__);
        goto INIT_EXIT;
    }
    return drvData->gyroCfg;

INIT_EXIT:
    (void)ReleaseSensorBusHandle(&drvData->gyroCfg->busCfg);
BASE_CONFIG_EXIT:
    drvData->gyroCfg->root = NULL;
    (void)memset_s(&drvData->gyroCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&drvData->gyroCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&drvData->gyroCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
    return drvData->gyroCfg;
}

void GyroReleaseCfgData(struct SensorCfgData *gyroCfg)
{
    CHECK_NULL_PTR_RETURN(gyroCfg);

    (void)DeleteSensorDevice(&gyroCfg->sensorInfo);
    ReleaseSensorAllRegConfig(gyroCfg);
    (void)ReleaseSensorBusHandle(&gyroCfg->busCfg);
    ReleaseSensorDirectionConfig(gyroCfg);

    gyroCfg->root = NULL;
    (void)memset_s(&gyroCfg->sensorInfo, sizeof(struct SensorBasicInfo), 0, sizeof(struct SensorBasicInfo));
    (void)memset_s(&gyroCfg->busCfg, sizeof(struct SensorBusCfg), 0, sizeof(struct SensorBusCfg));
    (void)memset_s(&gyroCfg->sensorAttr, sizeof(struct SensorAttr), 0, sizeof(struct SensorAttr));
}

int32_t GyroInitDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct GyroDrvData *drvData = (struct GyroDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (InitGyroData(drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init gyro config failed", __func__);
        return HDF_FAILURE;
    }

    drvData->gyroCfg = (struct SensorCfgData *)OsalMemCalloc(sizeof(*drvData->gyroCfg));
    if (drvData->gyroCfg == NULL) {
        HDF_LOGE("%s: Malloc gyro config data failed", __func__);
        return HDF_FAILURE;
    }

    drvData->gyroCfg->regCfgGroup = &g_regCfgGroup[0];

    HDF_LOGI("%s: Init gyro driver success", __func__);
    return HDF_SUCCESS;
}

void GyroReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct GyroDrvData *drvData = (struct GyroDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->detectFlag && drvData->gyroCfg != NULL) {
        GyroReleaseCfgData(drvData->gyroCfg);
    }

    OsalMemFree(drvData->gyroCfg);
    drvData->gyroCfg = NULL;

    HdfWorkDestroy(&drvData->gyroWork);
    HdfWorkQueueDestroy(&drvData->gyroWorkQueue);
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorGyroDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_GYRO",
    .Bind = GyroBindDriver,
    .Init = GyroInitDriver,
    .Release = GyroReleaseDriver,
};

HDF_INIT(g_sensorGyroDevEntry);
