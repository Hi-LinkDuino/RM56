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
#include <osal_sem.h>
#include <osal_thread.h>
#include <sys/syscall.h>
#include <time.h>
#include "usbhost_nosdk_speed.h"
#include "osal_time.h"
#include "osal_mem.h"
#include "implementation/global_implementation.h"
#include "liteos_ddk_usb.h"
#include "usb_pnp_notify.h"

#define USB_DEV_FS_PATH "/dev/bus/usb"
#define URB_COMPLETE_PROCESS_STACK_SIZE 8196
#define ENDPOINT_IN_OFFSET 7
static int32_t g_speedFlag = 0;
static int32_t g_busNum = 1;
static int32_t g_devAddr = 2;
static struct OsalSem sem;
static struct OsalSem timeSem;

static uint64_t g_send_count = 0;
static uint64_t g_recv_count = 0;
static uint64_t g_byteTotal = 0;
static struct UsbAdapterUrbs urb[TEST_CYCLE];
static struct urb *sendUrb = NULL;
static bool g_printData = false;
static uint8_t endNum;
static struct OsalSem timeSem;
static struct usb_device *fd = NULL;
static uint32_t sigCnt = 0;
static struct UsbAdapterHostEndpoint *uhe = NULL;
static bool g_writeOrRead = TEST_WRITE;
static struct AcmDevice *acm = NULL;

static void CloseDevice(void)
{
    return;
}

static int32_t OpenDevice(void)
{
    struct UsbGetDevicePara paraData;
    paraData.type = USB_PNP_DEVICE_ADDRESS_TYPE;
    paraData.busNum = (uint8_t)g_busNum;
    paraData.devNum = (uint8_t)g_devAddr;
    fd = UsbPnpNotifyGetUsbDevice(paraData);
    if (fd == NULL) {
        HDF_LOGE("%{public}s:%{public}d err fd\n", __func__, __LINE__);
        return -1;
    }
    return 0;
}

static int32_t ClaimInterface(unsigned int iface)
{
    HDF_LOGI("%{public}s:%{public}d claim success: iface=%u\n", __func__, __LINE__, iface);
    return HDF_SUCCESS;
}

void SpeedPrint(void)
{
    double speed;
    uint64_t count;

    sigCnt++;
    count = sigCnt * TEST_PRINT_TIME;
    if (count >= TEST_TIME) {
        g_speedFlag = true;
    }
    speed = (g_byteTotal * TEST_DOUBLE_COUNT) /
        (sigCnt * TEST_PRINT_TIME * TEST_BYTE_COUNT_UINT * TEST_BYTE_COUNT_UINT);
    printf("\nSpeed:%f MB/s\n", speed);
}

static int32_t SendProcess(void *argurb)
{
    int32_t i;
    int32_t r;
    while (!g_speedFlag) {
        OsalSemWait(&sem, HDF_WAIT_FOREVER);
        for (i = 0; i < TEST_CYCLE; i++) {
            if (urb[i].inUse == 0) {
                urb[i].inUse = 1;
                break;
            }
        }

        if (i == TEST_CYCLE) {
            i = TEST_CYCLE - 1;
        }
        sendUrb = urb[i].urb;
        r = usb_setup_endpoint(fd, uhe, TEST_BYTE_COUNT_UINT);
        if (r) {
            DPRINTFN(0, "setup failed ret:%d\n", r);
            return r;
        }
        r = usb_submit_urb(sendUrb, 0);
        if (r < 0) {
            HDF_LOGI("SubmitBulkRequest: ret:%d\n", r);
            urb[i].inUse = 0;
            continue;
        }
        g_send_count++;
    }
    return 0;
}

static void UrbCompleteHandle(const struct urb *curUrb)
{
    if (g_printData) {
        for (int32_t i = 0; i < curUrb->actual_length; i++) {
            printf("%c", *(((char*)curUrb->transfer_buffer) + i));
        }
        fflush(stdout);
    } else if (g_recv_count % TEST_PRINT_MAX_RANGE == 0) {
        printf("#");
        fflush(stdout);
    }
}

static void UrbComplete(struct urb *curUrb)
{
    int32_t i;
    for (i = 0; i < TEST_CYCLE; i++) {
        if (urb[i].urb == curUrb) {
            if (g_byteTotal == 0) {
                OsalSemPost(&timeSem);
            }
            g_recv_count++;
            g_byteTotal += curUrb->actual_length;
            UrbCompleteHandle(curUrb);
            urb[i].inUse = 0;
            OsalSemPost(&sem);
            break;
        }
    }
}

