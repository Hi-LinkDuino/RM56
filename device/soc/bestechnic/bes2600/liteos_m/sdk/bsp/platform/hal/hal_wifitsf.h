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
#ifndef __HAL_WIFITSF_H__
#define __HAL_WIFITSF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define PAC_TIM_TIMER_CONTROL  (0x82C00E00 + 0x34)
#define PAC_TIM_TSF_TIMER_LOW  (0x82C00E00 + 0x38)
#define PAC_TIM_TSF_TIMER_HIGH (0x82C00E00 + 0x3C)


void hal_wifitsf_enable(void);
void hal_wifitsf_disable(void);
uint32_t hal_wifitsf_timer_get();
void hal_wifitsf_timer_set(uint32_t low, uint32_t high);
uint32_t hal_wifitsf_dma_time_get();
uint32_t hal_wifitsf_usb_time_get();
void hal_wifitsf_set_trigger_time(uint32_t val);
void hal_wifitsf_set_usb_tirg_shreshold(uint32_t val);

uint32_t hal_wifitsf_dma_time_get_high();
uint32_t hal_wifitsf_get_tsf_high();
uint32_t hal_wifitsf_get_tsf_low();
uint32_t hal_wifitsf_set_tsf_init_value(uint32_t low, uint32_t hight);

#ifdef __cplusplus
}
#endif

#endif
