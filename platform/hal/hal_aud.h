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
#ifndef AUDIO_DEF_H
#define AUDIO_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CODEC_FREQ_24M                      24000000
#define CODEC_FREQ_26M                      26000000
#define CODEC_FREQ_24P576M                  24576000
#define CODEC_FREQ_22P5792M                 22579200

#define CODEC_FREQ_48K_SERIES               CODEC_FREQ_24P576M
#define CODEC_FREQ_44_1K_SERIES             CODEC_FREQ_22P5792M

#if 0
#elif defined(CHIP_BEST3001) || defined(CHIP_BEST3003) || defined(CHIP_BEST3005)

#define CODEC_FREQ_CRYSTAL                  CODEC_FREQ_24M

#define CODEC_PLL_DIV                       36
#define CODEC_CMU_DIV                       9
#ifdef CHIP_BEST3001
#define CODEC_PLAYBACK_BIT_DEPTH            20
#else
#define CODEC_PLAYBACK_BIT_DEPTH            24
#endif

#elif defined(CHIP_BEST1305) || \
    defined(CHIP_BEST1400) || defined(CHIP_BEST1402) || \
    defined(CHIP_BEST1501) || defined(CHIP_BEST1600) || \
    defined(CHIP_BEST2300) || defined(CHIP_BEST2300A) || \
    defined(CHIP_BEST2300P) || defined(CHIP_BEST1501SIMU) || defined(CHIP_BEST1600SIMU)

#if defined(CHIP_BEST1501) || defined(CHIP_BEST1501SIMU) || defined(CHIP_BEST1600SIMU)
#define CODEC_FREQ_CRYSTAL                  CODEC_FREQ_24M
#endif

#define CODEC_FREQ_EXTRA_DIV                2

#define CODEC_PLL_DIV                       16
#define CODEC_CMU_DIV                       8
#define CODEC_PLAYBACK_BIT_DEPTH            24

#elif defined(CHIP_BEST1000)

#if defined(__AUDIO_DIV_10___)
#define CODEC_PLL_DIV                       10
#elif defined(__AUDIO_DIV_9___)
#define CODEC_PLL_DIV                       9
#else
#define CODEC_PLL_DIV                       8
#endif
#define CODEC_CMU_DIV                       CODEC_PLL_DIV
#define CODEC_PLAYBACK_BIT_DEPTH            18

#elif defined(CHIP_BEST2000) || defined(CHIP_BEST2001)

#define CODEC_PLL_DIV                       32
#define CODEC_CMU_DIV                       8
#define CODEC_PLAYBACK_BIT_DEPTH            20

#elif defined(CHIP_BEST2003) || defined(CHIP_BEST2002)

#define CODEC_FREQ_CRYSTAL                  CODEC_FREQ_24M

#define CODEC_PLL_DIV                       156
#define CODEC_CMU_DIV                       13
#define CODEC_PLAYBACK_BIT_DEPTH            20

#else

#error "Please update audio definitions"

#endif

#ifndef CODEC_FREQ_CRYSTAL
#define CODEC_FREQ_CRYSTAL                  CODEC_FREQ_26M
#endif
#ifndef CODEC_FREQ_EXTRA_DIV
#define CODEC_FREQ_EXTRA_DIV                1
#endif

#define CODEC_TX_PA_GAIN_DEFAULT            -1

enum AUD_STREAM_USE_DEVICE_T{
    AUD_STREAM_USE_DEVICE_NULL = 0,
    AUD_STREAM_USE_EXT_CODEC,
    AUD_STREAM_USE_I2S0_MASTER,
    AUD_STREAM_USE_I2S0_SLAVE,
    AUD_STREAM_USE_I2S1_MASTER,
    AUD_STREAM_USE_I2S1_SLAVE,
    AUD_STREAM_USE_TDM0_MASTER,
    AUD_STREAM_USE_TDM0_SLAVE,
    AUD_STREAM_USE_TDM1_MASTER,
    AUD_STREAM_USE_TDM1_SLAVE,
    AUD_STREAM_USE_INT_CODEC,
    AUD_STREAM_USE_INT_CODEC2,
    AUD_STREAM_USE_INT_SPDIF,
    AUD_STREAM_USE_BT_PCM,
    AUD_STREAM_USE_DPD_RX,
    AUD_STREAM_USE_MC,
};

