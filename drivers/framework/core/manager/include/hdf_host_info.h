/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_HOST_INFO_H
#define HDF_HOST_INFO_H

#include "hdf_slist.h"

struct HdfHostInfo {
    struct HdfSListNode node;
    uint16_t hostId;
    uint16_t priority;
    const char *hostName;
};

struct HdfHostInfo *HdfHostInfoNewInstance(void);
void HdfHostInfoFreeInstance(struct HdfHostInfo *hostInfo);
void HdfHostInfoDelete(struct HdfSListNode *listEntry);

#endif /* HDF_HOST_INFO_H */
