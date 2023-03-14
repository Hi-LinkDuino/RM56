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

#include "hdf_log.h"
#include "osal_mem.h"
#include "hdf_io_service_if.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include "osal_thread.h"
#include "osal_mutex.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG   cdc_acm_write

enum UsbSerialCmd {
    USB_SERIAL_OPEN = 0,
    USB_SERIAL_CLOSE,
    USB_SERIAL_READ,
    USB_SERIAL_WRITE,
    USB_SERIAL_GET_BAUDRATE,
    USB_SERIAL_SET_BAUDRATE,
    USB_SERIAL_SET_PROP,
    USB_SERIAL_GET_PROP,
    USB_SERIAL_REGIST_PROP,
};
struct HdfSBuf *g_data;
struct HdfSBuf *g_reply;
struct HdfIoService *g_acmService;

int32_t check_service()
{
    if (g_acmService == NULL || g_acmService->dispatcher == NULL || g_acmService->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return HDF_FAILURE;
    }
    if (g_data == NULL || g_reply == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void acm_open()
{
    int32_t status;
    g_acmService = HdfIoServiceBind("usbfn_cdcacm");
    if (g_acmService == NULL || g_acmService->dispatcher == NULL || g_acmService->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    g_data = HdfSbufObtainDefaultSize();
    g_reply = HdfSbufObtainDefaultSize();
    if (g_data == NULL || g_reply == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
    }
    status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_OPEN, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_OPEN err", __func__);
    }
}

void acm_close()
{
    int32_t status;
    if (check_service()) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_CLOSE, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_CLOSE err", __func__);
    }
    HdfSbufRecycle(g_data);
    HdfSbufRecycle(g_reply);
    HdfIoServiceRecycle(g_acmService);
}

void acm_write(const char *buf)
{
    if (check_service()) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    HdfSbufFlush(g_data);
    (void)HdfSbufWriteString(g_data, buf);
    int32_t status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_WRITE, g_data, g_reply);
    if (status <= 0) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE failed status = %d", __func__, status);
        return;
    }
    printf("acm_write:%s\n", buf);
}

void acm_read(char *str, int32_t timeout)
{
    int32_t ret;
    uint32_t maxLen = 256;
    if (check_service()) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    while (timeout-- > 0) {
        HdfSbufFlush(g_reply);
        int32_t status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_READ, g_data, g_reply);
        if (status) {
            HDF_LOGE("%s: Dispatch USB_SERIAL_READ failed status = %d", __func__, status);
            return;
        }
        const char *tmp = HdfSbufReadString(g_reply);
        if (str && tmp && strlen(tmp) > 0) {
            ret = memcpy_s(str, maxLen, tmp, strlen(tmp));
            if (ret != EOK) {
                HDF_LOGE("%s:%d ret=%d memcpy_s error", __func__, __LINE__, ret);
            }
            printf("acm_read:%s\n", tmp);
            return;
        }
        sleep(1);
    }
}

void acm_prop_regist(const char *propName, const char *propValue)
{
    if (check_service()) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    HdfSbufFlush(g_data);
    (void)HdfSbufWriteString(g_data, propName);
    (void)HdfSbufWriteString(g_data, propValue);
    int32_t status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_REGIST_PROP, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE failed status = %d", __func__, status);
        return;
    }
    printf("prop_regist:%s = %s\n", propName, propValue);
}

void acm_prop_write(const char *propName, const char *propValue)
{
    if (check_service()) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    HdfSbufFlush(g_data);
    HdfSbufFlush(g_reply);
    (void)HdfSbufWriteString(g_data, propName);
    (void)HdfSbufWriteString(g_data, propValue);
    if (g_acmService == NULL || g_acmService->dispatcher == NULL || g_acmService->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    int32_t status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_SET_PROP, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE failed status = %d", __func__, status);
        return;
    }
    printf("prop_write:%s = %s\n", propName, propValue);
}

void acm_prop_read(const char *propName, char *propValue)
{
    uint32_t maxLen = 256;
    if (check_service()) {
        HDF_LOGE("%s: GetService err", __func__);
        return;
    }
    HdfSbufFlush(g_data);
    HdfSbufFlush(g_reply);
    (void)HdfSbufWriteString(g_data, propName);
    int32_t status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_GET_PROP, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s:%d Dispatch USB_SERIAL_GET_PROP failed status = %d", __func__, __LINE__, status);
        return;
    }
    const char *tmp = HdfSbufReadString(g_reply);
    if (propValue && tmp && strlen(tmp) > 0) {
        errno_t err = memcpy_s(propValue, maxLen, tmp, strlen(tmp));
        if (err != EOK) {
            HDF_LOGE("%s:%d err=%d memcpy_s error", __func__, __LINE__, err);
        }
        printf("prop_read:%s\n", tmp);
        return;
    }
}
