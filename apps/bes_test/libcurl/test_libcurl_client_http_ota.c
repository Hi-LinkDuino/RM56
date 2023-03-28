/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined(__NET_LIBCURL__) && defined(__NET_MBEDTLS__)
#include "curl/curl.h"
#endif
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_norflash.h"
#include "string.h"
#include "flash.h"

/* FIXME: step need to verify and recorrect */
/*               OTA DEMO
 *
 *  execute aiot_ota_test()
 *
 *  before run ota case,prepare enviroment as following steps:
 *
 *  1. in config/aiot_test_2000iwe/target.mk
 *     open MACRO(NET_LIBCURL_SUPPORT BES_OTA_ENABLED)
 *     close MACRO:(BES2300W_MASTER NET_LIBRWS_SUPPORT NET_MQTT_SUPPORT NET_COAP_SUPPORT
 *                  NET_JANSSON_SUPPORT NET_CJSON_SUPPORT NET_JSONC_SUPPORT OPUS_CODEC HTTPS_URL_SUPPORT)
 *     disable other test,make bin smaller.
 *     open ONLY_TEST_OTA in current file
 *
 *  2. you need a server in wireless network,test case can get ota bin from server (if not, use BES's dedault url,we will open server)
 *
 *  3. build bootloader (cmd: make T=wifi_bootloader GEN_LIB=1)
 *     burn to bootloader section  (cmd: dldtool.exe 6 programmer2000.bin wifi_bootloader.bin)
 *
 *  4. build sdk bin (cmd: make T=aiot_test_2000iwe GEN_LIB=1)
 *     burn to section zoneA (cmd: dldtool.exe 6 programmer2000.bin aiot_test_2000iwe.bin -m 0x10000)
 *
 *  5. execute aiot_ota_test(),device will reboot from zoneB after ota done(bin is load to zoneB)
 *
 *  6. if you want run other test,run at cmd:AT+OTA=2,0 ,then reset device,device will reboot from zoneA
 */

#include "kfifo.h"
#ifdef BES_OTA_ENABLED
#include "ota_handler.h"
#endif
#define BES_OTA_ENABLED
#define HTTP_OTA_URL        "http://192.168.0.111/test/best2600w_aiot.bin"
#if defined(BES_OTA_ENABLED) && defined(__NET_LIBCURL__)
//#include "http_media.h"
extern void hal_cmu_sys_reboot(void);


#define OTA_FIFO_SIZE (32*1024)
static struct kfifo ota_fifo;
static unsigned char ota_fifo_buffer[OTA_FIFO_SIZE];
#define FLASH_BLOCK_SIZE         (20*1024)
static size_t ota_bin_len = 0;

static char* bes_trim(char *str)
{
    if (str == NULL) return NULL;

    char* p = str;
    char* p1 = p + strlen(str) - 1;

    while (*p && isspace((int)*p)) {
        p++;
    }
    while (p1 > p && isspace((int)*p1)) {
        *p1-- = 0;
    }
    return p;
}

static size_t rcv_ota_hdata(void *buffer, size_t size, size_t nitems, void *outstream)
{
    //TRACE(0, "%s",buffer);
    char *key = strtok((char *)buffer, ":");
    if (!strncmp(key,"Content-Length",strlen("Content-Length")))
    {
        ota_bin_len = atoi(bes_trim(strtok(NULL,":")));
    }
    return nitems;
}

static size_t total_size = 0;
static size_t write_data_to_ota_thread(void *ptr, size_t size)
{
    unsigned int sendlen = 0;
    while (1)
    {
        if (size > (int)kfifo_available(&ota_fifo))
        {
            osDelay(10);
            continue;
        }
        sendlen = kfifo_put(&ota_fifo, (unsigned char*)ptr, size);
        break;
    }

    total_size += sendlen;
    //TRACE(0, "s total_size  = %d ",__func__,total_size);
    osDelay(1);
    return sendlen;
}

static size_t rcv_ota_bdata(void *buffer, size_t size, size_t nitems, void *outstream)
{
    // todo: write to flash
    write_data_to_ota_thread(buffer,nitems);
    return nitems;

}

