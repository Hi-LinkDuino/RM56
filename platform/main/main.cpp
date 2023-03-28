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
#include "plat_addr_map.h"
#include "analog.h"
#include "apps.h"
#include "app_bt_stream.h"
#include "app_trace_rx.h"
#include "cmsis.h"
#include "hal_bootmode.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_location.h"
#include "hal_norflash.h"
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"
#ifdef ARM_CMNS
#include "tz_trace_ns.h"
#endif
#include "hal_wdt.h"
#include "hal_trng.h"
#include "hwtimer_list.h"
#include "mpu_cfg.h"
#include "mpu.h"
#include "norflash_api.h"
#include "pmu.h"
#include "stdlib.h"
#include "tgt_hardware.h"
#include "app_utils.h"
#include "watchdog/watchdog.h"
#ifdef __WIFI_APP_SUPPORT__
#include "wifi_drv.h"
#endif
#ifdef RTOS
#include "cmsis_os.h"
#include "app_factory.h"
#endif
#ifdef CORE_DUMP_TO_FLASH
#include "coredump_section.h"
#endif
#include "tool_msg.h"

extern "C" void log_dump_init(void);
extern "C" void crash_dump_init(void);

#ifdef FIRMWARE_REV
#define SYS_STORE_FW_VER(x) \
      if(fw_rev_##x) { \
        *fw_rev_##x = fw.softwareRevByte##x; \
      }

typedef struct
{
    uint8_t softwareRevByte0;
    uint8_t softwareRevByte1;
    uint8_t softwareRevByte2;
    uint8_t softwareRevByte3;
} FIRMWARE_REV_INFO_T;

static FIRMWARE_REV_INFO_T fwRevInfoInFlash __attribute((section(".fw_rev"))) = {0, 0, 1, 0};
FIRMWARE_REV_INFO_T fwRevInfoInRam;

extern "C" void system_get_info(uint8_t *fw_rev_0, uint8_t *fw_rev_1,
    uint8_t *fw_rev_2, uint8_t *fw_rev_3)
{
  FIRMWARE_REV_INFO_T fw = fwRevInfoInFlash;

  SYS_STORE_FW_VER(0);
  SYS_STORE_FW_VER(1);
  SYS_STORE_FW_VER(2);
  SYS_STORE_FW_VER(3);
}
#endif

#if defined(_AUTO_TEST_)
static uint8_t fwversion[4] = {0,0,1,0};

void system_get_fwversion(uint8_t *fw_rev_0, uint8_t *fw_rev_1,
    uint8_t *fw_rev_2, uint8_t *fw_rev_3)
{
    *fw_rev_0 = fwversion[0];
    *fw_rev_1 = fwversion[1];
    *fw_rev_2 = fwversion[2];
    *fw_rev_3 = fwversion[3];
}
#endif

static osThreadId main_thread_tid = NULL;

extern "C" int system_shutdown(void)
{
    TR_INFO(TR_MOD(MAIN), "system_shutdown!!");
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x4);
    while (1) {
        TRACE(0, "system_shutdown");
        osDelay(1000);
    }
    return 0;
}

extern "C" int system_reset(void)
{
    osThreadSetPriority(main_thread_tid, osPriorityRealtime);
    osSignalSet(main_thread_tid, 0x8);
    while (1) {
        TRACE(0, "system_reset");
        osDelay(1000);
    }
    return 0;
}

int signal_send_to_main_thread(uint32_t signals)
{
    osSignalSet(main_thread_tid, signals);
    return 0;
}

int tgt_hardware_setup(void)
{
#ifdef __APP_USE_LED_INDICATE_IBRT_STATUS__
    for (uint8_t i=0;i<3;i++){
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_ibrt_indication_pinmux_pwl[i], 1);
        if(i==0)
            hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_ibrt_indication_pinmux_pwl[i].pin, HAL_GPIO_DIR_OUT, 0);
        else
            hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_ibrt_indication_pinmux_pwl[i].pin, HAL_GPIO_DIR_OUT, 1);
    }
#endif

#if CFG_HW_PWL_NUM > 0
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)cfg_hw_pinmux_pwl, sizeof(cfg_hw_pinmux_pwl)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
#endif
    if (app_battery_ext_charger_indicator_cfg.pin != HAL_IOMUX_PIN_NUM){
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_battery_ext_charger_indicator_cfg, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_battery_ext_charger_indicator_cfg.pin, HAL_GPIO_DIR_IN, 1);
    }

