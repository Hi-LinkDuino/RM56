
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
//#include "hexdump.h"
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
// #include "wifi_socket.h"
#include "nvrecord_env.h"

#ifdef CHIP_BEST2001
#include "hal_cmu_best2001.h"
#include "reg_cmu_best2001.h"
#endif
#ifdef CHIP_BEST2003
// #include "hal_cmu_best2003.h"
// #include "reg_cmu_best2003.h"
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

#include "hal_key.h"

#include "besfd_api.h"
#include "tgt_hardware.h"


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

//AT+WIFI[=?|0|1]
#include "bwifi_interface.h"
extern struct bwifi_station_config *nv_record_get_ap_info(void);
void nv_record_set_ap_info(struct bwifi_station_config *p);
void nv_record_set_ap_ssid_passwd(char *ssid, char *passwd);
//wifi ap mode: local settings
bool nv_record_set_softap_ssid_passwd(char *ssid, char *passwd);
void nv_record_get_softap_ssid_passwd(char **ssid, char **passwd);

bool nv_record_get_ftp_config(char **server, char **port, char **user, char **passwd);
void nv_record_set_ftp_config(char *server, char *port, char *user, char *passwd);

static int do_conf_ftp(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    bool help = false;
    if(argc == 1 || argv[1][0] == '?'){
        help = true;
    }
    char *server = (argc > 1)?argv[1]:NULL;
    char *port = (argc > 2)?argv[2]:(char*)"21";
    char *user = (argc > 3)?argv[3]:NULL;
    char *passwd = (argc > 4)?argv[4]:NULL;

    if(!help && server && *server){
        nv_record_set_ftp_config(server, port, user, passwd);
    }
    if (nv_record_get_ftp_config(&server, &port, &user, &passwd))
    {
        RESP(handler, "FTP settings: \n");
        RESP(handler, "    server=%s\n", server);
        RESP(handler, "    port=%s\n", port);
        RESP(handler, "    user=%s\n", user);
        RESP(handler, "    passwd=%s\n", passwd);
    }
    else
    {
        RESP(handler, "FTP settings: None\n");
    }
    RESP_OK(handler);
    return 0;
}

/**
 * 
 * If NO ftp server provided, just use FTP settings stored in NV
 */

static int do_ftp_put(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if(argc < 2)return -1;
    char *fname = argv[1];
    char *server = (argc > 2)?argv[2]:NULL;
    char *port = (argc > 3)?argv[3]:(char*)"21";
    char *user = (argc > 4)?argv[4]:NULL;
    char *passwd = (argc > 5)?argv[5]:NULL;

    if(!wifi_sta_connected()){
        cmd_printf(handler, "WIFI NOT connected \n");
        return -1;
    }
    FTP_HANDLE ftp_handler;
    if(server && *server){
        ftp_handler = ftp_connect_with(server, port, user, passwd);
        if(!ftp_handler){
            cmd_printf(handler, "ftp params error \n");
            return 0;
        }
    }else{
        ftp_handler = ftp_connect();
        if(!ftp_handler){
            cmd_printf(handler, "FTP settings not provided in AT and NV\n");
            return 0;
        }
    }
    ftp_put_file(ftp_handler, fname);
    ftp_close(ftp_handler);
    return 0;
}

