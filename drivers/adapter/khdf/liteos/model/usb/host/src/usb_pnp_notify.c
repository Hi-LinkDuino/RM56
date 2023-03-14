/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "usb_pnp_notify.h"
#include <unistd.h>
#include <los_queue.h>
#include <osal_thread.h>
#include "osal_file.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "osal_time.h"
#include "devsvc_manager_clnt.h"
#include "hdf_log.h"
#include "hdf_device_object.h"
#include "implementation/global_implementation.h"
#include "fs/fs.h"
#include "usbdi.h"
#include "linux_usb.h"
#include "usb_debug.h"

#define HDF_LOG_TAG LITEOS_USB_PNP_NOTIFY

static wait_queue_head_t g_usbPnpNotifyReportWait;
static struct OsalThread g_usbPnpNotifyReportThread;
static bool g_usbPnpThreadRunningFlag = false;
static enum UsbPnpNotifyServiceCmd g_usbPnpNotifyCmdType = USB_PNP_NOTIFY_ADD_INTERFACE;
static enum UsbPnpNotifyRemoveType g_usbPnpNotifyRemoveType = USB_PNP_NOTIFY_REMOVE_BUS_DEV_NUM;
struct OsalMutex g_usbSendEventLock;
struct usb_device *g_usbDevice = NULL;
struct UsbPnpAddRemoveInfo g_usbPnpInfo;
struct DListHead g_usbPnpInfoListHead;
#if USB_PNP_NOTIFY_TEST_MODE == true
struct UsbPnpNotifyMatchInfoTable *g_testUsbPnpInfo = NULL;
#endif

/* USB universal driver matching code in LiteOS */
struct DListHead g_usbPnpDeviceListHead;
struct OsalMutex g_usbPnpNotifyDevicelistLock;

static bool UsbPnpNotifyFindDeviceList(struct usb_device *deviceObj, bool freeFlag)
{
    struct UsbPnpNotifyDeviceList *pnpNotifyDevicePos = NULL;
    struct UsbPnpNotifyDeviceList *pnpNotifyDeviceTemp = NULL;
    bool findFlag = false;

    if (DListIsEmpty(&g_usbPnpDeviceListHead)) {
        PRINTK("%s:%d device list is empty\n", __func__, __LINE__);
        return false;
    }

    OsalMutexLock(&g_usbPnpNotifyDevicelistLock);
    DLIST_FOR_EACH_ENTRY_SAFE(pnpNotifyDevicePos, pnpNotifyDeviceTemp, &g_usbPnpDeviceListHead,
        struct UsbPnpNotifyDeviceList, deviceList) {
        if (pnpNotifyDevicePos->device == deviceObj) {
            findFlag = true;
            if (freeFlag) {
                DListRemove(&pnpNotifyDevicePos->deviceList);
                OsalMemFree(&pnpNotifyDevicePos);
            }
            break;
        }
    }
    OsalMutexUnlock(&g_usbPnpNotifyDevicelistLock);

    return findFlag;
}

/* HDF driver framework code */
static struct UsbPnpDeviceInfo *UsbPnpNotifyCreateInfo(void)
{
    struct UsbPnpDeviceInfo *infoTemp = NULL;
    static int32_t idNum = 0;

    infoTemp = (struct UsbPnpDeviceInfo *)OsalMemCalloc(sizeof(struct UsbPnpDeviceInfo));
    if (infoTemp == NULL) {
        HDF_LOGE("%s:%d OsalMemAlloc failed ", __func__, __LINE__);
        return NULL;
    }

    if (idNum++ == INT32_MAX) {
        idNum = 0;
    }
    infoTemp->id = idNum;
    OsalMutexInit(&infoTemp->lock);
    infoTemp->status = USB_PNP_DEVICE_INIT_STATUS;
    DListHeadInit(&infoTemp->list);
    DListInsertTail(&infoTemp->list, &g_usbPnpInfoListHead);

    return infoTemp;
}

static struct UsbPnpDeviceInfo *UsbPnpNotifyFindInfo(struct UsbInfoQueryPara queryPara)
{
    struct UsbPnpDeviceInfo *infoPos = NULL;
    struct UsbPnpDeviceInfo *infoTemp = NULL;
    bool findFlag = false;

    if (DListIsEmpty(&g_usbPnpInfoListHead)) {
        HDF_LOGE("%s:%d usb pnp list head is empty.", __func__, __LINE__);
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(infoPos, infoTemp, &g_usbPnpInfoListHead, struct UsbPnpDeviceInfo, list) {
        switch (queryPara.type) {
            case USB_INFO_NORMAL_TYPE:
                if ((infoPos->info.devNum == queryPara.devNum) && (infoPos->info.busNum == queryPara.busNum)) {
                    findFlag = true;
                }
                break;
            case USB_INFO_ID_TYPE:
                if (infoPos->id == queryPara.id) {
                    findFlag = true;
                }
                break;
            case USB_INFO_DEVICE_ADDRESS_TYPE:
                if (infoPos->info.usbDevAddr == queryPara.usbDevAddr) {
                    findFlag = true;
                }
                break;
            default:
                break;
        }

        if (findFlag) {
            break;
        }
    }

    if (!findFlag) {
        HDF_LOGE("%s:%d the usb pnp info to be find does not exist.", __func__, __LINE__);
        return NULL;
    } else {
        return infoPos;
    }
}

static HDF_STATUS UsbPnpNotifyDestroyInfo(struct UsbPnpDeviceInfo *deviceInfo)
{
    HDF_STATUS ret = HDF_SUCCESS;
    struct UsbPnpDeviceInfo *infoPos = NULL;
    struct UsbPnpDeviceInfo *infoTemp = NULL;
    bool findFlag = false;

    if (deviceInfo == NULL) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d the deviceInfo is NULL, ret = %d ", __func__, __LINE__, ret);
        return ret;
    }

