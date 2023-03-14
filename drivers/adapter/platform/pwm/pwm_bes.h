/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef __PWM_BES_H__
#define __PWM_BES_H__

#include "hal_pwm.h"
#include "hal_gpio.h"
#ifdef CHIP_BEST2003
#include "hal_iomux.h"
#endif
#include "pwm_core.h"

#ifdef __cplusplus
extern "C" {
#endif

struct PwmResource {
    uint32_t pwmPin;
    uint32_t pwmId;
};

struct PwmDevice {
    struct IDeviceIoService ioService;
    struct HAL_PWM_CFG_T pwmCfg;
    struct PwmConfig *cfg;
    struct PwmResource resource;
};

#ifdef __cplusplus
}
#endif
#endif
