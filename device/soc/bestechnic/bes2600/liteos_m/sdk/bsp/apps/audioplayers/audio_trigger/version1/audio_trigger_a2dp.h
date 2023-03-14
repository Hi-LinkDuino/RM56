#ifndef __AUDIO_TRIGGER_A2DP_H__
#define __AUDIO_TRIGGER_A2DP_H__


#define A2DP_PLAYER_PLAYBACK_WATER_LINE ((uint32_t)(3.f * a2dp_audio_latency_factor_get() + 0.5f))
#define A2DP_PLAYER_PLAYBACK_WATER_LINE_UPPER (25)

/********************************
        AUD_BITS_16
        dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
        AUD_BITS_24
        dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

        dma_buffer_delay_us
        scalable delay = 864/sample*1000*n ms
        scalable delay = 864/44100*1000*13 = 117ms
        scalable delay = 864/96000*1000*6 = 118ms
        waterline delay = 864/sample*1000*n ms
        waterline delay = 864/44100*1000*3 = 58ms
        waterline delay = 864/96000*1000*3 = 27ms
        audio_delay = scalable delay + waterline delay
 *********************************/

#define A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_HIRES_MTU (13)
#define A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_HIRES_BASE (9000)
#define A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_HIRES_US (A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_HIRES_BASE*A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_HIRES_MTU)

#define A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_BASERES_MTU  (6)
#define A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_BASERES_BASE (19500)
#define A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_BASERES_US (A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_BASERES_BASE*A2DP_PLAYER_PLAYBACK_DELAY_SCALABLE_BASERES_MTU)

/********************************
        AUD_BITS_16
        dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
        AUD_BITS_24
        dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

        dma_buffer_delay_us
        aac delay = 1024/sample*1000*n ms
        aac delay = 1024/44100*1000*5 = 116ms
        waterline delay = 1024/sample*1000*n ms
        waterline delay = 1024/44100*1000*3 = 69ms
        audio_delay = aac delay + waterline delay
 *********************************/
#ifndef A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU
#ifdef A2DP_PLAYER_PLAYBACK_WATER_LINE
#define A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU (3)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU (6)
#endif
#endif
#define A2DP_PLAYER_PLAYBACK_DELAY_AAC_BASE (23000)
#define A2DP_PLAYER_PLAYBACK_DELAY_AAC_US (A2DP_PLAYER_PLAYBACK_DELAY_AAC_BASE*A2DP_PLAYER_PLAYBACK_DELAY_AAC_MTU)

/********************************
    AUD_BITS_16
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
    AUD_BITS_24
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

    sbc delay = 128/sample*n ms
    sbc delay = 128/44100*45 = 130ms
    sbc_delay = sbc delay(23219us)
    waterline delay = 128/sample*SBC_FRAME_MTU*n ms
    waterline delay = 128/44100*5*3 = 43ms
    audio_delay = aac delay + waterline delay
*********************************/
#define A2DP_PLAYER_PLAYBACK_DELAY_SBC_FRAME_MTU (5)
#ifndef A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU
#ifdef A2DP_PLAYER_PLAYBACK_WATER_LINE
#define A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU (35)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU (50)
#endif
#endif
#define A2DP_PLAYER_PLAYBACK_DELAY_SBC_BASE (2800)
#define A2DP_PLAYER_PLAYBACK_DELAY_SBC_US (A2DP_PLAYER_PLAYBACK_DELAY_SBC_BASE*A2DP_PLAYER_PLAYBACK_DELAY_SBC_MTU)

#if defined(A2DP_LHDC_ON)
/********************************
    AUD_BITS_16
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
    AUD_BITS_24
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

    lhdc delay = 512/sample*1000*n ms
    lhdc delay =    *28 = 149ms
    audio_delay = lhdc delay

    lhdc_v2 delay = 512/96000*1000*38 =  202ms
    lhdc_v3 delay = 256/96000*1000*58 = 154ms
    audio_delay = lhdc_v3 delay
*********************************/
#if defined(IBRT)
#if defined(A2DP_LHDC_V3)
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_MTU (58)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_MTU (38)
#endif
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_MTU (28)
#endif

#if defined(A2DP_LHDC_V3)
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_BASE (2666)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_BASE (5333)
#endif
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_US (A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_MTU*A2DP_PLAYER_PLAYBACK_DELAY_LHDC_HIRES_BASE)

