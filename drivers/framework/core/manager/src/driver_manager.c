/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_dlist.h"
#include "hdf_driver.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_sysevent.h"

static struct DListHead *HdfDriverHead(void)
{
    static struct DListHead driverHead = {0};
    if (driverHead.next == NULL) {
        DListHeadInit(&driverHead);
    }

    return &driverHead;
}

int32_t HdfRegisterDriverEntry(const struct HdfDriverEntry *entry)
{
    struct HdfDriver *newDriver = NULL;

    if (entry == NULL || entry->moduleName == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    newDriver = OsalMemCalloc(sizeof(struct HdfDriver));
    if (newDriver == NULL) {
        return HDF_DEV_ERR_NO_MEMORY;
    }

    newDriver->entry = entry;

    DListInsertTail(&newDriver->node, HdfDriverHead());

    return HDF_SUCCESS;
}

int32_t HdfUnregisterDriverEntry(const struct HdfDriverEntry *entry)
{
    struct DListHead *driverHead = NULL;
    struct HdfDriver *driver = NULL;
    struct HdfDriver *tmp = NULL;

    if (entry == NULL || entry->moduleName == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    driverHead = HdfDriverHead();
    DLIST_FOR_EACH_ENTRY_SAFE(driver, tmp, driverHead, struct HdfDriver, node) {
        if (driver->entry == entry) {
            DListRemove(&driver->node);
            OsalMemFree(driver);
            break;
        }
    }

    return HDF_SUCCESS;
}

int32_t HdfRegisterDriver(struct HdfDriver *driver)
{
    if (driver == NULL || driver->entry == NULL || driver->entry->moduleName == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    DListInsertTail(&driver->node, HdfDriverHead());
    return HDF_SUCCESS;
}

int32_t HdfUnregisterDriver(struct HdfDriver *driver)
{
    struct DListHead *driverHead = NULL;
    struct HdfDriver *it = NULL;
    struct HdfDriver *tmp = NULL;

    if (driver == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    driverHead = HdfDriverHead();
    DLIST_FOR_EACH_ENTRY_SAFE(it, tmp, driverHead, struct HdfDriver, node) {
        if (it == driver) {
            DListRemove(&it->node);
            break;
        }
    }

    return HDF_SUCCESS;
}

static struct HdfDriver *HdfDriverManagerFoundDriver(const char *driverName)
{
    struct DListHead *driverHead = NULL;
    struct HdfDriver *driver = NULL;
    driverHead = HdfDriverHead();
    DLIST_FOR_EACH_ENTRY(driver, driverHead, struct HdfDriver, node) {
        if (driver->entry != NULL && driver->entry->moduleName != NULL &&
            !strcmp(driver->entry->moduleName, driverName)) {
            return driver;
        }
    }

    return NULL;
}

struct HdfDriver *HdfDriverManagerGetDriver(const char *driverName)
{
    struct HdfDriver *driver = NULL;
    if (driverName == NULL) {
        return NULL;
    }
    driver = HdfDriverManagerFoundDriver(driverName);
    if (driver != NULL) {
        return driver;
    }

    if (HdfSysEventSend != NULL) {
        HDF_LOGI("%s:try to dynamic load driver %s", __func__, driverName);
        if (HdfSysEventSend(HDF_SYSEVENT_CLASS_MODULE, KEVENT_MODULE_INSTALL, driverName, true) != HDF_SUCCESS) {
            return NULL;
        }

        driver = HdfDriverManagerFoundDriver(driverName);
    }
    if (driver == NULL) {
        HDF_LOGE("%s:driver %s not found", __func__, driverName);
    }
    return driver;
}

struct DListHead *HdfDriverManagerGetDriverList(void)
{
    return HdfDriverHead();
}
