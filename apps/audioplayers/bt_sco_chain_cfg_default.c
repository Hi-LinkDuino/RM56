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
/**
// TX: Transimt process
// RX: Receive process
// 16k: base 25M/208M(1300,1302) base 28M/104M(1400,1402)
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
| TX/RX | Algo                | description                       | MIPS(M)   | RAM(kB)       | Note                   |
|       |                     |                                   | 16k    8k | 16k        8k |                        |
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
|       | SPEECH_TX_DC_FILTER | Direct Current filter             | 1~2    \  | 0.05          |
|       | SPEECH_TX_AEC       | Acoustic Echo Cancellation(old)   | 40     \  |               | HWFFT: 37              |
|       | SPEECH_TX_AEC2      | Acoustic Echo Cancellation(new)   | 39     \  |               | enable NLP             |
|       | SPEECH_TX_AEC3      | Acoustic Echo Cancellation(new)   | 14/18  \  |               | 2 filters/4 filters    |
|       | SPEECH_TX_AEC2FLOAT | Acoustic Echo Cancellation(new)   | 23/22  \  | 29.3          | nlp/af(blocks=1)       |
|       | SPEECH_TX_AEC2FLOAT | Acoustic Echo Cancellation(ns)    | 14/10  \  |               | banks=256/banks=128    |
|       | (ns_enabled)        |                                   | 8/6    \  |               | banks=64/banks=32      |
|       | SPEECH_TX_NS        | 1 mic noise suppress(old)         | 30     \  |               | HWFFT: 19              |
|       | SPEECH_TX_NS2       | 1 mic noise suppress(new)         | 16     \  |               | HWFFT: 12              |
|       | SPEECH_TX_NS3       | 1 mic noise suppress(new)         | 26     \  | 33.3          |                        |
|       | SPEECH_TX_NN_NS     | 1 mic noise suppress(new)         | 117/52 \  | 78.7/20.3     | 16ms; large/small      |
|       | SPEECH_TX_NN_NS2    | 1 mic noise suppress(new)         | 37        | 30.3          |
|       | SPEECH_TX_NS2FLOAT  | 1 mic noise suppress(new float)   | 12.5   \  |               | banks=64               |
| TX    | SPEECH_TX_2MIC_NS   | 2 mic noise suppres(long space)   | \         |               |                        |
|       | SPEECH_TX_2MIC_NS2  | 2 mic noise suppres(short space)  | 20        | 14.8          | delay_taps 5M          |
|       |                     |                                   |           |               | freq_smooth_enable 1.5 |
|       |                     |                                   |           |               | wnr_enable 1.5M        |
|       | SPEECH_TX_2MIC_NS4  | sensor mic noise suppress         | 31.5      |               |                        |
|       | SPEECH_TX_2MIC_NS3  | 2 mic noise suppres(far field)    | \         |               |                        |
|       | SPEECH_TX_2MIC_NS5  | 2 mic noise suppr(left right end) | \         |               |                        |
|       | SPEECH_TX_2MIC_NS6  | 2 mic noise suppr(far field)      | 70        |               |                        |
|       | SPEECH_TX_2MIC_NS7  | 2 mic noise suppr(Cohernt&RLS)    | 58.4      | 38.9          |                        |
|       | SPEECH_TX_2MIC_NS8  | 2 mic noise suppr(DSB)            | 3.5    \  |               |                        |
|       | SPEECH_TX_3MIC_NS   | 3 mic 2FF+FB NS(new)              | 80        | 33.1          | Wnr_enable = 0         |
|       |                     |                                   |           |               | include 3mic_preprocess|
|       | SPEECH_TX_3MIC_NS2  | 3 mic 2FF+FB NS (Cohernt&RLS)     | 62        | 58.7          |                        |
|       | SPEECH_TX_3MIC_NS4  | 3 mic 2FF+FB                      | 67        | 54.6          | Wnr_enable = 0         |
|       |                     |                                   |           |               | include 3mic_preprocess|
|       | SPEECH_TX_AGC       | Automatic Gain Control            | 3         | 0.4           |                        |
|       | SPEECH_TX_COMPEXP   | Compressor and expander           | 4         | 0.6           |                        |
|       | SPEECH_TX_EQ        | Default EQ                        | 0.5     \ | 1.1           | each section           |
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
|       | SPEECH_RX_NS        | 1 mic noise suppress(old)         | 30      \ |               |                        |
| RX    | SPEECH_RX_NS2       | 1 mic noise suppress(new)         | 16      \ |               |                        |
|       | SPEECH_RX_NS2FLOAT  | 1 mic noise suppress(new float)   | 12.5   \  | 17.9          | banks=64               |
|       | SPEECH_RX_AGC       | Automatic Gain Control            | 3         | 0.4           |                        |
|       | SPEECH_RX_EQ        | Default EQ                        | 0.5     \ | 1.1           | each section           |
| ----- | ------------------- | --------------------------------- | --------- | ------------- | ---------------------- |
**/
#include "plat_types.h"
#include "bt_sco_chain_cfg.h"

