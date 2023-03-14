/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DRIVER_H
#define HDF_DRIVER_H

#include "hdf_device_desc.h"
#include "hdf_dlist.h"

struct HdfDriver {
    const struct HdfDriverEntry *entry;
    uint16_t type;
    uint16_t bus;
    struct DListHead node;
    void *priv;
};

int32_t HdfRegisterDriverEntry(const struct HdfDriverEntry *entry);
int32_t HdfUnregisterDriverEntry(const struct HdfDriverEntry *entry);
int32_t HdfRegisterDriver(struct HdfDriver *driver);
int32_t HdfUnregisterDriver(struct HdfDriver *driver);
struct HdfDriver *HdfDriverManagerGetDriver(const char *driverName);
struct DListHead *HdfDriverManagerGetDriverList(void);

#endif // HDF_DRIVER_H