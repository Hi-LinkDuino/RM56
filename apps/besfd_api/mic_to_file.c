#include "besfd_api.h"

#include "audioflinger.h"
#include <string.h>
#include "hal_trace.h"
#include "hal_location.h"
#include "cqueue.h"
#include "heap_api.h"
#include "cmsis_os.h"

#include "hal_timer.h"
#include "aud_player.h"

#define USE_DATA_DUMP
// #define MIC_TO_RAM //record pcm to RAM instead of file

/******************************************************************************
 * mic file size limit: 1M
 * for example, given as chan=2
 * - MIC_FILE_SAMPLE_RATE = 16000
 *      1s: 16000*2*2=64000
 *      up to 16s
 * - MIC_FILE_SAMPLE_RATE = 48000
 *      1s: 48000*2*2=192000
 *      up to 5s
 *
 ******************************************************************************/
#define MIC_FILE_LIMIT 0x100000 // 1M
#define MIC_FILE_SAMPLE_RATE 16000
#define MIC_FILE_SECONDS 15
#define MIC_FILE_CHAN 2

static uint32_t total = 0;
static uint8_t *mic_buffer = NULL;
static int wav_buffer_write(uint8_t *buf, uint32_t len){
    int n = len;
    if(total + len > MIC_FILE_LIMIT) n = MIC_FILE_LIMIT - total;
    memcpy(mic_buffer+total, buf, n);
    total += n;
    TRACE(0, "%s: %d/%d, total=0x%X/0x%X bytes", __func__, n, len, total, MIC_FILE_LIMIT);
    return n;
}
static void mic_buffer_play_done(int cause){
    TRACE(0, "%s cause=%d", __func__, cause);
    if(mic_buffer){
        vPortFree(mic_buffer);
        mic_buffer = NULL;
    }
}
static FILE *fp = NULL;

#ifdef USE_DATA_DUMP
#if (defined(__A7_DSP_ENABLE__) || defined(MCU_DSP) || defined(CP_DSP))
#include "mcu_audio.h"
#include "data_dump.h"
#endif
static bool mic_to_file_running = false;
#else
static void mic_to_file_timer_handler(void const *param);
osTimerDef(TIMER, mic_to_file_timer_handler);
static osTimerId timer_id = NULL;

static void mic_to_file_timer_handler(void const *param)
{
    mic_to_file_stop();
}
static uint32_t mic_to_file_cb(uint8_t *buf, uint32_t len)
{
    if (fp)
    {
        int ret = wav_file_write(fp, buf, len);
        if(ret > 0)total += ret;
        TRACE(0, "%s: %d/%d, total=%d(0x%X) bytes", __func__, ret, len, total, total);
    }  else {
        wav_buffer_write(buf, len);
    }
    return len;
}
#endif

static int mic_to_file_check(uint32_t seconds, char *fname){
#ifdef USE_DATA_DUMP
    if (mic_to_file_running)
    {
        TRACE(0, "%s is running, only one instance is allowed", __func__);
        return -1;
    }
#else
    if (timer_id != NULL)
    {
        TRACE(0, "%s is running, only one instance is allowed", __func__);
        return -1;
    }
#endif
    if (seconds > MIC_FILE_SECONDS)
    {
        TRACE(0, "%s recording duration MUST NOT larger than %d seconds", __func__, MIC_FILE_SECONDS);
        return -1;
    }
    if (media_is_current_playing(MT_HFP))
    {
        TRACE(0, "%s IN CALL now, please wait", __func__);
        return -1;
    }
    return 0;
}

/**
 * [Comment]
 * When fclose(), flush to flash will cause player interrupted
 */

#define SAMPLES     256 //>=512 cause player interrupted
#define AUD_PLAYER_C
#include "lplayer.h"

/*
 * This file provides an example of playing streaming PCM
 * important API：
 * 1：lplayer_open_buffer                       Create lplayer handle and it's buffer
 * 2：lplayer_set_stream_buffer_parameter       Set PCM Info: channels sampleRate  bitCount
 * 3：lplayer_buffer_write                      Write data to internal buffer
 * 4：lplayer_play                              Start audio play
 * 5：lplayer_close                             Stop  audio play
*/

#include <stdlib.h>
#include <string.h>
#include "lplayer.h"
#include "hal_trace.h"
#include "cmsis_os2.h"



static void lplayer_test_play_file(char *fname){
    lplayer_t * inst = lplayer_open_url(fname);
    TRACE(0, "%s(%s) player=%p", __func__, fname, inst);
    int ret = lplayer_play(inst);
    TRACE(0, "%s play......%d ", __func__, ret);
    //FIXME: lplayer instance not free
}
void lplayer_test_play_file2(char *fname){
    if(!mic_buffer){
        mic_buffer = pvPortMalloc(MIC_FILE_LIMIT);
    }
    //Load file to RAM
    FILE *fh = fopen(fname, "r");
    int size = fseek(fh, 0, SEEK_END);
    TRACE(0, "%s(%s) size=%d", __func__, fname, size);
    fseek(fh, 0, SEEK_SET);
    fread(mic_buffer, 1, size, fh);
    fclose(fh);
    //Play with RAM
    total = size;
    aud_player_start_buffer(mic_buffer, size, mic_buffer_play_done);
}
/**
 * @brief mic record for seconds and save to file
 * running in AT context
 * @param seconds
 * @param fname                NULL-record to ram file
 * @return int
 */