#if defined(SPEECH_TX_2MIC_NS4) || defined(SPEECH_TX_3MIC_NS)
static float ff_fb_h[256] = {1.f, };
#endif

#if defined(SPEECH_TX_MIC_CALIBRATION)
/****************************************************************************************************
 * Describtion:
 *     Mic Calibration Equalizer, implementation with 2 order iir filters
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     mic_num(1~4): the number of microphones. The filter num is (mic_num - 1)
 *     calib: {bypass, gain, num, {type, frequency, gain, q}}. Please refer to SPEECH_TX_EQ section
 *         in this file
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 0.5M/section;
 * Note:
 *     None
****************************************************************************************************/
const SpeechIirCalibConfig WEAK speech_tx_mic_calib_cfg =
{
    .bypass = 0,
    .mic_num = SPEECH_CODEC_CAPTURE_CHANNEL_NUM,
    .calib = {
        {
            .bypass = 0,
            .gain = 0.f,
            .num = 0,
            .params = {
                {IIR_BIQUARD_LOWSHELF, {{150, -2.5, 0.707}}},
            }
        },
    },
};
#endif

#if defined(SPEECH_TX_MIC_FIR_CALIBRATION)
/****************************************************************************************************
 * Describtion:
 *     Mic Calibration Equalizer, implementation with fir filter
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     mic_num(1~4): the number of microphones. The filter num is (mic_num - 1)
 *     calib: {filter, filter_length, nfft}. 
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 0.5M/section;
 * Note:
 *     None
****************************************************************************************************/
float mic2_ft_caliration[256] = {1.f, };
const SpeechFirCalibConfig WEAK speech_tx_mic_fir_calib_cfg =
{
    .bypass = 0,
    .mic_num = SPEECH_CODEC_CAPTURE_CHANNEL_NUM,
    .delay = 0,
    .calib = {
        {
            .filter = mic2_ft_caliration,
            .filter_length = ARRAY_SIZE(mic2_ft_caliration),
        },
    },
};
#endif

