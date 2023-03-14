/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_OBJECT_MANAGER_H
#define HDF_OBJECT_MANAGER_H

#include "hdf_object.h"

enum {
    HDF_OBJECT_ID_DEVMGR_SERVICE = 0,
    HDF_OBJECT_ID_DEVSVC_MANAGER,
    HDF_OBJECT_ID_DEVHOST_SERVICE,
    HDF_OBJECT_ID_DRIVER_INSTALLER,
    HDF_OBJECT_ID_DRIVER_LOADER,
    HDF_OBJECT_ID_DEVICE,
    HDF_OBJECT_ID_DEVICE_TOKEN,
    HDF_OBJECT_ID_DEVICE_SERVICE,
    HDF_OBJECT_ID_REMOTE_SERVICE,
    HDF_OBJECT_ID_MAX
};

struct HdfObjectCreator {
    struct HdfObject *(*Create)(void);
    void (*Release)(struct HdfObject *);
};

const struct HdfObjectCreator *HdfObjectManagerGetCreators(int objectId);
struct HdfObject *HdfObjectManagerGetObject(int ObjectId);
void HdfObjectManagerFreeObject(struct HdfObject *object);

#endif /* HDF_OBJECT_MANAGER_H */
