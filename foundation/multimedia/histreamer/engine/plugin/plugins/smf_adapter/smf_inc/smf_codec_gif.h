#pragma once
#ifndef __SMF_CODEC_GIF_H__
#define __SMF_CODEC_GIF_H__

#include "smf_api.h"


typedef struct smf_gif_enc_open_param_t {
	smf_media_info_t media;
}smf_gif_enc_open_param_t;

typedef struct smf_gif_dec_open_param_t {
	smf_media_info_t media;	
}smf_gif_dec_open_param_t;

/**
 * register gif encoder
 */
EXTERNC void smf_gif_encoder_register(void);

/**
 * register gif decoder
 */
EXTERNC void smf_gif_decoder_register(void);
#endif
