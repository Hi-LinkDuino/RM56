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
#include <unistd.h>
#include <sys/time.h>
#include <hdf_sbuf.h>
#include "securec.h"
#include "osal_atomic.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_device_desc.h"
#include "hdf_usb_pnp_manage.h"

#define HDF_LOG_TAG   USB_HOST_PNP_TEST

#if USB_PNP_NOTIFY_TEST_MODE == true
#define USB_HOST_PNP_TEST_SERVICE_NAME "hdf_usb_pnp_notify_service"
#define STR_LEN     1024
#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif
#define USB_TEST_INTERFACE_NUM  2

struct HdfSBuf *g_data;
struct HdfSBuf *g_reply;

static void TestPnpWriteLog(char *string)
{
    char str[STR_LEN] = {0};
    FILE *fp = NULL;
    struct timeval time;

    gettimeofday(&time, NULL);

    fp = fopen("/data/usbhost_pnp_xts", "a+");

    int32_t ret = snprintf_s(str, STR_LEN, STR_LEN - 1, "[XTSCHECK] %d.%06d, %s\n",
        time.tv_sec, time.tv_usec, string);
    if (ret < 0) {
        HDF_LOGE("%s: sbuf write failed", __func__);
        (void)fclose(fp);
        return;
    }

    (void)fwrite(str, strlen(str), 1, fp);
    (void)fclose(fp);
}

static void TestInitPnpInfo(enum UsbPnpNotifyServiceCmd cmdType)
{
    struct UsbPnpNotifyMatchInfoTable infoTable;
    uint8_t i;

    infoTable.usbDevAddr = 0;
    infoTable.devNum = 0;
    if (cmdType == USB_PNP_NOTIFY_REMOVE_TEST) {
        infoTable.busNum = -1;
    } else {
        infoTable.busNum = 0;
    }

    infoTable.deviceInfo.vendorId = 0xFFF0;
    infoTable.deviceInfo.productId = 0xFFF0;
    infoTable.deviceInfo.bcdDeviceLow = 0x0000;
    infoTable.deviceInfo.bcdDeviceHigh = 0x0000;
    infoTable.deviceInfo.deviceClass = 0;
    infoTable.deviceInfo.deviceSubClass = 0;
    infoTable.deviceInfo.deviceProtocol = 0;

    infoTable.removeType = USB_PNP_NOTIFY_REMOVE_BUS_DEV_NUM;

    if (cmdType != USB_PNP_NOTIFY_REMOVE_TEST) {
        infoTable.numInfos = USB_TEST_INTERFACE_NUM;
        for (i = 0; i < infoTable.numInfos; i++) {
            infoTable.interfaceInfo[i].interfaceClass = 0;
            infoTable.interfaceInfo[i].interfaceSubClass = 0;
            infoTable.interfaceInfo[i].interfaceProtocol = 0;
            infoTable.interfaceInfo[i].interfaceNumber = i;
        }
    }

    if (!HdfSbufWriteBuffer(g_data, (const void *)(&infoTable), sizeof(struct UsbPnpNotifyMatchInfoTable))) {
        HDF_LOGE("%s: sbuf write infoTable failed", __func__);
    }
}

static void TestPnpAdd(struct HdfIoService *serv)
{
    int32_t replyData = 0;
    bool flag = false;

    TestPnpWriteLog("usb pnp sample device driver test add start");

    TestInitPnpInfo(USB_PNP_NOTIFY_ADD_TEST);

    int32_t status = serv->dispatcher->Dispatch(&serv->object, USB_PNP_NOTIFY_ADD_TEST, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_PNP_NOTIFY_ADD_TEST failed status = %d", __func__, status);
        return;
    }

    flag = HdfSbufReadInt32(g_reply, &replyData);
    if ((!flag) || (replyData != INT32_MAX)) {
        TestPnpWriteLog("usb pnp sample device driver test add reply faile.");
    } else if (flag && replyData == INT32_MAX) {
        TestPnpWriteLog("usb pnp sample device driver test add reply ok.");
    }
}

static void TestPnpRemove(struct HdfIoService *serv)
{
    int32_t replyData = 0;
    bool flag = false;

    TestPnpWriteLog("usb pnp sample device driver test remove start");

    TestInitPnpInfo(USB_PNP_NOTIFY_REMOVE_TEST);

    int32_t status = serv->dispatcher->Dispatch(&serv->object, USB_PNP_NOTIFY_REMOVE_TEST, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_PNP_NOTIFY_REMOVE_TEST failed status = %d", __func__, status);
        return;
    }

    flag = HdfSbufReadInt32(g_reply, &replyData);
    if ((!flag) || (replyData != INT32_MAX)) {
        TestPnpWriteLog("usb pnp sample device driver test remove reply faile.");
    } else if (flag && replyData == INT32_MAX) {
        TestPnpWriteLog("usb pnp sample device driver test remove reply ok.");
    }
}
#endif
int32_t main(int32_t argc, char *argv[])
{
#if USB_PNP_NOTIFY_TEST_MODE == true
    HDF_LOGI("%s:%d usbhost pnp test start", __func__, __LINE__);
    char cmd;
    int32_t argNum = 2;
    if (argc != argNum) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (!strcmp(argv[1], "-add")) {
        cmd = 'a';
    } else if (!strcmp(argv[1], "-remove")) {
        cmd = 'r';
    } else {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_FAILURE;
    }

    struct HdfIoService *serv = HdfIoServiceBind(USB_HOST_PNP_TEST_SERVICE_NAME);
    if (serv == NULL) {
        HDF_LOGE("%s:%d fail to get service %s", \
            __func__, __LINE__, USB_HOST_PNP_TEST_SERVICE_NAME);
        return HDF_FAILURE;
    }

    g_data = HdfSbufObtainDefaultSize();
    g_reply = HdfSbufObtainDefaultSize();
    if (g_data == NULL || g_reply == NULL) {
        HDF_LOGE("%s:%d GetService err", __func__, __LINE__);
        return HDF_FAILURE;
    }

    switch (cmd) {
        case 'a':
            TestPnpAdd(serv);
            break;
        case 'r':
            TestPnpRemove(serv);
            break;
        default:
            break;
    }

    HdfSbufRecycle(g_data);
    HdfSbufRecycle(g_reply);

    HdfIoServiceRecycle(serv);

    HDF_LOGI("%s:%d usbhost pnp test end", __func__, __LINE__);
#else
    HDF_LOGE("%s:%d USB_PNP_NOTIFY_TEST_MODE is not support!!!", __func__, __LINE__);
#endif
    return HDF_SUCCESS;
}

