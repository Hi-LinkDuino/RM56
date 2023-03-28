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

#ifndef _AUDIO_DEC_AAC_ADTS_H_
#define _AUDIO_DEC_AAC_ADTS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define AUDIO_DEC_AAC_ADTS_FIFO_SIZE (1024)

    void *audio_aac_adts_dec_open(void);
    int audio_aac_adts_dec_write(void *aac_dec_handle, uint8_t *data_in, int input_size, audio_dec_pcm_handler_cb cb, void *cb_arg);
    int audio_aac_adts_dec_close(void *aac_dec_handle);
    uint32_t audio_aac_adts_dec_get_decoder_available_input_data_size(void *aac_dec_handle);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DEC_AAC_ADTS_H_ */