/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_POWER_STATE_H
#define HDF_POWER_STATE_H

enum HdfPowerState {
    POWER_STATE_DOZE_RESUME,
    POWER_STATE_DOZE_SUSPEND,
    POWER_STATE_RESUME,
    POWER_STATE_SUSPEND,
    POWER_STATE_MAX,
};

static inline bool IsPowerWakeState(uint32_t state)
{
    return state == POWER_STATE_DOZE_RESUME || state == POWER_STATE_RESUME;
}

static inline bool IsValidPowerState(uint32_t state)
{
    return state < POWER_STATE_MAX;
}

#endif /* HDF_POWER_STATE_H */