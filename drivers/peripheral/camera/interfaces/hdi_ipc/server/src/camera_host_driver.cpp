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

#include <hdf_log.h>
#include <hdf_base.h>
#include <osal_mem.h>
#include <hdf_device_desc.h>
#include "camera_host_stub.h"

#define HDF_LOG_TAG camera_service

struct HdfCameraService {
    struct IDeviceIoService ioservice;
    void *instance = nullptr;
};

static int32_t CameraServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HdfCameraService *hdfCameraService = CONTAINER_OF(client->device->service, HdfCameraService, ioservice);
    return CameraHostServiceOnRemoteRequest(hdfCameraService->instance, cmdId, data, reply);
}

int HdfCameraHostDriverInit(struct HdfDeviceObject *deviceObject)
{
    return HDF_SUCCESS;
}

int HdfCameraHostDriverBind(HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfCameraHostDriverBind enter!");
    if (deviceObject == nullptr) {
        HDF_LOGE("HdfCameraHostDriverBind: HdfDeviceObject is NULL !");
        return HDF_FAILURE;
    }

    HdfCameraService *hdfCameraService =
        reinterpret_cast<HdfCameraService *>(OsalMemAlloc(sizeof(HdfCameraService)));
    if (hdfCameraService == nullptr) {
        HDF_LOGE("HdfCameraHostDriverBind OsalMemAlloc HdfCameraService failed!");
        return HDF_FAILURE;
    }

    hdfCameraService->ioservice.Dispatch = CameraServiceDispatch;
    hdfCameraService->ioservice.Open = nullptr;
    hdfCameraService->ioservice.Release = nullptr;
    hdfCameraService->instance = CameraHostStubInstance();

    deviceObject->service = &hdfCameraService->ioservice;
    return HDF_SUCCESS;
}

void HdfCameraHostDriverRelease(HdfDeviceObject *deviceObject)
{
    if (deviceObject == nullptr || deviceObject->service == nullptr) {
        HDF_LOGE("%{public}s deviceObject or deviceObject->service  is NULL!", __FUNCTION__);
        return;
    }
    HdfCameraService *hdfCameraService = CONTAINER_OF(deviceObject->service, HdfCameraService, ioservice);
    if (hdfCameraService == nullptr) {
        HDF_LOGE("%{public}s hdfCameraService is NULL!", __FUNCTION__);
        return;
    }
    OsalMemFree(hdfCameraService);
}

struct HdfDriverEntry g_cameraHostDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "camera_service",
    .Bind = HdfCameraHostDriverBind,
    .Init = HdfCameraHostDriverInit,
    .Release = HdfCameraHostDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif

HDF_INIT(g_cameraHostDriverEntry);

#ifndef __cplusplus
}
#endif