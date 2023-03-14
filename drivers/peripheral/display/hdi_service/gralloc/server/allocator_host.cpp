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

#include "allocator_service_stub.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"

using HdfAllocatorService = struct HdfAllocatorService_ {
    struct IDeviceIoService ioservice;
    void *instance;
};

static int32_t AllocatorServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HdfAllocatorService *allocatorService = CONTAINER_OF(
        client->device->service, HdfAllocatorService, ioservice);
    return AllocatorServiceOnRemoteRequest(allocatorService->instance, cmdId, *data, *reply);
}

int HdfAllocatorDriverInit(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    return HDF_SUCCESS;
}

int HdfAllocatorDriverBind(struct HdfDeviceObject *deviceObject)
{
    HdfAllocatorService *allocatorService =
        (HdfAllocatorService *)OsalMemAlloc(sizeof(HdfAllocatorService));
    if (allocatorService == nullptr) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed", __func__);
        return HDF_FAILURE;
    }

    allocatorService->ioservice.Dispatch = AllocatorServiceDispatch;
    allocatorService->ioservice.Open = NULL;
    allocatorService->ioservice.Release = NULL;
    allocatorService->instance = AllocatorServiceInstance();

    deviceObject->service = &allocatorService->ioservice;
    HDF_LOGI("%{public}s: exit succ", __func__);
    return HDF_SUCCESS;
}

void HdfAllocatorDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HdfAllocatorService *allocatorService = CONTAINER_OF(deviceObject->service, HdfAllocatorService, ioservice);
    AllocatorServiceRelease(allocatorService->instance);
    OsalMemFree(allocatorService);
}

struct HdfDriverEntry g_AllocatorDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "allocator_service",
    .Bind = HdfAllocatorDriverBind,
    .Init = HdfAllocatorDriverInit,
    .Release = HdfAllocatorDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif

HDF_INIT(g_AllocatorDriverEntry);

#ifndef __cplusplus
}
#endif
