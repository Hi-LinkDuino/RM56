/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_sensor_test.h"
#include "osal_math.h"
#include "osal_time.h"
#include "sensor_platform_if.h"
#include "sensor_device_manager.h"
#include "sensor_device_type.h"

#define HDF_LOG_TAG    hdf_sensor_test_c

#define HDF_SENSOR_TEST_VALUE    1024000000 // 1g = 9.8m/s^2
#define SENSOR_TEST_MAX_RANGE    8
#define SENSOR_TEST_MAX_POWER    230
#define HDF_SENSOR_TEST_WORK_QUEUE_NAME    "hdf_sensor_test_work_queue"

static struct SensorTestDrvData *GetSensorTestDrvData(void)
{
    static struct SensorTestDrvData sensorTestDrvData = {
        .enable = false,
        .initStatus = false,
        .interval = SENSOR_TEST_SAMPLING_200_MS,
    };

    return &sensorTestDrvData;
}
static void SensorTestDataWorkEntry(void *arg)
{
    int32_t value = HDF_SENSOR_TEST_VALUE;
    struct SensorReportEvent event;
    OsalTimespec time;

    (void)memset_s(&event, sizeof(event), 0, sizeof(event));
    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)OsalGetTime(&time);

    event.timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT; // nanosecond
    event.sensorId = SENSOR_TAG_NONE;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_REALTIME;
    event.dataLen = sizeof(value);
    event.data = (uint8_t *)&value;
    ReportSensorEvent(&event);
}

static void SensorTestTimerEntry(uintptr_t arg)
{
    int64_t interval;
    struct SensorTestDrvData *drvData = (struct SensorTestDrvData *)arg;
    CHECK_NULL_PTR_RETURN(drvData);

    if (!HdfAddWork(&drvData->workQueue, &drvData->work)) {
        HDF_LOGE("%s: sensor test add work queue failed", __func__);
    }

    interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
    interval = (interval < SENSOR_TIMER_MIN_TIME) ? SENSOR_TIMER_MIN_TIME : interval;

    if (OsalTimerSetTimeout(&drvData->timer, interval) != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test modify time failed", __func__);
    }
}

static int32_t SensorInitTestConfig(void)
{
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    if (HdfWorkQueueInit(&drvData->workQueue, HDF_SENSOR_TEST_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test init work queue failed", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->work, SensorTestDataWorkEntry, drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test create thread failed", __func__);
        return HDF_FAILURE;
    }

    drvData->enable = false;
    drvData->initStatus = true;

    return HDF_SUCCESS;
}

static int32_t SensorEnableTest(void)
{
    int32_t ret;
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    if (drvData->enable) {
        HDF_LOGE("%s: sensor test had enable", __func__);
        return HDF_SUCCESS;
    }

    ret = OsalTimerCreate(&drvData->timer, SENSOR_TIMER_MIN_TIME, SensorTestTimerEntry, (uintptr_t)drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test create timer failed[%d]", __func__, ret);
        return ret;
    }

    ret = OsalTimerStartLoop(&drvData->timer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test start timer failed[%d]", __func__, ret);
        return ret;
    }
    drvData->enable = true;

    return HDF_SUCCESS;
}

static int32_t SensorDisableTest(void)
{
    int32_t ret;
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    if (!drvData->enable) {
        HDF_LOGE("%s: sensor test had disable", __func__);
        return HDF_SUCCESS;
    }

    if (drvData->timer.realTimer != NULL) {
        ret = OsalTimerDelete(&drvData->timer);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: sensor test delete timer failed", __func__);
            return ret;
        }
    }

    drvData->enable = false;

    return HDF_SUCCESS;
}

static int32_t SensorSetBatchTest(int64_t samplingInterval, int64_t interval)
{
    struct SensorTestDrvData *drvData = NULL;
    (void)interval;

    drvData = GetSensorTestDrvData();

    drvData->interval = samplingInterval;
    return HDF_SUCCESS;
}

static int32_t SensorSetModeTest(int32_t mode)
{
    return (mode == SENSOR_WORK_MODE_REALTIME) ? HDF_SUCCESS : HDF_FAILURE;
}

static int32_t SensorSetOptionTest(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t SensorTestDispatch(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t BindSensorDriverTest(struct HdfDeviceObject *device)
{
    static struct IDeviceIoService service = {
        .object = {0},
        .Dispatch = SensorTestDispatch,
    };

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    device->service = &service;
    return HDF_SUCCESS;
}

int32_t InitSensorDriverTest(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct SensorDeviceInfo deviceInfo = {
        .sensorInfo = {
            .sensorName = "sensor_test",
            .vendorName = "default",
            .firmwareVersion = "1.0",
            .hardwareVersion = "1.0",
            .sensorTypeId = SENSOR_TAG_NONE,
            .sensorId = SENSOR_TAG_NONE,
            .maxRange = SENSOR_TEST_MAX_RANGE,
            .accuracy = 1,
            .power = SENSOR_TEST_MAX_POWER,
        },
        .ops = {
            .Enable = SensorEnableTest,
            .Disable = SensorDisableTest,
            .SetBatch = SensorSetBatchTest,
            .SetMode = SensorSetModeTest,
            .SetOption = SensorSetOptionTest,
        },
    };

    (void)device;

    ret = SensorInitTestConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test config failed", __func__);
        return ret;
    }

    ret = AddSensorDevice(&deviceInfo);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test register failed", __func__);
        return ret;
    }

    HDF_LOGI("%s: init sensor test driver success", __func__);
    return HDF_SUCCESS;
}

void ReleaseSensorDriverTest(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct SensorTestDrvData *drvData = NULL;
    struct SensorDeviceInfo deviceInfo = {
        .sensorInfo = {
            .sensorName = "sensor_test",
            .vendorName = "default",
            .firmwareVersion = "1.0",
            .hardwareVersion = "1.0",
            .sensorTypeId = SENSOR_TAG_NONE,
            .sensorId = SENSOR_TAG_NONE,
            .maxRange = SENSOR_TEST_MAX_RANGE,
            .accuracy = 1,
            .power = SENSOR_TEST_MAX_POWER,
        }
    };
    drvData = GetSensorTestDrvData();
    (void)device;
    (void)DeleteSensorDevice(&deviceInfo.sensorInfo);

    if (drvData->timer.realTimer != NULL) {
        ret = OsalTimerDelete(&drvData->timer);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: sensor test delete timer failed", __func__);
        }
    }
}

struct HdfDriverEntry g_sensorTestDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_TEST_DRIVER",
    .Bind = BindSensorDriverTest,
    .Init = InitSensorDriverTest,
    .Release = ReleaseSensorDriverTest,
};

HDF_INIT(g_sensorTestDevEntry);
