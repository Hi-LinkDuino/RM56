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

#include <fcntl.h>
#include <hdf_io_service_if.h>
#include <hdf_log.h>
#include <osal_file.h>
#include <osal_mem.h>
#include <osal_mutex.h>
#include <osal_thread.h>
#include <osal_time.h>
#include <pthread.h>
#include <securec.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define HDF_LOG_TAG   cdc_acm_speed

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
    USB_SERIAL_WRITE_SPEED,
    USB_SERIAL_WRITE_GET_TEMP_SPEED,
    USB_SERIAL_WRITE_SPEED_DONE,
    USB_SERIAL_WRITE_GET_TEMP_SPEED_UINT32,
};

static struct HdfSBuf *g_data;
static struct HdfSBuf *g_reply;
static struct HdfIoService *g_acmService;
static bool g_readRuning = false;
static sigset_t g_mask;

static void TestSpeed(void)
{
    HdfSbufFlush(g_reply);
    int32_t status = g_acmService->dispatcher->Dispatch(&g_acmService->object,
        USB_SERIAL_WRITE_SPEED, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE_SPEED failed status = %d",
            __func__, status);
        return;
    }
}

static void GetTempSpeed(void)
{
    const float calc = 10000;
    uint32_t speed = 0;
    HdfSbufFlush(g_reply);
    int32_t status = g_acmService->dispatcher->Dispatch(&g_acmService->object,
        USB_SERIAL_WRITE_GET_TEMP_SPEED_UINT32, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE_GET_TEMP_SPEED failed status = %d",
            __func__, status);
        return;
    }
    if (!HdfSbufReadUint32(g_reply, &speed)) {
        HDF_LOGE("%s: HdfSbufReadFloat failed", __func__);
        return;
    }
    if (speed > 0) {
        printf("speed : %f MB/s\n", (float)speed / calc);
    }
}

static void WriteSpeedDone(void)
{
    int32_t status = g_acmService->dispatcher->Dispatch(g_acmService,
        USB_SERIAL_WRITE_SPEED_DONE, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE_SPEED_DONE failed status = %d",
            __func__, status);
        return;
    }
}

static void *StopHandler(void *arg)
{
    int32_t err, signo;
    while (1) {
        err = sigwait(&g_mask, &signo);
        if (err != 0) {
            printf("Sigwait failed: %d\n", err);
        }

        switch (signo) {
            case SIGINT:
            case SIGQUIT:
                printf("acm_speed_write exit\n");
                WriteSpeedDone();
                g_readRuning = false;
                return NULL;
            default:
                printf("Unexpected signal %d\n", signo);
        }
    }
}

static pthread_t g_threads;
static void StartStopHandler(void)
{
    sigemptyset(&g_mask);
    sigaddset(&g_mask, SIGINT);
    sigaddset(&g_mask, SIGQUIT);
    if ((pthread_sigmask(SIG_BLOCK, &g_mask, NULL)) != 0) {
        printf("SIG_BLOCK error\n");
        return;
    }
    if (pthread_create(&g_threads, NULL, StopHandler, NULL) != 0) {
        printf("Could not create core thread\n");
        return;
    }
}

int32_t acm_speed_write(int32_t argc, const char *argv[])
{
    int32_t status;

    g_acmService = HdfIoServiceBind("usbfn_cdcacm");
    if (g_acmService == NULL || g_acmService->dispatcher == NULL || g_acmService->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return HDF_FAILURE;
    }

    g_data = HdfSbufObtainDefaultSize();
    g_reply = HdfSbufObtainDefaultSize();
    if (g_data == NULL || g_reply == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return HDF_FAILURE;
    }

    status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_OPEN, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_OPEN err", __func__);
        return HDF_FAILURE;
    }
    StartStopHandler();
    TestSpeed();
    g_readRuning = true;
    while (g_readRuning) {
        sleep(0x2);
        if (g_readRuning) {
            GetTempSpeed();
        }
    }

    status = g_acmService->dispatcher->Dispatch(&g_acmService->object, USB_SERIAL_CLOSE, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_CLOSE err", __func__);
        return HDF_FAILURE;
    }

    HdfSbufRecycle(g_data);
    HdfSbufRecycle(g_reply);
    HdfIoServiceRecycle(g_acmService);
    return 0;
}