/********************************
    AUD_BITS_16
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
    AUD_BITS_24
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

    lhdc delay = 512/sample*1000*n ms
    lhdc_v2 delay = 512/48000*1000*14 = 149ms
    lhdc_v3 delay = 256/48000*1000*28 = 149ms
    audio_delay = lhdc delay
*********************************/
#if defined(A2DP_LHDC_V3)
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_BASERES_MTU (28)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_BASERES_MTU (14)
#endif

#if defined(A2DP_LHDC_V3)
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_BASERES_BASE (5333)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_BASERES_BASE (10666)
#endif
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_BASERES_US (A2DP_PLAYER_PLAYBACK_DELAY_LHDC_BASERES_MTU*A2DP_PLAYER_PLAYBACK_DELAY_LHDC_BASERES_BASE)

/********************************
    AUD_BITS_16
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
    AUD_BITS_24
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

    lhdc delay = 512/sample*1000*n ms
    lhdc_v2 delay = 512/48000*1000*9 = 96ms
    lhdc_v3 delay = 256/48000*1000*19 = 101ms
    audio_delay = lhdc delay
*********************************/
#if defined(IBRT)
#if defined(A2DP_LHDC_V3)
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_MTU (15)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_MTU (9)
#endif
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_MTU (6)
#endif

#if defined(A2DP_LHDC_V3)
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_BASE (5333)
#else
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_BASE (10666)
#endif
#define A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_US (A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_MTU*A2DP_PLAYER_PLAYBACK_DELAY_LHDC_LLC_BASE)
#endif

#if defined(A2DP_LDAC_ON)
/********************************
    AUD_BITS_16
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
    AUD_BITS_24
    dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

    ldac delay = 256/sample*1000*n ms
    ldac delay = 256/96000*1000*56 = 149ms
    audio_delay = ldac delay
*********************************/
#define A2DP_PLAYER_PLAYBACK_DELAY_LDAC_FRAME_MTU (5)
#define A2DP_PLAYER_PLAYBACK_DELAY_LDAC_MTU  (60)
#define A2DP_PLAYER_PLAYBACK_DELAY_LDAC_BASE (2667)
#define A2DP_PLAYER_PLAYBACK_DELAY_LDAC_US (A2DP_PLAYER_PLAYBACK_DELAY_LDAC_BASE*A2DP_PLAYER_PLAYBACK_DELAY_LDAC_MTU)
#endif

#if defined(A2DP_LC3_ON)
/********************************
        AUD_BITS_16
        dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/2*1000000LL/stream_cfg->sample_rate;
        AUD_BITS_24
        dma_buffer_delay_us = stream_cfg->data_size/stream_cfg->channel_num/4*1000000LL/stream_cfg->sample_rate;

        dma_buffer_delay_us
        lc3 delay = 480/sample*1000*n ms
        lc3 delay = 480/44100*1000*13 = 141ms
        lc3 delay = 960/96000*1000*6 = 118ms
        audio_delay = scalable delay
 *********************************/

#define A2DP_PLAYER_PLAYBACK_DELAY_LC3_HIRES_MTU (13)
#define A2DP_PLAYER_PLAYBACK_DELAY_LC3_HIRES_BASE (21768)
#define A2DP_PLAYER_PLAYBACK_DELAY_LC3_HIRES_US (A2DP_PLAYER_PLAYBACK_DELAY_LC3_HIRES_BASE*A2DP_PLAYER_PLAYBACK_DELAY_LC3_HIRES_MTU)

#define A2DP_PLAYER_PLAYBACK_DELAY_LC3_BASERES_MTU  (6)
#define A2DP_PLAYER_PLAYBACK_DELAY_LC3_BASERES_BASE (20000)
#define A2DP_PLAYER_PLAYBACK_DELAY_LC3_BASERES_US (A2DP_PLAYER_PLAYBACK_DELAY_LC3_BASERES_BASE*A2DP_PLAYER_PLAYBACK_DELAY_LC3_BASERES_MTU)
#endif


#ifdef __cplusplus
extern "C" {
#endif

bool app_bt_stream_trigger_onprocess(void);
void app_bt_stream_trigger_result(uint8_t device_id);
void app_bt_stream_trigger_start(uint8_t device_id, uint8_t offset);
void app_bt_stream_trigger_init(void);
void app_bt_stream_trigger_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
