#pragma once

#include "smf_api.h"

typedef struct smf_jpeg_dec_open_param_t {
	smf_media_info_t media;
}smf_h264_dec_open_param_t;

/**
 * register h264 decoder
 */
EXTERNC void smf_h264_decoder_register(void);