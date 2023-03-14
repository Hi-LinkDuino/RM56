#pragma once
#ifndef __SMF_CODEC_gSBC_H__
#define __SMF_CODEC_gSBC_H__

#include "smf_api.h"
#include <stdbool.h>

typedef struct smf_gsbc_enc_open_param_t {
	smf_media_info_t media;
} smf_gsbc_enc_open_param_t;

typedef struct smf_gsbc_dec_open_param_t {
	smf_media_info_t media;
	//
	bool plc;
}smf_gsbc_dec_open_param_t;
///
/**
 * register msbc decoder
 */
EXTERNC void smf_gsbc_decoder_register(void);

/**
 * register msbc encoder
 */
EXTERNC void smf_gsbc_encoder_register(void);
///
#endif
