/**
 * @file at.c
 * @author Alvin (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-07
 * 
 * @copyright Copyright (c) 2022
 * 
 * BES release new AT command with lib and without source codes.
 * And we have made some changes with the original AT commands implemented in at.c.
 * So we have to move them out and implement again with differrent names
 */

#include <arpa/inet.h>
#include "cmsis_os.h"
#include "console.h"
#include "at.h"
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
// #include "hexdump.h"
#include "net_memory.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
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


#include "wifi_drv.h"
#include "bt_if.h"
#include "besfd_api.h"

#define MAX_LEN 128

extern void str2mac(const char * szMac,uint8_t * pMac);

struct bwifi_station_config *nv_record_get_ap_info(void);
void nv_record_set_ap_info(struct bwifi_station_config *p);
//
/**
 * AT+WSCONN --------> AT+WSCONN2
 * wifi sta mode, connect specified ap
 * If NONE provided, just load ap info from NV and connect it.
 */
static int do_conn( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
//  u8 channel;
//  int8_t rssi;
//  struct ip_info ip;

    struct bwifi_station_config p;
    memset(&p, 0, sizeof(p));
    p.mfp = WIFI_MFP_DEFAULT;
    if (argc > 1)
        strcpy((char*)p.ssid,argv[1]);

    if (argc > 2)
        strcpy((char*)p.passwd, argv[2]);

    if (argc > 3 && !strcmp(argv[3], "hidden"))
        p.hidden = 1;

    if (argc > 4 && strlen(argv[4])==17) {
        str2mac(argv[4], p.bssid);
        if (!net_is_valid_mac(p.bssid)) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
    }

    if (argc > 5)
        p.wep_keyid = atoi(argv[5]);

    if(argc > 6) {
        if(!strcmp(argv[6], "disable")) {
            p.mfp = WIFI_MFP_DISABLE;
        } else if(!strcmp(argv[6], "enable")) {
            p.mfp = WIFI_MFP_ENABLE;
        } else if(!strcmp(argv[6], "require")) {
            p.mfp = WIFI_MFP_REQUIRED;
        } else if(!strcmp(argv[6], "default")) {
            p.mfp = WIFI_MFP_DEFAULT;
        } else {
            cmd_printf(handler, "parse mfp error\r\n");
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
    }
    ret = bwifi_connect(&p);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    nv_record_set_ap_info(&p);
    RESP_OK(handler);
    return 0;
}

#if LWIP_SUPPORT
/**
 * AT+WSSETIP ------> AT+WSSETIP2
 * enable DNS provided
 * AT+WSSETIP=enabled [,ip,netmask,gw [,dns]]
 */
static int do_set_static_ip(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    int enable;
    struct ip_info ip;
	ip_addr_t dnsip;

    if (argc != 2 && argc != 5 && argc != 6) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    enable = atoi(argv[1]);
    if (enable == 0) {
        ret = bwifi_set_static_ip(NULL);
		TRACE(0, "bwifi_set_static_ip ret=%d", ret);
        if (ret) {
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }
    } else {
        if (argc != 5 && argc != 6) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }

        ret = inet_aton(argv[2], &ip.ip) &&
            inet_aton(argv[3], &ip.netmask) &&
            inet_aton(argv[4], &ip.gw);
        if (ret == 0) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
		
        ret = bwifi_set_static_ip(&ip);
        if (ret) {
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }

		// dns 
		if (argc == 6) {
			ret = inet_aton(argv[5], &dnsip);
			if (ret == 0) {
            	RESP_ERROR(handler, ERROR_ARG);
            	return 0;
			}
        } else 
        	inet_aton("202.96.134.133", &dnsip);

		extern void dns_setserver(u8_t numdns, const ip_addr_t *dnsserver);
		dns_setserver(0, &dnsip);
    }
    RESP_OK(handler);
    return 0;
}

#endif


int hal_analogif_reg_read(unsigned short reg, unsigned short *val);
int hal_analogif_reg_write(unsigned short reg, unsigned short val);
#define MAX_USERNAME_LEN 20
#define MAX_PASSWD_LEN 30

/**
 * AT+MD ------> AT+MD2
 * AT+MD2=addr,len[,ftp_ip,port,username,password,filename]
 */