#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio_speak_on = {
    CFG_HW_AUD_OUTPUT_POP_SWITCH, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
    hal_iomux_init(&cfg_hw_gpio_speak_on, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_gpio_speak_on.pin, HAL_GPIO_DIR_OUT, 0);
    analog_aud_codec_set_pa(CFG_HW_AUD_OUTPUT_POP_SWITCH);
#endif

    return 0;
}

#if defined(ROM_UTILS_ON)
void rom_utils_init(void);
#endif

#ifdef FPGA
uint32_t a2dp_audio_more_data(uint8_t *buf, uint32_t len);
uint32_t a2dp_audio_init(void);
extern "C" void app_audio_manager_open(void);
extern "C" void app_bt_init(void);
extern "C" uint32_t hal_iomux_init(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count);
void app_overlay_open(void);

extern "C" void BesbtInit(void);
extern "C" int app_os_init(void);
extern "C" uint32_t af_open(void);
extern "C" int list_init(void);
extern "C" void app_audio_open(void);


volatile uint32_t ddddd = 0;

#if defined(AAC_TEST)
#include "app_overlay.h"
int decode_aac_frame_test(unsigned char *pcm_buffer, unsigned int pcm_len);
#define AAC_TEST_PCM_BUFF_LEN (4096)
unsigned char aac_test_pcm_buff[AAC_TEST_PCM_BUFF_LEN];
#endif

#endif

#if defined(_AUTO_TEST_)
extern int32_t at_Init(void);
#endif

#ifdef DEBUG_MODE_USB_DOWNLOAD
static void process_usb_download_mode(void)
{
    if (pmu_charger_get_status() == PMU_CHARGER_PLUGIN && hal_pwrkey_pressed()) {
        hal_sw_bootmode_set(HAL_SW_BOOTMODE_FORCE_USB_DLD);
        pmu_reboot();
    }
}
#endif

#ifndef RAM_TOTAL_SIZE
#define RAM_TOTAL_SIZE RAM_SIZE
#endif

#if defined(__ARM_ARCH_8M_MAIN__)
/* Never overlap the memory region for armv8 MPU, or there will be data abort
 * When access overlaped memory
 */
static const  mpu_regions_t mpu_table[] = {
    {0,             0x800,              MPU_ATTR_EXEC,          MAIR_ATTR_INT_SRAM },
    {RAMX_BASE,     RAM_TOTAL_SIZE,     MPU_ATTR_READ_EXEC,     MAIR_ATTR_INT_SRAM },
#if defined(CHIP_BEST2001) || defined(CHIP_BEST2003)
    {DSP_RAM_BASE,  DSP_RAM_SIZE,       MPU_ATTR_READ_WRITE,    MAIR_ATTR_INT_SRAM },
#endif
};
#else
/* Can overlap the memory region for armv7 MPU, the attribute of high number region
 * will override the low one
 */
static const  mpu_regions_t mpu_table[] = {
#ifdef CHIP_BEST1000
    {0,             0x400,          MPU_ATTR_NO_ACCESS },
#else
    {0,             0x800,          MPU_ATTR_NO_ACCESS },
#endif
    {RAMX_BASE,     RAM_TOTAL_SIZE,       MPU_ATTR_READ_EXEC },
    {RAM_BASE,      RAM_TOTAL_SIZE,       MPU_ATTR_READ_WRITE },
#if defined(CHIP_BEST2001) || defined(CHIP_BEST2003)
    {DSP_RAM_BASE,  DSP_RAM_SIZE,         MPU_ATTR_READ_WRITE},
#endif
};
#endif

#if RTT_APP_SUPPORT
extern "C" void platform_console_init(void)
{
    hwtimer_init();
    hal_audma_open();
    hal_gpdma_open();

#ifdef DEBUG
#if (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
    {
        hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
    }
#endif

#if (DEBUG_PORT == 2)
    {
        hal_iomux_set_analog_i2c();
    }
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#endif
#endif
};
#endif

