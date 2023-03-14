#pragma once
#ifndef __SMF_FORMAT_PCM_H__
#define __SMF_FORMAT_PCM_H__

#include "smf_api.h"


typedef struct {
	smf_media_info_t media;
	smf_media_info_t target;
}smf_pcm_fmt_open_param_t;

/**
 * register pcm format,include:
 **		resample;
 **		rechannels;
 **     resampleBits;
 **     interlace/deinterlace;
 */
EXTERNC void smf_pcm_fmt_register();

#endif