static int do_mem_read(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	//AT+MD=0x20000000,128,192.168.0.111,21,ad,ad,dump.hex
	//		 addr,len(bit),[ftp_ip,port,username,password,filename]
    uint32_t addr, len;
    unsigned short reg_val;
    uint32_t val;
    if (argc != 3 && argc !=8) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    addr = strtoul(argv[1], NULL, 16);
	len = atoi(argv[2]);
	// to check addr and len in future;
	if (argc == 3) {// output to terminal
		for(int i = 0; i < len; i+=32){
		    if (addr < 0x4000) {
		        hal_analogif_reg_read((unsigned short)addr, &reg_val);
				RESP(handler, "%04x", reg_val);
				hal_analogif_reg_read((unsigned short)addr+16, &reg_val);
				RESP(handler, "%04x", reg_val);
		    } else if (addr < 0x8000) {
		        addr -= 0x4000;
		        hal_phyif_reg_read((unsigned short)addr, &reg_val);
		        RESP(handler, "%04x", reg_val);
				hal_phyif_reg_read((unsigned short)addr+16, &reg_val);
		        RESP(handler, "%04x", reg_val);
		    } else {
		        val = *(volatile uint32_t *)addr;
		        RESP(handler, "%08x", (unsigned int)val);
		    }
			if ((i+32)%256 == 0)
				RESP(handler, "\n");
			else
				RESP(handler, "  ");
			addr +=4;
		}
	}else if (argc == 8) {// output to ftp
		char ftp_ip[20] = {0};
	    int ftp_port = 21;
	    char ftp_data_path[MAX_LEN] = {0};
	    char ftp_username[MAX_USERNAME_LEN] = {0};
	    char ftp_password[MAX_PASSWD_LEN] = {0};

		memcpy(ftp_ip, argv[3], sizeof(argv[3]));
		ftp_port = atoi(argv[4]);
        strcpy(ftp_username,argv[5]);
        strcpy(ftp_password,argv[6]);
		strcpy(ftp_data_path,argv[7]);
		cmd_printf(handler, "[%s] argvs=%s, %s, %s, %s, %s !\n",cmd->name, argv[3], argv[4], argv[5], argv[6], argv[7]);
		cmd_printf(handler, "[%s] argvs=%s, %d, %s, %s, %s !\n",cmd->name, ftp_ip, ftp_port, ftp_username, ftp_password, ftp_data_path);
		
		void *ftp_handler = NULL;
			
		ftp_handler = ftp_client_init();
		if (ftp_handler == NULL)
		{
			cmd_printf(handler, "ftp client init failed\n");
			return 0;
		}
		cmd_printf(handler, "[%s] ip:%s port:%d\n",cmd->name,argv[3],ftp_port);
		if (ftp_connect_server(ftp_handler, argv[3], ftp_port) < 0)
		{
			cmd_printf(handler, "[%s] ftp connect server failed !\n",cmd->name);
			ftp_client_free(ftp_handler);
			return 0;
		}
	
		if(ftp_login(ftp_handler, argv[5], argv[6]) < 0)
		{
			cmd_printf(handler, "[%s] ftp login on server failed !\n",cmd->name);
			ftp_disconnect(ftp_handler);
			ftp_client_free(ftp_handler);
			return 0;
		}
	
		if(ftp_client_notice(ftp_handler,argv[7]) !=0)
		{
			TRACE(0, "[%s] notice ftp server %s fail\n", cmd->name,ftp_data_path);
			ftp_disconnect(ftp_handler);
			ftp_client_free(ftp_handler);
			return 0;
		}
			
		for(int i = 0; i < len; i+=32){
		    if (addr < 0x4000) {
		        hal_analogif_reg_read((unsigned short)addr, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				hal_analogif_reg_read((unsigned short)addr+16, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				TRACE(0, "[%s] ftp_client_dumpdata\n", cmd->name);
		    } else if (addr < 0x8000) {
		        addr -= 0x4000;
				hal_phyif_reg_read((unsigned short)addr, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				hal_phyif_reg_read((unsigned short)addr+16, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				TRACE(0, "[%s] ftp_client_dumpdata\n", cmd->name);
		    } else {
		        val = *(volatile uint32_t *)addr;
		        ftp_client_dumpdata(ftp_handler, (const char *)&val, 4);
				TRACE(0, "[%s] ftp_client_dumpdata, addr=%08x, val=%08x\n", cmd->name, addr, val);
		    }
			addr +=4;
		}
		ftp_disconnect(ftp_handler);
    	ftp_client_free(ftp_handler);
	}
	RESP_OK(handler) ;
    return 0;
}


#if defined(__NET_FTP_CLIENT_SUPPORT__) && defined(__A7_DSP_ENABLE__)
#define MAX_USERNAME_LEN 20
#define MAX_PASSWD_LEN 30


/**
 * mic record dump
 * AT+RECDUMP ------> AT+RECDUMP2
 * AT+RECDUMP2=[,-r<samprate>] [,-d5] [,-s192.168.137.1, -p21, -uusername, -wpassword]
 * 
 * 3 channels & 16 bits per sample per channel
 * If NO ftp server provided, just use FTP settings stored in NV
 */

static int do_record_to_ftp(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc > 10) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    if(!wifi_sta_connected()){
        cmd_printf(handler, "WIFI MUST be in STA mode and connected");
        return 0;
    }
    /**
     * AT+RECDUMP=-c2,-fS16, -r16000, -d5, -s192.168.137.1, -p21, -uusername, -wpassword, -h/test.pcm
     * [-c] mic record audio channel num(default 2)
     * [-f] mic record audio format (default pcm)
     * [-r] mic record audio sample rate (48000 16000 etc. 16000 default)
     * [-d] mic record audio duration (unit secord, default 5 seconds)
     * [-s] ftp server ip address
     * [-p] ftp server port
     * [-u] ftp server username
     * [-w] ftp server password
     */
    int mic_ch = 1;
    int dur_s = 5;
    char audio_format[10] = {0};
    int format = -1;
    int sample_rate = 16000;


    char *server="", *port="", *user="", *passwd="";

    for (int i = 1; i < argc; i++)
    {
        cmd_printf(handler, " argv[%d] = %s  \n",i,argv[i]);
        char *p = argv[i];
        if (*p == '-'){
            p++;
            char opt = *p++;

            switch (opt) {
            case 'c':
                mic_ch = atoi(p);
                break;
            case 'f':
                if (!strcmp("S16", p)) {
                    format = 0; /* for audio format*/
                }
                break;
            case 'r':
                sample_rate = atoi(p);
                break;
            case 'd':
                dur_s = atoi(p);
                if(dur_s <= 0 || dur_s > 600){
                    cmd_printf(handler, "Duration(%d) out of range (1...600) seconds.", dur_s);
                    return 0;
                }
                break;
            case 's':
                server = p;
                break;
            case 'p':
                port = p;
                break;
            case 'u':
                user = p;
                break;
            case 'w':
                passwd = p;
                break;
            case 'h':
                break;
            default:
                break;
            }
        }
    }
    FTP_HANDLE ftp_handler;
    if(*server){
        ftp_handler = ftp_connect_with(server, port, user, passwd);
        if(!ftp_handler){
            cmd_printf(handler, "ftp params error, Usage: %s,....,-s<server>,-p<port>,-u<user>,-p<passwd>,.... \n", cmd->name);
            return 0;
        }
    }else{
        ftp_handler = ftp_connect();
        if(!ftp_handler){
            cmd_printf(handler, "FTP settings not provided in AT and NV. Usage: %s,....,-s<server>,-p<port>,-u<user>,-p<passwd>,.... \n", cmd->name);
            return 0;
        }
    }
    mic_to_ftp(ftp_handler, sample_rate, dur_s);

    // cmd_printf(handler, "%s done, total record %ds data size %d byte\n", cmd->name, dur_s, total_record_size);

out:
    ftp_close(ftp_handler);
    (void)format;
    (void)mic_ch;
    return 0;
}


#endif//__NET_FTP_CLIENT_SUPPORT__



#if !defined(__BT_CLOSE__)


#ifdef DEBUG_SPP_LOG
static void bt_spp_init(void)
{
    AtSppInitApi();
}
#endif

/*
#include "bt_drv_reg_op.h"
int get_bt_rssi_value(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    rx_agc_t rssi_val;
    for(int i =0;i<3;i++)
    {
        bt_drv_read_rssi_in_dbm(0x80+i,&rssi_val);
        TRACE(0, "RSSI:%d  rxgain:%d",rssi_val.rssi,rssi_val.rxgain);
    }
    return 0;
}
*/

extern int factory_section_set_bt_address(uint8_t *bt_addr);
extern uint8_t* factory_section_get_bt_address(void);
extern int factory_section_set_bt_name(const char *name,int len);
extern int factory_section_set_ble_address(uint8_t* bleAddr);

/**
 *
 * AT+SETBTMAC & AT+GETBTMAC ------> AT+BTMAC
 * AT+BTMAC      - read bt mac
 * AT+BTMAC=?    - read bt mac
 * AT+BTMAC=aa:bb:cc:dd:ee:ff   -set bt mac
 * 
 * NOTICE: The last bytes MUST be even
 * 
 */
static int at_bt_mac(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if (argc < 2 || strcmp(argv[1], "?") == 0)
  {
    const uint8_t *mac = factory_section_get_bt_address();
    if (mac)
    {
      TRACE(0, "get bt mac from factory: " MACSTR "\n", MAC2STR(mac));
      RESP_OK(handler);
    }
    else
    {
      TRACE(0, "get bt mac from factory failed: ");
      RESP_ERROR(handle, -1);
    }
    return 0;
  }
  int ret = 0;
  unsigned char maddr[6] = {0};

  if (strlen(argv[1]) == 17)
  {
    str2mac(argv[1], maddr);
    if (!bt_is_valid_mac(maddr))
    {
      TRACE(0, "The mac addr is invalid");
      return -1;
    }
    ret = factory_section_set_bt_address(maddr);
    if (ret != 0)
    {
      TRACE(0, "Notice set bt mac addrress fail(%d),please check!!!", ret);
    }
    else
    {
      RESP_OK(handler);
    }
  }
  return 0;
}

/**
 * 
 * AT+SETBTMAC & AT+GETBTMAC ------> AT+BTMAC
 * AT+BTMAC      - read bt mac
 * AT+BTMAC=?    - read bt mac
 * AT+BTMAC=aa:bb:cc:dd:ee:ff   -set bt mac
 * 
 * NOTICE: The last bytes MUST be even
 * 
 */
static int at_ble_mac(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if (argc < 2 || strcmp(argv[1], "?") == 0)
  {
    const uint8_t *mac = factory_section_get_ble_address();
    if (mac)
    {
      TRACE(0, "get ble mac from factory: " MACSTR "\n", MAC2STR(mac));
      RESP_OK(handler);
    }
    else
    {
      TRACE(0, "get ble mac from factory failed: ");
      RESP_ERROR(handle, -1);
    }
    return 0;
  }

  if (strlen(argv[1]) == 17)
  {
    int ret = 0;
    unsigned char maddr[6] = {0};
    str2mac(argv[1], maddr);
    if (!bt_is_valid_mac(maddr))
    {
      TRACE(0, "The mac addr is invalid");
      return -1;
    }
    ret = factory_section_set_ble_address(maddr);
    if (ret != 0)
    {
      TRACE(0, "Notice set ble mac addrress fail(%d),please check!!!", ret);
    }
    else
    {
      RESP_OK(handler);
    }
  }
  return 0;
}

// New AT command: AT+BTNAME=?|<name>
static int at_bt_name(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  int ret = 0;
  unsigned char maddr[6] = {0};
  if (argc < 2 || strcmp(argv[1], "?") == 0)
  {
    const uint8_t *name = factory_section_get_bt_name();
    if (name)
    {
      TRACE(0, "get bt name from factory: %s", name);
      RESP_OK(handler);
    }
    else
    {
      TRACE(0, "get bt name from factory failed");
      RESP_ERROR(handle, -1);
    }
    return 0;
  }
  char *name = argv[1];
  if (strlen(name) == 0)
  {
      TRACE(0, "bt name is invalid");
      return -1;
  }
  ret = factory_section_set_bt_name(name, strlen(name)+1);
  if (ret != 0)
  {
    TRACE(0, "Notice set bt name fail(%d),please check!!!", ret);
  }
  else
  if(app_is_stack_ready()){
    ret=bt_set_local_dev_name((const unsigned char *)name, strlen(name)+1);
    ret = bt_stack_config((const unsigned char *)name, strlen(name)+1);
    //btif_update_bt_name((const unsigned char *)name, strlen(name)+1);
    TRACE(0, "%s: bt_stack_config(%s) returns %d", __func__, name, ret);
    RESP_OK(handler);
  }

  return 0;
}



#endif



#if !defined(__BT_CLOSE__)
#include "hal_aud.h"
#include "app_bt_stream.h"

/**
 * MAX vol level: TGT_VOLUME_LEVEL_MAX
 * AT+VOLUP ------> AT+VOLUP2
 * 
 */
extern int8_t stream_bis_volume;
static int do_volumeup(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    stream_bis_volume ++;
    if (stream_bis_volume > TGT_VOLUME_LEVEL_MAX)
        stream_bis_volume = TGT_VOLUME_LEVEL_MAX;
    app_bt_stream_volumeset(stream_bis_volume);
    return 0;
}

#endif


#ifdef BT_MEDIA_SUPPORT_LPLAYER
#include "aud_player.h"

static void lplayer_printf_process_result(const char * process, const char* result)
{
    if(process && result) {
        TRACE(0, "lplayer %s %s", process, result);
    }
}

#define LPLAYER_SUCC(s) do{lplayer_printf_process_result(s, "success");}while(0);
#define LPLAYER_ERROR(s) do{lplayer_printf_process_result(s, "error");}while(0);

static int do_lplayer_start(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    char *uri = NULL;
    TRACE(0, "argc = %d", argc);
    if (argc > 1)
    {
        uri = argv[1];
    }

    if(argc > 2){
        extern void lplayer_test_play_file2(char *fname);
        lplayer_test_play_file2(uri);
    } else {
        int ret = uri?aud_player_start(uri, NULL):aud_player_resume();
        TRACE(0, "%s(%s) returns %d", __func__, uri?uri:"", ret);
    }

    return 0;
}

static int do_lplayer_stop(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = aud_player_stop();
    TRACE(0, "%s returns %d", __func__, ret);

    return 0;
}

static int do_lplayer_pause(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = aud_player_pause();
    TRACE(0, "%s returns %d", __func__, ret);

    return 0;
}

static int do_lplayer_set_vol(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int vol = atoi(argv[1]);
        if (0 != aud_player_set_vol(vol))
        {
            LPLAYER_ERROR("set volume");
            return -1;
        }
        RESP(handler, "volume=%d", vol);
    } else {
        //read volume
        uint8_t nv_record_get_aud_vol(void);
        RESP(handler, "volume=%d", nv_record_get_aud_vol());
    }
    return 0;
}


static int do_lplayer_seek(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int time_s = atoi(argv[1]);
        TRACE(0, "time_s = %d", time_s);
        if (0 != aud_player_seek(time_s))
        {
            LPLAYER_ERROR("seek");
            return -1;
        }
        LPLAYER_SUCC("seek");
        return 0;
    }
    LPLAYER_ERROR("seek");
    return -1;
}

static int do_lplayer_show_current_play_time(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int cur_play_time = aud_player_tell();
    if(cur_play_time < 0)
    {
        LPLAYER_ERROR("get_pos");
        return -1;
    }
    TRACE(0, "%s cur_play_time ====> %d ms", __func__, cur_play_time);
    LPLAYER_SUCC("get_pos");
    return 0;
}

#endif


#include "apps.h"
static int at_prompt_play(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if(argc > 2){
    RESP_ERROR(handle, -1);
    return 0;
  }
  APP_STATUS_INDICATION_T status = APP_STATUS_INDICATION_POWERON;
  if(argc > 1)status = atoi(argv[1]);
  int ret = app_voice_report(status, 0);
  TRACE(0, "app_voice_report: returns %d", ret);

  return 0;
}
#if defined(LFS_FLASH_TOTAL_SIZE) && defined(LFS_FLASH_BASE_ADDR)
#if defined(LITTLEFS_PARTITION_SUPPORT)
#include <sys/dirent.h>
#include <sys/stat.h>
//AT+FSLIST=[<dir_or_file_name>] [,size]
static int at_fs_list(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if(argc > 3){
    RESP_ERROR(handle, -1);
    return 0;
  }
  char *path = "/";
  if(argc > 1 && argv[1] && strlen(argv[1]) > 0){
    path = argv[1];
  }
    int size = argc > 2?atoi(argv[2]):0;
    struct stat st;
    int ret = stat(path, &st);
    if(ret < 0){
        RESP(handler, "stat(%s) returns %d", path, ret);
        return -1;
    }
    if(st.st_mode & S_IFDIR){
        //DIR, list all the files
        RESP(handler, "DIR: %s\n", path);
        DIR * dir = opendir(path);
        if(dir == NULL){
            TRACE(0, "%s: open %s failed", __func__, path);
            return -1;
        }
        while(true){
            struct dirent *ent = readdir(dir);
            //TRACE(0, "readdir returns %d", ret);
            if(ent == NULL)break;
            if (ent->d_type == DT_REG)
                TRACE(0, "    [F] %s        %d", ent->d_name, ent->d_size);
            else if (ent->d_type == DT_DIR)
                TRACE(0, "    [D] %s", ent->d_name);
            else
                TRACE(0, "    [E] %s        %d", ent->d_name, ent->d_size);
            free(ent);
        }
        closedir(dir);
    }else if(st.st_mode & S_IFREG){
        //FILE
        RESP(handler, "FILE: %s     Size: %ld\n", path, st.st_size);
        if(size <= 0)return 0;
        if(size > st.st_size)size = st.st_size;
        // dump file content
        FILE *fp = fopen(path, "r");
        if (fp)
        {
            uint8_t buf[256];
            while (size > 0)
            {
                int nread = (size > sizeof(buf)) ? sizeof(buf) : size;
                ret = fread(buf, nread, 1, fp);
                RESP(handler, "read return: %d\n", ret);
                if (ret < 0)
                {
                    fclose(fp);
                    return -1;
                }
                for (int i = 0; i < ret; i += 16)
                {
                    DUMP8(" %02X", &buf[i], 16);
                }
                size -= ret;
            }
            fclose(fp);
        }
    }
    return 0;
}

#else
#error
    //services/wifi_app/littlefs/lfs_file_ops.h
#include "lfs_file_ops.h"
static int at_fs_list(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if(argc > 2){
    RESP_ERROR(handle, -1);
    return 0;
  }
  char *path = "/";
  if(argc > 1 && argv[1] && strlen(argv[1]) > 0){
    path = argv[1];
  }
  lfs_dir_t * dir = lfs_file_ops_opendir(path);
  if(dir == NULL){
    TRACE(0, "%s: open %s failed", __func__, path);
    return -1;
  }
  while(true){
    struct lfs_info info;
    int ret = lfs_file_ops_readdir(dir, &info);
    //TRACE(0, "readdir returns %d", ret);
    if(ret <= 0)break;
    if (info.type == LFS_TYPE_REG)
        TRACE(0, "    [F] %s        %d", info.name, info.size);
    else if (info.type == LFS_TYPE_DIR)
        TRACE(0, "    [D] %s        %d", info.name, info.size);
    else
        TRACE(0, "    [E] %s        %d", info.name, info.size);
  }
  lfs_file_ops_closedir(dir);
  return 0;
}
#endif //defined(LITTLEFS_PARTITION_SUPPORT)

#endif //defined(LFS_FLASH_TOTAL_SIZE) && defined(LFS_FLASH_BASE_ADDR)

#ifdef MCU_TFLM_ENABLE
extern int tf_main(int argc, char** argv);

static int do_tflm(cmd_tbl_t *cmd, int argc, char *argv[], void *handler) {
	uint32_t start_ticks = hal_sys_timer_get();

	tf_main(0,0);
	uint32_t endtick = hal_sys_timer_get();
	uint32_t costtime = TICKS_TO_MS(endtick - start_ticks);
	
	printf("%s %d start:%u, end:%u costtime:%ums freq is %d\n", __FUNCTION__, __LINE__, start_ticks, endtick, costtime, hal_cmu_sys_get_freq());


return 0;
}

#endif

static const cmd_tbl_t at_cmd_list[] = {
        {
            "AT+MD",           8,  do_mem_read,
            "AT+MD             - read mem or reg\n"
        },
        /*WIFI CMD*/
        {
            "AT+WSCONN",        7,   do_conn,
            "AT+WSCONN          - start wifi connect\n"
        },

#if LWIP_SUPPORT
        /*NET CMD*/
        {
            "AT+WSSETIP",       6,   do_set_static_ip,
            "AT+WSSETIP         - set static ip addr\n"
        },
#endif
#if defined(__NET_FTP_CLIENT_SUPPORT__) && defined(__A7_DSP_ENABLE__)
        {
            "AT+RECDUMP",    10,   do_record_to_ftp,
            "AT+RECDUMP       - mic record data to ftp server\n"
        },
#endif

#if !defined(__BT_CLOSE__)
        /*{
            "AT+GETBTRSSI",   1,   get_bt_rssi_value,
            "AT+GETBTRSSI        get bt rssi \n"
        },*/
        {
            "AT+BTMAC",    2,   at_bt_mac,
            "AT+BTMAC       - get/set bt mac address from/to factory\n"
        },
        {
            "AT+BTNAME", 2, at_bt_name,
            "AT+BTNAME=?    - get/set bt name from/to factory\n"
        },
        {
            "AT+BLEMAC", 2, at_ble_mac,
            "AT+BLEMAC      - get/set ble mac address from/to factory\n"
        },
#endif


#ifdef BT_MEDIA_SUPPORT_LPLAYER
        {
            "AT+LPLAYER_START", 3, do_lplayer_start,
            "AT+LPLAYER_START        - lplayer create and start. the arg could be a url or local file path.\n"
        },
        {
            "AT+LPLAYER_STOP", 2, do_lplayer_stop,
            "AT+LPLAYER_STOP         - lplayer stop and destory.\n"
        },
        {
            "AT+LPLAYER_PAUSE", 2, do_lplayer_pause,
            "AT+LPLAYER_PAUSE        - lplayer pause, resume could use AT+LPLAYER_START.\n"
        },
        {
            "AT+LPLAYER_VOL", 2, do_lplayer_set_vol,
            "AT+LPLAYER_VOL      - lplayer set vol. [0,16]\n"
        },
        {
            "AT+LPLAYER_SEEK", 2, do_lplayer_seek,
            "AT+LPLAYER_SEEK         - lplayer seek to specific position. unit: s\n"
        },
        {
            "AT+LPLAYER_PLAY_TIME", 2, do_lplayer_show_current_play_time,
            "AT+LPLAYER_PLAY_TIME    - show current program play time.\n"
        },
        // {
        //     "AT+LPLAYER_SHOW_MEM", 2, do_lplayer_show_memory_usage,
        //     "AT+LPLAYER_SHOW_MEM     - show current os memory usage.\n"
        // },
#endif


#if !defined(__BT_CLOSE__)
        {
            "AT+VOLUP", 3, do_volumeup,
            "AT+VOLUP,     - volume up \n"
        },
#endif


#ifdef MEDIA_PLAYER_SUPPORT
        {
            "AT+PROMPT", 2, at_prompt_play,
            "AT+PROMPT[=<aud_id>]       - Play prompt audio (0 default). \n"
        },
#endif
#if defined(LFS_FLASH_TOTAL_SIZE) && defined(LFS_FLASH_BASE_ADDR)
        {
            "AT+FSLIST", 3, at_fs_list,
            "AT+FSLIST[=<path>]       - show file list (\"/\" default). \n"
        },
#endif
#ifdef MCU_TFLM_ENABLE
		{
            "AT+TFLM", 3, do_tflm,
            "AT+TFLM      - tensorflow lite micro test\n"
        }
#endif
    };


static void add_cmd(const cmd_tbl_t *tbl, int size)
{
    for (int i = 0; i < size; i ++) {
        if (0 != console_cmd_add(&tbl[i])) {
            AT_PRINT("Add cmd failed\r\n");
        }
    }
}

extern const cmd_tbl_t at_cmd_list_factory[];
extern const int at_cmd_list_factory_size;
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(ar)  sizeof(ar)/sizeof(ar[0])
#endif
void besfd_at_init(void)
{
    add_cmd(at_cmd_list, ARRAY_SIZE(at_cmd_list));
    add_cmd(at_cmd_list_factory, at_cmd_list_factory_size);
}

