
#include <arpa/inet.h>
#include "cmsis_os.h"
#include "console.h"
#include "at.h"
#include "ping.h"
#include "iperf.h"
#include <netdb.h>
#include "bwifi_interface.h"
#ifdef __SNIFFER_MODE__
#include "bes_sniffer.h"
#endif
#ifdef BES_AIRKISS_SUPPORT
#include "bes_airkiss.h"
#endif

#ifdef __WIFI_NVR_SUPPORT__
#include "nvrecord_wifi.h"
#endif
#include "hexdump.h"
#include "net_memory.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "hal_norflash.h"
#include "cmsis_os.h"
#ifdef osCMSIS_FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#endif
#if BES_OTA_ENABLED
#include "ota_handler.h"
#include "crc32.h"
#include "hal_norflash.h"
#endif
#include "net_misc.h"
#if (defined(__A7_DSP_ENABLE__) || defined(MCU_DSP) || defined(CP_DSP))
#include "mcu_audio.h"
#include "data_dump.h"
#endif
#ifdef __A7_DSP_ENABLE__
#include "a7_cmd.h"
#endif
#if defined(__NET_FTP_CLIENT_SUPPORT__)
#include "ftp_client.h"
#endif

#include "hal_timer.h"
#include "hal_phyif.h"

#ifdef IR_FUNCTION_TEST
#include "app_ir_test.h"
#endif

#include"hal_bootmode.h"
#include "factory_section.h"
#include "wifi_socket.h"
#include "nvrecord_env.h"

#ifdef CHIP_BEST2001
#include "hal_cmu_best2001.h"
#include "reg_cmu_best2001.h"
#endif
#ifdef CHIP_BEST2003
#include "hal_cmu_best2003.h"
#include "reg_cmu_best2003.h"
#endif

#ifdef IBRT
#include "app_ibrt_if.h"
#endif

#if RF_EFUSE_TEST
#include "factory_efuse.h"
#endif



#ifdef DEBUG_SPP_LOG
#include "at_spp.h"
#endif

#ifdef CMSIS_API_TEST
#include "cmsis_test.h"
#endif

#include "usb_audio.h"
#ifdef __NET_TELNET_SERVER_SUPPORT__
#include "telnetserver.h"
#endif

#include "wifi_drv.h"

#ifdef CHIP_HAS_RGB_LED
#include "app_led.h"
#endif

static void _confwifi_timer_handler(void const *param);
osTimerDef(CONFWIFI_TIMER, _confwifi_timer_handler);

static osTimerId confwifi_timer = NULL;

extern void httpd_init(void);
/*
	1. AT+CONFWIFI=abc
	2. mobile connect abc softap
	3. input 192.168.0.1/pw.html
	4. input ssid and password in pw.html
	5. submit
*/
static void _confwifi_timer_handler(void const *param)
{
    extern struct bwifi_station_config *nv_record_get_ap_info(void);

    struct bwifi_station_config * p = nv_record_get_ap_info();
	if (p) {
		//这里需要起一个tread来处理比较好，在这个上下文里太久时间会引起异常
		osTimerStop(confwifi_timer);
		confwifi_timer = NULL;
	
		bwifi_softap_stop();
		printf("softap stop\r\n");
		osDelay(2000);
		printf("sta start and connect ap...%s, %s\r\n", p->ssid, p->passwd);
		//bwifi_connect(p);
        bwifi_connect_to_ssid((const char *)p->ssid, (const char *)p->passwd, 0, 0, NULL);
	}
}

static int do_conf_wifi(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	char *ssid = NULL;
	int channel = 0;
	int sec_channel_offset = 0;
	int hidden = 0;
	BWIFI_SEC_TYPE_T sec = BWIFI_SECURITY_NONE;
	char *passwd = NULL;

	if (argc > 1 && strcmp("", argv[1])) /* ssid is not "" */
        ssid = argv[1];
	
	if (bwifi_set_softap_config(ssid, (u8)channel, sec_channel_offset, (u8)hidden, sec, passwd)) {
        RESP_ERROR(handler, ERROR_ARG);
		return 0;
	}

    if (bwifi_softap_start()) {
        RESP_ERROR(handler, ERROR_FAILE);
		return 0;
	}
	
	httpd_init();

	if (NULL == confwifi_timer)
    {
        confwifi_timer = osTimerCreate(osTimer(CONFWIFI_TIMER), osTimerPeriodic, NULL);
    }
	osTimerStart(confwifi_timer, 1000);
	RESP_OK(handler);
	return 0;
}

