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

#include <hdf_log.h>
#include <hdf_remote_service.h>
#include <hdf_sbuf.h>
#include <servmgr_hdi.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include "cdcacm.h"
#include "osal_time.h"
#include "usb_dev_test.h"

#define HDF_LOG_TAG   cdc_acm_read

static struct HdfSBuf *g_data;
static struct HdfSBuf *g_reply;
static struct HdfRemoteService *g_acmService;

#define STR_LEN  8192
#define SLEEP_READ 100000
static void TestRead(FILE *fp)
{
    int32_t ret;
    char str[STR_LEN] = {0};
    struct timeval time;
    HdfSbufFlush(g_reply);
    int32_t status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_READ, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_READ failed status = %d", __func__, status);
        return;
    }
    const char *tmp = HdfSbufReadString(g_reply);
    if (tmp && strlen(tmp) > 0) {
        gettimeofday(&time, NULL);
        ret = snprintf_s(str, STR_LEN, STR_LEN - 1, "[XTSCHECK] %d.%06d, recv data[%s] from host\n",
            time.tv_sec, time.tv_usec, tmp);
        if (ret < 0) {
            HDF_LOGE("%s: snprintf_s failed", __func__);
            return;
        }
        (void)fwrite(str, strlen(str), 1, fp);
        fflush(fp);
    }
    usleep(SLEEP_READ);
}

int32_t acm_read(int32_t argc, const char *argv[])
{
    int32_t status;
    struct HDIServiceManager *servmgr = HDIServiceManagerGet();
    if (servmgr == NULL) {
        HDF_LOGE("%s: HDIServiceManagerGet err", __func__);
        return HDF_FAILURE;
    }
    g_acmService = servmgr->GetService(servmgr, "usbfn_cdcacm");
    HDIServiceManagerRelease(servmgr);
    if (g_acmService == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return HDF_FAILURE;
    }

    g_data = HdfSbufTypedObtain(SBUF_IPC);
    g_reply = HdfSbufTypedObtain(SBUF_IPC);
    if (g_data == NULL || g_reply == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return HDF_FAILURE;
    }

    status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_OPEN, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_OPEN err", __func__);
        return HDF_FAILURE;
    }
    FILE *fp = fopen("/data/acm_read_xts", "a+");
    if (fp == NULL) {
        HDF_LOGE("%s: fopen err", __func__);
        return HDF_FAILURE;
    }
    while (1) {
        TestRead(fp);
    }
    (void)fclose(fp);
    status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_CLOSE, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_CLOSE err", __func__);
        return HDF_FAILURE;
    }

    HdfSbufRecycle(g_data);
    HdfSbufRecycle(g_reply);

    return 0;
}