const SpeechConfig WEAK speech_cfg_default = {

#if defined(SPEECH_TX_DC_FILTER)
    .tx_dc_filter = {
        .bypass                 = 0,
        .gain                   = 0,
    },
#endif

#if defined(SPEECH_TX_AEC)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     delay(>0): delay samples from mic to speak, unit(sample).
 *     leak_estimate(0~32767): echo supression value. This is a fixed mode. It has relatively large
 *         echo supression and large damage to speech signal.
 *     leak_estimate_shift(0~8): echo supression value. if leak_estimate == 0, then leak_estimate_shift
 *         can take effect. This is a adaptive mode. It has relatively small echo supression and also 
 *         small damage to speech signal.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 40M;
 * Note:
 *     If possible, use SPEECH_TX_AEC2FLOAT
****************************************************************************************************/
    .tx_aec = {
        .bypass                 = 0,
        .delay                  = 60,
        .leak_estimate          = 16383,
        .leak_estimate_shift    = 4,
    },
#endif

#if defined(SPEECH_TX_AEC2)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     .
 *     .
 *     .
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, enNlpEnable = 1, 39M;
 * Note:
 *     If possible, use SPEECH_TX_AEC2FLOAT
****************************************************************************************************/
    .tx_aec2 = {
        .bypass                 = 0,
        .enEAecEnable           = 1,
        .enHpfEnable            = 1,
        .enAfEnable             = 0,
        .enNsEnable             = 0,
        .enNlpEnable            = 1,
        .enCngEnable            = 0,
        .shwDelayLength         = 0,
        .shwNlpBandSortIdx      = 0.75f,
        .shwNlpBandSortIdxLow   = 0.5f,
        .shwNlpTargetSupp       = 3.0f,
        .shwNlpMinOvrd          = 1.0f,
    },
#endif

#if defined(SPEECH_TX_AEC2FLOAT)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     hpf_enabled(0/1): high pass filter enable or disable. Used to remove DC for Near and Ref signals.
 *     af_enabled(0/1): adaptive filter enable or disable. If the echo signal is very large, enable this
 *     nlp_enabled(0/1): non-linear process enable or disable. Enable this by default.
 *     ns_enabled(0/1): noise supression enable or disable. Enable this by default.
 *     cng_enabled(0/1):  comfort noise generator enable or disable.
 *     blocks(1~8): the length of adaptive filter = blocks * frame length
 *     delay(>0): delay samples from mic to speak, unit(sample).
 *     gamma(0~1): forgetting factor for psd estimation
 *     echo_band_start(0~8000): start band for echo detection, unit(Hz)
 *     echo_band_end(echo_band_start~8000): end band for echo detection, unit(Hz)
 *     min_ovrd(1~6): supression factor, min_ovrd becomes larger and echo suppression becomes larger.
 *     target_supp(<0): target echo suppression, unit(dB)
 *     noise_supp(-30~0): noise suppression, unit(dB)
 *     cng_type(0/1): noise type(0: White noise; 1: Pink noise)
 *     cng_level(<0): noise gain, unit(dB)
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     This is the recommended AEC
****************************************************************************************************/
    .tx_aec2float = {
        .bypass         = 0,
        .hpf_enabled    = false,
        .af_enabled     = false,
        .nlp_enabled    = true,
        .clip_enabled   = false,
        .stsupp_enabled = false,
#if defined(SPEECH_TX_NS3)
        .ns_enabled     = false,
#else
        .ns_enabled     = true,
#endif
        .cng_enabled    = false,
        .blocks         = 1,
#if defined(SPEECH_TX_AEC_CODEC_REF)
        .delay          = 70,
#else
        .delay          = 139,
#endif
        .gamma          = 0.9,
        .echo_band_start = 300,
        .echo_band_end  = 1800,
        .min_ovrd       = 2,
        .target_supp    = -40,
        .noise_supp     = -15,
        .cng_type       = 1,
        .cng_level      = -60,
        .clip_threshold = -20.f,
        .banks          = 64,
    },
#endif

#if defined(SPEECH_TX_AEC3)
    .tx_aec3 = {
        .bypass         = 0,
        .filter_size     = 16,
    },
#endif

#if defined(SPEECH_TX_2MIC_NS)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns = {
        .bypass             = 0,
        .alpha_h            = 0.8,
        .alpha_slow         = 0.9,
        .alpha_fast         = 0.6,
        .thegma             = 0.01,
        .thre_corr          = 0.2,
        .thre_filter_diff   = 0.2,
        .cal_left_gain      = 1.0,
        .cal_right_gain     = 1.0,
        .delay_mono_sample  = 6,
        .wnr_enable         = 0,
    },
#endif

#if defined(SPEECH_TX_2MIC_NS2)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     delay_taps(0~4): ceil{[d(MIC1~mouth) - d(MIC2~mouth)] / 2}.
 *         ceil: Returns the largest integer less than or equal to the specified data
 *         d(MIC~mouth): The dinstance from MIC to mouth
 *         e.g. 0: 0~2cm; 1: 2~4; 2: 5~6...
 *     freq_smooth_enable(1): Must enable
 *     wnr_enable(0/1): wind noise reduction enable or disable. This is also useful for improving
 *         noise suppression, but it also has some damage to speech signal. 
 *     skip_freq_range_start(0~8000): skip dualmic process between skip_freq_range_start and skip_freq_range_end
 *     skip_freq_range_end(0~8000): skip dualmic process between skip_freq_range_start and skip_freq_range_end
 *     betalow(-0.1~0.1): suppression factor for frequency below 500Hz, large means more suppression
 *     betamid(-0.2~0.2): suppression factor for frequency between 500Hz and 2700Hz, large means more suppression
 *     betahigh(-0.3~0.3): suppression factor for frequency between 2700Hz and 8000Hz, large means more suppression
 *     vad_threshold(-1~0): threshold for vad, large means detection is more likely to be triggered
 *     vad_threshold(0~0.1): flux for threshold to avoid miss detection
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 32M;
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns2 = {
        .bypass             = 0,
        .delay_taps         = 0.f,  
        .dualmic_enable     = 1,
        .freq_smooth_enable = 1,
        .wnr_enable         = 1, 
        .skip_freq_range_start = 0,
        .skip_freq_range_end = 0,       // TODO: Jay: Audio developer has a bug, int type can not be negative
        .noise_supp         = -40,
        .betalow            = 0.07f, // 500
        .betamid            = -0.1f, // 2700
        .betahigh           = -0.2f, // above 2700
        .vad_threshold      = 0.f,
        .vad_threshold_flux = 0.05f,
    },