static int do_conf_wifi2(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    bool isap = wifi_in_ap_mode();
    if(argc == 1 || argv[1][0] == '?'){
        RESP(handler, "WIFI is in %s mode\n", isap?"AP":"STA");
        struct bwifi_station_config *ap = nv_record_get_ap_info();
        char *ssid, *passwd;
        nv_record_get_softap_ssid_passwd(&ssid, &passwd);
        if(*ssid){
            RESP(handler, "WIFI AP settings: \n");
            RESP(handler, "    ssid=%s\n", ssid);
            RESP(handler, "    passwd=%s\n", passwd);
        }else{
            RESP(handler, "WIFI AP settings: None\n");
        }

        if(ap == NULL){
            RESP(handler, "WIFI STA settings: None\n");
        }else{
            RESP(handler, "WIFI STA settings: \n");
            RESP(handler, "    ssid=%s\n", ap->ssid);
            RESP(handler, "    passwd=%s\n", ap->passwd);
        }
        char *server, *port, *user;
        if(nv_record_get_ftp_config(&server, &port, &user, &passwd)){
            RESP(handler, "FTP settings: \n");
            RESP(handler, "    server=%s\n", server);
            RESP(handler, "    port=%s\n", port);
            RESP(handler, "    user=%s\n", user);
            RESP(handler, "    passwd=%s\n", passwd);
        }else{
            RESP(handler, "FTP settings: None\n");
        }
    	RESP_OK(handler);
        return 0;
    }

    char *ssid = (argc > 2)?argv[2]:NULL;
    char *passwd = (argc > 3)?argv[3]:NULL;

    if(strcmp(argv[1], "ap") == 0){
    	RESP_OK(handler);
        if(ssid && *ssid && nv_record_set_softap_ssid_passwd(ssid, passwd)){
            //softap ssid or passwd changed
            if(isap){
                wifi_switch_mode("sta");
            }
        }
        wifi_switch_mode("ap");
    }else if(strcmp(argv[1], "sta") == 0){
    	RESP_OK(handler);
        if(ssid && *ssid){
            if(!bwifi_connect_to_ssid(ssid, passwd, 0, 0, NULL)){
                nv_record_set_ap_ssid_passwd(ssid, passwd);
            }
        }else{
            wifi_switch_mode(argv[1]);
        }
    }else if(strcmp(argv[1], "toggle") == 0){
    	RESP_OK(handler);
        wifi_switch_mode(argv[1]);
    } else {
        RESP(handler, "Usage: AT+WIFI=?|sta|ap|toggle [,ssid] [,passwd]\n");
        RESP_ERROR(handler, ERROR_ARG);
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

#define FLASH2_TEST_BYTE    0xAB
#define FLASH2_TEST_BUFSZ   4096
#define FLASH2_CHECK_DURATION   1000 //1s
uint8_t flash2_test_buf[FLASH2_TEST_BUFSZ];

void demo_func_in_flash1(void){
    TRACE(1, "%s: HEY, I am running in FLASH1", __func__);
}

static SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T norflash_erase_and_check(int id, uint32_t addr, uint32_t size, bool trace){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint32_t dst = addr;
    if(trace)TRACE(0, "%s(0x%X) %d", __func__, dst, size);
    ret = hal_norflash_erase(id, dst, size);
    if(ret != 0){
        TRACE(0, "%s failed, hal_norflash_erase %X return: %d", __func__, dst, ret);
        return ret;
    }
    // if(id == HAL_FLASH_ID_1){
    //     TRACE(0, "%s(0x%X) %d.........1", __func__, dst, size);
    //     hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    //     hal_cache_invalidate_all(HAL_CACHE_ID_D_CACHE);
    //     TRACE(0, "%s(0x%X) %d.........2", __func__, dst, size);
    // }

    uint32_t tick = hal_sys_timer_get();
#if 1
    uint32_t sector_size;
    hal_norflash_get_size(id, NULL, NULL, &sector_size, NULL);
    int remain_len = size;
    int read_len = 0;
    uint8_t *buf2read = pvPortMalloc(sector_size);
    memset(buf2read, 0, sector_size);

    if(trace)TRACE(0, "%s sector_size=%d", __func__, sector_size);
    for (int j = 0; remain_len > 0; j++) {
        read_len = sector_size - (dst & (sector_size - 1));
        if (read_len > remain_len) {
            read_len = remain_len;
        }
        ret = hal_norflash_read(id, dst, buf2read, read_len);
        if(ret){
            TRACE(0, "%s(0x%X) failed, hal_norflash_read return: %d", __func__, dst, ret);
            goto out;
        }
        while(buf2read[0] != 0xff){
            if(TICKS_TO_MS(hal_sys_timer_get()-tick) > FLASH2_CHECK_DURATION){
                TRACE(0, "%s(0x%X) failed, invalid data after %d seconds", __func__, dst, FLASH2_CHECK_DURATION/1000);
                ret = -1;
                goto out;
            }
            // osDelay(1); 
            hal_sys_timer_delay_us(200);
            ret = hal_norflash_read(id, dst, buf2read, read_len);
            if(ret){
                TRACE(0, "%s(0x%X) failed, hal_norflash_read return: %d", __func__, dst, ret);
                goto out;
            }
        }
        if(trace)TRACE(0, "%s(0x%X) sector %d ready(%d ticks=%d ms)", __func__, dst, j, hal_sys_timer_get()-tick, TICKS_TO_MS(hal_sys_timer_get()-tick));
        for(int i = 0; i < read_len; i++){
            if(buf2read[i] != 0xff){
                TRACE(0, "%s(0x%X) failed, [%d]: 0x%X != 0xFF", __func__, dst, i, buf2read[i]);
                ret = -1;
                goto out;
            }
        }
        remain_len -= read_len;
        dst += read_len;
    }
#else
    uint8_t *buf2read = (uint8_t *)dst;
    while(buf2read[0] != 0xff){
        if(TICKS_TO_MS(hal_sys_timer_get())-tick > FLASH2_CHECK_DURATION){
            TRACE(0, "%s failed, invalid data after %d seconds", __func__, FLASH2_CHECK_DURATION/1000);
            return -1;
        }
        osDelay(1);
    }
    for(int i = 0; i < size; i++){
        if(buf2read[i] != 0xFF){
            TRACE(0, "%s failed, [%d]: 0x%X != 0xFF", __func__, i, buf2read[i]);
            return -1;
        }
    }
#endif
out:
    vPortFree( buf2read );
    if(ret == 0 && trace){
        TRACE(0, "%s(0x%X) success(%d ms), read and checked", __func__, addr, TICKS_TO_MS(hal_sys_timer_get()-tick));
    }
    return ret;
}

static SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T norflash_page_write_and_check(int id, uint32_t address, uint8_t *buf, uint32_t size, bool check, bool trace){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint32_t dst = address;
    // TRACE(0, "%s(0x%X) buf=0x%X---->0x%X", __func__, dst, buf, PSRAM_C_TO_NC(buf));
    //buf = PSRAM_C_TO_NC(buf);
    ret = hal_norflash_write(id, dst, (const uint8_t *)buf, size);
    if(ret != 0){
        TRACE(0, "%s(0x%X) failed, hal_norflash_write return: %d", __func__, dst, ret);
        return ret;
    }
    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    hal_cache_invalidate_all(HAL_CACHE_ID_D_CACHE);
    if(!check)return ret;

    uint32_t tick = hal_sys_timer_get();

    uint8_t *buf2read = pvPortMalloc(size);
    memset(buf2read, 0, size);
    do {
        bool success = true;
        ret = hal_norflash_read(id, dst, buf2read, size);
        if (ret)
        {
            TRACE(0, "%s(0x%X) failed, hal_norflash_read return: %d", __func__, dst, ret);
            goto out;
        }
        for (int i = 0; i < size; i++)
        {
            if (buf2read[i] != buf[i])
            {
                if (TICKS_TO_MS(hal_sys_timer_get() - tick) > FLASH2_CHECK_DURATION)
                {
                    TRACE(0, "%s(0x%X) failed, [%d]: 0x%X != 0x%X, invalid data after %d seconds", __func__, dst, i, buf2read[i], buf[i], FLASH2_CHECK_DURATION / 1000);
                    ret = -1;
                    break;
                }
                // uint32_t tmp = hal_sys_timer_get();
                osDelay(1);
                // tmp = hal_sys_timer_get()-tmp;
                // TRACE(0, "    @(0x%X)[%d]: 0x%X != 0x%X, wait %d ticks=%d ms", dst,  i, buf2read[i], buf[i], tmp, TICKS_TO_MS(tmp));
                success = false;
                break;
            }
        }
        if(success)break;
    }while(1);

out:    
    vPortFree( buf2read );
    // for(int i = 0; i < size; i+=32){
    //     DUMP8("%02X ", &buf2read[i], 32);
    // }

    if(ret == 0 && trace){
        tick = hal_sys_timer_get()-tick;
        TRACE(0, "%s(0x%X) success(%d ticks=%d ms)", __func__, address, tick, TICKS_TO_MS(tick));
    }
    return ret;
}

static SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T norflash_write_and_check3(int id, uint32_t address, uint8_t *buf, uint32_t size, bool trace){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint32_t dst = address;
    uint8_t *src = buf;


    uint32_t page_size;
    hal_norflash_get_size(id, NULL, NULL, NULL, &page_size);

    //WRITE
    int remain_len = size;
    int read_len = 0;
    while (remain_len > 0) {
        read_len = page_size - (dst & (page_size - 1));
        if (read_len > remain_len) {
            read_len = remain_len;
        }
        ret = norflash_page_write_and_check(id, dst, src, read_len, false, trace);
        if(ret){
            TRACE(0, "%s(0x%X)", __func__, dst);
            return ret;
        }
        remain_len -= read_len;
        src += read_len;
        dst += read_len;
    }
    //CHECK
    dst = address;
    src = buf;

    uint32_t tick = hal_sys_timer_get();

    remain_len = size;
    read_len = 0;
    uint8_t *buf2read = pvPortMalloc(page_size);
    memset(buf2read, 0, page_size);

    for (int j = 0; remain_len > 0; j++) {
        read_len = page_size - (dst & (page_size - 1));
        if (read_len > remain_len) {
            read_len = remain_len;
        }
        do {
            bool success = true;
            ret = hal_norflash_read(id, dst, buf2read, read_len);
            if(ret){
                TRACE(0, "%s(0x%X) failed, hal_norflash_read return: %d", __func__, dst, ret);
                goto out;
            }
            for(int i = 0; i < read_len; i++){
                if(buf2read[i] != src[i]){
                    if(TICKS_TO_MS(hal_sys_timer_get()-tick) > FLASH2_CHECK_DURATION){
                        TRACE(0, "%s(0x%X) failed, [%d]: 0x%X != 0x%X, invalid data after %d seconds", __func__, dst,  i, buf2read[i], buf[i], FLASH2_CHECK_DURATION/1000);
                        ret = -1;
                        goto out;
                    }
                    // uint32_t tmp = hal_sys_timer_get();
                    osDelay(1);
                    // tmp = hal_sys_timer_get()-tmp;
                    // TRACE(0, "    @(0x%X)[%d]: 0x%X != 0x%X, wait %d ticks=%d ms", dst,  i, buf2read[i], buf[i], tmp, TICKS_TO_MS(tmp));
                    success = false;
                    break;
                }
            }
            if(success)break;
        }while(1);
        if(trace)TRACE(0, "%s(0x%X) page %d ready(%d ticks=%d ms)", __func__, dst, j, hal_sys_timer_get()-tick, TICKS_TO_MS(hal_sys_timer_get()-tick));

        remain_len -= read_len;
        src += read_len;
        dst += read_len;
    }
out:    
    vPortFree( buf2read );
    // for(int i = 0; i < size; i+=32){
    //     DUMP8("%02X ", &buf2read[i], 32);
    // }

    if(ret == 0 && trace){
        TRACE(0, "%s(0x%X) success(%d=%d ms)", __func__, address, hal_sys_timer_get()-tick, TICKS_TO_MS(hal_sys_timer_get()-tick));
    }
    return ret;
}

static SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T norflash_write_and_check2(int id, uint32_t address, uint8_t *buf, uint32_t size, bool trace){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint32_t dst = address;
    // TRACE(0, "%s(0x%X) buf=0x%X---->0x%X", __func__, dst, buf, PSRAM_C_TO_NC(buf));
    //buf = PSRAM_C_TO_NC(buf);
    uint32_t page_size;
    hal_norflash_get_size(id, NULL, NULL, NULL, &page_size);

    uint32_t tick = hal_sys_timer_get();

    int remain_len = size;
    int read_len = 0;

    while (remain_len > 0) {
        read_len = page_size - (dst & (page_size - 1));
        if (read_len > remain_len) {
            read_len = remain_len;
        }
        ret = norflash_page_write_and_check(id, dst, buf, read_len, true, trace);
        if(ret){
            // TRACE(0, "%s(0x%X) failed, hal_norflash_read return: %d", __func__, dst, ret);
            break;
        }
        // TRACE(0, "%s(0x%X) page %d ready(%d ticks=%d ms)", __func__, dst, j, hal_sys_timer_get()-tick, TICKS_TO_MS(hal_sys_timer_get()-tick));

        remain_len -= read_len;
        buf += read_len;
        dst += read_len;
    }
   
    if(ret == 0 && trace){
        TRACE(0, "%s(0x%X) success(%d=%d ms)", __func__, address, hal_sys_timer_get()-tick, TICKS_TO_MS(hal_sys_timer_get()-tick));
    }
    return ret;
}

static SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T norflash_write_and_check(int id, uint32_t address, uint8_t *buf, uint32_t size, bool trace){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint32_t dst = address;
    // TRACE(0, "%s(0x%X) buf=0x%X---->0x%X", __func__, dst, buf, PSRAM_C_TO_NC(buf));
    //buf = PSRAM_C_TO_NC(buf);
    ret = hal_norflash_write(id, dst, (const uint8_t *)buf, size);
    if(ret != 0){
        TRACE(0, "%s(0x%X) failed, hal_norflash_write return: %d", __func__, dst, ret);
        return ret;
    }
    hal_cache_invalidate_all(HAL_CACHE_ID_I_CACHE);
    hal_cache_invalidate_all(HAL_CACHE_ID_D_CACHE);

    uint32_t page_size;
    hal_norflash_get_size(id, NULL, NULL, NULL, &page_size);

    uint32_t tick = hal_sys_timer_get();
#if 1
    int remain_len = size;
    int read_len = 0;
    uint8_t *buf2read = pvPortMalloc(page_size);
    memset(buf2read, 0, page_size);

    for (int j = 0; remain_len > 0; j++) {
        read_len = page_size - (dst & (page_size - 1));
        if (read_len > remain_len) {
            read_len = remain_len;
        }
        do {
            bool success = true;
            ret = hal_norflash_read(id, dst, buf2read, read_len);
            if(ret){
                TRACE(0, "%s(0x%X) failed, hal_norflash_read return: %d", __func__, dst, ret);
                goto out;
            }
            for(int i = 0; i < read_len; i++){
                if(buf2read[i] != buf[i]){
                    if(TICKS_TO_MS(hal_sys_timer_get()-tick) > FLASH2_CHECK_DURATION){
                        TRACE(0, "%s(0x%X) failed, [%d]: 0x%X != 0x%X, invalid data after %d seconds", __func__, dst,  i, buf2read[i], buf[i], FLASH2_CHECK_DURATION/1000);
                        ret = -1;
                        goto out;
                    }
                    // uint32_t tmp = hal_sys_timer_get();
                    // osDelay(1);
                    hal_sys_timer_delay_us(100);
                    // tmp = hal_sys_timer_get()-tmp;
                    // TRACE(0, "    @(0x%X)[%d]: 0x%X != 0x%X, wait %d ticks=%d ms", dst,  i, buf2read[i], buf[i], tmp, TICKS_TO_MS(tmp));
                    success = false;
                    break;
                }
            }
            if(success)break;
        }while(1);
        if(trace)TRACE(0, "%s(0x%X) page %d ready(%d ticks=%d ms)", __func__, dst, j, hal_sys_timer_get()-tick, TICKS_TO_MS(hal_sys_timer_get()-tick));

        remain_len -= read_len;
        buf += read_len;
        dst += read_len;
    }
out:    
    vPortFree( buf2read );
    // for(int i = 0; i < size; i+=32){
    //     DUMP8("%02X ", &buf2read[i], 32);
    // }
#else
    uint8_t *p = (uint8_t*)dst;
    while(p[0] == 0xff || p[0] != buf[0]){
        if(TICKS_TO_MS(hal_sys_timer_get())-tick > FLASH2_CHECK_DURATION){
            TRACE(0, "%s failed, invalid data after %d seconds", __func__, FLASH2_CHECK_DURATION/1000);
            for(int i = 0; i < 1024; i+=32){
                DUMP8("%02X ", &p[i], 32);
            }
            return -1;
        }
        osDelay(1);
    }
    for(int i = 0; i < size; i++){
        if(buf[i] != p[i]){
            TRACE(0, "%s failed, [%d]: 0x%X != 0x%X", __func__, i, p[i], buf[i]);
            return -1;
        }
    }
#endif
    if(ret == 0 && trace){
        TRACE(0, "%s(0x%X) success(%d=%d ms)", __func__, address, hal_sys_timer_get()-tick, TICKS_TO_MS(hal_sys_timer_get()-tick));
    }
    return ret;
}

/**
 * cp codes running in flash2 from flash1 image
 * 1. erase
 * 2. write
 * 3. read and check
 */
static SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T demo_func_cp_flash2(void){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint8_t *buf = flash2_test_buf;

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
    for(int i = 0; i < size; i+=32){
        DUMP8("%02X ", &buf[i], 32);
    }
    TRACE(0, "-------------------------------------");

    uint32_t dst = FLASH1_NC_BASE; //FLASH1_BASE
    //hal_cache_disable(HAL_CACHE_ID_I_CACHE);
    //hal_cache_disable(HAL_CACHE_ID_D_CACHE);
    ret = norflash_erase_and_check(HAL_FLASH_ID_1, dst, size, true);
    if(ret != 0)return ret;

    uint8_t *p = (uint8_t*)dst;
    ret = norflash_write_and_check(HAL_FLASH_ID_1, dst, buf, size, true);
    for(int i = 0; i < size; i+=32){
        DUMP8("%02X ", &p[i], 32);
    }
    TRACE(0, "-------------------------------------");
    return ret;
}

static SRAM_TEXT_LOC enum HAL_NORFLASH_RET_T __norflash_test_cmd(int id, uint32_t address, const char *cmd){
    enum HAL_NORFLASH_RET_T ret = 0;
    uint8_t *buf = flash2_test_buf;
    uint32_t size = FLASH2_TEST_BUFSZ;

    uint32_t dst = address;

    // buf = PSRAM_C_TO_NC(buf);
    if(strcmp(cmd, "erase") == 0){
        ret = norflash_erase_and_check(id, dst, size, true);
    }else if(strcmp(cmd, "read") == 0){
        memset(buf, 0, size);
        ret = hal_norflash_read(id, dst, buf, size);
        if(ret != 0){
            TRACE(4, "%s(%s) failed, hal_norflash_read %X return: %d", __func__, cmd, dst, ret);
            return ret;
        }
        for(int i = 0; i < size; i+=32){
            DUMP8("%02X ", &buf[i], 32);
        }
        // uint8_t *p = (uint8_t*)dst;
        // for(int i = 0; i < size; i+=16){
        //     DUMP8("%02X ", &p[i], 16);
        // }
    }else if(strcmp(cmd, "write") == 0){
        memset(buf, FLASH2_TEST_BYTE, size);
        // for(int i = 0; i < size; i++){
        //     buf[i] = i & 0xFF;
        // }
        norflash_write_and_check(id, dst, buf, size, true); //hal_write all, check page by page---------failed
        // norflash_write_and_check2(id, dst, buf, size, true); // write&check page by page-------done
        // norflash_write_and_check3(id, dst, buf, size, true);  // write all page by page, check page by page-----failed
    }else if(strcmp(cmd, "xip") == 0){
        //启动测试
        if(id == HAL_FLASH_ID_1){
            ret = demo_func_cp_flash2(); // load code to ext flash
            if(ret == 0)demo_func_in_flash2(); // xip on ext flash
        }
    }else{
        TRACE(0, "ERROR command: %s", cmd);
    }

    return ret;
}

static int cmd_norflash2_test_func(cmd_tbl_t * cmdtp, int argc, char *argv[], void *handler){
    TRACE(0, "argc=%d", argc);
    for(int i = 0; i < argc; i++){
        TRACE(0, "       [%d]: %s", i, argv[i]);
    }
    if(argc > 3){
        TRACE(0, "Usage: AT+XIPFLASH2=erase|read|write|xip");
        return -1;
    }
    int id = HAL_FLASH_ID_1;
    // if(argc > 2)id = atoi(argv[2]);
    if(id == HAL_FLASH_ID_0){
        uint32_t addr = LFS_FLASH_BASE_ADDR+LFS_FLASH_TOTAL_SIZE;
        return __norflash_test_cmd(id, addr, argv[1]);
    }
    if(id == HAL_FLASH_ID_1){
        uint32_t addr = FLASH1_NC_BASE;
        return __norflash_test_cmd(id, addr, argv[1]);
    }
    TRACE(0, "Error flash id: %d", id);
    return -1;
}

#endif

#ifdef HITEST
static bool hitest_exit = 0;
static uint32_t hitest_duration = 0;
static osThreadId hitest_tid = NULL;
static osThreadId hitest_mic_tid = NULL;
static osThreadId hitest_led_tid = NULL;
static osThreadId hitest_flash_tid = NULL;

static void hitest_mic_task(const void *arg) {
    FTP_HANDLE ftp_handler = NULL;
    int ret = -1;
    TRACE(0, "**************************%s running...", __func__);

    while(!hitest_exit){
        ftp_handler = ftp_connect();
        if(ftp_handler){
            while (!hitest_exit) {
                ret = mic_to_ftp(ftp_handler, 16000, 5);
                if(ret < 0){
                    // TRACE(0, "[%s] __do_record_to_ftp() returns %d", __func__, ret);
                    break;
                }
            }
        }
        ftp_close(ftp_handler);
    }
    TRACE(0, "**************************%s exit...", __func__);
    hitest_mic_tid = NULL;
	osThreadExit();
}

#define LED_INTERVAL 200
static void hitest_led_task(const void *arg) {
    //start sub tasks to test
    TRACE(0, "**************************%s running...", __func__);
	while (!hitest_exit) {
        led_on(LED(G));
        osDelay(LED_INTERVAL);
        led_on(LED(R));
        osDelay(LED_INTERVAL);
        led_on(LED(R)|LED(G));
        osDelay(LED_INTERVAL);
        led_on(LED(R)|LED(G)|LED(B));
        osDelay(LED_INTERVAL);
        led_on(LED(G)|LED(B));
        osDelay(LED_INTERVAL);
        led_on(LED(B));
        osDelay(LED_INTERVAL);
        led_on(LED(R)|LED(B));
        osDelay(LED_INTERVAL);
        led_off();
	}
    TRACE(0, "**************************%s exit...", __func__);
    hitest_led_tid = NULL;
	osThreadExit();
}

static void hitest_flash_task(const void *arg) {
    TRACE(0, "**************************%s running...", __func__);
#if defined(CHIP_HAS_EXT_NORFLASH)&&defined(FLASH1_CTRL_BASE)
    uint32_t total_size, block_size, sector_size, page_size, base, size;

    hal_norflash_get_size(HAL_FLASH_ID_1, &total_size,
                         &block_size, &sector_size, &page_size);
    TRACE(4,"flash info: total: %d KB, block: %d KB, sector: %d KB, page: %d B",
            total_size/1024, block_size/1024, sector_size/1024, page_size);

    ASSERT(FLASH2_TEST_BUFSZ >= sector_size, "FLASH2_TEST_BUFSZ MUST be no less than sector size: %d", sector_size);
    base = FLASH1_NC_BASE;
    size = 0;
    uint32_t total = 0; 
    uint32_t failed_e = 0;
    uint32_t failed_w = 0;
    int ret = 0;
    uint8_t *buf = flash2_test_buf;
    // buf = PSRAM_C_TO_NC(buf);
	while (!hitest_exit) {
        if(size + sector_size <= total_size) {
            uint32_t ws = 0;
            ret = norflash_erase_and_check(HAL_FLASH_ID_1, base, sector_size, false);
            if(ret)failed_e++;
#if 1
            memset(buf, FLASH2_TEST_BYTE, sector_size);
            ret = norflash_write_and_check(HAL_FLASH_ID_1, base, buf, sector_size, false);
            if(ret)failed_w++;
            base += sector_size;
#else
            while(ws < sector_size){
                memset(buf, FLASH2_TEST_BYTE, page_size);
                norflash_write_and_check3(HAL_FLASH_ID_1, base, buf, page_size);
                if(ret)failed_w++;
                base += page_size;
                ws += page_size;
            }
#endif
            size += sector_size;
            total++;
            //base += FLASH2_TEST_BUFSZ;
        } else {
            size = 0;
            base = FLASH1_NC_BASE;
        }
        // osDelay(1);
	}
#endif
    TRACE(0, "**************************%s exit...erase failed=%d/%d,write failed=%d/%d", __func__, failed_e, total, failed_w, total);
    hitest_flash_tid = NULL;
	osThreadExit();
}

osThreadDef(hitest_mic_task, osPriorityNormal, 1, 24*1024, "hitest_mic");
osThreadDef(hitest_led_task, osPriorityNormal, 1, 8*1024, "hitest_led");
osThreadDef(hitest_flash_task, osPriorityNormal, 1, 8*1024, "hitest_flash");

#ifdef __A7_DSP_ENABLE__
#include "a7_cmd.h"
#endif
static void a7_hitest_send_cmd(bool start){
#ifdef __A7_DSP_ENABLE__
    A7_CMD_T a7_cmd;
    a7_cmd.p2 = 0;
    a7_cmd.p1 = start?1:0;
    a7_cmd.type = A7_CMD_TYPE_HITEST;
    mcu_cmd_send(&a7_cmd);            
#endif
}


#define tskRUNNING_CHAR     ( 'X' )
#define tskBLOCKED_CHAR     ( 'B' )
#define tskREADY_CHAR       ( 'R' )
#define tskDELETED_CHAR     ( 'D' )
#define tskSUSPENDED_CHAR   ( 'S' )

static int __show_Runtime_Stat(void)
{
#ifdef osCMSIS_FreeRTOS
    uint8_t x;
    uint32_t ulRunTimeTick,uxArraySize;
    uxArraySize = uxTaskGetNumberOfTasks();
    TaskStatus_t * pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );
    if( pxTaskStatusArray != NULL )
    {
        /* Generate the (binary) data. */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulRunTimeTick );
        TRACE(0, "%-15.15s %-10.10s %-10.10s %-10.10s %-10.10s %-10.10s %-10.10s %-10.10s"
            ,"TskName","TskNum","TskState","TskPrio","StackBase","TcbAddr","MinFreeStk","CPU Usage");
        char cStatus;
        for(x = 0;x < uxArraySize; x++)
        {
            switch( pxTaskStatusArray[ x ].eCurrentState )
            {
                case eRunning:      cStatus = tskRUNNING_CHAR;
                                    break;
                case eReady:        cStatus = tskREADY_CHAR;
                                    break;
                case eBlocked:      cStatus = tskBLOCKED_CHAR;
                                    break;
                case eSuspended:    cStatus = tskSUSPENDED_CHAR;
                                    break;
                case eDeleted:      cStatus = tskDELETED_CHAR;
                                    break;
                case eInvalid:      /* Fall through. */
                default:            cStatus = ( char ) 0x00;
                                    break;
            }
            TRACE(0, "%-15.15s %-10.1d %-10.1c %-10.1d 0x%x 0x%x %-10.1d %-0.2f%%",
                    pxTaskStatusArray[x].pcTaskName,
                    (int)pxTaskStatusArray[x].xTaskNumber,
                    cStatus,
                    (int)pxTaskStatusArray[x].uxCurrentPriority,
                    ( unsigned int )pxTaskStatusArray[x].pxStackBase,
                    ( unsigned int )pxTaskStatusArray[x].xHandle,
                    (int)pxTaskStatusArray[x].usStackHighWaterMark,
                    ((float)pxTaskStatusArray[x].ulRunTimeCounter*1.0/ulRunTimeTick*100.0f));
        }
        TRACE(0, "freeHeapSize:%u, minimumEver:%u", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
        TRACE(0, "dump task ok!");
    }
    vPortFree( pxTaskStatusArray );
#if 0
    static char task_memory[2048] = {0};
    int i = 0, len = 0;
    char buf[49] = {0};

    memset(task_memory, 0, sizeof(task_memory));
    vTaskGetRunTimeStats(task_memory);
    len = strlen(task_memory);

    hal_trace_flush_buffer();
    cmd_printf(handler, "run time info: len %d\n", len);
    for (i = 0; i < len; i += 48) {
        memset(buf, 0, 49);
        memcpy(buf, &task_memory[i], 48);
        cmd_printf(handler, "%s", buf);
    }
    cmd_printf(handler, "\n");
    hal_trace_flush_buffer();
    cmd_printf(handler, "run time info end\n");
#endif
#endif

#ifdef KERNEL_LITEOS_M
    extern uint32_t OsGetAllTskInfo(void);
    OsGetAllTskInfo();
#endif

    return 0;
}




static void hitest_task(const void *arg) {
    a7_hitest_send_cmd(true); // notify a7 to start

    //start sub tasks to test
    hitest_mic_tid = osThreadCreate(osThread(hitest_mic_task), NULL);
    ASSERT(hitest_mic_tid != NULL, "Failed to create hitest_mic Thread");
    hitest_led_tid = osThreadCreate(osThread(hitest_led_task), NULL);
    ASSERT(hitest_led_tid != NULL, "Failed to create hitest_led Thread");

    hitest_flash_tid = osThreadCreate(osThread(hitest_flash_task), NULL);
    ASSERT(hitest_flash_tid != NULL, "Failed to create hitest_flash Thread");

    TRACE(0, "%s init...ok", __func__);
    uint32_t tick = TICKS_TO_MS(hal_sys_timer_get());
    uint32_t start = tick;
	while (!hitest_exit && (!hitest_duration || (TICKS_TO_MS(hal_sys_timer_get())-start)/1000<hitest_duration  )) {
        if(TICKS_TO_MS(hal_sys_timer_get()) - tick > 10*1000){
            extern int __show_Runtime_Stat(void); 
            __show_Runtime_Stat();           
            tick = TICKS_TO_MS(hal_sys_timer_get());
        }
        // osDelay(1);
	}
    //wait sub tasks exit
    if(!hitest_exit){
        TRACE(0, "%s times up...", __func__);
        hitest_exit = true;
    }
    a7_hitest_send_cmd(false); // notify a7 to stop
    TRACE(0, "%s wait for children threads exit...", __func__);
    while(hitest_mic_tid || hitest_led_tid || hitest_flash_tid){
        osDelay(10);
    }
    hitest_tid = NULL;
    TRACE(0, "**************************%s exit...", __func__);
	osThreadExit();
}
osThreadDef(hitest_task, osPriorityNormal, 1, 4*1024, "hitest");


static int do_hitest(cmd_tbl_t *cmd, int argc, char *argv[], void *handler){
    if(argc == 1 || argv[1][0] == '?'){
        RESP(handler, "HITEST is %srunning\n", hitest_tid == NULL?"NOT ":"");
        struct bwifi_station_config *ap = nv_record_get_ap_info();
        if(ap == NULL){
            RESP(handler, "WIFI settings: None\n");
        }else{
            RESP(handler, "WIFI settings: \n");
            RESP(handler, "    ssid=%s\n", ap->ssid);
            RESP(handler, "    passwd=%s\n", ap->passwd);
        }
        char *server, *port, *user, *passwd;
        if(nv_record_get_ftp_config(&server, &port, &user, &passwd)){
            RESP(handler, "FTP settings: \n");
            RESP(handler, "    server=%s\n", server);
            RESP(handler, "    port=%s\n", port);
            RESP(handler, "    user=%s\n", user);
            RESP(handler, "    passwd=%s\n", passwd);
        }else{
            RESP(handler, "FTP settings: None\n");
        }
    	RESP_OK(handler);
        if(hitest_tid){
            extern int __show_Runtime_Stat(void); 
            __show_Runtime_Stat();           
        }
        return 0;
    }

    if(strcmp(argv[1], "1") == 0){
    	RESP_OK(handler);
        if(hitest_tid){
            RESP(handler, "HITEST is running\n");
        }else{
            if(argc > 2)hitest_duration = atoi(argv[2]);
            hitest_tid = osThreadCreate(osThread(hitest_task), NULL);
            ASSERT(hitest_tid != NULL, "Failed to create hitest Thread");
            hitest_exit = false;
            //connect wifi
            wifi_switch_mode(false);
        }
    } else if(strcmp(argv[1], "0") == 0){
    	RESP_OK(handler);
        if(hitest_tid && !hitest_exit){
            hitest_exit = true;
            //wait exit
            while(hitest_tid != NULL){
                osDelay(100);
            }
        } else if(hitest_exit){
            RESP(handler, "HITEST is EXITTING\n");
        }else{            
            RESP(handler, "HITEST is NOT running\n");
        }
    } else if(strcmp(argv[1], "switch") == 0){
        if(hitest_tid){
            if(!hitest_exit){
                hitest_exit = true;
                //wait exit
                while(hitest_tid != NULL){
                    osDelay(100);
                }
            }else{            
                RESP(handler, "HITEST is EXITTING\n");
            }
        }else{
            hitest_tid = osThreadCreate(osThread(hitest_task), NULL);
            ASSERT(hitest_tid != NULL, "Failed to create hitest Thread");
            hitest_exit = false;
            //connect wifi
            wifi_switch_mode(false);
        }
    } else {
        RESP(handler, "Usage: AT+HITEST=?|0|1\n");
        RESP_ERROR(handler, ERROR_ARG);
    }
    return 0;
}
#endif

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
#ifdef CHIP_HAS_RGB_LED
static int do_led_brightness(cmd_tbl_t *cmd, int argc, char *argv[], void *handler){
    if(argc < 2 || argv[1][0] == '?'){
        //read brightness
        RESP(handler, "led brightness: %d\n", led_get_brightness());
        return 0;
    }
    int level = atoi(argv[1]);
    if(level < 1 || level > 100){
        RESP(handler, "brightness out of range: (1, 100)\n");
        return 0;
    }
    led_set_brightness(level);
    RESP_OK();
    return 0;
}


/**
 * Usage: AT+LED=<r|g|b> <on|off|blink> [duration] [counter]
 */
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
#define LED_MIN_ARGC    3
#define LED_USAGE "AT+LED=<on|off|blink>,<r|g|b|all>[,<on_duration_ms>][,<off_duration_ms>][,<counter>]\n"
#define LED_PRINT_HELP()    do { \
        RESP(handler, "Usage: %sExamples:\n", LED_USAGE); \
        RESP(handler, "    AT+LED=on,r                 #red on\n"); \
        RESP(handler, "    AT+LED=on,all               #R/G/B all on\n"); \
        RESP(handler, "    AT+LED=off,all              #R/G/B all off\n"); \
        RESP(handler, "    AT+LED=blink,r,2000,1000,5  #Red blink 5 times like: on(2s)->off(1s)->on(2s)...\n"); \
    }while(0)