/*
Lit Red, brightness
Lit Green, brightness
Lit Blue, brightness
Lit White, brightness 
Turn OFF

AT+LEDCTRL=red,2
AT+LEDCTRL=all,0
AT+LEDCTRL=off
brightness is [0,100]
*/
#define RED_GPIO HAL_IOMUX_PIN_P3_0

static void _set_GPIO_output(u8 index, u8 level)
{
	struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio;

	cfg_hw_gpio.pin = index;
    cfg_hw_gpio.function = HAL_IOMUX_FUNC_AS_GPIO;
    cfg_hw_gpio.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
    cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_NOPULL;
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_hw_gpio, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_gpio.pin, HAL_GPIO_DIR_OUT, level);
}



/*
Volume updown response test
Mic mute button response test

AT+KEYSTEST
*/
#define VOLUMEUP_KEY HAL_IOMUX_PIN_P3_1

static void key_io_irq_handler(enum HAL_GPIO_PIN_T pin)
{
    printf("key%d pressed!\n", pin);
}

static int do_keys_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int edge = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    u8 i;
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_io;

    for (i = 0; i < CFG_HW_GPIOKEY_NUM; i++) {
        if (cfg_hw_gpio_key_cfg[i].key_code == HAL_KEY_CODE_NONE) {
            continue;
        }
        hal_iomux_init(&cfg_hw_gpio_key_cfg[i].key_config, 1);

        hal_gpio_pin_set_dir(cfg_hw_gpio_key_cfg[i].key_config.pin, HAL_GPIO_DIR_IN, 0);    //设置gpio输入

        struct HAL_GPIO_IRQ_CFG_T irq_cfg;
	    irq_cfg.irq_debounce = 1;
	    irq_cfg.irq_enable = 1;
	    irq_cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
	    irq_cfg.irq_handler = key_io_irq_handler;
	    irq_cfg.irq_polarity = edge;
	    hal_gpio_setup_irq(cfg_hw_gpio_key_cfg[i].key_config.pin, &irq_cfg);
    
    }
	RESP_OK(handler);
    return 0;
}


/*
  Push audio wav file
  
  embedded a wav file in fold /data/
*/

/*
  Play wav file,        Enter [ Ctrl + C ] to aborted it.

  1. AT+WSCONN
  2. AT+LPLAYER_START
  3. AT+LPLAYER_STOP
*/

/*  
  Record feedback channel (default max volume record) (play and record at the same time, 10S)
  Spk to mic 边放音乐边录音，再录音文件拿出来

  1. AT+WSCONN
  2. AT+LPLAYER_START
  3. AT+RECDUMP
*/

/*
  Record 5 seconds, then play it.

  1. AT+WSCONN
  2. AT+RECDUMP
  3. AT+LPLAYER_START
*/

/*
  Mic to Speaker directly, Enter [ Ctrl + C ] to aborted it.

  AT+AUDIOLOOP=start
  AT+AUDIOLOOP=stop
*/

/*
  Get recorded wav file
  1. AT+WSCONN
  2. AT+RECDUMP
*/

/*
  Set output volume
  
  AT+LPLAYER_SET_VOL for lplayer 
  or
  AT+VOLUP and AT+VOLDOWN for bt
*/  

/*
  Audio output mute

  AT+MUTE
*/  
#ifdef LPLAYER_SUPPORT_AT_CMD
#ifdef BT_MEDIA_SUPPORT_LPLAYER
#include "aud_player.h"
#else
#include "lplayer.h"
#endif
#endif
#include "app_bt_stream.h"

static int do_mute_ctrl(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    app_bt_stream_volumeset(0);
    aud_player_set_vol(0);
    RESP_OK(handler);
    return 0;
}

/*
  Bypass EQ
  Bypass DRC
  
  AT+BPED=1,1,1 //first 1 is eq, second 1 is drc, third 1 is limiter
*/
extern void audio_process_hook(bool eq, bool drc, bool lmt);
static int do_bypass_eqdrc(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    u8 bpeq, bpdrc, bplmt;
    bpeq = atoi(argv[1]);
    bpdrc = atoi(argv[2]);
    bplmt = atoi(argv[3]);

    audio_process_hook(!bpeq, !bpdrc, !bplmt);
    RESP_OK(handler);
    return 0;
}

#include "audioflinger.h"
#include <string.h>
#include "hal_trace.h"
#include "hal_location.h"
#include "cqueue.h"
#include "heap_api.h"

