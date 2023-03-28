#ifndef __SPEECH_2MIC_NS7_H__
#define __SPEECH_2MIC_NS7_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	int32_t     bypass;
	int32_t     wnr_enable;
	float       denoise_dB;
	float       delay_taps;
	int32_t     freq_smooth_enable;
} Speech2MicNs7Config;
struct Speech2MicNs7State_;
typedef struct Speech2MicNs7State_ Speech2MicNs7State;
Speech2MicNs7State *speech_2mic_ns7_create(int sample_rate, int frame_size, Speech2MicNs7Config *cfg);
int32_t speech_2mic_ns7_destroy(Speech2MicNs7State *st);
int32_t speech_2mic_ns7_process(Speech2MicNs7State *st, short *pcm_buf, short *ref_buf, int32_t pcm_len, short *out_buf);
int speech_2mic_ns7_set_config(Speech2MicNs7State *st, const Speech2MicNs7Config *cfg);
float speech_2mic_ns7_get_required_mips(Speech2MicNs7State *st);
#ifdef __cplusplus
}
#endif

#endif
