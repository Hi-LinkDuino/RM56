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

#ifndef _AUDIO_DEC_AAC_M4A_H_
#define _AUDIO_DEC_AAC_M4A_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define AUDIO_DEC_AAC_M4A_FIFO_SIZE (1024)

    int audio_aac_m4a_dec_open(void);
    int audio_aac_m4a_dec_write(uint8_t *data, uint32_t input_size, audio_dec_pcm_handler_cb cb, void * cb_arg);
    int audio_aac_m4a_dec_close(void);
    uint32_t audio_aac_m4a_dec_get_decoder_available_input_data_size(void);
    
    int audio_aac_m4a_dec_prepare_file(uint8_t *data, uint32_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DEC_AAC_M4A_H_ */