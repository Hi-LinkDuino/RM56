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

#include "usb_serial_rawapi.h"
#include <unistd.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_usb_pnp_manage.h"

#define HDF_LOG_TAG                     USB_HOST_ACM_RAW_API
#define USB_CTRL_REQ_SIZE               64
#define USB_IO_THREAD_STACK_SIZE        8192
#define USB_RAW_IO_SLEEP_MS_TIME        100
#define USB_RAW_IO_STOP_WAIT_MAX_TIME   3

static struct UsbRawRequest *g_syncRequest = NULL;
static UsbRawIoProcessStatusType g_stopIoStatus = USB_RAW_IO_PROCESS_RUNNING;
struct OsalMutex g_stopIoLock;
static bool g_rawAcmReleaseFlag = false;

static int32_t SerialSendCtrlMsg(struct AcmDevice *acm, uint8_t request,
    uint16_t value, void *buf, uint16_t len);
static void AcmWriteBulkCallback(const void *requestArg);
static int32_t UsbSerialInit(struct AcmDevice *acm);
static void UsbSerialRelease(struct AcmDevice *acm);

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
        if ((ret < 0) || (g_stopIoStatus != USB_RAW_IO_PROCESS_RUNNING)) {
            HDF_LOGE("%s:%d UsbIoThread faile, g_stopIoStatus=%d ret=%d ",
                __func__, __LINE__, g_stopIoStatus, ret);
            break;
        }
    }

    OsalMutexLock(&g_stopIoLock);
    g_stopIoStatus = USB_RAW_IO_PROCESS_STOPED;
    OsalMutexUnlock(&g_stopIoLock);

    HDF_LOGD("%s:%d exit", __func__, __LINE__);

    return HDF_SUCCESS;
}

static int32_t UsbStartIo(struct AcmDevice *acm)
{
    struct OsalThreadParam threadCfg;
    int32_t ret;

    HDF_LOGI("%s start", __func__);

    OsalMutexInit(&g_stopIoLock);

    OsalMutexLock(&g_stopIoLock);
    g_stopIoStatus = USB_RAW_IO_PROCESS_RUNNING;
    OsalMutexUnlock(&g_stopIoLock);

    /* creat Io thread */
    (void)memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    threadCfg.name      = "usb io thread";
    threadCfg.priority  = OSAL_THREAD_PRI_LOW;
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

    return HDF_SUCCESS;
}

static void UsbStopIo(struct AcmDevice *acm)
{
    int32_t ret;
    int32_t i = 0;

    if (g_stopIoStatus != USB_RAW_IO_PROCESS_STOPED) {
        HDF_LOGD("%s:%d not stopped", __func__, __LINE__);
        OsalMutexLock(&g_stopIoLock);
        g_stopIoStatus = USB_RAW_IO_PROCESS_STOP;
        OsalMutexUnlock(&g_stopIoLock);
    } else {
        HDF_LOGD("%s:%d stopped", __func__, __LINE__);
    }

    while (g_stopIoStatus != USB_RAW_IO_PROCESS_STOPED) {
        i++;
        OsalMSleep(USB_RAW_IO_SLEEP_MS_TIME);
        if (i > USB_RAW_IO_STOP_WAIT_MAX_TIME) {
            HDF_LOGD("%s:%d", __func__, __LINE__);
            break;
        }
    }

    ret = OsalThreadDestroy(&acm->ioThread);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalThreadDestroy faile, ret=%d ",
            __func__, __LINE__, ret);
    }

    OsalMutexDestroy(&g_stopIoLock);

    return;
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
        HDF_LOGE("%s:%d acm or config is NULL",
                 __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i < acm->interfaceCnt; i++) {
        uint8_t interfaceIndex = acm->interfaceIndex[i];
        const struct UsbRawInterface *interface = config->interface[interfaceIndex];

        ret = UsbRawClaimInterface(acm->devHandle, interfaceIndex);
        if (ret) {
            HDF_LOGE("%s:%d claim interface %u failed",
                     __func__, __LINE__, i);
            continue;
        }

        UsbParseConfigDescriptorProcess(acm, interface, interfaceIndex);
    }

    return HDF_SUCCESS;
}

static void UsbReleaseInterfaces(struct AcmDevice *acm)
{
    if ((acm == NULL) || (acm->devHandle == NULL)) {
        HDF_LOGE("%s:%d acm is NULL",
                 __func__, __LINE__);
        return;
    }

    (void)UsbRawReleaseInterface(acm->devHandle, acm->ctrlIface);
    (void)UsbRawReleaseInterface(acm->devHandle, acm->dataIface);

    if (acm->notifyEp) {
        OsalMemFree(acm->notifyEp);
        acm->notifyEp = NULL;
    }
    if (acm->dataInEp) {
        OsalMemFree(acm->dataInEp);
        acm->dataInEp = NULL;
    }
    if (acm->dataOutEp) {
        OsalMemFree(acm->dataOutEp);
        acm->dataOutEp = NULL;
    }
}

