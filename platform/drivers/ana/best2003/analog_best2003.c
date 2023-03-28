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
#include "plat_types.h"
#include "analog.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "tgt_hardware.h"
#include CHIP_SPECIFIC_HDR(pll)
#include CHIP_SPECIFIC_HDR(reg_analog)

#define VCM_ON

//DAC_HIGH_OUTPUT is defined at Makefile when VCODEC=1.9V

// Not using 1uF
#define VCM_CAP_100NF

#define LOW_CODEC_BIAS

#define DAC_DC_CALIB_BIT_WIDTH              14

#define DEFAULT_VOICE_ADC_GAIN_DB           6

#ifndef ANALOG_ADC_A_GAIN_DB
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif

#ifndef ANALOG_ADC_B_GAIN_DB
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif

#ifndef ANALOG_ADC_C_GAIN_DB
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif

#ifndef LINEIN_ADC_GAIN_DB
#define LINEIN_ADC_GAIN_DB                  0
#endif

#ifndef CFG_HW_AUD_MICKEY_DEV
#define CFG_HW_AUD_MICKEY_DEV               (AUD_VMIC_MAP_VMIC1)
#endif

enum ANA_CODEC_USER_T {
    ANA_CODEC_USER_DAC          = (1 << 0),
    ANA_CODEC_USER_ADC          = (1 << 1),

    ANA_CODEC_USER_CODEC        = (1 << 2),
    ANA_CODEC_USER_MICKEY       = (1 << 3),

    ANA_CODEC_USER_VAD          = (1 << 4),
};

struct ANALOG_PLL_CFG_T {
    uint32_t freq;
    uint8_t div;
};

#define OPT_TYPE                        const

static OPT_TYPE uint16_t vcodec_mv = (uint16_t)(VCODEC_VOLT * 1000);

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static enum ANA_CODEC_USER_T adc_map[MAX_ANA_MIC_CH_NUM];
static enum ANA_CODEC_USER_T vmic_map[MAX_VMIC_CH_NUM];
static enum ANA_CODEC_USER_T codec_common_map;
static enum ANA_CODEC_USER_T adda_common_map;
static enum ANA_CODEC_USER_T vcodec_map;

static enum ANA_AUD_PLL_USER_T ana_aud_pll_map;

#ifdef DYN_ADC_GAIN
static int8_t adc_gain_offset[MAX_ANA_MIC_CH_NUM];
#endif

static const int8_t adc_db[] = { -15, -12, -9, -6, -3, 0, 3, 6 };

static const int8_t tgt_adc_db[MAX_ANA_MIC_CH_NUM] = {
    ANALOG_ADC_A_GAIN_DB, ANALOG_ADC_B_GAIN_DB,
    ANALOG_ADC_C_GAIN_DB,
};

static const int16_t adc_reg[MAX_ANA_MIC_CH_NUM] = {ANA_REG_61, ANA_REG_71, ANA_REG_81};

// Max allowed total tune ratio (5000ppm)
#define MAX_TOTAL_TUNE_RATIO                0.005000

static struct ANALOG_PLL_CFG_T ana_pll_cfg;

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div)
{
    if (ana_pll_cfg.freq == freq && ana_pll_cfg.div == div)
        return ;

    ana_pll_cfg.freq = freq;
    ana_pll_cfg.div = div;


    /*
     * 2003 codec always use resample mode and don't support pll mode. CODEC_PLL_DIV has no practical meaning.
     * I2S support bbpll and usbpll(dsi pll). Select usbpll(dsi pll) as default
     * Dsi pll go through div_psram at MIPI PHY, and I2S use this clock as source clock directly.
     * We always set div_psram as 12. CODEC_PLL_DIV is 156, so I2S source clock freq is 13 times of CODEC_FREQ
     */
    pmu_pll_div_set(HAL_CMU_PLL_DSI, PMU_PLL_DIV_PSRAM, 0x18); //div = 12

    dsi_pll_config(freq*div);
}

void analog_aud_pll_tune(float ratio)
{
    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)
    // Delta = ((SampleDiff / Fs) / TimeDiff) * PLL_cfg_val

    int64_t delta, new_pll;

    if (ana_pll_cfg.freq == 0) {
        ANALOG_INFO_TRACE(1,"%s: WARNING: aud pll config cache invalid. Skip tuning", __FUNCTION__);
        return;
    }

    if (ABS(ratio) > MAX_TOTAL_TUNE_RATIO) {
        ANALOG_INFO_TRACE(1,"\n------\nWARNING: TUNE: ratio=%d is too large and will be cut\n------\n", FLOAT_TO_PPB_INT(ratio));
        if (ratio > 0) {
            ratio = MAX_TOTAL_TUNE_RATIO;
        } else {
            ratio = -MAX_TOTAL_TUNE_RATIO;
        }
    }

    ANALOG_INFO_TRACE(2,"%s: ratio=%d", __FUNCTION__, FLOAT_TO_PPB_INT(ratio));

    new_pll = (int64_t)ana_pll_cfg.freq * ana_pll_cfg.div;
    delta = (int64_t)(new_pll * ratio);

    new_pll += delta;

    dsi_pll_config(new_pll);
}

