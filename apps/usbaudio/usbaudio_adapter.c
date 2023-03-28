#ifdef USB_AUDIO_ADAPTER

#include "cmsis_os.h"
#include "cmsis.h"
#include "hal_trace.h"
#include "app_utils.h"
#include "usb_audio_app.h"
#include "usbaudio_thread.h"
#include "wifiusb_audio.h"
#include "string.h"
#include "alsa.h"
#include "hal_location.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_codec.h"
#include "hal_aud.h"
#include "audioflinger.h"
#include "analog.h"
#include "usb_audio.h"

#include "usbaudio_adapter.h"
#include "usbaudio_exp.h"
#include "hal_bootmode.h"
#include "net_memory.h"
#include "usb_audio_frm_defs.h"

static int g_usbaudio_testmode = 0;
static UsbAudioExpCallback usbaudio_notify = NULL;

#define USB_SIGNAL_ID 0xAA
#define USB_SPEAK_BUFFER_SIZE 4*1024
#define STREAM_DEVICE_USB_STREAM_ID AUD_STREAM_ID_0

static uint8_t usb_speak_buffer[USB_SPEAK_BUFFER_SIZE];
UsbAudioPcmOut   usb_speak_pcm;
alsa_handle_t * alsa_handle;
static uint8_t alsa_playback_buf[1024];
uint32_t play_packet_size = 0;
void usbaudio_player_oper_init(void);

osThreadId usbaudio_thread_tid;
static void usbaudio_player_thread(void const *argument);
osThreadDef(usbaudio_player_thread, osPriorityNormal, 1, 1024*8, "usbaudio_player");
static uint8_t g_wait_capture = 0;

#define USB_PCM_CAP_BUFFER_SIZE 32*1024
static uint32_t capture_status;
UsbAudioCaptureIn usb_capture_in;
static uint8_t usb_capture_in_buffer[USB_PCM_CAP_BUFFER_SIZE];
osSemaphoreDef(usb_cap_semaphore);
static osSemaphoreId g_usb_cap_semaphore;
uint32_t CAPTURE_FRAM_SIZE = USB_MUTI_MIC*SAMPLE_SIZE_SEND*(SAMPLE_RATE_SEND/1000);
uint8_t usb_capture_buf[1*1024];
uint8_t _USB_MUTI_MIC = USB_MUTI_MIC;

static void usb_capture_thread(void const *argument);
osThreadDef(usb_capture_thread, osPriorityNormal, 1, 1024*8, "usb_capture_thread");


uint32_t usbaudio_capture_read(uint8_t *buffer, uint32_t size)
{
    uint32_t ret = 0;
    uint32_t fifolen = 0;
    uint32_t lock;

    if (size == 0 || buffer == NULL)
        return 0;
    lock = int_lock();
    fifolen = kfifo_len(&usb_capture_in.fifo);
    size = (size <= fifolen) ? size : fifolen;

    if (size)
        ret = kfifo_get(&usb_capture_in.fifo, buffer, size);
    int_unlock(lock);
    return ret;
}

uint32_t usbaudio_capture_write(uint8_t *buffer, uint32_t size)
{
    uint32_t ret = 0;
    uint32_t lock;

    if (size == 0 || buffer == NULL)
        return 0;

    lock = int_lock();
    ret = kfifo_available(&usb_capture_in.fifo);
    if (ret == 0) {
        int_unlock(lock);
        return ret;
    }

    ret = kfifo_put(&usb_capture_in.fifo, buffer, size);
    int_unlock(lock);
    return ret;
}

void usbaudio_cap_buffer_clean(void)
{
    kfifo_init(&usb_capture_in.fifo, usb_capture_in_buffer, USB_PCM_CAP_BUFFER_SIZE);
}

int UsbAudioRegisterClient(UsbAudioExpNotify *callback)
{
    usbaudio_notify = callback->notifier_call;
    return 0;
}

