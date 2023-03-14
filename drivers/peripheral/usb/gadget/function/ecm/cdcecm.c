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

#include "cdcecm.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_device_object.h"
#include "osal_mem.h"
#include "osal_sem.h"
#include "osal_time.h"
#include "securec.h"
#include "usbfn_device.h"
#include "usbfn_interface.h"
#include "usbfn_request.h"
#include "default_config.h"

#define HDF_LOG_TAG    cdc_ecm

#define QUEUE_SIZE              8
#define WRITE_BUF_SIZE          8192
#define READ_BUF_SIZE           8192
#define ECM_STATUS_BYTECOUNT        16
#define ECM_BIT 9728000

static int32_t EcmInit(struct HdfDeviceObject *device);
static int32_t EcmRelease(struct HdfDeviceObject *device);

static inline unsigned EcmBitrate(void)
{
    return ECM_BIT;
}

/* Usb Serial Related Functions */
static int32_t UsbEcmStartTx(struct UsbEcm *port)
{
    struct DListHead *pool = &port->writePool;
    int32_t ret;
    if (port->ecm == NULL) {
        return HDF_SUCCESS;
    }

    while (!port->writeBusy && !DListIsEmpty(pool)) {
        struct UsbFnRequest *req = NULL;
        uint32_t len;
        if (port->writeStarted >= QUEUE_SIZE) {
            break;
        }
        req = DLIST_FIRST_ENTRY(pool, struct UsbFnRequest, list);
        OsalMutexLock(&port->lockWriteFifo);
        len = DataFifoRead(&port->writeFifo, req->buf, port->ecm->dataInPipe.maxPacketSize);
        OsalMutexUnlock(&port->lockWriteFifo);
        if (len == 0) {
            break;
        }
        req->length = len;
        DListRemove(&req->list);
        port->writeBusy = true;
        ret = UsbFnSubmitRequestAsync(req);
        port->writeBusy = false;
        if (ret != HDF_SUCCESS) {
            HDF_LOGD("%s: send request erro %d", __func__, ret);
            DListInsertTail(&req->list, pool);
            break;
        }
        port->writeStarted++;
        /* if ecm is disconnect, abort immediately */
        if (port->ecm == NULL) {
            break;
        }
    }
    return HDF_SUCCESS;
}

static uint32_t UsbEcmStartRx(struct UsbEcm *port)
{
    struct DListHead *pool = &port->readPool;
    struct UsbEcmPipe *out = &port->ecm->dataOutPipe;

    while (!DListIsEmpty(pool)) {
        struct UsbFnRequest *req = NULL;
        int32_t ret;

        if (port->readStarted >= QUEUE_SIZE) {
            break;
        }

        req = DLIST_FIRST_ENTRY(pool, struct UsbFnRequest, list);
        DListRemove(&req->list);
        req->length = out->maxPacketSize;
        ret = UsbFnSubmitRequestAsync(req);
        if (ret != HDF_SUCCESS) {
            HDF_LOGD("%s: send request erro %d", __func__, ret);
            DListInsertTail(&req->list, pool);
            break;
        }
        port->readStarted++;
        /* if ecm is disconnect, abort immediately */
        if (port->ecm == NULL) {
            break;
        }
    }
    return port->readStarted;
}