#define AUDIO_USE_DEVICE_I2S    0
#define AUDIO_STREAM_CH 	AUD_STREAM_ID_1
#define AUDIO_SAMPLE_RATE AUD_SAMPRATE_48000
#define MIDDLE_BUFFER_SIZE (48 *2 *2 *4 *8)
#define CAPTURE_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)
#define PLAYBACK_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 2)
#define AUDIO_OUT_QUEUE_BUFFER_SIZE (MIDDLE_BUFFER_SIZE * 4)

static uint16_t capture_auido_trace_count = 0;
static uint16_t playback_auido_trace_count = 0;
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
    
	uint8_t *capture_buffer;//[CAPTURE_BUFFER_SIZE];
	uint8_t *playback_buffer;//[PLAYBACK_BUFFER_SIZE];
	uint8_t *audio_output_queue_buf;//[AUDIO_OUT_QUEUE_BUFFER_SIZE];

    TRACE(0,"%s, sample_rate = %d", __func__, AUDIO_SAMPLE_RATE);
	
	syspool_init();
	syspool_get_buff(&capture_buffer, CAPTURE_BUFFER_SIZE);
	syspool_get_buff(&playback_buffer, PLAYBACK_BUFFER_SIZE);
	//app_audio_mempool_get_buff((uint8_t **)&app_audioloop_play_cache, BT_AUDIO_FACTORMODE_BUFF_SIZE*2/2/2);
	syspool_get_buff(&audio_output_queue_buf, AUDIO_OUT_QUEUE_BUFFER_SIZE);
	
	af_stream_stop(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    af_stream_close(AUD_STREAM_ID_0, AUD_STREAM_CAPTURE);
    
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
            stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)(AUD_CHANNEL_MAP_DIGMIC_CH0 | AUD_CHANNEL_MAP_DIGMIC_CH1 | AUD_CHANNEL_MAP_CH0);
            stream_cfg.channel_num = AUD_CHANNEL_NUM_3;
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


static int do_audioloop_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	if (argc != 2) {
		RESP_ERROR(handler, ERROR_ARG);
		return 0;
	}

	
	if (strcasecmp(argv[1], "start") == 0) {
		audio_loop_test(2);
	} else if (strcasecmp(argv[1], "stop") == 0) {
		af_stream_stop(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE);
    	af_stream_close(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK);
    	af_stream_stop(AUDIO_STREAM_CH, AUD_STREAM_CAPTURE);
    	af_stream_close(AUDIO_STREAM_CH, AUD_STREAM_PLAYBACK);
	} else {
		RESP_ERROR(handler, ERROR_ARG);
		return 0;
	} 
	
	RESP_OK(handler);
	return 0;

}

/*
  2 Mic recording 
  Get mic record file (2 channel, 1 mic per channel)

  1. AT+WSCONN
  2. AT+RECDUMP
*/

/*
  Button press for BT pairing, then output BT audio

  1. AT+BTINIT=1
  2. AT+BTSCAN=1
*/

/*
  Provide a method for WiFi connect

  1. AT+CONFWIFI
  2. Mobile connect to device, open url:192.168.0.1/pw.html
  3. input ssid hand password, then submit
  4. device will connect to the ssid
*/

/*
Wifi fix channel TX
Wifi sensitivity test

BT DUT mode for signaling test
BT fix channel TX
BT sensitivity test

refer to the pdf
*/

/*
  Wifi throughput test through iperf
  
  AT+NIPERF
*/

/*
  Run/load SoC system at usual working power for thermal test, able to play audio through BT or command.
*/

/*
  
"This could be another image file which contains only these features.
1. Keep blinking LED in Green, around 2 blinks per second.
2.  Long press 2 seconds O Button for BT pairing. Blink LED in Blue when BT connected. Play audio from BT.
3. Press +/- to adjust audio output volume. Max=0dBFS. 
4. Short press center button to record from 2 mic, make a ""Beep"" alert when start record. Press again or after 6 minutes, stop and save as mic_record.wav. ""Beep Beep"" as stop alert. Recording should be able to run at the same time when BT audio playing. 
5. Press top button ~ to play the recording in 4.
6. Provide a method to connect to WiFi, Keep blinking in Green for sucess connection. Blink LED in RED if lost WiFi connection. Reconnect to resume to Green blinking LED. 
7. All above need to implemented at the same time. "
*/
static void _emcesd_timer_handler(void const *param);
osTimerDef(EMCESD_TIMER, _emcesd_timer_handler);
static osTimerId emcesd_timer = NULL;

