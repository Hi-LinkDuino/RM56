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
#include "tgt_hardware.h"
#include "iir_process.h"
#include "fir_process.h"
#include "drc.h"
#include "limiter.h"
#include "spectrum_fix.h"

const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PWL_NUM] = {
#if (CFG_HW_PWL_NUM > 0)
    {HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
    {HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE},
#endif
};

//adckey define
/*
const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER] = {
#if (CFG_HW_ADCKEY_NUMBER > 0)
    HAL_KEY_CODE_FN9,HAL_KEY_CODE_FN8,HAL_KEY_CODE_FN7,
    HAL_KEY_CODE_FN6,HAL_KEY_CODE_FN5,HAL_KEY_CODE_FN4,
    HAL_KEY_CODE_FN3,HAL_KEY_CODE_FN2,HAL_KEY_CODE_FN1,
#endif
};
*/
const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER] = {
    HAL_KEY_CODE_FN1,//HAL_KEY_CODE_FN3,HAL_KEY_CODE_FN2,HAL_KEY_CODE_FN1,
    HAL_KEY_CODE_FN2,
    HAL_KEY_CODE_FN3,
    HAL_KEY_CODE_FN5,
    HAL_KEY_CODE_FN6,
};

const uint16_t CFG_HW_ADCKEY_VOL_TABLE[CFG_HW_ADCKEY_NUMBER] = {
    150,
    350,
    500,
    600,
    800,
};
//gpiokey define
#define CFG_HW_GPIOKEY_DOWN_LEVEL          (0)
#define CFG_HW_GPIOKEY_UP_LEVEL            (1)
const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM] = {
#if (CFG_HW_GPIOKEY_NUM > 0)
    //{HAL_KEY_CODE_FN1,{HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    //{HAL_KEY_CODE_FN2,{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    //{HAL_KEY_CODE_FN15,{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
#ifdef IS_MULTI_AI_ENABLED
    {HAL_KEY_CODE_FN13,{HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
    {HAL_KEY_CODE_FN14,{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE}},
#endif
#endif
};

//bt config
const char *BT_LOCAL_NAME = TO_STRING(BT_DEV_NAME) "\0";
const char *BLE_DEFAULT_NAME = "BES_BLE";
uint8_t ble_global_addr[6] = {
#ifdef BLE_DEV_ADDR
	BLE_DEV_ADDR
#else
	0xBE,0x99,0x34,0x45,0x56,0x67
#endif
};
uint8_t bt_global_addr[6] = {
#ifdef BT_DEV_ADDR
	BT_DEV_ADDR
#else
	0x1e,0x57,0x34,0x45,0x56,0x67
#endif
};

uint8_t wifi_addr[6] = {
    0x00, 0x80, 0xe1, 0x30, 0x50, 0x60
};

#define TX_PA_GAIN                          CODEC_TX_PA_GAIN_DEFAULT

#ifdef TGT_VOLUME_LEVEL_QTY_USER_SPECIFIC
const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY_USER_SPECIFIC] = {

    {TX_PA_GAIN,0x03,-99}, // TGT_VOLUME_LEVEL_USER_0       TGT_VOLUME_LEVEL_0   mute

#if (TGT_VOLUME_LEVEL_USER_MAX == 17)
    {TX_PA_GAIN,0x03,-57}, // TGT_VOLUME_LEVEL_USER_1       TGT_VOLUME_LEVEL_1
    {TX_PA_GAIN,0x03,-47}, // TGT_VOLUME_LEVEL_USER_2       TGT_VOLUME_LEVEL_2
    {TX_PA_GAIN,0x03,-41}, // TGT_VOLUME_LEVEL_USER_3       TGT_VOLUME_LEVEL_3
    {TX_PA_GAIN,0x03,-34}, // TGT_VOLUME_LEVEL_USER_4       TGT_VOLUME_LEVEL_4
    {TX_PA_GAIN,0x03,-29}, // TGT_VOLUME_LEVEL_USER_5       TGT_VOLUME_LEVEL_5
    {TX_PA_GAIN,0x03,-25}, // TGT_VOLUME_LEVEL_USER_6       TGT_VOLUME_LEVEL_6
    {TX_PA_GAIN,0x03,-22}, // TGT_VOLUME_LEVEL_USER_7       TGT_VOLUME_LEVEL_7
    {TX_PA_GAIN,0x03,-20}, // TGT_VOLUME_LEVEL_USER_8       TGT_VOLUME_LEVEL_8
    {TX_PA_GAIN,0x03,-18}, // TGT_VOLUME_LEVEL_USER_9       TGT_VOLUME_LEVEL_9
    {TX_PA_GAIN,0x03,-16}, // TGT_VOLUME_LEVEL_USER_10      TGT_VOLUME_LEVEL_10
    {TX_PA_GAIN,0x03,-14}, // TGT_VOLUME_LEVEL_USER_11      TGT_VOLUME_LEVEL_11
    {TX_PA_GAIN,0x03,-12}, // TGT_VOLUME_LEVEL_USER_12      TGT_VOLUME_LEVEL_12
    {TX_PA_GAIN,0x03,-10}, // TGT_VOLUME_LEVEL_USER_13      TGT_VOLUME_LEVEL_13
    {TX_PA_GAIN,0x03, -8}, // TGT_VOLUME_LEVEL_USER_14      TGT_VOLUME_LEVEL_14
    {TX_PA_GAIN,0x03, -6}, // TGT_VOLUME_LEVEL_USER_15      TGT_VOLUME_LEVEL_15
    {TX_PA_GAIN,0x03, -3}, // TGT_VOLUME_LEVEL_USER_16      TGT_VOLUME_LEVEL_16
    {TX_PA_GAIN,0x03,  0}, // TGT_VOLUME_LEVEL_USER_17      TGT_VOLUME_LEVEL_17
#elif (TGT_VOLUME_LEVEL_USER_MAX == 24)
    {TX_PA_GAIN,0x03,-59}, // TGT_VOLUME_LEVEL_USER_1       TGT_VOLUME_LEVEL_1
    {TX_PA_GAIN,0x03,-51}, // TGT_VOLUME_LEVEL_USER_2       TGT_VOLUME_LEVEL_2
    {TX_PA_GAIN,0x03,-44}, // TGT_VOLUME_LEVEL_USER_3       TGT_VOLUME_LEVEL_3
    {TX_PA_GAIN,0x03,-38}, // TGT_VOLUME_LEVEL_USER_4       TGT_VOLUME_LEVEL_4
    {TX_PA_GAIN,0x03,-33}, // TGT_VOLUME_LEVEL_USER_5       TGT_VOLUME_LEVEL_5
    {TX_PA_GAIN,0x03,-29}, // TGT_VOLUME_LEVEL_USER_6       TGT_VOLUME_LEVEL_6
    {TX_PA_GAIN,0x03,-26}, // TGT_VOLUME_LEVEL_USER_7       TGT_VOLUME_LEVEL_7
    {TX_PA_GAIN,0x03,-23}, // TGT_VOLUME_LEVEL_USER_8       TGT_VOLUME_LEVEL_8
    {TX_PA_GAIN,0x03,-21}, // TGT_VOLUME_LEVEL_USER_9       TGT_VOLUME_LEVEL_9
    {TX_PA_GAIN,0x03,-19}, // TGT_VOLUME_LEVEL_USER_10      TGT_VOLUME_LEVEL_10
    {TX_PA_GAIN,0x03,-17}, // TGT_VOLUME_LEVEL_USER_11      TGT_VOLUME_LEVEL_11
    {TX_PA_GAIN,0x03,-15}, // TGT_VOLUME_LEVEL_USER_12      TGT_VOLUME_LEVEL_12
    {TX_PA_GAIN,0x03,-13}, // TGT_VOLUME_LEVEL_USER_13      TGT_VOLUME_LEVEL_13
    {TX_PA_GAIN,0x03,-11}, // TGT_VOLUME_LEVEL_USER_14      TGT_VOLUME_LEVEL_14
    {TX_PA_GAIN,0x03,-10}, // TGT_VOLUME_LEVEL_USER_15      TGT_VOLUME_LEVEL_15
    {TX_PA_GAIN,0x03, -8}, // TGT_VOLUME_LEVEL_USER_16      TGT_VOLUME_LEVEL_16
    {TX_PA_GAIN,0x03, -7}, // TGT_VOLUME_LEVEL_USER_17      TGT_VOLUME_LEVEL_17
    {TX_PA_GAIN,0x03, -6}, // TGT_VOLUME_LEVEL_USER_18      TGT_VOLUME_LEVEL_18
    {TX_PA_GAIN,0x03, -5}, // TGT_VOLUME_LEVEL_USER_19      TGT_VOLUME_LEVEL_19
    {TX_PA_GAIN,0x03, -4}, // TGT_VOLUME_LEVEL_USER_20      TGT_VOLUME_LEVEL_20
    {TX_PA_GAIN,0x03, -3}, // TGT_VOLUME_LEVEL_USER_21      TGT_VOLUME_LEVEL_21
    {TX_PA_GAIN,0x03, -2}, // TGT_VOLUME_LEVEL_USER_22      TGT_VOLUME_LEVEL_22
    {TX_PA_GAIN,0x03, -1}, // TGT_VOLUME_LEVEL_USER_23      TGT_VOLUME_LEVEL_23
    {TX_PA_GAIN,0x03,  0}, // TGT_VOLUME_LEVEL_USER_24      TGT_VOLUME_LEVEL_24
#else
    {TX_PA_GAIN,0x03,-45}, // TGT_VOLUME_LEVEL_USER_1       TGT_VOLUME_LEVEL_1
    {TX_PA_GAIN,0x03,-42}, // TGT_VOLUME_LEVEL_USER_2       TGT_VOLUME_LEVEL_2
    {TX_PA_GAIN,0x03,-39}, // TGT_VOLUME_LEVEL_USER_3       TGT_VOLUME_LEVEL_3
    {TX_PA_GAIN,0x03,-36}, // TGT_VOLUME_LEVEL_USER_4       TGT_VOLUME_LEVEL_4
    {TX_PA_GAIN,0x03,-33}, // TGT_VOLUME_LEVEL_USER_5       TGT_VOLUME_LEVEL_5
    {TX_PA_GAIN,0x03,-30}, // TGT_VOLUME_LEVEL_USER_6       TGT_VOLUME_LEVEL_6
    {TX_PA_GAIN,0x03,-27}, // TGT_VOLUME_LEVEL_USER_7       TGT_VOLUME_LEVEL_7
    {TX_PA_GAIN,0x03,-24}, // TGT_VOLUME_LEVEL_USER_8       TGT_VOLUME_LEVEL_8
    {TX_PA_GAIN,0x03,-21}, // TGT_VOLUME_LEVEL_USER_9       TGT_VOLUME_LEVEL_9
    {TX_PA_GAIN,0x03,-18}, // TGT_VOLUME_LEVEL_USER_10      TGT_VOLUME_LEVEL_10
    {TX_PA_GAIN,0x03,-15}, // TGT_VOLUME_LEVEL_USER_11      TGT_VOLUME_LEVEL_11
    {TX_PA_GAIN,0x03,-12}, // TGT_VOLUME_LEVEL_USER_12      TGT_VOLUME_LEVEL_12
    {TX_PA_GAIN,0x03, -9}, // TGT_VOLUME_LEVEL_USER_13      TGT_VOLUME_LEVEL_13
    {TX_PA_GAIN,0x03, -6}, // TGT_VOLUME_LEVEL_USER_14      TGT_VOLUME_LEVEL_14
    {TX_PA_GAIN,0x03, -3}, // TGT_VOLUME_LEVEL_USER_15      TGT_VOLUME_LEVEL_15
    {TX_PA_GAIN,0x03,  0}, // TGT_VOLUME_LEVEL_USER_16      TGT_VOLUME_LEVEL_16
#endif
    {TX_PA_GAIN,0x03, -5}, // TGT_VOLUME_LEVEL_FACTORY_0    TGT_VOLUME_LEVEL_17
    {TX_PA_GAIN,0x03, -4}, // TGT_VOLUME_LEVEL_FACTORY_1    TGT_VOLUME_LEVEL_18
    {TX_PA_GAIN,0x03, -3}, // TGT_VOLUME_LEVEL_FACTORY_2    TGT_VOLUME_LEVEL_19
    {TX_PA_GAIN,0x03, -2}, // TGT_VOLUME_LEVEL_FACTORY_3    TGT_VOLUME_LEVEL_20
    {TX_PA_GAIN,0x03, -1}, // TGT_VOLUME_LEVEL_FACTORY_4    TGT_VOLUME_LEVEL_21
    {TX_PA_GAIN,0x03,  0}, // TGT_VOLUME_LEVEL_FACTORY_5    TGT_VOLUME_LEVEL_22
};
#else
const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY] = {
    {TX_PA_GAIN,0x03,-99},
    {TX_PA_GAIN,0x03,-45},
    {TX_PA_GAIN,0x03,-42},
    {TX_PA_GAIN,0x03,-39},
    {TX_PA_GAIN,0x03,-36},
    {TX_PA_GAIN,0x03,-33},
    {TX_PA_GAIN,0x03,-30},
    {TX_PA_GAIN,0x03,-27},
    {TX_PA_GAIN,0x03,-24},
    {TX_PA_GAIN,0x03,-21},
    {TX_PA_GAIN,0x03,-18},
    {TX_PA_GAIN,0x03,-15},
    {TX_PA_GAIN,0x03,-12},
    {TX_PA_GAIN,0x03, -9},
    {TX_PA_GAIN,0x03, -6},
    {TX_PA_GAIN,0x03, -3},
    {TX_PA_GAIN,0x03,  0},  //0dBm
};
#endif

