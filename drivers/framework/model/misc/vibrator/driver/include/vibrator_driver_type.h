/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef VIBRATOR_DRIVER_TYPE_H
#define VIBRATOR_DRIVER_TYPE_H

#include "hdf_log.h"

#define CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(ptr, ret) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return errno", __func__, __LINE__); \
        return (ret); \
    } \
} while (0)

#define CHECK_VIBRATOR_PARSER_RESULT_RETURN_VALUE(ret, str) do { \
    if (ret != HDF_SUCCESS) { \
        HDF_LOGE("%s:line %d %s fail, ret = %d!", __func__, __LINE__, str, ret); \
        return HDF_FAILURE; \
    } \
} while (0)

#define CHECK_VIBRATOR_NULL_PTR_RETURN(ptr) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return", __func__, __LINE__); \
        return; \
    } \
} while (0)

enum VibratorBusType {
    VIBRATOR_BUS_I2C   = 0,
    VIBRATOR_BUS_GPIO  = 1,
};

enum VibratorState {
    VIBRATOR_STATE_IDLE        = 0,
    VIBRATOR_STATE_START_TIMER = 1,
    VIBRATOR_STATE_STOP        = 2,
    VIBRATOR_STATE_SET_EFFECT  = 3,
};

enum VibratorConfigMode {
    VIBRATOR_MODE_ONCE   = 0, // The mode of a one-shot vibration effect.
    VIBRATOR_MODE_PRESET = 1, // The mode of a preset vibration effect.
    VIBRATOR_MODE_BUTT   = 0XFF,
};

enum VibratorDrvIoCmd {
    VIBRATOR_DRV_IO_START_ONCE    = 0,
    VIBRATOR_DRV_IO_START_PRESET  = 1,
    VIBRATOR_DRV_IO_STOP          = 2,
    VIBRATOR_DRV_IO_END,
};

#endif /* VIBRATOR_DRIVER_TYPE_H */
