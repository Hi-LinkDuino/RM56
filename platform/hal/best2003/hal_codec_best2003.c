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
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(reg_codec)
#include "hal_codec.h"
#include "hal_psc.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "analog.h"
#include "cmsis.h"
#include "string.h"
#include "tgt_hardware.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_chipid.h"

#define NO_DAC_RESET

//#define SDM_MUTE_NOISE_SUPPRESSION
//#define SDM_MUTE_NOISE_SUPPRESSION_V2

#define DAC_ZERO_CROSSING_GAIN

#ifndef CODEC_OUTPUT_DEV
#define CODEC_OUTPUT_DEV                    CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV
#endif
#if ((CODEC_OUTPUT_DEV & CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV) == 0)
#ifndef AUDIO_OUTPUT_SWAP
#define AUDIO_OUTPUT_SWAP
#endif
#endif

// For 44.1K/48K sample rate
#ifndef CODEC_DAC_GAIN_RAMP_INTERVAL
#define CODEC_DAC_GAIN_RAMP_INTERVAL        CODEC_DAC_GAIN_RAMP_INTVL_16_SAMP
#endif

#if defined(SPEECH_SIDETONE) && \
        (defined(CFG_HW_AUD_SIDETONE_MIC_DEV) && (CFG_HW_AUD_SIDETONE_MIC_DEV)) && \
        defined(CFG_HW_AUD_SIDETONE_GAIN_DBVAL)
#define SIDETONE_ENABLE
#endif

//#define CODEC_TIMER

#ifdef AUDIO_ANC_FB_MC
#error "2003 doesn't support ANC"
#endif

#ifndef __AUDIO_RESAMPLE__
#error "2003 must use resample"
#endif

#ifdef CODEC_DSD
#ifdef __AUDIO_RESAMPLE__
#error "__AUDIO_RESAMPLE__ conflicts with CODEC_DSD"
#endif
#endif

#define RS_CLOCK_FACTOR                     400

// Trigger DMA request when TX-FIFO *empty* count > threshold
#define HAL_CODEC_TX_FIFO_TRIGGER_LEVEL     (3)
// Trigger DMA request when RX-FIFO count >= threshold
#define HAL_CODEC_RX_FIFO_TRIGGER_LEVEL     (4)

#define MAX_DIG_DBVAL                       (50)
#define ZERODB_DIG_DBVAL                    (0)
#define MIN_DIG_DBVAL                       (-99)

#ifndef DAC_DEFAULT_DIG_DBVAL
#define DAC_DEFAULT_DIG_DBVAL               (ZERODB_DIG_DBVAL)
#endif

#define MAX_SIDETONE_DBVAL                  (30)
#define MIN_SIDETONE_DBVAL                  (-32)
#define SIDETONE_DBVAL_STEP                 (-2)

#define MAX_SIDETONE_REGVAL                 (0)
#define MIN_SIDETONE_REGVAL                 (31)

#ifndef MC_DELAY_COMMON
#define MC_DELAY_COMMON                     28
#endif

#ifndef CODEC_DIGMIC_PHASE
#define CODEC_DIGMIC_PHASE                  3
#endif

#define MAX_DIG_MIC_CH_NUM                  6

#define NORMAL_ADC_CH_NUM                   6
// Echo cancel ADC channel number
#define EC_ADC_CH_NUM                       2
#define MAX_ADC_CH_NUM                      (NORMAL_ADC_CH_NUM + EC_ADC_CH_NUM)

#define MAX_DAC_CH_NUM                      2

#ifdef CODEC_DSD
#define NORMAL_MIC_MAP                      (AUD_CHANNEL_MAP_NORMAL_ALL & ~(AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7 | \
                                                AUD_CHANNEL_MAP_DIGMIC_CH6 | AUD_CHANNEL_MAP_DIGMIC_CH7 | \
                                                AUD_CHANNEL_MAP_DIGMIC_CH4 | AUD_CHANNEL_MAP_DIGMIC_CH5))
#else
#define NORMAL_MIC_MAP                      (AUD_CHANNEL_MAP_NORMAL_ALL & ~(AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7 | \
                                                AUD_CHANNEL_MAP_DIGMIC_CH6 | AUD_CHANNEL_MAP_DIGMIC_CH7))
#endif
#define NORMAL_ADC_MAP                      (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5)

#define EC_MIC_MAP                          (AUD_CHANNEL_MAP_ECMIC_CH0 | AUD_CHANNEL_MAP_ECMIC_CH1)
#define EC_ADC_MAP                          (AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7)

#define VALID_MIC_MAP                       (NORMAL_MIC_MAP | EC_MIC_MAP)
#define VALID_ADC_MAP                       (NORMAL_ADC_MAP | EC_ADC_MAP)

#define VALID_SPK_MAP                       (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define VALID_DAC_MAP                       VALID_SPK_MAP

#if (CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV & ~VALID_SPK_MAP)
#error "Invalid CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV"
#endif
#if (CODEC_OUTPUT_DEV & ~VALID_SPK_MAP)
#error "Invalid CODEC_OUTPUT_DEV"
#endif

#define RSTN_ADC_FREE_RUNNING_CLK           CODEC_SOFT_RSTN_ADC(1 << NORMAL_ADC_CH_NUM)

enum CODEC_ADC_EN_REQ_T {
    CODEC_ADC_EN_REQ_STREAM,
    CODEC_ADC_EN_REQ_MC,
    CODEC_ADC_EN_REQ_DSD,

    CODEC_ADC_EN_REQ_QTY,
};

enum CODEC_IRQ_TYPE_T {
    CODEC_IRQ_TYPE_BT_TRIGGER,
    CODEC_IRQ_TYPE_BT_TRIGGER1,
    CODEC_IRQ_TYPE_BT_TRIGGER2,
    CODEC_IRQ_TYPE_BT_TRIGGER3,
    CODEC_IRQ_TYPE_VAD,
    CODEC_IRQ_TYPE_ANC_FB_CHECK,
    CODEC_IRQ_TYPE_EVENT_TRIGGER,
    CODEC_IRQ_TYPE_TIMER_TRIGGER,
    CODEC_IRQ_TYPE_WIFI_TRIGGER,
    CODEC_IRQ_TYPE_QTY,
};

enum CODEC_DAC_GAIN_RAMP_INTVL_T {
    CODEC_DAC_GAIN_RAMP_INTVL_1_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_2_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_4_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_8_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_16_SAMP,
    CODEC_DAC_GAIN_RAMP_INTVL_32_SAMP,

    CODEC_DAC_GAIN_RAMP_INTVL_QTY,
};

struct CODEC_DAC_DRE_CFG_T {
    uint8_t dre_delay;
    uint8_t thd_db_offset;
    uint8_t step_mode;
    uint32_t dre_win;
    uint16_t amp_high;
};

struct CODEC_DAC_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t dac_up;
    uint8_t bypass_cnt;
    uint8_t mc_delay;
};

static const struct CODEC_DAC_SAMPLE_RATE_T codec_dac_sample_rate[] = {
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,      CODEC_FREQ_CRYSTAL,               1, 6, 0, MC_DELAY_COMMON + 160},
    {AUD_SAMPRATE_16927,     CODEC_FREQ_CRYSTAL,               1, 3, 0, MC_DELAY_COMMON +  85},
    {AUD_SAMPRATE_50781,     CODEC_FREQ_CRYSTAL,               1, 1, 0, MC_DELAY_COMMON +  29},
#endif
    {AUD_SAMPRATE_7350,   CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 6, 0, MC_DELAY_COMMON + 174},
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 6, 0, MC_DELAY_COMMON + 168}, // T
    {AUD_SAMPRATE_14700,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 3, 0, MC_DELAY_COMMON +  71},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 3, 0, MC_DELAY_COMMON +  88}, // T
    {AUD_SAMPRATE_22050,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 2, 0, MC_DELAY_COMMON +  60},
    {AUD_SAMPRATE_24000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 2, 0, MC_DELAY_COMMON +  58},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 0, MC_DELAY_COMMON +  31}, // T
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 0, MC_DELAY_COMMON +  30}, // T
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 1, MC_DELAY_COMMON +  12},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 1, MC_DELAY_COMMON +  12}, // T
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 2, MC_DELAY_COMMON +   5},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 2, MC_DELAY_COMMON +   5},
    {AUD_SAMPRATE_352800, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 3, MC_DELAY_COMMON +   2},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 3, MC_DELAY_COMMON +   2},
};

struct CODEC_ADC_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t adc_down;
    uint8_t bypass_cnt;
};

static const struct CODEC_ADC_SAMPLE_RATE_T codec_adc_sample_rate[] = {
#ifdef __AUDIO_RESAMPLE__
    {AUD_SAMPRATE_8463,      CODEC_FREQ_CRYSTAL,               1, 6, 0},
    {AUD_SAMPRATE_16927,     CODEC_FREQ_CRYSTAL,               1, 3, 0},
    {AUD_SAMPRATE_50781,     CODEC_FREQ_CRYSTAL,               1, 1, 0},
#endif
    {AUD_SAMPRATE_7350,   CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 6, 0},
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 6, 0},
    {AUD_SAMPRATE_14700,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 3, 0},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 3, 0},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 0},
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 0},
    {AUD_SAMPRATE_88200,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 1},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 1},
    {AUD_SAMPRATE_176400, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 2},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 2},
    {AUD_SAMPRATE_352800, CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, 1, 3},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, 1, 3},
};

const CODEC_ADC_VOL_T WEAK codec_adc_vol[TGT_ADC_VOL_LEVEL_QTY] = {
    -99, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28,
};



static struct CODEC_T * const codec = (struct CODEC_T *)CODEC_BASE;

static bool codec_init = false;
static bool codec_opened = false;

static int8_t digdac_gain[MAX_DAC_CH_NUM];
static int8_t digadc_gain[MAX_ADC_CH_NUM];

static bool codec_mute[AUD_STREAM_NUM];

#ifdef AUDIO_OUTPUT_SWAP
static bool output_swap = true;
#endif

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
static bool codec_nr_enabled;
static int8_t digdac_gain_offset_nr;
#endif
#ifdef AUDIO_OUTPUT_DC_CALIB
static int32_t dac_dc_l;
static int32_t dac_dc_r;
static float dac_dc_gain_attn;
#endif
#ifdef __AUDIO_RESAMPLE__
static uint8_t rs_clk_map;
STATIC_ASSERT(sizeof(rs_clk_map) * 8 >= AUD_STREAM_NUM, "rs_clk_map size too small");

static uint32_t resample_clk_freq;
static uint8_t resample_rate_idx[AUD_STREAM_NUM];
#endif
#ifdef CODEC_TIMER
static uint32_t cur_codec_freq;
#endif

static uint8_t codec_rate_idx[AUD_STREAM_NUM];

//static HAL_CODEC_DAC_RESET_CALLBACK dac_reset_callback;

static uint8_t codec_irq_map;
STATIC_ASSERT(sizeof(codec_irq_map) * 9 >= CODEC_IRQ_TYPE_QTY, "codec_irq_map size too small");
static HAL_CODEC_IRQ_CALLBACK codec_irq_callback[CODEC_IRQ_TYPE_QTY];

static enum AUD_CHANNEL_MAP_T codec_dac_ch_map;
static enum AUD_CHANNEL_MAP_T codec_adc_ch_map;
static enum AUD_CHANNEL_MAP_T codec_mic_ch_map;

static uint8_t dac_delay_ms;

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE uint8_t codec_digmic_phase = CODEC_DIGMIC_PHASE;

#ifdef CODEC_DSD
static bool dsd_enabled;
static uint8_t dsd_rate_idx;
#endif

#if defined(CODEC_DSD)
static uint8_t adc_en_map;
STATIC_ASSERT(sizeof(adc_en_map) * 8 >= CODEC_ADC_EN_REQ_QTY, "adc_en_map size too small");
#endif

#ifdef PERF_TEST_POWER_KEY
static enum HAL_CODEC_PERF_TEST_POWER_T cur_perft_power;
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
static int8_t swdac_gain;
#define SW_OUTPUT_COEF_CALLBACK_COUNT_MAX (3)
static HAL_CODEC_SW_OUTPUT_COEF_CALLBACK sw_output_coef_callback[SW_OUTPUT_COEF_CALLBACK_COUNT_MAX] = {0};
#endif

#ifdef VOICE_DETECTOR_EN

#ifdef CODEC_VAD_CFG_BUF_SIZE
#define CODEC_VAD_BUF_SIZE    CODEC_VAD_CFG_BUF_SIZE // max 0x30000
#else
#define CODEC_VAD_BUF_SIZE    0
#endif
#define CODEC_VAD_BUF_ADDR    (CODEC_BASE - 0x80000 + CODEC_VAD_MAX_BUF_SIZE - CODEC_VAD_BUF_SIZE)
static enum AUD_CHANNEL_MAP_T vad_mic_ch;
static enum AUD_VAD_TYPE_T vad_type;
static bool vad_enabled;
static bool ana_vad_trig;
static uint8_t stream_started;
static uint8_t vad_mic_num;
static uint8_t vad_adc_down;
static uint8_t codec_adc_down;
static enum AUD_BITS_T vad_adc_bits;
static enum AUD_BITS_T codec_adc_bits;
static AUD_VAD_CALLBACK vad_handler;
static uint32_t vad_data_cnt;
static uint32_t vad_addr_cnt;
#endif

#define BT_TRIGGER_NUM                      4

static HAL_CODEC_BT_TRIGGER_CALLBACK bt_trigger_callback[BT_TRIGGER_NUM];
static HAL_CODEC_EVENT_TRIGGER_CALLBACK event_trigger_callback = NULL;
static HAL_CODEC_TIMER_TRIGGER_CALLBACK timer_trigger_callback = NULL;

#if defined(DAC_CLASSG_ENABLE)
static struct dac_classg_cfg _dac_classg_cfg = {
    .thd2 = 0xC0,
    .thd1 = 0x10,
    .thd0 = 0x10,
    .lr = 1,
    .step_4_3n = 0,
    .quick_down = 1,
    .wind_width = 0x400,
};
#endif

#ifdef DAC_DRE_ENABLE
static const struct CODEC_DAC_DRE_CFG_T dac_dre_cfg = {
    .dre_delay = 8,
    .thd_db_offset = 0xF, //5,
    .step_mode = 0,
    .dre_win = 0x6000,
    .amp_high = 2, //0x10,
};
#endif

static void hal_codec_set_dig_adc_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain);
static void hal_codec_set_dig_dac_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain);
static void hal_codec_restore_dig_dac_gain(void);
static void hal_codec_set_dac_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val);
static int hal_codec_set_adc_down(enum AUD_CHANNEL_MAP_T map, uint32_t val);
static int hal_codec_set_adc_hbf_bypass_cnt(enum AUD_CHANNEL_MAP_T map, uint32_t cnt);
static uint32_t hal_codec_get_adc_chan(enum AUD_CHANNEL_MAP_T mic_map);
#ifdef AUDIO_OUTPUT_SW_GAIN
static void hal_codec_set_sw_gain(int32_t gain);
#endif
#ifdef __AUDIO_RESAMPLE__
static float get_playback_resample_phase(void);
static float get_capture_resample_phase(void);
static uint32_t resample_phase_float_to_value(float phase);
static float resample_phase_value_to_float(uint32_t value);
#endif

static void hal_codec_reg_update_delay(void)
{
    hal_sys_timer_delay_us(2);
}

#if defined(DAC_CLASSG_ENABLE)
void hal_codec_classg_config(const struct dac_classg_cfg *cfg)
{
    _dac_classg_cfg = *cfg;
}

static void hal_codec_classg_enable(bool en)
{
    struct dac_classg_cfg *config;

    if (en) {
        config = &_dac_classg_cfg;

        codec->REG_0B4 = SET_BITFIELD(codec->REG_0B4, CODEC_CODEC_CLASSG_THD2, config->thd2);
        codec->REG_0B4 = SET_BITFIELD(codec->REG_0B4, CODEC_CODEC_CLASSG_THD1, config->thd1);
        codec->REG_0B4 = SET_BITFIELD(codec->REG_0B4, CODEC_CODEC_CLASSG_THD0, config->thd0);

        // Make class-g set the lowest gain after several samples.
        // Class-g gain will have impact on dc.
        codec->REG_0B0 = SET_BITFIELD(codec->REG_0B0, CODEC_CODEC_CLASSG_WINDOW, 6);

        if (config->lr)
            codec->REG_0B0 |= CODEC_CODEC_CLASSG_LR;
        else
            codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_LR;

        if (config->step_4_3n)
            codec->REG_0B0 |= CODEC_CODEC_CLASSG_STEP_3_4N;
        else
            codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_STEP_3_4N;

        if (config->quick_down)
            codec->REG_0B0 |= CODEC_CODEC_CLASSG_QUICK_DOWN;
        else
            codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_QUICK_DOWN;

        codec->REG_0B0 |= CODEC_CODEC_CLASSG_EN;

        // Restore class-g window after the gain has been updated
        hal_codec_reg_update_delay();
        codec->REG_0B0 = SET_BITFIELD(codec->REG_0B0, CODEC_CODEC_CLASSG_WINDOW, config->wind_width);
    } else {
        codec->REG_0B0 &= ~CODEC_CODEC_CLASSG_QUICK_DOWN;
    }
}
#endif

