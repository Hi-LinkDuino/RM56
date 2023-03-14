/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DRIVER_INSTALLER_H
#define HDF_DRIVER_INSTALLER_H

#include "hdf_object.h"
#include "hdf_slist.h"

struct IDriverInstaller {
    struct HdfObject object;
    int (*StartDeviceHost)(uint32_t, const char *, bool);
    int (*StopDeviceHost)(uint32_t, const char *);
};

struct DriverInstaller {
    struct IDriverInstaller super;
};

struct HdfObject *DriverInstallerCreate(void);
struct IDriverInstaller *DriverInstallerGetInstance(void);

#endif /* HDF_DRIVER_INSTALLER_H */
