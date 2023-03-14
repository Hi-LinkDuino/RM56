#pragma once
#ifndef __SMF_CODEC_JPEG_H__
#define __SMF_CODEC_JPEG_H__

#include "smf_api.h"


typedef struct smf_jpeg_enc_open_param_t {
	smf_media_info_t media;
}smf_jpeg_enc_open_param_t;

typedef struct smf_jpeg_dec_open_param_t {
	smf_media_info_t media;	
}smf_jpeg_dec_open_param_t;

/**
 * register jpeg encoder
 */
EXTERNC void smf_jpeg_encoder_register(void);

/**
 * register jpeg decoder
 */
EXTERNC void smf_jpeg_decoder_register(void);
#endif
