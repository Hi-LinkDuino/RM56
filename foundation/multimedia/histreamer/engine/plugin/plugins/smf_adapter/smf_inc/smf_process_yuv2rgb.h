#pragma once
#include "smf_api.h"
#include "smf_common.h"

typedef struct smf_yun2rgb_open_param_t {
	smf_media_info_t imedia;
	smf_media_info_t omedia;
	uint16_t width;
	uint16_t height;
	void* other;
}smf_yun2rgb_open_param_t;

EXTERNC void yuv2rgb_register();