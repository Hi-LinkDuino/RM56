/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <hdf_device_desc.h>
#include <osal_mem.h>
#include "input_server_stub.h"

#define HDF_LOG_TAG InputServer

using namespace OHOS::Input;
struct HdfInputService {
    struct IDeviceIoService ioservice;
    void *instance;
};

static int32_t InputServiceDispatch(struct HdfDeviceIoClient *client, int32_t cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGE("%{public}s: line%{public}d!", __func__, __LINE__);
    struct HdfInputService *hdfInputService = CONTAINER_OF(
        client->device->service, struct HdfInputService, ioservice);
    return InputServiceOnRemoteRequest(hdfInputService->instance, cmdId, *data, *reply);
}

static int32_t HdfInputDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGE("HdfInputDriverInit enter, new hdi impl");
    return HDF_SUCCESS;
}

static int32_t HdfInputDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfInputDriverBind enter! line: %{public}d", __LINE__);
    struct HdfInputService *hdfInputService = (struct HdfInputService *)OsalMemAlloc(
        sizeof(struct HdfInputService));
    if (hdfInputService == nullptr) {
        HDF_LOGI("HdfInputDriverBind OsalMemAlloc hdfInputService failed!");
        return HDF_FAILURE;
    }
    hdfInputService->ioservice.Dispatch = InputServiceDispatch;
    hdfInputService->ioservice.Open = NULL;
    hdfInputService->ioservice.Release = NULL;
    hdfInputService->instance = InputStubInstance();
    deviceObject->service = &hdfInputService->ioservice;
    HDF_LOGI("HdfInputDriverBind end!");
    return HDF_SUCCESS;
}

static void HdfInputDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfInputDriverRelease enter!");
    struct HdfInputService *hdfInputService = CONTAINER_OF(deviceObject->service, struct HdfInputService, ioservice);
    InputStubRelease(hdfInputService->instance);
    OsalMemFree(hdfInputService);
}

struct HdfDriverEntry g_inputDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "input_service",
    .Bind = HdfInputDriverBind,
    .Init = HdfInputDriverInit,
    .Release = HdfInputDriverRelease,
};
#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_inputDriverEntry);
#ifndef __cplusplus
}
#endif