void hal_codec_dac_dc_offset_enable(int32_t dc_l, int32_t dc_r)
{
    codec->REG_0E0 &= ~CODEC_CODEC_DAC_DC_UPDATE_CH0;
    hal_codec_reg_update_delay();
    codec->REG_0E8 = SET_BITFIELD(codec->REG_0E8, CODEC_CODEC_DAC_DC_CH1, dc_r);
    codec->REG_0E0 = SET_BITFIELD(codec->REG_0E0, CODEC_CODEC_DAC_DC_CH0, dc_l) | CODEC_CODEC_DAC_DC_UPDATE_CH0;
}

int hal_codec_open(enum HAL_CODEC_ID_T id)
{
    uint32_t i;

    bool first_open;

#ifdef CODEC_POWER_DOWN
    first_open = true;
#else
    first_open = !codec_init;
#endif

    analog_aud_pll_open(ANA_AUD_PLL_USER_CODEC);

    if (!codec_init) {
        for (i = 0; i < CFG_HW_AUD_INPUT_PATH_NUM; i++) {
            if (cfg_audio_input_path_cfg[i].cfg & AUD_CHANNEL_MAP_ALL & ~VALID_MIC_MAP) {
                ASSERT(false, "Invalid input path cfg: i=%d io_path=%d cfg=0x%X",
                    i, cfg_audio_input_path_cfg[i].io_path, cfg_audio_input_path_cfg[i].cfg);
            }
        }
#ifdef VOICE_DETECTOR_EN
        vad_mic_ch = hal_codec_get_input_path_cfg(AUD_INPUT_PATH_VADMIC) & AUD_CHANNEL_MAP_NORMAL_ALL;
        vad_mic_num = hal_codec_get_input_map_chan_num(vad_mic_ch);

        if (vad_mic_num == 1) {
            ASSERT((vad_mic_ch & NORMAL_MIC_MAP) && ((vad_mic_ch & (vad_mic_ch - 1)) == 0),
                "Bad vad mic ch: 0x%X", vad_mic_ch);
        } else {
            TRACE(0, "vad mic ch: %#x", vad_mic_ch);
        }
#endif
        codec_init = true;
    }

    if (first_open) {
        // Codec will be powered down first
        hal_psc_codec_enable();
    }
    hal_cmu_codec_clock_enable();
    hal_cmu_codec_reset_clear();

    codec_opened = true;

    codec->REG_060 |= CODEC_EN_CLK_DAC | CODEC_EN_CLK_ADC_MASK | CODEC_EN_CLK_ADC_ANA_MASK | CODEC_POL_ADC_ANA_MASK | CODEC_POL_DAC_OUT;
    codec->REG_064 |= CODEC_SOFT_RSTN_32K | CODEC_SOFT_RSTN_IIR | CODEC_SOFT_RSTN_RS0 | CODEC_SOFT_RSTN_RS1 |
        CODEC_SOFT_RSTN_DAC | CODEC_SOFT_RSTN_ADC_MASK | CODEC_SOFT_RSTN_ADC_ANA_MASK;
    codec->REG_000 = 0;
    codec->REG_004 = ~0UL;
    hal_codec_reg_update_delay();
    codec->REG_004 = 0;
    codec->REG_000 |= CODEC_CODEC_IF_EN;

    codec->REG_054 |= CODEC_FAULT_MUTE_DAC_ENABLE;

#ifdef AUDIO_OUTPUT_SWAP
    if (output_swap) {
        codec->REG_0A4 |= CODEC_CODEC_DAC_OUT_SWAP;
    } else {
        codec->REG_0A4 &= ~CODEC_CODEC_DAC_OUT_SWAP;
    }
#endif

    if (first_open) {
        // Enable ADC zero-crossing gain adjustment
        for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
            if (i < MAX_ANA_MIC_CH_NUM)
                *(&codec->REG_084 + i) |= CODEC_CODEC_ADC_GAIN_SEL_CH0;
            else
                *(&codec->REG_084 + i) |= CODEC_CODEC_ADC_GAIN_SEL_CH3;
        }
        codec->REG_240 |= CODEC_CODEC_ADC_GAIN_SEL_CH6;
        codec->REG_244 |= CODEC_CODEC_ADC_GAIN_SEL_CH7;

#ifdef ADC_DC_FILTER
        codec->REG_0B0 = CODEC_CODEC_ADC_UDC_CH0(4) | CODEC_CODEC_ADC_UDC_CH1(4)|
                                       CODEC_CODEC_ADC_UDC_CH2(4) | CODEC_CODEC_ADC_UDC_CH3(4)|
                                       CODEC_CODEC_ADC_UDC_CH4(4) | CODEC_CODEC_ADC_UDC_CH5(4);
#else
        codec->REG_0B0 = CODEC_CODEC_ADC_DCF_BYPASS_CH0 | CODEC_CODEC_ADC_DCF_BYPASS_CH1|
                                       CODEC_CODEC_ADC_DCF_BYPASS_CH2 | CODEC_CODEC_ADC_DCF_BYPASS_CH3|
                                       CODEC_CODEC_ADC_DCF_BYPASS_CH4 | CODEC_CODEC_ADC_DCF_BYPASS_CH5;
#endif

        // DRE ini gain and offset
        uint8_t max_gain, ini_gain, dre_offset;
        max_gain = analog_aud_get_max_dre_gain();
        if (max_gain > 0xF) {
            ini_gain = 0;
            dre_offset = max_gain - 0xF;
            max_gain = 0xF;
        } else {
            ini_gain = 0xF - max_gain;
            dre_offset = 0;
        }
        codec->REG_0C0 = CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(ini_gain);
        codec->REG_0C4 = CODEC_CODEC_DRE_GAIN_OFFSET_CH0(dre_offset);
        codec->REG_0C8 = CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(ini_gain);
        codec->REG_0CC = CODEC_CODEC_DRE_GAIN_OFFSET_CH1(dre_offset);
        codec->REG_0E0 = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH0(0);
        codec->REG_0E8 = CODEC_CODEC_DAC_ANA_GAIN_UPDATE_DELAY_CH1(0);
        codec->REG_0B8 = CODEC_CODEC_DRE_GAIN_TOP_CH0(max_gain);
        codec->REG_0BC = CODEC_CODEC_DRE_GAIN_TOP_CH1(max_gain);

#if defined(FIXED_CODEC_ADC_VOL) && defined(SINGLE_CODEC_ADC_VOL)
        const CODEC_ADC_VOL_T *adc_gain_db;

        adc_gain_db = hal_codec_get_adc_volume(CODEC_SADC_VOL);
        if (adc_gain_db) {
            hal_codec_set_dig_adc_gain(NORMAL_ADC_MAP, *adc_gain_db);
        }
#endif
        hal_codec_set_dig_adc_gain(EC_ADC_MAP, 0);

#ifdef AUDIO_OUTPUT_DC_CALIB
        hal_codec_dac_dc_offset_enable(dac_dc_l, dac_dc_r);
#elif defined(SDM_MUTE_NOISE_SUPPRESSION)
        hal_codec_dac_dc_offset_enable(1, 1);
#endif

#ifdef AUDIO_OUTPUT_SW_GAIN
        const struct CODEC_DAC_VOL_T *vol_tab_ptr;

        // Init gain settings
        vol_tab_ptr = hal_codec_get_dac_volume(0);
        if (vol_tab_ptr) {
            analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
            hal_codec_set_dig_dac_gain(VALID_DAC_MAP, DAC_DEFAULT_DIG_DBVAL);
        }
#else
#ifdef DAC_ZERO_CROSSING_GAIN
        // Enable DAC zero-crossing gain adjustment
        codec->REG_0A0 |= CODEC_CODEC_DAC_GAIN_SEL_CH0;
        codec->REG_0A4 |= CODEC_CODEC_DAC_GAIN_SEL_CH1;
#elif defined(DAC_RAMP_GAIN)
        // Enable DAC ramp gain (adjust 2^-14 on each sample)
        codec->REG_0EC = CODEC_CODEC_RAMP_STEP_CH0(1) | CODEC_CODEC_RAMP_EN_CH0 | CODEC_CODEC_RAMP_INTERVAL_CH0(0);
        codec->REG_0F0 = CODEC_CODEC_RAMP_STEP_CH1(1) | CODEC_CODEC_RAMP_EN_CH1 | CODEC_CODEC_RAMP_INTERVAL_CH1(0);
#endif
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        // Reset SDM
        hal_codec_set_dac_gain_value(VALID_DAC_MAP, 0);
        codec->REG_09C |= CODEC_CODEC_DAC_SDM_CLOSE;
#endif

#ifdef SDM_MUTE_NOISE_SUPPRESSION
        codec->REG_09C = SET_BITFIELD(codec->REG_09C, CODEC_CODEC_DAC_DITHER_GAIN, 0x10);
#endif

#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~(CODEC_CODEC_RESAMPLE_DAC_ENABLE | CODEC_CODEC_RESAMPLE_ADC_ENABLE |
            CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE | CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE);
#endif

#ifdef CODEC_DSD
        for(i = 0; i < ARRAY_SIZE(codec_adc_sample_rate); i++) {
            if(codec_adc_sample_rate[i].sample_rate == AUD_SAMPRATE_44100) {
                break;
            }
        }
        hal_codec_set_adc_down((AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5), codec_adc_sample_rate[i].adc_down);
        hal_codec_set_adc_hbf_bypass_cnt((AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5), codec_adc_sample_rate[i].bypass_cnt);
#endif

        // Mute DAC when cpu fault occurs
        hal_cmu_codec_set_fault_mask(0x2F);

#ifdef CODEC_TIMER
        // Disable sync stamp auto clear to avoid impacting codec timer capture
        codec->REG_054 &= ~CODEC_STAMP_CLR_USED;
#else
        // Enable sync stamp auto clear
        codec->REG_054 |= CODEC_STAMP_CLR_USED;
#endif
    }

    return 0;
}

int hal_codec_close(enum HAL_CODEC_ID_T id)
{
    codec->REG_000 &= ~CODEC_CODEC_IF_EN;
    codec->REG_064 &= ~(CODEC_SOFT_RSTN_32K | CODEC_SOFT_RSTN_IIR | CODEC_SOFT_RSTN_RS0 | CODEC_SOFT_RSTN_RS1 |
        CODEC_SOFT_RSTN_DAC | CODEC_SOFT_RSTN_ADC_MASK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
    codec->REG_060 &= ~(CODEC_EN_CLK_DAC | CODEC_EN_CLK_ADC_MASK | CODEC_EN_CLK_ADC_ANA_MASK | CODEC_POL_ADC_ANA_MASK);

    codec_opened = false;

#ifdef CODEC_KEEP_CLOCK
    // NEVER reset or power down CODEC registers and keep CODEC clock for vad memory used by cpu
#elif defined(CODEC_POWER_DOWN)
    hal_cmu_codec_reset_set();
    hal_cmu_codec_clock_disable();
    hal_psc_codec_disable();
#else
    // NEVER reset or power down CODEC registers, for the CODEC driver expects that last configurations
    // still exist in the next stream setup
    hal_cmu_codec_clock_disable();
#endif

    analog_aud_pll_close(ANA_AUD_PLL_USER_CODEC);

    return 0;
}

void hal_codec_crash_mute(void)
{
    if (codec_opened) {
        codec->REG_000 &= ~CODEC_CODEC_IF_EN;
    }
}

#ifdef DAC_DRE_ENABLE
static void hal_codec_dac_dre_enable(void)
{
    uint8_t max_gain;

    // Ini gain is set to max gain to avoid pop sound when starting play (most of time the beginning is silence)
    max_gain = GET_BITFIELD(codec->REG_0B8, CODEC_CODEC_DRE_GAIN_TOP_CH0);

    codec->REG_0C0 = (codec->REG_0C0 & ~(CODEC_CODEC_DRE_STEP_MODE_CH0_MASK | CODEC_CODEC_DRE_INI_ANA_GAIN_CH0_MASK |
        CODEC_CODEC_DRE_DELAY_CH0_MASK | CODEC_CODEC_DRE_AMP_HIGH_CH0_MASK)) |
        CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(dac_dre_cfg.step_mode) | CODEC_CODEC_DRE_INI_ANA_GAIN_CH0(max_gain) |
        CODEC_CODEC_DRE_DELAY_CH0(dac_dre_cfg.dre_delay) | CODEC_CODEC_DRE_AMP_HIGH_CH0(dac_dre_cfg.amp_high) | CODEC_CODEC_DRE_ENABLE_CH0;
    codec->REG_0C4 = (codec->REG_0C4 & ~(CODEC_CODEC_DRE_WINDOW_CH0_MASK | CODEC_CODEC_DRE_THD_DB_OFFSET_CH0_MASK | CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH0)) |
        CODEC_CODEC_DRE_WINDOW_CH0(dac_dre_cfg.dre_win) | CODEC_CODEC_DRE_THD_DB_OFFSET_CH0(dac_dre_cfg.thd_db_offset);

    codec->REG_0C8 = (codec->REG_0C8 & ~(CODEC_CODEC_DRE_STEP_MODE_CH1_MASK | CODEC_CODEC_DRE_INI_ANA_GAIN_CH1_MASK |
        CODEC_CODEC_DRE_DELAY_CH1_MASK | CODEC_CODEC_DRE_AMP_HIGH_CH1_MASK)) |
        CODEC_CODEC_DRE_THD_DB_OFFSET_CH1(dac_dre_cfg.step_mode) | CODEC_CODEC_DRE_INI_ANA_GAIN_CH1(max_gain) |
        CODEC_CODEC_DRE_DELAY_CH1(dac_dre_cfg.dre_delay) | CODEC_CODEC_DRE_AMP_HIGH_CH1(dac_dre_cfg.amp_high) | CODEC_CODEC_DRE_ENABLE_CH1;
    codec->REG_0CC = (codec->REG_0CC & ~(CODEC_CODEC_DRE_WINDOW_CH1_MASK | CODEC_CODEC_DRE_THD_DB_OFFSET_CH1_MASK | CODEC_CODEC_DRE_THD_DB_OFFSET_SIGN_CH1)) |
        CODEC_CODEC_DRE_WINDOW_CH1(dac_dre_cfg.dre_win) | CODEC_CODEC_DRE_THD_DB_OFFSET_CH1(dac_dre_cfg.thd_db_offset);
}

static void hal_codec_dac_dre_disable(void)
{
    codec->REG_0C0 &= ~CODEC_CODEC_DRE_ENABLE_CH0;
    codec->REG_0C8 &= ~CODEC_CODEC_DRE_ENABLE_CH1;
}
#endif

#ifdef PERF_TEST_POWER_KEY
static void hal_codec_update_perf_test_power(void)
{
    int32_t nominal_vol;
    uint32_t ini_ana_gain;
    int32_t dac_vol;

    if (!codec_opened) {
        return;
    }

    dac_vol = 0;
    if (cur_perft_power == HAL_CODEC_PERF_TEST_30MW) {
        nominal_vol = 0;
        ini_ana_gain = 0;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_10MW) {
        nominal_vol = -5;
        ini_ana_gain = 6;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_5MW) {
        nominal_vol = -8;
        ini_ana_gain = 0xA;
    } else if (cur_perft_power == HAL_CODEC_PERF_TEST_M60DB) {
        nominal_vol = -60;
        ini_ana_gain = 0xF; // about -11 dB
        dac_vol = -49;
    } else {
        return;
    }

    if (codec->REG_0C0 & CODEC_CODEC_DRE_ENABLE_CH0) {
        dac_vol = nominal_vol;
    } else {
        codec->REG_0C0 = SET_BITFIELD(codec->REG_0C0, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0, ini_ana_gain);
        codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_DRE_INI_ANA_GAIN_CH1, ini_ana_gain);
    }

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(dac_vol);
#else
    hal_codec_set_dig_dac_gain(VALID_DAC_MAP, dac_vol);
#endif

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    if (codec_nr_enabled) {
        codec_nr_enabled = false;
        hal_codec_set_noise_reduction(true);
    }
#endif
}

