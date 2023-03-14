/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "devhost_service.h"
#include "devmgr_service.h"
#include "devsvc_manager.h"
#include "devsvc_manager_ext.h"
#include "hdf_device.h"
#include "hdf_device_node_ext.h"
#include "hdf_device_token.h"
#include "hdf_driver_installer.h"
#include "hdf_driver_loader.h"
#include "hdf_object_manager.h"

static const struct HdfObjectCreator g_liteObjectCreators[] = {
    [HDF_OBJECT_ID_DEVMGR_SERVICE] =
        {
            .Create = DevmgrServiceCreate,
            .Release = DevmgrServiceRelease,
        },
    [HDF_OBJECT_ID_DEVSVC_MANAGER] =
        {
            .Create = DevSvcManagerCreate,
            .Release = DevSvcManagerRelease,
        },
    [HDF_OBJECT_ID_DEVHOST_SERVICE] =
        {
            .Create = DevHostServiceCreate,
            .Release = DevHostServiceRelease,
        },
    [HDF_OBJECT_ID_DRIVER_INSTALLER] =
        {
            .Create = DriverInstallerCreate,
            .Release = NULL,
        },
    [HDF_OBJECT_ID_DRIVER_LOADER] =
        {
            .Create = HdfDriverLoaderCreate,
            .Release = NULL,
        },
    [HDF_OBJECT_ID_DEVICE] =
        {
            .Create = HdfDeviceCreate,
            .Release = HdfDeviceRelease,
        },
    [HDF_OBJECT_ID_DEVICE_TOKEN] =
        {
            .Create = HdfDeviceTokenCreate,
            .Release = HdfDeviceTokenRelease,
        },
    [HDF_OBJECT_ID_DEVICE_SERVICE] =
        {
            .Create = DeviceNodeExtCreate,
            .Release = DeviceNodeExtRelease,
        }
};

const struct HdfObjectCreator *HdfObjectManagerGetCreators(int objectId)
{
    int numConfigs = sizeof(g_liteObjectCreators) / sizeof(g_liteObjectCreators[0]);
    if ((objectId >= 0) && (objectId < numConfigs)) {
        return &g_liteObjectCreators[objectId];
    }
    return NULL;
}