__attribute__((weak)) int32_t AppUsbAudioPlay(uint8_t *buf, uint32_t len)
{
    int32_t ret = 0;
    if (alsa_handle) {
        alsa_write(alsa_handle, buf, len);
    }
    return ret;
}

__attribute__((weak)) int32_t app_usb_audio_open(int sample_rate, int channels, int audio_bits)
{
    int32_t ret = 0;
    alsa_pcm_format_t format;
    TRACE(0,"%s %s", __FILE__, __func__);
    if (audio_bits == AUD_BITS_16) {
        format = ALSA_PCM_FORMAT_S16_LE;
    } else if (audio_bits == AUD_BITS_24) {
        format = ALSA_PCM_FORMAT_S24_LE;
    } else if (audio_bits == AUD_BITS_32) {
        format = ALSA_PCM_FORMAT_S32_LE;
    } else {
        TRACE(0,"%s audio_bits=%d default use ALSA_PCM_FORMAT_S16_LE",__func__, audio_bits);
        format = ALSA_PCM_FORMAT_S16_LE;
    }
    alsa_handle = alsa_open(ALSA_MODE_OUT, sample_rate, channels, format);
    TRACE(0,"%s %s alsa_handle=%p format=%d", __FILE__, __func__, alsa_handle, format);
    if (alsa_handle) {
        alsa_start(alsa_handle);
    }
    return ret;
}

__attribute__((weak)) int32_t app_usb_audio_start(void)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    af_stream_start(STREAM_DEVICE_USB_STREAM_ID, AUD_STREAM_PLAYBACK);
    return ret;
}

__attribute__((weak)) int32_t AppUsbAudioClose(void)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    af_stream_close(STREAM_DEVICE_USB_STREAM_ID, AUD_STREAM_PLAYBACK);
    return ret;
}

__attribute__((weak)) int32_t AppUsbAudioStop(void)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    af_stream_stop(STREAM_DEVICE_USB_STREAM_ID, AUD_STREAM_PLAYBACK);
    return ret;
}

__attribute__((weak)) int32_t app_usb_audio_alsa_close(void)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    if (alsa_handle != NULL) {
        alsa_close(alsa_handle);
        alsa_handle = NULL;
    }
    return ret;
}

__attribute__((weak)) int32_t app_usb_audio_alsa_stop(void)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    if (alsa_handle != NULL) {
        alsa_stop(alsa_handle);
    }
    return ret;
}

__attribute__((weak)) int32_t AppUsbVolSet(uint8_t vol)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    alsa_volume_set(vol);
    return ret;
}

__attribute__((weak)) int32_t AppUsbMuteSet(void)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    alsa_mute_set();
    return ret;
}

__attribute__((weak)) int32_t AppUsbMuteCancel(void)
{
    int32_t ret = 0;
    TRACE(0,"%s %s", __FILE__, __func__);
    alsa_mute_cancel();
    return ret;
}


void usb_audio_play_close(void)
{
    app_usb_audio_alsa_stop();
    app_usb_audio_alsa_close();
}

alsa_handle_t * usb_audio_play_open(void)
{
    enum AUD_SAMPRATE_T sample_rate = 0;
    uint8_t sample_size = 0;
    uint8_t ch_recv = 0;
    enum AUD_BITS_T af_format_bits = AUD_BITS_16;
    struct USB_AUDIO_SMAPLE_INFO_T usbaudio_sample_info;
    alsa_pcm_format_t format;

    usb_get_sample_info(&usbaudio_sample_info);
    sample_rate = usbaudio_sample_info.sample_rate_recv;
    ch_recv = usbaudio_sample_info.ch_recv;
    if (usbaudio_sample_info.sample_size_recv == 2) {
        format = ALSA_PCM_FORMAT_S16_LE;
        af_format_bits = AUD_BITS_16;
    } else if (usbaudio_sample_info.sample_size_recv == 3) {
        format = ALSA_PCM_FORMAT_S24_LE;
        af_format_bits = AUD_BITS_24;
    } else {
        format = ALSA_PCM_FORMAT_S32_LE;
        af_format_bits = AUD_BITS_32;
    }
    AppUsbAudioStop();
    AppUsbAudioClose();
    app_usb_audio_alsa_stop();
    app_usb_audio_alsa_close();
    app_usb_audio_open(sample_rate, ch_recv, af_format_bits);
    TRACE(0,"sample_rate=%d, ch_recv=%d, af_format_bits=%d", sample_rate, ch_recv, af_format_bits);
    return alsa_handle;
}

