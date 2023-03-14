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

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "securec.h"
#include "hdf_device_desc.h"
#include "osal_atomic.h"

#define HDF_LOG_TAG     USB_PNP_SAMPLE_TEST
#define STR_LEN         1024

static void UsbPnpSampleTestWriteLog(char *string)
{
    char str[STR_LEN];
    FILE *fp = NULL;
    struct timeval time;

    gettimeofday(&time, NULL);

    fp = fopen("/data/usbhost_pnp_xts", "a+");
    if (!fp) {
        HDF_LOGE("%s: fopen failed", __func__);
        return;
    }

    (void)snprintf_s(str, STR_LEN, STR_LEN - 1, "[XTSCHECK] %d.%06d, %s\n",
        time.tv_sec, time.tv_usec, string);

    (void)fwrite(str, strlen(str), 1, fp);
    (void)fclose(fp);
}

static int32_t UsbPnpSampleTestServiceDispatch(struct HdfDeviceIoClient *client, int32_t cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("%s: %d", __func__, __LINE__);
    return HDF_SUCCESS;
}

int32_t UsbPnpSampleTestDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%s: %d enter", __func__, __LINE__);
    static struct IDeviceIoService testService = {
        .Dispatch = UsbPnpSampleTestServiceDispatch,
        .Open = NULL,
        .Release = NULL,
    };
    deviceObject->service = &testService;
    HDF_LOGI("%s: %d done", __func__, __LINE__);
    return HDF_SUCCESS;
}

int32_t UsbPnpSampleTestDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%s: %d", __func__, __LINE__);

    UsbPnpSampleTestWriteLog("usb pnp sample device driver was loaded successfully");

    return HDF_SUCCESS;
}

void UsbPnpSampleTestDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%s: %d", __func__, __LINE__);

    (void)deviceObject;

    UsbPnpSampleTestWriteLog("usb pnp sample device driver was unloaded successfully");

    return;
}

struct HdfDriverEntry g_usbPnpSampleDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usb_pnp_sample_driver",
    .Bind          = UsbPnpSampleTestDriverBind,
    .Init          = UsbPnpSampleTestDriverInit,
    .Release       = UsbPnpSampleTestDriverRelease,
};

HDF_INIT(g_usbPnpSampleDriverEntry);
