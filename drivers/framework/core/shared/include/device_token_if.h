/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVICE_TOKEN_IF_H
#define DEVICE_TOKEN_IF_H

#include "hdf_object.h"
#include "hdf_device.h"

struct IHdfDeviceToken {
    struct HdfObject object;
    devid_t devid;
};

#endif /* DEVICE_TOKEN_IF_H */
