/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_gravity_driver.h"
#include <securec.h>
#include "sensor_accel_driver.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_math.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_gravity_driver

#define HDF_GRAVITY_WORK_QUEUE_NAME    "hdf_gravity_work_queue"

static struct GravityDrvData *g_gravityDrvData = NULL;
static int32_t g_accelRawData[GRAVITY_AXIS_NUM];

static struct GravityDrvData *GravityGetDrvData(void)
{
    return g_gravityDrvData;
}

static int32_t GravitySubscribeAccelData(int32_t *accelData, int32_t size)
{
    CHECK_NULL_PTR_RETURN_VALUE(accelData, HDF_ERR_INVALID_PARAM);

    if (size > GRAVITY_AXIS_NUM * sizeof(int32_t)) {
        HDF_LOGE("%s: size exceed max value", __func__);
        return HDF_FAILURE;
    }

    g_accelRawData[GRAVITY_X_AXIS] = accelData[GRAVITY_X_AXIS];
    g_accelRawData[GRAVITY_Y_AXIS] = accelData[GRAVITY_Y_AXIS];
    g_accelRawData[GRAVITY_Z_AXIS] = accelData[GRAVITY_Z_AXIS];

    return HDF_SUCCESS;
}

static void GravityDataWorkEntry(void *arg)
{
    struct GravityDrvData *drvData = NULL;
    static int32_t GravityRawData[GRAVITY_AXIS_NUM];
    struct SensorReportEvent event;
    drvData = (struct GravityDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);
    OsalTimespec time;

    GravityRawData[GRAVITY_X_AXIS] = (g_accelRawData[GRAVITY_X_AXIS] * GRAVITY_UNITS +
        GRAVITY_FILTER_UNITS * (GravityRawData[GRAVITY_X_AXIS] - g_accelRawData[GRAVITY_X_AXIS])) / GRAVITY_UNITS;
    GravityRawData[GRAVITY_Y_AXIS] = (g_accelRawData[GRAVITY_Y_AXIS] * GRAVITY_UNITS +
        GRAVITY_FILTER_UNITS * (GravityRawData[GRAVITY_Y_AXIS] - g_accelRawData[GRAVITY_Y_AXIS])) / GRAVITY_UNITS;
    GravityRawData[GRAVITY_Z_AXIS] = (g_accelRawData[GRAVITY_Z_AXIS] * GRAVITY_UNITS +
        GRAVITY_FILTER_UNITS * (GravityRawData[GRAVITY_Z_AXIS] - g_accelRawData[GRAVITY_Z_AXIS])) / GRAVITY_UNITS;

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return;
    }
    event.timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT;
    event.sensorId = SENSOR_TAG_GRAVITY;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_REALTIME;
    event.dataLen = sizeof(GravityRawData);
    event.data = (uint8_t *)&GravityRawData;

    if (ReportSensorEvent(&event) != HDF_SUCCESS) {
        HDF_LOGE("%s: Report gravity data failed", __func__);
    }
}

static void GravityTimerEntry(uintptr_t arg)
{
    int32_t ret;
    struct GravityDrvData *drvData = (struct GravityDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->gravityWorkQueue, &drvData->gravityWork)) {
        HDF_LOGE("%s: Gravity add work queue failed", __func__);
    }

    ret = OsalTimerSetTimeout(&drvData->gravityTimer, drvData->interval);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gravity modify time failed", __func__);
    }
}

static int32_t InitGravityData(struct GravityDrvData *drvData)
{
    if (HdfWorkQueueInit(&drvData->gravityWorkQueue, HDF_GRAVITY_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: Gravity init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->gravityWork, GravityDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Gravity init work failed!", __func__);
        return HDF_FAILURE;
    }

    drvData->interval = GRAVITY_TIMER_MAX_TIME;
    drvData->enable = false;

    return HDF_SUCCESS;
}

static int32_t SetGravityEnable(void)
{
    int32_t ret;
    struct GravityDrvData *drvData = GravityGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->gravityCfg, HDF_ERR_INVALID_PARAM);

    if (drvData->enable) {
        HDF_LOGE("%s: Gravity sensor is enabled", __func__);
        return HDF_SUCCESS;
    }

    ret = OsalTimerCreate(&drvData->gravityTimer, GRAVITY_TIMER_MAX_TIME, GravityTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gravity create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->gravityTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gravity start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SetGravityDisable(void)
{
    int32_t ret;
    struct GravityDrvData *drvData = GravityGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->gravityCfg, HDF_ERR_INVALID_PARAM);

    if (!drvData->enable) {
        HDF_LOGE("%s: Gravity sensor had disable", __func__);
        return HDF_SUCCESS;
    }

    ret = OsalTimerDelete(&drvData->gravityTimer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Gravity delete timer failed", __func__);
        return ret;
    }
    drvData->enable = false;

    return HDF_SUCCESS;
}

static int32_t SetGravityBatch(int64_t samplingInterval, int64_t interval)
{
    (void)interval;

    int64_t ms;
    struct GravityDrvData *drvData = NULL;

    drvData = GravityGetDrvData();
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    ms = OsalDivS64(samplingInterval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));

    drvData->interval = (ms <= GRAVITY_TIMER_MIN_TIME) ? GRAVITY_TIMER_MIN_TIME : GRAVITY_TIMER_MAX_TIME;

    return HDF_SUCCESS;
}

