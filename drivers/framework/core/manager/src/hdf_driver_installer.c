/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_driver_installer.h"
#include "devhost_service.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"

#define HDF_LOG_TAG driver_installer

static int DriverInstallerStartDeviceHost(uint32_t devHostId, const char *devHostName, bool dynamic)
{
    struct IDevHostService *hostServiceIf = DevHostServiceNewInstance(devHostId, devHostName);
    int ret;
    (void)dynamic;
    if ((hostServiceIf == NULL) || (hostServiceIf->StartService == NULL)) {
        HDF_LOGE("hostServiceIf or hostServiceIf->StartService is null");
        return HDF_FAILURE;
    }
    ret = hostServiceIf->StartService(hostServiceIf);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("failed to start host service, ret: %d", ret);
        DevHostServiceFreeInstance(hostServiceIf);
    }
    return ret;
}

static void DriverInstallerConstruct(struct DriverInstaller *inst)
{
    struct IDriverInstaller *driverInstallIf = (struct IDriverInstaller *)inst;
    driverInstallIf->StartDeviceHost = DriverInstallerStartDeviceHost;
}

struct HdfObject *DriverInstallerCreate(void)
{
    static bool isDriverInstInit = false;
    static struct DriverInstaller driverInstaller;
    if (!isDriverInstInit) {
        DriverInstallerConstruct(&driverInstaller);
        isDriverInstInit = true;
    }
    return (struct HdfObject *)&driverInstaller;
}

struct IDriverInstaller *DriverInstallerGetInstance()
{
    static struct IDriverInstaller *installer = NULL;
    if (installer == NULL) {
        installer = (struct IDriverInstaller *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DRIVER_INSTALLER);
    }
    return installer;
}

