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

#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>
#include <osal_mem.h>
#include "sensor_if.h"
#include "sensor_impl.h"

#define HDF_LOG_TAG    hdf_sensor_if_driver

using namespace OHOS::HDI::Sensor::V1_0;

struct HdfSensorInterfaceHost {
    struct IDeviceIoService ioservice;
    SensorImpl *service;
};

static int32_t SensorInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct HdfSensorInterfaceHost *hdfSensorInterfaceHost = CONTAINER_OF(
        client->device->service, struct HdfSensorInterfaceHost, ioservice);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    (void)SbufToParcel(reply, &replyParcel);
    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfSensorInterfaceHost->service->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int HdfSensorInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfSensorInterfaceDriverInit enter");
    struct HdfSensorInterfaceHost *hdfSensorInterfaceHost =
        CONTAINER_OF(deviceObject->service, struct HdfSensorInterfaceHost, ioservice);
    if (hdfSensorInterfaceHost != nullptr && hdfSensorInterfaceHost->service != nullptr) {
        hdfSensorInterfaceHost->service->Init();
    }
    
    return HDF_SUCCESS;
}

static int HdfSensorInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    struct HdfSensorInterfaceHost *hdfSensorInterfaceHost = (struct HdfSensorInterfaceHost *)OsalMemAlloc(
        sizeof(struct HdfSensorInterfaceHost));
    if (hdfSensorInterfaceHost == nullptr) {
        HDF_LOGE("HdfSensorInterfaceDriverBind OsalMemAlloc HdfSensorInterfaceHost failed!");
        return HDF_FAILURE;
    }

    hdfSensorInterfaceHost->ioservice.Dispatch = SensorInterfaceDriverDispatch;
    hdfSensorInterfaceHost->ioservice.Open = nullptr;
    hdfSensorInterfaceHost->ioservice.Release = nullptr;
    hdfSensorInterfaceHost->service = new SensorImpl();

    deviceObject->service = &hdfSensorInterfaceHost->ioservice;
    HDF_LOGI("HdfSensorInterfaceDriverBind Success");
    return HDF_SUCCESS;
}

static void HdfSensorInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    struct HdfSensorInterfaceHost *hdfSensorInterfaceHost =
        CONTAINER_OF(deviceObject->service, struct HdfSensorInterfaceHost, ioservice);
    delete hdfSensorInterfaceHost->service;
    OsalMemFree(hdfSensorInterfaceHost);
    HDF_LOGI("HdfSensorInterfaceDriverRelease Success");
}

struct HdfDriverEntry g_sensorinterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "sensor_service",
    .Bind = HdfSensorInterfaceDriverBind,
    .Init = HdfSensorInterfaceDriverInit,
    .Release = HdfSensorInterfaceDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_sensorinterfaceDriverEntry);
#ifndef __cplusplus
}
#endif