void analog_aud_osc_clk_enable(bool enable)
{
    uint16_t val;

    if (enable) {
        analog_read(ANA_REG_16D, &val);
        val |= REG_PU_OSC | REG_CRYSTAL_SEL;
        analog_write(ANA_REG_16D, val);

        //analog_read(ANA_REG_17E, &val);
        //val |= REG_CLKMUX_LOWF_IN;
        //analog_write(ANA_REG_17E, val);
#if 0
        analog_read(ANA_REG_176, &val);
        val |= CFG_TX_CLK_INV;
        analog_write(ANA_REG_176, val);
#endif
    } else {
#if 0
        analog_read(ANA_REG_176, &val);
        val &= ~CFG_TX_CLK_INV;
        analog_write(ANA_REG_176, val);
#endif
        analog_read(ANA_REG_16D, &val);
        val &= ~(REG_PU_OSC | REG_CRYSTAL_SEL);
        analog_write(ANA_REG_16D, val);
    }
}

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_END) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {

        analog_aud_osc_clk_enable(true);
        return;
    }
#endif

    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
    ana_aud_pll_map |= user;
}

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_END) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {

        analog_aud_osc_clk_enable(false);
        return;
    }
#endif

    ana_aud_pll_map &= ~user;
    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
}

static void analog_aud_enable_dac_with_classab(uint32_t dac, bool switch_pa)
{
    uint16_t val_16e;
    uint16_t val_172;
    uint16_t val_175;
    uint16_t val_176;

    analog_read(ANA_REG_16E, &val_16e);
    analog_read(ANA_REG_172, &val_172);
    analog_read(ANA_REG_175, &val_175);
    analog_read(ANA_REG_176, &val_176);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_16e |= REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS | REG_CODEC_TX_EAR_LPBIAS;
        analog_write(ANA_REG_16E, val_16e);
        osDelay(1);
        val_176 |= CFG_TX_TREE_EN;
        analog_write(ANA_REG_176, val_176);
        osDelay(1);

        if (dac & AUD_CHANNEL_MAP_CH0) {
            val_175 |= REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_EARPA_R | REG_CODEC_TX_EN_LCLK | REG_CODEC_TX_EN_RCLK;
            if (!switch_pa) {
                val_175 |= (REG_CODEC_TX_EN_LDAC | REG_CODEC_TX_EN_RDAC);
            }
        }
        analog_write(ANA_REG_175, val_175);
        val_172 |= REG_CODEC_TX_EN_DACLDO;
        analog_write(ANA_REG_172, val_172);
        val_176 |= REG_CODEC_TX_EN_LPPA | REG_CODEC_TX_VCMO_SEL;
        analog_write(ANA_REG_176, val_176);
        osDelay(1);
        val_175 |= REG_CODEC_TX_EN_S1PA;
        val_175 |= REG_CODEC_TX_EN_S2PA;
        analog_write(ANA_REG_175, val_175);
        // Ensure 1ms delay before enabling dac_pa
        osDelay(1);
    } else {
        // Ensure 1ms delay after disabling dac_pa
        osDelay(1);
        val_175 &= ~REG_CODEC_TX_EN_S1PA;
        analog_write(ANA_REG_175, val_175);
        osDelay(1);
        val_175 &= ~(REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_EARPA_R | REG_CODEC_TX_EN_LCLK | REG_CODEC_TX_EN_RCLK);
        if (!switch_pa) {
            val_175 &= ~(REG_CODEC_TX_EN_LDAC | REG_CODEC_TX_EN_RDAC);
        }
        analog_write(ANA_REG_175, val_175);
        val_172 &= ~REG_CODEC_TX_EN_DACLDO;
        analog_write(ANA_REG_172, val_172);
        val_176 &= ~REG_CODEC_TX_EN_LPPA;
        analog_write(ANA_REG_176, val_176);
        osDelay(1);

        val_176 &= ~CFG_TX_TREE_EN;
        analog_write(ANA_REG_176, val_176);
        osDelay(1);

        val_16e &= ~(REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS | REG_CODEC_TX_EAR_LPBIAS);
        analog_write(ANA_REG_16E, val_16e);
    }
}

static void analog_aud_enable_dac_pa_classab(uint32_t dac)
{
    uint16_t val_175;

    analog_read(ANA_REG_175, &val_175);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_175 |= REG_CODEC_TX_EN_S4PA;
    } else {
        val_175 &= ~REG_CODEC_TX_EN_S4PA;
    }
    analog_write(ANA_REG_175, val_175);
}

static void analog_aud_enable_dac(uint32_t dac)
{
    analog_aud_enable_dac_with_classab(dac, false);
}

