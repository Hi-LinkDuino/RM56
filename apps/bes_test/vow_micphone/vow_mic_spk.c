#include <string.h>
#include "audioflinger.h"
#include "hal_trace.h"
#include "hal_location.h"
#include "hal_analogif.h"
#include "vow_mic_spk.h"
#include "hal_aud.h"
#include "vow_alsa.h"
#include "cmsis_os.h"
#include "vow_data.h"
#include "bwifi_interface.h"
#include "vow_key.h"

#define MIC_LOCAL_PLAYBACK  0


#define WADC_INPUT_STREAM_ID (AUD_STREAM_ID_0)
#define ADC_INPUT_BUFFER_SIZE (640 * 2) //16 * 2 * 1 =20ms VOW_FRAME_MAX_PAYLOAD_LEN = 96*15, avoid fragment when rf send.
#define ALIGNED4 ALIGNED(4)

void* alsa_handle = NULL;
/*
16K * 16bit *1 /s = 16 *2 *1=32Byte/ms
32Byte/ms * 20ms = 640Byte
*/

static VOW_AUDIO_CFG wadc_audio_cfg = {
    .sam_rate = AUD_SAMPRATE_16000,
    .vol = 16,
    .mute_state = 0,
    .depth = AUD_BITS_16,
    .ch = AUD_CHANNEL_NUM_1,
    .burst_size = 1,
    .delay = 20 * 1000,
};
SRAM_BSS_LOC static uint8_t ALIGNED4 adc_input_buf[ADC_INPUT_BUFFER_SIZE];
SRAM_BSS_LOC static uint8_t ALIGNED4 adc_temp[ADC_INPUT_BUFFER_SIZE / 2];
uint32_t g_buffer_len = 0;


static osSemaphoreId alsa_wait_sem;
osSemaphoreDef(alsa_wait_sem);

static osSemaphoreId record_wait_sem;
osSemaphoreDef(record_wait_sem);


void asla_evnet_happen()
{
    osSemaphoreRelease(alsa_wait_sem);
}

void alsa_evnet_waiting()
{
    osSemaphoreWait(alsa_wait_sem, osWaitForever);
}

void alsa_event_init()
{
    alsa_wait_sem = osSemaphoreCreate(osSemaphore(alsa_wait_sem), 0);

    if (alsa_wait_sem == NULL)
    {
        TRACE(0,"create event_pairing_sem fail\n");
        return ;
    }

    TRACE(0, "alsa sem init sucess");
}


void record_evnet_happen()
{
    osSemaphoreRelease(record_wait_sem);
}

void record_evnet_waiting()
{
    osSemaphoreWait(record_wait_sem, osWaitForever);
}

void record_event_init()
{
    record_wait_sem = osSemaphoreCreate(osSemaphore(record_wait_sem), 0);

    if (record_wait_sem == NULL)
    {
        TRACE(0,"create record_wait_sem fail\n");
        return ;
    }

    TRACE(0, "record sem init sucess");
}


static uint32_t adc_input_callback(uint8_t *buf, uint32_t len)
{
    g_buffer_len = len;
    memcpy(adc_temp, buf, len);
    asla_evnet_happen();
    return 0;
}

