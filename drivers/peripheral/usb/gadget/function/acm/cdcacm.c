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

#include "../include/cdcacm.h"
#include "hdf_base.h"
#include "hdf_device_object.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usbfn_device.h"
#include "usbfn_interface.h"
#include "usbfn_request.h"
#include "default_config.h"

#define HDF_LOG_TAG hdf_cdc_acm

#define PENDING_FLAG            0
#define CTRL_REQUEST_NUM        2
#define QUEUE_SIZE              8
#define WRITE_BUF_SIZE          8192
#define READ_BUF_SIZE           8192

#define PORT_RATE       9600
#define DATA_BIT        8
static int32_t g_inFifo = 0;
/* Usb Serial Related Functions */

static int32_t UsbSerialInit(struct UsbAcmDevice *acm);
static int32_t UsbSerialRelease(struct UsbAcmDevice *acm);
static int32_t UsbSerialStartTx(struct UsbSerial *port)
{
    if (port == NULL) {
        return HDF_FAILURE;
    }
    struct DListHead *pool = &port->writePool;
    int32_t ret = HDF_FAILURE;
    if (port->acm == NULL) {
        return HDF_SUCCESS;
    }
    while (!port->writeBusy && !DListIsEmpty(pool)) {
        struct UsbFnRequest *req = NULL;
        uint32_t len;
        if (port->writeStarted >= QUEUE_SIZE) {
            break;
        }
        req = DLIST_FIRST_ENTRY(pool, struct UsbFnRequest, list);
        if (req == NULL) {
            break;
        }
        len = DataFifoRead(&port->writeFifo, req->buf, port->acm->dataInPipe.maxPacketSize);
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
        /* if acm is disconnect, abort immediately */
        if (port->acm == NULL) {
            break;
        }
    }
    return ret;
}

static uint32_t UsbSerialStartRx(struct UsbSerial *port)
{
    struct DListHead *pool = &port->readPool;
    struct UsbAcmPipe *out = &port->acm->dataOutPipe;
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
        /* if acm is disconnect, abort immediately */
        if (port->acm == NULL) {
            break;
        }
    }
    return port->readStarted;
}

static void UsbSerialRxPush(struct UsbSerial *port)
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

        if (g_inFifo && req->actual) {
            uint32_t size = req->actual;
            uint8_t *data = req->buf;
            uint32_t count;

            if (DataFifoIsFull(&port->readFifo)) {
                DataFifoSkip(&port->readFifo, size);
            }
            count = DataFifoWrite(&port->readFifo, data, size);
            if (count != size) {
                HDF_LOGW("%s: write %u less than expected %u", __func__, count, size);
            }
        }

        DListRemove(&req->list);
        DListInsertTail(&req->list, &port->readPool);
        port->readStarted--;
    }

    if (!disconnect && port->acm) {
        UsbSerialStartRx(port);
    }
}

static void UsbSerialFreeRequests(struct DListHead *head, int32_t *allocated)
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

static bool g_isStartRead = false;
static bool g_isReadDone = false;
static uint64_t g_readCnt = 0;
struct timeval g_readTimeStart, g_readTimeEnd;
static float g_readSpeed = 0;
static bool isGetReadTimeStart = false;
static void UsbSerialReadComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct UsbSerial *port = (struct UsbSerial *)req->context;
    if ((!g_isReadDone) && g_isStartRead && req->status == USB_REQUEST_COMPLETED) {
        g_readCnt += req->actual;
        if (!isGetReadTimeStart) {
            isGetReadTimeStart = true;
            gettimeofday(&g_readTimeStart, NULL);
        }
    }
    OsalMutexLock(&port->lock);
    DListInsertTail(&req->list, &port->readQueue);
    UsbSerialRxPush(port);
    OsalMutexUnlock(&port->lock);
}

static int32_t SpeedReadThread(void *arg)
{
    g_readCnt = 0;
    g_isReadDone = false;
    g_readSpeed = 0;
    isGetReadTimeStart = false;
    double timeUse;
    double usec = 1000000;
    double k = 1024;
    struct timeval timeTmp;
    while (!g_isReadDone) {
        if (g_readCnt == 0) {
            OsalSleep(1);
            continue;
        } else {
            OsalSleep(1);
        }
        gettimeofday(&timeTmp, NULL);
        timeUse = (double)(timeTmp.tv_sec - g_readTimeStart.tv_sec) +
        (double)timeTmp.tv_usec / usec - (double)g_readTimeStart.tv_usec / usec;
        g_readSpeed = (float)((double)g_readCnt / k / k / timeUse);
    }
    timeUse = (double)(g_readTimeEnd.tv_sec - g_readTimeStart.tv_sec) +
        (double)g_readTimeEnd.tv_usec / usec - (double)g_readTimeStart.tv_usec / usec;
    HDF_LOGD("timeUse = %lf\n", timeUse);
    g_readSpeed = (float)((double)g_readCnt / k / k / timeUse);
    HDF_LOGD("%s: g_speed = %f MB/s", __func__, g_readSpeed);
    return HDF_SUCCESS;
}

