/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "hal_iomux.h"
#include "hal_pwm.h"
#include "hal_timer.h"
#include "hal_trace.h"

#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))

/**
 * pwm pin map default for CHIP_BEST2003:
 * pwm0 index = 00  HAL_IOMUX_PIN_P0_0
 * pwm1 index = 01  HAL_IOMUX_PIN_P0_1
 * pwm2 index = 02  HAL_IOMUX_PIN_P0_2
 * pwm3 index = 03  HAL_IOMUX_PIN_P0_3
 * pwm4 index = 04  HAL_IOMUX_PIN_P0_4
 * pwm5 index = 05  HAL_IOMUX_PIN_P0_5
 * pwm6 index = 06  HAL_IOMUX_PIN_P0_6
 * pwm7 index = 07  HAL_IOMUX_PIN_P0_7
 */
void pwm_test(void)
{
    int ret;
    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = 8000, // unit: Hz
        .ratio = 25,  // duty cycle = high/(high+low)
        .inv = false,
        .sleep_on = false,
    };

    printf("pwm test start...\r\n");
    printf("Please use an oscilloscope to view the waveform!\r\n");

    hal_iomux_set_pwm0();
    hal_iomux_set_pwm1();
    hal_iomux_set_pwm2();
    hal_iomux_set_pwm3();
    hal_iomux_set_pwm4();
    hal_iomux_set_pwm5();
    hal_iomux_set_pwm6();
    hal_iomux_set_pwm7();

    ret = hal_pwm_enable(HAL_PWM_ID_0, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM0: %d", ret);

    pwm_cfg.freq = 4000;
    ret = hal_pwm_enable(HAL_PWM_ID_1, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM1: %d", ret);

    pwm_cfg.freq = 2000;
    pwm_cfg.ratio = 50;
    ret = hal_pwm_enable(HAL_PWM_ID_2, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM2: %d", ret);

    pwm_cfg.freq = 1000;
    pwm_cfg.ratio = 75;
    ret = hal_pwm_enable(HAL_PWM_ID_3, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM3: %d", ret);

    pwm_cfg.freq = 8000;
    ret = hal_pwm_enable(HAL_PWM1_ID_0, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM0: %d", ret);

    pwm_cfg.freq = 4000;
    ret = hal_pwm_enable(HAL_PWM1_ID_1, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM1: %d", ret);

    pwm_cfg.freq = 2000;
    ret = hal_pwm_enable(HAL_PWM1_ID_2, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM2: %d", ret);

    pwm_cfg.freq = 1000;
    ret = hal_pwm_enable(HAL_PWM1_ID_3, &pwm_cfg);
    ASSERT(ret == 0, "Failed to enable PWM3: %d", ret);
    mdelay(5 * 1000);

    printf("change freq while pwm running\r\n");
    pwm_cfg.freq = 1000;
    ret = hal_pwm_enable(HAL_PWM_ID_2, &pwm_cfg);
    mdelay(5 * 1000);

    printf("change ratio while pwm running\r\n");
    pwm_cfg.ratio = 66;
    ret = hal_pwm_enable(HAL_PWM_ID_2, &pwm_cfg);
    mdelay(5 * 1000);

    printf("disable pwm one by one\r\n");
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM_ID_0);
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM_ID_1);
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM_ID_2);
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM_ID_3);
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM1_ID_0);
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM1_ID_1);
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM1_ID_2);
    mdelay(5 * 1000);
    hal_pwm_disable(HAL_PWM1_ID_3);
    printf("pwm test done\r\n");
}
