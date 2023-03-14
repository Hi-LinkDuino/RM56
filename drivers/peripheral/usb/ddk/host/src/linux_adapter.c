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

#include "linux_adapter.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#define HDF_LOG_TAG         USB_LINUX_ADAPTER

#define PATH_LEN            24
#define DESC_READ_LEN       256
#define EP_NUM_MAX          30
#define SLEEP_TIME          500000
static void *OsAdapterRealloc(void *ptr, size_t oldSize, size_t newSize)
{
    void *mem = RawUsbMemAlloc(newSize);
    if (mem == NULL) {
        HDF_LOGE("%s:%d", __func__, __LINE__);
        goto OUT;
    }

    if (oldSize > 0) {
        if (memmove_s(mem, newSize, ptr, oldSize) != EOK) {
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
        HDF_LOGE("%s:invalid param\n", __func__);
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
        HDF_LOGE("%s: allocate handle failed", __func__);
        return NULL;
    }

    OsalMutexInit(&handle->lock);

    return handle;
}

static struct UsbDevice *OsAllocDevice(struct UsbSession *session, struct UsbDeviceHandle *handle)
{
    struct UsbDevice *dev = RawUsbMemCalloc(sizeof(*dev));
    if (dev == NULL) {
        HDF_LOGE("%s:%d no memory", __func__, __LINE__);
        return NULL;
    }

    dev->session = session;
    dev->devHandle = handle;

    RawRequestListInit(dev);

    handle->dev = dev;

    return dev;
}

static int32_t OsGetUsbFd(struct UsbDevice *dev, mode_t mode)
{
    char path[PATH_LEN];
    int32_t fd = HDF_FAILURE;
    int32_t ret;

    if (dev == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = sprintf_s(path, sizeof(path), USB_DEV_FS_PATH "/%03u/%03u", dev->busNum, dev->devAddr);
    if (ret < HDF_SUCCESS) {
        HDF_LOGE("%s:%d path error", __func__, __LINE__);
        return HDF_FAILURE;
    }

    fd = open(path, mode | O_CLOEXEC);
    if (fd != HDF_FAILURE) {
        return fd;
    }

    usleep(SLEEP_TIME);
    switch (errno) {
        case ENOENT:
            fd = open(path, mode | O_CLOEXEC);
            if (fd != HDF_FAILURE) {
                return fd;
            }
            ret = HDF_DEV_ERR_NO_DEVICE;
            break;
        case EACCES:
            ret = HDF_ERR_BAD_FD;
            break;
        default:
            ret = HDF_FAILURE;
            break;
    }

    return ret;
}

static int32_t OsReadDescriptors(struct UsbDevice *dev)
{
    int32_t fd = dev->devHandle->fd;
    size_t allocLen = 0;
    uint8_t *ptr = NULL;
    int32_t len;
    int32_t ret;

    do {
        size_t oldLen = allocLen;
        allocLen += DESC_READ_LEN;
        dev->descriptors = OsAdapterRealloc(dev->descriptors, oldLen, allocLen);
        if (!dev->descriptors) {
            HDF_LOGE("%s:%d", __func__, __LINE__);
            return HDF_ERR_MALLOC_FAIL;
        }
        ptr = (uint8_t *)dev->descriptors + dev->descriptorsLength;
        ret = memset_s(ptr, DESC_READ_LEN, 0, DESC_READ_LEN);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
            return ret;
        }

        len = read(fd, ptr, DESC_READ_LEN);
        if (len < 0) {
            HDF_LOGE("read descriptor failed, errno=%d", errno);
            return HDF_ERR_IO;
        }
        dev->descriptorsLength += (size_t)len;
    } while (dev->descriptorsLength == allocLen);

    return HDF_SUCCESS;
}

static int32_t OsParseConfigDescriptors(struct UsbDevice *dev)
{
    struct UsbDeviceDescriptor *deviceDesc = NULL;
    uint8_t i;
    uint8_t numConfigs;
    uint8_t *buffer = NULL;
    size_t descLen;

    deviceDesc = dev->descriptors;
    numConfigs = deviceDesc->bNumConfigurations;
    if (numConfigs == 0) {
        return HDF_SUCCESS;
    }
    dev->configDescriptors = RawUsbMemAlloc(numConfigs * sizeof(struct UsbDeviceConfigDescriptor));
    if (dev->configDescriptors == NULL) {
        HDF_LOGE("%s:%d", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    buffer = (uint8_t *)dev->descriptors + USB_DDK_DT_DEVICE_SIZE;
    descLen = dev->descriptorsLength - USB_DDK_DT_DEVICE_SIZE;

    for (i = 0; i < numConfigs; i++) {
        struct UsbConfigDescriptor *configDesc = NULL;
        uint16_t configLen;

        if (descLen < USB_DDK_DT_CONFIG_SIZE) {
            HDF_LOGE("%s:%d read %zu", __func__, __LINE__, descLen);
            return HDF_ERR_IO;
        }
        configDesc = (struct UsbConfigDescriptor *)buffer;
        if ((configDesc->bDescriptorType != USB_DDK_DT_CONFIG) ||
            (configDesc->bLength < USB_DDK_DT_CONFIG_SIZE)) {
            HDF_LOGE("%s:%d config desc error: type 0x%02x, length %u",
                __func__, __LINE__, configDesc->bDescriptorType, configDesc->bLength);
            return HDF_ERR_IO;
        }
        configLen = Le16ToCpu(configDesc->wTotalLength);
        if (configLen < USB_DDK_DT_CONFIG_SIZE) {
            HDF_LOGE("invalid wTotalLength value %u", configLen);
            return HDF_ERR_IO;
        }
        if (configLen > descLen) {
            HDF_LOGD("%s:%d read %zu/%u",
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

static int32_t OsInitDevice(struct UsbDevice *dev, uint8_t busNum, uint8_t devAddr)
{
    struct UsbDeviceHandle *devHandle = dev->devHandle;
    int32_t fd;
    int32_t ret;

    dev->busNum = busNum;
    dev->devAddr = devAddr;

    fd = OsGetUsbFd(dev, O_RDWR);
    if (fd < 0) {
        return fd;
    }
    devHandle->fd = fd;

    ret = ioctl(fd, USBDEVFS_GET_CAPABILITIES, &devHandle->caps);
    if (ret < 0) {
        HDF_LOGE("%s:%d get capabilities failed, errno=%d",
            __func__, __LINE__, errno);
        devHandle->caps = USB_ADAPTER_CAP_BULK_CONTINUATION;
    }

    dev->descriptorsLength = 0;
    ret = OsReadDescriptors(dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d ret=%d", __func__, __LINE__, ret);
        return ret;
    }
    ret = OsParseConfigDescriptors(dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d ret=%d", __func__, __LINE__, ret);
        return ret;
    }
    ret = memcpy_s(&dev->deviceDescriptor, sizeof(struct UsbDeviceDescriptor),
        dev->descriptors, USB_DDK_DT_DEVICE_SIZE);
    if (ret != EOK) {
        HDF_LOGE("%s:%d ret=%d", __func__, __LINE__, ret);
        ret = HDF_ERR_IO;
    }
    return ret;
}

static int32_t OsGetActiveConfig(struct UsbDevice *dev, int32_t fd)
{
    int32_t ret;
    uint8_t activeConfig = 0;
    struct UsbControlRequestData ctrlData;

    if (dev == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ctrlData.requestType = USB_PIPE_DIRECTION_IN,
    ctrlData.requestCmd = USB_REQUEST_GET_CONFIGURATION,
    ctrlData.value = 0,
    ctrlData.index = 0,
    ctrlData.length = 1,
    ctrlData.timeout = USB_RAW_REQUEST_DEFAULT_TIMEOUT,
    ctrlData.data = &activeConfig;

    ret = ioctl(fd, USBDEVFS_CONTROL, &ctrlData);
    if (ret < 0) {
        HDF_LOGE("%s:%d errno=%d", __func__, __LINE__, errno);
        return HDF_FAILURE;
    }
    dev->activeConfig = activeConfig;

    return HDF_SUCCESS;
}

static void OsFreeIsoUrbs(struct UsbHostRequest *request)
{
    struct UsbAdapterUrb *urb = NULL;

    for (int32_t i = 0; i < request->numUrbs; i++) {
        urb = request->isoUrbs[i];
        if (urb == NULL) {
            break;
        }
        RawUsbMemFree(urb);
    }

    RawUsbMemFree(request->isoUrbs);
    request->isoUrbs = NULL;
}

static void OsDiscardUrbs(const struct UsbHostRequest *request, int32_t first, int32_t last)
{
    struct UsbAdapterUrb *urb = NULL;

    if ((request == NULL) || (request->devHandle == NULL) ||
        (first > URBS_PER_REQUEST) || (first > last)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return;
    }

    for (int32_t i = last - 1; i >= first; i--) {
        if (request->requestType == USB_REQUEST_TYPE_ISOCHRONOUS) {
            urb = request->isoUrbs[i];
        } else {
            urb = &request->urbs[i];
        }
        if (ioctl(request->devHandle->fd, USBDEVFS_DISCARDURB, urb) == 0) {
            continue;
        }
    }
}

static int32_t OsSubmitControlRequest(struct UsbHostRequest *request)
{
    struct UsbAdapterUrb *urb = NULL;
    int32_t ret;
    int32_t fd;

    if ((request == NULL) || (request->length  > MAX_BULK_DATA_BUFFER_LENGTH) ||
        (request->devHandle == NULL)) {
        HDF_LOGD("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    fd = request->devHandle->fd;
    urb = RawUsbMemCalloc(sizeof(*urb));
    if (urb == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    urb->type = USB_ADAPTER_URB_TYPE_CONTROL;
    urb->endPoint = request->endPoint;
    urb->buffer = request->buffer;
    urb->bufferLength = request->length;
    urb->userContext = request;
    request->urbs = urb;
    request->numUrbs = 1;

    ret = ioctl(fd, USBDEVFS_SUBMITURB, urb);
    if (ret < 0) {
        HDF_LOGE("submiturb failed, errno=%d", errno);
        RawUsbMemFree(urb);
        request->urbs = NULL;
        if (errno == ENODEV) {
            return HDF_DEV_ERR_NO_DEVICE;
        }
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t OsSubmitBulkRequestHandleUrb(struct UsbHostRequest *request, struct UsbAdapterUrb *urb,
    int32_t bulkBufferLen, int32_t number)
{
    if (bulkBufferLen == 0) {
        HDF_LOGE("%s:%d bulkBufferLen can not be zero", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    urb->userContext = (void *)request;
    switch (request->requestType) {
        case USB_REQUEST_TYPE_BULK:
            urb->type = USB_ADAPTER_URB_TYPE_BULK;
            break;
        case USB_REQUEST_TYPE_INTERRUPT:
            urb->type = USB_ADAPTER_URB_TYPE_INTERRUPT;
            break;
        default:
            HDF_LOGE("%s:%d unknown requestType=%u", __func__, __LINE__, request->requestType);
            return HDF_ERR_INVALID_PARAM;
    }
    urb->endPoint = request->endPoint;
    urb->buffer = request->buffer + (number * bulkBufferLen);
    if (number == request->numUrbs - 1) {
        uint32_t len = request->length % bulkBufferLen;
        urb->bufferLength = (len == 0) ? bulkBufferLen : len;
    } else {
        urb->bufferLength = bulkBufferLen;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitBulkRequestHandle(
    struct UsbHostRequest *request, struct UsbAdapterUrb *urbs, int32_t bulkBufferLen)
{
    struct UsbAdapterUrb *urb = NULL;
    int32_t fd = request->devHandle->fd;
    int32_t numUrbs = request->numUrbs;
    int32_t i;
    int32_t ret;

    for (i = 0; i < numUrbs; i++) {
        urb = &urbs[i];
        ret = OsSubmitBulkRequestHandleUrb(request, urb, bulkBufferLen, i);
        if (ret != HDF_SUCCESS) {
            return ret;
        }

        ret = ioctl(fd, USBDEVFS_SUBMITURB, urb);
        if (ret == 0) {
            continue;
        }

        if (i == 0) {
            HDF_LOGE("submitUrb: ret=%d errno=%d \
                urb->bufferLength=%d urb->buffer=%p urb->endPoint=%d urb->type=%d",
                ret, errno, urb->bufferLength, urb->buffer, urb->endPoint, urb->type);
            return HDF_ERR_IO;
        }
        request->numRetired += numUrbs - i;
        if (errno != EREMOTEIO) {
            request->reqStatus = USB_REQUEST_ERROR;
        }

        return HDF_SUCCESS;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitBulkRequest(struct UsbHostRequest *request)
{
    struct UsbAdapterUrb *urbs = NULL;
    int32_t bulkBufferLen;
    int32_t numUrbs;

    if (request == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (request->length > MAX_BULK_DATA_BUFFER_LENGTH || request->length <= 0) {
        HDF_LOGE("Bulk request size err");
        return -1;
    }

    if (request->devHandle->caps & USB_ADAPTER_CAP_BULK_SCATTER_GATHER) {
        bulkBufferLen = request->length ? request->length : 1;
    } else if (request->devHandle->caps & USB_ADAPTER_CAP_BULK_CONTINUATION) {
        bulkBufferLen = MAX_BULK_DATA_BUFFER_LENGTH;
    } else if (request->devHandle->caps & USB_ADAPTER_CAP_NO_PACKET_SIZE_LIM) {
        bulkBufferLen = request->length ? request->length : 1;
    } else {
        bulkBufferLen = MAX_BULK_DATA_BUFFER_LENGTH;
    }
    numUrbs = request->length / bulkBufferLen;
    if (request->length == 0) {
        numUrbs = 1;
    } else if ((request->length % bulkBufferLen) > 0) {
        numUrbs++;
    }

    if (numUrbs != 1) {
        urbs = RawUsbMemCalloc(numUrbs * sizeof(*urbs));
        if (request->bulkUrb) {
            RawUsbMemFree(request->bulkUrb);
        }
        request->bulkUrb = urbs;
        request->urbs = NULL;
    } else {
        urbs = request->bulkUrb;
    }

    if (urbs == NULL) {
        HDF_LOGE("%s:%d no mem", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    request->urbs       = urbs;
    request->numUrbs    = numUrbs;
    request->numRetired = 0;
    request->reqStatus = USB_REQUEST_COMPLETED;

    return OsSubmitBulkRequestHandle(request, urbs, bulkBufferLen);
}

static int32_t OsAllocIsoUrbs(struct UsbHostRequest *request, int32_t numUrbs, struct UsbAdapterUrb **urbs)
{
    struct UsbAdapterUrb *urb = NULL;
    unsigned char *urbBuffer = request->buffer;
    int32_t numPacketsLeft = request->numIsoPackets;
    int32_t packetIdx = 0;
    int32_t i, j;

    for (i = 0; i < numUrbs; i++) {
        int32_t numPackets = MIN(numPacketsLeft, MAX_ISO_PACKETS_PER_URB);
        urb = RawUsbMemCalloc(sizeof(struct UsbAdapterUrb));
        if (urb == NULL) {
            OsFreeIsoUrbs(request);
            return HDF_ERR_MALLOC_FAIL;
        }
        urbs[i] = urb;

        for (j = 0; j < numPackets; j++) {
            unsigned int packetLen = request->isoPacketDesc[packetIdx++].length;
            urb->bufferLength += packetLen;
            urb->isoFrameDesc[j].length = packetLen;
        }
        urb->type = USB_ADAPTER_URB_TYPE_ISO;
        urb->flags = USB_ADAPTER_URB_ISO_ASAP;
        urb->endPoint = request->endPoint;
        urb->numberOfPackets = numPackets;
        urb->buffer = (void *)urbBuffer;
        urb->userContext = request;
        urbBuffer += urb->bufferLength;
        numPacketsLeft -= numPackets;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitIsoUrbs(struct UsbHostRequest *request, int32_t numUrbs, struct UsbAdapterUrb **pUrbs)
{
    int32_t i;
    int32_t ret;

    for (i = 0; i < numUrbs; i++) {
        ret = ioctl(request->devHandle->fd, USBDEVFS_SUBMITURB, *pUrbs[i]);
        if (ret == 0) {
            continue;
        }

        if (errno == ENODEV) {
            ret = HDF_DEV_ERR_NO_DEVICE;
        } else {
            HDF_LOGE("%s:%d submit iso urb failed errno=%d", __func__, __LINE__, errno);
            ret = HDF_ERR_IO;
        }

        if (i == 0) {
            HDF_LOGE("first URB failed");
            OsFreeIsoUrbs(request);
            return ret;
        }
        request->reqStatus = USB_REQUEST_ERROR;
        request->numRetired += numUrbs - i;
        if (request->numRetired == numUrbs) {
            RawUsbMemFree(pUrbs);
            request->urbs = NULL;
        }
        break;
    }

    return HDF_SUCCESS;
}

static int32_t OsSubmitIsoRequest(struct UsbHostRequest *request)
{
    struct UsbAdapterUrb **pUrbs = NULL;
    int32_t numUrbs;
    unsigned int packetLen;
    unsigned int totalLen = 0;
    int32_t ret;

    if ((request == NULL) || (request->devHandle == NULL) || (request->numIsoPackets < 1)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (request->length > MAX_ISO_DATA_BUFFER_LEN) {
        HDF_LOGE("%s:%d request length exceed the maximum", __func__, __LINE__);
        return -1;
    }

    for (int32_t i = 0; i < request->numIsoPackets; i++) {
        packetLen = request->isoPacketDesc[i].length;
        if (packetLen > MAX_ISO_DATA_BUFFER_LEN) {
            HDF_LOGE("%s:%d packet length: %u exceeds maximum: %u",
                __func__, __LINE__, packetLen, MAX_ISO_DATA_BUFFER_LEN);
            return HDF_ERR_INVALID_PARAM;
        }
        totalLen += packetLen;
    }
    if (request->length < totalLen) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    numUrbs = (request->numIsoPackets + (MAX_ISO_PACKETS_PER_URB - 1)) / MAX_ISO_PACKETS_PER_URB;
    pUrbs = RawUsbMemCalloc(numUrbs * sizeof(struct UsbAdapterUrb*));
    if (pUrbs == NULL) {
        HDF_LOGE("%s:%d RawUsbMemCalloc pUrbs failed", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    request->isoUrbs = (void **)pUrbs;
    request->numUrbs = numUrbs;
    request->numRetired = 0;
    request->isoPacketOffset = 0;
    ret = OsAllocIsoUrbs(request, numUrbs, pUrbs);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d alloc iso urbs failed", __func__, __LINE__);
        return ret;
    }

    return OsSubmitIsoUrbs(request, numUrbs, pUrbs);
}

static int32_t OsControlCompletion(struct UsbHostRequest *request, struct UsbAdapterUrb *urb)
{
    int32_t status;

    request->actualLength += urb->actualLength;
    if (request->reqStatus == USB_REQUEST_CANCELLED) {
        RawUsbMemFree(request->urbs);
        request->urbs = NULL;
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
            status = USB_REQUEST_STALL;
            break;
        case -EOVERFLOW:
            status = USB_REQUEST_OVERFLOW;
            break;
        case -ENODEV:
        case -ESHUTDOWN:
            status = USB_REQUEST_NO_DEVICE;
            break;
        default:
            status = USB_REQUEST_ERROR;
            break;
    }
    RawUsbMemFree(request->urbs);
    request->urbs = NULL;
    return RawHandleRequestCompletion(request, status);
}

static void OsIsoRequestDesStatus(struct UsbHostRequest *request, struct UsbAdapterUrb *urb)
{
    int32_t i;
    struct UsbIsoPacketDesc *urbDesc = NULL;
    struct UsbIsoPacketDesc *requestDesc = NULL;

    for (i = 0; i < urb->numberOfPackets; i++) {
        urbDesc = &urb->isoFrameDesc[i];
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

        requestDesc->actualLength = urbDesc->actualLength;
    }
}

static int32_t OsIsoCompletion(struct UsbHostRequest *request, struct UsbAdapterUrb *urb)
{
    UsbRequestStatus status;
    int32_t urbIndex = 0;
    int32_t numUrbs;

    if (request == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    numUrbs = request->numUrbs;

    for (int32_t i = 0; i < numUrbs; i++) {
        if (urb == request->isoUrbs[i]) {
            urbIndex = i + 1;
            break;
        }
    }
    if (urbIndex == 0) {
        HDF_LOGE("%s:%d urbIndex is zero", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    OsIsoRequestDesStatus(request, urb);
    request->numRetired++;
    if  (request->reqStatus != USB_REQUEST_COMPLETED) {
        HDF_LOGE("%s:%d urb status=%d", __func__, __LINE__, urb->status);
        if (request->numRetired == numUrbs) {
            OsFreeIsoUrbs(request);
            return RawHandleRequestCompletion(request, USB_REQUEST_ERROR);
        }
        goto OUT;
    }

    if (urb->status == -ESHUTDOWN) {
        status = USB_REQUEST_NO_DEVICE;
    } else if (!((urb->status == HDF_SUCCESS) || (urb->status == -ENOENT) ||
                 (urb->status == -ECONNRESET))) {
        status = USB_REQUEST_ERROR;
    } else {
        status = USB_REQUEST_COMPLETED;
    }

    if (request->numRetired == numUrbs) {
        OsFreeIsoUrbs(request);
        return RawHandleRequestCompletion(request, status);
    }
OUT:
    return 0;
}

static int32_t OsProcessAbnormalReap(struct UsbHostRequest *request, const struct UsbAdapterUrb *urb)
{
    int32_t ret;

    if (urb->actualLength > 0) {
        unsigned char *target = request->buffer + request->actualLength;
        if (urb->buffer != target) {
            ret = memmove_s(target, urb->actualLength, urb->buffer, urb->actualLength);
            if (ret) {
                HDF_LOGE("%s: memmove_s failed", __func__);
            }
        }
        request->actualLength += urb->actualLength;
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
            if (request->reqStatus == USB_REQUEST_COMPLETED) {
                request->reqStatus = USB_REQUEST_OVERFLOW;
            }
            ret = HDF_FAILURE;
            break;
        default:
            if (request->reqStatus == USB_REQUEST_COMPLETED) {
                request->reqStatus = USB_REQUEST_ERROR;
            }
            ret = HDF_DEV_ERR_NO_DEVICE;
            break;
    }

    return ret;
}

static int32_t OsBulkCompletion(struct UsbHostRequest * const request, const struct UsbAdapterUrb *urb)
{
    int32_t ret;
    int32_t urbIdx = urb - (struct UsbAdapterUrb *)request->urbs;

    request->numRetired++;
    if (request->reqStatus != USB_REQUEST_COMPLETED) {
        if (OsProcessAbnormalReap(request, urb) == HDF_SUCCESS) {
            goto COMPLETED;
        } else {
            goto OUT;
        }
    }
    request->actualLength += urb->actualLength;

    ret = OsUrbStatusToRequestStatus(request, urb);
    if (ret == HDF_DEV_ERR_NO_DEVICE) {
        goto CANCEL;
    } else if (ret == HDF_FAILURE) {
        goto COMPLETED;
    }

    if (request->numRetired == request->numUrbs) {
        goto COMPLETED;
    } else if (urb->actualLength < urb->bufferLength) {
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
    return HDF_SUCCESS;
COMPLETED:
    return RawHandleRequestCompletion(request, request->reqStatus);
}

static int32_t AdapterInit(const struct UsbSession *session)
{
    return HDF_SUCCESS;
}

static void AdapterExit(const struct UsbSession *session)
{
    return;
}

static struct UsbDeviceHandle *AdapterOpenDevice(struct UsbSession *session, uint8_t busNum,  uint8_t usbAddr)
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
        return NULL;
    }

    dev = OsAllocDevice(session, handle);
    if (dev == NULL) {
        goto ERR;
    }

    ret = OsInitDevice(dev, busNum, usbAddr);
    if (ret) {
        RawUsbMemFree(dev);
        goto ERR;
    }

    OsalAtomicSet(&dev->refcnt, 1);
    /* add the new device to the device list on session */
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
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return;
    }

    dev = handle->dev;
    if (AdapterAtomicDec(&dev->refcnt) > 0) {
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
    RawUsbMemFree(dev);

    close(handle->fd);
    OsalMutexDestroy(&handle->lock);
    RawUsbMemFree(handle);
}

static int32_t AdapterGetConfigDescriptor(const struct UsbDevice *dev,
    uint8_t configIndex, void *buffer, size_t len)
{
    struct UsbDeviceConfigDescriptor *config = NULL;
    uint8_t i;
    int32_t ret;

    if ((dev == NULL) || (buffer == NULL) ||
        ((dev != NULL) && (configIndex > dev->deviceDescriptor.bNumConfigurations))) {
        HDF_LOGE("%{public}s:%{public}d Invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i < dev->deviceDescriptor.bNumConfigurations; i++) {
        if (configIndex == dev->configDescriptors[i].desc->bConfigurationValue) {
            config = &dev->configDescriptors[i];
            break;
        }
    }

    if (config == NULL) {
        HDF_LOGE("%s:%d config is NULL", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    len = MIN(len, config->actualLen);
    ret = memcpy_s(buffer, len, config->desc, len);
    if (ret != EOK) {
        HDF_LOGE("%s:%d memcpy_s fail", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    return len;
}

static int32_t AdapterGetConfiguration(const struct UsbDeviceHandle *handle, uint8_t *activeConfig)
{
    if ((handle == NULL) || (activeConfig == NULL) || (handle->dev == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ret = OsGetActiveConfig(handle->dev, handle->fd);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    *activeConfig = handle->dev->activeConfig;
    if (*activeConfig == 0) {
        HDF_LOGD("%s:%d activeConfig is zero", __func__, __LINE__);
    }

    return HDF_SUCCESS;
}

static int32_t AdapterSetConfiguration(struct UsbDeviceHandle *handle, int32_t activeConfig)
{
    int32_t ret;

    if ((handle == NULL) || (handle->dev == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = ioctl(handle->fd, USBDEVFS_SETCONFIGURATION, &activeConfig);
    if (ret < 0) {
        HDF_LOGE("%s:%d set config failed errno=%d", __func__, __LINE__, errno);
        return HDF_FAILURE;
    }
    if (activeConfig == HDF_FAILURE) {
        activeConfig = 0;
    }
    handle->dev->activeConfig = (uint8_t)activeConfig;

    return HDF_SUCCESS;
}

static int32_t AdapterClaimInterface(const struct UsbDeviceHandle *handle, unsigned int interfaceNumber)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = ioctl(handle->fd, USBDEVFS_CLAIMINTERFACE, &interfaceNumber);
    if (ret < 0) {
        HDF_LOGE("%s;%d claim failed errno=%d", __func__, __LINE__, errno);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterReleaseInterface(const struct UsbDeviceHandle *handle, unsigned int interfaceNumber)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = ioctl(handle->fd, USBDEVFS_RELEASEINTERFACE, &interfaceNumber);
    if (ret < 0) {
        HDF_LOGE("%s:%d release failed errno=%d", __func__, __LINE__, errno);
        if (errno == ENODEV) {
            return HDF_DEV_ERR_NO_DEVICE;
        }
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterSetInterface(const struct UsbDeviceHandle *handle, uint8_t interface, uint8_t altSetting)
{
    struct UsbAdapterSetInterface setIntf;
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    setIntf.interface = interface;
    setIntf.altSetting = altSetting;
    ret = ioctl(handle->fd, USBDEVFS_SETINTERFACE, &setIntf);
    if (ret < 0) {
        HDF_LOGE("%s:%d set interface failed errno=%d", __func__, __LINE__, errno);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterClearHalt(const struct UsbDeviceHandle *handle, unsigned int endPoint)
{
    int32_t ret;

    if (handle == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = ioctl(handle->fd, USBDEVFS_CLEAR_HALT, &endPoint);
    if (ret < 0) {
        HDF_LOGE("%s:%d clear halt failed errno=%d", __func__, __LINE__, errno);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterResetDevice(const struct UsbDeviceHandle *handle)
{
    int32_t ret;
    uint8_t i;

    if (handle == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i < USB_MAXINTERFACES; i++) {
        if (handle->claimedInterfaces & (1UL << i)) {
            AdapterReleaseInterface(handle, i);
        }
    }

    ret = ioctl(handle->fd, USBDEVFS_RESET, NULL);
    if (ret < 0) {
        HDF_LOGE("%s:%d errno=%d", __func__, __LINE__, errno);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static struct UsbHostRequest *AdapterAllocRequest(const struct UsbDeviceHandle *handle, int32_t isoPackets, size_t len)
{
    void *memBuf = NULL;
    size_t allocSize;
    struct UsbHostRequest *request = NULL;
    if (handle == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return NULL;
    }
    allocSize = sizeof(struct UsbHostRequest)
                + (sizeof(struct UsbIsoPacketDesc) * (size_t)isoPackets)
                + (sizeof(unsigned char) * len);

    memBuf = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
    if (memBuf == MAP_FAILED) {
        HDF_LOGE("%s:%d mmap failed, errno=%d", __func__, __LINE__, errno);
        return NULL;
    }
    request = (struct UsbHostRequest *)memBuf;
    request->numIsoPackets = isoPackets;
    request->buffer = (unsigned char *)request + allocSize - len;
    request->bufLen = len;
    request->bulkUrb = RawUsbMemCalloc(sizeof(struct UsbAdapterUrb));
    if (request->bulkUrb == NULL) {
        HDF_LOGE("%s RawUsbMemAlloc fail", __func__);
        return NULL;
    }
    request->urbs = request->bulkUrb;
    return request;
}

static int32_t AdapterFreeRequest(struct UsbHostRequest *request)
{
    size_t allocSize;
    if (request == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    allocSize = sizeof(struct UsbHostRequest)
                + (sizeof(struct UsbIsoPacketDesc) * (size_t)(request->numIsoPackets))
                + request->bufLen;
    if (request->bulkUrb) {
        RawUsbMemFree(request->bulkUrb);
        request->bulkUrb = NULL;
    }
    request->urbs = NULL;
    if (munmap((void *)request, allocSize) != 0) {
        HDF_LOGE("%s:%d munmap failed, errno=%d", __func__, __LINE__, errno);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterSubmitRequest(struct UsbHostRequest *request)
{
    int32_t ret;

    if (request == NULL) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_FAILURE;
    }

    request->actualLength = 0;
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
            HDF_LOGE("%s:%d unknown requestType=%u", __func__, __LINE__, request->requestType);
            ret = HDF_ERR_INVALID_PARAM;
            break;
    }

    return ret;
}

static int32_t AdapterCancelRequest(struct UsbHostRequest * const request)
{
    if (!request->urbs) {
        HDF_LOGE("%s:%d", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    OsDiscardUrbs(request, 0, request->numUrbs);

    if (!((request->requestType == USB_REQUEST_TYPE_BULK) && (request->reqStatus == USB_REQUEST_ERROR))) {
        request->reqStatus = USB_REQUEST_CANCELLED;
    }

    return HDF_SUCCESS;
}

static int32_t AdapterUrbCompleteHandle(const struct UsbDeviceHandle *devHandle)
{
    struct UsbAdapterUrb *urb = NULL;
    struct UsbHostRequest *request = NULL;
    int32_t ret;

    if (devHandle == NULL) {
        HDF_LOGE("%s:%d invalid parameter", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = ioctl(devHandle->fd, USBDEVFS_REAPURB, &urb);
    if (ret < 0) {
        HDF_LOGE("%s:%d reap failed, errno=%d", __func__, __LINE__, errno);
        if (errno == EAGAIN) {
            return 1;
        }
        if (errno == ENODEV) {
            return HDF_DEV_ERR_NO_DEVICE;
        }

        return HDF_ERR_IO;
    }

    request = urb->userContext;

    switch (request->requestType) {
        case USB_REQUEST_TYPE_CONTROL:
            ret = OsControlCompletion(request, urb);
            break;
        case USB_REQUEST_TYPE_ISOCHRONOUS:
            ret = OsIsoCompletion(request, urb);
            break;
        case USB_REQUEST_TYPE_BULK:
        case USB_REQUEST_TYPE_INTERRUPT:
            ret = OsBulkCompletion(request, (const struct UsbAdapterUrb *)urb);
            break;
        default:
            HDF_LOGE("%s:%d unrecognised requestType %u", __func__, __LINE__, request->requestType);
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

static void OsSignalHandler(int32_t signo)
{
    switch (signo) {
        case SIGUSR1:
            break;
        default:
            break;
    }
}

struct UsbOsAdapterOps *UsbAdapterGetOps(void)
{
    return &g_usbAdapter;
}

UsbRawTidType UsbAdapterGetTid(void)
{
    return gettid();
}

int32_t UsbAdapterRegisterSignal(void)
{
    if (signal(SIGUSR1, OsSignalHandler) == SIG_ERR) {
        HDF_LOGE("%s:%d Can't set AdapterSignalHandler for SIGUSR1", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

int32_t UsbAdapterKillSignal(struct UsbDeviceHandle *devHandle, UsbRawTidType tid)
{
    if (kill(tid, SIGUSR1) == HDF_ERR_IO) {
        HDF_LOGE("%s:%d kill tid=%d SIGUSR1 failed", __func__, __LINE__, tid);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

int32_t AdapterAtomicInc(OsalAtomic *v)
{
    return OsalAtomicInc(v);
}

int32_t AdapterAtomicDec(OsalAtomic *v)
{
    return OsalAtomicDec(v);
}
