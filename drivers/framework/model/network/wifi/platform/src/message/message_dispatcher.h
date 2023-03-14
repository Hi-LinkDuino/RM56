/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H
#include "osal/osal_mutex.h"
#include "hdf_wlan_priority_queue.h"
#include "shared_obj.h"
#include "sidecar.h"
#include "message_config.h"

#define BAD_DISPATCHER_ID 255
#define DISPATCHER_ID_AUTO 254

#define MAX_PRI_LEVEL_COUNT 3

struct MessageDispatcher;

#define INHERT_MESSAGE_DISPATCHER \
    SHARED_OBJ(MessageDispatcher); \
    OSAL_DECLARE_MUTEX(mutex); \
    ErrorCode (*AppendMessage)(struct MessageDispatcher *, const uint8_t priority, MessageContext * context); \
    ErrorCode (*Start)(struct MessageDispatcher * dispatcher);                        \
    void (*Shutdown)(struct MessageDispatcher * dispatcher);                          \
    PriorityQueue *messageQueue

typedef struct MessageDispatcher {
    INHERT_MESSAGE_DISPATCHER;
} MessageDispatcher;

void ReleaseMessageMapper(struct ServiceDef *mapper);
void ReleaseMessageContext(MessageContext *context);
void SetToResponse(MessageContext *context);

struct MessageDef* GetMsgDef(const struct ServiceDef *mapper, uint32_t commandID);

ErrorCode CreateLocalDispatcher(MessageDispatcher **dispatcher, const DispatcherConfig *config);

#endif
