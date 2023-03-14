#pragma once
#ifndef __SMF_CODEC_PNG_H__
#define __SMF_CODEC_PNG_H__

#include "smf_api.h"


typedef struct smf_png_enc_open_param_t {
	smf_media_info_t media;
}smf_png_enc_open_param_t;

typedef struct smf_png_dec_open_param_t {
	smf_media_info_t media;	
}smf_png_dec_open_param_t;

/**
 * register png encoder
 */
EXTERNC void smf_png_encoder_register(void);

/**
 * register png decoder
 */
EXTERNC void smf_png_decoder_register(void);
#endif
