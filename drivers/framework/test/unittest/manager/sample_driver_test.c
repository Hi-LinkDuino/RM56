/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "sample_driver_test.h"
#include "devmgr_service.h"
#include "devsvc_manager_clnt.h"
#include "hdf_device_object.h"
#include "hdf_log.h"
#include "hdf_pm.h"
#include "osal_file.h"
#include "osal_mem.h"

#define HDF_LOG_TAG sample_driver_test

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif

struct SampleTestDevice {
    struct DListHead listNode;
    struct HdfDeviceObject *devobj;
};

struct DListHead g_sampleDeviceList = { NULL };

#define REGISTER_DEV_MAX 16
struct HdfDeviceObject *g_resistedDevice[REGISTER_DEV_MAX] = { 0 };

static void SaveRegistedDevice(struct SampleTestDevice *sampleDev)
{
    if (g_sampleDeviceList.next == NULL) {
        DListHeadInit(&g_sampleDeviceList);
    }
    DListInsertTail(&sampleDev->listNode, &g_sampleDeviceList);
}

struct SampleTestDevice *GetRegistedDevice(const char *serviceName)
{
    struct SampleTestDevice *sampleDev = NULL;
    struct SampleTestDevice *sampleDevTmp = NULL;
    DLIST_FOR_EACH_ENTRY_SAFE(sampleDev, sampleDevTmp, &g_sampleDeviceList, struct SampleTestDevice, listNode) {
        if (sampleDev->devobj == NULL || HdfDeviceGetServiceName(sampleDev->devobj) == NULL) {
            DListRemove(&sampleDev->listNode);
            OsalMemFree(sampleDev);
            continue;
        }

        if (strcmp(HdfDeviceGetServiceName(sampleDev->devobj), serviceName) == 0) {
            return sampleDev;
        }
    }

    return NULL;
}

void HdfSampleDriverRelease(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    return;
}

int32_t SampleDriverRegisterDevice(struct HdfDeviceObject *dev, struct HdfSBuf *data)
{
    const char *moduleName = NULL;
    const char *serviceName = NULL;
    struct HdfDeviceObject *devObj = NULL;
    struct SampleTestDevice *sampleDev = NULL;
    int ret;

    HDF_LOGI("%s:called", __func__);
    if (data == NULL) {
        return HDF_FAILURE;
    }

    moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }

    devObj = HdfDeviceObjectAlloc(dev, moduleName);
    if (devObj == NULL) {
        HDF_LOGE("failed to alloc new device for %s", moduleName);
        return HDF_FAILURE;
    }

    ret = HdfDeviceObjectRegister(devObj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to register device for %s", moduleName);
        HdfDeviceObjectRelease(devObj);
        return HDF_FAILURE;
    }

    ret = HdfDeviceObjectPublishService(devObj, serviceName, SERVICE_POLICY_CAPACITY,
        OSAL_S_IREAD | OSAL_S_IWRITE | OSAL_S_IRGRP | OSAL_S_IWGRP | OSAL_S_IROTH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to publish service for %s", serviceName);
        HdfDeviceObjectRelease(devObj);
        return ret;
    }

    sampleDev = OsalMemAlloc(sizeof(struct SampleTestDevice));
    if (sampleDev == NULL) {
        HdfDeviceObjectRelease(devObj);
        return HDF_ERR_MALLOC_FAIL;
    }
    sampleDev->devobj = devObj;
    SaveRegistedDevice(sampleDev);
    HDF_LOGI("register device %s:%s success", moduleName, serviceName);
    return ret;
}

int32_t SampleDriverUnregisterDevice(struct HdfSBuf *data)
{
    const char *moduleName = NULL;
    const char *serviceName = NULL;
    struct SampleTestDevice *dev = NULL;
    if (data == NULL) {
        return HDF_FAILURE;
    }

    moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }
    dev = GetRegistedDevice(serviceName);
    if (dev == NULL) {
        HDF_LOGE("failed to found device %s", serviceName);
        return HDF_DEV_ERR_NO_DEVICE;
    }
    HdfDeviceObjectRelease(dev->devobj);
    DListRemove(&dev->listNode);
    OsalMemFree(dev);
    HDF_LOGI("unregister device %s:%s success", moduleName, serviceName);
    return HDF_SUCCESS;
}

int32_t SampleDriverSendEvent(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *data, bool broadcast)
{
    return broadcast ? HdfDeviceSendEvent(client->device, id, data) : HdfDeviceSendEventToClient(client, id, data);
}

