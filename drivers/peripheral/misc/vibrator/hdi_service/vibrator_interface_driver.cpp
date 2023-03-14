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
#include <hdf_sbuf_ipc.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include "vibrator_interface_impl.h"

#define HDF_LOG_TAG              hdf_vibrator_if_driver

using namespace OHOS::HDI::Vibrator::V1_0;

struct HdfVibratorInterfaceHost {
    struct IDeviceIoService ioservice;
    VibratorInterfaceImpl *service;
};

static int32_t VibratorInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct HdfVibratorInterfaceHost *hdfVibratorInterfaceHost = CONTAINER_OF(
        client->device->service, struct HdfVibratorInterfaceHost, ioservice);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    (void)SbufToParcel(reply, &replyParcel);
    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfVibratorInterfaceHost->service->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int HdfVibratorInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfVibratorInterfaceDriverInit enter");
    return HDF_SUCCESS;
}

static int HdfVibratorInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    struct HdfVibratorInterfaceHost *hdfVibratorInterfaceHost = (struct HdfVibratorInterfaceHost *)OsalMemAlloc(
        sizeof(struct HdfVibratorInterfaceHost));
    if (hdfVibratorInterfaceHost == nullptr) {
        HDF_LOGE("HdfVibratorInterfaceDriverBind OsalMemAlloc HdfVibratorInterfaceHost failed!");
        return HDF_FAILURE;
    }

    hdfVibratorInterfaceHost->ioservice.Dispatch = VibratorInterfaceDriverDispatch;
    hdfVibratorInterfaceHost->ioservice.Open = nullptr;
    hdfVibratorInterfaceHost->ioservice.Release = nullptr;
    hdfVibratorInterfaceHost->service = new VibratorInterfaceImpl();

    deviceObject->service = &hdfVibratorInterfaceHost->ioservice;
    HDF_LOGI("HdfVibratorInterfaceDriverBind Success");
    return HDF_SUCCESS;
}

static void HdfVibratorInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfVibratorInterfaceDriverRelease enter");

    struct HdfVibratorInterfaceHost *hdfVibratorInterfaceHost = CONTAINER_OF(
        deviceObject->service, struct HdfVibratorInterfaceHost, ioservice);
    delete hdfVibratorInterfaceHost->service;
    OsalMemFree(hdfVibratorInterfaceHost);
}

struct HdfDriverEntry g_vibratorInterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "vibrator_service",
    .Bind = HdfVibratorInterfaceDriverBind,
    .Init = HdfVibratorInterfaceDriverInit,
    .Release = HdfVibratorInterfaceDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_vibratorInterfaceDriverEntry);
#ifndef __cplusplus
}
#endif
