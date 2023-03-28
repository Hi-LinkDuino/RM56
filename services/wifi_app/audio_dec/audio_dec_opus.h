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
#ifndef _AUDIO_DEC_OPUS_H_
#define _AUDIO_DEC_OPUS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "audio_dec.h"

    /* call this function befor audio_opus_dec_open to config decoder arg */
    int audio_opus_dec_set_arg(uint32_t sample_rate, uint32_t frame_size_time_ms, uint8_t channles_num);
    void *audio_opus_dec_open(void);
    int audio_opus_dec_write(void *opus_dec_handle, uint8_t *data, uint32_t input_size, audio_dec_pcm_handler_cb cb, void *cb_arg);
    int audio_opus_dec_close(void *opus_dec_handle);
    uint32_t audio_opus_dec_get_decoder_available_input_data_size(void *opus_dec_handle);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DEC_OPUS_H_ */