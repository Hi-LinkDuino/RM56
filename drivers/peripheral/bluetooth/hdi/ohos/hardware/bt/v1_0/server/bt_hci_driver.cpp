/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "bt_hci_stub.h"

using namespace ohos::hardware::bt::v1_0;

struct HdfBtHciService {
    struct IDeviceIoService ioservice;
    void *instance;
};

static int32_t BtHciServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("%{public}s, ", __func__);
    struct HdfBtHciService *hdfBtHciService = CONTAINER_OF(
        client->device->service, struct HdfBtHciService, ioservice);
    return BtHciServiceOnRemoteRequest(hdfBtHciService->instance, cmdId, data, reply);
}

int HdfBtHciDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfBtHciDriverInit enter, new hdi impl.");
    return HDF_SUCCESS;
}

int HdfBtHciDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfBtHciDriverBind enter.");

    struct HdfBtHciService *hdfBtHciService = (struct HdfBtHciService *)OsalMemAlloc(
        sizeof(struct HdfBtHciService));
    if (hdfBtHciService == nullptr) {
        HDF_LOGE("HdfBtHciDriverBind OsalMemAlloc HdfBtHciService failed!");
        return HDF_FAILURE;
    }

    hdfBtHciService->ioservice.Dispatch = BtHciServiceDispatch;
    hdfBtHciService->ioservice.Open = NULL;
    hdfBtHciService->ioservice.Release = NULL;
    hdfBtHciService->instance = BtHciStubInstance();

    deviceObject->service = &hdfBtHciService->ioservice;
    return HDF_SUCCESS;
}

void HdfBtHciDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfIBtHciDriverRelease enter.");
    if (deviceObject == nullptr) {
        return;
    }

    struct HdfBtHciService *hdfBtHciService = CONTAINER_OF(deviceObject->service, struct HdfBtHciService, ioservice);
    if (hdfBtHciService == nullptr) {
        HDF_LOGI("%{public}s hdfBtHciService is nullptr, no need to release", __func__);
        return;
    }
    BtHciStubRelease(hdfBtHciService->instance);
    OsalMemFree(hdfBtHciService);
}

struct HdfDriverEntry g_bthciDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "bthci",
    .Bind = HdfBtHciDriverBind,
    .Init = HdfBtHciDriverInit,
    .Release = HdfBtHciDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_bthciDriverEntry);
#ifndef __cplusplus
}
#endif
