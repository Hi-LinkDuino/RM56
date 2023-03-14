/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVMGR_SERVICE_IF_H
#define DEVMGR_SERVICE_IF_H

#include "devhost_service_if.h"
#include "device_token_if.h"
#include "hdf_object.h"
#include "power_state_token_if.h"
#include "hdf_power_state.h"

struct IDevmgrService {
    struct HdfObject base;
    struct HdfDeviceObject object;
    int (*AttachDeviceHost)(struct IDevmgrService *, uint16_t, struct IDevHostService *);
    int (*AttachDevice)(struct IDevmgrService *, struct IHdfDeviceToken *);
    int (*DetachDevice)(struct IDevmgrService *, devid_t);
    int (*LoadDevice)(struct IDevmgrService *, const char *);
    int (*UnloadDevice)(struct IDevmgrService *, const char *);
    int (*StartService)(struct IDevmgrService *);
    int (*PowerStateChange)(struct IDevmgrService *, enum HdfPowerState pEvent);
};

#endif /* DEVMGR_SERVICE_IF_H */
