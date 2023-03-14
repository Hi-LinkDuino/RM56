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

#include "usb_raw_api_library.h"
#include "linux_adapter.h"

#define HDF_LOG_TAG USB_RAW_API_LIBRARY

struct UsbSession *g_usbRawDefaultSession = NULL;
static struct RawUsbRamTestList *g_usbRamTestHead = NULL;
static bool g_usbRamTestFlag = false;

static void SyncRequestCallback(const void *requestArg)
{
    struct UsbHostRequest *request = (struct UsbHostRequest *)requestArg;
    if ((request == NULL) || (request->userData == NULL)) {
        HDF_LOGE("%s:%d request is NULL!", __func__, __LINE__);
        return;
    }

    int32_t *completed = request->userData;
    *completed = 1;
    OsalSemPost(&request->sem);
}

static inline unsigned char *ControlRequestGetData(const struct UsbHostRequest *request)
{
    return request->buffer + USB_RAW_CONTROL_SETUP_SIZE;
}

static int32_t HandleSyncRequestCompletion(const struct UsbHostRequest *request, struct UsbRequestData *requestData)
{
    int32_t ret;
    uint32_t waitTime;

    if (request->timeout == USB_RAW_REQUEST_TIME_ZERO_MS) {
        waitTime = HDF_WAIT_FOREVER;
    } else {
        waitTime = request->timeout;
    }

    ret = OsalSemWait((struct OsalSem *)&request->sem, waitTime);
    if (ret == HDF_ERR_TIMEOUT) {
        RawCancelRequest(request);
        RawHandleRequestCompletion((struct UsbHostRequest *)request, USB_REQUEST_TIMEOUT);
    } else if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalSemWait faile, ret=%d ", __func__, __LINE__, ret);
        goto OUT;
    }

    if (requestData->requested) {
        *(requestData->requested) = request->actualLength;
    }

    switch (request->status) {
        case USB_REQUEST_COMPLETED:
            ret = HDF_SUCCESS;
            break;
        case USB_REQUEST_TIMEOUT:
            ret = HDF_ERR_TIMEOUT;
            break;
        case USB_REQUEST_NO_DEVICE:
            ret = HDF_DEV_ERR_NO_DEVICE;
            break;
        case USB_REQUEST_STALL:
        case USB_REQUEST_OVERFLOW:
        case USB_REQUEST_ERROR:
        case USB_REQUEST_CANCELLED:
            ret = HDF_ERR_IO;
            break;
        default:
            HDF_LOGW("%s: unrecognised status code %d", __func__, request->status);
            ret = HDF_FAILURE;
            break;
    }

OUT:
    OsalSemDestroy((struct OsalSem *)&request->sem);
    return ret;
}

static int32_t HandleSyncRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbRequestData *requestData, unsigned char type)
{
    int32_t ret;
    int32_t completed = 0;

    if (UsbEndpointDirOut(requestData->endPoint)) {
        ret = memcpy_s(request->buffer, request->bufLen, requestData->data, requestData->length);
        if (ret != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail!", __func__, __LINE__);
            return ret;
        }
    }

    request->devHandle      = (struct UsbDeviceHandle *)devHandle;
    request->endPoint       = requestData->endPoint;
    request->requestType    = type;
    request->timeout        = requestData->timeout;
    request->length         = requestData->length;
    request->userData       = &completed;
    request->callback       = SyncRequestCallback;
    request->userCallback   = NULL;

    ret = OsalSemInit(&request->sem, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalSemInit faile, ret=%d ", __func__, __LINE__, ret);
        return ret;
    }

    ret = RawSubmitRequest(request);
    if (ret < 0) {
        OsalSemDestroy(&request->sem);
        return ret;
    }

    return HandleSyncRequestCompletion(request, (struct UsbRequestData *)requestData);
}

static void GetInterfaceNumberDes(
    const struct UsbiDescriptorHeader *header, uint8_t nIntf[], uint8_t nAlts[], int32_t *num)
{
    uint8_t inum;
    int32_t i;
    struct UsbiInterfaceDescriptor *desc = NULL;

    desc = (struct UsbiInterfaceDescriptor *)header;
    if (desc->bLength < USB_DDK_DT_INTERFACE_SIZE) {
        HDF_LOGW("%s: invalid interface descriptor length %d, skipping", \
            __func__, desc->bLength);
        return;
    }

    inum = desc->bInterfaceNumber;
    for (i = 0; i < *num; ++i) {
        if (nIntf[i] == inum) {
            break;
        }
    }
    if (i < *num) {
        if (nAlts[i] < USB_MAXALTSETTING) {
            ++nAlts[i];
        }
    } else if (*num < USB_MAXINTERFACES) {
        nIntf[*num] = inum;
        nAlts[*num] = 1;
        ++*num;
    }
}

static int32_t GetInterfaceNumber(const uint8_t *buffer, size_t size, uint8_t nIntf[], uint8_t nAlts[])
{
    struct UsbiDescriptorHeader *header = NULL;
    const uint8_t *buffer2;
    size_t size2;
    int32_t num = 0;

    for ((buffer2 = buffer, size2 = size);
         size2 > 0;
         (buffer2 += header->bLength, size2 -= header->bLength)) {
        if (size2 < sizeof(struct UsbiDescriptorHeader)) {
            HDF_LOGW("%s: descriptor has %zu excess bytes", __func__, size2);
            break;
        }
        header = (struct UsbiDescriptorHeader *)buffer2;
        if ((header->bLength > size2) || (header->bLength < sizeof(struct UsbDescriptorHeader))) {
            HDF_LOGW("%s: invalid descriptor length %d, skipping remainder",
                     __func__, header->bLength);
            break;
        }

        if (header->bDescriptorType == USB_DDK_DT_INTERFACE) {
            GetInterfaceNumberDes(header, nIntf, nAlts, &num);
        }
    }

    return num;
}

