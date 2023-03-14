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

#include <osal_mem.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_sbuf_ipc.h"
#include "battery_log.h"
#include "battery_interface_impl.h"

using namespace OHOS::HDI::Battery;
using namespace OHOS::HDI::Battery::V1_0;

struct HdfBatteryInterfaceHost {
    struct IDeviceIoService ioService;
    BatteryInterfaceImpl *service;
};

static int32_t BatteryInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    auto *hdfBatteryInterfaceHost = CONTAINER_OF(client->device->service, struct HdfBatteryInterfaceHost, ioService);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        BATTERY_HILOGE(COMP_HDI, "invalid data sbuf object to dispatch");
        return HDF_ERR_INVALID_PARAM;
    }
    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        BATTERY_HILOGE(COMP_HDI, "invalid reply sbuf object to dispatch");
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfBatteryInterfaceHost->service->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int32_t HdfBatteryInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    auto *hdfBatteryInterfaceHost = CONTAINER_OF(deviceObject->service, struct HdfBatteryInterfaceHost, ioService);
    return hdfBatteryInterfaceHost->service->Init();
}

static int32_t HdfBatteryInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    auto *hdfBatteryInterfaceHost =
        (struct HdfBatteryInterfaceHost *)OsalMemAlloc(sizeof(struct HdfBatteryInterfaceHost));
    if (hdfBatteryInterfaceHost == nullptr) {
        BATTERY_HILOGE(COMP_HDI, "OsalMemAlloc HdfBatteryInterfaceHost failed");
        return HDF_FAILURE;
    }

    hdfBatteryInterfaceHost->ioService.Dispatch = BatteryInterfaceDriverDispatch;
    hdfBatteryInterfaceHost->ioService.Open = nullptr;
    hdfBatteryInterfaceHost->ioService.Release = nullptr;
    hdfBatteryInterfaceHost->service = new BatteryInterfaceImpl();

    deviceObject->service = &hdfBatteryInterfaceHost->ioService;
    return HDF_SUCCESS;
}

static void HdfBatteryInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    auto *hdfBatteryInterfaceHost = CONTAINER_OF(deviceObject->service, struct HdfBatteryInterfaceHost, ioService);
    delete hdfBatteryInterfaceHost->service;
    OsalMemFree(hdfBatteryInterfaceHost);
}

static struct HdfDriverEntry g_batteryInterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "battery_interface_service",
    .Bind = HdfBatteryInterfaceDriverBind,
    .Init = HdfBatteryInterfaceDriverInit,
    .Release = HdfBatteryInterfaceDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif
HDF_INIT(g_batteryInterfaceDriverEntry);
#ifndef __cplusplus
}
#endif
