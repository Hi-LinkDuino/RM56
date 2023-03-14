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

#include "hdf_device_object.h"
#include "wlan_hdi_service_stub.h"
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <osal_mem.h>
#include <fcntl.h>

static int32_t WlanHdiServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    return WlanHdiServiceOnRemoteRequest(client, cmdId, data, reply);
}

void HdfWlanHdiDriverRelease(struct HdfDeviceObject *deviceObject)
{
    struct HdfWlanRemoteNode *pos = NULL;
    struct HdfWlanRemoteNode *tmp = NULL;
    struct HdfWlanStubData *stubData = HdfStubDriver();
    if (stubData == NULL) {
        HDF_LOGE("%s: stubData is NUll!", __func__);
        return;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &stubData->remoteListHead, struct HdfWlanRemoteNode, node) {
        DListRemove(&(pos->node));
        OsalMemFree(pos);
    }
    OsalMutexDestroy(&stubData->mutex);
    struct IDeviceIoService *testService = deviceObject->service;
    OsalMemFree(testService);
}

int HdfWlanHdiDriverBind(struct HdfDeviceObject *deviceObject)
{
    struct IDeviceIoService *ioService = (struct IDeviceIoService *)OsalMemAlloc(sizeof(struct IDeviceIoService));
    if (ioService == NULL) {
        HDF_LOGE("%s: OsalMemAlloc IDeviceIoService failed!", __func__);
        return HDF_FAILURE;
    }
    ioService->Dispatch = WlanHdiServiceDispatch;
    ioService->Open = NULL;
    ioService->Release = NULL;
    int ret = HdfDeviceObjectSetInterfaceDesc(deviceObject, "HDI.WLAN.V1_0");
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to set interface desc");
        return ret;
    }
    deviceObject->service = ioService;
    return HDF_SUCCESS;
}

int32_t HdfWlanRegisterInit (void)
{
    int32_t ret;

    struct HdfWlanStubData *stubData = HdfStubDriver();
    DListHeadInit(&stubData->remoteListHead);
    ret = OsalMutexInit(&stubData->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Mutex init failed, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HdfWlanHdiDriverInit(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;

    int32_t ret = HdfWlanRegisterInit();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: register init failed, error code: %d", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_wlanHdiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "wlan_device",
    .Bind = HdfWlanHdiDriverBind,
    .Init = HdfWlanHdiDriverInit,
    .Release = HdfWlanHdiDriverRelease,
};

HDF_INIT(g_wlanHdiDriverEntry);