static int libcurl_ota_request(char* http_ota_url)
{
    TRACE(0, "%s begin", __func__);
    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != return_code)
    {
        TRACE(0, "curl_global_init() failed: %s ", curl_easy_strerror(return_code));
        return -1;
    }

    CURL *easy_handle = curl_easy_init();
    if (NULL == easy_handle)
    {
        curl_global_cleanup();
        return -1;
    }

    char * stream = NULL;
    curl_easy_setopt(easy_handle,CURLOPT_URL,http_ota_url);
    curl_easy_setopt(easy_handle, CURLOPT_HTTPGET, 1L);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Connection: close");
    curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(easy_handle,CURLOPT_FORBID_REUSE,1L);
    curl_easy_setopt(easy_handle, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, rcv_ota_hdata);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, rcv_ota_bdata);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stream);
    return_code = curl_easy_perform(easy_handle);
    if (NULL != headers)
    {
        curl_slist_free_all(headers);
    }

    if (CURLE_OK != return_code)
    {
        TRACE(0, "curl_easy_perform() failed: %s ", curl_easy_strerror(return_code));
        curl_easy_cleanup(easy_handle);
        curl_global_cleanup();
        return -1;
    }
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
    TRACE(0, "%s done", __func__);
    return 0;

}
extern int ota_upgrade_slice(const char *image_name,       uint32_t offset, uint32_t slice_size, uint32_t is_last_slice, uint8_t *buffer);
extern int ota_upgrade_end(void);
static void ota_thd_proc(void *param)
{
    printf("4TH %s %d", __func__, __LINE__);osDelay(10);
    //OTA_ZONE_NUM ota_zone = *((OTA_ZONE_NUM *)param);
    //ota_erase_partition(ota_zone, 0);
    //TRACE(0, "s %d, ota_erase_partition ota_zone %d finish.", __func__, __LINE__,ota_zone);
    hal_logic_partition_t partition_info;
	static uint8_t ota_buf[FLASH_BLOCK_SIZE] = {0};

    uint32_t offset = 0;
    int ret = 0;
	uint32_t is_last = 0;

	ret = hal_flash_info_get(HAL_PARTITION_RTOSA, &partition_info);
    if (ret) {
        return ERR_RETURN;
    }
	printf("%s %d", __func__, __LINE__);osDelay(10);
    while (1)
    {
        size_t read_len = kfifo_get(&ota_fifo, ota_buf, sizeof(ota_buf));
        if (read_len == 0)
        {
            osDelay(10);
            continue;
        }
		//printf("%s %d", __func__, __LINE__);osDelay(10);
		if ( (offset+read_len) == ota_bin_len )
			is_last = 1;
		ret = ota_upgrade_slice("ota_rtos.bin", offset, read_len, is_last, ota_buf);
        //ret = ota_write_partition(ota_zone, offset, ota_buf, read_len);
        if (ret)
        {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            break;
        }
		//printf("%s %d", __func__, __LINE__);osDelay(10);
        offset += read_len;
        if (offset == ota_bin_len && ota_bin_len != 0)
        {
            TRACE(0, "%s %d, flash write bin finish!", __func__, __LINE__);
            //ota_set_boot_partition(ota_zone);
            ota_upgrade_end();
            break;
        }
		//printf("%s %d", __func__, __LINE__);
        osDelay(1);
    }
    if (!ret)
    {
        hal_cmu_sys_reboot();
        while (1) {}
    }
}

#define PTHREAD_STACK_SIZE	50*1024
osThreadDef(ota_thd_proc, osPriorityAboveNormal, 1, PTHREAD_STACK_SIZE, "ota_thread");
#endif /* BES_OTA_ENABLED */
int bes_curl_http_ota_test(void)
{
    int ret = -1;
#if defined(BES_OTA_ENABLED) && defined(__NET_LIBCURL__)
    //OTA_ZONE_NUM flash_zone = OTA_ZONE_B;
    kfifo_init(&ota_fifo, ota_fifo_buffer, OTA_FIFO_SIZE);

    // thread for ota operation
    if (osThreadCreate(osThread(ota_thd_proc), NULL) == NULL)
    {
        TRACE(0, "Failed to Create http_media_thread");
        return -1;
    }

    // request http bin file url is yangjian's http serser
    ret = libcurl_ota_request(HTTP_OTA_URL);
#endif /* BES_OTA_ENABLED */
    return ret;
}
