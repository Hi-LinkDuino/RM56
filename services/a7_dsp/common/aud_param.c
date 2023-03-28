#include <stdbool.h>
#include "stdio.h"
#include "string.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_trace.h"
#include "mcu_audio.h"

static int mic_handshake_done = 0;
static int __mic_samplerate = MIC_SAMPLERATE;
void mic_set_samplerate(int mic_samplerate)
{
    __mic_samplerate = mic_samplerate;
    mic_handshake_done = 1;
    printf("%s %d\n", __FUNCTION__, __mic_samplerate);
}
int mic_get_samplerate()
{
    return __mic_samplerate;
}
int mic_get_handshake_done()
{
    return mic_handshake_done;
}
void mic_reset_handshake()
{
    mic_handshake_done = 0;
}
static int __mic_period_ms = MIC_PERIOD_MS;
void mic_set_period(int mic_period_ms)
{
    __mic_period_ms = mic_period_ms;
    printf("%s %d\n", __FUNCTION__, __mic_period_ms);
}
int mic_get_period()
{
    return __mic_period_ms;
}

static e_MIC_TYPE_T __mic_type = eANALOG_MIC;
void mic_set_type(e_MIC_TYPE_T type)
{
    __mic_type = type;
    printf("%s %d\n", __FUNCTION__, __mic_type);
}
e_MIC_TYPE_T mic_get_type(void)
{
    return __mic_type;
}

static short resample_channel1(int32_t PcmValue)
{
    float gain = 1.0F;
    const float NUM[5] = {0.0210,    0.0842,    0.1263,    0.0842,    0.0210};
    const float DEN[5] = {1.0000,   -1.4765,    1.1829,   -0.4380,    0.0682};
    static float y0 = 0, y1 = 0, y2 = 0,y3=0,y4=0, x0 = 0, x1 = 0, x2 = 0,x3=0,x4=0;
    int32_t PcmOut = 0;

    // Left channel
    x0 = PcmValue* gain;
    y0 = x0*NUM[0] + x1*NUM[1] + x2*NUM[2] + x3*NUM[3] + x4*NUM[4] - y1*DEN[1] - y2*DEN[2] - y3*DEN[3] - y4*DEN[4];
    y4 = y3;
    y3 = y2;
    y2 = y1;
    y1 = y0;
    x4 = x3;
    x3 = x2;
    x2 = x1;
    x1 = x0;

    PcmOut = (int32_t)y0;

    PcmOut = __SSAT(PcmOut, 16);

    return (short)PcmOut;
}

static short resample_channel2(int32_t PcmValue)
{
    float gain = 1.0F;
    const float NUM[5] = {0.0210,    0.0842,    0.1263,    0.0842,    0.0210};
    const float DEN[5] = {1.0000,   -1.4765,    1.1829,   -0.4380,    0.0682};
    static float y0 = 0, y1 = 0, y2 = 0,y3=0,y4=0, x0 = 0, x1 = 0, x2 = 0,x3=0,x4=0;
    int32_t PcmOut = 0;

    // Left channel
    x0 = PcmValue* gain;
    y0 = x0*NUM[0] + x1*NUM[1] + x2*NUM[2] + x3*NUM[3] + x4*NUM[4] - y1*DEN[1] - y2*DEN[2] - y3*DEN[3] - y4*DEN[4];
    y4 = y3;
    y3 = y2;
    y2 = y1;
    y1 = y0;
    x4 = x3;
    x3 = x2;
    x2 = x1;
    x1 = x0;

    PcmOut = (int32_t)y0;

    PcmOut = __SSAT(PcmOut, 16);

    return (short)PcmOut;
}

