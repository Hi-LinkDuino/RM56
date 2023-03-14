/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_device_manager.h"
#include <securec.h>
#include "asm/io.h"
#include "osal_mem.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_commom

#define HDF_SENSOR_INFO_MAX_BUF (4 * 1024) // 4kB for all sensor info
#define HDF_SENSOR_EVENT_MAX_BUF (4 * 1024) // 4kB

struct SensorDevMgrData *g_sensorDeviceManager = NULL;

static struct SensorDevMgrData *GetSensorDeviceManager(void)
{
    return g_sensorDeviceManager;
}

int32_t AddSensorDevice(const struct SensorDeviceInfo *deviceInfo)
{
    struct SensorDevInfoNode *pos = NULL;
    struct SensorDevInfoNode *devInfoNode = NULL;
    struct SensorDevMgrData *manager = GetSensorDeviceManager();

    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_ERR_INVALID_PARAM);

    DLIST_FOR_EACH_ENTRY(pos, &manager->sensorDevInfoHead, struct SensorDevInfoNode, node) {
        if ((deviceInfo->sensorInfo.sensorId == pos->devInfo.sensorInfo.sensorId) &&
            (strcmp(deviceInfo->sensorInfo.sensorName, pos->devInfo.sensorInfo.sensorName) == 0)) {
            HDF_LOGE("%s: sensor id[%d] had existed", __func__, deviceInfo->sensorInfo.sensorId);
            return HDF_FAILURE;
        }
    }

    (void)OsalMutexLock(&manager->mutex);
    devInfoNode = (struct SensorDevInfoNode*)OsalMemCalloc(sizeof(*devInfoNode));
    if (devInfoNode == NULL) {
        (void)OsalMutexUnlock(&manager->mutex);
        return HDF_FAILURE;
    }
    if (memcpy_s(&devInfoNode->devInfo, sizeof(devInfoNode->devInfo),
        (void *)deviceInfo, sizeof(*deviceInfo)) != EOK) {
        HDF_LOGE("%s: copy sensor info failed", __func__);
        OsalMemFree(devInfoNode);
        (void)OsalMutexUnlock(&manager->mutex);
        return HDF_FAILURE;
    }
    DListInsertTail(&devInfoNode->node, &manager->sensorDevInfoHead);
    (void)OsalMutexUnlock(&manager->mutex);
    HDF_LOGI("%s: register sensor name[%s] success", __func__, deviceInfo->sensorInfo.sensorName);

    return HDF_SUCCESS;
}

int32_t DeleteSensorDevice(const struct SensorBasicInfo *sensorBaseInfo)
{
    struct SensorDevInfoNode *pos = NULL;
    struct SensorDevInfoNode *tmp = NULL;
    struct SensorDevMgrData *manager = GetSensorDeviceManager();

    CHECK_NULL_PTR_RETURN_VALUE(sensorBaseInfo, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_ERR_INVALID_PARAM);

    (void)OsalMutexLock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->sensorDevInfoHead, struct SensorDevInfoNode, node) {
        if ((sensorBaseInfo->sensorId == pos->devInfo.sensorInfo.sensorId) &&
            (strcmp(sensorBaseInfo->sensorName, pos->devInfo.sensorInfo.sensorName) == 0)) {
            DListRemove(&pos->node);
            OsalMemFree(pos);
            (void)OsalMutexUnlock(&manager->mutex);
            return HDF_SUCCESS;
        }
    }
    (void)OsalMutexUnlock(&manager->mutex);
    HDF_LOGE("%s: delete sensor id invalid para", __func__);
    return HDF_FAILURE;
}

int32_t ReportSensorEvent(const struct SensorReportEvent *events)
{
    int32_t ret;
    struct SensorDevMgrData *manager = NULL;
    struct HdfSBuf *msg = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(events, HDF_ERR_INVALID_PARAM);

    manager = GetSensorDeviceManager();
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_ERR_INVALID_PARAM);

    (void)OsalMutexLock(&manager->eventMutex);
    msg = HdfSbufObtain(HDF_SENSOR_EVENT_MAX_BUF);
    if (msg == NULL) {
        (void)OsalMutexUnlock(&manager->eventMutex);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteBuffer(msg, events, sizeof(*events))) {
        HDF_LOGE("%s: sbuf write event failed", __func__);
        ret = HDF_FAILURE;
        goto EXIT;
    }

    if (!HdfSbufWriteBuffer(msg, events->data, events->dataLen)) {
        HDF_LOGE("%s: sbuf write event data failed", __func__);
        ret = HDF_FAILURE;
        goto EXIT;
    }

    if (HdfDeviceSendEvent(manager->device, 0, msg) != HDF_SUCCESS) {
        HDF_LOGE("%s: send sensor data event failed", __func__);
        ret = HDF_FAILURE;
        goto EXIT;
    }
    ret = HDF_SUCCESS;

