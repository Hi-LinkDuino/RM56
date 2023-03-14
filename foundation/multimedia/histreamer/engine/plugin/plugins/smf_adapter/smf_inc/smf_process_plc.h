#pragma once
#ifndef __SMF_PROCESS_PLC_H__
#define __SMF_PROCESS_PLC_H__

#include "smf_api.h"

enum smf_plc_channel_e {
	SMF_PLC_CHANNEL_SELECT_LCHNL = 0,
	SMF_PLC_CHANNEL_SELECT_RCHNL = 1,
	SMF_PLC_CHANNEL_SELECT_LRCHNL = 2,
};

enum PLC_CODEC_TYPE {
	PLC_CODEC_TYPE_SBC = 0,
	PLC_CODEC_TYPE_AAC = 1,
	PLC_CODEC_TYPE_LC3 = 2,
};



typedef struct smf_plc_open_param_s {
	int sampleRate;
	int channelNum;
	int targetchannelNum;
	int targetchannel;
	int sampleBits;
	int frameSize;
	enum PLC_CODEC_TYPE type;
	void* other;
}smf_plc_open_param_t;

/**
 * register plc processer
 */
EXTERNC void smf_plc_register();

#endif
