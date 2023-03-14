/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MMC_SD_H
#define MMC_SD_H

#include "mmc_corex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct SdRegister {
    uint32_t rawScr[SCR_LEN];
    uint16_t rawDsr;
    struct SdScr scr;
    struct SdSsr ssr;
    struct SdSwitchCaps swCaps;
};

struct SdDevice {
    struct MmcDevice mmc;
    enum SdBusSpeedMode busSpeedMode;
    struct SdRegister reg;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MMC_SD_H */
