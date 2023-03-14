#pragma once
#ifndef __SMF_CODEC_OGG_H__
#define __SMF_CODEC_OGG_H__

#include "smf_api.h"



enum encmode_s{
	SMF_VBR = 0,
	SMF_ABR = 1,
};

typedef struct smf_vorbis_enc_open_param_s {
	smf_media_info_t media;
	//
	int encmode;
}smf_vorbis_enc_open_param_t;

typedef struct vorbis_dec_open_param_s {
	smf_media_info_t media;
}smf_vorbis_dec_open_param_t;


/**
 * register vorbis encoder
 */
EXTERNC void smf_vorbis_encoder_register(void);

/**
 * register vorbis decoder
 */
EXTERNC void smf_vorbis_decoder_register(void);
#endif