#endif

#if defined(SPEECH_TX_2MIC_NS5)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     delay_taps(0~4): ceil{[d(MIC1~mouth) - d(MIC2~mouth)] / 2}. Default as 0
 *         ceil: Returns the largest integer less than or equal to the specified data
 *         d(MIC~mouth): The dinstance from MIC to mouth
 *         e.g. 0: 0~2cm; 1: 2~4; 2: 5~6...
 *     freq_smooth_enable(1): Must enable
 *     wnr_enable(0/1): wind noise reduction enable or disable. This is also useful for improving
 *         noise suppression, but it also has some damage to speech signal. 
 *     delay_enable(0/1): enable the delay_taps or not. Ideally, never need to enable the delay and
 *          delay_taps will be a useless parameter.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 32M;
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns5 = {
        .bypass             = 0,
        .delay_taps         = 0.0f,
        .freq_smooth_enable = 1,
        .wnr_enable         = 0, 
        .delay_enable       = 0,
    },
#endif


#if defined(SPEECH_TX_2MIC_NS6)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     wnr_enable(0/1): wind noise reduction enable or disable. This is also useful for improving
 *         noise suppression, but it also has some damage to speech signal. 
 *     denoise_dB : The minimum gain in the MMSE NS algorithm which is integrated in wind noise.
 * Resource consumption:
 *     RAM:     TBD
 *     FLASE:   TBD
 *     MIPS:    fs = 16kHz, TBD;
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns6 = {
        .bypass             = 0,
        .wnr_enable         = 0, 
        .denoise_dB       = -12,
    },
#endif

#if defined(SPEECH_TX_2MIC_NS4)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns4 = {
        .bypass             = 0,
        .blend_en           = 1,
        // .left_gain          = 1.0f,
        // .right_gain         = 1.0f,
        .delay_tapsM        = 0,
        .delay_tapsS        = 0,
        // .delay_tapsC        = 32,
        //////////////////////////{{a0,a1,a2,a3,a4},{b0,b1,b2,b3,b4}}///////////////////////////
        // .coefH[0]           = {1.0, -1.88561808316413, 1.55555555555556, -0.628539361054709, 0.111111111111111},
        // .coefH[1]           = {0.323801506930344, -1.29520602772138, 1.94280904158206, -1.29520602772138, 0.323801506930344},
        // .coefL[0]           = {1.0, -1.88561808316413, 1.55555555555556, -0.628539361054709, 0.111111111111111},
        // .coefL[1]           = {0.00953182640298944, 0.0381273056119578, 0.0571909584179366, 0.0381273056119578, 0.00953182640298944},
        .crossover_freq     = 1000,
        .ff_fb_coeff        = ff_fb_h,
        .ff_fb_coeff_len    = ARRAY_SIZE(ff_fb_h),
    },
#endif

#if defined(SPEECH_TX_2MIC_NS8)
/****************************************************************************************************
 * Describtion:
 *     2 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     delay_taps(0~4): ceil{[d(MIC1~mouth) - d(MIC2~mouth)] / 2}. Default as 0
 *         ceil: Returns the largest integer less than or equal to the specified data
 *         d(MIC~mouth): The dinstance from MIC to mouth
 *         e.g. 0: 0~2cm; 1: 2~4; 2: 5~6...
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 32M;
 * Note:
 *     None
****************************************************************************************************/
    .tx_2mic_ns8 = {
        .bypass             = 0,
        .delay_taps         = 1.5f,
        .ref_pwr_threshold  = 30.f,
    },
#endif

