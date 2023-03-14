/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DRIVER_LOADER_H
#define HDF_DRIVER_LOADER_H

#include "hdf_device_info.h"
#include "hdf_device_node.h"
#include "hdf_object.h"

struct IDriverLoader {
    struct HdfObject object;
    struct HdfDriver *(*GetDriver)(const char *driverName);
    void (*ReclaimDriver)(struct HdfDriver *driver);
};

struct HdfDriverLoader {
    struct IDriverLoader super;
};

struct HdfObject *HdfDriverLoaderCreate(void);
void HdfDriverLoaderConstruct(struct HdfDriverLoader *inst);
void HdfDriverLoaderRelease(struct HdfObject *object);
struct IDriverLoader *HdfDriverLoaderGetInstance(void);
struct HdfDriver *HdfDriverLoaderGetDriver(const char *moduleName);
struct HdfDeviceNode *HdfDriverLoaderLoadNode(
    struct IDriverLoader *loader, const struct HdfDeviceInfo *deviceInfo);
void HdfDriverLoaderUnLoadNode(struct IDriverLoader *loader, const struct HdfDeviceInfo *deviceInfo);

#endif /* HDF_DRIVER_LOADER_H */
