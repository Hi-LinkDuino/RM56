/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_BL_H
#define HDF_BL_H

#include "hdf_disp.h"
#include "hdf_types.h"

enum FbPowerStatus {
    FB_POWER_ON,
    FB_POWER_STANDBY,
    FB_POWER_SUSPEND,
    FB_POWER_OFF,
};

struct BacklightDev;
struct BacklightOps {
    int32_t (*updateBrightness)(struct BacklightDev *bl, uint32_t brightness);
    uint32_t (*getBrightness)(struct BacklightDev *bl);
};

struct BacklightProperties {
    uint32_t brightness;
    uint32_t maxBrightness;
    uint32_t minBrightness;
    uint32_t defBrightness;
    enum FbPowerStatus fbStatus;
};

struct BacklightDev *RegisterBlDev(const char *name, struct BacklightProperties *props,
    struct BacklightOps *ops, void *priv);
void *ToBlDevPriv(struct BacklightDev *blDev);
struct BacklightDev *GetBacklightDev(const char *name);
int32_t UpdateBacklightState(struct BacklightDev *blDev, enum FbPowerStatus status);
int32_t UpdateBrightness(struct BacklightDev *blDev, uint32_t brightness);
int32_t GetMinBrightness(struct BacklightDev *blDev, uint32_t *brightness);
int32_t GetCurrBrightness(struct BacklightDev *blDev, uint32_t *brightness);
int32_t GetDefBrightness(struct BacklightDev *blDev, uint32_t *brightness);
int32_t GetMaxBrightness(struct BacklightDev *blDev, uint32_t *brightness);
#endif /* HDF_BL_H */

