/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_device_token.h"
#include "hdf_object_manager.h"
#include "osal_mem.h"

static void HdfDeviceTokenConstruct(struct HdfDeviceToken *inst)
{
    inst->super.object.objectId = HDF_OBJECT_ID_DEVICE_TOKEN;
}

struct HdfObject *HdfDeviceTokenCreate()
{
    struct HdfDeviceToken *token =
        (struct HdfDeviceToken *)OsalMemCalloc(sizeof(struct HdfDeviceToken));
    if (token != NULL) {
        HdfDeviceTokenConstruct(token);
    }
    return (struct HdfObject *)token;
}

void HdfDeviceTokenRelease(struct HdfObject *object)
{
    struct HdfDeviceToken *deviceToken = (struct HdfDeviceToken *)object;
    if (deviceToken != NULL) {
        OsalMemFree(deviceToken);
    }
}

struct IHdfDeviceToken *HdfDeviceTokenNewInstance()
{
    return (struct IHdfDeviceToken *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DEVICE_TOKEN);
}

void HdfDeviceTokenFreeInstance(struct IHdfDeviceToken *token)
{
    if (token != NULL) {
        HdfObjectManagerFreeObject(&token->object);
    }
}