    if (DListIsEmpty(&g_usbPnpInfoListHead)) {
        HDF_LOGI("%s:%d the g_usbPnpInfoListHead is empty.", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(infoPos, infoTemp, &g_usbPnpInfoListHead, struct UsbPnpDeviceInfo, list) {
        if (infoPos->id == deviceInfo->id) {
            findFlag = true;
            DListRemove(&infoPos->list);
            OsalMemFree((void *)infoPos);
            infoPos = NULL;
            break;
        }
    }

    if (!findFlag) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d the deviceInfoto be destroyed does not exist, ret = %d ", __func__, __LINE__, ret);
    }

    return ret;
}

static int32_t UsbPnpNotifyAddInitInfo(struct UsbPnpDeviceInfo *deviceInfo, union UsbPnpDeviceInfoData infoData)
{
    int32_t ret = HDF_SUCCESS;

    deviceInfo->info.usbDevAddr = (uint64_t)infoData.usbDev;
    deviceInfo->info.devNum = infoData.usbDev->address;
    deviceInfo->info.busNum = (int32_t)infoData.usbDev->port_no;

    deviceInfo->info.deviceInfo.vendorId = UGETW(infoData.usbDev->ddesc.idVendor);
    deviceInfo->info.deviceInfo.productId = UGETW(infoData.usbDev->ddesc.idProduct);
    deviceInfo->info.deviceInfo.bcdDeviceLow = UGETW(infoData.usbDev->ddesc.bcdDevice);
    deviceInfo->info.deviceInfo.bcdDeviceHigh = UGETW(infoData.usbDev->ddesc.bcdDevice);
    deviceInfo->info.deviceInfo.deviceClass = infoData.usbDev->ddesc.bDeviceClass;
    deviceInfo->info.deviceInfo.deviceSubClass = infoData.usbDev->ddesc.bDeviceSubClass;
    deviceInfo->info.deviceInfo.deviceProtocol = infoData.usbDev->ddesc.bDeviceProtocol;

    if (infoData.usbDev->cdesc == NULL) {
        DPRINTFN(0, "%s infoData.usbDev->cdesc=%p is NULL", __func__, infoData.usbDev->cdesc);
        ret = HDF_ERR_INVALID_PARAM;
        goto OUT;
    }
    deviceInfo->info.numInfos = infoData.usbDev->cdesc->bNumInterface;
    for (uint8_t i = 0; i < deviceInfo->info.numInfos; i++) {
        if (infoData.usbDev->ifaces[i].idesc == NULL) {
            HDF_LOGE("%s:%d interface[%hhu].idesc is NULL", __func__, __LINE__, i);
            ret = HDF_ERR_INVALID_PARAM;
            goto OUT;
        }
        deviceInfo->info.interfaceInfo[i].interfaceClass =
            infoData.usbDev->ifaces[i].idesc->bInterfaceClass;
        deviceInfo->info.interfaceInfo[i].interfaceSubClass =
            infoData.usbDev->ifaces[i].idesc->bInterfaceSubClass;
        deviceInfo->info.interfaceInfo[i].interfaceProtocol =
            infoData.usbDev->ifaces[i].idesc->bInterfaceProtocol;
        deviceInfo->info.interfaceInfo[i].interfaceNumber =
            infoData.usbDev->ifaces[i].idesc->bInterfaceNumber;

        HDF_LOGI("%s:%d i = %hhu, interfaceInfo=0x%x-0x%x-0x%x-0x%x",
            __func__, __LINE__, i, deviceInfo->info.interfaceInfo[i].interfaceClass,
            deviceInfo->info.interfaceInfo[i].interfaceSubClass,
            deviceInfo->info.interfaceInfo[i].interfaceProtocol,
            deviceInfo->info.interfaceInfo[i].interfaceNumber);
    }

OUT:
    return ret;
}

static void UsbPnpNotifyAddInterfaceInitInfo(struct UsbPnpDeviceInfo *deviceInfo,
    union UsbPnpDeviceInfoData infoData, struct UsbPnpNotifyMatchInfoTable *infoTable)
{
    uint8_t i;
    uint8_t j;

    for (i = 0; i < deviceInfo->info.numInfos; i++) {
        if ((infoData.infoData->interfaceClass == deviceInfo->info.interfaceInfo[i].interfaceClass)
            && (infoData.infoData->interfaceSubClass == deviceInfo->info.interfaceInfo[i].interfaceSubClass)
            && (infoData.infoData->interfaceProtocol == deviceInfo->info.interfaceInfo[i].interfaceProtocol)
            && (infoData.infoData->interfaceNumber == deviceInfo->info.interfaceInfo[i].interfaceNumber)) {
            if (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_INTERFACE) {
                deviceInfo->interfaceRemoveStatus[i] = true;
            } else if (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_ADD_INTERFACE) {
                deviceInfo->interfaceRemoveStatus[i] = false;
            }
        }
    }