#define HDF_PROCESS_STACK_SIZE 10000
struct OsalThread g_threadRead;
static int32_t StartThreadReadSpeed(struct UsbSerial *port)
{
    int32_t ret;
    struct OsalThreadParam threadCfg;
    ret = memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
        return ret;
    }
   
    threadCfg.name = "speed read process";
    threadCfg.priority = OSAL_THREAD_PRI_LOW;
    threadCfg.stackSize = HDF_PROCESS_STACK_SIZE;

    ret = OsalThreadCreate(&g_threadRead, (OsalThreadEntry)SpeedReadThread, port);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadCreate faile, ret=%d ", __func__, __LINE__, ret);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = OsalThreadStart(&g_threadRead, &threadCfg);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadStart faile, ret=%d ", __func__, __LINE__, ret);
        return HDF_ERR_DEVICE_BUSY;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialGetTempReadSpeed(struct UsbSerial *port, struct HdfSBuf *reply)
{
    if (!HdfSbufWriteFloat(reply, g_readSpeed)) {
        HDF_LOGE("%s: HdfSbufWriteFloat failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialGetTempReadSpeedInt(struct UsbSerial *port, struct HdfSBuf *reply)
{
    uint32_t calc = 10000;
    if (!HdfSbufWriteUint32(reply, (uint32_t)(g_readSpeed * calc))) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialReadSpeedDone(struct UsbSerial *port)
{
    gettimeofday(&g_readTimeEnd, NULL);
    g_isReadDone = true;
    g_isStartRead = false;
    return HDF_SUCCESS;
}

static int32_t UsbSerialReadSpeedStart(struct UsbSerial *port)
{
    g_inFifo = 0;
    g_isStartRead = true;
    return StartThreadReadSpeed(port);
}

static void UsbSerialWriteComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct UsbSerial *port = (struct UsbSerial *)req->context;

    OsalMutexLock(&port->lock);
    DListInsertTail(&req->list, &port->writePool);
    port->writeStarted--;

    switch (req->status) {
        case USB_REQUEST_COMPLETED:
            UsbSerialStartTx(port);
            break;
        case USB_REQUEST_NO_DEVICE:
            HDF_LOGV("%s: acm device was disconnected", __func__);
            break;
        default:
            HDF_LOGV("%s: unexpected status %d", __func__, req->status);
            break;
    }
    OsalMutexUnlock(&port->lock);
}

static int32_t UsbSerialAllocReadRequests(struct UsbSerial *port, int32_t num)
{
    struct UsbAcmDevice *acm = port->acm;
    struct DListHead *head = &port->readPool;
    struct UsbFnRequest *req = NULL;
    int32_t  i;

    for (i = 0; i < num; i++) {
        req = UsbFnAllocRequest(acm->dataIface.handle, acm->dataOutPipe.id,
            acm->dataOutPipe.maxPacketSize);
        if (!req) {
            return DListIsEmpty(head) ? HDF_FAILURE : HDF_SUCCESS;
        }

        req->complete = UsbSerialReadComplete;
        req->context = port;
        DListInsertTail(&req->list, head);
        port->readAllocated++;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialAllocWriteRequests(struct UsbSerial *port, int32_t num)
{
    struct UsbAcmDevice *acm = port->acm;
    struct DListHead *head = &port->writePool;
    struct UsbFnRequest  *req = NULL;
    int32_t i;

    for (i = 0; i < num; i++) {
        req = UsbFnAllocRequest(acm->dataIface.handle, acm->dataInPipe.id,
            acm->dataInPipe.maxPacketSize);
        if (!req) {
            return DListIsEmpty(head) ? HDF_FAILURE : HDF_SUCCESS;
        }

        req->complete = UsbSerialWriteComplete;
        req->context = port;
        DListInsertTail(&req->list, head);
        port->writeAllocated++;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialStartIo(struct UsbSerial *port)
{
    struct DListHead *head = &port->readPool;
    int32_t          ret = HDF_SUCCESS;
    uint32_t         started;

    /* allocate requests for read/write */
    if (port->readAllocated == 0) {
        ret = UsbSerialAllocReadRequests(port, QUEUE_SIZE);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    if (port->writeAllocated == 0) {
        ret = UsbSerialAllocWriteRequests(port, QUEUE_SIZE);
        if (ret != HDF_SUCCESS) {
            UsbSerialFreeRequests(head, &port->readAllocated);
            return ret;
        }
    }

    started = UsbSerialStartRx(port);
    if (started) {
        UsbSerialStartTx(port);
    } else {
        UsbSerialFreeRequests(head, &port->readAllocated);
        UsbSerialFreeRequests(&port->writePool, &port->writeAllocated);
        ret = HDF_ERR_IO;
    }

    return ret;
}

static int32_t UsbSerialAllocFifo(struct DataFifo *fifo, uint32_t size)
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

static void UsbSerialFreeFifo(struct DataFifo *fifo)
{
    void *buf = fifo->data;
    OsalMemFree(buf);
    DataFifoInit(fifo, 0, NULL);
}

int32_t UsbSerialOpen(struct UsbSerial *port)
{
    int32_t ret;

    if (port == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    g_inFifo = 1;
    OsalMutexLock(&port->lock);
    ret = UsbSerialAllocFifo(&port->writeFifo, WRITE_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAllocFifo failed", __func__);
        goto OUT;
    }
    ret = UsbSerialAllocFifo(&port->readFifo, READ_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAllocFifo failed", __func__);
        goto OUT;
    }

    /* the acm is enabled, start the io stream */
    if (port->acm) {
        if (!port->suspended) {
            struct UsbAcmDevice *acm = port->acm;
            HDF_LOGD("%s: start usb serial", __func__);
            ret = UsbSerialStartIo(port);
            if (ret != HDF_SUCCESS) {
                goto OUT;
            }
            if (acm->notify && acm->notify->Connect) {
                acm->notify->Connect(acm);
            }
        } else {
            HDF_LOGD("%s: delay start usb serial", __func__);
            port->startDelayed = true;
        }
    }

OUT:
    OsalMutexUnlock(&port->lock);
    return HDF_SUCCESS;
}

int32_t UsbSerialClose(struct UsbSerial *port)
{
    struct UsbAcmDevice *acm = NULL;

    if (port == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock(&port->lock);

    HDF_LOGD("%s: close usb serial", __func__);
    acm = port->acm;
    if (acm && !port->suspended) {
        if (acm->notify && acm->notify->Disconnect) {
                acm->notify->Disconnect(acm);
        }
    }
    DataFifoReset(&port->writeFifo);
    DataFifoReset(&port->readFifo);
    port->startDelayed = false;

    OsalMutexUnlock(&port->lock);
    return HDF_SUCCESS;
}

#define WRITE_SPEED_REQ_NUM 8
struct UsbFnRequest  *g_req[WRITE_SPEED_REQ_NUM] = {NULL};
static bool g_isWriteDone = false;
static uint64_t g_writeCnt = 0;
struct timeval g_timeStart, g_timeEnd;
static float g_speed = 0;
static bool isGetWriteTimeStart = false;
static void UsbSerialWriteSpeedComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    int32_t ret;
    switch (req->status) {
        case USB_REQUEST_COMPLETED:
            g_writeCnt += req->actual;
            if (!isGetWriteTimeStart) {
                isGetWriteTimeStart = true;
                gettimeofday(&g_timeStart, NULL);
            }
            if (g_isWriteDone) {
                UsbFnFreeRequest(req);
            } else {
                ret = memset_s(req->buf, req->length, 'a', req->length);
                if (ret != HDF_SUCCESS) {
                    HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
                    return;
                }
                UsbFnSubmitRequestAsync(req);
            }
            break;
        case USB_REQUEST_NO_DEVICE:
            HDF_LOGV("%s: acm device was disconnected", __func__);
            break;
        default:
            HDF_LOGD("%s: req->status = %d", __func__, req->status);
            break;
    }
}

static int32_t SpeedThread(void *arg)
{
    int32_t i;
    int32_t ret;
    g_writeCnt = 0;
    g_isWriteDone = false;
    isGetWriteTimeStart = false;
    g_speed = 0;
    double timeUse;
    double usec = 1000000;
    double k = 1024;
    struct timeval timeTmp;
    struct UsbSerial *port = (struct UsbSerial *)arg;

    for (i = 0; i < WRITE_SPEED_REQ_NUM; i++) {
        g_req[i] = UsbFnAllocRequest(port->acm->dataIface.handle, port->acm->dataInPipe.id,
            port->acm->dataInPipe.maxPacketSize);
        if (g_req[i] == NULL) {
            return HDF_FAILURE;
        }
        g_req[i]->complete = UsbSerialWriteSpeedComplete;
        g_req[i]->context = port;
        g_req[i]->length = port->acm->dataInPipe.maxPacketSize;
        ret = memset_s(g_req[i]->buf, port->acm->dataInPipe.maxPacketSize, 'a', port->acm->dataInPipe.maxPacketSize);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
            return ret;
        }
        UsbFnSubmitRequestAsync(g_req[i]);
    }
    while (!g_isWriteDone) {
        if (g_writeCnt == 0) {
            OsalSleep(1);
            continue;
        } else {
            OsalSleep(1);
        }
        gettimeofday(&timeTmp, NULL);
        timeUse = (double)(timeTmp.tv_sec - g_timeStart.tv_sec) +
        (double)timeTmp.tv_usec / usec - (double)g_timeStart.tv_usec / usec;
        g_speed = (float)((double)g_writeCnt / k / k / timeUse);
    }
    timeUse = (double)(g_timeEnd.tv_sec - g_timeStart.tv_sec) +
        (double)g_timeEnd.tv_usec / usec - (double)g_timeStart.tv_usec / usec;
    HDF_LOGE("timeUse = %lf\n", timeUse);
    g_speed = (float)((double)g_writeCnt / k / k / timeUse);
    HDF_LOGD("%s: g_speed = %f MB/s", __func__, g_speed);
    return HDF_SUCCESS;
}

struct OsalThread     g_thread;
static int32_t StartThreadSpeed(struct UsbSerial *port)
{
    int32_t ret;

    struct OsalThreadParam threadCfg;
    ret = memset_s(&threadCfg, sizeof(threadCfg), 0, sizeof(threadCfg));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
        return ret;
    }
    threadCfg.name = "speed test process";
    threadCfg.priority = OSAL_THREAD_PRI_LOW;
    threadCfg.stackSize = HDF_PROCESS_STACK_SIZE;

    ret = OsalThreadCreate(&g_thread, (OsalThreadEntry)SpeedThread, port);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadCreate faile, ret=%d ", __func__, __LINE__, ret);
        return HDF_ERR_DEVICE_BUSY;
    }

    ret = OsalThreadStart(&g_thread, &threadCfg);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s:%d OsalThreadStart faile, ret=%d ", __func__, __LINE__, ret);
        return HDF_ERR_DEVICE_BUSY;
    }
    return 0;
}

static int32_t UsbSerialGetTempSpeed(struct UsbSerial *port, struct HdfSBuf *reply)
{
    if (!HdfSbufWriteFloat(reply, g_speed)) {
        HDF_LOGE("%s: HdfSbufWriteFloat failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialGetTempSpeedInt(struct UsbSerial *port, struct HdfSBuf *reply)
{
    uint32_t calc = 10000;
    if (!HdfSbufWriteUint32(reply, (uint32_t)(g_speed * calc))) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialSpeedDone(struct UsbSerial *port)
{
    gettimeofday(&g_timeEnd, NULL);
    g_isWriteDone = true;
    return HDF_SUCCESS;
}

static int32_t UsbSerialSpeed(struct UsbSerial *port)
{
    StartThreadSpeed(port);
    return HDF_SUCCESS;
}

static int32_t UsbSerialRead(struct UsbSerial *port, struct HdfSBuf *reply)
{
    uint32_t len, fifoLen;
    int32_t ret = HDF_SUCCESS;
    uint8_t *buf = NULL;
    uint32_t i;
    OsalMutexLock(&port->lock);
    if (DataFifoIsEmpty(&port->readFifo)) {
        OsalMutexUnlock(&port->lock);
        return 0;
    }
    fifoLen = DataFifoLen(&port->readFifo);
    buf = (uint8_t *)OsalMemCalloc(fifoLen + 1);
    if (buf == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        OsalMutexUnlock(&port->lock);
        return HDF_ERR_MALLOC_FAIL;
    }
    for (i = 0; i < fifoLen; i++) {
        len = DataFifoRead(&port->readFifo, buf + i, 1);
        if (len == 0) {
            HDF_LOGE("%s: no data", __func__);
            ret = HDF_ERR_IO;
            goto OUT;
        }
        if (*(buf + i) == 0) {
            if (i == 0) {
                goto OUT;
            }
            break;
        }
    }

    if (!HdfSbufWriteString(reply, (const char *)buf)) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        ret = HDF_ERR_IO;
    }
OUT:
    if (port->acm) {
        UsbSerialStartRx(port);
    }
    OsalMemFree(buf);
    OsalMutexUnlock(&port->lock);
    return ret;
}

static int32_t UsbSerialWrite(struct UsbSerial *port, struct HdfSBuf *data)
{
    uint32_t size;
    const char *tmp = NULL;

    OsalMutexLock(&port->lock);

    tmp = HdfSbufReadString(data);
    if (tmp == NULL) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
    char *buf = OsalMemCalloc(strlen(tmp) + 1);
    if (buf == NULL) {
        HDF_LOGE("%s: OsalMemCalloc failed", __func__);
        return HDF_ERR_IO;
    }

    int32_t ret = strcpy_s(buf, strlen(tmp) + 1, tmp);
    if (ret != EOK) {
        HDF_LOGE("%s: strcpy_s failed", __func__);
        OsalMemFree(buf);
        return HDF_ERR_IO;
    }

    size = DataFifoWrite(&port->writeFifo, (uint8_t *)buf, strlen(buf));

    if (port->acm) {
        UsbSerialStartTx(port);
    }
    OsalMutexUnlock(&port->lock);
    OsalMemFree(buf);
    return size;
}

static int32_t UsbSerialGetBaudrate(struct UsbSerial *port, struct HdfSBuf *reply)
{
    uint32_t baudRate = Le32ToCpu(port->lineCoding.dwDTERate);
    if (!HdfSbufWriteBuffer(reply, &baudRate, sizeof(baudRate))) {
        HDF_LOGE("%s: sbuf write buffer failed", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialSetBaudrate(struct UsbSerial *port, struct HdfSBuf *data)
{
    uint32_t size;
    uint32_t *baudRate = NULL;

    if (!HdfSbufReadBuffer(data, (const void **)&baudRate, &size)) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
    port->lineCoding.dwDTERate = CpuToLe32(*baudRate);
    if (port->acm) {
        port->acm->lineCoding.dwDTERate = CpuToLe32(*baudRate);
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialGetProp(struct UsbAcmDevice *acmDevice, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct UsbFnInterface *intf = acmDevice->ctrlIface.fn;
    const char *propName = NULL;
    char propValue[USB_MAX_PACKET_SIZE] = {0};
    int32_t ret;

    propName = HdfSbufReadString(data);
    if (propName == NULL) {
        return HDF_ERR_IO;
    }
    ret = UsbFnGetInterfaceProp(intf, propName, propValue);
    if (ret) {
        return HDF_ERR_IO;
    }
    if (!HdfSbufWriteString(reply, propValue)) {
        HDF_LOGE("%s:failed to write result", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialSetProp(struct UsbAcmDevice *acmDevice, struct HdfSBuf *data)
{
    struct UsbFnInterface *intf = acmDevice->ctrlIface.fn;
    int32_t ret;
    char tmp[USB_MAX_PACKET_SIZE] = {0};

    const char *propName = HdfSbufReadString(data);
    if (propName == NULL) {
        return HDF_ERR_IO;
    }
    const char *propValue = HdfSbufReadString(data);
    if (propValue == NULL) {
        return HDF_ERR_IO;
    }
    (void)memset_s(&tmp, sizeof(tmp), 0, sizeof(tmp));
    ret = snprintf_s(tmp, USB_MAX_PACKET_SIZE, USB_MAX_PACKET_SIZE - 1, "%s", propValue);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_FAILURE;
    }
    ret = UsbFnSetInterfaceProp(intf, propName, tmp);
    if (ret) {
        HDF_LOGE("%s: UsbFnInterfaceSetProp failed", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t UsbSerialRegistPropAGet(const struct UsbFnInterface *intf, const char *name, const char *value)
{
    (void)intf;
    HDF_LOGE("%s: name = %s", __func__, name);
    HDF_LOGE("%s: value = %s", __func__, value);

    return 0;
}

static int32_t UsbSerialRegistPropASet(const struct UsbFnInterface *intf, const char *name, const char *value)
{
    (void)intf;
    HDF_LOGE("%s: name = %s", __func__, name);
    HDF_LOGE("%s: value = %s", __func__, value);

    return 0;
}

static int32_t UsbSerialRegistProp(struct UsbAcmDevice *acmDevice, struct HdfSBuf *data)
{
    struct UsbFnInterface *intf = acmDevice->ctrlIface.fn;
    struct UsbFnRegistInfo registInfo;
    int32_t ret;

    const char *propName = HdfSbufReadString(data);
    if (propName == NULL) {
        return HDF_ERR_IO;
    }
    const char *propValue = HdfSbufReadString(data);
    if (propValue == NULL) {
        return HDF_ERR_IO;
    }
    registInfo.name = propName;
    registInfo.value = propValue;
    registInfo.getProp = UsbSerialRegistPropAGet;
    registInfo.setProp = UsbSerialRegistPropASet;
    ret = UsbFnRegistInterfaceProp(intf, &registInfo);
    if (ret) {
        HDF_LOGE("%s: UsbFnInterfaceSetProp failed", __func__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t AcmSerialCmd(struct UsbAcmDevice *acm, int32_t cmd, struct UsbSerial *port,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    switch (cmd) {
        case USB_SERIAL_OPEN:
            return UsbSerialOpen(port);
        case USB_SERIAL_CLOSE:
            return UsbSerialClose(port);
        case USB_SERIAL_READ:
            return UsbSerialRead(port, reply);
        case USB_SERIAL_WRITE:
            return UsbSerialWrite(port, data);
        case USB_SERIAL_GET_BAUDRATE:
            return UsbSerialGetBaudrate(port, reply);
        case USB_SERIAL_SET_BAUDRATE:
            return UsbSerialSetBaudrate(port, data);
        case USB_SERIAL_SET_PROP:
            return UsbSerialSetProp(acm, data);
        case USB_SERIAL_GET_PROP:
            return UsbSerialGetProp(acm, data, reply);
        case USB_SERIAL_REGIST_PROP:
            return UsbSerialRegistProp(acm, data);
        case USB_SERIAL_WRITE_SPEED:
            return UsbSerialSpeed(port);
        case USB_SERIAL_WRITE_GET_TEMP_SPEED:
            return UsbSerialGetTempSpeed(port, reply);
        case USB_SERIAL_WRITE_SPEED_DONE:
            return UsbSerialSpeedDone(port);
        case USB_SERIAL_WRITE_GET_TEMP_SPEED_UINT32:
            return UsbSerialGetTempSpeedInt(port, reply);
        case USB_SERIAL_READ_SPEED:
            return UsbSerialReadSpeedStart(port);
        case USB_SERIAL_READ_GET_TEMP_SPEED:
            return UsbSerialGetTempReadSpeed(port, reply);
        case USB_SERIAL_READ_SPEED_DONE:
            return UsbSerialReadSpeedDone(port);
        case USB_SERIAL_READ_GET_TEMP_SPEED_UINT32:
            return UsbSerialGetTempReadSpeedInt(port, reply);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t AcmDeviceDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct UsbAcmDevice *acm = NULL;
    struct UsbSerial *port = NULL;

    if (client == NULL || client->device == NULL || client->device->service == NULL) {
        HDF_LOGE("%s: client is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    acm = (struct UsbAcmDevice *)client->device->service;
    if (acm == NULL) {
        return HDF_ERR_IO;
    }

    if (HdfDeviceObjectCheckInterfaceDesc(client->device, data) == false) {
        HDF_LOGE("%{public}s:%{public}d check interface desc fail", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    
    switch (cmd) {
        case USB_SERIAL_INIT:
            return UsbSerialInit(acm);
        case USB_SERIAL_RELEASE:
            return UsbSerialRelease(acm);
        default:
            HDF_LOGE("%s: unknown cmd %d", __func__, cmd);
            break;
    }
    port = acm->port;
    if (port == NULL) {
        return HDF_ERR_IO;
    }

    return AcmSerialCmd(acm, cmd, port, data, reply);
}

static void AcmDeviceDestroy(struct UsbAcmDevice *acm)
{
    if (acm == NULL) {
        return;
    }
    OsalMemFree(acm);
}

static void AcmCtrlComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    if (req == NULL) {
        return;
    }
    struct CtrlInfo *ctrlInfo = (struct CtrlInfo *)req->context;
    if (ctrlInfo == NULL) {
        return;
    }
    struct UsbAcmDevice *acm = ctrlInfo->acm;
    if (req->status != USB_REQUEST_COMPLETED) {
        HDF_LOGD("%s: ctrl completion error %d", __func__, req->status);
        goto OUT;
    }

    if (ctrlInfo->request == USB_DDK_CDC_REQ_SET_LINE_CODING) {
        struct UsbCdcLineCoding *value = req->buf;
        if (req->actual == sizeof(*value)) {
            acm->lineCoding = *value;
            HDF_LOGD("dwDTERate =  %d", acm->lineCoding.dwDTERate);
            HDF_LOGD("bCharFormat =  %d", acm->lineCoding.bCharFormat);
            HDF_LOGD("bParityType =  %d", acm->lineCoding.bParityType);
            HDF_LOGD("bDataBits =  %d", acm->lineCoding.bDataBits);
        }
    }

OUT:
    DListInsertTail(&req->list, &acm->ctrlPool);
}

static int32_t AcmAllocCtrlRequests(struct UsbAcmDevice *acm, int32_t num)
{
    struct DListHead *head = &acm->ctrlPool;
    struct UsbFnRequest *req = NULL;
    struct CtrlInfo *ctrlInfo = NULL;
    int32_t i;

    DListHeadInit(&acm->ctrlPool);
    acm->ctrlReqNum = 0;

    for (i = 0; i < num; i++) {
        ctrlInfo = (struct CtrlInfo *)OsalMemCalloc(sizeof(*ctrlInfo));
        if (ctrlInfo == NULL) {
            HDF_LOGE("%s: Allocate ctrlInfo failed", __func__);
            goto OUT;
        }
        ctrlInfo->acm = acm;
        req = UsbFnAllocCtrlRequest(acm->ctrlIface.handle,
            sizeof(struct UsbCdcLineCoding) + sizeof(struct UsbCdcLineCoding));
        if (req == NULL) {
            goto OUT;
        }
        req->complete = AcmCtrlComplete;
        req->context  = ctrlInfo;
        DListInsertTail(&req->list, head);
        acm->ctrlReqNum++;
    }
    return HDF_SUCCESS;

OUT:
    return DListIsEmpty(head) ? HDF_FAILURE : HDF_SUCCESS;
}

static void AcmFreeCtrlRequests(struct UsbAcmDevice *acm)
{
    struct DListHead *head = &acm->ctrlPool;
    struct UsbFnRequest *req = NULL;

    while (!DListIsEmpty(head)) {
        req = DLIST_FIRST_ENTRY(head, struct UsbFnRequest, list);
        DListRemove(&req->list);
        OsalMemFree(req->context);
        (void)UsbFnFreeRequest(req);
        acm->ctrlReqNum--;
    }
}

static int32_t AcmNotifySerialState(struct UsbAcmDevice *acm);
static void AcmNotifyComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    struct UsbAcmDevice *acm = (struct UsbAcmDevice *)req->context;
    bool pending = false;

    if (acm == NULL) {
        HDF_LOGE("%s: acm is null", __func__);
        return;
    }

    OsalMutexLock(&acm->lock);
    /* estimate pending */
    if (req->status == PENDING_FLAG) {
        pending = acm->pending;
    }
    acm->notifyReq = req;
    OsalMutexUnlock(&acm->lock);
    if (pending) {
        AcmNotifySerialState(acm);
    }
}

static int32_t AcmAllocNotifyRequest(struct UsbAcmDevice *acm)
{
    /* allocate notification request */
    acm->notifyReq = UsbFnAllocRequest(acm->ctrlIface.handle, acm->notifyPipe.id,
        sizeof(struct UsbCdcNotification) * 2);
    if (acm->notifyReq == NULL) {
        HDF_LOGE("%s: allocate notify request failed", __func__);
        return HDF_FAILURE;
    }
    acm->notifyReq->complete = AcmNotifyComplete;
    acm->notifyReq->context = acm;

    return HDF_SUCCESS;
}

static void AcmFreeNotifyRequest(struct UsbAcmDevice *acm)
{
    int32_t ret;

    /* free notification request */
    ret = UsbFnFreeRequest(acm->notifyReq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: free notify request failed", __func__);
        return;
    }
    acm->notifyReq = NULL;
}

static uint32_t AcmEnable(struct UsbAcmDevice *acm)
{
    int32_t ret;
    struct UsbSerial *port = acm->port;

    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return HDF_FAILURE;
    }

    OsalMutexLock(&port->lock);
    port->acm = acm;
    acm->lineCoding = port->lineCoding;

    ret = UsbSerialAllocFifo(&port->writeFifo, WRITE_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAllocFifo failed", __func__);
        return HDF_FAILURE;
    }
    ret = UsbSerialAllocFifo(&port->readFifo, READ_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAllocFifo failed", __func__);
        return HDF_FAILURE;
    }

    ret = UsbSerialStartIo(port);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialStartIo failed", __func__);
    }
    if (acm->notify && acm->notify->Connect) {
        acm->notify->Connect(acm);
    }

    OsalMutexUnlock(&port->lock);

    return HDF_SUCCESS;
}

static uint32_t AcmDisable(struct UsbAcmDevice *acm)
{
    struct UsbSerial *port = acm->port;

    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return HDF_FAILURE;
    }

    OsalMutexLock(&port->lock);
    port->lineCoding = acm->lineCoding;

    UsbSerialFreeFifo(&port->writeFifo);
    UsbSerialFreeFifo(&port->readFifo);

    OsalMutexUnlock(&port->lock);

    return HDF_SUCCESS;
}

static struct UsbFnRequest *AcmGetCtrlReq(struct UsbAcmDevice *acm)
{
    struct UsbFnRequest *req = NULL;
    struct DListHead *pool = &acm->ctrlPool;

    if (!DListIsEmpty(pool)) {
        req = DLIST_FIRST_ENTRY(pool, struct UsbFnRequest, list);
        DListRemove(&req->list);
    }
    return req;
}

static void AcmSetup(struct UsbAcmDevice *acm, struct UsbFnCtrlRequest *setup)
{
    if (acm == NULL || setup == NULL) {
        return;
    }
    struct UsbFnRequest *req = NULL;
    struct CtrlInfo *ctrlInfo = NULL;
    uint16_t value  = Le16ToCpu(setup->value);
    uint16_t length = Le16ToCpu(setup->length);
    int32_t ret = 0;

    req = AcmGetCtrlReq(acm);
    if (req == NULL) {
        HDF_LOGE("%s: control request pool is empty", __func__);
        return;
    }

    switch (setup->request) {
        case USB_DDK_CDC_REQ_SET_LINE_CODING:
            if (length != sizeof(struct UsbCdcLineCoding)) {
                goto OUT;
            }
            ret = (int)length;
            break;
        case USB_DDK_CDC_REQ_GET_LINE_CODING:
            ret = (int)MIN(length, sizeof(struct UsbCdcLineCoding));
            if (acm->lineCoding.dwDTERate == 0) {
                acm->lineCoding = acm->port->lineCoding;
            }
            if (memcpy_s(req->buf, ret, &acm->lineCoding, ret) != EOK) {
                return;
            }
            break;
        case USB_DDK_CDC_REQ_SET_CONTROL_LINE_STATE:
            ret = 0;
            acm->handshakeBits = value;
            break;
        default:
            HDF_LOGE("%s: setup request is not supported", __func__);
            break;
    }

OUT:
    ctrlInfo = (struct CtrlInfo *)req->context;
    ctrlInfo->request = setup->request;
    req->length = ret;
    ret = UsbFnSubmitRequestAsync(req);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: acm send setup response error", __func__);
    }
}

static void AcmSuspend(struct UsbAcmDevice *acm)
{
    struct UsbSerial *port = acm->port;

    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return;
    }

    OsalMutexLock(&port->lock);
    port->suspended = true;
    OsalMutexUnlock(&port->lock);
}

static void AcmResume(struct UsbAcmDevice *acm)
{
    int32_t ret;
    struct UsbSerial *port = acm->port;
    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return;
    }

    OsalMutexLock(&port->lock);
    port->suspended = false;
    if (!port->startDelayed) {
        OsalMutexUnlock(&port->lock);
        return;
    }
    ret = UsbSerialStartIo(port);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialStartIo failed", __func__);
    }
    if (acm->notify && acm->notify->Connect) {
        acm->notify->Connect(acm);
    }
    port->startDelayed = false;
    OsalMutexUnlock(&port->lock);
}

static void UsbAcmEventCallback(struct UsbFnEvent *event)
{
    struct UsbAcmDevice *acm = NULL;

    if (event == NULL || event->context == NULL) {
        HDF_LOGE("%s: event is null", __func__);
        return;
    }

    acm = (struct UsbAcmDevice *)event->context;
    switch (event->type) {
        case USBFN_STATE_BIND:
            HDF_LOGI("%s: receive bind event", __func__);
            break;
        case USBFN_STATE_UNBIND:
            HDF_LOGI("%s: receive unbind event", __func__);
            break;
        case USBFN_STATE_ENABLE:
            HDF_LOGI("%s: receive enable event", __func__);
            AcmEnable(acm);
            break;
        case USBFN_STATE_DISABLE:
            HDF_LOGI("%s: receive disable event", __func__);
            AcmDisable(acm);
            acm->enableEvtCnt = 0;
            break;
        case USBFN_STATE_SETUP:
            HDF_LOGI("%s: receive setup event", __func__);
            if (event->setup != NULL) {
                AcmSetup(acm, event->setup);
            }
            break;
        case USBFN_STATE_SUSPEND:
            HDF_LOGI("%s: receive suspend event", __func__);
            AcmSuspend(acm);
            break;
        case USBFN_STATE_RESUME:
            HDF_LOGI("%s: receive resume event", __func__);
            AcmResume(acm);
            break;
        default:
            break;
    }
}

static int32_t AcmSendNotifyRequest(struct UsbAcmDevice *acm, uint8_t type,
    uint16_t value, const void *data, uint32_t length)
{
    struct UsbFnRequest *req = acm->notifyReq;
    struct UsbCdcNotification *notify = NULL;
    int32_t ret;

    if (req == NULL || req->buf == NULL) {
        HDF_LOGE("%s: req is null", __func__);
        return HDF_FAILURE;
    }

    acm->notifyReq = NULL;
    acm->pending   = false;
    req->length    = sizeof(*notify) + length;

    notify = (struct UsbCdcNotification *)req->buf;
    notify->bmRequestType = USB_DDK_DIR_IN | USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    notify->bNotificationType = type;
    notify->wValue = CpuToLe16(value);
    notify->wIndex = CpuToLe16(acm->ctrlIface.fn->info.index);
    notify->wLength = CpuToLe16(length);
    ret = memcpy_s((void *)(notify + 1), length, data, length);
    if (ret != EOK) {
        HDF_LOGE("%s: memcpy_s failed", __func__);
        return HDF_FAILURE;
    }

    ret = UsbFnSubmitRequestAsync(req);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: send notify request failed", __func__);
        acm->notifyReq = req;
    }

    return ret;
}

static int32_t AcmNotifySerialState(struct UsbAcmDevice *acm)
{
    int32_t ret = 0;
    uint16_t serialState;

    OsalMutexLock(&acm->lock);
    if (acm->notifyReq) {
        HDF_LOGD("acm serial state %04x\n", acm->serialState);
        serialState = CpuToLe16(acm->serialState);
        ret = AcmSendNotifyRequest(acm, USB_DDK_CDC_NOTIFY_SERIAL_STATE,
            0, &serialState, sizeof(acm->serialState));
    } else {
        acm->pending = true;
    }
    OsalMutexUnlock(&acm->lock);

    return ret;
}

static void AcmConnect(struct UsbAcmDevice *acm)
{
    if (acm == NULL) {
        HDF_LOGE("%s: acm is null", __func__);
        return;
    }
    acm->serialState |= SERIAL_STATE_DSR | SERIAL_STATE_DCD;
    AcmNotifySerialState(acm);
}

static void AcmDisconnect(struct UsbAcmDevice *acm)
{
    if (acm == NULL) {
        HDF_LOGE("%s: acm is null", __func__);
        return;
    }
    acm->serialState &= ~(SERIAL_STATE_DSR | SERIAL_STATE_DCD);
    AcmNotifySerialState(acm);
}

static int32_t AcmSendBreak(struct UsbAcmDevice *acm, int32_t duration)
{
    uint16_t state;

    if (acm == NULL) {
        HDF_LOGE("%s: acm is null", __func__);
        return HDF_FAILURE;
    }

    state = acm->serialState;
    state &= ~SERIAL_STATE_BREAK;
    if (duration)
        state |= SERIAL_STATE_BREAK;

    acm->serialState = state;
    return AcmNotifySerialState(acm);
}

static struct AcmNotifyMethod g_acmNotifyMethod = {
    .Connect    = AcmConnect,
    .Disconnect = AcmDisconnect,
    .SendBreak  = AcmSendBreak,
};

static int32_t AcmParseEachPipe(struct UsbAcmDevice *acm, struct UsbAcmInterface *iface)
{
    struct UsbFnInterface *fnIface = iface->fn;
    int32_t ret;
    uint32_t i;

    for (i = 0; i < fnIface->info.numPipes; i++) {
        struct UsbFnPipeInfo pipeInfo;
        (void)memset_s(&pipeInfo, sizeof(pipeInfo), 0, sizeof(pipeInfo));
        ret = UsbFnGetInterfacePipeInfo(fnIface, i, &pipeInfo);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: get pipe info error", __func__);
            return ret;
        }
        switch (pipeInfo.type) {
            case USB_PIPE_TYPE_INTERRUPT:
                acm->notifyPipe.id = pipeInfo.id;
                acm->notifyPipe.maxPacketSize = pipeInfo.maxPacketSize;
                acm->ctrlIface = *iface;
                break;
            case USB_PIPE_TYPE_BULK:
                if (pipeInfo.dir == USB_PIPE_DIRECTION_IN) {
                    acm->dataInPipe.id = pipeInfo.id;
                    acm->dataInPipe.maxPacketSize = pipeInfo.maxPacketSize;
                    acm->dataIface = *iface;
                } else {
                    acm->dataOutPipe.id = pipeInfo.id;
                    acm->dataOutPipe.maxPacketSize = pipeInfo.maxPacketSize;
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

static int32_t AcmParseAcmIface(struct UsbAcmDevice *acm, struct UsbFnInterface *fnIface)
{
    int32_t ret;
    struct UsbAcmInterface iface;
    UsbFnInterfaceHandle handle = UsbFnOpenInterface(fnIface);
    if (handle == NULL) {
        HDF_LOGE("%s: open interface failed", __func__);
        return HDF_FAILURE;
    }
    iface.fn = fnIface;
    iface.handle = handle;

    ret = AcmParseEachPipe(acm, &iface);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t AcmParseEachIface(struct UsbAcmDevice *acm, struct UsbFnDevice *fnDev)
{
    struct UsbFnInterface *fnIface = NULL;
    uint32_t i;
    if (fnDev == NULL) {
        return HDF_FAILURE;
    }
    for (i = 0; i < fnDev->numInterfaces; i++) {
        fnIface = (struct UsbFnInterface *)UsbFnGetInterface(fnDev, i);
        if (fnIface == NULL) {
            HDF_LOGE("%s: get interface failed", __func__);
            return HDF_FAILURE;
        }

        if (fnIface->info.subclass == USB_DDK_CDC_SUBCLASS_ACM) {
            (void)AcmParseAcmIface(acm, fnIface);
            fnIface = (struct UsbFnInterface *)UsbFnGetInterface(fnDev, i + 1);
            if (fnIface == NULL) {
                HDF_LOGE("%s: get interface failed", __func__);
                return HDF_FAILURE;
            }
            (void)AcmParseAcmIface(acm, fnIface);
            return HDF_SUCCESS;
        }
    }
    return HDF_FAILURE;
}

static int32_t AcmCreateFuncDevice(struct UsbAcmDevice *acm,
                                   struct DeviceResourceIface *iface)
{
    int32_t ret;
    struct UsbFnDevice *fnDev = NULL;

    if (iface->GetString(acm->device->property, "udc_name",
        (const char **)&acm->udcName, UDC_NAME) != HDF_SUCCESS) {
        HDF_LOGE("%s: read udc_name failed, use default", __func__);
    }

    fnDev = (struct UsbFnDevice *)UsbFnGetDevice(acm->udcName);
    if (fnDev == NULL) {
        HDF_LOGE("%s: create usb function device failed", __func__);
        return HDF_FAILURE;
    }

    ret = AcmParseEachIface(acm, fnDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get pipes failed", __func__);
        return HDF_FAILURE;
    }

    acm->fnDev = fnDev;
    return HDF_SUCCESS;
}

static int32_t AcmReleaseFuncDevice(struct UsbAcmDevice *acm)
{
    int32_t ret = HDF_SUCCESS;
    if (acm->fnDev == NULL) {
        HDF_LOGE("%s: fnDev is null", __func__);
        return HDF_FAILURE;
    }
    AcmFreeCtrlRequests(acm);
    AcmFreeNotifyRequest(acm);
    (void)UsbFnCloseInterface(acm->ctrlIface.handle);
    (void)UsbFnCloseInterface(acm->dataIface.handle);
    (void)UsbFnStopRecvInterfaceEvent(acm->ctrlIface.fn);
    return ret;
}

static int32_t UsbSerialAlloc(struct UsbAcmDevice *acm)
{
    struct UsbSerial *port = NULL;

    port = (struct UsbSerial *)OsalMemCalloc(sizeof(*port));
    if (port == NULL) {
        HDF_LOGE("%s: Alloc usb serial port failed", __func__);
        return HDF_FAILURE;
    }

    if (OsalMutexInit(&port->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        return HDF_FAILURE;
    }

    DListHeadInit(&port->readPool);
    DListHeadInit(&port->readQueue);
    DListHeadInit(&port->writePool);

    port->lineCoding.dwDTERate = CpuToLe32(PORT_RATE);
    port->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    port->lineCoding.bParityType = USB_CDC_NO_PARITY;
    port->lineCoding.bDataBits = DATA_BIT;

    acm->port = port;
    return HDF_SUCCESS;
}

static void UsbSerialFree(struct UsbAcmDevice *acm)
{
    struct UsbSerial *port = acm->port;

    if (port == NULL) {
        HDF_LOGE("%s: port is null", __func__);
        return;
    }
    OsalMemFree(port);
}

/* HdfDriverEntry implementations */
static int32_t AcmDriverBind(struct HdfDeviceObject *device)
{
    struct UsbAcmDevice *acm = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    acm = (struct UsbAcmDevice *)OsalMemCalloc(sizeof(*acm));
    if (acm == NULL) {
        HDF_LOGE("%s: Alloc usb acm device failed", __func__);
        return HDF_FAILURE;
    }

    if (OsalMutexInit(&acm->lock) != HDF_SUCCESS) {
        HDF_LOGE("%s: init lock fail!", __func__);
        OsalMemFree(acm);
        return HDF_FAILURE;
    }

    if (HdfDeviceObjectSetInterfaceDesc(device, "hdf.usb.usbfn") != HDF_SUCCESS) {
        HDF_LOGE(" Set Desc fail!");
        OsalMemFree(acm);
        return HDF_FAILURE;
    }

    acm->device  = device;
    device->service = &(acm->service);
    acm->device->service->Dispatch = AcmDeviceDispatch;
    acm->notify = NULL;
    acm->initFlag = false;
    return HDF_SUCCESS;
}

static int32_t UsbSerialInit(struct UsbAcmDevice *acm)
{
    struct DeviceResourceIface *iface = NULL;
    int32_t ret;

    if (acm == NULL || acm->initFlag) {
        HDF_LOGE("%s: acm is null", __func__);
        return HDF_FAILURE;
    }
    iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }

    ret = AcmCreateFuncDevice(acm, iface);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcmCreateFuncDevice failed", __func__);
        return HDF_FAILURE;
    }

    ret = UsbSerialAlloc(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAlloc failed", __func__);
        goto ERR;
    }

    ret = AcmAllocCtrlRequests(acm, CTRL_REQUEST_NUM);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcmAllocCtrlRequests failed", __func__);
        goto ERR;
    }

    ret = AcmAllocNotifyRequest(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcmAllocNotifyRequest failed", __func__);
        goto ERR;
    }

    ret = UsbFnStartRecvInterfaceEvent(acm->ctrlIface.fn, 0xff, UsbAcmEventCallback, acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: register event callback failed", __func__);
        goto ERR;
    }

    acm->notify = &g_acmNotifyMethod;
    acm->initFlag = true;
    return HDF_SUCCESS;

ERR:
    UsbSerialFree(acm);
    (void)AcmReleaseFuncDevice(acm);
    return ret;
}

static int32_t UsbSerialRelease(struct UsbAcmDevice *acm)
{
    if (acm == NULL || acm->initFlag == false) {
        HDF_LOGE("%s: acm is null", __func__);
        return HDF_FAILURE;
    }
    (void)AcmReleaseFuncDevice(acm);
    UsbSerialFree(acm);
    acm->initFlag = false;
    return 0;
}

static int32_t AcmDriverInit(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: do nothing...", __func__);
    return 0;
}

static void AcmDriverRelease(struct HdfDeviceObject *device)
{
    struct UsbAcmDevice *acm = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }

    acm = (struct UsbAcmDevice *)device->service;
    if (acm == NULL) {
        HDF_LOGE("%s: acm is null", __func__);
        return;
    }
    (void)OsalMutexDestroy(&acm->lock);
    AcmDeviceDestroy(acm);
}

struct HdfDriverEntry g_acmDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usbfn_cdcacm",
    .Bind          = AcmDriverBind,
    .Init          = AcmDriverInit,
    .Release       = AcmDriverRelease,
};

HDF_INIT(g_acmDriverEntry);