void hal_codec_dac_gain_m60db_check(enum HAL_CODEC_PERF_TEST_POWER_T type)
{
    cur_perft_power = type;

    if (!codec_opened || (codec->REG_09C & CODEC_CODEC_DAC_EN) == 0) {
        return;
    }

    hal_codec_update_perf_test_power();
}
#endif

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
void hal_codec_set_noise_reduction(bool enable)
{
    uint32_t ini_ana_gain;

    if (codec_nr_enabled == enable) {
        // Avoid corrupting digdac_gain_offset_nr or using an invalid one
        return;
    }

    codec_nr_enabled = enable;

    if (!codec_opened) {
        return;
    }

    // ini_ana_gain=0   -->   0dB
    // ini_ana_gain=0xF --> -11dB
    if (enable) {
        ini_ana_gain = GET_BITFIELD(codec->REG_0C0, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0);
        digdac_gain_offset_nr = ((0xF - ini_ana_gain) * 11 + 0xF / 2) / 0xF;
        ini_ana_gain = 0xF;
    } else {
        ini_ana_gain = 0xF - (digdac_gain_offset_nr * 0xF + 11 / 2) / 11;
        digdac_gain_offset_nr = 0;
    }

    codec->REG_0C0 = SET_BITFIELD(codec->REG_0C0, CODEC_CODEC_DRE_INI_ANA_GAIN_CH0, ini_ana_gain);
    codec->REG_0C8 = SET_BITFIELD(codec->REG_0C8, CODEC_CODEC_DRE_INI_ANA_GAIN_CH1, ini_ana_gain);

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(swdac_gain);
#else
    hal_codec_restore_dig_dac_gain();
#endif
}
#endif

void hal_codec_stop_playback_stream(enum HAL_CODEC_ID_T id)
{
#ifndef CLOSE_PA_DAC_NOT_IN_STOP
#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!(defined(__TWS__) || defined(IBRT)))
    // Disable PA
    analog_aud_codec_speaker_enable(false);
#endif
#endif
    codec->REG_09C &= ~(CODEC_CODEC_DAC_EN | CODEC_CODEC_DAC_EN_CH0 | CODEC_CODEC_DAC_EN_CH1);

#ifdef CODEC_TIMER
    // Reset codec timer
    codec->REG_054 &= ~CODEC_EVENT_FOR_CAPTURE;
#endif

#ifdef DAC_DRE_ENABLE
    hal_codec_dac_dre_disable();
#endif

#if defined(DAC_CLASSG_ENABLE)
    hal_codec_classg_enable(false);
#endif

#ifndef NO_DAC_RESET
    // Reset DAC
    // Avoid DAC outputing noise after it is disabled
    codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC;
    codec->REG_064 |= CODEC_SOFT_RSTN_DAC;
#endif
}

void hal_codec_start_playback_stream(enum HAL_CODEC_ID_T id)
{
#ifndef NO_DAC_RESET
    // Reset DAC
    codec->REG_064 &= ~CODEC_SOFT_RSTN_DAC;
    codec->REG_064 |= CODEC_SOFT_RSTN_DAC;
#endif

#ifdef DAC_DRE_ENABLE
    if (
            //(codec->REG_044 & CODEC_MODE_16BIT_DAC) == 0 &&
            1
            )
    {
        hal_codec_dac_dre_enable();
    }
#endif

#ifdef PERF_TEST_POWER_KEY
    hal_codec_update_perf_test_power();
#endif

#if defined(DAC_CLASSG_ENABLE)
    hal_codec_classg_enable(true);
#endif

#ifdef CODEC_TIMER
    // Enable codec timer and record time by bt event instead of gpio event
    codec->REG_054 = (codec->REG_054 & ~CODEC_EVENT_SEL) | CODEC_EVENT_FOR_CAPTURE;
#endif

    if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_09C |= CODEC_CODEC_DAC_EN_CH0;
    } else {
        codec->REG_09C &= ~CODEC_CODEC_DAC_EN_CH0;
    }
    if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_09C |= CODEC_CODEC_DAC_EN_CH1;
    } else {
        codec->REG_09C &= ~CODEC_CODEC_DAC_EN_CH1;
    }

#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!(defined(__TWS__) || defined(IBRT)))
#if 0
    uint32_t cfg_en;
    uint32_t anc_ff_gain, anc_fb_gain;

    cfg_en = codec->REG_000 & CODEC_DAC_ENABLE;
    anc_ff_gain = codec->REG_0D4;
    anc_fb_gain = codec->REG_0D8;

    if (cfg_en) {
        codec->REG_000 &= ~cfg_en;
    }
    if (anc_ff_gain) {
        codec->REG_0D4 = CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1;
        anc_ff_gain |= CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FF_CH1;
    }
    if (anc_fb_gain) {
        codec->REG_0D8 = CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1;
        anc_fb_gain = CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH0 | CODEC_CODEC_ANC_MUTE_GAIN_PASS0_FB_CH1;
    }
    osDelay(1);
#endif
#endif

    codec->REG_09C |= CODEC_CODEC_DAC_EN;

#if (defined(AUDIO_OUTPUT_DC_CALIB_ANA) || defined(AUDIO_OUTPUT_DC_CALIB)) && (!(defined(__TWS__) || defined(IBRT)))
#ifdef AUDIO_OUTPUT_DC_CALIB
    // At least delay 4ms for 8K-sample-rate mute data to arrive at DAC PA
    osDelay(5);
#endif

#if 0
    if (cfg_en) {
        codec->REG_000 |= cfg_en;
    }
    if (anc_ff_gain) {
        codec->REG_0D4 = anc_ff_gain;
    }
    if (anc_fb_gain) {
        codec->REG_0D8 = anc_fb_gain;
    }
#endif

    // Enable PA
    analog_aud_codec_speaker_enable(true);
#endif
}

int hal_codec_start_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
#ifdef VOICE_DETECTOR_EN
    if (vad_type == AUD_VAD_TYPE_MIX && vad_enabled && !ana_vad_trig) {
        ASSERT(false, "VAD mix mode must use codec exclusively before ana vad trig");
    }
    stream_started |= (1 << stream);
#endif

    if (stream == AUD_STREAM_PLAYBACK) {
        // Reset and start DAC
        hal_codec_start_playback_stream(id);
    } else {
#if defined(CODEC_DSD)
        adc_en_map |= (1 << CODEC_ADC_EN_REQ_STREAM);
        if (adc_en_map == (1 << CODEC_ADC_EN_REQ_STREAM))
#endif
        {
            // Reset ADC free running clock and ADC ANA
            codec->REG_064 &= ~(RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
            codec->REG_064 |= (RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
            codec->REG_080 |= CODEC_CODEC_ADC_EN;
        }
    }

    return 0;
}

int hal_codec_stop_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
#ifdef VOICE_DETECTOR_EN
    stream_started &= ~(1 << stream);
#endif

    if (stream == AUD_STREAM_PLAYBACK) {
        // Stop and reset DAC
        hal_codec_stop_playback_stream(id);
    } else {
#if defined(CODEC_DSD)
        adc_en_map &= ~(1 << CODEC_ADC_EN_REQ_STREAM);
        if (adc_en_map == 0)
#endif
        {
            codec->REG_080 &= ~CODEC_CODEC_ADC_EN;
        }
    }

    return 0;
}

#ifdef CODEC_DSD
void hal_codec_dsd_enable(void)
{
    dsd_enabled = true;
}

void hal_codec_dsd_disable(void)
{
    dsd_enabled = false;
}

static void hal_codec_dsd_cfg_start(void)
{
#if !(defined(FIXED_CODEC_ADC_VOL) && defined(SINGLE_CODEC_ADC_VOL))
    uint32_t vol;
    const CODEC_ADC_VOL_T *adc_gain_db;

    vol = hal_codec_get_mic_chan_volume_level(AUD_CHANNEL_MAP_DIGMIC_CH4);
    adc_gain_db = hal_codec_get_adc_volume(vol);
    if (adc_gain_db) {
        hal_codec_set_dig_adc_gain((AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5), *adc_gain_db);
    }
#endif

    codec->REG_004 |= CODEC_DSD_RX_FIFO_FLUSH | CODEC_DSD_TX_FIFO_FLUSH;
    hal_codec_reg_update_delay();
    codec->REG_004 &= ~(CODEC_DSD_RX_FIFO_FLUSH | CODEC_DSD_TX_FIFO_FLUSH);

    codec->REG_0B8 = CODEC_CODEC_DSD_ENABLE_L | CODEC_CODEC_DSD_ENABLE_R | CODEC_CODEC_DSD_SAMPLE_RATE(dsd_rate_idx);
    codec->REG_048 = CODEC_DSD_IF_EN | CODEC_DSD_ENABLE | CODEC_DSD_DUAL_CHANNEL | CODEC_MODE_24BIT_DSD |
        /* CODEC_DMA_CTRL_RX_DSD | */ CODEC_DMA_CTRL_TX_DSD | CODEC_DSD_IN_16BIT;

    codec->REG_080 = (codec->REG_080 & ~(CODEC_CODEC_LOOP_SEL_L_MASK | CODEC_CODEC_LOOP_SEL_R_MASK)) |
        CODEC_CODEC_ADC_LOOP | CODEC_CODEC_LOOP_SEL_L(4) | CODEC_CODEC_LOOP_SEL_R(5);

    codec->REG_0AC = SET_BITFIELD(codec->REG_0AC, CODEC_CODEC_PDM_MUX_CH4, 4);
    codec->REG_0AC = SET_BITFIELD(codec->REG_0AC, CODEC_CODEC_PDM_MUX_CH5, 5);

    codec->REG_064 &= ~(CODEC_SOFT_RSTN_ADC(1 << 4) | CODEC_SOFT_RSTN_ADC(1 << 5));
    codec->REG_064 |= CODEC_SOFT_RSTN_ADC(1 << 4) | CODEC_SOFT_RSTN_ADC(1 << 5);
    codec->REG_080 |= CODEC_CODEC_ADC_EN_CH4 | CODEC_CODEC_ADC_EN_CH5;

    if (adc_en_map == 0) {
        // Reset ADC free running clock and ADC ANA
        codec->REG_064 &= ~(RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
        codec->REG_064 |= (RSTN_ADC_FREE_RUNNING_CLK | CODEC_SOFT_RSTN_ADC_ANA_MASK);
        codec->REG_080 |= CODEC_CODEC_ADC_EN;
    }
    adc_en_map |= (1 << CODEC_ADC_EN_REQ_DSD);
}

static void hal_codec_dsd_cfg_stop(void)
{
    adc_en_map &= ~(1 << CODEC_ADC_EN_REQ_DSD);
    if (adc_en_map == 0) {
        codec->REG_080 &= ~CODEC_CODEC_ADC_EN;
    }

    codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH4 | CODEC_CODEC_ADC_EN_CH5);
    codec->REG_048 = 0;
    codec->REG_0B8 = 0;

    codec->REG_080 &= ~CODEC_CODEC_ADC_LOOP;
}
#endif

#ifdef __AUDIO_RESAMPLE__
void hal_codec_resample_clock_enable(enum AUD_STREAM_T stream)
{
    uint32_t clk;
    bool set = false;

    // 192K-24BIT requires 52M clock, and 384K-24BIT requires 104M clock
    if (stream == AUD_STREAM_PLAYBACK) {
        clk = codec_dac_sample_rate[resample_rate_idx[AUD_STREAM_PLAYBACK]].sample_rate * RS_CLOCK_FACTOR;
    } else {
        clk = codec_adc_sample_rate[resample_rate_idx[AUD_STREAM_CAPTURE]].sample_rate * RS_CLOCK_FACTOR;
    }

    if (rs_clk_map == 0) {
        set = true;
    } else {
        if (resample_clk_freq < clk) {
            set = true;
        }
    }

    if (set) {
        resample_clk_freq = clk;
        hal_cmu_codec_rs_enable(clk);
    }

    rs_clk_map |= (1 << stream);
}

void hal_codec_resample_clock_disable(enum AUD_STREAM_T stream)
{
    if (rs_clk_map == 0) {
        return;
    }
    rs_clk_map &= ~(1 << stream);
    if (rs_clk_map == 0) {
        hal_cmu_codec_rs_disable();
    }
}
#endif

static void hal_codec_enable_dig_mic(enum AUD_CHANNEL_MAP_T mic_map)
{
    uint32_t phase = 0;
    uint32_t line_map = 0;

    phase = codec->REG_0AC;
    for (uint32_t i = 0; i < MAX_DIG_MIC_CH_NUM; i++) {
        if (mic_map & (AUD_CHANNEL_MAP_DIGMIC_CH0 << i)) {
            line_map |= (1 << (i / 2));
        }
        phase = (phase & ~(CODEC_CODEC_PDM_CAP_PHASE_CH0_MASK << (i * 2))) |
            (CODEC_CODEC_PDM_CAP_PHASE_CH0(codec_digmic_phase) << (i * 2));
    }
    codec->REG_0AC = phase;
    codec->REG_0A8 |= CODEC_CODEC_PDM_ENABLE;
    hal_iomux_set_dig_mic(line_map);
}

static void hal_codec_disable_dig_mic(void)
{
    codec->REG_0A8 &= ~CODEC_CODEC_PDM_ENABLE;
}

static void hal_codec_ec_enable(void)
{
    if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH6) {
        codec->REG_240 |= CODEC_CODEC_MC_ENABLE_CH0;
    }
    if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH7) {
        codec->REG_244 |= CODEC_CODEC_MC_ENABLE_CH1;
    }
    //ec resample has been setup at hal_codec_setup_stream
    //CODEC_CODEC_RESAMPLE_ADC_CH6_SEL, CODEC_CODEC_RESAMPLE_ADC_CH7_SEL
}

static void hal_codec_ec_disable(void)
{
    codec->REG_240 &= ~CODEC_CODEC_MC_ENABLE_CH0;
    codec->REG_244 &= ~CODEC_CODEC_MC_ENABLE_CH1;

}

int hal_codec_start_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, int dma)
{
    uint32_t fifo_flush = 0;

    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef CODEC_DSD
        if (dsd_enabled) {
            hal_codec_dsd_cfg_start();
        }
#endif
#ifdef __AUDIO_RESAMPLE__
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) {
            hal_codec_resample_clock_enable(stream);
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_ENABLE;
        }
#endif
#ifdef DAC_RAMP_GAIN
        if (codec->REG_0EC & CODEC_CODEC_RAMP_EN_CH0) {
            codec->REG_0EC &= ~CODEC_CODEC_RAMP_EN_CH0;
            codec->REG_0F0 &= ~CODEC_CODEC_RAMP_EN_CH1;
            hal_codec_reg_update_delay();
            hal_codec_set_dac_gain_value(VALID_DAC_MAP, 0);
            hal_codec_reg_update_delay();
            codec->REG_0EC |= CODEC_CODEC_RAMP_EN_CH0;
            codec->REG_0F0 |= CODEC_CODEC_RAMP_EN_CH1;
            hal_codec_reg_update_delay();
            hal_codec_restore_dig_dac_gain();
        }
#endif
        fifo_flush |= CODEC_TX_FIFO_FLUSH;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        if (dma) {
            codec->REG_008 = SET_BITFIELD(codec->REG_008, CODEC_CODEC_TX_THRESHOLD, HAL_CODEC_TX_FIFO_TRIGGER_LEVEL);
            codec->REG_000 |= CODEC_DMACTRL_TX;
            // Delay a little time for DMA to fill the TX FIFO before sending
            for (volatile uint32_t i = 0; i < 50; i++);
        }
        codec->REG_000 |= CODEC_DAC_ENABLE;
    } else {
#ifdef VOICE_DETECTOR_EN
        if (vad_type == AUD_VAD_TYPE_MIX || vad_type == AUD_VAD_TYPE_DIG) {
            ASSERT(vad_adc_bits == codec_adc_bits, "%s: Cap bits conflict: vad=%d codec=%d", __func__, vad_adc_bits, codec_adc_bits);
            ASSERT(vad_adc_down == codec_adc_down, "%s: Adc down conflict: vad=%d codec=%d", __func__, vad_adc_down, codec_adc_down);
            // Stop vad buffering
            hal_codec_vad_stop();
        }
#endif
#ifdef __AUDIO_RESAMPLE__
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) {
            hal_codec_resample_clock_enable(stream);
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_ENABLE;
        }