#ifdef TGT_VOLUME_LEVEL_USER_MAX
#if (TGT_VOLUME_LEVEL_USER_MAX == 17)
uint32_t const alsa_float_volume_mapping_array[TGT_VOLUME_LEVEL_USER_MAX] = {1, 6, 13, 19, 25, 32, 37, 44, 50, 56, 63, 69, 75, 81, 87, 93, 100};
#elif (TGT_VOLUME_LEVEL_USER_MAX == 24)
uint32_t const alsa_float_volume_mapping_array[TGT_VOLUME_LEVEL_USER_MAX] = {1, 3, 8, 13, 18, 23, 28, 33, 38, 43, 48, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 100};
#else
uint32_t const alsa_float_volume_mapping_array[TGT_VOLUME_LEVEL_USER_MAX] = {0};
#endif
uint8_t tgt_user_volume_step_get(void)
{
    return TGT_VOLUME_LEVEL_USER_MAX;
}
#endif

#ifdef TGT_VOLUME_LEVEL_FACTORY_MAX
uint8_t tgt_factory_volume_step_get(void)
{
    return TGT_VOLUME_LEVEL_FACTORY_MAX;
}
#endif

#if SPEECH_CODEC_CAPTURE_CHANNEL_NUM == 2
#ifndef CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_VMIC_MAP_VMIC1)
#endif
#else
#ifdef USB_AUDIO_FS
#ifndef CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_VMIC_MAP_VMIC1)
#endif
#else
#ifndef CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | AUD_VMIC_MAP_VMIC1)
#endif
#endif //USB_AUDIO_FS
#endif
#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV    (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV    (AUD_CHANNEL_MAP_CH0 | AUD_VMIC_MAP_VMIC1)

