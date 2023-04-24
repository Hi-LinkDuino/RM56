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
#ifndef __TGT_HARDWARE__
#define __TGT_HARDWARE__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"
#include "hal_aud.h"

//config hwardware codec iir.
#define EQ_HW_DAC_IIR_LIST_NUM              1
#define EQ_HW_ADC_IIR_LIST_NUM              1
#define EQ_HW_IIR_LIST_NUM                  1
#define EQ_SW_IIR_LIST_NUM                  1
#define EQ_HW_FIR_LIST_NUM                  3

//pwl
#define CFG_HW_PWL_NUM (2)
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_pinmux_pwl[CFG_HW_PWL_NUM];

//adckey define
#define CFG_HW_ADCKEY_NUMBER 5
#define CFG_HW_ADCKEY_BASE 0
#define CFG_HW_ADCKEY_ADC_MAXVOLT 1000
#define CFG_HW_ADCKEY_ADC_MINVOLT 0
#define CFG_HW_ADCKEY_ADC_KEYVOLT_BASE 0
extern const uint16_t CFG_HW_ADCKEY_MAP_TABLE[CFG_HW_ADCKEY_NUMBER];
extern const uint16_t CFG_HW_ADCKEY_VOL_TABLE[CFG_HW_ADCKEY_NUMBER];
#define BTA_AV_CO_SBC_MAX_BITPOOL  52

//gpiokey define
#ifdef IS_MULTI_AI_ENABLED
#define CFG_HW_GPIOKEY_NUM (3)
#else
#define CFG_HW_GPIOKEY_NUM (1)
#endif
extern const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM];

// ANC function key
#define ANC_FUNCTION_KEY                    HAL_KEY_CODE_PWR

// ANC coefficient curve number
#define ANC_COEF_NUM                        (1)

//#define ANC_TALK_THROUGH

#ifdef ANC_TALK_THROUGH
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM + 1)
#else
#define ANC_COEF_LIST_NUM                   (ANC_COEF_NUM)
#endif

#define ANC_FF_MIC_CH_L                     AUD_CHANNEL_MAP_CH0
#define ANC_FF_MIC_CH_R                     AUD_CHANNEL_MAP_CH1
#define ANC_FB_MIC_CH_L                     AUD_CHANNEL_MAP_CH2
#define ANC_FB_MIC_CH_R                     AUD_CHANNEL_MAP_CH3

// audio codec
#define CFG_HW_AUD_INPUT_PATH_NUM           3
#ifdef HW_AUD_LOOPBACK_MODE
#ifndef CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_VMIC_MAP_VMIC1)
#endif
#else
#ifndef USB_AUDIO_FS
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_VMIC_MAP_VMIC1)
#else
#define CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV   (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_VMIC_MAP_VMIC1)
#endif /*USB_AUDIO_FS*/
#endif /* HW_AUD_LOOPBACK_MODE */

extern const struct AUD_IO_PATH_CFG_T cfg_audio_input_path_cfg[CFG_HW_AUD_INPUT_PATH_NUM];

#ifdef ALSA_STEREO_TO_MONO_EN
#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH1)
#else
#define CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#endif
#define CFG_HW_AUD_SIDETONE_MIC_DEV         (AUD_CHANNEL_MAP_CH0)
#define CFG_HW_AUD_SIDETONE_GAIN_DBVAL      (-20)

//bt config
extern const char *BT_LOCAL_NAME;
extern const char *BLE_DEFAULT_NAME;
extern uint8_t ble_global_addr[6];
extern uint8_t bt_global_addr[6];
extern uint8_t wifi_addr[6];

#define CODEC_SADC_VOL (12)

// #ifndef TGT_VOLUME_LEVEL_USER_MAX    // for user specific
// #define TGT_VOLUME_LEVEL_USER_MAX        (17)
// #endif

#ifdef TGT_VOLUME_LEVEL_USER_MAX
#define TGT_VOLUME_LEVEL_FACTORY_MAX     (6)

/* silence + user_vol_tbl + factory_vol_tbl + + debug_vol_tbl + 1 */
#define TGT_VOLUME_LEVEL_QTY_USER_SPECIFIC      ( 1 + \
                                                  TGT_VOLUME_LEVEL_USER_MAX + \
                                                  TGT_VOLUME_LEVEL_FACTORY_MAX + \
                                                  TGT_VOLUME_LEVEL_DEBUG_MAX + \
                                                  1 )

#ifdef TGT_VOLUME_LEVEL_USER_MAX
extern const uint32_t alsa_float_volume_mapping_array[TGT_VOLUME_LEVEL_USER_MAX];
uint8_t tgt_user_volume_step_get(void);
#endif

#ifdef TGT_VOLUME_LEVEL_FACTORY_MAX
uint8_t tgt_factory_volume_step_get(void);
#endif
#endif // TGT_VOLUME_LEVEL_USER_MAX

#ifdef TGT_VOLUME_LEVEL_QTY_USER_SPECIFIC
extern const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY_USER_SPECIFIC];
#else
extern const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY];
#endif

#define CFG_AUD_EQ_IIR_NUM_BANDS (4)

//battery info
#define APP_BATTERY_MIN_MV (3200)
#define APP_BATTERY_PD_MV   (3100)

#define APP_BATTERY_MAX_MV (4200)

extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_enable_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg;

#define CFG_HW_AUD_OUTPUT_POP_SWITCH HAL_IOMUX_PIN_P2_5

#ifdef __cplusplus
}
#endif

#endif
