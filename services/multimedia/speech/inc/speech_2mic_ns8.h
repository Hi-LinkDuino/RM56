#ifndef __SPEECH_2MIC_NS8_H__
#define __SPEECH_2MIC_NS8_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    float       delay_taps;
    float       ref_pwr_threshold;
} Speech2MicNs8Config;

struct Speech2MicNs8State_;

typedef struct Speech2MicNs8State_ Speech2MicNs8State;

Speech2MicNs8State *speech_2mic_ns8_create(int32_t sample_rate, int32_t frame_size, Speech2MicNs8Config *cfg);

int32_t speech_2mic_ns8_destroy(Speech2MicNs8State *st);

int32_t speech_2mic_ns8_process(Speech2MicNs8State *st, int16_t *pcm_buf, int16_t *ref_buf, int32_t pcm_len, int16_t *out_buf);

float speech_2mic_ns8_get_required_mips(Speech2MicNs8State *st);

int32_t speech_2mic_ns8_get_delay(Speech2MicNs8State *st);

#ifdef __cplusplus
}
#endif

#endif