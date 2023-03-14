/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SIDECAR_H
#define SIDECAR_H
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal/osal_sem.h"
#include "message_types.h"
#include "hdf_sbuf.h"

enum MessageType {
    MESSAGE_REQ_START = 0,
    MESSAGE_TYPE_SYNC_REQ,
    MESSAGE_TYPE_ASYNC_REQ,

    MESSAGE_RSP_START,
    MESSAGE_TYPE_SYNC_RSP,
    MESSAGE_TYPE_ASYNC_RSP,
};

#define INHERT_REQUEST_CONTEXT \
    uint32_t commandId;        \
    uint8_t senderId;          \
    uint8_t receiverId;        \
    uint8_t requestType;       \
    bool crossNode;            \
    struct HdfDeviceIoClient *client

#define RESERVED_SERVICE_ID 0
#define BAD_SERVICE_ID 254

#ifdef __cplusplus
extern "C" {
#endif

int32_t DispatchToMessage(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *reqData, struct HdfSBuf *rspData);

typedef struct {
    INHERT_REQUEST_CONTEXT;
} RequestContext;

typedef void (*MessageCallBack)(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData,
    ErrorCode responseStatus);

typedef ErrorCode (*MessageHandler)(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData);

struct MessageContext {
    INHERT_REQUEST_CONTEXT;
#if defined(KERNEL_SERVER_SUPPORT) || defined(USERSPACE_CLIENT_SUPPORT)
    uint16_t messageID;
#endif
    ErrorCode responseStatus;
    struct HdfSBuf *reqData;
    struct HdfSBuf *rspData;
    union {
        MessageCallBack callback;
        OSAL_DECLARE_SEMAPHORE(rspSemaphore);
    };
};
typedef struct MessageContext MessageContext;

typedef struct SideCar_ {
    ErrorCode (*SendOneWayMessage)(const struct SideCar_ *sideCar, ServiceId receiver, uint32_t commandId,
        struct HdfSBuf *reqData);

    ErrorCode (*SendSyncMessage)(const struct SideCar_ *sideCar, ServiceId receiver, uint32_t commandId,
        struct HdfSBuf *sendData, struct HdfSBuf *recvData);

    ErrorCode (*SendAsyncMessage)(const struct SideCar_ *sideCar, ServiceId receiver, uint32_t commandId,
        struct HdfSBuf *reqData, MessageCallBack callback);

    ErrorCode (*Destroy)(struct SideCar_ *sideCar);

    void *privateData;
} Service;

typedef struct {
    DispatcherId dispatcherId;
} ServiceCfg;

struct MessageDef {
    MessageHandler handler;
    uint8_t pri;
};

struct ServiceDef {
    ServiceId serviceId;
    uint8_t messagesLength;
    struct MessageDef *messages;
};

Service *InitService(struct ServiceDef *def, const ServiceCfg *cfg);

#define DUEMessage(CMDID, HANDLER, PRI) \
    [CMDID] = {                         \
        .handler = HANDLER,             \
        .pri = PRI                      \
    }

#define ServiceDefine(ServiceName, SERVICE_ID, ServiceMap)                    \
    Service *CreateService##ServiceName(const ServiceCfg *cfg)                \
    {                                                                         \
        static struct ServiceDef serviceDef = {                               \
            .serviceId = SERVICE_ID,                                          \
            .messagesLength = sizeof(ServiceMap) / sizeof(struct MessageDef), \
            .messages = ServiceMap                                            \
        };                                                                    \
        return InitService(&serviceDef, cfg);                                 \
    }

#define CreateService(ServiceName, cfg) CreateService##ServiceName(cfg)


#ifdef __cplusplus
}
#endif

#endif