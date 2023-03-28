/**
 * Copyright (2019) BesTech. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "app_utils.h"
#include "bes_test/bes_test_cases.h"
#include "beswifi.h"
#include "bwifi_interface.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "net_memory.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef LVGL_TEST
#include "lv_app.h"
#endif
#ifdef USB_STACK
#include "usb_hcd.h"
#endif
#ifdef CSI_REPORT
#include "csi_reporter.h"
#endif

#ifdef __AIRPLAY_SUPPORT_TEST__
#include "airplay_test.h"
#endif

#ifdef __VOW_MICPHONE__
#include "vow_mic_spk.h"
#endif

#ifdef __DLNA_SUPPORT_TEST__
extern void dlna_test_entry();
#endif

#ifdef LIBC_ADAPT_FS_EN
extern int mount(const char *source, const char *target,
          const char *filesystemtype, unsigned long mountflags,
          const void *data);
#endif

int lv_main(void);

void master_main(const void *argument)
{
    int ret;
    TRACE(0, "enter %s %d ", __func__, __LINE__);
    app_sysfreq_req(APP_SYSFREQ_USER_APP_15, APP_SYSFREQ_390M);
    TRACE(0, "sys freq calc: %d", hal_sys_timer_calc_cpu_freq(5, 0));

#ifdef CSI_REPORT
#define MAIN_SSID "bes_test"
#define MAIN_PASSWD "12345678"
    if (bwifi_count_configured_ssid() == 0)	{
        TRACE(0, "bwifi_connect_to_ssid ----------->");
        ret = bwifi_connect_to_ssid((const char *)MAIN_SSID, (const char *)MAIN_PASSWD, 0, 0, NULL);
    } else {
        TRACE(0, "bwifi_auto_connect ----------->");
        ret = bwifi_auto_connect();
    }
    if (ret) {
        TRACE(0, "bwifi connect fail\n");
    }
    csi_reporter_task();
#endif

#ifdef __DLNA_SUPPORT_TEST__
    dlna_test_entry();
#endif

#ifdef __AIRPLAY_SUPPORT_TEST__
    airplay_with_bis_entry();
#endif

#ifdef LIBC_ADAPT_FS_EN
    TRACE(0, "%s ===========<mount fs>============", __func__);
    ret = mount(NULL, NULL, NULL, 0, NULL);
    if (ret < 0) {
        TRACE(0, "%s mount failed", __func__);
    }
#endif

#ifdef __VOW_MICPHONE__
    wadc_app_start();
#endif

#ifdef BES_TEST
    test_case_event_handle(0, 0);  /* start up bes test case */
#endif

    /* TODO: startup your app */
#ifdef LVGL_TEST
    lv_main();
#endif



#ifdef USB_STACK
    usb_hcd_register();
#endif

    while (1) {
        osDelay(1000);
    }

    return;
}

osThreadId master_main_thread_id;
osThreadDef(master_main, osPriorityAboveNormal, 1, (64 * 1024), "master_main");

#if defined(CHIP_HAS_EXT_NORFLASH) && defined(TEST_FOR_EXT_NORFLASH)
#include "hal_norflash.h"
#endif
void user_app_init(void)
{
    TRACE(0, "enter %s %d", __func__, __LINE__);
#if defined(CHIP_HAS_EXT_NORFLASH) && defined(TEST_FOR_EXT_NORFLASH)
    enum HAL_NORFLASH_RET_T ret = hal_norflash2_init();
    TRACE(0, "%s hal_norflash2_init return: %d", __func__, ret);
#endif
    master_main_thread_id = osThreadCreate(osThread(master_main), NULL);
    if (master_main_thread_id == NULL) {
        TRACE(0, "%s %d fail to create master_main task", __func__, __LINE__);
        return;
    }
    TRACE(0, "create master_main task = 0x%X", (uint32_t)master_main_thread_id);

    return;
}