static void usbaudio_player_thread(void const *argument)
{
    uint32_t len = 0;
    osEvent evt;
    UsbAudioExpInfo notify_data;

    TRACE(0,"usbaudio_player_thread func start");
    while(1) {
        evt = osSignalWait(USB_SIGNAL_ID, osWaitForever);
        if (evt.status == osEventSignal) {
            len = UsbAudioPlayRead(alsa_playback_buf, play_packet_size);
            if (usbaudio_notify) {
                notify_data.type = USB_AUDIO_NOTIFY_DATA;
                notify_data.info.data.len = len ;
                notify_data.info.data.buff = (uint8_t*)alsa_playback_buf;
                usbaudio_notify(&notify_data);
            } else {
                AppUsbAudioPlay(alsa_playback_buf, len);
            }
        } else {
            TRACE(0,"[%s] ERROR: evt.status = %d", __func__, evt.status);
            continue;
        }
    }
}

static void usb_capture_thread(void const *argument)
{
    uint32_t len = 0;
    TRACE(2,"usb_capture_thread func start");
    while(1) {
        osSemaphoreWait(g_usb_cap_semaphore, osWaitForever);
        len = usbaudio_capture_read(usb_capture_buf, CAPTURE_FRAM_SIZE);
        if (len == 0) {
            TRACE(0, "cap kfifo no data");
            //osDelay(1);
        } else  {
            //TRACE(0, "write to usb, len=%d", len);
            usb_audio_data_capture_handler(usb_capture_buf,len);
        }
    }
}

unsigned int CaptureDataToUsbNormalMode(unsigned char *pData, unsigned int len)
{
    uint32_t ret = 0;

    if (g_usbaudio_testmode)
        return 0;
    if (capture_status)
    {
        ret = usbaudio_capture_write(pData, len);
    } else {
        //TRACE(0, "capture_status = %d", capture_status);
    }
    return ret;
}

unsigned int CaptureDataToUsbTestMode(unsigned char *pData, unsigned int len)
{
    uint32_t ret = 0;
    if (!g_usbaudio_testmode)
        return 0;
    if (capture_status)
    {
        ret = usbaudio_capture_write(pData, len);
    } else {
        //TRACE(0, "capture_status = %d", capture_status);
    }
    return ret;
}

void release_cap_sem(void)
{
    osSemaphoreRelease(g_usb_cap_semaphore);
}

