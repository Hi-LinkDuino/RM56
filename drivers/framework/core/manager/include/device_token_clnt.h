/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVICE_TOKEN_CLNT_H
#define DEVICE_TOKEN_CLNT_H

#include "device_token_if.h"
#include "hdf_device_info.h"
#include "hdf_slist.h"

struct DeviceTokenClnt {
    struct HdfSListNode node;
    struct IHdfDeviceToken *tokenIf;
    const struct HdfDeviceInfo *deviceInfo;
};

struct DeviceTokenClnt *DeviceTokenClntNewInstance(struct IHdfDeviceToken *token);
void DeviceTokenClntFreeInstance(struct DeviceTokenClnt *token);
void DeviceTokenClntDelete(struct HdfSListNode *listEntry);

#endif /* DEVICE_TOKEN_CLNT_H */
