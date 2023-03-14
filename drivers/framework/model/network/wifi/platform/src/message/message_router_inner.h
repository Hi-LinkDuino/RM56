/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#ifndef MESSAGE_ROUTER_INNER_H
#define MESSAGE_ROUTER_INNER_H
#include "message_types.h"
#include "message_router.h"
#include "message_dispatcher.h"
#include "shared_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

struct MessageNode;
struct RemoteService;

#define INHERT_MESSAGE_NODE                                                                              \
    SHARED_OBJ(MessageNode);                                                                            \
    OSAL_DECLARE_MUTEX(mutex);                                                                           \
    ErrorCode (*Init)(struct MessageNode *);                                                            \
    struct RemoteService *(*CreateRemoteService)(struct MessageNode *, MessageDispatcher * dispatcher, \
        struct ServiceDef * mapper);                                                                     \
    ErrorCode (*SyncService)(struct MessageNode *);                                                     \
    ErrorCode (*NotifyServiceAdd)(const struct MessageNode *, const struct ServiceDef *mapper);         \
    ErrorCode (*NotifyServiceDel)(const struct MessageNode *, const ServiceId serviceId)

DECLEAR_SHARED_OBJ_FUNC(MessageNode);

typedef struct MessageNode {
    INHERT_MESSAGE_NODE;
} MessageNode;

#define INHERT_REMOTE_SERVICE SHARED_OBJ(RemoteService);                                                              \
    void (*ExecRequestMsg)(const struct RemoteService *service, MessageContext *context);   \
    void (*ExecResponseMsg)(const struct RemoteService *service, MessageContext *context);  \
    ErrorCode (*SendMessage)(const struct RemoteService *service, MessageContext *context); \
    void (*Shutdown)(struct RemoteService * service);                                       \
    ServiceId serviceId

DECLEAR_SHARED_OBJ_FUNC(RemoteService);

typedef struct RemoteService {
    INHERT_REMOTE_SERVICE;
} RemoteService;

RemoteService *RefRemoteService(ServiceId id);

ErrorCode SendMessage(MessageContext *context);

ErrorCode CreateLocalNode(MessageNode **node);

ErrorCode AddDispatcherInner(const NodeId nodeId, DispatcherConfig *config);

void DestroyDispatcherInner(const NodeId nodeId, const DispatcherId dispatcherId);

ErrorCode RegistRemoteService(NodeId nodeId, RemoteService *service);

ErrorCode UnregistRemoteService(NodeId nodeId, ServiceId serviceId);

MessageNode *RefMessageNode(const NodeId nodeId, bool isRequireLock);

#ifdef __cplusplus
}
#endif

#endif