extern "C" void wifidrv_set_rf_tx_ionumber(int rf_io);
extern "C" int platform_io_cfg(int fem_idx, int pa_en_idx)
{
    TRACE(0, "%s fem %d pa %d", __func__, fem_idx, pa_en_idx);
    if (fem_idx != -1)
        wifidrv_set_rf_tx_ionumber(fem_idx);
    if (pa_en_idx != -1) {
        analog_aud_codec_set_pa(pa_en_idx);
    #ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
        struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio_speak_on = {
            (HAL_IOMUX_PIN_T)pa_en_idx, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL};
        hal_iomux_init(&cfg_hw_gpio_speak_on, 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_gpio_speak_on.pin, HAL_GPIO_DIR_OUT, 0);
    #endif
    }
    return 0;
}

#ifndef OS_MAIN_SUPPORT
int WEAK main(void)
{
    uint8_t sys_case = 0;
    int ret = 0;
#if !defined(BLE_ONLY_ENABLED)
    app_wdt_open(15);
#else
    app_wdt_open(30);
#endif

#if (defined(PSRAM_ENABLE) || defined(PSRAMUHS_ENABLE)) && defined(PSRAM_INIT_CHECK)
    pmu_wdt_stop();
#endif

#ifdef __FACTORY_MODE_SUPPORT__
    uint32_t bootmode = hal_sw_bootmode_get();
#endif

#ifdef DEBUG_MODE_USB_DOWNLOAD
    process_usb_download_mode();
#endif

    tgt_hardware_setup();

#if defined(ROM_UTILS_ON)
    rom_utils_init();
#endif

    main_thread_tid = osThreadGetId();

    hwtimer_init();

    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);
    hal_audma_open();
    hal_gpdma_open();
    norflash_api_init();
#if defined(DUMP_LOG_ENABLE)
    log_dump_init();
#endif
#if (defined(DUMP_CRASH_LOG) || defined(TOTA_CRASH_DUMP_TOOL_ENABLE))
    crash_dump_init();
#endif
#ifdef CORE_DUMP_TO_FLASH
    coredump_to_flash_init();
#endif

#ifdef DEBUG
#if (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
    }
#endif

#if (DEBUG_PORT == 2)
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_iomux_set_analog_i2c();
    }
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#endif
    hal_sleep_start_stats(10000, 10000);
    hal_trace_set_log_level(TR_LEVEL_DEBUG);

#ifdef ARM_CMNS
    cmns_trace_init();
#endif
#endif

    hal_iomux_ispi_access_init();

#ifndef FPGA
    uint8_t flash_id[HAL_NORFLASH_DEVICE_ID_LEN];
    NORFLASH_API_WRAP(hal_norflash_get_id)(HAL_FLASH_ID_0, flash_id, ARRAY_SIZE(flash_id));
    TR_INFO(TR_MOD(MAIN), "FLASH_ID: %02X-%02X-%02X", flash_id[0], flash_id[1], flash_id[2]);
    NORFLASH_API_WRAP(hal_norflash_show_calib_result)(HAL_FLASH_ID_0);
    ASSERT(NORFLASH_API_WRAP(hal_norflash_opened)(HAL_FLASH_ID_0), "Failed to init flash: %d", NORFLASH_API_WRAP(hal_norflash_get_open_state)(HAL_FLASH_ID_0));

    // Software will load the factory data and user data from the bottom TWO sectors from the flash,
    // the FLASH_SIZE defined is the common.mk must be equal or greater than the actual chip flash size,
    // otherwise the ota will load the wrong information
    uint32_t actualFlashSize = NORFLASH_API_WRAP(hal_norflash_get_flash_total_size)(HAL_FLASH_ID_0);
    if (FLASH_SIZE > actualFlashSize)
    {
        TRACE_IMM(0,"Wrong FLASH_SIZE defined in target.mk!");
        TRACE_IMM(2,"FLASH_SIZE is defined as 0x%x while the actual chip flash size is 0x%x!", FLASH_SIZE, actualFlashSize);
        TRACE_IMM(1,"Please change the FLASH_SIZE in common.mk to 0x%x to enable the OTA feature.", actualFlashSize);
        ASSERT(false, " ");
    }
#endif

    mpu_cfg();

    pmu_open();

    analog_open();

    srand(hal_sys_timer_get());

