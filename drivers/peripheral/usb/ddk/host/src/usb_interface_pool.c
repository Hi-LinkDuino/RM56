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

#include "usb_interface_pool.h"
#include "linux_adapter.h"
#include "usb_io_manage.h"
#include "usb_protocol.h"

#define HDF_LOG_TAG USB_INTERFACE_POOL

static int32_t g_usbRequestObjectId = 0;

static HDF_STATUS IfFreePipeObj(struct UsbPipe *pipeObj)
{
    HDF_STATUS ret = HDF_SUCCESS;

    if (pipeObj == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    pipeObj->object.objectId = 0;

    RawUsbMemFree(pipeObj);

    return ret;
}

static HDF_STATUS IfDestroyPipeObj(const struct UsbSdkInterface *interfaceObj, const struct UsbPipe *pipeObj)
{
    HDF_STATUS ret = HDF_SUCCESS;
    struct UsbPipe *pipePos = NULL;
    struct UsbPipe *pipeTemp = NULL;
    bool found = false;
    bool destroyFlag = false;

    if (interfaceObj == NULL) {
        HDF_LOGE("%s:%d interfaceObj is NULL ", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (DListIsEmpty((struct DListHead *)&interfaceObj->pipeList)) {
        HDF_LOGE("%s:%d pipeList is empty ", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    if (pipeObj == NULL) {
        /* Destroy all pipe object */
        destroyFlag = true;
    } else {
        /* Destroys the specified pipe object */
        destroyFlag = false;
    }

    OsalMutexLock((struct OsalMutex *)&interfaceObj->listLock);
    DLIST_FOR_EACH_ENTRY_SAFE(pipePos, pipeTemp, &interfaceObj->pipeList, struct UsbPipe, object.entry) {
        if ((destroyFlag == true) || ((destroyFlag == false) &&
            (pipePos->object.objectId == pipeObj->object.objectId))) {
            found = true;
            DListRemove(&pipePos->object.entry);
            ret = IfFreePipeObj(pipePos);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s:%d IfFreePipeObj fail, ret=%d ", __func__, __LINE__, ret);
                break;
            }

            if (!destroyFlag) {
                break;
            }
        }
    }
    OsalMutexUnlock((struct OsalMutex *)&interfaceObj->listLock);

    if (!found) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d the pipe object to be destroyed does not exist, ret=%d ", \
            __func__, __LINE__, ret);
    }

    return ret;
}

static void IfInterfaceObjInit(struct UsbSdkInterface *interfaceObj)
{
    DListHeadInit(&interfaceObj->pipeList);
    OsalMutexInit(&interfaceObj->listLock);
    OsalAtomicSet(&interfaceObj->refCount, 0);
    interfaceObj->status = USB_INTERFACE_STATUS_NORMAL;
}

static HDF_STATUS IfFreeInterfaceObj(struct UsbSdkInterface *interfaceObj)
{
    HDF_STATUS ret;

    if (interfaceObj == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    interfaceObj->interface.object.objectId = 0;
    interfaceObj->parentObjectId = 0;
    ret = IfDestroyPipeObj(interfaceObj, NULL);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    OsalMutexDestroy(&interfaceObj->listLock);
    interfaceObj->status = USB_INTERFACE_STATUS_NORMAL;

    RawUsbMemFree(interfaceObj);

    return ret;
}

static HDF_STATUS IfInterfacePoolInit(struct UsbInterfacePool *interfacePool, uint8_t busNum, uint8_t devAddr)
{
    interfacePool->session = NULL;
    OsalMutexInit(&interfacePool->mutex);
    DListHeadInit(&interfacePool->interfaceList);
    OsalMutexInit(&interfacePool->interfaceLock);
    DListHeadInit(&interfacePool->object.entry);
    OsalAtomicSet(&interfacePool->refCount, 0);
    interfacePool->busNum = busNum;
    interfacePool->devAddr = devAddr;
    OsalAtomicSet(&interfacePool->ioRefCount, 0);
    OsalMutexInit(&interfacePool->ioStopLock);
    OsalMutexLock(&interfacePool->ioStopLock);
    interfacePool->ioProcessStopStatus = USB_POOL_PROCESS_RUNNING;
    OsalMutexUnlock(&interfacePool->ioStopLock);
    interfacePool->device = NULL;

    /* create submit queue and wait complete queue */
    return UsbIoCreateQueue(interfacePool);
}

static HDF_STATUS IfFreeInterfacePool(struct UsbInterfacePool *interfacePool)
{
    HDF_STATUS ret;

    if (interfacePool == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = UsbIoDestroyQueue(interfacePool);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    interfacePool->object.objectId = 0;
    OsalMutexDestroy(&interfacePool->mutex);
    ret = UsbIfDestroyInterfaceObj(interfacePool, NULL);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    OsalMutexDestroy(&interfacePool->interfaceLock);
    interfacePool->busNum = 0;
    interfacePool->devAddr = 0;

    RawUsbMemFree(interfacePool);

    return ret;
}

static HDF_STATUS IfDestroyInterfacePool(const struct UsbInterfacePool *interfacePool)
{
    HDF_STATUS ret = HDF_SUCCESS;
    struct UsbInterfacePool *interfacePoolPos = NULL;
    struct UsbInterfacePool *interfacePoolTemp = NULL;
    struct UsbSession *session = NULL;
    bool found = false;

    if ((interfacePool == NULL) || (interfacePool->session == NULL)) {
        HDF_LOGE("%s:%d the interfacePool is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    session = interfacePool->session;
    if (DListIsEmpty(&session->ifacePoolList)) {
        HDF_LOGE("%s:%d interface pool list is empty", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(interfacePoolPos, interfacePoolTemp, &session->ifacePoolList,
        struct UsbInterfacePool, object.entry) {
        if (interfacePoolPos->object.objectId == interfacePool->object.objectId) {
            found = true;
            DListRemove(&interfacePoolPos->object.entry);
            ret = IfFreeInterfacePool(interfacePoolPos);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s:%d IfFreeInterfacePool fail, ret=%d ",
                    __func__, __LINE__, ret);
                break;
            }
            interfacePoolPos = NULL;
            break;
        }
    }

    if (!found) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d the interfacePool object to be destroyed does not exist",
            __func__, __LINE__);
    }

    return ret;
}

static void IfInterfaceRefCount(const struct UsbSdkInterface *interfaceObj,
    uint8_t interfaceIndex, bool refCountFlag, bool *claimFlag)
{
    if (refCountFlag && (interfaceIndex != USB_CTRL_INTERFACE_ID)) {
        if (OsalAtomicRead((OsalAtomic *)&interfaceObj->refCount) == 0) {
            if (claimFlag != NULL) {
                *claimFlag = true;
            }
        } else {
            if (claimFlag != NULL) {
                *claimFlag = false;
            }
        }

        AdapterAtomicInc((OsalAtomic *)&interfaceObj->refCount);
    }
}

static struct UsbPipe *IfFindPipeObj(const struct UsbSdkInterface *interfaceObj, struct UsbPipeQueryPara queryPara)
{
    struct UsbPipe *pipePos = NULL;
    struct UsbPipe *pipeTemp = NULL;
    bool findFlag = false;

    if ((interfaceObj == NULL) || (interfaceObj->status == USB_INTERFACE_STATUS_REMOVE)
        || DListIsEmpty(&interfaceObj->pipeList)) {
        HDF_LOGE("%s:%d interfaceObj is NULL or status is remove or pipe list is empty.",
            __func__, __LINE__);
        return NULL;
    }

    OsalMutexLock((struct OsalMutex *)&interfaceObj->listLock);
    DLIST_FOR_EACH_ENTRY_SAFE(pipePos, pipeTemp, &interfaceObj->pipeList, struct UsbPipe, object.entry) {
        switch (queryPara.type) {
            case USB_PIPE_INDEX_TYPE:
                if (pipePos->info.pipeId == queryPara.pipeId) {
                    findFlag = true;
                }
                break;
            case USB_PIPE_DIRECTION_TYPE:
                if (pipePos->info.pipeDirection == queryPara.pipeDirection) {
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
    OsalMutexUnlock((struct OsalMutex *)&interfaceObj->listLock);

    if (!findFlag) {
        HDF_LOGE("%s:%d the pipe object to be find does not exist. ", __func__, __LINE__);
        return NULL;
    }
    return pipePos;
}

static struct UsbSdkInterface *IfFindInterfaceObj(const struct UsbInterfacePool *interfacePool,
    struct UsbInterfaceQueryPara queryPara, bool refCountFlag, bool *claimFlag, bool statusFlag)
{
    struct UsbSdkInterface *interfacePos = NULL;
    struct UsbSdkInterface *interfaceTemp = NULL;
    bool found = false;

    if ((interfacePool == NULL) || DListIsEmpty(&interfacePool->interfaceList)) {
        HDF_LOGE("%s:%d interfacePool is NULL or interface list is empty.", __func__, __LINE__);
        return NULL;
    }

    OsalMutexLock((struct OsalMutex *)&interfacePool->interfaceLock);
    DLIST_FOR_EACH_ENTRY_SAFE(interfacePos, interfaceTemp, &interfacePool->interfaceList, \
        struct UsbSdkInterface, interface.object.entry) {
        switch (queryPara.type) {
            case USB_INTERFACE_INTERFACE_INDEX_TYPE:
                if ((interfacePos->interface.info.interfaceIndex == queryPara.interfaceIndex)
                && (interfacePos->interface.info.curAltSetting == interfacePos->altSettingId)) {
                    found = true;
                }
                break;
            case USB_INTERFACE_ALT_SETTINGS_TYPE:
                if ((interfacePos->interface.info.interfaceIndex == queryPara.interfaceIndex)
                && (interfacePos->altSettingId == queryPara.altSettingId)) {
                    found = true;
                }
                break;
            default:
                break;
        }

        if (found) {
            IfInterfaceRefCount(interfacePos, queryPara.interfaceIndex, refCountFlag, claimFlag);
            break;
        }
    }
    OsalMutexUnlock((struct OsalMutex *)&interfacePool->interfaceLock);

    if (!found) {
        HDF_LOGE("%s:%d the interface object to be find does not exist.",
                 __func__, __LINE__);
        return NULL;
    }

    if (statusFlag && (interfacePos->status == USB_INTERFACE_STATUS_REMOVE)) {
        HDF_LOGE("%s:%d status=%d error.", __func__, __LINE__, interfacePos->status);
        return NULL;
    }
    return interfacePos;
}

static struct UsbInterfacePool *IfFindInterfacePool(
    const struct UsbSession *session, struct UsbPoolQueryPara queryPara, bool refCountFlag)
{
    struct UsbInterfacePool *interfacePoolPos = NULL;
    struct UsbInterfacePool *interfacePoolTemp = NULL;
    struct DListHead *ifacePoolList = NULL;
    bool found = false;

    if (session == NULL) {
        HDF_LOGE("%s:%d session is NULL", __func__, __LINE__);
        return NULL;
    }

    OsalMutexLock((struct OsalMutex *)&session->lock);
    ifacePoolList = (struct DListHead *)&session->ifacePoolList;
    if (DListIsEmpty(ifacePoolList)) {
        OsalMutexUnlock((struct OsalMutex *)&session->lock);
        HDF_LOGE("%s:%d interface pool list is empty", __func__, __LINE__);
        return NULL;
    }

    DLIST_FOR_EACH_ENTRY_SAFE(interfacePoolPos, interfacePoolTemp, ifacePoolList, \
        struct UsbInterfacePool, object.entry) {
        switch (queryPara.type) {
            case USB_POOL_NORMAL_TYPE:
                if ((interfacePoolPos->busNum == queryPara.busNum) \
                    && (interfacePoolPos->devAddr == queryPara.usbAddr)) {
                    found = true;
                }
                break;
            case USB_POOL_OBJECT_ID_TYPE:
                if (interfacePoolPos->object.objectId == queryPara.objectId) {
                    found = true;
                }
                break;
            default:
                break;
        }

        if (found) {
            if (refCountFlag) {
                AdapterAtomicInc(&interfacePoolPos->refCount);
            }

            break;
        }
    }
    OsalMutexUnlock((struct OsalMutex *)&session->lock);

    if (!found) {
        HDF_LOGE("%s:%d the interfacePool object to be find does not exist.", __func__, __LINE__);
        return NULL;
    }

    return interfacePoolPos;
}

static int32_t IfGetRequestPipeType(
    const struct UsbDeviceHandle *devHandle, uint8_t interfaceId, uint8_t pipeId, UsbPipeType *pipeType)
{
    struct UsbInterfacePool *interfacePool = NULL;
    struct UsbInterfaceQueryPara interfaceQueryPara = {0};
    struct UsbSdkInterface *interfaceObj = NULL;
    struct UsbPipeQueryPara pipeQueryPara = {0};
    struct UsbPipe *pipeObj = NULL;

    if (pipeType == NULL) {
        HDF_LOGE("%s:%d pipeType is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    /* Find interfacePool object */
    interfacePool = (struct UsbInterfacePool *)devHandle->dev->privateObject;
    if (interfacePool == NULL) {
        HDF_LOGE("%s:%d get interfacePool faile", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    /* Find interface object */
    interfaceQueryPara.type = USB_INTERFACE_INTERFACE_INDEX_TYPE;
    interfaceQueryPara.interfaceIndex = interfaceId;
    interfaceObj = IfFindInterfaceObj(interfacePool, interfaceQueryPara, false, NULL, true);
    if (interfaceObj == NULL) {
        return HDF_ERR_BAD_FD;
    }

    /* Find pipe object */
    pipeQueryPara.type = USB_PIPE_INDEX_TYPE;
    pipeQueryPara.pipeId = pipeId;
    pipeObj = IfFindPipeObj(interfaceObj, pipeQueryPara);
    if (pipeObj == NULL) {
        return HDF_ERR_BAD_FD;
    }

    *pipeType = pipeObj->info.pipeType;

    return HDF_SUCCESS;
}

static int32_t IfFillControlRequest(struct UsbHostRequest *hostRequest,
    const struct UsbDeviceHandle *devHandle, const struct UsbRequestParams *params)
{
    struct UsbFillRequestData fillRequestData;
    struct UsbControlRequest ctrlReq = params->ctrlReq;
    unsigned char *setup = hostRequest->buffer;
    int32_t ret;

    ret = UsbProtocalFillControlSetup(setup, &ctrlReq);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (ctrlReq.directon == USB_REQUEST_DIR_TO_DEVICE) {
        fillRequestData.endPoint = 0;
        if (ctrlReq.length > 0) {
            ret = memcpy_s(hostRequest->buffer + USB_RAW_CONTROL_SETUP_SIZE,
                USB_RAW_CONTROL_SETUP_SIZE + ctrlReq.length, ctrlReq.buffer, ctrlReq.length);
            if (ret != EOK) {
                HDF_LOGE("%s:%d memcpy_s fail, ctrlReq.length=%d",
                    __func__, __LINE__, ctrlReq.length);
                return ret;
            }
        }
    } else {
        fillRequestData.endPoint = (((uint8_t)ctrlReq.directon)  << USB_DIR_OFFSET);
    }
    /* fill control request */
    fillRequestData.length = USB_RAW_CONTROL_SETUP_SIZE + ctrlReq.length;
    fillRequestData.userCallback = params->callback;
    fillRequestData.callback     = UsbIoSetRequestCompletionInfo;
    fillRequestData.userData     = params->userData;
    fillRequestData.timeout      = params->timeout;

    return RawFillControlRequest(hostRequest, devHandle, &fillRequestData);
}

static int32_t IfFillIsoRequest(struct UsbHostRequest *hostRequest,
    const struct UsbDeviceHandle *devHandle, const struct UsbRequestParams *params)
{
    if (devHandle == NULL || params == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct UsbFillRequestData fillRequestData;
    uint8_t pipeAddress = params->pipeAddress;
    struct UsbRequestParamsData requestData = params->dataReq;
    UsbRequestDirection dir = requestData.directon;

    fillRequestData.endPoint      = (((uint8_t)dir) << USB_DIR_OFFSET) | pipeAddress;
    fillRequestData.buffer        = requestData.buffer;
    fillRequestData.length        = requestData.length;
    fillRequestData.numIsoPackets = requestData.numIsoPackets;
    fillRequestData.userCallback  = params->callback;
    fillRequestData.callback      = UsbIoSetRequestCompletionInfo;
    fillRequestData.userData      = params->userData;
    fillRequestData.timeout       = params->timeout;

    return RawFillIsoRequest(hostRequest, devHandle, &fillRequestData);
}

static int32_t IfFillBulkRequest(struct UsbHostRequest *hostRequest,
    const struct UsbDeviceHandle *devHandle, const struct UsbRequestParams *params)
{
    struct UsbRequestParamsData requestData = params->dataReq;
    UsbRequestDirection dir = params->dataReq.directon;
    uint8_t pipeAddress = params->pipeAddress;

    if ((params->dataReq.directon == USB_REQUEST_DIR_TO_DEVICE) && (requestData.length > 0)) {
        int32_t ret = memcpy_s(hostRequest->buffer, hostRequest->bufLen, requestData.buffer, requestData.length);
        if (ret != EOK) {
            HDF_LOGE("%s:%d memcpy_s fail", __func__, __LINE__);
            return HDF_ERR_IO;
        }
    }
    hostRequest->devHandle    = (struct UsbDeviceHandle *)devHandle;
    hostRequest->endPoint     = (((uint8_t)dir) << USB_DIR_OFFSET) | pipeAddress;
    hostRequest->requestType  = USB_PIPE_TYPE_BULK;
    hostRequest->timeout      = params->timeout;
    hostRequest->length       = requestData.length;
    hostRequest->userData     = params->userData;
    hostRequest->callback     = UsbIoSetRequestCompletionInfo;
    hostRequest->userCallback = params->callback;

    return HDF_SUCCESS;
}

static int32_t IfFillInterrupteRequest(struct UsbHostRequest *hostRequest,
    const struct UsbDeviceHandle *devHandle, const struct UsbRequestParams *params)
{
    struct UsbFillRequestData fillRequestData;
    uint8_t pipeAddress = params->pipeAddress;
    struct UsbRequestParamsData requestData = params->dataReq;
    UsbRequestDirection dir = requestData.directon;

    fillRequestData.endPoint = (((uint8_t)dir) << USB_DIR_OFFSET) | pipeAddress;
    fillRequestData.buffer   = requestData.buffer;
    fillRequestData.length   = requestData.length;
    fillRequestData.userCallback = params->callback;
    fillRequestData.callback = UsbIoSetRequestCompletionInfo;
    fillRequestData.userData = params->userData;
    fillRequestData.timeout  = params->timeout;

    return RawFillInterruptRequest(hostRequest, devHandle, &fillRequestData);
}

static int32_t IfSubmitRequestToQueue(const struct UsbIfRequest *requestObj)
{
    int32_t ret;
    struct UsbHostRequest *hostRequest = NULL;
    struct UsbInterfacePool *interfacePool = NULL;

    if (requestObj == NULL) {
        HDF_LOGE("%s:%d requestObj is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    hostRequest = requestObj->hostRequest;
    if ((hostRequest == NULL) || (hostRequest->devHandle == NULL) || (hostRequest->devHandle->dev == NULL)) {
        HDF_LOGE("%s:%d hostRequest is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    interfacePool = (struct UsbInterfacePool *)hostRequest->devHandle->dev->privateObject;
    if (interfacePool == NULL) {
        HDF_LOGE("%s:%d get interfacePool faile", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    ret = UsbIoSendRequest(&interfacePool->submitRequestQueue, hostRequest);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    return ret;
}

static int32_t IfFillRequestByPipeType(struct UsbIfRequest *requestObj, UsbPipeType pipeType,
    struct UsbHostRequest *hostRequest, const struct UsbDeviceHandle *devHandle,
    const struct UsbRequestParams *params)
{
    int32_t ret;

    switch (pipeType) {
        case USB_PIPE_TYPE_CONTROL:
            if (params->requestType != USB_REQUEST_PARAMS_CTRL_TYPE) {
                ret = HDF_ERR_INVALID_PARAM;
                HDF_LOGE("%s:%d params is not CTRL_TYPE", __func__, __LINE__);
                break;
            }

            requestObj->request.compInfo.type = USB_REQUEST_TYPE_DEVICE_CONTROL;

            ret = IfFillControlRequest(hostRequest, devHandle, params);
            break;
        case USB_PIPE_TYPE_ISOCHRONOUS:
            if (params->requestType != USB_REQUEST_PARAMS_DATA_TYPE) {
                ret = HDF_ERR_INVALID_PARAM;
                HDF_LOGE("%s:%d params is not DATA_TYPE", __func__, __LINE__);
                break;
            }

            ret = IfFillIsoRequest(hostRequest, devHandle, params);
            break;
        case USB_PIPE_TYPE_BULK:
            if (params->requestType != USB_REQUEST_PARAMS_DATA_TYPE) {
                ret = HDF_ERR_INVALID_PARAM;
                HDF_LOGE("%s:%d params is not DATA_TYPE", __func__, __LINE__);
                break;
            }

            ret = IfFillBulkRequest(hostRequest, devHandle, params);
            break;
        case USB_PIPE_TYPE_INTERRUPT:
            if (params->requestType != USB_REQUEST_PARAMS_DATA_TYPE) {
                ret = HDF_ERR_INVALID_PARAM;
                HDF_LOGE("%s:%d params is not DATA_TYPE", __func__, __LINE__);
                break;
            }

            ret = IfFillInterrupteRequest(hostRequest, devHandle, params);
            break;
        default:
            ret = HDF_FAILURE;
            break;
    }

    return ret;
}

static int32_t IfDestoryDevice(const struct UsbSession *session, const struct UsbInterfacePool *interfacePool,
    const struct UsbDeviceHandle *devHandle, bool refCountFlag)
{
    int32_t ret;

    if ((session == NULL) || (interfacePool == NULL) || (devHandle == NULL)) {
        HDF_LOGE("%s:%d invalid param", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMutexLock((struct OsalMutex *)&session->lock);
    if (refCountFlag) {
        AdapterAtomicDec((OsalAtomic *)&interfacePool->refCount);
    }

    if (OsalAtomicRead((OsalAtomic *)&interfacePool->refCount) > 0) {
        OsalMutexUnlock((struct OsalMutex *)&session->lock);
        return HDF_SUCCESS;
    }

    ret = IfDestroyInterfacePool(interfacePool);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d destroy interface pool failed", __func__, __LINE__);
        OsalMutexUnlock((struct OsalMutex *)&session->lock);
        return ret;
    }
    OsalMutexUnlock((struct OsalMutex *)&session->lock);

    ret = RawCloseDevice(devHandle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d close device failed", __func__, __LINE__);
    }

    return ret;
}

static struct UsbInterfacePool *IfGetInterfacePool(
    struct UsbDeviceHandle **devHandle, const struct UsbSession *realSession, uint8_t busNum, uint8_t usbAddr)
{
    struct UsbPoolQueryPara poolQueryPara;
    struct UsbInterfacePool *interfacePool = NULL;
    int32_t ret;

    *devHandle = RawOpenDevice(realSession, busNum, usbAddr);
    if (*devHandle == NULL) {
        HDF_LOGE("%s:%d RawOpenDevice faile", __func__, __LINE__);
        return NULL;
    }

    ret = UsbProtocalParseDescriptor(*devHandle, busNum, usbAddr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d faile, ret=%d", __func__, __LINE__, ret);
        (void)RawCloseDevice(*devHandle);
        return NULL;
    }

    poolQueryPara.type = USB_POOL_NORMAL_TYPE;
    poolQueryPara.busNum = busNum;
    poolQueryPara.usbAddr = usbAddr;
    interfacePool = IfFindInterfacePool(realSession, poolQueryPara, true);
    if ((interfacePool == NULL) || (interfacePool->device == NULL)) {
        interfacePool = (struct UsbInterfacePool *)((*devHandle)->dev->privateObject);
        ret = IfDestoryDevice(realSession, interfacePool, *devHandle, false);
        return NULL;
    }

    return interfacePool;
}

int32_t UsbIfCreatPipeObj(const struct UsbSdkInterface *interfaceObj, struct UsbPipe **pipeObj)
{
    struct UsbPipe *pipeObjTemp = NULL;
    static int32_t idNum = 0;

    if ((interfaceObj == NULL) || (pipeObj == NULL)) {
        HDF_LOGE("%s:%d param is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    pipeObjTemp = (struct UsbPipe *)RawUsbMemCalloc(sizeof(struct UsbPipe));
    if (pipeObjTemp == NULL) {
        HDF_LOGE("%s:%d RawUsbMemCalloc faile", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ++idNum;
    idNum %= INTERFACE_POOL_ID_MAX;
    pipeObjTemp->object.objectId = idNum;
    DListHeadInit(&pipeObjTemp->object.entry);

    OsalMutexLock((struct OsalMutex *)&interfaceObj->listLock);
    DListInsertTail(&pipeObjTemp->object.entry, (struct DListHead *)&interfaceObj->pipeList);
    OsalMutexUnlock((struct OsalMutex *)&interfaceObj->listLock);

    *pipeObj = pipeObjTemp;
    (*pipeObj)->info.interfaceId = interfaceObj->interface.info.interfaceIndex;
    return HDF_SUCCESS;
}

int32_t UsbIfCreatInterfaceObj(const struct UsbInterfacePool *interfacePool, struct UsbSdkInterface **interfaceObj)
{
    struct UsbSdkInterface *interfaceObjTemp = NULL;
    static int32_t idNum = 0;

    if ((interfacePool == NULL) || (interfaceObj == NULL)) {
        HDF_LOGE("%s:%d param is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    interfaceObjTemp = (struct UsbSdkInterface *)RawUsbMemCalloc(sizeof(struct UsbSdkInterface));
    if (interfaceObjTemp == NULL) {
        HDF_LOGE("%s:%d RawUsbMemCalloc faile", __func__, __LINE__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ++idNum;
    idNum %= INTERFACE_POOL_ID_MAX;
    interfaceObjTemp->interface.object.objectId = idNum;
    DListHeadInit(&interfaceObjTemp->interface.object.entry);
    IfInterfaceObjInit(interfaceObjTemp);
    interfaceObjTemp->parentObjectId = interfacePool->object.objectId;

    OsalMutexLock((struct OsalMutex *)&interfacePool->interfaceLock);
    DListInsertTail(&interfaceObjTemp->interface.object.entry, (struct DListHead *)&interfacePool->interfaceList);
    OsalMutexUnlock((struct OsalMutex *)&interfacePool->interfaceLock);

    *interfaceObj = interfaceObjTemp;

    return HDF_SUCCESS;
}

HDF_STATUS UsbIfDestroyInterfaceObj(const struct UsbInterfacePool *interfacePool,
    const struct UsbSdkInterface *interfaceObj)
{
    HDF_STATUS ret = HDF_SUCCESS;
    struct UsbSdkInterface *interfacePos = NULL;
    struct UsbSdkInterface *interfaceTemp = NULL;
    bool found = false;
    bool destroyFlag = false;

    if (interfacePool == NULL) {
        HDF_LOGE("%s:%d interfacePool is NULL", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (DListIsEmpty(&interfacePool->interfaceList)) {
        HDF_LOGE("%s:%d interfaceList is empty ", __func__, __LINE__);
        return HDF_SUCCESS;
    }

    if (interfaceObj == NULL) {
        /* Destroy all interface object */
        destroyFlag = true;
    } else {
        /* Destroys the specified interface object */
        destroyFlag = false;
    }

    OsalMutexLock((struct OsalMutex *)&interfacePool->interfaceLock);
    DLIST_FOR_EACH_ENTRY_SAFE(interfacePos, interfaceTemp, &interfacePool->interfaceList, \
        struct UsbSdkInterface, interface.object.entry) {
        if ((destroyFlag == true) || ((destroyFlag == false) &&
            (interfacePos->interface.object.objectId == interfaceObj->interface.object.objectId))) {
            found = true;
            DListRemove(&interfacePos->interface.object.entry);
            ret = IfFreeInterfaceObj(interfacePos);
            if (ret != HDF_SUCCESS) {
                HDF_LOGE("%s:%d IfFreeInterfaceObj fail, ret=%d ", __func__, __LINE__, ret);
                break;
            }

            if (!destroyFlag) {
                break;
            }
        }
    }
    OsalMutexUnlock((struct OsalMutex *)&interfacePool->interfaceLock);

    if (!found) {
        ret = HDF_FAILURE;
        HDF_LOGE("%s:%d the interface object to be destroyed does not exist", __func__, __LINE__);
    }

    return ret;
}

int32_t UsbIfCreatInterfacePool(const struct UsbSession *session, uint8_t busNum, uint8_t devAddr,
    struct UsbInterfacePool **interfacePool)
{
    struct UsbInterfacePool *interfacePoolTemp = NULL;
    static int32_t idNum = 0;

    if (interfacePool == NULL) {
        HDF_LOGE("%s:%d interfacePool is NULL!", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    interfacePoolTemp = (struct UsbInterfacePool *)RawUsbMemAlloc(sizeof(struct UsbInterfacePool));
    if (interfacePoolTemp == NULL) {
        HDF_LOGE("%s:%d RawUsbMemAlloc faile", __func__, __LINE__);
        *interfacePool = NULL;
        return HDF_ERR_MALLOC_FAIL;
    }

    ++idNum;
    idNum %= INTERFACE_POOL_ID_MAX;
    interfacePoolTemp->object.objectId = idNum;
    interfacePoolTemp->ioProcessTid = 0;

    if (IfInterfacePoolInit(interfacePoolTemp, busNum, devAddr) != HDF_SUCCESS) {
        RawUsbMemFree(interfacePoolTemp);
        *interfacePool = NULL;
        return HDF_ERR_IO;
    }
    OsalMutexLock((struct OsalMutex *)&session->lock);
    DListInsertTail(&interfacePoolTemp->object.entry, (struct DListHead *)&session->ifacePoolList);
    OsalMutexUnlock((struct OsalMutex *)&session->lock);

    *interfacePool = interfacePoolTemp;

    return HDF_SUCCESS;
}

int32_t UsbInitHostSdk(struct UsbSession **session)
{
    return RawInit(session);
}

int32_t UsbExitHostSdk(const struct UsbSession *session)
{
    return RawExit(session);
}

struct UsbInterface *UsbClaimInterfaceUnforce(
    const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr, uint8_t interfaceIndex)
{
    int ret;
    struct UsbInterface *interfaceObj = NULL;
    struct UsbSdkInterface *interfaceSdk = NULL;

    interfaceObj = UsbClaimInterface(session, busNum, usbAddr, interfaceIndex);

    interfaceSdk = (struct UsbSdkInterface *)interfaceObj;
    if (OsalAtomicRead((OsalAtomic *)&interfaceSdk->refCount) > INTERFACE_REFCOUNT_UNFORCE) {
        ret = UsbReleaseInterface(interfaceObj);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:%d UsbReleaseInterface failed!", __func__, __LINE__);
        }
        return NULL;
    }

    return interfaceObj;
}

struct UsbInterface *UsbClaimInterface(
    const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr, uint8_t interfaceIndex)
{
    struct UsbPoolQueryPara poolQueryPara = {0};
    struct UsbInterfacePool *interfacePool = NULL;
    struct UsbInterfaceQueryPara interfaceQueryPara = {0};
    struct UsbSdkInterface *interfaceObj = NULL;
    struct UsbDeviceHandle *devHandle = NULL;
    struct UsbSession *realSession = RawGetSession(session);
    int32_t ret;
    bool claimFlag = false;

    if (realSession == NULL) {
        HDF_LOGE("%s:%d interfacePoolList is empty", __func__, __LINE__);
        return NULL;
    }

    poolQueryPara.type = USB_POOL_NORMAL_TYPE;
    poolQueryPara.busNum = busNum;
    poolQueryPara.usbAddr = usbAddr;
    interfacePool = IfFindInterfacePool(realSession, poolQueryPara, true);
    if ((interfacePool == NULL) || (interfacePool->device == NULL)) {
        interfacePool = IfGetInterfacePool(&devHandle, realSession, busNum, usbAddr);
        if ((interfacePool == NULL) || (interfacePool->device == NULL)) {
            return NULL;
        }
    }

    interfaceQueryPara.type = USB_INTERFACE_INTERFACE_INDEX_TYPE;
    interfaceQueryPara.interfaceIndex = interfaceIndex;
    interfaceObj = IfFindInterfaceObj(interfacePool, interfaceQueryPara, true, &claimFlag, true);
    if (interfaceObj == NULL) {
        goto ERROR;
    }

    if ((interfaceIndex != USB_CTRL_INTERFACE_ID) && (claimFlag == true)) {
        OsalMutexLock(&interfacePool->interfaceLock);
        devHandle = interfacePool->device->devHandle;
        ret = RawClaimInterface(devHandle, interfaceIndex);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:%d RawClaimInterface faile, ret=%d", __func__, __LINE__, ret);
            AdapterAtomicDec(&interfaceObj->refCount);
            OsalMutexUnlock(&interfacePool->interfaceLock);
            goto ERROR;
        }
        OsalMutexUnlock(&interfacePool->interfaceLock);
    }
    interfaceObj->session = realSession;

    return (struct UsbInterface *)interfaceObj;
ERROR:
    (void)IfDestoryDevice(realSession, interfacePool, devHandle, true);
    return NULL;
}

int32_t UsbReleaseInterface(const struct UsbInterface *interfaceObj)
{
    int32_t ret;
    struct UsbPoolQueryPara queryPara;
    struct UsbInterfacePool *interfacePool = NULL;
    struct UsbSdkInterface *interfaceSdk = (struct UsbSdkInterface *)interfaceObj;
    struct UsbDeviceHandle *devHandle = NULL;
    uint8_t interfaceIndex;

    if ((interfaceSdk == NULL) || (interfaceSdk->session == NULL)) {
        HDF_LOGE("%s:%d interfaceObj is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    queryPara.type = USB_POOL_OBJECT_ID_TYPE;
    queryPara.objectId = interfaceSdk->parentObjectId;
    interfacePool = IfFindInterfacePool(interfaceSdk->session, queryPara, false);
    if ((interfacePool == NULL) || (interfacePool->session == NULL)) {
        return HDF_ERR_BAD_FD;
    }

    devHandle = interfacePool->device->devHandle;
    interfaceIndex = interfaceSdk->interface.info.interfaceIndex;
    OsalMutexLock(&interfacePool->interfaceLock);
    if ((interfaceIndex != USB_CTRL_INTERFACE_ID) && (AdapterAtomicDec(&interfaceSdk->refCount) <= 0)) {
        ret = RawReleaseInterface(devHandle, interfaceIndex);
        if ((ret != HDF_SUCCESS) && (ret != HDF_DEV_ERR_NO_DEVICE)) {
            HDF_LOGE("%s:%d RawReleaseInterface faile, ret=%d", __func__, __LINE__, ret);
            AdapterAtomicInc(&interfaceSdk->refCount);
            OsalMutexUnlock(&interfacePool->interfaceLock);
            return ret;
        }
    }
    OsalMutexUnlock(&interfacePool->interfaceLock);

    return IfDestoryDevice(interfacePool->session, interfacePool, devHandle, true);
}

int32_t UsbAddOrRemoveInterface(const struct UsbSession *session, uint8_t busNum, uint8_t usbAddr,
    uint8_t interfaceIndex, UsbInterfaceStatus status)
{
    int32_t ret;
    struct UsbInterfaceQueryPara interfaceQueryPara = {0};
    struct UsbSdkInterface *interfaceObj = NULL;
    struct UsbPoolQueryPara poolQueryPara;
    struct UsbInterfacePool *interfacePool = NULL;
    enum UsbPnpNotifyServiceCmd cmdType = USB_PNP_NOTIFY_ADD_INTERFACE;
    struct UsbPnpAddRemoveInfo infoData = {0};
    struct UsbSession *realSession = RawGetSession(session);

    /* Find interfacePool object */
    poolQueryPara.type = USB_POOL_NORMAL_TYPE;
    poolQueryPara.busNum = busNum;
    poolQueryPara.usbAddr = usbAddr;
    interfacePool = IfFindInterfacePool(realSession, poolQueryPara, false);
    if (interfacePool == NULL) {
        return HDF_ERR_BAD_FD;
    }

    interfaceQueryPara.type = USB_INTERFACE_INTERFACE_INDEX_TYPE;
    interfaceQueryPara.interfaceIndex = interfaceIndex;
    interfaceObj = IfFindInterfaceObj(interfacePool, interfaceQueryPara, false, NULL, false);
    if (interfaceObj == NULL) {
        return HDF_ERR_BAD_FD;
    }

    if (interfaceObj->status == status) {
        HDF_LOGE("%s:%d interfaceObj->status=%d is error",
            __func__, __LINE__, interfaceObj->status);
        return HDF_ERR_INVALID_PARAM;
    }

    if (status == USB_INTERFACE_STATUS_ADD) {
        cmdType = USB_PNP_NOTIFY_ADD_INTERFACE;
    } else if (status == USB_INTERFACE_STATUS_REMOVE) {
        cmdType = USB_PNP_NOTIFY_REMOVE_INTERFACE;
    } else {
        HDF_LOGE("%s:%d status=%d is not define",
            __func__, __LINE__, status);
        return HDF_ERR_INVALID_PARAM;
    }

    infoData.devNum = (int32_t)interfacePool->devAddr;
    infoData.busNum = (int32_t)interfacePool->busNum;
    infoData.interfaceNumber = interfaceObj->interface.info.interfaceIndex;
    infoData.interfaceClass = interfaceObj->interface.info.interfaceClass;
    infoData.interfaceSubClass = interfaceObj->interface.info.interfaceSubClass;
    infoData.interfaceProtocol = interfaceObj->interface.info.interfaceProtocol;
    ret = RawInitPnpService(cmdType, infoData);
    if (ret == HDF_SUCCESS) {
        interfaceObj->status = status;
    }

    return ret;
}

UsbInterfaceHandle *UsbOpenInterface(const struct UsbInterface *interfaceObj)
{
    struct UsbPoolQueryPara poolQueryPara;
    struct UsbInterfacePool *interfacePool = NULL;
    struct UsbInterfaceHandleEntity *ifaceHdl = NULL;

    if (interfaceObj == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return NULL;
    }

    struct UsbSdkInterface *interfaceSdk = (struct UsbSdkInterface *)interfaceObj;
    if ((interfaceSdk != NULL) && ((interfaceSdk->session == NULL)
        || (interfaceSdk->status == USB_INTERFACE_STATUS_REMOVE))) {
        HDF_LOGE("%s:%d interfaceSdk->status=%d is error",
            __func__, __LINE__, interfaceSdk->status);
        return NULL;
    }

    poolQueryPara.type = USB_POOL_OBJECT_ID_TYPE;
    poolQueryPara.objectId = interfaceSdk->parentObjectId;
    interfacePool = IfFindInterfacePool(interfaceSdk->session, poolQueryPara, false);
    if ((interfacePool  == NULL) || (interfacePool->device == NULL)
        || (interfacePool->device->devHandle == NULL)) {
        return NULL;
    }

    OsalMutexLock(&interfacePool->interfaceLock);
    ifaceHdl = RawUsbMemCalloc(sizeof(struct UsbInterfaceHandleEntity));
    if (ifaceHdl == NULL) {
        HDF_LOGE("%s:%d RawUsbMemAlloc failed", __func__, __LINE__);
        goto OUT;
    }
    ifaceHdl->devHandle = interfacePool->device->devHandle;
    ifaceHdl->interfaceIndex = interfaceSdk->interface.info.interfaceIndex;

    if (OsalAtomicRead(&interfacePool->ioRefCount) == 0) {
        HDF_STATUS ret = UsbIoStart(interfacePool);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:%d UsbIoStart faile, ret=%d ", __func__, __LINE__, ret);
            ifaceHdl->devHandle = NULL;
            RawUsbMemFree(ifaceHdl);
            goto OUT;
        }
    }
    AdapterAtomicInc(&interfaceSdk->refCount);
    AdapterAtomicInc(&interfacePool->ioRefCount);
    OsalMutexUnlock(&interfacePool->interfaceLock);

    return (UsbInterfaceHandle *)ifaceHdl;

OUT:
    OsalMutexUnlock(&interfacePool->interfaceLock);
    return NULL;
}

int32_t UsbCloseInterface(const UsbInterfaceHandle *interfaceHandle)
{
    HDF_STATUS ret;
    struct UsbInterfaceHandleEntity *ifaceHdl = NULL;
    struct UsbInterfacePool *interfacePool = NULL;
    struct UsbInterfaceQueryPara interfaceQueryPara = {0};
    struct UsbSdkInterface *interfaceObj = NULL;

    if (interfaceHandle == NULL) {
        HDF_LOGE("%s:%d handle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ifaceHdl = (struct UsbInterfaceHandleEntity *)interfaceHandle;
    if ((ifaceHdl->devHandle == NULL) || (ifaceHdl->devHandle->dev == NULL)
        || (ifaceHdl->devHandle->dev->privateObject == NULL)) {
        HDF_LOGE("%s:%d ifaceHdl is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    interfacePool = (struct UsbInterfacePool *)ifaceHdl->devHandle->dev->privateObject;
    /* Find interface object */
    interfaceQueryPara.type = USB_INTERFACE_INTERFACE_INDEX_TYPE;
    interfaceQueryPara.interfaceIndex = ifaceHdl->interfaceIndex;
    interfaceObj = IfFindInterfaceObj(interfacePool, interfaceQueryPara, false, NULL, false);
    if (interfaceObj == NULL) {
        return HDF_ERR_BAD_FD;
    }

    OsalMutexLock(&interfacePool->interfaceLock);
    if (AdapterAtomicDec(&interfacePool->ioRefCount) == 0) {
        ret = UsbIoStop(interfacePool);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:%d UsbIoStop failed, ret=%d ", __func__, __LINE__, ret);
            goto OUT;
        }
    }
    AdapterAtomicDec(&interfaceObj->refCount);
    ifaceHdl->devHandle = NULL;
    RawUsbMemFree(ifaceHdl);
    OsalMutexUnlock(&interfacePool->interfaceLock);

    return HDF_SUCCESS;
OUT:
    AdapterAtomicInc(&interfacePool->ioRefCount);
    OsalMutexUnlock(&interfacePool->interfaceLock);
    return ret;
}

int32_t UsbSelectInterfaceSetting(
    const UsbInterfaceHandle *interfaceHandle, uint8_t settingIndex, struct UsbInterface **interfaceObj)
{
    struct UsbInterfaceHandleEntity *ifaceHdl = (struct UsbInterfaceHandleEntity *)interfaceHandle;
    struct UsbInterfacePool *interfacePool = NULL;
    struct UsbSdkInterface *interfacePos = NULL;
    struct UsbSdkInterface *interfaceTemp = NULL;
    struct UsbInterfaceQueryPara interfaceQueryPara = {0};
    int32_t ret;

    if ((interfaceHandle == NULL) || (interfaceObj == NULL)) {
        HDF_LOGE("%s:%d handle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    interfacePool = (struct UsbInterfacePool *)ifaceHdl->devHandle->dev->privateObject;
    if (interfacePool == NULL) {
        HDF_LOGE("%s:%d interfacePool is NULL", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    ret = RawSetInterfaceAltsetting(ifaceHdl->devHandle, ifaceHdl->interfaceIndex, settingIndex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d RawEnableInterface faile, ret=%d", __func__, __LINE__, ret);
        return ret;
    }

    OsalMutexLock(&interfacePool->interfaceLock);
    DLIST_FOR_EACH_ENTRY_SAFE(interfacePos, interfaceTemp, &interfacePool->interfaceList,
                              struct UsbSdkInterface, interface.object.entry) {
        if (interfacePos->interface.info.interfaceIndex == ifaceHdl->interfaceIndex) {
            interfacePos->interface.info.curAltSetting = settingIndex;
        }
    }
    OsalMutexUnlock(&interfacePool->interfaceLock);

    interfaceQueryPara.type = USB_INTERFACE_INTERFACE_INDEX_TYPE;
    interfaceQueryPara.interfaceIndex = ifaceHdl->interfaceIndex;
    interfaceTemp = IfFindInterfaceObj(interfacePool, interfaceQueryPara, false, NULL, true);
    if (interfaceTemp == NULL) {
        return HDF_FAILURE;
    }
    interfaceTemp->session = interfacePool->session;

    *interfaceObj = &interfaceTemp->interface;

    return HDF_SUCCESS;
}

int32_t UsbGetPipeInfo(const UsbInterfaceHandle *interfaceHandle, uint8_t altSettingIndex,
    uint8_t pipeId, struct UsbPipeInfo *pipeInfo)
{
    struct UsbInterfaceHandleEntity *ifaceHdl = (struct UsbInterfaceHandleEntity *)interfaceHandle;
    struct UsbInterfacePool *interfacePool = NULL;
    struct UsbInterfaceQueryPara interfaceQueryPara;
    struct UsbSdkInterface *interfaceObj = NULL;
    struct UsbPipeQueryPara pipeQueryPara;
    struct UsbPipe *pipeObj = NULL;

    if ((interfaceHandle == NULL) || (pipeInfo == NULL) || (ifaceHdl == NULL)
        || (ifaceHdl->devHandle == NULL) || (ifaceHdl->devHandle->dev == NULL)) {
        HDF_LOGE("%s:%d invalid parameter", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    /* Find interfacePool object */
    interfacePool = (struct UsbInterfacePool *)ifaceHdl->devHandle->dev->privateObject;
    if (interfacePool == NULL) {
        HDF_LOGE("%s:%d interfacePool is NULL", __func__, __LINE__);
        return HDF_ERR_BAD_FD;
    }

    /* Find interface object */
    interfaceQueryPara.type = USB_INTERFACE_ALT_SETTINGS_TYPE;
    interfaceQueryPara.interfaceIndex = ifaceHdl->interfaceIndex;
    interfaceQueryPara.altSettingId = altSettingIndex;
    interfaceObj = IfFindInterfaceObj(interfacePool, interfaceQueryPara, false, NULL, true);
    if (interfaceObj == NULL) {
        return HDF_ERR_BAD_FD;
    }

    /* Find pipe object */
    pipeQueryPara.type = USB_PIPE_INDEX_TYPE;
    pipeQueryPara.pipeId = pipeId;
    pipeObj = IfFindPipeObj(interfaceObj, pipeQueryPara);
    if (pipeObj == NULL) {
        return HDF_ERR_BAD_FD;
    }

    *pipeInfo = pipeObj->info;

    return HDF_SUCCESS;
}

int32_t UsbClearInterfaceHalt(const UsbInterfaceHandle *interfaceHandle, uint8_t pipeAddress)
{
    struct UsbInterfaceHandleEntity *ifaceHdl = (struct UsbInterfaceHandleEntity *)interfaceHandle;

    if ((ifaceHdl == NULL) || (ifaceHdl->devHandle == NULL)) {
        HDF_LOGE("%s:%d handle is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    return RawClearHalt(ifaceHdl->devHandle, pipeAddress);
}

struct UsbRequest *UsbAllocRequest(const UsbInterfaceHandle *interfaceHandle, int32_t isoPackets, int32_t length)
{
    struct UsbInterfaceHandleEntity *ifaceHdl = (struct UsbInterfaceHandleEntity *)interfaceHandle;
    struct UsbIfRequest *requestObj = NULL;
    struct UsbHostRequest *hostRequest = NULL;

    if ((ifaceHdl == NULL) || (ifaceHdl->devHandle == NULL)) {
        HDF_LOGE("%s:%d handle is NULL ", __func__, __LINE__);
        return NULL;
    }

    requestObj = (struct UsbIfRequest *)RawUsbMemCalloc(sizeof(struct UsbIfRequest));
    if (requestObj == NULL) {
        HDF_LOGE("%s:%d RawUsbMemCalloc UsbRequest error. ", __func__, __LINE__);
        return NULL;
    }

    hostRequest = RawAllocRequest(ifaceHdl->devHandle, isoPackets, length);
    if (hostRequest == NULL) {
        RawUsbMemFree(requestObj);
        return NULL;
    }
    hostRequest->devHandle = ifaceHdl->devHandle;

    ++g_usbRequestObjectId;
    g_usbRequestObjectId %= MAX_OBJECT_ID;
    requestObj->request.object.objectId = g_usbRequestObjectId;
    DListHeadInit(&requestObj->request.object.entry);
    requestObj->request.compInfo.type = USB_REQUEST_TYPE_INVALID;
    requestObj->request.compInfo.buffer = hostRequest->buffer;
    requestObj->request.compInfo.length = (uint32_t)hostRequest->length;
    requestObj->hostRequest = hostRequest;
    requestObj->isSyncReq = false;
    hostRequest->privateObj = requestObj;

    return (struct UsbRequest *)requestObj;
}

int32_t UsbFreeRequest(const struct UsbRequest *request)
{
    struct UsbHostRequest *hostRequest = NULL;
    struct UsbIfRequest *requestObj = (struct UsbIfRequest *)request;
    int32_t ret;

    if (requestObj == NULL) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    hostRequest = requestObj->hostRequest;
    if (hostRequest == NULL) {
        HDF_LOGE("%s:%d hostRequest is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = RawFreeRequest(hostRequest);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    RawUsbMemFree(requestObj);

    return ret;
}

int32_t UsbSubmitRequestAsync(const struct UsbRequest *request)
{
    if (request == NULL) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct UsbIfRequest *requestObj = (struct UsbIfRequest *)request;
    requestObj->isSyncReq = false;
    if (memset_s((void *)&request->compInfo, sizeof(request->compInfo), 0, sizeof(request->compInfo)) != EOK) {
        HDF_LOGE("%s:%d memset_s failed ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return IfSubmitRequestToQueue(requestObj);
}

int32_t UsbFillRequest(const struct UsbRequest *request, const UsbInterfaceHandle *interfaceHandle,
    const struct UsbRequestParams *params)
{
    struct UsbInterfaceHandleEntity *ifaceHdl = (struct UsbInterfaceHandleEntity *)interfaceHandle;
    struct UsbIfRequest *requestObj = (struct UsbIfRequest *)request;
    struct UsbHostRequest *hostRequest = NULL;
    UsbPipeType pipeType;
    UsbRequestDirection directon;
    int32_t ret;

    if ((requestObj == NULL) || (params == NULL) || (ifaceHdl == NULL) || (ifaceHdl->devHandle == NULL)) {
        HDF_LOGE("%s:%d params or request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    hostRequest = requestObj->hostRequest;
    if (hostRequest == NULL) {
        HDF_LOGE("%s:%d hostRequest is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = IfGetRequestPipeType(ifaceHdl->devHandle, params->interfaceId, \
        params->pipeId, &pipeType);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d IfGetRequestPipeType error, ret=%d", __func__, __LINE__, ret);
        return ret;
    }

    ret = IfFillRequestByPipeType(requestObj, pipeType, hostRequest, ifaceHdl->devHandle, params);
    if (params->requestType == USB_REQUEST_PARAMS_DATA_TYPE) {
        directon = (params->pipeAddress >> USB_DIR_OFFSET) & 0x01;
        if (directon == USB_REQUEST_DIR_TO_DEVICE) {
            requestObj->request.compInfo.type = USB_REQUEST_TYPE_PIPE_WRITE;
        } else if (directon == USB_REQUEST_DIR_FROM_DEVICE) {
            requestObj->request.compInfo.type = USB_REQUEST_TYPE_PIPE_READ;
        }
    }

    return ret;
}

int32_t UsbCancelRequest(const struct UsbRequest *request)
{
    int32_t ret;
    struct UsbHostRequest *hostRequest = NULL;
    struct UsbIfRequest *requestObj = (struct UsbIfRequest *)request;

    if ((requestObj == NULL) || (requestObj->hostRequest == NULL)) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    hostRequest = requestObj->hostRequest;
    ret = RawCancelRequest(hostRequest);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d RawCancelRequest faile, ret=%d ", __func__, __LINE__, ret);
        return ret;
    }

    requestObj->request.compInfo.status = USB_REQUEST_CANCELLED;

    return HDF_SUCCESS;
}

int32_t UsbSubmitRequestSync(const struct UsbRequest *request)
{
    int32_t ret;
    uint32_t waitTime;
    struct UsbIfRequest *requestObj = (struct UsbIfRequest *)request;

    if ((request == NULL) || (requestObj->hostRequest == NULL)) {
        HDF_LOGE("%s:%d request is NULL", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    /* Init request semaphore */
    if (OsalSemInit(&requestObj->hostRequest->sem, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalSemInit faile! ", __func__, __LINE__);
        return HDF_ERR_IO;
    }
    requestObj->request.compInfo.status = USB_REQUEST_COMPLETED;
    if (requestObj->hostRequest->timeout == USB_RAW_REQUEST_TIME_ZERO_MS) {
        waitTime = HDF_WAIT_FOREVER;
    } else {
        waitTime = requestObj->hostRequest->timeout;
    }

    requestObj->isSyncReq = true;
    ret = IfSubmitRequestToQueue(requestObj);
    if (ret != HDF_SUCCESS) {
        goto OUT;
    }

    ret = OsalSemWait(&requestObj->hostRequest->sem, waitTime);
    if (ret == HDF_ERR_TIMEOUT) {
        UsbCancelRequest(&requestObj->request);
        if (OsalSemWait(&requestObj->hostRequest->sem, waitTime) == HDF_ERR_TIMEOUT) {
            HDF_LOGE("%s:%d UsbCancelRequest sem wait timeout!", __func__, __LINE__);
        }
        requestObj->request.compInfo.status = USB_REQUEST_TIMEOUT;
    } else if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:%d OsalSemWait faile, ret=%d ", __func__, __LINE__, ret);
    }

OUT:
    OsalSemDestroy(&requestObj->hostRequest->sem);
    return ret;
}

int32_t UsbMemTestTrigger(bool enable)
{
    return RawUsbMemTestTrigger(enable);
}