#if defined(SPEECH_TX_3MIC_NS)
/****************************************************************************************************
 * Describtion:
 *     3 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     delay_tapsM(0~4): MIC L/R delay samples. Refer to SPEECH_TX_2MIC_NS2 delay_taps
 *     delay_tapsS(0~4): MIC L/S delay samples. Refer to SPEECH_TX_2MIC_NS2 delay_taps
 *     freq_smooth_enable(1): Must enable
 *     wnr_enable(0/1): wind noise reduction enable or disable. This is also useful for improving
 *         noise suppression, but it also has some damage to speech signal. 
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_3mic_ns = {
		.bypass = 0,
		.blend_en = 1,
		.authen_en = 0,
		.delay_tapsM = 0.f,
		.delay_tapsS = 4.f,
		.denoise_dB = -6.0f,
		.crossover_freq = 500,
		.freq_smooth_enable = 1,
		.wnr_enable = 1,
		.noise_supp = -20,
		.betalow = -0.05f,
		.betamid = -0.1f,
		.betahigh = -0.2f,
		.ref_pwr_threshold = 2,
		.ff_fb_coeff = ff_fb_h,
		.ff_fb_coeff_len = ARRAY_SIZE(ff_fb_h),
		.eq_cfg = {
			.bypass = 1,
			.gain = 5.f,
			.num = 1,
			.params = {
				{IIR_BIQUARD_HPF, {{60, 0, 0.707}}},
			}
		}
    },
#endif

#if defined(SPEECH_TX_3MIC_NS2)
/****************************************************************************************************
 * Describtion:
 *     3 MICs Noise Suppression2
 * Parameters:

 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_3mic_ns2 = {
		.bypass      = 0,
		.wnr_enable  = 1,
		.denoise_dB  = -6,
		.delay_taps  = 0.6,
		.freq_smooth_enable = 1,
		.crossover_freq = 1000,
	},
#endif

#if defined(SPEECH_TX_3MIC_NS3)
/****************************************************************************************************
 * Describtion:
 *     3 MICs Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     delay_taps(0~4): ceil{[d(MIC1~mouth) - d(MIC2~mouth)] / 2}.
 *         ceil: Returns the largest integer less than or equal to the specified data
 *         d(MIC~mouth): The dinstance from MIC to mouth
 *         e.g. 0: 0~2cm; 1: 2~4; 2: 5~6...
 *     freq_smooth_enable(1): Must enable
 *     wnr_enable(0/1): wind noise reduction enable or disable. This is also useful for improving
 *         noise suppression, but it also has some damage to speech signal. 
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 32M;
 * Note:
 *     None
****************************************************************************************************/
    .tx_3mic_ns3 = {
        .bypass             = 0,
        .endfire_enable     = 1,
        .broadside_enable   = 1,
        .delay_taps         = 0.7f,  
        .freq_smooth_enable = 1,
        .wnr_enable         = 0, 
    },
#endif

#if defined(SPEECH_TX_NS)
/****************************************************************************************************
 * Describtion:
 *     Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 30M;
 * Note:
 *     If possible, use SPEECH_TX_NS2 or SPEECH_TX_NS2FLOAT
****************************************************************************************************/
    .tx_ns = {
        .bypass     = 0,
        .denoise_dB = -12,
    },
#endif

#if defined(SPEECH_TX_NS2)
/****************************************************************************************************
 * Describtion:
 *     Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     denoise_dB(-30~0): noise suppression, unit(dB). 
 *         e.g. -15: Can reduce 15dB of stationary noise.
 * Resource consumption:
 *     RAM:     fs = 16k:   RAM = 8k; 
 *              fs = 8k:    RAM = 4k;
 *              RAM = frame_size * 30
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 16M;
 * Note:
 *     None
****************************************************************************************************/
    .tx_ns2 = {
        .bypass     = 0,
        .denoise_dB = -15,
    },
#endif

#if defined(SPEECH_TX_NS2FLOAT)
/****************************************************************************************************
 * Describtion:
 *     Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     denoise_dB(-30~0): noise suppression, unit(dB). 
 *         e.g. -15: Can reduce 15dB of stationary noise.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     This is a 'float' version for SPEECH_TX_NS2. 
 *     It needs more MIPS and RAM, but can redece quantization noise.
****************************************************************************************************/
    .tx_ns2float = {
        .bypass     = 0,
        .denoise_dB = -15,
        .banks      = 64,
    },
#endif

