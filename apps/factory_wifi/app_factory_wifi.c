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
#include "hal_cmu.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_trace.h"
#include "hal_iomux.h"
#include "hal_bootmode.h"
#include "hal_norflash.h"
#include "hal_sleep.h"
#include "hal_sysfreq.h"
#include "cmsis.h"
#include "hwtimer_list.h"
#include "pmu.h"
#include "analog.h"
#include "rf_cmd_command.h"

#ifdef RTOS
#include "cmsis_os.h"
#ifdef KERNEL_RTX
#include "rt_Time.h"
#endif
#endif

#ifdef HWTEST
#include "hwtest.h"
#ifdef VD_TEST
#include "voice_detector.h"
#endif
#endif

#ifdef __cplusplus
#define EXTERN_C                        extern "C"
#else
#define EXTERN_C                        extern
#endif

#define TIMER_IRQ_PERIOD_MS             2500
#define DELAY_PERIOD_MS                 4000

#ifndef FLASH_FILL
#define FLASH_FILL                      1
#endif

#ifdef KERNEL_RTX
#define OS_TIME_STR                     "[%2u/%u]"
#define OS_CUR_TIME                     , SysTick->VAL, os_time
#else
#define OS_TIME_STR
#define OS_CUR_TIME
#endif

#if defined(MS_TIME)
#define TIME_STR                        "[%u]" OS_TIME_STR
#define CUR_TIME                        TICKS_TO_MS(hal_sys_timer_get())  OS_CUR_TIME
#elif defined(RAW_TIME)
#define TIME_STR                        "[0x%X]" OS_TIME_STR
#define CUR_TIME                        hal_sys_timer_get()  OS_CUR_TIME
#else
#define TIME_STR                        "[%u/0x%X]" OS_TIME_STR
#define CUR_TIME                        TICKS_TO_MS(hal_sys_timer_get()), hal_sys_timer_get() OS_CUR_TIME
#endif

const static unsigned char bytes[FLASH_FILL] = { 0x1, };

// GDB can set a breakpoint on the main function only if it is
// declared as below, when linking with STD libraries.
extern int trace_enable_flag;
EXTERN_C void wifi_nosignaling_test(uint8_t band);

void app_factory_enter_wifi_nosignaltest_mode(void)
{
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_TEST_MODE | HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI);
}

void app_factory_exit_wifi_nosignaltest_mode(void)
{
    //TODO
}

