/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DEVICE_NODE_EXT_H
#define HDF_DEVICE_NODE_EXT_H

#include "hdf_io_service.h"
#include "hdf_device_node.h"

struct DeviceNodeExt {
    struct HdfDeviceNode super;
    struct HdfIoService *ioService;
};

struct HdfObject *DeviceNodeExtCreate(void);
void DeviceNodeExtRelease(struct HdfObject *object);

#endif /* HDF_DEVICE_NODE_EXT_H */