#endif
        if (codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL) {
            hal_codec_enable_dig_mic(codec_mic_ch_map);
        }
        if (codec_adc_ch_map & EC_ADC_MAP) {
            hal_codec_ec_enable();
        }
        int adc_cnt = 0;
        for (uint32_t i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                if ((codec->REG_080 & (CODEC_CODEC_ADC_EN_CH0 << i)) == 0) {
                    // Reset ADC channel
                    codec->REG_064 &= ~CODEC_SOFT_RSTN_ADC(1 << i);
                    codec->REG_064 |= CODEC_SOFT_RSTN_ADC(1 << i);
                    codec->REG_080 |= (CODEC_CODEC_ADC_EN_CH0 << i);
                }
                codec->REG_000 |= (CODEC_ADC_ENABLE_CH0 << adc_cnt);
                adc_cnt++;
            }
        }
        fifo_flush = CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1 | CODEC_RX_FIFO_FLUSH_CH2 |
            CODEC_RX_FIFO_FLUSH_CH3 | CODEC_RX_FIFO_FLUSH_CH4 | CODEC_RX_FIFO_FLUSH_CH5 |
            CODEC_RX_FIFO_FLUSH_CH6 | CODEC_RX_FIFO_FLUSH_CH7;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        if (dma) {
            codec->REG_008 = SET_BITFIELD(codec->REG_008, CODEC_CODEC_RX_THRESHOLD, HAL_CODEC_RX_FIFO_TRIGGER_LEVEL);
            codec->REG_000 |= CODEC_DMACTRL_RX;
        }
        codec->REG_000 |= CODEC_ADC_ENABLE;
    }

    return 0;
}

int hal_codec_stop_interface(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t fifo_flush = 0;

    if (stream == AUD_STREAM_PLAYBACK) {
        codec->REG_000 &= ~CODEC_DAC_ENABLE;
        codec->REG_000 &= ~CODEC_DMACTRL_TX;
#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_ENABLE;
        hal_codec_resample_clock_disable(stream);
#endif
#ifdef CODEC_DSD
        hal_codec_dsd_cfg_stop();
        dsd_enabled = false;
#endif
        fifo_flush |= CODEC_TX_FIFO_FLUSH;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        // Cancel dac sync request
        hal_codec_sync_dac_disable();
        hal_codec_sync_dac_resample_rate_disable();
        hal_codec_sync_dac_gain_disable();
#ifdef NO_DAC_RESET
        // Clean up DAC intermediate states
        osDelay(dac_delay_ms);
#endif
    } else {
        codec->REG_000 &= ~(CODEC_ADC_ENABLE | CODEC_ADC_ENABLE_CH0 | CODEC_ADC_ENABLE_CH1 | CODEC_ADC_ENABLE_CH2 |
            CODEC_ADC_ENABLE_CH3 | CODEC_ADC_ENABLE_CH4 | CODEC_ADC_ENABLE_CH5 |
            CODEC_ADC_ENABLE_CH6 | CODEC_ADC_ENABLE_CH7);
        codec->REG_000 &= ~CODEC_DMACTRL_RX;
        for (uint32_t i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                codec->REG_080 &= ~(CODEC_CODEC_ADC_EN_CH0 << i);
            }
        }
        if (codec_adc_ch_map & EC_ADC_MAP) {
            hal_codec_ec_disable();
        }
        if ((codec_mic_ch_map & AUD_CHANNEL_MAP_DIGMIC_ALL)) {
            hal_codec_disable_dig_mic();
        }
#ifdef __AUDIO_RESAMPLE__
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_ENABLE;
        hal_codec_resample_clock_disable(stream);
#endif
        fifo_flush = CODEC_RX_FIFO_FLUSH_CH0 | CODEC_RX_FIFO_FLUSH_CH1 | CODEC_RX_FIFO_FLUSH_CH2 |
            CODEC_RX_FIFO_FLUSH_CH3 | CODEC_RX_FIFO_FLUSH_CH4 | CODEC_RX_FIFO_FLUSH_CH5 |
            CODEC_RX_FIFO_FLUSH_CH6 | CODEC_RX_FIFO_FLUSH_CH7;
        codec->REG_004 |= fifo_flush;
        hal_codec_reg_update_delay();
        codec->REG_004 &= ~fifo_flush;
        // Cancel adc sync request
        hal_codec_sync_adc_disable();
        hal_codec_sync_adc_resample_rate_disable();
        hal_codec_sync_adc_gain_disable();
    }

    return 0;
}

//for stress tests, silence dac at night
static uint32_t codec_dac_silence, codec_adc_silence;
void codec_set_dac_silence_mode(uint32_t enable)
{
    codec_dac_silence = enable;
}
static void hal_codec_set_adc_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val);
static void hal_codec_restore_dig_adc_gain(void);
void codec_set_adc_silence_mode(uint32_t enable)
{
    codec_adc_silence = enable;

    if (enable) {
        hal_codec_set_adc_gain_value(NORMAL_ADC_MAP, 0);
    } else {
        hal_codec_restore_dig_adc_gain();
    }
}

static void hal_codec_set_dac_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    codec->REG_0A0 &= ~CODEC_CODEC_DAC_GAIN_UPDATE;
    hal_codec_reg_update_delay();
    if (map & AUD_CHANNEL_MAP_CH0) {
        codec->REG_0A0 = SET_BITFIELD(codec->REG_0A0, CODEC_CODEC_DAC_GAIN_CH0, val);
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        codec->REG_0A4 = SET_BITFIELD(codec->REG_0A4, CODEC_CODEC_DAC_GAIN_CH1, val);
    }
    codec->REG_0A0 |= CODEC_CODEC_DAC_GAIN_UPDATE;
}

void hal_codec_get_dac_gain(float *left_gain,float *right_gain)
{
    struct DAC_GAIN_T {
        int32_t v : 20;
    };

    struct DAC_GAIN_T left;
    struct DAC_GAIN_T right;

    left.v  = GET_BITFIELD(codec->REG_0A0, CODEC_CODEC_DAC_GAIN_CH0);
    right.v = GET_BITFIELD(codec->REG_0A4, CODEC_CODEC_DAC_GAIN_CH1);

    if (left_gain) {
        *left_gain = left.v;
        // Gain format: 6.14
        *left_gain /= (1 << 14);
    }
    if (right_gain) {
        *right_gain = right.v;
        // Gain format: 6.14
        *right_gain /= (1 << 14);
    }
}

void hal_codec_dac_mute(bool mute)
{
    codec_mute[AUD_STREAM_PLAYBACK] = mute;

#ifdef AUDIO_OUTPUT_SW_GAIN
    hal_codec_set_sw_gain(swdac_gain);
#else
    if (mute) {
        hal_codec_set_dac_gain_value(VALID_DAC_MAP, 0);
    } else {
        hal_codec_restore_dig_dac_gain();
    }
#endif
}

static float db_to_amplitude_ratio(int32_t db)
{
    float coef;

    if (db == ZERODB_DIG_DBVAL) {
        coef = 1;
    } else if (db <= MIN_DIG_DBVAL) {
        coef = 0;
    } else {
        if (db > MAX_DIG_DBVAL) {
            db = MAX_DIG_DBVAL;
        }
        coef = db_to_float(db);
    }

    return coef;
}

static float digdac_gain_to_float(int32_t gain)
{
    float coef;

#if defined(NOISE_GATING) && defined(NOISE_REDUCTION)
    gain += digdac_gain_offset_nr;
#endif

    coef = db_to_amplitude_ratio(gain);

#ifdef AUDIO_OUTPUT_DC_CALIB
    coef *= dac_dc_gain_attn;
#endif

#if 0
    static const float thd_attn = 0.982878873; // -0.15dB

    // Ensure that THD is good at max gain
    if (coef > thd_attn) {
        coef = thd_attn;
    }
#endif

    return coef;
}

static void hal_codec_set_dig_dac_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;
    bool mute;

    if (map & AUD_CHANNEL_MAP_CH0) {
        digdac_gain[0] = gain;
    }
    if (map & AUD_CHANNEL_MAP_CH1) {
        digdac_gain[1] = gain;
    }

#ifdef AUDIO_OUTPUT_SW_GAIN
    mute = false;
#else
    mute = codec_mute[AUD_STREAM_PLAYBACK];
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    if (codec->REG_09C & CODEC_CODEC_DAC_SDM_CLOSE) {
        mute = true;
    }
#endif

    if (mute) {
        val = 0;
    } else {
        coef = digdac_gain_to_float(gain);

        // Gain format: 6.14
        int32_t s_val = (int32_t)(coef * (1 << 14));
        val = __SSAT(s_val, 20);
    }

    hal_codec_set_dac_gain_value(map, val);
}

static void hal_codec_restore_dig_dac_gain(void)
{
    if (digdac_gain[0] == digdac_gain[1]) {
        hal_codec_set_dig_dac_gain(VALID_DAC_MAP, digdac_gain[0]);
    } else {
        hal_codec_set_dig_dac_gain(AUD_CHANNEL_MAP_CH0, digdac_gain[0]);
        hal_codec_set_dig_dac_gain(AUD_CHANNEL_MAP_CH1, digdac_gain[1]);
    }
}

#ifdef AUDIO_OUTPUT_SW_GAIN
static void hal_codec_set_sw_gain(int32_t gain)
{
    float coef;
    bool mute;

    swdac_gain = gain;

    mute = codec_mute[AUD_STREAM_PLAYBACK];

    if (mute) {
        coef = 0;
    } else {
        coef = digdac_gain_to_float(gain);
    }

    for (uint32_t i = 0; i < SW_OUTPUT_COEF_CALLBACK_COUNT_MAX; i++) {
        if (sw_output_coef_callback[i]) {
            sw_output_coef_callback[i](coef);
        }
    }
}

void hal_codec_set_sw_output_coef_callback(HAL_CODEC_SW_OUTPUT_COEF_CALLBACK callback)
{
    uint32_t i = 0;
    for (i = 0; i < SW_OUTPUT_COEF_CALLBACK_COUNT_MAX; i++) {
        if (sw_output_coef_callback[i] == NULL) {
            sw_output_coef_callback[i] = callback;
            break;
        }
    }

    if (i == SW_OUTPUT_COEF_CALLBACK_COUNT_MAX) {
        TRACE(0, "%s error. SW_OUTPUT_COEF_CALLBACK_COUNT_MAX = %d", __func__, SW_OUTPUT_COEF_CALLBACK_COUNT_MAX);
    }
}
#endif

static void hal_codec_set_adc_gain_value(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    uint32_t gain_update = 0;

    for (uint32_t i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (i < MAX_ANA_MIC_CH_NUM)
                *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_GAIN_CH0, val);
            else
                *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_GAIN_CH3, val);
            gain_update |= (CODEC_CODEC_ADC_GAIN_UPDATE_CH0 << i);
        }
    }

    if (map & AUD_CHANNEL_MAP_CH6) {
        codec->REG_240 = SET_BITFIELD(codec->REG_240, CODEC_CODEC_ADC_GAIN_CH6, val);
        gain_update |= CODEC_CODEC_ADC_GAIN_UPDATE_CH6;
    }
    if (map & AUD_CHANNEL_MAP_CH7) {
        codec->REG_244 = SET_BITFIELD(codec->REG_244, CODEC_CODEC_ADC_GAIN_CH7, val);
        gain_update |= CODEC_CODEC_ADC_GAIN_UPDATE_CH7;
    }

    codec->REG_0A0 &= ~gain_update;
    hal_codec_reg_update_delay();
    codec->REG_0A0 |= gain_update;
}

static void hal_codec_set_dig_adc_gain(enum AUD_CHANNEL_MAP_T map, int32_t gain)
{
    uint32_t val;
    float coef;
    bool mute;
    uint32_t i;

    for (i = 0; i < MAX_ADC_CH_NUM; i++) {
        if (map & (1 << i)) {
            digadc_gain[i] = gain;
        }
    }

    mute = codec_mute[AUD_STREAM_CAPTURE];

    if (mute) {
        val = 0;
    } else {
        coef = db_to_amplitude_ratio(gain);

        // Gain format: 8.12
        int32_t s_val = (int32_t)(coef * (1 << 12));
        val = __SSAT(s_val, 20);
    }

    hal_codec_set_adc_gain_value(map, val);
}

static void hal_codec_restore_dig_adc_gain(void)
{
    uint32_t i;

    //normal and ec adc ch
    for (i = 0; i < MAX_ADC_CH_NUM; i++) {
        hal_codec_set_dig_adc_gain((1 << i), digadc_gain[i]);
    }
}

void hal_codec_adc_mute(bool mute)
{
    codec_mute[AUD_STREAM_CAPTURE] = mute;

    if (mute) {
        hal_codec_set_adc_gain_value(VALID_ADC_MAP, 0);
    } else {
        hal_codec_restore_dig_adc_gain();
    }
}

int hal_codec_set_chan_vol(enum AUD_STREAM_T stream, enum AUD_CHANNEL_MAP_T ch_map, uint8_t vol)
{
    if (stream == AUD_STREAM_PLAYBACK) {
#ifdef AUDIO_OUTPUT_SW_GAIN
        ASSERT(false, "%s: Cannot set play chan vol with AUDIO_OUTPUT_SW_GAIN", __func__);
#else
#ifdef SINGLE_CODEC_DAC_VOL
        ASSERT(false, "%s: Cannot set play chan vol with SINGLE_CODEC_DAC_VOL", __func__);
#else
        const struct CODEC_DAC_VOL_T *vol_tab_ptr;

        vol_tab_ptr = hal_codec_get_dac_volume(vol);
        if (vol_tab_ptr) {
            if (ch_map & AUD_CHANNEL_MAP_CH0) {
                hal_codec_set_dig_dac_gain(AUD_CHANNEL_MAP_CH0, vol_tab_ptr->sdac_volume);
            }
            if (ch_map & AUD_CHANNEL_MAP_CH1) {
                hal_codec_set_dig_dac_gain(AUD_CHANNEL_MAP_CH1, vol_tab_ptr->sdac_volume);
            }
        }
#endif
#endif
    } else {
#ifdef SINGLE_CODEC_ADC_VOL
        ASSERT(false, "%s: Cannot set cap chan vol with SINGLE_CODEC_ADC_VOL", __func__);
#else
        uint8_t mic_ch, adc_ch;
        enum AUD_CHANNEL_MAP_T map;
        const CODEC_ADC_VOL_T *adc_gain_db;

        adc_gain_db = hal_codec_get_adc_volume(vol);
        if (adc_gain_db) {
            map = ch_map & ~EC_MIC_MAP; //gain for EC ch is 0db
            while (map) {
                mic_ch = get_lsb_pos(map);
                map &= ~(1 << mic_ch);
                adc_ch = hal_codec_get_adc_chan(1 << mic_ch);
                ASSERT(adc_ch < NORMAL_ADC_CH_NUM, "%s: Bad cap ch_map=0x%X (ch=%u)", __func__, ch_map, mic_ch);
                hal_codec_set_dig_adc_gain((1 << adc_ch), *adc_gain_db);
            }
        }
#endif
    }

    return 0;
}

static int hal_codec_set_dac_hbf_bypass_cnt(uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_DAC_HBF1_BYPASS | CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_DAC_HBF3_BYPASS;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_DAC_HBF1_BYPASS | CODEC_CODEC_DAC_HBF2_BYPASS | CODEC_CODEC_DAC_HBF3_BYPASS;
    } else {
        ASSERT(false, "%s: Invalid dac bypass cnt: %u", __FUNCTION__, cnt);
    }

    // OSR is fixed to 512
    //codec->REG_09C = SET_BITFIELD(codec->REG_09C, CODEC_CODEC_DAC_OSR_SEL, 2);

    codec->REG_09C = (codec->REG_09C & ~bypass_mask) | bypass;
    return 0;
}

static int hal_codec_set_dac_up(uint32_t val)
{
    uint32_t sel = 0;

    if (val == 2) {
        sel = 0;
    } else if (val == 3) {
        sel = 1;
    } else if (val == 4) {
        sel = 2;
    } else if (val == 6) {
        sel = 3;
    } else if (val == 1) {
        sel = 4;
    } else {
        ASSERT(false, "%s: Invalid dac up: %u", __FUNCTION__, val);
    }
    codec->REG_09C = SET_BITFIELD(codec->REG_09C, CODEC_CODEC_DAC_UP_SEL, sel);
    return 0;
}