const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM] = {
#if defined(SPEECH_TX_AEC_CODEC_REF)
#ifdef USB_AUDIO_FS
    { AUD_INPUT_PATH_MAINMIC, CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV, },
#else
    { AUD_INPUT_PATH_MAINMIC, CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV | AUD_CHANNEL_MAP_ECMIC_CH0, },
#endif //USB_AUDIO_FS
#else
    { AUD_INPUT_PATH_MAINMIC, CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV, },
#endif
    { AUD_INPUT_PATH_LINEIN,  CFG_HW_AUD_INPUT_PATH_LINEIN_DEV, },
    { AUD_INPUT_PATH_ASRMIC,  CFG_HW_AUD_INPUT_PATH_ASRMIC_DEV, },
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg = {
    HAL_IOMUX_PIN_NUM, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENABLE
};


const IIR_CFG_T audio_eq_sw_iir_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 5,
    .param = {
        {IIR_TYPE_PEAK, .0,   200,   2},
        {IIR_TYPE_PEAK, .0,   600,  2},
        {IIR_TYPE_PEAK, .0,   2000.0, 2},
        {IIR_TYPE_PEAK, .0,  6000.0, 2},
        {IIR_TYPE_PEAK, .0,  12000.0, 2}
    }
};

const IIR_CFG_T * const audio_eq_sw_iir_cfg_list[EQ_SW_IIR_LIST_NUM]={
    &audio_eq_sw_iir_cfg,
};

