#pragma once
#ifndef __SMF_MUXER_H__
#define __SMF_MUXER_H__

#include "smf_api.h"

typedef struct {
	smf_sink_param_t sink;
	///
	//smf_media_info_t media;
	smf_meta_info_t meta;
	///
	const char* url;
	void* other;
}smf_muxer_param_t;

#endif
