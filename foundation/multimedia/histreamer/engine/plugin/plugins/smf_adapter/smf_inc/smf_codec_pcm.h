#pragma once
#ifndef __SMF_CODEC_PCM_H__
#define __SMF_CODEC_PCM_H__

#include "smf_format_pcm.h"

/**
 * register pcm format
 */
EXTERNC void smf_pcm_fmt_register(void);

typedef smf_pcm_fmt_open_param_t smf_pcm_dec_open_param_t;
/**
 * register pcm decoder
 */
EXTERNC void smf_pcm_decoder_register(void);

typedef smf_pcm_fmt_open_param_t smf_pcm_enc_open_param_t;
/**
 * register pcm encoder
 */
EXTERNC void smf_pcm_encoder_register(void);

/// <summary>
/// adpcm
/// </summary>
typedef struct {
	smf_media_info_t media;
}smf_adpcm_dec_open_param_t;
typedef struct {
	smf_media_info_t media;
}smf_adpcm_enc_open_param_t;

/**
 * register adpcm decoder
 */
EXTERNC void smf_adpcm_decoder_register(void);

/**
 * register adpcm encoder
 */
EXTERNC void smf_adpcm_encoder_register(void);
#endif
