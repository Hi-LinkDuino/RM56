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
#ifndef __BES_TEST_CASES__
#define __BES_TEST_CASES__
#include <stdbool.h>

typedef int (*APP_CASE_HANDLER_CB_T)(void);

typedef struct
{
    char *                  FUNC_NAME;
    APP_CASE_HANDLER_CB_T   CALLBACK;
    bool                    DEDAULT_ENABLE;
    void *                  PARM;
} BES_CASE_HANDLER_T;

extern unsigned int  __test_cases_table_start[];
extern unsigned int  __test_cases_table_end[];

#define CASES_HANDLER_TABLE(id_name)    \
    static const BES_CASE_HANDLER_T id_name##_ENTRY[]  __attribute__((used, section(".test_cases_table"))) =


#define CASE_HANDLER_TYPE_PTR_FROM_ENTRY_INDEX(index)   \
    ((BES_CASE_HANDLER_T *)((unsigned int)__test_cases_table_start + (index)*sizeof(BES_CASE_HANDLER_T)))

#define FUNC_ADD(func) extern int func(void);
/* list of test cases,must less than SUPPORT_MAX_CASE_NUM */
FUNC_ADD(bes_curl_post_test)
FUNC_ADD(bes_curl_https_redirect_test)
FUNC_ADD(bes_softap_test)
FUNC_ADD(bes_curl_http_ota_test)
FUNC_ADD(bes_websocket_test)
FUNC_ADD(bes_audio_dec_opus_test)
FUNC_ADD(bes_ntp_test)
FUNC_ADD(bes_sniffer_test)
FUNC_ADD(bes_ftp_client_dump_test)
#if 0
/* obsolete, instead by lplayer */
FUNC_ADD(bes_audio_dec_mp3_test)
FUNC_ADD(bes_audio_dec_AAC_test)
FUNC_ADD(bes_audio_enc_opus_test)
FUNC_ADD(bes_voice_test)
FUNC_ADD(bes_wifi_audio_http_test)
FUNC_ADD(bes_wifi_audio_https_test)
#endif
FUNC_ADD(bes_lplayer_test)
FUNC_ADD(bes_mbestls_ssl_test)
FUNC_ADD(bes_curl_https_test)
FUNC_ADD(bes_posix_test)
FUNC_ADD(bes_key_test)
FUNC_ADD(bes_spi_clk_test)
FUNC_ADD(bes_littlefs_test)
FUNC_ADD(bes_at_cmd_test)
FUNC_ADD(bes_a7_cmd_test)
FUNC_ADD(bes_m33_cmd_test)
FUNC_ADD(bes_cmsis_test)
FUNC_ADD(bes_pcm_test)
FUNC_ADD(bes_resample_test)
FUNC_ADD(bes_mbedtls_aes_test)
FUNC_ADD(bes_wifi_func_test)
FUNC_ADD(bes_heap_test)
FUNC_ADD(bes_peripheral_modules_test)
FUNC_ADD(bes_cmsis_os2_test_supplement)
#ifdef SPI_AUDIO_TEST
FUNC_ADD(bes_spi_audio_test)
#endif
void test_case_event_handle(int redo, int casedo);
#endif /* __BES_TEST_CASES__ */
