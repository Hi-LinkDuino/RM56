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
#include <stddef.h>
#include <stdio.h>
#include "bes_test_cases.h"
#include "net_memory.h"
#include "hal_timer.h"
#include "cmsis_os.h"

WEAK void os_heap_stat_print(void) {};

CASES_HANDLER_TABLE(BES_CASES)
{
    { "bes_at_cmd_test",                    bes_at_cmd_test,                   true,        NULL },//0 ok
    { "bes_spi_clk_test",                   bes_spi_clk_test,                  false,        NULL },//1 to test, need hw ready
    { "bes_key_test",                       bes_key_test,                      false,        NULL },//2 ok
    { "bes_ntp_test",                       bes_ntp_test,                      false,        "need wifi connected" },//3 ok
    { "bes_websocket_test",                 bes_websocket_test,                false,        NULL },//4 to test, need libwrt.a
    { "bes_ftp_client_dump_test",           bes_ftp_client_dump_test,          false,        NULL },//5 ok
    { "bes_sniffer_test",                   bes_sniffer_test,                  false,        NULL },//6 nok, wait bes's patch
    { "bes_pcm_test",                       bes_pcm_test,                      false,        NULL },//7 ok
    { "bes_lplayer_test",                   bes_lplayer_test,                  false,        NULL },//8 ok
    { "bes_curl_https_test",                bes_curl_https_test,               false,        NULL },//9 nok, Cert verify failed: BADCERT_NOT_TRUSTED
    { "bes_curl_post_test",                 bes_curl_post_test,                false,        NULL },//10 ok
    { "bes_curl_https_redirect_test",       bes_curl_https_redirect_test,      false,        NULL },//11 to teset, ca_path need be pre-built 
    { "bes_softap_test",                    bes_softap_test,                   false,        NULL },//12 ok
    { "bes_curl_http_ota_test",             bes_curl_http_ota_test,            false,        NULL },//13 not used, need update new OTA interface
    { "bes_mbestls_ssl_test",               bes_mbestls_ssl_test,              false,        NULL },//14 ok
    { "bes_littlefs_test",                  bes_littlefs_test,                 false,        NULL },//15 ok
    { "bes_a7_cmd_test",                    bes_a7_cmd_test,                   false,        NULL },//16 ok
    { "bes_m33_cmd_test",                   bes_m33_cmd_test,                  false,        NULL },//17 ok, 16&17 toghter run
    { "bes_cmsis_test",                     bes_cmsis_test,                    false,        NULL },//18 ok
    { "bes_resample_test",                  bes_resample_test,                 false,        NULL },//19 ok, default close for saving dwl time	
    { "bes_mbedtls_aes_test",               bes_mbedtls_aes_test,              false,        NULL },//20 ok
    { "bes_wifi_func_test",                 bes_wifi_func_test,                false,        NULL },//21 bwifi_config_scan() has no effect, other ok
    { "bes_heap_test",                      bes_heap_test,                     false,        NULL},//22 ok
    { "bes_peripheral_modules_test",        bes_peripheral_modules_test,       false,        NULL},//23 to test, need hw ready
    { "bes_cmsis_test_supplement",          bes_cmsis_os2_test_supplement,     false,        NULL},//24 ok
#ifdef SPI_AUDIO_TEST
    { "bes_spi_audio",                      bes_spi_audio_test,                true,        NULL},//25 to test, need hw ready
#endif
};


#define TEST_RESULT_BUF     2048
typedef struct
{
    int suc_times;
    int used_time;
    int total_times;
}CASE_RUN_INFO_T;

