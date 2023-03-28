/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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

#include "cmsis.h"
#include "hal_trace.h"
#include "cmsis_os.h"
#include "bwifi_interface.h"
#ifndef NUTTX_BUILD
#include "console.h"
#endif
#include "hal_timer.h"
#include "hal_sysfreq.h"
#include "hal_bootmode.h"
#ifdef __NET_MBEDTLS__
#include "threading_alt.h"
#endif
#include "wifi_def.h"

#if FORMULA_POWERLEVEL_OFFSET
extern int wifi_set_tpc_table_to_rf(void);
#endif

#ifdef __A7_DSP_ENABLE__
extern void pmu_vcore_set_normal_volt();
extern void a7_dsp_boot(void);
#ifdef MCU_SLAVE_FS_EN
extern void load_fs_bin(void);
#endif
#endif

#ifdef CP_BOOT
extern void cp_boot(void);
#endif

void start_at(void);

#ifdef __A7_DSP_ENABLE__
static int a7_boot_flag = 0;

extern int get_a7_boot_flag(void)
{
    return a7_boot_flag;
}

extern int set_a7_boot_flag(void)
{
    a7_boot_flag = 1;
    return 0;
}

#ifdef CHIP_BEST2001
void vcore_recover_main(const void *argument)
{
    osDelay(20*1000);
    pmu_vcore_set_normal_volt();
    osThreadExit();
}
osThreadDef(vcore_recover_main, osPriorityNormal, 1, 4*1024, "apps_recover");
#endif

void a7_vcore_recover_main_thread_init(void)
{
#ifdef CHIP_BEST2001
    osThreadCreate(osThread(vcore_recover_main), NULL);
#endif
}
#endif

int bes_wifi_init(void)
{
    int ret = 0;
    static uint8_t bes_wifi_init_done = 0;

    if (bes_wifi_init_done)
        return ret;

    /* reset sys freq for wifi user */
    hal_sysfreq_req(HAL_SYSFREQ_USER_WIFI, HAL_CMU_FREQ_390M);
    TRACE(1, "%s CPU freq: %u", __func__, hal_sys_timer_calc_cpu_freq(5, 0));

#if defined(CP_BOOT) && defined(WIFI_ON_CP)
    if (((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI) == 0)) {
        cp_boot();
    }
#endif

#ifdef FORMULA_POWERLEVEL_OFFSET
    wifi_set_tpc_table_to_rf();
#endif

    if((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI) == 0) {
        hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_208M);
        ret = bwifi_init();
        hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_390M);
        if (ret) {
            TRACE(0, "bwifi init fail\n");
            return ret;
        }
    }

#if defined(__AT_CMD_SUPPORT__) && !defined(MAIN_CPU_SLEEP_TEST) && !defined(DEBUG_BY_UART) && !defined(NUTTX_BUILD)
    console_init();

#ifdef __RF_TRACE_BY_SPRINTF__
    start_at();
#else
    if((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI) == 0) {
        start_at();
    }
#endif

{
    extern void besfd_at_init(void);
    besfd_at_init();
}

#endif

#ifdef RB_CODEC
    fs_platform_init();
#endif

#if defined(MBEDTLS_THREADING_ALT) && defined(__NET_MBEDTLS__)
    mbedtls_threading_set_alt(mbedtlsThreading_MutexInit,mbedtlsThreading_MutexFree
            ,mbedtlsThreading_MutexLock,mbedtlsThreading_MutexUnlock);
#endif

#if defined(__A7_DSP_ENABLE__) && !defined(CONFIG_BES_RPTUN_DELAY_BOOT)
#ifdef MCU_SLAVE_FS_EN
    load_fs_bin();
#endif
    if ((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI) == 0) {
        a7_dsp_boot();
#ifdef CHIP_BEST2001
        osThreadCreate(osThread(vcore_recover_main), NULL);
#endif
        set_a7_boot_flag();
    }
#endif

    /* recover sys freq for wifi user */
    hal_sysfreq_req(HAL_SYSFREQ_USER_WIFI, HAL_CMU_FREQ_390M);
    hal_sysfreq_req(HAL_SYSFREQ_USER_INIT, HAL_CMU_FREQ_32K);
    bes_wifi_init_done = 1;
    return ret;
}

int bes_wifi_connect(const char *ssid, const char *passwd, unsigned char *bssid)
{
    int ret;

    ret = bwifi_connect_to_ssid(ssid, passwd, 0, 0, bssid);
    if (ret) {
        TRACE(0, "bwifi connect to %s fail:%d\n", ssid, ret);
        return -1;
    }
    TRACE(0, "bwifi connect to %s succeed\n", ssid);
    return 0;
}

#ifdef __WIFI_NVR_SUPPORT__
int wifi_app_volume_update(int volume)
{
    nvrec_wifidevicerecord record;

    if (bwifi_find_record(BWIFI_RECORD_TYPE_VOLUME,&record))
        memset(&record, 0, sizeof(record));

    record.wifi_record.volume.hfp_vol = volume;
    memcpy(record.type, BWIFI_RECORD_TYPE_VOLUME, sizeof(BWIFI_RECORD_TYPE_VOLUME));
    if (!bwifi_add_record(&record))
    {
        TRACE(0, "%s a2dp_vol:%d hfp_vol:%d", __func__,
        record.wifi_record.volume.a2dp_vol, record.wifi_record.volume.hfp_vol);
        return 0;
    }
    return -1;
}
#endif
