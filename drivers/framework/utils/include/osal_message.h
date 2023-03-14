/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OSAL_MESSAGE_H
#define OSAL_MESSAGE_H

#include "hdf_slist.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfMessage {
    struct HdfSListNode entry;
    struct HdfMessageTask *target;
    int16_t messageId;
    uint64_t timeStamp;
    void *data[1];
};

struct HdfMessage *HdfMessageObtain(size_t extendSize);
void HdfMessageRecycle(struct HdfMessage *message);
void HdfMessageDelete(struct HdfSListNode *listEntry);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_MESSAGE_H */
