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

#include "usbhost_sdkapi_speed.h"
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
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_usb_pnp_manage.h"
#include "implementation/global_implementation.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usb_ddk_interface.h"
#include "usb_pnp_notify.h"

#define HDF_LOG_TAG   USB_HOST_ACM

static bool g_speedFlag = false;
static uint64_t g_recv_count = 0;
static uint64_t g_send_count = 0;
static uint64_t g_byteTotal = 0;
static bool g_writeOrRead = TEST_WRITE;
static bool g_printData = false;
static struct AcmDevice *g_acm = NULL;
static struct OsalSem timeSem;
static uint32_t sigCnt = 0;

static void AcmTestBulkCallback(struct UsbRequest *req);
static int32_t SerialBegin(struct AcmDevice *acm);

static int32_t AcmDbAlloc(struct AcmDevice *acm)
{
    int32_t i, dbn;
    struct AcmDb *db = NULL;
    dbn = 0;
    i = 0;
    while (true) {
        if (TEST_CYCLE <= dbn) {
            return -1;
        }
        db = &acm->db[dbn];
        if (!db->use) {
            db->use = 1;
            db->len = 0;
            return dbn;
        }
        dbn = (dbn + 1) % TEST_CYCLE;
        if (++i >= TEST_CYCLE)
            return -1;
    }
}

static int32_t AcmDbIsAvail(const struct AcmDevice *acm)
{
    int32_t i, n;
    n = TEST_CYCLE;
    for (i = 0; i < TEST_CYCLE; i++) {
        n -= acm->db[i].use;
    }
    return n;
}

static UsbInterfaceHandle *InterfaceIdToHandle(const struct AcmDevice *acm, uint8_t id)
{
    UsbInterfaceHandle *devHandle = NULL;

    if (id == 0xFF) {
        devHandle = acm->ctrDevHandle;
    } else {
        for (int32_t i = 0; i < acm->interfaceCnt; i++) {
            if (acm->iface[i]->info.interfaceIndex == id) {
                devHandle = acm->devHandle[i];
                break;
            }
        }
    }
    return devHandle;
}

static int32_t AcmStartDb(struct AcmDevice *acm,
    struct AcmDb *db, struct UsbPipeInfo *pipe)
{
    int32_t rc;
    rc = UsbSubmitRequestAsync(db->request);
    if (rc < 0) {
        HDF_LOGE("UsbSubmitRequestAsync failed, ret=%d \n", rc);
        db->use = 0;
    }
    return rc;
}

static int32_t AcmDataBufAlloc(struct AcmDevice *acm)
{
    int32_t i;
    struct AcmDb *db = NULL;
    if (acm == NULL) {
        return 0;
    }
    for (db = &acm->db[0], i = 0; i < TEST_CYCLE; i++, db++) {
        db->buf = OsalMemCalloc(acm->dataSize);
        if (!db->buf) {
            while (i != 0) {
                --i;
                --db;
                OsalMemFree(db->buf);
                db->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        } else {
            memset_s(db->buf, acm->dataSize, 'a', acm->dataSize);
            db->instance = acm;
        }
    }
    return 0;
}

static int32_t AcmDataBufFree(const struct AcmDevice *acm)
{
    int32_t i;
    struct AcmDb *db;
    for (db = (struct AcmDb *)&acm->db[0], i = 0; i < TEST_CYCLE; i++, db++) {
        OsalMemFree(db->buf);
        db->use = 0;
    }
    return 0;
}

static void AcmTestBulkCallback(struct UsbRequest *req)
{
    if (req == NULL) {
        printf("req is null\r\n");
        return;
    }
    int32_t status = req->compInfo.status;
    struct AcmDb *db  = (struct AcmDb *)req->compInfo.userData;
    if (status == 0) {
        if (g_byteTotal == 0) {
            OsalSemPost(&timeSem);
        }
        g_recv_count++;
        g_byteTotal += req->compInfo.actualLength;
    } else {
        printf("error status=%d\r\n", status);
    }

    if (g_printData) {
        for (unsigned int i = 0; i < req->compInfo.actualLength; i++) {
            printf("%c", req->compInfo.buffer[i]);
        }
        fflush(stdout);
    } else if (g_recv_count % TEST_RECV_COUNT == 0) {
        printf("#");
        fflush(stdout);
    }
    if (db == NULL) {
        return;
    }
    db->use = 0;
    if (!g_speedFlag) {
        if (SerialBegin(db->instance) != HDF_SUCCESS) {
            HDF_LOGW("%s:%d SerialBegin error!", __func__, __LINE__);
        }
        g_send_count++;
    }
}

static int32_t SerialBegin(struct AcmDevice *acm)
{
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
    db->len = (int)acm->dataSize;
    ret = AcmStartDb(acm, db, NULL);
    return ret;
}


static struct UsbInterface *GetUsbInterfaceById(const struct AcmDevice *acm,
    uint8_t interfaceIndex)
{
    return UsbClaimInterface(NULL, acm->busNum, acm->devAddr, interfaceIndex);
}

static struct UsbPipeInfo *EnumePipe(const struct AcmDevice *acm,
    uint8_t interfaceIndex, UsbPipeType pipeType, UsbPipeDirection pipeDirection)
{
    uint8_t i;
    int32_t ret;
    struct UsbInterfaceInfo *info = NULL;
    UsbInterfaceHandle *interfaceHandle = NULL;
    if (USB_PIPE_TYPE_CONTROL == pipeType) {
        info = &acm->ctrIface->info;
        interfaceHandle = acm->ctrDevHandle;
    } else {
        info = &acm->iface[interfaceIndex]->info;
        interfaceHandle = InterfaceIdToHandle(acm, info->interfaceIndex);
    }

    for (i = 0;  i <= info->pipeNum; i++) {
        struct UsbPipeInfo p;
        ret = UsbGetPipeInfo(interfaceHandle, info->curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == pipeDirection) && (p.pipeType == pipeType)) {
            struct UsbPipeInfo *pi = OsalMemCalloc(sizeof(*pi));
            if (pi == NULL) {
                HDF_LOGE("%s: Alloc pipe failed", __func__);
                return NULL;
            }
            p.interfaceId = info->interfaceIndex;
            *pi = p;
            return pi;
        }
    }
    return NULL;
}

