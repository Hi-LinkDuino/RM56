/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVHOST_SERVICE_IF_H
#define DEVHOST_SERVICE_IF_H

#include "hdf_device.h"
#include "hdf_device_info.h"
#include "hdf_object.h"

struct IDevHostService {
    struct HdfObject object;
    int (*AddDevice)(struct IDevHostService *hostService, const struct HdfDeviceInfo *devInfo);
    int (*DelDevice)(struct IDevHostService *hostService, devid_t devId);
    int (*StartService)(struct IDevHostService *hostService);
    int (*PmNotify)(struct IDevHostService *service, uint32_t powerState);
};

#endif /* DEVHOST_SERVICE_IF_H */
