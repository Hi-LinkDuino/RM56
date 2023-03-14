/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVSVC_MANAGER_IF_H
#define DEVSVC_MANAGER_IF_H

#include "devsvc_listener_holder.h"
#include "hdf_device_desc.h"
#include "hdf_object.h"

struct IDevSvcManager {
    struct HdfObject object;
    int (*StartService)(struct IDevSvcManager *);
    int (*AddService)(struct IDevSvcManager *, const char *, uint16_t, struct HdfDeviceObject *, const char *);
    int (*UpdateService)(struct IDevSvcManager *, const char *, uint16_t, struct HdfDeviceObject *, const char *);
    int (*SubscribeService)(struct IDevSvcManager *, const char *, struct SubscriberCallback);
    int (*UnsubscribeService)(struct IDevSvcManager *, const char *);
    struct HdfObject *(*GetService)(struct IDevSvcManager *, const char *);
    struct HdfDeviceObject *(*GetObject)(struct IDevSvcManager *, const char *);
    void (*RemoveService)(struct IDevSvcManager *, const char *);
    int (*RegsterServListener)(struct IDevSvcManager *, struct ServStatListenerHolder *);
    void (*UnregsterServListener)(struct IDevSvcManager *, struct ServStatListenerHolder *);
};

#endif /* DEVSVC_MANAGER_IF_H */