    if (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_INTERFACE) {
        infoTable->numInfos = 1;
        infoTable->interfaceInfo[0].interfaceClass = infoData.infoData->interfaceClass;
        infoTable->interfaceInfo[0].interfaceSubClass = infoData.infoData->interfaceSubClass;
        infoTable->interfaceInfo[0].interfaceProtocol = infoData.infoData->interfaceProtocol;
        infoTable->interfaceInfo[0].interfaceNumber = infoData.infoData->interfaceNumber;
    } else {
        for (i = 0, j = 0; i < deviceInfo->info.numInfos; i++) {
            if (deviceInfo->interfaceRemoveStatus[i] == true) {
                HDF_LOGI("%s:%d j = %hhu deviceInfo->interfaceRemoveStatus[%hhu] is true!", __func__, __LINE__, j, i);
                continue;
            }
            infoTable->interfaceInfo[j].interfaceClass = deviceInfo->info.interfaceInfo[i].interfaceClass;
            infoTable->interfaceInfo[j].interfaceSubClass = deviceInfo->info.interfaceInfo[i].interfaceSubClass;
            infoTable->interfaceInfo[j].interfaceProtocol = deviceInfo->info.interfaceInfo[i].interfaceProtocol;
            infoTable->interfaceInfo[j].interfaceNumber = deviceInfo->info.interfaceInfo[i].interfaceNumber;
            j++;

            HDF_LOGI("%s:%d i = %d, j = %d, interfaceInfo=0x%x-0x%x-0x%x-0x%x",
                __func__, __LINE__, i, j - 1, infoTable->interfaceInfo[j - 1].interfaceClass,
                infoTable->interfaceInfo[j - 1].interfaceSubClass,
                infoTable->interfaceInfo[j - 1].interfaceProtocol,
                infoTable->interfaceInfo[j - 1].interfaceNumber);
        }
        infoTable->numInfos = j;
    }
}

static int32_t UsbPnpNotifyInitInfo(
    struct HdfSBuf *sbuf, struct UsbPnpDeviceInfo *deviceInfo, union UsbPnpDeviceInfoData infoData)
{
    int32_t ret = HDF_SUCCESS;
    const void *data = NULL;

    if ((g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_ADD_INTERFACE) ||
        (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_INTERFACE)) {
        static struct UsbPnpNotifyMatchInfoTable infoTable;

        infoTable.usbDevAddr = deviceInfo->info.usbDevAddr;
        infoTable.devNum = deviceInfo->info.devNum;
        infoTable.busNum = deviceInfo->info.busNum;
        infoTable.deviceInfo.vendorId = deviceInfo->info.deviceInfo.vendorId;
        infoTable.deviceInfo.productId = deviceInfo->info.deviceInfo.productId;
        infoTable.deviceInfo.bcdDeviceLow = deviceInfo->info.deviceInfo.bcdDeviceLow;
        infoTable.deviceInfo.bcdDeviceHigh = deviceInfo->info.deviceInfo.bcdDeviceHigh;
        infoTable.deviceInfo.deviceClass = deviceInfo->info.deviceInfo.deviceClass;
        infoTable.deviceInfo.deviceSubClass = deviceInfo->info.deviceInfo.deviceSubClass;
        infoTable.deviceInfo.deviceProtocol = deviceInfo->info.deviceInfo.deviceProtocol;
        infoTable.removeType = g_usbPnpNotifyRemoveType;

        UsbPnpNotifyAddInterfaceInitInfo(deviceInfo, infoData, &infoTable);

        data = (const void *)(&infoTable);
    } else if (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REPORT_INTERFACE) {
        ret = UsbPnpNotifyAddInitInfo(deviceInfo, infoData);
        if (ret != HDF_SUCCESS) {
            goto OUT;
        }

        data = (const void *)(&deviceInfo->info);
    } else {
        data = (const void *)(&deviceInfo->info);
    }

    if (!HdfSbufWriteBuffer(sbuf, data, sizeof(struct UsbPnpNotifyMatchInfoTable))) {
        HDF_LOGE("%s:%d sbuf write data failed", __func__, __LINE__);
        ret = HDF_FAILURE;
        goto OUT;
    }

OUT:
    return ret;
}

static int32_t UsbPnpNotifySendEventLoader(struct HdfSBuf *data)
{
    int32_t ret;

    struct HdfIoService *loaderService = HdfIoServiceBind(USB_PNP_LOADER_SERVICE_NAME);
    if (loaderService == NULL) {
        HDF_LOGE("%s:%d fail to get service %s", __func__, __LINE__, USB_PNP_LOADER_SERVICE_NAME);
        return HDF_FAILURE;
    }

    ret = loaderService->dispatcher->Dispatch(&loaderService->object, g_usbPnpNotifyCmdType, data, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d Dispatch USB_PNP_DRIVER_REGISTER_DEVICE err", __func__, __LINE__);
        goto OUT;
    }

OUT:
    HdfIoServiceRecycle(loaderService);

    return HDF_SUCCESS;
}

static int32_t UsbPnpNotifyGetDeviceInfo(void *eventData, union UsbPnpDeviceInfoData *pnpInfoData,
    struct UsbPnpDeviceInfo **deviceInfo)
{
    struct UsbInfoQueryPara infoQueryPara;

    if ((g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_ADD_INTERFACE)
        || (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_INTERFACE)) {
        pnpInfoData->infoData = (struct UsbPnpAddRemoveInfo *)eventData;
    } else {
        pnpInfoData->usbDev = (struct usb_device *)eventData;
    }

