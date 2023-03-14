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

#include "cdc_ether.h"
#include <unistd.h>
#include "hdf_base.h"
#include "hdf_device_info.h"
#include "hdf_log.h"
#include "hdf_usb_pnp_manage.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usb_ddk_interface.h"

#define HDF_LOG_TAG   USB_HOST_ECM

static bool g_ecmReleaseFlag = false;

static void EcmWriteBulk(struct UsbRequest *req);
void EcmAllocWriteReq(struct EcmDevice *ecm);
void EcmFreeWriteReq(struct EcmDevice *ecm);
int32_t EcmAllocIntReq(struct EcmDevice *ecm);
void EcmAllocReadReq(struct EcmDevice *ecm);
void EcmFreeReadReq(struct EcmDevice *ecm);
static int32_t EcmInit(struct EcmDevice *ecm);
static void EcmRelease(struct EcmDevice *ecm);
static struct UsbInterface *EcmGetUsbInterfaceById(struct EcmDevice *ecm, uint8_t interfaceIndex);

static int32_t EcmWbAlloc(struct EcmDevice *ecm)
{
    int32_t i, wbn;
    struct EcmWb *wb = NULL;
    wbn = 0;
    i = 0;
    OsalMutexLock(&ecm->writeLock);
    for (;;) {
        wb = &ecm->wb[wbn];
        if (!wb->use) {
            wb->use = 1;
            wb->len = 0;
            OsalMutexUnlock(&ecm->writeLock);
            return wbn;
        }
        wbn = (wbn + 1) % ECM_NW;
        if (++i >= ECM_NW) {
            OsalMutexUnlock(&ecm->writeLock);
            return 0;
        }
    }
    OsalMutexUnlock(&ecm->writeLock);
}

static UsbInterfaceHandle *InterfaceIdToHandle(const struct EcmDevice *ecm, uint8_t id)
{
    UsbInterfaceHandle *devHandle = NULL;

    if (id == 0xFF) {
        devHandle = ecm->ctrDevHandle;
    } else {
        for (int32_t i = 0; i < ecm->interfaceCnt; i++) {
            if (ecm->iface[i]->info.interfaceIndex == id) {
                devHandle = ecm->devHandle[i];
                break;
            }
        }
    }
    return devHandle;
}

static int32_t EcmAllocFifo(struct DataFifo *fifo, uint32_t size)
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

static void EcmFreeFifo(struct DataFifo *fifo)
{
    void *buf = fifo->data;
    OsalMemFree(buf);
    DataFifoInit(fifo, 0, NULL);
}

static int32_t EcmWbIsAvail(struct EcmDevice *ecm)
{
    int32_t i, n;
    n = ECM_NW;
    OsalMutexLock(&ecm->writeLock);
    for (i = 0; i < ECM_NW; i++) {
        n -= ecm->wb[i].use;
    }
    OsalMutexUnlock(&ecm->writeLock);
    return n;
}