int32_t SampleDriverPowerStateInject(uint32_t powerState)
{
    int ret;
    struct IDevmgrService *devmgrService = DevmgrServiceGetInstance();
    if (devmgrService == NULL || devmgrService->PowerStateChange == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = devmgrService->PowerStateChange(devmgrService, powerState);

    HDF_LOGI("%s: inject power state(%u) done, ret = %d", __func__, powerState, ret);
    return ret;
}

int32_t SampleDriverUpdateService(struct HdfDeviceIoClient *client, struct HdfSBuf *data)
{
    const char *servInfo = HdfSbufReadString(data);
    int32_t ret;
    if (servInfo == NULL) {
        HDF_LOGE("%s: miss servce info", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (HdfDeviceObjectSetServInfo(client->device, servInfo) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to set servce info", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = HdfDeviceObjectUpdate(client->device);
    HDF_LOGE("%s:set servce info done, ret = %d", __func__, ret);
    return ret;
}

int32_t SampleDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t powerState = 0;
    int32_t ret = HDF_SUCCESS;
    if (reply == NULL || client == NULL) {
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case SAMPLE_DRIVER_REGISTER_DEVICE: {
            ret = SampleDriverRegisterDevice(client->device, data);
            HdfSbufWriteInt32(reply, ret);
            break;
        }
        case SAMPLE_DRIVER_UNREGISTER_DEVICE:
            ret = SampleDriverUnregisterDevice(data);
            HdfSbufWriteInt32(reply, ret);
            break;
        case SAMPLE_DRIVER_UPDATE_SERVICE_INFO:
            ret = SampleDriverUpdateService(client, data);
            break;
        case SAMPLE_DRIVER_SENDEVENT_SINGLE_DEVICE:
            ret = SampleDriverSendEvent(client, cmdId, data, false);
            HdfSbufWriteInt32(reply, INT32_MAX);
            break;
        case SAMPLE_DRIVER_SENDEVENT_BROADCAST_DEVICE:
            ret = SampleDriverSendEvent(client, cmdId, data, true);
            HdfSbufWriteInt32(reply, INT32_MAX);
            break;
        case SAMPLE_DRIVER_PM_STATE_INJECT:
            HdfSbufReadUint32(data, &powerState);
            return SampleDriverPowerStateInject(powerState);
        default:
            break;
    }

    return ret;
}

int HdfSampleDriverBind(struct HdfDeviceObject *deviceObject)
{
    static struct IDeviceIoService testService = {
        .Dispatch = SampleDriverDispatch,
        .Open = NULL,
        .Release = NULL,
    };
    HDF_LOGD("%s::enter", __func__);
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }

    deviceObject->service = &testService;
    return HDF_SUCCESS;
}

int HdfSampleDozeResume(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    HDF_LOGI("%s:called", __func__);
    return HDF_SUCCESS;
}

int HdfSampleDozeSuspend(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    HDF_LOGI("%s:called", __func__);
    return HDF_SUCCESS;
}

int HdfSampleResume(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    HDF_LOGI("%s:called", __func__);
    return HDF_SUCCESS;
}

int HdfSampleSuspend(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    HDF_LOGI("%s:called", __func__);
    return HDF_SUCCESS;
}

struct SampleDriverPmListener {
    struct IPowerEventListener powerListener;
    void *p;
};

int HdfSampleDriverInit(struct HdfDeviceObject *deviceObject)
{
    static struct SampleDriverPmListener pmListener = { 0 };
    int ret;
    HDF_LOGI("%s::enter!", __func__);
    if (deviceObject == NULL) {
        HDF_LOGE("%s::ptr is null!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s:Init success", __func__);
    HdfDeviceObjectSetServInfo(deviceObject, SAMPLE_SERVICE);
    pmListener.powerListener.DozeResume = HdfSampleDozeResume;
    pmListener.powerListener.DozeSuspend = HdfSampleDozeSuspend;
    pmListener.powerListener.Resume = HdfSampleResume;
    pmListener.powerListener.Suspend = HdfSampleSuspend;

    ret = HdfPmRegisterPowerListener(deviceObject, &pmListener.powerListener);
    HDF_LOGI("%s:register power listener, ret = %d", __func__, ret);

    return HDF_SUCCESS;
}

struct HdfDriverEntry g_sampleDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "sample_driver",
    .Bind = HdfSampleDriverBind,
    .Init = HdfSampleDriverInit,
    .Release = HdfSampleDriverRelease,
};

HDF_INIT(g_sampleDriverEntry);

