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
#ifndef _AUDIO_ENC_DEC_OPUS_H_
#define _AUDIO_ENC_DEC_OPUS_H_

#ifdef __cplusplus
extern "C"
{
#endif
typedef enum
{
    OPUS_ENC_APP_AUDIO,
    OPUS_ENC_APP_VOIP,
    OPUS_ENC_APP_RESTRICTED_LOWDELAY,
} OPUS_CODEC_APP_T;


typedef enum
{
    DSP_CODEC_OPUS_ENC0_ID = 0,
    DSP_CODEC_OPUS_ENC1_ID = 1,
    DSP_CODEC_OPUS_ENC2_ID = 2,
    DSP_CODEC_OPUS_ENC3_ID = 3,
} OPUS_ENC_ID_T;

    int bap_audio_opus_enc_init(uint32_t sample_rate, uint8_t channels_num);

    int bap_audio_opus_enc_write(OPUS_ENC_ID_T encode_id , int32_t *data_in, uint32_t frame_size, uint8_t *data_out, int32_t *out_len);
    int bap_audio_opus_enc_close(void);

    int bap_audio_opus_dec_init(uint32_t sample_rate, uint8_t channels_num);
    int bap_audio_opus_dec_write(uint8_t *data_in, int input_data_len, int32_t *data_out,int *outlen);
    int bap_audio_opus_dec_close(void);

    int bap_audio_opus_config();

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_ENC_DEC_OPUS_H_ */