#else
#define LED_MIN_ARGC    2
#define LED_USAGE "AT+LED=<on|off|blink>[,<r|g|b|w|y|m|c>][,<on_duration_ms>][,<off_duration_ms>][,<counter>]\n"
#define LED_PRINT_HELP()    do { \
        RESP(handler, "Usage: %s", LED_USAGE); \
        RESP(handler, "Examples(r-Red,g-Green,b-Blue,w-White,y-Yellow,m-Magenta,c-Cyan):\n"); \
        RESP(handler, "    AT+LED=off                  #led off\n"); \
        RESP(handler, "    AT+LED=on,y                 #led on in yellow\n"); \
        RESP(handler, "    AT+LED=on,r                 #led on in red\n"); \
        RESP(handler, "    AT+LED=blink,r,2000,1000,5  #led blink 5 times in red like: on(2s)->off(1s)->on(2s)...\n"); \
    }while(0)
#endif

static int do_led_ctrl(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	int ret;
		
	if (argc < LED_MIN_ARGC) {
		LED_PRINT_HELP();
        return 0;
    }

    uint8_t led = 0;
    bool all = false;
    if(argc > 2){
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
        if (strcasecmp(argv[2], "all") == 0) {
            all = true;
        } else if (strcasecmp(argv[2], "red") == 0 || strcasecmp(argv[2], "r") == 0) {
            led = LED_R;
        } else if (strcasecmp(argv[2], "green") == 0 || strcasecmp(argv[2], "g") == 0) {
            led = LED_G;
        } else if (strcasecmp(argv[2], "blue") == 0 || strcasecmp(argv[2], "b") == 0) {
            led = LED_B;
        } else {
            RESP(handler, "Unknown led: %s\n", argv[2]);
            LED_PRINT_HELP();
            return 0;
        }
#else
        if (strcasecmp(argv[2], "red") == 0 || strcasecmp(argv[2], "r") == 0) {
            led = LED(R);
        } else if (strcasecmp(argv[2], "green") == 0 || strcasecmp(argv[2], "g") == 0) {
            led = LED(G);
        } else if (strcasecmp(argv[2], "blue") == 0 || strcasecmp(argv[2], "b") == 0) {
            led = LED(B);
        } else if (strcasecmp(argv[2], "white") == 0 || strcasecmp(argv[2], "w") == 0) {
            led = LED_W;
        } else if (strcasecmp(argv[2], "yellow") == 0 || strcasecmp(argv[2], "y") == 0) {
            led = LED_Y;
        } else if (strcasecmp(argv[2], "magenta") == 0 || strcasecmp(argv[2], "m") == 0) {
            led = LED_M;
        } else if (strcasecmp(argv[2], "cyan") == 0 || strcasecmp(argv[2], "c") == 0) {
            led = LED_C;
        } else {
            RESP(handler, "Unknown led: %s\n", argv[2]);
            LED_PRINT_HELP();
            return 0;
        }
#endif
    }
    if(strcmp(argv[1], "off") == 0){
#ifdef LED_SEPARATELY_CONTROLLED_BY_USER
        led_off(led);
#else
        led_off();
#endif
        RESP_OK(handler);
        return 0;
    }
    if(strcmp(argv[1], "on") == 0){
        led_on(led);
        RESP_OK(handler);
        return 0;
    }
    if(strcmp(argv[1], "blink") == 0){
        if (argc < 4) {
    		LED_PRINT_HELP();
            return 0;
        }
        int on_ms = atoi(argv[3]);
        int off_ms = argc > 4?atoi(argv[4]):0;
        int c = argc > 5?atoi(argv[5]):0;
        if(on_ms <= 0 || off_ms < 0){
            RESP(handler, "duration must > 0");
            return 0;
        }
        if(c < 0){
            RESP(handler, "counter must >= 0, 0 for INFINITE");
            return 0;
        }
        led_blink(led, (uint16_t)on_ms, (uint16_t)off_ms, (uint16_t)c);
        RESP_OK(handler);
        return 0;
    }
    RESP(handler, "Unknown action %s", argv[1]);
    LED_PRINT_HELP();
    return 0;
}