static void UsbEcmRxPush(struct UsbEcm *port)
{
    struct DListHead *queue = &port->readQueue;
    bool disconnect = false;

    while (!DListIsEmpty(queue)) {
        struct UsbFnRequest *req;

        req = DLIST_FIRST_ENTRY(queue, struct UsbFnRequest, list);
        switch (req->status) {
            case USB_REQUEST_NO_DEVICE:
                disconnect = true;
                HDF_LOGV("%s: the device is disconnected", __func__);
                break;
            case USB_REQUEST_COMPLETED:
                break;
            default:
                HDF_LOGV("%s: unexpected status %d", __func__, req->status);
                break;
        }
        if (req->actual && req->status == 0) {
            uint32_t size = req->actual;
            uint8_t *data = req->buf;
            uint32_t count;
            OsalMutexLock(&port->lockReadFifo);
            if (DataFifoIsFull(&port->readFifo)) {
                DataFifoSkip(&port->readFifo, size);
            }
            count = DataFifoWrite(&port->readFifo, data, size);
            if (count != size) {
                HDF_LOGW("%s: write %d less than expected %u",
                    __func__, count, size);
            }
            OsalMutexUnlock(&port->lockReadFifo);
        }
        DListRemove(&req->list);
        DListInsertTail(&req->list, &port->readPool);
        port->readStarted--;
    }

    if (!disconnect && port->ecm) {
        UsbEcmStartRx(port);
    }
}

static void UsbEcmFreeRequests(const struct DListHead *head, int32_t *allocated)
{
    struct UsbFnRequest *req = NULL;
    while (!DListIsEmpty(head)) {
        req = DLIST_FIRST_ENTRY(head, struct UsbFnRequest, list);
        DListRemove(&req->list);
        (void)UsbFnFreeRequest(req);
        if (allocated) {
            (*allocated)--;
        }
    }
}

static void UsbEcmReadComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct UsbEcm *port = (struct UsbEcm *)req->context;
    OsalMutexLock(&port->lock);
    DListInsertTail(&req->list, &port->readQueue);
    UsbEcmRxPush(port);
    OsalMutexUnlock(&port->lock);
}

static void UsbEcmWriteComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct UsbEcm *port = (struct UsbEcm *)req->context;
    OsalMutexLock(&port->lock);
    DListInsertTail(&req->list, &port->writePool);
    port->writeStarted--;

    switch (req->status) {
        case USB_REQUEST_COMPLETED:
            UsbEcmStartTx(port);
            break;
        case USB_REQUEST_NO_DEVICE:
            HDF_LOGV("%s: ecm device was disconnected", __func__);
            break;
        default:
            HDF_LOGV("%s: unexpected status %d", __func__, req->status);
            break;
    }
    OsalMutexUnlock(&port->lock);
}

static int32_t UsbEcmAllocReadRequests(struct UsbEcm *port, int32_t num)
{
    struct UsbEcmDevice *ecm = port->ecm;
    struct DListHead *head = &port->readPool;
    struct UsbFnRequest *req = NULL;
    int32_t  i;

    for (i = 0; i < num; i++) {
        req = UsbFnAllocRequest(ecm->dataIface.handle, ecm->dataOutPipe.id,
            ecm->dataOutPipe.maxPacketSize);
        if (!req) {
            return DListIsEmpty(head) ? HDF_FAILURE : HDF_SUCCESS;
        }

        req->complete = UsbEcmReadComplete;
        req->context = port;
        DListInsertTail(&req->list, head);
        port->readAllocated++;
    }
    return HDF_SUCCESS;
}

static int32_t UsbEcmAllocWriteRequests(struct UsbEcm *port, int32_t num)
{
    struct UsbEcmDevice *ecm = port->ecm;
    struct DListHead *head = &port->writePool;
    struct UsbFnRequest  *req = NULL;
    int32_t i;

    for (i = 0; i < num; i++) {
        req = UsbFnAllocRequest(ecm->dataIface.handle, ecm->dataInPipe.id,
            ecm->dataInPipe.maxPacketSize);
        if (!req) {
            return DListIsEmpty(head) ? HDF_FAILURE : HDF_SUCCESS;
        }

        req->complete = UsbEcmWriteComplete;
        req->context = port;
        DListInsertTail(&req->list, head);
        port->writeAllocated++;
    }
    return HDF_SUCCESS;
}

