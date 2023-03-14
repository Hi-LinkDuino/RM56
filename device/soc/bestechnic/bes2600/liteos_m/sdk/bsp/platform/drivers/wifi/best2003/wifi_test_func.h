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
#ifndef __WIFI_TEST_FUNC_H__
#define __WIFI_TEST_FUNC_H__

//#include "hwtest.h"
//#include "hal_transq.h"
//#include "cmsis.h"
//#include "string.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#define DEFALT_DIG_GAIN   0x140
#define DEFALT_DIG_GAIN_5G   0x140

#define DEFALT_11B_DIG_GAIN   0x1c0

 extern uint8_t test_ch;
 extern uint16_t use_freq ;


 extern int modulate_mode;
 extern uint16_t* cali_tx_power;
 extern uint16_t cali_tx_gain[];
 extern uint16_t test_dig_gain;

extern void  WSM_TransmitReq_test(uint8_t *pMsg, uint32_t test_cfg);

extern void rf_tx_tone_init_start(uint8_t freq);
extern void rf_tx_two_tone_init_start(uint8_t freq_base,uint8_t freq_delta);

extern void rf_rx_tone_test_init_regs(void);
void rf_tx_tone_reset_regs(void);

extern void rf_rx_tone_test(void);

extern void check_ch_switch(void);
extern void tx_tone_regs_recover(void);
extern void cfg_iqtest(void);
extern void cfg_iqtest_Triangle(int8_t step,uint16_t gain);
extern void cfg_iqtest_Triangle_new(int8_t step,uint16_t gain);
extern void cfg_rx_mem_dump(uint32_t len);
void wifi_rx_mem_dump_test(uint32_t len);
void cfg_iqtest_ax(void);

void cfg_iqtest_11n(uint8_t gain_idx);
void set_wifi_test_continue_mode(uint8_t mode);
void rf_rx_force_init_regs(void);
void rf_tx_rx_loop_dump(void);
void rf_tx_rx_loop_dump_rm_pudr(void);

void wifi_dpd_test(int8_t step,uint16_t gain,int32_t len);

void wifi_dpd_test_new(int8_t step,uint16_t gain,uint32_t len);
void init_digital_gain(uint16_t* cal_tx_power,uint8_t ch);
void config_dig_gain_no_signal(int mode);
void dig_test_config(void);
void ana_and_dig_config(void);
int wifi_rf_set_tpc_table(double table_a [][39], double table_b [][39], int table_size);
int stop_11n_cont_mode(void);
int stop_11b_cont_mode(void);
int stop_cntinue_test_mode(void);


#ifdef __cplusplus
}
#endif

#endif //__WIFI_TEST_H__