static void analog_aud_enable_dac_pa_internal(uint32_t dac)
{
    analog_aud_enable_dac_pa_classab(dac);
}

static void analog_aud_enable_dac_pa(uint32_t dac)
{
    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        analog_aud_enable_dac_pa_internal(dac);

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_clear();
#endif
    } else {
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
        hal_codec_dac_sdm_reset_set();
#endif

        analog_aud_enable_dac_pa_internal(dac);
    }
}


static void analog_aud_enable_codec_vcm_buffer(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_B2, &val);
    if (en) {
        val |= REG_CODEC_EN_VCM_BUFFER;
    } else {
        val &= ~REG_CODEC_EN_VCM_BUFFER;
    }
    analog_write(ANA_REG_B2, val);
}

POSSIBLY_UNUSED
static void analog_aud_enable_codec_bias_lp(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_B0, &val);
    if (en) {
        val |= REG_CODEC_EN_BIAS_LP;
    } else {
        val &= ~REG_CODEC_EN_BIAS_LP;
    }
    analog_write(ANA_REG_B0, val);
}

static void analog_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    int i;
    uint16_t reg;
    uint16_t val;
    enum ANA_CODEC_USER_T old_map;
    bool set;
#if !defined(CODEC_TX_PEAK_DETECT)
    bool global_update = false;
#endif

    ANALOG_DEBUG_TRACE(3,"[%s] user=%d ch_map=0x%x", __func__, user, ch_map);

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            set = false;
            if (en) {
                if (adc_map[i] == 0) {
                    set = true;
                }
                adc_map[i] |= user;
            } else {
                old_map = adc_map[i];
                adc_map[i] &= ~user;
                if (old_map != 0 && adc_map[i] == 0) {
                    set = true;
                }
            }
            if (set) {
#if !defined(CODEC_TX_PEAK_DETECT)
                if (!global_update) {
                    global_update = true;
                    if (en) {
                        analog_aud_enable_codec_vcm_buffer(true);
                        analog_aud_enable_codec_bias_lp(true);
                    }
                }
#endif
                reg = adc_reg[i];
                analog_read(reg, &val);
                if (adc_map[i]) {
                    val |= REG_CODEC_EN_ADCA;
                } else {
                    val &= ~REG_CODEC_EN_ADCA;
                }
                analog_write(reg, val);
                val |= REG_CODEC_RESET_ADCA | CFG_RESET_ADCA_DR;
                analog_write(reg, val);
                val &= ~(REG_CODEC_RESET_ADCA);
                analog_write(reg, val);
            }
        }
    }

#if !defined(CODEC_TX_PEAK_DETECT)
    if (global_update && !en) {
        for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
            if (adc_map[i]) {
                break;
            }
        }
        if (i >= MAX_ANA_MIC_CH_NUM) {
            analog_aud_enable_codec_vcm_buffer(false);
            analog_aud_enable_codec_bias_lp(false);
        }
    }
#endif
}

static uint32_t db_to_adc_gain(int db)
{
    int i;
    uint8_t cnt;
    const int8_t *list;

    list = adc_db;
    cnt = ARRAY_SIZE(adc_db);

    for (i = 0; i < cnt - 1; i++) {
        if (db < list[i + 1]) {
            break;
        }
    }

    if (i == cnt - 1) {
        return i;
    }
    else if (db * 2 < list[i] + list[i + 1]) {
        return i;
    } else {
        return i + 1;
    }
}

static int8_t get_chan_adc_gain(uint32_t i)
{
    int8_t gain;

    gain = tgt_adc_db[i];

#ifdef DYN_ADC_GAIN
    if (adc_gain_offset[i] < 0 && -adc_gain_offset[i] > gain) {
        gain = 0;
    } else {
        gain += adc_gain_offset[i];
    }
#endif

    return gain;
}

static void analog_aud_set_adc_gain(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map)
{
    int i;
    int gain;
    uint16_t gain_val;
    uint16_t val;

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            uint16_t reg = adc_reg[i];
            if (0) {
            } else if (input_path == AUD_INPUT_PATH_LINEIN) {
                gain = LINEIN_ADC_GAIN_DB;
            } else {
                gain = get_chan_adc_gain(i);
            }
            gain_val = db_to_adc_gain(gain);
            analog_read(reg, &val);
            val = SET_BITFIELD(val, REG_CODEC_ADCA_GAIN_BIT, gain_val);
            analog_write(reg, val);
        }
    }

}


#ifdef DYN_ADC_GAIN
void analog_aud_apply_adc_gain_offset(enum AUD_CHANNEL_MAP_T ch_map, int16_t offset)
{
    enum AUD_CHANNEL_MAP_T map;
    int i;

    if (ch_map) {
        map = ch_map;

        while (map) {
            i = get_msb_pos(map);
            map &= ~(1 << i);
            if (i < MAX_ANA_MIC_CH_NUM) {
                adc_gain_offset[i] = offset;
            }
        }

        ANALOG_INFO_TRACE(2,"ana: apply adc gain offset: ch_map=0x%X offset=%d", ch_map, offset);

        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
    }
}
#endif

