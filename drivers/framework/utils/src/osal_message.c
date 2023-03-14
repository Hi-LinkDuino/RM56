/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_message.h"
#include "osal_mem.h"

struct HdfMessage *HdfMessageObtain(size_t extendSize)
{
    size_t newSize = sizeof(struct HdfMessage) + extendSize;
    return (struct HdfMessage *)OsalMemCalloc(newSize);
}

void HdfMessageRecycle(struct HdfMessage *message)
{
    OsalMemFree(message);
}

void HdfMessageDelete(struct HdfSListNode *listEntry)
{
    struct HdfMessage *message = (struct HdfMessage *)listEntry;
    if (message != NULL) {
        HdfMessageRecycle(message);
    }
}

