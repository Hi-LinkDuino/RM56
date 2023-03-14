/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SERVICE_SUBSCRIBER_H
#define HDF_SERVICE_SUBSCRIBER_H

#include "hdf_device.h"
#include "hdf_device_desc.h"
#include "hdf_slist.h"

struct HdfServiceSubscriber {
    struct HdfSListNode entry;
    uint32_t state;
    uint32_t devId;
    struct SubscriberCallback callback;
};

enum {
    HDF_SUBSCRIBER_STATE_PENDING,
    HDF_SUBSCRIBER_STATE_READY
};

struct HdfServiceSubscriber *HdfServiceSubscriberObtain(struct SubscriberCallback callback, devid_t devid);
void HdfServiceSubscriberRecycle(struct HdfServiceSubscriber *subscriber);
void HdfServiceSubscriberDelete(struct HdfSListNode *listEntry);

#endif /* HDF_SERVICE_SUBSCRIBER_H */