    if ((g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_ADD_INTERFACE)
        || (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_INTERFACE)) {
        infoQueryPara.type = USB_INFO_NORMAL_TYPE;
        infoQueryPara.devNum = pnpInfoData->infoData->devNum;
        infoQueryPara.busNum = pnpInfoData->infoData->busNum;
        *deviceInfo = UsbPnpNotifyFindInfo(infoQueryPara);
    } else if ((g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_ADD_DEVICE)
        || (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_DEVICE)) {
        infoQueryPara.type = USB_INFO_DEVICE_ADDRESS_TYPE;
        infoQueryPara.usbDevAddr = (uint64_t)pnpInfoData->usbDev;
        *deviceInfo = UsbPnpNotifyFindInfo(infoQueryPara);
    } else {
        *deviceInfo = UsbPnpNotifyCreateInfo();
    }

    if (*deviceInfo == NULL) {
        HDF_LOGE("%s:%d create or find info failed", __func__, __LINE__);
        return HDF_FAILURE;
    }

    (*deviceInfo)->info.removeType = g_usbPnpNotifyRemoveType;

    return HDF_SUCCESS;
}

static int32_t UsbPnpNotifyHdfSendEvent(const struct HdfDeviceObject *deviceObject,
    void *eventData)
{
    int32_t ret;
    struct UsbPnpDeviceInfo *deviceInfo = NULL;
    struct HdfSBuf *data = NULL;
    union UsbPnpDeviceInfoData pnpInfoData;

    if ((deviceObject == NULL) || (eventData == NULL)) {
        HDF_LOGE("%s:%d deviceObject and eventData is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s:%d InitDataBlock failed", __func__, __LINE__);
        return HDF_FAILURE;
    }

    ret = UsbPnpNotifyGetDeviceInfo(eventData, &pnpInfoData, &deviceInfo);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d UsbPnpNotifyGetDeviceInfo failed, ret=%d", __func__, __LINE__, ret);
        goto ERROR_DEVICE_INFO;
    }

    ret = UsbPnpNotifyInitInfo(data, deviceInfo, pnpInfoData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d UsbPnpNotifyInitInfo failed, ret=%d", __func__, __LINE__, ret);
        goto OUT;
    }

    HDF_LOGI("%s:%d report one device information, devNum=%d, busNum=%d, infoTable=%d-0x%x-0x%x!",
        __func__, __LINE__, deviceInfo->info.devNum, deviceInfo->info.busNum, deviceInfo->info.numInfos,
        deviceInfo->info.deviceInfo.vendorId, deviceInfo->info.deviceInfo.productId);

    OsalMutexLock(&deviceInfo->lock);
    if (deviceInfo->status == USB_PNP_DEVICE_INIT_STATUS) {
        ret = UsbPnpNotifySendEventLoader(data);
        if (ret != HDF_SUCCESS) {
            OsalMutexUnlock(&deviceInfo->lock);
            HDF_LOGE("%s:%d UsbPnpNotifySendEventLoader ret=%d", __func__, __LINE__, ret);
            goto OUT;
        }
        deviceInfo->status = USB_PNP_DEVICE_ADD_STATUS;
    } else {
        ret = HDF_ERR_INVALID_OBJECT;
    }
    OsalMutexUnlock(&deviceInfo->lock);

OUT:
    if ((ret != HDF_SUCCESS) || (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_DEVICE)) {
        if (UsbPnpNotifyDestroyInfo(deviceInfo) != HDF_SUCCESS) {
            HDF_LOGE("%s:%d UsbPnpNotifyDestroyInfo fail", __func__, __LINE__);
        }
    }
ERROR_DEVICE_INFO:
    HdfSbufRecycle(data);
    return ret;
}

#if USB_PNP_NOTIFY_TEST_MODE == true
static void TestReadPnpInfo(struct HdfSBuf *data)
{
    uint32_t infoSize;
    bool flag;

    flag = HdfSbufReadBuffer(data, (const void **)(&g_testUsbPnpInfo), &infoSize);
    if ((!flag) || (g_testUsbPnpInfo == NULL)) {
        HDF_LOGE("%s: fail to read g_testUsbPnpInfo, flag=%d, g_testUsbPnpInfo=%px", \
            __func__, flag, g_testUsbPnpInfo);
        return;
    }

    HDF_LOGI("%s:%d infoSize=%d g_testUsbPnpInfo=%px read success!", __func__, __LINE__, infoSize, g_testUsbPnpInfo);
}

static void TestPnpNotifyFillInfoTable(struct UsbPnpNotifyMatchInfoTable *infoTable)
{
    int8_t i;

    infoTable->usbDevAddr = g_testUsbPnpInfo->usbDevAddr;
    infoTable->devNum = g_testUsbPnpInfo->devNum;
    if (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_TEST) {
        infoTable->busNum = -1;
    } else {
        infoTable->busNum = g_testUsbPnpInfo->busNum;
    }

    infoTable->deviceInfo.vendorId = g_testUsbPnpInfo->deviceInfo.vendorId;
    infoTable->deviceInfo.productId = g_testUsbPnpInfo->deviceInfo.productId;
    infoTable->deviceInfo.bcdDeviceLow = g_testUsbPnpInfo->deviceInfo.bcdDeviceLow;
    infoTable->deviceInfo.bcdDeviceHigh = g_testUsbPnpInfo->deviceInfo.bcdDeviceHigh;
    infoTable->deviceInfo.deviceClass = g_testUsbPnpInfo->deviceInfo.deviceClass;
    infoTable->deviceInfo.deviceSubClass = g_testUsbPnpInfo->deviceInfo.deviceSubClass;
    infoTable->deviceInfo.deviceProtocol = g_testUsbPnpInfo->deviceInfo.deviceProtocol;

    infoTable->removeType = g_usbPnpNotifyRemoveType;

    if (g_usbPnpNotifyCmdType != USB_PNP_NOTIFY_REMOVE_TEST) {
        infoTable->numInfos = g_testUsbPnpInfo->numInfos;
        for (i = 0; i < infoTable->numInfos; i++) {
            infoTable->interfaceInfo[i].interfaceClass = g_testUsbPnpInfo->interfaceInfo[i].interfaceClass;
            infoTable->interfaceInfo[i].interfaceSubClass = g_testUsbPnpInfo->interfaceInfo[i].interfaceSubClass;
            infoTable->interfaceInfo[i].interfaceProtocol = g_testUsbPnpInfo->interfaceInfo[i].interfaceProtocol;
            infoTable->interfaceInfo[i].interfaceNumber = g_testUsbPnpInfo->interfaceInfo[i].interfaceNumber;
        }
    }
}