void app_factorymode_wifi_nosignalingtest(void const *arg)
{
    int POSSIBLY_UNUSED ret;
    uint8_t band = 0;
    uint32_t val = 0;

#ifdef __DUAL_BAND__
    band = 2;
#else
    band = 0;
#endif

    TR_INFO(TR_MOD(MAIN), "test band =%d",band);
    wifi_nosignaling_test(band);

    TRACE(0,"exit wifi rf test ");

    while(1){
        osDelay(1000);
    }

    SAFE_PROGRAM_STOP();
}
uint32_t delay_time = 10000;
void app_wifi_detail_test(void const *arg)
{
    uint32_t wifi_test_loop = 1, test_mode = 0, test_channel = 1;
    while(1)
    {

        if(test_mode == 0)
        {
            TR_INFO(TR_MOD(MAIN), "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test Loop %d $$$$$", wifi_test_loop++);
            TR_INFO(TR_MOD(MAIN), "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
        }
#if 0
        //2.4G channel 1, Max tx power
        if(test_mode == 0)
        {
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0x3FFF);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Max TX Power (0x3FFF) Test with Channel %d $$$$$", test_channel);
            test_mode = 1;
            osDelay(delay_time);
        }
        else if (test_mode == 1)//2.4G channel 1, Middle tx power
        {
            wifi_rf_test_calibrate_txPower(0x1800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Middle TX Power (0x1800) Test with Channel %d $$$$$", test_channel);
            test_mode = 2;
            osDelay(delay_time);
        }
        else if (test_mode == 2)//2.4G channel 1, Min tx power
        {
            wifi_rf_test_calibrate_txPower(0x800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Min TX Power (0x800) Test with Channel %d $$$$$", test_channel);
            test_mode = 3;
            osDelay(delay_time);
        }
        else if (test_mode == 3)//2.4G channel 7, Max tx power
        {
            test_channel = 7;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0x3FFF);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Max TX Power (0x3FFF) Test with Channel %d $$$$$", test_channel);
            test_mode = 4;
            osDelay(delay_time);
        }
        else if (test_mode == 4)//2.4G channel 7, Middle tx power
        {
            wifi_rf_test_calibrate_txPower(0x1800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Middle TX Power (0x1800) Test with Channel %d $$$$$", test_channel);
            test_mode = 5;
            osDelay(delay_time);
        }
        else if (test_mode == 5)//2.4G channel 7, Min tx power
        {
            wifi_rf_test_calibrate_txPower(0x800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Min TX Power (0x800) Test with Channel %d $$$$$", test_channel);
            test_mode = 6;
            osDelay(delay_time);
        }
        else if (test_mode == 6)//2.4G channel 13, Max tx power
        {
            test_channel = 13;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0x3FFF);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Max TX Power (0x3FFF) Test with Channel %d $$$$$", test_channel);
            test_mode = 7;
            osDelay(delay_time);
        }
        else if (test_mode == 7)//2.4G channel 13, Middle tx power
        {
            wifi_rf_test_calibrate_txPower(0x1800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Middle TX Power (0x1800) Test with Channel %d $$$$$", test_channel);
            test_mode = 8;
            osDelay(delay_time);
        }
        else if (test_mode == 8)//2.4G channel 13, Min tx power
        {
            wifi_rf_test_calibrate_txPower(0x800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G Min TX Power (0x800) Test with Channel %d $$$$$", test_channel);
            test_mode = 10;
            osDelay(delay_time);
        }
        //// start test WIFI 5G/////
        else if(test_mode == 10)
        {
            test_channel = 36;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0x3FFF);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Max TX Power (0x3FFF) Test with Channel %d $$$$$", test_channel);
            test_mode = 11;
            osDelay(delay_time);
        }
        else if (test_mode == 11)//5G channel 36, Middle tx power
        {
            wifi_rf_test_calibrate_txPower(0x1800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Middle TX Power (0x1800) Test with Channel %d $$$$$", test_channel);
            test_mode = 12;
            osDelay(delay_time);
        }
        else if (test_mode == 12)//5G channel 36, Min tx power
        {
            wifi_rf_test_calibrate_txPower(0x800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Min TX Power (0x800) Test with Channel %d $$$$$", test_channel);
            test_mode = 13;
            osDelay(delay_time);
        }
        else if (test_mode == 13)//5G channel 100, Max tx power
        {
            test_channel = 100;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0x3FFF);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Max TX Power (0x3FFF) Test with Channel %d $$$$$", test_channel);
            test_mode = 14;
            osDelay(delay_time);
        }
        else if (test_mode == 14)//5G channel 100, Middle tx power
        {
            wifi_rf_test_calibrate_txPower(0x1800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Middle TX Power (0x1800) Test with Channel %d $$$$$", test_channel);
            test_mode = 15;
            osDelay(delay_time);
        }
        else if (test_mode == 15)//5G channel 100, Min tx power
        {
            wifi_rf_test_calibrate_txPower(0x800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Min TX Power (0x800) Test with Channel %d $$$$$", test_channel);
            test_mode = 16;
            osDelay(delay_time);
        }
        else if (test_mode == 16)//5G channel 165, Max tx power
        {
            test_channel = 165;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0x3FFF);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Max TX Power (0x3FFF) Test with Channel %d $$$$$", test_channel);
            test_mode = 17;
            osDelay(delay_time);
        }
        else if (test_mode == 17)//5G channel 165, Middle tx power
        {
            wifi_rf_test_calibrate_txPower(0x1800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Middle TX Power (0x1800) Test with Channel %d $$$$$", test_channel);
            test_mode = 18;
            osDelay(delay_time);
        }
        else if (test_mode == 18)//5G channel 165, Min tx power
        {
            wifi_rf_test_calibrate_txPower(0x800);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Min TX Power (0x800) Test with Channel %d $$$$$", test_channel);
            test_mode = 20;
            osDelay(delay_time);
        }
        else if (test_mode == 20)//5G rx mode
        {
            //test_channel = 165;
            //wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_RX_MODE);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G RX Test with Channel %d $$$$$", test_channel);
            test_mode = 21;
            osDelay(delay_time);
        }
        else if (test_mode == 21)//2.4G rx mode
        {
            test_channel = 1;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_RX_MODE);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G RX Test with Channel %d $$$$$", test_channel);
            test_mode = 22;
            osDelay(delay_time);
        }
        else if (test_mode == 22)//WIFI Stop Test mode
        {
            wifi_rf_test_mode(RF_TX_STOP);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--Stop Test with Channel %d $$$$$", test_channel);
            test_mode = 0;
            osDelay(delay_time);
        }
#else
//2.4G channel 1, Max tx power
        if(test_mode == 0)
        {
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G TX Power (0) Test with Channel %d $$$$$", test_channel);
            test_mode = 1;
            osDelay(delay_time);
        }
        else if (test_mode == 1)//2.4G channel 7, Max tx power
        {
            test_channel = 7;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G TX Power (0) Test with Channel %d $$$$$", test_channel);
            test_mode = 2;
            osDelay(delay_time);
        }
        else if (test_mode == 2)//2.4G channel 13, Max tx power
        {
            test_channel = 13;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G TX Power (0) Test with Channel %d $$$$$", test_channel);
            test_mode = 3;
            osDelay(delay_time);
        }
        //// start test WIFI 5G/////
        else if(test_mode == 3)
        {
            test_channel = 36;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Max TX Power (0) Test with Channel %d $$$$$", test_channel);
            test_mode = 4;
            osDelay(delay_time);
        }
        else if (test_mode == 4)//5G channel 100, Max tx power
        {
            test_channel = 100;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G TX Power (0) Test with Channel %d $$$$$", test_channel);
            test_mode = 5;
            osDelay(delay_time);
        }
        else if (test_mode == 5)//5G channel 165, Max tx power
        {
            test_channel = 165;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_TX_MODE);
            osDelay(8000);
            wifi_rf_test_calibrate_txPower(0);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G Max TX Power (0) Test with Channel %d $$$$$", test_channel);
            test_mode = 6;
            osDelay(delay_time);
        }
        else if (test_mode == 6)//5G rx mode
        {
            //test_channel = 165;
            //wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_RX_MODE);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--5G RX Test with Channel %d $$$$$", test_channel);
            test_mode = 7;
            osDelay(delay_time);
        }
        else if (test_mode == 7)//2.4G rx mode
        {
            test_channel = 1;
            wifi_rf_test_set_channel(test_channel);
            wifi_rf_test_mode(RF_RX_MODE);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--2.4G RX Test with Channel %d $$$$$", test_channel);
            test_mode = 8;
            osDelay(delay_time);
        }
        else if (test_mode == 8)//WIFI Stop Test mode
        {
            wifi_rf_test_mode(RF_TX_STOP);
            TR_INFO(TR_MOD(MAIN), "$$$$$ WIFI Test--Stop Test with Channel %d $$$$$", test_channel);
            test_mode = 0;
            osDelay(delay_time);
        }
#endif
    }
}
