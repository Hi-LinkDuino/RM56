/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_observer_record.h"
#include "hdf_log.h"
#include "hdf_service_subscriber.h"
#include "osal_mem.h"

#define HDF_LOG_TAG observer_record
#define HALF_INT_LEN 16

struct HdfServiceObserverRecord *HdfServiceObserverRecordObtain(uint32_t serviceKey)
{
    struct HdfServiceObserverRecord *observerRecord =
        (struct HdfServiceObserverRecord *)OsalMemCalloc(sizeof(struct HdfServiceObserverRecord));
    if (observerRecord != NULL) {
        observerRecord->serviceKey = serviceKey;
        observerRecord->publisher = NULL;
        if (OsalMutexInit(&observerRecord->obsRecMutex) != HDF_SUCCESS) {
            OsalMemFree(observerRecord);
            return NULL;
        }
        HdfSListInit(&observerRecord->subscribers);
    }
    return observerRecord;
}

void HdfServiceObserverRecordRecycle(struct HdfServiceObserverRecord *observerRecord)
{
    if (observerRecord != NULL) {
        HdfSListFlush(&observerRecord->subscribers, HdfServiceSubscriberDelete);
        OsalMutexDestroy(&observerRecord->obsRecMutex);
        observerRecord->obsRecMutex.realMutex = NULL;
        OsalMemFree(observerRecord);
    }
}

bool HdfServiceObserverRecordCompare(struct HdfSListNode *listEntry, uint32_t serviceKey)
{
    struct HdfServiceObserverRecord *record = NULL;
    if (listEntry == NULL) {
        return false;
    }
    record = (struct HdfServiceObserverRecord *)listEntry;
    if (record->serviceKey == serviceKey) {
        return true;
    }
    return false;
}

void HdfServiceObserverRecordNotifySubscribers(
    struct HdfServiceObserverRecord *record, uint32_t devId, uint16_t policy)
{
    struct HdfSListIterator it;
    if (record == NULL) {
        HDF_LOGE("%s: record is null", __func__);
        return;
    }

    OsalMutexLock(&record->obsRecMutex);
    HdfSListIteratorInit(&it, &record->subscribers);
    while (HdfSListIteratorHasNext(&it)) {
        struct HdfServiceSubscriber *subscriber =
            (struct HdfServiceSubscriber *)HdfSListIteratorNext(&it);
        if (devId == subscriber->devId || policy != SERVICE_POLICY_PRIVATE) {
            subscriber->state = HDF_SUBSCRIBER_STATE_READY;
            if (subscriber->callback.OnServiceConnected != NULL) {
                subscriber->callback.OnServiceConnected(subscriber->callback.deviceObject, record->publisher);
            }
        }
    }
    OsalMutexUnlock(&record->obsRecMutex);
}

void HdfServiceObserverRecordDelete(struct HdfSListNode *listEntry)
{
    struct HdfServiceObserverRecord *observerRecord = (struct HdfServiceObserverRecord *)listEntry;
    if (observerRecord != NULL) {
        HdfServiceObserverRecordRecycle(observerRecord);
    }
}