static int32_t TestPnpNotifyHdfSendEvent(const struct HdfDeviceObject *deviceObject)
{
    int32_t ret;
    struct UsbPnpNotifyMatchInfoTable infoTable;
    struct HdfSBuf *data = NULL;

    if ((deviceObject == NULL) || (g_testUsbPnpInfo == NULL)) {
        HDF_LOGE("%s deviceObject=%px or g_testUsbPnpInfo=%px is NULL", __func__, deviceObject, g_testUsbPnpInfo);
        return HDF_ERR_INVALID_PARAM;
    }

    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s InitDataBlock failed", __func__);
        return HDF_FAILURE;
    }

    TestPnpNotifyFillInfoTable(&infoTable);

    if (!HdfSbufWriteBuffer(data, (const void *)(&infoTable), sizeof(struct UsbPnpNotifyMatchInfoTable))) {
        HDF_LOGE("%s: sbuf write infoTable failed", __func__);
        goto OUT;
    }

    HDF_LOGI("%s: report one device information, %d usbDev=%llu, devNum=%d, busNum=%d, infoTable=%d-0x%x-0x%x!", \
        __func__, g_usbPnpNotifyCmdType, infoTable.usbDevAddr, infoTable.devNum, infoTable.busNum, \
        infoTable.numInfos, infoTable.deviceInfo.vendorId, infoTable.deviceInfo.productId);

    ret = UsbPnpNotifySendEventLoader(data);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbPnpNotifySendEventLoader error=%d", __func__, ret);
        goto OUT;
    }

    HdfSbufRecycle(data);
    return ret;

OUT:
    HdfSbufRecycle(data);
    return HDF_FAILURE;
}
#endif