const FIR_CFG_T audio_eq_hw_fir_cfg_44p1k = {
    .gain = 0.0f,
    .len = 384,
    .coef =
    {
        (1<<23)-1,
    }
};

const FIR_CFG_T audio_eq_hw_fir_cfg_48k = {
    .gain = 0.0f,
    .len = 384,
    .coef =
    {
        (1<<23)-1,
    }
};


const FIR_CFG_T audio_eq_hw_fir_cfg_96k = {
    .gain = 0.0f,
    .len = 384,
    .coef =
    {
        (1<<23)-1,
    }
};

const FIR_CFG_T * const audio_eq_hw_fir_cfg_list[EQ_HW_FIR_LIST_NUM]={
    &audio_eq_hw_fir_cfg_44p1k,
    &audio_eq_hw_fir_cfg_48k,
    &audio_eq_hw_fir_cfg_96k,
};

//hardware dac iir eq
const IIR_CFG_T audio_eq_hw_dac_iir_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 8,
    .param = {
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
        {IIR_TYPE_PEAK, 0,   1000.0,   0.7},
    }
};

const IIR_CFG_T * const POSSIBLY_UNUSED audio_eq_hw_dac_iir_cfg_list[EQ_HW_DAC_IIR_LIST_NUM]={
    &audio_eq_hw_dac_iir_cfg,
};

