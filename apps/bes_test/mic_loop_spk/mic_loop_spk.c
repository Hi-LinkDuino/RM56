#include "audioflinger.h"
#include <string.h>
#include "hal_trace.h"
#include "hal_location.h"
#include "cqueue.h"

#define AUDIO_USE_DEVICE_I2S    0
#define AUDIO_STREAM_CH 	AUD_STREAM_ID_1
#define AUDIO_SAMPLE_RATE AUD_SAMPRATE_48000
#define MIDDLE_BUFFER_SIZE (48 *2 *2 *4 *8)
#define CAPTURE_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)
#define PLAYBACK_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)
#define AUDIO_OUT_QUEUE_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 4)

static uint16_t capture_auido_trace_count = 0;
static uint16_t playback_auido_trace_count = 0;
static SRAM_BSS_LOC uint8_t capture_buffer[CAPTURE_BUFFER_SIZE];
static SRAM_BSS_LOC uint8_t playback_buffer[PLAYBACK_BUFFER_SIZE];
static SRAM_BSS_LOC uint8_t audio_output_queue_buf[AUDIO_OUT_QUEUE_BUFFER_SIZE];
CQueue audio_output_queue;

static uint32_t capture_input_cb(uint8_t *buf, uint32_t len)
{
    uint32_t queue_len = 0;
    capture_auido_trace_count++;
    static uint8_t enable_log = 0;
    if (capture_auido_trace_count == 100)
    {
        enable_log = 1;
        capture_auido_trace_count = 0;
		TRACE(0, "0x%2x,0x%02x ,0x%02x,0x%02x 0x%2x,0x%02x ,0x%02x,0x%02x", *buf,*(buf+1), *(buf+2),*(buf+3), *(buf+4),*(buf+5),*(buf+6),*(buf+7));
    }
    if (CQ_OK != EnCQueue(&audio_output_queue, buf, len))
    {
        TRACE(0,"capture_cb en queue error.");
    }
    queue_len = LengthOfCQueue(&audio_output_queue);
    if (enable_log)
    {
        TRACE(0,"[capture cb] queue_len = %d", queue_len);
		enable_log = 0;
    }

    return len;
}

static uint32_t playback_output_cb(uint8_t *buf, uint32_t len)
{
    uint32_t queue_len = 0;
    playback_auido_trace_count++;
    static uint8_t enable_log = 0;
    if (playback_auido_trace_count == 100)
    {
        enable_log = 1;
        playback_auido_trace_count = 0;
    }
    memset(buf, 0, len);

    if (CQ_OK != PeekCQueueToBuf(&audio_output_queue, buf, len))
    {
        TRACE(0,"playback peek queue error.");
    }
    queue_len = LengthOfCQueue(&audio_output_queue);

    if (CQ_OK != DeCQueue(&audio_output_queue, NULL, len))
    {
        TRACE(0,"playback de queue error.");
    }
    if (enable_log)
    {
        TRACE(0,"[playback cb] queue_len = %d", queue_len);
		enable_log = 0;
    }

    return len;
}


void audio_loop_test(uint8_t channel)
{
    struct AF_STREAM_CONFIG_T stream_cfg;

    TRACE(0,"%s, sample_rate = %d", __func__, AUDIO_SAMPLE_RATE);

    InitCQueue(&audio_output_queue, AUDIO_OUT_QUEUE_BUFFER_SIZE, (CQItemType *)audio_output_queue_buf);
    {
        // enable audio output
        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.bits = AUD_BITS_16;
        if(8 == channel)
        {
            stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7);
            stream_cfg.channel_num = AUD_CHANNEL_NUM_8;
        }
        else if(2 == channel)
        {
            stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
            stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
        }
        else
        {
            TRACE(0, "####ERROR: The i2s channel config error ,support 2 or 8");
        }

        stream_cfg.sample_rate = AUDIO_SAMPLE_RATE;
        stream_cfg.vol = 16;
#if AUDIO_USE_DEVICE_I2S
        stream_cfg.device = AUD_STREAM_USE_I2S1_MASTER;
#else
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#endif
        stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
        stream_cfg.handler = playback_output_cb;
        stream_cfg.data_ptr = playback_buffer;
        stream_cfg.data_size = PLAYBACK_BUFFER_SIZE;
        af_stream_open(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK, &stream_cfg);
        af_stream_start(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK);

        // enable audio input
        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.bits = AUD_BITS_16;

        if(8 == channel)
        {
            stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4 | AUD_CHANNEL_MAP_CH5 | AUD_CHANNEL_MAP_CH6 | AUD_CHANNEL_MAP_CH7);
            stream_cfg.channel_num = AUD_CHANNEL_NUM_8;
        }
        else if(2 == channel)
        {
            stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
            stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
        }
        else
        {
            TRACE(0, "####EROOR: The i2s channel config error ,support 2 or 8");
        }

        stream_cfg.sample_rate = AUDIO_SAMPLE_RATE;
        stream_cfg.vol = 16;