static int32_t UsbPnpNotifyFirstReportDevice(struct HdfDeviceIoClient *client)
{
    struct UsbPnpNotifyDeviceList *pnpNotifyDevicePos = NULL;
    struct UsbPnpNotifyDeviceList *pnpNotifyDeviceTemp = NULL;
    int32_t ret;

    dprintf("%s:%d Enter!\n", __func__, __LINE__);

    if (DListIsEmpty(&g_usbPnpDeviceListHead)) {
        dprintf("%s:%d device list is empty\n", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    OsalMutexLock(&g_usbPnpNotifyDevicelistLock);
    DLIST_FOR_EACH_ENTRY_SAFE(pnpNotifyDevicePos, pnpNotifyDeviceTemp, &g_usbPnpDeviceListHead,
        struct UsbPnpNotifyDeviceList, deviceList) {
        ret = UsbPnpNotifyHdfSendEvent(client->device, pnpNotifyDevicePos->device);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d UsbPnpNotifyHdfSendEvent failed, ret=%{public}d", __func__, __LINE__, ret);
        }
    }
    OsalMutexUnlock(&g_usbPnpNotifyDevicelistLock);

    dprintf("%s:%d report all device information!", __func__, __LINE__);

    return HDF_SUCCESS;
}

static int32_t UsbPnpNotifyReportThread(void* arg)
{
    int32_t ret;
    struct HdfDeviceObject *deviceObject = (struct HdfDeviceObject *)arg;

    while (g_usbPnpThreadRunningFlag) {
#if USB_PNP_NOTIFY_TEST_MODE == false
        ret = wait_event_interruptible(g_usbPnpNotifyReportWait, g_usbDevice != NULL);
#else
        ret = wait_event_interruptible(g_usbPnpNotifyReportWait,
            ((g_usbDevice != NULL) || (g_testUsbPnpInfo != NULL)));
#endif
        if (!ret) {
            HDF_LOGI("%s: UsbPnpNotifyReportThread start!", __func__);
        }

        OsalMutexLock(&g_usbSendEventLock);
#if USB_PNP_NOTIFY_TEST_MODE == true
        if ((g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_ADD_TEST) || \
            (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_TEST)) {
            ret = TestPnpNotifyHdfSendEvent(deviceObject);
        } else {
#endif
            if ((g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_ADD_INTERFACE)
                || (g_usbPnpNotifyCmdType == USB_PNP_NOTIFY_REMOVE_INTERFACE)) {
                OsalMSleep(USB_PNP_INTERFACE_MSLEEP_TIME);
                ret = UsbPnpNotifyHdfSendEvent(deviceObject, &g_usbPnpInfo);
            } else {
                ret = UsbPnpNotifyHdfSendEvent(deviceObject, g_usbDevice);
            }
#if USB_PNP_NOTIFY_TEST_MODE == true
        }
#endif
        if (ret != HDF_SUCCESS) {
            HDF_LOGI("%s: UsbPnpNotifyHdfSendEvent error=%d!", __func__, ret);
        }

        g_usbDevice = NULL;
#if USB_PNP_NOTIFY_TEST_MODE == true
        g_testUsbPnpInfo = NULL;
#endif
        OsalMutexUnlock(&g_usbSendEventLock);

        OsalMSleep(USB_PNP_NOTIFY_MSLEEP_TIME);
    }

    HDF_LOGI("%s: usb pnp notify handle kthread exiting!", __func__);

    return 0;
}

static void UsbPnpNotifyCreateDeviceList(struct usb_device *udev)
{
    struct UsbPnpNotifyDeviceList *deviceListTemp = NULL;

    deviceListTemp = (struct UsbPnpNotifyDeviceList *)OsalMemCalloc(sizeof(struct UsbPnpNotifyDeviceList));
    if (deviceListTemp == NULL) {
        HDF_LOGE("%s:%d deviceListTemp is NULL!", __func__, __LINE__);
        return;
    }

    deviceListTemp->device = udev;
    DListHeadInit(&deviceListTemp->deviceList);

    OsalMutexLock(&g_usbPnpNotifyDevicelistLock);
    DListInsertTail(&deviceListTemp->deviceList, &g_usbPnpDeviceListHead);
    OsalMutexUnlock(&g_usbPnpNotifyDevicelistLock);
}

static void UsbPnpNotifyAddDevice(struct usb_device *udev)
{
    int32_t ret;
    struct UsbPnpDeviceInfo *deviceInfo = NULL;
    union UsbPnpDeviceInfoData pnpInfoData;

    pnpInfoData.usbDev = udev;
    deviceInfo = UsbPnpNotifyCreateInfo();
    if (deviceInfo == NULL) {
        DPRINTFN(0, "%s:%d USB_DEVICE_ADD create info failed\n", __func__, __LINE__);
    } else {
        ret = UsbPnpNotifyAddInitInfo(deviceInfo, pnpInfoData);
        if (ret != HDF_SUCCESS) {
            PRINTK("%s:%d USB_DEVICE_ADD UsbPnpNotifyAddInitInfo error\n", __func__, __LINE__);
        } else {
            OsalMutexLock(&g_usbSendEventLock);
            g_usbDevice = udev;
            g_usbPnpNotifyCmdType = USB_PNP_NOTIFY_ADD_DEVICE;
            OsalMutexUnlock(&g_usbSendEventLock);
            wake_up_interruptible(&g_usbPnpNotifyReportWait);
        }
    }
}

static void UsbPnpNotifyAttachDevice(struct usb_device *udev)
{
    int32_t error;
    static int32_t listLockInit = 0;

    if ((udev->ddesc.bDeviceClass == UICLASS_HUB) || UsbPnpNotifyFindDeviceList(udev, false)) {
        PRINTK("%s:%d findDeviceList is true!\n", __func__, __LINE__);
        return;
    }

    if (listLockInit == 0) {
        error = OsalMutexInit(&g_usbPnpNotifyDevicelistLock);
        if (error != HDF_SUCCESS) {
            HDF_LOGE("%s:%d error=%d!", __func__, __LINE__, error);
            return;
        }
        listLockInit = 1;
    }

    error = usb_create_usb_device(udev);
    if (error != HDF_SUCCESS) {
        PRINTK("%s:%d usb_create_usb_device error\n", __func__, __LINE__);
        return;
    }

    UsbPnpNotifyCreateDeviceList(udev);
    UsbPnpNotifyAddDevice(udev);

    return;
}

static void UsbPnpNotifyDetachDevice(struct usb_device *udev)
{
    struct UsbInfoQueryPara infoQueryPara;
    struct UsbPnpDeviceInfo *deviceInfo = NULL;

    if (UsbPnpNotifyFindDeviceList(udev, true)) {
        infoQueryPara.type = USB_INFO_DEVICE_ADDRESS_TYPE;
        infoQueryPara.usbDevAddr = (uint64_t)udev;
        deviceInfo = UsbPnpNotifyFindInfo(infoQueryPara);
        if (deviceInfo == NULL) {
            PRINTK("%s:%d USB_DEVICE_REMOVE find info failed", __func__, __LINE__);
        } else {
            OsalMutexLock(&deviceInfo->lock);
            if (deviceInfo->status != USB_PNP_DEVICE_INIT_STATUS) {
                deviceInfo->status = USB_PNP_DEVICE_INIT_STATUS;
            } else {
                deviceInfo->status = USB_PNP_DEVICE_REMOVE_STATUS;
            }
            OsalMutexUnlock(&deviceInfo->lock);
            OsalMutexLock(&g_usbSendEventLock);
            g_usbDevice = udev;
            g_usbPnpNotifyCmdType = USB_PNP_NOTIFY_REMOVE_DEVICE;
            g_usbPnpNotifyRemoveType = USB_PNP_NOTIFY_REMOVE_BUS_DEV_NUM;
            OsalMutexUnlock(&g_usbSendEventLock);
            wake_up_interruptible(&g_usbPnpNotifyReportWait);
        }
    }
}

void UsbPnpNotifyDevice(const char *type, struct usb_device *udev)
{
    if (strcmp(type, "ATTACH") == 0) {
        UsbPnpNotifyAttachDevice(udev);
    } else if (strcmp(type, "DETACH") == 0) {
        UsbPnpNotifyDetachDevice(udev);
    } else {
        PRINTK("%s:%d type=%s is not define!\n", __func__, __LINE__, type);
    }
}

struct usb_device *UsbPnpNotifyGetUsbDevice(struct UsbGetDevicePara paraData)
{
    bool findFlag = false;
    struct usb_device *usbPnpDevice = NULL;
    struct UsbPnpNotifyDeviceList *pnpNotifyDevicePos = NULL;

    OsalMutexLock(&g_usbPnpNotifyDevicelistLock);
    DLIST_FOR_EACH_ENTRY(pnpNotifyDevicePos, &g_usbPnpDeviceListHead,
        struct UsbPnpNotifyDeviceList, deviceList) {
        switch (paraData.type) {
            case USB_PNP_DEVICE_ADDRESS_TYPE:
                if ((pnpNotifyDevicePos->device->address == paraData.devNum)
                    && (pnpNotifyDevicePos->device->port_no == paraData.busNum)) {
                    findFlag = true;
                }
                break;
            case USB_PNP_DEVICE_VENDOR_PRODUCT_TYPE:
                if ((UGETW(pnpNotifyDevicePos->device->ddesc.idVendor) == paraData.vendorId)
                    && (UGETW(pnpNotifyDevicePos->device->ddesc.idProduct) == paraData.productId)) {
                    findFlag = true;
                }
                break;
            default:
                findFlag = false;
                break;
        }

        if (findFlag) {
            usbPnpDevice = pnpNotifyDevicePos->device;
            break;
        }
    }
    OsalMutexUnlock(&g_usbPnpNotifyDevicelistLock);

    return usbPnpDevice;
}

static void UsbPnpNotifyReadPnpInfo(struct HdfSBuf *data)
{
    struct UsbInfoQueryPara infoQueryPara;
    struct UsbPnpDeviceInfo *deviceInfo = NULL;
    struct UsbPnpAddRemoveInfo *usbPnpInfo = NULL;
    uint32_t infoSize;
    bool flag;

    flag = HdfSbufReadBuffer(data, (const void **)(&usbPnpInfo), &infoSize);
    if ((!flag) || (usbPnpInfo == NULL)) {
        HDF_LOGE("%s:%d fail to read g_usbPnpInfo, flag=%d", __func__, __LINE__, flag);
        return;
    }

    g_usbPnpInfo.devNum = usbPnpInfo->devNum;
    g_usbPnpInfo.busNum = usbPnpInfo->busNum;
    g_usbPnpInfo.interfaceNumber = usbPnpInfo->interfaceNumber;
    g_usbPnpInfo.interfaceClass = usbPnpInfo->interfaceClass;
    g_usbPnpInfo.interfaceSubClass = usbPnpInfo->interfaceSubClass;
    g_usbPnpInfo.interfaceProtocol = usbPnpInfo->interfaceProtocol;

    g_usbDevice = (struct usb_device *)&g_usbPnpInfo;

    infoQueryPara.type = USB_INFO_NORMAL_TYPE;
    infoQueryPara.devNum = usbPnpInfo->devNum;
    infoQueryPara.busNum = usbPnpInfo->busNum;
    deviceInfo = UsbPnpNotifyFindInfo(infoQueryPara);
    if (deviceInfo == NULL) {
        HDF_LOGE("%s:%d add or remove interface find info failed", __func__, __LINE__);
    } else {
        OsalMutexLock(&deviceInfo->lock);
        if (deviceInfo->status != USB_PNP_DEVICE_INIT_STATUS) {
            deviceInfo->status = USB_PNP_DEVICE_INIT_STATUS;
        } else {
            deviceInfo->status = USB_PNP_DEVICE_INTERFACE_STATUS;
        }
        OsalMutexUnlock(&deviceInfo->lock);
    }

    HDF_LOGI("%s:%d infoSize=%d g_usbPnpInfo=%d-%d-%d-%d-%d-%d read success!",
        __func__, __LINE__, infoSize, g_usbPnpInfo.devNum, g_usbPnpInfo.busNum,
        g_usbPnpInfo.interfaceNumber, g_usbPnpInfo.interfaceClass, g_usbPnpInfo.interfaceSubClass,
        g_usbPnpInfo.interfaceProtocol);
}

static int32_t UsbPnpNotifyDriverRegisterDevice(struct HdfDeviceObject *device, struct HdfSBuf *data)
{
    if (data == NULL) {
        return HDF_FAILURE;
    }

    const char *moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }

    int32_t ret;
    struct HdfDeviceObject *devObj = HdfDeviceObjectAlloc(device, moduleName);
    if (devObj == NULL) {
        HDF_LOGE("%s: failed to alloc device object", __func__);
        return HDF_DEV_ERR_NO_MEMORY;
    }

    ret = HdfDeviceObjectRegister(devObj);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to regitst device %s", __func__, moduleName);
        HdfDeviceObjectRelease(devObj);
        return ret;
    }

    ret = HdfDeviceObjectPublishService(devObj, serviceName, SERVICE_POLICY_CAPACITY,
        OSAL_S_IREAD | OSAL_S_IWRITE | OSAL_S_IRGRP | OSAL_S_IWGRP | OSAL_S_IROTH);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to regitst device %s", __func__, serviceName);
        HdfDeviceObjectRelease(devObj);
    }

    return ret;
}