//AT+LEDHW=<on|off|blink|unblink>,<r|g|b|all>[,<on_ms>][,<off_ms>]
static int do_led_hwtest(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	int ret;
		
	if (argc < LED_MIN_ARGC) {
		LED_PRINT_HELP();
        return 0;
    }
    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = 8000, // unit: Hz
        .ratio = 25,  // duty cycle = high/(high+low)
        .inv = false,
        .sleep_on = false,
    };
    uint8_t led = 0;
    bool all = false;

    if(argc > 2){
        if (strcasecmp(argv[2], "all") == 0) {
            all = true;
        } else if (strcasecmp(argv[2], "red") == 0 || strcasecmp(argv[2], "r") == 0) {
            led = LED_R;
        } else if (strcasecmp(argv[2], "green") == 0 || strcasecmp(argv[2], "g") == 0) {
            led = LED_G;
        } else if (strcasecmp(argv[2], "blue") == 0 || strcasecmp(argv[2], "b") == 0) {
            led = LED_B;
        } else {
            RESP(handler, "Unknown led: %s\n", argv[2]);
            LED_PRINT_HELP();
            return 0;
        }
    }
    if(strcmp(argv[1], "off") == 0){
        if(argc < 3 || all){
            // ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_1);
            // TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_1, ret);
            ret = hal_pwm_disable(HAL_PWM_ID_1);
            TRACE(3, "hal_pwm_disable(%d) returns %d", HAL_PWM_ID_1, ret);

            // ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_2);
            // TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_2, ret);
            ret = hal_pwm_disable(HAL_PWM_ID_2);
            TRACE(3, "hal_pwm_disable(%d) returns %d", HAL_PWM_ID_2, ret);

            // ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_3);
            // TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_3, ret);
            ret = hal_pwm_disable(HAL_PWM_ID_3);
            TRACE(3, "hal_pwm_disable(%d) returns %d", HAL_PWM_ID_3, ret);
        } else if(led == LED_R){
            // ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_1);
            // TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_1, ret);
            ret = hal_pwm_disable(HAL_PWM_ID_1);
            TRACE(3, "hal_pwm_disable(%d) returns %d", HAL_PWM_ID_1, ret);
        } else if(led == LED_G){
            // ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_2);
            // TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_2, ret);
            ret = hal_pwm_disable(HAL_PWM_ID_2);
            TRACE(3, "hal_pwm_disable(%d) returns %d", HAL_PWM_ID_2, ret);
        } else if(led == LED_B){
            // ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_3);
            // TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_3, ret);
            ret = hal_pwm_disable(HAL_PWM_ID_3);
            TRACE(3, "hal_pwm_disable(%d) returns %d", HAL_PWM_ID_3, ret);
        }
        RESP_OK(handler);
        return 0;
    } else if(strcmp(argv[1], "on") == 0){
        if(argc < 3 || all){
            ret = hal_pwm_enable(HAL_PWM_ID_1, &pwm_cfg);
            TRACE(3, "hal_pwm_enable(%d) returns %d", HAL_PWM_ID_1, ret);
            ret = hal_pwm_enable(HAL_PWM_ID_2, &pwm_cfg);
            TRACE(3, "hal_pwm_enable(%d) returns %d", HAL_PWM_ID_2, ret);
            ret = hal_pwm_enable(HAL_PWM_ID_3, &pwm_cfg);
            TRACE(3, "hal_pwm_enable(%d) returns %d", HAL_PWM_ID_3, ret);
        } else if(led == LED_R){
            ret = hal_pwm_enable(HAL_PWM_ID_1, &pwm_cfg);
            TRACE(3, "hal_pwm_enable(%d) returns %d", HAL_PWM_ID_1, ret);
        } else if(led == LED_G){
            ret = hal_pwm_enable(HAL_PWM_ID_2, &pwm_cfg);
            TRACE(3, "hal_pwm_enable(%d) returns %d", HAL_PWM_ID_2, ret);
        } else if(led == LED_B){
            ret = hal_pwm_enable(HAL_PWM_ID_3, &pwm_cfg);
            TRACE(3, "hal_pwm_enable(%d) returns %d", HAL_PWM_ID_3, ret);
        }
        RESP_OK(handler);
        return 0;
    } else if(strcmp(argv[1], "unblink") == 0){
        if(argc < 3 || all){
            ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_1);
            TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_1, ret);

            ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_2);
            TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_2, ret);

            ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_3);
            TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_3, ret);
        } else if(led == LED_R){
            ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_1);
            TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_1, ret);
        } else if(led == LED_G){
            ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_2);
            TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_2, ret);
        } else if(led == LED_B){
            ret = hal_pwm_breathing_led_disable(HAL_PWM_ID_3);
            TRACE(3, "hal_pwm_breathing_led_disable(%d) returns %d", HAL_PWM_ID_3, ret);
        }
        RESP_OK(handler);
        return 0;
    } else if(strcmp(argv[1], "blink") == 0){
        if (argc < 4) {
    		LED_PRINT_HELP();
            return 0;
        }
        int on_ms = atoi(argv[3]);
        int off_ms = argc > 4?atoi(argv[4]):0;
        if(on_ms <= 0 || off_ms < 0){
            RESP(handler, "duration must > 0");
            return 0;
        }
        struct HAL_PWM_BR_CFG_T br = {
            .off_time_ms = off_ms,
            .on_time_ms = on_ms,
            .fade_time_ms = 0,
        };

        if(all){
            ret = hal_pwm_breathing_led_enable(HAL_PWM_ID_1, &br);
            TRACE(3, "hal_pwm_breathing_led_enable(%d) returns %d", HAL_PWM_ID_1, ret);
            ret = hal_pwm_breathing_led_enable(HAL_PWM_ID_2, &br);
            TRACE(3, "hal_pwm_breathing_led_enable(%d) returns %d", HAL_PWM_ID_2, ret);
            ret = hal_pwm_breathing_led_enable(HAL_PWM_ID_3, &br);
            TRACE(3, "hal_pwm_breathing_led_enable(%d) returns %d", HAL_PWM_ID_3, ret);
        } else if(led == LED_R){
            ret = hal_pwm_breathing_led_enable(HAL_PWM_ID_1, &br);
            TRACE(3, "hal_pwm_breathing_led_enable(%d) returns %d", HAL_PWM_ID_1, ret);
        } else if(led == LED_G){
            ret = hal_pwm_breathing_led_enable(HAL_PWM_ID_2, &br);
            TRACE(3, "hal_pwm_breathing_led_enable(%d) returns %d", HAL_PWM_ID_2, ret);
        } else if(led == LED_B){
            ret = hal_pwm_breathing_led_enable(HAL_PWM_ID_3, &br);
            TRACE(3, "hal_pwm_breathing_led_enable(%d) returns %d", HAL_PWM_ID_3, ret);
        }
        RESP_OK(handler);
        return 0;
    }
    RESP(handler, "Unknown action %s", argv[1]);
    LED_PRINT_HELP();
    return 0;
}