void analog_aud_set_dac_gain(int32_t v)
{
}

uint32_t analog_codec_get_dac_gain(void)
{
    return 0;
}

uint32_t analog_codec_dac_gain_to_db(int32_t gain)
{
    return 0;
}

int32_t analog_codec_dac_max_attn_db(void)
{
    return 0;
}

static int POSSIBLY_UNUSED dc_calib_checksum_valid(uint32_t efuse)
{
    int i;
    uint32_t cnt = 0;
    uint32_t chksum_mask = (1 << (16 - DAC_DC_CALIB_BIT_WIDTH)) - 1;

    for (i = 0; i < DAC_DC_CALIB_BIT_WIDTH; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & chksum_mask) == ((efuse >> DAC_DC_CALIB_BIT_WIDTH) & chksum_mask));
}

static int16_t dc_calib_val_decode(int16_t val)
{
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    uint32_t sign_bit = (1 << (DAC_DC_CALIB_BIT_WIDTH - 1));
    uint32_t num_mask = sign_bit - 1;

    if (val & sign_bit) {
        val = -(val & num_mask);
    }
#endif
    return val;
}

void analog_aud_get_dc_calib_value(int16_t *dc_l, int16_t *dc_r)
{
    static const uint8_t EFUSE_PAGE_DIFF_1P7V[2] = { PMU_EFUSE_PAGE_DCCALIB2_L, PMU_EFUSE_PAGE_DCCALIB2_R, };
    static const uint8_t EFUSE_PAGE_DIFF_1P95V[2] = { PMU_EFUSE_PAGE_DCCALIB_L, PMU_EFUSE_PAGE_DCCALIB_R, };
    const uint8_t *page;
    uint16_t efuse;

    union DC_EFUSE_T {
        struct DC_VALUE_T {
            int16_t dc          : DAC_DC_CALIB_BIT_WIDTH;
            uint16_t checksum   : (16 - DAC_DC_CALIB_BIT_WIDTH);
        } val;
        uint16_t reg;
    };
    union DC_EFUSE_T dc;

    if (vcodec_mv >= 1900) {
        page = EFUSE_PAGE_DIFF_1P95V;
    } else {
        page = EFUSE_PAGE_DIFF_1P7V;
    }

    pmu_get_efuse(page[0], &efuse);
    if (dc_calib_checksum_valid(efuse)) {
        ANALOG_INFO_TRACE(1,"Dc calib L OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_l = dc.val.dc;
    } else {
        ANALOG_INFO_TRACE(1,"Warning: Bad dc calib efuse L: 0x%04x", efuse);
        *dc_l = 0;
    }

    pmu_get_efuse(page[1], &efuse);
    if (dc_calib_checksum_valid(efuse)) {
        ANALOG_INFO_TRACE(1,"Dc calib R OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_r = dc.val.dc;
    } else {
        ANALOG_INFO_TRACE(1,"Warning: Bad dc calib efuse R: 0x%04x", efuse);
        *dc_r = 0;
    }

    ANALOG_INFO_TRACE(2,"ANA: DC CALIB L=0x%04hX/%d R=0x%04hX/%d", *dc_l, dc_calib_val_decode(*dc_l), *dc_r, dc_calib_val_decode(*dc_r));

#if defined(ANA_DC_CALIB_L) || defined(ANA_DC_CALIB_R)
#ifdef ANA_DC_CALIB_L
    *dc_l = ANA_DC_CALIB_L;
#endif
#ifdef ANA_DC_CALIB_R
    *dc_r = ANA_DC_CALIB_R;
#endif
    ANALOG_INFO_TRACE(2,"ANA: OVERRIDE DC CALIB L=0x%04hX/%d R=0x%04hX/%d", *dc_l, dc_calib_val_decode(*dc_l), *dc_r, dc_calib_val_decode(*dc_r));
#endif

    return;
}

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
static void analog_aud_dc_calib_init(void)
{
    uint16_t val;
    int16_t dc_l, dc_r;

    analog_aud_get_dc_calib_value(&dc_l, &dc_r);

    analog_read(ANA_REG_172, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL, dc_l);
    analog_write(ANA_REG_172, val);

    analog_read(ANA_REG_173, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITR, dc_r);
    analog_write(ANA_REG_173, val);
}
#endif

void analog_aud_dc_calib_enable(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_172, &val);
    val |= REG_CODEC_TX_EAR_OFFEN;
    analog_write(ANA_REG_172, val);
}

static void analog_aud_vcodec_enable(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (vcodec_map == 0) {
            set = true;
        }
        vcodec_map |= user;
    } else {
        vcodec_map &= ~user;
        if (vcodec_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        pmu_codec_config(!!vcodec_map);
    }
}

static void analog_aud_enable_common_internal(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val_b2;
    uint16_t val_b3;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (codec_common_map == 0) {
            set = true;
        }
        codec_common_map |= user;
    } else {
        codec_common_map &= ~user;
        if (codec_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_B2, &val_b2);
        analog_read(ANA_REG_B3, &val_b3);
        if (codec_common_map) {
            val_b2 |= REG_CODEC_EN_VCM;
        } else {
            val_b2 &= ~REG_CODEC_EN_VCM;
            val_b3 &= ~REG_CODEC_VCM_EN_LPF;
        }
        if (codec_common_map) {
            // RTOS application startup time is long enougth for VCM charging
#if !(defined(VCM_ON) && defined(RTOS))
            // VCM fully stable time is about 60ms/1.95V or 150ms/1.7V
            // Quick startup:
            // 1) Disable VCM LPF and target to a higher voltage than the required one
            // 2) Wait for a short time when VCM is in quick charge (high voltage)
            // 3) Enable VCM LPF and target to the required VCM LPF voltage
            analog_write(ANA_REG_B2, SET_BITFIELD(val_b2, REG_CODEC_VCM_LOW_VCM, 0));
            uint32_t delay;

#if defined(VCM_CAP_100NF)
            if (vcodec_mv >= 1900) {
                delay = 6;
            } else {
                delay = 10;
            }
#else
            if (vcodec_mv >= 1900) {
                delay = 25;
            } else {
                delay = 100;
            }
#endif
            osDelay(delay);
#if 0
            // Target to a voltage near the required one
            analog_write(ANA_REG_6B, val_6b);
            osDelay(10);
#endif
#endif // !(VCM_ON && RTOS)
            val_b3 |= REG_CODEC_VCM_EN_LPF;
        }
        analog_write(ANA_REG_B2, val_b2);
        analog_write(ANA_REG_B3, val_b3);
    }
}


