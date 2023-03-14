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
#include <hdf_log.h>
#include <hdf_device_desc.h>
#include <osal_mem.h>
#include "video_layer_stub.h"

#define HDF_LOG_TAG display_hdi_service

using HdfDisplayHdiService = struct _HdfDisplayHdiService {
    struct IDeviceIoService ioservice;
    void *instance;
};

static int32_t DisplayHdiServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HdfDisplayHdiService *displayHdiService = CONTAINER_OF(client->device->service, HdfDisplayHdiService, ioservice);
    return LayerServiceOnRemoteRequest(displayHdiService->instance, cmdId, data, reply);
}

static int32_t HdfDisplayHostDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: enter", __func__);
    return HDF_SUCCESS;
}

static int32_t HdfDisplayHostDriverBind(struct HdfDeviceObject *deviceObject)
{
    HdfDisplayHdiService *displayHdiService =
        reinterpret_cast<HdfDisplayHdiService *>(OsalMemAlloc(sizeof(HdfDisplayHdiService)));
    if (displayHdiService == nullptr) {
        HDF_LOGE("%{public}s: OsalMemAlloc HdfDisplayHdiService failed", __func__);
        return HDF_FAILURE;
    }

    displayHdiService->ioservice.Dispatch = DisplayHdiServiceDispatch;
    displayHdiService->ioservice.Open = NULL;
    displayHdiService->ioservice.Release = NULL;
    displayHdiService->instance = LayerStubInstance();

    deviceObject->service = &displayHdiService->ioservice;
    HDF_LOGI("%{public}s: exit succ", __func__);
    return HDF_SUCCESS;
}

static void HdfDisplayHostDriverRelease(HdfDeviceObject *deviceObject)
{
    HdfDisplayHdiService *displayHdiService = CONTAINER_OF(deviceObject->service, HdfDisplayHdiService, ioservice);
    OsalMemFree(displayHdiService);
}

struct HdfDriverEntry g_displayHostDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "hdi_video_layer_service",
    .Bind = HdfDisplayHostDriverBind,
    .Init = HdfDisplayHostDriverInit,
    .Release = HdfDisplayHostDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif

HDF_INIT(g_displayHostDriverEntry);

#ifndef __cplusplus
}
#endif