enum AUD_SAMPRATE_T {
    AUD_SAMPRATE_NULL = 0,
    AUD_SAMPRATE_7350 = 7350,
    AUD_SAMPRATE_8000 = 8000,
    AUD_SAMPRATE_8463 = 8463,       // 26M / 512 / 6
    AUD_SAMPRATE_14700 = 14700,
    AUD_SAMPRATE_16000 = 16000,
    AUD_SAMPRATE_16927 = 16927,     // 26M / 512 / 3
    AUD_SAMPRATE_22050 = 22050,
    AUD_SAMPRATE_24000 = 24000,
    AUD_SAMPRATE_32000 = 32000,
    AUD_SAMPRATE_44100 = 44100,
    AUD_SAMPRATE_48000 = 48000,
    AUD_SAMPRATE_50781 = 50781,     // 26M / 512
    AUD_SAMPRATE_64000 = 64000,
    AUD_SAMPRATE_88200 = 88200,
    AUD_SAMPRATE_96000 = 96000,
    AUD_SAMPRATE_101562 = 101562,   // 26M / 256
    AUD_SAMPRATE_128000 = 128000,
    AUD_SAMPRATE_176400 = 176400,
    AUD_SAMPRATE_192000 = 192000,
    AUD_SAMPRATE_203125 = 203125,   // 26M / 128
    AUD_SAMPRATE_256000 = 256000,
    AUD_SAMPRATE_352800 = 352800,
    AUD_SAMPRATE_384000 = 384000,
    AUD_SAMPRATE_406250 = 406250,   // 26M / 64
    AUD_SAMPRATE_705600 = 705600,
    AUD_SAMPRATE_768000 = 768000,
    AUD_SAMPRATE_812500 = 812500,   // 26M / 32
    // NOTE: DIV must be a multiple of 32
};

enum AUD_CHANNEL_NUM_T {
    AUD_CHANNEL_NUM_NULL = 0,
    AUD_CHANNEL_NUM_1 = 1,
    AUD_CHANNEL_NUM_2 = 2,
    AUD_CHANNEL_NUM_3 = 3,
    AUD_CHANNEL_NUM_4 = 4,
    AUD_CHANNEL_NUM_5 = 5,
    AUD_CHANNEL_NUM_6 = 6,
    AUD_CHANNEL_NUM_7 = 7,
    AUD_CHANNEL_NUM_8 = 8,
};

// For preprocess check
#define AUD_CHANNEL_MAP_CH0                 (1 << 0)
#define AUD_CHANNEL_MAP_CH1                 (1 << 1)
#define AUD_CHANNEL_MAP_CH2                 (1 << 2)
#define AUD_CHANNEL_MAP_CH3                 (1 << 3)
#define AUD_CHANNEL_MAP_CH4                 (1 << 4)
#define AUD_CHANNEL_MAP_CH5                 (1 << 5)
#define AUD_CHANNEL_MAP_CH6                 (1 << 6)
#define AUD_CHANNEL_MAP_CH7                 (1 << 7)
#define AUD_CHANNEL_MAP_DIGMIC_CH0          (1 << 8)
#define AUD_CHANNEL_MAP_DIGMIC_CH1          (1 << 9)
#define AUD_CHANNEL_MAP_DIGMIC_CH2          (1 << 10)
#define AUD_CHANNEL_MAP_DIGMIC_CH3          (1 << 11)
#define AUD_CHANNEL_MAP_DIGMIC_CH4          (1 << 12)
#define AUD_CHANNEL_MAP_DIGMIC_CH5          (1 << 13)
#define AUD_CHANNEL_MAP_DIGMIC_CH6          (1 << 14)
#define AUD_CHANNEL_MAP_DIGMIC_CH7          (1 << 15)
#define AUD_CHANNEL_MAP_ECMIC_CH0           (1 << 16)
#define AUD_CHANNEL_MAP_ECMIC_CH1           (1 << 17)

