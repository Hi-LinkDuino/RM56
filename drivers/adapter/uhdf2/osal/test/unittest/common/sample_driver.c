/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sample_driver.h"
#include <linux/ioctl.h>
#include "hdf_log.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"

#define HDF_LOG_TAG sample_driver

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif

void HdfSampleDriverRelease(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    return;
}

int32_t SampleDriverRegisteDevice(struct HdfSBuf *data)
{
    if (data == NULL) {
        return HDF_FAILURE;
    }

    const char *moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }

    struct HdfDeviceObject *devObj = HdfRegisteDevice(moduleName, serviceName);
    if (devObj == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t SampleDriverUnregisteDevice(struct HdfSBuf *data)
{
    if (data == NULL) {
        return HDF_FAILURE;
    }

    const char *moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }
    HdfUnregisteDevice(moduleName, serviceName);
    return HDF_SUCCESS;
}

int32_t SampleDriverSendEvent(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *data, bool broadcast)
{
    int32_t ret = broadcast ? HdfDeviceSendEvent(client->device, id, data) :
        HdfDeviceSendEventToClient(client, id, data);
    HDF_LOGI("%{public}s:report event done, broadcast = %{public}d", __func__, broadcast);
    return ret;
}

int32_t SampleDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;
    if (reply == NULL || client == NULL) {
        return HDF_FAILURE;
    }
    HDF_LOGI("%{public}s:cmdId = %{public}d", __func__, cmdId);
    switch (cmdId) {
        case SAMPLE_DRIVER_REGISTE_DEVICE: {
            ret = SampleDriverRegisteDevice(data);
            HdfSbufWriteInt32(reply, ret);
            break;
        }
        case SAMPLE_DRIVER_UNREGISTE_DEVICE:
            ret = SampleDriverUnregisteDevice(data);
            HdfSbufWriteInt32(reply, ret);
            break;
        case SAMPLE_DRIVER_SENDEVENT_SINGLE_DEVICE:
            ret =  SampleDriverSendEvent(client, cmdId, data, false);
            HdfSbufWriteInt32(reply, INT32_MAX);
            break;
        case SAMPLE_DRIVER_SENDEVENT_BROADCAST_DEVICE:
            ret = SampleDriverSendEvent(client, cmdId, data, true);
            HdfSbufWriteInt32(reply, INT32_MAX);
            break;
        default:
            break;
    }

    return ret;
}

int HdfSampleDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGD("%{public}s::enter!, deviceObject=%{public}p", __func__, deviceObject);
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }
    static struct IDeviceIoService testService = {
        .Dispatch = SampleDriverDispatch,
        .Open = NULL,
        .Release = NULL,
    };
    deviceObject->service = &testService;
    return HDF_SUCCESS;
}

int HdfSampleDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGD("%{public}s::enter!, deviceObject=%{public}p", __func__, deviceObject);
    if (deviceObject == NULL) {
        HDF_LOGE("%{public}s::ptr is null!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%{public}s:Init success", __func__);
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

