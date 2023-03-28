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
#ifndef __PMU_BEST2003_H__
#define __PMU_BEST2003_H__

#include "hal_cmu.h"
#include "hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_PMU_REG(reg)                   (((reg) & 0xFFF) | 0x0000)
#define ISPI_ANA_REG(reg)                   (((reg) & 0xFFF) | 0x1000)
#define ISPI_RF_REG(reg)                    (((reg) & 0xFFF) | 0x2000)
#define ISPI_WFRF_REG(reg)                  (((reg) & 0xFFF) | 0x3000)

#define ana_read(reg,val)                   hal_analogif_reg_read(ISPI_ANA_REG(reg),val)
#define ana_write(reg,val)                  hal_analogif_reg_write(ISPI_ANA_REG(reg),val)
#define rf_read(reg,val)                    hal_analogif_reg_read(ISPI_RF_REG(reg),val)
#define rf_write(reg,val)                   hal_analogif_reg_write(ISPI_RF_REG(reg),val)
#define wfrf_read(reg,val)                  hal_analogif_reg_read(ISPI_WFRF_REG(reg),val)
#define wfrf_write(reg,val)                 hal_analogif_reg_write(ISPI_WFRF_REG(reg),val)

#define MAX_VMIC_CH_NUM                     2

enum PMU_EFUSE_PAGE_T {
    PMU_EFUSE_PAGE_SECURITY     = 0,
    PMU_EFUSE_PAGE_BOOT         = 1,
    PMU_EFUSE_PAGE_FLASH_CFG    = 2,
    PMU_EFUSE_PAGE_BATTER_LV    = 3,

    PMU_EFUSE_PAGE_BATTER_HV    = 4,
    PMU_EFUSE_PAGE_RESERVED_5   = 5,
    PMU_EFUSE_PAGE_CODE_VER     = 6,
    PMU_EFUSE_PAGE_WIFIBT_CALI  = 7,

    PMU_EFUSE_PAGE_DCDC_CALI    = 8,
    PMU_EFUSE_PAGE_DCCALIB2_L   = 9,
    PMU_EFUSE_PAGE_DCCALIB2_R   = 10,
    PMU_EFUSE_PAGE_DCCALIB_L    = 11,

    PMU_EFUSE_PAGE_DCCALIB_R    = 12,
    PMU_EFUSE_PAGE_TEMP_CALI    = 13,
    PMU_EFUSE_PAGE_GPADC_CALI   = 14,
    PMU_EFUSE_PAGE_WIFI_5G_CALI = 15,
};

#define PMU_EFUSE_PAGE_GPADC_CALI PMU_EFUSE_PAGE_GPADC_CALI

enum PMU_PLL_DIV_TYPE_T {
    PMU_PLL_DIV_DIG,
    PMU_PLL_DIV_PSRAM,
    PMU_PLL_DIV_DSI,
};

enum PMU_IRQ_TYPE_T {
    PMU_IRQ_TYPE_GPADC,
    PMU_IRQ_TYPE_RTC,
    PMU_IRQ_TYPE_CHARGER,
    PMU_IRQ_TYPE_GPIO,
    PMU_IRQ_TYPE_WDT,

    PMU_IRQ_TYPE_QTY
};

enum FLASH_SIZE_TYPE_T {
    FLASH_16M,
    FLASH_32M,
};

#define MAX_VMIC_CH_NUM                 2

union SECURITY_VALUE_T {
    struct {
        unsigned short security_en      :1;
        unsigned short mode             :1;
        unsigned short sig_type         :1;
        unsigned short reserved         :1;
        unsigned short key_id           :2;
        unsigned short vendor_id        :5;
        unsigned short flash_id         :1;
        unsigned short chksum           :4;
    } root;
    struct {
        unsigned short security_en      :1;
        unsigned short mode             :1;
        unsigned short sig_type         :1;
        unsigned short skip_romkey      :1;
        unsigned short key_id           :2;
        unsigned short reg_base         :2;
        unsigned short reg_size         :2;
        unsigned short reg_offset       :1;
        unsigned short flash_id         :1;
        unsigned short chksum           :4;
    } otp;
    unsigned short reg;
};

union BOOT_SETTINGS_T {
    struct {
        unsigned short usb_dld_dis      :1;
        unsigned short uart_dld_en      :1;
        unsigned short trace_en         :1;
        unsigned short pll_dis          :1;
        unsigned short uart_baud_div2   :1;
        unsigned short sec_freq_div2    :1;
        unsigned short crystal_freq     :2;
        unsigned short timeout_div      :2;
        unsigned short uart_connected   :1;
        unsigned short uart_1p8v        :1;
        unsigned short sec_eng_dis      :1;
        unsigned short sec_boot_struct  :1;
        unsigned short sec_check_ver    :1;
        unsigned short reserved         :1;
    };
    unsigned short reg;
};

union BOOT_FLASH_CFG_T {
    struct {
        unsigned short nand_flash       :1;
        unsigned short page_type        :1;
        unsigned short ecc_type         :3;
        unsigned short plane_num        :2;
        unsigned short plane_offset_type:2;
        unsigned short reserved         :3;
        unsigned short chksum           :4;
    };
    unsigned short reg;
};

#define SECURITY_VALUE_T                       SECURITY_VALUE_T

#define PMU_BOOT_FLASH_ID

enum HAL_FLASH_ID_T pmu_get_boot_flash_id(void);

uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg);

void pmu_codec_hppa_enable(int enable);

void pmu_codec_mic_bias_enable(uint32_t map);

void pmu_codec_mic_bias_lowpower_mode(uint32_t map);

void pmu_set_dsi_clk(uint32_t dsi_clk);

void pmu_pll_div_reset_set(enum HAL_CMU_PLL_T pll);

void pmu_pll_div_reset_clear(enum HAL_CMU_PLL_T pll);

void pmu_pll_div_set(enum HAL_CMU_PLL_T pll, enum PMU_PLL_DIV_TYPE_T type, uint32_t div);

void pmu_led_uart_enable(enum HAL_IOMUX_PIN_T pin);

void pmu_led_uart_disable(enum HAL_IOMUX_PIN_T pin);

void pmu_reboot_hook();

void pmu_vcore_set_high_volt();

void pmu_vcore_set_normal_volt();

uint16_t wifi_rf_read_temperature_2003(void);

void pmu_temperature_sensor_init(void);

void wifi_temperature_init(void);

int32_t pmu_read_temperature(void);

void bbpl_temprt_comp_init_2003(void);

void bbpl_compensate_by_temprt_2003(void);

int pmu_set_security_value(enum FLASH_SIZE_TYPE_T flash_size);

int pmu_set_sec_boot_struct(void);

int pmu_get_boot_setting(union BOOT_SETTINGS_T *val);

int pmu_set_code_ver(unsigned short data);

int pmu_get_code_ver(unsigned short *val);

int pmu_set_boot_flash_cfg(void);

int pmu_get_boot_flash_cfg(union BOOT_FLASH_CFG_T *val);

int pmu_get_efuse_wifi_cali(uint8_t *pa_pad_cap_m, uint8_t *pa_driver_a);

int pmu_set_efuse_wifi_cali(uint8_t pa_pad_cap_m, uint8_t pa_driver_a);

void pmu_wf_tx_config(uint8_t band);

#ifdef __cplusplus
}
#endif

#endif