#define AUD_CHANNEL_MAP_ANA_ALL             (0x000000FF)
#define AUD_CHANNEL_MAP_DIGMIC_ALL          (0x0000FF00)
#define AUD_CHANNEL_MAP_NORMAL_ALL          (0x0000FFFF)
#define AUD_CHANNEL_MAP_ALL                 (0x0003FFFF)

enum AUD_CHANNEL_MAP_T {
    AUD_CHANNEL_MAP_END = AUD_CHANNEL_MAP_ECMIC_CH1,
};

enum AUD_VMIC_MAP_T {
    AUD_VMIC_MAP_VMIC1 = (AUD_CHANNEL_MAP_END << 1),
    AUD_VMIC_MAP_VMIC2 = (AUD_CHANNEL_MAP_END << 2),
    AUD_VMIC_MAP_VMIC3 = (AUD_CHANNEL_MAP_END << 3),
    AUD_VMIC_MAP_VMIC4 = (AUD_CHANNEL_MAP_END << 4),
    AUD_VMIC_MAP_VMIC5 = (AUD_CHANNEL_MAP_END << 5),
};

enum AUD_BITS_T {
    AUD_BITS_NULL = 0,
    AUD_BITS_8 = 8,
    AUD_BITS_12 = 12,
    AUD_BITS_16 = 16,
    AUD_BITS_20 = 20,
    AUD_BITS_24 = 24,
    AUD_BITS_32 = 32,
};

enum AUD_DATA_ALIGN_T {
    AUD_DATA_ALIGN_I2S = 0,
    AUD_DATA_ALIGN_LEFT_JUSTIFIED,
    AUD_DATA_ALIGN_RIGHT_JUSTIFIED,
};

enum AUD_FS_FIRST_EDGE_T {
    AUD_FS_FIRST_EDGE_NEG = 0,  // I2S mode
    AUD_FS_FIRST_EDGE_POS,      // TDM mode (likely)
};

enum AUD_STREAM_ID_T {
    AUD_STREAM_ID_0 = 0,
    AUD_STREAM_ID_1,
    AUD_STREAM_ID_2,
    AUD_STREAM_ID_3,

    AUD_STREAM_ID_NUM,
};

enum AUD_STREAM_T {
    AUD_STREAM_PLAYBACK = 0,
    AUD_STREAM_CAPTURE,

    AUD_STREAM_NUM,
};

// TODO: Need rename
enum AUD_IO_PATH_T {
    AUD_IO_PATH_NULL = 0,

    // Input path
    AUD_INPUT_PATH_MAINMIC,
    AUD_INPUT_PATH_VOICE_DEV,
    AUD_INPUT_PATH_VADMIC,
    AUD_INPUT_PATH_ASRMIC,
    AUD_INPUT_PATH_LINEIN,
    AUD_INPUT_PATH_NTMIC,
    AUD_INPUT_PATH_USBAUDIO,
    AUD_INPUT_PATH_ANC_ASSIST,
    AUD_INPUT_PATH_HEARING,

    // Output path
    AUD_OUTPUT_PATH_SPEAKER,
};

struct AUD_IO_PATH_CFG_T {
    enum AUD_IO_PATH_T io_path;
    unsigned int cfg;
};

#define ANC_TYPE_NUM    (8)
enum ANC_TYPE_T {
    ANC_NOTYPE          = 0,
    ANC_FEEDFORWARD     = (1 << 0),
    ANC_FEEDBACK        = (1 << 1),
    ANC_TALKTHRU        = (1 << 2),
    ANC_MUSICCANCLE     = (1 << 3),
    ANC_SPKCALIB        = (1 << 4),
    ANC_DEHOWLING       = (1 << 5),
    PSAP_FEEDFORWARD    = (1 << 6),
};

struct CODEC_DAC_VOL_T {
    signed char tx_pa_gain      :6;
    unsigned char sdm_gain      :2;
    signed char sdac_volume;
};

typedef signed char CODEC_ADC_VOL_T;

