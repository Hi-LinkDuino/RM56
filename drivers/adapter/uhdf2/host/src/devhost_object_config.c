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

#include "devhost_service_stub.h"
#include "device_token_stub.h"
#include "devmgr_service_proxy.h"
#include "devsvc_manager_proxy.h"
#include "driver_loader_full.h"
#include "device_service_stub.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"

#define HDF_LOG_TAG devhost_object_config

static const struct HdfObjectCreator g_fullDevHostObjectCreators[] = {
    [HDF_OBJECT_ID_DEVMGR_SERVICE] =
        {
            .Create = DevmgrServiceProxyCreate,
            .Release = DevmgrServiceProxyRelease,
        },
    [HDF_OBJECT_ID_DEVSVC_MANAGER] =
        {
            .Create = DevSvcManagerProxyCreate,
            .Release = DevSvcManagerProxyRelease,
        },
    [HDF_OBJECT_ID_DEVHOST_SERVICE] =
        {
            .Create = DevHostServiceStubCreate,
            .Release = DevHostServiceStubRelease,
        },
    [HDF_OBJECT_ID_DRIVER_LOADER] =
        {
            .Create = HdfDriverLoaderFullCreate,
            .Release = HdfDriverLoaderFullRelease,
        },
    [HDF_OBJECT_ID_DEVICE] =
        {
            .Create = HdfDeviceCreate,
            .Release = HdfDeviceRelease,
        },
    [HDF_OBJECT_ID_DEVICE_TOKEN] =
        {
            .Create = DeviceTokenStubCreate,
            .Release = DeviceTokenStubRelease,
        },
    [HDF_OBJECT_ID_DEVICE_SERVICE] =
        {
            .Create = DeviceServiceStubCreate,
            .Release = DeviceServiceStubRelease,
        }
};

const struct HdfObjectCreator *HdfObjectManagerGetCreators(int objectId)
{
    int numConfigs = sizeof(g_fullDevHostObjectCreators) / sizeof(g_fullDevHostObjectCreators[0]);
    if (objectId < 0 || objectId >= numConfigs) {
        HDF_LOGE("Invalid objectId: %{public}d", objectId);
        return NULL;
    }
    return &g_fullDevHostObjectCreators[objectId];
}