int mic_to_file_start(uint32_t seconds, char *fname)
{
    TRACE(0, "%s: fname=%s, duration=%d seconds", __func__, fname, seconds);
    int ret = mic_to_file_check(seconds, fname);
    if(ret)return ret;

    /*********************************************
     * Create wav file
     *********************************************/
    total = 0;
    if(fname && *fname){
        fp = wav_file_new(fname, MIC_FILE_SAMPLE_RATE, MIC_FILE_CHAN);
        if (fp == NULL)
        {
            return -1;
        }
    } else {
        if(!mic_buffer){
            mic_buffer = pvPortMalloc(MIC_FILE_LIMIT);
        }
        total = wav_buffer_new(mic_buffer, MIC_FILE_SAMPLE_RATE, MIC_FILE_CHAN);
    }
#ifdef USE_DATA_DUMP
    /**
     * get record data
     */
    unsigned int size_KB = 32;
    void *handle = data_dump_open(aud_record_subscribe, size_KB, MIC_FILE_SAMPLE_RATE, 1);
    uint8_t record_buf[CHAN_NUM_CAPTURE * SAMPLES * 2] = {0};

    unsigned int record_start_time = TICKS_TO_MS(hal_sys_timer_get());
    // TRACE(0, "[%s] record_start_time : %d\n",__func__,record_start_time);
    mic_to_file_running = true;
    while (mic_to_file_running && TICKS_TO_MS(hal_sys_timer_get()) < (unsigned int)(record_start_time + seconds * 1000))
    {
        memset(record_buf, 0, sizeof(record_buf));
        int read_len = data_dump_read(handle, record_buf, sizeof(record_buf));
        /* handle pcm data ,dump to ftp server */
        if (read_len > 0)
        {
            // TRACE(0, "[%s] read_len : %d\n",__func__,read_len);
            int ret;
            if(CHAN_NUM_CAPTURE == 3){
                uint8_t buf[2 * SAMPLES * 2];
                for(int i = 0; i*3 < read_len; i += 2){
                    memcpy(&buf[i*2], &record_buf[i*3+2], 4);
                }
                read_len = read_len * 2 / 3;
                if(fp){
                    ret = wav_file_write(fp, buf, read_len);
                    if(ret > 0)total += ret;
                    TRACE(0, "%s: %d/%d, total=0x%X bytes, %d ms", __func__, ret, read_len, total, TICKS_TO_MS(hal_sys_timer_get()) - record_start_time);            
                } else {
                    ret = wav_buffer_write(buf, read_len);
                }
            } else {
                if(fp){
                    ret = wav_file_write(fp, record_buf, read_len);
                    if(ret > 0)total += ret;
                    TRACE(0, "%s: %d/%d, total=0x%X bytes, %d ms", __func__, ret, read_len, total, TICKS_TO_MS(hal_sys_timer_get()) - record_start_time);            
                } else {
                    ret = wav_buffer_write(record_buf, read_len);
                }
            }
        }
        osDelay(10);
    }
    data_dump_close(handle, aud_record_unsubscribe);
    TRACE(0, "%s(%s) done, total record %ds(%d ms), data size %d bytes\n", __func__, fname, seconds, TICKS_TO_MS(hal_sys_timer_get()) - record_start_time, total);
    mic_to_file_stop();
    return total;
#else
	af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);

    /*********************************************
     * Start recording
     *********************************************/
    aud_capture_start(2, MIC_FILE_SAMPLE_RATE, TGT_VOLUME_LEVEL_MAX, mic_to_file_cb);
    timer_id = osTimerCreate(osTimer(TIMER), osTimerOnce, NULL);
    osTimerStart(timer_id, seconds * 1000);
    return 0;
#endif
}

void mic_to_file_stop(void){
#ifdef USE_DATA_DUMP
    if(!mic_to_file_running)return;
    mic_to_file_running = false;
#else
    if(!timer_id)return;
    aud_capture_stop();
    osTimerStop(timer_id);
    timer_id = NULL;
#endif

    TRACE(0, "%s", __func__);
    if (fp)
    {
        wav_file_done(fp, total);
        fp = NULL;
        // cmd_send("AT+LPLAYER_START=/record.wav");
        aud_player_start("/record.wav", NULL);
        // FTP_HANDLE handler = ftp_connect();
        // if(handler){
        // }
    } else if(mic_buffer){
        TRACE(0, "%s, buf=%p, size=0x%x", __func__, mic_buffer, total);
        wav_buffer_done(mic_buffer, total);
        for (int i = 0; i < 64; i += 16)
        {
            DUMP8(" %02X", &mic_buffer[i], 16);
        }
        // osDelay(1*1000);
        TRACE(0, "%s, aud_player_start_buffer", __func__);
        aud_player_start_buffer(mic_buffer, total, mic_buffer_play_done);
    }
}

