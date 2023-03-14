/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_power.h"
#include "gpio_if.h"
#include "hdf_base.h"
#include "hdf_chip.h"
#include "hdf_chip_config.h"

#define MAX_POWER_COUNT 4

struct NoManagablePower {
    struct HdfPower base;
    uint8_t powerSeqDelay;
};

struct GpioBasedPower {
    struct HdfPower base;
    uint8_t powerSeqDelay;
    uint8_t gpioId;
    uint8_t activeLevel;
};

struct MutiPowers {
    struct HdfPower base;
    uint8_t innerPowerCount;
    struct HdfPower *powers[0];
};

static int32_t NotManagablePowerOn(struct HdfPower *power) {
    (void)power;
    return HDF_SUCCESS;
}

static int32_t NotManagablePowerOff(struct HdfPower *power) {
    (void)power;
    return HDF_FAILURE;
}

static void ReleasePower(struct HdfPower *power) {
    if (power == NULL) {
        return;
    }
    OsalMemFree(power);
}

static struct NoManagablePower *CreateNoManagablePower(const struct HdfPowerConfig *power) {
    struct NoManagablePower *result = NULL;
    static const struct HdfPowerOps notManagablePowerOps = {.On = NotManagablePowerOn,
                                                            .Off = NotManagablePowerOff,
                                                            .Release = ReleasePower};
    result = (struct NoManagablePower *)OsalMemCalloc(sizeof(struct NoManagablePower));
    if (result == NULL) {
        return NULL;
    }
    result->base.ops = &notManagablePowerOps;
    result->powerSeqDelay = power->powerSeqDelay;
    return result;
}

