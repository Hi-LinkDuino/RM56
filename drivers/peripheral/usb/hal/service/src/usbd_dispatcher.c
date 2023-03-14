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

#include "usbd_dispatcher.h"
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include "hdf_log.h"
#include "hdf_device_object.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usb_interface_pool.h"
#include "usbd.h"
#include "usbd_function.h"
#include "usbd_port.h"
#include "usbd_publisher.h"

#define HDF_LOG_TAG Usbd

#define MAX_BUFF_SIZE 16384
#define MAX_CONTROL_BUFF_SIZE 1024
#define READ_BUF_SIZE 8192

#define USB_CTRL_SET_TIMEOUT 5000
#define GET_STRING_SET_TIMEOUT 50
#define USB_PIPE_DIR_OFFSET 7
#define CHARFORMAT 8
#define USB_REUQEST_SLEEP_TIME 100
#define USB_MAX_DESCRIPTOR_SIZE 256
#define USB_BULK_CALLBACK_SLEEP_TIME 500
#define USB_BULK_CANCEL_SLEEP_TIME 1000

#define OPEN_SLEPP_TIME 1
#define SUBMIT_SLEEP_TIME 1
#define USBD_ASYNC_GETENODE_TIME 1
#define USBD_ASYNC_GETENODE_TRY 3

#define POS_STEP 3

#define MULTIPLE 3
#define ADD_NUM_50 100
#define ERROR_0 0

#define USB_RECIP_MASK 0x1F
#define ENDPOINT_DIRECTION_MASK 0x1
#define CMD_TYPE_MASK 0x3

#define MAX_REQUESTASYNC_NUM 20

static const int32_t CMD_OFFSET_5 = 5;
static const int32_t DIRECTION_OFFSET_7 = 7;
static const int32_t TYPE_OFFSET_8 = 8;
static const char* USB_TOKEN_VALUE = "UsbdBulkCallback.V1_0";

enum UsbdReqNodeStatus {
    USBD_REQNODE_INIT,
    USBD_REQNODE_NOUSE,
    USBD_REQNODE_USE,
    USBD_REQNODE_OTHER,
};

int32_t UsbdRealseDevices(struct UsbdService *service);
int32_t UsbdDeviceCreateAndAttach(struct UsbdService *service, uint8_t busNum, uint8_t devAddr);
int32_t UsbdDeviceDettach(struct UsbdService *service, uint8_t busNum, uint8_t devAddr);
int32_t UsbdRemoveBusDev(struct UsbdService *service, uint8_t busNum);

static bool UsbdHdfWriteBuf(struct HdfSBuf *data, uint8_t *buffer, uint32_t length)
{
    if ((!data) || ((length > 0) && (!buffer))) {
        HDF_LOGE("%{public}s:%{public}d hdfwritebuf failed", __func__, __LINE__);
        return false;
    }
    if (length == 0) {
        return HdfSbufWriteUint32(data, length);
    }
    return HdfSbufWriteBuffer(data, (const void *)buffer, length);
}

