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

#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>
#include <osal_mem.h>
#include "thermal_interface_impl.h"

#define HDF_LOG_TAG ThermalInterfaceDriver

using namespace OHOS::HDI::Thermal::V1_0;

struct HdfThermalInterfaceHost {
    struct IDeviceIoService ioservice;
    ThermalInterfaceImpl *service;
};

static int32_t ThermalInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct HdfThermalInterfaceHost *hdfThermalInterfaceHost =
        CONTAINER_OF(client->device->service, struct HdfThermalInterfaceHost, ioservice);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    (void)SbufToParcel(reply, &replyParcel);
    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfThermalInterfaceHost->service->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}

int HdfThermalInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfThermalInterfaceDriverInit enter");
    return HDF_SUCCESS;
}

int HdfThermalInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfThermalInterfaceDriverBind enter");

    struct HdfThermalInterfaceHost *hdfThermalInterfaceHost = (struct HdfThermalInterfaceHost *)OsalMemAlloc(
        sizeof(struct HdfThermalInterfaceHost));
    if (hdfThermalInterfaceHost == nullptr) {
        HDF_LOGE("HdfThermalInterfaceDriverBind OsalMemAlloc HdfThermalInterfaceHost failed!");
        return HDF_FAILURE;
    }

    hdfThermalInterfaceHost->ioservice.Dispatch = ThermalInterfaceDriverDispatch;
    hdfThermalInterfaceHost->ioservice.Open = NULL;
    hdfThermalInterfaceHost->ioservice.Release = NULL;
    hdfThermalInterfaceHost->service = new ThermalInterfaceImpl();

    deviceObject->service = &hdfThermalInterfaceHost->ioservice;
    return HDF_SUCCESS;
}

void HdfThermalInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfThermalInterfaceDriverRelease enter");
    struct HdfThermalInterfaceHost *hdfThermalInterfaceHost =
        CONTAINER_OF(deviceObject->service, struct HdfThermalInterfaceHost, ioservice);
    delete hdfThermalInterfaceHost->service;
    OsalMemFree(hdfThermalInterfaceHost);
}

struct HdfDriverEntry g_thermalinterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "thermal_interface_service",
    .Bind = HdfThermalInterfaceDriverBind,
    .Init = HdfThermalInterfaceDriverInit,
    .Release = HdfThermalInterfaceDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_thermalinterfaceDriverEntry);
#ifndef __cplusplus
}
#endif