#endif

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



static int do_audioloop_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	if (argc != 2) {
		RESP_ERROR(handler, ERROR_ARG);
		return 0;
	}
	if (strcasecmp(argv[1], "start") == 0) {
		aud_loop_test_start(2, 14);
	} else if (strcasecmp(argv[1], "stop") == 0) {
        aud_loop_test_stop();
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
  read HW version
  
  AT+PWM
*/


static int do_pwm_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	
	RESP_OK(handler);
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

static int do_spiflsh_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	RESP_OK(handler);
	return 0;
}

#define RECORD_FNAME    (char *)"/record.wav"
static int do_mic2file(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if(0){
        FILE *fp = fopen("/record.wav", "w+");
        if(!fp)return -1;
        int ret = fseek(fp, 0, SEEK_END);
        TRACE(0, "fseek end: %d", ret);
        ret = fwrite("abcd", 1, 4, fp);
        TRACE(0, "fwrite: %d", ret);
        ret = fseek(fp, 0, SEEK_END);
        TRACE(0, "fseek end: %d", ret);
        fclose(fp);
        return 0;
    }
    uint32_t duration = 5;
    if(argc == 1){
        mic_to_file_start(duration, RECORD_FNAME);
    }else if(!strcmp(argv[1], "start")){
        if(argc > 2)duration = atoi(argv[2]);        
        mic_to_file_start(duration, argc>3?NULL:RECORD_FNAME);
    }else if(!strcmp(argv[1], "stop")){
        mic_to_file_stop();
    }else {
        RESP(handler, "Usage: %s", cmd->usage);
        return -1;
    }
    
	RESP_OK(handler);
	return 0;
}