static int32_t ParseDeviceBuf(struct HdfSBuf *data, uint8_t *busNum, uint8_t *devAddr)
{
    if (data == NULL || busNum == NULL || devAddr == NULL) {
        HDF_LOGE("%{public}s:%{public}d devicebuf failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    HdfSbufReadUint8(data, busNum);
    HdfSbufReadUint8(data, devAddr);
    return HDF_SUCCESS;
}

static int32_t UsbdAllocFifo(struct DataFifo *fifo, uint32_t size)
{
    if (!DataFifoIsInitialized(fifo)) {
        void *data = OsalMemAlloc(size);
        if (data == NULL) {
            HDF_LOGE("%{public}s:%{public}d allocate failed", __func__, __LINE__);
            return HDF_ERR_MALLOC_FAIL;
        }
        DataFifoInit(fifo, size, data);
    }
    return HDF_SUCCESS;
}

static void UsbdFreeFifo(struct DataFifo *fifo)
{
    if (fifo == NULL) {
        HDF_LOGE("%{public}s:%{public}d fifo is NULL", __func__, __LINE__);
        return;
    }

    OsalMemFree(fifo->data);
    fifo->data = NULL;

    DataFifoInit(fifo, 0, NULL);
}

static void UsbdReadCallback(struct UsbRequest *req)
{
    struct UsbIfRequest *reqObj = (struct UsbIfRequest *)req;
    if (req == NULL) {
        HDF_LOGE("%{public}s:%{pulib}d req is NULL!", __func__, __LINE__);
        return;
    }
    struct UsbdRequestASync *dev = (struct UsbdRequestASync *)req->compInfo.userData;
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{pulib}d dev is NULL!", __func__, __LINE__);
        OsalSemPost(&reqObj->hostRequest->sem);
        return;
    }
}

static void UsbdWriteCallback(struct UsbRequest *req)
{
    if (req == NULL) {
        HDF_LOGE("%{public}s:%{pulib}d req is NULL!", __func__, __LINE__);
        return;
    }
    int32_t status = req->compInfo.status;
    HDF_LOGI("%{public}s:%{pulib}d statue is %d!", __func__, __LINE__, status);
    return;
}

static UsbInterfaceHandle *InterfaceIdToHandle(const struct HostDevice *dev, uint8_t id)
{
    UsbInterfaceHandle *devHandle = NULL;

    if (id == 0xFF) {
        devHandle = dev->ctrDevHandle;
    } else if (id < USB_MAX_INTERFACES) {
        return dev->devHandle[id];
    }
    return devHandle;
}

static int32_t UsbControlSetUp(struct UsbControlParams *controlParams, struct UsbControlRequest *controlReq)
{
    if (controlParams == NULL || controlReq == NULL) {
        HDF_LOGE("%{public}s:%{public}d param is null", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    controlReq->target = controlParams->target;
    controlReq->reqType = controlParams->reqType;
    controlReq->directon = controlParams->directon;
    controlReq->request = controlParams->request;
    controlReq->value = CpuToLe16(controlParams->value);
    controlReq->index = CpuToLe16(controlParams->index);
    controlReq->buffer = controlParams->data;
    controlReq->length = CpuToLe16(controlParams->size);
    return HDF_SUCCESS;
}

static struct UsbInterface *GetUsbInterfaceById(const struct HostDevice *dev, uint8_t interfaceIndex)
{
    if (!dev || (dev->service == NULL)) {
        HDF_LOGE("%{public}s:%{public}d idx:%{public}hhu service is null", __func__, __LINE__, interfaceIndex);
        return NULL;
    }
    struct UsbInterface *tmpIf = UsbClaimInterface(dev->service->session, dev->busNum, dev->devAddr, interfaceIndex);
    return tmpIf;
}

static int32_t GetInterfacePipe(const struct HostDevice *dev, struct UsbInterface *interface, uint8_t pipeAddr,
    struct UsbPipeInfo *pipe)
{
    int32_t ret = HDF_FAILURE;
    struct UsbInterfaceInfo *info = NULL;
    UsbInterfaceHandle *interfaceHandle = NULL;
    struct UsbPipeInfo pipeTmp;
    if (memset_s(&pipeTmp, sizeof(pipeTmp), 0, sizeof(pipeTmp)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (dev == NULL || interface == NULL || pipe == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid parmas", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    info = &interface->info;
    if (info == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid interface", __func__, __LINE__);
        return HDF_FAILURE;
    }

    interfaceHandle = InterfaceIdToHandle(dev, info->interfaceIndex);
    if (interfaceHandle == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid interface handle", __func__, __LINE__);
        return HDF_FAILURE;
    }

    for (uint8_t i = 1; i <= info->pipeNum; ++i) {
        ret = UsbGetPipeInfo(interfaceHandle, info->curAltSetting, i, &pipeTmp);
        if ((ret == HDF_SUCCESS) && ((pipeTmp.pipeAddress | (uint8_t)pipeTmp.pipeDirection) == pipeAddr)) {
            if (pipe) {
                *pipe = pipeTmp;
            }
            return HDF_SUCCESS;
        }
    }
    return HDF_FAILURE;
}

static int32_t GetPipe(const struct HostDevice *dev, uint8_t interfaceId, uint8_t pipeId, struct UsbPipeInfo *pipe)
{
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid params ifId:%{public}hhu epId:%{public}d", __func__, __LINE__,
                 interfaceId, pipeId);
        return HDF_ERR_INVALID_PARAM;
    }
    if (interfaceId > USB_MAX_INTERFACES) {
        HDF_LOGE("%{public}s:%{public}d invalid params ifId:%{public}d epId:%{public}d", __func__, __LINE__,
                 interfaceId, pipeId);
        return HDF_ERR_INVALID_PARAM;
    }
    struct UsbInterface *interface = dev->iface[interfaceId];
    if (interface == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid interface ifId:%{public}hhu, epId:%{public}d",
            __func__, __LINE__, interfaceId, pipeId);
        return HDF_FAILURE;
    }

    int32_t ret = GetInterfacePipe(dev, interface, pipeId, pipe);
    return ret;
}

static void UsbdFreeCtrlPipe(struct HostDevice *dev)
{
    if (dev == NULL) {
        HDF_LOGW("%{public}s:%{public}d params dev is null", __func__, __LINE__);
        return;
    }
    OsalMemFree(dev->ctrPipe);
    dev->ctrPipe = NULL;
}

static int32_t UsbdGetCtrlPipe(struct HostDevice *dev)
{
    struct UsbPipeInfo *pipe = (struct UsbPipeInfo *)OsalMemAlloc(sizeof(struct UsbPipeInfo));
    if (pipe == NULL) {
        HDF_LOGE("%{public}s:%{public}d OsalMemAlloc failed", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (memset_s(pipe, sizeof(struct UsbPipeInfo), 0, sizeof(struct UsbPipeInfo)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed ", __func__, __LINE__);
        OsalMemFree(pipe);
        return HDF_FAILURE;
    }
    int32_t ret = UsbGetPipeInfo(dev->ctrDevHandle, dev->ctrIface->info.curAltSetting, 0, pipe);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d get pipe failed ret = %{public}d", __func__, __LINE__, ret);
        OsalMemFree(pipe);
        pipe = NULL;
        return HDF_FAILURE;
    }
    dev->ctrPipe = pipe;

    return HDF_SUCCESS;
}

static int32_t UsbdBulkReadSyncGetParams(struct HdfSBuf *data, uint8_t *ifId, uint8_t *pipeId, int32_t *timeout)
{
    if ((ifId == NULL) || (data == NULL) || (pipeId == NULL) || (timeout == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint8(data, ifId)) {
        HDF_LOGE("%{public}s:%{public}d read interfaceId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadUint8(data, pipeId)) {
        HDF_LOGE("%{public}s:%{public}d read pipeId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, timeout)) {
        HDF_LOGE("%{public}s:%{public}d read timeout error", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    return HDF_SUCCESS;
}

static int32_t UsbdBulkReadSyncBase(int32_t timeout, uint8_t *buffer, uint32_t size, uint32_t *actlength,
    struct UsbdRequestSync *requestSync)
{
    int32_t ret = HDF_FAILURE;
    if ((!requestSync) || (!buffer) || (!actlength) || (size < 1) || (!requestSync->request)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    uint64_t intTimeout = timeout < 0 ? 0 : timeout;
    int32_t timeonce = 500;
    uint64_t stime = OsalGetSysTimeMs();
    uint64_t ntime = 0;
    uint32_t tcur = 0;
    uint32_t msize = 0;
    OsalMutexLock(&requestSync->lock);
    requestSync->params.timeout = timeonce;
    msize = requestSync->pipe.maxPacketSize;
    while (tcur + msize < size) {
        ret = UsbFillRequest(requestSync->request, requestSync->ifHandle, &requestSync->params);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s: UsbFillRequest failed, ret=%{public}d \n", __func__, ret);
            break;
        }
        ret = UsbSubmitRequestSync(requestSync->request);
        if (HDF_SUCCESS == ret) {
            memcpy_s(buffer + tcur, size - tcur, requestSync->request->compInfo.buffer,
                     requestSync->request->compInfo.actualLength);
            tcur += requestSync->request->compInfo.actualLength;
        } else if (HDF_ERR_TIMEOUT == ret) {
            if (tcur > 0) {
                ret = HDF_SUCCESS;
                break;
            }
            ntime = OsalGetSysTimeMs();
            if ((intTimeout == 0) || (ntime < stime + intTimeout)) {
                continue;
            } else {
                HDF_LOGE("%{public}s:%{public}d tcur:%{public}d ret:%{public}d", __func__, __LINE__, tcur, ret);
                break;
            }
        } else {
            HDF_LOGE("%{public}s:%{public}d tcur:%{public}d ret:%{public}d", __func__, __LINE__, tcur, ret);
            break;
        }
    }
    OsalMutexUnlock(&requestSync->lock);
    *actlength = tcur;
    return ret;
}

static struct UsbdRequestSync *UsbdFindRequestSync(struct HostDevice *port, uint8_t interfaceId, uint8_t pipeAddr)
{
    struct UsbdRequestSync *req = NULL;
    struct HdfSListIterator it;
    bool flag = false;
    if (!port) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return NULL;
    }
    OsalMutexLock(&port->reqSyncLock);
    HdfSListIteratorInit(&it, &port->reqSyncList);
    while (HdfSListIteratorHasNext(&it)) {
        req = (struct UsbdRequestSync *)HdfSListIteratorNext(&it);
        if (!req) {
            continue;
        }
        if ((req->pipe.interfaceId == interfaceId) && ((req->pipe.pipeAddress | req->pipe.pipeDirection) == pipeAddr)) {
            flag = true;
            break;
        }
    }
    OsalMutexUnlock(&port->reqSyncLock);

    if (flag) {
        return req;
    }
    HDF_LOGE("%{public}s:%{public}d req null", __func__, __LINE__);
    return NULL;
}

static struct UsbdRequestSync *UsbdRequestSyncAlloc(void)
{
    struct UsbdRequestSync *req = NULL;
    req = (struct UsbdRequestSync *)OsalMemAlloc(sizeof(struct UsbdRequestSync));
    if (req) {
        if (memset_s(req, sizeof(struct UsbdRequestSync), 0, sizeof(struct UsbdRequestSync)) != EOK) {
            HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
            OsalMemFree(req);
            return NULL;
        }
        req->request = NULL;
        req->endPointAddr = 0;
        req->ifHandle = NULL;
        OsalMutexInit(&req->lock);
    }
    return req;
}

static void UsbRequestParamsWSyncInit(struct UsbRequestParams *parmas, int32_t timeout, const struct UsbPipeInfo *pipe)
{
    if (parmas == NULL || pipe == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return;
    }
    parmas->interfaceId = pipe->interfaceId;
    parmas->pipeAddress = pipe->pipeDirection | pipe->pipeAddress;
    parmas->pipeId = pipe->pipeId;
    parmas->requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    parmas->timeout = timeout;
    parmas->dataReq.numIsoPackets = 0;

    parmas->dataReq.directon = (pipe->pipeDirection >> USB_DIR_OFFSET) & 0x1;
    parmas->dataReq.length = pipe->maxPacketSize;
    return;
}

static int32_t UsbdRequestSyncInit(struct HostDevice *port, UsbInterfaceHandle *ifHandle,
    struct UsbPipeInfo *pipe, struct UsbdRequestSync *requestSync)
{
    int32_t ret = HDF_SUCCESS;
    if ((port == NULL) || (requestSync == NULL) || (ifHandle == NULL) || (pipe == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    memcpy_s(&requestSync->pipe, sizeof(struct UsbPipeInfo), pipe, sizeof(struct UsbPipeInfo));
    requestSync->ifHandle = ifHandle;
    requestSync->request = UsbAllocRequest(requestSync->ifHandle, 0, requestSync->pipe.maxPacketSize);
    if (!requestSync->request) {
        HDF_LOGE("%{public}s:%{public}d alloc request failed\n", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    UsbRequestParamsWSyncInit(&requestSync->params, USB_CTRL_SET_TIMEOUT, &requestSync->pipe);
    requestSync->params.userData = port;
    OsalMutexLock(&port->reqSyncLock);
    HdfSListAdd(&port->reqSyncList, &requestSync->node);
    OsalMutexUnlock(&port->reqSyncLock);
    return ret;
}

static int32_t UsbdRequestSyncRelease(struct UsbdRequestSync *requestSync)
{
    int32_t ret = HDF_SUCCESS;
    if (requestSync) {
        OsalMutexLock(&requestSync->lock);
        if (requestSync->request) {
            ret = UsbFreeRequest(requestSync->request);
            if (HDF_SUCCESS != ret) {
                HDF_LOGW("%{public}s:%{public}d UsbFreeRequest failed", __func__, __LINE__);
            }
            requestSync->request = NULL;
        }
        OsalMutexUnlock(&requestSync->lock);
        OsalMemFree(requestSync);
    }
    return ret;
}

static int32_t UsbdFindRequestSyncAndCreat(struct HostDevice *port, uint8_t interfaceId,
    uint8_t pipeAddr, struct UsbdRequestSync **request)
{
    int32_t ret = HDF_SUCCESS;
    if ((!port) || (!request)) {
        HDF_LOGE("%{public}s:%{public}d ifId:%{public}hhu, epId:%{public}d", __func__, __LINE__, interfaceId, pipeAddr);
        return HDF_ERR_INVALID_PARAM;
    }
    *request = NULL;
    struct UsbdRequestSync *requestSync = UsbdFindRequestSync(port, interfaceId, pipeAddr);
    if (!requestSync) {
        UsbInterfaceHandle *ifHandle = NULL;
        struct UsbPipeInfo pipe;
        if (memset_s(&pipe, sizeof(struct UsbPipeInfo), 0, sizeof(struct UsbPipeInfo)) != EOK) {
            HDF_LOGE("%{public}s:%{public}d  memset_s failed ", __func__, __LINE__);
            return HDF_FAILURE;
        }
        ret = GetPipe(port, interfaceId, pipeAddr, &pipe);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: get pipe failed ifId:%{public}hhu, epId:%{public}d", __func__, interfaceId, pipeAddr);
            return HDF_FAILURE;
        }
        ifHandle = InterfaceIdToHandle(port, interfaceId);
        if (ifHandle == NULL) {
            HDF_LOGE("%{public}s:%{public}d get interface handle failed \n", __func__, __LINE__);
            return HDF_ERR_INVALID_PARAM;
        }
        requestSync = UsbdRequestSyncAlloc();
        if (!requestSync) {
            HDF_LOGE("%{public}s:%{public}d fail ifId:%{public}hhu pipeId:%{public}d", __func__, __LINE__, interfaceId,
                     pipeAddr);
            return HDF_ERR_MALLOC_FAIL;
        }
        ret = UsbdRequestSyncInit(port, ifHandle, &pipe, requestSync);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s:%{public}d UsbdRequestSyncInit fail ret:%{public}d", __func__, __LINE__, ret);
            int32_t tRet = UsbdRequestSyncRelease(requestSync);
            requestSync = NULL;
            if (HDF_SUCCESS != tRet) {
                HDF_LOGE("%{public}s:%{public}d Release fail ifId:%{public}hhu pipeId:%{public}d", __func__, __LINE__,
                         interfaceId, pipeAddr);
            }
            return ret;
        }
    }
    *request = requestSync;
    return ret;
}

static int32_t FunBulkReadSync(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t interfaceId = 0, pipeId = 0;
    int32_t timeout = 0;
    uint8_t tbuf[READ_BUF_SIZE] = {0};
    uint32_t tsize = READ_BUF_SIZE;
    uint32_t actlength = 0;
    struct UsbdRequestSync *requestSync = NULL;

    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d mangf invalid parma", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = UsbdBulkReadSyncGetParams(data, &interfaceId, &pipeId, &timeout);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d UsbdBulkReadSyncGetParams error:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbdFindRequestSyncAndCreat(port, interfaceId, pipeId, &requestSync);
    if ((HDF_SUCCESS != ret) || (!requestSync)) {
        HDF_LOGE("%{public}s:%{public}d read timeout error", __func__, __LINE__);
        return ret;
    }
    if ((requestSync->pipe.pipeDirection != USB_PIPE_DIRECTION_IN) ||
        (requestSync->pipe.pipeType != USB_PIPE_TYPE_BULK)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbdBulkReadSyncBase(timeout, tbuf, tsize, &actlength, requestSync);
    if (HDF_SUCCESS != ret) {
        HDF_LOGW(
            "%{public}s:%{public}d UsbdBulkReadSyncBase ret:%{public}d "
            "len:%{public}d",
            __func__, __LINE__, ret, actlength);
    }
    if (actlength > 0) {
        if (!UsbdHdfWriteBuf(reply, tbuf, actlength)) {
            HDF_LOGE("%{public}s:%{public}d sbuf write buffer failed:%{public}u", __func__, __LINE__, actlength);
            ret = HDF_ERR_IO;
        } else {
            ret = HDF_SUCCESS;
        }
    }
    return ret;
}

static int32_t FunBulkWriteSyncGetParam(struct HdfSBuf *data, uint8_t *interfaceId, uint8_t *pipeId, int32_t *timeout)
{
    if (!HdfSbufReadUint8(data, interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadUint8(data, pipeId)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, timeout)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t UsbdRequestSyncReleaseList(struct HostDevice *port)
{
    int32_t ret = HDF_SUCCESS;
    struct UsbdRequestSync *req = NULL;
    struct HdfSListIterator it;
    if (!port) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&port->reqSyncLock);
    HdfSListIteratorInit(&it, &port->reqSyncList);
    while (HdfSListIteratorHasNext(&it)) {
        req = (struct UsbdRequestSync *)HdfSListIteratorNext(&it);
        if (!req) {
            continue;
        }
        HdfSListIteratorRemove(&it);
        ret = UsbdRequestSyncRelease(req);
        req = NULL;
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s:%{public}d UsbdRequestSyncRelease failed\n", __func__, __LINE__);
        }
    }
    OsalMutexUnlock(&port->reqSyncLock);

    return HDF_SUCCESS;
}

static int32_t UsbdBulkWriteSyncBase(struct HostDevice *port, struct UsbdRequestSync *requestSync,
    const uint8_t *buffer, uint32_t length, int32_t timeout)
{
    int32_t ret = HDF_FAILURE;
    if ((!port) || (!requestSync) || (!buffer) || (length < 1)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock(&requestSync->lock);
    int32_t initTimeout = timeout < 0 ? 0 : timeout;
    requestSync->params.timeout = initTimeout;
    requestSync->params.userData = port;
    uint16_t tsize = requestSync->pipe.maxPacketSize;
    uint16_t tcur = 0;
    uint16_t msize = requestSync->pipe.maxPacketSize;
    while (tcur < length) {
        tsize = (length - tcur) < msize ? (length - tcur) : msize;
        requestSync->params.dataReq.buffer = (unsigned char *)(buffer + tcur);
        requestSync->params.dataReq.length = tsize;
        tcur += tsize;
        ret = UsbFillRequest(requestSync->request, requestSync->ifHandle, &requestSync->params);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE(
                "%{public}s:%{public}d UsbFillRequest failed, ret=%{public}d "
                "handle:%{public}p tcur:%{public}d "
                "length:%{public}u\n",
                __func__, __LINE__, ret, requestSync->ifHandle, tcur, length);
            break;
        }
        ret = UsbSubmitRequestSync(requestSync->request);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE(
                "%{public}s:%{public}d UsbSubmitRequestSync failed, "
                "ret=%{public}d tcur:%{public}d length:%{public}u\n",
                __func__, __LINE__, ret, tcur, length);
            break;
        }
    }
    OsalMutexUnlock(&requestSync->lock);
    return ret;
}

static int32_t FunBulkWriteSync(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t *buffer = NULL;
    uint8_t interfaceId = 0;
    uint8_t pipeId = 0;
    int32_t timeout = 0;
    uint32_t length = 0;
    struct UsbdRequestSync *requestSync = NULL;
    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = FunBulkWriteSyncGetParam(data, &interfaceId, &pipeId, &timeout);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadBuffer(data, (const void **)&buffer, &length)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    ret = UsbdFindRequestSyncAndCreat(port, interfaceId, pipeId, &requestSync);
    if ((HDF_SUCCESS != ret) || (!requestSync)) {
        HDF_LOGE("%{public}s:%{public}d read timeout error", __func__, __LINE__);
        return ret;
    }
    if ((requestSync->pipe.pipeDirection != USB_PIPE_DIRECTION_OUT) ||
        (requestSync->pipe.pipeType != USB_PIPE_TYPE_BULK)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbdBulkWriteSyncBase(port, requestSync, buffer, length, timeout);
    return ret;
}

static void UsbRequestParamsInit(struct UsbRequestParams *parmas, int32_t timeout)
{
    if (parmas == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return;
    }
    parmas->interfaceId = USB_CTRL_INTERFACE_ID;
    parmas->pipeAddress = 0;
    parmas->pipeId = 0;
    parmas->requestType = USB_REQUEST_PARAMS_CTRL_TYPE;
    parmas->timeout = timeout;
    return;
}

static int32_t UsbControlTransferEx(struct HostDevice *dev, struct UsbControlParams *ctrParams, int32_t timeout)
{
    struct UsbRequest *request = NULL;
    struct UsbRequestParams parmas = {};

    if (memset_s(&parmas, sizeof(parmas), 0, sizeof(parmas)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if ((dev == NULL) || (dev->ctrDevHandle == NULL) || (ctrParams == NULL) || (ctrParams->data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d null pointer failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!dev->ctrlReq) {
        request = UsbAllocRequest(dev->ctrDevHandle, 0, MAX_CONTROL_BUFF_SIZE);
        if (!request) {
            HDF_LOGE("%{public}s:%{public}d UsbAllocRequest alloc request failed\n", __func__, __LINE__);
            return HDF_ERR_MALLOC_FAIL;
        }
        dev->ctrlReq = request;
    } else {
        request = dev->ctrlReq;
    }

    UsbRequestParamsInit(&parmas, timeout);

    int32_t ret = UsbControlSetUp(ctrParams, &parmas.ctrlReq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d UsbControlSetUp failed, ret=%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    OsalMutexLock(&dev->lock);
    do {
        ret = UsbFillRequest(request, dev->ctrDevHandle, &parmas);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s:%{public}d UsbFillRequest failed, ret=%{public}d", __func__, __LINE__, ret);
            break;
        }

        ret = UsbSubmitRequestSync(request);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s:%{public}d UsbSubmitRequestSync failed, ret=%{public}d", __func__, __LINE__, ret);
            OsalMSleep(SUBMIT_SLEEP_TIME);
            break;
        }

        if (USB_REQUEST_DIR_FROM_DEVICE == ctrParams->directon) {
            memcpy_s(ctrParams->data, ctrParams->size, request->compInfo.buffer, request->compInfo.actualLength);
            ctrParams->size =
                ctrParams->size < request->compInfo.actualLength ? ctrParams->size : request->compInfo.actualLength;
        }
    } while (0);

    OsalMutexUnlock(&dev->lock);
    return ret;
}

static int32_t CtrlTranParamGetReqType(struct HdfSBuf *data, struct UsbControlParams *pCtrParams, int32_t requestType)
{
    if (data == NULL || pCtrParams == NULL) {
        HDF_LOGE("%{public}s:%{public}d param failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    uint8_t *buffer = NULL;
    uint32_t length = 0;

    int32_t target = requestType & USB_RECIP_MASK;
    int32_t direction = (requestType >> DIRECTION_OFFSET_7) & ENDPOINT_DIRECTION_MASK;
    int32_t cmdType = ((uint32_t)(requestType >> CMD_OFFSET_5)) & CMD_TYPE_MASK;
    if (direction == USB_REQUEST_DIR_TO_DEVICE) {
        if (!HdfSbufReadBuffer(data, (const void **)&buffer, &length)) {
            HDF_LOGE("%{public}s:%{public}d hdf sbuf Read failed", __func__, __LINE__);
            return HDF_FAILURE;
        }
    } else {
        length = MAX_CONTROL_BUFF_SIZE;
        buffer = (uint8_t *)OsalMemAlloc(length);
        if (buffer == NULL) {
            HDF_LOGE("%{public}s:%{public}d OsalMemAlloc failed length = %{public}u", __func__, __LINE__, length);
            return HDF_ERR_MALLOC_FAIL;
        }
        if (memset_s(buffer, length, 0, length) != EOK) {
            HDF_LOGE("%{public}s:%{public}d  memset_s failed ", __func__, __LINE__);
            OsalMemFree(buffer);
            return HDF_FAILURE;
        }
    }
    pCtrParams->target = target;
    pCtrParams->directon = direction;
    pCtrParams->reqType = cmdType;
    pCtrParams->size = length;
    pCtrParams->data = buffer;

    return HDF_SUCCESS;
}

static int32_t CtrlTransferParamInit(struct HdfSBuf *data, struct UsbControlParams *pCtrParams, int32_t *timeout)
{
    if (data == NULL || pCtrParams == NULL) {
        HDF_LOGE("%{public}s:%{public}d param failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t requestCmd = 0;
    int32_t requestType = 0;
    int32_t value = 0;
    int32_t index = 0;

    if (!HdfSbufReadInt32(data, &requestType)) {
        HDF_LOGE("%{public}s:%{public}d read param failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, &requestCmd)) {
        HDF_LOGE("%{public}s:%{public}d read param failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, &value)) {
        HDF_LOGE("%{public}s:%{public}d read param failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, &index)) {
        HDF_LOGE("%{public}s:%{public}d read param failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, timeout)) {
        HDF_LOGE("%{public}s:%{public}d read param failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    pCtrParams->request = (uint8_t)requestCmd;
    pCtrParams->value = value;
    pCtrParams->index = index;

    int32_t ret = CtrlTranParamGetReqType(data, pCtrParams, requestType);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d CtrlTransferParamInit failed:%{public}d", __func__, __LINE__, ret);
        OsalMemFree(pCtrParams->data);
        pCtrParams->data = NULL;
    }
    return ret;
}

static int32_t FunControlTransfer(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t timeout = 0;
    int32_t ret = HDF_SUCCESS;

    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (port->ctrDevHandle == NULL) {
        HDF_LOGE("%{public}s:%{public}d interface handle is null \n", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct UsbControlParams controlParams = {};
    if (memset_s(&controlParams, sizeof(controlParams), 0, sizeof(controlParams)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    ret = CtrlTransferParamInit(data, &controlParams, &timeout);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d CtrlTransferParamInit failed:%{public}d", __func__, __LINE__, ret);
        OsalMemFree(controlParams.data);
        controlParams.data = NULL;
        return ret;
    }

    ret = UsbControlTransferEx(port, &controlParams, timeout);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d UsbControlTransfer failed ret:%{public}d\n", __func__, __LINE__, ret);
    }
    if (controlParams.directon == USB_REQUEST_DIR_FROM_DEVICE) {
        if ((HDF_SUCCESS == ret) && (!UsbdHdfWriteBuf(reply, (uint8_t *)controlParams.data, controlParams.size))) {
            HDF_LOGE("%{public}s:%{public}d sbuf write buffer failed", __func__, __LINE__);
        }
        OsalMemFree(controlParams.data);
        controlParams.data = NULL;
    }
    return ret;
}

static void UsbdReleaseInterfaces(struct HostDevice *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{public}d: invalid param", __func__, __LINE__);
        return;
    }

    for (int32_t i = 0; i < USB_MAX_INTERFACES; ++i) {
        if (dev->iface[i]) {
            UsbReleaseInterface(dev->iface[i]);
            dev->iface[i] = NULL;
        }
    }
    if (dev->ctrIface) {
        UsbReleaseInterface(dev->ctrIface);
        dev->ctrIface = NULL;
    }
}

static void UsbdCloseInterfaces(struct HostDevice *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{public}d: invalid param", __func__, __LINE__);
        return;
    }

    for (int32_t i = 0; i < USB_MAX_INTERFACES; ++i) {
        if (dev->devHandle[i]) {
            UsbCloseInterface(dev->devHandle[i]);
            dev->devHandle[i] = NULL;
        }
    }
    if (dev->ctrDevHandle) {
        UsbCloseInterface(dev->ctrDevHandle);
        dev->ctrDevHandle = NULL;
    }
}

static int32_t FunClaimInterface(struct HostDevice *port, struct HdfSBuf *data)
{
    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint8_t interfaceId;
    uint8_t force = 1;
    if (!HdfSbufReadUint8(data, &interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d sbuf read interfaceNum failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &force)) {
        HDF_LOGE("%{public}s:%{public}d sbuf read interfaceNum failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (interfaceId >= USB_MAX_INTERFACES) {
        HDF_LOGE("%{public}s:%{public}d interfaceId larger then max num", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (port->iface[interfaceId] == NULL) {
        if (force) {
            port->iface[interfaceId] = UsbClaimInterface(
                port->service->session, port->busNum, port->devAddr, interfaceId);
        } else {
            port->iface[interfaceId] = (struct UsbInterface *)UsbClaimInterfaceUnforce(
                port->service->session, port->busNum, port->devAddr, interfaceId);
        }
        if (port->iface[interfaceId] == NULL) {
            HDF_LOGE(
                "%{public}s:%{public}d UsbClaimInterface failed id = "
                "%{public}d, busNum=%{public}d, devAddr=%{public}d",
                __func__, __LINE__, interfaceId, (int32_t)port->busNum, (int32_t)port->devAddr);
            return HDF_FAILURE;
        }
    }
    if (port->devHandle[interfaceId] == NULL) {
        port->devHandle[interfaceId] = UsbOpenInterface(port->iface[interfaceId]);
        if (port->devHandle[interfaceId] == NULL) {
            HDF_LOGE("%{public}s:%{public}d UsbOpenInterface failed id = %{public}hhu",
                __func__, __LINE__, interfaceId);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t FunReleaseInterface(struct HostDevice *port, struct HdfSBuf *data)
{
    int32_t ret = HDF_SUCCESS;
    uint8_t interfaceId = 0;
    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint8(data, &interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d sbuf read interfaceNum failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (interfaceId >= USB_MAX_INTERFACES) {
        HDF_LOGE("%{public}s:%{public}d interfaceId:%{public}hhu failed", __func__, __LINE__, interfaceId);
        return HDF_ERR_INVALID_PARAM;
    }
    return ret;
}

static int32_t UsbdOpenInterfaces(struct HostDevice *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{public}d: invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (memset_s(dev->devHandle, sizeof(uint8_t) * USB_MAX_INTERFACES, 0, sizeof(uint8_t) * USB_MAX_INTERFACES) !=
        EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    dev->ctrDevHandle = UsbOpenInterface(dev->ctrIface);
    if (dev->ctrDevHandle == NULL) {
        HDF_LOGE("%{public}s:%{public}d ctrDevHandle UsbOpenInterface null", __func__, __LINE__);
        UsbdCloseInterfaces(dev);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void MakeUsbControlParams(struct UsbControlParams *controlParams, uint8_t *buffer, uint32_t *length,
    uint16_t value, uint16_t index)
{
    controlParams->request = USB_DDK_REQ_GET_DESCRIPTOR;
    controlParams->target = USB_REQUEST_TARGET_DEVICE;
    controlParams->reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams->directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams->value = value;
    controlParams->index = index;
    controlParams->data = buffer;
    controlParams->size = *length;
}

static int32_t FunGetDeviceDescriptor(struct HostDevice *port, struct HdfSBuf *reply)
{
    if ((port == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t length = MAX_CONTROL_BUFF_SIZE;
    uint8_t buffer[USB_MAX_DESCRIPTOR_SIZE] = {0};
    struct UsbControlParams controlParams = {};
    MakeUsbControlParams(&controlParams, buffer, &length, (int32_t)USB_DDK_DT_DEVICE << TYPE_OFFSET_8, 0);
    int32_t ret = UsbControlTransferEx(port, &controlParams, USB_CTRL_SET_TIMEOUT);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    if (!UsbdHdfWriteBuf(reply, buffer, controlParams.size)) {
        HDF_LOGE("%{public}s:%{public}d WriteBuffer failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    return ret;
}

static int32_t FunGetConfigDescriptor(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t configId = 0;

    if ((port == NULL) || (reply == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint8(data, &configId)) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t length = MAX_CONTROL_BUFF_SIZE;
    uint8_t buffer[USB_MAX_DESCRIPTOR_SIZE] = {0};
    struct UsbControlParams controlParams = {};
    MakeUsbControlParams(&controlParams, buffer, &length, ((int32_t)USB_DDK_DT_CONFIG << TYPE_OFFSET_8) + configId, 0);
    int32_t ret = UsbControlTransferEx(port, &controlParams, USB_CTRL_SET_TIMEOUT);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);

        return ret;
    }
    if (!UsbdHdfWriteBuf(reply, buffer, controlParams.size)) {
        HDF_LOGE("%{public}s:%{public}d WriteBuffer failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    return ret;
}

static int32_t FunGetStringDescriptor(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t stringId = 0;

    if ((port == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint8(data, &stringId)) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t length = MAX_CONTROL_BUFF_SIZE;
    uint8_t buffer[USB_MAX_DESCRIPTOR_SIZE] = {0};
    struct UsbControlParams controlParams = {};
    MakeUsbControlParams(&controlParams, buffer, &length, ((int32_t)USB_DDK_DT_STRING << TYPE_OFFSET_8) + stringId, 0);
    int32_t ret = UsbControlTransferEx(port, &controlParams, GET_STRING_SET_TIMEOUT);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d failed ret=%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    if (!UsbdHdfWriteBuf(reply, buffer, controlParams.size)) {
        HDF_LOGE("%{public}s:%{public}d WriteBuffer failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    return ret;
}

void MakeGetActiveUsbControlParams(struct UsbControlParams *controlParams, uint8_t *buffer,
    uint32_t *length, uint16_t value, uint16_t index)
{
    controlParams->request = USB_DDK_REQ_GET_CONFIGURATION;
    controlParams->target = USB_REQUEST_TARGET_DEVICE;
    controlParams->reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams->directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams->value = value;
    controlParams->index = index;
    controlParams->data = buffer;
    controlParams->size = *length;
}

static int32_t FunGetActiveConfig(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t configId = 0;
    uint32_t length = 1;

    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct UsbControlParams controlParams = {};
    MakeGetActiveUsbControlParams(&controlParams, &configId, &length, 0, 0);
    int32_t ret = UsbControlTransferEx(port, &controlParams, USB_CTRL_SET_TIMEOUT);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufWriteUint8(reply, configId)) {
        HDF_LOGE("%{public}s:%{public}d WriteBuffer failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    return ret;
}

static void RemoveDevFromService(struct UsbdService *service, struct HostDevice *port)
{
    struct HdfSListIterator it;
    struct HostDevice *tempPort = NULL;
    if (!service || !port) {
        return;
    }

    OsalMutexLock(&service->lock);
    HdfSListIteratorInit(&it, &service->devList);
    while (HdfSListIteratorHasNext(&it)) {
        tempPort = (struct HostDevice *)HdfSListIteratorNext(&it);
        if (!tempPort) {
            continue;
        }
        if ((tempPort->busNum == port->busNum) && (tempPort->devAddr == port->devAddr)) {
            HdfSListIteratorRemove(&it);
            break;
        }
    }
    OsalMutexUnlock(&service->lock);

    return;
}

static int32_t UsbdClaimInterfaces(struct HostDevice *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{public}d: invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (memset_s(dev->iface, sizeof(uint8_t) * USB_MAX_INTERFACES, 0, sizeof(uint8_t) * USB_MAX_INTERFACES) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failded", __func__, __LINE__);
        return HDF_FAILURE;
    }
    dev->ctrIface = GetUsbInterfaceById((const struct HostDevice *)dev, USB_CTRL_INTERFACE_ID);
    if (dev->ctrIface == NULL) {
        HDF_LOGE("%{public}s:%{public}d GetUsbInterfaceById null", __func__, __LINE__);
        UsbdReleaseInterfaces(dev);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t ReturnGetPipes(int32_t ret, struct HostDevice *dev)
{
    UsbdCloseInterfaces(dev);
    UsbdReleaseInterfaces(dev);
    UsbExitHostSdk(dev->service->session);
    dev->service->session = NULL;
    return ret;
}

static int32_t ReturnOpenInterfaces(int32_t ret, struct HostDevice *dev)
{
    UsbdReleaseInterfaces(dev);
    UsbExitHostSdk(dev->service->session);
    dev->service->session = NULL;
    return ret;
}

static int32_t ReturnClainInterfaces(int32_t ret, struct HostDevice *dev)
{
    UsbExitHostSdk(dev->service->session);
    dev->service->session = NULL;
    return ret;
}

static int32_t UsbdInit(struct HostDevice *dev)
{
    struct UsbSession *session = NULL;
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{public}d: invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (dev->initFlag == true) {
        HDF_LOGE("%{public}s:%{public}d: initFlag is true", __func__, __LINE__);
        return HDF_SUCCESS;
    }
    int32_t ret = UsbInitHostSdk(NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d UsbInitHostSdk failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (dev->service) {
        dev->service->session = session;
    }

    ret = UsbdClaimInterfaces(dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d UsbdClaimInterfaces failed ret:%{public}d", __func__, __LINE__, ret);
        return ReturnClainInterfaces(ret, dev);
    }
    ret = UsbdOpenInterfaces(dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d UsbdOpenInterfaces failed ret:%{public}d", __func__, __LINE__, ret);
        return ReturnOpenInterfaces(ret, dev);
    }
    ret = UsbdGetCtrlPipe(dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d UsbdGetPipes failed ret:%{public}d", __func__, __LINE__, ret);
        return ReturnGetPipes(ret, dev);
    }

    return HDF_SUCCESS;
}

static int32_t UsbdRequestASyncReleaseData(struct UsbdRequestASync *request)
{
    if (request) {
        if (request->reqMsg.buffer) {
            HDF_LOGI("%{public}s:%{public}d free buffer\n", __func__, __LINE__);
            OsalMemFree(request->reqMsg.buffer);
            request->reqMsg.buffer = NULL;
        }
        request->reqMsg.length = 0;
        if (request->reqMsg.clientData) {
            HDF_LOGI("%{public}s:%{public}d free clientData\n", __func__, __LINE__);
            OsalMemFree(request->reqMsg.clientData);
            request->reqMsg.clientData = NULL;
        }
        request->reqMsg.clientLength = 0;
        request->status = 0;
    }
    return HDF_SUCCESS;
}

static int32_t UsbdRequestASyncRelease(struct UsbdRequestASync *request)
{
    int32_t ret = HDF_SUCCESS;
    if (request) {
        OsalMutexLock(&request->lock);
        UsbdRequestASyncReleaseData(request);
        if (request->reqMsg.request) {
            ret = UsbFreeRequest(request->reqMsg.request);
            request->reqMsg.request = NULL;
            if (HDF_SUCCESS != ret) {
                HDF_LOGE("%{public}s:%{public}d UsbFreeRequest failed", __func__, __LINE__);
            }
        }
        OsalMutexUnlock(&request->lock);
        OsalMemFree(request);
    }
    return ret;
}

static int32_t UsbdRequestASyncReleaseList(struct HostDevice *port)
{
    int32_t ret = HDF_SUCCESS;
    struct UsbdRequestASync *req = NULL;
    struct HdfSListIterator it;
    if (!port) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&port->reqASyncLock);
    HdfSListIteratorInit(&it, &port->reqASyncList);
    while (HdfSListIteratorHasNext(&it)) {
        req = (struct UsbdRequestASync *)HdfSListIteratorNext(&it);
        if (!req) {
            continue;
        }
        HdfSListIteratorRemove(&it);
        ret = UsbdRequestASyncRelease(req);
        req = NULL;
        if (HDF_SUCCESS != ret) {
            HDF_LOGW("%{public}s:%{public}d UsbdRequestASyncRelease failed\n", __func__, __LINE__);
        }
    }
    OsalMutexUnlock(&port->reqASyncLock);

    return HDF_SUCCESS;
}

static int32_t UsbdBulkASyncReqRelease(struct UsbdBulkASyncReqList *list)
{
    for (int32_t i = 0; i < USBD_BULKASYNCREQ_NUM_MAX; ++i) {
        UsbFreeRequest(list->node[i].request);
        list->node[i].request = NULL;
    }
    DListHeadInit(&list->eList);
    DListHeadInit(&list->uList);
    OsalMutexDestroy(&list->elock);
    OsalMutexDestroy(&list->ulock);

    return HDF_SUCCESS;
}

static int32_t UsbdBulkASyncListRelease(struct UsbdBulkASyncList *list)
{
    UsbdBulkASyncReqRelease(&list->rList);
    OsalMutexDestroy(&list->asmHandle.lock);
    OsalMemFree(list);

    return HDF_SUCCESS;
}

static int32_t UsbdBulkASyncListReleasePort(struct HostDevice *port)
{
    if ((!port) || (!port->bulkASyncList)) {
        return HDF_SUCCESS;
    }
    struct UsbdBulkASyncList *list = port->bulkASyncList;
    while (port->bulkASyncList) {
        list = port->bulkASyncList;
        port->bulkASyncList = list->next;
        UsbdBulkASyncListRelease(list);
        list = NULL;
    }
    return HDF_SUCCESS;
}

static void UsbdRelease(struct HostDevice *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%{public}s:%{public}d: invalid param", __func__, __LINE__);
        return;
    }

    UsbdCloseInterfaces(dev);
    UsbdReleaseInterfaces(dev);
    UsbdFreeCtrlPipe(dev);
    UsbdRequestSyncReleaseList(dev);
    UsbdRequestASyncReleaseList(dev);
    UsbdBulkASyncListReleasePort(dev);

    if (dev->ctrlReq) {
        UsbFreeRequest(dev->ctrlReq);
        dev->ctrlReq = NULL;
    }
    UsbExitHostSdk(dev->service->session);
    dev->service->session = NULL;
    OsalMutexDestroy(&dev->writeLock);
    OsalMutexDestroy(&dev->readLock);
    OsalMutexDestroy(&dev->lock);
    OsalMutexDestroy(&dev->requestLock);
    OsalMutexDestroy(&dev->reqSyncLock);
    OsalMutexDestroy(&dev->reqASyncLock);
    dev->busNum = 0;
    dev->devAddr = 0;
    dev->initFlag = false;
}

static int32_t ReOpenDevice(struct HostDevice *port)
{
    if (!port) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_FAILURE;
    }
    uint8_t busNum = port->busNum;
    uint8_t devAddr = port->devAddr;
    UsbdRelease(port);
    port->busNum = busNum;
    port->devAddr = devAddr;
    OsalMutexInit(&port->writeLock);
    OsalMutexInit(&port->readLock);
    OsalMutexInit(&port->lock);
    OsalMutexInit(&port->requestLock);
    OsalMutexInit(&port->reqSyncLock);
    OsalMutexInit(&port->reqASyncLock);

    HdfSListInit(&port->requestQueue);
    HdfSListInit(&port->reqSyncList);
    HdfSListInit(&port->reqASyncList);
    int32_t ret = UsbdInit(port);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d UsbInit failed ret:%{public}d", __func__, __LINE__, ret);
        UsbdRelease(port);
        RemoveDevFromService(port->service, port);
        OsalMemFree(port);
        return ret;
    }
    DataFifoReset(&port->readFifo);
    port->initFlag = true;
    return HDF_SUCCESS;
}

void MakeSetActiveUsbControlParams(struct UsbControlParams *controlParams, uint8_t *buffer,
    uint32_t *length, uint16_t value, uint16_t index)
{
    controlParams->request = USB_DDK_REQ_SET_CONFIGURATION;
    controlParams->target = USB_REQUEST_TARGET_DEVICE;
    controlParams->reqType = USB_REQUEST_TYPE_STANDARD;
    controlParams->directon = USB_REQUEST_DIR_FROM_DEVICE;
    controlParams->value = value;
    controlParams->index = index;
    controlParams->data = buffer;
    controlParams->size = *length;
}

static int32_t FunSetActiveConfig(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t configId = 0;
    uint8_t configIdOld = 0;
    uint8_t configIdNew = 0;
    uint32_t length = 1;
    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &configId)) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct UsbControlParams controlParams = {};
    MakeGetActiveUsbControlParams(&controlParams, &configIdOld, &length, 0, 0);
    int32_t ret = UsbControlTransferEx(port, &controlParams, USB_CTRL_SET_TIMEOUT);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d getConfiguration failed ret:%{public}d", __func__, __LINE__, ret);
        return HDF_ERR_INVALID_PARAM;
    }
    if (configId == configIdOld) {
        HDF_LOGE(
            "%{public}s:%{public}d setConfiguration success "
            "configId:%{public}d old:%{public}d",
            __func__, __LINE__, configId, configIdOld);
        return HDF_SUCCESS;
    }
    length = 0;
    MakeSetActiveUsbControlParams(&controlParams, &configId, &length, (int32_t)0 + configId, 0);
    ret = UsbControlTransferEx(port, &controlParams, USB_CTRL_SET_TIMEOUT);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d setConfiguration failed ret:%{public}d", __func__, __LINE__, ret);
        return HDF_ERR_IO;
    }
    length = 1;
    MakeGetActiveUsbControlParams(&controlParams, &configIdNew, &length, 0, 0);
    ret = UsbControlTransferEx(port, &controlParams, USB_CTRL_SET_TIMEOUT);
    if ((HDF_SUCCESS != ret) || (configId != configIdNew)) {
        HDF_LOGE("%{public}s:%{public}d getConfiguration failed ret:%{public}d", __func__, __LINE__, ret);
        return HDF_ERR_IO;
    }
    if (configId != 0) {
        ret = ReOpenDevice(port);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d ReOpenDevice failed ret:%{public}d", __func__, __LINE__, ret);
            return ret;
        }
    }
    return ret;
}

static int32_t FunSetInterface(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t interfaceId = 0;
    uint8_t altIndex = 0;
    UsbInterfaceHandle *interfaceHandle = NULL;

    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint8(data, &interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &altIndex)) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    interfaceHandle = InterfaceIdToHandle(port, interfaceId);
    if (interfaceHandle == NULL) {
        HDF_LOGE(
            "%{public}s:%{public}d InterfaceIdToHandle failed bus:%{public}d "
            "devAddr:%{public}d interfaceId:%{public}d",
            __func__, __LINE__, port->busNum, port->devAddr, interfaceId);
        return HDF_FAILURE;
    }
    if (port->iface[interfaceId] && (port->iface[interfaceId]->info.curAltSetting == altIndex)) {
        HDF_LOGE(
            "%{public}s:%{public}d bus:%{public}d devAddr:%{public}d "
            "interfaceId:%{public}d cur:%{public}d",
            __func__, __LINE__, port->busNum, port->devAddr, interfaceId, altIndex);
        return HDF_SUCCESS;
    }

    int32_t ret = UsbSelectInterfaceSetting(interfaceHandle, altIndex, &port->iface[interfaceId]);
    if (HDF_SUCCESS == ret) {
        UsbdRequestSyncReleaseList(port);
        UsbdRequestASyncReleaseList(port);
        UsbdBulkASyncListReleasePort(port);
    }
    return ret;
}

static bool UsbdHdfReadBufAndMalloc(struct HdfSBuf *data, uint8_t **ptr, uint32_t *length)
{
    if ((!data) && (!length) && (!ptr)) {
        HDF_LOGE("%{public}s:%{public}d param failed", __func__, __LINE__);
        return false;
    }
    if (!HdfSbufReadUint32(data, length)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return false;
    }
    if (*length > 0) {
        const uint8_t *tclientData = HdfSbufReadUnpadBuffer(data, *length);
        if (tclientData == NULL) {
            HDF_LOGE("%{public}s:%{public}d read failed length:%{public}d", __func__, __LINE__, *length);
            return false;
        }
        *ptr = (uint8_t *)OsalMemAlloc(*length);
        if (!(*ptr)) {
            HDF_LOGE("%{public}s:%{public}d OsalMemAlloc failed size:%{public}d", __func__, __LINE__, *length);
            return false;
        }
        int32_t err = memcpy_s(*ptr, *length, tclientData, *length);
        if (err != EOK) {
            HDF_LOGE("%{public}s:%{public}d memcpy_s failed size:%{public}u", __func__, __LINE__, *length);
            OsalMemFree(*ptr);
            *ptr = NULL;
            return false;
        }
    } else {
        *ptr = NULL;
    }
    return true;
}

static int32_t FillReqAyncParams(struct UsbdRequestASync *userData, struct UsbPipeInfo *pipe,
    struct UsbRequestParams *parmas, uint8_t *buffer, int32_t length)
{
    bool bWrite = false;
    if ((!userData) || (!pipe) || (!parmas)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    bWrite = (pipe->pipeDirection == USB_PIPE_DIRECTION_OUT);
    parmas->interfaceId = pipe->interfaceId;
    parmas->pipeAddress = pipe->pipeDirection | pipe->pipeAddress;
    parmas->pipeId = pipe->pipeId;
    parmas->requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    parmas->timeout = USB_CTRL_SET_TIMEOUT;
    parmas->dataReq.numIsoPackets = 0;
    parmas->userData = (void *)userData;
    parmas->dataReq.length = length;
    parmas->dataReq.directon = (pipe->pipeDirection >> USB_PIPE_DIR_OFFSET) & 0x1;
    if (bWrite) {
        parmas->callback = UsbdWriteCallback;
        parmas->dataReq.buffer = buffer;
    } else {
        parmas->callback = UsbdReadCallback;
        parmas->dataReq.length = length;
    }
    return HDF_SUCCESS;
}

static struct UsbdRequestASync *UsbdRequestASyncAlloc(void)
{
    struct UsbdRequestASync *req = (struct UsbdRequestASync *)OsalMemAlloc(sizeof(struct UsbdRequestASync));
    if (req) {
        if (memset_s(req, sizeof(struct UsbdRequestASync), 0, sizeof(struct UsbdRequestASync)) != EOK) {
            HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
            OsalMemFree(req);
            return NULL;
        }
        req->reqMsg.request = NULL;
        req->endPointAddr = 0;
        req->ifHandle = NULL;
        req->status = 0;
        OsalMutexInit(&req->lock);
    }
    return req;
}

static int32_t UsbdRequestASyncInit(struct HostDevice *port, UsbInterfaceHandle *ifHandle,
    struct UsbPipeInfo *pipe, struct UsbdRequestASync *request)
{
    int32_t ret = HDF_SUCCESS;
    if ((port == NULL) || (request == NULL) || (ifHandle == NULL) || (pipe == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    memcpy_s(&request->pipe, sizeof(struct UsbPipeInfo), pipe, sizeof(struct UsbPipeInfo));
    request->ifHandle = ifHandle;
    request->reqMsg.request = UsbAllocRequest(request->ifHandle, 0, request->pipe.maxPacketSize);
    if (request->reqMsg.request == NULL) {
        HDF_LOGE("%{public}s:%{public}d alloc request failed\n", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    FillReqAyncParams(request, &request->pipe, &request->params, NULL, 0);
    OsalMutexLock(&port->reqASyncLock);
    HdfSListAddTail(&port->reqASyncList, &request->node);
    OsalMutexUnlock(&port->reqASyncLock);
    return ret;
}

static struct UsbdRequestASync *UsbdRequestASyncCreatAndInsert(struct HostDevice *port, uint8_t interfaceId,
    uint8_t pipeAddr)
{
    int32_t ret = HDF_SUCCESS;
    struct UsbdRequestASync *req = NULL;
    UsbInterfaceHandle *ifHandle = NULL;
    struct UsbPipeInfo pipe;
    if (memset_s(&pipe, sizeof(struct UsbPipeInfo), 0, sizeof(struct UsbPipeInfo)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
        return NULL;
    }
    ret = GetPipe(port, interfaceId, pipeAddr, &pipe);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE(
            "%{public}s: get pipe info failed interfaceId=%{public}d, "
            "pipeAddr=%{public}d",
            __func__, interfaceId, pipeAddr);
        return NULL;
    }
    ifHandle = InterfaceIdToHandle(port, interfaceId);
    if (ifHandle == NULL) {
        HDF_LOGE("%{public}s:%{public}d get interface handle failed\n", __func__, __LINE__);
        return NULL;
    }
    req = UsbdRequestASyncAlloc();
    if (req) {
        ret = UsbdRequestASyncInit(port, ifHandle, &pipe, req);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s:%{public}d UsbdRequestASyncInit failed:%{public}d\n", __func__, __LINE__, ret);
            UsbdRequestASyncRelease(req);
            req = NULL;
            return NULL;
        }
    }
    return req;
}

static struct UsbdRequestASync *UsbdFindRequestASync(struct HostDevice *port, uint8_t interfaceId, uint8_t pipeAddr)
{
    struct UsbdRequestASync *req = NULL;
    struct HdfSListIterator it;
    uint8_t reqnum = 0;
    uint8_t usenum = 0;
    bool flag = false;
    if (!port) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return NULL;
    }
    OsalMutexLock(&port->reqASyncLock);
    HdfSListIteratorInit(&it, &port->reqASyncList);
    while (HdfSListIteratorHasNext(&it)) {
        req = (struct UsbdRequestASync *)HdfSListIteratorNext(&it);
        if (!req) {
            continue;
        }
        if ((req->pipe.interfaceId == interfaceId) && ((req->pipe.pipeAddress | req->pipe.pipeDirection) == pipeAddr)) {
            reqnum++;
            if (req->status) {
                usenum++;
            } else {
                flag = true;
                break;
            }
        }
    }
    OsalMutexUnlock(&port->reqASyncLock);

    if (flag) {
        if (req) {
            OsalMutexLock(&req->lock);
            req->status = 1;
            OsalMutexUnlock(&req->lock);
        }
        return req;
    } else {
        if (reqnum < MAX_REQUESTASYNC_NUM) {
            req = UsbdRequestASyncCreatAndInsert(port, interfaceId, pipeAddr);
            if (req) {
                OsalMutexLock(&req->lock);
                req->status = 1;
                OsalMutexUnlock(&req->lock);
            }
            return req;
        }
    }
    return NULL;
}

static int32_t UsbReqQueueParamInfo(struct HdfSBuf *data, uint8_t *interfaceId, uint8_t *pipeAddr)
{
    if ((!data) || (!interfaceId) || (!pipeAddr)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, pipeAddr)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int32_t FunRequestQueueFillAndSubmit(struct HostDevice *port, struct UsbdRequestASync *reqAsync, uint8_t *buffer,
    uint32_t length)
{
    if ((port == NULL) || (reqAsync == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    FillReqAyncParams(reqAsync, &reqAsync->pipe, &reqAsync->params, buffer, length);
    int32_t ret = UsbFillRequest(reqAsync->reqMsg.request, reqAsync->ifHandle, &reqAsync->params);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d UsbFillRequest failed, ret=%{public}d \n", __func__, __LINE__, ret);
        OsalMutexLock(&reqAsync->lock);
        reqAsync->reqMsg.clientData = NULL;
        reqAsync->reqMsg.clientLength = 0;
        reqAsync->params.dataReq.buffer = NULL;
        reqAsync->params.dataReq.length = 0;
        reqAsync->status = 0;
        OsalMutexUnlock(&reqAsync->lock);
        return ret;
    }

    OsalSemInit(&((struct UsbIfRequest *)reqAsync->reqMsg.request)->hostRequest->sem, 0);
    ret = UsbSubmitRequestAsync(reqAsync->reqMsg.request);
    if (HDF_SUCCESS == ret) {
        OsalMutexLock(&port->requestLock);
        HdfSListAddTail(&port->requestQueue, &reqAsync->qNode);
        OsalMutexUnlock(&port->requestLock);
    } else {
        HDF_LOGE("%{public}s:%{public}d UsbSubmitRequestAsync failed, ret=%{public}d", __func__, __LINE__, ret);
        OsalMutexLock(&reqAsync->lock);
        reqAsync->status = 0;
        reqAsync->reqMsg.clientData = NULL;
        reqAsync->reqMsg.clientLength = 0;
        reqAsync->params.dataReq.buffer = NULL;
        reqAsync->params.dataReq.length = 0;
        OsalMutexUnlock(&reqAsync->lock);
    }

    return ret;
}

static int32_t FunRequestQueue(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t *clientData = NULL;
    uint8_t *buffer = NULL;
    uint32_t clientLength = 0;
    uint32_t length = 0;
    struct UsbdRequestASync *reqAsync = NULL;
    uint8_t interfaceId = 0;
    uint8_t pipeAddr = 0;
    if ((port == NULL) || (reply == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = UsbReqQueueParamInfo(data, &interfaceId, &pipeAddr);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d UsbReqQueueParamInfo failed", __func__, __LINE__);
        return ret;
    }
    if (!UsbdHdfReadBufAndMalloc(data, &clientData, &clientLength) ||
        !UsbdHdfReadBufAndMalloc(data, &buffer, &length)) {
        HDF_LOGE("%{public}s:%{public}d UsbdHdfReadBufAndMalloc failed", __func__, __LINE__);
        OsalMemFree(buffer);
        buffer = NULL;
        OsalMemFree(clientData);
        clientData = NULL;
        return HDF_ERR_INVALID_PARAM;
    }
    reqAsync = UsbdFindRequestASync(port, interfaceId, pipeAddr);
    if (!reqAsync) {
        HDF_LOGE("%{public}s:%{public}d UsbdFindRequestASync failed", __func__, __LINE__);
        OsalMemFree(buffer);
        buffer = NULL;
        OsalMemFree(clientData);
        clientData = NULL;
        return HDF_ERR_DEVICE_BUSY;
    }

    reqAsync->reqMsg.clientData = (void *)clientData;
    reqAsync->reqMsg.clientLength = clientLength;
    ret = FunRequestQueueFillAndSubmit(port, reqAsync, buffer, length);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d FunRequestQueueFillAndSubmit failed:%{public}d", __func__, __LINE__, ret);
        OsalMemFree(clientData);
        clientData = NULL;
    }
    OsalMemFree(buffer);
    buffer = NULL;
    return ret;
}

static int32_t GetRequestMsgFromQueue(struct HostDevice *port, struct UsbdRequestASync **reqMsg)
{
    struct HdfSListNode *requestNode = NULL;
    int32_t requestCount = 0;
    if (port == NULL) {
        HDF_LOGE("%{public}s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&port->requestLock);
    if (!HdfSListIsEmpty(&port->requestQueue)) {
        requestNode = HdfSListPop(&port->requestQueue);
        requestCount = HdfSListCount(&port->requestQueue);
    }
    OsalMutexUnlock(&port->requestLock);
    if (requestNode == NULL) {
        HDF_LOGE("%{public}s:%{public}d request node is null", __func__, __LINE__);
        return HDF_FAILURE;
    }
    *reqMsg = HDF_SLIST_CONTAINER_OF(struct HdfSListNode, requestNode, struct UsbdRequestASync, qNode);
    if (*reqMsg == NULL) {
        HDF_LOGE("%{public}s:%{public}d request msg is null", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GetRequestMsgData(struct HostDevice *port, struct UsbdRequestASync *reqMsg, int32_t timeout,
    uint8_t **buffer, uint32_t *length)
{
    int32_t ret = HDF_SUCCESS;
    if ((reqMsg == NULL) || (reqMsg->reqMsg.request == NULL)) {
        HDF_LOGE("%{public}s: invalid parma  request %{public}p", __func__, reqMsg);
        return HDF_ERR_INVALID_PARAM;
    }
    if ((int32_t)(reqMsg->reqMsg.request->compInfo.status) == -1) {
        ret = OsalSemWait(&((struct UsbIfRequest *)reqMsg->reqMsg.request)->hostRequest->sem, timeout);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s:%{public}d OsalSemWait failed, ret=%{public}d", __func__, __LINE__, ret);
            OsalMutexLock(&port->requestLock);
            HdfSListAdd(&port->requestQueue, &reqMsg->qNode);
            OsalMutexUnlock(&port->requestLock);
            return ret;
        }
    }

    OsalMutexLock(&reqMsg->lock);
    *length = reqMsg->reqMsg.length;
    *buffer = reqMsg->reqMsg.buffer;
    reqMsg->reqMsg.buffer = NULL;
    reqMsg->reqMsg.length = 0;
    reqMsg->status = 0;
    OsalMutexUnlock(&reqMsg->lock);
    return ret;
}

static int32_t FunRequestWait(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct UsbdRequestASync *reqMsg = NULL;
    uint8_t *buffer = NULL;
    uint32_t length = 0;
    int32_t timeout = 0;
    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s: invalid parma", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &timeout)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = GetRequestMsgFromQueue(port, &reqMsg);
    if ((HDF_SUCCESS != ret) || (!reqMsg) || (!reqMsg->reqMsg.request)) {
        HDF_LOGE("%{public}s:%{public}d GetRequestMsgFromQueue error:%{public}d", __func__, __LINE__, ret);
        return ret;
    }

    if ((int32_t)(reqMsg->reqMsg.request->compInfo.status) == -1) {
        ret = OsalSemWait(&((struct UsbIfRequest *)reqMsg->reqMsg.request)->hostRequest->sem, timeout);
        if (HDF_SUCCESS != ret) {
            HDF_LOGE("%{public}s:%{public}d OsalSemWait failed, ret=%{public}d", __func__, __LINE__, ret);
            OsalMutexLock(&port->requestLock);
            HdfSListAdd(&port->requestQueue, &reqMsg->qNode);
            OsalMutexUnlock(&port->requestLock);
            return ret;
        }
    }

    ret = GetRequestMsgData(port, reqMsg, timeout, &buffer, &length);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d GetRequestMsgData error:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    if (!UsbdHdfWriteBuf(reply, reqMsg->reqMsg.clientData, reqMsg->reqMsg.clientLength)) {
        ret = HDF_ERR_IO;
        HDF_LOGE("%{public}s:%{public}d  HdfSbufWriteBuffer failed", __func__, __LINE__);
    }
    if (!UsbdHdfWriteBuf(reply, reqMsg->reqMsg.request->compInfo.buffer, reqMsg->reqMsg.request->compInfo.length)) {
        ret = HDF_ERR_IO;
        HDF_LOGE("%{public}s:%{public}d  HdfSbufWriteBuffer failed", __func__, __LINE__);
    }
    UsbdRequestASyncReleaseData(reqMsg);
    return ret;
}

static int32_t FunRequestCancel(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct UsbdRequestASync *reqMsg = NULL;
    uint8_t interfaceId = 0;
    uint8_t endpointId = 0;
    int32_t ret = HDF_FAILURE;

    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint8(data, &interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &endpointId)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    while (HDF_SUCCESS == GetRequestMsgFromQueue(port, &reqMsg)) {
        if (reqMsg) {
            ret = UsbCancelRequest(reqMsg->reqMsg.request);
            if (ret != HDF_SUCCESS) {
                HDF_LOGW("%{public}s:%{public}d request cancel failed = %{public}d", __func__, __LINE__, ret);
            }
            OsalMutexLock(&reqMsg->lock);
            UsbdRequestASyncReleaseData(reqMsg);
            reqMsg->status = 0;
            OsalMutexUnlock(&reqMsg->lock);
        }
        reqMsg = NULL;
    }
    return HDF_SUCCESS;
}

static int32_t FunOpenDevice(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if ((port == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    port->initFlag = true;
    return HDF_SUCCESS;
}

static int32_t FunCloseDevice(struct HostDevice *port, struct HdfSBuf *data)
{
    if (port == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    port->initFlag = false;
    return HDF_SUCCESS;
}

static int32_t FunSetRole(struct HdfSBuf *data, struct HdfSBuf *reply, struct UsbdService *service)
{
    int32_t portId = 0;
    int32_t powerRole = 0;
    int32_t dataRole = 0;
    if (!HdfSbufReadInt32(data, &portId)) {
        HDF_LOGE("%{public}s:%{public}d Read data failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, &powerRole)) {
        HDF_LOGE("%{public}s:%{public}d Read data failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, &dataRole)) {
        HDF_LOGE("%{public}s:%{public}d Read data failed", __func__, __LINE__);
        return HDF_ERR_IO;
    }

    int32_t ret = SetPort(portId, powerRole, dataRole, service);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d FunSetRole failed, ret=%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    if (reply) {
        HdfSbufWriteInt32(reply, ret);
    }

    return HDF_SUCCESS;
}

static int32_t FunQueryPort(struct HdfSBuf *data, struct HdfSBuf *reply, struct UsbdService *service)
{
    int32_t portId = 0;
    int32_t powerRole = 0;
    int32_t dataRole = 0;
    int32_t mode = 0;
    int32_t ret = QueryPort(&portId, &powerRole, &dataRole, &mode, service);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d QueryPort failed, ret=%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    if (reply) {
        HdfSbufWriteInt32(reply, portId);
        HdfSbufWriteInt32(reply, powerRole);
        HdfSbufWriteInt32(reply, dataRole);
        HdfSbufWriteInt32(reply, mode);
    }
    return HDF_SUCCESS;
}

static int32_t FunGetCurrentFunctions(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t funcs = UsbdGetFunction();
    if (reply) {
        HdfSbufWriteInt32(reply, funcs);
    }

    return HDF_SUCCESS;
}

static int32_t FunSetCurrentFunctions(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t funcs = 0;
    HdfSbufReadInt32(data, &funcs);
    int32_t ret = UsbdSetFunction(funcs);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d FunSetCurrentFunctions failed, ret=%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t DispatchBindUsbSubscriber(struct UsbdService *service, struct HdfSBuf *data)
{
    struct UsbdSubscriber *subscriber = NULL;
    if (service == NULL || data == NULL) {
        HDF_LOGE("%{public}s:%{public}d param is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct HdfRemoteService *remoteService = HdfSbufReadRemoteService(data);
    if (remoteService == NULL) {
        HDF_LOGE("%{public}s: remoteService is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    subscriber = (struct UsbdSubscriber *)OsalMemCalloc(sizeof(struct UsbdSubscriber));
    if (subscriber == NULL) {
        HDF_LOGE("%{public}s: calloc subscriber error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (memset_s(subscriber, sizeof(struct UsbdSubscriber), 0, sizeof(struct UsbdSubscriber)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
        OsalMemFree(subscriber);
        return HDF_FAILURE;
    }
    subscriber->remoteService = remoteService;
    return BindUsbSubscriber(service, subscriber);
}

static int32_t DispatchUnbindUsbSubscriber(struct UsbdService *service)
{
    if (service == NULL) {
        HDF_LOGE("%{public}s:%{public}d param is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return UnbindUsbSubscriber(service);
}

static int32_t FunGetRawDeviceDescriptor(struct HostDevice *port, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;

    if ((port == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d mangf invalid parma", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (port->ctrDevHandle == NULL) {
        HDF_LOGE("%{public}s:%{public}d devHandle failed", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    struct UsbInterfaceHandleEntity *handle = (struct UsbInterfaceHandleEntity *)port->ctrDevHandle;
    OsalMutexLock(&handle->devHandle->lock);
    uint8_t *ptr = handle->devHandle->dev->descriptors;
    uint32_t length = handle->devHandle->dev->descriptorsLength;
    if (!UsbdHdfWriteBuf(reply, ptr, length)) {
        HDF_LOGE("%{public}s:%{public}d WriteBuffer failed", __func__, __LINE__);
        ret = HDF_ERR_IO;
    }
    OsalMutexUnlock(&handle->devHandle->lock);

    return ret;
}

static int32_t FunGetFileDescriptor(struct HostDevice *port, struct HdfSBuf *reply)
{
    if ((port == NULL) || (reply == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (port->ctrDevHandle == NULL) {
        HDF_LOGE("%{public}s:%{public}d devHandle failed", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    struct UsbInterfaceHandleEntity *handle = (struct UsbInterfaceHandleEntity *)port->ctrDevHandle;
    OsalMutexLock(&handle->devHandle->lock);
    int32_t fd = handle->devHandle->fd;
    OsalMutexUnlock(&handle->devHandle->lock);
    if (!HdfSbufWriteInt32(reply, fd)) {
        HDF_LOGE("%{public}s:%{public}d writeint32 failed fd:%{public}d", __func__, __LINE__, fd);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static struct HostDevice *FindDevFromService(struct UsbdService *service, uint8_t busNum, uint8_t devAddr)
{
    struct HdfSListIterator it;
    struct HostDevice *port = NULL;
    bool flag = false;
    if (!service) {
        return NULL;
    }

    OsalMutexLock(&service->lock);
    HdfSListIteratorInit(&it, &service->devList);
    while (HdfSListIteratorHasNext(&it)) {
        port = (struct HostDevice *)HdfSListIteratorNext(&it);
        if (!port) {
            continue;
        }
        if ((port->busNum == busNum) && (port->devAddr == devAddr)) {
            flag = true;
            break;
        }
    }
    OsalMutexUnlock(&service->lock);
    if (!flag) {
        return NULL;
    }
    return port;
}

static int32_t HostDeviceInit(struct HostDevice *port)
{
    if (!port) {
        HDF_LOGE("%{public}s:%{public}d param failed", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    port->busNum = 0;
    port->devAddr = 0;
    port->initFlag = false;
    port->interfaceCnt = 0;

    if (OsalMutexInit(&port->lock) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d init lock failed!", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&port->requestLock) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d init lock failed!", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&port->writeLock) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d init lock failed!", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&port->readLock) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d init lock failed!", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&port->reqSyncLock) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d init lock failed!", __func__, __LINE__);
        return HDF_FAILURE;
    }
    if (OsalMutexInit(&port->reqASyncLock) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d init lock failed!", __func__, __LINE__);
        return HDF_FAILURE;
    }
    HdfSListInit(&port->requestQueue);
    HdfSListInit(&port->reqSyncList);
    HdfSListInit(&port->reqASyncList);
    return HDF_SUCCESS;
}

int32_t HostDeviceCreate(struct HostDevice **port)
{
    struct HostDevice *tmp = NULL;
    if (!port) {
        return HDF_FAILURE;
    }
    tmp = (struct HostDevice *)OsalMemCalloc(sizeof(struct HostDevice));
    if (!tmp) {
        HDF_LOGE("%{public}s:%{public}d Alloc usb host device failed", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (memset_s(tmp, sizeof(struct HostDevice), 0, sizeof(struct HostDevice)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
        OsalMemFree(tmp);
        return HDF_FAILURE;
    }

    int32_t ret = HostDeviceInit(tmp);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:%{public}d HostDeviceInit failed!", __func__, __LINE__);
        OsalMemFree(tmp);
        return ret;
    }

    tmp->initFlag = false;
    *port = tmp;
    return HDF_SUCCESS;
}

int32_t UsbdRealseDevices(struct UsbdService *service)
{
    if (!service) {
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&service->lock);
    while (!HdfSListIsEmpty(&service->devList)) {
        struct HostDevice *port = (struct HostDevice *)HdfSListPop(&service->devList);
        if (port) {
            UsbdRelease(port);
            OsalMemFree(port);
            port = NULL;
        }
    }
    OsalMutexUnlock(&service->lock);
    return HDF_SUCCESS;
}

int32_t FunAttachDevice(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret = HDF_SUCCESS;
    if (port == NULL) {
        HDF_LOGE("%{public}s:%{public}d mangf invalid parma", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (port->initFlag) {
        HDF_LOGE(
            "%{public}s:%{public}d device is already on flag:%{public}d "
            "bus:%{public}d dev:%{public}d",
            __func__, __LINE__, port->initFlag, port->busNum, port->devAddr);
        return HDF_SUCCESS;
    }

    do {
        ret = UsbdInit(port);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d mangf UsbInit failed ret:%{public}d", __func__, __LINE__, ret);
            UsbdRelease(port);
            RemoveDevFromService(port->service, port);
            OsalMemFree(port);
            return ret;
        }
        ret = UsbdAllocFifo(&port->readFifo, READ_BUF_SIZE);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d mangf UsbAllocFifo failed ret:%{public}d", __func__, __LINE__, ret);
            ret = HDF_ERR_INVALID_PARAM;
            break;
        }
        if (HDF_SUCCESS == ret) {
            port->initFlag = true;
            HDF_LOGI("%{public}s:%{public}d UsbOpen success", __func__, __LINE__);
        } else {
            HDF_LOGE("%{public}s:%{public}d UsbOpen fail:%{public}d", __func__, __LINE__, ret);
        }
        return ret;
    } while (0);

    UsbdFreeFifo(&port->readFifo);
    UsbdRelease(port);
    RemoveDevFromService(port->service, port);
    OsalMemFree(port);

    return ret;
}

int32_t UsbdDeviceCreateAndAttach(struct UsbdService *service, uint8_t busNum, uint8_t devAddr)
{
    struct HostDevice *port = NULL;
    if (!service) {
        HDF_LOGE("%{public}s:%{public}d invalid param!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ret = HostDeviceCreate(&port);
    if (ret == HDF_SUCCESS) {
        port->busNum = busNum;
        port->devAddr = devAddr;
        port->service = service;
        OsalMutexLock(&service->lock);
        HdfSListAdd(&service->devList, &port->node);
        OsalMutexUnlock(&service->lock);
        ret = FunAttachDevice(port, NULL, NULL);
        port = NULL;
    } else {
        HDF_LOGE("%{public}s:%{public}d createdevice error ret:%{public}d", __func__, __LINE__, ret);
    }

    return ret;
}

int32_t FunDetachDevice(struct HostDevice *port, struct HdfSBuf *data)
{
    if (port == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    RemoveDevFromService(port->service, port);
    UsbdRelease(port);
    UsbdFreeFifo(&port->readFifo);
    OsalMemFree(port);

    return HDF_SUCCESS;
}

int32_t UsbdDeviceDettach(struct UsbdService *service, uint8_t busNum, uint8_t devAddr)
{
    if (!service) {
        HDF_LOGE("%{public}s:%{public}d invalid param!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct HostDevice *port = FindDevFromService(service, busNum, devAddr);
    int32_t ret = FunDetachDevice(port, NULL);
    port = NULL;
    return ret;
}

struct HostDevice *UsbdFindDevForBusNum(struct UsbdService *service, uint8_t busNum)
{
    struct HdfSListIterator it;
    uint8_t flg = false;
    struct HostDevice *tempPort = NULL;
    if (!service) {
        return NULL;
    }
    OsalMutexLock(&service->lock);
    HdfSListIteratorInit(&it, &service->devList);
    while (HdfSListIteratorHasNext(&it)) {
        tempPort = (struct HostDevice *)HdfSListIteratorNext(&it);
        if (!tempPort) {
            continue;
        }
        if (tempPort->busNum == busNum) {
            HdfSListIteratorRemove(&it);
            flg = true;
            break;
        }
    }
    OsalMutexUnlock(&service->lock);
    if (flg) {
        return tempPort;
    }
    return NULL;
}

int32_t UsbdRemoveBusDev(struct UsbdService *service, uint8_t busNum)
{
    struct HostDevice *tempPort = NULL;
    while (1) {
        tempPort = UsbdFindDevForBusNum(service, busNum);
        if (!tempPort) {
            break;
        }
        NotifySubscriberDevice(service->subscriber, ACT_DEVDOWN, tempPort->busNum, tempPort->devAddr);

        UsbdRelease(tempPort);

        UsbdFreeFifo(&tempPort->readFifo);

        OsalMemFree(tempPort);
        tempPort = NULL;
    }
    return HDF_SUCCESS;
}

static int32_t UsbdBulkASyncReqInit(struct UsbdBulkASyncReqList *list, struct UsbdBulkASyncList *pList)
{
    int32_t ret = HDF_SUCCESS;
    int32_t i = 0;
    DListHeadInit(&list->eList);
    DListHeadInit(&list->uList);
    OsalMutexInit(&list->elock);
    OsalMutexInit(&list->ulock);
    for (i = 0; i < USBD_BULKASYNCREQ_NUM_MAX; ++i) {
        list->node[i].request = UsbAllocRequest(pList->ifHandle, 0, pList->pipe.maxPacketSize);
        if (!list->node[i].request) {
            HDF_LOGE("%{public}s:%{public}d alloc request failed i:%{public}d", __func__, __LINE__, i);
            ret = HDF_ERR_MALLOC_FAIL;
            break;
        }
        list->node[i].list = list;
        list->node[i].id = i;
        DListInsertTail(&list->node[i].node, &list->eList);
        pList->params.userData = (void *)&list->node[i];
    }

    if (i != USBD_BULKASYNCREQ_NUM_MAX) {
        for (; i >= 0; --i) {
            UsbFreeRequest(list->node[i].request);
            list->node[i].request = NULL;
        }
        DListHeadInit(&list->eList);
        DListHeadInit(&list->uList);
        OsalMutexDestroy(&list->elock);
        OsalMutexDestroy(&list->ulock);
    }
    list->pList = pList;
    return ret;
}

static int32_t UsbdBulkASyncReqFillParams(struct UsbPipeInfo *pipe, struct UsbRequestParams *params, uint8_t *buffer);

static struct UsbdBulkASyncList *UsbdBulkASyncListAlloc(struct HostDevice *port, uint8_t ifId, uint8_t epId)
{
    int32_t ret = HDF_SUCCESS;
    UsbInterfaceHandle *ifHandle = NULL;
    struct UsbPipeInfo pipe;
    struct UsbdBulkASyncList *bulkAsyncList = NULL;

    if (memset_s(&pipe, sizeof(struct UsbPipeInfo), 0, sizeof(struct UsbPipeInfo)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
        return NULL;
    }
    ret = GetPipe(port, ifId, epId, &pipe);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE(
            "%{public}s:%{public}d GetPipe ret:%{public}d ifId:%{public}d "
            "epId:%{public}d",
            __func__, __LINE__, ret, ifId, epId);
        return NULL;
    }
    ifHandle = InterfaceIdToHandle(port, ifId);
    if (ifHandle == NULL) {
        HDF_LOGE("%{public}s:%{public}d get interface handle failed", __func__, __LINE__);
        return NULL;
    }

    bulkAsyncList = OsalMemAlloc(sizeof(struct UsbdBulkASyncList));
    if (!bulkAsyncList) {
        HDF_LOGE("%{public}s:%{public}d malloc failed!", __func__, __LINE__);
        return NULL;
    }
    if (memset_s(bulkAsyncList, sizeof(struct UsbdBulkASyncList), 0, sizeof(struct UsbdBulkASyncList)) != EOK) {
        HDF_LOGE("%{public}s:%{public}d  memset_s failed", __func__, __LINE__);
        OsalMemFree(bulkAsyncList);
        return NULL;
    }
    bulkAsyncList->ifId = ifId;
    bulkAsyncList->epId = epId;
    bulkAsyncList->instance = port;
    OsalMutexInit(&bulkAsyncList->asmHandle.lock);
    bulkAsyncList->pipe = pipe;
    bulkAsyncList->ifHandle = ifHandle;

    UsbdBulkASyncReqFillParams(&bulkAsyncList->pipe, &bulkAsyncList->params, NULL);

    ret = UsbdBulkASyncReqInit(&bulkAsyncList->rList, bulkAsyncList);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d UsbdBulkASyncReqInit ret:%{public}d", __func__, __LINE__, ret);
        UsbdBulkASyncListRelease(bulkAsyncList);
        bulkAsyncList = NULL;
        return NULL;
    }

    return bulkAsyncList;
}

static struct UsbdBulkASyncList *UsbdBulkASyncListFind(struct HostDevice *port, uint8_t ifId, uint8_t epId)
{
    struct UsbdBulkASyncList *list = NULL;
    if (!port) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return NULL;
    }
    list = port->bulkASyncList;
    while (list) {
        if ((list->epId == epId) && (list->ifId == ifId)) {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

static struct UsbdBulkASyncList *UsbdBulkASyncListInit(struct HostDevice *port, uint8_t ifId, uint8_t epId)
{
    struct UsbdBulkASyncList *list = UsbdBulkASyncListFind(port, ifId, epId);
    if (!list) {
        list = UsbdBulkASyncListAlloc(port, ifId, epId);
        if (list) {
            list->next = port->bulkASyncList;
            port->bulkASyncList = list;
        }
    }
    return list;
}

static int32_t UsbdBulkASyncReqNodeSetNoUse(struct UsbdBulkASyncReqNode *db)
{
    OsalMutexLock(&db->list->elock);
    db->use = USBD_REQNODE_NOUSE;
    DListInsertTail(&db->node, &db->list->eList);
    OsalMutexUnlock(&db->list->elock);
    return HDF_SUCCESS;
}

static struct UsbdBulkASyncReqNode *UsbdBulkASyncReqGetENode(struct UsbdBulkASyncReqList *list)
{
    OsalMutexLock(&list->elock);
    if (DListIsEmpty(&list->eList)) {
        OsalMutexUnlock(&list->elock);
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return NULL;
    }
    struct UsbdBulkASyncReqNode *ptr = DLIST_FIRST_ENTRY(&list->eList, struct UsbdBulkASyncReqNode, node);
    if (ptr) {
        ptr->use = USBD_REQNODE_OTHER;
        DListRemove(&ptr->node);
    }
    OsalMutexUnlock(&list->elock);
    return ptr;
}

static int32_t UsbdBulkReadRemoteCallback(struct HdfRemoteService *service, int32_t status,
    struct UsbdBufferHandle *handle)
{
    int32_t ret = HDF_SUCCESS;
    int32_t actLength = 0;
    uint8_t flg = 0;
    if ((service == NULL) || (handle == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&handle->lock);
    flg = handle->cbflg;
    handle->cbflg = 1;
    actLength = handle->rcur;
    OsalMutexUnlock(&handle->lock);
    if (flg) {
        return HDF_SUCCESS;
    }
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    do {
        if (data == NULL || reply == NULL) {
            HDF_LOGE("%{public}s:%{public}d failed to obtain hdf sbuf", __func__, __LINE__);
            ret = HDF_ERR_IO;
            break;
        }
        if (!HdfRemoteServiceWriteInterfaceToken(service, data)) {
            HDF_LOGE("%{public}s:%{public}d write interface token failed", __func__, __LINE__);
            ret = HDF_ERR_IO;
            break;
        }
        if (!HdfSbufWriteInt32(data, status)) {
            HDF_LOGE("%{public}s:%{public}d write status error status:%{public}d", __func__, __LINE__, status);
            ret = HDF_ERR_IO;
            break;
        }
        if (!HdfSbufWriteInt32(data, actLength)) {
            HDF_LOGE("%{public}s:%{public}d write actLength error length:%{public}d", __func__, __LINE__, actLength);
            ret = HDF_ERR_IO;
            break;
        }
        ret = service->dispatcher->Dispatch(service, CMD_USBD_BULK_CALLBACK_READ, data, reply);
    } while (0);

    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

static int32_t UsbdBulkWriteRemoteCallback(struct HdfRemoteService *service, int32_t status,
    struct UsbdBufferHandle *handle)
{
    int32_t ret = HDF_SUCCESS;
    int32_t actLength = 0;
    uint8_t flg = 0;
    if ((service == NULL) || (handle == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&handle->lock);
    flg = handle->cbflg;
    handle->cbflg = 1;
    actLength = handle->cur;
    OsalMutexUnlock(&handle->lock);
    if (flg) {
        return HDF_SUCCESS;
    }
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    do {
        if (data == NULL || reply == NULL) {
            HDF_LOGE("%{public}s:%{public}d failed to obtain hdf sbuf", __func__, __LINE__);
            ret = HDF_ERR_IO;
            break;
        }
        if (!HdfRemoteServiceWriteInterfaceToken(service, data)) {
            HDF_LOGE("%{public}s:%{public}d write interface token failed", __func__, __LINE__);
            ret = HDF_ERR_IO;
            break;
        }
        if (!HdfSbufWriteInt32(data, status)) {
            HDF_LOGE("%{public}s:%{public}d write status error status:%{public}d", __func__, __LINE__, status);
            ret = HDF_ERR_IO;
            break;
        }
        if (!HdfSbufWriteInt32(data, actLength)) {
            HDF_LOGE("%{public}s:%{public}d write actLength error length:%{public}d", __func__, __LINE__, actLength);
            ret = HDF_ERR_IO;
            break;
        }
        ret = service->dispatcher->Dispatch(service, CMD_USBD_BULK_CALLBACK_WRITE, data, reply);
    } while (0);

    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

static int32_t UsbdBulkASyncPutAsmData(struct UsbdBufferHandle *handle, uint8_t *buffer, uint32_t len)
{
    int32_t tlen = len;
    int32_t ret = HDF_SUCCESS;
    if ((handle == NULL) || (buffer == NULL) || (len < 1)) {
        HDF_LOGE("%{public}s:%{public}d invalid param len:%{public}d", __func__, __LINE__, len);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMutexLock(&handle->lock);
    do {
        if (handle->fd < 1) {
            HDF_LOGE("%{public}s:%{public}d fd:%{public}d", __func__, __LINE__, handle->fd);
            ret = HDF_ERR_BAD_FD;
            break;
        }
        tlen = (handle->size > handle->rcur) ? (handle->size - handle->rcur) : 0;
        tlen = tlen < len ? tlen : len;
        if (tlen > 0) {
            memcpy_s(handle->starAddr + handle->rcur, tlen, buffer, len);
            handle->rcur += tlen;
        }
    } while (0);
    OsalMutexUnlock(&handle->lock);
    return ret;
}

static int32_t UsbdBulkAsyncGetAsmData(struct UsbdBufferHandle *handle, struct UsbRequestParams *params,
    uint16_t maxPacketSize)
{
    int32_t ret = HDF_ERR_INVALID_PARAM;
    if ((handle == NULL) || (params == NULL) || (handle->size < 1) || (maxPacketSize < 1)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    OsalMutexLock(&handle->lock);
    if (handle->cur < handle->size) {
        params->dataReq.length =
            (handle->size - handle->cur) < maxPacketSize ? (handle->size - handle->cur) : maxPacketSize;
        params->dataReq.buffer = handle->starAddr + handle->cur;
        handle->cur += params->dataReq.length;
        ret = HDF_SUCCESS;
    } else {
        params->dataReq.length = 0;
        params->dataReq.buffer = NULL;
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        ret = HDF_DEV_ERR_NODATA;
    }
    OsalMutexUnlock(&handle->lock);
    return ret;
}

static int32_t UsbdBulkAsyncGetAsmReqLen(struct UsbdBufferHandle *handle, uint32_t *reqLen, uint16_t maxPacketSize)
{
    if ((handle == NULL) || (reqLen == NULL) || (handle->size < 1) || (maxPacketSize < 1)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    uint32_t tlen = 0;
    OsalMutexLock(&handle->lock);
    if (handle->cur < handle->size) {
        tlen = handle->size - handle->cur;
        tlen = tlen < maxPacketSize ? tlen : maxPacketSize;
        handle->cur += tlen;
    }
    OsalMutexUnlock(&handle->lock);
    *reqLen = tlen;
    return HDF_SUCCESS;
}

static int32_t UsbdBulkASyncReqWriteAutoSubmit(struct UsbRequest *request)
{
    int32_t ret = HDF_SUCCESS;
    struct UsbdBulkASyncReqNode *db = (struct UsbdBulkASyncReqNode *)request->compInfo.userData;
    struct UsbRequestParams params = {0};
    memcpy_s(&params, sizeof(params), &db->list->pList->params, sizeof(params));
    params.userData = (void *)db;
    ret = UsbdBulkAsyncGetAsmData(&db->list->pList->asmHandle, &params, db->list->pList->pipe.maxPacketSize);
    if (HDF_SUCCESS != ret) {
        UsbdBulkASyncReqNodeSetNoUse(db);
        return ret;
    }
    db->request->compInfo.status = 0;
    ret = UsbFillRequest(request, db->list->pList->ifHandle, &params);
    if (HDF_SUCCESS != ret) {
        UsbdBulkASyncReqNodeSetNoUse(db);
        HDF_LOGE("%{public}s:%{public}d UsbFillRequest ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbSubmitRequestAsync(request);
    if (HDF_SUCCESS != ret) {
        UsbdBulkASyncReqNodeSetNoUse(db);
        HDF_LOGE("%{public}s:%{public}d UsbSubmitRequestAsync ret:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

static int32_t UsbdBulkASyncReqReadAutoSubmit(struct UsbRequest *request)
{
    uint32_t readLen = 0;
    struct UsbdBulkASyncReqNode *db = (struct UsbdBulkASyncReqNode *)request->compInfo.userData;
    int32_t ret =
        UsbdBulkASyncPutAsmData(&db->list->pList->asmHandle, request->compInfo.buffer, request->compInfo.actualLength);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE(
            "%{public}s:%{public}d UsbdBulkASyncPutAsmData error "
            "size:%{public}d ret:%{public}d ",
            __func__, __LINE__, request->compInfo.actualLength, ret);
        UsbdBulkASyncReqNodeSetNoUse(db);
        return ret;
    }
    ret = UsbdBulkAsyncGetAsmReqLen(&db->list->pList->asmHandle, &readLen, db->list->pList->pipe.maxPacketSize);
    if (readLen < 1) {
        UsbdBulkASyncReqNodeSetNoUse(db);
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_DEV_ERR_NODATA;
    }
    db->request->compInfo.status = 0;
    struct UsbHostRequest *hostRequest = NULL;
    hostRequest = ((struct UsbIfRequest *)request)->hostRequest;
    if (readLen != (uint32_t)hostRequest->length) {
        struct UsbRequestParams params;
        memcpy_s(&params, sizeof(params), &db->list->pList->params, sizeof(params));
        params.dataReq.length = readLen;
        params.userData = (void *)db;
        ret = UsbFillRequest(request, db->list->pList->ifHandle, &params);
        if (HDF_SUCCESS != ret) {
            UsbdBulkASyncReqNodeSetNoUse(db);
            HDF_LOGE("%{public}s:%{public}d UsbFillRequest ret:%{public}d ", __func__, __LINE__, ret);
            return ret;
        }
    }
    ret = UsbSubmitRequestAsync(request);
    if (HDF_SUCCESS != ret) {
        UsbdBulkASyncReqNodeSetNoUse(db);
        HDF_LOGE("%{public}s:%{public}d UsbSubmitRequestAsync ret:%{public}d ", __func__, __LINE__, ret);
    }
    return ret;
}

static void UsbdBulkASyncWriteCallbackAutoSubmit(struct UsbRequest *request)
{
    if (!request) {
        return;
    }
    int32_t ret = HDF_SUCCESS;
    struct UsbdBulkASyncReqNode *node = (struct UsbdBulkASyncReqNode *)request->compInfo.userData;
    int32_t status = request->compInfo.status;
    if (status == 0) {
        ret = UsbdBulkASyncReqWriteAutoSubmit(request);
        if (HDF_DEV_ERR_NODATA == ret) {
            int32_t count = DlistGetCount(&node->list->eList);
            if (count >= USBD_BULKASYNCREQ_NUM_MAX) {
                ret = UsbdBulkWriteRemoteCallback(node->list->pList->cb, HDF_SUCCESS, &node->list->pList->asmHandle);
            }
        } else if (HDF_SUCCESS != ret) {
            ret = UsbdBulkWriteRemoteCallback(node->list->pList->cb, ret, &node->list->pList->asmHandle);
            HDF_LOGE(
                "%{public}s:%{public}d UsbdBulkWriteRemoteCallback "
                "ret:%{public}d req:%{public}p id:%{public}d",
                __func__, __LINE__, ret, request, node->id);
        }
    } else {
        UsbdBulkASyncReqNodeSetNoUse(node);
        ret = UsbdBulkWriteRemoteCallback(node->list->pList->cb, status, &node->list->pList->asmHandle);
        HDF_LOGE(
            "%{public}s:%{public}d UsbdBulkWriteRemoteCallback ret:%{public}d "
            "req:%{public}p id:%{public}d status:%{public}d",
            __func__, __LINE__, ret, request, node->id, status);
    }
}

static void UsbdBulkASyncReadCallbackAutoSubmit(struct UsbRequest *request)
{
    if (!request) {
        return;
    }
    int32_t ret = HDF_SUCCESS;
    struct UsbdBulkASyncReqNode *node = (struct UsbdBulkASyncReqNode *)request->compInfo.userData;
    int32_t status = request->compInfo.status;
    if (status == 0) {
        ret = UsbdBulkASyncReqReadAutoSubmit(request);
        if (HDF_DEV_ERR_NODATA == ret) {
            int32_t count = DlistGetCount(&node->list->eList);
            if (count >= USBD_BULKASYNCREQ_NUM_MAX) {
                ret = UsbdBulkReadRemoteCallback(node->list->pList->cb, HDF_SUCCESS, &node->list->pList->asmHandle);
            }
        } else if (HDF_SUCCESS != ret) {
            ret = UsbdBulkReadRemoteCallback(node->list->pList->cb, ret, &node->list->pList->asmHandle);
            HDF_LOGE(
                "%{public}s:%{public}d UsbdBulkReadRemoteCallback "
                "ret:%{public}d req:%{public}p id:%{public}d",
                __func__, __LINE__, ret, request, node->id);
        }
    } else {
        UsbdBulkASyncReqNodeSetNoUse(node);
        ret = UsbdBulkReadRemoteCallback(node->list->pList->cb, status, &node->list->pList->asmHandle);
        HDF_LOGE(
            "%{public}s:%{public}d UsbdBulkReadRemoteCallback ret:%{public}d "
            "req:%{public}p id:%{public}d status:%{public}d",
            __func__, __LINE__, ret, request, node->id, status);
    }
}

static int32_t UsbdBulkASyncReqFillParams(struct UsbPipeInfo *pipe, struct UsbRequestParams *params, uint8_t *buffer)
{
    params->interfaceId = pipe->interfaceId;
    params->pipeAddress = pipe->pipeDirection | pipe->pipeAddress;
    params->pipeId = pipe->pipeId;
    params->requestType = USB_REQUEST_PARAMS_DATA_TYPE;
    params->timeout = USB_CTRL_SET_TIMEOUT;
    params->dataReq.numIsoPackets = 0;
    params->dataReq.directon = (pipe->pipeDirection >> USB_PIPE_DIR_OFFSET) & 0x1;
    params->dataReq.length = pipe->maxPacketSize;

    if (pipe->pipeDirection == USB_PIPE_DIRECTION_OUT) {
        params->callback = UsbdBulkASyncWriteCallbackAutoSubmit;
        params->dataReq.buffer = buffer;
    } else {
        params->callback = UsbdBulkASyncReadCallbackAutoSubmit;
    }
    return HDF_SUCCESS;
}

static int32_t UsbdBulkASyncReqWriteSubmit(struct UsbdBulkASyncReqNode *req)
{
    int32_t ret = HDF_SUCCESS;
    struct UsbRequestParams params = {0};
    memcpy_s(&params, sizeof(params), &req->list->pList->params, sizeof(params));
    params.userData = (void *)req;
    ret = UsbdBulkAsyncGetAsmData(&req->list->pList->asmHandle, &params, req->list->pList->pipe.maxPacketSize);
    if (HDF_SUCCESS != ret) {
        UsbdBulkASyncReqNodeSetNoUse(req);
        HDF_LOGE("%{public}s:%{public}d UsbdBulkAsyncGetAsmData ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    req->request->compInfo.status = 0;
    ret = UsbFillRequest(req->request, req->list->pList->ifHandle, &params);
    if (HDF_SUCCESS != ret) {
        UsbdBulkASyncReqNodeSetNoUse(req);
        HDF_LOGE("%{public}s:%{public}d UsbFillRequest ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbSubmitRequestAsync(req->request);
    if (HDF_SUCCESS != ret) {
        UsbdBulkASyncReqNodeSetNoUse(req);
        HDF_LOGE("%{public}s:%{public}d UsbSubmitRequestAsync ret:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

static int32_t UsbdBulkASyncWriteSubmitStart(struct UsbdBulkASyncList *list)
{
    int32_t ret = HDF_SUCCESS;
    struct UsbdBulkASyncReqNode *req = NULL;
    uint32_t i = 0;
    if (list == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    do {
        req = UsbdBulkASyncReqGetENode(&list->rList);
        if (!req) {
            ret = HDF_ERR_DEVICE_BUSY;
            break;
        } else {
            ret = UsbdBulkASyncReqWriteSubmit(req);
            if (HDF_SUCCESS != ret) {
                break;
            }
        }
        if (++i > USBD_BULKASYNCREQ_NUM_MAX) {
            break;
        }
    } while (!DListIsEmpty(&list->rList.eList));
    return ret;
}

static int32_t UsbdBulkASyncReqReadSubmit(struct UsbdBulkASyncReqNode *db)
{
    int32_t ret = HDF_SUCCESS;
    uint32_t readLen = 0;
    ret = UsbdBulkAsyncGetAsmReqLen(&db->list->pList->asmHandle, &readLen, db->list->pList->pipe.maxPacketSize);
    if (readLen == 0) {
        UsbdBulkASyncReqNodeSetNoUse(db);
        HDF_LOGE("%{public}s:%{public}d readLen:%{public}d", __func__, __LINE__, readLen);
        return HDF_DEV_ERR_NODATA;
    }

    db->request->compInfo.status = 0;
    struct UsbRequestParams params;
    memcpy_s(&params, sizeof(params), &db->list->pList->params, sizeof(params));
    params.dataReq.length = readLen;
    params.userData = (void *)db;
    ret = UsbFillRequest(db->request, db->list->pList->ifHandle, &params);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d readLen:%{public}d", __func__, __LINE__, readLen);
        UsbdBulkASyncReqNodeSetNoUse(db);
        return ret;
    }

    ret = UsbSubmitRequestAsync(db->request);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d readLen:%{public}d", __func__, __LINE__, readLen);
        UsbdBulkASyncReqNodeSetNoUse(db);
    }
    return ret;
}

static int32_t UsbdBulkReadASyncSubmitStart(struct UsbdBulkASyncList *list)
{
    int32_t ret = HDF_SUCCESS;
    struct UsbdBulkASyncReqNode *req = NULL;
    uint32_t i = 0;
    if (list == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    do {
        req = UsbdBulkASyncReqGetENode(&list->rList);
        if (!req) {
            HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
            ret = HDF_ERR_DEVICE_BUSY;
            break;
        } else {
            ret = UsbdBulkASyncReqReadSubmit(req);
            if (HDF_SUCCESS != ret) {
                HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
                break;
            }
        }
        if (++i > USBD_BULKASYNCREQ_NUM_MAX) {
            break;
        }
    } while (!DListIsEmpty(&list->rList.eList));
    return ret;
}

static int32_t FunRegBulkCallback(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t interfaceId = 0;
    uint8_t pipeId = 0;
    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d get interfaceId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadUint8(data, &pipeId)) {
        HDF_LOGE("%{public}s:%{public}d get pipeId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    struct UsbdBulkASyncList *list = UsbdBulkASyncListInit(port, interfaceId, pipeId);
    if (!list) {
        HDF_LOGE("%{public}s:%{public}d failed ifId:%{public}hhu epId:%{public}hhu",
            __func__, __LINE__, interfaceId, pipeId);
        return HDF_DEV_ERR_DEV_INIT_FAIL;
    }
    list->cb = HdfSbufReadRemoteService(data);
    if (!list->cb) {
        HDF_LOGE("%{public}s:%{public}d get callback error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(list->cb, USB_TOKEN_VALUE)) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(list->cb);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t FunUnRegBulkCallback(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t interfaceId = 0;
    uint8_t pipeId = 0;
    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d get interfaceId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadUint8(data, &pipeId)) {
        HDF_LOGE("%{public}s:%{public}d get pipeId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    struct UsbdBulkASyncList *list = UsbdBulkASyncListFind(port, interfaceId, pipeId);
    if (!list) {
        HDF_LOGW("%{public}s:%{public}d fail ifId:%{public}d epId:%{public}d", __func__, __LINE__, interfaceId, pipeId);
        return HDF_SUCCESS;
    }
    list->cb = NULL;
    return HDF_SUCCESS;
}

static void ReleaseAsmBufferHandle(struct UsbdBufferHandle *handle)
{
    if (!handle) {
        return;
    }
    int32_t tsize = handle->size;
    if (handle->size > 0) {
        OsalMutexLock(&handle->lock);
        handle->size = 0;
        handle->cur = 0;
        handle->rcur = 0;
        handle->fd = -1;
        handle->cbflg = 0;
        OsalMutexUnlock(&handle->lock);
    }

    handle->fd = -1;
    handle->cur = 0;
    handle->rcur = 0;
    handle->size = 0;
    handle->cbflg = 0;
    if (tsize > 0) {
        OsalMutexLock(&handle->lock);
        munmap(handle->starAddr, tsize);
        OsalMutexUnlock(&handle->lock);
    }
    handle->starAddr = NULL;
}

static int32_t BulkRequestCancel(struct UsbdBulkASyncList *list)
{
    int32_t ret = HDF_SUCCESS;
    if (!list) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    for (int32_t i = 0; i < USBD_BULKASYNCREQ_NUM_MAX; ++i) {
        if ((list->rList.node[i].use == USBD_REQNODE_USE) || (list->rList.node[i].use == USBD_REQNODE_OTHER)) {
            ret = UsbCancelRequest(list->rList.node[i].request);
        }
    }
    OsalMSleep(USB_BULK_CANCEL_SLEEP_TIME);

    for (int32_t i = 0; i < USBD_BULKASYNCREQ_NUM_MAX; ++i) {
        list->rList.node[i].request->compInfo.status = 0;
        if ((list->rList.node[i].use == USBD_REQNODE_USE) || (list->rList.node[i].use == USBD_REQNODE_OTHER)) {
            list->rList.node[i].request->compInfo.status = 0;
            UsbdBulkASyncReqNodeSetNoUse(&list->rList.node[i]);
        }
    }
    return HDF_SUCCESS;
}

static int32_t FunBulkCancel(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint8_t interfaceId = 0;
    uint8_t pipeId = 0;
    struct HdfRemoteService *tcb = NULL;
    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, &interfaceId)) {
        HDF_LOGE("%{public}s:%{public}d get interfaceId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadUint8(data, &pipeId)) {
        HDF_LOGE("%{public}s:%{public}d get pipeId error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    struct UsbdBulkASyncList *list = UsbdBulkASyncListFind(port, interfaceId, pipeId);
    if (!list) {
        HDF_LOGW("%{public}s:%{public}d failed ifId:%{public}hhu epId:%{public}hhu",
            __func__, __LINE__, interfaceId, pipeId);
        return HDF_SUCCESS;
    }
    tcb = list->cb;
    list->cb = NULL;
    ReleaseAsmBufferHandle(&list->asmHandle);
    BulkRequestCancel(list);
    list->cb = tcb;
    return HDF_SUCCESS;
}

static int32_t UsbdGetBulkParams(struct HdfSBuf *data, uint8_t *ifId, uint8_t *epId, int32_t *fd, int32_t *size)
{
    if ((!data) || (!ifId) || (!epId) || (!fd) || (!size)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint8(data, ifId)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadUint8(data, epId)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    *fd = HdfSbufReadFileDescriptor(data);
    if (*fd < 0) {
        HDF_LOGE("%{public}s:%{public}d get fd error:%{public}d", __func__, __LINE__, *fd);
        return HDF_ERR_IO;
    }
    if (!HdfSbufReadInt32(data, size)) {
        HDF_LOGE("%{public}s:%{public}d file size error", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t InitAsmBufferHandle(struct UsbdBufferHandle *handle, int32_t fd, int32_t size)
{
    if (handle == NULL) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    ReleaseAsmBufferHandle(handle);
    if ((fd < 0) || (size < 0)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    handle->fd = fd;
    handle->size = size;
    handle->cur = 0;
    handle->rcur = 0;
    handle->cbflg = 0;
    lseek(fd, 0, SEEK_SET);
    handle->starAddr = (uint8_t *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (handle->starAddr == NULL) {
        handle->fd = -1;
        handle->size = 0;
        HDF_LOGE("%{public}s:%{public}d mmap error:%s", __func__, __LINE__, strerror(errno));
        return HDF_ERR_MALLOC_FAIL;
    }
    return HDF_SUCCESS;
}

static int32_t FunBulkRead(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t fd = -1;
    int32_t size = 0;
    uint8_t ifId = 0;
    uint8_t epId = 0;
    struct UsbdBulkASyncList *list = NULL;
    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = UsbdGetBulkParams(data, &ifId, &epId, &fd, &size);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d UsbdReadAshmemFromHdfSBuf error ret:%{public}d ", __func__, __LINE__, ret);
        return ret;
    }
    list = UsbdBulkASyncListInit(port, ifId, epId);
    if (!list) {
        HDF_LOGE("%{public}s:%{public}d fail ifId:%{public}hhu epId:%{public}hhu", __func__, __LINE__, ifId, epId);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = InitAsmBufferHandle(&list->asmHandle, fd, size);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE(
            "%{public}s:%{public}d InitAsmBufferHandle error ret:%{public}d "
            "ashmem fd:%{public}d size:%{public}d",
            __func__, __LINE__, ret, fd, size);
        return ret;
    }
    ret = UsbdBulkReadASyncSubmitStart(list);
    if ((HDF_DEV_ERR_NODATA == ret) || (HDF_DEV_ERR_NO_MEMORY == ret) || (HDF_ERR_DEVICE_BUSY == ret)) {
        ret = HDF_SUCCESS;
    }
    if (HDF_SUCCESS != ret) {
        HDF_LOGE(
            "%{public}s:%{public}d UsbdBulkReadASyncSubmitStart error "
            "ret:%{public}d ",
            __func__, __LINE__, ret);
        return ret;
    }
    return ret;
}

static int32_t FunBulkWrite(struct HostDevice *port, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t fd = -1;
    int32_t size = 0;
    uint8_t ifId = 0;
    uint8_t epId = 0;
    struct UsbdBulkASyncList *list = NULL;
    if ((port == NULL) || (data == NULL)) {
        HDF_LOGE("%{public}s:%{public}d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = UsbdGetBulkParams(data, &ifId, &epId, &fd, &size);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE("%{public}s:%{public}d UsbdReadAshmemFromHdfSBuf error ret:%{public}d ", __func__, __LINE__, ret);
        return ret;
    }
    list = UsbdBulkASyncListInit(port, ifId, epId);
    if (!list) {
        HDF_LOGE("%{public}s:%{public}d fail ifId:%{public}d epId:%{public}d", __func__, __LINE__, ifId, epId);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = InitAsmBufferHandle(&list->asmHandle, fd, size);
    if (HDF_SUCCESS != ret) {
        HDF_LOGE(
            "%{public}s:%{public}d InitAsmBufferHandle error ret:%{public}d "
            "ashmem fd:%{public}d size:%{public}d",
            __func__, __LINE__, ret, fd, size);
        return ret;
    }
    ret = UsbdBulkASyncWriteSubmitStart(list);
    if ((HDF_DEV_ERR_NODATA == ret) || (HDF_DEV_ERR_NO_MEMORY == ret) || (HDF_ERR_DEVICE_BUSY == ret)) {
        ret = HDF_SUCCESS;
    }
    if (HDF_SUCCESS != ret) {
        HDF_LOGE(
            "%{public}s:%{public}d UsbdBulkASyncWriteSubmitStart error "
            "ret:%{public}d ",
            __func__, __LINE__, ret);
        return ret;
    }
    return ret;
}

static int32_t DispatchCheckParam(struct HdfDeviceIoClient *client)
{
    if (client == NULL) {
        HDF_LOGE("%{public}s:%{public}d client is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (client->device == NULL) {
        HDF_LOGE("%{public}s:%{public}d client->device is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    if (client->device->service == NULL) {
        HDF_LOGE("%{public}s:%{public}d client->device->service is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return HDF_SUCCESS;
}

static bool FilterCmd(int32_t cmd)
{
    if (cmd != CMD_FUN_GET_CURRENT_FUNCTIONS && cmd != CMD_FUN_SET_CURRENT_FUNCTIONS && cmd != CMD_SET_ROLE &&
        cmd != CMD_QUERY_PORT && CMD_BIND_USB_SUBSCRIBER != cmd && CMD_UNBIND_USB_SUBSCRIBER != cmd) {
        return true;
    }
    return false;
}

int32_t DispatchSwitchHost(int32_t cmd, struct UsbdService *service, struct HostDevice *port, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    switch (cmd) {
        case CMD_FUN_OPEN_DEVICE:
            return FunOpenDevice(port, data, reply);
        case CMD_FUN_CLOSE_DEVICE:
            return FunCloseDevice(port, data);
        case CMD_FUN_SEND_BULK_READ_ASYNC:
            return FunBulkRead(port, data, reply);
        case CMD_FUN_SEND_BULK_WRITE_ASYNC:
            return FunBulkWrite(port, data, reply);
        case CMD_FUN_SEND_BULK_READ_SYNC:
        case CMD_FUN_SEND_INTERRUPT_READ_SYNC:
        case CMD_FUN_SEND_ISO_READ_SYNC:
            return FunBulkReadSync(port, data, reply);
        case CMD_FUN_SEND_BULK_WRITE_SYNC:
        case CMD_FUN_SEND_INTERRUPT_WRITE_SYNC:
        case CMD_FUN_SEND_ISO_WRITE_SYNC:
            return FunBulkWriteSync(port, data, reply);
        case CMD_FUN_GET_DESCRIPTOR:
            return FunGetRawDeviceDescriptor(port, reply);
        case CMD_FUN_GET_FILEDESCRIPTOR:
            return FunGetFileDescriptor(port, reply);
        case CMD_FUN_REG_BULK_CALLBACK:
            return FunRegBulkCallback(port, data, reply);
        case CMD_FUN_UNREG_BULK_CALLBACK:
            return FunUnRegBulkCallback(port, data, reply);
        case CMD_FUN_BULK_CANCEL:
            return FunBulkCancel(port, data, reply);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

int32_t DispatchSwitchDevice(int32_t cmd, struct UsbdService *service, struct HostDevice *port, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    switch (cmd) {
        case CMD_SET_ROLE:
            return FunSetRole(data, reply, service);
        case CMD_QUERY_PORT:
            return FunQueryPort(data, reply, service);
        case CMD_FUN_GET_CURRENT_FUNCTIONS:
            return FunGetCurrentFunctions(data, reply);
        case CMD_FUN_SET_CURRENT_FUNCTIONS:
            return FunSetCurrentFunctions(data, reply);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

int32_t DispatchSwitch(int32_t cmd, struct UsbdService *service, struct HostDevice *port, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    if (cmd == CMD_FUN_OPEN_DEVICE || cmd == CMD_FUN_CLOSE_DEVICE || cmd == CMD_FUN_SEND_BULK_READ_ASYNC ||
        cmd == CMD_FUN_SEND_BULK_WRITE_ASYNC || cmd == CMD_FUN_SEND_BULK_READ_SYNC ||
        cmd == CMD_FUN_SEND_INTERRUPT_READ_SYNC || cmd == CMD_FUN_SEND_ISO_READ_SYNC ||
        cmd == CMD_FUN_SEND_BULK_WRITE_SYNC || cmd == CMD_FUN_SEND_INTERRUPT_WRITE_SYNC ||
        cmd == CMD_FUN_SEND_ISO_WRITE_SYNC || cmd == CMD_FUN_GET_DESCRIPTOR || cmd == CMD_FUN_REG_BULK_CALLBACK ||
        cmd == CMD_FUN_UNREG_BULK_CALLBACK || cmd == CMD_FUN_BULK_CANCEL || cmd == CMD_FUN_GET_FILEDESCRIPTOR) {
        return DispatchSwitchHost(cmd, service, port, data, reply);
    }
    if (cmd == CMD_SET_ROLE || cmd == CMD_QUERY_PORT || cmd == CMD_FUN_GET_CURRENT_FUNCTIONS ||
        cmd == CMD_FUN_SET_CURRENT_FUNCTIONS) {
        return DispatchSwitchDevice(cmd, service, port, data, reply);
    }

    switch (cmd) {
        case CMD_BIND_USB_SUBSCRIBER:
            return DispatchBindUsbSubscriber(service, data);
        case CMD_UNBIND_USB_SUBSCRIBER:
            return DispatchUnbindUsbSubscriber(service);
        case CMD_FUN_SEND_CTRL_REQUEST_SYNC:
            return FunControlTransfer(port, data, reply);
        case CMD_FUN_GET_DEVICE_DESCRIPTOR:
            return FunGetDeviceDescriptor(port, reply);
        case CMD_FUN_GET_CONFIG_DESCRIPTOR:
            return FunGetConfigDescriptor(port, data, reply);
        case CMD_FUN_GET_STRING_DESCRIPTOR:
            return FunGetStringDescriptor(port, data, reply);
        case CMD_FUN_REQUEST_QUEUE:
            return FunRequestQueue(port, data, reply);
        case CMD_FUN_REQUEST_WAIT:
            return FunRequestWait(port, data, reply);
        case CMD_FUN_REQUEST_CANCEL:
            return FunRequestCancel(port, data, reply);
        case CMD_FUN_CLAIM_INTERFACE:
            return FunClaimInterface(port, data);
        case CMD_FUN_RELEASE_INTERFACE:
            return FunReleaseInterface(port, data);
        case CMD_FUN_SET_CONFIG:
            return FunSetActiveConfig(port, data, reply);
        case CMD_FUN_GET_CONFIG:
            return FunGetActiveConfig(port, data, reply);
        case CMD_FUN_SET_INTERFACE:
            return FunSetInterface(port, data, reply);
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

int32_t UsbdServiceDispatch(struct HdfDeviceIoClient *client, int32_t cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct UsbdService *service = NULL;
    struct HostDevice *port = NULL;
    if (DispatchCheckParam(client) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (HdfDeviceObjectCheckInterfaceDesc(client->device, data) == false) {
        HDF_LOGE("%{public}s:%{public}d check interface desc failed", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    
    service = (struct UsbdService *)client->device->service;
    if (FilterCmd(cmd)) {
        uint8_t busNum = 0;
        uint8_t devAddr = 0;
        int32_t ret = ParseDeviceBuf(data, &busNum, &devAddr);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%{public}s:%{public}d cmd = %{public}d parse error:%{public}d", __func__, __LINE__, cmd, ret);
            return ret;
        }
        port = FindDevFromService(service, busNum, devAddr);
        switch (cmd) {
            case CMD_FUN_OPEN_DEVICE:
            case CMD_FUN_CLOSE_DEVICE:
                if (port == NULL) {
                    HDF_LOGE(
                        "%{public}s:%{public}d cmd = %{public}d busNum:%{public}hhu"
                        "devAddr:%{public}hhu no device",
                        __func__, __LINE__, cmd, busNum, devAddr);
                    return HDF_DEV_ERR_NO_DEVICE;
                }
                break;
            default:
                if ((port == NULL) || (!port->initFlag)) {
                    HDF_LOGE(
                        "%{public}s:%{public}d cmd = %{public}d busNum:%{public}hhu"
                        "devAddr:%{public}hhu no device",
                        __func__, __LINE__, cmd, busNum, devAddr);
                    return HDF_DEV_ERR_NO_DEVICE;
                }
                break;
        }
    }
    return DispatchSwitch(cmd, service, port, data, reply);
}