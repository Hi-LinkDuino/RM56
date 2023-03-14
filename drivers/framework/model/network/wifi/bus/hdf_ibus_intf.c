/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_sdio_intf.h"
#include "osal_mem.h"
#include "securec.h"
#include "wifi_inc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct BusDev *HdfWlanCreateBusManager(const struct HdfConfigWlanBus *busConfig)
{
    struct BusDev *bus = NULL;
    if (busConfig == NULL) {
        return NULL;
    }
    bus = (struct BusDev *)OsalMemCalloc(sizeof(struct BusDev));
    if (bus == NULL) {
        return NULL;
    }
    switch (busConfig->busType) {
        case BUS_SDIO:
            if (HdfSdioBusInit(bus, busConfig) != HDF_SUCCESS) {
                OsalMemFree(bus);
                return NULL;
            }
            break;
        default:
            HDF_LOGE("%s:bus type not support!", __func__);
            OsalMemFree(bus);
            return NULL;
    }
    return bus;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
