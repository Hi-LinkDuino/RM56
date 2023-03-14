/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_token_clnt.h"
#include "hdf_device_token.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG device_token_clnt

static void DeviceTokenClntConstruct(struct DeviceTokenClnt *tokenClnt, struct IHdfDeviceToken *tokenIf)
{
    tokenClnt->tokenIf = tokenIf;
    tokenClnt->deviceInfo = NULL;
}

struct DeviceTokenClnt *DeviceTokenClntNewInstance(struct IHdfDeviceToken *tokenIf)
{
    struct DeviceTokenClnt *tokenClnt = NULL;
    if (tokenIf == NULL) {
        HDF_LOGE("failed to create token client, tokenIf is null");
        return NULL;
    }
    tokenClnt = (struct DeviceTokenClnt *)OsalMemCalloc(sizeof(struct DeviceTokenClnt));
    if (tokenClnt != NULL) {
        DeviceTokenClntConstruct(tokenClnt, tokenIf);
    }
    return tokenClnt;
}

void DeviceTokenClntFreeInstance(struct DeviceTokenClnt *tokenClnt)
{
    if (tokenClnt != NULL) {
        OsalMemFree(tokenClnt);
    }
}

void DeviceTokenClntDelete(struct HdfSListNode *listEntry)
{
    struct DeviceTokenClnt *tokenClnt = (struct DeviceTokenClnt *)listEntry;
    if (tokenClnt != NULL) {
        DeviceTokenClntFreeInstance(tokenClnt);
    }
}
