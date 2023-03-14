#pragma once
#ifndef __SMF_CODEC_MP3_H__
#define __SMF_CODEC_MP3_H__

#include "smf_api.h"

typedef enum {
	//MP3_MPEG25 = 2,//0,
	//MP3_MPEG0  = 1,
	SMF_MP3_MPEG1 = 0,//3,
	SMF_MP3_MPEG2 = 1,//2,	
}smf_mp3_version_e;

typedef enum {
	SMF_MP3_LAYER1 = 0,
	SMF_MP3_LAYER2 = 1,
	SMF_MP3_LAYER3 = 2,
}smf_mp3_layer_e;

typedef struct smf_mp3_enc_open_param_t {
	smf_media_info_t media;
	//
	uint8_t layer;
}smf_mp3_enc_open_param_t;

typedef struct smf_mp3_dec_open_param_t {
	smf_media_info_t media;	
}smf_mp3_dec_open_param_t;

/**
 * register mp3 encoder
 */
EXTERNC void smf_mp3_encoder_register(void);

/**
 * register mp3 decoder
 */
EXTERNC void smf_mp3_decoder_register(void);
#endif
