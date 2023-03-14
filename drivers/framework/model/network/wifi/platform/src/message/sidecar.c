/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "hdf_base.h"
#include "securec.h"
#include "utils/hdf_log.h"
#include "message_router.h"
#include "message_router_inner.h"
#include "sidecar.h"

#ifdef USERSPACE_CLIENT_SUPPORT
#define HDF_LOG_TAG UMsgEngine
#else
#define HDF_LOG_TAG KMsgEngine
#endif

// Service has only one private data.The service ID of current service
typedef struct {
    DispatcherId dispatcherId;
    ServiceId serviceId;
} SideCarPrivateData;

static ErrorCode MessageInputCheck(const Service *sideCar, ServiceId receiver, struct HdfSBuf *sendData)
{
    SideCarPrivateData *privateData = NULL;
    if (sideCar == NULL || sideCar->privateData == NULL) {
        HDF_LOGE("%s:sideCar or sideCar.privateData is NULL", __func__);
        return ME_ERROR_NULL_PTR;
    }

    privateData = (SideCarPrivateData *)sideCar->privateData;
    if (receiver >= MESSAGE_ENGINE_MAX_SERVICE || privateData->serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        return ME_ERROR_NO_SUCH_SERVICE;
    }
    return ME_SUCCESS;
}

static MessageContext *CreateMessageContext(ServiceId sender, ServiceId receiver, uint32_t commandId,
    struct HdfSBuf *sendData)
{
    MessageContext *context = (MessageContext *)OsalMemCalloc(sizeof(MessageContext));
    if (context == NULL) {
        return NULL;
    }

    context->commandId = commandId;
    context->senderId = sender;
    context->receiverId = receiver;
    context->reqData = sendData;
    context->crossNode = false;

    return context;
}

#define MESSAGE_CMD_BITS 16
#define MESSAGE_CMD_MASK 0xffff

inline static uint32_t GetServiceID(uint32_t id)
{
    return (id & ~((uint32_t)(MESSAGE_CMD_MASK))) >> MESSAGE_CMD_BITS;
}

inline static uint32_t GetCmd(uint32_t id)
{
    return id & MESSAGE_CMD_MASK;
}

