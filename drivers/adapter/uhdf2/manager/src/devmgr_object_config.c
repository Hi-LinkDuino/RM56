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

#include "hdf_object_manager.h"
#include "devmgr_service_stub.h"
#include "devsvc_manager_stub.h"
#include "driver_installer_full.h"
#include "hdf_log.h"

#define HDF_LOG_TAG devmgr_object_config

const struct HdfObjectCreator g_fullDevMgrObjectCreators[] = {
    [HDF_OBJECT_ID_DEVMGR_SERVICE] = {.Create = DevmgrServiceStubCreate},
    [HDF_OBJECT_ID_DEVSVC_MANAGER] = {.Create = DevSvcManagerStubCreate},
    [HDF_OBJECT_ID_DRIVER_INSTALLER] = {.Create = DriverInstallerFullCreate},
};

const struct HdfObjectCreator* HdfObjectManagerGetCreators(int objectId)
{
    int numConfigs = sizeof(g_fullDevMgrObjectCreators) / sizeof(g_fullDevMgrObjectCreators[0]);
    if (objectId < 0 || objectId >= numConfigs) {
        HDF_LOGE("Invalid objectId %{public}d", objectId);
        return NULL;
    }
    return &g_fullDevMgrObjectCreators[objectId];
}