static void analog_aud_enable_codec_common(enum ANA_CODEC_USER_T user, bool en)
{
#ifndef VCM_ON
    analog_aud_enable_common_internal(user, en);
#endif
}

static void analog_aud_enable_adda_common(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val_b0;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (adda_common_map == 0) {
            set = true;
        }
        adda_common_map |= user;
    } else {
        adda_common_map &= ~user;
        if (adda_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_B0, &val_b0);
        if (adda_common_map) {
            val_b0 |= REG_CODEC_EN_BIAS;
            // bypass or pu tx regulator
        } else {
            val_b0 &= ~REG_CODEC_EN_BIAS;
        }
        analog_write(ANA_REG_B0, val_b0);

#if defined(CODEC_TX_PEAK_DETECT)
        analog_aud_enable_codec_vcm_buffer(!!adda_common_map);
#endif
    }
}


static void analog_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev)
{
    uint32_t lock;
    enum ANA_CODEC_USER_T old_map;
    bool set = false;
    int i;
    uint32_t pmu_map = 0;

    lock = int_lock();

    for (i = 0; i < MAX_VMIC_CH_NUM; i++) {
        if (dev & (AUD_VMIC_MAP_VMIC1 << i)) {
            if (vmic_map[i] == 0) {
                set = true;
            }
            vmic_map[i] |= user;
        } else {
            old_map = vmic_map[i];
            vmic_map[i] &= ~user;
            if (old_map != 0 && vmic_map[i] == 0) {
                set = true;
            }
        }

        if (vmic_map[i]) {
            pmu_map |= (AUD_VMIC_MAP_VMIC1 << i);
        }
    }

    int_unlock(lock);

    if (set) {
        pmu_codec_mic_bias_enable(pmu_map);
#ifdef VOICE_DETECTOR_EN
        pmu_codec_mic_bias_lowpower_mode(pmu_map);
#endif
        if (pmu_map) {
            osDelay(1);
        }
    }
}

uint32_t analog_aud_get_max_dre_gain(void)
{
    if (vcodec_mv >= 1800) {
        return 0x13;
    } else {
        return 0xF;
    }
}

int analog_reset(void)
{
    return 0;
}

