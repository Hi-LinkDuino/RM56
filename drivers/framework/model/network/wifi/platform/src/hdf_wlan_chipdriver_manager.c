/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_wlan_chipdriver_manager.h"
#include "gpio_if.h"
#include "osal_mem.h"
#include "hdf_log.h"
#include "wifi_module.h"
#include "securec.h"

#define HDF_LOG_TAG HDF_WIFI_CORE

static struct HdfChipDriverFactory *g_wlanChipDriverManagerFactory[MAX_CHIPDRIVER_COUNT] = {NULL};

/* chip driver manager method get chip driver factory by chip name */
struct HdfChipDriverFactory *HdfWlanGetChipDriverByName(const char *driverName)
{
    int32_t i;
    if (driverName == NULL) {
        HDF_LOGE("%s fail : driverName is NULL", __func__);
        return NULL;
    }

    for (i = 0; i < MAX_CHIPDRIVER_COUNT; i++) {
        if (g_wlanChipDriverManagerFactory[i] != NULL && g_wlanChipDriverManagerFactory[i]->driverName != NULL) {
            struct HdfChipDriverFactory *factory = g_wlanChipDriverManagerFactory[i];
            if (strcmp(factory->driverName, driverName) == 0) {
                return factory;
            }
        }
    }
    return NULL;
}

/* chip driver manager register */
int32_t HdfWlanRegChipDriver(struct HdfChipDriverFactory *obj)
{
    int32_t index;
    if (obj == NULL || obj->driverName == NULL) {
        HDF_LOGE("%s: HdfChipDriverFactory obj is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (HdfWlanGetChipDriverByName(obj->driverName) != NULL) {
        HDF_LOGW("%s: chipdriver factory is already registered.name=%s", __func__, obj->driverName);
        return HDF_SUCCESS;
    }
    for (index = 0; index < MAX_CHIPDRIVER_COUNT; index++) {
        if (g_wlanChipDriverManagerFactory[index] == NULL) {
            g_wlanChipDriverManagerFactory[index] = obj;
            HDF_LOGI("%s:Chip driver %s registered.", __func__, obj->driverName);
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s:Factory table is full", __func__);
    return HDF_FAILURE;
}

/* to release the chipdriver manager factory */
int32_t ChipDriverMgrDeInit(void)
{
    int cnt;
    for (cnt = 0; cnt < MAX_CHIPDRIVER_COUNT; cnt++) {
        if (g_wlanChipDriverManagerFactory[cnt] != NULL) {
            if (g_wlanChipDriverManagerFactory[cnt]->ReleaseFactory != NULL) {
                g_wlanChipDriverManagerFactory[cnt]->ReleaseFactory(g_wlanChipDriverManagerFactory[cnt]);
            }
            g_wlanChipDriverManagerFactory[cnt] = NULL;
        }
    }

    return HDF_SUCCESS;
}

static struct HdfChipDriverManager g_chipDriverManager = {
    .chipFactoryInsts = g_wlanChipDriverManagerFactory,
    .RegChipDriver = HdfWlanRegChipDriver,
    .GetChipDriverByName = HdfWlanGetChipDriverByName,
};
struct HdfChipDriverManager *HdfWlanGetChipDriverMgr(void)
{
    return &g_chipDriverManager;
}