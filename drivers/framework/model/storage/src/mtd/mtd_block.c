/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "mtd_core.h"

__attribute__((weak)) int32_t MtdBlockOsInit(struct MtdDevice *mtdDevice)
{
    (void)mtdDevice;
    return HDF_SUCCESS;
}

__attribute__ ((weak)) void MtdBlockOsUninit(struct MtdDevice *mtdDevice)
{
    (void)mtdDevice;
}

int32_t MtdBlockInit(struct MtdDevice *mtdDevice)
{
    int32_t ret;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (MtdDeviceGet(mtdDevice) == NULL) {
        HDF_LOGE("%s: get mtd device failed", __func__);
        return HDF_PLT_ERR_DEV_GET;
    }

    ret = MtdBlockOsInit(mtdDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: os init failed, ret = %d", __func__, ret);
        MtdDevicePut(mtdDevice);
        return ret;
    }

    return HDF_SUCCESS;
}

void MtdBlockUninit(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL) {
        MtdBlockOsUninit(mtdDevice);
        MtdDevicePut(mtdDevice);
    }
}