int32_t DispatchToMessage(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    ErrorCode errCode;
    ServiceId serviceId = GetServiceID(id);
    uint32_t cmd = GetCmd(id);
    MessageContext *context = NULL;
    RemoteService *targetService = NULL;

    if (client == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (serviceId >= MESSAGE_ENGINE_MAX_SERVICE) {
        return ME_ERROR_NO_SUCH_SERVICE;
    }
    context = CreateMessageContext(RESERVED_SERVICE_ID, serviceId, cmd, reqData);
    if (context == NULL) {
        return ME_ERROR_NULL_PTR;
    }
    context->rspData = rspData;
    context->requestType = MESSAGE_TYPE_SYNC_REQ;
    context->client = client;
    do {
        targetService = RefRemoteService(serviceId);
        if (targetService == NULL || targetService->SendMessage == NULL) {
            HDF_LOGE("%s:Target service is NULL or has no SendMessage!", __func__);
            errCode = ME_ERROR_NO_SUCH_SERVICE;
            break;
        }

        errCode = targetService->SendMessage(targetService, context);
    } while (false);
    if (targetService != NULL && targetService->Disref != NULL) {
        targetService->Disref(targetService);
    }
    OsalMemFree(context);
    return errCode;
}

static ErrorCode SideCarSendSyncMessage(const Service *sideCar, ServiceId receiver, uint32_t commandId,
    struct HdfSBuf *sendData, struct HdfSBuf *recvData)
{
    SideCarPrivateData *privateData = NULL;
    MessageContext *context = NULL;
    RemoteService *targetService = NULL;
    ErrorCode errCode = MessageInputCheck(sideCar, receiver, sendData);
    if (errCode != ME_SUCCESS) {
        return errCode;
    }
    privateData = (SideCarPrivateData *)sideCar->privateData;
    context = CreateMessageContext(privateData->serviceId, receiver, commandId, sendData);
    if (context == NULL) {
        return ME_ERROR_NULL_PTR;
    }
    context->rspData = recvData;
    context->requestType = MESSAGE_TYPE_SYNC_REQ;
    do {
        targetService = RefRemoteService(receiver);
        if (targetService == NULL || targetService->SendMessage == NULL) {
            HDF_LOGE("Target service is NULL or has no SendMessage!");
            errCode = ME_ERROR_NO_SUCH_SERVICE;
            break;
        }

        errCode = targetService->SendMessage(targetService, context);
    } while (false);
    if (targetService != NULL && targetService->Disref != NULL) {
        targetService->Disref(targetService);
    }
    OsalMemFree(context);
    return errCode;
}

static ErrorCode SideCarSendAsyncMessageInner(const Service *sideCar, ServiceId receiver, uint32_t commandId,
    struct HdfSBuf *reqData, MessageCallBack callback)
{
    SideCarPrivateData *privateData = NULL;
    MessageContext *context = NULL;
    struct HdfSBuf *rspData = NULL;
    RemoteService *targetService = NULL;
    ErrorCode errCode = MessageInputCheck(sideCar, receiver, reqData);
    if (errCode != ME_SUCCESS) {
        return errCode;
    }

    privateData = (SideCarPrivateData *)sideCar->privateData;
    context = CreateMessageContext(privateData->serviceId, receiver, commandId, reqData);
    if (context == NULL) {
        return ME_ERROR_NULL_PTR;
    }
    rspData = HdfSbufObtainDefaultSize();
    if (rspData == NULL) {
        OsalMemFree(context);
        return HDF_FAILURE;
    }
    context->requestType = MESSAGE_TYPE_ASYNC_REQ;
    context->callback = callback;
    context->rspData = rspData;
    do {
        targetService = RefRemoteService(receiver);
        if (targetService == NULL || targetService->SendMessage == NULL) {
            HDF_LOGE("Target service is NULL or has no SendMessage!");
            errCode = ME_ERROR_NO_SUCH_SERVICE;
            break;
        }

        errCode = targetService->SendMessage(targetService, context);
    } while (false);
    if (targetService != NULL && targetService->Disref != NULL) {
        targetService->Disref(targetService);
    }
    if (errCode != ME_SUCCESS) {
        HdfSbufRecycle(rspData);
        OsalMemFree(context);
    }
    return errCode;
}

static ErrorCode SideCarSendAsyncMessage(const Service *sideCar, ServiceId receiver, uint32_t commandId,
    struct HdfSBuf *reqData, MessageCallBack callback)
{
    if (callback == NULL) {
        HDF_LOGE("%s:Callback function can not be NULL.Did you mean SendOnewayMessage?", __func__);
        return ME_ERROR_PARA_WRONG;
    }
    return SideCarSendAsyncMessageInner(sideCar, receiver, commandId, reqData, callback);
}

static ErrorCode SideCarSendOneWayMessage(const struct SideCar_ *sideCar, ServiceId receiver, uint32_t commandId,
    struct HdfSBuf *reqData)
{
    return SideCarSendAsyncMessageInner(sideCar, receiver, commandId, reqData, NULL);
}

static ErrorCode DestroyService(Service *service)
{
    SideCarPrivateData *data = NULL;
    ErrorCode errCode;
    if (service == NULL) {
        return ME_ERROR_NULL_PTR;
    }

    if (service->privateData == NULL) {
        HDF_LOGE("%s:privateData is NULL!", __func__);
        return ME_ERROR_PARA_WRONG;
    }
    data = (SideCarPrivateData *)service->privateData;
    HDF_LOGE("Destroy service! id=%d", data->serviceId);
    errCode = UnregistLocalService(data->dispatcherId, data->serviceId);
    if (errCode != ME_SUCCESS) {
        HDF_LOGE("Unregist service failed!ret=%d", errCode);
        return errCode;
    }

    OsalMemFree(data);
    service->privateData = NULL;
    OsalMemFree(service);
    return ME_SUCCESS;
}

Service *InitService(struct ServiceDef *def, const ServiceCfg *cfg)
{
    Service *service = NULL;
    SideCarPrivateData *privateData = NULL;
    ErrorCode errCode;
    if (cfg == NULL || def == NULL) {
        return NULL;
    }

    if (def->serviceId == RESERVED_SERVICE_ID) {
        HDF_LOGE("%s:Init service with ID 0 is not allowed!", __func__);
        return NULL;
    }

    service = (Service *)OsalMemCalloc(sizeof(Service));
    if (service == NULL) {
        HDF_LOGE("%s:OsalMemAlloc return NULL!", __func__);
        return NULL;
    }
    service->SendAsyncMessage = SideCarSendAsyncMessage;
    service->SendSyncMessage = SideCarSendSyncMessage;
    service->SendOneWayMessage = SideCarSendOneWayMessage;
    service->Destroy = DestroyService;

    privateData = (SideCarPrivateData *)OsalMemCalloc(sizeof(SideCarPrivateData));
    if (privateData == NULL) {
        OsalMemFree(service);
        HDF_LOGE("%s:OsalMemAlloc return NULL!", __func__);
        return NULL;
    }
    privateData->serviceId = def->serviceId;
    privateData->dispatcherId = cfg->dispatcherId;
    service->privateData = (void *)privateData;
    privateData = NULL;

    errCode = RegistLocalService(cfg->dispatcherId, def);
    if (errCode != ME_SUCCESS) {
        OsalMemFree(service->privateData);
        service->privateData = NULL;
        OsalMemFree(service);
        service = NULL;
        HDF_LOGE("Register service failed!serviceId=%d,ret=%d", def->serviceId, errCode);
        return NULL;
    }
    return service;
}
