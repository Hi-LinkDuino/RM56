#pragma once
#ifndef __SMF_CODEC_APE_H__
#define __SMF_CODEC_APE_H__

#include "smf_api.h"

typedef struct ape_enc_open_param_t {
	smf_media_info_t media;
	uint8_t bitwidth;
	int bitrate;
	int bandwidth;
}smf_ape_enc_open_param_t;

typedef struct ape_dec_open_param_t {
	smf_media_info_t media;
	uint8_t bitwidth;
	int bitrate;
	uint8_t bandwidth; 
}smf_ape_dec_open_param_t;

/**
 * register ape encoder
 */
EXTERNC void smf_ape_encoder_register(void);

/**
 * register ape decoder
 */
EXTERNC void smf_ape_decoder_register(void);
#endif