static void usb_audio_input(USB_AUDIO_INFO *audio_info)
{
    UsbAudioExpInfo notify_info;

    if(audio_info->type == USB_AUDIO_SUBMIT_CMD)
    {
        if (usbaudio_notify) {
            if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_START_CAPTURE ||
               audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_SET_SEND_RATE) {
                /*to do*/
                TRACE(2,"notify start capture cmd=%d", audio_info->info.cmd.cmd);
                wifiusb_start_capture2(audio_info->info.cmd.seq);
                capture_status = 1;
            } else if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_STOP_CAPTURE) {
                /*to do*/
                TRACE(2,"notify stop capture cmd=%d", audio_info->info.cmd.cmd);
                wifiusb_stop_capture2();
                capture_status = 0;
            } else {
                notify_info.type = audio_info->type;
                notify_info.info.cmd.cmd = audio_info->info.cmd.cmd;
                notify_info.info.cmd.seq = audio_info->info.cmd.seq;
                notify_info.info.cmd.arg = audio_info->info.cmd.arg;
                usbaudio_notify(&notify_info);
            }
        } else {
            TRACE(0,"%s: cmd: %d,seq: %d, arg:%d", __func__,
                            audio_info->info.cmd.cmd,
                            audio_info->info.cmd.seq,
                            audio_info->info.cmd.arg);
            if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_START_CAPTURE ||
               audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_SET_SEND_RATE) {
                /*to do*/
                TRACE(2,"usb audio start capture cmd=%d", audio_info->info.cmd.cmd);
                wifiusb_start_capture2(audio_info->info.cmd.seq);
                capture_status = 1;
            } else if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_STOP_CAPTURE) {
                /*to do*/
                TRACE(2,"usb audio stop capture cmd=%d", audio_info->info.cmd.cmd);
                wifiusb_stop_capture2();
                capture_status = 0;
            } else if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_SET_VOLUME) {
                TRACE(0,"usb audio set volume cmd");
                alsa_volume_set(audio_info->info.cmd.arg);
            } else if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_MUTE_CTRL) {
                TRACE(0,"usb audio mute cmd");
                if (audio_info->info.cmd.arg == 1) {
                    alsa_mute_set();
                } else {
                    alsa_mute_cancel();
                }
            } else if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_START_PLAY ||
                    audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_SET_RECV_RATE) {
                TRACE(0,"usb audio start play cmd=%d", audio_info->info.cmd.cmd);
                usb_audio_play_open();
            } else if(audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_STOP_PLAY) {
                TRACE(0,"usb audio stop play cmd");
                usb_audio_play_close();
            } else if (audio_info->info.cmd.cmd == WIFIUSB_AUDIO_CMD_USB_CONFIG) {
                TRACE(0,"usb audio cmd usb config");
            }
        }
    } else if(audio_info->type == USB_AUDIO_SUBMIT_DATA) {
        if (alsa_handle || usbaudio_notify != NULL) {
            UsbAudioPlayWrite(audio_info->info.data.buff, audio_info->info.data.len);
            play_packet_size = audio_info->info.data.len;
            osSignalSet(usbaudio_thread_tid,USB_SIGNAL_ID);
        }
    }
}

uint32_t UsbAudioPlayRead(uint8_t *buffer, uint32_t size)
{
    uint32_t ret = 0;
    uint32_t fifolen = 0;
    uint32_t lock;

    if (size == 0 || buffer == NULL)
        return 0;
    lock = int_lock();
    fifolen = kfifo_len(&usb_speak_pcm.fifo);
    size = (size <= fifolen) ? size : fifolen;

    if (size)
        ret = kfifo_get(&usb_speak_pcm.fifo, buffer, size);
    int_unlock(lock);
    return ret;
}

uint32_t UsbAudioPlayWrite(uint8_t *buffer, uint32_t size)
{
    uint32_t ret = 0;
    uint32_t lock;

    if (size == 0 || buffer == NULL)
        return 0;

    lock = int_lock();
    ret = kfifo_available(&usb_speak_pcm.fifo);
    if (ret == 0) {
        int_unlock(lock);
        return ret;
    }

    ret = kfifo_put(&usb_speak_pcm.fifo, buffer, size);
    int_unlock(lock);
    return ret;
}

void UsbAudioBufClean(void)
{
    kfifo_init(&usb_speak_pcm.fifo, usb_speak_buffer, USB_SPEAK_BUFFER_SIZE);
}