void test_case_event_handle(int redo, int casedo)
{

    /* core callback func */
    unsigned int total_case_num = ((unsigned int)__test_cases_table_end-(unsigned int)__test_cases_table_start) / sizeof(BES_CASE_HANDLER_T);
    CASE_RUN_INFO_T CASE_INFO[total_case_num];
	if (casedo >=total_case_num) {
		TRACE(0, "max test case is %d, your input is %d\r\n", total_case_num, casedo);
		return;
	}
		
    for ( unsigned int index = 0;index < total_case_num; index++)
    {
        CASE_INFO[index].suc_times = 0;
        CASE_INFO[index].total_times = 0;
    }

    int _rt_heap_size = 0;
    int _freeRTOS_heap_size = 0;
    do
    {
        _rt_heap_size = nts_get_free_heap_size();
#ifdef osCMSIS_FreeRTOS
        _freeRTOS_heap_size = xPortGetFreeHeapSize();
#endif /* osCMSIS_FreeRTOS */

        char test_result[TEST_RESULT_BUF] = {0};
        int len = TEST_RESULT_BUF;
        len -= snprintf(
                &test_result[TEST_RESULT_BUF-len],
                len,
                "%-32s%-16s%-16s%-16s%-16s\r\n","test item","total number","success number ","success rate(%)","Used Time(ms)"
            );

        for ( uint32_t index = 0;index < total_case_num; index++)
        {
        	//if casedo >= 0, only run casedo;
        	//else run default case(s);
        	if (casedo >= 0) 
				index = casedo;
            else if (!CASE_HANDLER_TYPE_PTR_FROM_ENTRY_INDEX(index)->DEDAULT_ENABLE)
            {
                continue;
            }
            char * name = CASE_HANDLER_TYPE_PTR_FROM_ENTRY_INDEX(index)->FUNC_NAME;
            TRACE(0, "******************************* %s %d *******************************",name,CASE_INFO[index].total_times+1);
            if (CASE_HANDLER_TYPE_PTR_FROM_ENTRY_INDEX(index)->PARM != NULL)
            {
                TRACE(0, "\r\n************[%s] REQUIRE: %s**************\r\n",name,(char *)(CASE_HANDLER_TYPE_PTR_FROM_ENTRY_INDEX(index)->PARM));
            }
            /* time calculate */
            int begin_time = TICKS_TO_MS(hal_sys_timer_get());

            int ret = CASE_HANDLER_TYPE_PTR_FROM_ENTRY_INDEX(index)->CALLBACK();

            int end_time = TICKS_TO_MS(hal_sys_timer_get());

            CASE_INFO[index].used_time = end_time - begin_time;
            CASE_INFO[index].total_times++;

            if (!ret)
            {
                CASE_INFO[index].suc_times++;
            }

            len -= snprintf(
                    &test_result[TEST_RESULT_BUF-len],
                    len,
                    "\t\t %-32s%-16d%-16d%-16d%-16d\n",
                    name,
                    total_case_num,
                    CASE_INFO[index].suc_times,
                    CASE_INFO[index].suc_times*100/CASE_INFO[index].total_times,
                    CASE_INFO[index].used_time
                );
            TRACE(0, "******************************* %s %d end *******************************",name,CASE_INFO[index].total_times);
            osDelay(1000);
			if (casedo >= 0) break;
        }
        TRACE(0, "******************************************************************************");
        TRACE(0, "*******************************case_test result*******************************");
        TRACE(0, "******************************************************************************");
        TRACE(0, "%s\n", test_result);
        TRACE(0, "rt free memory, start size:%d, now size:%d, leakage %d\r\n", _rt_heap_size, nts_get_free_heap_size(),
                    (nts_get_free_heap_size() < _rt_heap_size)?(_rt_heap_size - nts_get_free_heap_size()):0);
    #ifdef osCMSIS_FreeRTOS
        TRACE(0, "freeRTOS free heap, start size:%d, now size:%d, leakage:%d\r\n", _freeRTOS_heap_size, xPortGetFreeHeapSize(),
                    (xPortGetFreeHeapSize() < _freeRTOS_heap_size)?(_freeRTOS_heap_size - xPortGetFreeHeapSize()):0);
    #else
        os_heap_stat_print();
    #endif /* osCMSIS_FreeRTOS */
        TRACE(0, "******************************case_test result end*****************************");
        osDelay(5000);
    }while(redo);
}
