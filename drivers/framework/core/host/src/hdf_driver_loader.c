/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_driver_loader.h"
#include "hdf_driver.h"
#include "hdf_log.h"
#include "hdf_object_manager.h"

#define HDF_LOG_TAG driver_loader

int32_t HdfDriverEntryConstruct()
{
    int i;
    struct HdfDriverEntry *driverEntry = NULL;
    size_t *addrBegin = NULL;
    int32_t count = (int32_t)(((uint8_t *)(HDF_DRIVER_END()) - (uint8_t *)(HDF_DRIVER_BEGIN())) / sizeof(size_t));
    if (count <= 0) {
        HDF_LOGE("%s: no hdf driver exist", __func__);
        return HDF_FAILURE;
    }

    addrBegin = (size_t *)(HDF_DRIVER_BEGIN());
    for (i = 0; i < count; i++) {
        driverEntry = (struct HdfDriverEntry *)(*addrBegin);
        if (HdfRegisterDriverEntry(driverEntry) != HDF_SUCCESS) {
            HDF_LOGE("failed to register driver %s, skip and try another", driverEntry ? driverEntry->moduleName : "");
            continue;
        }
        addrBegin++;
    }
    return HDF_SUCCESS;
}

struct HdfDriver *HdfDriverLoaderGetDriver(const char *moduleName)
{
    if (moduleName == NULL) {
        HDF_LOGE("%s: failed to get device entry, moduleName is NULL", __func__);
        return NULL;
    }

    return HdfDriverManagerGetDriver(moduleName);
}

void HdfDriverLoaderReclaimDriver(struct HdfDriver *driver)
{
    // kernel driver do not need release
    (void)driver;
}

void HdfDriverLoaderConstruct(struct HdfDriverLoader *inst)
{
    if (inst != NULL) {
        inst->super.GetDriver = HdfDriverLoaderGetDriver;
        inst->super.ReclaimDriver = HdfDriverLoaderReclaimDriver;
    }
}

struct HdfObject *HdfDriverLoaderCreate(void)
{
    static bool isDriverLoaderInit = false;
    static struct HdfDriverLoader driverLoader;
    if (!isDriverLoaderInit) {
        if (HdfDriverEntryConstruct() != HDF_SUCCESS) {
            return NULL;
        }
        HdfDriverLoaderConstruct(&driverLoader);
        isDriverLoaderInit = true;
    }
    return (struct HdfObject *)&driverLoader;
}

struct IDriverLoader *HdfDriverLoaderGetInstance(void)
{
    static struct IDriverLoader *instance = NULL;
    if (instance == NULL) {
        instance = (struct IDriverLoader *)HdfObjectManagerGetObject(HDF_OBJECT_ID_DRIVER_LOADER);
    }
    return instance;
}
