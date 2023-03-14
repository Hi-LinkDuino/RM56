/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "light_driver.h"
#include <securec.h>
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "osal_mem.h"
#include "osal_mutex.h"

#define HDF_LOG_TAG    hdf_light_driver

#define LIGHT_WORK_QUEUE_NAME    "light_queue"

struct LightDriverData *g_lightDrvData = NULL;

static struct LightDriverData *GetLightDrvData(void)
{
    return g_lightDrvData;
}

static int32_t GetAllLightInfo(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    uint32_t i;
    struct LightInfo lightInfo;
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(reply, HDF_ERR_INVALID_PARAM);

    if (!HdfSbufWriteUint32(reply, drvData->lightNum)) {
        HDF_LOGE("%s: write sbuf failed", __func__);
        return HDF_FAILURE;
    }

    for (i = 0; i < LIGHT_TYPE_BUTT; ++i) {
        if (drvData->info[i] == NULL) {
            continue;
        }
        lightInfo.lightType = i;
        lightInfo.reserved = 0;

        if (!HdfSbufWriteBuffer(reply, &lightInfo, sizeof(lightInfo))) {
            HDF_LOGE("%s: write sbuf failed", __func__);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

int32_t StartLight(uint32_t lightType)
{
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (GpioWrite(drvData->info[lightType]->busNum, GPIO_VAL_HIGH) != HDF_SUCCESS) {
        HDF_LOGE("%s: pull gpio%d to %d level failed", __func__, drvData->info[lightType]->busNum, GPIO_VAL_LOW);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StopLight(uint32_t lightType)
{
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (GpioWrite(drvData->info[lightType]->busNum, GPIO_VAL_LOW) != HDF_SUCCESS) {
        HDF_LOGE("%s: pull gpio%d to %d level failed", __func__, drvData->info[lightType]->busNum, GPIO_VAL_LOW);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void LightTimerEntry(uintptr_t para)
{
    uint32_t duration;
    uint32_t lightType;
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null", __func__);
        return;
    }

    lightType = (uint32_t)para;
    drvData->lightType = lightType;

    if (drvData->info[lightType]->lightState == LIGHT_STATE_START) {
        duration = drvData->info[lightType]->offTime;
    }
    if (drvData->info[lightType]->lightState == LIGHT_STATE_STOP) {
        duration = drvData->info[lightType]->onTime;
    }

    HdfAddWork(&drvData->workQueue, &drvData->work);

    if ((OsalTimerSetTimeout(&drvData->timer, duration) == HDF_SUCCESS)) {
        return;
    }

    if (drvData->timer.realTimer != NULL) {
        if (OsalTimerDelete(&drvData->timer) != HDF_SUCCESS) {
            HDF_LOGE("%s: delete light timer fail!", __func__);
        }
    }

    return;
}

static int32_t Enable(uint32_t lightType, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    uint32_t len;
    struct LightEffect *buf = NULL;
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (drvData->info[lightType] == NULL) {
        HDF_LOGE("%s: light type info is null", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadBuffer(data, (const void **)&buf, &len)) {
        HDF_LOGE("%s: light read data failed", __func__);
        return HDF_FAILURE;
    }

    drvData->info[lightType]->lightBrightness = (buf->lightBrightness == 0) ?
        drvData->info[lightType]->lightBrightness : buf->lightBrightness;

    if ((drvData->info[lightType]->lightBrightness & LIGHT_MAKE_R_BIT) != 0) {
        drvData->info[lightType]->busNum = drvData->info[lightType]->busRNum;
    } else if ((drvData->info[lightType]->lightBrightness & LIGHT_MAKE_G_BIT) != 0) {
        drvData->info[lightType]->busNum = drvData->info[lightType]->busGNum;
    } else if ((drvData->info[lightType]->lightBrightness & LIGHT_MAKE_B_BIT) != 0) {
        drvData->info[lightType]->busNum = drvData->info[lightType]->busBNum;
    }

    if (buf->flashEffect.flashMode == LIGHT_FLASH_NONE) {
        if (GpioWrite(drvData->info[lightType]->busNum, GPIO_VAL_HIGH) != HDF_SUCCESS) {
            HDF_LOGE("%s: gpio busNum %d write failed", __func__, drvData->info[lightType]->busNum);
            return HDF_FAILURE;
        }
    }

    if (buf->flashEffect.flashMode == LIGHT_FLASH_TIMED) {
        drvData->info[lightType]->onTime = (buf->flashEffect.onTime < drvData->info[lightType]->onTime) ?
        drvData->info[lightType]->onTime : buf->flashEffect.onTime;
        drvData->info[lightType]->offTime = (buf->flashEffect.offTime < drvData->info[lightType]->offTime) ?
        drvData->info[lightType]->offTime : buf->flashEffect.offTime;

        if (OsalTimerCreate(&drvData->timer, LIGHT_WAIT_TIME, LightTimerEntry, (uintptr_t)lightType) != HDF_SUCCESS) {
            HDF_LOGE("%s: create light timer fail!", __func__);
            return HDF_FAILURE;
        }

        if (OsalTimerStartLoop(&drvData->timer) != HDF_SUCCESS) {
            HDF_LOGE("%s: start light timer fail!", __func__);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

static int32_t Disable(uint32_t lightType, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)data;
    (void)reply;
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    if (drvData->info[lightType] == NULL) {
        HDF_LOGE("%s: light type info is null", __func__);
        return HDF_FAILURE;
    }

    if (drvData->timer.realTimer != NULL) {
        if (OsalTimerDelete(&drvData->timer) != HDF_SUCCESS) {
            HDF_LOGE("%s: delete light timer fail!", __func__);
            return HDF_FAILURE;
        }
    }

    if (GpioWrite(drvData->info[lightType]->busRNum, GPIO_VAL_LOW) != HDF_SUCCESS) {
        HDF_LOGE("%s: gpio write failed", __func__);
        return HDF_FAILURE;
    }

    drvData->info[lightType]->lightState = LIGHT_STATE_STOP;

    return HDF_SUCCESS;
}

static struct LightCmdHandleList g_lightCmdHandle[] = {
    {LIGHT_OPS_IO_CMD_ENABLE, Enable},
    {LIGHT_OPS_IO_CMD_DISABLE, Disable},
};

static int32_t DispatchCmdHandle(uint32_t lightType, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t opsCmd;
    int32_t loop;
    int32_t count;

    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    if (!HdfSbufReadInt32(data, &opsCmd)) {
        HDF_LOGE("%s: sbuf read opsCmd failed", __func__);
        return HDF_FAILURE;
    }

    if ((opsCmd >= LIGHT_OPS_IO_CMD_END) || (opsCmd < LIGHT_OPS_IO_CMD_ENABLE)) {
        HDF_LOGE("%s: invalid cmd = %d", __func__, opsCmd);
        return HDF_FAILURE;
    }

    count = sizeof(g_lightCmdHandle) / sizeof(g_lightCmdHandle[0]);
    for (loop = 0; loop < count; ++loop) {
        if ((opsCmd == g_lightCmdHandle[loop].cmd) && (g_lightCmdHandle[loop].func != NULL)) {
            return g_lightCmdHandle[loop].func(lightType, data, reply);
        }
    }

    return HDF_FAILURE;
}

static int32_t DispatchLight(struct HdfDeviceIoClient *client,
    int32_t cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t lightType;
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(client, HDF_ERR_INVALID_PARAM);

    if (cmd >= LIGHT_IO_CMD_END) {
        HDF_LOGE("%s: light cmd invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cmd == LIGHT_IO_CMD_GET_INFO_LIST) {
        CHECK_LIGHT_NULL_PTR_RETURN_VALUE(reply, HDF_ERR_INVALID_PARAM);
        return GetAllLightInfo(data, reply);
    }

    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    (void)OsalMutexLock(&drvData->mutex);
    if (!HdfSbufReadUint32(data, &lightType)) {
        HDF_LOGE("%s: sbuf read lightType failed", __func__);
        (void)OsalMutexUnlock(&drvData->mutex);
        return HDF_ERR_INVALID_PARAM;
    }

    if (lightType < LIGHT_TYPE_NONE || lightType >= LIGHT_TYPE_BUTT) {
        HDF_LOGE("%s: light type invalid para", __func__);
        (void)OsalMutexUnlock(&drvData->mutex);
        return HDF_FAILURE;
    }

    ret = DispatchCmdHandle(lightType, data, reply);
    (void)OsalMutexUnlock(&drvData->mutex);

    return ret;
}

static int32_t ParseLightInfo(const struct DeviceResourceNode *node, const struct DeviceResourceIface *parser)
{
    int32_t ret;
    uint32_t i;
    uint32_t temp;
    uint32_t *lightBrightness = NULL;
    struct LightDriverData *drvData = NULL;

    drvData = GetLightDrvData();
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(parser, HDF_ERR_INVALID_PARAM);

    drvData->lightNum = (uint32_t)parser->GetElemNum(node, "lightType");
    if (drvData->lightNum > LIGHT_TYPE_NUM) {
        HDF_LOGE("%s: lightNum cross the border", __func__);
        return HDF_FAILURE;
    }

    ret = memset_s(drvData->info, sizeof(drvData->info[LIGHT_TYPE_NONE]) * LIGHT_TYPE_BUTT, 0,
        sizeof(drvData->info[LIGHT_TYPE_NONE]) * LIGHT_TYPE_BUTT);
    CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "memset_s");

    for (i = 0; i < drvData->lightNum; ++i) {
        ret = parser->GetUint32ArrayElem(node, "lightType", i, &temp, 0);
        CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "lightType");

        if (temp >= LIGHT_TYPE_BUTT) {
            HDF_LOGE("%s: light type invalid para", __func__);
            return HDF_FAILURE;
        }

        drvData->info[temp] = (struct LightDeviceInfo *)OsalMemCalloc(sizeof(struct LightDeviceInfo));
        if (drvData->info[temp] == NULL) {
            HDF_LOGE("%s: malloc fail", __func__);
            return HDF_FAILURE;
        }

        ret = parser->GetUint32(node, "busRNum", &drvData->info[temp]->busRNum, 0);
        CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "busRNum");
        ret = parser->GetUint32(node, "busGNum", &drvData->info[temp]->busGNum, 0);
        CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "busGNum");
        ret = parser->GetUint32(node, "busBNum", &drvData->info[temp]->busBNum, 0);
        CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "busBNum");
        lightBrightness = (uint32_t *)&drvData->info[temp]->lightBrightness;
        ret = parser->GetUint32(node, "lightBrightness", lightBrightness, 0);
        CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "lightBrightness");
        ret = parser->GetUint32(node, "onTime", &drvData->info[temp]->onTime, 0);
        CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "onTime");
        ret = parser->GetUint32(node, "offTime", &drvData->info[temp]->offTime, 0);
        CHECK_LIGHT_PARSER_RESULT_RETURN_VALUE(ret, "offTime");

        drvData->info[temp]->lightState = LIGHT_STATE_STOP;
    }

    return HDF_SUCCESS;
}

static int32_t GetLightConfigData(const struct DeviceResourceNode *node)
{
    struct DeviceResourceIface *parser = NULL;
    const struct DeviceResourceNode *light = NULL;
    const struct DeviceResourceNode *childNode = NULL;

    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);

    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(parser, HDF_ERR_INVALID_PARAM);
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(parser->GetChildNode, HDF_ERR_INVALID_PARAM);

    childNode = parser->GetChildNode(node, "lightAttr");
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(childNode, HDF_ERR_INVALID_PARAM);
    light = parser->GetChildNode(childNode, "light01");
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(light, HDF_ERR_INVALID_PARAM);

    if (ParseLightInfo(light, parser) != HDF_SUCCESS) {
        HDF_LOGE("%s: ParseLightInfo  is failed!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t BindLightDriver(struct HdfDeviceObject *device)
{
    struct LightDriverData *drvData = NULL;

    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(device, HDF_FAILURE);

    drvData = (struct LightDriverData *)OsalMemCalloc(sizeof(*drvData));
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_MALLOC_FAIL);

    drvData->ioService.Dispatch = DispatchLight;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_lightDrvData = drvData;

    return HDF_SUCCESS;
}

static void LightWorkEntry(void *para)
{
    uint32_t lightType;
    struct LightDriverData *drvData = (struct LightDriverData *)para;
    CHECK_LIGHT_NULL_PTR_RETURN(drvData);
    lightType = drvData->lightType;

    if (drvData->info[lightType] == NULL) {
        HDF_LOGE("%s: lightType info is NULL!", __func__);
        return;
    }

    if (drvData->info[lightType]->lightState == LIGHT_STATE_START) {
        if (StopLight(lightType) != HDF_SUCCESS) {
        HDF_LOGE("%s: add light work fail! device state[%d]!", __func__, drvData->info[lightType]->lightState);
        }
        drvData->info[lightType]->lightState = LIGHT_STATE_STOP;
        return;
    }

    if (drvData->info[lightType]->lightState == LIGHT_STATE_STOP) {
        if (StartLight(lightType) != HDF_SUCCESS) {
        HDF_LOGE("%s: add light work fail! device state[%d]!", __func__, drvData->info[lightType]->lightState);
        }
        drvData->info[lightType]->lightState = LIGHT_STATE_START;
        return;
    }
}

int32_t InitLightDriver(struct HdfDeviceObject *device)
{
    struct LightDriverData *drvData = NULL;

    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(device, HDF_FAILURE);
    drvData = (struct LightDriverData *)device->service;
    CHECK_LIGHT_NULL_PTR_RETURN_VALUE(drvData, HDF_FAILURE);

    if (OsalMutexInit(&drvData->mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: init mutex fail!", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkQueueInit(&drvData->workQueue, LIGHT_WORK_QUEUE_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: init workQueue fail!", __func__);
        return HDF_FAILURE;
    }

    if (HdfWorkInit(&drvData->work, LightWorkEntry, (void*)drvData) != HDF_SUCCESS) {
        HDF_LOGE("%s: init work fail!", __func__);
        return HDF_FAILURE;
    }

    if (GetLightConfigData(device->property) != HDF_SUCCESS) {
        HDF_LOGE("%s: get light config fail!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void ReleaseLightDriver(struct HdfDeviceObject *device)
{
    int32_t i;
    struct LightDriverData *drvData = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }

    drvData = (struct LightDriverData *)device->service;
    if (drvData == NULL) {
        HDF_LOGE("%s: drvData is null", __func__);
        return;
    }

    for (i = LIGHT_TYPE_NONE; i < LIGHT_TYPE_BUTT; ++i) {
        if (drvData->info[i] != NULL) {
            OsalMemFree(drvData->info[i]);
            drvData->info[i] = NULL;
        }
    }

    HdfWorkDestroy(&drvData->work);
    HdfWorkQueueDestroy(&drvData->workQueue);
    (void)OsalMutexDestroy(&drvData->mutex);
    OsalMemFree(drvData);
    g_lightDrvData = NULL;
}

struct HdfDriverEntry g_lightDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_LIGHT",
    .Bind = BindLightDriver,
    .Init = InitLightDriver,
    .Release = ReleaseLightDriver,
};

HDF_INIT(g_lightDriverEntry);