static uint32_t POSSIBLY_UNUSED hal_codec_get_dac_up(void)
{
    uint32_t sel;

    sel = GET_BITFIELD(codec->REG_09C, CODEC_CODEC_DAC_UP_SEL);
    if (sel == 0) {
        return 2;
    } else if (sel == 1) {
        return 3;
    } else if (sel == 2) {
        return 4;
    } else if (sel == 3) {
        return 6;
    } else {
        return 1;
    }
}

static int hal_codec_set_adc_down(enum AUD_CHANNEL_MAP_T map, uint32_t val)
{
    uint32_t sel = 0;

    if (val == 3) {
        sel = 0;
    } else if (val == 6) {
        sel = 1;
    } else if (val == 1) {
        sel = 2;
    } else {
        ASSERT(false, "%s: Invalid adc down: %u", __FUNCTION__, val);
    }
    for (uint32_t i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (i < MAX_ANA_MIC_CH_NUM)
                *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_DOWN_SEL_CH0, sel);
            else
                *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_DOWN_SEL_CH3, sel);
        }
    }

    if (map & AUD_CHANNEL_MAP_CH6) {
        codec->REG_240 = SET_BITFIELD(codec->REG_240, CODEC_CODEC_ADC_DOWN_SEL_CH6, sel);
    }
    if (map & AUD_CHANNEL_MAP_CH7) {
        codec->REG_244 = SET_BITFIELD(codec->REG_244, CODEC_CODEC_ADC_DOWN_SEL_CH7, sel);
    }
    return 0;
}

static int hal_codec_set_adc_hbf_bypass_cnt(enum AUD_CHANNEL_MAP_T map, uint32_t cnt)
{
    uint32_t bypass = 0;
    uint32_t bypass_mask = CODEC_CODEC_ADC_HBF1_BYPASS_CH0 | CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_ADC_HBF1_BYPASS_CH0 | CODEC_CODEC_ADC_HBF2_BYPASS_CH0 | CODEC_CODEC_ADC_HBF3_BYPASS_CH0;
    } else {
        ASSERT(false, "%s: Invalid bypass cnt: %u", __FUNCTION__, cnt);
    }
    for (uint32_t i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (i > MAX_ANA_MIC_CH_NUM)
                bypass_mask <<= 4;
            *(&codec->REG_084 + i) = (*(&codec->REG_084 + i) & ~bypass_mask) | bypass;
        }
    }

    bypass_mask = CODEC_CODEC_ADC_HBF1_BYPASS_CH6 | CODEC_CODEC_ADC_HBF2_BYPASS_CH6 | CODEC_CODEC_ADC_HBF3_BYPASS_CH6;

    if (cnt == 0) {
    } else if (cnt == 1) {
        bypass = CODEC_CODEC_ADC_HBF3_BYPASS_CH6;
    } else if (cnt == 2) {
        bypass = CODEC_CODEC_ADC_HBF2_BYPASS_CH6 | CODEC_CODEC_ADC_HBF3_BYPASS_CH6;
    } else if (cnt == 3) {
        bypass = CODEC_CODEC_ADC_HBF1_BYPASS_CH6 | CODEC_CODEC_ADC_HBF2_BYPASS_CH6 | CODEC_CODEC_ADC_HBF3_BYPASS_CH6;
    } else {
        ASSERT(false, "%s: Invalid bypass cnt: %u", __FUNCTION__, cnt);
    }
    if (map & AUD_CHANNEL_MAP_CH6) {
        codec->REG_240 = (codec->REG_240 & ~ bypass_mask) | bypass;
    }
    if (map & AUD_CHANNEL_MAP_CH7) {
        codec->REG_244 = (codec->REG_244 & ~ bypass_mask) | bypass;
    }

    return 0;
}

#ifdef __AUDIO_RESAMPLE__
static float get_playback_resample_phase(void)
{
    return (float)codec_dac_sample_rate[resample_rate_idx[AUD_STREAM_PLAYBACK]].codec_freq / hal_cmu_get_crystal_freq();
}

static float get_capture_resample_phase(void)
{
    return (float)hal_cmu_get_crystal_freq() / codec_adc_sample_rate[resample_rate_idx[AUD_STREAM_CAPTURE]].codec_freq;
}

static uint32_t resample_phase_float_to_value(float phase)
{
    if (phase >= 4.0) {
        return (uint32_t)-1;
    } else {
        // Phase format: 2.30
        return (uint32_t)(phase * (1 << 30));
    }
}

static float POSSIBLY_UNUSED resample_phase_value_to_float(uint32_t value)
{
    // Phase format: 2.30
    return (float)value / (1 << 30);
}
#endif

static void hal_codec_set_dac_gain_ramp_interval(enum AUD_SAMPRATE_T rate)
{
#ifdef DAC_RAMP_GAIN
    int ramp_intvl = CODEC_DAC_GAIN_RAMP_INTERVAL;

    if ((codec->REG_0EC & CODEC_CODEC_RAMP_EN_CH0) == 0) {
        return;
    }

    if (rate >= AUD_SAMPRATE_44100 * 8) {
        ramp_intvl += 3;
    } else if (rate >= AUD_SAMPRATE_44100 * 4) {
        ramp_intvl += 2;
    } else if (rate >= AUD_SAMPRATE_44100 * 2) {
        ramp_intvl += 1;
    } else if (rate >= AUD_SAMPRATE_44100) {
        ramp_intvl += 0;
    } else if (rate >= AUD_SAMPRATE_44100 / 2) {
        ramp_intvl -= 1;
    } else if (rate >= AUD_SAMPRATE_44100 / 4) {
        ramp_intvl -= 2;
    } else {
        ramp_intvl -= 3;
    }
    if (ramp_intvl < 0) {
        ramp_intvl = 0;
    } else if (ramp_intvl >= CODEC_DAC_GAIN_RAMP_INTVL_QTY) {
        ramp_intvl = CODEC_DAC_GAIN_RAMP_INTVL_QTY - 1;
    }
    codec->REG_0EC = SET_BITFIELD(codec->REG_0EC, CODEC_CODEC_RAMP_INTERVAL_CH0, ramp_intvl);
    codec->REG_0F0 = SET_BITFIELD(codec->REG_0F0, CODEC_CODEC_RAMP_INTERVAL_CH1, ramp_intvl);
#endif
}

int hal_codec_setup_stream(enum HAL_CODEC_ID_T id, enum AUD_STREAM_T stream, const struct HAL_CODEC_CONFIG_T *cfg)
{
    uint32_t i;
    int rate_idx;
    enum AUD_SAMPRATE_T sample_rate;

    if (stream == AUD_STREAM_PLAYBACK) {
        if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
            if (cfg->channel_num == AUD_CHANNEL_NUM_2) {
                if (cfg->channel_map != (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
                    TRACE(2,"\n!!! WARNING:%s: Bad play stereo ch map: 0x%X\n", __func__, cfg->channel_map);
                }
                codec->REG_044 |= CODEC_DUAL_CHANNEL_DAC;
            } else {
                ASSERT(cfg->channel_num == AUD_CHANNEL_NUM_1, "%s: Bad play ch num: %u", __func__, cfg->channel_num);
                // Allow to DMA one channel but output 2 channels
                ASSERT((cfg->channel_map & ~(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) == 0,
                    "%s: Bad play mono ch map: 0x%X", __func__, cfg->channel_map);
                codec->REG_044 &= ~CODEC_DUAL_CHANNEL_DAC;
            }
            codec_dac_ch_map = 0;
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH0) {
                codec_dac_ch_map |= AUD_CHANNEL_MAP_CH0;
            }
            if (cfg->channel_map & AUD_CHANNEL_MAP_CH1) {
                codec_dac_ch_map |= AUD_CHANNEL_MAP_CH1;
            }
        }

        if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
            if (cfg->bits == AUD_BITS_16) {
                codec->REG_044 = (codec->REG_044 & ~CODEC_MODE_32BIT_DAC) | CODEC_MODE_16BIT_DAC;
            } else if (cfg->bits == AUD_BITS_24) {
                codec->REG_044 &= ~(CODEC_MODE_16BIT_DAC | CODEC_MODE_32BIT_DAC);
            } else if (cfg->bits == AUD_BITS_32) {
                codec->REG_044 = (codec->REG_044 & ~CODEC_MODE_16BIT_DAC) | CODEC_MODE_32BIT_DAC;
            } else {
                ASSERT(false, "%s: Bad play bits: %u", __func__, cfg->bits);
            }
        }

        if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
            sample_rate = cfg->sample_rate;
#ifdef CODEC_DSD
            if (dsd_enabled) {
                if (sample_rate == AUD_SAMPRATE_176400) {
                    dsd_rate_idx = 0;
                } else if (sample_rate == AUD_SAMPRATE_352800) {
                    dsd_rate_idx = 1;
                } else if (sample_rate == AUD_SAMPRATE_705600) {
                    dsd_rate_idx = 2;
                } else {
                    ASSERT(false, "%s: Bad DSD sample rate: %u", __func__, sample_rate);
                }
                sample_rate = AUD_SAMPRATE_44100;
            }
#endif

            for (i = 0; i < ARRAY_SIZE(codec_dac_sample_rate); i++) {
                if (codec_dac_sample_rate[i].sample_rate == sample_rate) {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_dac_sample_rate), "%s: Invalid playback sample rate: %u", __func__, sample_rate);
            rate_idx = i;

            TRACE(2,"[%s] playback sample_rate=%d", __func__, sample_rate);

#ifdef CODEC_TIMER
            cur_codec_freq = codec_dac_sample_rate[rate_idx].codec_freq;
#endif

            codec_rate_idx[AUD_STREAM_PLAYBACK] = rate_idx;

#ifdef __AUDIO_RESAMPLE__
            uint32_t mask, val;

            if (hal_cmu_get_audio_resample_status() && codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL) {
#ifdef CODEC_TIMER
                cur_codec_freq = CODEC_FREQ_CRYSTAL;
#endif
                if ((codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) == 0 ||
                        resample_rate_idx[AUD_STREAM_PLAYBACK] != rate_idx) {
                    resample_rate_idx[AUD_STREAM_PLAYBACK] = rate_idx;
                    codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                    hal_codec_reg_update_delay();
                    codec->REG_0F4 = resample_phase_float_to_value(get_playback_resample_phase());
                    hal_codec_reg_update_delay();
                    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                }

                mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE;
                val = 0;
                if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH0) {
                    val |= CODEC_CODEC_RESAMPLE_DAC_L_ENABLE;
                }
                if (codec_dac_ch_map & AUD_CHANNEL_MAP_CH1) {
                    val |= CODEC_CODEC_RESAMPLE_DAC_R_ENABLE;
                }
            } else {
                mask = CODEC_CODEC_RESAMPLE_DAC_L_ENABLE | CODEC_CODEC_RESAMPLE_DAC_R_ENABLE |
                    CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
                val = 0;
            }
            codec->REG_0E4 = (codec->REG_0E4 & ~mask) | val;
#endif

            // 8K -> 4ms, 16K -> 2ms, ...
            dac_delay_ms = 4 / ((sample_rate + AUD_SAMPRATE_8000 / 2) / AUD_SAMPRATE_8000);
            if (dac_delay_ms < 2) {
                dac_delay_ms = 2;
            }

#ifdef __AUDIO_RESAMPLE__
            if (!hal_cmu_get_audio_resample_status())
#endif
            {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_dac_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL,
                    "%s: playback sample rate %u is for resample only", __func__, sample_rate);
#endif
            }

            hal_codec_set_dac_gain_ramp_interval(cfg->sample_rate);

            hal_codec_set_dac_up(codec_dac_sample_rate[rate_idx].dac_up);
            hal_codec_set_dac_hbf_bypass_cnt(codec_dac_sample_rate[rate_idx].bypass_cnt);
        }

        if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
            const struct CODEC_DAC_VOL_T *vol_tab_ptr;

            vol_tab_ptr = hal_codec_get_dac_volume(cfg->vol);
            if (vol_tab_ptr) {
#ifdef AUDIO_OUTPUT_SW_GAIN
                hal_codec_set_sw_gain(vol_tab_ptr->sdac_volume);
#else
                analog_aud_set_dac_gain(vol_tab_ptr->tx_pa_gain);
                hal_codec_set_dig_dac_gain(VALID_DAC_MAP, vol_tab_ptr->sdac_volume);
#endif
#ifdef PERF_TEST_POWER_KEY
                // Update performance test power after applying new dac volume
                hal_codec_update_perf_test_power();
#endif
            }
        }
    } else {
        enum AUD_CHANNEL_MAP_T mic_map;
        enum AUD_CHANNEL_MAP_T reserv_map;
        uint8_t cnt;
        uint8_t ch_idx;//software chan index
        uint32_t cfg_set_mask;
        uint32_t cfg_clr_mask;

        mic_map = 0;
        if ((HAL_CODEC_CONFIG_CHANNEL_MAP | HAL_CODEC_CONFIG_CHANNEL_NUM) & cfg->set_flag) {
            codec_adc_ch_map = 0;
            codec_mic_ch_map = 0;
            mic_map = cfg->channel_map;
        }

        if (mic_map) {
            codec_mic_ch_map = mic_map;
            reserv_map = 0;

#ifdef CODEC_DSD
            reserv_map |= AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5;
#endif
            if (mic_map & AUD_CHANNEL_MAP_ECMIC_CH0) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH6;
                mic_map &= ~AUD_CHANNEL_MAP_ECMIC_CH0;
                codec->REG_240 &= ~CODEC_CODEC_MC_SEL_CH0;
            }
            if (mic_map & AUD_CHANNEL_MAP_ECMIC_CH1) {
                codec_adc_ch_map |= AUD_CHANNEL_MAP_CH7;
                mic_map &= ~AUD_CHANNEL_MAP_ECMIC_CH1;
                codec->REG_244 |= CODEC_CODEC_MC_SEL_CH1;
            }
            reserv_map |= codec_adc_ch_map;

            i = 0;//codec ad ch
            while (mic_map && i < NORMAL_ADC_CH_NUM) {
                ASSERT(i < MAX_ANA_MIC_CH_NUM || (mic_map & AUD_CHANNEL_MAP_DIGMIC_ALL),
                    "%s: Not enough ana cap chan: mic_map=0x%X adc_map=0x%X reserv_map=0x%X",
                    __func__, mic_map, codec_adc_ch_map, reserv_map);
                ch_idx = get_lsb_pos(mic_map);//user ana ch or dig ch
                while ((reserv_map & (AUD_CHANNEL_MAP_CH0 << i)) && i < NORMAL_ADC_CH_NUM) {
                    i++;
                }
                if (i < NORMAL_ADC_CH_NUM) {
#ifdef SIDETONE_ENABLE
                    if ((1 << ch_idx) & CFG_HW_AUD_SIDETONE_MIC_DEV) {
                        codec->REG_080 = SET_BITFIELD(codec->REG_080, CODEC_CODEC_SIDE_TONE_MIC_SEL, i);
                    }
#endif
                    codec_adc_ch_map |= (AUD_CHANNEL_MAP_CH0 << i);
                    reserv_map |= codec_adc_ch_map;
                    mic_map &= ~(1 << ch_idx);
                    if ((1 << ch_idx) & AUD_CHANNEL_MAP_DIGMIC_ALL) {
                        ch_idx = hal_codec_get_digmic_hw_index(ch_idx);
                        codec->REG_0AC = (codec->REG_0AC & ~(CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) |
                            (CODEC_CODEC_PDM_MUX_CH0(ch_idx) << (3 * i));
                        if (i < MAX_ANA_MIC_CH_NUM)
                            codec->REG_0A8 |= CODEC_CODEC_PDM_ADC_SEL_CH0 << i;
                    } else {
                        ASSERT(i < MAX_ANA_MIC_CH_NUM,
                            "%s: Not enough ana chan: mic_map=0x%X adc_map=0x%X reserv_map=0x%X",
                            __func__, mic_map, codec_adc_ch_map, reserv_map);
                        *(&codec->REG_084 + i) = SET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0, ch_idx);
                        codec->REG_0A8 &= ~(CODEC_CODEC_PDM_ADC_SEL_CH0 << i);
                    }
                    i++;
                }
            }

            ASSERT(mic_map == 0, "%s: Bad cap chan map: 0x%X", __func__, mic_map);
        }

        if (HAL_CODEC_CONFIG_BITS & cfg->set_flag) {
            cfg_set_mask = 0;
            cfg_clr_mask = CODEC_MODE_16BIT_ADC | CODEC_MODE_24BIT_ADC | CODEC_MODE_32BIT_ADC;
            if (cfg->bits == AUD_BITS_16) {
                cfg_set_mask |= CODEC_MODE_16BIT_ADC;
            } else if (cfg->bits == AUD_BITS_24) {
                cfg_set_mask |= CODEC_MODE_24BIT_ADC;
            } else if (cfg->bits == AUD_BITS_32) {
                cfg_set_mask |= CODEC_MODE_32BIT_ADC;
            } else {
                ASSERT(false, "%s: Bad cap bits: %d", __func__, cfg->bits);
            }
            codec->REG_040 = (codec->REG_040 & ~cfg_clr_mask) | cfg_set_mask;
#ifdef VOICE_DETECTOR_EN
            codec_adc_bits = cfg->bits;
#endif
        }

        cnt = 0;
        for (i = 0; i < MAX_ADC_CH_NUM; i++) {
            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                cnt++;
            }
        }
        ASSERT(cnt == cfg->channel_num, "%s: Invalid capture stream chan cfg: mic_map=0x%X adc_map=0x%X num=%u",
            __func__, codec_mic_ch_map, codec_adc_ch_map, cfg->channel_num);

        if (HAL_CODEC_CONFIG_SAMPLE_RATE & cfg->set_flag) {
            sample_rate = cfg->sample_rate;

            for(i = 0; i < ARRAY_SIZE(codec_adc_sample_rate); i++) {
                if(codec_adc_sample_rate[i].sample_rate == sample_rate) {
                    break;
                }
            }
            ASSERT(i < ARRAY_SIZE(codec_adc_sample_rate), "%s: Invalid capture sample rate: %d", __func__, sample_rate);
            rate_idx = i;

            TRACE(2,"[%s] capture sample_rate=%d", __func__, sample_rate);

#ifdef CODEC_TIMER
            cur_codec_freq = codec_adc_sample_rate[rate_idx].codec_freq;
#endif

            codec_rate_idx[AUD_STREAM_CAPTURE] = rate_idx;

            uint32_t normal_chan_num;

            normal_chan_num = cfg->channel_num;
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH6) {
                normal_chan_num--;
            }
            if (codec_adc_ch_map & AUD_CHANNEL_MAP_CH7) {
                normal_chan_num--;
            }