void analog_open(void)
{
    uint16_t i;
    uint16_t val;
    uint16_t bfv;

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        val = REG_CODEC_EN_ADCA | REG_CODEC_ADCA_CH_SEL(1) | REG_CODEC_ADCA_GAIN_BIT(4);
        analog_write(adc_reg[i], val);//61
        val = REG_CODEC_ADCA_CAP_BIT1(0x7f) | REG_CODEC_ADCA_CAP_BIT2(0x1F);
        analog_write(adc_reg[i]+1, val);//62
        val = REG_CODEC_ADCA_CAP_BIT3(0x1F) |REG_CODEC_ADCA_REG_VSEL(4);
        analog_write(adc_reg[i]+2, val);//63
        val = REG_CODEC_ADCA_IBSEL_OFFSET(8) | REG_CODEC_ADCA_IBSEL_OP1(0xC) | REG_CODEC_ADCA_IBSEL_OP2(8) | REG_CODEC_ADCA_IBSEL_OP3(8);
        analog_write(adc_reg[i]+3, val);//64
        val = REG_CODEC_ADCA_IBSEL_OP4(8) | REG_CODEC_ADCA_IBSEL_REG(2) | REG_CODEC_ADCA_IBSEL_VCOMP(4) | REG_CODEC_ADCA_IBSEL_VREF(8);
        analog_write(adc_reg[i]+4, val);//65
        val = REG_CODEC_ADCA_IBSEL_VREFBUF(4) | REG_CODEC_ADCA_IBSEL_IDAC2(8) | REG_CODEC_ADCA_OFFSET_CURRENT_SEL;
        analog_write(adc_reg[i]+5, val);//66
        val = REG_CODEC_ADCA_OFFSET_BIT(0x2000) | REG_CODEC_ADCA_PU_REG;
        analog_write(adc_reg[i]+6, val);//67
        val = REG_CODEC_ADCA_VREF_SEL(7) | REG_CODEC_ADCA_VREFBUF_BIT(8) | ADCA_TIMER_RSTN_DLY(0x10);
        analog_write(adc_reg[i]+7, val);//68
        val = ADCA_OPEN_TIMER_DLY(4) | ADCA_PRE_CHARGE_TIMER_DLY(0x14) | REG_CODEC_ADCA_HP_MODE;
        analog_write(adc_reg[i]+8, val);//69
    }

    val = 0;
    analog_write(ANA_REG_B0, val);

    val = REG_CODEC_BIAS_IBSEL(8) | REG_CODEC_BIAS_IBSEL_TX(2) | REG_CODEC_BIAS_IBSEL_VOICE(8);
    analog_write(ANA_REG_B1, val);

    val = RESERVED_DIG_32 | REG_CODEC_VCM_LOW_VCM(7) | REG_CODEC_VCM_LOW_VCM_LP(7) | REG_CODEC_VCM_LOW_VCM_LPF(7);
    analog_write(ANA_REG_B2, val);

    val = REG_CODEC_BUF_LOWVCM(4);
    analog_write(ANA_REG_B3, val);

    val = REG_CODEC_TX_REGULATOR_BIT(8);
#ifdef CODEC_TX_PEAK_DETECT
    val |= REG_CODEC_TX_PEAK_NL_EN | REG_CODEC_TX_PEAK_PL_EN;
#endif
    analog_write(ANA_REG_16D, val);

    val = REG_CODEC_TX_EAR_DRE_GAIN_L(0xF);
    val |= REG_CODEC_TX_EAR_DRE_GAIN_R(0xF);
#ifdef DAC_DRE_GAIN_DC_UPDATE
    val |= REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE | REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE;
#endif
    analog_write(ANA_REG_16E, val);

    if (vcodec_mv >= 1800) {
        bfv = 0x6;
    } else {
        bfv = 5;
    }
    val = REG_CODEC_TX_DAC_VREF_L(bfv) | REG_CODEC_TX_DAC_VREF_R(bfv);
    analog_write(ANA_REG_16F, val);

    if (vcodec_mv >= 1800) {
        bfv = 0x5d;
    } else {
        bfv = 0xf3;
    }
    val = REG_CODEC_TX_EAR_COMP1(bfv);
    analog_write(ANA_REG_170, val);

    if (vcodec_mv >= 1800) {
        bfv = 3;
    } else {
        bfv = 1;
    }
    val = REG_CODEC_TX_EAR_IBSEL(bfv);
    analog_write(ANA_REG_171, val);

    val = REG_CODEC_TX_EAR_SOFTSTART(8) | REG_CODEC_TX_EAR_DR_ST(8);
    if (vcodec_mv >= 1800) {
        bfv = 2;
    } else {
        bfv = 1;
    }
    val |= REG_CODEC_TX_EAR_OUTPUTSEL(bfv);
    analog_write(ANA_REG_174, val);

    val = REG_BYPASS_CODEC_TX_REGULATOR | REG_CODEC_DAC_CLK_EDGE_SEL | REG_CODEC_TX_EAR_GAIN(1);
    analog_write(ANA_REG_175, val);

    val = REG_CODEC_TX_EAR_VCM(3);
    analog_write(ANA_REG_176, val);

    val = REG_CODEC_TX_CASN_L(2) | REG_CODEC_TX_CASP_L(2) |
        REG_CODEC_TX_CASN_R(2) | REG_CODEC_TX_CASP_R(2) |
        REG_CODEC_TX_IB_SEL_ST2(2);
    analog_write(ANA_REG_17B, val);

    //val = REG_CODEC_TX_VREF_RZ;
    //if (vcodec_mv >= 1800) {
    //    val |= REG_CODEC_TX_REG_LOWGAIN;
    //}
    val = REG_CODEC_TX_VREFBUF_CAS_L(1) | REG_CODEC_TX_VREFBUF_CAS_R(1) | REG_CODEC_TX_VREFBUF_ISEL(2);
    analog_write(ANA_REG_17C, val);