static int32_t BeginProcessHandleFirst(void)
{
    char *data = NULL;
    int32_t i;

    for (i = 0; i < TEST_CYCLE; i++) {
        if (urb[i].urb == NULL) {
            urb[i].urb = OsalMemCalloc(sizeof(struct urb));
            if (urb[i].urb == NULL) {
                HDF_LOGE("%{public}s:%{public}d urb calloc err\n", __func__, __LINE__);
                return -1;
            }
        }
        urb[i].inUse = 0;
        urb[i].urb->dev = fd;
        urb[i].urb->endpoint = uhe;
        urb[i].urb->complete = UrbComplete;

        if (data == NULL) {
            data = OsalMemCalloc(TEST_LENGTH);
            if (data == NULL) {
                HDF_LOGE("%{public}s:%{public}d data calloc err\n", __func__, __LINE__);
                return -1;
            }
        }

        memset_s(data, TEST_LENGTH, 'c', TEST_LENGTH);
        data[TEST_LENGTH - 1] = '\0';
        urb[i].urb->transfer_buffer = (void *)data;
        urb[i].urb->transfer_buffer_length = TEST_LENGTH;
    }

    return HDF_SUCCESS;
}

static int32_t BeginProcess(unsigned char endPoint)
{
    int32_t r;
    const int32_t transNum = 0;
    int32_t i;

    if (endPoint <= 0) {
        HDF_LOGE("%{public}s:%{public}d parameter error\n", __func__, __LINE__);
        return -1;
    }

    uhe = usb_find_host_endpoint(fd, USB_REQUEST_TYPE_BULK, endPoint);
    if (uhe == NULL) {
        HDF_LOGE("%{public}s:%{public}d usb_find_host_endpoint error\n", __func__, __LINE__);
        return -1;
    }
    r = BeginProcessHandleFirst();
    if (r != HDF_SUCCESS) {
        return r;
    }

    HDF_LOGI("%{public}s:%{public}d test NO SDK endpoint:%u\n", __func__, __LINE__, endPoint);

    for (i = 0; i < TEST_CYCLE; i++) {
        if (urb[i].inUse == 0) {
            urb[i].inUse = 1;
            urb[i].urbNum = transNum;
            sendUrb = urb[i].urb;
            r = usb_setup_endpoint(fd, uhe, 1024);
            if (r) {
                DPRINTFN(0, "setup failed ret:%d\n", r);
                return r;
            }
            r = usb_submit_urb(sendUrb, 0);
            if (r < 0) {
                HDF_LOGI("%{public}s:%{public}d SubmitBulkRequest: ret:%d\n", __func__, __LINE__, r);
                urb[i].inUse = 0;
                continue;
            }
            g_send_count++;
        }
    }

    OsalSemWait(&timeSem, TEST_TIME);
    while (!g_speedFlag) {
        OsalSemWait(&timeSem, TEST_PRINT_TIME * TEST_PRINT_TIME_UINT);
        SpeedPrint();
    }

    for (i = 0; i < TEST_CYCLE; i++) {
        usb_kill_urb(urb[i].urb);
    }

    return HDF_SUCCESS;
}

static void ShowHelp(const char *name)
{
    printf(">> usage:\n");
    printf(">>      %s [<busNum> <devAddr>]  <ifaceNum> <endpoint> [<printdata>]\n", name);
    printf("\n");
}

static void UsbGetDevInfo(int32_t *busNum, int32_t *devNum)
{
    struct UsbGetDevicePara paraData;
    struct usb_device *usbPnpDevice = NULL;
    paraData.type = USB_PNP_DEVICE_VENDOR_PRODUCT_TYPE;
    paraData.vendorId = USB_DEVICE_VENDOR_ID;
    paraData.productId = USB_DEVICE_PRODUCT_ID;
    usbPnpDevice = UsbPnpNotifyGetUsbDevice(paraData);
    if (usbPnpDevice == NULL) {
        HDF_LOGE("%s:%d UsbPnpNotifyGetUsbDevice is NULL!", __func__, __LINE__);
        return;
    }
    *busNum = (int)usbPnpDevice->address;
    *devNum = (int)usbPnpDevice->port_no;
    printf("%s:%d busNum=%d devNum=%d!\n", __func__, __LINE__, *busNum, *devNum);
}

