
#if defined(__NET_FTP_CLIENT_SUPPORT__) && defined(__A7_DSP_ENABLE__)

#include <stdio.h>
#include "besfd_api.h"


#include "cmsis_os.h"
#ifdef osCMSIS_FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#if (defined(__A7_DSP_ENABLE__) || defined(MCU_DSP) || defined(CP_DSP))
#include "mcu_audio.h"
#include "data_dump.h"
#endif


#if defined(__NET_FTP_CLIENT_SUPPORT__)
#include "ftp_client.h"
#endif

#include "hal_timer.h"


/**
 * @brief
 * mic record and upload 3-channels raw audio data to ftp
 *
 * @param ftp_handler
 * @param sample_rate 16000/48000
 * @param seconds   duration
 * @return int      <0: error failed, >= 0: datalength in bytes uploaded to ftp
 */
int mic_to_ftp(FTP_HANDLE ftp_handler, uint32_t sample_rate, uint32_t seconds)
{
    int format = 0; //-1; 0-S16
    int ret = -1;
    char fname[20] = {0};

    uint32_t tick = hal_sys_timer_get();
    sprintf(fname, "mic3-%d", tick);

    if ((ret = ftp_client_notice(ftp_handler, fname)) != 0)
    {
        TRACE(0, "[%s] notice ftp server %s fail: %d\n", __func__, fname, ret);
        goto out;
    }
    /**
     * get record data
     */
    unsigned int size_KB = 32;
    void *handle = data_dump_open(aud_record_subscribe, size_KB, sample_rate, 1);
    unsigned total_record_size = 0;
    uint8_t record_buf[CHAN_NUM_CAPTURE * 1024] = {0};

    unsigned int record_start_time = TICKS_TO_MS(hal_sys_timer_get());
    // TRACE(0, "[%s] record_start_time : %d\n",__func__,record_start_time);
    while (TICKS_TO_MS(hal_sys_timer_get()) < (unsigned int)(record_start_time + seconds * 1000))
    {
        memset(record_buf, 0, sizeof(record_buf));
        int read_len = data_dump_read(handle, record_buf, sizeof(record_buf));
        /* handle pcm data ,dump to ftp server */
        if (read_len > 0)
        {
            // TRACE(0, "[%s] read_len : %d\n",__func__,read_len);
            total_record_size += ftp_client_dumpdata(ftp_handler, (const char *)record_buf, read_len);
        }
        // osDelay(1);
    }
    data_dump_close(handle, aud_record_unsubscribe);
    TRACE(0, "%s(%s) done, total record %ds(%d ms), data size %d bytes\n", __func__, fname, seconds, TICKS_TO_MS(hal_sys_timer_get()) - record_start_time, total_record_size);
    ret = total_record_size;
out:
    return ret;
}

#endif