static void _emcesd_timer_handler(void const *param)
{
	//case 1
	_set_GPIO_output(RED_GPIO, 1);
	osDelay(100);
	_set_GPIO_output(RED_GPIO, 0);
}

static int do_emcesd_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	if (NULL == emcesd_timer)
    {
        emcesd_timer = osTimerCreate(osTimer(EMCESD_TIMER), osTimerPeriodic, NULL);
    }
	osTimerStart(emcesd_timer, 1000);
    return 0;
}

/*
  read HW version
  
  AT+HWVER
*/
static int32_t hwver_val;
static int32_t hwver_volt;

volatile bool hwver_read_done = false;
#define ADC_DELTA 20
extern int32_t pmu_temperature_adc2temp(uint16_t gpadcVal);
static void app_hwver_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    hwver_val = irq_val;
	hwver_volt = volt;
    hwver_read_done = true;
}

static int do_read_hwver(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	int ret;
	uint32_t cnt = 0;
	hwver_read_done = false;
	char hwver[4] = {0};
	const uint32_t hwver_map[6] = {0, 85, 167, 248, 334, 420};

retry:
	ret = hal_gpadc_open(HAL_GPADC_CHAN_2, HAL_GPADC_ATP_ONESHOT, app_hwver_irqhandler);
	if (ret) {
		osDelay(1);
		if (++cnt > 10) { // timeout, return 25 degrees Celsius
			TRACE(0, "error, read temperature timeout0");
			return 25;
		}
		goto retry;
	}

	cnt = 0;
	while(!hwver_read_done) {
		if (++cnt > 10) { // timeout, return 25 degrees Celsius
			TRACE(0, "error, read temperature timeout1");
			return 25;
		}
		osDelay(1);
	}
	hal_gpadc_close(HAL_GPADC_CHAN_0);
	for (uint32_t i=0; i<sizeof(hwver_map)/sizeof(uint32_t)-1; i++) {
		if (hwver_val>(hwver_map[i]+ADC_DELTA) && hwver_val<(hwver_map[i+1]-ADC_DELTA)) {
			sprintf(hwver, "V%d", i+1);
			break;
		}
	}	
	
	RESP_OK_EQU(handler, "HW version is %s, ADC value is %d, volt is %d\n", hwver, hwver_val, hwver_volt);
	return 0;
}


/*
  read SN 
  
  AT+RDSN
  notice: we use the flash id as the unique SN.
*/
#define SN_MAKEU32(a,b,c,d) (((uint32_t)((a) & 0xff) << 24) | \
                               ((uint32_t)((b) & 0xff) << 16) | \
                               ((uint32_t)((c) & 0xff) << 8)  | \
                                (uint32_t)((d) & 0xff))

static int do_read_sn(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	uint8_t unique_id8[HAL_NORFLASH_UNIQUE_ID_LEN + HAL_NORFLASH_CP_ID_LEN] = {0};
	uint32_t unique_id32[HAL_NORFLASH_UNIQUE_ID_LEN/4] = {0};
	
	enum HAL_NORFLASH_RET_T ret = hal_norflash_get_unique_id(HAL_FLASH_ID_0, unique_id8, HAL_NORFLASH_UNIQUE_ID_LEN + HAL_NORFLASH_CP_ID_LEN);
	if (ret == HAL_NORFLASH_OK) {
		for (uint8_t i=0; i<HAL_NORFLASH_UNIQUE_ID_LEN/4; i++) {
			unique_id32[i] = SN_MAKEU32(unique_id8[i*4], unique_id8[i*4+1], unique_id8[i*4+2], unique_id8[i*4+3]);
		}
		RESP_OK_EQU(handler, "SN is %x-%x-%x-%x\n", unique_id32[0], unique_id32[1], unique_id32[2], unique_id32[3]);
	} else {
		RESP_ERROR(handler, ret);
	}
	return 0;
}


#if defined(CHIP_HAS_EXT_NORFLASH) && defined(TEST_FOR_EXT_NORFLASH)
#include "hal_norflash.h"
#include "hal_location.h"
#include "hal_cache.h"

uint8_t extflash_testing = false;

extern uint32_t __flash2_code_start[];
extern uint32_t __flash2_code_end[];
extern uint32_t __flash2_code_start_flash[];
extern uint32_t __flash2_code_end_flash[];
extern void demo_func_in_flash2(void);

void demo_func_in_flash1(void){
    TRACE(1, "%s: HEY, I am running in FLASH1", __func__);
}

SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T demo_func_cp_flash2(void){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint8_t buf[100], buf2[100];
    uint32_t size =  (uint32_t)__flash2_code_end - (uint32_t)__flash2_code_start;
    TRACE(0, "%s: code size=%d(%X ~ %X)", __func__, size, (uint32_t)__flash2_code_start, (uint32_t)__flash2_code_end);
    TRACE(0, "%s: flash size=%d(%X ~ %X)", __func__, size, (uint32_t)__flash2_code_start_flash, (uint32_t)__flash2_code_end_flash);
    TRACE(0, "FLASH1_BASE=%X, flash2_demo_func=%X", FLASH1_BASE, (uint32_t)demo_func_in_flash2);

    uint32_t total_size, block_size, sector_size, page_size;

    hal_norflash_get_size(HAL_FLASH_ID_1, &total_size,
                         &block_size, &sector_size, &page_size);

    TRACE(4,"flash info: total: %d KB, block: %d KB, sector: %d KB, page: %d B",
            total_size/1024, block_size/1024, sector_size/1024, page_size);


    memcpy(buf, __flash2_code_start_flash, size);
    TRACE(0, "%s: __flash2_code_start_flash(%X)-->buf %X dump", __func__, (uint32_t)__flash2_code_start_flash, (uint32_t)buf);
    for(int i = 0; i < size; i+=16){
        DUMP8("%02X ", &buf[i], 16);
    }
    TRACE(0, "-------------------------------------");

    uint32_t dst = FLASH1_NC_BASE; //FLASH1_BASE
    uint8_t *p;
    memset(buf2, 0, sizeof(buf2));

    int retry = 0;
    p=(uint8_t*)dst;
    TRACE(2, "%s: before hal_norflash_erase", __func__);
    if(ret != 0)return ret;
    ret = hal_norflash_read(HAL_FLASH_ID_1, dst, buf2, size);
    for(int i = 0; i < size; i+=16){
        DUMP8("%02X ", &buf2[i], 16);
    }
    TRACE(0, "-------------------------------------");

    //hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    //hal_cache_disable(HAL_CACHE_ID_D_CACHE);
    ret = hal_norflash_erase(HAL_FLASH_ID_1, dst, size);
    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    hal_cache_invalidate_all(HAL_CACHE_ID_D_CACHE);
    TRACE(2, "%s: hal_norflash_erase %X return: %d", __func__, dst, ret);
    if(ret != 0)return ret;
    ret = hal_norflash_read(HAL_FLASH_ID_1, dst, buf2, size);
    for(int i = 0; i < size; i+=16){
        DUMP8("%02X ", &buf2[i], 16);
    }
    TRACE(0, "-------------------------------------");
    retry = 0;
    while(buf2[0] != 0xFF && ret == 0){
        // ret = hal_norflash_erase(HAL_FLASH_ID_1, dst, size);
        // hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
        // hal_cache_invalidate_all(HAL_CACHE_ID_D_CACHE);
        ret = hal_norflash_read(HAL_FLASH_ID_1, dst, buf2, size);
        retry++;
    }
    TRACE(0, "erase done：p[0]==0xFF, retry read=%d", retry);
    if(retry){
        for(int i = 0; i < size; i+=16){
            DUMP8("%02X ", &buf2[i], 16);
        }    
    }
    ASSERT(ret == 0, "hal_norflash_erase return: %d", ret);

    ret = hal_norflash_write(HAL_FLASH_ID_1, (uint32_t)dst, (const uint8_t*)buf, size);
    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    hal_cache_invalidate_all(HAL_CACHE_ID_D_CACHE);
    TRACE(2, "%s: hal_norflash_write return: %d", __func__, ret);
    ret = hal_norflash_read(HAL_FLASH_ID_1, dst, buf2, size);
    for(int i = 0; i < size; i+=16){
        DUMP8("%02X ", &buf2[i], 16);
    }
    TRACE(0, "-------------------------------------");
    retry=0;
    while((buf2[0] == 0xFF || buf2[0] == 0) && ret == 0){
        //ret = hal_norflash_write(HAL_FLASH_ID_1, (uint32_t)dst, (const uint8_t*)buf, size);
        ret = hal_norflash_read(HAL_FLASH_ID_1, dst, buf2, size);
        retry++;
    }    
    TRACE(0, "write done：p[0]!=0xFF, retry read=%d", retry);
    if(retry){
        for(int i = 0; i < size; i+=16){
            DUMP8("%02X ", &buf2[i], 16);
        }    
    }
    ASSERT(ret == 0, "hal_norflash_write return: %d", ret);

    /*
    while(buf2[0] != 0x08){
        hal_norflash_read(HAL_FLASH_ID_1, dst, buf2, size);
    }*/
    for(int i = 0; i < size; i++){
        //TRACE(3, "%s: buf[%d]: %X", __func__, i, buf[i]);
        ASSERT(buf[i] == buf2[i], "[%d] mismatched: %X, %X", i, buf[i], buf2[i]);
    }

    return ret;
}