#ifdef __AUDIO_RESAMPLE__
            uint32_t mask, val;

            if (hal_cmu_get_audio_resample_status() && codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL) {
#ifdef CODEC_TIMER
                cur_codec_freq = CODEC_FREQ_CRYSTAL;
#endif
                if ((codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) == 0 ||
                        resample_rate_idx[AUD_STREAM_CAPTURE] != rate_idx) {
                    resample_rate_idx[AUD_STREAM_CAPTURE] = rate_idx;
                    codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                    hal_codec_reg_update_delay();
                    codec->REG_0F8 = resample_phase_float_to_value(get_capture_resample_phase());
                    hal_codec_reg_update_delay();
                    codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
                }

                mask = 0;
                val = 0;
                cnt = 0;
                for (i = 0; i < MAX_ADC_CH_NUM; i++) {//include ec ch
                    if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                        mask |= (CODEC_CODEC_RESAMPLE_ADC_CH0_SEL_MASK << cnt*3);
                        val |= (CODEC_CODEC_RESAMPLE_ADC_CH0_SEL(i) << cnt*3);
                        cnt++;
                    }
                }
                codec->REG_100 = (codec->REG_100 & ~mask) | val;
                codec->REG_0E4 = SET_BITFIELD(codec->REG_0E4, CODEC_CODEC_RESAMPLE_ADC_CH_CNT, cfg->channel_num - 1);
            } else {
                codec->REG_0E4 &= (~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE);
            }
#endif

            if (hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0) {
                // Echo cancel channels will check the enable signal of resample ADC CH0, even when resample is disabled
                if (codec_adc_ch_map & EC_ADC_MAP) {
                    if (normal_chan_num && (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) == 0) {
                        for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
                            if (codec_adc_ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
                                codec->REG_100 = SET_BITFIELD(codec->REG_100, CODEC_CODEC_RESAMPLE_ADC_CH0_SEL, i);
                                break;
                            }
                        }
                    }
                }
            }

#ifdef __AUDIO_RESAMPLE__
            if (!hal_cmu_get_audio_resample_status())
#endif
            {
#ifdef __AUDIO_RESAMPLE__
                ASSERT(codec_adc_sample_rate[rate_idx].codec_freq != CODEC_FREQ_CRYSTAL,
                    "%s: capture sample rate %u is for resample only", __func__, sample_rate);
#endif
            }
            hal_codec_set_adc_down(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].adc_down);
            hal_codec_set_adc_hbf_bypass_cnt(codec_adc_ch_map, codec_adc_sample_rate[rate_idx].bypass_cnt);
#ifdef VOICE_DETECTOR_EN
            codec_adc_down = codec_adc_sample_rate[rate_idx].adc_down;
#endif
        }

#if !(defined(FIXED_CODEC_ADC_VOL) && defined(SINGLE_CODEC_ADC_VOL))
        if (HAL_CODEC_CONFIG_VOL & cfg->set_flag) {
#ifdef SINGLE_CODEC_ADC_VOL
            const CODEC_ADC_VOL_T *adc_gain_db;
            adc_gain_db = hal_codec_get_adc_volume(cfg->vol);
            if (adc_gain_db) {
                hal_codec_set_dig_adc_gain(NORMAL_ADC_MAP, *adc_gain_db);
            }
#else
            uint32_t vol;

            mic_map = codec_mic_ch_map;
            while (mic_map) {
                ch_idx = get_lsb_pos(mic_map);
                mic_map &= ~(1 << ch_idx);
                vol = hal_codec_get_mic_chan_volume_level(1 << ch_idx);
                hal_codec_set_chan_vol(AUD_STREAM_CAPTURE, (1 << ch_idx), vol);
            }
#endif
        }
#endif
    }

    return 0;
}

int hal_codec_aux_mic_dma_enable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

int hal_codec_aux_mic_dma_disable(enum HAL_CODEC_ID_T id)
{
    return 0;
}

uint32_t hal_codec_get_alg_dac_shift(void)
{
    return 0;
}

#ifdef AUDIO_OUTPUT_DC_CALIB
void hal_codec_set_dac_dc_gain_attn(float attn)
{
    dac_dc_gain_attn = attn;
}

void hal_codec_set_dac_dc_offset(int16_t dc_l, int16_t dc_r)
{
    // DC calib values are based on 16-bit, but hardware compensation is based on 24-bit
    dac_dc_l = dc_l << 8;
    dac_dc_r = dc_r << 8;
#ifdef SDM_MUTE_NOISE_SUPPRESSION
    if (dac_dc_l == 0) {
        dac_dc_l = 1;
    }
    if (dac_dc_r == 0) {
        dac_dc_r = 1;
    }
#endif
}
#endif

void hal_codec_set_dac_reset_callback(HAL_CODEC_DAC_RESET_CALLBACK callback)
{
    //dac_reset_callback = callback;
}

static uint32_t POSSIBLY_UNUSED hal_codec_get_adc_chan(enum AUD_CHANNEL_MAP_T mic_map)
{
    uint8_t adc_ch;
    uint8_t mic_ch;
    uint8_t digmic_ch0;
    uint8_t en_ch;
    bool digmic;
    uint32_t i;

    adc_ch = MAX_ADC_CH_NUM;

    mic_ch = get_lsb_pos(mic_map);

    if (((1 << mic_ch) & codec_mic_ch_map) == 0) {
        return adc_ch;
    }

    if ((1 << mic_ch) == AUD_CHANNEL_MAP_ECMIC_CH0) {
        return get_lsb_pos(AUD_CHANNEL_MAP_CH6);
    }
    if ((1 << mic_ch) == AUD_CHANNEL_MAP_ECMIC_CH1) {
        return get_lsb_pos(AUD_CHANNEL_MAP_CH7);
    }

    digmic_ch0 = get_lsb_pos(AUD_CHANNEL_MAP_DIGMIC_CH0);

    if (mic_ch >= digmic_ch0) {
        mic_ch -= digmic_ch0;
        digmic = true;
    } else {
        digmic = false;
    }

    for (i = 0; i < NORMAL_ADC_CH_NUM; i++) {
        if (codec_adc_ch_map & (1 << i)) {
            if (i < MAX_ANA_MIC_CH_NUM) {
                if (digmic ^ !!(codec->REG_0A8 & (CODEC_CODEC_PDM_ADC_SEL_CH0 << i))) {
                    continue;
                }
            }
            if (digmic) {
                en_ch = (codec->REG_0AC & (CODEC_CODEC_PDM_MUX_CH0_MASK << (3 * i))) >> (CODEC_CODEC_PDM_MUX_CH0_SHIFT + 3 * i);
            } else {
                ASSERT(i < MAX_ANA_MIC_CH_NUM,
                    "%s: Not enough ana chan: mic_map=0x%X adc_map=0x%X codec_mic_ch_map=0x%X",
                    __func__, mic_map, codec_adc_ch_map, codec_mic_ch_map);
                en_ch = GET_BITFIELD(*(&codec->REG_084 + i), CODEC_CODEC_ADC_IN_SEL_CH0);
            }
            if (mic_ch == en_ch) {
                adc_ch = i;
                break;
            }
        }
    }

    return adc_ch;
}

void hal_codec_sidetone_enable(void)
{
#ifdef SIDETONE_ENABLE
#if (CFG_HW_AUD_SIDETONE_MIC_DEV & (CFG_HW_AUD_SIDETONE_MIC_DEV - 1))
#error "Invalid CFG_HW_AUD_SIDETONE_MIC_DEV: only 1 mic can be defined"
#endif
#if (CFG_HW_AUD_SIDETONE_MIC_DEV == 0) || (CFG_HW_AUD_SIDETONE_MIC_DEV & ~NORMAL_MIC_MAP)
#error "Invalid CFG_HW_AUD_SIDETONE_MIC_DEV: bad mic channel"
#endif
    int gain = CFG_HW_AUD_SIDETONE_GAIN_DBVAL;
    uint32_t val;

    if (gain > MAX_SIDETONE_DBVAL) {
        gain = MAX_SIDETONE_DBVAL;
    } else if (gain < MIN_SIDETONE_DBVAL) {
        gain = MIN_SIDETONE_DBVAL;
    }

    val = MIN_SIDETONE_REGVAL + (gain - MIN_SIDETONE_DBVAL) / SIDETONE_DBVAL_STEP;

    codec->REG_080 = SET_BITFIELD(codec->REG_080, CODEC_CODEC_SIDE_TONE_GAIN, val);
#ifdef CFG_HW_AUD_SIDETONE_IIR_INDEX
#error "Not support CFG_HW_AUD_SIDETONE_IIR_INDEX"
#endif
#endif
}

void hal_codec_sidetone_disable(void)
{
#ifdef SIDETONE_ENABLE
    codec->REG_080 = SET_BITFIELD(codec->REG_080, CODEC_CODEC_SIDE_TONE_GAIN, MIN_SIDETONE_REGVAL);
#endif
}

void hal_codec_sync_dac_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_DAC_ENABLE_SEL, type) | CODEC_EVENT_FOR_EN;
}

void hal_codec_sync_dac_disable(void)
{
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_DAC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_adc_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_ADC_ENABLE_SEL, type);
}

