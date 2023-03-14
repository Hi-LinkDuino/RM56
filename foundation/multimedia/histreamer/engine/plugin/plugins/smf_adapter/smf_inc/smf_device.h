#pragma once
#ifndef __SMF_DEVICE_H__
#define __SMF_DEVICE_H__
enum CODEC_TYPE {
    CODEC_USE_INTCODEC = 0,
    CODEC_USE_I2S = 1
};
typedef struct smf_microphone_open_param_t {
	smf_source_param_t media;
}smf_microphone_open_param_t;

typedef struct smf_speaker_open_param_t {
	smf_sink_param_t media;
}smf_speaker_open_param_t;

/**
 * register speaker
 */
EXTERNC void smf_speaker_register(void);

/**
 * register microphone
 */

EXTERNC void smf_microphone_register(void);
#endif