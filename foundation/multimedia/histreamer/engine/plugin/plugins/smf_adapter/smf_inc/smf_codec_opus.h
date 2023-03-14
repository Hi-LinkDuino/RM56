#pragma once
#ifndef __SMF_CODEC_OPUS_H__
#define __SMF_CODEC_OPUS_H__

#include "smf_api.h"

#define __CHANNEL_COUNT       (1)
#define __COMPLEXITY          (0)
#define __PACKET_LOSS_PERC    (0)
#define __BYTES_PER_SAMPLE    (2)
#define __APP                 (OPUS_APPLICATION_VOIP)
#define __BANDWIDTH           (OPUS_BANDWIDTH_WIDEBAND)
#define __BITRATE             (16000)
#define __SAMPLE_RATE         (16000)
#define __SIGNAL_TYPE         (OPUS_SIGNAL_VOICE)
#define __FRAME_PERIOD        (20)
#define __USE_VBR             (0)
#define __CONSTRAINT_USE_VBR  (0)
#define __USE_INBANDFEC       (0)
#define __USE_DTX             (0)
#define __LSB_DEPTH			  (16)


typedef struct {
	smf_media_info_t media;
	//frame_dms;///(0.1ms)0(auto),25,50,100,200,400,600,800,1000,1200;
	//
	uint16_t application;	
	uint8_t complexity; ///0 (lowest) ... 10 (highest)
	uint8_t lsb_depth;
}smf_opus_enc_open_param_t;

typedef struct {
	smf_media_info_t media;
}smf_opus_dec_open_param_t;

/**
 * register opus encoder
 */
EXTERNC void smf_opus_encoder_register(void);

/**
 * register opus decoder
 */
EXTERNC void smf_opus_decoder_register(void);
#endif
