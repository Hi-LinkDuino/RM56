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

#include "usb_serial.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_usb_pnp_manage.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usb_ddk_interface.h"

#define HDF_LOG_TAG USB_HOST_ACM
#define STR_LEN     512

static struct UsbRequest *g_syncRequest = NULL;
static struct UsbRequest *g_ctrlCmdRequest = NULL;
static bool g_acmReleaseFlag = false;
static uint8_t *g_acmReadBuffer = NULL;

static int32_t SerialCtrlMsg(struct AcmDevice *acm, uint8_t request,
    uint16_t value, void *buf, uint16_t len);
static void AcmWriteBulk(struct UsbRequest *req);
static int32_t AcmInit(struct AcmDevice *acm);
static void AcmRelease(struct AcmDevice *acm);
static struct UsbInterface *GetUsbInterfaceById(const struct AcmDevice *acm,
    uint8_t interfaceIndex);

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
        OsalMemFree(fifo->data);
    }

    DataFifoInit((struct DataFifo *)fifo, 0, NULL);
}

static int32_t AcmWbIsAvail(const struct AcmDevice *acm)
{
    int32_t i, n;
    n = ACM_NW;
    OsalMutexLock((struct OsalMutex *)&acm->writeLock);
    for (i = 0; i < ACM_NW; i++)
        n -= acm->wb[i].use;
    OsalMutexUnlock((struct OsalMutex *)&acm->writeLock);
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

static int32_t AcmStartWb(struct AcmDevice *acm,
    struct AcmWb *wb, struct UsbPipeInfo *pipe)
{
    int32_t rc;
    struct UsbRequestParams parmas = {};
    acm->transmitting++;
    parmas.interfaceId = acm->dataOutPipe->interfaceId;
    parmas.pipeAddress = acm->dataOutPipe->pipeAddress;
    parmas.pipeId = acm->dataOutPipe->pipeId;
    parmas.callback = AcmWriteBulk;
    parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.dataReq.numIsoPackets = 0;
    parmas.userData = (void *)wb;
    parmas.dataReq.length = wb->len;
    parmas.dataReq.buffer = wb->buf;
    rc = UsbFillRequest(wb->request, InterfaceIdToHandle(acm, acm->dataOutPipe->interfaceId), &parmas);
    if (HDF_SUCCESS != rc) {
        HDF_LOGE("%s:UsbFillRequest faile,ret=%d \n", __func__, rc);
        return rc;
    }
    acm->writeReq = wb->request;
    rc = UsbSubmitRequestAsync(wb->request);
    if (rc < 0) {
        HDF_LOGE("UsbSubmitRequestAsync faile, ret=%d \n", rc);
        wb->use = 0;
        acm->transmitting--;
    }
    return rc;
}

static int32_t AcmStartWbSync(struct AcmDevice *acm,
    struct AcmWb *wb, struct UsbPipeInfo *pipe)
{
    int32_t rc;
    struct UsbRequestParams parmas = {};
    parmas.interfaceId = acm->dataOutPipe->interfaceId;
    parmas.pipeAddress = acm->dataOutPipe->pipeAddress;
    parmas.pipeId = acm->dataOutPipe->pipeId;
    parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.dataReq.numIsoPackets = 0;
    parmas.userData = (void *)wb;
    parmas.dataReq.length = wb->len;
    parmas.dataReq.buffer = wb->buf;
    parmas.callback = NULL;
    rc = UsbFillRequest(wb->request, InterfaceIdToHandle(acm, acm->dataOutPipe->interfaceId), &parmas);
    if (HDF_SUCCESS != rc) {
        HDF_LOGE("%s:UsbFillRequest faile,ret=%d \n", __func__, rc);
        return rc;
    }
    acm->writeReq = wb->request;
    rc = UsbSubmitRequestSync(wb->request);
    if (rc < 0) {
        HDF_LOGE("UsbSubmitRequestSync faile, ret=%d \n", rc);
    }
    wb->use = 0;
    return rc;
}

static int32_t AcmWriteBufAlloc(const struct AcmDevice *acm)
{
    int32_t i;
    struct AcmWb *wb;
    for (wb = (struct AcmWb *)&acm->wb[0], i = 0; i < ACM_NW; i++, wb++) {
        wb->buf = OsalMemCalloc(acm->writeSize);
        if (!wb->buf) {
            while (i != 0) {
                --i;
                --wb;
                OsalMemFree(wb->buf);
                wb->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        }
    }
    return 0;
}

static void AcmWriteBufFree(struct AcmDevice *acm)
{
    int32_t i;
    struct AcmWb *wb;
    for (wb = &acm->wb[0], i = 0; i < ACM_NW; i++, wb++) {
        if (wb->buf) {
            OsalMemFree(wb->buf);
            wb->buf = NULL;
        }
    }
    return;
}

static void AcmWriteBulk(struct UsbRequest *req)
{
    if (req == NULL) {
        HDF_LOGE("%s:%{pulib}d req is NULL!", __func__, __LINE__);
        goto EXIT;
    }
    int32_t status = req->compInfo.status;
    struct AcmWb *wb  = (struct AcmWb *)req->compInfo.userData;
    switch (status) {
        case 0:
            if (wb)
                wb->use = 0;
            break;
        case -ECONNRESET:
        case -ENOENT:
        case -ESHUTDOWN:
            return;
        default:
            goto EXIT;
    }
EXIT:
    return;
}

static struct UsbControlRequest UsbControlSetUp(struct UsbControlParams *controlParams)
{
    struct UsbControlRequest dr;
    dr.target = controlParams->target;
    dr.reqType = controlParams->reqType;
    dr.directon = controlParams->directon;
    dr.request = controlParams->request;
    dr.value = CpuToLe16(controlParams->value);
    dr.index = CpuToLe16(controlParams->index);
    dr.buffer = controlParams->data;
    dr.length = CpuToLe16(controlParams->size);
    return dr;
}

static int32_t UsbGetDescriptor(struct UsbDescriptorParams *descParams)
{
    int32_t ret;
    struct UsbControlParams controlParams = {};
    struct UsbRequestParams parmas = {};
    const int32_t offset = 8;

    if ((descParams == NULL) || (descParams->devHandle == NULL) ||
        (descParams->request == NULL) || (descParams->buf == NULL)) {
        HDF_LOGE("%s:null pointer failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    controlParams.request = USB_DDK_REQ_GET_DESCRIPTOR;
    controlParams.target = USB_REQUEST_TARGET_DEVICE;
    controlParams.reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams.directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams.value = (((uint32_t)(descParams->type)) << offset) + descParams->index;
    controlParams.index = 0;
    controlParams.data = descParams->buf;
    controlParams.size = descParams->size;

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.ctrlReq = UsbControlSetUp(&controlParams);
    parmas.callback = NULL;
    ret = UsbFillRequest(descParams->request, descParams->devHandle, &parmas);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: faile, ret=%d ", __func__, ret);
        return ret;
    }
    ret = UsbSubmitRequestSync(descParams->request);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("UsbSubmitRequestSync  faile, ret=%d ", ret);
        return ret;
    }
    ret = memcpy_s(descParams->buf, descParams->size, descParams->request->compInfo.buffer,
        descParams->request->compInfo.actualLength);
    if (EOK != ret) {
        HDF_LOGE("memcpy_s fail ret=%d", ret);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t GetDeviceDescriptor(UsbInterfaceHandle *devHandle, struct UsbRequest *request, void *buf, uint16_t size)
{
    struct UsbDescriptorParams descParams = {};
    descParams.devHandle = devHandle;
    descParams.request = request;
    descParams.type = USB_DDK_DT_DEVICE;
    descParams.index = 0;
    descParams.buf = buf;
    descParams.size = size;
    return UsbGetDescriptor(&descParams);
}

static int32_t UsbGetStatus(UsbInterfaceHandle *devHandle,
    struct UsbRequest *request, uint16_t *status)
{
    int32_t ret;
    uint16_t ss;
    struct UsbControlParams controlParams = {};
    struct UsbRequestParams parmas = {};
    if (NULL == devHandle || NULL == request) {
        HDF_LOGE("%s:null pointer failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    controlParams.request = USB_DDK_REQ_GET_STATUS;
    controlParams.target = USB_REQUEST_TARGET_DEVICE;
    controlParams.reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams.directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams.value = 0;
    controlParams.index = 0;
    controlParams.data = (void *)(&ss);
    controlParams.size = sizeof(ss);

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.ctrlReq = UsbControlSetUp(&controlParams);
    parmas.callback = NULL;
    ret = UsbFillRequest(request, devHandle, &parmas);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: faile, ret=%d ", __func__, ret);
        return ret;
    }
    ret = UsbSubmitRequestSync(request);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("UsbSubmitRequestSync  faile, ret=%d ", ret);
        return ret;
    }
    if (request->compInfo.buffer) {
        ret = memcpy_s((void *)(&ss), sizeof(ss), request->compInfo.buffer, request->compInfo.actualLength);
        if (EOK != ret) {
            HDF_LOGE("memcpy_s fail ret=%d", ret);
            return ret;
        }
    }
    *status = Le16ToCpu(ss);
    return HDF_SUCCESS;
}

static int32_t UsbGetInterface(const UsbInterfaceHandle *devHandle,
    const struct UsbRequest *request, const uint8_t *buf)
{
    int32_t ret;
    struct UsbControlParams controlParams = {};
    struct UsbRequestParams parmas = {};
    if (NULL == devHandle || NULL == request) {
        HDF_LOGE("%s:null pointer failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    controlParams.request = USB_DDK_REQ_GET_INTERFACE;
    controlParams.target = USB_REQUEST_TARGET_INTERFACE;
    controlParams.reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams.directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams.value = 0;
    controlParams.index = 0;
    controlParams.data = (void *)buf;
    controlParams.size = 1;

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.ctrlReq = UsbControlSetUp(&controlParams);
    parmas.callback = NULL;
    ret = UsbFillRequest((struct UsbRequest *)request, (UsbInterfaceHandle *)devHandle, &parmas);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: faile, ret=%d ", __func__, ret);
        return ret;
    }
    ret = UsbSubmitRequestSync((struct UsbRequest *)request);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("UsbSubmitRequestSync  faile, ret=%d ", ret);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t UsbGetConfig(const UsbInterfaceHandle *devHandle,
    const struct UsbRequest *request, const uint8_t *buf)
{
    int32_t ret;
    struct UsbControlParams controlParams = {};
    struct UsbRequestParams parmas = {};
    if (NULL == devHandle || NULL == request) {
        HDF_LOGE("%s:null pointer failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    controlParams.request = USB_DDK_REQ_GET_CONFIGURATION;
    controlParams.target = USB_REQUEST_TARGET_DEVICE;
    controlParams.reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams.directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams.value = 0;
    controlParams.index = 0;
    controlParams.data = (void *)buf;
    controlParams.size = 1;

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.ctrlReq = UsbControlSetUp(&controlParams);
    parmas.callback = NULL;
    ret = UsbFillRequest((struct UsbRequest *)request, (UsbInterfaceHandle *)devHandle, &parmas);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: faile, ret=%d ", __func__, ret);
        return ret;
    }
    ret = UsbSubmitRequestSync((struct UsbRequest *)request);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("UsbSubmitRequestSync  faile, ret=%d ", ret);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t SerialCtrlMsg(struct AcmDevice *acm, uint8_t request,
    uint16_t value, void *buf, uint16_t len)
{
    int32_t ret;
    if (acm == NULL || buf == NULL || acm->intPipe == NULL) {
        HDF_LOGE("%s:invalid param", __func__);
        return HDF_ERR_IO;
    }
    uint16_t index = acm->intPipe->interfaceId;
    struct UsbControlParams controlParams = {};
    struct UsbRequestParams parmas = {};
    if (acm->ctrlReq == NULL) {
        acm->ctrlReq = UsbAllocRequest(acm->ctrDevHandle, 0, len);
        if (acm->ctrlReq == NULL) {
            HDF_LOGE("%s: UsbAllocRequest failed", __func__);
            return HDF_ERR_IO;
        }
    }

    controlParams.request = request;
    controlParams.target = USB_REQUEST_TARGET_INTERFACE;
    controlParams.reqType = USB_REQUEST_TYPE_CLASS;
    controlParams.directon = USB_REQUEST_DIR_TO_DEVICE;
    controlParams.value = value;
    controlParams.index = index;
    controlParams.data = buf;
    controlParams.size = len;

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    if (acm->ctrPipe != NULL) {
        parmas.pipeAddress = acm->ctrPipe->pipeAddress;
        parmas.pipeId = acm->ctrPipe->pipeId;
    }
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.ctrlReq = UsbControlSetUp(&controlParams);
    parmas.callback = NULL;
    ret = UsbFillRequest(acm->ctrlReq, acm->ctrDevHandle, &parmas);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: faile, ret=%d ", __func__, ret);
        return ret;
    }
    ret = UsbSubmitRequestSync(acm->ctrlReq);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("UsbSubmitRequestSync  faile, ret=%d ", ret);
        return ret;
    }
    if (!acm->ctrlReq->compInfo.status) {
        HDF_LOGE("%s  status=%d ", __func__, acm->ctrlReq->compInfo.status);
    }
    return HDF_SUCCESS;
}

static int32_t SerialCtrlAsyncMsg(UsbInterfaceHandle *devHandle,
    struct UsbRequest *request, void *buf, uint16_t size)
{
    const int32_t offset = 8;
    struct UsbControlParams controlParams = {};
    struct UsbRequestParams parmas = {};
    if (NULL == devHandle || NULL == request || NULL == buf) {
        HDF_LOGE("%s:null pointer failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    controlParams.request = USB_DDK_REQ_GET_DESCRIPTOR;
    controlParams.target = USB_REQUEST_TARGET_DEVICE;
    controlParams.reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams.directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams.value = (((uint8_t)USB_DDK_DT_DEVICE) << offset);
    controlParams.index = 0;
    controlParams.data = buf;
    controlParams.size = size;

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.ctrlReq = UsbControlSetUp(&controlParams);
    int32_t ret = UsbFillRequest(request, devHandle, &parmas);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: faile, ret=%d ", __func__, ret);
        return ret;
    }
    ret = UsbSubmitRequestAsync(request);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("UsbRequestSubmitAsync  faile, ret=%d ", ret);
        return ret;
    }
    OsalMSleep(500);
    HDF_LOGE("SerialCtrlAsyncMsg  length%d ", request->compInfo.actualLength);
    for (unsigned int i = 0; i < request->compInfo.actualLength; i++) {
        HDF_LOGE("0x%02x", ((uint8_t *)(request->compInfo.buffer))[i]);
    }
    ret = memcpy_s(buf, size, request->compInfo.buffer, request->compInfo.actualLength);
    if (ret != EOK) {
        HDF_LOGE("memcpy_s fail\n");
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
    port->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    port->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    port->lineCoding.bParityType = USB_CDC_NO_PARITY;
    port->lineCoding.bDataBits = DATA_BITS_LENGTH;
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
    uint32_t len;
    int32_t ret = HDF_SUCCESS;
    struct AcmDevice *acm = port->acm;

    for (int32_t i = 0; i < ACM_NR; i++) {
        if(acm->readReq[i]->compInfo.status != USB_REQUEST_COMPLETED) {
            HDF_LOGE("%s:%d i=%d status=%d!",
                __func__, __LINE__, i, acm->readReq[i]->compInfo.status);
            return HDF_FAILURE;
        }
    }

    OsalMutexLock(&acm->readLock);

    if (g_acmReadBuffer == NULL) {
        OsalMutexUnlock(&acm->readLock);
        HDF_LOGE("%s:%d g_acmReadBuffer is NULL", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = memset_s(g_acmReadBuffer, READ_BUF_SIZE, 0, READ_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d memset_s failed", __func__, __LINE__);
        return ret;
    }

    if (DataFifoIsEmpty(&port->readFifo)) {
        ret = HDF_SUCCESS;
        goto OUT;
    }

    len = DataFifoRead(&port->readFifo, g_acmReadBuffer, DataFifoLen(&port->readFifo));
    if (len == 0) {
        HDF_LOGE("%s:%d no data", __func__, __LINE__);
        ret = HDF_SUCCESS;
        goto OUT;
    }
OUT:
    if (!HdfSbufWriteString(reply, (const char *)g_acmReadBuffer)) {
        HDF_LOGE("%s:%d sbuf write buffer failed", __func__, __LINE__);
        ret = HDF_ERR_IO;
    }

    OsalMutexUnlock(&acm->readLock);
    return ret;
}

static int32_t SerialSetBaudrate(struct SerialDevice *port, const struct HdfSBuf *data)
{
    int32_t ret;
    struct AcmDevice *acm = port->acm;
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
        ret = SerialCtrlMsg(acm, USB_DDK_CDC_REQ_SET_LINE_CODING, 0,
            &acm->lineCoding, sizeof (struct UsbCdcLineCoding));
        if (ret) {
            HDF_LOGE("SerialCtrlMsg fail\n");
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static int32_t UsbCtrlMsg(struct SerialDevice *port, struct HdfSBuf *data)
{
    int32_t ret;
    struct AcmDevice *acm = port->acm;
    struct UsbCdcLineCoding lineCoding = {
        .dwDTERate = CpuToLe32(DATARATE),
        .bCharFormat = USB_CDC_1_STOP_BITS,
        .bParityType = USB_CDC_NO_PARITY,
        .bDataBits = DATA_BITS_LENGTH,
    };
    ret = SerialCtrlMsg(acm, USB_DDK_CDC_REQ_SET_LINE_CODING, 0,
        &lineCoding, sizeof (struct UsbCdcLineCoding));
    if (ret) {
        HDF_LOGE("SerialCtrlMsg fail\n");
        return ret;
    }
    return ret;
}

static int32_t SerialGetBaudrate(struct SerialDevice *port, struct HdfSBuf *reply)
{
    uint32_t baudRate = Le32ToCpu(port->lineCoding.dwDTERate);
    if (!HdfSbufWriteUint32(reply, baudRate)) {
        HDF_LOGE("%s:%d sbuf write buffer failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    HDF_LOGE("%s:%d baudRate=%u", __func__, __LINE__, baudRate);
    return HDF_SUCCESS;
}

static int32_t UsbSerialReadSync(const struct SerialDevice *port, const struct HdfSBuf *reply)
{
    struct AcmDevice *acm = port->acm;
    uint8_t *data = NULL;
    struct UsbRequestParams readParmas = {};
    if (g_syncRequest == NULL) {
        g_syncRequest = UsbAllocRequest(InterfaceIdToHandle(acm, acm->dataInPipe->interfaceId), 0, acm->readSize);
        if (g_syncRequest == NULL) {
            return HDF_ERR_MALLOC_FAIL;
        }
    }
    readParmas.pipeAddress = acm->dataInPipe->pipeAddress;
    readParmas.pipeId = acm->dataInPipe->pipeId;
    readParmas.interfaceId = acm->dataInPipe->interfaceId;
    readParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    readParmas.timeout = USB_CTRL_SET_TIMEOUT;
    readParmas.dataReq.numIsoPackets = 0;
    readParmas.dataReq.directon = (((uint8_t)acm->dataInPipe->pipeDirection) >> USB_DIR_OFFSET) & DIRECTION_MASK;
    readParmas.dataReq.length = (int)acm->readSize;
    readParmas.callback = NULL;
    int32_t ret = UsbFillRequest(g_syncRequest, InterfaceIdToHandle(acm, acm->dataInPipe->interfaceId), &readParmas);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = UsbSubmitRequestSync(g_syncRequest);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    uint32_t count = g_syncRequest->compInfo.actualLength;
    data = (uint8_t *)OsalMemCalloc(count + 1);
    if ((data == NULL) || (!g_syncRequest)) {
        return HDF_FAILURE;
    }

    ret = memcpy_s(data, g_syncRequest->compInfo.actualLength, g_syncRequest->compInfo.buffer, count);
    if (ret != EOK) {
        HDF_LOGE("memcpy_s error %s, %d", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString((struct HdfSBuf *)reply, (const char *)data)) {
        HDF_LOGE("%s:%d sbuf write buffer failed", __func__, __LINE__);
    }

    OsalMemFree(data);
    data = NULL;

    return HDF_SUCCESS;
}

static int32_t UsbStdCtrlCmd(struct SerialDevice *port, SerialOPCmd cmd, struct HdfSBuf *reply)
{
    int32_t ret;
    static uint16_t ss;
    static uint8_t data;
    static uint8_t id;
    char str[STR_LEN] = {};
    static struct UsbDeviceDescriptor des = {};
    struct AcmDevice *acm = port->acm;
    struct UsbRequestParams parmas = {};
    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = 0;
    parmas.pipeId = 0;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    if (g_ctrlCmdRequest == NULL) {
        g_ctrlCmdRequest = UsbAllocRequest(acm->ctrDevHandle, 0, acm->readSize);
        if (g_ctrlCmdRequest == NULL) {
            HDF_LOGE("ctrlRequest request failed");
            return HDF_ERR_MALLOC_FAIL;
        }
    }
    switch (cmd) {
        case CMD_STD_CTRL_GET_DESCRIPTOR_CMD:
            ret = GetDeviceDescriptor(acm->ctrDevHandle, g_ctrlCmdRequest, (void *)(&des), sizeof(des));
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("GetDeviceDescriptor fail ret:%d", ret);
                return HDF_FAILURE;
            }
            (void)snprintf_s(str, STR_LEN, STR_LEN - 1,
                "device descriptor info:[0x%04x 0x%04x 0x%02x 0x%02x 0x%02x]\n",
                des.idVendor, des.idProduct, des.bDeviceClass, des.bDeviceSubClass, des.bDeviceProtocol);
            if (!HdfSbufWriteString(reply, (const char *)str)) {
                HDF_LOGE("%s: sbuf write buffer failed", __func__);
                return HDF_FAILURE;
            }
            for (unsigned int i = 0; i < sizeof(des); i++) {
                HDF_LOGD("0x%02x", ((uint8_t *)(&des))[i]);
            }
            break;
        case CMD_STD_CTRL_GET_DESCRIPTOR_ASYNC:
             ret = SerialCtrlAsyncMsg(acm->ctrDevHandle, g_ctrlCmdRequest, (void *)(&des), sizeof(des));
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("GetDeviceDescriptor async fail ret:%d", ret);
                return HDF_FAILURE;
            }
            (void)snprintf_s(str, STR_LEN, STR_LEN - 1,
                "device descriptor info:[0x%04x 0x%04x 0x%02x 0x%02x 0x%02x]\n",
                des.idVendor, des.idProduct, des.bDeviceClass, des.bDeviceSubClass, des.bDeviceProtocol);
            if (!HdfSbufWriteString(reply, (const char *)str)) {
                HDF_LOGE("%s: sbuf write buffer failed", __func__);
                return HDF_FAILURE;
             }
            for (unsigned int i = 0; i < sizeof(des); i++)
                    HDF_LOGE("0x%02x", ((uint8_t *)(&des))[i]);
            break;
        case CMD_STD_CTRL_GET_STATUS_CMD:
            ret = UsbGetStatus(acm->ctrDevHandle, g_ctrlCmdRequest, &ss);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("UsbGetStatus fail ret:%d", ret);
                return HDF_FAILURE;
            }
            ret = HdfSbufWriteUint16(reply, ss);
            break;
        case CMD_STD_CTRL_GET_CONFIGURATION:
            ret = UsbGetConfig(acm->ctrDevHandle, g_ctrlCmdRequest, &data);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("UsbGetStatus fail ret:%d", ret);
                return HDF_FAILURE;
            }
            ret = HdfSbufWriteUint8(reply, data);
            break;
        case CMD_STD_CTRL_GET_INTERFACE:
            ret = UsbGetInterface(acm->ctrDevHandle, g_ctrlCmdRequest, &id);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("UsbGetStatus fail ret:%d", ret);
                return HDF_FAILURE;
            }
            ret = HdfSbufWriteUint8(reply, id);
            break;
        default:
            ret = -1;
            break;
    }
    if (!ret) {
        HDF_LOGE("cmd:%d ret:%d", cmd, ret);
    }
    return ret;
}

static int32_t SerialWriteSync(const struct SerialDevice *port, const struct HdfSBuf *data)
{
    uint32_t size;
    int32_t ret;
    const char *tmp = NULL;
    int32_t wbn;
    struct AcmWb *wb = NULL;
    if (port == NULL || data == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct AcmDevice *acm = port->acm;
    if (acm == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (AcmWbIsAvail(acm)) {
        wbn = AcmWbAlloc(acm);
    } else {
        HDF_LOGE("no write buf\n");
        return 0;
    }
    if (wbn >= ACM_NW || wbn < 0) {
        wbn = 0;
    }
    wb = &acm->wb[wbn];
    if (wb == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    tmp = HdfSbufReadString((struct HdfSBuf *)data);
    if (tmp == NULL) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
    size = strlen(tmp) + 1;
    size = (size > acm->writeSize) ? acm->writeSize : size;
    ret = memcpy_s(wb->buf, acm->writeSize, tmp, size);
    if (ret != EOK) {
        HDF_LOGE("memcpy_s fail, ret=%d", ret);
    }
    wb->len = (int)size;
    if (acm->dataOutPipe == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = AcmStartWbSync(acm, wb, acm->dataOutPipe);
    return size;
}

static int32_t SerialOpen(const struct SerialDevice *port, struct HdfSBuf *data)
{
    int32_t ret;
    int32_t cmdType = HOST_ACM_ASYNC_READ;

    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct AcmDevice *acm = port->acm;
    if (acm == NULL) {
        HDF_LOGE("%s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &cmdType)) {
        HDF_LOGE("%s:%d sbuf read cmdType failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = AcmInit(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d AcmInit failed", __func__, __LINE__);
        return  HDF_FAILURE;
    }

    if (cmdType != HOST_ACM_ASYNC_READ) {
        HDF_LOGD("%s:%d asyncRead success", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    if (g_acmReadBuffer == NULL) {
        g_acmReadBuffer = (uint8_t *)OsalMemCalloc(READ_BUF_SIZE);
        if (g_acmReadBuffer == NULL) {
            HDF_LOGE("%s:%d OsalMemCalloc g_acmReadBuffer error", __func__, __LINE__);
            return HDF_ERR_MALLOC_FAIL;
        }
    }

    ret = UsbSerialAllocFifo((struct DataFifo *)&port->readFifo, READ_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAllocFifo failed", __func__);
        return  HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < ACM_NR; i++) {
        ret = UsbSubmitRequestAsync(acm->readReq[i]);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("UsbSubmitRequestAsync  faile, ret=%d ", ret);
            goto ERR;
        }
    }
    return HDF_SUCCESS;
ERR:
    OsalMemFree(g_acmReadBuffer);
    g_acmReadBuffer = NULL;
    UsbSerialFreeFifo((struct DataFifo *)&port->readFifo);
    return ret;
}

static int32_t SerialClose(const struct SerialDevice *port, struct HdfSBuf *data)
{
    int32_t cmdType = HOST_ACM_SYNC_READ;
    struct AcmDevice *acm = NULL;

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

    if ((cmdType == HOST_ACM_SYNC_READ) || (cmdType == HOST_ACM_SYNC_WRITE)
        || (cmdType == HOST_ACM_ASYNC_WRITE) || (cmdType == HOST_ACM_ADD_INTERFACE)
        || (cmdType == HOST_ACM_REMOVE_INTERFACE)) {
        HDF_LOGD("%s:%d cmdType=%d success", __func__, __LINE__, cmdType);
        return HDF_SUCCESS;
    }

    if (g_acmReadBuffer != NULL) {
        OsalMemFree(g_acmReadBuffer);
        g_acmReadBuffer = NULL;
    }

    UsbSerialFreeFifo((struct DataFifo *)&port->readFifo);
    AcmRelease(acm);
    return HDF_SUCCESS;
}

static int32_t SerialWrite(const struct SerialDevice *port, struct HdfSBuf *data)
{
    int32_t size;
    int32_t ret;
    const char *tmp = NULL;

    int32_t wbn;
    struct AcmWb *wb = NULL;
    if (port == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct AcmDevice *acm = port->acm;
    if (acm == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (AcmWbIsAvail(acm)) {
        wbn = AcmWbAlloc(acm);
    } else {
        HDF_LOGE("no write buf\n");
        return 0;
    }
    if (wbn < 0) {
        HDF_LOGE("AcmWbAlloc failed\n");
        return HDF_FAILURE;
    }
    wb = &acm->wb[wbn];

    tmp = HdfSbufReadString(data);
    if (tmp == NULL) {
        HDF_LOGE("%s: sbuf read buffer failed", __func__);
        return HDF_ERR_IO;
    }
    size = strlen(tmp) + 1;
    size = (size > acm->writeSize) ? acm->writeSize : size;
    ret = memcpy_s(wb->buf, acm->writeSize, tmp, size);
    if (ret != EOK) {
        HDF_LOGE("memcpy_s fail, ret=%d", ret);
    }
    wb->len = (int)size;
    ret = AcmStartWb(acm, wb, acm->dataOutPipe);
    return size;
}

static int32_t SerialAddOrRemoveInterface(int32_t cmd, const struct SerialDevice *port, const struct HdfSBuf *data)
{
    struct AcmDevice *acm = port->acm;
    UsbInterfaceStatus status = 0;
    uint32_t index = 0;

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

    return UsbAddOrRemoveInterface(acm->session, acm->busNum, acm->devAddr, index, status);
}

static int32_t UsbSerialCheckCmd(struct SerialDevice *port, int32_t cmd,
    struct HdfSBuf *data, const struct HdfSBuf *reply)
{
    switch (cmd) {
        case CMD_OPEN_PARM:
            return SerialOpen(port, data);
        case CMD_CLOSE_PARM:
            return SerialClose(port, data);
        case CMD_WRITE_PARM:
            return SerialWrite(port, data);
        case CMD_READ_PARM:
            return UsbSerialRead(port, (struct HdfSBuf *)reply);
        case CMD_GET_BAUDRATE:
            return SerialGetBaudrate(port, (struct HdfSBuf *)reply);
        case CMD_SET_BAUDRATE:
            return SerialSetBaudrate(port, (struct HdfSBuf *)data);
        case CMD_WRITE_DATA_SYNC:
            return SerialWriteSync(port, data);
        case CMD_READ_DATA_SYNC:
            return UsbSerialReadSync(port, (struct HdfSBuf *)reply);
        case CMD_CLASS_CTRL_SYNC:
            return UsbCtrlMsg(port, (struct HdfSBuf *)reply);
        case CMD_STD_CTRL_GET_DESCRIPTOR_CMD:
            return UsbStdCtrlCmd(port, CMD_STD_CTRL_GET_DESCRIPTOR_CMD, (struct HdfSBuf *)reply);
        case CMD_STD_CTRL_GET_STATUS_CMD:
            return UsbStdCtrlCmd(port, CMD_STD_CTRL_GET_STATUS_CMD, (struct HdfSBuf *)reply);
        case CMD_STD_CTRL_GET_CONFIGURATION:
            return UsbStdCtrlCmd(port, CMD_STD_CTRL_GET_CONFIGURATION, (struct HdfSBuf *)reply);
        case CMD_STD_CTRL_GET_INTERFACE:
            return UsbStdCtrlCmd(port, CMD_STD_CTRL_GET_INTERFACE, (struct HdfSBuf *)reply);
        case CMD_STD_CTRL_GET_DESCRIPTOR_ASYNC:
            return UsbStdCtrlCmd(port, CMD_STD_CTRL_GET_DESCRIPTOR_ASYNC, (struct HdfSBuf *)reply);
        case CMD_ADD_INTERFACE:
        case CMD_REMOVE_INTERFACE:
            return SerialAddOrRemoveInterface(cmd, port, data);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
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
    acm = (struct AcmDevice *)client->device->service;
    port = acm->port;
    if (port == NULL) {
        HDF_LOGE("%s:%d port is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (g_acmReleaseFlag) {
        HDF_LOGE("%s:%d g_acmReleaseFlag is true", __func__, __LINE__);
        return HDF_FAILURE;
    }

    return UsbSerialCheckCmd(port, cmd, data, reply);
}

static struct UsbInterface *GetUsbInterfaceById(const struct AcmDevice *acm,
    uint8_t interfaceIndex)
{
    return UsbClaimInterface(acm->session, acm->busNum, acm->devAddr, interfaceIndex);
}

static void AcmFreePipes(struct AcmDevice *acm)
{
    if (acm == NULL) {
        return;
    }
    if (acm->ctrPipe) {
        OsalMemFree(acm->ctrPipe);
        acm->ctrPipe = NULL;
    }
    if (acm->intPipe) {
        OsalMemFree(acm->intPipe);
        acm->intPipe = NULL;
    }
    if (acm->dataInPipe) {
        OsalMemFree(acm->dataInPipe);
        acm->dataInPipe = NULL;
    }
    if (acm->dataOutPipe) {
        OsalMemFree(acm->dataOutPipe);
        acm->dataOutPipe = NULL;
    }
}

static struct UsbPipeInfo *EnumePipe(const struct AcmDevice *acm,
    uint8_t interfaceIndex, UsbPipeType pipeType, UsbPipeDirection pipeDirection)
{
    uint8_t i;
    int32_t ret;
    struct UsbInterfaceInfo *info = NULL;
    UsbInterfaceHandle *interfaceHandle = NULL;
    if (pipeType == USB_PIPE_TYPE_CONTROL)
    {
        info = &acm->ctrIface->info;
        interfaceHandle = acm->ctrDevHandle;
    }
    else
    {
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

/* HdfDriverEntry implementations */
static int32_t UsbSerialDriverBind(struct HdfDeviceObject *device)
{
    struct UsbPnpNotifyServiceInfo *info = NULL;
    errno_t err;
    struct AcmDevice *acm = NULL;
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
        HDF_LOGD("%s:%d busNum=%d,devAddr=%d,interfaceLength=%d", \
            __func__, __LINE__, info->busNum, info->devNum, info->interfaceLength);
        acm->busNum = (uint8_t)info->busNum;
        acm->devAddr = (uint8_t)info->devNum;
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
    acm->device  = device;
    device->service = &(acm->service);
    acm->device->service->Dispatch = UsbSerialDeviceDispatch;
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
                __func__,
                dr->bNotificationType, dr->wIndex, dr->wLength);
    }
    return;
}

static int32_t AcmCtrlIrqCheckSize(struct UsbRequest *req, struct AcmDevice *acm,
    struct UsbCdcNotification *dr)
{
    unsigned int allocSize;
    unsigned int copySize;
    int32_t ret;

    if ((req == NULL) || (acm == NULL) || (dr == NULL)) {
        HDF_LOGE("%s:%d Invalid parameter", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    unsigned int currentSize = req->compInfo.actualLength;
    HDF_LOGD("actualLength:%u\n", currentSize);

    unsigned int expectedSize = sizeof(struct UsbCdcNotification) + Le16ToCpu(dr->wLength);
    if (currentSize < expectedSize) {
        if (acm->nbSize < expectedSize) {
            if (acm->nbSize) {
                OsalMemFree(acm->notificationBuffer);
                acm->nbSize = 0;
            }
            allocSize = expectedSize;
            acm->notificationBuffer = OsalMemCalloc(allocSize);
            if (!acm->notificationBuffer) {
                return HDF_ERR_MALLOC_FAIL;
            }
            acm->nbSize = allocSize;
        }
        copySize = MIN(currentSize, expectedSize - acm->nbIndex);
        ret = memcpy_s(&acm->notificationBuffer[acm->nbIndex], acm->nbSize - acm->nbIndex,
            req->compInfo.buffer, copySize);
        if (ret != EOK) {
            HDF_LOGE("memcpy_s fail ret=%d", ret);
        }
        acm->nbIndex += copySize;
        currentSize = acm->nbIndex;
    }

    if (currentSize >= expectedSize) {
        AcmProcessNotification(acm, (unsigned char *)dr);
        acm->nbIndex = 0;
    }
    return HDF_SUCCESS;
}

static void AcmCtrlIrq(struct UsbRequest *req)
{
    if (req == NULL) {
        HDF_LOGE("%s:%d req is NULL!", __func__, __LINE__);
        goto EXIT;
    }
    int32_t ret;
    struct AcmDevice *acm = (struct AcmDevice *)req->compInfo.userData;
    int32_t status = req->compInfo.status;
    HDF_LOGD("Irqstatus:%d", status);

    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)req->compInfo.buffer;
    switch (status) {
        case 0:
            break;
        default:
            goto EXIT;
    }
    if ((acm != NULL) && acm->nbIndex) {
        dr = (struct UsbCdcNotification *)acm->notificationBuffer;
    }
    if ((dr == NULL) || (acm == NULL)) {
        HDF_LOGE("%s:%d dr or acm is NULL!", __func__, __LINE__);
        goto EXIT;
    }

    ret = AcmCtrlIrqCheckSize(req, acm, dr);
    if (ret != HDF_SUCCESS) {
        goto EXIT;
    }

    UsbSubmitRequestAsync(req);

EXIT:
    HDF_LOGE("%s:%d exit", __func__, __LINE__);
}

static void AcmReadBulk(struct UsbRequest *req)
{
    if (req == NULL) {
        HDF_LOGE("%s:%d req is NULL!", __func__, __LINE__);
        return;
    }
    int32_t retval;
    int32_t status = req->compInfo.status;
    size_t size = req->compInfo.actualLength;
    struct AcmDevice *acm = (struct AcmDevice *)req->compInfo.userData;
    if (acm == NULL || acm->port == NULL) {
        HDF_LOGE("%s:%d acm is NULL!", __func__, __LINE__);
        return;
    }

    switch (status) {
        case 0:
            HDF_LOGD("Bulk status: %d+size:%zu\n", status, size);
            if (size) {
                uint8_t *data = req->compInfo.buffer;
                uint32_t count;
                OsalMutexLock(&acm->readLock);
                if (DataFifoIsFull(&acm->port->readFifo)) {
                    HDF_LOGD("%s:%d", __func__, __LINE__);
                    DataFifoSkip(&acm->port->readFifo, size);
                }
                count = DataFifoWrite(&acm->port->readFifo, data, size);
                if (count != size) {
                    HDF_LOGW("%s: write %u less than expected %zu",
                        __func__, count, size);
                }
                OsalMutexUnlock(&acm->readLock);
            }
            break;
        default:
            HDF_LOGE("%s:%d status=%d", __func__, __LINE__, status);
            return;
    }

    retval = UsbSubmitRequestAsync(req);
    if (retval && retval != -EPERM) {
        HDF_LOGE("%s - usb_submit_urb failed: %d", __func__, retval);
    }
}

static void AcmFreeWriteRequests(struct AcmDevice *acm)
{
    int32_t i;
    int32_t ret;
    struct AcmWb *snd = NULL;

    for (i = 0; i < ACM_NW; i++) {
        snd = &acm->wb[i];
        ret = UsbCancelRequest(snd->request);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("UsbCancelRequest rd faile, ret=%d ", ret);
        }
    }
    for (i = 0; i < ACM_NW; i++) {
        snd = &acm->wb[i];
        if (snd->request != NULL) {
            UsbFreeRequest(snd->request);
            snd->request = NULL;
        }
    }
}

static void AcmFreeReadRequests(struct AcmDevice *acm)
{
    int32_t i;
    int32_t ret;

    if (acm == NULL) {
        HDF_LOGE("%s: acm is NULL", __func__);
        return;
    }
    for (i = 0; i < ACM_NR; i++) {
        ret = UsbCancelRequest(acm->readReq[i]);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("UsbCancelRequest rd faile, ret=%d ", ret);
        }
    }
    for (i = 0; i < ACM_NR; i++) {
        if (acm->readReq[i]) {
            UsbFreeRequest(acm->readReq[i]);
            acm->readReq[i] = NULL;
        }
    }
}

static void AcmFreeNotifyReqeust(struct AcmDevice *acm)
{
    int32_t ret;

    if ((acm == NULL) || (acm->notifyReq == NULL)) {
        HDF_LOGE("%s: acm or notifyReq is NULL", __func__);
        return;
    }
    ret = UsbCancelRequest(acm->notifyReq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("UsbCancelRequest rd faile, ret=%d ", ret);
    }
    ret = UsbFreeRequest(acm->notifyReq);
    if (ret == HDF_SUCCESS) {
        acm->notifyReq = NULL;
    } else {
        HDF_LOGE("%s: AcmFreeNotifyReqeust failed, ret=%d",
            __func__, ret);
    }
}

static int32_t AcmAllocReadRequests(struct AcmDevice *acm)
{
    int32_t ret;
    struct UsbRequestParams readParmas = {};
    for (int32_t i = 0; i < ACM_NR; i++) {
        acm->readReq[i] = UsbAllocRequest(InterfaceIdToHandle(acm, acm->dataInPipe->interfaceId), 0, acm->readSize);
        if (!acm->readReq[i]) {
            HDF_LOGE("readReq request failed\n");
            goto ERROR;
        }
        readParmas.userData = (void *)acm;
        readParmas.pipeAddress = acm->dataInPipe->pipeAddress;
        readParmas.pipeId = acm->dataInPipe->pipeId;
        readParmas.interfaceId = acm->dataInPipe->interfaceId;
        readParmas.callback = AcmReadBulk;
        readParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        readParmas.timeout = USB_CTRL_SET_TIMEOUT;
        readParmas.dataReq.numIsoPackets = 0;
        readParmas.dataReq.directon = (((uint8_t)acm->dataInPipe->pipeDirection) >> USB_PIPE_DIR_OFFSET) & 0x1;
        readParmas.dataReq.length = (int)acm->readSize;
        ret = UsbFillRequest(acm->readReq[i], InterfaceIdToHandle(acm, acm->dataInPipe->interfaceId), &readParmas);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%s: UsbFillRequest faile, ret=%d \n", __func__, ret);
            goto ERROR;
        }
    }
    return HDF_SUCCESS;

ERROR:
    AcmFreeReadRequests(acm);
    return HDF_ERR_MALLOC_FAIL;
}

static int32_t AcmAllocNotifyRequest(struct AcmDevice *acm)
{
    int32_t ret;
    struct UsbRequestParams intParmas = {};
    acm->notifyReq = UsbAllocRequest(InterfaceIdToHandle(acm, acm->intPipe->interfaceId), 0, acm->intSize);
    if (!acm->notifyReq) {
        HDF_LOGE("notifyReq request fail\n");
        return HDF_ERR_MALLOC_FAIL;
    }
    intParmas.userData = (void *)acm;
    intParmas.pipeAddress = acm->intPipe->pipeAddress;
    intParmas.pipeId = acm->intPipe->pipeId;
    intParmas.interfaceId = acm->intPipe->interfaceId;
    intParmas.callback = AcmCtrlIrq;
    intParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    intParmas.timeout = USB_CTRL_SET_TIMEOUT;
    intParmas.dataReq.numIsoPackets = 0;
    intParmas.dataReq.directon = (((uint8_t)acm->intPipe->pipeDirection) >> USB_PIPE_DIR_OFFSET) & DIRECTION_MASK;
    intParmas.dataReq.length = (int)acm->intSize;
    ret = UsbFillRequest(acm->notifyReq, InterfaceIdToHandle(acm, acm->intPipe->interfaceId), &intParmas);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%s: UsbFillRequest faile, ret=%d", __func__, ret);
        goto ERROR;
    }
    return HDF_SUCCESS;

ERROR:
    AcmFreeNotifyReqeust(acm);
    return ret;
}


static void AcmReleaseInterfaces(struct AcmDevice *acm)
{
    for (uint8_t i = 0; i < acm->interfaceCnt; i++) {
        if (acm->iface[i]) {
            UsbReleaseInterface(acm->iface[i]);
            acm->iface[i] = NULL;
        }
    }
    if (acm->ctrIface) {
        UsbReleaseInterface(acm->ctrIface);
        acm->ctrIface = NULL;
    }
}

static int32_t AcmClaimInterfaces(struct AcmDevice *acm)
{
    for (uint8_t i = 0; i < acm->interfaceCnt; i++) {
        acm->iface[i] = GetUsbInterfaceById((const struct AcmDevice *)acm, acm->interfaceIndex[i]);
        if (acm->iface[i] == NULL) {
            HDF_LOGE("%s: interface%d is null", __func__, acm->interfaceIndex[i]);
            goto ERROR;
        }
    }

    acm->ctrIface = GetUsbInterfaceById((const struct AcmDevice *)acm, USB_CTRL_INTERFACE_ID);
    if (acm->ctrIface == NULL) {
        HDF_LOGE("%s: GetUsbInterfaceById null", __func__);
        goto ERROR;
    }

    return HDF_SUCCESS;

 ERROR:
    AcmReleaseInterfaces(acm);
    return HDF_FAILURE;
}

static void AcmCloseInterfaces(struct AcmDevice *acm)
{
    for (uint8_t i = 0; i < acm->interfaceCnt; i++) {
        if (acm->devHandle[i]) {
            UsbCloseInterface(acm->devHandle[i]);
            acm->devHandle[i] = NULL;
        }
    }
    if (acm->ctrDevHandle) {
        UsbCloseInterface(acm->ctrDevHandle);
        acm->ctrDevHandle = NULL;
    }
}

static int32_t AcmOpenInterfaces(struct AcmDevice *acm)
{
    for (uint8_t i = 0; i < acm->interfaceCnt; i++) {
        if (acm->iface[i]) {
            acm->devHandle[i] = UsbOpenInterface(acm->iface[i]);
            if (acm->devHandle[i] == NULL) {
                HDF_LOGE("%s: UsbOpenInterface null", __func__);
                goto ERROR;
            }
        }
    }
    acm->ctrDevHandle = UsbOpenInterface(acm->ctrIface);
    if (acm->ctrDevHandle == NULL) {
        HDF_LOGE("%s: ctrDevHandle UsbOpenInterface null", __func__);
        goto ERROR;
    }

    return HDF_SUCCESS;

ERROR:
    AcmCloseInterfaces(acm);
    return HDF_FAILURE;
}

static int32_t AcmGetPipes(struct AcmDevice *acm)
{
    acm->dataInPipe = GetPipe(acm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_IN);
    if (acm->dataInPipe == NULL) {
        HDF_LOGE("dataInPipe is NULL");
        goto ERROR;
    }

    acm->dataOutPipe = GetPipe(acm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_OUT);
    if (acm->dataOutPipe == NULL) {
        HDF_LOGE("dataOutPipe is NULL");
        goto ERROR;
    }

    acm->ctrPipe = EnumePipe(acm, acm->ctrIface->info.interfaceIndex, USB_PIPE_TYPE_CONTROL, USB_PIPE_DIRECTION_OUT);
    if (acm->ctrPipe == NULL) {
        HDF_LOGE("ctrPipe is NULL");
        goto ERROR;
    }

    acm->intPipe = GetPipe(acm, USB_PIPE_TYPE_INTERRUPT, USB_PIPE_DIRECTION_IN);
    if (acm->intPipe == NULL) {
        HDF_LOGE("intPipe is NULL");
        goto ERROR;
    }

    acm->readSize  = acm->dataInPipe->maxPacketSize;
    acm->writeSize = acm->dataOutPipe->maxPacketSize;
    acm->ctrlSize  = acm->ctrPipe->maxPacketSize;
    acm->intSize   = acm->intPipe->maxPacketSize;

    return HDF_SUCCESS;

ERROR:
    AcmFreePipes(acm);
    return HDF_FAILURE;
}

static void AcmFreeRequests(struct AcmDevice *acm)
{
    if (g_syncRequest != NULL) {
        UsbFreeRequest(g_syncRequest);
        g_syncRequest = NULL;
    }
    AcmFreeReadRequests(acm);
    AcmFreeNotifyReqeust(acm);
    AcmFreeWriteRequests(acm);
    AcmWriteBufFree(acm);
}

static int32_t AcmAllocRequests(const struct AcmDevice *acm)
{
    int32_t ret;

    if (AcmWriteBufAlloc(acm) < 0) {
        HDF_LOGE("%s: AcmWriteBufAlloc failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    for (int32_t i = 0; i < ACM_NW; i++) {
        struct AcmWb *snd = (struct AcmWb *)&(acm->wb[i]);
        snd->request = UsbAllocRequest(InterfaceIdToHandle((struct AcmDevice *)acm,
            acm->dataOutPipe->interfaceId), 0, acm->writeSize);
        snd->instance = (struct AcmDevice *)acm;
        if (snd->request == NULL) {
            HDF_LOGE("%s:%d snd request fail", __func__, __LINE__);
            goto ERROR_ALLOC_WRITE_REQ;
        }
    }

    ret = AcmAllocNotifyRequest((struct AcmDevice *)acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d AcmAllocNotifyRequest fail", __func__, __LINE__);
        goto ERROR_ALLOC_INT_REQ;
    }

    ret = AcmAllocReadRequests((struct AcmDevice *)acm);
    if (ret) {
        HDF_LOGE("%s:%d AcmAllocReadRequests fail", __func__, __LINE__);
        goto ERROR_ALLOC_READ_REQ;
    }

    return HDF_SUCCESS;

ERROR_ALLOC_READ_REQ:
    AcmFreeNotifyReqeust((struct AcmDevice *)acm);
ERROR_ALLOC_INT_REQ:
    AcmFreeWriteRequests((struct AcmDevice *)acm);
ERROR_ALLOC_WRITE_REQ:
    AcmWriteBufFree((struct AcmDevice *)acm);
    return HDF_FAILURE;
}

static int32_t AcmInit(struct AcmDevice *acm)
{
    int32_t ret;
    struct UsbSession *session = NULL;

    if (acm->initFlag) {
        HDF_LOGE("%s:%d: initFlag is true", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    ret = UsbInitHostSdk(NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbInitHostSdk failed", __func__);
        return HDF_ERR_IO;
    }
    acm->session = session;

    ret = AcmClaimInterfaces(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcmClaimInterfaces failed", __func__);
        goto ERROR_CLAIM_INTERFACES;
    }

    ret = AcmOpenInterfaces(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcmOpenInterfaces failed", __func__);
        goto ERROR_OPEN_INTERFACES;
    }

    ret = AcmGetPipes(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcmGetPipes failed", __func__);
        goto ERROR_GET_PIPES;
    }

    ret = AcmAllocRequests(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: AcmAllocRequests failed", __func__);
        goto ERROR_ALLOC_REQS;
    }

    acm->lineCoding.dwDTERate = CpuToLe32(DATARATE);
    acm->lineCoding.bCharFormat = USB_CDC_1_STOP_BITS;
    acm->lineCoding.bParityType = USB_CDC_NO_PARITY;
    acm->lineCoding.bDataBits = DATA_BITS_LENGTH;
    acm->initFlag = true;

    HDF_LOGD("%s:%d========OK", __func__, __LINE__);
    return HDF_SUCCESS;

ERROR_ALLOC_REQS:
    AcmFreePipes(acm);
ERROR_GET_PIPES:
    AcmCloseInterfaces(acm);
ERROR_OPEN_INTERFACES:
    AcmReleaseInterfaces(acm);
ERROR_CLAIM_INTERFACES:
    UsbExitHostSdk(acm->session);
    acm->session = NULL;
    return ret;
}

static void AcmRelease(struct AcmDevice *acm)
{
    if (!(acm->initFlag)) {
        HDF_LOGE("%s:%d: initFlag is false", __func__, __LINE__);
        return;
    }

    AcmCloseInterfaces(acm);
    AcmReleaseInterfaces(acm);
    AcmFreeRequests(acm);
    AcmFreePipes(acm);
    UsbExitHostSdk(acm->session);
    acm->session = NULL;

    acm->initFlag = false;
}

static int32_t UsbSerialDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct AcmDevice *acm = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    acm = (struct AcmDevice *)device->service;
    if (acm == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    OsalMutexInit(&acm->readLock);
    OsalMutexInit(&acm->writeLock);
    HDF_LOGD("%s:%d busNum=%d,devAddr=%d", \
        __func__, __LINE__, acm->busNum, acm->devAddr);

    ret = UsbSerialDeviceAlloc(acm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Serial Device alloc failed", __func__);
    }

    acm->initFlag = false;
    g_acmReleaseFlag = false;

    HDF_LOGD("%s:%d init ok!", __func__, __LINE__);

    return ret;
}

static void UsbSerialDriverRelease(struct HdfDeviceObject *device)
{
    struct AcmDevice *acm = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }
    acm = (struct AcmDevice *)device->service;
    if (acm == NULL) {
        HDF_LOGE("%s: acm is null", __func__);
        return;
    }

    g_acmReleaseFlag = true;

    if (acm->initFlag) {
        HDF_LOGE("%s:%d AcmRelease", __func__, __LINE__);
        AcmRelease(acm);
    }
    UsbSeriaDevicelFree(acm);
    OsalMutexDestroy(&acm->writeLock);
    OsalMutexDestroy(&acm->readLock);
    OsalMutexDestroy(&acm->lock);
    OsalMemFree(acm);
    acm = NULL;
    HDF_LOGD("%s:%d exit", __func__, __LINE__);
}

struct HdfDriverEntry g_usbSerialDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usbhost_acm",
    .Bind          = UsbSerialDriverBind,
    .Init          = UsbSerialDriverInit,
    .Release       = UsbSerialDriverRelease,
};
HDF_INIT(g_usbSerialDriverEntry);
