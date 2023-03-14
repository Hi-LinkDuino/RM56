/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_MESSAGE_LOOPER_H
#define HDF_MESSAGE_LOOPER_H

#include "osal_msg_queue.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MESSAGE_STOP_LOOP (-1)

struct HdfMessageLooper {
    struct HdfMessageQueue messageQueue;
    void (*Start)(struct HdfMessageLooper *);
    void (*Stop)(struct HdfMessageLooper *);
};

void HdfMessageLooperConstruct(struct HdfMessageLooper *looper);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_MESSAGE_LOOPER_H */
