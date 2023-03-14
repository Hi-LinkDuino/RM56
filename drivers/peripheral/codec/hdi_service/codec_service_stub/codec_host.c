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

#include "codec_stub.h"
#include <hdf_device_object.h>
#include <hdf_log.h>
#include <osal_mem.h>

static int32_t CodecServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    return CodecServiceOnRemoteRequest(client, cmdId, data, reply);
}

void HdfCodecDriverRelease(struct HdfDeviceObject *deviceObject)
{
    struct IDeviceIoService *testService = deviceObject->service;
    OsalMemFree(testService);
}

int HdfCodecDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGE("HdfCodecDriverBind enter!");

    struct IDeviceIoService *ioService = (struct IDeviceIoService *)OsalMemAlloc(sizeof(struct IDeviceIoService));
    if (ioService == NULL) {
        HDF_LOGE("HdfCodecDriverBind OsalMemAlloc IDeviceIoService failed!");
        return HDF_FAILURE;
    }

    ioService->Dispatch = CodecServiceDispatch;
    ioService->Open = NULL;
    ioService->Release = NULL;
    int ret = HdfDeviceObjectSetInterfaceDesc(deviceObject, "ohos.hdi.codec_service");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to set interface desc");
        return ret;
    }
    deviceObject->service = ioService;
    return HDF_SUCCESS;
}

int HdfCodecDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGE("HdfSampleDriverCInit enter, new hdi impl");
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_codecHostDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "libcodec_server.z.so",
    .Bind = HdfCodecDriverBind,
    .Init = HdfCodecDriverInit,
    .Release = HdfCodecDriverRelease,
};

HDF_INIT(g_codecHostDriverEntry);