static short resample_channel3(int32_t PcmValue)
{
    float gain = 1.0F;
    const float NUM[5] = {0.0210,    0.0842,    0.1263,    0.0842,    0.0210};
    const float DEN[5] = {1.0000,   -1.4765,    1.1829,   -0.4380,    0.0682};
    static float y0 = 0, y1 = 0, y2 = 0,y3=0,y4=0, x0 = 0, x1 = 0, x2 = 0,x3=0,x4=0;
    int32_t PcmOut = 0;

    // Left channel
    x0 = PcmValue* gain;
    y0 = x0*NUM[0] + x1*NUM[1] + x2*NUM[2] + x3*NUM[3] + x4*NUM[4] - y1*DEN[1] - y2*DEN[2] - y3*DEN[3] - y4*DEN[4];
    y4 = y3;
    y3 = y2;
    y2 = y1;
    y1 = y0;
    x4 = x3;
    x3 = x2;
    x2 = x1;
    x1 = x0;

    PcmOut = (int32_t)y0;

    PcmOut = __SSAT(PcmOut, 16);

    return (short)PcmOut;
}

static short resample_channel4(int32_t PcmValue)
{
    float gain = 1.0F;
    const float NUM[5] = {0.0210,    0.0842,    0.1263,    0.0842,    0.0210};
    const float DEN[5] = {1.0000,   -1.4765,    1.1829,   -0.4380,    0.0682};
    static float y0 = 0, y1 = 0, y2 = 0,y3=0,y4=0, x0 = 0, x1 = 0, x2 = 0,x3=0,x4=0;
    int32_t PcmOut = 0;

    // Left channel
    x0 = PcmValue* gain;
    y0 = x0*NUM[0] + x1*NUM[1] + x2*NUM[2] + x3*NUM[3] + x4*NUM[4] - y1*DEN[1] - y2*DEN[2] - y3*DEN[3] - y4*DEN[4];
    y4 = y3;
    y3 = y2;
    y2 = y1;
    y1 = y0;
    x4 = x3;
    x3 = x2;
    x2 = x1;
    x1 = x0;

    PcmOut = (int32_t)y0;

    PcmOut = __SSAT(PcmOut, 16);

    return (short)PcmOut;
}

#define RESAMPLE_TIMES   (48/16)
uint32_t mic_resample_48_to_16(uint8_t *src, uint8_t *dst, uint32_t len)
{
    int step_src = ((MIC_BITRATE/1000/8)*CHAN_NUM_CAPTURE);
    //int step_dst = ((16000/1000/8)*CHAN_NUM_CAPTURE);
    ASSERT((len%step_src) == 0, "mic len/18 error");
    int w=0;
    if ((src == NULL) || (dst == NULL)) {
        printf("%s src(%p) or dst(%p) is NULL!\n", __FUNCTION__, src, dst);
        return 0;
    }
    short *pcm_buf  = (short *)src;
    short *temp_buf = (short *)dst;
    int pcm_len = len / 2;
    int const_step = (CHAN_NUM_CAPTURE * RESAMPLE_TIMES);

    for (uint32_t i = 0; i < pcm_len/const_step; i++)
    {
        if (CHAN_NUM_CAPTURE > 0) {
            temp_buf[(CHAN_NUM_CAPTURE * i) + 0] = \
            resample_channel1(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 0) + 0]);
            resample_channel1(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 1) + 0]);
            resample_channel1(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 2) + 0]);
        }

        if (CHAN_NUM_CAPTURE > 1) {
            temp_buf[(CHAN_NUM_CAPTURE * i) + 1] = \
            resample_channel2(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 0) + 1]);
            resample_channel2(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 1) + 1]);
            resample_channel2(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 2) + 1]);
        }

        if (CHAN_NUM_CAPTURE > 2) {
            temp_buf[(CHAN_NUM_CAPTURE * i) + 2] = \
            resample_channel3(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 0) + 2]);
            resample_channel3(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 1) + 2]);
            resample_channel3(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 2) + 2]);
        }

        if (CHAN_NUM_CAPTURE > 3) {
            temp_buf[(CHAN_NUM_CAPTURE * i) + 3] = \
            resample_channel4(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 0) + 3]);
            resample_channel4(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 1) + 3]);
            resample_channel4(pcm_buf[const_step * i + (CHAN_NUM_CAPTURE * 2) + 3]);
        }
    }
    dst = (uint8_t *)temp_buf;
    w = (pcm_len/RESAMPLE_TIMES) * 2;
    return w;
}

