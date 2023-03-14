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
#include <stdlib.h>
#include <termios.h>
#include "cdcacm.h"
#include "osal_time.h"
#include <signal.h>
#include "usb_dev_test.h"

#define HDF_LOG_TAG   cdc_acm_test

static int32_t running = 1;
static struct termios g_orgOpts, g_newOpts;
static struct HdfSBuf *g_data;
static struct HdfSBuf *g_reply;
static struct HdfRemoteService *g_acmService;
static struct OsalThread      g_thread;

static void TestWrite(char *buf)
{
    HdfSbufFlush(g_data);

    if (HdfRemoteServiceWriteInterfaceToken(g_acmService, g_data) == false) {
        HDF_LOGE("%{public}s:%{public}d write interface token fail\n", __func__, __LINE__);
        return;
    }
    
    (void)HdfSbufWriteString(g_data, buf);
    int32_t status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_WRITE, g_data, g_reply);
    if (status <= 0) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_WRITE failed status = %d", __func__, status);
    }
}

static void TestRead()
{
    size_t i;
    HdfSbufFlush(g_reply);

    if (HdfRemoteServiceWriteInterfaceToken(g_acmService, g_data) == false) {
        HDF_LOGE("%{public}s:%{public}d write interface token fail\n", __func__, __LINE__);
        return;
    }

    int32_t status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_READ, g_data, g_reply);
    if (status) {
        printf("%s: Dispatch USB_SERIAL_READ failed status = %d", __func__, status);
        return;
    }
    const char *tmp = HdfSbufReadString(g_reply);
    if (tmp && strlen(tmp) > 0) {
        for (i = 0; i < strlen(tmp); i++) {
            if (tmp[i] == 0x0A || tmp[i] == 0x0D) {
                printf("\r\n");
            } else {
                putchar(tmp[i]);
            }
        }
        fflush(stdout);
    }
}

#define SLEEP_100MS 100000
static int32_t ReadThread(void *arg)
{
    while (running) {
        TestRead();
        usleep(SLEEP_100MS);
    }
    return 0;
}
#define HDF_PROCESS_STACK_SIZE 10000
static int32_t StartThreadRead()
{
    int32_t ret;
    struct OsalThreadParam threadCfg;
    memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name = "Read process";
    threadCfg.priority = OSAL_THREAD_PRI_DEFAULT;
    threadCfg.stackSize = HDF_PROCESS_STACK_SIZE;

    ret = OsalThreadCreate(&g_thread, (OsalThreadEntry)ReadThread, NULL);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadCreate faile, ret=%d ", __func__, __LINE__, ret);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = OsalThreadStart(&g_thread, &threadCfg);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadStart faile, ret=%d ", __func__, __LINE__, ret);
        return HDF_ERR_DEVICE_BUSY;
    }
    return 0;
}

static void SetTermios()
{
    tcgetattr(STDIN_FILENO, &g_orgOpts);
    tcgetattr(STDIN_FILENO, &g_newOpts);
    g_newOpts.c_lflag &= ~(ICANON | ECHOE | ECHOK | ECHONL);
    tcsetattr(STDIN_FILENO, TCSANOW, &g_newOpts);
}

#define STR_LEN 256
static void WriteThread()
{
    char str[STR_LEN] = {0};
    while (running) {
        str[0] = getchar();
        if (running) {
            TestWrite(str);
        }
    }
}

void StopAcmTest(int32_t signo)
{
    int32_t status;
    running = 0;
    status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_CLOSE, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_CLOSE err", __func__);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &g_orgOpts);
    printf("acm_test exit.\n");
}

int32_t acm_test(int32_t argc, const char *argv[])
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

    if (HdfRemoteServiceSetInterfaceDesc(g_acmService, "hdf.usb.usbfn") == false) {
        HDF_LOGE("%{public}s:%{public}d set desc fail\n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    g_data = HdfSbufTypedObtain(SBUF_IPC);
    g_reply = HdfSbufTypedObtain(SBUF_IPC);
    if (g_data == NULL || g_reply == NULL) {
        HDF_LOGE("%s: GetService err", __func__);
        return HDF_FAILURE;
    }

    if (HdfRemoteServiceWriteInterfaceToken(g_acmService, g_data) == false) {
        HDF_LOGE("%{public}s:%{public}d write interface token fail\n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    status = g_acmService->dispatcher->Dispatch(g_acmService, USB_SERIAL_OPEN, g_data, g_reply);
    if (status) {
        HDF_LOGE("%s: Dispatch USB_SERIAL_OPEN err", __func__);
        return HDF_FAILURE;
    }
    printf("Press any key to send.\n");
    printf("Press CTRL-C to exit.\n");

    signal(SIGINT, StopAcmTest);
    StartThreadRead();
    SetTermios();
    WriteThread();
    return 0;
}