static struct UsbPipeInfo *GetPipe(const struct AcmDevice *acm,
    UsbPipeType pipeType, UsbPipeDirection pipeDirection)
{
    uint8_t i;
    if (acm == NULL) {
        HDF_LOGE("%s: invalid parmas", __func__);
        return NULL;
    }
    for (i = 0; i < acm->interfaceCnt; i++) {
        struct UsbPipeInfo *p = NULL;
        if (!acm->iface[i]) {
            continue;
        }
        p = EnumePipe(acm, i, pipeType, pipeDirection);
        if (p == NULL) {
            continue;
        }
        return p;
    }
    return NULL;
}

static void SpeedPrint(void)
{
    double speed;
    uint64_t count;

    sigCnt++;
    count = sigCnt * TEST_PRINT_TIME;
    if (count >= TEST_TIME) {
        g_speedFlag = true;
    }
    speed = (g_byteTotal * TEST_FLOAT_COUNT) / (sigCnt * TEST_PRINT_TIME * TEST_BYTE_COUNT * TEST_BYTE_COUNT);
    printf("\nSpeed:%f MB/s\n", speed);
}

static void ShowHelp(const char *name)
{
    printf(">> usage:\n");
    printf(">>      %s [<busNum> <devAddr>]  <ifaceNum> <w>/<r> [printdata]> \n", name);
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
    int32_t busNum = 1;
    int32_t devAddr = 2;
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

    UsbGetDevInfo(&busNum, &devAddr);
    if (input->paramNum == TEST_SIX_TYPE) {
        busNum = input->busNum;
        devAddr = input->devAddr;
        *ifaceNum = input->ifaceNum;
        g_writeOrRead = input->writeOrRead;
        if (g_writeOrRead == TEST_READ) {
            g_printData = input->printData;
        }
    } else if (input->paramNum == TEST_FIVE_TYPE) {
        busNum = input->busNum;
        devAddr = input->devAddr;
        *ifaceNum = input->ifaceNum;
        g_writeOrRead = input->writeOrRead;
    } else if (input->paramNum == TEST_THREE_TYPE) {
        *ifaceNum = input->ifaceNum;
        g_writeOrRead = input->writeOrRead;
    } else {
        printf("Error: parameter error! \n\n");
        ShowHelp("speedtest");
        return HDF_FAILURE;
    }

    g_acm->busNum = (uint8_t)busNum;
    g_acm->devAddr = (uint8_t)devAddr;
    g_acm->interfaceCnt = 1;
    g_acm->interfaceIndex[0] = *ifaceNum;
    OsalSemInit(&timeSem, 0);

    return HDF_SUCCESS;
}

