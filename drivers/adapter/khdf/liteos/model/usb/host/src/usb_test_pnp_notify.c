/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "hdf_device_desc.h"

#define HDF_LOG_TAG USB_TEST_PNP_NOTIFY

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif

int32_t UsbTestPnpNotifyDispatch(struct HdfDeviceIoClient *client,
    int32_t cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;
    if (reply == NULL || client == NULL) {
        return HDF_FAILURE;
    }

    HDF_LOGD("%s:%d dispatch success, cmdId=%d", __func__, __LINE__, cmdId);

    return ret;
}

int32_t UsbTestPnpNotifyBind(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }
    static struct IDeviceIoService testService = {
        .Dispatch = UsbTestPnpNotifyDispatch,
        .Open = NULL,
        .Release = NULL,
    };
    deviceObject->service = &testService;
    return HDF_SUCCESS;
}

int32_t UsbTestPnpNotifyInit(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        HDF_LOGE("%s::ptr is null!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void UsbTestPnpNotifyRelease(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    HDF_LOGD("ZXX %s:%d release success", __func__, __LINE__);
    return;
}

struct HdfDriverEntry g_usbTestPnpNotifyEntry = {
    .moduleVersion = 1,
    .Bind = UsbTestPnpNotifyBind,
    .Init = UsbTestPnpNotifyInit,
    .Release = UsbTestPnpNotifyRelease,
    .moduleName = "usb_test_sample_driver",
};

HDF_INIT(g_usbTestPnpNotifyEntry);