static int32_t UsbEcmStartIo(struct UsbEcm *port)
{
    struct DListHead *head = &port->readPool;
    int32_t          ret = HDF_SUCCESS;
    uint32_t         started;

    /* allocate requests for read/write */
    if (port->readAllocated == 0) {
        ret = UsbEcmAllocReadRequests(port, QUEUE_SIZE);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    if (port->writeAllocated == 0) {
        ret = UsbEcmAllocWriteRequests(port, QUEUE_SIZE);
        if (ret != HDF_SUCCESS) {
            UsbEcmFreeRequests(head, &port->readAllocated);
            return ret;
        }
    }

    started = UsbEcmStartRx(port);
    if (started) {
        UsbEcmStartTx(port);
    } else {
        UsbEcmFreeRequests(head, &port->readAllocated);
        UsbEcmFreeRequests(&port->writePool, &port->writeAllocated);
        ret = HDF_ERR_IO;
    }

    return ret;
}

static int32_t UsbEcmAllocFifo(struct DataFifo *fifo, uint32_t size)
{
    if (!DataFifoIsInitialized(fifo)) {
        void *data = OsalMemAlloc(size);
        if (data == NULL) {
            HDF_LOGE("%s: allocate fifo data buffer failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
        DataFifoInit(fifo, size, data);
    }
    return HDF_SUCCESS;
}

static void UsbEcmFreeFifo(struct DataFifo *fifo)
{
    void *buf = fifo->data;
    OsalMemFree(buf);
    DataFifoInit(fifo, 0, NULL);
}

int32_t UsbEcmOpen(struct UsbEcm *port)
{
    int32_t ret;

    if (port == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock(&port->lock);
    ret = UsbEcmAllocFifo(&port->writeFifo, WRITE_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbEcmAllocFifo failed", __func__);
        goto OUT;
    }
    ret = UsbEcmAllocFifo(&port->readFifo, READ_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbEcmAllocFifo failed", __func__);
        goto OUT;
    }
    DataFifoReset(&port->writeFifo);
    DataFifoReset(&port->readFifo);

    if (port->refCount++) {
        HDF_LOGE("%s: refCount failed", __func__);
        goto OUT;
    }

    /* the ecm is enabled, start the io stream */
    if (port->ecm) {
        HDF_LOGD("%s: start usb io", __func__);
        ret = UsbEcmStartIo(port);
        if (ret != HDF_SUCCESS) {
            goto OUT;
        }
    }

OUT:
    OsalMutexUnlock(&port->lock);
    return HDF_SUCCESS;
}

int32_t UsbEcmClose(struct UsbEcm *port)
{
    struct UsbEcmDevice *ecm = NULL;

    if (port == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock(&port->lock);
    if (port->refCount != 1) {
        --port->refCount;
        goto OUT;
    }

    HDF_LOGD("%s: close usb serial", __func__);
    ecm = port->ecm;

    DataFifoReset(&port->writeFifo);
    DataFifoReset(&port->readFifo);
    port->refCount = 0;

OUT:
    OsalMutexUnlock(&port->lock);
    return HDF_SUCCESS;
}

static int32_t UsbEcmRead(struct UsbEcm *port, struct HdfSBuf *reply)
{
    uint32_t len;
    int32_t ret = HDF_SUCCESS;
    uint8_t *buf = NULL;
    OsalMutexLock(&port->lock);
    OsalMutexLock(&port->lockReadFifo);
    if (DataFifoIsEmpty(&port->readFifo)) {
        OsalMutexUnlock(&port->lockReadFifo);
        OsalMutexUnlock(&port->lock);
        return 0;
    }

    buf = (uint8_t *)OsalMemCalloc(DataFifoLen(&port->readFifo) + sizeof(uint32_t));
    if (buf == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        OsalMutexUnlock(&port->lockReadFifo);
        OsalMutexUnlock(&port->lock);
        return HDF_ERR_MALLOC_FAIL;
    }

    len = DataFifoRead(&port->readFifo, buf, DataFifoLen(&port->readFifo));
    if (len == 0) {
        HDF_LOGE("%s: no data", __func__);
        ret = HDF_ERR_IO;
        OsalMutexUnlock(&port->lockReadFifo);
        goto OUT;
    }
    OsalMutexUnlock(&port->lockReadFifo);

    bool bufok = HdfSbufWriteBuffer(reply, (const void *)buf, len);
    if (!bufok) {
        HDF_LOGE("UsbEcmRead HdfSbufWriteBuffer error");
        ret = HDF_ERR_IO;
        goto OUT;
    }

OUT:
    if (port->ecm) {
        UsbEcmStartRx(port);
    }
    OsalMemFree(buf);
    OsalMutexUnlock(&port->lock);
    return ret;
}

static int32_t UsbEcmWrite(struct UsbEcm *port, struct HdfSBuf *data)
{
    uint32_t size = 0;
    uint8_t *buf = NULL;

    if (!HdfSbufReadBuffer(data, (const void **)&buf, &size)) {
        HDF_LOGE("UsbEcmWrite HdfSbufReadBuffer err");
        return HDF_ERR_IO;
    }

    OsalMutexLock(&port->lock);
    if (size > 0 && buf != NULL) {
        OsalMutexLock(&port->lockWriteFifo);
        size = DataFifoWrite(&port->writeFifo, buf, size);
        OsalMutexUnlock(&port->lockWriteFifo);
    }
    if (port->ecm) {
        UsbEcmStartTx(port);
    }
    OsalMutexUnlock(&port->lock);
    return HDF_SUCCESS;
}

static void EcmDoNotify(struct UsbEcmDevice *ecm)
{
    struct UsbFnRequest *req = ecm->notifyReq;
    struct UsbCdcNotification *event = NULL;
    uint32_t *data = NULL;
    int32_t status;

    if (!req) {
        return;
    }
    ecm->isOpen = true;
    event = (struct UsbCdcNotification *) req->buf;
    if (event == NULL) {
        return;
    }
    switch (ecm->notifyState) {
        case ECM_NOTIFY_NONE:
            return;

        case ECM_NOTIFY_CONNECT:
            event->bNotificationType = USB_DDK_CDC_NOTIFY_NETWORK_CONNECTION;
            if (ecm->isOpen)
                event->wValue = CpuToLe16(1);
            else
                event->wValue = CpuToLe16(0);
            event->wLength = 0;
            req->length = sizeof(*event);

            HDF_LOGD("notify connect %s\n", ecm->isOpen ? "true" : "false");
            ecm->notifyState = ECM_NOTIFY_SPEED;
            break;

        case ECM_NOTIFY_SPEED:
            event->bNotificationType = USB_DDK_CDC_NOTIFY_SPEED_CHANGE;
            event->wValue = CpuToLe16(0);
            event->wLength = CpuToLe16(0x08);
            req->length = ECM_STATUS_BYTECOUNT;

            /* SPEED_CHANGE data is up/down speeds in bits/sec */
            data = (uint32_t *)((char *)req->buf + sizeof(*event));
            data[0] = CpuToLe32(EcmBitrate());
            data[1] = data[0];

            HDF_LOGD("notify speed %d\n", EcmBitrate());
            ecm->notifyState = ECM_NOTIFY_NONE;
            break;

        default:
            break;
    }
    event->bmRequestType = 0xA1;
    event->wIndex = CpuToLe16(ecm->ctrlId);

    ecm->notifyReq = NULL;
    status = UsbFnSubmitRequestAsync(req);
    if (status < 0) {
        ecm->notifyReq = req;
        HDF_LOGD("notify --> %d\n", status);
    }
}

static void EcmNotify(struct UsbEcmDevice *ecm)
{
    ecm->notifyState = ECM_NOTIFY_CONNECT;
    EcmDoNotify(ecm);
}

static void EcmNotifyComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct UsbEcmDevice *ecm = req->context;
    struct UsbCdcNotification *event = req->buf;
    ecm->notifyReq = req;
    switch (req->status) {
        case 0:
            EcmDoNotify(ecm);
            break;
        default:
            HDF_LOGD("event %d --> %d\n",
                event->bNotificationType, req->status);
            break;
    }
}

static int32_t EcmSetup(const struct UsbEcmDevice *ecm, const struct UsbFnCtrlRequest *ctrl)
{
    struct UsbFnRequest *req = ecm->ep0Req;
    int32_t ret = -1;
    uint16_t index = Le16ToCpu(ctrl->index);
    uint16_t value = Le16ToCpu(ctrl->value);
    uint16_t length = Le16ToCpu(ctrl->length);

    switch ((ctrl->reqType << 0x08) | ctrl->request) {
        case ((USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE) << 0x08)
                | USB_DDK_CDC_SET_ETHERNET_PACKET_FILTER:
            if (length != 0 || index != ecm->ctrlId)
                goto INVALID;
            HDF_LOGD("packet filter %02x\n", value);
            ret = 0;
            break;

        default:
            INVALID:
            HDF_LOGD("invalid control req%02x.%02x v%04x i%04x l%hu\n",
                ctrl->reqType, ctrl->request, value, index, length);
    }

    if (ret >= 0) {
        HDF_LOGD("ecm req%02x.%02x v%04x i%04x l%d\n",
            ctrl->reqType, ctrl->request,
            value, index, length);
        req->length = (uint32_t)ret;
        ret = UsbFnSubmitRequestSync(req, 0);
        if (ret < 0) {
            HDF_LOGD("ecm req %02x.%02x response err %d\n",
                ctrl->reqType, ctrl->request, ret);
        }
    }

    return value;
}

static int32_t EcmDeviceDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct UsbEcmDevice *ecm = NULL;
    struct UsbEcm *port = NULL;
    int32_t ret;
    if (client == NULL || client->device == NULL || client->device->service == NULL) {
        HDF_LOGE("%s: client is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: data or reply is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (HdfDeviceObjectCheckInterfaceDesc(client->device, data) == false) {
        HDF_LOGE("%{public}s:%{public}d check interface desc fail", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (cmd) {
        case USB_ECM_INIT:
            return EcmInit(client->device);
        case USB_ECM_RELEASE:
            return EcmRelease(client->device);
        default:
            break;
    }
    ecm = (struct UsbEcmDevice *)client->device->service;
    port = ecm->port;
    if (port == NULL) {
        return HDF_ERR_IO;
    }
    OsalMutexLock(&port->lockRW);
    switch (cmd) {
        case USB_ECM_OPEN:
            ret = UsbEcmOpen(port);
            break;
        case USB_ECM_CLOSE:
            ret = UsbEcmClose(port);
            break;
        case USB_ECM_READ:
            ret = UsbEcmRead(port, reply);
            break;
        case USB_ECM_WRITE:
            ret = UsbEcmWrite(port, data);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }
    OsalMutexUnlock(&port->lockRW);
    return ret;
}

static int32_t EcmEnable(struct UsbEcmDevice *ecm)
{
    int32_t ret;
    struct UsbEcm *port = ecm->port;

    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return HDF_FAILURE;
    }

    OsalMutexLock(&port->lock);
    port->ecm = ecm;
    if (port->refCount) {
        ret = UsbEcmStartIo(port);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: UsbEcmStartIo failed", __func__);
        }
    }
    EcmNotify(ecm);
    OsalMutexUnlock(&port->lock);

    return HDF_SUCCESS;
}

static void EcmDisable(const struct UsbEcmDevice *ecm)
{
    struct UsbEcm *port = ecm->port;

    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return;
    }

    OsalMutexLock(&port->lock);
    if (port->refCount == 0) {
        UsbEcmFreeFifo(&port->writeFifo);
        UsbEcmFreeFifo(&port->readFifo);
    }
    OsalMutexUnlock(&port->lock);
}

static void UsbEcmEventCallback(struct UsbFnEvent *event)
{
    struct UsbEcmDevice *ecm = NULL;

    if (event == NULL || event->context == NULL) {
        HDF_LOGE("%s: event is null", __func__);
        return;
    }

    ecm = (struct UsbEcmDevice *)event->context;
    switch (event->type) {
        case USBFN_STATE_BIND:
            HDF_LOGI("%s: receive bind event", __func__);
            break;
        case USBFN_STATE_UNBIND:
            HDF_LOGI("%s: receive unbind event", __func__);
            break;
        case USBFN_STATE_ENABLE:
            HDF_LOGI("%s: receive enable event", __func__);
            EcmEnable(ecm);
            break;
        case USBFN_STATE_DISABLE:
            HDF_LOGI("%s: receive disable event", __func__);
            EcmDisable(ecm);
            break;
        case USBFN_STATE_SETUP:
            HDF_LOGI("%s: receive setup event", __func__);
            if (event->setup != NULL) {
                EcmSetup(ecm, event->setup);
            }
            break;
        case USBFN_STATE_SUSPEND:
            HDF_LOGI("%s: receive suspend event", __func__);
            break;
        case USBFN_STATE_RESUME:
            HDF_LOGI("%s: receive resume event", __func__);
            break;
        default:
            break;
    }
}

static int32_t EcmAllocNotifyRequest(struct UsbEcmDevice *ecm)
{
    /* allocate notification request */
    ecm->notifyReq = UsbFnAllocRequest(ecm->ctrlIface.handle, ecm->notifyPipe.id,
        ECM_STATUS_BYTECOUNT);
    if (ecm->notifyReq == NULL) {
        HDF_LOGE("%s: allocate notify request failed", __func__);
        return HDF_FAILURE;
    }
    ecm->notifyReq->complete = EcmNotifyComplete;
    ecm->notifyReq->context = ecm;

    return HDF_SUCCESS;
}

static int32_t EcmAllocEp0Request(struct UsbEcmDevice *ecm)
{
    /* allocate notification request */
    ecm->ep0Req = UsbFnAllocCtrlRequest(ecm->ctrlIface.handle, ECM_STATUS_BYTECOUNT);
    if (ecm->ep0Req == NULL) {
        HDF_LOGE("%s: allocate ep0Req request failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t EcmParseEachPipe(struct UsbEcmDevice *ecm, struct UsbEcmInterface *iface)
{
    struct UsbFnInterface *fnIface = iface->fn;
    uint32_t i;
    int32_t ret;
    for (i = 0; i < fnIface->info.numPipes; i++) {
        struct UsbFnPipeInfo pipeInfo;
        (void)memset_s(&pipeInfo, sizeof(pipeInfo), 0, sizeof(pipeInfo));
        ret = UsbFnGetInterfacePipeInfo(fnIface, i, &pipeInfo);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: get pipe info error", __func__);
            return HDF_FAILURE;
        }

        switch (pipeInfo.type) {
            case USB_PIPE_TYPE_INTERRUPT:
                ecm->notifyPipe.id = pipeInfo.id;
                ecm->notifyPipe.maxPacketSize = pipeInfo.maxPacketSize;
                ecm->ctrlIface = *iface;
                break;
            case USB_PIPE_TYPE_BULK:
                if (pipeInfo.dir == USB_PIPE_DIRECTION_IN) {
                    ecm->dataInPipe.id = pipeInfo.id;
                    ecm->dataInPipe.maxPacketSize = pipeInfo.maxPacketSize;
                    ecm->dataIface = *iface;
                } else {
                    ecm->dataOutPipe.id = pipeInfo.id;
                    ecm->dataOutPipe.maxPacketSize = pipeInfo.maxPacketSize;
                }
                break;
            default:
                HDF_LOGE("%s: pipe type %d don't support",
                    __func__, pipeInfo.type);
                break;
        }
    }

    return HDF_SUCCESS;
}

static int32_t EcmParseEcmIface(struct UsbEcmDevice *ecm, struct UsbFnInterface *fnIface)
{
    int32_t ret;
    struct UsbEcmInterface iface;
    UsbFnInterfaceHandle handle = UsbFnOpenInterface(fnIface);
    if (handle == NULL) {
        HDF_LOGE("%s: open interface failed", __func__);
        return HDF_FAILURE;
    }
    iface.fn = fnIface;
    iface.handle = handle;

    ret = EcmParseEachPipe(ecm, &iface);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t EcmParseEachIface(struct UsbEcmDevice *ecm, struct UsbFnDevice *fnDev)
{
    struct UsbFnInterface *fnIface = NULL;
    uint32_t i;

    for (i = 0; i < fnDev->numInterfaces; i++) {
        fnIface = (struct UsbFnInterface *)UsbFnGetInterface(fnDev, i);
        if (fnIface == NULL) {
            HDF_LOGE("%s: get interface failed", __func__);
            return HDF_FAILURE;
        }

        if (fnIface->info.subclass == USB_DDK_CDC_SUBCLASS_ETHERNET) {
            (void)EcmParseEcmIface(ecm, fnIface);
            fnIface = (struct UsbFnInterface *)UsbFnGetInterface(fnDev, i + 1);
            if (fnIface == NULL) {
                HDF_LOGE("%s: get interface failed", __func__);
                return HDF_FAILURE;
            }
            (void)EcmParseEcmIface(ecm, fnIface);
            return HDF_SUCCESS;
        }
    }

    return HDF_FAILURE;
}

static int32_t EcmCreateFuncDevice(struct UsbEcmDevice *ecm,
    struct DeviceResourceIface *iface)
{
    struct UsbFnDevice *fnDev = NULL;
    int32_t ret;

    if (iface->GetString(ecm->device->property, "udc_name",
        (const char **)&ecm->udcName, UDC_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: read udc_name failed, use default", __func__);
    }

    fnDev = (struct UsbFnDevice *)UsbFnGetDevice(ecm->udcName);
    if (fnDev == NULL) {
        HDF_LOGE("%s: create usb function device failed", __func__);
        return HDF_FAILURE;
    }

    ret = EcmParseEachIface(ecm, fnDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get pipes failed", __func__);
        goto ERR;
    }

    ret = EcmAllocEp0Request(ecm);
    if (ret != HDF_SUCCESS) {
        goto ERR;
    }

    ret = EcmAllocNotifyRequest(ecm);
    if (ret != HDF_SUCCESS) {
        goto ERR;
    }
    ret = UsbFnStartRecvInterfaceEvent(ecm->ctrlIface.fn,
        0xff, UsbEcmEventCallback, ecm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: register event callback failed", __func__);
        goto ERR;
    }
    ecm->fnDev = fnDev;
    return HDF_SUCCESS;

ERR:
    return ret;
}

static int32_t EcmReleaseFuncDevice(const struct UsbEcmDevice *ecm)
{
    int32_t ret = HDF_SUCCESS;
    if (ecm->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is null", __func__);
        return HDF_FAILURE;
    }
    (void)UsbFnCloseInterface(ecm->ctrlIface.handle);
    (void)UsbFnCloseInterface(ecm->dataIface.handle);
    (void)UsbFnStopRecvInterfaceEvent(ecm->ctrlIface.fn);
    return ret;
}

static int32_t UsbEcmAlloc(struct UsbEcmDevice *ecm)
{
    struct UsbEcm *port = NULL;

    port = (struct UsbEcm *)OsalMemCalloc(sizeof(*port));
    if (port == NULL) {
        HDF_LOGE("%s: Alloc usb serial port failed", __func__);
        return HDF_FAILURE;
    }

    if (OsalMutexInit(&port->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        goto ERR;
    }

    if (OsalMutexInit(&port->lockRW) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        goto ERR;
    }

    if (OsalMutexInit(&port->lockReadFifo) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        goto ERR;
    }

    if (OsalMutexInit(&port->lockWriteFifo) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        goto ERR;
    }
    DListHeadInit(&port->readPool);
    DListHeadInit(&port->readQueue);
    DListHeadInit(&port->writePool);

    ecm->port = port;
    return HDF_SUCCESS;
ERR:
    OsalMemFree(port);
    return HDF_FAILURE;
}

/* HdfDriverEntry implementations */
static int32_t EcmDriverBind(struct HdfDeviceObject *device)
{
    struct UsbEcmDevice *ecm = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ecm = (struct UsbEcmDevice *)OsalMemCalloc(sizeof(*ecm));
    if (ecm == NULL) {
        HDF_LOGE("%s: Alloc usb ecm device failed", __func__);
        return HDF_FAILURE;
    }
    ecm->ctrlId = 0;
    ecm->dataId = 1;
    if (OsalMutexInit(&ecm->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        OsalMemFree(ecm);
        return HDF_FAILURE;
    }

    if (HdfDeviceObjectSetInterfaceDesc(device, "hdf.usb.usbfn") != HDF_SUCCESS) {
        HDF_LOGE(" Set Desc fail!");
        OsalMemFree(ecm);
        return HDF_FAILURE;
    }

    ecm->device  = device;
    device->service = &(ecm->service);
    if (ecm->device->service) {
        ecm->device->service->Dispatch = EcmDeviceDispatch;
    }
    return HDF_SUCCESS;
}

static int32_t EcmInit(struct HdfDeviceObject *device)
{
    struct UsbEcmDevice *ecm = NULL;
    struct DeviceResourceIface *iface = NULL;
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ecm = (struct UsbEcmDevice *)device->service;
    if (ecm == NULL || ecm->initFlag) {
        HDF_LOGE("%s: ecm is null", __func__);
        return HDF_FAILURE;
    }

    iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }

    ret = UsbEcmAlloc(ecm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbEcmAlloc failed", __func__);
        return HDF_FAILURE;
    }

    ret = EcmCreateFuncDevice(ecm, iface);
    if (ret) {
        HDF_LOGE("%s: EcmCreateFuncDevice failed", __func__);
        return HDF_FAILURE;
    }
    ecm->initFlag = true;
    return ret;
}

static int32_t EcmRelease(struct HdfDeviceObject *device)
{
    struct UsbEcmDevice *ecm = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return HDF_FAILURE;
    }

    ecm = (struct UsbEcmDevice *)device->service;
    if (ecm == NULL) {
        HDF_LOGE("%s: ecm is null", __func__);
        return HDF_FAILURE;
    }
    if (ecm->initFlag == false) {
        HDF_LOGE("%s: ecm not init!", __func__);
        return HDF_FAILURE;
    }
    (void)EcmReleaseFuncDevice(ecm);
    if (ecm->port) {
        OsalMemFree(ecm->port);
        ecm->port = NULL;
    }
    ecm->initFlag = false;
    return HDF_SUCCESS;
}

static int32_t EcmDriverInit(struct HdfDeviceObject *device)
{
    (void)device;
    HDF_LOGE("%s: usbfn do nothing...", __func__);
    return 0;
}

static void EcmDriverRelease(struct HdfDeviceObject *device)
{
    struct UsbEcmDevice *ecm = NULL;
    if (device == NULL) {
        HDF_LOGE("%{public}s: device is NULL", __func__);
        return;
    }

    ecm = (struct UsbEcmDevice *)device->service;
    if (ecm == NULL) {
        HDF_LOGE("%s: ecm is null", __func__);
        return;
    }
    (void)OsalMutexDestroy(&ecm->lock);
    OsalMemFree(ecm);
}

struct HdfDriverEntry g_ecmDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usbfn_cdcecm",
    .Bind          = EcmDriverBind,
    .Init          = EcmDriverInit,
    .Release       = EcmDriverRelease,
};

HDF_INIT(g_ecmDriverEntry);