//hardware dac iir eq
const IIR_CFG_T audio_eq_hw_adc_iir_adc_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 1,
    .param = {
        {IIR_TYPE_PEAK, 0.0,   1000.0,   0.7},
    }
};

const IIR_CFG_T * const POSSIBLY_UNUSED audio_eq_hw_adc_iir_cfg_list[EQ_HW_ADC_IIR_LIST_NUM]={
    &audio_eq_hw_adc_iir_adc_cfg,
};



//hardware iir eq
const IIR_CFG_T audio_eq_hw_iir_cfg = {
    .gain0 = 0,
    .gain1 = 0,
    .num = 8,
    .param = {
        {IIR_TYPE_PEAK, -10.1,   100.0,   7},
        {IIR_TYPE_PEAK, -10.1,   400.0,   7},
        {IIR_TYPE_PEAK, -10.1,   700.0,   7},
        {IIR_TYPE_PEAK, -10.1,   1000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   3000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   5000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   7000.0,   7},
        {IIR_TYPE_PEAK, -10.1,   9000.0,   7},

    }
};

const IIR_CFG_T * const POSSIBLY_UNUSED audio_eq_hw_iir_cfg_list[EQ_HW_IIR_LIST_NUM]={
    &audio_eq_hw_iir_cfg,
};

const DrcConfig audio_drc_cfg = {
     .knee = 3,
     .filter_type = {14, -1},
     .band_num = 2,
     .look_ahead_time = 10,
     .band_settings = {
         {-20, 0, 2, 3, 3000, 1},
         {-20, 0, 2, 3, 3000, 1},
     }
 };

const LimiterConfig audio_limiter_cfg = {
    .knee = 2,
    .look_ahead_time = 10,
    .threshold = -20,
    .makeup_gain = 19,
    .ratio = 1000,
    .attack_time = 3,
    .release_time = 3000,
};

const SpectrumFixConfig audio_spectrum_cfg = {
    .freq_num = 9,
    .freq_list = {200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800},
};
