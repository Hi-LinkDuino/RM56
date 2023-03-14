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

#include "usbhost_sdkraw_speed.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <osal_thread.h>
#include <inttypes.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_usb_pnp_manage.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usb_ddk_interface.h"

#define HDF_LOG_TAG                USB_HOST_ACM_RAW_API

#define USB_CTRL_REQ_SIZE          64
#define USB_IO_THREAD_STACK_SIZE   8192
#define USB_RAW_IO_SLEEP_MS_TIME        500
#define USB_RAW_IO_STOP_WAIT_MAX_TIME   2

static struct AcmDevice *g_acm = NULL;
static bool g_stopIoThreadFlag = false;
static unsigned int g_speedFlag = 0;
static uint64_t g_send_count = 0;
static uint64_t g_recv_count = 0;
static uint64_t g_byteTotal = 0;
static bool g_writeOrRead = TEST_WRITE;
static bool g_printData = false;
static struct OsalSem sem;

static void AcmTestBulkCallback(const void *requestArg);
static int32_t SerialBegin(struct AcmDevice *acm);

static int32_t UsbIoThread(void *data)
{
    int32_t ret;
    struct AcmDevice *acm = (struct AcmDevice *)data;

    for (;;) {
        if (acm == NULL) {
            HDF_LOGE("%s:%d acm is NULL", __func__, __LINE__);
            OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            continue;
        }

        if (acm->devHandle == NULL) {
            HDF_LOGE("%s:%d acm->devHandle is NULL!", __func__, __LINE__);
            OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            continue;
        }

        ret = UsbRawHandleRequests(acm->devHandle);
        if (ret < 0) {
            HDF_LOGE("%s:%d UsbRawHandleRequests failed, ret=%d ", \
                __func__, __LINE__, ret);
            if (ret == USB_REQUEST_NO_DEVICE) {
                HDF_LOGE("%s:%d, ret=%d", __func__, __LINE__, ret);
                OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
            }
        }

        if (g_stopIoThreadFlag) {
            HDF_LOGD("%s:%d", __func__, __LINE__);
            g_stopIoThreadFlag = false;
            break;
        }
    }

    HDF_LOGE("%s:%d exit", __func__, __LINE__);

    return HDF_SUCCESS;
}

static int32_t UsbIoSendThread(void *data)
{
    struct AcmDevice *acm = (struct AcmDevice *)data;

    for (;;) {
        OsalSemWait(&sem, HDF_WAIT_FOREVER);
        if (!g_speedFlag) {
            if (SerialBegin(acm) != HDF_SUCCESS) {
                HDF_LOGW("%s:%d SerialBegin error!", __func__, __LINE__);
            }
            g_send_count++;
        }
    }
}