void usbaudio_player_oper_init(void)
{
    usb_speak_pcm.read = UsbAudioPlayRead;
    usb_speak_pcm.write = UsbAudioPlayWrite;
    usb_speak_pcm.clean = UsbAudioBufClean;
    kfifo_init(&usb_speak_pcm.fifo, usb_speak_buffer, USB_SPEAK_BUFFER_SIZE);

    usb_capture_in.read = usbaudio_capture_read;
    usb_capture_in.write = usbaudio_capture_write;
    usb_capture_in.clean = usbaudio_cap_buffer_clean;
    kfifo_init(&usb_capture_in.fifo, usb_capture_in_buffer, USB_PCM_CAP_BUFFER_SIZE);

}

void UsbAudioDevInit(unsigned int recvInterval)
{
    uint32_t submit_size;
    uint32_t xtal_time = 0;

    submit_size = 48*2*2*recvInterval/1000; //48k 2ch 16bit
    TRACE(0,"%s, line=%d, recvInterval=%d, submit_size=%d",__func__, __LINE__, recvInterval, submit_size);

    while (1)
    {
        if(g_wait_capture) {
            TRACE(2, "capture init done");
            break;
        } else {
            osDelay(5);
            TRACE(2, "wait capture init done");
        }
    }

    usbaudio_player_oper_init();
    usbaudio_thread_tid = osThreadCreate(osThread(usbaudio_player_thread), NULL);

    if (!g_usb_cap_semaphore) {
        g_usb_cap_semaphore = osSemaphoreCreate(osSemaphore(usb_cap_semaphore), 0);
    }
    osThreadCreate(osThread(usb_capture_thread), NULL);
    TRACE(0,"UsbAudioDevInit start");
    wifiusb_audio_app_init(submit_size, xtal_time, (USB_AUDIO_CALLBACK)usb_audio_input);
    wifiusb_audio_open();
}

void UsbAudioHidPause(void)
{
    usb_audio_hid_set_event(USB_AUDIO_HID_PLAY_PAUSE,1);
    osDelay(10);
    usb_audio_hid_set_event(USB_AUDIO_HID_PLAY_PAUSE,0);
}

extern void uaud_dev_set_mic_num(uint8_t num);
extern void usb_audio_app_set_mic_num(uint8_t num);

void UsbaudioSetMicMumNormalMode(uint8_t num)
{
    if (g_usbaudio_testmode)
        return;
    if (num <= 8) {
        uaud_dev_set_mic_num(num);
        usb_audio_app_set_mic_num(num);
        _USB_MUTI_MIC = num;
        CAPTURE_FRAM_SIZE = _USB_MUTI_MIC*SAMPLE_SIZE_SEND*(SAMPLE_RATE_SEND/1000);
        g_wait_capture = 1;
        TRACE(2, "chnanel num = %d, cap_to_usb_fram=%d, g_wait_captrure=%d", _USB_MUTI_MIC, CAPTURE_FRAM_SIZE, g_wait_capture);
    } else {
        TRACE(2, "ERROR: mic number is big than 8");
    }
}

void UsbaudioSetMicMumTestMode(uint8_t num)
{
    if (!g_usbaudio_testmode)
        return;
    if (num <= 8) {
        uaud_dev_set_mic_num(num);
        usb_audio_app_set_mic_num(num);
        _USB_MUTI_MIC = num;
        CAPTURE_FRAM_SIZE = _USB_MUTI_MIC*SAMPLE_SIZE_SEND*(SAMPLE_RATE_SEND/1000);
        g_wait_capture = 1;
        TRACE(2, "chnanel num = %d, cap_to_usb_fram=%d, g_wait_captrure=%d", _USB_MUTI_MIC, CAPTURE_FRAM_SIZE, g_wait_capture);
    } else {
        TRACE(2, "ERROR: mic number is big than 8");
    }
}

void UsbaudioSetTestMode(void)
{
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_ROM_RESERVED_17);
}

int UsbaudioGetMode(void)
{
    g_usbaudio_testmode = ((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_ROM_RESERVED_17) >> 17);
    if (g_usbaudio_testmode) {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_ROM_RESERVED_17);
    }
    return g_usbaudio_testmode;
}

#endif