#if defined(SPEECH_TX_NS3)
/****************************************************************************************************
 * Describtion:
 *     Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     mode: None
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_ns3 = {
        .bypass = 0,
        .denoise_dB = -18,
    },
#endif

#if defined(SPEECH_TX_WNR)
/****************************************************************************************************
 * Describtion:
 *     Wind Noise Suppression
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     lpf_cutoff: lowpass filter for wind noise detection
 *     hpf_cutoff: highpass filter for wind noise suppression
 *     power_ratio_thr: ratio of the power spectrum of the lower frequencies over the total power
                        spectrum for all frequencies
 *     power_thr: normalized power of the low frequencies
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_wnr = {
        .bypass = 0,
        .lpf_cutoff   = 1000,
        .hpf_cutoff = 400,
        .power_ratio_thr = 0.9f,
        .power_thr = 1.f,
    },
#endif

#if defined(SPEECH_TX_NOISE_GATE)
/****************************************************************************************************
 * Describtion:
 *     Noise Gate
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     data_threshold(0~32767): distinguish between noise and speech, unit(sample).
 *     data_shift(1~3): 1: -6dB; 2: -12dB; 3: -18dB
 *     factor_up(float): attack time, unit(s)
 *     factor_down(float): release time, unit(s)
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_noise_gate = {
        .bypass         = 0,
        .data_threshold = 640,
        .data_shift     = 1,
        .factor_up      = 0.001f,
        .factor_down    = 0.5f,
    },
#endif

#if defined(SPEECH_TX_COMPEXP)
/****************************************************************************************************
 * Describtion:
 *     Compressor and expander
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     ...
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_compexp = {
        .bypass             = 0,
        .comp_threshold     = -10.f,
        .comp_ratio         = 2.f,
        .expand_threshold   = -45.f,
        .expand_ratio       = 0.555f,
        .attack_time        = 0.001f,
        .release_time       = 0.6f,
        .makeup_gain        = 6,
        .delay              = 128,
    },
#endif

#if defined(SPEECH_TX_AGC)
/****************************************************************************************************
 * Describtion:
 *     Automatic Gain Control
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     target_level(>0): signal can not exceed (-1 * target_level)dB.
 *     compression_gain(>0): excepted gain.
 *     limiter_enable(0/1): 0: target_level does not take effect; 1: target_level takes effect.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 3M;
 * Note:
 *     None
****************************************************************************************************/
    .tx_agc = {
        .bypass             = 0,
        .target_level       = 3,
        .compression_gain   = 6,
        .limiter_enable     = 1,
    },
#endif