void wadc_open(bool on)
{
    TRACE(0,"wadc open");

    struct AF_STREAM_CONFIG_T stream_cfg;

    static bool isRun = false;

    if (on)
    {
        memset(&stream_cfg, 0, sizeof(stream_cfg));
        stream_cfg.bits = (enum AUD_BITS_T)wadc_audio_cfg.depth;
        if (wadc_audio_cfg.ch == 2)
        {
            stream_cfg.channel_num = AUD_CHANNEL_NUM_2;
        }
        else
        {
            stream_cfg.channel_num = AUD_CHANNEL_NUM_1;
        }
        stream_cfg.channel_map = AUD_CHANNEL_MAP_CH0;
        stream_cfg.sample_rate = (enum AUD_SAMPRATE_T)wadc_audio_cfg.sam_rate;
        stream_cfg.vol = wadc_audio_cfg.vol;
        stream_cfg.chan_sep_buf = false; // set true use seperate buffer for each channel, not cross
        stream_cfg.device = AUD_STREAM_USE_INT_CODEC;
        stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;
        stream_cfg.handler = adc_input_callback;
        stream_cfg.data_ptr = adc_input_buf;
        stream_cfg.data_size = ADC_INPUT_BUFFER_SIZE;
        af_stream_open(WADC_INPUT_STREAM_ID, AUD_STREAM_CAPTURE, &stream_cfg);
        af_stream_start(WADC_INPUT_STREAM_ID, AUD_STREAM_CAPTURE);
        af_stream_set_chan_vol(WADC_INPUT_STREAM_ID, AUD_STREAM_CAPTURE, (enum AUD_CHANNEL_MAP_T)AUD_CHANNEL_MAP_CH0, (uint8_t)wadc_audio_cfg.vol);
    } else {
        af_stream_stop(WADC_INPUT_STREAM_ID, AUD_STREAM_CAPTURE);
        af_stream_close(WADC_INPUT_STREAM_ID, AUD_STREAM_CAPTURE);
    }
}


static bool isinit = false;

void vow_record_start()
{
    if (!isinit) {
        isinit = true;
        wadc_open(true);
    }
}

void vow_record_stop()
{
    if (isinit) {
        isinit = false;
        wadc_open(false);
    }
}

void record_task_handler(void* arg)
{
    while (1) {
        record_evnet_waiting();
        if(vow_get_key_status() == KEY_RECORD_START) {
            vow_record_start();
        } else if (vow_get_key_status() == KEY_RECORD_STOP) {
            vow_record_stop();
        }
    }
}

static osThreadId record_handle_tid;
osThreadDef(record_task_handler, osPriorityAboveNormal, 1, (8 * 1024), "record");

void record_task_init(void)
{
    TRACE(0,"enter %s %d", __func__, __LINE__);

    record_handle_tid = osThreadCreate(osThread(record_task_handler), NULL);
    if (record_handle_tid == NULL)
    {
        TRACE(0,"%s %d fail to create my_main task", __func__, __LINE__);
        return;
    }
    TRACE(0,"create alsa_task_init  = 0x%X", (uint32_t)record_handle_tid);

    return;
}

void alsa_tash_handler(void* arg)
{
    while (1) {
        alsa_evnet_waiting();

#if MIC_LOCAL_PLAYBACK
        sink_alsa_write(alsa_handle, adc_temp, g_buffer_len);
#endif
        vow_send_data(adc_temp, g_buffer_len);
    }
}

static osThreadId alsa_hanle_tid;
osThreadDef(alsa_tash_handler, osPriorityAboveNormal, 1, (8 * 1024), "alsa_app");

void alsa_task_init(void)
{
    TRACE(0,"enter %s %d", __func__, __LINE__);

    alsa_hanle_tid = osThreadCreate(osThread(alsa_tash_handler), NULL);
    if (alsa_hanle_tid == NULL)
    {
        TRACE(0,"%s %d fail to create my_main task", __func__, __LINE__);
        return;
    }
    TRACE(0,"create alsa_task_init  = 0x%X", (uint32_t)alsa_hanle_tid);

    return;
}

//#define SSID_INFO   "BES_SZoffices"
//#define PWD_INFO    "BESsz@@2021!!"
//#define SSID_INFO   "TP-LINK_5G_EFC3"
#define SSID_INFO     "bes_mic_test"
#define PWD_INFO      "12345678"


void wadc_app_start()
{
    BWIFI_STATUS_T wifi_status  = BWIFI_STATUS_IDLE;

    alsa_task_init();
    alsa_event_init();

#if MIC_LOCAL_PLAYBACK
    alsa_handle = sink_alsa_open(wadc_audio_cfg.sam_rate, wadc_audio_cfg.ch, wadc_audio_cfg.depth);
    sink_alsa_start(alsa_handle);
#endif

    while (wifi_status != BWIFI_STATUS_GOT_IP) {
        bwifi_connect_to_ssid((char*)SSID_INFO, PWD_INFO, 0, 0, NULL);
        wifi_status = bwifi_get_current_status();
        osDelay(500);
    }

    vow_data_handle_init();

    record_task_init();
    record_event_init();

    vow_key_init();

}
