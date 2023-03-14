/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_reset.h"
#include "gpio_if.h"
#include "hdf_base.h"

struct GpioBasedReset {
    struct HdfReset base;
    uint8_t resetHoldTime;
    uint8_t gpioId;
    uint8_t activeLevel;
};

int32_t ResetNoManagableReset(struct HdfReset *reset) {
    (void)reset;
    return HDF_FAILURE;
}

void ReleaseNoManagableReset(struct HdfReset *reset) {
    if (reset != NULL) {
        OsalMemFree(reset);
    }
}

int32_t ResetGpioBasedReset(struct HdfReset *reset) {
    int ret;
    struct GpioBasedReset *gpioBasedReset = (struct GpioBasedReset *)reset;
    if (reset == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return HDF_FAILURE;
    }
    ret = GpioSetDir(gpioBasedReset->gpioId, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set dir fail!", __func__);
        return ret;
    }
    ret = GpioWrite(gpioBasedReset->gpioId, gpioBasedReset->activeLevel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write active fail! ret=%d", __func__, ret);
        return ret;
    }
    OsalMSleep(gpioBasedReset->resetHoldTime);

    ret = GpioWrite(gpioBasedReset->gpioId, !gpioBasedReset->activeLevel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write deactivate fail! ret=%d", __func__, ret);
        return ret;
    }
    return ret;
}

void ReleaseGpioBasedReset(struct HdfReset *reset) {
    if (reset != NULL) {
        OsalMemFree(reset);
    }
}

struct HdfReset *CreateVirtualReset(const struct HdfResetConfig *resetConfig) {
    struct HdfReset *result = NULL;
    if (resetConfig == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return NULL;
    }
    if (resetConfig->resetType == RESET_TYPE_NOT_MANAGEABLE) {
        const static struct HdfResetOps noManagableResetOps = {.Reset = ResetNoManagableReset,
                                                               .Release = ReleaseNoManagableReset};
        result = (struct HdfReset *)OsalMemCalloc(sizeof(struct HdfReset));
        result->ops = &noManagableResetOps;
    } else if (resetConfig->resetType == RESET_TYPE_GPIO) {
        const static struct HdfResetOps gpioBasedResetOps = {.Reset = ResetGpioBasedReset,
                                                             .Release = ReleaseGpioBasedReset};
        struct GpioBasedReset *reset = (struct GpioBasedReset *)OsalMemCalloc(sizeof(struct GpioBasedReset));
        reset->resetHoldTime = resetConfig->resetHoldTime;
        reset->gpioId = resetConfig->gpio.gpioId;
        reset->activeLevel = resetConfig->gpio.activeLevel;
        reset->base.ops = &gpioBasedResetOps;
        result = (struct HdfReset *)reset;
    }
    return result;
}