#if defined(_AUTO_TEST_)
    at_Init();
#endif

#ifdef VOICE_DATAPATH
    app_audio_buffer_check();
#endif

    /* config platform io for rf & pa */
    int fem_idx = -1;
    int pa_en_idx = -1;
#ifdef RF_TX_CONTROL_IO
    fem_idx = RF_TX_CONTROL_IO;
#endif
#ifdef PA_ENABLE_IO
    pa_en_idx = PA_ENABLE_IO;
#endif
    platform_io_cfg(fem_idx, pa_en_idx);

#ifdef FPGA

    TR_INFO(TR_MOD(MAIN), "\n[best of best of best...]\n");
    TR_INFO(TR_MOD(MAIN), "\n[ps: w4 0x%x,2]", &ddddd);

    ddddd = 1;
    while (ddddd == 1);
    TR_INFO(TR_MOD(MAIN), "bt start");

    list_init();

    app_os_init();
    app_bt_init();
    a2dp_audio_init();

    af_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();

#if defined(AAC_TEST)
    app_overlay_select(APP_OVERLAY_A2DP_AAC);
    decode_aac_frame_test(aac_test_pcm_buff, AAC_TEST_PCM_BUFF_LEN);
#endif

    SAFE_PROGRAM_STOP();

#else // !FPGA

#if defined( __FACTORY_MODE_SUPPORT__) && !defined(SLIM_BTC_ONLY)
    if (bootmode & HAL_SW_BOOTMODE_FACTORY){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_FACTORY);
        ret = app_factorymode_init(bootmode);

    }else if(bootmode & HAL_SW_BOOTMODE_CALIB){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CALIB);
        ret = app_factorymode_calib_only();
    }
#ifdef __USB_COMM__
    else if(bootmode & HAL_SW_BOOTMODE_CDC_COMM)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CDC_COMM);
        ret = app_factorymode_cdc_comm();
    }
#endif
    else
#endif
    {
#ifdef FIRMWARE_REV
        fwRevInfoInRam = fwRevInfoInFlash;
        TR_INFO(TR_MOD(MAIN), "The Firmware rev is %d.%d.%d.%d",
        fwRevInfoInRam.softwareRevByte0,
        fwRevInfoInRam.softwareRevByte1,
        fwRevInfoInRam.softwareRevByte2,
        fwRevInfoInRam.softwareRevByte3);
#endif
#ifdef SLIM_BTC_ONLY
        ret = app_init_btc();
#else
        ret = app_init(0);
#endif
    }

    if (!ret){
#if defined(_AUTO_TEST_)
        AUTO_TEST_SEND("BT Init ok.");
#endif
        while(1)
        {
            osEvent evt;
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
            osSignalClear (main_thread_tid, 0x0f);
#endif
            //wait any signal
            evt = osSignalWait(0x0, osWaitForever);

            //get role from signal value
            if(evt.status == osEventSignal)
            {
                if(evt.value.signals & 0x04)
                {
                    sys_case = 1;
                    break;
                }
                else if(evt.value.signals & 0x08)
                {
                    sys_case = 2;
                    break;
                }
            }else{
                sys_case = 1;
                break;
            }
         }
    }
    system_shutdown_wdt_config(10);
#ifdef SLIM_BTC_ONLY
    app_deinit_btc(ret);
#else
    app_deinit(ret);
#endif
    TR_INFO(TR_MOD(MAIN), "byebye~~~ %d\n", sys_case);
    if ((sys_case == 1)||(sys_case == 0)){
        TR_INFO(TR_MOD(MAIN), "shutdown\n");
#if defined(_AUTO_TEST_)
        AUTO_TEST_SEND("System shutdown.");
        osDelay(50);
#endif
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
        pmu_shutdown();
    }else if (sys_case == 2){
        TR_INFO(TR_MOD(MAIN), "reset\n");
#if defined(_AUTO_TEST_)
        AUTO_TEST_SEND("System reset.");
        osDelay(50);
#endif
        pmu_reboot();
    }

#endif // !FPGA

    return 0;
}
#endif

extern "C" void config_antenna_switch(void);
extern "C" void call_boot_struct_info(void);

extern "C" int platform_trace_disable()
{
    hal_trace_flush_buffer();
    hal_trace_close();
    return 0;
}

