/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DEVICE_TOKEN_H
#define HDF_DEVICE_TOKEN_H

#include "device_token_if.h"
#include "hdf_slist.h"

struct HdfDeviceToken {
    struct IHdfDeviceToken super;
    struct HdfSListNode node;
};

struct HdfObject *HdfDeviceTokenCreate(void);
void HdfDeviceTokenRelease(struct HdfObject *object);
struct IHdfDeviceToken *HdfDeviceTokenNewInstance(void);
void HdfDeviceTokenFreeInstance(struct IHdfDeviceToken *token);

#endif /* HDF_DEVICE_TOKEN_H */
