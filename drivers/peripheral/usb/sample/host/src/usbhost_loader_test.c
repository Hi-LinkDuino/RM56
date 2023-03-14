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

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <hdf_sbuf.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "hdf_usb_pnp_manage.h"

#define HDF_LOG_TAG   USB_HOST_PNP_TEST

#define USB_HOST_PNP_TEST_SERVICE_NAME  "hdf_usb_pnp_notify_service"
#define USB_TEST_SAMPLE_MODULE_NAME     "usb_test_sample_driver"
#define USB_TEST_SAMPLE_SERVICE_NAME    "usb_test_sample_service"

#define STR_LEN     1024
#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif
#define USB_TEST_INTERFACE_NUM  2

struct HdfSBuf *g_data;
struct HdfSBuf *g_reply;

static int32_t UsbPnpTestEventReceived(void *priv, uint32_t id, struct HdfSBuf *data)
{
    int32_t ret;

    switch (id) {
        default:
            ret = HDF_ERR_INVALID_PARAM;
            break;
    }

    HDF_LOGI("%s:%d ret=%d DONE", __func__, __LINE__, ret);

    return ret;
}

int32_t main(int32_t argc, char *argv[])
{
    HDF_LOGI("%s:%d usbhost pnp test start", __func__, __LINE__);
    int32_t ret;
    struct HdfIoService *testService = NULL;
    static struct HdfDevEventlistener usbPnpTestListener = {
        .callBack = UsbPnpTestEventReceived,
    };

    struct HdfIoService *serv = HdfIoServiceBind(USB_HOST_PNP_TEST_SERVICE_NAME);
    if (serv == NULL) {
        HDF_LOGE("%s:%d fail to get service %s", \
            __func__, __LINE__, USB_HOST_PNP_TEST_SERVICE_NAME);
        return HDF_FAILURE;
    }

    ret = HdfDeviceRegisterEventListener(serv, &usbPnpTestListener);
    if (ret != HDF_SUCCESS) {
        HdfIoServiceRecycle(serv);
        HDF_LOGE("HdfDeviceRegisterEventListener faile ret=%d", ret);
        return HDF_FAILURE;
    }

    g_data = HdfSbufObtainDefaultSize();
    g_reply = HdfSbufObtainDefaultSize();
    if (g_data == NULL || g_reply == NULL) {
        HdfIoServiceRecycle(serv);
        HDF_LOGE("%s:%d GetService err", __func__, __LINE__);
        return HDF_FAILURE;
    }

    HdfSbufWriteString(g_data, USB_TEST_SAMPLE_MODULE_NAME);
    HdfSbufWriteString(g_data, USB_TEST_SAMPLE_SERVICE_NAME);

    ret = serv->dispatcher->Dispatch(&serv->object, USB_PNP_DRIVER_REGISTER_DEVICE, g_data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d Dispatch USB_PNP_DRIVER_REGISTER_DEVICE err", __func__, __LINE__);
        goto OUT;
    }
    HDF_LOGI("%s:%d", __func__, __LINE__);
    testService = HdfIoServiceBind(USB_TEST_SAMPLE_SERVICE_NAME);
    if (testService == NULL) {
        HDF_LOGE("%s:%d testService USB_PNP_DRIVER_REGISTER_DEVICE err", __func__, __LINE__);
        goto OUT;
    }

    HDF_LOGI("%s:%d", __func__, __LINE__);
    ret = serv->dispatcher->Dispatch(&serv->object, USB_PNP_DRIVER_UNREGISTER_DEVICE, g_data, NULL);
    if (ret != HDF_SUCCESS) {
        HdfIoServiceRecycle(testService);
        HDF_LOGE("%s:%d Dispatch USB_PNP_DRIVER_UNREGISTER_DEVICE err", __func__, __LINE__);
        goto OUT;
    }

    HDF_LOGI("%s:%d", __func__, __LINE__);

OUT:
    HdfSbufRecycle(g_data);
    HdfSbufRecycle(g_reply);

    HdfIoServiceRecycle(serv);

    HDF_LOGI("%s:%d usbhost pnp test end", __func__, __LINE__);
    return HDF_SUCCESS;
}