static int32_t UsbStartIo(struct AcmDevice *acm)
{
    struct OsalThreadParam threadCfg;
    int32_t ret;

    HDF_LOGI("%s start", __func__);

    /* creat Io thread */
    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name      = "usb io thread";
    threadCfg.priority  = OSAL_THREAD_PRI_DEFAULT;
    threadCfg.stackSize = USB_IO_THREAD_STACK_SIZE;

    ret = OsalThreadCreate(&acm->ioThread, \
                           (OsalThreadEntry)UsbIoThread, (void *)acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadCreate faile, ret=%d ",
                 __func__, __LINE__, ret);
        return ret;
    }

    ret = OsalThreadStart(&acm->ioThread, &threadCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadStart faile, ret=%d ",
                 __func__, __LINE__, ret);
        return ret;
    }

    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name      = "usb io send thread";
    threadCfg.priority  = OSAL_THREAD_PRI_DEFAULT;
    threadCfg.stackSize = USB_IO_THREAD_STACK_SIZE;
    ret = OsalThreadCreate(&acm->ioSendThread, (OsalThreadEntry)UsbIoSendThread, (void *)acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadCreate faile, ret=%d ",
                 __func__, __LINE__, ret);
        return ret;
    }

    ret = OsalThreadStart(&acm->ioSendThread, &threadCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadStart faile, ret=%d ",
                 __func__, __LINE__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t UsbStopIo(struct AcmDevice *acm)
{
    int32_t ret;
    int32_t i = 0;

    HDF_LOGD("%s:%d", __func__, __LINE__);
    if (!g_stopIoThreadFlag) {
        HDF_LOGD("%s:%d", __func__, __LINE__);
        g_stopIoThreadFlag = true;
    }
    HDF_LOGD("%s:%d", __func__, __LINE__);

    while (g_stopIoThreadFlag) {
        i++;
        OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
        if (i > USB_RAW_IO_STOP_WAIT_MAX_TIME) {
            HDF_LOGD("%s:%d", __func__, __LINE__);
            g_stopIoThreadFlag = false;
        }
    }
    HDF_LOGD("%s:%d", __func__, __LINE__);

    ret = OsalThreadDestroy(&acm->ioThread);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadDestroy faile, ret=%d ", __func__, __LINE__, ret);
        return ret;
    }

    ret = OsalThreadDestroy(&acm->ioSendThread);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadDestroy faile, ret=%d ",
                 __func__, __LINE__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t UsbGetConfigDescriptor(UsbRawHandle *devHandle, struct UsbRawConfigDescriptor **config)
{
    UsbRawDevice *dev = NULL;
    int32_t activeConfig;
    int32_t ret;

    if (devHandle == NULL) {
        HDF_LOGE("%s:%d devHandle is NULL",
                 __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = UsbRawGetConfiguration(devHandle, &activeConfig);
    if (ret) {
        HDF_LOGE("%s:%d UsbRawGetConfiguration failed, ret=%d",
                 __func__, __LINE__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGE("%s:%d activeConfig=%d", __func__, __LINE__, activeConfig);
    dev = UsbRawGetDevice(devHandle);
    if (dev == NULL) {
        HDF_LOGE("%s:%d UsbRawGetDevice failed",
                 __func__, __LINE__);
        return HDF_FAILURE;
    }

    ret = UsbRawGetConfigDescriptor(dev, activeConfig, config);
    if (ret) {
        HDF_LOGE("UsbRawGetConfigDescriptor failed, ret=%d\n", ret);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UsbGetBulkEndpoint(struct AcmDevice *acm, const struct UsbRawEndpointDescriptor *endPoint)
{
    if ((endPoint->endpointDescriptor.bEndpointAddress & USB_DDK_ENDPOINT_DIR_MASK) == USB_DDK_DIR_IN) {
        /* get bulk in endpoint */
        acm->dataInEp = OsalMemAlloc(sizeof(struct UsbEndpoint));
        if (acm->dataInEp == NULL) {
            HDF_LOGE("%s:%d allocate dataInEp failed", __func__, __LINE__);
            return HDF_FAILURE;
        }
        acm->dataInEp->addr = endPoint->endpointDescriptor.bEndpointAddress;
        acm->dataInEp->interval = endPoint->endpointDescriptor.bInterval;
        acm->dataInEp->maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
    } else {
        /* get bulk out endpoint */
        acm->dataOutEp = OsalMemAlloc(sizeof(struct UsbEndpoint));
        if (acm->dataOutEp == NULL) {
            HDF_LOGE("%s:%d allocate dataOutEp failed", __func__, __LINE__);
            return HDF_FAILURE;
        }
        acm->dataOutEp->addr = endPoint->endpointDescriptor.bEndpointAddress;
        acm->dataOutEp->interval = endPoint->endpointDescriptor.bInterval;
        acm->dataOutEp->maxPacketSize = endPoint->endpointDescriptor.wMaxPacketSize;
    }

    return HDF_SUCCESS;
}

static void UsbParseConfigDescriptorProcess(struct AcmDevice *acm,
    const struct UsbRawInterface *interface, uint8_t interfaceIndex)
{
    uint8_t ifaceClass = interface->altsetting->interfaceDescriptor.bInterfaceClass;
    uint8_t numEndpoints = interface->altsetting->interfaceDescriptor.bNumEndpoints;

    switch (ifaceClass) {
        case USB_DDK_CLASS_COMM:
            acm->ctrlIface = interfaceIndex;
            acm->notifyEp = OsalMemAlloc(sizeof(struct UsbEndpoint));
            if (acm->notifyEp == NULL) {
                HDF_LOGE("%s:%d allocate endpoint failed", __func__, __LINE__);
                break;
            }
            /* get the first endpoint by default */
            acm->notifyEp->addr = interface->altsetting->endPoint[0].endpointDescriptor.bEndpointAddress;
            acm->notifyEp->interval = interface->altsetting->endPoint[0].endpointDescriptor.bInterval;
            acm->notifyEp->maxPacketSize = interface->altsetting->endPoint[0].endpointDescriptor.wMaxPacketSize;
            break;
        case USB_DDK_CLASS_CDC_DATA:
            acm->dataIface = interfaceIndex;
            for (uint8_t j = 0; j < numEndpoints; j++) {
                const struct UsbRawEndpointDescriptor *endPoint = &interface->altsetting->endPoint[j];
                if (UsbGetBulkEndpoint(acm, endPoint) != HDF_SUCCESS) {
                    break;
                }
            }
            break;
        default:
            HDF_LOGE("%s:%d wrong descriptor type", __func__, __LINE__);
            break;
    }
}

static int32_t UsbParseConfigDescriptor(struct AcmDevice *acm, struct UsbRawConfigDescriptor *config)
{
    uint8_t i;
    int32_t ret;

    if ((acm == NULL) || (config == NULL)) {
        HDF_LOGE("%s:%d acm or config is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i < acm->interfaceCnt; i++) {
        uint8_t interfaceIndex = acm->interfaceIndex[i];
        const struct UsbRawInterface *interface = config->interface[interfaceIndex];

        ret = UsbRawClaimInterface(acm->devHandle, interfaceIndex);
        if (ret) {
            HDF_LOGE("%s:%d claim interface %u failed", __func__, __LINE__, i);
            return ret;
        }

        UsbParseConfigDescriptorProcess(acm, interface, interfaceIndex);
    }

    return HDF_SUCCESS;
}

static int32_t UsbAllocDataRequests(struct AcmDevice *acm)
{
    int32_t i;
    int32_t ret;
    for (i = 0; i < TEST_CYCLE; i++) {
        struct AcmDb *snd = &acm->db[i];
        snd->request = UsbRawAllocRequest(acm->devHandle, 0, acm->dataEp->maxPacketSize);
        snd->instance = acm;
        if (snd->request == NULL) {
            HDF_LOGE("%s: UsbRawAllocRequest failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
        struct UsbRawFillRequestData reqData;
        reqData.endPoint      = acm->dataEp->addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmTestBulkCallback;
        reqData.userData      = (void *)snd;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.buffer        = snd->buf;
        reqData.length        = acm->dataSize;

        ret = UsbRawFillBulkRequest(snd->request, acm->devHandle, &reqData);
        if (ret) {
            HDF_LOGE("%s: FillInterruptRequest faile, ret=%d",
                     __func__, ret);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}
static int32_t UsbSerialDeviceAlloc(struct AcmDevice *acm)
{
    struct SerialDevice *port = NULL;

    if (acm == NULL) {
        HDF_LOGE("%s: acm null pointer", __func__);
        return HDF_FAILURE;
    }

    port = (struct SerialDevice *)OsalMemCalloc(sizeof(*port));
    if (port == NULL) {
        HDF_LOGE("%s: Alloc usb serial port failed", __func__);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&port->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        return HDF_FAILURE;
    }
    port->lineCoding.dwDTERate   = CpuToLe32(DATARATE);
    port->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    port->lineCoding.bParityType = USB_CDC_NO_PARITY;
    port->lineCoding.bDataBits   = DATA_BITS_LENGTH;
    acm->lineCoding = port->lineCoding;
    acm->port = port;
    port->acm = acm;

    return HDF_SUCCESS;
}

static int32_t AcmDbAlloc(struct AcmDevice *acm)
{
    struct AcmDb *db = NULL;
    int32_t i;

    for (i = 0; i < TEST_CYCLE; i++) {
        db = &acm->db[i];
        if (!db->use) {
            db->use = 1;
            db->len = 0;
            return i;
        }
    }
    return -1;
}

static int32_t AcmDbIsAvail(struct AcmDevice *acm)
{
    int32_t i;
    int32_t n = TEST_CYCLE;

    for (i = 0; i < TEST_CYCLE; i++) {
        n -= acm->db[i].use;
    }
    return n;
}

static int32_t AcmStartdb(struct AcmDevice *acm, struct AcmDb *db)
{
    int32_t ret;
    ret = UsbRawSubmitRequest(db->request);
    if (ret) {
        HDF_LOGE("UsbRawSubmitRequest failed, ret=%d", ret);
        db->use = 0;
    }
    return ret;
}

static int32_t AcmDataBufAlloc(struct AcmDevice *acm)
{
    struct AcmDb *db = &acm->db[0];
    int32_t i;

    for (i = 0; i < TEST_CYCLE; i++, db++) {
        db->buf = OsalMemCalloc(acm->dataEp->maxPacketSize);
        if (!db->buf) {
            while (i != 0) {
                --i;
                --db;
                OsalMemFree(db->buf);
                db->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        }
        else {
            memset_s(db->buf, acm->dataSize, 'b', acm->dataSize);
            db->instance = acm;
        }
    }
    return HDF_SUCCESS;
}

static void AcmTestBulkCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;
    struct itimerval new_value, old_value;
    if (req == NULL) {
        HDF_LOGE("%s:%{pulib}d req is NULL!", __func__, __LINE__);
        return;
    }
    struct AcmDb *db  = (struct AcmDb *)req->userData;
    if (db == NULL) {
        HDF_LOGE("%s:%{pulib}d userData(db) is NULL!", __func__, __LINE__);
        return;
    }

    if (req->status == USB_REQUEST_COMPLETED) {
        if (g_byteTotal == 0) {
            new_value.it_value.tv_sec = TEST_PRINT_TIME;
            new_value.it_value.tv_usec = 0;
            new_value.it_interval.tv_sec = TEST_PRINT_TIME;
            new_value.it_interval.tv_usec = 0;
            setitimer(ITIMER_REAL, &new_value, &old_value);
        }
        g_recv_count++;
        g_byteTotal += req->actualLength;
    } else {
        printf("status error\n");
    }

    if (g_printData) {
        for (int32_t i = 0; i < req->actualLength; i++)
            printf("%c", req->buffer[i]);
        fflush(stdout);
    } else if (g_recv_count % TEST_RECV_COUNT == 0) {
        printf("#");
        fflush(stdout);
    }

    db->use = 0;
    OsalSemPost(&sem);
}

static int32_t SerialBegin(struct AcmDevice *acm)
{
    uint32_t size = acm->dataSize;
    int32_t ret;
    struct AcmDb *db = NULL;
    int32_t dbn;
    if (AcmDbIsAvail(acm)) {
        dbn = AcmDbAlloc(acm);
    } else {
        HDF_LOGE("no buf\n");
        return 0;
    }
    if (dbn < 0) {
        HDF_LOGE("AcmDbAlloc failed\n");
        return HDF_FAILURE;
    }
    db = &acm->db[dbn];
    db->len = acm->dataSize;
    if (g_writeOrRead == TEST_READ) {
        memset_s(db->buf, TEST_LENGTH, '0', TEST_LENGTH);
    }
    struct UsbRawFillRequestData reqData;
    reqData.endPoint      = acm->dataEp->addr;
    reqData.numIsoPackets = 0;
    reqData.callback      = AcmTestBulkCallback;
    reqData.userData      = (void *)db;
    reqData.timeout       = USB_CTRL_SET_TIMEOUT;
    reqData.buffer        = db->buf;
    reqData.length        = acm->dataSize;

    ret = UsbRawFillBulkRequest(db->request, acm->devHandle, &reqData);
    if (ret) {
        HDF_LOGE("%s: FillInterruptRequest faile, ret=%d",
                 __func__, ret);
        return HDF_FAILURE;
    }

    ret = AcmStartdb(acm, db);
    return size;
}

void SignalHandler(int32_t signo)
{
    static uint32_t sigCnt = 0;
    struct itimerval new_value, old_value;
    double speed = 0;
    switch (signo) {
        case SIGALRM:
            sigCnt++;
            if (sigCnt * TEST_PRINT_TIME >= TEST_TIME) {
                g_speedFlag = 1;
                break;
            }

            speed = (g_byteTotal * TEST_FLOAT_COUNT) / (sigCnt * TEST_PRINT_TIME * TEST_BYTE_COUNT * TEST_BYTE_COUNT);
            printf("\nSpeed:%f MB/s\n", speed);
            new_value.it_value.tv_sec = TEST_PRINT_TIME;
            new_value.it_value.tv_usec = 0;
            new_value.it_interval.tv_sec = TEST_PRINT_TIME;
            new_value.it_interval.tv_usec = 0;
            setitimer(ITIMER_REAL, &new_value, &old_value);
            break;
        case SIGINT:
            g_speedFlag = 1;
            break;
        default:
            break;
    }
}

static void ShowHelp(const char *name)
{
    printf(">> usage:\n");
    printf(">>      %s [<busNum> <devAddr>]  <ifaceNum> <w>/<r> [printdata]> \n", name);
    printf("\n");
}

int32_t CheckParam(int32_t argc, const char *argv[])
{
    int32_t busNum = 1;
    int32_t devAddr = 2;
    int32_t ifaceNum = 3;
    int32_t ret = HDF_SUCCESS;

    if (argc == TEST_SIX_TYPE) {
        busNum = atoi(argv[TEST_ONE_TYPE]);
        devAddr = atoi(argv[TEST_TWO_TYPE]);
        ifaceNum = atoi(argv[TEST_THREE_TYPE]);
        g_writeOrRead = (strncmp(argv[TEST_FOUR_TYPE], "r", TEST_ONE_TYPE)) ? TEST_WRITE : TEST_READ;
        if (g_writeOrRead == TEST_READ) {
            g_printData = (strncmp(argv[TEST_FIVE_TYPE], "printdata", TEST_ONE_TYPE)) ? false : true;
        }
    } else if (argc == TEST_FIVE_TYPE) {
        busNum = atoi(argv[TEST_ONE_TYPE]);
        devAddr = atoi(argv[TEST_TWO_TYPE]);
        ifaceNum = atoi(argv[TEST_THREE_TYPE]);
        g_writeOrRead = (strncmp(argv[TEST_FOUR_TYPE], "r", TEST_ONE_TYPE)) ? TEST_WRITE : TEST_READ;
    } else if (argc == TEST_THREE_TYPE) {
        ifaceNum = atoi(argv[TEST_ONE_TYPE]);
        g_writeOrRead = (strncmp(argv[TEST_TWO_TYPE], "r", TEST_ONE_TYPE)) ? TEST_WRITE : TEST_READ;
    } else {
        printf("Error: parameter error!\n\n");
        ShowHelp(argv[TEST_ZERO_TYPE]);
        ret = HDF_FAILURE;
        goto END;
    }
    OsalSemInit(&sem, 0);

    g_acm = (struct AcmDevice *)OsalMemCalloc(sizeof(*g_acm));
    if (g_acm == NULL) {
        HDF_LOGE("%s: Alloc usb serial device failed", __func__);
        ret = HDF_FAILURE;
        goto END;
    }

    g_acm->busNum = busNum;
    g_acm->devAddr = devAddr;
    g_acm->interfaceCnt = 1;
    g_acm->interfaceIndex[0] = ifaceNum;
END:
    return ret;
}

int32_t InitUsbDdk(void)
{
    int32_t ret;
    struct UsbSession *session = NULL;
    UsbRawHandle *devHandle = NULL;

    ret = UsbRawInit(&session);
    if (ret) {
        HDF_LOGE("%s: UsbRawInit failed", __func__);
        goto END;
    }

    ret = UsbSerialDeviceAlloc(g_acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialDeviceAlloc failed", __func__);
        goto END;
    }

    devHandle = UsbRawOpenDevice(session, g_acm->busNum, g_acm->devAddr);
    if (devHandle == NULL) {
        HDF_LOGE("%s: UsbRawOpenDevice failed", __func__);
        ret =  HDF_FAILURE;
        goto END;
    }
    g_acm->devHandle = devHandle;
    ret = UsbGetConfigDescriptor(devHandle, &g_acm->config);
    if (ret) {
        HDF_LOGE("%s: UsbGetConfigDescriptor failed", __func__);
        goto END;
    }
    ret = UsbParseConfigDescriptor(g_acm, g_acm->config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbParseConfigDescriptor failed", __func__);
        ret = HDF_FAILURE;
        goto END;
    }
    g_acm->dataSize = TEST_LENGTH;
    g_acm->dataEp = (g_writeOrRead == TEST_WRITE) ? g_acm->dataOutEp : g_acm->dataInEp;

    ret = AcmDataBufAlloc(g_acm);
    if (ret < 0) {
        HDF_LOGE("%s: AcmDataBufAlloc failed", __func__);
        goto END;
    }
    ret = UsbAllocDataRequests(g_acm);
    if (ret < 0) {
        HDF_LOGE("%s: UsbAllocDataRequests failed", __func__);
        goto END;
    }

END:
    return ret;
}

int32_t main(int32_t argc, char *argv[])
{
    int32_t ret;
    struct timeval time;
    int32_t i;

    ret = CheckParam(argc, (const char **)argv);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    ret = InitUsbDdk();
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    ret = UsbStartIo(g_acm);
    if (ret) {
        HDF_LOGE("%s: UsbAllocReadRequests failed", __func__);
        goto END;
    }

    if (signal(SIGINT, SignalHandler) == SIG_ERR) {
        HDF_LOGE("signal SIGINT failed");
        return HDF_FAILURE;
    }
    if (signal(SIGALRM, SignalHandler) == SIG_ERR) {
        HDF_LOGE("signal SIGINT failed");
        return HDF_FAILURE;
    }
    gettimeofday(&time, NULL);

    printf("test SDK rawAPI [%s]\n", g_writeOrRead?"write":"read");

    for (i = 0; i < TEST_CYCLE; i++) {
        if (SerialBegin(g_acm) != HDF_SUCCESS) {
            printf("SerialBegin error!\n");
        }
        g_send_count++;
    }

    while (!g_speedFlag) {
        OsalMSleep(TEST_SLEEP_TIME);
    }

    if (UsbStopIo(g_acm) != HDF_SUCCESS) {
        printf("UsbStopIo error!\n");
    }
END:
    if (ret != HDF_SUCCESS) {
        printf("please check whether usb drv so is existing or not,like acm,ecm,if not, remove it and test again!\n");
    }
    return ret;
}
