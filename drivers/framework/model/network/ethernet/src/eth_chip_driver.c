/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "eth_chip_driver.h"
#include "hdf_log.h"

#define HDF_LOG_TAG HDF_ETH_CORE

static struct HdfEthChipDriverFactory *g_ethChipDriverFactory[MAX_CHIPDRIVER_COUNT] = {NULL};

struct HdfEthChipDriverFactory *HdfEthGetChipDriverByName(const char *driverName)
{
    int32_t i;
    if (driverName == NULL) {
        HDF_LOGE("%s fail: drivreName is NULL", __func__);
        return NULL;
    }

    for (i = 0; i < MAX_CHIPDRIVER_COUNT; i++) {
        if (g_ethChipDriverFactory[i] != NULL && g_ethChipDriverFactory[i]->driverName != NULL) {
            struct HdfEthChipDriverFactory *factory = g_ethChipDriverFactory[i];
            if (strcmp(factory->driverName, driverName) == 0) {
                return factory;
            }
        }
    }
    return NULL;
}

int32_t HdfEthRegChipDriver(struct HdfEthChipDriverFactory *obj)
{
    int32_t index;
    if (obj == NULL || obj->driverName == NULL) {
        HDF_LOGE("%s: HdfEthChipDriverFactory obj is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (HdfEthGetChipDriverByName(obj->driverName) != NULL) {
        HDF_LOGW("%s: chipDriver factory is already registered. name = %s", __func__, obj->driverName);
        return HDF_FAILURE;
    }
    for (index = 0; index < MAX_CHIPDRIVER_COUNT; index++) {
        if (g_ethChipDriverFactory[index] == NULL) {
            g_ethChipDriverFactory[index] = obj;
            HDF_LOGI("%s:Chip driver %s registered.", __func__, obj->driverName);
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: Factory table is NULL", __func__);
    return HDF_FAILURE;
}

static struct HdfEthChipDriverManager g_chipDriverManager = {
    .chipFactoryInsts = g_ethChipDriverFactory,
    .RegChipDriver = HdfEthRegChipDriver,
    .GetEthChipDriverByName = HdfEthGetChipDriverByName,
};

struct HdfEthChipDriverManager *HdfEthGetChipDriverMgr(void)
{
    return &g_chipDriverManager;
}