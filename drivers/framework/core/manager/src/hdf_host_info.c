/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_host_info.h"
#include "osal_mem.h"

struct HdfHostInfo *HdfHostInfoNewInstance()
{
    return (struct HdfHostInfo *)OsalMemCalloc(sizeof(struct HdfHostInfo));
}

void HdfHostInfoFreeInstance(struct HdfHostInfo *hostInfo)
{
    if (hostInfo != NULL) {
        OsalMemFree(hostInfo);
    }
}

void HdfHostInfoDelete(struct HdfSListNode *listEntry)
{
    struct HdfHostInfo *hostInfo = (struct HdfHostInfo *)listEntry;
    if (hostInfo != NULL) {
        HdfHostInfoFreeInstance(hostInfo);
    }
}