static int32_t GpioPowerOn(struct HdfPower *power) {
    int32_t ret;
    struct GpioBasedPower *gpioPower = (struct GpioBasedPower *)power;
    if (power == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return HDF_FAILURE;
    }
    ret = GpioSetDir(gpioPower->gpioId, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:set dir fail! ret=%d\n", __func__, ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(gpioPower->gpioId, gpioPower->activeLevel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:set power on fail! ret=%d\n", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GpioPowerOff(struct HdfPower *power) {
    int32_t ret;
    struct GpioBasedPower *gpioPower = (struct GpioBasedPower *)power;
    if (power == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return HDF_FAILURE;
    }
    ret = GpioSetDir(gpioPower->gpioId, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:set dir fail! ret=%d\n", __func__, ret);
        return HDF_FAILURE;
    }
    ret = GpioWrite(gpioPower->gpioId, !gpioPower->activeLevel);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:set power on fail! ret=%d\n", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct GpioBasedPower *CreateGpioBasedPower(const struct HdfPowerConfig *power) {
    struct GpioBasedPower *result = NULL;
    result = (struct GpioBasedPower *)OsalMemCalloc(sizeof(struct GpioBasedPower));
    if (result == NULL) {
        return NULL;
    }
    static const struct HdfPowerOps notManagablePowerOps = {.On = GpioPowerOn,
                                                            .Off = GpioPowerOff,
                                                            .Release = ReleasePower};
    result->base.ops = &notManagablePowerOps;
    result->powerSeqDelay = power->powerSeqDelay;
    result->gpioId = power->gpio.gpioId;
    result->activeLevel = power->gpio.activeLevel;
    return result;
}

static struct HdfPower *CreatePower(const struct HdfPowerConfig *power) {
    if (power == NULL) {
        return NULL;
    }
    if (power->type == POWER_TYPE_ALWAYS_ON) {
        return (struct HdfPower *)CreateNoManagablePower(power);
    } else if (power->type == POWER_TYPE_GPIO) {
        return (struct HdfPower *)CreateGpioBasedPower(power);
    } else {
        HDF_LOGE("%s:not supported power type %d", __func__, power->type);
        return NULL;
    }
}

static int32_t ActiveMutiPower(struct HdfPower *power) {
    struct MutiPowers *mutiPower = (struct MutiPowers *)power;
    int ret;
    if (power == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return HDF_FAILURE;
    }
    for (uint8_t i = 0; i < mutiPower->innerPowerCount; i++) {
        if (mutiPower->powers[i] == NULL || mutiPower->powers[i]->ops == NULL ||
            mutiPower->powers[i]->ops->On == NULL) {
            HDF_LOGW("%s:bad power!index=%d", __func__, i);
            ret = HDF_FAILURE;
            break;
        }
        if (i > 0) {
            struct NoManagablePower *innerPower = (struct NoManagablePower *)mutiPower->powers[i];
            OsalMSleep(innerPower->powerSeqDelay);
        }
        ret = mutiPower->powers[i]->ops->On(mutiPower->powers[i]);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    return ret;
}

static int32_t DeactiveMutiPower(struct HdfPower *power) {
    struct MutiPowers *mutiPower = (struct MutiPowers *)power;
    int ret;
    if (power == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return HDF_FAILURE;
    }
    for (uint8_t i = 0; i < mutiPower->innerPowerCount; i++) {
        if (mutiPower->powers[i] == NULL || mutiPower->powers[i]->ops == NULL ||
            mutiPower->powers[i]->ops->Off == NULL) {
            HDF_LOGW("%s:bad power!index=%d", __func__, i);
            ret = HDF_FAILURE;
            break;
        }
        ret = mutiPower->powers[i]->ops->Off(mutiPower->powers[i]);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    return ret;
}

static void ReleaseMutiPower(struct HdfPower *power) {
    struct MutiPowers *mutiPower = (struct MutiPowers *)power;
    if (power == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return;
    }
    for (uint8_t i = 0; i < mutiPower->innerPowerCount; i++) {
        if (mutiPower->powers[i] == NULL || mutiPower->powers[i]->ops == NULL ||
            mutiPower->powers[i]->ops->Release == NULL) {
            HDF_LOGW("%s:bad power!index=%d", __func__, i);
        } else {
            mutiPower->powers[i]->ops->Release(mutiPower->powers[i]);
        }
        mutiPower->powers[i] = NULL;
    }
    OsalMemFree(power);
}

static struct MutiPowers *CreateMutiPower(const struct HdfPowersConfig *powersConfig) {
    int ret = HDF_SUCCESS;
    struct MutiPowers *mutiPower =
        OsalMemCalloc(sizeof(struct MutiPowers) + sizeof(struct HdfPower *) * powersConfig->powerCount);
    static const struct HdfPowerOps mutiPowerOps = {.On = ActiveMutiPower,
                                                    .Off = DeactiveMutiPower,
                                                    .Release = ReleaseMutiPower};
    for (uint8_t i = 0; i < powersConfig->powerCount; i++) {
        mutiPower->powers[i] = CreatePower(powersConfig->power + i);
        if (mutiPower->powers[i] == NULL) {
            OsalMemFree(mutiPower);
            ret = HDF_FAILURE;
            break;
        }
    }
    mutiPower->innerPowerCount = powersConfig->powerCount;
    if (ret != HDF_SUCCESS) {
        ReleaseMutiPower((struct HdfPower *)mutiPower);
        return NULL;
    }
    mutiPower->base.ops = &mutiPowerOps;
    return mutiPower;
}

struct HdfPower *CreateVirtualPower(const struct HdfPowersConfig *powers) {
    if (powers == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return NULL;
    }
    if (powers->powerCount > MAX_POWER_COUNT) {
        HDF_LOGE("%s:too many power in config!count=%d", __func__, powers->powerCount);
        return NULL;
    }
    if (powers->powerCount == 1) {
        return CreatePower(&powers->power[0]);
    } else if (powers->powerCount > 1) {
        return (struct HdfPower *)CreateMutiPower(powers);
    } else {
        return NULL;
    }
}
