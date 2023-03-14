/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MMC_EMMC_H
#define MMC_EMMC_H

#include "emmc_if.h"
#include "mmc_corex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct EmmcRegister {
    struct EmmcExtCsd extCsd;
};

struct EmmcDevice {
    struct MmcDevice mmc;
    struct EmmcDeviceOps *emmcOps;
    struct EmmcRegister emmcReg;
};

struct EmmcDeviceOps {
    int32_t (*getCid)(struct EmmcDevice *, uint8_t *, uint32_t);
};

int32_t EmmcDeviceGetCid(struct EmmcDevice *dev, uint8_t *cid, uint32_t len);
void EmmcDeviceAddOps(struct EmmcDevice *dev, struct EmmcDeviceOps *ops);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MMC_EMMC_H */