static int32_t UsbPnpNotifyDriverUnregisterDevice(struct HdfSBuf *data)
{
    if (data == NULL) {
        return HDF_FAILURE;
    }

    const char *moduleName = HdfSbufReadString(data);
    if (moduleName == NULL) {
        return HDF_FAILURE;
    }
    const char *serviceName = HdfSbufReadString(data);
    if (serviceName == NULL) {
        return HDF_FAILURE;
    }

    struct HdfDeviceObject *devObj = DevSvcManagerClntGetDeviceObject(serviceName);
    if (devObj == NULL) {
        return HDF_DEV_ERR_NO_DEVICE;
    }
    HdfDeviceObjectRelease(devObj);
    return HDF_SUCCESS;
}

static int32_t UsbPnpNotifyDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;

    if (reply == NULL || client == NULL) {
        HDF_LOGE("%s:%d reply or client is NULL, cmd = %d", __func__, __LINE__, cmd);
        return HDF_FAILURE;
    }

    dprintf("%s:%d dispatch success, cmd = %d\n", __func__, __LINE__, cmd);

    OsalMutexLock(&g_usbSendEventLock);
    g_usbPnpNotifyCmdType = cmd;

    switch (cmd) {
        case USB_PNP_NOTIFY_ADD_INTERFACE:
            UsbPnpNotifyReadPnpInfo(data);
            wake_up_interruptible(&g_usbPnpNotifyReportWait);
            break;
        case USB_PNP_NOTIFY_REMOVE_INTERFACE:
            UsbPnpNotifyReadPnpInfo(data);
            g_usbPnpNotifyRemoveType = USB_PNP_NOTIFY_REMOVE_INTERFACE_NUM;
            wake_up_interruptible(&g_usbPnpNotifyReportWait);
            break;
        case USB_PNP_NOTIFY_REPORT_INTERFACE:
            UsbPnpNotifyFirstReportDevice(client);
            break;
#if USB_PNP_NOTIFY_TEST_MODE == true
        case USB_PNP_NOTIFY_ADD_TEST:
            TestReadPnpInfo(data);
            wake_up_interruptible(&g_usbPnpNotifyReportWait);
            break;
        case USB_PNP_NOTIFY_REMOVE_TEST:
            TestReadPnpInfo(data);
            g_usbPnpNotifyRemoveType = g_testUsbPnpInfo->removeType;
            wake_up_interruptible(&g_usbPnpNotifyReportWait);
            break;
#endif
        case USB_PNP_DRIVER_REGISTER_DEVICE:
            ret = UsbPnpNotifyDriverRegisterDevice(client->device, data);
            break;
        case USB_PNP_DRIVER_UNREGISTER_DEVICE:
            ret = UsbPnpNotifyDriverUnregisterDevice(data);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }
    OsalMutexUnlock(&g_usbSendEventLock);

    if (!HdfSbufWriteInt32(reply, INT32_MAX)) {
        dprintf("%s: reply int32 fail\n", __func__);
    }

    return ret;
}

