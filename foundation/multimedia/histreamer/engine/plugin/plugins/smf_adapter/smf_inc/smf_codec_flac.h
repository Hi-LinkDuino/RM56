#pragma once
#ifndef __SMF_CODEC_FLAC_H__
#define __SMF_CODEC_FLAC_H__

#include "smf_api.h"

typedef struct smf_flac_enc_open_param_t {
	smf_media_info_t media;
	
}smf_flac_enc_open_param_t;

typedef struct smf_flac_dec_open_param_t {
	smf_media_info_t media;
	
}smf_flac_dec_open_param_t;

/**
 * register aac encoder
 */
EXTERNC void smf_flac_encoder_register(void);

/**
 * register aac decoder
 */
EXTERNC void smf_flac_decoder_register(void);

#endif
