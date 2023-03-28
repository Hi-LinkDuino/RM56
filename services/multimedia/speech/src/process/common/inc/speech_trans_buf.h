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
#ifndef __SPEECH_TRANS_BUF_H__
#define __SPEECH_TRANS_BUF_H__

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
int speech_trans_buf_init(void);
int speech_trans_buf_deinit(void);

int speech_trans_buf_input_save_data(short *buf, int len);
int16_t *speech_trans_buf_input_get_data(int len);

int speech_trans_buf_output_save_data(short *buf, int len);
int speech_trans_buf_output_get_data(short **buf);


int speech_echo_buffer_init(void);
int speech_echo_buffer_save_data(short *buf, uint32_t len);
// int speech_echo_buffer_get_data(short **buf, int len);
short *speech_echo_buffer_get_data(int len);
#endif
    
#ifdef __cplusplus
}
#endif

#endif
