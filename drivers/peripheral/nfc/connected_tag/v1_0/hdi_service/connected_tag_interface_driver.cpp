/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "connected_tag_server_stub.h"

using namespace OHOS::HDI::NFC::V1_0;
struct HdfNfcHdiService {
    struct IDeviceIoService ioservice;
    void *instance;
};

static int32_t NfcHdiDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("%{public}s, cmdId=%{public}d", __func__, cmdId);
    struct HdfNfcHdiService *hdfNfcHdiService = CONTAINER_OF(
        client->device->service, struct HdfNfcHdiService, ioservice);
    return DispatchStubOnRemoteRequest(hdfNfcHdiService->instance, cmdId, data, reply);
}

static int HdfNfcHdiDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfNfcHdiDriverInit enter, new hdi impl.");
    return HDF_SUCCESS;
}

static int HdfNfcHdiDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfNfcHdiDriverBind enter.");

    struct HdfNfcHdiService *hdfNfcHdiService = (struct HdfNfcHdiService *)OsalMemAlloc(
        sizeof(struct HdfNfcHdiService));
    if (hdfNfcHdiService == nullptr) {
        HDF_LOGE("HdfNfcHdiDriverBind OsalMemAlloc HdfNfcHdiService failed!");
        return HDF_FAILURE;
    }

    hdfNfcHdiService->ioservice.Dispatch = NfcHdiDispatch;
    hdfNfcHdiService->ioservice.Open = NULL;
    hdfNfcHdiService->ioservice.Release = NULL;
    hdfNfcHdiService->instance = ConnectedTagServerStubInstance();

    deviceObject->service = &hdfNfcHdiService->ioservice;
    return HDF_SUCCESS;
}

static void HdfNfcHdiDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfNfcHdiDriverRelease enter.");
    if (deviceObject == nullptr) {
        return;
    }

    struct HdfNfcHdiService *hdfNfcHdiService = CONTAINER_OF(deviceObject->service, struct HdfNfcHdiService, ioservice);
    if (hdfNfcHdiService == nullptr) {
        HDF_LOGI("%{public}s hdfNfcHdiService is nullptr, no need to release", __func__);
        return;
    }
    ConnectedTagServerStubRelease(hdfNfcHdiService->instance);
    OsalMemFree(hdfNfcHdiService);
}

static struct HdfDriverEntry g_nfchdiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "connectedTagHdiModule",
    .Bind = HdfNfcHdiDriverBind,
    .Init = HdfNfcHdiDriverInit,
    .Release = HdfNfcHdiDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_nfchdiDriverEntry);
#ifndef __cplusplus
}
#endif