static int32_t UsbSpeedDdkInit(void)
{
    int32_t ret;
    uint8_t i;

    ret = UsbInitHostSdk(NULL);
    if (ret != HDF_SUCCESS) {
        printf("%s: UsbInitHostSdk failed", __func__);
        return HDF_ERR_IO;
    }

    for (i = 0; i < g_acm->interfaceCnt; i++) {
        g_acm->iface[i] = GetUsbInterfaceById((const struct AcmDevice *)g_acm, g_acm->interfaceIndex[i]);
    }

    for (i = 0; i < g_acm->interfaceCnt; i++) {
        if (g_acm->iface[i]) {
            g_acm->devHandle[i] = UsbOpenInterface(g_acm->iface[i]);
            if (g_acm->devHandle[i] == NULL) {
                HDF_LOGI("%s: UsbOpenInterface null", __func__);
            }
        } else {
            return HDF_FAILURE;
        }
    }
    if (g_writeOrRead == TEST_WRITE) {
        g_acm->dataPipe = GetPipe(g_acm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_OUT);
    } else {
        g_acm->dataPipe = GetPipe(g_acm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_IN);
    }
    if (g_acm->dataPipe == NULL) {
        HDF_LOGE("%{public}s:%{public}d dataPipe is NULL\n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    g_acm->dataSize = TEST_LENGTH;
    if (AcmDataBufAlloc(g_acm) < 0) {
        HDF_LOGE("%{public}s:%{public}d AcmDataBufAlloc failed", __func__, __LINE__);
        return HDF_FAILURE;
    }

    return ret;
}

static int32_t UsbSpeedRequestHandle(void)
{
    int32_t rc;
    for (int32_t i = 0; i < TEST_CYCLE; i++) {
        struct AcmDb *snd = &(g_acm->db[i]);
        snd->request = UsbAllocRequest(InterfaceIdToHandle(g_acm, g_acm->dataPipe->interfaceId), 0, g_acm->dataSize);
        if (snd->request == NULL) {
            printf("%s:%d snd request fail", __func__, __LINE__);
        }
        g_acm->transmitting++;
        struct UsbRequestParams parmas = {};
        parmas.interfaceId = g_acm->dataPipe->interfaceId;
        parmas.pipeAddress = g_acm->dataPipe->pipeAddress;
        parmas.pipeId = g_acm->dataPipe->pipeId;
        parmas.callback = AcmTestBulkCallback;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_CTRL_SET_TIMEOUT;
        parmas.dataReq.numIsoPackets = 0;
        parmas.userData = (void *)snd;
        parmas.dataReq.length = (int)g_acm->dataSize;
        parmas.dataReq.buffer = snd->buf;
        parmas.dataReq.directon = (((uint8_t)g_acm->dataPipe->pipeDirection) >> USB_PIPE_DIR_OFFSET) & 0x1;
        snd->dbNum = g_acm->transmitting;
        rc = UsbFillRequest(snd->request, InterfaceIdToHandle(g_acm, g_acm->dataPipe->interfaceId), &parmas);
        if (rc != HDF_SUCCESS) {
            printf("%s:UsbFillRequest faile,ret=%d \n", __func__, rc);
            goto END;
        }
    }

END:
    return rc;
}

static void UsbSpeedDdkExit(void)
{
    uint8_t i;

    for (i = 0; i < TEST_CYCLE; i++) {
        struct AcmDb *snd = &(g_acm->db[i]);
        UsbCancelRequest(snd->request);
        UsbFreeRequest(snd->request);
    }
    AcmDataBufFree(g_acm);
    for (i = 0; i < g_acm->interfaceCnt; i++) {
        UsbCloseInterface(g_acm->devHandle[i]);
        UsbReleaseInterface(g_acm->iface[i]);
    }
    UsbExitHostSdk(NULL);
    OsalSemDestroy(&timeSem);
    g_acm->busy = false;
}

static int32_t UsbSerialSpeed(struct HdfSBuf *data)
{
    int32_t ifaceNum = 3;
    int32_t ret = HDF_SUCCESS;
    struct UsbSpeedTest *input = NULL;
    uint32_t size = 0;
    int32_t i;
    if (g_acm->busy) {
        printf("%s: speed test busy\n", __func__);
        ret = HDF_ERR_IO;
        goto END;
    } else {
        g_acm->busy = true;
    }

    (void)HdfSbufReadBuffer(data, (const void **)&input, &size);
    if ((input == NULL) || (size != sizeof(struct UsbSpeedTest))) {
        printf("%s: sbuf read buffer failed", __func__);
        ret = HDF_ERR_IO;
        goto END;
    }

    ret = UsbSerialSpeedInit(input, &ifaceNum);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    ret = UsbSpeedDdkInit();
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    ret = UsbSpeedRequestHandle();
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    printf("test SDK API [%s]\n", g_writeOrRead ? "write" : "read");

    for (i = 0; i < TEST_CYCLE; i++) {
        if (SerialBegin(g_acm) != HDF_SUCCESS) {
            printf("SerialBegin error!\n");
        }
        g_send_count++;
    }

    OsalSemWait(&timeSem, TEST_TIME);
    while (!g_speedFlag) {
        OsalSemWait(&timeSem, TEST_PRINT_TIME * TEST_PRINT_TIME_UINT);
        SpeedPrint();
    }

    UsbSpeedDdkExit();

END:
    if (ret != HDF_SUCCESS) {
        printf("please check whether usb drv so is existing or not,like acm, ecm, if not, remove it and test again!\n");
    }
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

    g_acm = (struct AcmDevice *)client->device->service;

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

    g_acm = (struct AcmDevice *)OsalMemCalloc(sizeof(*g_acm));
    if (g_acm == NULL) {
        HDF_LOGE("%s: Alloc usb acm device failed", __func__);
        return HDF_FAILURE;
    }

    g_acm->device  = device;
    device->service = &(g_acm->service);
    if (g_acm->device && g_acm->device->service) {
        g_acm->device->service->Dispatch = AcmDeviceDispatch;
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

struct HdfDriverEntry g_usbSdkApiSpeedDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usb_sdkapispeed",
    .Bind          = AcmDriverBind,
    .Init          = AcmDriverInit,
    .Release       = AcmDriverRelease,
};

HDF_INIT(g_usbSdkApiSpeedDriverEntry);