#ifdef CODEC_TX_ZERO_CROSSING_EN_GAIN
    val = REG_PU_ZERO_DET_L | REG_PU_ZERO_DET_R | REG_ZERO_DETECT_CHANGE;
    analog_write(ANA_REG_17D, val);
#endif

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    analog_aud_dc_calib_init();
    analog_aud_dc_calib_enable(true);
#endif

#ifdef VCM_ON
    analog_aud_enable_common_internal(ANA_CODEC_USER_DAC, true);
#endif
}



void analog_sleep(void)
{
#ifndef VCM_ON
    if (codec_common_map)
#endif
    {
        uint16_t val;

        analog_read(ANA_REG_B2, &val);
        val &= ~REG_CODEC_LP_VCM;
        analog_write(ANA_REG_B2, val);
    }
}

void analog_wakeup(void)
{
#ifndef VCM_ON
    if (codec_common_map)
#endif
    {
        uint16_t val;

        analog_read(ANA_REG_B2, &val);
        val &= ~REG_CODEC_LP_VCM;
        analog_write(ANA_REG_B2, val);
    }
}

void analog_aud_mickey_enable(bool en)
{
    if (en) {
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV);
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, true);
    } else {
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, 0);
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, false);
    }
}

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    if (en) {
        dev = hal_codec_get_input_path_cfg(input_path);
        // Enable vmic first to overlap vmic stable time with codec vcm stable time
        analog_aud_enable_vmic(ANA_CODEC_USER_ADC, dev);
        analog_aud_enable_codec_common(ANA_CODEC_USER_ADC, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_ADC, true);
        analog_aud_set_adc_gain(input_path, ch_map);
        analog_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, true);
    } else {
        analog_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, false);
        analog_aud_enable_adda_common(ANA_CODEC_USER_ADC, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_ADC, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_ADC, 0);
    }
}

static int analog_aud_codec_speaker_dev = -1;
void analog_aud_codec_set_dev(int speaker_dev)
{
    analog_aud_codec_speaker_dev = speaker_dev;
}
int analog_aud_codec_get_dev()
{
    return analog_aud_codec_speaker_dev;
}
static int analog_aud_codec_speaker_pa = -1;
void analog_aud_codec_set_pa(int speaker_pa)
{
    analog_aud_codec_speaker_pa = speaker_pa;
}

static void analog_aud_codec_config_speaker(void)
{
    bool en;

    if (ana_spk_req && !ana_spk_muted) {
        en = true;
    } else {
        en = false;
    }

    TRACE(0, "%s ana_spk_req = %d, ana_spk_muted = %d, en = %d", __func__, ana_spk_req, ana_spk_muted, en);

    if (ana_spk_enabled != en) {
        ana_spk_enabled = en;
        if (en) {
            TRACE(0, "open all PA");
            analog_aud_enable_dac_pa((analog_aud_codec_speaker_dev == -1) ? CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV : analog_aud_codec_speaker_dev);
#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
            hal_sys_timer_delay_us(5);
            hal_gpio_pin_set((analog_aud_codec_speaker_pa == -1)? CFG_HW_AUD_OUTPUT_POP_SWITCH : analog_aud_codec_speaker_pa);
#endif
        } else {
            TRACE(0, "close all PA");
            analog_aud_enable_dac_pa(0);
#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
            hal_gpio_pin_clr((analog_aud_codec_speaker_pa == -1)? CFG_HW_AUD_OUTPUT_POP_SWITCH : analog_aud_codec_speaker_pa);
#endif
        }
    }
}

void analog_aud_codec_speaker_enable(bool en)
{
    ana_spk_req = en;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_dac_enable(bool en)
{
    if (en) {
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, true);
        pmu_codec_hppa_enable(1);
        analog_aud_enable_dac((analog_aud_codec_speaker_dev == -1) ? CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV : analog_aud_codec_speaker_dev);
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        osDelay(1);
        analog_aud_codec_speaker_enable(true);
#endif
    } else {
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        analog_aud_codec_speaker_enable(false);
        osDelay(1);
#endif
        analog_aud_enable_dac(0);
        pmu_codec_hppa_enable(0);
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, false);
    }
}

void analog_aud_codec_open(void)
{
    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, true);
}

void analog_aud_codec_close(void)
{
    static const enum AUD_CHANNEL_MAP_T all_ch = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 |
        AUD_CHANNEL_MAP_CH2;

    analog_aud_codec_speaker_enable(false);
    osDelay(1);
    analog_aud_codec_dac_enable(false);

    analog_aud_codec_adc_enable(AUD_IO_PATH_NULL, all_ch, false);

    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, false);
}

