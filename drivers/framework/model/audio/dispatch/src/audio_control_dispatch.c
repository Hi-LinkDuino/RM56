/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_control_dispatch.h"
#include "audio_control.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

static struct AudioKcontrol *AudioGetKctrlInstance(const struct AudioCtrlElemId *ctrlElemId)
{
    struct AudioKcontrol *kctrl = NULL;
    struct AudioCard *audioCard = NULL;

    if (ctrlElemId == NULL || ctrlElemId->itemName == NULL || ctrlElemId->cardServiceName == NULL) {
        ADM_LOG_ERR("input params check error: ctrlElemId is NULL.");
        return NULL;
    }

    audioCard = GetCardInstance(ctrlElemId->cardServiceName);
    if (audioCard == NULL) {
        ADM_LOG_ERR("get kcontrol instance fail!");
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY(kctrl, &audioCard->controls, struct AudioKcontrol, list) {
        if (kctrl->name == NULL) {
            continue;
        }
        if (strcmp(kctrl->name, ctrlElemId->itemName) != 0) {
            continue;
        }
        if (kctrl->iface != ctrlElemId->iface) {
            continue;
        }
        return kctrl;
    }
    return NULL;
}

static int32_t ControlHostElemInfoSub(struct HdfSBuf *rspData, const struct AudioCtrlElemId id)
{
    int32_t result;
    struct AudioKcontrol *kctrl = NULL;
    struct AudioCtrlElemInfo elemInfo;
    if (rspData == NULL) {
        ADM_LOG_ERR("Input rspData is null.");
        return HDF_FAILURE;
    }
    kctrl = AudioGetKctrlInstance(&id);
    if (kctrl == NULL || kctrl->Info == NULL) {
        ADM_LOG_ERR("Find kctrl or Info fail!");
        return HDF_FAILURE;
    }

    (void)memset_s(&elemInfo, sizeof(struct AudioCtrlElemInfo), 0, sizeof(struct AudioCtrlElemInfo));
    result = kctrl->Info(kctrl, &elemInfo);
    if (result != HDF_SUCCESS) {
        ADM_LOG_ERR("Get control info fail result=%d", result);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(rspData, elemInfo.type)) {
        ADM_LOG_ERR("Write response data type failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(rspData, elemInfo.max)) {
        ADM_LOG_ERR("Write response data max failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(rspData, elemInfo.min)) {
        ADM_LOG_ERR("Write response data min failed!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(rspData, elemInfo.count)) {
        ADM_LOG_ERR("Write response data count failed!");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t ControlHostElemInfo(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    struct AudioCtrlElemId id;
    ADM_LOG_DEBUG("entry.");

    if (reqData == NULL) {
        ADM_LOG_ERR("Input ElemInfo params check error: reqData is NULL.");
        return HDF_FAILURE;
    }
    if (client == NULL) {
        ADM_LOG_ERR("Input ElemInfo params check error: client is NULL.");
        return HDF_FAILURE;
    }

    (void)memset_s(&id, sizeof(struct AudioCtrlElemId), 0, sizeof(struct AudioCtrlElemId));
    if (!HdfSbufReadInt32(reqData, &id.iface)) {
        ADM_LOG_ERR("Read ElemInfo request id failed!");
        return HDF_FAILURE;
    }

    if (!(id.cardServiceName = HdfSbufReadString(reqData))) {
        ADM_LOG_ERR("Read ElemInfo request cardServiceName failed!");
        return HDF_FAILURE;
    }

    if (!(id.itemName = HdfSbufReadString(reqData))) {
        ADM_LOG_ERR("Read ElemInfo request itemName failed!");
        return HDF_FAILURE;
    }

    if (ControlHostElemInfoSub(rspData, id)) {
        ADM_LOG_ERR("ControlHostElemInfoSub failed!");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t ControlHostElemRead(const struct HdfDeviceIoClient *client, struct HdfSBuf *reqData,
    struct HdfSBuf *rspData)
{
    struct AudioKcontrol *kctrl = NULL;
    struct AudioCtrlElemValue elemValue;
    struct AudioCtrlElemId id;
    int32_t result;

    if (client == NULL) {
        ADM_LOG_ERR("Input ElemRead params check error: client is NULL.");
        return HDF_FAILURE;
    }
    if (reqData == NULL) {
        ADM_LOG_ERR("Input ElemRead params check error: reqData is NULL.");
        return HDF_FAILURE;
    }
    if (rspData == NULL) {
        ADM_LOG_ERR("Input ElemRead params check error: rspData is NULL.");
        return HDF_FAILURE;
    }

    (void)memset_s(&id, sizeof(struct AudioCtrlElemId), 0, sizeof(struct AudioCtrlElemId));
    if (!HdfSbufReadInt32(reqData, &id.iface)) {
        ADM_LOG_ERR("ElemRead request id failed!");
        return HDF_FAILURE;
    }

    if (!(id.cardServiceName = HdfSbufReadString(reqData))) {
        ADM_LOG_ERR("ElemRead request cardServiceName failed!");
        return HDF_FAILURE;
    }

    if (!(id.itemName = HdfSbufReadString(reqData))) {
        ADM_LOG_ERR("ElemRead request itemName failed!");
        return HDF_FAILURE;
    }
    ADM_LOG_DEBUG("itemName: %s cardServiceName: %s iface: %d ", id.itemName, id.cardServiceName, id.iface);

    kctrl = AudioGetKctrlInstance(&id);
    if (kctrl == NULL || kctrl->Get == NULL) {
        ADM_LOG_ERR("Find kctrl or Get fail.");
        return HDF_FAILURE;
    }

    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    result = kctrl->Get(kctrl, &elemValue);
    if (result != HDF_SUCCESS) {
        ADM_LOG_ERR("Get elemValue fail result=%d", result);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(rspData, elemValue.value[0])) {
        ADM_LOG_ERR("Write response data value[0]=%d failed!", elemValue.value[0]);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(rspData, elemValue.value[1])) {
        ADM_LOG_ERR("Write response data value[1]=%d failed!", elemValue.value[1]);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t ControlHostElemWrite(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    struct AudioKcontrol *kctrl = NULL;
    struct AudioCtrlElemValue elemValue;
    int32_t result;

    if (client == NULL) {
        ADM_LOG_ERR("Input params check error: client is NULL.");
        return HDF_FAILURE;
    }
    if (reqData == NULL) {
        ADM_LOG_ERR("Input params check error: reqData is NULL.");
        return HDF_FAILURE;
    }
    (void)rspData;

    (void)memset_s(&elemValue, sizeof(struct AudioCtrlElemValue), 0, sizeof(struct AudioCtrlElemValue));
    if (!HdfSbufReadInt32(reqData, &elemValue.value[0])) {
        ADM_LOG_ERR("Read request elemValue failed!");
        return HDF_FAILURE;
    }
    elemValue.value[1] = elemValue.value[0];

    if (!HdfSbufReadInt32(reqData, &elemValue.id.iface)) {
        ADM_LOG_ERR("Read request id failed!");
        return HDF_FAILURE;
    }

    if (!(elemValue.id.cardServiceName = HdfSbufReadString(reqData))) {
        ADM_LOG_ERR("Read request cardServiceName failed!");
        return HDF_FAILURE;
    }

    if (!(elemValue.id.itemName = HdfSbufReadString(reqData))) {
        ADM_LOG_ERR("Read request itemName failed!");
        return HDF_FAILURE;
    }

    ADM_LOG_DEBUG("itemName: %s cardServiceName: %s iface: %d value: %d ", elemValue.id.itemName,
        elemValue.id.cardServiceName,
        elemValue.id.iface, elemValue.value[0]);

    kctrl = AudioGetKctrlInstance(&elemValue.id);
    if (kctrl == NULL || kctrl->Set == NULL) {
        ADM_LOG_ERR("Find kctrl or Set fail!");
        return HDF_FAILURE;
    }

    result = kctrl->Set(kctrl, &elemValue);
    if (result != HDF_SUCCESS) {
        ADM_LOG_ERR("Get control value fail result=%d", result);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct ControlDispCmdHandleList g_controlDispCmdHandle[] = {
    {AUDIODRV_CTRL_IOCTRL_ELEM_INFO, ControlHostElemInfo},
    {AUDIODRV_CTRL_IOCTRL_ELEM_READ, ControlHostElemRead},
    {AUDIODRV_CTRL_IOCTRL_ELEM_WRITE, ControlHostElemWrite},
};

static int32_t ControlDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    unsigned int i;

    if (client == NULL) {
        ADM_LOG_ERR("Input params check error: client is NULL.");
        return HDF_FAILURE;
    }
    if (data == NULL) {
        ADM_LOG_ERR("Input params check error: data is NULL.");
        return HDF_FAILURE;
    }

    if (cmdId >= AUDIODRV_CTRL_IOCTRL_ELEM_BUTT || cmdId < 0) {
        ADM_LOG_ERR("Invalid [cmdId=%d].", cmdId);
        return HDF_FAILURE;
    }

    for (i = 0; i < HDF_ARRAY_SIZE(g_controlDispCmdHandle); ++i) {
        if ((cmdId == (int)(g_controlDispCmdHandle[i].cmd)) && (g_controlDispCmdHandle[i].func != NULL)) {
            return g_controlDispCmdHandle[i].func(client, data, reply);
        }
    }
    return HDF_FAILURE;
}

static struct ControlHost *ControlHostCreateAndBind(struct HdfDeviceObject *device)
{
    struct ControlHost *controlHost = NULL;

    if (device == NULL) {
        ADM_LOG_ERR("Input params check error: device is NULL.");
        return NULL;
    }

    controlHost = (struct ControlHost *)OsalMemCalloc(sizeof(*controlHost));
    if (controlHost == NULL) {
        ADM_LOG_ERR("Malloc controlHost fail!");
        return NULL;
    }
    controlHost->device = device;
    device->service = &controlHost->service;
    return controlHost;
}

static int32_t AudioControlBind(struct HdfDeviceObject *device)
{
    struct ControlHost *controlHost = NULL;
    ADM_LOG_DEBUG("entry.");

    if (device == NULL) {
        ADM_LOG_ERR("Input params check error: device is NULL.");
        return HDF_FAILURE;
    }

    controlHost = ControlHostCreateAndBind(device);
    if (controlHost == NULL) {
        ADM_LOG_ERR("controlHost is NULL.");
        return HDF_FAILURE;
    }

    controlHost->service.Dispatch = ControlDispatch;

    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static int32_t AudioControlInit(struct HdfDeviceObject *device)
{
    (void)device;
    ADM_LOG_INFO("success.");
    return HDF_SUCCESS;
}

static void AudioControlRelease(struct HdfDeviceObject *device)
{
    struct ControlHost *controlHost = NULL;

    if (device == NULL) {
        ADM_LOG_ERR("Input params check error: device is NULL.");
        return;
    }

    controlHost = (struct ControlHost *)device->service;
    if (controlHost == NULL) {
        ADM_LOG_ERR("controlHost is NULL.");
        return;
    }
    OsalMemFree(controlHost);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_audioControlEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_AUDIO_CONTROL",
    .Bind = AudioControlBind,
    .Init = AudioControlInit,
    .Release = AudioControlRelease,
};
HDF_INIT(g_audioControlEntry);