EXIT:
    HdfSbufRecycle(msg);
    (void)OsalMutexUnlock(&manager->eventMutex);
    return ret;
}

static int32_t GetAllSensorInfo(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t count = 0;
    struct SensorDevInfoNode *pos = NULL;
    struct SensorBasicInfo *sensorInfo = NULL;
    struct SensorDevMgrData *manager = NULL;

    (void)data;

    manager = GetSensorDeviceManager();

    CHECK_NULL_PTR_RETURN_VALUE(reply, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_ERR_INVALID_PARAM);

    DLIST_FOR_EACH_ENTRY(pos, &manager->sensorDevInfoHead, struct SensorDevInfoNode, node) {
        sensorInfo = &(pos->devInfo.sensorInfo);
        if (!HdfSbufWriteBuffer(reply, sensorInfo, sizeof(*sensorInfo))) {
            HDF_LOGE("%s: write sbuf failed", __func__);
            return HDF_FAILURE;
        }

        count++;
        if ((count + 1) * sizeof(*sensorInfo) > HDF_SENSOR_INFO_MAX_BUF) {
            HDF_LOGE("%s: write sbuf exceed max buf, sensor count[%d]", __func__, count);
            break;
        }
    }

    return HDF_SUCCESS;
}

static int32_t Enable(struct SensorDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;
    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo->ops.Enable, HDF_ERR_INVALID_PARAM);
    return deviceInfo->ops.Enable();
}

static int32_t Disable(struct SensorDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;
    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo->ops.Disable, HDF_ERR_INVALID_PARAM);

    return deviceInfo->ops.Disable();
}

static int32_t SetBatch(struct SensorDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int64_t samplingInterval;
    int64_t reportInterval;
    (void)reply;

    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo->ops.SetBatch, HDF_ERR_INVALID_PARAM);

    if (!HdfSbufReadInt64(data, &samplingInterval) || !HdfSbufReadInt64(data, &reportInterval)) {
        HDF_LOGE("%s: sbuf read interval failed", __func__);
        return HDF_FAILURE;
    }

    return deviceInfo->ops.SetBatch(samplingInterval, reportInterval);
}

static int32_t SetMode(struct SensorDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t mode;
    (void)reply;

    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo->ops.SetMode, HDF_ERR_INVALID_PARAM);

    if (!HdfSbufReadInt32(data, &mode)) {
        HDF_LOGE("%s: sbuf read mode failed", __func__);
        return HDF_FAILURE;
    }

    return deviceInfo->ops.SetMode(mode);
}

static int32_t SetOption(struct SensorDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t option;
    (void)reply;

    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(deviceInfo->ops.SetOption, HDF_ERR_INVALID_PARAM);

    if (!HdfSbufReadUint32(data, &option)) {
        HDF_LOGE("%s: sbuf read option failed", __func__);
        return HDF_FAILURE;
    }

    return deviceInfo->ops.SetOption(option);
}

static struct SensorCmdHandleList g_sensorCmdHandle[] = {
    {SENSOR_OPS_CMD_ENABLE, Enable},        // SENSOR_CMD_ENABLE
    {SENSOR_OPS_CMD_DISABLE, Disable},      // SENSOR_CMD_DISABLE
    {SENSOR_OPS_CMD_SET_BATCH, SetBatch},   // SENSOR_CMD_SET_BATCH
    {SENSOR_OPS_CMD_SET_MODE, SetMode},     // SENSOR_CMD_SET_MODE
    {SENSOR_OPS_CMD_SET_OPTION, SetOption}, // SENSOR_CMD_SET_OPTION
};

