/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_OBSERVER_RECORD_H
#define HDF_OBSERVER_RECORD_H

#include "hdf_device.h"
#include "hdf_object.h"
#include "hdf_slist.h"
#include "osal_mutex.h"

struct HdfServiceObserverRecord {
    struct HdfSListNode entry;
    uint32_t serviceKey;
    uint16_t policy;
    devid_t devId;
    struct OsalMutex obsRecMutex;
    struct HdfSList subscribers;
    struct HdfObject *publisher;
};

struct HdfServiceObserverRecord *HdfServiceObserverRecordObtain(uint32_t serviceKey);
void HdfServiceObserverRecordRecycle(struct HdfServiceObserverRecord *record);
bool HdfServiceObserverRecordCompare(struct HdfSListNode *listEntry, uint32_t serviceKey);
void HdfServiceObserverRecordNotifySubscribers(
    struct HdfServiceObserverRecord *record, devid_t devid, uint16_t policy);
void HdfServiceObserverRecordDelete(struct HdfSListNode *listEntry);

#endif /* HDF_OBSERVER_RECORD_H */
