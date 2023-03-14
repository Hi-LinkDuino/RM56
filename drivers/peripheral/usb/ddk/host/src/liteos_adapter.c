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

#include "liteos_adapter.h"
#include "los_spinlock.h"
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
#include "usb_pnp_notify.h"
#endif

#define HDF_LOG_TAG     USB_LITEOS_ADAPTER

#define PATH_LEN        24
#define DESC_READ_LEN   256
#define EP_NUM_MAX      30

static bool g_CompleteExit;

enum UrbState {
    URB_INIT_STATE,
    URB_SUBMIT_STATE,
    URB_REAP_STATE,
};

struct Async {
    struct DListHead asynclist;
    enum UrbState state;
    struct UsbDevice *dev;
    struct UsbAdapterUrb urb;
};

struct OsDev {
    struct DListHead asyncCompleted;
    struct UsbAdapterDevice *adapterDevice;
    SPIN_LOCK_S completeLock;
    struct OsalMutex completeMux;
    struct OsalSem  cvWait;
};

static void *OsAdapterRealloc(void *ptr, size_t oldSize, size_t newSize)
{
    void *mem;

    mem = RawUsbMemAlloc(newSize);
    if (mem == NULL) {
        HDF_LOGE("%s:%d", __func__, __LINE__);
        goto OUT;
    }

    if (oldSize > 0) {
        if (memmove_s(mem, newSize, ptr, oldSize) != HDF_SUCCESS) {
            HDF_LOGE("%s:%d", __func__, __LINE__);
            RawUsbMemFree(mem);
            mem = NULL;
            goto OUT;
        }
    }

    RawUsbMemFree(ptr);
OUT:
    return mem;
}

static bool UsbFindUrb(struct Async *urb, struct DListHead *list)
{
    bool findFlag = false;
    struct OsDev *osDev = CONTAINER_OF(list, struct OsDev, asyncCompleted);
    struct Async *asPos = NULL;
    struct Async *asTemp = NULL;
    if (DListIsEmpty(&osDev->asyncCompleted)) {
        return false;
    }
    DLIST_FOR_EACH_ENTRY_SAFE(asPos, asTemp, list, struct Async, asynclist) {
        if (asPos == urb) {
            findFlag = true;
            break;
        }
    }
    return findFlag;
}

static void OsUrbComplete(struct UsbAdapterUrb *urb)
{
    uint32_t save;
    struct Async *as = CONTAINER_OF(urb, struct Async, urb);
    struct UsbDevice *dev = as->dev;
    if (dev == NULL) {
        DPRINTFN(0, "--dev:%p\n", dev);
        return;
    }
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    if (osDev == NULL) {
        DPRINTFN(0, "--osDev:%p\n", osDev);
        return;
    }
    as->state = URB_REAP_STATE;
    LOS_SpinLockSave(&osDev->completeLock, &save);
    if (UsbFindUrb(as, &osDev->asyncCompleted) == false) {
        DListInsertTail(&as->asynclist, &osDev->asyncCompleted);
        LOS_SpinUnlockRestore(&osDev->completeLock, save);
        OsalSemPost(&osDev->cvWait);
    } else {
        LOS_SpinUnlockRestore(&osDev->completeLock, save);
    }
}

static int32_t OsSubmitUrb(
    struct UsbAdapterUrb *urb, struct UsbAdapterDevice *adapterDevice, struct UsbAdapterHostEndpoint *uhe)
{
    int32_t err;

    if ((uhe == NULL) || (urb == NULL) || (adapterDevice == NULL)) {
        DPRINTFN(0, "invalid param\n");
        return (-EINVAL);
    }

    err = usb_setup_endpoint(adapterDevice, uhe, 1024);
    if (err) {
        DPRINTFN(0, "setup failed err:%d\n", err);
        return (err);
    }
    err = usb_submit_urb(urb, 0);
    return (err);
}

static int32_t OsControlMsg(struct UsbAdapterUrb *urb)
{
    int32_t err;
    uint16_t actLen = 0;
    struct UsbAdapterDevice *adapterDevice = urb->dev;
    void *buffer = urb->transfer_buffer;
    struct UsbAdapterDeviceRequest req;

    err = memcpy_s(&req, sizeof(struct UsbAdapterDeviceRequest), buffer, sizeof(req));
    if (err != EOK) {
        DPRINTFN(0, "%s:%d err=%d\n", __func__, __LINE__, err);
        err = HDF_ERR_IO;
        return err;
    }
    err = usbd_do_request_flags(adapterDevice,  NULL, &req, (char *)buffer + sizeof(req),
             USB_SHORT_XFER_OK, &actLen, urb->timeout);
    if (err) {
        DPRINTFN(1, "OsControlMsg!err:%d\n", err);
    }
    urb->actual_length = actLen;
    if (urb->complete) {
        (urb->complete)(urb);
    }
    return err;
}

static int32_t OsWaitUrb(struct OsDev *osDev)
{
    if (osDev == NULL) {
        return HDF_FAILURE;
    }
    do {
        OsalSemWait(&osDev->cvWait, HDF_WAIT_FOREVER);
    } while (!g_CompleteExit && DListIsEmpty(&osDev->asyncCompleted));
    return HDF_SUCCESS;
}

