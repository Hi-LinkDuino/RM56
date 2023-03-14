/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_MESSAGE_TASK_H
#define HDF_MESSAGE_TASK_H

#include "hdf_message_looper.h"
#include "osal_msg_queue.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfMessageTask;

struct IHdfMessageHandler {
    int32_t (*Dispatch)(struct HdfMessageTask *task, struct HdfMessage *msg);
};

struct HdfMessageTask {
    int32_t (*SendMessage)(struct HdfMessageTask *task, struct HdfMessage *msg, bool sync);
    void (*RemoveMessage)(struct HdfMessageTask *task, struct HdfMessage *msg);
    void (*SendMessageLater)(struct HdfMessageTask *task, struct HdfMessage *msg, long delay);
    void (*DispatchMessage)(struct HdfMessageTask *task, struct HdfMessage *msg);
    struct HdfMessageQueue *messageQueue;
    struct IHdfMessageHandler *messageHandler;
};

void HdfMessageTaskConstruct(struct HdfMessageTask *inst,
    struct HdfMessageLooper *looper, struct IHdfMessageHandler *handler);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_MESSAGE_TASK_H */
