/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_sref.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hdf_sref

void HdfSRefAcquire(struct HdfSRef *sref)
{
    int32_t lockRef;
    if (sref == NULL) {
        HDF_LOGE("Acquire input sref is null");
        return;
    }
    OsalAtomicInc(&sref->refs);
    lockRef = OsalAtomicRead(&sref->refs);
    if ((lockRef == 1) && (sref->listener != NULL)) {
        struct IHdfSRefListener *listener = sref->listener;
        if (listener->OnFirstAcquire != NULL) {
            listener->OnFirstAcquire(sref);
        }
    }
}

int HdfSRefCount(const struct HdfSRef *sref)
{
    if (sref == NULL) {
        HDF_LOGE("invalid sref");
        return 0;
    }

    return OsalAtomicRead(&sref->refs);
}

void HdfSRefRelease(struct HdfSRef *sref)
{
    int32_t lockRef;
    if (sref == NULL) {
        HDF_LOGE("Release input sref is null");
        return;
    }
    OsalAtomicDec(&sref->refs);
    lockRef = OsalAtomicRead(&sref->refs);
    if ((lockRef == 0) && (sref->listener != NULL)) {
        struct IHdfSRefListener *listener = sref->listener;
        if (listener->OnLastRelease != NULL) {
            listener->OnLastRelease(sref);
        }
    }
}

void HdfSRefConstruct(struct HdfSRef *sref, struct IHdfSRefListener *listener)
{
    if ((sref == NULL) || (listener == NULL)) {
        HDF_LOGE("Input params is invalid");
        return;
    }
    OsalAtomicSet(&sref->refs, 0);
    sref->listener = listener;
    sref->Acquire = HdfSRefAcquire;
    sref->Release = HdfSRefRelease;
    sref->Count = HdfSRefCount;
}

