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

#include "driver_loader_full.h"
#include <dlfcn.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include "hdf_device.h"
#include "dev_attribute_serialize.h"
#include "hdf_device_node.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"
#include "osal_mem.h"
#include "securec.h"

#define DRIVER_DESC "driverDesc"
#define HDF_LOG_TAG driver_loader_full
#ifdef __ARM64__
#define DRIVER_PATH HDF_LIBRARY_DIR"64/"
#else
#define DRIVER_PATH HDF_LIBRARY_DIR"/"
#endif

static struct DriverLoaderFull *g_fullLoader = NULL;

struct HdfDriver *HdfDriverLoaderGetDriver(const char *moduleName)
{
    char realPath[PATH_MAX] = { 0 };
    char driverPath[PATH_MAX] = { 0 };
    if (moduleName == NULL) {
        return NULL;
    }

    if (strcat_s(driverPath, sizeof(driverPath) - 1, DRIVER_PATH) != EOK) {
        return NULL;
    }

    if (strcat_s(driverPath, (sizeof(driverPath) - 1 - sizeof(DRIVER_PATH)), moduleName) != EOK) {
        return NULL;
    }

    if (realpath(driverPath, realPath) == NULL) {
        HDF_LOGE("%{public}s no valid, errno:%{public}d", driverPath, errno);
        return NULL;
    }

    struct HdfDriver *driver = OsalMemAlloc(sizeof(struct HdfDriver));
    if (driver == NULL) {
        return NULL;
    }

    void *handle = dlopen(realPath, RTLD_LAZY);
    if (handle == NULL) {
        HDF_LOGE("get driver entry failed, %{public}s load fail, %{public}s", realPath, dlerror());
        OsalMemFree(driver);
        return NULL;
    }

    struct HdfDriverEntry **driverEntry = (struct HdfDriverEntry **)dlsym(handle, DRIVER_DESC);
    if (driverEntry == NULL) {
        HDF_LOGE("driver entry %{public}s dlsym failed", realPath);
        dlclose(handle);
        OsalMemFree(driver);
        return NULL;
    }

    driver->entry = *driverEntry;
    driver->priv = handle;

    return driver;
}

void HdfDriverLoaderFullReclaimDriver(struct HdfDriver *driver)
{
    if (driver == NULL) {
        return;
    }

    if (driver->priv != NULL) {
        dlclose(driver->priv);
        driver->priv = NULL;
    }

    OsalMemFree(driver);
}


void HdfDriverLoaderFullConstruct(struct DriverLoaderFull *inst)
{
    struct HdfDriverLoader *pvtbl = (struct HdfDriverLoader *)inst;
    pvtbl->super.GetDriver = HdfDriverLoaderGetDriver;
    pvtbl->super.ReclaimDriver = HdfDriverLoaderFullReclaimDriver;
}

struct HdfObject *HdfDriverLoaderFullCreate()
{
    if (g_fullLoader == NULL) {
        struct DriverLoaderFull *instance =
            (struct DriverLoaderFull *)OsalMemCalloc(sizeof(struct DriverLoaderFull));
        if (instance != NULL) {
            HdfDriverLoaderFullConstruct(instance);
            g_fullLoader = instance;
        }
    }
    return (struct HdfObject *)g_fullLoader;
}

void HdfDriverLoaderFullRelease(struct HdfObject *object)
{
    struct DriverLoaderFull *instance = (struct DriverLoaderFull *)object;
    if (instance == g_fullLoader) {
        g_fullLoader = NULL;
    }
    if (instance != NULL) {
        OsalMemFree(instance);
    }
}

struct IDriverLoader *HdfDriverLoaderGetInstance(void)
{
    static struct IDriverLoader *instance = NULL;
    if (instance == NULL) {
        instance = (struct IDriverLoader *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DRIVER_LOADER);
    }
    return instance;
}