static int32_t UsbAllocWriteRequests(struct AcmDevice *acm)
{
    int32_t i;

    for (i = 0; i < ACM_NW; i++) {
        struct AcmWb *snd = &acm->wb[i];
        snd->request = UsbRawAllocRequest(acm->devHandle, 0, acm->dataOutEp->maxPacketSize);
        snd->instance = acm;
        if (snd->request == NULL) {
            HDF_LOGE("%s: UsbRawAllocRequest failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
    }

    return HDF_SUCCESS;
}

static void UsbFreeWriteRequests(struct AcmDevice *acm)
{
    int32_t i;
    struct AcmWb *snd = NULL;

    for (i = 0; i < ACM_NW; i++) {
        snd = &acm->wb[i];
        if (snd->request != NULL) {
            UsbRawFreeRequest(snd->request);
            snd->request = NULL;
        }
    }
}

static int32_t AcmWbAlloc(const struct AcmDevice *acm)
{
    struct AcmWb *wb = NULL;
    int32_t i;

    for (i = 0; i < ACM_NW; i++) {
        wb = (struct AcmWb *)&acm->wb[i];
        if (!wb->use) {
            wb->use = 1;
            wb->len = 0;
            return i;
        }
    }
    return -1;
}

static int32_t UsbSerialAllocFifo(struct DataFifo *fifo, uint32_t size)
{
    if (!DataFifoIsInitialized(fifo)) {
        void *data = OsalMemAlloc(size);
        if (data == NULL) {
            HDF_LOGE("%s:allocate failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
        DataFifoInit(fifo, size, data);
    }
    return HDF_SUCCESS;
}

static void UsbSerialFreeFifo(const struct DataFifo *fifo)
{
    if (fifo == NULL) {
        HDF_LOGE("%s:%d fifo is NULL", __func__, __LINE__);
        return;
    }

    if (fifo->data != NULL) {
        OsalMemFree((void *)fifo->data);
    }

    DataFifoInit((struct DataFifo *)fifo, 0, NULL);
}

static int32_t AcmWbIsAvail(const struct AcmDevice *acm)
{
    int32_t i;
    int32_t n = ACM_NW;

    OsalMutexLock((struct OsalMutex *)&acm->writeLock);
    for (i = 0; i < ACM_NW; i++) {
        n -= acm->wb[i].use;
    }
    OsalMutexUnlock((struct OsalMutex *)&acm->writeLock);
    return n;
}

static int32_t AcmStartWb(struct AcmDevice *acm, struct AcmWb *wb)
{
    struct UsbRawFillRequestData reqData;
    int32_t ret;
    if ((acm == NULL) || (wb == NULL) || (acm->dataOutEp == NULL)
        || (acm->devHandle == NULL) || (wb->request == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }

    acm->transmitting++;

    reqData.endPoint      = acm->dataOutEp->addr;
    reqData.numIsoPackets = 0;
    reqData.callback      = AcmWriteBulkCallback;
    reqData.userData      = (void *)wb;
    reqData.timeout       = USB_CTRL_SET_TIMEOUT;
    reqData.buffer        = wb->buf;
    reqData.length        = wb->len;

    ret = UsbRawFillBulkRequest(wb->request, acm->devHandle, &reqData);
    if (ret) {
        HDF_LOGE("%s: FillInterruptRequest faile, ret=%d", __func__, ret);
        return HDF_FAILURE;
    }

    acm->writeReq = wb->request;
    ret = UsbRawSubmitRequest(wb->request);
    if (ret) {
        HDF_LOGE("UsbRawSubmitRequest faile, ret=%d", ret);
        wb->use = 0;
        acm->transmitting--;
    }

    return ret;
}

static int32_t AcmWriteBufAlloc(const struct AcmDevice *acm)
{
    struct AcmWb *wb = (struct AcmWb *)&acm->wb[0];
    int32_t i;

    for (i = 0; i < ACM_NW; i++, wb++) {
        wb->buf = OsalMemCalloc(acm->dataOutEp->maxPacketSize);
        if (!wb->buf) {
            while (i > 0) {
                --i;
                --wb;
                OsalMemFree(wb->buf);
                wb->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        }
    }
    return HDF_SUCCESS;
}

static void AcmWriteBufFree(struct AcmDevice *acm)
{
    struct AcmWb *wb = &acm->wb[0];
    int32_t i;

    for (i = 0; i < ACM_NW; i++, wb++) {
        if (wb->buf) {
            OsalMemFree(wb->buf);
            wb->buf = NULL;
        }
    }
    return;
}

static void AcmWriteBulkCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;
    if (req == NULL) {
        HDF_LOGE("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    struct AcmWb *wb  = (struct AcmWb *)req->userData;
    if (wb == NULL) {
        HDF_LOGE("%s:%d userData(wb) is NULL!", __func__, __LINE__);
        return;
    }

    if (req->status != USB_REQUEST_COMPLETED) {
        HDF_LOGE("%s: write req failed, status=%d", __func__, req->status);
    }

    wb->use = 0;
}

static int32_t SerialSendCtrlMsg(struct AcmDevice *acm, uint8_t request,
    uint16_t value, void *buf, uint16_t len)
{
    struct UsbControlRequestData ctrlReq;
    int32_t ret;

    if (acm == NULL || buf == NULL) {
        HDF_LOGE("%s:invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (acm->ctrlReq == NULL) {
        acm->ctrlReq = UsbRawAllocRequest(acm->devHandle, 0, USB_CTRL_REQ_SIZE);
        if (acm->ctrlReq == NULL) {
            HDF_LOGE("%s: UsbRawAllocRequest failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }
    }

    ctrlReq.requestType = USB_DDK_DIR_OUT | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    ctrlReq.requestCmd  = request;
    ctrlReq.value       = CpuToLe16(value);
    ctrlReq.index       = 0;
    ctrlReq.data        = buf;
    ctrlReq.length      = len;
    ctrlReq.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendControlRequest(acm->ctrlReq, acm->devHandle, &ctrlReq);
    if (ret < HDF_SUCCESS) {
        HDF_LOGE("%s: UsbRawSendControlRequest failed, ret=%d", __func__, ret);
        return ret;
    }
    if (acm->ctrlReq->status) {
        HDF_LOGE("%s  status=%d ", __func__, acm->ctrlReq->status);
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

static void UsbSeriaDevicelFree(struct AcmDevice *acm)
{
    struct SerialDevice *port = acm->port;

    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return;
    }
    OsalMemFree(port);
    port = NULL;
}

static int32_t UsbSerialRead(struct SerialDevice *port, struct HdfSBuf *reply)
{
    struct AcmDevice *acm = port->acm;
    uint8_t *buf = NULL;
    int32_t ret = HDF_SUCCESS;
    uint32_t len;

    for (int32_t i = 0; i < ACM_NR; i++) {
        if(acm->readReq[i]->status != USB_REQUEST_COMPLETED) {
            HDF_LOGE("%s:%d i=%d status=%d!",
                __func__, __LINE__, i, acm->readReq[i]->status);
            return HDF_FAILURE;
        }
    }

    if (DataFifoIsEmpty(&port->readFifo)) {
        if (!HdfSbufWriteString(reply, NULL)) {
            HDF_LOGE("%s:%d sbuf write buffer failed", __func__, __LINE__);
            ret = HDF_ERR_IO;
        }
        return HDF_SUCCESS;
    }

    buf = (uint8_t *)OsalMemCalloc(DataFifoLen(&port->readFifo) + 1);
    if (buf == NULL) {
        HDF_LOGE("%s:%d OsalMemCalloc error", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    OsalMutexLock(&acm->readLock);
    len = DataFifoRead(&port->readFifo, buf, DataFifoLen(&port->readFifo));
    if (len == 0) {
        HDF_LOGE("%s:%d no data", __func__, __LINE__);
        ret = HDF_SUCCESS;
        OsalMutexUnlock(&acm->readLock);
        goto OUT;
    }
    OsalMutexUnlock(&acm->readLock);

    if (!HdfSbufWriteString(reply, (const char *)buf)) {
        HDF_LOGE("%s:%d sbuf write buffer failed", __func__, __LINE__);
        ret = HDF_ERR_IO;
    }

OUT:
    OsalMemFree(buf);
    return ret;
}

static int32_t SerialSetBaudrate(struct SerialDevice *port, const struct HdfSBuf *data)
{
    struct AcmDevice *acm = port->acm;
    int32_t ret;
    uint32_t baudRate = 0;

    if (!HdfSbufReadUint32((struct HdfSBuf *)data, &baudRate)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
    port->lineCoding.dwDTERate = CpuToLe32(baudRate);
    if (memcmp(&acm->lineCoding, &port->lineCoding, sizeof(struct UsbCdcLineCoding))) {
        ret = memcpy_s(&acm->lineCoding, sizeof(struct UsbCdcLineCoding),
            &port->lineCoding, sizeof(port->lineCoding));
        if (ret != EOK) {
            HDF_LOGE("memcpy_s fail, ret=%d", ret);
        }

        HDF_LOGE("%s - set line: %d %d %d %d\n",
            __func__, (port->lineCoding.dwDTERate), port->lineCoding.bCharFormat,
            port->lineCoding.bParityType, port->lineCoding.bDataBits);

        ret = SerialSendCtrlMsg(acm, USB_DDK_CDC_REQ_SET_LINE_CODING, 0, &acm->lineCoding,
            sizeof(struct UsbCdcLineCoding));
        if (ret) {
            HDF_LOGE("SerialSendCtrlMsg fail\n");
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static int32_t SerialGetBaudrate(struct SerialDevice *port, struct HdfSBuf *reply)
{
    uint32_t baudRate = Le32ToCpu(port->lineCoding.dwDTERate);

    if (!HdfSbufWriteUint32(reply, baudRate)) {
        HDF_LOGE("%s:%d sbuf write buffer failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    HDF_LOGE("%s:%d baudRate=%d", __func__, __LINE__, baudRate);

    return HDF_SUCCESS;
}

static int32_t SerialOpen(struct SerialDevice *port, struct HdfSBuf *data)
{
    struct AcmDevice *acm = NULL;
    int32_t ret;
    int32_t cmdType = HOST_ACM_ASYNC_READ;

    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    acm = port->acm;
    if (acm == NULL) {
        HDF_LOGE("%s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &cmdType)) {
        HDF_LOGE("%s:%d sbuf read cmdType failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = UsbSerialInit(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d UsbSerialInit failed", __func__, __LINE__);
        return  HDF_FAILURE;
    }

    if (cmdType != HOST_ACM_ASYNC_READ) {
        HDF_LOGD("%s:%d asyncRead success", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    ret = UsbSerialAllocFifo(&port->readFifo, READ_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAllocFifo failed", __func__);
        return  HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < ACM_NR; i++) {
        ret = UsbRawSubmitRequest(acm->readReq[i]);
        if (ret) {
            HDF_LOGE("%s: UsbRawSubmitRequest failed, ret=%d ", __func__, ret);
            goto ERR;
        }
    }
    return HDF_SUCCESS;

ERR:
    UsbSerialFreeFifo(&port->readFifo);
    return ret;
}

static int32_t SerialClose(struct SerialDevice *port, struct HdfSBuf *data)
{
    int32_t cmdType = HOST_ACM_SYNC_READ;

    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%s:%d invalid parma", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (port->acm == NULL) {
        HDF_LOGE("%s:%d acm is NULL invalid parma", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &cmdType)) {
        HDF_LOGE("%s:%d sbuf read cmdType failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if ((cmdType == HOST_ACM_SYNC_READ) || (cmdType == HOST_ACM_SYNC_WRITE) || (cmdType == HOST_ACM_ASYNC_WRITE)) {
        HDF_LOGD("%s:%d cmdType=%d success", __func__, __LINE__, cmdType);
        return HDF_SUCCESS;
    }

    OsalMutexLock(&port->acm->readLock);
    UsbSerialFreeFifo(&port->readFifo);
    OsalMutexUnlock(&port->acm->readLock);

    UsbSerialRelease(port->acm);

    return HDF_SUCCESS;
}

static int32_t SerialWrite(struct SerialDevice *port, struct HdfSBuf *data)
{
    struct AcmDevice *acm = NULL;
    struct AcmWb *wb = NULL;
    const char *tmp = NULL;
    uint32_t size;
    int32_t ret;
    int32_t wbn;

    if (port == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    acm = port->acm;
    if (acm == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (AcmWbIsAvail(acm)) {
        wbn = AcmWbAlloc(acm);
    } else {
        HDF_LOGE("no write buf\n");
        return HDF_SUCCESS;
    }
    if (wbn < 0 || wbn >= ACM_NW) {
        HDF_LOGE("AcmWbAlloc failed\n");
        return HDF_FAILURE;
    }
    wb = &acm->wb[wbn];
    if (wb == NULL) {
        return HDF_FAILURE;
    }
    tmp = HdfSbufReadString(data);
    if (tmp == NULL) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
    size = strlen(tmp) + 1;
    if (acm->dataOutEp != NULL) {
        size = (size > acm->dataOutEp->maxPacketSize) ? acm->dataOutEp->maxPacketSize : size;
        ret = memcpy_s(wb->buf, acm->dataOutEp->maxPacketSize, tmp, size);
        if (ret != EOK) {
            HDF_LOGE("%s: memcpy_s fail", __func__);
        }
    }
    wb->len = (int)size;
    ret = AcmStartWb(acm, wb);
    return size;
}

static int32_t AcmStartWbSync(struct AcmDevice *acm, struct AcmWb *wb)
{
    int32_t ret;
    int32_t size;
    struct UsbRequestData requestData;

    requestData.endPoint    = acm->dataOutEp->addr;
    requestData.data        = wb->buf;
    requestData.length      = wb->len;
    requestData.requested   = &size;
    requestData.timeout     = USB_CTRL_SET_TIMEOUT;

    acm->writeReq = wb->request;
    ret = UsbRawSendBulkRequest(wb->request, acm->devHandle, &requestData);
    if (ret) {
        HDF_LOGE("UsbRawSendBulkRequest faile, ret=%d", ret);
    }

    wb->use = 0;

    return ret;
}

static int32_t SerialWriteSync(const struct SerialDevice *port, const struct HdfSBuf *data)
{
    struct AcmDevice *acm = NULL;
    struct AcmWb *wb = NULL;
    const char *tmp = NULL;
    uint32_t size;
    int32_t ret;
    int32_t wbn;

    if (port == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    acm = port->acm;
    if (acm == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (AcmWbIsAvail(acm)) {
        wbn = AcmWbAlloc(acm);
    } else {
        HDF_LOGE("no write buf\n");
        return HDF_SUCCESS;
    }

    if (wbn >= ACM_NW || wbn < 0) {
        wbn = 0;
    }
    wb = &acm->wb[wbn];
    if ((wb == NULL) || (wb->buf == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }
    tmp = HdfSbufReadString((struct HdfSBuf *)data);
    if (tmp == NULL) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
    size = strlen(tmp) + 1;
    if (acm->dataOutEp == NULL) {
        return HDF_ERR_IO;
    }
    size = (size > acm->dataOutEp->maxPacketSize) ? acm->dataOutEp->maxPacketSize : size;
    ret = memcpy_s(wb->buf, acm->dataOutEp->maxPacketSize, tmp, size);
    if (ret != EOK) {
        HDF_LOGE("%s: memcpy_s fail, ret=%d", __func__, ret);
    }
    wb->len = (int)size;
    ret = AcmStartWbSync(acm, wb);

    return size;
}

static int32_t UsbSerialReadSync(const struct SerialDevice *port, const struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t size;
    struct AcmDevice *acm = port->acm;
    uint8_t *data = NULL;
    uint32_t count;
    struct UsbRequestData requestData;

    if (g_syncRequest == NULL) {
        g_syncRequest = UsbRawAllocRequest(acm->devHandle, 0, acm->dataInEp->maxPacketSize);
        if (g_syncRequest == NULL) {
            HDF_LOGE("UsbRawAllocRequest g_syncRequest failed\n");
            return HDF_ERR_MALLOC_FAIL;
        }
    }
    HDF_LOGD("%s:%d g_syncRequest \n", __func__, __LINE__);

    requestData.endPoint    = acm->dataInEp->addr;
    requestData.data        = g_syncRequest->buffer;
    requestData.length      = acm->dataInEp->maxPacketSize;
    requestData.requested   = &size;
    requestData.timeout     = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawSendBulkRequest(g_syncRequest, acm->devHandle, &requestData);
    if (ret) {
        HDF_LOGE("UsbRawSendBulkRequest faile, ret=%d", ret);
        return ret;
    }

    count = (uint32_t)g_syncRequest->actualLength;
    data = (uint8_t *)OsalMemCalloc(count + 1);
    if (data == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    HDF_LOGD("buffer:%p-%s-actualLength:%d", g_syncRequest->buffer,
        (uint8_t *)g_syncRequest->buffer, count);
    ret = memcpy_s(data, g_syncRequest->actualLength, g_syncRequest->buffer, count);
    if (ret) {
        HDF_LOGE("memcpy_s error");
    }
    if (!HdfSbufWriteString((struct HdfSBuf *)reply, (char *)data)) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        ret = HDF_ERR_IO;
    }

    OsalMemFree(data);
    data = NULL;
    return HDF_SUCCESS;
}

static int32_t SerialAddOrRemoveInterface(int32_t cmd, const struct SerialDevice *port, const struct HdfSBuf *data)
{
    UsbInterfaceStatus status;
    uint32_t index;

    if (!HdfSbufReadUint32((struct HdfSBuf *)data, &index)) {
        HDF_LOGE("%s:%d sbuf read interfaceNum failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cmd == CMD_ADD_INTERFACE) {
        status = USB_INTERFACE_STATUS_ADD;
    } else if (cmd == CMD_REMOVE_INTERFACE) {
        status = USB_INTERFACE_STATUS_REMOVE;
    } else {
        HDF_LOGE("%s:%d cmd=% is not define", __func__, __LINE__, cmd);
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

static int32_t UsbSerialDeviceDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct AcmDevice *acm = NULL;
    struct SerialDevice *port = NULL;

    if (client == NULL) {
        HDF_LOGE("%s:%d client is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (client->device == NULL) {
        HDF_LOGE("%s:%d client->device is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (client->device->service == NULL) {
        HDF_LOGE("%s:%d client->device->service is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (g_rawAcmReleaseFlag) {
        HDF_LOGE("%s:%d g_rawAcmReleaseFlag is true", __func__, __LINE__);
        return HDF_FAILURE;
    }

    acm = (struct AcmDevice *)client->device->service;
    if (acm == NULL) {
        return HDF_FAILURE;
    }
    port = acm->port;
    if (port == NULL) {
        return HDF_FAILURE;
    }
    switch (cmd) {
        case CMD_OPEN_PARM:
            return SerialOpen(port, data);
        case CMD_CLOSE_PARM:
            return SerialClose(port, data);
        case CMD_WRITE_PARM:
            return SerialWrite(port, data);
        case CMD_READ_PARM:
            return UsbSerialRead(port, reply);
        case CMD_GET_BAUDRATE:
            return SerialGetBaudrate(port, reply);
        case CMD_SET_BAUDRATE:
            return SerialSetBaudrate(port, data);
        case CMD_WRITE_DATA_SYNC:
            return SerialWriteSync(port, data);
        case CMD_READ_DATA_SYNC:
            return UsbSerialReadSync(port, reply);
        case CMD_ADD_INTERFACE:
        case CMD_REMOVE_INTERFACE:
            return SerialAddOrRemoveInterface(cmd, port, data);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
}

/* HdfDriverEntry implementations */
static int32_t UsbSerialDriverBind(struct HdfDeviceObject *device)
{
    struct AcmDevice *acm = NULL;
    struct UsbPnpNotifyServiceInfo *info = NULL;
    errno_t err;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    acm = (struct AcmDevice *)OsalMemCalloc(sizeof(*acm));
    if (acm == NULL) {
        HDF_LOGE("%s: Alloc usb serial device failed", __func__);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&acm->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalMutexInit fail", __func__, __LINE__);
        goto ERROR;
    }

    info = (struct UsbPnpNotifyServiceInfo *)device->priv;
    if (info != NULL) {
        acm->busNum       = (uint8_t)info->busNum;
        acm->devAddr      = (uint8_t)info->devNum;
        acm->interfaceCnt = info->interfaceLength;
        err = memcpy_s((void *)(acm->interfaceIndex), USB_MAX_INTERFACES,
                       (const void*)info->interfaceNumber, info->interfaceLength);
        if (err != EOK) {
            HDF_LOGE("%s:%d memcpy_s faile err=%d", \
                __func__, __LINE__, err);
            goto LOCK_ERROR;
        }
    } else {
        HDF_LOGE("%s:%d info is NULL!", __func__, __LINE__);
        goto LOCK_ERROR;
    }

    device->service = &(acm->service);
    device->service->Dispatch = UsbSerialDeviceDispatch;
    acm->device = device;
    HDF_LOGD("UsbSerialDriverBind=========================OK");
    return HDF_SUCCESS;

LOCK_ERROR:
    if (OsalMutexDestroy(&acm->lock)) {
        HDF_LOGE("%s:%d OsalMutexDestroy fail", __func__, __LINE__);
    }
ERROR:
    OsalMemFree(acm);
    acm = NULL;
    return HDF_FAILURE;
}

static void AcmProcessNotification(const struct AcmDevice *acm, const unsigned char *buf)
{
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)buf;

    switch (dr->bNotificationType) {
        case USB_DDK_CDC_NOTIFY_NETWORK_CONNECTION:
            HDF_LOGE("%s - network connection: %d\n", __func__, dr->wValue);
            break;
        case USB_DDK_CDC_NOTIFY_SERIAL_STATE:
            HDF_LOGE("the serial State change\n");
            break;
        default:
            HDF_LOGE("%s-%d received: index %d len %d\n",
                     __func__, dr->bNotificationType, dr->wIndex, dr->wLength);
    }
}

static int32_t AcmNotificationBufferProcess(const struct UsbRawRequest *req,
    struct AcmDevice *acm, unsigned int currentSize, unsigned int expectedSize)
{
    int32_t ret;
    unsigned int copySize;
    unsigned int allocSize;

    if (acm->nbSize < expectedSize) {
        if (acm->nbSize) {
            OsalMemFree(acm->notificationBuffer);
            acm->nbSize = 0;
        }
        allocSize = expectedSize;
        acm->notificationBuffer = (uint8_t *)OsalMemCalloc(allocSize);
        if (!acm->notificationBuffer) {
            return HDF_FAILURE;
        }
        acm->nbSize = allocSize;
    }
    copySize = MIN(currentSize, expectedSize - acm->nbIndex);
    ret = memcpy_s(&acm->notificationBuffer[acm->nbIndex], acm->nbSize - acm->nbIndex,
        req->buffer, copySize);
    if (ret != EOK) {
        HDF_LOGE("memcpy_s fail ret=%d", ret);
    }
    acm->nbIndex += copySize;

    return HDF_SUCCESS;
}

static void AcmNotifyReqCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;
    if (req == NULL) {
        HDF_LOGE("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    struct AcmDevice *acm = (struct AcmDevice *)req->userData;
    if (acm == NULL) {
        HDF_LOGE("%s:%d userData(acm) is NULL!", __func__, __LINE__);
        return;
    }
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)req->buffer;
    if (dr == NULL) {
        HDF_LOGE("%s:%d req->buffer(dr) is NULL!", __func__, __LINE__);
        return;
    }
    unsigned int currentSize = (unsigned int)req->actualLength;
    unsigned int expectedSize = 0;

    HDF_LOGD("Irqstatus:%d,actualLength:%u\n", req->status, currentSize);

    if (req->status != USB_REQUEST_COMPLETED) {
        goto EXIT;
    }

    if (acm->nbIndex) {
        dr = (struct UsbCdcNotification *)acm->notificationBuffer;
    }
    if (dr != NULL) {
        expectedSize = sizeof(struct UsbCdcNotification) + Le16ToCpu(dr->wLength);
    } else {
        HDF_LOGE("%s:%d dr is NULL!", __func__, __LINE__);
        return;
    }
    if (currentSize < expectedSize) {
        if (AcmNotificationBufferProcess(req, acm, currentSize, expectedSize) != HDF_SUCCESS) {
            goto EXIT;
        }
        currentSize = acm->nbIndex;
    }
    if (currentSize >= expectedSize) {
        AcmProcessNotification(acm, (unsigned char *)dr);
        acm->nbIndex = 0;
    }

    if (UsbRawSubmitRequest(req)) {
        HDF_LOGE("%s - UsbRawSubmitRequest failed", __func__);
    }

EXIT:
    HDF_LOGE("%s:%d exit", __func__, __LINE__);
}

static void AcmReadBulkCallback(const void *requestArg)
{
    struct UsbRawRequest *req = (struct UsbRawRequest *)requestArg;
    if (req == NULL) {
        HDF_LOGE("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    struct AcmDevice *acm = (struct AcmDevice *)req->userData;
    if (acm == NULL || acm->port == NULL) {
        HDF_LOGE("%s:%d userData(acm) is NULL!", __func__, __LINE__);
        return;
    }
    size_t size = (size_t)req->actualLength;

    switch (req->status) {
        case USB_REQUEST_COMPLETED:
            HDF_LOGD("Bulk status: %d+size:%zu", req->status, size);
            if (size) {
                uint8_t *data = req->buffer;
                uint32_t count;

                OsalMutexLock(&acm->readLock);
                if (DataFifoIsFull(&acm->port->readFifo)) {
                    DataFifoSkip(&acm->port->readFifo, size);
                }
                count = DataFifoWrite(&acm->port->readFifo, data, size);
                if (count != size) {
                    HDF_LOGW("%s: write %u less than expected %zu", __func__, count, size);
                }
                OsalMutexUnlock(&acm->readLock);
            }
            break;
        default:
            HDF_LOGW("%s:%d the request is failed, staus=%d",
                __func__, __LINE__, req->status);
            return;
    }

    if (UsbRawSubmitRequest(req)) {
        HDF_LOGE("%s - UsbRawSubmitRequest failed", __func__);
    }
}

static int32_t UsbAllocReadRequests(struct AcmDevice *acm)
{
    struct UsbRawFillRequestData reqData;
    uint32_t size = acm->dataInEp->maxPacketSize;
    int32_t ret;

    for (int32_t i = 0; i < ACM_NR; i++) {
        acm->readReq[i] = UsbRawAllocRequest(acm->devHandle, 0, size);
        if (!acm->readReq[i]) {
            HDF_LOGE("readReq request failed\n");
            return HDF_ERR_MALLOC_FAIL;
        }

        reqData.endPoint      = acm->dataInEp->addr;
        reqData.numIsoPackets = 0;
        reqData.callback      = AcmReadBulkCallback;
        reqData.userData      = (void *)acm;
        reqData.timeout       = USB_CTRL_SET_TIMEOUT;
        reqData.length        = size;

        ret = UsbRawFillBulkRequest(acm->readReq[i], acm->devHandle, &reqData);
        if (ret) {
            HDF_LOGE("%s: FillBulkRequest faile, ret=%d \n",
                     __func__, ret);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

static void UsbFreeReadRequests(struct AcmDevice *acm)
{
    int32_t i;

    if (acm == NULL) {
        HDF_LOGE("%s: acm is NULL", __func__);
        return;
    }

    for (i = 0; i < ACM_NR; i++) {
        if (acm->readReq[i]) {
            UsbRawFreeRequest(acm->readReq[i]);
            acm->readReq[i] = NULL;
        }
    }
}

static int32_t UsbAllocNotifyRequest(struct AcmDevice *acm)
{
    struct UsbRawFillRequestData fillRequestData;
    uint32_t size = acm->notifyEp->maxPacketSize;
    int32_t ret;

    acm->notifyReq = UsbRawAllocRequest(acm->devHandle, 0, size);
    if (!acm->notifyReq) {
        HDF_LOGE("notifyReq request fail\n");
        return HDF_ERR_MALLOC_FAIL;
    }

    fillRequestData.endPoint = acm->notifyEp->addr;
    fillRequestData.length = size;
    fillRequestData.numIsoPackets = 0;
    fillRequestData.callback = AcmNotifyReqCallback;
    fillRequestData.userData = (void *)acm;
    fillRequestData.timeout = USB_CTRL_SET_TIMEOUT;

    ret = UsbRawFillInterruptRequest(acm->notifyReq, acm->devHandle, &fillRequestData);
    if (ret) {
        HDF_LOGE("%s: FillInterruptRequest faile, ret=%d", __func__, ret);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static void UsbFreeNotifyReqeust(struct AcmDevice *acm)
{
    int32_t ret;

    if ((acm == NULL) || (acm->notifyReq == NULL)) {
        HDF_LOGE("%s: acm or notifyReq is NULL", __func__);
        return;
    }

    ret = UsbRawFreeRequest(acm->notifyReq);
    if (ret == HDF_SUCCESS) {
        acm->notifyReq = NULL;
    } else {
        HDF_LOGE("%s: UsbFreeNotifyReqeust failed, ret=%d", __func__, ret);
    }
}

static int32_t UsbSerialInit(struct AcmDevice *acm)
{
    struct UsbSession *session = NULL;
    UsbRawHandle *devHandle = NULL;
    int32_t ret;

    if (acm->initFlag) {
        HDF_LOGE("%s:%d: initFlag is true", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    ret = UsbRawInit(NULL);
    if (ret) {
        HDF_LOGE("%s:%d UsbRawInit failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    acm->session = session;

    devHandle = UsbRawOpenDevice(session, acm->busNum, acm->devAddr);
    if (devHandle == NULL) {
        HDF_LOGE("%s:%d UsbRawOpenDevice failed", __func__, __LINE__);
        ret =  HDF_FAILURE;
        goto ERR_OPEN_DEVICE;
    }
    acm->devHandle = devHandle;
    ret = UsbGetConfigDescriptor(devHandle, &acm->config);
    if (ret) {
        HDF_LOGE("%s:%d UsbGetConfigDescriptor failed", __func__, __LINE__);
        ret =  HDF_FAILURE;
        goto ERR_GET_DESC;
    }
    ret = UsbParseConfigDescriptor(acm, acm->config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d UsbParseConfigDescriptor failed", __func__, __LINE__);
        ret = HDF_FAILURE;
        goto ERR_PARSE_DESC;
    }

    ret = AcmWriteBufAlloc(acm);
    if (ret < 0) {
        HDF_LOGE("%s:%d AcmWriteBufAlloc failed", __func__, __LINE__);
        ret = HDF_FAILURE;
        goto ERR_ALLOC_WRITE_BUF;
    }
    ret = UsbAllocWriteRequests(acm);
    if (ret < 0) {
        HDF_LOGE("%s:%d UsbAllocWriteRequests failed", __func__, __LINE__);
        ret = HDF_FAILURE;
        goto ERR_ALLOC_WRITE_REQS;
    }
    ret = UsbAllocNotifyRequest(acm);
    if (ret) {
        HDF_LOGE("%s:%d UsbAllocNotifyRequests failed", __func__, __LINE__);
        goto ERR_ALLOC_NOTIFY_REQ;
    }
    ret = UsbAllocReadRequests(acm);
    if (ret) {
        HDF_LOGE("%s:%d UsbAllocReadRequests failed", __func__, __LINE__);
        goto ERR_ALLOC_READ_REQS;
    }
    ret = UsbStartIo(acm);
    if (ret) {
        HDF_LOGE("%s:%d UsbAllocReadRequests failed", __func__, __LINE__);
        goto ERR_START_IO;
    }

    acm->lineCoding.dwDTERate   = CpuToLe32(DATARATE);
    acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    acm->lineCoding.bDataBits   = DATA_BITS_LENGTH;

    ret = UsbRawSubmitRequest(acm->notifyReq);
    if (ret) {
        HDF_LOGE("%s:%d UsbRawSubmitRequest failed", __func__, __LINE__);
        goto ERR_SUBMIT_REQ;
    }

    acm->initFlag = true;

    HDF_LOGD("%s:%d=========================OK", __func__, __LINE__);

    return HDF_SUCCESS;

ERR_SUBMIT_REQ:
    UsbStopIo(acm);
ERR_START_IO:
    UsbFreeReadRequests(acm);
ERR_ALLOC_READ_REQS:
    UsbFreeNotifyReqeust(acm);
ERR_ALLOC_NOTIFY_REQ:
    UsbFreeWriteRequests(acm);
ERR_ALLOC_WRITE_REQS:
    AcmWriteBufFree(acm);
ERR_ALLOC_WRITE_BUF:
    UsbReleaseInterfaces(acm);
ERR_PARSE_DESC:
    UsbRawFreeConfigDescriptor(acm->config);
    acm->config = NULL;
ERR_GET_DESC:
    (void)UsbRawCloseDevice(devHandle);
ERR_OPEN_DEVICE:
    UsbRawExit(acm->session);

    return ret;
}

static void UsbSerialRelease(struct AcmDevice *acm)
{
    if (!(acm->initFlag)) {
        HDF_LOGE("%s:%d: initFlag is false", __func__, __LINE__);
        return;
    }

    /* stop io thread and release all resources */
    UsbStopIo(acm);
    if (g_syncRequest != NULL) {
        UsbRawFreeRequest(g_syncRequest);
        g_syncRequest = NULL;
    }
    UsbFreeReadRequests(acm);
    UsbFreeNotifyReqeust(acm);
    UsbFreeWriteRequests(acm);
    AcmWriteBufFree(acm);
    UsbReleaseInterfaces(acm);
    (void)UsbRawCloseDevice(acm->devHandle);
    UsbRawFreeConfigDescriptor(acm->config);
    acm->config = NULL;
    UsbRawExit(acm->session);

    acm->initFlag = false;
}

static int32_t UsbSerialDriverInit(struct HdfDeviceObject *device)
{
    struct AcmDevice *acm = NULL;
    int32_t ret;

    if (device == NULL) {
        HDF_LOGE("%s:%d device is null", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    acm = (struct AcmDevice *)device->service;
    if (acm == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    OsalMutexInit(&acm->readLock);
    OsalMutexInit(&acm->writeLock);

    ret = UsbSerialDeviceAlloc(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d UsbSerialDeviceAlloc failed", __func__, __LINE__);
    }

    acm->initFlag = false;
    g_rawAcmReleaseFlag = false;

    HDF_LOGD("%s:%d init ok!", __func__, __LINE__);

    return ret;
}

static void UsbSerialDriverRelease(struct HdfDeviceObject *device)
{
    struct AcmDevice *acm = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }

    acm = (struct AcmDevice *)device->service;
    if (acm == NULL) {
        HDF_LOGE("%s: acm is null", __func__);
        return;
    }

    g_rawAcmReleaseFlag = true;

    if (acm->initFlag) {
        HDF_LOGE("%s:%d UsbSerialRelease", __func__, __LINE__);
        UsbSerialRelease(acm);
    }
    UsbSeriaDevicelFree(acm);
    OsalMutexDestroy(&acm->writeLock);
    OsalMutexDestroy(&acm->readLock);
    OsalMutexDestroy(&acm->lock);
    OsalMemFree(acm);
    acm = NULL;
    HDF_LOGD("%s:%d exit", __func__, __LINE__);
}

struct HdfDriverEntry g_usbSerialRawDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usbhost_acm_rawapi",
    .Bind          = UsbSerialDriverBind,
    .Init          = UsbSerialDriverInit,
    .Release       = UsbSerialDriverRelease,
};
HDF_INIT(g_usbSerialRawDriverEntry);

