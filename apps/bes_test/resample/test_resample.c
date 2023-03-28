//#define RESAMPLE_TEST
#ifdef RESAMPLE_TEST

#include <stdlib.h>
#include <string.h>
#include "hal_trace.h"
#include "alsa.h"

#define RESAMPLE_PLAY_SAMPLE_RATE   (48000)
#define RESAMPLE_PLAY_CHANNELS_NUM  (1)
#define RESAMPLE_PLAY_BUF_SIZE      (1024)

static const uint32_t resample_play_file_16K[] =
{
    #include "16kHz-0db.pcm"
};
static const uint32_t resample_play_file_17K[] =
{
    #include "17kHz-0db.pcm"
};
static const uint32_t resample_play_file_18K[] =
{
    #include "18kHz-0db.pcm"
};
static const uint32_t resample_play_file_19K[] =
{
    #include "19kHz-0db.pcm"
};
static const uint32_t resample_play_file_20K[] =
{
    #include "20kHz-0db.pcm"
};
static const uint32_t resample_play_file_80[] =
{
    #include "80Hz-0db.pcm"
};
static uint32_t resample_silence_data[RESAMPLE_PLAY_BUF_SIZE/4] = {0};

int bes_resample_paly_file(alsa_handle_t * h, uint8_t * play_file_ptr, uint32_t size)
{
    if(!play_file_ptr || !size || !h)
        return -1;

    uint8_t * play_buf = (uint8_t*)calloc(RESAMPLE_PLAY_BUF_SIZE, sizeof(uint8_t));
    if(!play_buf)
        return -1;

    uint8_t *silence_data_ptr = (uint8_t*)resample_silence_data;
    uint32_t remain_len = size;
    uint32_t write_len = 0;
    uint32_t offset = 0;

    while(remain_len)
    {
        write_len = remain_len > RESAMPLE_PLAY_BUF_SIZE ? RESAMPLE_PLAY_BUF_SIZE : remain_len;
        memcpy(play_buf, play_file_ptr+offset, write_len);
        alsa_write(h, play_buf, write_len);
        remain_len -= write_len;
        offset += write_len;
    }

#if 1
    // 2s silence data
    remain_len = 192*1000*2;
    while(remain_len)
    {
        write_len = remain_len > RESAMPLE_PLAY_BUF_SIZE ? RESAMPLE_PLAY_BUF_SIZE : remain_len;
        memcpy(play_buf, silence_data_ptr, write_len);
        alsa_write(h, play_buf, write_len);
        remain_len -= write_len;
    }
#endif

    free(play_buf);
    return 0;
}

int bes_resample_play_test(void)
{
    alsa_volume_set(17);

    alsa_handle_t * h = alsa_open(ALSA_MODE_OUT, RESAMPLE_PLAY_SAMPLE_RATE, RESAMPLE_PLAY_CHANNELS_NUM, ALSA_PCM_FORMAT_S16_LE);
    if(!h)
        return -1;

	alsa_start(h);
	alsa_volume_set(17);
	
    bes_resample_paly_file(h, (uint8_t*)resample_play_file_16K, sizeof(resample_play_file_16K));
    bes_resample_paly_file(h, (uint8_t*)resample_play_file_17K, sizeof(resample_play_file_17K));
    bes_resample_paly_file(h, (uint8_t*)resample_play_file_18K, sizeof(resample_play_file_18K));
    bes_resample_paly_file(h, (uint8_t*)resample_play_file_19K, sizeof(resample_play_file_19K));
    bes_resample_paly_file(h, (uint8_t*)resample_play_file_20K, sizeof(resample_play_file_20K));

    //alsa_volume_set(17);
    alsa_stop(h);
    alsa_close(h);

    return 0;
}
#endif//RESAMPLE_TEST
int bes_resample_test(void)
{
#ifdef RESAMPLE_TEST
    bes_resample_play_test();
#endif

    return 0;
}
