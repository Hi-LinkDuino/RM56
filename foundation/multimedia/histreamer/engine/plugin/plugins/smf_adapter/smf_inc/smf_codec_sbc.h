#pragma once
#ifndef __SMF_CODEC_SBC_H__
#define __SMF_CODEC_SBC_H__

#include "smf_api.h"
enum smf_sbc_sample_rate_e{
	SMF_SBC_SAMPLE_RATE_16K=16000,
	SMF_SBC_SAMPLE_RATE_32K=32000,
	SMF_SBC_SAMPLE_RATE_44K1=44100,
	SMF_SBC_SAMPLE_RATE_48K=48000,
};
enum smf_sbc_alloc_method_e {
	SMF_SBC_ALLOC_METHOD_LOUDNESS=0,
	SMF_SBC_ALLOC_METHOD_SNR=1,
};
enum smf_sbc_channel_mode_e {
	SMF_SBC_CHNL_MODE_MONO          =0,
	SMF_SBC_CHNL_MODE_DUAL_CHNL     =1,
	SMF_SBC_CHNL_MODE_STEREO        =2,
	SMF_SBC_CHNL_MODE_JOINT_STEREO  =3,
};
enum smf_sbc_blocks_e{
	SMF_SBC_BLOCKS_4=4,
	SMF_SBC_BLOCKS_8=8,
	SMF_SBC_BLOCKS_12=12,
	SMF_SBC_BLOCKS_16=16,
};
enum smf_sbc_subbands_e{
	SMF_SBC_SUBBANDS_4=4,
	SMF_SBC_SUBBANDS_8=8,
};
enum smf_sbc_package_e{
	SMF_SBC_PACKAGE_NORMAL=0,
	SMF_SBC_PACKAGE_A2DP,
};
typedef struct{
	smf_media_info_t media;
	//
	unsigned char channelMode;// sbc_channel_mode_e
	unsigned char allocMethod;//sbc_alloc_method_e
	unsigned char numBlocks;// sbc_blocks_e
	unsigned char numSubBands;//sbc_subbands_e
	int bitPool;
	unsigned char mSbcFlag;
	unsigned char gSbcFlag;
}smf_sbc_enc_open_param_t;

typedef struct {
	smf_media_info_t media;
	//
}smf_sbc_dec_open_param_t;
///

/**
 * register sbc decoder
 */
EXTERNC void smf_sbc_decoder_register(void);

/**
 * register sbc encoder
 */
EXTERNC void smf_sbc_encoder_register(void);
//
enum{
	SMF_SBC_FRAME_FLAGS_MUTE_FRAME = 1<<1,
};
#endif
