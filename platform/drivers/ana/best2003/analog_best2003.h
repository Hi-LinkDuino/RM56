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
#ifndef __ANALOG_BEST2003_H__
#define __ANALOG_BEST2003_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_ANA_REG(reg)                   (((reg) & 0xFFF) | 0x1000)

#define MAX_ANA_MIC_CH_NUM                  3

enum ANA_AUD_PLL_USER_T {
    ANA_AUD_PLL_USER_CODEC      = (1 << 0),
    ANA_AUD_PLL_USER_I2S        = (1 << 1),
    ANA_AUD_PLL_USER_SPDIF      = (1 << 2),
    ANA_AUD_PLL_USER_PCM        = (1 << 3),
    ANA_AUD_PLL_USER_IIR        = (1 << 4),
    ANA_AUD_PLL_USER_RS         = (1 << 5),

    ANA_AUD_PLL_USER_END        = (1 << 6),
};
#define ANA_AUD_PLL_USER_T                  ANA_AUD_PLL_USER_T

uint32_t analog_aud_get_max_dre_gain(void);

int analog_debug_config_vad_mic(bool enable);

void analog_aud_vad_enable(enum AUD_VAD_TYPE_T type, bool en);

void analog_aud_vad_adc_enable(bool en);

void analog_aud_codec_set_dev(int speaker_dev);

int analog_aud_codec_get_dev();

void analog_aud_codec_set_pa(int speaker_pa);

#ifdef __cplusplus
}
#endif

#endif

