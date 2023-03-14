/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mtd_char.h"

__attribute__((weak)) int32_t MtdCharOsInit(struct MtdDevice *mtdDevice)
{
    (void)mtdDevice;
    return HDF_SUCCESS;
}

__attribute__ ((weak)) void MtdCharOsUninit(struct MtdDevice *mtdDevice)
{
    (void)mtdDevice;
}

int32_t MtdCharInit(struct MtdDevice *mtdDevice)
{
    return MtdCharOsInit(mtdDevice);
}

void MtdCharUninit(struct MtdDevice *mtdDevice)
{
    MtdCharOsUninit(mtdDevice);
}