enum TGT_VOLUME_LEVEL_T {
    TGT_VOLUME_LEVEL_MUTE = 0,
    TGT_VOLUME_LEVEL_1,
    TGT_VOLUME_LEVEL_2,
    TGT_VOLUME_LEVEL_3,
    TGT_VOLUME_LEVEL_4,
    TGT_VOLUME_LEVEL_5,
    TGT_VOLUME_LEVEL_6,
    TGT_VOLUME_LEVEL_7,
    TGT_VOLUME_LEVEL_8,
    TGT_VOLUME_LEVEL_9,
    TGT_VOLUME_LEVEL_10,
    TGT_VOLUME_LEVEL_11,
    TGT_VOLUME_LEVEL_12,
    TGT_VOLUME_LEVEL_13,
    TGT_VOLUME_LEVEL_14,
    TGT_VOLUME_LEVEL_15,
    TGT_VOLUME_LEVEL_MAX,

    TGT_VOLUME_LEVEL_QTY
};

enum TGT_ADC_VOL_LEVEL_T {
    TGT_ADC_VOL_LEVEL_0 = 0,
    TGT_ADC_VOL_LEVEL_1,
    TGT_ADC_VOL_LEVEL_2,
    TGT_ADC_VOL_LEVEL_3,
    TGT_ADC_VOL_LEVEL_4,
    TGT_ADC_VOL_LEVEL_5,
    TGT_ADC_VOL_LEVEL_6,
    TGT_ADC_VOL_LEVEL_7,
    TGT_ADC_VOL_LEVEL_8,
    TGT_ADC_VOL_LEVEL_9,
    TGT_ADC_VOL_LEVEL_10,
    TGT_ADC_VOL_LEVEL_11,
    TGT_ADC_VOL_LEVEL_12,
    TGT_ADC_VOL_LEVEL_13,
    TGT_ADC_VOL_LEVEL_14,
    TGT_ADC_VOL_LEVEL_15,

    TGT_ADC_VOL_LEVEL_QTY
};

typedef void (*AUD_VAD_CALLBACK)(int found);

enum AUD_VAD_TYPE_T {
    AUD_VAD_TYPE_NONE = 0,
    AUD_VAD_TYPE_MIX,
    AUD_VAD_TYPE_DIG,
    AUD_VAD_TYPE_ANA,

    AUD_VAD_TYPE_NUM,
};

#define VAD_DEF_MIC_FRAME_LEN       0x50    // 16K sample rate
#define VAD_DEF_STH         0x10
#define VAD_DEF_MVAD        0x7
#define VAD_DEF_PSD_TH0     0x0
#define VAD_DEF_PSD_TH1     0x07ffffff
#define VAD_DEF_UDC         0x1//0xa
#define VAD_DEF_UPRE        0x4
#define VAD_DEF_DIG_MODE    0x1
#define VAD_DEF_PRE_GAIN    0x4
#define VAD_DEF_DC_BYPASS   0x0
#define VAD_DEF_FRAME_TH0   0x32
#define VAD_DEF_FRAME_TH1   0x1f4
#define VAD_DEF_FRAME_TH2   0x1388
#define VAD_DEF_RANGE0      0xf
#define VAD_DEF_RANGE1      0x32
#define VAD_DEF_RANGE2      0x96
#define VAD_DEF_RANGE3      0x12c

struct AUD_VAD_SIMP_CFG_T {
    enum AUD_VAD_TYPE_T type;
    enum AUD_SAMPRATE_T sample_rate;
    enum AUD_BITS_T bits;
    AUD_VAD_CALLBACK handler;
    uint8_t adc_gain;
    uint8_t frame_len;
    uint8_t frame_thresh;
    uint8_t sound_thresh;
};

struct AUD_VAD_CONFIG_T {
    enum AUD_VAD_TYPE_T type;
    enum AUD_SAMPRATE_T sample_rate;
    enum AUD_BITS_T bits;
    AUD_VAD_CALLBACK handler;

    uint8_t udc;
    uint8_t upre;
    uint8_t frame_len;
    uint8_t mvad;

    uint8_t dig_mode;
    uint8_t pre_gain;
    uint8_t sth;
    uint8_t dc_bypass;

    uint8_t ds_bypass;
    uint8_t pre_bypass;
    uint8_t adc_gain;

    uint16_t range[4];

    uint32_t frame_th[3];
    uint32_t psd_th[2];
};

struct CODEC_VAD_BUF_INFO_T {
    uint32_t base_addr;
    uint32_t buf_size;
    uint32_t data_count;
    uint32_t addr_count;
};

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_DEF_H */
