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
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "cmsis.h"
#include "hal_trace.h"
#include "string.h"
#include "mcu_audio.h"
#include "heart_beat.h"

/*
static void dummy_record_pre_handler(uint8_t *buf, uint32_t len)
{
    // pre-process the mic data here before m33 send to a7
}
*/
#ifdef A7_RECOVER_TEST_CASE
void mcu_a7_recover_cb1(int status)
{
    TRACE(0, "%s status:%d ", __func__, status);
    /* send msg to your task for nofity a7 is recover status */
}
void mcu_a7_recover_cb2(int status)
{
    TRACE(0, "%s status:%d ", __func__, status);
    /* send msg to your task for nofity a7 is recover status */
}
#endif

#if (defined A7_PCMAUDIO_TEST_CASE) || (defined PCM_AUDIO_INSTANCE_ENABLE)
void speech_record_config_get(uint32_t *sample_rate, uint32_t *record_period_ms)
{
    TRACE(0, "%s strong sample_rate:%d, record_period_ms:%d", __func__, *sample_rate, *record_period_ms);
    if (sample_rate)
        *sample_rate = 48000;

    if (record_period_ms)
        *record_period_ms = 32;
    TRACE(0, "%s strong set sample_rate:%d, record_period_ms:%d", __func__, *sample_rate, *record_period_ms);
    printf("%s, %d ch: %d\n\r",__FUNCTION__, __LINE__, CHAN_NUM_CAPTURE);
}
#endif

void mcu_audio_task_init_hook()
{
    printf("%s,%d,.\n\r",__FUNCTION__,__LINE__);
    // do what needed here when mic init
    // mcu_record_pre_handler_set(dummy_record_pre_handler);
    printf("%s,%d ch: %d\n\r",__FUNCTION__,__LINE__, CHAN_NUM_CAPTURE);

#ifdef A7_RECOVER_TEST_CASE
    a7_dsp_recover_subscribe(mcu_a7_recover_cb1);
    a7_dsp_recover_subscribe(mcu_a7_recover_cb2);
#endif
}
