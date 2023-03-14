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

#include "securec.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "hdf_io_service_if.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include "osal_mutex.h"
#include "signal.h"

#define TEST_WRITE              true
#define TEST_READ               false
#define SERVER_NAME_SDKAPI "usb_sdkapispeed_service"
#define SERVER_NAME_RAWAPI "usb_rawapispeed_service"
#define SERVER_NAME_NOSDK  "usb_nosdkspeed_service"

enum UsbSerialCmd {
    USB_SERIAL_OPEN = 0,
    USB_SERIAL_CLOSE,
    USB_SERIAL_SPEED,
};
struct UsbSpeedTest {
    int32_t busNum;
    int32_t devAddr;
    int32_t ifaceNum;
    int32_t writeOrRead;
    bool printData;
    int32_t paramNum;
};
enum speedServer {
    SDKAPI_SERVER = 0,
    RAWAPI_SERVER,
    NOSDK_SERVER,
};

static struct HdfIoService *g_service = NULL;
static struct HdfSBuf *g_data = NULL;
static struct HdfSBuf *g_reply = NULL;
static struct OsalMutex g_lock;
static enum speedServer spdserver = SDKAPI_SERVER;

static sigset_t mask;
pid_t stopHandlerTid;

void speedTest(struct UsbSpeedTest test)
{
    OsalMutexLock(&g_lock);
    HdfSbufFlush(g_data);
    bool bufok = HdfSbufWriteBuffer(g_data, (const void *)&test, sizeof(test));
    if (!bufok) {
        printf("HdfSbufWriteBuffer err");
        goto RET;
    }
    int32_t status = g_service->dispatcher->Dispatch(&g_service->object, USB_SERIAL_SPEED, g_data, g_reply);
    if (status < 0) {
        printf("%s: Dispatch USB_SERIAL_SPEED failed status = %d\n", __func__, status);
    }
RET:
    OsalMutexUnlock(&g_lock);
}

void speedInit()
{
    int32_t status;

    switch (spdserver) {
        case SDKAPI_SERVER:
            g_service = HdfIoServiceBind(SERVER_NAME_SDKAPI);
            break;
        case RAWAPI_SERVER:
            g_service = HdfIoServiceBind(SERVER_NAME_RAWAPI);
            break;
        case NOSDK_SERVER:
            g_service = HdfIoServiceBind(SERVER_NAME_NOSDK);
            break;
    }
    if (g_service == NULL || g_service->dispatcher == NULL || g_service->dispatcher->Dispatch == NULL) {
        printf("%s: GetService spdserver=%d err \n", __func__, spdserver);
        return;
    }

    g_data = HdfSbufObtain(2000);
    g_reply = HdfSbufObtain(2000);
    if (g_data == NULL || g_reply == NULL) {
        printf("%s: HdfSbufTypedObtain err", __func__);
        return;
    }

    status = g_service->dispatcher->Dispatch(&g_service->object, USB_SERIAL_OPEN, g_data, g_reply);
    if (status) {
        printf("%s: Dispatch USB_SERIAL_OPEN err status = %d\n", __func__, status);
        return;
    }

    if (OsalMutexInit(&g_lock) != HDF_SUCCESS) {
        printf("%s: init lock fail!", __func__);
        return;
    }
}

void speedExit()
{
    int32_t status = g_service->dispatcher->Dispatch(&g_service->object, USB_SERIAL_CLOSE, g_data, g_reply);
    if (status) {
        printf("%s: Dispatch USB_SERIAL_CLOSE err status = %d\n", __func__, status);
    }

    if (g_service != NULL) {
        HdfIoServiceRecycle(g_service);
        g_service = NULL;
    }
    HdfSbufRecycle(g_data);
    HdfSbufRecycle(g_reply);
}

static void ShowHelp(const char *name)
{
    printf(">> usage:\n");
    printf(">> %s <-SDK>/<-RAW>/<-NOSDK> [<busNum> <devAddr>]  <ifaceNum> <w>/<r>/<endpoint> [printdata]> \n", name);
    printf("\n");
}