#if defined(SPEECH_TX_EQ)
/****************************************************************************************************
 * Describtion:
 *     Equalizer, implementation with 2 order iir filters
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 *     gain(float): normalized gain. It is usually less than or equal to 0
 *     num(0~6): the number of EQs
 *     params: {type, frequency, gain, q}. It supports a lot of types, please refer to iirfilt.h file
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz, 0.5M/section;
 * Note:
 *     None
****************************************************************************************************/
    .tx_eq = {
        .bypass     = 0,
        .gain       = 0.f,
        .num        = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif

#if defined(SPEECH_TX_POST_GAIN)
/****************************************************************************************************
 * Describtion:
 *     Linear Gain
 * Parameters:
 *     bypass(0/1): bypass enable or disable.
 * Resource consumption:
 *     RAM:     None
 *     FLASE:   None
 *     MIPS:    fs = 16kHz;
 * Note:
 *     None
****************************************************************************************************/
    .tx_post_gain = {
        .bypass     = 0,
        .gain_dB    = 6.0f,
    },
#endif

// #if defined(SPEECH_CS_VAD)
// /****************************************************************************************************
//  * Describtion:
//  *     Voice Activity Detector
//  * Parameters:
//  *     bypass(0/1): bypass enable or disable.
//  * Resource consumption:
//  *     RAM:     None
//  *     FLASE:   None
//  *     MIPS:    fs = 16kHz;
//  * Note:
//  *     None
// ****************************************************************************************************/
//     .tx_vad = {
//         .snrthd     = 5.f,
//         .energythd  = 100.f,
//     },
// #endif

#if defined(SPEECH_RX_NS)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     Refer to SPEECH_TX_NS
 * Note:
 *     None
****************************************************************************************************/
    .rx_ns = {
        .bypass     = 0,
        .denoise_dB = -12,
    },
#endif

#if defined(SPEECH_RX_NS2)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     Refer to SPEECH_TX_NS2
 * Note:
 *     None
****************************************************************************************************/
    .rx_ns2 = {
        .bypass     = 0,
        .denoise_dB = -15,
    },
#endif

#if defined(SPEECH_RX_NS2FLOAT)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     Refer to SPEECH_TX_NS2FLOAT
 * Note:
 *     None
****************************************************************************************************/
    .rx_ns2float = {
        .bypass     = 0,
        .denoise_dB = -15,
        .banks      = 64,
    },
#endif

#if defined(SPEECH_RX_NS3)
/****************************************************************************************************
 * Describtion:
 *     Acoustic Echo Cancellation
 * Parameters:
 *     Refer to SPEECH_TX_NS3
 * Note:
 *     None
****************************************************************************************************/
    .rx_ns3 = {
        .bypass = 0,
        .mode   = NS3_SUPPRESSION_HIGH,
    },
#endif

#if defined(SPEECH_RX_NOISE_GATE)
/****************************************************************************************************
 * Describtion:
 *     Noise Gate
 * Parameters:
 *     Refer to SPEECH_TX_NOISE_GATE
 * Note:
 *     None
****************************************************************************************************/
    .rx_noise_gate = {
        .bypass         = 0,
        .data_threshold = 640,
        .data_shift     = 1,
        .factor_up      = 0.001f,
        .factor_down    = 0.5f,
    },
#endif

#if defined(SPEECH_RX_COMPEXP)
/****************************************************************************************************
 * Describtion:
 *     Compressor and expander
 * Parameters:
 *     Refer to SPEECH_TX_COMPEXP
 * Note:
 *     None
****************************************************************************************************/
    .rx_compexp = {
        .bypass = 0,
        .num = 2,
        .xover_freq = {5000},
        .order = 4,
        .params = {
            {
                .bypass             = 0,
                .comp_threshold     = -10.f,
                .comp_ratio         = 2.f,
                .expand_threshold   = -60.f,
                .expand_ratio       = 0.5556f,
                .attack_time        = 0.01f,
                .release_time       = 0.6f,
                .makeup_gain        = 0,
                .delay              = 128,
            },
            {
                .bypass             = 0,
                .comp_threshold     = -10.f,
                .comp_ratio         = 2.f,
                .expand_threshold   = -60.f,
                .expand_ratio       = 0.5556f,
                .attack_time        = 0.01f,
                .release_time       = 0.6f,
                .makeup_gain        = 0,
                .delay              = 128,
            }
        }
    },
#endif

#if defined(SPEECH_RX_AGC)
/****************************************************************************************************
 * Describtion:
 *      Automatic Gain Control
 * Parameters:
 *     Refer to SPEECH_TX_AGC
 * Note:
 *     None
****************************************************************************************************/
    .rx_agc = {
        .bypass             = 0,
        .target_level       = 3,
        .compression_gain   = 6,
        .limiter_enable     = 1,
    },
#endif

#if defined(SPEECH_RX_EQ)
/****************************************************************************************************
 * Describtion:
 *     Equalizer, implementation with 2 order iir filters
 * Parameters:
 *     Refer to SPEECH_TX_EQ
 * Note:
 *     None
****************************************************************************************************/
    .rx_eq = {
        .bypass = 0,
        .gain   = 0.f,
        .num    = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif

#if defined(SPEECH_RX_HW_EQ)
/****************************************************************************************************
 * Describtion:
 *     Equalizer, implementation with 2 order iir filters
 * Parameters:
 *     Refer to SPEECH_TX_EQ
 * Note:
 *     None
****************************************************************************************************/
    .rx_hw_eq = {
        .bypass = 0,
        .gain   = 0.f,
        .num    = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif

#if defined(SPEECH_RX_DAC_EQ)
/****************************************************************************************************
 * Describtion:
 *     Equalizer, implementation with 2 order iir filters
 * Parameters:
 *     Refer to SPEECH_TX_EQ
 * Note:
 *     None
****************************************************************************************************/
    .rx_dac_eq = {
        .bypass = 0,
        .gain   = 0.f,
        .num    = 1,
        .params = {
            {IIR_BIQUARD_HPF, {{60, 0, 0.707f}}},
        },
    },
#endif

#if defined(SPEECH_RX_POST_GAIN)
/****************************************************************************************************
 * Describtion:
 *     Linear Gain
 * Parameters:
 *     Refer to SPEECH_TX_POST_GAIN
 * Note:
 *     None
****************************************************************************************************/
    .rx_post_gain = {
        .bypass     = 0,
        .gain_dB    = 6.0f,
    },
#endif

};