void analog_aud_codec_mute(void)
{
#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_codec_tx_pa_gain_sel(0);
#endif

    ana_spk_muted = true;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_nomute(void)
{
    ana_spk_muted = false;
    analog_aud_codec_config_speaker();

#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_aud_set_dac_gain(dac_gain);
#endif
}

int analog_debug_config_audio_output(bool diff)
{
    return 0;
}

int analog_debug_config_codec(uint16_t mv)
{
    return 0;
}

int analog_debug_config_low_power_adc(bool enable)
{
    return 0;
}

int analog_debug_config_vad_mic(bool enable)
{
    return 0;
}

static void analog_aud_codec_vad_ini(void)
{
#if 1
    unsigned short v;

    analog_read(ANA_REG_B6, &v);
    v |= CFG_VOICE_RESET | CFG_VOICE2ADC;
    v = SET_BITFIELD(v, CFG_VOICE_PU_DELAY, 0x4);
    analog_write(ANA_REG_B6, v);

    analog_read(ANA_REG_B5, &v);
    v = SET_BITFIELD(v, REG_VOICE_HIGH_TUNE_A, 0xa);
    v = SET_BITFIELD(v, REG_VOICE_HIGH_TUNE_B, 0x4);
    v = SET_BITFIELD(v, REG_VOICE_LDO_VBIT, 0x5);
    v = SET_BITFIELD(v, REG_VOICE_LOW_TUNE, 0x3);
    analog_write(ANA_REG_B5, v);

    analog_read(ANA_REG_B9, &v);
    v = SET_BITFIELD(v, CFG_VOICE_PRECH_DELAY, 0x7f);
    v = SET_BITFIELD(v, CFG_VOICE_DET_DELAY, 0x8);
    analog_write(ANA_REG_B9, v);

    analog_read(ANA_REG_B4, &v);
    v = SET_BITFIELD(v, REG_VOICE_GAIN_A, 0x3);
    v = SET_BITFIELD(v, REG_VOICE_GAIN_B, 0x3);
    analog_write(ANA_REG_B4, v);

    analog_read(ANA_REG_B8, &v);
    v = SET_BITFIELD(v, REG_VOICE_SEL_VREF_BIN, 0xA);
    analog_write(ANA_REG_B8, v);

    osDelay(1);
#endif
}

static void analog_aud_codec_vad_trig_en(bool en)
{
#if 1
    unsigned short v;
    uint32_t lock;

    lock = int_lock();
    analog_read(ANA_REG_B6, &v);
    if (en)
        v |= CFG_VOICE_TRIG_ENABLE;
    else
        v &= ~CFG_VOICE_TRIG_ENABLE;

    analog_write(ANA_REG_B6, v);

    int_unlock(lock);
#endif
}

static void analog_aud_codec_vad_pu(bool en)
{
#if 1
    unsigned short v;
    uint32_t lock;

    lock = int_lock();
    analog_read(ANA_REG_B6, &v);
    if (en)
        v |= CFG_VOICE_POWER_ON;
    else
        v &= ~CFG_VOICE_POWER_ON;

    analog_write(ANA_REG_B6, v);
    int_unlock(lock);
#endif
}

void analog_aud_vad_enable(enum AUD_VAD_TYPE_T type, bool en)
{
    if (type == AUD_VAD_TYPE_DIG) {
        uint32_t dev;
        enum AUD_CHANNEL_MAP_T ch_map;

        dev = hal_codec_get_input_path_cfg(AUD_INPUT_PATH_VADMIC);
        ch_map = dev & AUD_CHANNEL_MAP_ANA_ALL;
        analog_aud_enable_adc(ANA_CODEC_USER_VAD, ch_map, en);
    } else if (type == AUD_VAD_TYPE_MIX || type == AUD_VAD_TYPE_ANA) {
        if (en) {
            analog_aud_codec_vad_ini();
            analog_aud_codec_vad_pu(true);
            osDelay(1);
            analog_aud_codec_vad_trig_en(true);
            //pmu_codec_vad_save_power();
        } else {
            //pmu_codec_vad_restore_power();
            // Must power off before disabling trigger
            analog_aud_codec_vad_pu(false);
            osDelay(1);
            analog_aud_codec_vad_trig_en(false);
        }
    }
}

void analog_aud_vad_adc_enable(bool en)
{
    uint32_t dev;
    enum AUD_CHANNEL_MAP_T ch_map;

    dev = hal_codec_get_input_path_cfg(AUD_INPUT_PATH_VADMIC);
    ch_map = dev & AUD_CHANNEL_MAP_ANA_ALL;

    analog_aud_osc_clk_enable(true);

    if (en) {
        // Enable vmic first to overlap vmic stable time with codec vcm stable time
        analog_aud_enable_vmic(ANA_CODEC_USER_VAD, dev);
        analog_aud_enable_codec_common(ANA_CODEC_USER_VAD, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_VAD, true);
        analog_aud_set_adc_gain(AUD_INPUT_PATH_VADMIC, ch_map);
    } else {
        analog_aud_enable_adda_common(ANA_CODEC_USER_VAD, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_VAD, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_VAD, 0);
    }
}

