#ifndef HISTREAMER_SMF_API_H
#define HISTREAMER_SMF_API_H

#include "smf_inc/smf_demuxer_mp3.h"
#include "smf_inc/smf_demuxer_wav.h"
#include "smf_inc/smf_demuxer_mp4.h"
#include "smf_inc/smf_demuxer_ogg.h"
#include "smf_inc/smf_demuxer_sbc.h"

#include "smf_inc/smf_codec_mp3.h"
#include "smf_inc/smf_codec_pcm.h"
#include "smf_inc/smf_codec_sbc.h"

#define KEY_MP3 "mp3"
#define KEY_WAV "wav"
#define KEY_PCM "pcm"
#define KEY_MP4 "mp4"
#define KEY_M4A "m4a"
#define KEY_SBC "sbc"

#define SMF_FS_CALLBACK_REGISTER_IOFILE

typedef void SMF_DEMUXER_T;

typedef smf_demuxer_param_t SMF_DEMUXER_PARAM_T;

typedef void SMF_DECODER_T;

typedef void SMF_DECODER_PARAM_T;

#ifdef SMF_FS_CALLBACK_REGISTER_IOFILE
typedef smf_io_callback_param_t SMF_IO_CALLBACK_PARAM_T;
#endif

#endif // HISTREAMER_SMF_API_H