#if AUDIO_USE_DEVICE_I2S
        stream_cfg.device = AUD_STREAM_USE_I2S1_MASTER;
#else
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
#endif
        stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
        stream_cfg.handler = capture_input_cb;
        stream_cfg.data_ptr = capture_buffer;
        stream_cfg.data_size = CAPTURE_BUFFER_SIZE;
        af_stream_open(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE, &stream_cfg);
        af_stream_start(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE);
    }
}




#if 1 // adc->dac loop test
#include "hal_location.h"
#define MIDDLE_BUFFER_SIZE (192 * 10)
#define ADC_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)
#define DAC_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)

static SRAM_BSS_LOC uint8_t adc_buffer[ADC_BUFFER_SIZE];
static SRAM_BSS_LOC uint8_t dac_buffer[DAC_BUFFER_SIZE];

static uint8_t adc_cb_count = 0;
static uint8_t dac_cb_count = 0;
static uint8_t adc_log = 0;
static uint8_t dac_log = 0;

#include "cqueue.h"
CQueue adc_loop_dac_queue;
#define ADC_LOOP_DAC_QUEUE_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 4)
static SRAM_BSS_LOC uint8_t adc_loop_dac_queue_buf[ADC_LOOP_DAC_QUEUE_BUFFER_SIZE];

static const int16_t tone_1k_samp_48k[] = {
    0,
    3422,
    6785,
    10032,
    13107,
    15958,
    18536,
    20797,
    22702,
    24219,
    25321,
    25990,
    26214,
    25990,
    25321,
    24219,
    22702,
    20797,
    18536,
    15958,
    13107,
    10032,
    6785,
    3422,
    0,
    -3422,
    -6785,
    -10032,
    -13107,
    -15958,
    -18536,
    -20797,
    -22702,
    -24219,
    -25321,
    -25990,
    -26214,
    -25990,
    -25321,
    -24219,
    -22702,
    -20797,
    -18536,
    -15958,
    -13107,
    -10032,
    -6785,
    -3422,
};

static int16_t get_one_sample_of_1k_tone(void)
{
    static uint32_t offset_of_1k_tone = 0;
    int16_t sample = tone_1k_samp_48k[offset_of_1k_tone];
    uint32_t total_tone_sample_count = sizeof(tone_1k_samp_48k) / sizeof(int16_t);
    offset_of_1k_tone++;
    if (offset_of_1k_tone == total_tone_sample_count)
    {
        offset_of_1k_tone = 0;
    }
    return sample;
}

static void replace_1k_tone(int16_t *buf, uint32_t len, uint8_t ch_num)
{
    uint32_t remain_sample_count;
    uint32_t offset = 0;
    int16_t sample = 0;
    int i = 0;

    if (ch_num == 2)
    {
        remain_sample_count = len / sizeof(int16_t) / 2;
        while (remain_sample_count)
        {
            sample = get_one_sample_of_1k_tone();
            buf[offset++] = sample;
            buf[offset++] = sample;
            remain_sample_count--;
        }
        // for(i=0;i<96;i++)
        // {
        //     TRACE(0,"buf[%d] = %d (0x%0x)", i, buf[i], buf[i]);
        // }
        return;
    }

    if (ch_num == 1)
    {
        remain_sample_count = len / sizeof(int16_t);
        while (remain_sample_count)
        {
            sample = get_one_sample_of_1k_tone();
            buf[offset++] = sample;
            remain_sample_count--;
        }
        // for(i=0;i<48;i++)
        // {
        //     TRACE(0,"buf[%d] = %d (0x%0x)", i, buf[i], buf[i]);
        // }
        return;
    }
}

