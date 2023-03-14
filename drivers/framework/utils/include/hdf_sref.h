/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SREF_H
#define HDF_SREF_H

#include "osal_atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HdfSRef;

struct IHdfSRefListener {
    void (*OnFirstAcquire)(struct HdfSRef *);
    void (*OnLastRelease)(struct HdfSRef *);
};

struct HdfSRef {
    OsalAtomic refs;
    struct IHdfSRefListener *listener;
    void (*Acquire)(struct HdfSRef *);
    void (*Release)(struct HdfSRef *);
    int (*Count)(const struct HdfSRef *);
};

void HdfSRefAcquire(struct HdfSRef *sref);
void HdfSRefRelease(struct HdfSRef *sref);
int HdfSRefCount(const struct HdfSRef *sref);
void HdfSRefConstruct(struct HdfSRef *sref, struct IHdfSRefListener *listener);

#ifdef __cplusplus
}
#endif

#endif /* HDF_SREF_H */
