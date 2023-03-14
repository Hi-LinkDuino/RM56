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

#include "usb_pnp_manager.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "usb_ddk_pnp_loader.h"

#define HDF_LOG_TAG usb_pnp_manager
#define MODULENAMESIZE 128

bool UsbPnpManagerWriteModuleName(struct HdfSBuf *sbuf, const char *moduleName)
{
    char modName[MODULENAMESIZE] = {0};
    if (sprintf_s(modName, MODULENAMESIZE, "lib%s.z.so", moduleName) < 0) {
        HDF_LOGE("%s: sprintf_s modName failed", __func__);
        return false;
    }

    return HdfSbufWriteString(sbuf, modName);
}

static int32_t UsbPnpManagerDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    HDF_LOGI("received cmd = %d", cmd);
    return HDF_SUCCESS;
}

static int32_t UsbPnpManagerBind(struct HdfDeviceObject *device)
{
    static struct IDeviceIoService pnpLoaderService = {
        .Dispatch = UsbPnpManagerDispatch,
    };

    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    device->service = &pnpLoaderService;
    HDF_LOGI("usb pnp manager bind success\n");

    return HDF_SUCCESS;
}

static int32_t UsbPnpManagerInit(struct HdfDeviceObject *device)
{
    int32_t status;
    struct HdfIoService *usbPnpServ = HdfIoServiceBind(USB_PNP_NOTIFY_SERVICE_NAME);
    static struct HdfDevEventlistener usbPnpListener = {
        .callBack = UsbDdkPnpLoaderEventReceived,
    };
    usbPnpListener.priv = (void *)(device);

    if (usbPnpServ == NULL) {
        HDF_LOGE("HdfIoServiceBind failed");
        return HDF_ERR_INVALID_OBJECT;
    }

    status = HdfDeviceRegisterEventListener(usbPnpServ, &usbPnpListener);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("HdfDeviceRegisterEventListener failed status=%d", status);
        return status;
    }

    HDF_LOGE("UsbPnpManagerInit done");
    return UsbDdkPnpLoaderEventHandle();
}

static void UsbPnpManagerRelease(struct HdfDeviceObject *device)
{
    (void)device;
    return;
}

struct HdfDriverEntry g_usbPnpManagerEntry = {
    .moduleVersion = 1,
    .Bind = UsbPnpManagerBind,
    .Init = UsbPnpManagerInit,
    .Release = UsbPnpManagerRelease,
    .moduleName = "HDF_USB_PNP_MANAGER",
};

HDF_INIT(g_usbPnpManagerEntry);

