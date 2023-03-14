/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "osal_mem.h"
#include "device_token_stub.h"
#include "hdf_base.h"
#include "hdf_log.h"

#define HDF_LOG_TAG device_token_stub

int DeviceTokenStubDispatch(
    struct HdfObject *stub, int code, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)code;
    (void)data;
    (void)reply;
    struct DeviceTokenStub *tokenStub = (struct DeviceTokenStub *)stub;
    struct IDevHostService *super = (struct IDevHostService *)&tokenStub->super;
    if (super == NULL) {
        HDF_LOGE("Dispatch failed, super is null");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void DeviceTokenStubConstruct(struct DeviceTokenStub *inst)
{
    inst->remote = NULL;
}

struct HdfObject *DeviceTokenStubCreate()
{
    struct DeviceTokenStub *instance =
        (struct DeviceTokenStub *)OsalMemCalloc(sizeof(struct DeviceTokenStub));
    if (instance != NULL) {
        DeviceTokenStubConstruct(instance);
    }
    return (struct HdfObject *)instance;
}

void DeviceTokenStubRelease(struct HdfObject *object)
{
    struct DeviceTokenStub *instance = (struct DeviceTokenStub *)object;
    if (instance != NULL) {
        if (instance->remote != NULL) {
            HdfRemoteServiceRecycle(instance->remote);
            instance->remote = NULL;
        }
        OsalMemFree(instance);
    }
}