static int32_t DispatchCmdHandle(struct SensorDeviceInfo *deviceInfo, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t opsCmd;
    int32_t loop;
    int32_t count;

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    if (!HdfSbufReadInt32(data, &opsCmd)) {
        HDF_LOGE("%s: sbuf read opsCmd failed", __func__);
        return HDF_FAILURE;
    }

    if ((opsCmd >= SENSOR_OPS_CMD_BUTT) || (opsCmd < SENSOR_OPS_CMD_ENABLE)) {
        HDF_LOGE("%s: invalid cmd = %d", __func__, opsCmd);
        return HDF_FAILURE;
    }

    count = sizeof(g_sensorCmdHandle) / sizeof(g_sensorCmdHandle[0]);
    for (loop = 0; loop < count; ++loop) {
        if ((opsCmd == g_sensorCmdHandle[loop].cmd) && (g_sensorCmdHandle[loop].func != NULL)) {
            return g_sensorCmdHandle[loop].func(deviceInfo, data, reply);
        }
    }

    return HDF_FAILURE;
}

static int32_t DispatchSensor(struct HdfDeviceIoClient *client,
    int32_t cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct SensorDevMgrData *manager = GetSensorDeviceManager();
    struct SensorDevInfoNode *pos = NULL;
    int32_t sensorId;
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(client, HDF_ERR_INVALID_PARAM);

    if (cmd >= SENSOR_CMD_END) {
        HDF_LOGE("%s: sensor cmd invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cmd == SENSOR_CMD_GET_INFO_LIST) {
        return GetAllSensorInfo(data, reply);
    }

    (void)OsalMutexLock(&manager->mutex);
    if (!HdfSbufReadInt32(data, &sensorId)) {
        HDF_LOGE("%s: sbuf read sensorId failed", __func__);
        (void)OsalMutexUnlock(&manager->mutex);
        return HDF_ERR_INVALID_PARAM;
    }

    DLIST_FOR_EACH_ENTRY(pos, &manager->sensorDevInfoHead, struct SensorDevInfoNode, node) {
        if (sensorId == pos->devInfo.sensorInfo.sensorId) {
            ret = DispatchCmdHandle(&pos->devInfo, data, reply);
            (void)OsalMutexUnlock(&manager->mutex);
            return ret;
        }
    }
    (void)OsalMutexUnlock(&manager->mutex);

    HDF_LOGE("%s: not find sensor[%d] handle function", __func__, sensorId);
    return HDF_FAILURE;
}

int32_t BindSensorDevManager(struct HdfDeviceObject *device)
{
    struct SensorDevMgrData *manager = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    manager = (struct SensorDevMgrData *)OsalMemCalloc(sizeof(*manager));
    if (manager == NULL) {
        HDF_LOGE("%s: malloc manager fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    manager->ioService.Dispatch = DispatchSensor;
    manager->device = device;
    device->service = &manager->ioService;
    g_sensorDeviceManager = manager;

    return HDF_SUCCESS;
}

int32_t InitSensorDevManager(struct HdfDeviceObject *device)
{
    struct SensorDevMgrData *manager = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    manager = (struct SensorDevMgrData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(manager, HDF_ERR_INVALID_PARAM);

    DListHeadInit(&manager->sensorDevInfoHead);
    if (OsalMutexInit(&manager->mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: init mutex failed", __func__);
        return HDF_FAILURE;
    }

    if (OsalMutexInit(&manager->eventMutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: init eventMutex failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfDeviceSetClass(device, DEVICE_CLASS_SENSOR)) {
        HDF_LOGE("%s: init sensor set class failed", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: init sensor manager successfully", __func__);
    return HDF_SUCCESS;
}

void ReleaseSensorDevManager(struct HdfDeviceObject *device)
{
    struct SensorDevInfoNode *pos = NULL;
    struct SensorDevInfoNode *tmp = NULL;
    struct SensorDevMgrData *manager = NULL;

    CHECK_NULL_PTR_RETURN(device);

    manager = (struct SensorDevMgrData *)device->service;
    CHECK_NULL_PTR_RETURN(manager);

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &manager->sensorDevInfoHead, struct SensorDevInfoNode, node) {
        DListRemove(&pos->node);
        OsalMemFree(pos);
    }

    OsalMutexDestroy(&manager->mutex);
    OsalMutexDestroy(&manager->eventMutex);
    OsalMemFree(manager);
    g_sensorDeviceManager = NULL;
}

struct HdfDriverEntry g_sensorDevManagerEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_MGR_AP",
    .Bind = BindSensorDevManager,
    .Init = InitSensorDevManager,
    .Release = ReleaseSensorDevManager,
};

HDF_INIT(g_sensorDevManagerEntry);