extern "C" int platform_trace_enable()
{
#ifdef __FACTORY_MODE_SUPPORT__
    uint32_t bootmode = hal_sw_bootmode_get();
#endif

#ifdef DEBUG
#if (DEBUG_PORT == 1)
    hal_iomux_set_uart0();
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_trace_open(HAL_TRACE_TRANSPORT_UART0);
    }
#endif
#ifndef CFG_BATTERY_VER
#if (DEBUG_PORT == 2)
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_iomux_set_analog_i2c();
    }
    hal_iomux_set_uart1();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART1);
#endif

#if (DEBUG_PORT == 3)
#ifdef __FACTORY_MODE_SUPPORT__
    if (!(bootmode & HAL_SW_BOOTMODE_FACTORY))
#endif
    {
        hal_iomux_set_analog_i2c();
    }
    hal_iomux_set_uart2();
    hal_trace_open(HAL_TRACE_TRANSPORT_UART2);
#endif
#endif
    hal_sleep_start_stats(10000, 10000);

#ifdef ARM_CMNS
    cmns_trace_init();
#endif
#endif

    return 0;
}

#ifdef USE_TRACE_ID
const char *unkonw_str __attribute__((section(".trc_str"))) = "unknow string";
#endif


#define PMU_RESERVED_REG (0x39)
#define UART_SWITCH_FLAG_BIT (15)
static bool uart0_is_i2c = false;
static void uart0_i2c_switch_timer_handler(void const *param);
osTimerDef(UART0_I2C_SWITCH_TIMER, uart0_i2c_switch_timer_handler);
osTimerId uart0_i2c_switch_timer = NULL;

static void uart0_i2c_switch_timer_handler(void const *param)
{
    uint16_t reg16 = 0;
    pmu_read(PMU_RESERVED_REG, &reg16);
    if(uart0_is_i2c){
        if(!(reg16 & (1 << UART_SWITCH_FLAG_BIT))){
            hal_iomux_set_uart0();
            uart0_is_i2c = false;
            osTimerStop(uart0_i2c_switch_timer);
            osTimerDelete(uart0_i2c_switch_timer);
            uart0_i2c_switch_timer = NULL;
        }
    }
}

/*
call this funtion will swith uart0 to i2c
if use i2c tools write pmu 0x39 bit15, it'll switch i2c to uart0
*/
extern "C" int hal_uart0_i2c_switch(void)
{
    uint16_t reg16 = 0;
#if (DEBUG_PORT != 1)
    return -1;
#endif
    if( !uart0_is_i2c){
        //switch uart to i2c
        hal_iomux_set_analog_i2c();
        uart0_is_i2c = true;
        pmu_read(PMU_RESERVED_REG, &reg16);
        reg16 |= (1 << UART_SWITCH_FLAG_BIT);
        pmu_write(PMU_RESERVED_REG, reg16);
        if(!uart0_i2c_switch_timer){
            uart0_i2c_switch_timer = osTimerCreate(osTimer(UART0_I2C_SWITCH_TIMER),
                                                                            osTimerPeriodic,
                                                                            NULL);
            if(uart0_i2c_switch_timer){
                osTimerStart(uart0_i2c_switch_timer, 500);
            }
        }
    }
    return 0;
}

extern "C" void hal_uart0_i2c_switch_init(void)
{
    uint16_t reg16 = 0;
    //should set after bt power on  or it'll down
    uart0_is_i2c = false;
    pmu_read(PMU_RESERVED_REG, &reg16);
    reg16 &= ~(1 << UART_SWITCH_FLAG_BIT);
    pmu_write(PMU_RESERVED_REG, reg16);
    uart0_i2c_switch_timer = NULL;
}

