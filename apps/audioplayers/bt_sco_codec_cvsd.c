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
#include <string.h>
#include "plat_types.h"
#include "hal_trace.h"
#include "audio_dump.h"
#include "plc_utils.h"
#include "speech_cfg.h"
#include "bt_drv.h"
#include "bt_sco_codec.h"

#define ENABLE_LPC_PLC

// #define PLC_DEBUG_PRINT_DATA

#if defined(CVSD_BYPASS)
#include "Pcm8k_Cvsd.h"
#endif

#define CVSD_PACKET_SIZE (120)

#define CVSD_PACKET_NUM (2)

#define CVSD_MUTE_PATTERN (0x55)

#define CVSD_PCM_SIZE (120)

#if defined(ENABLE_LPC_PLC)
#include "lpc_plc_api.h"
static LpcPlcState *plc_st = NULL;
#else
#include "plc_8000.h"
static  void *plc_st;
#endif

#if defined(CVSD_BYPASS) && defined(ENABLE_LPC_PLC)
static uint8_t *cvsd_buf_all = NULL;

plc_type_t check_cvsd_mute_pattern(uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        if (buf[i] != CVSD_MUTE_PATTERN)
            return PLC_TYPE_PASS;

    return PLC_TYPE_CONTROLLER_MUTE;
}
#endif

int32_t bt_sco_codec_cvsd_init(uint32_t sample_rate)
{
    TRACE(0, "[%s] SCO: CVSD...", __func__);

#if defined(CVSD_BYPASS) && defined(ENABLE_LPC_PLC)
    // 120 + 120 = 240 bytes
    cvsd_buf_all = (uint8_t *)speech_calloc(CVSD_PACKET_SIZE + CVSD_OFFSET_BYTES, sizeof(uint8_t));
    memset(cvsd_buf_all, CVSD_MUTE_PATTERN & 0xFF, (CVSD_PACKET_SIZE + CVSD_OFFSET_BYTES) * sizeof(uint8_t));
#endif

#if defined(CVSD_BYPASS)
    Pcm8k_CvsdInit();
#endif

#if defined(ENABLE_LPC_PLC)
    plc_st = lpc_plc_create(sample_rate);
#else
    plc_st = (PlcSt_8000 *)speech_plc_8000_init(voicebtpcm_get_ext_buff);
#endif

    return 0;
}

int32_t bt_sco_codec_cvsd_deinit(void)
{
    TRACE(0, "[%s] ...", __func__);

#if defined(ENABLE_LPC_PLC)
    lpc_plc_destroy(plc_st);
#endif

#if defined(CVSD_BYPASS) && defined(ENABLE_LPC_PLC)
    speech_free(cvsd_buf_all);
#endif

    return 0;
}

// cvsd_size = 240, pcm_size = 240
int32_t bt_sco_codec_cvsd_encoder(uint8_t *cvsd_buf, uint32_t cvsd_size, uint8_t *pcm_buf, uint32_t pcm_size)
{
    // TRACE(0, "[%s] cvsd_size: %d, pcm_size: %d", __func__, cvsd_size, pcm_size);

    ASSERT(cvsd_size / CVSD_PACKET_SIZE == pcm_size / CVSD_PCM_SIZE,
        "[%s] cvsd_size = %d, pcm_size = %d", __func__, cvsd_size, pcm_size);

#if defined(CVSD_BYPASS)
    Pcm8kToCvsd((short *)pcm_buf, cvsd_buf, pcm_size/2);
#endif

    return 0;
}

int32_t bt_sco_codec_cvsd_decoder(uint8_t *cvsd_buf, uint32_t cvsd_size, uint8_t *pcm_buf, uint32_t pcm_size)
{
    // TRACE(0, "[%s] cvsd_size: %d, pcm_size: %d", __func__, cvsd_size, pcm_size);

    ASSERT(cvsd_size / CVSD_PACKET_SIZE == pcm_size / CVSD_PCM_SIZE,
        "[%s] cvsd_size = %d, pcm_size = %d", __func__, cvsd_size, pcm_size);

#if defined(CVSD_BYPASS) && defined(ENABLE_LPC_PLC)
    ASSERT(cvsd_size % CVSD_PACKET_SIZE == 0, "[%s] cvsd input length(%d) error", __FUNCTION__, cvsd_size);
    for (uint32_t i = 0; i < cvsd_size; i += CVSD_PACKET_SIZE) {
        memcpy(&cvsd_buf_all[CVSD_OFFSET_BYTES], cvsd_buf, CVSD_PACKET_SIZE);
        memcpy(cvsd_buf, cvsd_buf_all, CVSD_PACKET_SIZE);
        memcpy(cvsd_buf_all, &cvsd_buf_all[CVSD_PACKET_SIZE], CVSD_OFFSET_BYTES);

        //DUMP16("0x%x, ", cvsd_buf, CVSD_PACKET_SIZE / 2);

        plc_type_t plc_type = check_cvsd_mute_pattern(cvsd_buf, CVSD_PACKET_SIZE);

        if (plc_type != PLC_TYPE_PASS) {
            memset(pcm_buf, 0, CVSD_PCM_SIZE);
            goto do_plc;
        }

        CvsdToPcm8k(cvsd_buf, (short *)(pcm_buf), CVSD_PACKET_SIZE/2, 0);

do_plc:
        if (plc_type == PLC_TYPE_PASS) {
            lpc_plc_save(plc_st, (int16_t *)pcm_buf);
        } else {
            TRACE(1,"PLC bad frame, plc type: %d", plc_type);
#if defined(PLC_DEBUG_PRINT_DATA)
            DUMP16("0x%x, ", cvsd_buf, CVSD_PACKET_SIZE / 2);
#endif
            lpc_plc_generate(plc_st, (int16_t *)pcm_buf, NULL);
        }

        cvsd_buf += CVSD_PACKET_SIZE;
        pcm_buf += CVSD_PCM_SIZE;
    }
#else
#if defined(CVSD_BYPASS)
    CvsdToPcm8k(cvsd_buf, (short *)(pcm_buf), cvsd_size/2, 0);
#else
    memcpy(pcm_buf,cvsd_buf,cvsd_size);
#endif
#ifndef ENABLE_LPC_PLC
    speech_plc_8000((PlcSt_8000 *)plc_st, (short *)pcm_buf, pcm_size);
#endif
#endif

    return 0;
}

bt_sco_codec_t bt_sco_codec_cvsd = {
    .init       = bt_sco_codec_cvsd_init,
    .deinit     = bt_sco_codec_cvsd_deinit,
    .encoder    = bt_sco_codec_cvsd_encoder,
    .decoder    = bt_sco_codec_cvsd_decoder
};