const cmd_tbl_t at_cmd_list_factory[] = {
        /*Basic CMD*/
        {
             "AT+CONFWIFI",          4,   do_conf_wifi,
             "AT+CONFWIFI                - do wifi test\n"
        },
        {
             "AT+WIFI",          4,   do_conf_wifi2,
             "AT+WIFI=<?|sta|ap|toggle>   - read wifi mode&config or switch ap/sta mode\n"
        },
        {
             "AT+FTPCONF",          5,   do_conf_ftp,
             "AT+FTPCONF=[server][,port][,user][,passwd] - read/write ftp setting\n"
        },
        {
             "AT+FTPPUT",          6,   do_ftp_put,
             "AT+FTPPUT=filename[,server][,port][,user][,passwd] - upload to ftp\n"
        },
#ifdef CHIP_HAS_RGB_LED
        {
             "AT+LEDBRIGHTNESS",          2,   do_led_brightness,
             "AT+LEDBRIGHTNESS           - AT+LEDBRIGHTNESS=[brightness]\n                brightness: 1~100, 25 default\n"
        },
        {
             "AT+LED",          6,   do_led_ctrl,
             "AT+LED                     - "LED_USAGE
        },
        {
             "AT+LEDHW",          5,   do_led_hwtest,
             "AT+LEDHW                   - AT+LEDHW=<on|off|blink|unblink>,<r|g|b|all>[,<on_ms>][,<off_ms>]\n"
        },
#endif
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
             "AT+PWM",          4,   do_pwm_test,
             "AT+PWM                - do pwm test\n"
        },
        {
             "AT+RDSN",          4,   do_read_sn,
            "AT+RDSN                - read SN\n"
        },
        {
             "AT+SPIFLASH",          4,   do_spiflsh_test,
             "AT+RF                - do rf test\n"
        },   
#if defined(CHIP_HAS_EXT_NORFLASH) && defined(TEST_FOR_EXT_NORFLASH)
        {
            "AT+XIPFLASH2",
            2,
            cmd_norflash2_test_func,
            "AT+XIPFLASH2           - AT+XIPFLASH2=erase|read|write|xip   test external nor flash\n"
        },
#endif
#ifdef HITEST
        {
            "AT+HITEST",            3, do_hitest,
            "AT+HITEST[=?|0|1|switch][,duration]   - do full pressure test"
        },
#endif
        {
            "AT+MIC2FILE",            4, do_mic2file,
            "AT+MIC2FILE[=start|stop]  - record for 10 seconds and save to file /record.wav"
        },
    };
	
const int at_cmd_list_factory_size = ARRAY_SIZE(at_cmd_list_factory);