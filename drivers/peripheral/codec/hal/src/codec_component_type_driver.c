/*
 * Copyright (c) 2022 Shenzhen Kaihong DID Co., Ltd.
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

#include <hdf_device_desc.h>
#include <hdf_device_object.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include "codec_component_type_stub.h"
#include "codec_component_capability_config.h"

#define HDF_LOG_TAG codec_hdi_server

struct HdfCodecComponentTypeHost {
    struct IDeviceIoService ioservice;
    struct CodecComponentTypeStub *instance;
};

static int32_t CodecComponentTypeDriverDispatch(struct HdfDeviceIoClient *client, int32_t cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct HdfCodecComponentTypeHost *omxcomponenttypeHost = CONTAINER_OF(
        client->device->service, struct HdfCodecComponentTypeHost, ioservice);
    omxcomponenttypeHost->instance->device = client->device;
    return CodecComponentTypeServiceOnRemoteRequest(omxcomponenttypeHost->instance, cmdId, data, reply);
}

int32_t HdfCodecComponentTypeDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfCodecComponentTypeDriverInit enter.");
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }
    InitDataNode(deviceObject->property);
    if (LoadCapabilityData() != HDF_SUCCESS) {
        ClearCapabilityData();
    }
    return HDF_SUCCESS;
}

int32_t HdfCodecComponentTypeDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfCodecComponentTypeDriverBind enter.");

    struct HdfCodecComponentTypeHost *omxcomponenttypeHost = (struct HdfCodecComponentTypeHost *)OsalMemAlloc(
        sizeof(struct HdfCodecComponentTypeHost));
    if (omxcomponenttypeHost == NULL) {
        HDF_LOGE("HdfCodecComponentTypeDriverBind OsalMemAlloc HdfCodecComponentTypeHost failed!");
        return HDF_FAILURE;
    }

    int ret = HdfDeviceObjectSetInterfaceDesc(deviceObject, "ohos.hdi.codec_service");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Failed to set interface desc");
        return ret;
    }

    omxcomponenttypeHost->ioservice.Dispatch = CodecComponentTypeDriverDispatch;
    omxcomponenttypeHost->ioservice.Open = NULL;
    omxcomponenttypeHost->ioservice.Release = NULL;
    omxcomponenttypeHost->instance = CodecComponentTypeStubGetInstance();
    if (omxcomponenttypeHost->instance == NULL) {
        OsalMemFree(omxcomponenttypeHost);
        return HDF_FAILURE;
    }

    deviceObject->service = &omxcomponenttypeHost->ioservice;
    return HDF_SUCCESS;
}

void HdfCodecComponentTypeDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfCodecComponentTypeDriverRelease enter.");
    struct HdfCodecComponentTypeHost *omxcomponenttypeHost
        = CONTAINER_OF(deviceObject->service, struct HdfCodecComponentTypeHost, ioservice);
    CodecComponentTypeStubRelease(omxcomponenttypeHost->instance);
    OsalMemFree(omxcomponenttypeHost);
    ClearCapabilityData();
}

struct HdfDriverEntry g_omxcomponenttypeDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "codec_hdi_omx_server",
    .Bind = HdfCodecComponentTypeDriverBind,
    .Init = HdfCodecComponentTypeDriverInit,
    .Release = HdfCodecComponentTypeDriverRelease,
};

HDF_INIT(g_omxcomponenttypeDriverEntry);