static int32_t FindNextDescriptor(const uint8_t *buffer, size_t size)
{
    struct UsbDescriptorHeader *h = NULL;
    const uint8_t *buffer0 = buffer;

    while (size > 0) {
        h = (struct UsbDescriptorHeader *)buffer;
        if (h->bDescriptorType == USB_DDK_DT_INTERFACE || h->bDescriptorType == USB_DDK_DT_ENDPOINT) {
            break;
        }
        buffer += h->bLength;
        size -= h->bLength;
    }

    return buffer - buffer0;
}
static int32_t GetConfigDescriptor(const struct UsbDevice *dev, uint8_t configIdx,
    uint8_t *buffer, size_t size)
{
    int32_t ret;
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (dev == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!osAdapterOps->getConfigDescriptor) {
        HDF_LOGE("%s: getConfigDescriptor is NULL", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = osAdapterOps->getConfigDescriptor(dev, configIdx, buffer, size);
    if (ret < 0) {
        HDF_LOGE("%s: getConfigDescriptor error = %d", __func__, ret);
        return ret;
    }

    if (ret < USB_DDK_DT_CONFIG_SIZE) {
        HDF_LOGE("%s: short config descriptor read error = %d", __func__, ret);
        return HDF_ERR_IO;
    } else if (ret != (int)size) {
        HDF_LOGE("%s: short config descriptor read size=%d, ret=%d",
            __func__, (int)size, ret);
    }

    return ret;
}

static void ParseDescriptor(const void *source, enum UsbRawDescriptorType bDescriptorType, void *dest)
{
    int32_t ret;

    if (source == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return;
    }

    switch (bDescriptorType) {
        case USB_RAW_CONFIG_DESCRIPTOR_TYPE: {
            struct UsbConfigDescriptor *desc = (struct UsbConfigDescriptor *)dest;
            ret = memcpy_s(dest, sizeof(struct UsbConfigDescriptor), source, USB_DDK_DT_CONFIG_SIZE);
            if (ret != EOK) {
                HDF_LOGE("%s:%d memcpy_s fail, ret=%d", __func__, __LINE__, ret);
                break;
            }
            desc->wTotalLength = Le16ToCpu(desc->wTotalLength);
            break;
        }
        case USB_RAW_INTERFACE_DESCRIPTOR_TYPE: {
            ret = memcpy_s(dest, sizeof(struct UsbInterfaceDescriptor), source, USB_DDK_DT_INTERFACE_SIZE);
            if (ret != EOK) {
                HDF_LOGE("%s:%d memcpy_s fail, ret=%d", __func__, __LINE__, ret);
            }
            break;
        }
        case USB_RAW_ENDPOINT_DESCRIPTOR_TYPE: {
            struct UsbEndpointDescriptor *desc = (struct UsbEndpointDescriptor *)dest;
            ret = memcpy_s(dest, sizeof(struct UsbEndpointDescriptor), source, USB_DDK_DT_ENDPOINT_SIZE);
            if (ret != EOK) {
                HDF_LOGE("%s:%d memcpy_s fail, ret=%d", __func__, __LINE__, ret);
                break;
            }
            desc->wMaxPacketSize = Le16ToCpu(desc->wMaxPacketSize);
            break;
        }
        case USB_RAW_AUDIO_ENDPOINT_DESCRIPTOR_TYPE: {
            struct UsbEndpointDescriptor *desc = (struct UsbEndpointDescriptor *)dest;
            ret = memcpy_s(dest, sizeof(struct UsbEndpointDescriptor), source, USB_DDK_DT_ENDPOINT_AUDIO_SIZE);
            if (ret != EOK) {
                HDF_LOGE("%s:%d memcpy_s fail, ret=%d", __func__, __LINE__, ret);
                break;
            }
            desc->wMaxPacketSize = Le16ToCpu(desc->wMaxPacketSize);
            break;
        }
        default:
            HDF_LOGE("%s: error bDescriptorType=%d", __func__, bDescriptorType);
            break;
    }
}

static void ClearEndpoint(struct UsbRawEndpointDescriptor *endPoint)
{
    if ((endPoint != NULL) && (endPoint->extra != NULL)) {
        RawUsbMemFree((void *)endPoint->extra);
        endPoint->extra = NULL;
    }
}

static int32_t ParseEndpoint(struct UsbRawEndpointDescriptor *endPoint, const uint8_t *buffer, int32_t size)
{
    const uint8_t *buffer0 = buffer;
    const struct UsbiDescriptorHeader *header = NULL;
    void *extra = NULL;
    int32_t len;
    int32_t ret;

    if (size < DESC_HEADER_LENGTH) {
        HDF_LOGE("%s: size = %d is short endPoint descriptor ", __func__, size);
        return HDF_ERR_IO;
    }

    header = (const struct UsbiDescriptorHeader *)buffer;
    if ((header->bDescriptorType != USB_DDK_DT_ENDPOINT) ||
        (header->bLength > size)) {
        HDF_LOGE("%s: unexpected descriptor, type = 0x%x, length = %u",
                 __func__, header->bDescriptorType, header->bLength);
        return buffer - buffer0;
    } else if (header->bLength < USB_DDK_DT_ENDPOINT_SIZE) {
        HDF_LOGE("%s: invalid endpoint length = %u", __func__, header->bLength);
        return HDF_ERR_IO;
    }

    if (header->bLength >= USB_DDK_DT_ENDPOINT_AUDIO_SIZE) {
        ParseDescriptor(buffer, USB_RAW_AUDIO_ENDPOINT_DESCRIPTOR_TYPE, endPoint);
    } else {
        ParseDescriptor(buffer, USB_RAW_ENDPOINT_DESCRIPTOR_TYPE, endPoint);
    }

    buffer += header->bLength;
    size -= header->bLength;

    len = FindNextDescriptor(buffer, size);
    if (!len) {
        return buffer - buffer0;
    }

    extra = RawUsbMemAlloc((size_t)len);
    if (extra == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = memcpy_s(extra, len + endPoint->extraLength, buffer, len);
    if (ret != EOK) {
        HDF_LOGE("%s:%d memcpy_s fail!", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    endPoint->extra = extra;
    endPoint->extraLength = len;

    return buffer + len - buffer0;
}

static void ClearInterface(const struct UsbRawInterface *usbInterface)
{
    struct UsbRawInterfaceDescriptor *infPtr = NULL;
    uint8_t i;
    uint8_t j;

    if (usbInterface == NULL) {
        HDF_LOGE("%s:%d usbInterface is NULL", __func__, __LINE__);
        return;
    }

    if (usbInterface->numAltsetting > USB_MAXALTSETTING) {
        HDF_LOGE("%s:%d numAltsetting=%d is error",
            __func__, __LINE__, usbInterface->numAltsetting);
        return;
    }

    for (i = 0; i < usbInterface->numAltsetting; i++) {
        infPtr = (struct UsbRawInterfaceDescriptor *)(usbInterface->altsetting + i);
        if (infPtr == NULL) {
            HDF_LOGE("%s:%d altsetting is NULL", __func__, __LINE__);
            continue;
        }

        if (infPtr->extra != NULL) {
            RawUsbMemFree((void *)infPtr->extra);
            infPtr->extra = NULL;
        }

        if (infPtr->endPoint != NULL) {
            for (j = 0; j < infPtr->interfaceDescriptor.bNumEndpoints; j++) {
                ClearEndpoint((struct UsbRawEndpointDescriptor *)infPtr->endPoint + j);
            }

            RawUsbMemFree((void *)infPtr->endPoint);
            infPtr->endPoint = NULL;
        }
    }

    RawUsbMemFree((void *)usbInterface);
}

static int32_t RawParseDescriptor(int32_t size, const uint8_t *buffer, enum UsbRawDescriptorType bDescriptorType,
    const struct UsbRawInterfaceDescriptor *ifp)
{
    int32_t ret = HDF_SUCCESS;

    ParseDescriptor(buffer, bDescriptorType, (void *)ifp);
    if ((ifp->interfaceDescriptor.bDescriptorType != USB_DDK_DT_INTERFACE) ||
        (ifp->interfaceDescriptor.bLength > size)) {
        HDF_LOGE("%s: unexpected descriptor: type = 0x%x, size = %d",
                 __func__, ifp->interfaceDescriptor.bDescriptorType, size);
        ret = HDF_FAILURE;
    } else if ((ifp->interfaceDescriptor.bLength < USB_DDK_DT_INTERFACE_SIZE) ||
               (ifp->interfaceDescriptor.bNumEndpoints > USB_MAXENDPOINTS)) {
        HDF_LOGE("%s: invalid descriptor: length = %u, numEndpoints = %u ", __func__,
                 ifp->interfaceDescriptor.bLength, ifp->interfaceDescriptor.bNumEndpoints);
        ret = HDF_ERR_IO;
    }

    return ret;
}

static int32_t ParseInterfaceCopy(struct UsbRawInterfaceDescriptor * const ifp, int32_t len, const uint8_t *buffer)
{
    int32_t ret;

    ifp->extra = RawUsbMemAlloc((size_t)len);
    if (!ifp->extra) {
        ret = HDF_ERR_MALLOC_FAIL;
        goto ERR;
    }

    ret = memcpy_s((void *)ifp->extra, len + ifp->extraLength, buffer, len);
    if (ret != EOK) {
        HDF_LOGE("%s:%d memcpy_s fail, ret=%d", __func__, __LINE__, ret);
        goto ERR;
    }
    ifp->extraLength = len;
ERR:
    return ret;
}

static int32_t ParseInterfaceEndpoint(struct UsbRawInterfaceDescriptor *ifp, const uint8_t **buffer, int32_t *size)
{
    struct UsbRawEndpointDescriptor *endPoint = NULL;
    int32_t ret = HDF_SUCCESS;
    uint8_t i;

    if (ifp->interfaceDescriptor.bNumEndpoints > 0) {
        endPoint = RawUsbMemCalloc(ifp->interfaceDescriptor.bNumEndpoints * sizeof(*endPoint));
        if (endPoint == NULL) {
            ret = HDF_ERR_MALLOC_FAIL;
            goto ERR;
        }

        ifp->endPoint = endPoint;
        for (i = 0; i < ifp->interfaceDescriptor.bNumEndpoints; i++) {
            ret = ParseEndpoint(endPoint + i, *buffer, *size);
            if (ret == 0) {
                ifp->interfaceDescriptor.bNumEndpoints = i;
                break;
            } else if (ret < 0) {
                goto ERR;
            }

            *buffer += ret;
            *size -= ret;
        }
    }

ERR:
    return ret;
}

static int32_t ParseInterface(struct UsbRawInterface *usbInterface, const uint8_t *buffer, int32_t size)
{
    int32_t len;
    int32_t ret;
    const uint8_t *buffer0 = buffer;
    int32_t interfaceNumber = -1;
    const struct UsbiInterfaceDescriptor *ifDesc = NULL;
    struct UsbRawInterfaceDescriptor *ifp = NULL;
    bool tempFlag = false;

    if (usbInterface == NULL) {
        HDF_LOGD("%s:%d usbInterface is NULL", __func__, __LINE__);
        ret = HDF_DEV_ERR_NORANGE;
        goto ERR;
    }
    if (usbInterface->numAltsetting > USB_MAXALTSETTING) {
        HDF_LOGE("%s:%d numAltsetting=%d is error",
            __func__, __LINE__, usbInterface->numAltsetting);
        ret = HDF_DEV_ERR_NORANGE;
        goto ERR;
    }

    while (size >= USB_DDK_DT_INTERFACE_SIZE) {
        ifp = (struct UsbRawInterfaceDescriptor *)(usbInterface->altsetting + usbInterface->numAltsetting);
        ret = RawParseDescriptor(size, buffer, USB_RAW_INTERFACE_DESCRIPTOR_TYPE, ifp);
        if (ret == HDF_FAILURE) {
            return buffer - buffer0;
        } else if (ret == HDF_ERR_IO) {
            HDF_LOGD("%s:%d ret=%d", __func__, __LINE__, ret);
            goto ERR;
        }

        usbInterface->numAltsetting++;
        ifp->extra = NULL;
        ifp->extraLength = 0;
        ifp->endPoint = NULL;

        if (interfaceNumber == -1) {
            interfaceNumber = ifp->interfaceDescriptor.bInterfaceNumber;
        }

        buffer += ifp->interfaceDescriptor.bLength;
        size -= (int)ifp->interfaceDescriptor.bLength;

        len = FindNextDescriptor(buffer, size);
        if (len) {
            ret = ParseInterfaceCopy(ifp, len, buffer);
            if (ret != EOK) {
                goto ERR;
            }

            buffer += len;
            size -= len;
        }

        ret = ParseInterfaceEndpoint(ifp, &buffer, &size);
        if (ret < HDF_SUCCESS) {
            goto ERR;
        }

        ifDesc = (const struct UsbiInterfaceDescriptor *)buffer;
        tempFlag = (size < USB_DDK_DT_INTERFACE_SIZE) ||
                   (ifDesc->bDescriptorType != USB_DDK_DT_INTERFACE) ||
                   (ifDesc->bInterfaceNumber != interfaceNumber);
        if (tempFlag) {
            return buffer - buffer0;
        }
    }

    return buffer - buffer0;

ERR:
    return ret;
}

static int32_t ParseConfigurationDes(struct UsbRawConfigDescriptor *config, const uint8_t *buffer,
    int32_t size, struct UsbRawInterface *usbInterface, uint8_t nIntf[])
{
    int32_t ret, len;
    uint8_t i;

    len = FindNextDescriptor(buffer, size);
    if (len) {
        config->extra = RawUsbMemAlloc(len);
        if (!config->extra) {
            ret = HDF_ERR_MALLOC_FAIL;
            goto ERR;
        }

        ret = memcpy_s((void *)config->extra, len + config->extraLength, buffer, len);
        if (ret != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail! ret=%d", __func__, __LINE__, ret);
            goto ERR;
        }
        config->extraLength = len;

        buffer += len;
        size -= len;
    }

    while (size > 0) {
        struct UsbiInterfaceDescriptor *ifDesc = (struct UsbiInterfaceDescriptor *)buffer;
        for (i = 0; i < config->configDescriptor.bNumInterfaces; ++i) {
            if (nIntf[i] == ifDesc->bInterfaceNumber) {
                usbInterface = (struct UsbRawInterface *)config->interface[i];
                break;
            }
        }
        ret = ParseInterface(usbInterface, buffer, size);
        if (ret < 0) {
            goto ERR;
        }

        buffer += ret;
        size -= ret;
    }

    return size;

ERR:
    RawClearConfiguration(config);
    return ret;
}

static int32_t ParseConfiguration(struct UsbRawConfigDescriptor *config, const uint8_t *buffer, int32_t size)
{
    int32_t i;
    uint8_t j;
    uint32_t len;
    struct UsbRawInterface *usbInterface = NULL;
    uint8_t nIntf[USB_MAXINTERFACES] = {0};
    uint8_t nAlts[USB_MAXINTERFACES] = {0};
    int32_t intfNum;

    if ((size < USB_DDK_DT_CONFIG_SIZE) || (config == NULL)) {
        HDF_LOGE("%s: size=%d is short, or config is NULL!", __func__, size);
        return HDF_ERR_IO;
    }

    ParseDescriptor(buffer, USB_RAW_CONFIG_DESCRIPTOR_TYPE, config);
    if ((config->configDescriptor.bDescriptorType != USB_DDK_DT_CONFIG) ||
        (config->configDescriptor.bLength < USB_DDK_DT_CONFIG_SIZE) ||
        (config->configDescriptor.bLength > (uint8_t)size) ||
        (config->configDescriptor.bNumInterfaces > USB_MAXINTERFACES)) {
        HDF_LOGE("%s: invalid descriptor: type = 0x%x, length = %u", __func__,
                 config->configDescriptor.bDescriptorType, config->configDescriptor.bLength);
        return HDF_ERR_IO;
    }

    intfNum = GetInterfaceNumber(buffer, size, nIntf, nAlts);
    config->configDescriptor.bNumInterfaces = (uint8_t)intfNum;

    for (i = 0; i < intfNum; ++i) {
        j = nAlts[i];
        if (j > USB_MAXALTSETTING) {
            HDF_LOGW("%s: too many alternate settings: %hhu", __func__, j);
            nAlts[i] = USB_MAXALTSETTING;
            j = USB_MAXALTSETTING;
        }
        len = sizeof(struct UsbRawInterface) + sizeof(struct UsbRawInterfaceDescriptor) * j;
        usbInterface = RawUsbMemCalloc(len);
        config->interface[i] = usbInterface;
        if (usbInterface == NULL) {
            return HDF_ERR_MALLOC_FAIL;
        }
    }

    buffer += config->configDescriptor.bLength;
    size -= (int32_t)config->configDescriptor.bLength;

    return ParseConfigurationDes(config, buffer, size, usbInterface, nIntf);
}

static int32_t DescToConfig(const uint8_t *buf, int32_t size, struct UsbRawConfigDescriptor **config)
{
    struct UsbRawConfigDescriptor *tmpConfig = RawUsbMemCalloc(sizeof(struct UsbRawConfigDescriptor));
    int32_t ret;

    if (tmpConfig == NULL) {
        HDF_LOGE("%s: RawUsbMemCalloc failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = ParseConfiguration(tmpConfig, buf, size);
    if (ret < 0 && tmpConfig != NULL) {
        HDF_LOGE("%s: ParseConfiguration failed with error = %d", __func__, ret);
        RawUsbMemFree(tmpConfig);
        tmpConfig = NULL;
        return ret;
    } else if (ret > 0) {
        HDF_LOGW("%s: still %d bytes of descriptor data left", __func__, ret);
    }

    *config = tmpConfig;

    return ret;
}

static int32_t ControlRequestCompletion(
    const struct UsbHostRequest *request, struct UsbControlRequestData *requestData)
{
    int32_t ret;
    uint32_t waitTime;

    if (request->timeout == USB_RAW_REQUEST_TIME_ZERO_MS) {
        waitTime = HDF_WAIT_FOREVER;
    } else {
        waitTime = request->timeout;
    }

    ret = OsalSemWait((struct OsalSem *)&request->sem, waitTime);
    if (ret == HDF_ERR_TIMEOUT) {
        RawCancelRequest(request);
        RawHandleRequestCompletion((struct UsbHostRequest *)request, USB_REQUEST_TIMEOUT);
    } else if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalSemWait faile, ret=%d ", __func__, __LINE__, ret);
        goto OUT;
    }

    if ((requestData->requestType & USB_DDK_ENDPOINT_DIR_MASK) == USB_PIPE_DIRECTION_IN) {
        ret = memcpy_s(requestData->data, request->actualLength + requestData->length,
            ControlRequestGetData(request), request->actualLength);
        if (ret != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail! ret=%d", __func__, __LINE__, ret);
            goto OUT;
        }
    }

    switch (request->status) {
        case USB_REQUEST_COMPLETED:
            ret = request->actualLength;
            break;
        case USB_REQUEST_TIMEOUT:
            ret = HDF_ERR_TIMEOUT;
            break;
        case USB_REQUEST_NO_DEVICE:
            ret = HDF_DEV_ERR_NO_DEVICE;
            break;
        case USB_REQUEST_STALL:
        case USB_REQUEST_OVERFLOW:
        case USB_REQUEST_ERROR:
        case USB_REQUEST_CANCELLED:
            ret = HDF_ERR_IO;
            break;
        default:
            HDF_LOGW("%s: status=%d is unrecognised", __func__, request->status);
            ret = HDF_FAILURE;
            break;
    }

OUT:
    OsalSemDestroy((struct OsalSem *)&request->sem);
    return ret;
}

struct UsbSession *RawGetSession(const struct UsbSession *session)
{
    return (struct UsbSession *)(session ? session : g_usbRawDefaultSession);
}

int32_t RawInit(struct UsbSession **session)
{
    int32_t ret;
    struct UsbSession *tempSession = NULL;
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if ((session == NULL) && (g_usbRawDefaultSession != NULL)) {
        AdapterAtomicInc(&g_usbRawDefaultSession->refCount);
        return HDF_SUCCESS;
    }

    tempSession = (struct UsbSession *)RawUsbMemCalloc(sizeof(*tempSession));
    if (tempSession == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    OsalAtomicSet(&tempSession->refCount, 1);
    HdfSListInit(&tempSession->usbDevs);
    DListHeadInit(&tempSession->ifacePoolList);
    OsalMutexInit(&tempSession->lock);
    if ((session == NULL) && (g_usbRawDefaultSession == NULL)) {
        g_usbRawDefaultSession = tempSession;
        HDF_LOGI("%s: created default context", __func__);
    }

    if (osAdapterOps->init) {
        ret = osAdapterOps->init(tempSession);
        if (ret < 0) {
            HDF_LOGE("%s: init error, return %d", __func__, ret);
            goto ERR_FREE_SESSION;
        }
    } else {
        ret = HDF_ERR_NOT_SUPPORT;
        goto ERR_FREE_SESSION;
    }

    if (session != NULL) {
        *session = tempSession;
    }

    return HDF_SUCCESS;

ERR_FREE_SESSION:
    if (tempSession == g_usbRawDefaultSession) {
        g_usbRawDefaultSession = NULL;
    }

    RawUsbMemFree(tempSession);
    return ret;
}

int32_t RawExit(const struct UsbSession *session)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();
    struct UsbSession *realSession = RawGetSession(session);
    if ((realSession == NULL) || (AdapterAtomicDec(&realSession->refCount) > 0)) {
        return HDF_SUCCESS;
    }

    if (osAdapterOps->exit) {
        osAdapterOps->exit(realSession);
    }
    if (realSession == g_usbRawDefaultSession) {
        g_usbRawDefaultSession = NULL;
    }

    OsalMutexDestroy(&realSession->lock);
    RawUsbMemFree(realSession);

    return HDF_SUCCESS;
}

struct UsbDeviceHandle *RawOpenDevice(const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();
    struct UsbSession *realSession = NULL;

    if (osAdapterOps->openDevice == NULL) {
        HDF_LOGE("%s: openDevice is NULL", __func__);
        return NULL;
    }

    realSession = RawGetSession(session);
    if (realSession == NULL) {
        return NULL;
    }

    return osAdapterOps->openDevice(realSession, busNum, usbAddr);
}

int32_t RawCloseDevice(const struct UsbDeviceHandle *devHandle)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (devHandle == NULL) {
        HDF_LOGE("%s devHandle is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (osAdapterOps->closeDevice) {
        osAdapterOps->closeDevice((struct UsbDeviceHandle *)devHandle);
    } else {
        return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

int32_t RawClaimInterface(struct UsbDeviceHandle *devHandle, int32_t interfaceNumber)
{
    int32_t ret;
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if ((devHandle == NULL) || (interfaceNumber < 0) || (interfaceNumber >= USB_MAXINTERFACES)) {
        HDF_LOGE("%s:%d HDF_ERR_INVALID_PARAM", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock(&devHandle->lock);
    if ((devHandle->claimedInterfaces) & (1U << (uint32_t)interfaceNumber)) {
        ret = HDF_SUCCESS;
        goto OUT;
    }

    if (!osAdapterOps->claimInterface) {
        ret = HDF_ERR_NOT_SUPPORT;
        goto OUT;
    }

    ret = osAdapterOps->claimInterface(devHandle, (unsigned int)interfaceNumber);
    if (ret == HDF_SUCCESS) {
        devHandle->claimedInterfaces |= 1U << (uint32_t)interfaceNumber;
    }

OUT:
    OsalMutexUnlock(&devHandle->lock);

    return ret;
}

struct UsbHostRequest *AllocRequest(const struct UsbDeviceHandle *devHandle,  int32_t isoPackets, size_t length)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (osAdapterOps->allocRequest == NULL) {
        return NULL;
    }

    return osAdapterOps->allocRequest(devHandle,  isoPackets, length);
}

int32_t FreeRequest(const struct UsbHostRequest *request)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (osAdapterOps->freeRequest == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return osAdapterOps->freeRequest((struct UsbHostRequest *)request);
}

int32_t RawFillBulkRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData)
{
    int32_t ret;

    if ((request == NULL) || (request->buffer == NULL) || (devHandle == NULL)
        || (fillRequestData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (UsbEndpointDirOut(fillRequestData->endPoint)) {
        if (fillRequestData->buffer == NULL) {
            HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
            return HDF_ERR_INVALID_PARAM;
        }
        ret = memcpy_s(request->buffer, request->bufLen, fillRequestData->buffer, fillRequestData->length);
        if (ret != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail!", __func__, __LINE__);
            return ret;
        }
    }
    request->devHandle    = (struct UsbDeviceHandle *)devHandle;
    request->endPoint     = fillRequestData->endPoint;
    request->requestType  = USB_PIPE_TYPE_BULK;
    request->timeout      = fillRequestData->timeout;
    request->length       = fillRequestData->length;
    request->userData     = fillRequestData->userData;
    request->callback     = fillRequestData->callback;
    request->userCallback = fillRequestData->userCallback;

    return HDF_SUCCESS;
}

int32_t RawFillControlSetup(const unsigned char *setup, const struct UsbControlRequestData *requestData)
{
    struct UsbRawControlSetup *setupData = (struct UsbRawControlSetup *)setup;

    if ((setup == NULL) || (requestData == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    setupData->requestType = requestData->requestType;
    setupData->request     = requestData->requestCmd;
    setupData->value       = CpuToLe16(requestData->value);
    setupData->index       = CpuToLe16(requestData->index);
    setupData->length      = CpuToLe16(requestData->length);

    return HDF_SUCCESS;
}

int32_t RawFillControlRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData)
{
    if (request == NULL || devHandle == NULL || fillRequestData == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    request->devHandle    = (struct UsbDeviceHandle *)devHandle;
    request->endPoint     = fillRequestData->endPoint;
    request->requestType  = USB_PIPE_TYPE_CONTROL;
    request->timeout      = fillRequestData->timeout;
    request->userData     = fillRequestData->userData;
    request->callback     = fillRequestData->callback;
    request->userCallback = fillRequestData->userCallback;
    request->length       = fillRequestData->length;

    return HDF_SUCCESS;
}

int32_t RawFillInterruptRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData)
{
    int32_t ret;

    if ((request == NULL) || (devHandle == NULL) || (fillRequestData == NULL)) {
        HDF_LOGE("%s:%d param is NULL!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (UsbEndpointDirOut(fillRequestData->endPoint)) {
        ret = memcpy_s(request->buffer, request->bufLen, fillRequestData->buffer, fillRequestData->length);
        if (ret != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail!", __func__, __LINE__);
            return ret;
        }
    }
    request->devHandle    = (struct UsbDeviceHandle *)devHandle;
    request->endPoint     = fillRequestData->endPoint;
    request->requestType  = USB_PIPE_TYPE_INTERRUPT;
    request->timeout      = fillRequestData->timeout;
    request->length       = fillRequestData->length;
    request->userData     = fillRequestData->userData;
    request->callback     = fillRequestData->callback;
    request->userCallback = fillRequestData->userCallback;

    return HDF_SUCCESS;
}

int32_t RawFillIsoRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbFillRequestData *fillRequestData)
{
    int32_t ret;

    if ((request == NULL) || (devHandle == NULL) || (fillRequestData == NULL)) {
        HDF_LOGE("%s:%d param is NULL!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (UsbEndpointDirOut(fillRequestData->endPoint)) {
        ret = memcpy_s(request->buffer, request->bufLen, fillRequestData->buffer, fillRequestData->length);
        if (ret != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail!", __func__, __LINE__);
            return ret;
        }
    }
    request->devHandle     = (struct UsbDeviceHandle *)devHandle;
    request->endPoint      = fillRequestData->endPoint;
    request->requestType   = USB_PIPE_TYPE_ISOCHRONOUS;
    request->timeout       = fillRequestData->timeout;
    request->length        = fillRequestData->length;
    request->numIsoPackets = fillRequestData->numIsoPackets;
    request->userData      = fillRequestData->userData;
    request->callback      = fillRequestData->callback;
    request->userCallback  = fillRequestData->userCallback;

    return HDF_SUCCESS;
}

int32_t RawSendControlRequest(struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbControlRequestData *requestData)
{
    struct UsbFillRequestData fillRequestData = {0};
    unsigned char *setup = NULL;
    int32_t completed = 0;
    int32_t ret;

    if ((request == NULL) || (request->buffer == NULL) || (devHandle == NULL) || (requestData == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    setup = request->buffer;
    RawFillControlSetup(setup, requestData);
    if ((requestData->requestType & USB_DDK_ENDPOINT_DIR_MASK) == USB_PIPE_DIRECTION_OUT) {
        fillRequestData.endPoint = 0;
        fillRequestData.length = requestData->length;
        if (requestData->length > 0) {
            ret = memcpy_s(request->buffer + USB_RAW_CONTROL_SETUP_SIZE, fillRequestData.length,
                requestData->data, requestData->length);
            if (ret != EOK) {
                HDF_LOGE("%s:%d memcpy_s fail, requestData.length=%d",
                         __func__, __LINE__, requestData->length);
                return ret;
            }
        }
        fillRequestData.length = USB_RAW_CONTROL_SETUP_SIZE + requestData->length;
    } else {
        fillRequestData.endPoint = (0x1  << USB_DIR_OFFSET);
    }
    fillRequestData.userCallback = NULL;
    fillRequestData.callback  = SyncRequestCallback;
    fillRequestData.userData  = &completed;
    fillRequestData.timeout   = requestData->timeout;
    RawFillControlRequest(request, devHandle, &fillRequestData);

    ret = OsalSemInit(&request->sem, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalSemInit faile, ret=%d ", __func__, __LINE__, ret);
        return ret;
    }

    ret = RawSubmitRequest(request);
    if (ret < 0) {
        OsalSemDestroy(&request->sem);
        return ret;
    }

    return ControlRequestCompletion(request, (struct UsbControlRequestData *)requestData);
}

int32_t RawSendBulkRequest(const struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbRequestData *requestData)
{
    if ((request == NULL) || (devHandle == NULL) || (requestData == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }

    return HandleSyncRequest((struct UsbHostRequest *)request, devHandle, requestData, USB_PIPE_TYPE_BULK);
}

int32_t RawSendInterruptRequest(const struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbRequestData *requestData)
{
    if ((request == NULL) || (devHandle == NULL) || (requestData == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return HandleSyncRequest((struct UsbHostRequest *)request, devHandle, requestData, USB_PIPE_TYPE_INTERRUPT);
}

struct UsbHostRequest *RawAllocRequest(const struct UsbDeviceHandle *devHandle, int32_t isoPackets, int32_t length)
{
    struct UsbHostRequest *request = NULL;
    request = (struct UsbHostRequest *)AllocRequest(devHandle, isoPackets, length);
    if (request == NULL) {
        HDF_LOGE("%s RawMemAlloc fail", __func__);
        return NULL;
    }
    return request;
}

int32_t RawFreeRequest(const struct UsbHostRequest *request)
{
    if (request == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return FreeRequest(request);
}

int32_t RawGetConfigDescriptor(const struct UsbDevice *dev, uint8_t configIndex,
    struct UsbRawConfigDescriptor **config)
{
    int32_t ret;
    union UsbiConfigDescBuf tmpConfig;
    uint16_t configLen;
    uint8_t *buf = NULL;

    if (dev == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (configIndex > dev->deviceDescriptor.bNumConfigurations) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    ret = GetConfigDescriptor(dev, configIndex, tmpConfig.buf, sizeof(tmpConfig.buf));
    if (ret < HDF_SUCCESS) {
        HDF_LOGE("%s:%d ret=%d", __func__, __LINE__, ret);
        return ret;
    }
    configLen = Le16ToCpu(tmpConfig.desc.wTotalLength);
    buf = RawUsbMemAlloc(configLen);
    if (buf == NULL) {
        HDF_LOGE("%s:%d RawUsbMemAlloc failed", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GetConfigDescriptor(dev, configIndex, buf, configLen);
    if (ret >= HDF_SUCCESS) {
        ret = DescToConfig(buf, ret, config);
    }

    RawUsbMemFree(buf);
    buf = NULL;

    return ret;
}

void RawClearConfiguration(struct UsbRawConfigDescriptor *config)
{
    uint8_t i;

    if (config == NULL) {
        HDF_LOGE("%s:%d config is NULL", __func__, __LINE__);
        return;
    }

    for (i = 0; i < config->configDescriptor.bNumInterfaces; i++) {
        ClearInterface((const struct UsbRawInterface *)(config->interface[i]));
        config->interface[i] = NULL;
    }

    if (config->extra != NULL) {
        RawUsbMemFree((void *)config->extra);
        config->extra = NULL;
    }
}

int32_t RawGetConfiguration(const struct UsbDeviceHandle *devHandle, int32_t *config)
{
    int32_t ret;
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();
    uint8_t tmp = 0;

    if ((devHandle == NULL) || (config == NULL)) {
        HDF_LOGE("%s:%d param is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!osAdapterOps->getConfiguration) {
        HDF_LOGE("%s:%d adapter don't support getConfiguration",
                 __func__, __LINE__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = osAdapterOps->getConfiguration(devHandle, &tmp);
    *config = tmp;
    return ret;
}

int32_t RawSetConfiguration(const struct UsbDeviceHandle *devHandle, int32_t configuration)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (configuration < -1 || configuration > (int)0xFF) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!osAdapterOps->setConfiguration) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return osAdapterOps->setConfiguration((struct UsbDeviceHandle *)devHandle, configuration);
}

int32_t RawGetDescriptor(const struct UsbHostRequest *request, const struct UsbDeviceHandle *devHandle,
    const struct UsbRawDescriptorParam *param, const unsigned char *data)
{
    int32_t ret;
    struct UsbControlRequestData requestData;

    if ((request == NULL) || (devHandle == NULL) || (param == NULL) || (data == NULL)) {
        HDF_LOGE("%s:%d param is NULL!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    requestData.requestType = USB_PIPE_DIRECTION_IN;
    requestData.requestCmd  = USB_REQUEST_GET_DESCRIPTOR;
    requestData.value       = (uint16_t)((param->descType << BYTE_LENGTH) | param->descIndex);
    requestData.index       = 0;
    requestData.data        = (unsigned char *)data;
    requestData.length      = (uint16_t)param->length;
    requestData.timeout     = USB_RAW_REQUEST_DEFAULT_TIMEOUT;
    ret = RawSendControlRequest((struct UsbHostRequest *)request, devHandle, &requestData);

    return ret;
}

struct UsbDevice *RawGetDevice(const struct UsbDeviceHandle *devHandle)
{
    if (devHandle == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return NULL;
    }

    return devHandle->dev;
}

int32_t RawGetDeviceDescriptor(const struct UsbDevice *dev, struct UsbDeviceDescriptor *desc)
{
    if ((dev == NULL) || (desc == NULL) || (sizeof(dev->deviceDescriptor) != USB_DDK_DT_DEVICE_SIZE)) {
        HDF_LOGE("%s: struct UsbDeviceDescriptor is not expected size", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    *desc = dev->deviceDescriptor;
    return HDF_SUCCESS;
}

int32_t RawReleaseInterface(struct UsbDeviceHandle *devHandle, int32_t interfaceNumber)
{
    int32_t ret;
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if ((devHandle == NULL) || (interfaceNumber < 0) || (interfaceNumber >= USB_MAXINTERFACES)) {
        HDF_LOGE("%s:%d param is NULL or interfaceNumber = %d is out of range", __func__, __LINE__, interfaceNumber);
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock(&devHandle->lock);
    if (!(devHandle->claimedInterfaces & (1U << (uint32_t)interfaceNumber))) {
        ret = HDF_ERR_BAD_FD;
        goto OUT;
    }

    if (!osAdapterOps->releaseInterface) {
        ret = HDF_ERR_NOT_SUPPORT;
        goto OUT;
    }

    ret = osAdapterOps->releaseInterface(devHandle, (unsigned int)interfaceNumber);
    if (ret == HDF_SUCCESS) {
        devHandle->claimedInterfaces &= ~(1U << (uint32_t)interfaceNumber);
    }

OUT:
    OsalMutexUnlock(&devHandle->lock);

    return ret;
}

int32_t RawResetDevice(const struct UsbDeviceHandle *devHandle)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (!osAdapterOps->resetDevice) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return osAdapterOps->resetDevice(devHandle);
}

int32_t RawSubmitRequest(const struct UsbHostRequest *request)
{
    int32_t ret;
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (request == NULL) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!osAdapterOps->submitRequest) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = osAdapterOps->submitRequest((struct UsbHostRequest *)request);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d ret = %d", __func__, __LINE__, ret);
    }

    return ret;
}

int32_t RawCancelRequest(const struct UsbHostRequest *request)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (!osAdapterOps->cancelRequest) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return osAdapterOps->cancelRequest((struct UsbHostRequest *)request);
}

int32_t RawHandleRequest(const struct UsbDeviceHandle *devHandle)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();
    int32_t ret;

    if (!osAdapterOps->urbCompleteHandle) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = osAdapterOps->urbCompleteHandle(devHandle);
    if (ret < 0) {
        HDF_LOGE("%s:%d handleEvents error, return %d", __func__, __LINE__, ret);
    }

    return ret;
}

int32_t RawClearHalt(const struct UsbDeviceHandle *devHandle, uint8_t pipeAddress)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();
    unsigned int endPoint = pipeAddress;

    if (osAdapterOps->clearHalt == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return osAdapterOps->clearHalt(devHandle, endPoint);
}

int32_t RawHandleRequestCompletion(struct UsbHostRequest *request, UsbRequestStatus status)
{
    if (request == NULL) {
        HDF_LOGE("%s:%d request is NULL!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    request->status = status;
    if (request->callback) {
        request->callback((void *)request);
    }

    return HDF_SUCCESS;
}

int32_t RawSetInterfaceAltsetting(
    const struct UsbDeviceHandle *devHandle, uint8_t interfaceNumber, uint8_t settingIndex)
{
    struct UsbOsAdapterOps *osAdapterOps = UsbAdapterGetOps();

    if (osAdapterOps->setInterfaceAltsetting == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    return osAdapterOps->setInterfaceAltsetting(devHandle, interfaceNumber, settingIndex);
}

UsbRawTidType RawGetTid(void)
{
    return UsbAdapterGetTid();
}

int32_t RawRegisterSignal(void)
{
    return UsbAdapterRegisterSignal();
}

int32_t RawKillSignal(struct UsbDeviceHandle *devHandle, UsbRawTidType tid)
{
    return UsbAdapterKillSignal(devHandle, tid);
}

int32_t RawInitPnpService(enum UsbPnpNotifyServiceCmd cmdType, struct UsbPnpAddRemoveInfo infoData)
{
    int32_t ret;
    struct HdfSBuf *pnpData = NULL;
    struct HdfSBuf *pnpReply = NULL;
    int32_t replyData = 0;
    bool flag = false;

    if ((cmdType != USB_PNP_NOTIFY_ADD_INTERFACE) && (cmdType != USB_PNP_NOTIFY_REMOVE_INTERFACE)) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct HdfIoService *serv = HdfIoServiceBind(USB_HOST_PNP_SERVICE_NAME);
    if ((serv == NULL) || (serv->dispatcher == NULL) || (serv->dispatcher->Dispatch == NULL)) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d fail to get service %s", __func__, __LINE__, USB_HOST_PNP_SERVICE_NAME);
        return ret;
    }

    pnpData = HdfSbufObtainDefaultSize();
    pnpReply = HdfSbufObtainDefaultSize();
    if (pnpData == NULL || pnpReply == NULL) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d GetService err", __func__, __LINE__);
        goto ERR_SBUF;
    }

    if (!HdfSbufWriteBuffer(pnpData, (const void *)(&infoData), sizeof(struct UsbPnpAddRemoveInfo))) {
        HDF_LOGE("%s: sbuf write infoData failed", __func__);
        ret = HDF_FAILURE;
        goto OUT;
    }

    ret = serv->dispatcher->Dispatch(&serv->object, cmdType, pnpData, pnpReply);
    if (ret) {
        HDF_LOGE("%s: Dispatch USB_PNP_NOTIFY_REMOVE_TEST failed ret = %d", __func__, ret);
        goto OUT;
    }

    flag = HdfSbufReadInt32(pnpReply, &replyData);
    if ((!flag) || (replyData != INT32_MAX)) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d cmdType=%d reply faile.", __func__, __LINE__, cmdType);
        goto OUT;
    } else if (flag && replyData == INT32_MAX) {
        HDF_LOGE("%s:%d cmdType=%d reply success.", __func__, __LINE__, cmdType);
    }

    ret = HDF_SUCCESS;

OUT:
    HdfSbufRecycle(pnpData);
    HdfSbufRecycle(pnpReply);
ERR_SBUF:
    HdfIoServiceRecycle(serv);

    return ret;
}

void RawRequestListInit(struct UsbDevice *deviceObj)
{
    if (deviceObj == NULL) {
        HDF_LOGE("%s:%d deviceObj is NULL!", __func__, __LINE__);
        return;
    }

    OsalMutexInit(&deviceObj->requestLock);
    HdfSListInit(&deviceObj->requestList);
}


int32_t RawUsbMemTestTrigger(bool enable)
{
    g_usbRamTestFlag = enable;
    return HDF_SUCCESS;
}

void *RawUsbMemAlloc(size_t size)
{
    return RawUsbMemCalloc(size);
}

void *RawUsbMemCalloc(size_t size)
{
    void *buf = NULL;
    struct RawUsbRamTestList *testEntry = NULL;
    struct RawUsbRamTestList *pos = NULL;
    uint32_t totalSize = 0;
    int32_t ret = 0;

    if (size == 0) {
        HDF_LOGE("%s:%d size is 0", __func__, __LINE__);
        return NULL;
    }
    
    buf = OsalMemAlloc(size);
    if (buf == NULL) {
        HDF_LOGE("%{public}s: %{public}d, OsalMemAlloc failed", __func__, __LINE__);
        return NULL;
    }
    ret = memset_s(buf, size, 0, size);
    if (ret != EOK) {
        HDF_LOGE("%{public}s: %{public}d, memset_s failed", __func__, __LINE__);
        OsalMemFree(buf);
        return NULL;
    }
    if (g_usbRamTestFlag) {
        if (g_usbRamTestHead == NULL) {
            g_usbRamTestHead = OsalMemAlloc(sizeof(struct RawUsbRamTestList));
            OsalMutexInit(&g_usbRamTestHead->lock);
            DListHeadInit(&g_usbRamTestHead->list);
        }
        testEntry = OsalMemAlloc(sizeof(struct RawUsbRamTestList));
        if (testEntry == NULL) {
            HDF_LOGE("%s:%d testEntry is NULL", __func__, __LINE__);
            return buf;
        }
        testEntry->address = (uintptr_t)buf;
        testEntry->size = size;

        OsalMutexLock(&g_usbRamTestHead->lock);
        DListInsertTail(&testEntry->list, &g_usbRamTestHead->list);
        DLIST_FOR_EACH_ENTRY(pos, &g_usbRamTestHead->list, struct RawUsbRamTestList, list) {
            totalSize += pos->size;
        }
        OsalMutexUnlock(&g_usbRamTestHead->lock);

        HDF_LOGE("%{public}s add size=%{public}d totalSize=%{public}d", __func__, (uint32_t)size, totalSize);
    }
    return buf;
}

void RawUsbMemFree(void *mem)
{
    struct RawUsbRamTestList *pos = NULL;
    struct RawUsbRamTestList *tmp = NULL;
    uint32_t totalSize = 0;
    uint32_t size = 0;

    if (mem == NULL) {
        return;
    }

    if (g_usbRamTestFlag && g_usbRamTestHead != NULL) {
        OsalMutexLock(&g_usbRamTestHead->lock);
        DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &g_usbRamTestHead->list, struct RawUsbRamTestList, list) {
            if (pos->address == (uintptr_t)mem) {
                size = pos->size;
                DListRemove(&pos->list);
                OsalMemFree(pos);
                continue;
            }
            totalSize += pos->size;
        }
        OsalMutexUnlock(&g_usbRamTestHead->lock);
        HDF_LOGE("%{public}s rm size=%{public}d totalSize=%{public}d", __func__, size, totalSize);
    }

    if (mem != NULL) {
        OsalMemFree(mem);
    }
}

