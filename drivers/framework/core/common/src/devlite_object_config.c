/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
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
            .Create = DevSvcManagerExtCreate,
            .Release = DevSvcManagerExtRelease,
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