void hal_codec_sync_adc_disable(void)
{
    codec->REG_054 = SET_BITFIELD(codec->REG_054, CODEC_CODEC_ADC_ENABLE_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac_resample_rate_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
    codec->REG_0E4 = SET_BITFIELD(codec->REG_0E4, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL, type);
}

void hal_codec_sync_dac_resample_rate_disable(void)
{
    codec->REG_0E4 = SET_BITFIELD(codec->REG_0E4, CODEC_CODEC_RESAMPLE_DAC_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_adc_resample_rate_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
    codec->REG_0E4 = SET_BITFIELD(codec->REG_0E4, CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL, type);
}

void hal_codec_sync_adc_resample_rate_disable(void)
{
    codec->REG_0E4 = SET_BITFIELD(codec->REG_0E4, CODEC_CODEC_RESAMPLE_ADC_UPDATE_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_dac_gain_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
    codec->REG_0A0 = SET_BITFIELD(codec->REG_0A0, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL, type);
}

void hal_codec_sync_dac_gain_disable(void)
{
    codec->REG_0A0 = SET_BITFIELD(codec->REG_0A0, CODEC_CODEC_DAC_GAIN_TRIGGER_SEL, HAL_CODEC_SYNC_TYPE_NONE);
}

void hal_codec_sync_adc_gain_enable(enum HAL_CODEC_SYNC_TYPE_T type)
{
}

void hal_codec_sync_adc_gain_disable(void)
{
}

void hal_codec_set_dac_bt_sync_source(uint32_t src)
{
    codec->REG_058 = (codec->REG_058 & ~(
        CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK |
        CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_DAC_ENABLE_PLAYTIME_STAMP_SEL(src) |
        CODEC_RESAMPLE_DAC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_RESAMPLE_DAC_PHASE_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_set_adc_bt_sync_source(uint32_t src)
{
    codec->REG_058 = (codec->REG_058 & ~(
        CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK |
        CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL_MASK | CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL_MASK)) |
        CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_CODEC_ADC_ENABLE_PLAYTIME_STAMP_SEL(src) |
        CODEC_RESAMPLE_ADC_ENABLE_PLAYTIME_STAMP_SEL(src) | CODEC_RESAMPLE_ADC_PHASE_PLAYTIME_STAMP_SEL(src);
}

void hal_codec_gpio_trigger_debounce_enable(void)
{
    if (codec_opened) {
        codec->REG_054 |= CODEC_GPIO_TRIGGER_DB_ENABLE;
    }
}

void hal_codec_gpio_trigger_debounce_disable(void)
{
    if (codec_opened) {
        codec->REG_054 &= ~CODEC_GPIO_TRIGGER_DB_ENABLE;
    }
}

#ifdef CODEC_TIMER
uint32_t hal_codec_timer_get(void)
{
    if (codec_opened) {
        return codec->REG_050;
    }

    return 0;
}

uint32_t hal_codec_timer_ticks_to_us(uint32_t ticks)
{
    uint32_t timer_freq;

    timer_freq = cur_codec_freq / 4 / CODEC_FREQ_EXTRA_DIV;

    return (uint32_t)((float)ticks * 1000000 / timer_freq);
}

void hal_codec_timer_trigger_read(void)
{
    if (codec_opened) {
        codec->REG_078 ^= CODEC_GET_CNT_TRIG;
        hal_codec_reg_update_delay();
    }
}
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
int hal_codec_dac_sdm_reset_set(void)
{
    if (codec_opened) {
#ifdef DAC_RAMP_GAIN
        bool ramp = false;

        if (codec->REG_0EC & CODEC_CODEC_RAMP_EN_CH0) {
            codec->REG_0EC &= ~CODEC_CODEC_RAMP_EN_CH0;
            codec->REG_0F0 &= ~CODEC_CODEC_RAMP_EN_CH1;
            hal_codec_reg_update_delay();
            ramp = true;
        }
#endif
        hal_codec_set_dac_gain_value(VALID_DAC_MAP, 0);
#ifdef DAC_RAMP_GAIN
        if (ramp) {
            hal_codec_reg_update_delay();
            codec->REG_0EC |= CODEC_CODEC_RAMP_EN_CH0;
            codec->REG_0F0 |= CODEC_CODEC_RAMP_EN_CH1;
        }
#endif
        if (codec->REG_09C & CODEC_CODEC_DAC_EN) {
            osDelay(dac_delay_ms);
        }
#ifdef SDM_MUTE_NOISE_SUPPRESSION_V2
        for (int i = 0x200; i >= 0; i -= 0x100) {
            hal_codec_dac_dc_offset_enable(i, i);
            osDelay(1);
        }
#endif
        codec->REG_09C |= CODEC_CODEC_DAC_SDM_CLOSE;
        osDelay(1);
    }

    return 0;
}

int hal_codec_dac_sdm_reset_clear(void)
{
    if (codec_opened) {
        osDelay(1);
        codec->REG_09C &= ~CODEC_CODEC_DAC_SDM_CLOSE;
#ifdef SDM_MUTE_NOISE_SUPPRESSION_V2
        for (int i = 0x100; i <= 0x300; i += 0x100) {
            hal_codec_dac_dc_offset_enable(i, i);
            osDelay(1);
        }
#endif
        hal_codec_restore_dig_dac_gain();
    }

    return 0;
}
#endif

void hal_codec_tune_resample_rate(enum AUD_STREAM_T stream, float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    uint32_t val;

    if (!codec_opened) {
        return;
    }

    if (stream == AUD_STREAM_PLAYBACK) {
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE) {
            codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            val = resample_phase_float_to_value(get_playback_resample_phase());
            val += (int)(val * ratio);
            codec->REG_0F4 = val;
            hal_codec_reg_update_delay();
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
        }
    } else {
        if (codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE) {
            codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
            hal_codec_reg_update_delay();
            val = resample_phase_float_to_value(get_capture_resample_phase());
            val -= (int)(val * ratio);
            codec->REG_0F8 = val;
            hal_codec_reg_update_delay();
            codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
        }
    }
#endif
}

void hal_codec_tune_both_resample_rate(float ratio)
{
#ifdef __AUDIO_RESAMPLE__
    bool update[2];
    uint32_t val[2];
    uint32_t lock;

    if (!codec_opened) {
        return;
    }

    update[0] = !!(codec->REG_0E4 & CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE);
    update[1] = !!(codec->REG_0E4 & CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE);

    val[0] = val[1] = 0;

    if (update[0]) {
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
        val[0] = resample_phase_float_to_value(get_playback_resample_phase());
        val[0] += (int)(val[0] * ratio);
    }
    if (update[1]) {
        codec->REG_0E4 &= ~CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
        val[1] = resample_phase_float_to_value(get_capture_resample_phase());
        val[1] -= (int)(val[1] * ratio);
    }

    hal_codec_reg_update_delay();

    if (update[0]) {
        codec->REG_0F4 = val[0];
    }
    if (update[1]) {
        codec->REG_0F8 = val[1];
    }

    hal_codec_reg_update_delay();

    lock = int_lock();
    if (update[0]) {
        codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_DAC_PHASE_UPDATE;
    }
    if (update[1]) {
        codec->REG_0E4 |= CODEC_CODEC_RESAMPLE_ADC_PHASE_UPDATE;
    }
    int_unlock(lock);
#endif
}

int hal_codec_select_clock_out(uint32_t cfg)
{
    uint32_t lock;
    int ret = 1;

    lock = int_lock();

    if (codec_opened) {
        codec->REG_060 = SET_BITFIELD(codec->REG_060, CODEC_CFG_CLK_OUT, cfg);
        ret = 0;
    }

    int_unlock(lock);

    return ret;
}

void hal_codec_swap_output(bool swap)
{
#ifdef AUDIO_OUTPUT_SWAP
    output_swap = swap;

    if (codec_opened) {
        if (output_swap) {
            codec->REG_0A4 |= CODEC_CODEC_DAC_OUT_SWAP;
        } else {
            codec->REG_0A4 &= ~CODEC_CODEC_DAC_OUT_SWAP;
        }
    }
#endif
}

int hal_codec_config_digmic_phase(uint8_t phase)
{
#ifdef ANC_PROD_TEST
    codec_digmic_phase = phase;
#endif
    return 0;
}

static void hal_codec_general_irq_handler(void)
{
    uint32_t status;

    status = codec->REG_00C;
    codec->REG_00C = status;

    status &= codec->REG_010;

    //TRACE(0, "%s, status:0x%x, callback:%p", __func__, status, codec_irq_callback[0]);

    for (uint32_t i = 0; i < CODEC_IRQ_TYPE_QTY; i++) {
        if (codec_irq_callback[i]) {
            codec_irq_callback[i](status);
        }
    }
}

static POSSIBLY_UNUSED void hal_codec_set_irq_handler(enum CODEC_IRQ_TYPE_T type, HAL_CODEC_IRQ_CALLBACK cb)
{
    uint32_t lock;

    ASSERT(type < CODEC_IRQ_TYPE_QTY, "%s: Bad type=%d", __func__, type);

    lock = int_lock();

    codec_irq_callback[type] = cb;

    if (cb) {
        if (codec_irq_map == 0) {
            NVIC_SetVector(CODEC_IRQn, (uint32_t)hal_codec_general_irq_handler);
            NVIC_SetPriority(CODEC_IRQn, IRQ_PRIORITY_HIGHPLUSPLUS);
            NVIC_ClearPendingIRQ(CODEC_IRQn);
            NVIC_EnableIRQ(CODEC_IRQn);
        }
        codec_irq_map |= (1 << type);
    } else {
        codec_irq_map &= ~(1 << type);
        if (codec_irq_map == 0) {
            NVIC_DisableIRQ(CODEC_IRQn);
            NVIC_ClearPendingIRQ(CODEC_IRQn);
        }
    }

    int_unlock(lock);
}

//********************BT trigger functions: START********************
static void hal_codec_bt_trigger_isr(uint32_t irq_status)
{
    uint32_t index;

    index = 0;
    if (irq_status & CODEC_BT_TRIGGER) {
        if (bt_trigger_callback[index]) {
            TRACE(1,"[%s] bt_trigger_callback[%u] start", __func__, index);
            bt_trigger_callback[index]();
        } else {
            TRACE(1,"[%s] bt_trigger_callback[%u] = NULL", __func__, index);
        }
    }

    for (index = 1; index < BT_TRIGGER_NUM; index++) {
        if (irq_status & (CODEC_BT_TRIGGER1 << index)) {
            if (bt_trigger_callback[index]) {
                TRACE(1,"[%s] bt_trigger_callback[%u] start", __func__, index);
                bt_trigger_callback[index]();
            } else {
                TRACE(1,"[%s] bt_trigger_callback[%u] = NULL", __func__, index);
            }
        }
    }
}

static inline void hal_codec_bt_trigger_irq_en(uint32_t index, int enable)
{
    if (index == 0) {
        if (enable)
            codec->REG_010 |= (CODEC_BT_TRIGGER_MSK << index);
        else
            codec->REG_010 &= ~(CODEC_BT_TRIGGER_MSK << index);
        codec->REG_00C = (CODEC_BT_TRIGGER << index);
    } else {
        if (enable)
            codec->REG_010 |= (CODEC_BT_TRIGGER1_MSK << (index-1));
        else
            codec->REG_010 &= ~(CODEC_BT_TRIGGER1_MSK << (index-1));
        codec->REG_00C = (CODEC_BT_TRIGGER1 << (index-1));
    }
}

void hal_codec_set_bt_trigger_ex_callback(uint32_t index, HAL_CODEC_BT_TRIGGER_CALLBACK callback)
{
    if (index >= BT_TRIGGER_NUM) {
        return;
    }

    bt_trigger_callback[index] = callback;
}

int hal_codec_bt_trigger_ex_start(uint32_t index)
{
    uint32_t lock;

    TRACE(1,"[%s] Start: index=%d", __func__, index);

    if (index >= BT_TRIGGER_NUM) {
        return 1;
    }

    lock = int_lock();

    hal_codec_set_irq_handler(CODEC_IRQ_TYPE_BT_TRIGGER + index, hal_codec_bt_trigger_isr);
    hal_codec_bt_trigger_irq_en(index, 1);

    int_unlock(lock);

    return 0;
}

int hal_codec_bt_trigger_ex_stop(uint32_t index)
{
    uint32_t lock;

    TRACE(1,"[%s] Stop", __func__);

    if (index >= BT_TRIGGER_NUM) {
        return 1;
    }

    lock = int_lock();

    hal_codec_bt_trigger_irq_en(index, 0);
    hal_codec_set_irq_handler(CODEC_IRQ_TYPE_BT_TRIGGER + index, NULL);

    int_unlock(lock);

    return 0;
}

void hal_codec_set_bt_trigger_callback(HAL_CODEC_BT_TRIGGER_CALLBACK callback)
{
    hal_codec_set_bt_trigger_ex_callback(0, callback);
}

int hal_codec_bt_trigger_start(void)
{
    return hal_codec_bt_trigger_ex_start(0);
}

int hal_codec_bt_trigger_stop(void)
{
    return hal_codec_bt_trigger_ex_stop(0);
}

//********************BT trigger functions: END********************

static void hal_codec_event_trigger_isr(uint32_t irq_status)
{
    if ((irq_status & CODEC_EVENT_TRIGGER) == 0) {
        return;
    }

    if (event_trigger_callback) {
        event_trigger_callback();
    }
}

static inline void hal_codec_event_trigger_irq_en(int enable)
{
    if (enable) {
        codec->REG_010 |= CODEC_EVENT_TRIGGER_MSK;
    } else {
        codec->REG_010 &= ~CODEC_EVENT_TRIGGER_MSK;
    }

    codec->REG_00C = CODEC_EVENT_TRIGGER;
}

void hal_codec_set_event_trigger_callback(HAL_CODEC_EVENT_TRIGGER_CALLBACK callback)
{
    uint32_t lock;

    event_trigger_callback = callback;

    lock = int_lock();
    if (callback) {
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_EVENT_TRIGGER, hal_codec_event_trigger_isr);
        hal_codec_event_trigger_irq_en(1);
    } else {
        hal_codec_event_trigger_irq_en(0);
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_EVENT_TRIGGER, NULL);
    }
    int_unlock(lock);
}

static void hal_codec_timer_trigger_isr(uint32_t irq_status)
{
    if ((irq_status & CODEC_TIME_TRIGGER) == 0) {
        return;
    }

    if (timer_trigger_callback) {
        timer_trigger_callback();
    }
}

static inline void hal_codec_timer_trigger_irq_en(int enable)
{
    if (enable) {
        codec->REG_010 |= CODEC_TIME_TRIGGER_MSK;
    } else {
        codec->REG_010 &= ~CODEC_TIME_TRIGGER_MSK;
    }

    codec->REG_00C = CODEC_TIME_TRIGGER;
}

void hal_codec_set_timer_trigger_callback(HAL_CODEC_TIMER_TRIGGER_CALLBACK callback)
{
    uint32_t lock;

    timer_trigger_callback = callback;

    lock = int_lock();
    if (callback) {
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_TIMER_TRIGGER, hal_codec_timer_trigger_isr);
        hal_codec_timer_trigger_irq_en(1);
    } else {
        hal_codec_timer_trigger_irq_en(0);
        hal_codec_set_irq_handler(CODEC_IRQ_TYPE_TIMER_TRIGGER, NULL);
    }
    int_unlock(lock);
}

/* AUDIO CODEC VOICE ACTIVE DETECTION DRIVER */
#ifdef VOICE_DETECTOR_EN

#define CODEC_VAD_DEBUG

static inline void hal_codec_vad_set_udc(int v)
{
    codec->REG_14C &= ~CODEC_VAD_U_DC(0xf);
    codec->REG_14C |= CODEC_VAD_U_DC(v);
}

static inline void hal_codec_vad_set_upre(int v)
{
    codec->REG_14C &= ~CODEC_VAD_U_PRE(0x7);
    codec->REG_14C |= CODEC_VAD_U_PRE(v);
}

static inline void hal_codec_vad_set_frame_len(int v)
{
    codec->REG_14C &= ~CODEC_VAD_FRAME_LEN(0xff);
    codec->REG_14C |= CODEC_VAD_FRAME_LEN(v);
}

static inline void hal_codec_vad_set_mvad(int v)
{
    codec->REG_14C &= ~CODEC_VAD_MVAD(0xf);
    codec->REG_14C |= CODEC_VAD_MVAD(v);
}

static inline void hal_codec_vad_set_pre_gain(int v)
{
    codec->REG_14C &= ~CODEC_VAD_PRE_GAIN(0x3f);
    codec->REG_14C |= CODEC_VAD_PRE_GAIN(v);
}

static inline void hal_codec_vad_set_sth(int v)
{
    codec->REG_14C &= ~CODEC_VAD_STH(0x3f);
    codec->REG_14C |= CODEC_VAD_STH(v);
}

static inline void hal_codec_vad_set_frame_th1(int v)
{
    codec->REG_150 &= ~CODEC_VAD_FRAME_TH1(0xff);
    codec->REG_150 |= CODEC_VAD_FRAME_TH1(v);
}

static inline void hal_codec_vad_set_frame_th2(int v)
{
    codec->REG_150 &= ~CODEC_VAD_FRAME_TH2(0x3ff);
    codec->REG_150 |= CODEC_VAD_FRAME_TH2(v);
}

static inline void hal_codec_vad_set_frame_th3(int v)
{
    codec->REG_150 &= ~CODEC_VAD_FRAME_TH3(0x3fff);
    codec->REG_150 |= CODEC_VAD_FRAME_TH3(v);
}

static inline void hal_codec_vad_set_range1(int v)
{
    codec->REG_154 &= ~CODEC_VAD_RANGE1(0x1f);
    codec->REG_154 |= CODEC_VAD_RANGE1(v);
}

static inline void hal_codec_vad_set_range2(int v)
{
    codec->REG_154 &= ~CODEC_VAD_RANGE2(0x7f);
    codec->REG_154 |= CODEC_VAD_RANGE2(v);
}

static inline void hal_codec_vad_set_range3(int v)
{
    codec->REG_154 &= ~CODEC_VAD_RANGE3(0x1ff);
    codec->REG_154 |= CODEC_VAD_RANGE3(v);
}

static inline void hal_codec_vad_set_range4(int v)
{
    codec->REG_154 &= ~CODEC_VAD_RANGE4(0x3ff);
    codec->REG_154 |= CODEC_VAD_RANGE4(v);
}

static inline void hal_codec_vad_set_psd_th1(int v)
{
    codec->REG_158 &= ~CODEC_VAD_PSD_TH1(0x7ffffff);
    codec->REG_158 |= CODEC_VAD_PSD_TH1(v);
}

static inline void hal_codec_vad_set_psd_th2(int v)
{
    codec->REG_15C &= ~CODEC_VAD_PSD_TH2(0x7ffffff);
    codec->REG_15C |= CODEC_VAD_PSD_TH2(v);
}

static inline void hal_codec_vad_set_mem_mode(int mode)
{
    codec->REG_148 = SET_BITFIELD(codec->REG_148, CODEC_VAD_MEM_MODE, mode);
}

static inline void hal_codec_vad_flush_buff(int enable)
{
    if (enable)
        codec->REG_148 |= CODEC_VAD_FLUSH ; //clear addr cnt / mem cnt
    else
        codec->REG_148 &= ~CODEC_VAD_FLUSH;; //clear the bit
}

static inline void hal_codec_vad_en(int enable)
{
    if (enable) {
        codec->REG_148 |= CODEC_VAD_EN; //enable vad
        codec->REG_144 |= CODEC_VAD_IF_ENABLE | (vad_mic_ch << 1) | CODEC_VAD_BURST_CNT(1) | CODEC_VAD_THRESHOLD(3);

        if(vad_mic_num == 1) {
            codec->REG_148 = SET_BITFIELD(codec->REG_148, CODEC_VAD_MIC_SEL, get_lsb_pos(vad_mic_ch));
        } else {
            codec->REG_148 = SET_BITFIELD(codec->REG_148, CODEC_VAD_MIC_SEL, 2);
        }
    } else {
        codec->REG_148 &= ~CODEC_VAD_EN; //disable vad
        codec->REG_148 |= CODEC_VAD_FINISH;
        codec->REG_144 &= ~(CODEC_VAD_IF_ENABLE | (vad_mic_ch << 1));
    }
}

static inline void hal_codec_vad_bypass_ds(int bypass)
{
    if (bypass)
        codec->REG_148 |= CODEC_VAD_DS_BYPASS; //bypass ds
    else
        codec->REG_148 &= ~CODEC_VAD_DS_BYPASS; //not bypass ds
}

static inline void hal_codec_vad_bypass_dc(int bypass)
{
    if (bypass)
        codec->REG_148 |= CODEC_VAD_DC_CANCEL_BYPASS; // bypass dc
    else
        codec->REG_148 &= ~CODEC_VAD_DC_CANCEL_BYPASS; //not bypass dc
}

static inline void hal_codec_vad_bypass_pre(int bypass)
{
    if (bypass)
        codec->REG_148 |= CODEC_VAD_PRE_BYPASS; //bypass pre
    else
        codec->REG_148 &= ~CODEC_VAD_PRE_BYPASS; //not bypass pre
}

static inline void hal_codec_vad_dig_mode(int enable)
{
    if (enable)
        codec->REG_148 |= CODEC_VAD_DIG_MODE; //digital mode
    else
        codec->REG_148 &= ~CODEC_VAD_DIG_MODE; //not digital mode
}

static inline void hal_codec_vad_adc_en(int enable)
{
    if (enable) {
        codec->REG_080 |= (CODEC_CODEC_ADC_EN | (vad_mic_ch << 1));
    } else {
        uint32_t val;

        val = codec->REG_080;
        val &= ~(vad_mic_ch << 1);
        if ((val & (CODEC_CODEC_ADC_EN_CH0 | CODEC_CODEC_ADC_EN_CH1 | CODEC_CODEC_ADC_EN_CH2)) == 0) {
            val &= ~CODEC_CODEC_ADC_EN;
        }
        codec->REG_080 = val;
    }
}

static inline void hal_codec_vad_irq_en(int enable)
{
    if (enable){
        codec->REG_010 |= (CODEC_VAD_FIND_MSK | CODEC_VAD_NOT_FIND_MSK);
    }
    else{
        codec->REG_010 &= ~(CODEC_VAD_FIND_MSK | CODEC_VAD_NOT_FIND_MSK);
    }

    codec->REG_00C = CODEC_VAD_FIND | CODEC_VAD_NOT_FIND;
}

static inline void hal_codec_vad_adc_if_en(int enable)
{
    if (enable) {
        codec->REG_000 |= (CODEC_DMACTRL_RX | (vad_mic_ch << 2));
    } else {
        codec->REG_000 &= ~(CODEC_DMACTRL_RX | (vad_mic_ch << 2) | CODEC_ADC_ENABLE);
    }
}

#ifdef CODEC_VAD_DEBUG
void hal_codec_vad_reg_dump(void)
{
    TRACE(1,"codec base = %8x\n", (int)&(codec->REG_000));
    TRACE(1,"codec->REG_000 = %#x\n", codec->REG_000);
    TRACE(1,"codec->REG_00C = %#x\n", codec->REG_00C);
    TRACE(1,"codec->REG_010 = %#x\n", codec->REG_010);
    TRACE(1,"codec->REG_060 = %#x\n", codec->REG_060);
    TRACE(1,"codec->REG_064 = %#x\n", codec->REG_064);
    TRACE(1,"codec->REG_080 = %#x\n", codec->REG_080);
    TRACE(1,"codec->REG_094 = %#x\n", codec->REG_094);
    TRACE(1,"codec->REG_144 = %#x\n", codec->REG_144);
    TRACE(1,"codec->REG_148 = %#x\n", codec->REG_148);
    TRACE(1,"codec->REG_14C = %#x\n", codec->REG_14C);
    TRACE(1,"codec->REG_150 = %#x\n", codec->REG_150);
    TRACE(1,"codec->REG_154 = %#x\n", codec->REG_154);
    TRACE(1,"codec->REG_158 = %#x\n", codec->REG_158);
    TRACE(1,"codec->REG_15C = %#x\n", codec->REG_15C);
    TRACE(1,"codec->REG_160 = %#x\n", codec->REG_160);
}
#endif

static inline void hal_codec_vad_data_info(uint32_t *data_cnt, uint32_t *addr_cnt)
{
    uint32_t regval = codec->REG_184;

    *data_cnt = GET_BITFIELD(regval, CODEC_VAD_MEM_DATA_CNT) * 2;
    if (*data_cnt >= ((CODEC_VAD_MEM_DATA_CNT_MASK >> CODEC_VAD_MEM_DATA_CNT_SHIFT) - 1) * 2) {
        *data_cnt = ((CODEC_VAD_MEM_DATA_CNT_MASK >> CODEC_VAD_MEM_DATA_CNT_SHIFT) + 1) * 2;
    }

    regval = codec->REG_160;
    *addr_cnt = GET_BITFIELD(regval, CODEC_VAD_MEM_ADDR_CNT) * 2;
}

uint32_t hal_codec_vad_recv_data(uint8_t *dst, uint32_t dst_size)
{
    uint8_t *src = (uint8_t *)CODEC_VAD_BUF_ADDR;
    const uint32_t src_size = CODEC_VAD_BUF_SIZE;
    uint32_t len;
    uint32_t start_pos;

    TRACE(5,"%s, dst=%#x, dst_size=%d, data_cnt=%u, addr_cnt=%#x",
        __func__, (uint32_t)dst, dst_size, vad_data_cnt, vad_addr_cnt);

    ASSERT((dst_size <= src_size), "Bad vad dst size: 0x%X", dst_size);

    if (vad_addr_cnt < (CODEC_VAD_MAX_BUF_SIZE - CODEC_VAD_BUF_SIZE)) {
        TRACE(0, "%s %d", __func__, __LINE__);
        return 0;
    }
    if (vad_addr_cnt + 2 > CODEC_VAD_MAX_BUF_SIZE) {
        TRACE(0, "%s %d", __func__, __LINE__);
        return 0;
    }
    if (vad_data_cnt > CODEC_VAD_MAX_BUF_SIZE) {
        TRACE(0, "%s %d", __func__, __LINE__);
        return 0;
    }

    if (dst == NULL) {
        return vad_data_cnt;
    }

    if (vad_data_cnt < src_size) {
        start_pos = 0;
    } else {
        start_pos = vad_addr_cnt - (CODEC_VAD_MAX_BUF_SIZE - CODEC_VAD_BUF_SIZE);
    }

    len = MIN(dst_size, vad_data_cnt);
    if (len == 0) {
        return 0;
    }
#if defined(VAD_GET_DATA_FIFO)
    if (start_pos + len <= src_size) {
        memcpy(dst, src + start_pos, len);
    } else {
        uint32_t len1, len2;
        len1 = src_size - start_pos;
        len2 = len - len1;
        memcpy(dst, src + start_pos, len1);
        memcpy(dst + len1, src, len2);
    }
#else
    if (start_pos == 0) {
        memcpy(dst, src, len);
    } else {
        uint8_t *pos;
        uint32_t len1, len2;
        len2 = start_pos;
        len1 = len - len2;
        pos = src + src_size - len1;
        memcpy(dst, pos, len1);
        memcpy(dst + len1, src, len2);
    }
#endif
    TRACE(2,"%s, len=%d", __func__, len);
    return len;
}

void hal_codec_get_vad_data_info(struct CODEC_VAD_BUF_INFO_T* vad_buf_info)
{
    vad_buf_info->base_addr = CODEC_VAD_BUF_ADDR;
    vad_buf_info->buf_size = CODEC_VAD_BUF_SIZE;
    vad_buf_info->data_count = vad_data_cnt;
    vad_buf_info->addr_count = vad_addr_cnt;
}

static void hal_codec_vad_isr(uint32_t irq_status)
{
    if ((irq_status & (CODEC_VAD_FIND | CODEC_VAD_NOT_FIND)) == 0) {
        return;
    }

    TRACE(2,"%s VAD_FIND=%d", __func__, !!(irq_status & CODEC_VAD_FIND));

    ana_vad_trig = true;

    if (vad_handler) {
        vad_handler(!!(irq_status & CODEC_VAD_FIND));
    }
}

int hal_codec_vad_config(const struct AUD_VAD_CONFIG_T *conf)
{
    unsigned int cfg_set_mask = 0;
    unsigned int cfg_clr_mask = 0;

    if (!conf)
        return -1;

    vad_handler = conf->handler;

    hal_codec_vad_en(0);
    hal_codec_vad_irq_en(0);
    hal_codec_vad_flush_buff(1);
    if (conf->type == AUD_VAD_TYPE_ANA) {
        return 0;
    }

    hal_codec_vad_set_mem_mode((CODEC_VAD_MAX_BUF_SIZE - CODEC_VAD_BUF_SIZE) / 0x8000);

    hal_codec_vad_set_udc(conf->udc);
    hal_codec_vad_set_upre(conf->upre);
    hal_codec_vad_set_frame_len(conf->frame_len);
    hal_codec_vad_set_mvad(conf->mvad);
    hal_codec_vad_set_pre_gain(conf->pre_gain);
    hal_codec_vad_set_sth(conf->sth);
    hal_codec_vad_set_frame_th1(conf->frame_th[0]);
    hal_codec_vad_set_frame_th2(conf->frame_th[1]);
    hal_codec_vad_set_frame_th3(conf->frame_th[2]);
    hal_codec_vad_set_range1(conf->range[0]);
    hal_codec_vad_set_range2(conf->range[1]);
    hal_codec_vad_set_range3(conf->range[2]);
    hal_codec_vad_set_range4(conf->range[3]);
    hal_codec_vad_set_psd_th1(conf->psd_th[0]);
    hal_codec_vad_set_psd_th2(conf->psd_th[1]);
    hal_codec_vad_dig_mode(0);
    hal_codec_vad_bypass_dc(0);
    hal_codec_vad_bypass_pre(0);

    if (conf->sample_rate == AUD_SAMPRATE_8000) {
        // select adc down 8KHz
        hal_codec_vad_bypass_ds(1);
        vad_adc_down = 6;
    } else if (conf->sample_rate == AUD_SAMPRATE_16000) {
        // select adc down 16KHz
        hal_codec_vad_bypass_ds(0);
        vad_adc_down = 3;
    } else {
        ASSERT(false, "%s: Bad sample rate: %u", __func__, conf->sample_rate);
    }
    hal_codec_set_adc_down(vad_mic_ch, vad_adc_down);

    vad_adc_bits = conf->bits;
    cfg_set_mask = 0;
    cfg_clr_mask = CODEC_MODE_16BIT_ADC | CODEC_MODE_24BIT_ADC | CODEC_MODE_32BIT_ADC;
    if (vad_adc_bits == AUD_BITS_16) {
        cfg_set_mask |= CODEC_MODE_16BIT_ADC;
    } else if (vad_adc_bits == AUD_BITS_24) {
        cfg_set_mask |= CODEC_MODE_24BIT_ADC;
    } else if (vad_adc_bits == AUD_BITS_32) {
        cfg_set_mask |= CODEC_MODE_32BIT_ADC;
    } else {
        ASSERT(false, "%s: Bad cap bits: %d", __func__, vad_adc_bits);
    }
    codec->REG_040 = (codec->REG_040 & ~cfg_clr_mask) | cfg_set_mask;

    codec->REG_220 = 320;
    codec->REG_224 = 32000*3;//vad timeout value
#ifdef I2C_VAD
    codec->REG_230 |= CODEC_VAD_EXT_EN | CODEC_VAD_SRC_SEL;
#endif

#if !(defined(FIXED_CODEC_ADC_VOL) && defined(SINGLE_CODEC_ADC_VOL))
    const CODEC_ADC_VOL_T *adc_gain_db;

#ifdef SINGLE_CODEC_ADC_VOL
    adc_gain_db = hal_codec_get_adc_volume(CODEC_SADC_VOL);
#else
    adc_gain_db = hal_codec_get_adc_volume(hal_codec_get_mic_chan_volume_level(vad_mic_ch));
#endif
    if (adc_gain_db) {
        hal_codec_set_dig_adc_gain(vad_mic_ch, *adc_gain_db);
    }
#endif

    return 0;
}

int hal_codec_vad_open(const struct AUD_VAD_CONFIG_T *conf)
{
#ifndef __AUDIO_RESAMPLE__
#error "VOICE_DETECTOR_EN must work with AUDIO_RESAMPLE"
#endif

    vad_type = conf->type;

    TRACE(0, "%s: type=%d", __func__, vad_type);

    if (vad_type != AUD_VAD_TYPE_DIG && vad_mic_num == 1) {
        ASSERT((vad_mic_ch == AUD_CHANNEL_MAP_CH2), "%s: ANA VAD MIC must be MIC_C , Bad vad mic ch: %d", __func__, vad_mic_ch);
    }
    // open analog vad
    analog_aud_vad_adc_enable(true);

    hal_codec_vad_config(conf);

    return 0;
}

int hal_codec_vad_close(void)
{
    TRACE(0, "%s: type=%d", __func__, vad_type);

#ifdef I2C_VAD
    codec->REG_230 &= ~(CODEC_VAD_EXT_EN | CODEC_VAD_SRC_SEL);
#endif

    // close analog vad
    analog_aud_vad_adc_enable(false);

    vad_type = AUD_VAD_TYPE_NONE;

    return 0;
}

int hal_codec_vad_start(void)
{
    if (vad_enabled) {
        return 0;
    }
    vad_enabled = true;
    vad_data_cnt = 0;
    vad_addr_cnt = 0;

    if (vad_type == AUD_VAD_TYPE_MIX) {
        ASSERT(stream_started == 0, "VAD mix mode must use codec exclusively before ana vad trig: stream_started=0x%X", stream_started);
        ana_vad_trig = false;
    }

    // Enable TOP_OSC/CODEC_32K
    // MIX-MODE: Disable codec clock before enabling VAD (otherwise DIG VAD will start working right now)
    hal_cmu_codec_vad_clock_enable(vad_type);

    hal_codec_vad_irq_en(1);
    hal_codec_set_irq_handler(CODEC_IRQ_TYPE_VAD, hal_codec_vad_isr);

    if (vad_type == AUD_VAD_TYPE_MIX || vad_type == AUD_VAD_TYPE_DIG) {
        if (codec->REG_000 & CODEC_ADC_ENABLE) {
            ASSERT(false, "%s: VAD cannot work with cap stream", __func__);
        }
        hal_codec_vad_flush_buff(0);
        // digital vad
        hal_codec_vad_en(1);
        // enable adc if
        hal_codec_vad_adc_if_en(1);
        // enable adc
        hal_codec_vad_adc_en(1);
    }

    analog_aud_vad_enable(vad_type, true);

    return 0;
}

int hal_codec_vad_stop(void)
{
    if (!vad_enabled) {
        return 0;
    }
    vad_enabled = false;

    // Disable IRQ first
    hal_codec_vad_irq_en(0);
    hal_codec_set_irq_handler(CODEC_IRQ_TYPE_VAD, NULL);

    // Disable TOP_OSC/CODEC_32K
    // MIX-MODE: Enable codec_clock (might trigger DIG VAD to work -- it is OK since IRQ has been disabled)
    hal_cmu_codec_vad_clock_disable(vad_type);

    if (vad_type == AUD_VAD_TYPE_MIX || vad_type == AUD_VAD_TYPE_DIG) {
        // Get VAD data info before disabling VAD
        hal_codec_vad_data_info(&vad_data_cnt, &vad_addr_cnt);
        hal_codec_vad_en(0);
        hal_codec_vad_adc_if_en(0);
        hal_codec_vad_adc_en(0);
    }

    analog_aud_vad_enable(vad_type, false);

    if (vad_type == AUD_VAD_TYPE_MIX) {
        ana_vad_trig = false;
    }

    return 0;
}

#endif

static inline void hal_codec_clr_irq_status(unsigned int mask)
{
    codec->REG_00C |= mask;
}

static inline void hal_codec_trigger_irq_en(int enable)
{
    if (enable){
        codec->REG_010 |= CODEC_TSF_TRIGGER_MSK;
    }
    else{
        codec->REG_010 &= ~CODEC_TSF_TRIGGER_MSK;
    }

    codec->REG_00C |= CODEC_TSF_TRIGGER;
    codec->REG_054 |= CODEC_TSF_EVENT_SEL; // select tsf duplicate timer, not wifi tsf tiemr
}

static HAL_CODEC_IRQ_CALLBACK g_triggerCb = NULL;
static void hal_codec_trigger_tsf_isr(uint32_t irq_status)
{
    if ((irq_status & CODEC_TSF_TRIGGER) == 0) {
        return;
    }
    TRACE(0, "%s %d", __func__, !!(irq_status & CODEC_TSF_TRIGGER));

    if (g_triggerCb && (irq_status & CODEC_TSF_TRIGGER)) {
        g_triggerCb(0);
    }
}

int hal_codec_trigger_en(HAL_CODEC_IRQ_CALLBACK cb)
{
    hal_psc_codec_enable();
    hal_cmu_codec_clock_enable();
    hal_cmu_codec_reset_clear();
    hal_codec_clr_irq_status(CODEC_TSF_TRIGGER);
    hal_codec_trigger_irq_en(1);
    g_triggerCb = cb;
    hal_codec_set_irq_handler(CODEC_IRQ_TYPE_WIFI_TRIGGER, hal_codec_trigger_tsf_isr);
    return 0;
}
