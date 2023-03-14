/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MESSAGE_CONFIG_H
#define MESSAGE_CONFIG_H
#include "message_types.h"

#ifndef MESSAGE_ENGINE_MAX_SERVICE
#define MESSAGE_ENGINE_MAX_SERVICE 15
#endif

#ifndef MESSAGE_ENGINE_MAX_DISPATCHER
#define MESSAGE_ENGINE_MAX_DISPATCHER 10
#endif

#ifndef MESSAGE_ENGINE_DISPATCHERID_ASSIGN_START
#define MESSAGE_ENGINE_DISPATCHERID_ASSIGN_START 5
#endif

#ifndef DEFAULT_DISPATCHER_QUEUE_SIZE
#define DEFAULT_DISPATCHER_QUEUE_SIZE 128
#endif

#ifndef DEFAULT_DISPATCHER_PRIORITY_COUNT
#define DEFAULT_DISPATCHER_PRIORITY_COUNT 2
#endif

#define IOCTL_SEND_QUEUE_SIZE 300
#define IOCTL_SEND_QUEUE_PROPIRTY_LEVEL 1

#define MAX_OBJ_REF_COUNT 200

#define MAX_NODE_REF_COUNT 200

#define MESSAGE_DEVICE "/dev/msg_engine"
#define MESSAGE_DEVICE_MODE 0660

#define QUEUE_OPER_TIMEOUT 1000

#define MAX_BLOCK_SIZE 16380L

#define SYSTEM_ALIGN_WIDTH (sizeof(void*))

#define USER_TO_KERNEL_SYNC_MESSAGE_TIMEOUT 10000L

typedef struct DispatcherConfig_ {
    DispatcherId dispatcherId;
    uint8_t priorityLevelCount;
    uint16_t queueSize;
} DispatcherConfig;

#endif