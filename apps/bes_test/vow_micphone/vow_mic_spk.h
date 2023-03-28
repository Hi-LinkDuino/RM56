#ifndef __WADC_H__
#define __WADC_H__

typedef struct _VOW_AUDIO_CFG
{
    uint32_t sam_rate;
    uint8_t vol;
    uint8_t mute_state;
    uint8_t depth;
    uint8_t ch;
    uint8_t burst_size; // max number of audio frames to be dealed with one time
    uint32_t delay;     // delay time to start play in us, the minimum delay should cover 2 dma buf time.
} VOW_AUDIO_CFG;


void wadc_app_start();

#endif //__WADC_H__
