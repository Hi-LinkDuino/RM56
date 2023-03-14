#pragma once
#ifndef __SMF_CODEC_SILK_H__
#define __SMF_CODEC_SILK_H__

#include "smf_api.h"

#define MAX_BYTES_PER_FRAME     250 // Equals peak bitrate of 100 kbps 
#define MAX_INPUT_FRAMES        5
#define MAX_API_FS_KHZ          24
#define QQ_header               "\002#!SILK_V3"
#define Silk_header             "#!SILK_V3"
#define PACKETSIZE_MS           20
#define MAX_INTERNAL_FS_HZ      24000
#define TARGET_BITRATE          23900
#define TARGET_SAMPLERATE       16000
#define FRAME_LENGTH_MS         20
#define MAX_RECORDER_TIME		60
#define SILK_COMPLEXITY			2
#define PACKETLOSS_PERC			0
#define DTX_ENABLED				0
#define INBANDFEC_ENABLED		0
#define ENC_SILK_HEAP_SIZE		(20*1024)
#define FRAME_SIZE_READ_FROM_FILE_MS 20

typedef struct {
	smf_media_info_t media;
	//
	/*Supported sampling rates are 8000, 12000,16000, 24000.*/
	int bitwidth;
	int API_sampleRate;
	int bitRate;
	int complexity;
	int maxInternalSampleRate;
	int packetLossPercentage;
	int packetSize;
	int useDTX;
	int useInBandFEC;
	
}smf_silk_enc_open_param_t;

typedef struct {
	smf_media_info_t media;
	/*Supported sampling rates are 8000, 12000,16000, 24000.*/
	uint8_t bitwidth;
	int bitrate;
	uint8_t bandwidth; 
}smf_silk_dec_open_param_t;

/**
 * register silk encoder
 */
EXTERNC void smf_silk_encoder_register(void);

/**
 * register silk decoder
 */
EXTERNC void smf_silk_decoder_register(void);
#endif