extern "C" int platform_init_step0(int release_version)
{
    int ret = 0;
    struct HAL_TRNG_CFG_T trng_cfg = {0x100, HAL_TRNG_RND_SRC_SHORTEST};

#ifdef __WATCHER_DOG_RESET__
    if (release_version == 1) {
#if !defined(BLE_ONLY_ENABLED)
        app_wdt_open(30); //for ali things
#else
        app_wdt_open(30);
#endif
    }
#endif

#if (defined(PSRAM_ENABLE) || defined(PSRAMUHS_ENABLE)) && defined(PSRAM_INIT_CHECK)
    if (hal_sw_bootmode_get() & HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI) {
        hal_wdt_stop(HAL_WDT_ID_0);
    } else {
        pmu_wdt_stop();
    }
#endif

#ifdef __WIFI_RECOVERY_SUPPORT__
    if (release_version == 1)
        bwifi_recovery_enable(true);
#endif

#if 0
#ifdef __FACTORY_MODE_SUPPORT__
    uint32_t bootmode = hal_sw_bootmode_get();
#endif
#endif

#ifdef DEBUG_MODE_USB_DOWNLOAD
    process_usb_download_mode();
#endif

    tgt_hardware_setup();

    hal_trng_open(&trng_cfg);

#if defined(ROM_UTILS_ON)
    rom_utils_init();
#endif

    main_thread_tid = osThreadGetId();

    hwtimer_init();

    hal_dma_set_delay_func((HAL_DMA_DELAY_FUNC)osDelay);
    hal_audma_open();
    hal_gpdma_open();
    norflash_api_init();
#if defined(DUMP_LOG_ENABLE)
    log_dump_init();
#endif
#if (defined(DUMP_CRASH_ENABLE) || defined(TOTA_CRASH_DUMP_TOOL_ENABLE))
    crash_dump_init();
#endif
#ifdef CORE_DUMP_TO_FLASH
    coredump_to_flash_init();
#endif

    platform_trace_enable();

    hal_iomux_ispi_access_init();

#ifndef FPGA
    uint8_t flash_id[HAL_NORFLASH_DEVICE_ID_LEN];
    NORFLASH_API_WRAP(hal_norflash_get_id)(HAL_FLASH_ID_0, flash_id, ARRAY_SIZE(flash_id));
    TR_INFO(TR_MOD(MAIN), "FLASH_ID: %02X-%02X-%02X", flash_id[0], flash_id[1], flash_id[2]);
    NORFLASH_API_WRAP(hal_norflash_show_calib_result)(HAL_FLASH_ID_0);
    ASSERT(NORFLASH_API_WRAP(hal_norflash_opened)(HAL_FLASH_ID_0), "Failed to init flash: %d", NORFLASH_API_WRAP(hal_norflash_get_open_state)(HAL_FLASH_ID_0));

    // Software will load the factory data and user data from the bottom TWO sectors from the flash,
    // the FLASH_SIZE defined is the common.mk must be equal or greater than the actual chip flash size,
    // otherwise the ota will load the wrong information
    uint32_t actualFlashSize = NORFLASH_API_WRAP(hal_norflash_get_flash_total_size)(HAL_FLASH_ID_0);
    if (FLASH_SIZE > actualFlashSize)
    {
        TRACE_IMM(0,"Wrong FLASH_SIZE defined in target.mk!");
        TRACE_IMM(2,"FLASH_SIZE is defined as 0x%x while the actual chip flash size is 0x%x!", FLASH_SIZE, actualFlashSize);
        TRACE_IMM(1,"Please change the FLASH_SIZE in common.mk to 0x%x to enable the OTA feature.", actualFlashSize);
        ASSERT(false, " ");
    }
#endif

    pmu_open();

    analog_open();

    hal_uart0_i2c_switch_init();

    ret = mpu_setup(mpu_table, ARRAY_SIZE(mpu_table));
    if (ret) {
        TR_INFO(TR_MOD(MAIN), "Warning, MPU is not setup correctly!!!");
    }

    return ret;
}