static int32_t SetGravityMode(int32_t mode)
{
    if (mode <= SENSOR_WORK_MODE_DEFAULT || mode >= SENSOR_WORK_MODE_MAX) {
        HDF_LOGE("%s: The current mode is not supported", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SetGravityOption(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t DispatchGravity(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t GravityBindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct GravityDrvData *drvData = (struct GravityDrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc gravity drv data fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchGravity;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_gravityDrvData = drvData;
    return HDF_SUCCESS;
}

static int32_t InitGravityOps(struct SensorCfgData *config, struct SensorDeviceInfo *deviceInfo)
{
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    deviceInfo->ops.Enable = SetGravityEnable;
    deviceInfo->ops.Disable = SetGravityDisable;
    deviceInfo->ops.SetBatch = SetGravityBatch;
    deviceInfo->ops.SetMode = SetGravityMode;
    deviceInfo->ops.SetOption = SetGravityOption;

    if (memcpy_s(&deviceInfo->sensorInfo, sizeof(deviceInfo->sensorInfo),
        &config->sensorInfo, sizeof(config->sensorInfo)) != EOK) {
        HDF_LOGE("%s: Copy sensor info failed", __func__);
        return HDF_FAILURE;
    }

    if (SubscribeAccelDataCallbackFunc(GravitySubscribeAccelData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Gravity register accel  failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t InitGravityAfterBase(struct SensorCfgData *config)
{
    struct SensorDeviceInfo deviceInfo;
    CHECK_NULL_PTR_RETURN_VALUE(config, HDF_ERR_INVALID_PARAM);

    if (InitGravityOps(config, &deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init gravity ops failed", __func__);
        return HDF_FAILURE;
    }

    if (AddSensorDevice(&deviceInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: Add gravity device failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GravityCreateCfgData(const struct DeviceResourceNode *node)
{
    struct GravityDrvData *drvData = GravityGetDrvData();

    CHECK_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(drvData->gravityCfg, HDF_ERR_INVALID_PARAM);

    if (GetSensorBaseConfigData(node, drvData->gravityCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get sensor base config failed", __func__);
        return HDF_FAILURE;
    }

    if (InitGravityAfterBase(drvData->gravityCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init gravity sensor device failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t GravityInitDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct GravityDrvData *drvData = (struct GravityDrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (InitGravityData(drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: Init gravity config failed", __func__);
        return HDF_FAILURE;
    }

    drvData->gravityCfg = (struct SensorCfgData *)OsalMemCalloc(sizeof(*drvData->gravityCfg));
    if (drvData->gravityCfg == NULL) {
        HDF_LOGE("%s: Malloc gravity config data failed", __func__);
        return HDF_FAILURE;
    }

    if (GravityCreateCfgData(device->property) != HDF_SUCCESS) {
        HDF_LOGE("%s: create gravity device failed", __func__);
        OsalMemFree(drvData->gravityCfg);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: Init gravity driver success", __func__);
    return HDF_SUCCESS;
}

void GravityReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct GravityDrvData *drvData = (struct GravityDrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->gravityCfg != NULL) {
        (void)DeleteSensorDevice(&drvData->gravityCfg->sensorInfo);
        OsalMemFree(drvData->gravityCfg);
        drvData->gravityCfg = NULL;
    }

    HdfWorkDestroy(&drvData->gravityWork);
    HdfWorkQueueDestroy(&drvData->gravityWorkQueue);
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_sensorGravityDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_GRAVITY",
    .Bind = GravityBindDriver,
    .Init = GravityInitDriver,
    .Release = GravityReleaseDriver,
};

HDF_INIT(g_sensorGravityDevEntry);