static int32_t UsbSerialOpen(void)
{
    return HDF_SUCCESS;
}
static int32_t UsbSerialClose(void)
{
    if (!g_speedFlag) {
        g_speedFlag = true;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialSpeedInit(const struct UsbSpeedTest *input, int32_t *ifaceNum)
{
    int32_t ret = HDF_SUCCESS;
    if (input == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    g_speedFlag = false;
    g_send_count = 0;
    g_recv_count = 0;
    g_byteTotal = 0;
    g_printData = false;
    g_writeOrRead = TEST_WRITE;
    sigCnt = 0;
    g_busNum = 1;
    g_devAddr = 2;

    UsbGetDevInfo(&g_busNum, &g_devAddr);
    if (input->paramNum == INPUT_COMPARE_PARAMNUM) {
        g_busNum = input->busNum;
        g_devAddr = input->devAddr;
        *ifaceNum = input->ifaceNum;
        endNum = input->writeOrRead;
        g_writeOrRead = ((endNum >> ENDPOINT_IN_OFFSET) == 0) ? TEST_WRITE : TEST_READ;
        if (g_writeOrRead == TEST_READ) {
            g_printData = input->printData;
        }
    } else if (input->paramNum == 5) {
        g_busNum = input->busNum;
        g_devAddr = input->devAddr;
        *ifaceNum = input->ifaceNum;
        endNum = input->writeOrRead;
        g_writeOrRead = ((endNum >> ENDPOINT_IN_OFFSET) == 0) ? TEST_WRITE : TEST_READ;
    } else if (input->paramNum == 3) {
        *ifaceNum = input->ifaceNum;
        endNum = input->writeOrRead;
        g_writeOrRead = ((endNum >> ENDPOINT_IN_OFFSET) == 0) ? TEST_WRITE : TEST_READ;
    } else {
        printf("Error: parameter error! \n\n");
        ShowHelp("speedtest");
        return HDF_FAILURE;
    }
    OsalSemInit(&sem, 0);
    OsalSemInit(&timeSem, 0);

    return ret;
}

static int32_t UsbSerialSpeedThreadCreate(void)
{
    int32_t ret;
    struct OsalThread urbSendProcess;
    struct OsalThreadParam threadCfg;

    threadCfg.name = "urb send process";
    threadCfg.priority = OSAL_THREAD_PRI_DEFAULT;
    threadCfg.stackSize = URB_COMPLETE_PROCESS_STACK_SIZE;

    ret = OsalThreadCreate(&urbSendProcess, (OsalThreadEntry)SendProcess, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("OsalThreadCreate failed, ret = %d\n", ret);
        return ret;
    }

    ret = OsalThreadStart(&urbSendProcess, &threadCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("OsalThreadStart failed, ret = %d\n", ret);
        return ret;
    }

    return ret;
}

static int32_t UsbSerialSpeed(struct HdfSBuf *data)
{
    int32_t ifaceNum = 3;
    int32_t ret;
    struct UsbSpeedTest *input = NULL;
    uint32_t size = 0;
    if (acm->busy) {
        HDF_LOGE("%{public}s: %{public}d speed test busy\n", __func__, __LINE__);
        ret = HDF_ERR_IO;
        goto END;
    } else {
        acm->busy = true;
    }

    (void)HdfSbufReadBuffer(data, (const void **)&input, &size);
    if ((input == NULL) || (size != sizeof(struct UsbSpeedTest))) {
        HDF_LOGE("%{public}s: %{public}d sbuf read buffer failed\n", __func__, __LINE__);
        ret = HDF_ERR_IO;
        goto END;
    }

    ret = UsbSerialSpeedInit(input, &ifaceNum);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    OpenDevice();

    ret = ClaimInterface(ifaceNum);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    ret = UsbSerialSpeedThreadCreate();
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    ret = BeginProcess(endNum);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

END:
    acm->busy = false;
    if (ret != HDF_SUCCESS) {
        printf("please check whether usb drv so is existing or not,like acm, ecm,if not,remove it and test again!\n");
    }
    CloseDevice();
    return ret;
}

static int32_t AcmDeviceDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (client == NULL) {
        HDF_LOGE("%s: client is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (client->device == NULL) {
        HDF_LOGE("%s: client->device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (client->device->service == NULL) {
        HDF_LOGE("%s: client->device->service is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    acm = (struct AcmDevice *)client->device->service;

    switch (cmd) {
        case USB_SERIAL_OPEN:
            return UsbSerialOpen();
        case USB_SERIAL_CLOSE:
            return UsbSerialClose();
        case USB_SERIAL_SPEED:
            return UsbSerialSpeed(data);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static int32_t AcmDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    acm = (struct AcmDevice *)OsalMemCalloc(sizeof(*acm));
    if (acm == NULL) {
        HDF_LOGE("%s: Alloc usb acm device failed", __func__);
        return HDF_FAILURE;
    }

    acm->device  = device;
    device->service = &(acm->service);
    if (acm->device && acm->device->service) {
        acm->device->service->Dispatch = AcmDeviceDispatch;
    }
    return HDF_SUCCESS;
}

static int32_t AcmDriverInit(struct HdfDeviceObject *device)
{
    return 0;
}

static void AcmDriverRelease(struct HdfDeviceObject *device)
{
}

struct HdfDriverEntry g_usbNoSdkSpeedDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usb_nosdkspeed",
    .Bind          = AcmDriverBind,
    .Init          = AcmDriverInit,
    .Release       = AcmDriverRelease,
};

HDF_INIT(g_usbNoSdkSpeedDriverEntry);