static int32_t EcmStartWb(struct EcmDevice *ecm,
    struct EcmWb *wb)
{
    int32_t rc;
    struct UsbRequestParams parmas = {};
    ecm->transmitting++;
    parmas.interfaceId = ecm->dataOutPipe->interfaceId;
    parmas.pipeAddress = ecm->dataOutPipe->pipeAddress;
    parmas.pipeId = ecm->dataOutPipe->pipeId;
    parmas.callback = EcmWriteBulk;
    parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.dataReq.numIsoPackets = 0;
    parmas.userData = (void *)wb;
    parmas.dataReq.length = wb->len;
    parmas.dataReq.buffer = wb->buf;
    rc = UsbFillRequest(wb->request, InterfaceIdToHandle(ecm, ecm->dataOutPipe->interfaceId), &parmas);
    if (HDF_SUCCESS != rc) {
        HDF_LOGE("%s: UsbFillRequest faile, ret=%d", __func__, rc);
        return rc;
    }
    ecm->writeReq = wb->request;
    rc = UsbSubmitRequestAsync(wb->request);
    if (rc < 0) {
        HDF_LOGE("UsbRequestSubmitSync faile, ret=%d", rc);
        OsalMutexLock(&ecm->writeLock);
        wb->use = 0;
        OsalMutexUnlock(&ecm->writeLock);
        ecm->transmitting--;
    } else {
        ecm->writeReqNum++;
    }
    return rc;
}
static int32_t EcmWriteBufAlloc(struct EcmDevice *ecm)
{
    int32_t i;
    struct EcmWb *wb;
    for (wb = &ecm->wb[0], i = 0; i < ECM_NW; i++, wb++) {
        wb->buf = OsalMemCalloc(ecm->writeSize);
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

static int32_t EcmWriteBufFree(struct EcmDevice *ecm)
{
    int32_t i;
    struct EcmWb *wb;
    for (wb = &ecm->wb[0], i = 0; i < ECM_NW; i++, wb++) {
        if (wb->buf) {
            OsalMemFree(wb->buf);
            wb->buf = NULL;
        }
    }
    return 0;
}

static void EcmWriteBulk(struct UsbRequest *req)
{
    int32_t status = req->compInfo.status;
    struct EcmWb *wb  = (struct EcmWb *)req->compInfo.userData;
    struct EcmDevice *ecm = wb->ecm;
    ecm->writeReqNum--;

    switch (status) {
        case USB_REQUEST_COMPLETED:
            OsalMutexLock(&ecm->writeLock);
            wb->use = 0;
            OsalMutexUnlock(&ecm->writeLock);
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

static struct UsbControlRequest EcmUsbControlMsg(struct EcmControlParams *controlParams)
{
    struct UsbControlRequest dr;
    dr.target = controlParams->requestType & TARGET_MASK;
    dr.reqType = (controlParams->requestType >> USB_TYPE_OFFSET) & REQUEST_TYPE_MASK;
    dr.directon = (controlParams->requestType >> USB_DIR_OFFSET) & DIRECTION_MASK;
    dr.request = controlParams->request;
    dr.value = CpuToLe16(controlParams->value);
    dr.index = CpuToLe16(controlParams->index);
    dr.buffer = controlParams->data;
    dr.length = CpuToLe16(controlParams->size);
    return dr;
}

static int32_t EcmCtrlMsg(struct EcmDevice *ecm, uint8_t request,
    uint16_t value, void *buf, uint16_t len)
{
    int32_t ret;
    const uint16_t index = 0;
    struct UsbRequest *usbRequest = NULL;
    struct EcmControlParams controlParams;
    struct UsbRequestParams parmas = {};
    if (ecm == NULL) {
        HDF_LOGE("%s:invalid param", __func__);
        return HDF_ERR_IO;
    }
    usbRequest = UsbAllocRequest(ecm->ctrDevHandle, 0, len);
    if (usbRequest == NULL) {
        HDF_LOGE("%s: UsbAllocRequest failed", __func__);
        return HDF_ERR_IO;
    }
    ecm->ctrlReq = usbRequest;

    controlParams.request = request;
    controlParams.requestType = USB_DDK_TYPE_CLASS | USB_DDK_RECIP_INTERFACE;
    controlParams.value = value;
    controlParams.index = index;
    controlParams.data = buf;
    controlParams.size = len;

    parmas.interfaceId = USB_CTRL_INTERFACE_ID;
    parmas.pipeAddress = ecm->ctrPipe->pipeAddress;
    parmas.pipeId = ecm->ctrPipe->pipeId;
    parmas.requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas.timeout = USB_CTRL_SET_TIMEOUT;
    parmas.ctrlReq = EcmUsbControlMsg(&controlParams);

    ret = UsbFillRequest(ecm->ctrlReq, ecm->ctrDevHandle, &parmas);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: faile, ret=%d ", __func__, ret);
        return ret;
    }
    ret = UsbSubmitRequestAsync(ecm->ctrlReq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("UsbRequestSubmitSync  faile, ret=%d ", ret);
        return ret;
    }
    if (!ecm->ctrlReq->compInfo.status) {
        HDF_LOGE("%s  status=%d ", __func__, ecm->ctrlReq->compInfo.status);
    }
    return HDF_SUCCESS;
}

static int32_t EcmRead(struct EcmDevice *ecm, struct HdfSBuf *reply)
{
    uint32_t len;
    int32_t ret = HDF_SUCCESS;
    uint8_t *buf = NULL;
    if (ecm == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!(ecm->openFlag)) {
        return HDF_ERR_BAD_FD;
    }

    for (int32_t i = 0; i < ECM_NR; i++) {
        if(ecm->readReq[i]->compInfo.status != USB_REQUEST_COMPLETED) {
            HDF_LOGE("%s:%d i=%d status=%d!",
                __func__, __LINE__, i, ecm->readReq[i]->compInfo.status);
            return HDF_FAILURE;
        }
    }
    OsalMutexLock(&ecm->readLock);
    if (DataFifoIsEmpty(&ecm->readFifo)) {
        OsalMutexUnlock(&ecm->readLock);
        return 0;
    }
    OsalMutexUnlock(&ecm->readLock);
    buf = (uint8_t *)OsalMemCalloc(DataFifoLen(&ecm->readFifo) + sizeof(uint32_t));
    if (buf == NULL) {
        HDF_LOGE("%s: OsalMemCalloc error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    OsalMutexLock(&ecm->readLock);
    len = DataFifoRead(&ecm->readFifo, buf, DataFifoLen(&ecm->readFifo));
    if (len == 0) {
        HDF_LOGE("%s: no data", __func__);
        ret = 0;
        OsalMutexUnlock(&ecm->readLock);
        goto OUT;
    }
    OsalMutexUnlock(&ecm->readLock);
    bool bufok = HdfSbufWriteBuffer(reply, (const void *)buf, len);
    if (!bufok) {
        HDF_LOGE("EcmRead HdfSbufWriteBuffer err");
        ret = HDF_ERR_IO;
    }
OUT:
    OsalMemFree(buf);
    return ret;
}


static int32_t EcmOpen(struct EcmDevice *ecm, struct HdfSBuf *data)
{
    int32_t ret;
    int32_t cmdType = HOST_ECM_ADD_INTERFACE;

    if ((ecm == NULL) || (data == NULL)) {
        HDF_LOGE("%s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &cmdType)) {
        HDF_LOGE("%s:%d sbuf read cmdType failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = EcmInit(ecm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d EcmInit failed", __func__, __LINE__);
        return  HDF_FAILURE;
    }

    if ((cmdType == HOST_ECM_ADD_INTERFACE) || (cmdType == HOST_ECM_REMOVE_INTERFACE)) {
        HDF_LOGD("%s:%d add or remove interface success", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    ret = EcmAllocFifo(&ecm->readFifo, READ_BUF_SIZE);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbSerialAllocFifo failed", __func__);
        return  HDF_ERR_INVALID_PARAM;
    }
    ecm->openFlag = true;
    ecm->readReqNum = 0;
    ecm->writeReqNum = 0;
    EcmAllocWriteReq(ecm);
    EcmAllocReadReq(ecm);
    for (int32_t i = 0; i < ECM_NR; i++) {
        ret = UsbSubmitRequestAsync(ecm->readReq[i]);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("UsbRequestSubmitSync  faile, ret=%d ", ret);
            goto ERR;
        } else {
            ecm->readReqNum++;
        }
    }
    return HDF_SUCCESS;
ERR:
    EcmFreeFifo(&ecm->readFifo);
    return ret;
}

static void EcmClostRelease(struct EcmDevice *ecm)
{
    int32_t ret;
    int32_t cnt = 0;
    const int32_t temp = 20;

    if (!(ecm->openFlag)) {
        HDF_LOGE("%s:%d: openFlag is false", __func__, __LINE__);
        return;
    }

    for (int32_t i = 0; i < ECM_NR; i++ ) {
        ret = UsbCancelRequest(ecm->readReq[i]);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("UsbCancelRequest rd faile, ret=%d ", ret);
        }
    }
    for (int32_t i = 0; i < ECM_NW; i++) {
        struct EcmWb *snd = &(ecm->wb[i]);
        ret = UsbCancelRequest(snd->request);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("UsbCancelRequest wr faile, ret=%d ", ret);
        }
    }

    while ((cnt < temp) && ((ecm->readReqNum != 0) || (ecm->writeReqNum != 0))) {
        cnt++;
    }

    EcmFreeWriteReq(ecm);
    EcmFreeReadReq(ecm);
    EcmFreeFifo(&ecm->readFifo);
    ecm->openFlag = false;
}

static int32_t EcmClose(struct EcmDevice *ecm, struct HdfSBuf *data)
{
    int32_t cmdType = HOST_ECM_REMOVE_INTERFACE;

    if ((ecm == NULL) || (data == NULL)) {
        HDF_LOGE("%s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &cmdType)) {
        HDF_LOGE("%s:%d sbuf read cmdType failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if ((cmdType == HOST_ECM_ADD_INTERFACE) || (cmdType == HOST_ECM_REMOVE_INTERFACE)) {
        HDF_LOGD("%s:%d cmdType=%d success", __func__, __LINE__, cmdType);
        return HDF_SUCCESS;
    }

    EcmClostRelease(ecm);
    EcmRelease(ecm);
    return HDF_SUCCESS;
}

static int32_t EcmWrite(struct EcmDevice *ecm, struct HdfSBuf *data)
{
    uint32_t size;
    uint32_t totalSize = 0;
    int32_t ret;
    uint8_t *tmp = NULL;
    int32_t wbn;
    uint32_t len;
    struct EcmWb *wb = NULL;

    if (ecm == NULL || ecm->openFlag == false) {
        return HDF_ERR_BAD_FD;
    }
    if (!HdfSbufReadBuffer(data, (const void **)&tmp, &totalSize)) {
        return HDF_ERR_IO;
    }
    size = totalSize;
    while (size != 0) {
        if (EcmWbIsAvail(ecm)) {
            wbn = EcmWbAlloc(ecm);
        } else {
            HDF_LOGE("%s:%d no write buf", __func__, __LINE__);
            return size;
        }
        if (wbn < ECM_NW && wbn >= 0) {
            wb = &ecm->wb[wbn];
        }
        if (wb == NULL) {
            return HDF_ERR_INVALID_PARAM;
        }
        if (size > ecm->writeSize) {
            len = ecm->writeSize;
            size -= ecm->writeSize;
        } else {
            len = size;
            size = 0;
        }
        if (wb->buf) {
            ret = memcpy_s(wb->buf, ecm->writeSize, tmp, len);
            if (ret) {
                return size;
            }
            tmp += len;
            wb->len = (int)len;
            wb->ecm = ecm;
            ret = EcmStartWb(ecm, wb);
        }
    }
    return totalSize;
}

static int32_t EcmGetMac(struct EcmDevice *ecm, struct HdfSBuf *reply)
{
    (void)reply;
    return HDF_SUCCESS;
}

static int32_t EcmAddOrRemoveInterface(int32_t cmd, struct EcmDevice *ecm, struct HdfSBuf *data)
{
    UsbInterfaceStatus status = USB_INTERFACE_STATUS_NORMAL;
    uint32_t index = 0;
    if (ecm == NULL) {
        HDF_LOGE("%d: invalid parma", __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &index)) {
        HDF_LOGE("%s:%d sbuf read interfaceNum failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cmd == CMD_ECM_ADD_INTERFACE) {
        status = USB_INTERFACE_STATUS_ADD;
    } else if (cmd == CMD_ECM_REMOVE_INTERFACE) {
        status = USB_INTERFACE_STATUS_REMOVE;
    } else {
        HDF_LOGE("%s:%d cmd=% is not define", __func__, __LINE__, cmd);
        return HDF_ERR_INVALID_PARAM;
    }

    return UsbAddOrRemoveInterface(ecm->session, ecm->busNum, ecm->devAddr, index, status);
}

static int32_t EcmDeviceDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct EcmDevice *ecm = NULL;
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

    if (g_ecmReleaseFlag) {
        HDF_LOGE("%s:%d g_ecmReleaseFlag is true", __func__, __LINE__);
        return HDF_ERR_DEVICE_BUSY;
    }

    ecm = (struct EcmDevice *)client->device->service;

    switch (cmd) {
        case CMD_ECM_OPEN:
            return EcmOpen(ecm, data);
        case CMD_ECM_CLOSE:
            return EcmClose(ecm, data);
        case CMD_ECM_READ:
            return EcmRead(ecm, reply);
        case CMD_ECM_WRITE:
            return EcmWrite(ecm, data);
        case CMD_ECM_GET_MAC:
            return EcmGetMac(ecm, reply);
        case CMD_ECM_ADD_INTERFACE:
        case CMD_ECM_REMOVE_INTERFACE:
            return EcmAddOrRemoveInterface(cmd, ecm, data);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }

    return HDF_SUCCESS;
}

static struct UsbInterface *EcmGetUsbInterfaceById(struct EcmDevice *ecm, uint8_t interfaceIndex)
{
    return UsbClaimInterface(ecm->session, ecm->busNum, ecm->devAddr, interfaceIndex);
}

static void EcmFreePipes(struct EcmDevice *ecm)
{
    if (ecm == NULL) {
        return;
    }
    if (ecm->ctrPipe) {
        OsalMemFree(ecm->ctrPipe);
        ecm->ctrPipe = NULL;
    }
    if (ecm->intPipe) {
        OsalMemFree(ecm->intPipe);
        ecm->intPipe = NULL;
    }
    if (ecm->dataInPipe) {
        OsalMemFree(ecm->dataInPipe);
        ecm->dataInPipe = NULL;
    }
    if (ecm->dataOutPipe) {
        OsalMemFree(ecm->dataOutPipe);
        ecm->dataOutPipe = NULL;
    }
}

static struct UsbPipeInfo *EcmEnumePipe(struct EcmDevice *ecm, uint8_t interfaceIndex,
    UsbPipeType pipeType, UsbPipeDirection pipeDirection)
{
    uint8_t i;
    int32_t ret;
    struct UsbInterfaceInfo *info = NULL;
    UsbInterfaceHandle *interfaceHandle = NULL;
    if (pipeType == USB_PIPE_TYPE_CONTROL) {
        info = &ecm->ctrIface->info;
        interfaceHandle = ecm->ctrDevHandle;
    } else {
        info = &ecm->iface[interfaceIndex]->info;
        interfaceHandle = ecm->devHandle[interfaceIndex];
    }

    for (i = 0;  i <= info->pipeNum; i++) {
        struct UsbPipeInfo p;
        ret = UsbGetPipeInfo(interfaceHandle, info->curAltSetting, i, &p);
        if (ret < HDF_SUCCESS) {
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

static struct UsbPipeInfo *EcmGetPipe(struct EcmDevice *ecm,
    UsbPipeType pipeType, UsbPipeDirection pipeDirection)
{
    uint8_t i;
    if (ecm == NULL) {
        HDF_LOGE("%s: invalid parmas", __func__);
        return NULL;
    }
    for (i = 0; i < ecm->interfaceCnt; i++) {
        struct UsbPipeInfo *p = NULL;
        if (ecm->iface[i] == NULL) {
            continue;
        }
        p = EcmEnumePipe(ecm, i, pipeType, pipeDirection);
        if (p == NULL) {
            continue;
        }
        return p;
    }
    return NULL;
}

static int32_t EcmGetPipes(struct EcmDevice *ecm)
{
    ecm->dataInPipe = EcmGetPipe(ecm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_IN);
    if (ecm->dataInPipe == NULL) {
        HDF_LOGE("dataInPipe is NULL\n");
        goto ERROR;
    }
    ecm->dataOutPipe = EcmGetPipe(ecm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_OUT);
    if (ecm->dataOutPipe == NULL) {
        HDF_LOGE("dataOutPipe is NULL\n");
        goto ERROR;
    }
    ecm->ctrPipe = EcmEnumePipe(ecm, ecm->ctrIface->info.interfaceIndex,
                                USB_PIPE_TYPE_CONTROL, USB_PIPE_DIRECTION_OUT);
    if (ecm->ctrPipe == NULL) {
        HDF_LOGE("ctrPipe is NULL\n");
        goto ERROR;
    }
    ecm->intPipe = EcmGetPipe(ecm, USB_PIPE_TYPE_INTERRUPT, USB_PIPE_DIRECTION_IN);
    if (ecm->intPipe == NULL) {
        HDF_LOGE("intPipe is NULL\n");
        goto ERROR;
    }

    ecm->readSize  = ecm->dataInPipe->maxPacketSize;
    ecm->writeSize = ecm->dataOutPipe->maxPacketSize;
    ecm->ctrlSize  = ecm->ctrPipe->maxPacketSize;
    ecm->intSize   = ecm->intPipe->maxPacketSize;

    return HDF_SUCCESS;

ERROR:
    EcmFreePipes(ecm);
    return HDF_FAILURE;
}

static int32_t EcmDriverBind(struct HdfDeviceObject *device)
{
    struct UsbPnpNotifyServiceInfo *info = NULL;
    errno_t err;
    struct EcmDevice *ecm = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ecm = (struct EcmDevice *)OsalMemCalloc(sizeof(*ecm));
    if (ecm == NULL) {
        HDF_LOGE("%s: Alloc usb serial device failed", __func__);
        return HDF_FAILURE;
    }

    info = (struct UsbPnpNotifyServiceInfo *)device->priv;
    if (info != NULL) {
        HDF_LOGD("bus:%d+dev:%d", info->busNum, info->devNum);
        HDF_LOGD("interfaceLength:%d", info->interfaceLength);
        ecm->busNum = (uint8_t)info->busNum;
        ecm->devAddr = (uint8_t)info->devNum;
        ecm->interfaceCnt = info->interfaceLength;
        err = memcpy_s((void *)(ecm->interfaceIndex), USB_MAX_INTERFACES,
            (const void*)info->interfaceNumber, info->interfaceLength);
        if (err != EOK) {
            HDF_LOGE("%s:%d memcpy_s faile err=%d", \
                __func__, __LINE__, err);
            goto ERROR;
        }
    } else {
        HDF_LOGE("%s:%d info is NULL!", __func__, __LINE__);
        goto ERROR;
    }

    ecm->device  = device;
    device->service = &(ecm->service);
    ecm->device->service->Dispatch = EcmDeviceDispatch;
    HDF_LOGD("EcmDriverBind=========================OK");
    return HDF_SUCCESS;

ERROR:
    OsalMemFree(ecm);
    ecm = NULL;
    return HDF_FAILURE;
}

static void EcmProcessNotification(struct EcmDevice *ecm, unsigned char *buf)
{
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)buf;
    switch (dr->bNotificationType) {
        case USB_DDK_CDC_NOTIFY_NETWORK_CONNECTION:
            HDF_LOGE("%s - network connection: %s\n", __func__,  (dr->wValue ? "on" : "off"));
            break;
        case USB_DDK_CDC_NOTIFY_SPEED_CHANGE:
            HDF_LOGE("%s - speed change wLength: %d\n", __func__, dr->wLength);
            break;
        default:
            HDF_LOGE("%s-%d received: index %d len %d\n",
             __func__, dr->bNotificationType, dr->wIndex, dr->wLength);
    }
    return;
}

static void EcmCtrlIrq(struct UsbRequest *req)
{
    int32_t retval;
    int32_t ret;
    struct EcmDevice *ecm = (struct EcmDevice *)req->compInfo.userData;
    unsigned int expectedSize, copySize, allocSize;
    int32_t status = req->compInfo.status;
    struct UsbCdcNotification *dr = (struct UsbCdcNotification *)req->compInfo.buffer;
    unsigned int currentSize = req->compInfo.actualLength;
    switch (status) {
        case 0:
            break;
        case -ECONNRESET:
        case -ENOENT:
        case -ESHUTDOWN:
            return;
        default:
            goto EXIT;
    }
    if (ecm->nbIndex){
        dr = (struct UsbCdcNotification *)ecm->notificationBuffer;
    }
    expectedSize = sizeof(struct UsbCdcNotification) + Le16ToCpu(dr->wLength);
    if (currentSize < expectedSize) {
        if (ecm->nbSize < expectedSize) {
            if (ecm->nbSize) {
                OsalMemFree(ecm->notificationBuffer);
                ecm->nbSize = 0;
            }
            allocSize = expectedSize;
            ecm->notificationBuffer = OsalMemCalloc(allocSize);
            if (!ecm->notificationBuffer){
                goto EXIT;
            }
            ecm->nbSize = allocSize;
        }
        copySize = MIN(currentSize, expectedSize - ecm->nbIndex);
        ret = memcpy_s(&ecm->notificationBuffer[ecm->nbIndex], ecm->nbSize - ecm->nbIndex,
               req->compInfo.buffer, copySize);
        if (ret != EOK) {
            HDF_LOGE("memcpy_s fail, ret=%d", ret);
        }
        ecm->nbIndex += copySize;
        currentSize = ecm->nbIndex;
    }
    if (currentSize >= expectedSize) {
        EcmProcessNotification(ecm, (unsigned char *)dr);
        ecm->nbIndex = 0;
    }

    retval = UsbSubmitRequestAsync(req);
    if (retval && retval != -EPERM) {
        HDF_LOGE("%s - usb_submit_urb failed: %d\n", __func__, retval);
    }

EXIT:
    HDF_LOGE("%s:%d exit", __func__, __LINE__);
}

static void EcmReadBulk(struct UsbRequest *req)
{
    int32_t retval;
    int32_t status = req->compInfo.status;
    size_t size = req->compInfo.actualLength;
    struct EcmDevice *ecm = (struct EcmDevice *)req->compInfo.userData;
    ecm->readReqNum--;
    switch (status) {
        case USB_REQUEST_COMPLETED:
            OsalMutexLock(&ecm->readLock);
            if (size) {
                uint8_t *data = req->compInfo.buffer;
                uint32_t count;
                if (DataFifoIsFull(&ecm->readFifo)) {
                    HDF_LOGD("%s:%d fifo is full", __func__, __LINE__);
                    DataFifoSkip(&ecm->readFifo, size);
                }
                count = DataFifoWrite(&ecm->readFifo, data, size);
                if (count != size) {
                    HDF_LOGW("%s: write %u less than expected %zu", __func__, count, size);
                }
            }
            OsalMutexUnlock(&ecm->readLock);
            break;
        default:
            HDF_LOGE("%s:%d status=%d", __func__, __LINE__, status);
            return;
    }

    if (ecm->openFlag) {
        retval = UsbSubmitRequestAsync(req);
        if (retval && retval != -EPERM) {
            HDF_LOGE("%s - usb_submit_urb failed: %d\n", __func__, retval);
        } else {
            ecm->readReqNum++;
        }
    }
}

void EcmAllocWriteReq(struct EcmDevice *ecm)
{
    if (EcmWriteBufAlloc(ecm) < 0) {
        HDF_LOGE("EcmAllocWriteReq buf alloc failed\n");
        return;
    }

    for (int32_t i = 0; i < ECM_NW; i++) {
        struct EcmWb *snd = &(ecm->wb[i]);
        snd->request = UsbAllocRequest(InterfaceIdToHandle(ecm, ecm->dataOutPipe->interfaceId), 0, ecm->writeSize);
        snd->instance = ecm;
        snd->use = 0;
        if (snd->request == NULL) {
            HDF_LOGE("snd request fail\n");
            goto ERR;
        }
    }
    return;
ERR:
   EcmWriteBufFree(ecm);
   return;
}

void EcmFreeWriteReq(struct EcmDevice *ecm)
{
    OsalMutexLock(&ecm->writeLock);
    for (int32_t i = 0; i < ECM_NW; i++) {
        struct EcmWb *snd = &(ecm->wb[i]);
        snd->use = 0;
        UsbFreeRequest(snd->request);
    }
    OsalMutexUnlock(&ecm->writeLock);
    EcmWriteBufFree(ecm);
}

int32_t EcmAllocIntReq(struct EcmDevice *ecm)
{
    int32_t ret;
    struct UsbRequestParams intParmas = {};
    if (ecm == NULL || ecm->intPipe == NULL) {
        return HDF_FAILURE;
    }
    ecm->notifyReq = UsbAllocRequest(InterfaceIdToHandle(ecm, ecm->intPipe->interfaceId), 0, ecm->intSize);
    if (!ecm->notifyReq) {
        HDF_LOGE("notifyReq request fail\n");
        return HDF_ERR_MALLOC_FAIL;
    }
    intParmas.userData = (void *)ecm;
    intParmas.pipeAddress = ecm->intPipe->pipeAddress;
    intParmas.pipeId = ecm->intPipe->pipeId;
    intParmas.interfaceId = ecm->intPipe->interfaceId;
    intParmas.callback = EcmCtrlIrq;
    intParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    intParmas.timeout = USB_CTRL_SET_TIMEOUT;
    intParmas.dataReq.numIsoPackets = 0;
    intParmas.dataReq.directon = (((uint32_t)(ecm->intPipe->pipeDirection)) >> USB_DIR_OFFSET) & 0x1;
    intParmas.dataReq.length = (int)ecm->intSize;
    ret = UsbFillRequest(ecm->notifyReq, InterfaceIdToHandle(ecm, ecm->intPipe->interfaceId), &intParmas);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbFillRequest faile, ret=%d \n", __func__, ret);
        return ret;
    }
    return HDF_SUCCESS;
}

void EcmAllocReadReq(struct EcmDevice *ecm)
{
    int32_t ret;
    struct UsbRequestParams readParmas = {};
    for (int32_t i = 0; i < ECM_NR; i++) {
        ecm->readReq[i] = UsbAllocRequest(InterfaceIdToHandle(ecm, ecm->dataInPipe->interfaceId), 0, ecm->readSize);
        if (!ecm->readReq[i]) {
            HDF_LOGE("readReq request failed\n");
            return;
        }
        readParmas.userData = (void *)ecm;
        readParmas.pipeAddress = ecm->dataInPipe->pipeAddress;
        readParmas.pipeId = ecm->dataInPipe->pipeId;
        readParmas.interfaceId = ecm->dataInPipe->interfaceId;
        readParmas.callback = EcmReadBulk;
        readParmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        readParmas.timeout = USB_CTRL_SET_TIMEOUT;
        readParmas.dataReq.numIsoPackets = 0;
        readParmas.dataReq.directon = (((uint32_t)(ecm->dataInPipe->pipeDirection)) >> USB_DIR_OFFSET) & 0x1;
        readParmas.dataReq.length = (int)ecm->readSize;
        ret = UsbFillRequest(ecm->readReq[i], InterfaceIdToHandle(ecm, ecm->dataInPipe->interfaceId), &readParmas);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: UsbFillRequest faile, ret=%d \n", __func__, ret);
            return;
        }
    }
}

void EcmFreeReadReq(struct EcmDevice *ecm)
{
    int32_t ret;
    for (int32_t i = 0; i < ECM_NR; i++) {
        ret = UsbFreeRequest(ecm->readReq[i]);
        if (ret) {
            goto ERR;
        }
    }
ERR:
    return;
}

static void UsbFreeNotifyReqeust(struct EcmDevice *ecm)
{
    int32_t ret;

    if ((ecm == NULL) || (ecm->notifyReq == NULL)) {
        HDF_LOGE("%s: ecm or notifyReq is NULL", __func__);
        return;
    }
    ret = UsbCancelRequest(ecm->notifyReq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("UsbCancelRequest rd faile, ret=%d ", ret);
    }
    ret = UsbFreeRequest(ecm->notifyReq);
    if (ret == HDF_SUCCESS) {
        ecm->notifyReq = NULL;
    } else {
        HDF_LOGE("%s: UsbFreeNotifyReqeust failed, ret=%d",
            __func__, ret);
    }
}

static void EcmReleaseInterfaces(struct EcmDevice *ecm)
{
    for (uint8_t i = 0; i < ecm->interfaceCnt; i++) {
        if (ecm->iface[i]) {
            UsbReleaseInterface(ecm->iface[i]);
            ecm->iface[i] = NULL;
        }
    }
    if (ecm->ctrIface) {
        UsbReleaseInterface(ecm->ctrIface);
        ecm->ctrIface = NULL;
    }
}

static int32_t EcmClaimInterfaces(struct EcmDevice *ecm)
{
    for (uint8_t i = 0; i < ecm->interfaceCnt; i++) {
        ecm->iface[i] = EcmGetUsbInterfaceById(ecm, ecm->interfaceIndex[i]);
        if (ecm->iface[i] == NULL) {
            HDF_LOGE("interface%d is null", ecm->interfaceIndex[i]);
            goto ERROR;
        }
    }

    ecm->ctrIface = EcmGetUsbInterfaceById(ecm, USB_CTRL_INTERFACE_ID);
    if (ecm->ctrIface == NULL) {
        HDF_LOGE("%d: UsbClaimInterface null", __LINE__);
        goto ERROR;
    }

    return HDF_SUCCESS;

ERROR:
    EcmReleaseInterfaces(ecm);
    return HDF_FAILURE;
}

static void EcmCloseInterfaces(struct EcmDevice *ecm)
{
    for (uint8_t i = 0; i < ecm->interfaceCnt; i++) {
        if (ecm->devHandle[i]) {
            UsbCloseInterface(ecm->devHandle[i]);
            ecm->devHandle[i] = NULL;
        }
    }

    if (ecm->ctrDevHandle) {
        UsbCloseInterface(ecm->ctrDevHandle);
        ecm->ctrDevHandle = NULL;
    }
}

static int32_t EcmOpenInterfaces(struct EcmDevice *ecm)
{
    for (uint8_t i = 0; i < ecm->interfaceCnt; i++) {
        if (ecm->iface[i]) {
            ecm->devHandle[i] = UsbOpenInterface(ecm->iface[i]);
            if (ecm->devHandle[i] == NULL) {
                HDF_LOGE("%s: UsbOpenInterface null", __func__);
                goto ERROR;
            }
        }
    }

    ecm->ctrDevHandle = UsbOpenInterface(ecm->ctrIface);
    if (ecm->ctrDevHandle == NULL) {
        HDF_LOGE("%s: ctrDevHandle UsbOpenInterface null", __func__);
        goto ERROR;
    }

    return HDF_SUCCESS;

ERROR:
    EcmCloseInterfaces(ecm);
    return HDF_FAILURE;
}

static int32_t EcmInit(struct EcmDevice *ecm)
{
    int32_t ret;
    const uint8_t altsetting = 1;
    struct UsbSession *session = NULL;

    if (ecm->initFlag) {
        HDF_LOGE("%s:%d: initFlag is true", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    ret = UsbInitHostSdk(NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbInitHostSdk failed", __func__);
        return HDF_ERR_IO;
    }
    ecm->session = session;

    ret = EcmClaimInterfaces(ecm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: EcmClaimInterfaces failed", __func__);
        goto ERR_CLAIM_INTERFACES;
    }

    ret = EcmOpenInterfaces(ecm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: EcmOpenInterfaces failed", __func__);
        goto ERROR_OPEN_INTERFACES;
    }

    if (ecm->interfaceCnt > USB_MAX_INTERFACES) {
        HDF_LOGE("interfaceCnt invalid : %u\n", ecm->interfaceCnt);
        goto ERROR_OPEN_INTERFACES;
    }
    
    /* set altsetting */
    ret = UsbSelectInterfaceSetting(ecm->devHandle[ecm->interfaceCnt-1], altsetting,
        &ecm->iface[ecm->interfaceCnt-1]);
    if (ret) {
        HDF_LOGE("UsbSelectInterfaceSetting fail\n");
        goto ERROR_SELECT_SETTING;
    }

    ret = EcmGetPipes(ecm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: EcmGetPipes failed", __func__);
        goto ERROR_GET_PIPES;
    }

    ret = EcmAllocIntReq(ecm);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: EcmAllocIntReq failed", __func__);
        goto ERROR_ALLOC_REQ;
    }
    if (0) {
        EcmCtrlMsg(ecm, USB_DDK_CDC_SET_ETHERNET_PACKET_FILTER, USB_DDK_CDC_PACKET_TYPE_DIRECTED
            | USB_DDK_CDC_PACKET_TYPE_BROADCAST, NULL, 0);
        ret = UsbSubmitRequestAsync(ecm->notifyReq);
        if (ret != HDF_SUCCESS) {
            return ret;
        }
    }
    ecm->initFlag = true;
    return HDF_SUCCESS;

ERROR_ALLOC_REQ:
    EcmFreePipes(ecm);
ERROR_GET_PIPES:
ERROR_SELECT_SETTING:
    EcmCloseInterfaces(ecm);
ERROR_OPEN_INTERFACES:
    EcmReleaseInterfaces(ecm);
ERR_CLAIM_INTERFACES:
    UsbExitHostSdk(ecm->session);
    ecm->session = NULL;
    return ret;
}

static void EcmRelease(struct EcmDevice *ecm)
{
    if (!(ecm->initFlag)) {
        HDF_LOGE("%s:%d: initFlag is false", __func__, __LINE__);
        return;
    }

    EcmCloseInterfaces(ecm);
    EcmReleaseInterfaces(ecm);
    UsbFreeNotifyReqeust(ecm);
    EcmFreePipes(ecm);
    UsbExitHostSdk(ecm->session);

    ecm->initFlag = false;
}

static int32_t EcmDriverInit(struct HdfDeviceObject *device)
{
    struct EcmDevice *ecm = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ecm = (struct EcmDevice *)device->service;
    if (ecm == NULL) {
        HDF_LOGE("%s: ecm is null", __func__);
        return HDF_FAILURE;
    }

    OsalMutexInit(&ecm->readLock);
    OsalMutexInit(&ecm->writeLock);
    ecm->openFlag = false;
    ecm->initFlag = false;
    g_ecmReleaseFlag = false;

    HDF_LOGE("%s:%d EcmDriverInit OK", __func__, __LINE__);
    return HDF_SUCCESS;
}

static void EcmDriverRelease(struct HdfDeviceObject *device)
{
    struct EcmDevice *ecm = NULL;
    if (device == NULL) {
        HDF_LOGE("%s: device is NULL", __func__);
        return;
    }
    ecm = (struct EcmDevice *)device->service;
    if (ecm == NULL) {
        HDF_LOGE("%s: ecm is null", __func__);
        return;
    }

    g_ecmReleaseFlag = true;

    if (ecm->openFlag) {
        HDF_LOGD("%s:%d EcmClostRelease", __func__, __LINE__);
        EcmClostRelease(ecm);
    }
    if (ecm->initFlag) {
        HDF_LOGD("%s:%d EcmRelease", __func__, __LINE__);
        EcmRelease(ecm);
    }
    OsalMutexDestroy(&ecm->writeLock);
    OsalMutexDestroy(&ecm->readLock);
    OsalMemFree(ecm);
    ecm = NULL;
    HDF_LOGD("%s:%d exit", __func__, __LINE__);
}

struct HdfDriverEntry g_ecmUsbDriverEntry = {
    .moduleVersion = 1,
    .moduleName    = "usbhost_ecm",
    .Bind          = EcmDriverBind,
    .Init          = EcmDriverInit,
    .Release       = EcmDriverRelease,
};

HDF_INIT(g_ecmUsbDriverEntry);