static int32_t UsbPnpNotifyBind(struct HdfDeviceObject *device)
{
    static struct IDeviceIoService pnpNotifyService = {
        .Dispatch = UsbPnpNotifyDispatch,
    };

    dprintf("%s:%d enter!\n", __func__, __LINE__);

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    device->service = &pnpNotifyService;
    dprintf("%s:%d bind success\n", __func__, __LINE__);

    return HDF_SUCCESS;
}

static int32_t UsbPnpNotifyInit(struct HdfDeviceObject *device)
{
    static bool firstInitFlag = true;
    int32_t ret;
    struct OsalThreadParam threadCfg;

    dprintf("%s:%d enter!\n", __func__, __LINE__);

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (firstInitFlag) {
        firstInitFlag = false;

        DListHeadInit(&g_usbPnpInfoListHead);
        DListHeadInit(&g_usbPnpDeviceListHead);
    }

    init_waitqueue_head(&g_usbPnpNotifyReportWait);

    OsalMutexInit(&g_usbSendEventLock);

    /* Creat thread to handle send usb interface information. */
    ret = memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
        return ret;
    }
    
    g_usbPnpThreadRunningFlag = true;
    threadCfg.name = "LiteOS usb pnp notify handle kthread";
    threadCfg.priority = OSAL_THREAD_PRI_HIGH;
    threadCfg.stackSize = USB_PNP_NOTIFY_REPORT_STACK_SIZE;

    ret = OsalThreadCreate(&g_usbPnpNotifyReportThread, (OsalThreadEntry)UsbPnpNotifyReportThread, (void *)device);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadCreate faile, ret=%d ", __func__, __LINE__, ret);
        return ret;
    }

    ret = OsalThreadStart(&g_usbPnpNotifyReportThread, &threadCfg);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadStart faile, ret=%d ", __func__, __LINE__, ret);
        return ret;
    }

    dprintf("%s:%d Init success\n", __func__, __LINE__);

    return HDF_SUCCESS;
}

static void UsbPnpNotifyRelease(struct HdfDeviceObject *device)
{
    HDF_STATUS ret;

    if (device == NULL) {
        HDF_LOGI("%s: device is null", __func__);
        return;
    }

    g_usbPnpThreadRunningFlag = false;

    ret = OsalThreadDestroy(&g_usbPnpNotifyReportThread);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadDestroy faile, ret=%d ", __func__, __LINE__, ret);
        return;
    }

    OsalMutexDestroy(&g_usbSendEventLock);

    dprintf("%s:%d release success\n", __func__, __LINE__);

    return;
}

struct HdfDriverEntry g_usbPnpNotifyEntry = {
    .moduleVersion = 1,
    .Bind = UsbPnpNotifyBind,
    .Init = UsbPnpNotifyInit,
    .Release = UsbPnpNotifyRelease,
    .moduleName = "HDF_USB_PNP_NOTIFY",
};

HDF_INIT(g_usbPnpNotifyEntry);