static uint32_t adc_cb(uint8_t *buf, uint32_t len)
{
    uint32_t queue_len = 0;
    adc_cb_count++;
    adc_log = 0;
    if (adc_cb_count == 100)
    {
        adc_log = 1;
        adc_cb_count = 0;
        //TRACE(0,"[adc_cb] len = %d", len);
    }

    if (CQ_OK != EnCQueue(&adc_loop_dac_queue, buf, len))
    {
        //TRACE(0,"adc en queue error.");
    }
    queue_len = LengthOfCQueue(&adc_loop_dac_queue);
    if (adc_log)
    {
        //TRACE(0,"[adc_cb] queue_len = %d", queue_len);
    }

    return len;
}

static uint32_t dac_cb(uint8_t *buf, uint32_t len)
{
    uint32_t queue_len = 0;
    dac_cb_count++;
    dac_log = 0;
    if (dac_cb_count == 100)
    {
        dac_log = 1;
        dac_cb_count = 0;
        //TRACE(0,"[dac_cb] len = %d", len);
    }
    memset(buf, 0, len);

#if 1
    replace_1k_tone((int16_t*)buf, len, 2);
#else
    if (CQ_OK != PeekCQueueToBuf(&adc_loop_dac_queue, buf, len))
    {
        //TRACE(0,"dac peek queue error.");
    }
    if (CQ_OK != DeCQueue(&adc_loop_dac_queue, NULL, len))
    {
        //TRACE(0,"dac de queue error.");
    }
    queue_len = LengthOfCQueue(&adc_loop_dac_queue);
    if (dac_log)
    {
        //TRACE(0,"[dac_cb] queue_len = %d", queue_len);
    }
#endif
    return len;
}

#if 1
#define ADC_LOOP_DAC_SAMPLE_RATE AUD_SAMPRATE_48000
#else
#define ADC_LOOP_DAC_SAMPLE_RATE AUD_SAMPRATE_96000
#endif

 void adc_loop_dac_test(void)
{
    struct AF_STREAM_CONFIG_T stream_cfg;

    TRACE(0,"%s, sample_rate = %d", __func__, ADC_LOOP_DAC_SAMPLE_RATE);

    InitCQueue(&adc_loop_dac_queue, ADC_LOOP_DAC_QUEUE_BUFFER_SIZE, (CQItemType *)adc_loop_dac_queue_buf);

    // enable dac
    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.bits = AUD_BITS_16;
	stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1);
    stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
    stream_cfg.sample_rate = ADC_LOOP_DAC_SAMPLE_RATE;
    stream_cfg.vol = 17;
    stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
    stream_cfg.io_path = AUD_OUTPUT_PATH_SPEAKER;
    stream_cfg.handler = dac_cb;
    stream_cfg.data_ptr = dac_buffer;
    stream_cfg.data_size = DAC_BUFFER_SIZE;
    af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, &stream_cfg);
    af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK);

#if 1
    // enable adc
    memset(&stream_cfg, 0, sizeof(stream_cfg));
    stream_cfg.bits = AUD_BITS_16;
	stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH0);
    stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
    stream_cfg.sample_rate = ADC_LOOP_DAC_SAMPLE_RATE;
    stream_cfg.vol = 17;
    stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
    stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
    stream_cfg.handler = adc_cb;
    stream_cfg.data_ptr = adc_buffer;
    stream_cfg.data_size = ADC_BUFFER_SIZE;
    af_stream_open(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE, &stream_cfg);
    af_stream_start(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
#endif
}
#endif