extern "C" int platform_init_step1(int user_init)
{
    int ret = 0;

    main_thread_tid = osThreadGetId();

#ifdef __FACTORY_MODE_SUPPORT__
    uint32_t bootmode = hal_sw_bootmode_get();
#endif

    srand(hal_sys_timer_get());

#if defined(_AUTO_TEST_)
    at_Init();
#endif

#ifdef VOICE_DATAPATH
    app_audio_buffer_check();
#endif

    /* config platform io for rf & pa */
    int fem_idx = -1;
    int pa_en_idx = -1;
#ifdef RF_TX_CONTROL_IO
    fem_idx = RF_TX_CONTROL_IO;
#endif
#ifdef PA_ENABLE_IO
    pa_en_idx = PA_ENABLE_IO;
#endif
    platform_io_cfg(fem_idx, pa_en_idx);

#ifdef FPGA

    TR_INFO(TR_MOD(MAIN), "\n[best of best of best...]\n");
    TR_INFO(TR_MOD(MAIN), "\n[ps: w4 0x%x,2]", &ddddd);

    ddddd = 1;
    while (ddddd == 1);
    TR_INFO(TR_MOD(MAIN), "bt start");

    list_init();

    app_os_init();
    app_bt_init();
    a2dp_audio_init();

    af_open();
    app_audio_open();
    app_audio_manager_open();
    app_overlay_open();

#if defined(AAC_TEST)
    app_overlay_select(APP_OVERLAY_A2DP_AAC);
    decode_aac_frame_test(aac_test_pcm_buff, AAC_TEST_PCM_BUFF_LEN);
#endif

    SAFE_PROGRAM_STOP();

#else // !FPGA

#ifdef __FACTORY_MODE_SUPPORT__
    if (bootmode & HAL_SW_BOOTMODE_FACTORY){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_FACTORY);
        ret = app_factorymode_init(bootmode);

    }else if(bootmode & HAL_SW_BOOTMODE_CALIB){
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CALIB);
        ret = app_factorymode_calib_only();
    }
#ifdef __USB_COMM__
    else if(bootmode & HAL_SW_BOOTMODE_CDC_COMM)
    {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_CDC_COMM);
        ret = app_factorymode_cdc_comm();
    }
#endif
    else
#endif
    {
#ifdef FIRMWARE_REV
        fwRevInfoInRam = fwRevInfoInFlash;
        TR_INFO(TR_MOD(MAIN), "The Firmware rev is %d.%d.%d.%d",
        fwRevInfoInRam.softwareRevByte0,
        fwRevInfoInRam.softwareRevByte1,
        fwRevInfoInRam.softwareRevByte2,
        fwRevInfoInRam.softwareRevByte3);
#endif
        config_antenna_switch();

        ret = app_init(user_init);
    }
#endif // !FPGA

    /*if ((hal_sw_bootmode_get() & HAL_SW_BOOTMODE_RESERVED_BIT24)) {
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_RESERVED_BIT24);
    }*/
    call_boot_struct_info();
#ifdef PLATFORM_THERMAL_REBOOT
    if(!app_enter_factory_wifi_test()){
        TRACE("%s enter temprature cali on signal",__func__);
        temp_thread_id = osThreadCreate(osThread(temp_main), &temp_thread_id);
    }
#endif

    return ret;
}

extern "C" void platform_init_step2(int step1_init)
{
    uint8_t sys_case = 0;
    if (!step1_init){
#if defined(_AUTO_TEST_)
        AUTO_TEST_SEND("BT Init ok.");
#endif
        while(1)
        {
            osEvent evt;
#ifndef __POWERKEY_CTRL_ONOFF_ONLY__
            osSignalClear (main_thread_tid, 0x0f);
#endif
            //wait any signal
            evt = osSignalWait(0x0, osWaitForever);

            //get role from signal value
            if(evt.status == osEventSignal)
            {
                if(evt.value.signals & 0x04)
                {
                    sys_case = 1;
                    break;
                }
                else if(evt.value.signals & 0x08)
                {
                    sys_case = 2;
                    break;
                }
            }else{
                sys_case = 1;
                break;
            }
         }
    }
    system_shutdown_wdt_config(10);
    app_deinit(step1_init);
    TR_INFO(TR_MOD(MAIN), "byebye~~~ %d\n", sys_case);
    if ((sys_case == 1)||(sys_case == 0)){
        TR_INFO(TR_MOD(MAIN), "shutdown\n");
#if defined(_AUTO_TEST_)
        AUTO_TEST_SEND("System shutdown.");
        osDelay(50);
#endif
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_WARM_BOOT);
        hal_sw_bootmode_clear(HAL_SW_BOOTMODE_REBOOT);
        pmu_shutdown();
    }else if (sys_case == 2){
        TR_INFO(TR_MOD(MAIN), "reset\n");
#if defined(_AUTO_TEST_)
        AUTO_TEST_SEND("System reset.");
        osDelay(50);
#endif
        pmu_reboot();
    }

}
