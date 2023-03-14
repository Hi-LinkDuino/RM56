/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_service_subscriber.h"
#include "hdf_device.h"
#include "osal_mem.h"

struct HdfServiceSubscriber *HdfServiceSubscriberObtain(struct SubscriberCallback callback, devid_t devid)
{
    struct HdfServiceSubscriber *serviceSubscriber =
        (struct HdfServiceSubscriber *)OsalMemCalloc(sizeof(struct HdfServiceSubscriber));
    if (serviceSubscriber != NULL) {
        serviceSubscriber->state = HDF_SUBSCRIBER_STATE_PENDING;
        serviceSubscriber->devId = devid;
        serviceSubscriber->callback = callback;
    }
    return serviceSubscriber;
}

void HdfServiceSubscriberRecycle(struct HdfServiceSubscriber *subscriber)
{
    if (subscriber != NULL) {
        OsalMemFree(subscriber);
    }
}

void HdfServiceSubscriberDelete(struct HdfSListNode *listEntry)
{
    struct HdfServiceSubscriber *serviceSubscriber = (struct HdfServiceSubscriber *)listEntry;
    if (serviceSubscriber != NULL) {
        HdfServiceSubscriberRecycle(serviceSubscriber);
    }
}
