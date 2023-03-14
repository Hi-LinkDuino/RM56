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
#include <hdf_log.h>
#include <hdf_remote_service.h>
#include <hdf_sbuf.h>
#include <osal_time.h>
#include <servmgr_hdi.h>
#include <signal.h>
#include <unistd.h>
#include "cdcacm.h"
#include "usb_dev_test.h"

#define HDF_LOG_TAG   cdc_acm_speed_read

static struct HdfSBuf *g_data;
static struct HdfSBuf *g_reply;
static struct HdfRemoteService *g_acmService;
static bool g_readRuning = false;
static void TestSpeed()
{
    HdfSbufFlush(g_reply);
    int32_t status = g_acmService->dispatcher->Dispatch(g_acmService,
        USB_SERIAL_READ_SPEED, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE_SPEED failed status = %d",
            __func__, status);
        return;
    }
}

static void GetTempSpeed()
{
    float speed;
    HdfSbufFlush(g_reply);
    int32_t status = g_acmService->dispatcher->Dispatch(g_acmService,
        USB_SERIAL_READ_GET_TEMP_SPEED, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE_GET_TEMP_SPEED failed status = %d",
            __func__, status);
        return;
    }
    if (!HdfSbufReadFloat(g_reply, &speed)) {
        HDF_LOGE("%s: HdfSbufReadFloat failed", __func__);
        return;
    }
    if (speed > 0) {
        printf("speed : %f MB/s\n", speed);
    }
}

static void ReadSpeedDone()
{
    int32_t status = g_acmService->dispatcher->Dispatch(g_acmService,
        USB_SERIAL_READ_SPEED_DONE, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE_SPEED_DONE failed status = %d",
            __func__, status);
        return;
    }
}

void StopReadSpeedTest(int32_t signo)
{
    ReadSpeedDone();
    g_readRuning = false;
    printf("acm_speed_read exit.\n");
}

int32_t acm_speed_read(int32_t argc, const char *argv[])
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

    signal(SIGINT, StopReadSpeedTest);
    TestSpeed();
    g_readRuning = true;
    while (g_readRuning) {
        sleep(0x2);
        if (g_readRuning) {
            GetTempSpeed();
        }
    }

    status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_CLOSE, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_CLOSE err", __func__);
        return HDF_FAILURE;
    }

    HdfSbufRecycle(g_data);
    HdfSbufRecycle(g_reply);

    return 0;
}