static int32_t OsReapUrb(const struct UsbDeviceHandle *handle, struct Async **urb)
{
    if ((handle == NULL) || (handle->dev == NULL) || (handle->dev->privateData == NULL)) {
        PRINTK("invalid parmater\n");
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t err = 0;
    uint32_t save;
    struct Async *as = NULL;
    struct UsbDevice *dev = handle->dev;
    if (dev->privateData == NULL) {
        PRINTK("invalid parmater\n");
        return HDF_ERR_INVALID_PARAM;
    }
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    err = OsWaitUrb(osDev);
    LOS_SpinLockSave(&osDev->completeLock, &save);
    if (!DListIsEmpty(&osDev->asyncCompleted)) {
        as = DLIST_FIRST_ENTRY(&osDev->asyncCompleted, struct Async, asynclist);
        DListRemove(&as->asynclist);
    }
    LOS_SpinUnlockRestore(&osDev->completeLock, save);
    *urb = as;
    return err;
}

static bool OsDeviceCompare(struct HdfSListNode *listEntry, uint32_t searchKey)
{
    struct UsbDevice *dev = (struct UsbDevice *)listEntry;
    if (dev == NULL) {
        return false;
    }

    if ((dev->busNum == (searchKey >> BUS_OFFSET)) &&
        (dev->devAddr == (searchKey & 0xFF))) {
        return true;
    }

    return false;
}

static struct UsbDeviceHandle *OsGetDeviceHandle(struct UsbSession *session, uint8_t busNum, uint8_t usbAddr)
{
    struct UsbDevice *dev = NULL;
    struct UsbDeviceHandle *handle = NULL;

    if (session == NULL) {
        DPRINTFN(0, "%s:invalid param\n", __func__);
        return NULL;
    }

    OsalMutexLock(&session->lock);
    dev = (struct UsbDevice *)HdfSListSearch(
        &session->usbDevs, (busNum << BUS_OFFSET) | usbAddr, OsDeviceCompare);
    if (dev != NULL) {
        handle = dev->devHandle;
        AdapterAtomicInc(&dev->refcnt);
    }
    OsalMutexUnlock(&session->lock);

    return handle;
}

static struct UsbDeviceHandle *OsCallocDeviceHandle(void)
{
    struct UsbDeviceHandle *handle = NULL;

    handle = RawUsbMemCalloc(sizeof(*handle));
    if (handle == NULL) {
        DPRINTFN(0, "%s: allocate handle failed", __func__);
        return NULL;
    }

    OsalMutexInit(&handle->lock);

    return handle;
}

static struct UsbDevice *OsAllocDevice(struct UsbSession *session, struct UsbDeviceHandle *handle)
{
    struct UsbDevice *dev = RawUsbMemCalloc(sizeof(*dev));
    if (dev == NULL) {
        DPRINTFN(0, "%s:%d no memory", __func__, __LINE__);
        return NULL;
    }

    dev->session = session;
    dev->devHandle = handle;

    RawRequestListInit(dev);

    handle->dev = dev;
    return dev;
}

static int32_t OsReadDescriptors(struct UsbDevice *dev)
{
    size_t allocLen = 0;
    uint8_t *ptr = NULL;
    int32_t ret;
    size_t count;
    if (dev == NULL) {
        HDF_LOGE("%s:%d dev is NULL!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    if ((osDev == NULL) || (osDev->adapterDevice == NULL) || (osDev->adapterDevice->cdesc == NULL)) {
        HDF_LOGE("%s:%d is NULL!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    do {
        size_t oldLen = allocLen;
        allocLen += DESC_READ_LEN;
        dev->descriptors = OsAdapterRealloc(dev->descriptors, oldLen, allocLen);
        if ((dev->descriptors == NULL) || (dev->descriptorsLength > allocLen)) {
            DPRINTFN(0, "%s:%d\n", __func__, __LINE__);
            return HDF_ERR_MALLOC_FAIL;
        }
        ptr = (uint8_t *)dev->descriptors + dev->descriptorsLength;
        if (ptr == NULL) {
            DPRINTFN(0, "%s:%d ptr is NULL\n", __func__, __LINE__);
            ret = HDF_ERR_INVALID_PARAM;
            return ret;
        }
        ret = memset_s(ptr, DESC_READ_LEN, 0, DESC_READ_LEN);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
            return HDF_FAILURE;
        }

        count = UGETW(osDev->adapterDevice->cdesc->wTotalLength);
        if (count > DESC_READ_LEN) {
            ret = HDF_ERR_IO;
            return ret;
        }
        ret = memcpy_s(ptr, DESC_READ_LEN, osDev->adapterDevice->cdesc, count);
        if (ret != EOK) {
            DPRINTFN(0, "%s:%d ret=%d\n", __func__, __LINE__, ret);
            ret = HDF_ERR_IO;
            return ret;
        }
        DPRINTFN(0, "%s:+configdes_size:%d+type:%d\n", __func__,
            UGETW(osDev->adapterDevice->cdesc->wTotalLength), osDev->adapterDevice->cdesc->bDescriptorType);
        dev->descriptorsLength += count;
    } while (dev->descriptorsLength == allocLen);

    return HDF_SUCCESS;
}

static int32_t OsParseConfigDescriptors(struct UsbDevice *dev)
{
    uint8_t i;
    uint8_t numConfigs;
    uint8_t *buffer = NULL;
    size_t descLen;
    struct UsbAdapterDeviceDescriptor *deviceDesc = NULL;
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    deviceDesc = &osDev->adapterDevice->ddesc;
    numConfigs = deviceDesc->bNumConfigurations;
    if (numConfigs == 0) {
        return HDF_SUCCESS;
    }
    dev->configDescriptors = RawUsbMemAlloc(numConfigs * sizeof(struct UsbDeviceConfigDescriptor));
    if (dev->configDescriptors == NULL) {
        DPRINTFN(0, "%s:%d\n", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    buffer = (uint8_t *)dev->descriptors;
    descLen = dev->descriptorsLength;

    for (i = 0; i < numConfigs; i++) {
        struct UsbConfigDescriptor *configDesc = NULL;
        uint16_t configLen;

        if (descLen < USB_DDK_DT_CONFIG_SIZE) {
            DPRINTFN(0, "%s:%d read %zu", __func__, __LINE__, descLen);
            return HDF_ERR_IO;
        }
        configDesc = (struct UsbConfigDescriptor *)buffer;
        if ((configDesc->bDescriptorType != USB_DDK_DT_CONFIG) ||
            (configDesc->bLength < USB_DDK_DT_CONFIG_SIZE)) {
            DPRINTFN(0, "%s:%d config desc error: type 0x%02x, length %u\n",
                __func__, __LINE__, configDesc->bDescriptorType, configDesc->bLength);
            return HDF_ERR_IO;
        }
        configLen = Le16ToCpu(configDesc->wTotalLength);
        if (configLen < USB_DDK_DT_CONFIG_SIZE) {
            DPRINTFN(0, "invalid wTotalLength value %u\n", configLen);
            return HDF_ERR_IO;
        }
        if (configLen > descLen) {
            DPRINTFN(0, "%s:%d read %zu/%u\n",
                __func__, __LINE__, descLen, configLen);
            configLen = (uint16_t)descLen;
        }
        dev->configDescriptors[i].desc = configDesc;
        dev->configDescriptors[i].actualLen = configLen;
        buffer += configLen;
        descLen -= configLen;
    }
    return HDF_SUCCESS;
}

static struct OsDev *OsDevAllocInit(void)
{
    struct OsDev *osDev = NULL;
    osDev = RawUsbMemCalloc(sizeof(*osDev));
    if (osDev == NULL) {
        return NULL;
    }
    DListHeadInit(&osDev->asyncCompleted);
    OsalSemInit(&osDev->cvWait, 0);
    LOS_SpinInit(&osDev->completeLock);
    OsalMutexInit(&osDev->completeMux);
    return osDev;
}

static void OsDevDestory(struct OsDev *osDev)
{
    if (osDev == NULL) {
        DPRINTFN(0, "invalid parma\n");
        return;
    }
    OsalSemDestroy(&osDev->cvWait);
    OsalMutexDestroy(&osDev->completeMux);
    RawUsbMemFree(osDev);
}

static int32_t OsInitDevice(struct UsbDevice *dev, uint8_t busNum, uint8_t devAddr)
{
    struct OsDev *osDev = NULL;
    int32_t ret;

    dev->busNum = busNum;
    dev->devAddr = devAddr;
    osDev = OsDevAllocInit();
    if (osDev == NULL) {
        DPRINTFN(0, "%s:%d osDev  is NULL\n", __func__, __LINE__);
        return HDF_DEV_ERR_NO_DEVICE;
    }
    g_CompleteExit = false;
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
    struct UsbGetDevicePara paraData;
    paraData.type = USB_PNP_DEVICE_ADDRESS_TYPE;
    paraData.busNum = dev->busNum;
    paraData.devNum = dev->devAddr;
    osDev->adapterDevice = UsbPnpNotifyGetUsbDevice(paraData);
#else
    osDev->adapterDevice = NULL;
#endif
    if (osDev->adapterDevice == NULL) {
        DPRINTFN(0, "%s:%d osDev->adapterDevice is NULL\n", __func__, __LINE__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    dev->privateData = (void *)osDev;
    dev->descriptorsLength = 0;
    ret = OsReadDescriptors(dev);
    if (ret != HDF_SUCCESS) {
        DPRINTFN(0, "%s:%d ret=%d\n", __func__, __LINE__, ret);
        return ret;
    }
    ret = OsParseConfigDescriptors(dev);
    if (ret != HDF_SUCCESS) {
        DPRINTFN(0, "%s:%d ret=%d\n", __func__, __LINE__, ret);
        return ret;
    }
    ret = memcpy_s(&dev->deviceDescriptor, sizeof(struct UsbDeviceDescriptor),
        &osDev->adapterDevice->ddesc, USB_DDK_DT_DEVICE_SIZE);
    if (ret != HDF_SUCCESS) {
        DPRINTFN(0, "%s:%d ret=%d\n", __func__, __LINE__, ret);
        ret = HDF_ERR_IO;
    }
    return ret;
}

static void OsFreeIsoUrbs(struct UsbHostRequest *request)
{
    struct Async *urb = NULL;

    for (int32_t i = 0; i < request->numUrbs; i++) {
        urb = request->isoUrbs[i];
        if (urb == NULL) {
            break;
        }
        RawUsbMemFree(urb);
        request->isoUrbs[i] = NULL;
    }

    RawUsbMemFree(request->isoUrbs);
    request->isoUrbs = NULL;
}

static void OsDiscardUrbs(const struct UsbHostRequest *request, int32_t first, int32_t last)
{
    struct UsbAdapterUrb *urb = NULL;
    struct Async *as = NULL;

    if ((request == NULL) || (request->devHandle == NULL) ||
        (first > URBS_PER_REQUEST) || (first > last)) {
        DPRINTFN(0, "%s:%d invalid param", __func__, __LINE__);
        return;
    }

    for (int32_t i = last - 1; i >= first; i--) {
        if (request->requestType == USB_REQUEST_TYPE_ISOCHRONOUS) {
            as = (struct Async *)request->isoUrbs[i];
        } else {
            as = &(((struct Async *)request->urbs)[i]);
        }
        if (as == NULL) {
            DPRINTFN(0, "discard as null\n");
            return;
        }
        urb = &as->urb;
        if (as->state == URB_SUBMIT_STATE) {
            DPRINTFN(0, "usb kill urb\n");
            usb_kill_urb(urb);
            DPRINTFN(0, "%s:%d discard request%p+as:%p\n", __func__, __LINE__, request, as);
        }
    }
}

static int32_t OsSubmitControlMsg(
    struct UsbHostRequest * const request, const struct UsbAdapterDevice *adapterDevice, const struct UsbDevice *dev)
{
    int32_t ret;
    struct Async *as = NULL;
    struct UsbAdapterUrb *urb = NULL;
    struct UsbAdapterHostEndpoint *uhe = NULL;
    if ((request == NULL) || (request->length > MAX_BULK_DATA_BUFFER_LENGTH)
        || (request->buffer == NULL) || (adapterDevice == NULL) || (dev == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }
    uhe = usb_find_host_endpoint((struct UsbAdapterDevice *)adapterDevice, request->requestType, request->endPoint);
    if (uhe == NULL) {
        DPRINTFN(0, "no found endpoint\n");
        return -1;
    }
    if (request->urbs == NULL) {
        as = RawUsbMemCalloc(sizeof(*as));
        request->urbs =  (void *)as;
        request->numUrbs = 1;
    }

    if (request->urbs == NULL) {
        DPRINTFN(0, "%s:%d no mem", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    as = (struct Async *)request->urbs;
    DListHeadInit(&as->asynclist);
    as->dev = (struct UsbDevice *)dev;
    as->state = URB_SUBMIT_STATE;
    urb = &as->urb;
    ret = memset_s(urb, sizeof(*urb), 0, sizeof(*urb));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
        return HDF_FAILURE;
    }

    urb->dev = (struct UsbAdapterDevice *)adapterDevice;
    urb->endpoint = uhe;
    urb->timeout = 500;
    urb->transfer_buffer = request->buffer;
    urb->context = (void *)request;
    urb->complete = OsUrbComplete;
    ret = OsControlMsg(urb);
    DPRINTFN(0, "OsSubmitControlRequest:ret:%d\n", ret);
    if (ret) {
        DPRINTFN(0, "submiturb failed, errno=%d\n", errno);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitControlRequest(struct UsbHostRequest *request)
{
    struct OsDev *osDev = NULL;
    struct UsbAdapterDevice *adapterDevice = NULL;

    if ((request == NULL) || (request->length > MAX_BULK_DATA_BUFFER_LENGTH)
        || (request->devHandle == NULL) || (request->buffer == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct UsbDeviceHandle *handle = request->devHandle;
    struct UsbDevice *dev = handle->dev;
    if (dev) {
        osDev = (struct OsDev *)dev->privateData;
    }
    if (osDev) {
        adapterDevice = osDev->adapterDevice;
    }

    return OsSubmitControlMsg(request, adapterDevice, dev);
}

static int32_t OsSubmitBulkRequestHandleUrb(struct Async *pas, struct UsbHostRequest *request,
    int32_t bulkBufferLen, int32_t number)
{
    struct UsbAdapterUrb *urb = NULL;

    if (bulkBufferLen == 0) {
        HDF_LOGE("%s:%d bulkBufferLen can not be zero", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    urb = &pas->urb;
    urb->context = (void *)request;
    switch (request->requestType) {
        case USB_REQUEST_TYPE_BULK:
            break;
        case USB_REQUEST_TYPE_INTERRUPT:
            urb->interval = 50;
            break;
        default:
            DPRINTFN(0, "%s:%d unknown requestType=%u\n", __func__, __LINE__,
                request->requestType);
            return HDF_ERR_INVALID_PARAM;
    }
    urb->transfer_buffer = request->buffer + (number * bulkBufferLen);
    urb->complete = OsUrbComplete;
    if (number == request->numUrbs - 1) {
        uint32_t len = (uint32_t)(request->length % bulkBufferLen);
        urb->transfer_buffer_length = (len == 0) ? (uint32_t)bulkBufferLen : len;
    } else {
        urb->transfer_buffer_length = (uint32_t)bulkBufferLen;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitBulkRequestHandle(
    struct UsbHostRequest * const request, struct Async * const as, int32_t bulkBufferLen)
{
    struct Async *pas = NULL;
    int32_t numUrbs = request->numUrbs;
    int32_t i;
    int32_t ret;
    struct UsbDevice *dev = request->devHandle->dev;
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    struct UsbAdapterDevice *adapterDevice = osDev->adapterDevice;
    struct UsbAdapterHostEndpoint *uhe = NULL;
    struct UsbAdapterUrb *urb = NULL;

    uhe = usb_find_host_endpoint(adapterDevice, request->requestType, request->endPoint);
    if (uhe == NULL) {
        DPRINTFN(0, "no found endpoint\n");
        return HDF_DEV_ERR_NO_DEVICE;
    }

    for (i = 0, pas = as; i < numUrbs; i++, pas++) {
        urb = &pas->urb;
        ret = memset_s(urb, sizeof(*urb), 0, sizeof(*urb));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
            return HDF_FAILURE;
        }

        ret = OsSubmitBulkRequestHandleUrb(pas, request, bulkBufferLen, i);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
        pas->state =  URB_SUBMIT_STATE;
        DListHeadInit(&pas->asynclist);
        pas->dev = dev;
        urb->dev = adapterDevice;
        urb->endpoint = uhe;

        ret = OsSubmitUrb(urb, urb->dev, urb->endpoint);
        if (ret == 0) {
            continue;
        }
        if (i == 0) {
            DPRINTFN(0, "the first urb failed\n");
            return HDF_ERR_IO;
        }
        OsalMutexLock(&request->lock);
        request->numRetired += numUrbs - i;
        if (errno != EREMOTEIO) {
            request->reqStatus = USB_REQUEST_ERROR;
        }
        OsDiscardUrbs(request, 0, i);
        OsalMutexUnlock(&request->lock);
        return HDF_SUCCESS;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitBulkRequest(struct UsbHostRequest * const request)
{
    struct Async *as = NULL;
    uint32_t bulkBufferLen;
    int32_t numUrbs;

    if ((request == NULL) || (request->devHandle == NULL)) {
        DPRINTFN(0, "%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (request->length > MAX_BULK_DATA_BUFFER_LENGTH || request->length <= 0) {
        DPRINTFN(0, "Bulk request size err\n");
        return HDF_FAILURE;
    }

    if (request->devHandle->caps & USB_ADAPTER_CAP_BULK_SCATTER_GATHER) {
        bulkBufferLen = request->length ? request->length : 1;
    } else {
        bulkBufferLen = MAX_BULK_DATA_BUFFER_LENGTH;
    }
    if (request->length < bulkBufferLen) {
        numUrbs = 1;
    } else {
        numUrbs = (request->length + bulkBufferLen - 1) / bulkBufferLen;
    }
    if (request->urbs == NULL) {
        as = RawUsbMemCalloc(sizeof(*as));
        request->urbs =  (void *)as;
    } else if (numUrbs > 1) {
        RawUsbMemFree(request->urbs);
        as = RawUsbMemCalloc(numUrbs * sizeof(*as));
        request->urbs =  (void *)as;
    }
    if (request->urbs == NULL) {
        DPRINTFN(0, "%s:%d no mem", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    as = (struct Async *)request->urbs;
    request->numUrbs    = numUrbs;
    request->numRetired = 0;
    request->reqStatus = USB_REQUEST_COMPLETED;

    return OsSubmitBulkRequestHandle(request, as, bulkBufferLen);
}

static int32_t OsAllocIsoUrbs(struct UsbHostRequest *request, int32_t numUrbs, struct Async **ass)
{
    struct Async *as = NULL;
    unsigned char *urbBuffer = request->buffer;
    int32_t numPacketsLeft = request->numIsoPackets;
    int32_t packetIdx = 0;
    int32_t i, j;

    UsbPipeType pipeType = request->requestType;
    unsigned char endPoint = request->endPoint;
    struct UsbDeviceHandle *handle = request->devHandle;
    struct UsbDevice *dev = handle->dev;
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    struct UsbAdapterDevice *adapterDevice = osDev->adapterDevice;
    struct UsbAdapterHostEndpoint *uhe = NULL;
    uhe = usb_find_host_endpoint(adapterDevice, pipeType, endPoint);
    if (uhe == NULL) {
        DPRINTFN(0, "no found endpoint\n");
        return HDF_DEV_ERR_NO_DEVICE;
    }

    for (i = 0; i < numUrbs; i++) {
        struct UsbAdapterUrb *urb = NULL;
        int32_t numPackets = MIN(numPacketsLeft, MAX_ISO_PACKETS_PER_URB);
        as = RawUsbMemCalloc(sizeof(struct Async));
        if (as == NULL) {
            OsFreeIsoUrbs(request);
            return HDF_ERR_MALLOC_FAIL;
        }
        ass[i] = as;
        urb = &as->urb;
        for (j = 0; j < numPackets; j++) {
            unsigned int packetLen = request->isoPacketDesc[packetIdx++].length;
            urb->transfer_buffer_length += packetLen;
            urb->iso_frame_desc[j].length = packetLen;
        }
        urb->endpoint = uhe;
        urb->number_of_packets = (unsigned int)numPackets;
        urb->transfer_buffer = urbBuffer;
        urb->context = request;
        urbBuffer += urb->transfer_buffer_length;
        numPacketsLeft -= numPackets;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitIsoUrbs(struct UsbHostRequest *request, int32_t numUrbs, struct Async **pUrbs)
{
    int32_t i;
    int32_t ret;

    for (i = 0; i < numUrbs; i++) {
        struct UsbAdapterUrb *urb = &(pUrbs[i]->urb);
        ret =  OsSubmitUrb(urb, urb->dev, urb->endpoint);
        DPRINTFN(0, "submitUrb:%d errno=%d\n", ret, errno);
        if (ret == 0) {
            continue;
        }

        if (errno == ENODEV) {
            ret = HDF_DEV_ERR_NO_DEVICE;
        } else {
            DPRINTFN(0, "%s:%d submit iso urb failed errno=%d\n", __func__, __LINE__, errno);
            ret = HDF_ERR_IO;
        }

        if (i == 0) {
            DPRINTFN(0, "first URB failed");
            OsFreeIsoUrbs(request);
            return ret;
        }

        OsalMutexLock(&request->lock);
        request->reqStatus = USB_REQUEST_ERROR;
        request->numRetired += numUrbs - i;
        if (request->numRetired == numUrbs) {
            RawUsbMemFree(pUrbs);
            request->isoUrbs = NULL;
        }
        OsalMutexUnlock(&request->lock);

        break;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitIsoRequest(struct UsbHostRequest *request)
{
    struct Async **pUrbs = NULL;
    int32_t numUrbs;
    unsigned int packetLen;
    unsigned int totalLen = 0;
    int32_t ret;

    if ((request == NULL) || (request->devHandle == NULL) || (request->numIsoPackets < 1)) {
        DPRINTFN(0, "%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (request->length > MAX_ISO_DATA_BUFFER_LEN) {
        DPRINTFN(0, "%s:%d request length exceed the maximum", __func__, __LINE__);
        return HDF_ERR_NOT_SUPPORT;
    }

    for (int32_t i = 0; i < request->numIsoPackets; i++) {
        packetLen = request->isoPacketDesc[i].length;
        if (packetLen > MAX_ISO_DATA_BUFFER_LEN) {
            DPRINTFN(0, "%s:%d packet length: %u exceeds maximum: %u\n",
                     __func__, __LINE__, packetLen, MAX_ISO_DATA_BUFFER_LEN);
            return HDF_ERR_INVALID_PARAM;
        }
        totalLen += packetLen;
    }
    if (request->length < totalLen) {
        DPRINTFN(0, "%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    numUrbs = (request->numIsoPackets + (MAX_ISO_PACKETS_PER_URB - 1)) / MAX_ISO_PACKETS_PER_URB;
    pUrbs = RawUsbMemCalloc(numUrbs * sizeof(struct Async*));
    if (pUrbs == NULL) {
        DPRINTFN(0, "%s:%d RawUsbMemCalloc pUrbs failed", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    request->isoUrbs = (void **)pUrbs;
    request->numUrbs = numUrbs;
    request->numRetired = 0;
    request->isoPacketOffset = 0;
    ret = OsAllocIsoUrbs(request, numUrbs, pUrbs);
    if (ret != HDF_SUCCESS) {
        DPRINTFN(0, "%s:%d alloc iso urbs failed", __func__, __LINE__);
        return ret;
    }

    return OsSubmitIsoUrbs(request, numUrbs, pUrbs);
}

static int32_t OsControlCompletion(struct UsbHostRequest *request, struct Async *as)
{
    int32_t status;
    struct UsbAdapterUrb *urb = &as->urb;

    OsalMutexLock(&request->lock);
    request->numRetired++;
    request->actualLength += (int)urb->actual_length;
    if (request->reqStatus == USB_REQUEST_CANCELLED) {
        OsalMutexUnlock(&request->lock);
        as->state = URB_INIT_STATE;
        return RawHandleRequestCompletion(request, USB_REQUEST_CANCELLED);
    }

    switch (urb->status) {
        case 0:
            status = USB_REQUEST_COMPLETED;
            break;
        case -ENOENT:
            status = USB_REQUEST_CANCELLED;
            break;
        case -EPIPE:
            DPRINTFN(0, "%s:%d unsupported control request", __func__, __LINE__);
            status = USB_REQUEST_STALL;
            break;
        case -EOVERFLOW:
            DPRINTFN(0, "%s:%d overflow actualLength=%d\n", __func__, __LINE__, urb->actual_length);
            status = USB_REQUEST_OVERFLOW;
            break;
        case -ENODEV:
        case -ESHUTDOWN:
            DPRINTFN(0, "device removed");
            status = USB_REQUEST_NO_DEVICE;
            break;
        default:
            DPRINTFN(0, "%s:%d urb status=%d\n", __func__, __LINE__, urb->status);
            status = USB_REQUEST_ERROR;
            break;
    }
    OsalMutexUnlock(&request->lock);
    as->state = URB_INIT_STATE;
    return RawHandleRequestCompletion(request, status);
}

static void OsIsoRequestDesStatus(struct UsbHostRequest *request, struct UsbAdapterUrb *urb)
{
    uint32_t i;
    struct UsbAdapterIsoPacketDescriptor *urbDesc = NULL;
    struct UsbIsoPacketDesc *requestDesc = NULL;

    for (i = 0; i < urb->number_of_packets; i++) {
        urbDesc = &urb->iso_frame_desc[i];
        requestDesc = &request->isoPacketDesc[request->isoPacketOffset++];

        switch (urbDesc->status) {
            case HDF_SUCCESS:
                requestDesc->status = USB_REQUEST_COMPLETED;
                break;
            case -ENODEV:
            case -ESHUTDOWN:
                requestDesc->status = USB_REQUEST_NO_DEVICE;
                break;
            case -EPIPE:
                requestDesc->status = USB_REQUEST_STALL;
                break;
            case -EOVERFLOW:
                requestDesc->status = USB_REQUEST_OVERFLOW;
                break;
            default:
                requestDesc->status = USB_REQUEST_ERROR;
                break;
        }
        DPRINTFN(0, "%s:%d urb status=%d-%d\n", __func__, __LINE__, i, urbDesc->status);

        requestDesc->actualLength = urbDesc->actual_length;
    }
}

static int32_t OsIsoCompletion(struct UsbHostRequest *request, struct Async *as)
{
    UsbRequestStatus status;
    int32_t urbIndex = 0;
    int32_t numUrbs;
    struct UsbAdapterUrb *urb = &as->urb;

    if (request == NULL) {
        DPRINTFN(0, "%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    numUrbs = request->numUrbs;
    OsalMutexLock(&request->lock);
    for (int32_t i = 0; i < numUrbs; i++) {
        if (urb == request->isoUrbs[i]) {
            urbIndex = i + 1;
            break;
        }
    }
    if (urbIndex == 0) {
        DPRINTFN(0, "%s:%d urbIndex is zero", __func__, __LINE__);
        OsalMutexUnlock(&request->lock);
        return HDF_ERR_BAD_FD;
    }

    OsIsoRequestDesStatus(request, urb);
    request->numRetired++;
    if  (request->reqStatus != USB_REQUEST_COMPLETED) {
        DPRINTFN(0, "%s:%d urb status=%d\n", __func__, __LINE__, urb->status);
        if (request->numRetired == numUrbs) {
            OsFreeIsoUrbs(request);
            OsalMutexUnlock(&request->lock);
            return RawHandleRequestCompletion(request, USB_REQUEST_ERROR);
        }
        goto OUT;
    }

    DPRINTFN(0, "%s:%d urb status is %d\n", __func__, __LINE__, urb->status);
    if (urb->status == -ESHUTDOWN) {
        status = USB_REQUEST_NO_DEVICE;
    } else if (!((urb->status == HDF_SUCCESS) || (urb->status == -ENOENT) ||
        (urb->status == -ECONNRESET))) {
        status = USB_REQUEST_ERROR;
    } else {
        status = USB_REQUEST_COMPLETED;
    }

    if (request->numRetired == numUrbs) {
        DPRINTFN(0, "%s:%d all URBs reaped for complete", __func__, __LINE__);
        OsFreeIsoUrbs(request);
        OsalMutexUnlock(&request->lock);
        return RawHandleRequestCompletion(request, status);
    }
OUT:
    OsalMutexUnlock(&request->lock);
    return HDF_SUCCESS;
}

static int32_t OsProcessAbnormalReap(struct UsbHostRequest *request, const struct UsbAdapterUrb *urb)
{
    int32_t ret;

    if (urb->actual_length > 0) {
        unsigned char *target = request->buffer + request->actualLength;
        if (urb->transfer_buffer != target) {
            ret = memmove_s(target, urb->actual_length, urb->transfer_buffer, urb->actual_length);
            if (ret != EOK) {
                DPRINTFN(0, "%s: memmove_s failed, ret=%d", __func__, ret);
            }
        }
        request->actualLength += urb->actual_length;
    }
    if (request->numRetired == request->numUrbs) {
        return HDF_SUCCESS;
    }

    return HDF_ERR_IO;
}

static int32_t OsUrbStatusToRequestStatus(struct UsbHostRequest *request, const struct UsbAdapterUrb *urb)
{
    int32_t ret;

    switch (urb->status) {
        case 0:
            ret = HDF_SUCCESS;
            break;
        case -ESHUTDOWN:
            DPRINTFN(0, "%s:%d device is removed", __func__, __LINE__);
            request->reqStatus = USB_REQUEST_NO_DEVICE;
            ret = HDF_DEV_ERR_NO_DEVICE;
            break;
        case -EPIPE:
            if (request->reqStatus == USB_REQUEST_COMPLETED) {
                request->reqStatus = USB_REQUEST_STALL;
            }
            ret = HDF_DEV_ERR_NO_DEVICE;
            break;
        case -EOVERFLOW:
            DPRINTFN(0, "%s:%d overflow error, actualLength=%d\n",
                     __func__, __LINE__, urb->actual_length);
            if (request->reqStatus == USB_REQUEST_COMPLETED) {
                request->reqStatus = USB_REQUEST_OVERFLOW;
            }
            ret = HDF_FAILURE;
            break;
        case -ECONNRESET:
            ret = HDF_DEV_ERR_OP;
            if (request->reqStatus == USB_REQUEST_COMPLETED) {
                request->reqStatus = USB_REQUEST_CANCELLED;
            }
            break;
        default:
            DPRINTFN(0, "unknown urb status %d\n", urb->status);
            if (request->reqStatus == USB_REQUEST_COMPLETED) {
                request->reqStatus = USB_REQUEST_ERROR;
            }
            ret = HDF_FAILURE;
            break;
    }

    return ret;
}

static int32_t OsBulkCompletion(struct UsbHostRequest * const request, struct Async * const as)
{
    int32_t ret;
    int32_t urbIdx = as - (struct Async *)request->urbs;
    const struct UsbAdapterUrb *urb = &as->urb;

    OsalMutexLock(&request->lock);
    request->numRetired++;
    if (request->reqStatus != USB_REQUEST_COMPLETED) {
        if (OsProcessAbnormalReap(request, urb) == HDF_SUCCESS) {
            goto COMPLETED;
        } else {
            goto OUT;
        }
    }
    request->actualLength += urb->actual_length;
    ret = OsUrbStatusToRequestStatus(request, urb);
    if (ret == HDF_DEV_ERR_NO_DEVICE) {
        goto CANCEL;
    } else if (ret == HDF_FAILURE) {
        goto COMPLETED;
    }

    if (request->numRetired == request->numUrbs) {
        goto COMPLETED;
    } else if (urb->actual_length < urb->transfer_buffer_length) {
        if (request->reqStatus == USB_REQUEST_COMPLETED) {
            request->reqStatus = USB_REQUEST_COMPLETED_SHORT;
        }
    } else {
        goto OUT;
    }

CANCEL:
    if (request->numRetired == request->numUrbs) {
        goto COMPLETED;
    }
    OsDiscardUrbs(request, urbIdx + 1, request->numUrbs);

OUT:
    OsalMutexUnlock(&request->lock);
    return HDF_SUCCESS;

COMPLETED:
    OsalMutexUnlock(&request->lock);
    as->state = URB_INIT_STATE;
    return RawHandleRequestCompletion(request, request->reqStatus);
}

static int32_t OsFreeRequest(const struct UsbHostRequest *request)
{
    int32_t retry = 0;
    if (request == NULL) {
        DPRINTFN(0, "%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    while (true) {
        OsalMSleep(20);
        if (request->numUrbs != request->numRetired) {
            if (++retry > 10) {
                DPRINTFN(0, "request busy numUrbs:%d+numretired:%d\n", request->numUrbs, request->numRetired);
                return HDF_ERR_DEVICE_BUSY;
            }
            continue;
        } else {
            break;
        }
    }

    return HDF_SUCCESS;
}

static int32_t AdapterInit(const struct UsbSession *session)
{
    return HDF_SUCCESS;
}

static void AdapterExit(const struct UsbSession *session)
{
    return;
}

static struct UsbDeviceHandle *AdapterOpenDevice(struct UsbSession *session, uint8_t busNum, uint8_t usbAddr)
{
    int32_t ret;
    struct UsbDevice *dev = NULL;
    struct UsbDeviceHandle *handle = NULL;

    handle = OsGetDeviceHandle(session, busNum, usbAddr);
    if (handle != NULL) {
        return handle;
    }
    handle = OsCallocDeviceHandle();
    if (handle == NULL) {
        DPRINTFN(0, "%s: Calloc Device Handle failed", __func__);
        return NULL;
    }
    dev = OsAllocDevice(session, handle);
    if (dev == NULL) {
        DPRINTFN(0, "%s: OsAllocDevice failed\n", __func__);
        goto ERR;
    }
    ret = OsInitDevice(dev, busNum, usbAddr);
    if (ret) {
        DPRINTFN(0, "%s: OsInitDevice failed ret=%d\n", __func__, ret);
        RawUsbMemFree(dev);
        goto ERR;
    }
    OsalAtomicSet(&dev->refcnt, 1);
    OsalMutexLock(&session->lock);
    HdfSListAdd(&session->usbDevs, &dev->list);
    OsalMutexUnlock(&session->lock);
    return handle;
ERR:
    OsalMutexDestroy(&handle->lock);
    RawUsbMemFree(handle);
    return NULL;
}

static void AdapterCloseDevice(struct UsbDeviceHandle *handle)
{
    struct UsbDevice *dev = NULL;

    if ((handle == NULL) || (handle->dev == NULL)) {
        DPRINTFN(0, "%s:%d invalid param", __func__, __LINE__);
        return;
    }
    if (RawKillSignal(handle, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s:%d RawKillSignal failed", __func__, __LINE__);
    }
    dev = handle->dev;
    if (AdapterAtomicDec(&dev->refcnt)) {
        return;
    }
    if (dev->session == NULL) {
        return;
    }
    OsalMutexLock(&dev->session->lock);
    HdfSListRemove(&dev->session->usbDevs, &dev->list);
    OsalMutexUnlock(&dev->session->lock);

    if (dev->configDescriptors) {
        RawUsbMemFree(dev->configDescriptors);
    }
    if (dev->descriptors) {
        RawUsbMemFree(dev->descriptors);
    }
    if (dev->privateData) {
        OsDevDestory(dev->privateData);
        dev->privateData = NULL;
    }
    RawUsbMemFree(dev);

    OsalMutexDestroy(&handle->lock);
    RawUsbMemFree(handle);
}

static int32_t AdapterGetConfigDescriptor(const struct UsbDevice *dev,
    uint8_t configIndex, void *buffer, size_t len)
{
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    struct UsbAdapterDevice *adapterDevice = osDev->adapterDevice;
    size_t count;
    if ((buffer == NULL) || (len == 0) || (adapterDevice == NULL) || (adapterDevice->cdesc == NULL)) {
        DPRINTFN(0, "invalid param is NULL");
        return HDF_ERR_INVALID_PARAM;
    }
    count = UGETW(adapterDevice->cdesc->wTotalLength);
    if (count > len) {
        DPRINTFN(0, "count length is error");
        return HDF_ERR_IO;
    }
    if (memcpy_s(buffer, len, adapterDevice->cdesc, count) != EOK) {
        DPRINTFN(0, "memcpy_s fail");
        return HDF_ERR_IO;
    }
    return len;
}

static int32_t AdapterGetConfiguration(const struct UsbDeviceHandle *handle, uint8_t *activeConfig)
{
    struct UsbDevice *dev = handle->dev;
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    struct UsbAdapterDevice *adapterDevice = osDev->adapterDevice;

    if (adapterDevice != NULL) {
        *activeConfig = adapterDevice->curr_config_index;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterSetConfiguration(struct UsbDeviceHandle *handle, int32_t activeConfig)
{
    struct UsbDevice *dev = handle->dev;
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    struct UsbAdapterDevice *adapterDevice = osDev->adapterDevice;

    if (adapterDevice->flags.usb_mode != USB_MODE_HOST) {
        return HDF_DEV_ERR_DEV_INIT_FAIL;
    }
    if ((activeConfig < 0) || (activeConfig >= (int)adapterDevice->curr_config_no)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (activeConfig == adapterDevice->curr_config_index) {
        return HDF_SUCCESS;
    }
    if (usbd_set_config_index(adapterDevice, activeConfig) != 0) {
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterClaimInterface(const struct UsbDeviceHandle *handle, unsigned int interfaceNumber)
{
    return HDF_SUCCESS;
}

static int32_t AdapterReleaseInterface(const struct UsbDeviceHandle *handle, unsigned int interfaceNumber)
{
    return HDF_SUCCESS;
}

static int32_t AdapterSetInterface(const struct UsbDeviceHandle *handle, uint8_t interface, uint8_t altSetting)
{
    struct UsbDevice *dev = handle->dev;
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    struct UsbAdapterDevice *adapterDevice = osDev->adapterDevice;

    if (adapterDevice->flags.usb_mode != USB_MODE_HOST) {
        return HDF_DEV_ERR_DEV_INIT_FAIL;
    }
        DPRINTFN(0, "altSetting interfaceId:%d+altSetting:%d\n", interface, altSetting);
    if (usb_set_interface(adapterDevice, interface, altSetting)) {
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterClearHalt(const struct UsbDeviceHandle *handle, unsigned int endPoint)
{
    struct UsbDevice *dev = handle->dev;
    struct OsDev *osDev = (struct OsDev *)dev->privateData;
    struct UsbAdapterDevice *adapterDevice = osDev->adapterDevice;
    int32_t ret;
    struct UsbAdapterHostEndpoint *uhe = usb_find_host_endpoint(adapterDevice, PIPE_BULK, endPoint);
    if (uhe == NULL) {
        printf("no found uhe\n");
        return -1;
    }
    ret = usb_clear_halt(adapterDevice, uhe);
    return ret;
}

static int32_t AdapterResetDevice(const struct UsbDeviceHandle *handle)
{
     return HDF_SUCCESS;
}

static struct UsbHostRequest *AdapterAllocRequest(
    const struct UsbDeviceHandle *handle, int32_t isoPackets, size_t length)
{
    size_t allocSize;
    struct UsbHostRequest *request = NULL;

    allocSize = sizeof(struct UsbHostRequest)
                + (sizeof(struct UsbIsoPacketDesc) * (size_t)isoPackets)
                + (sizeof(unsigned char) * length);
    request = RawUsbMemCalloc(allocSize);
    if (request == NULL) {
        HDF_LOGE("%s RawMemAlloc fail", __func__);
        return NULL;
    }
    request->numIsoPackets = isoPackets;
    request->buffer = (unsigned char *)request + allocSize - length;
    request->bufLen = (int)length;
    return request;
}

static int32_t AdapterFreeRequest(struct UsbHostRequest *request)
{
    int32_t ret;
    if (request == NULL) {
        DPRINTFN(0, "%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (request->numUrbs >  request->numRetired) {
        OsDiscardUrbs(request, request->numRetired, request->numUrbs);
        ret = OsFreeRequest(request);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    if (request->urbs) {
        RawUsbMemFree(request->urbs);
        request->urbs = NULL;
    }
    RawUsbMemFree((void *)request);
    return HDF_SUCCESS;
}

static int32_t AdapterSubmitRequest(struct UsbHostRequest *request)
{
    int32_t ret;

    if (request == NULL) {
        DPRINTFN(0, "%s:%d request is NULL", __func__, __LINE__);
        return HDF_FAILURE;
    }
    OsalMutexInit(&(request->lock));
    request->actualLength = 0;
    request->numRetired = 0;
    request->numUrbs = 0;
    switch (request->requestType) {
        case USB_REQUEST_TYPE_CONTROL:
            ret = OsSubmitControlRequest(request);
            break;
        case USB_REQUEST_TYPE_ISOCHRONOUS:
            ret = OsSubmitIsoRequest(request);
            break;
        case USB_REQUEST_TYPE_BULK:
        case USB_REQUEST_TYPE_INTERRUPT:
            ret = OsSubmitBulkRequest(request);
            break;
        default:
            DPRINTFN(0, "%s:%d unknown requestType=%u\n", __func__, __LINE__, request->requestType);
            ret = HDF_ERR_INVALID_PARAM;
            break;
    }

    return ret;
}

static int32_t AdapterCancelRequest(const struct UsbHostRequest *request)
{
    if (!request->urbs) {
        DPRINTFN(0, "adapter cancel urb null\n");
        goto END;
    }

    OsDiscardUrbs(request, 0, request->numUrbs);
END:
    return HDF_SUCCESS;
}

static int32_t AdapterUrbCompleteHandle(const struct UsbDeviceHandle *devHandle)
{
    int32_t ret;
    struct UsbDevice *dev = NULL;
    struct OsDev *osDev = NULL;
    struct UsbAdapterUrb *urb = NULL;
    struct Async *as = NULL;
    struct UsbHostRequest *request = NULL;
    if ((devHandle == NULL) || (devHandle->dev == NULL) || (devHandle->dev->privateData == NULL)) {
        HDF_LOGE("%s:%d invalid parameter", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    dev = devHandle->dev;
    osDev = (struct OsDev *)dev->privateData;
    ret = OsReapUrb(devHandle, &as);
    if (as == NULL) {
        HDF_LOGE("as is null\n");
        return HDF_FAILURE;
    }
    urb = &as->urb;
    request = urb->context;
    switch (request->requestType) {
        case USB_REQUEST_TYPE_CONTROL:
            ret = OsControlCompletion(request, as);
            break;
        case USB_REQUEST_TYPE_ISOCHRONOUS:
            ret = OsIsoCompletion(request, as);
            break;
        case USB_REQUEST_TYPE_BULK:
        case USB_REQUEST_TYPE_INTERRUPT:
            ret = OsBulkCompletion(request, as);
            break;
        default:
            DPRINTFN(0, "%s:%d unrecognised requestType %u\n", __func__, __LINE__, request->requestType);
            ret = HDF_FAILURE;
            break;
    }
    return ret;
}

static struct UsbOsAdapterOps g_usbAdapter = {
    .init = AdapterInit,
    .exit = AdapterExit,
    .openDevice = AdapterOpenDevice,
    .closeDevice = AdapterCloseDevice,
    .getConfigDescriptor = AdapterGetConfigDescriptor,
    .getConfiguration = AdapterGetConfiguration,
    .setConfiguration = AdapterSetConfiguration,
    .claimInterface = AdapterClaimInterface,
    .releaseInterface = AdapterReleaseInterface,
    .setInterfaceAltsetting = AdapterSetInterface,
    .clearHalt = AdapterClearHalt,
    .resetDevice = AdapterResetDevice,
    .allocRequest = AdapterAllocRequest,
    .freeRequest = AdapterFreeRequest,
    .submitRequest = AdapterSubmitRequest,
    .cancelRequest = AdapterCancelRequest,
    .urbCompleteHandle = AdapterUrbCompleteHandle,
};

struct UsbOsAdapterOps *UsbAdapterGetOps(void)
{
    return &g_usbAdapter;
}

UsbRawTidType UsbAdapterGetTid(void)
{
    return HDF_FAILURE;
}

int32_t UsbAdapterRegisterSignal(void)
{
    return HDF_SUCCESS;
}

int32_t UsbAdapterKillSignal(struct UsbDeviceHandle *handle, UsbRawTidType tid)
{
    if ((handle != NULL) && (handle->dev != NULL)) {
        struct UsbDevice *dev = handle->dev;
        struct OsDev *osDev = (struct OsDev *)dev->privateData;
        if (osDev != NULL) {
            g_CompleteExit = true;
            OsalSemPost(&osDev->cvWait);
            HDF_LOGD("%s:%d sigal post", __func__, __LINE__);
            return HDF_SUCCESS;
        } else {
            return HDF_FAILURE;
        }
    } else {
        return HDF_FAILURE;
    }
}

int32_t AdapterAtomicInc(OsalAtomic *v)
{
    int32_t valOld;
    int32_t val;
    uint32_t status = 0;
    Atomic *p = NULL;
    if (v) {
        p = (Atomic *)&(v)->counter;
    } else {
        return HDF_FAILURE;
    }
    do {
        __asm__ __volatile__("ldrex   %1, [%4]\n"
                             "mov   %0, %1\n"
                             "add   %1, %1, #1\n"
                             "strex   %2, %1, [%4]"
                             :  "=&r"(valOld), "=&r"(val), "=&r"(status), "+m"(*p)
                             : "r"(p)
                             : "cc");
    } while (__builtin_expect(status != 0, 0));

    return valOld;
}

int32_t AdapterAtomicDec(OsalAtomic *v)
{
    if (v) {
        return LOS_AtomicDecRet((Atomic *)&(v)->counter);
    } else {
        return HDF_FAILURE;
    }
}
