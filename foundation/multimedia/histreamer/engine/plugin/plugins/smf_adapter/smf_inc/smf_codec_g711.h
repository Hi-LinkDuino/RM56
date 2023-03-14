#pragma once
#ifndef __SMF_CODEC_G711_H__
#define __SMF_CODEC_G711_H__

#include "smf_api.h"

typedef struct {
	smf_media_info_t media;
	
}smf_g711_enc_open_param_t;

typedef struct {
	smf_media_info_t media;
}smf_g711_dec_open_param_t;

/**
 * register g711 encoder
 */
EXTERNC void smf_g711_encoder_register(void);

/**
 * register g711 decoder
 */
EXTERNC void smf_g711_decoder_register(void);
#endif
