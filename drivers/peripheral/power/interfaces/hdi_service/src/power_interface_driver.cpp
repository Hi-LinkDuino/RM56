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
#include "power_interface_impl.h"

#define HDF_LOG_TAG PowerInterfaceDriver

using namespace OHOS::HDI::Power::V1_0;

struct HdfPowerInterfaceHost {
    struct IDeviceIoService ioservice;
    PowerInterfaceImpl *service;
};

static int32_t PowerInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct HdfPowerInterfaceHost *hdfPowerInterfaceHost =
	    CONTAINER_OF(client->device->service, struct HdfPowerInterfaceHost, ioservice);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    (void)SbufToParcel(reply, &replyParcel);
    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfPowerInterfaceHost->service->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int HdfPowerInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfPowerInterfaceDriverInit enter");
    return HDF_SUCCESS;
}

static int HdfPowerInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfPowerInterfaceDriverBind enter");

    struct HdfPowerInterfaceHost *hdfPowerInterfaceHost = (struct HdfPowerInterfaceHost *)OsalMemAlloc(
        sizeof(struct HdfPowerInterfaceHost));
    if (hdfPowerInterfaceHost == nullptr) {
        HDF_LOGE("HdfPowerInterfaceDriverBind OsalMemAlloc HdfPowerInterfaceHost failed!");
        return HDF_FAILURE;
    }

    hdfPowerInterfaceHost->ioservice.Dispatch = PowerInterfaceDriverDispatch;
    hdfPowerInterfaceHost->ioservice.Open = NULL;
    hdfPowerInterfaceHost->ioservice.Release = NULL;
    hdfPowerInterfaceHost->service = new PowerInterfaceImpl();

    deviceObject->service = &hdfPowerInterfaceHost->ioservice;
    return HDF_SUCCESS;
}

static void HdfPowerInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfPowerInterfaceDriverRelease enter");

    struct HdfPowerInterfaceHost *hdfPowerInterfaceHost =
	    CONTAINER_OF(deviceObject->service, struct HdfPowerInterfaceHost, ioservice);
    delete hdfPowerInterfaceHost->service;
    OsalMemFree(hdfPowerInterfaceHost);
}

struct HdfDriverEntry g_powerinterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "power_interface_service",
    .Bind = HdfPowerInterfaceDriverBind,
    .Init = HdfPowerInterfaceDriverInit,
    .Release = HdfPowerInterfaceDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_powerinterfaceDriverEntry);
#ifndef __cplusplus
}
#endif