static void *StopHandler(void)
{
    int32_t err, signo;
    stopHandlerTid = getpid();

    while (true) {
        err = sigwait(&mask, &signo);
        if (err != 0) {
            printf("Sigwait failed: %d\n", err);
        }

        if ((signo == SIGINT) || (signo == SIGQUIT)) {
            printf("normal exit\n");
            speedExit();
            return 0;
        } else {
            printf("Unexpected signal %d\n", signo);
        }
    }
}

static enum speedServer checkServer(const char* input)
{
    char middle[10] = {0};
    enum speedServer out;
    if (input == NULL) {
        HDF_LOGE("%s:%d input is NULL", __func__, __LINE__);
        out = SDKAPI_SERVER;
        return out;
    }

    int32_t ret = strncpy_s(middle, sizeof(middle), input, (uint32_t)strlen(input));
    if (ret != EOK) {
        HDF_LOGE("%s:%d strncpy_s failed", __func__, __LINE__);
        return out;
    }

    if (!strcmp(middle, "-SDK")) {
        out = SDKAPI_SERVER;
    } else if (!strcmp(middle, "-RAW")) {
        out = RAWAPI_SERVER;
    } else if (!strcmp(middle, "-NOSDK")) {
        out = NOSDK_SERVER;
    } else {
        out = SDKAPI_SERVER;
    }
    return out;
}

static int32_t GetWriteOrReadFlag(const char *buffer)
{
    int32_t writeOrRead;

    if (!strncmp(buffer, "r", 1)) {
        writeOrRead = TEST_READ;
    } else if (!strncmp(buffer, "w", 1)) {
        writeOrRead = TEST_WRITE;
    } else {
        writeOrRead = atoi(buffer);
    }

    return writeOrRead;
}

static int32_t CheckParam(int32_t argc, const char *argv[], struct UsbSpeedTest *speedTest)
{
    int32_t ret = HDF_SUCCESS;
    bool printData = false;
    int32_t paramNum;

    if ((argv == NULL) || (speedTest == NULL) || (argc <= 0)) {
        return HDF_ERR_INVALID_PARAM;
    }
    switch (argc) {
        case 7:
        case 6:
            spdserver = checkServer(argv[1]);
            speedTest->busNum = atoi(argv[2]);
            speedTest->devAddr = atoi(argv[3]);
            speedTest->ifaceNum = atoi(argv[4]);
            speedTest->writeOrRead = GetWriteOrReadFlag(argv[5]);
            if ((argc == 7) && (speedTest->writeOrRead == TEST_READ)) {
                printData = (strncmp(argv[6], "printdata", 1)) ? false : true;
            }
            break;
        case 4:
            spdserver = checkServer(argv[1]);
            speedTest->busNum = 1;
            speedTest->devAddr = 2;
            speedTest->ifaceNum = atoi(argv[2]);
            speedTest->writeOrRead = GetWriteOrReadFlag(argv[3]);
            break;
        default:
            printf("Error: parameter error!\n\n");
            ShowHelp(argv[0]);
            ret = HDF_FAILURE;
            break;
    }
    if (ret == HDF_SUCCESS) {
        paramNum = argc - 1;
        speedTest->printData = printData;
        speedTest->paramNum = paramNum;
    }

    return ret;
}

int32_t main(int32_t argc, char *argv[])
{
    int32_t ret;
    struct UsbSpeedTest test;

    ret = CheckParam(argc, argv, &test);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    pthread_t threads;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        printf("SIG_BLOCK error\n");
        ret = HDF_FAILURE;
        goto END;
    }
    if (pthread_create(&threads, NULL, StopHandler, NULL) != 0) {
        printf("Could not create core thread\n");
        ret = HDF_FAILURE;
        goto END;
    }

    speedInit();
    speedTest(test);
    kill(stopHandlerTid, SIGINT);
    pthread_join(threads, NULL);
END:
    return ret;
}
