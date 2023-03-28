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
#include "plat_addr_map.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "string.h"
#include "stdio.h"
#include "mcu_audio.h"

#ifdef DSP_TFLM_ENABLE
extern int tf_main(int argc, char** argv);
#endif

void a7_ctrl_plane_handler(unsigned int cmd1,unsigned int cmd2)
{
    /* m33==>a7 it's the message of a7 receive cmd and para from m33 */
    printf("%s: cmd1=%d, cmd2=%d\n", __FUNCTION__, cmd1, cmd2);
	#ifdef DSP_TFLM_ENABLE
	uint32_t start_ticks = hal_sys_timer_get();
	tf_main(0,0);
	uint32_t endtick = hal_sys_timer_get();
	uint32_t costtime = TICKS_TO_MS(endtick - start_ticks);
	
	printf("%s %d start:%u, end:%u costtime:%ums freq is %d\n", __FUNCTION__, __LINE__, start_ticks, endtick, costtime, hal_cmu_sys_get_freq());
	#endif
	
    /* m33<==a7 a7 notify message to  m33 */
    char * data = "one two three ...";
    a7_cmd_send(data,strlen(data));
}

#ifndef A7_PCMAUDIO_TEST_CASE
/* use pcm audio, need to mask this strong function,use defaul weak function to get cb mic data*/
extern uint32_t a7_audio_record_deliver(uint8_t *buf, uint32_t len);
void a7_record_process_hook(uint8_t * buf, uint32_t len)
{
	// it's the original mic data
    // printf("%s: buf=%p, len=%d\n", __FUNCTION__, buf, len);
    static uint32_t cnt = 0;
    if (cnt++ % 60 == 0) {
        //printf("%s,%d ch: %d\n\r",__FUNCTION__,__LINE__, CHAN_NUM_CAPTURE);
    }

#ifdef SPEECH_MSG_TEST_TXRX_LOOP
    extern uint32_t speech_msg_get_signal(int signal);
    extern int speech_msg_signal_send(uint8_t * buf, uint32_t len, uint32_t signal);
    speech_msg_signal_send(buf,len,speech_msg_get_signal(1));
#else
    a7_audio_record_deliver(buf,len);
#endif
}
#endif

extern void a7_dsp_dummy_main(void);
void a7_dsp_init_hook(void)
{
	// do what needed here when a7 init
    printf("%s: call a7_dsp_dummy_main\n", __FUNCTION__);
	a7_dsp_dummy_main();
#ifdef SPEECH_MSG_TEST_RX
    extern void speech_msg_test_rx_init(void);
    speech_msg_test_rx_init();
#endif
}