// #include "console.h"
SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T __norflash_test_cmd(int id, uint32_t address, const char *cmd){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint8_t buf[16];
    uint8_t buf2[16];
    uint32_t size = 16;
    for(int i = 0; i < size; i++){
        buf[i] = 0x10+i+1;
    }
    memset(buf2, 0, sizeof(buf2));

    uint32_t dst = address; //FLASH1_BASE

    if(strcmp(cmd, "erase") == 0){
        ret = hal_norflash_erase(id, dst, size);
        TRACE(2, "%s: hal_norflash_erase %X return: %d", __func__, dst, ret);
    }else if(strcmp(cmd, "read") == 0){
        ret = hal_norflash_read(id, dst, buf2, size);
        TRACE(2, "%s: hal_norflash_read %X return: %d", __func__, dst, ret);
        for(int i = 0; i < size; i+=16){
            DUMP8("%02X ", &buf2[i], 16);
        }
        uint8_t *p = (uint8_t*)dst;
        for(int i = 0; i < size; i+=16){
            DUMP8("%02X ", &p[i], 16);
        }
    }else if(strcmp(cmd, "write") == 0){
        ret = hal_norflash_write(id, (uint32_t)dst, (const uint8_t*)buf, size);
        TRACE(2, "%s: hal_norflash_write %X return: %d", __func__, dst, ret);
    }else if(strcmp(cmd, "xip") == 0){
        //启动测试
        demo_func_cp_flash2(); // load code to ext flash
        demo_func_in_flash2(); // xip on ext flash
    }else{
        TRACE(0, "ERROR command: %s", cmd);
    }

    return ret;
}

int cmd_norflash2_test_func(cmd_tbl_t * cmdtp, int argc, char *argv[], void *handler){
    TRACE(0, "argc=%d", argc);
    for(int i = 0; i < argc; i++){
        TRACE(0, "       [%d]: %s", i, argv[i]);
    }
    if(argc != 2){
        TRACE(0, "Usage: flash2 erase|read|write|xip");
        return -1;
    }
    return __norflash_test_cmd(HAL_FLASH_ID_1, FLASH1_NC_BASE, argv[1]);
}

#endif

void add_factory_cmd()
{
    int i, j;
    cmd_tbl_t cmd_list[] = {
        /*Basic CMD*/
        {
             "AT+CONFWIFI",          4,   do_conf_wifi,
             "AT+CONFWIFI                - do wifi test\n"
        },
        {
             "AT+KEYSTEST",          4,   do_keys_test,
             "AT+KEYSTEST                - do keys test\n"
        },
        {
             "AT+AUDIOLOOP",          4,   do_audioloop_test,
             "AT+AUDIOLOOP                - do audio loop test\n"
        },
        {
             "AT+MUTE",          4,   do_mute_ctrl,
             "AT+MUTE                - do mute test\n"
        },
        {
             "AT+BPED",          4,   do_bypass_eqdrc,
             "AT+BPED                - bypass eqdrc\n"
        },
        {
             "AT+EMCESD",          4,   do_emcesd_test,
             "AT+EMCESD                - do emcesd test\n"
        },
        {
             "AT+HWVER",          4,   do_read_hwver,
             "AT+HWVER                - read hardware version\n"
        },
        {
             "AT+RDSN",          4,   do_read_sn,
            "AT+RDSN                - read SN\n"
        },
#if defined(CHIP_HAS_EXT_NORFLASH) && defined(TEST_FOR_EXT_NORFLASH)
        {
            "AT+XIPFLASH2",
            2,
            cmd_norflash2_test_func,
            "AT+XIPFLASH2           - AT+XIPFLASH2=erase|read|write|xip   test external nor flash\n"
        },
#endif
    };
	
    j = sizeof(cmd_list) / sizeof(cmd_tbl_t);
    for (i = 0; i < j; i ++) {
        if (0 != console_cmd_add(&cmd_list[i])) {
            AT_PRINT("Add cmd failed\r\n");
        }
    }
}


