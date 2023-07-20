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
#ifndef __HWTEST_H__
#define __HWTEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_addr_map.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hwtimer_list.h"

#define REG(a)                          *(volatile uint32_t *)(a)

void sleep_test_register_timer_callback(HWTIMER_CALLBACK_T cb);

int test_rsa(void);

void fm_test(void);

void dpd_test(void);

void codec_test(void);

void spdif_test(void);

void i2c_test(void);

void i2s_test(void);

void af_test(void);

void spi_test(void);

void efuse_test(void);

void ispi_dma_test(void);

void timer_test(void);

void usb_serial_speed_test(void);

void usb_serial_test(void);

void usb_serial_direct_xfer_test(void);

void usb_audio_test(void);

void uart_bridge(void);

void uart_test(void);

void intersys_comm_test(void);

void sleep_test(void);

void flash_test(void);

void nandflash_test(void);

void psram_test(void);

void mbw_test();

void usbhost_test(void);

void bt_conn_sleep_test(void);

void pwm_test(void);

void psramuhs_test(void);

void psram_simu_test(void);

void psramuhs_simu_test(void);

void rf_calib_test(void);

void transq_test(void);

void patch_test(void);

void hwfft_test(void);

void wdt_test(void);

void pmu_wdt_test(void);

void cp_test(void);

void tdm_test(void);

void sec_eng_test(void);

void a7_dsp_test(void);

void sensor_hub_test(void);

void bt_host_test(void);

void spi_norflash_test(void);

void dsi_test(void);

void csi_test(void);

void qspi_display_test(void);

void beco_test(void);

void trng_test(void);

void tz_test(void);

void ir_test(void);

void ft_test_main(void);

void bt_conn_simu(void);

void dac2_test(void);

int wifi_test(uint8_t band);

void sdio_device_test(void);

void sdio_host_test(void);

void sdmmc_test(void);

void sdemmc_test(void);

void emmc_test(void);

void coremark_test(void);

#ifdef __cplusplus
}
#endif

#endif

