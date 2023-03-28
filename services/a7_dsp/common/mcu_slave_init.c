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
#if (defined(CHIP_BEST2001) || defined(CHIP_BEST2003)) && !defined(__ARM_ARCH_ISA_ARM)
#include "cmsis.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_location.h"
#include "hal_psramuhs.h"
#include "hal_trace.h"
#include "hal_cache.h"
#include "hal_timer.h"
#include "hal_bootmode.h"
#include "string.h"
#include "transq_msg.h"
#include "cmsis_os.h"
#include "mcu_audio.h"
#include "audioflinger.h"
#include "app_utils.h"
#include "app_mic.h"
#include "dsp_codec.h"
#include "heart_beat.h"

static int boot_num = -1;
#if (CONFIG_A7_BIN_OFFSET == 0)
extern uint32_t __dsp_code_start[];
extern uint32_t __dsp_code_end[];
#ifdef DSP_USE_PSRAM
extern uint32_t __dsp_code_text_start[];
extern uint32_t __dsp_code_text_end[];
#endif
#ifdef MCU_SLAVE_FS_EN
extern uint32_t __fs_bin_start[];
extern uint32_t __fs_bin_end[];
#endif
#elif defined (NUTTX_BUILD)
#include "endian.h"
#endif

#ifdef DSP_IMAGE_COMP
//extern void dsp_code_decom(int inbuf, int in_size, unsigned char *outbuf);
extern int dsp_code_decom (unsigned char *outStream, unsigned int *uncompressedSize,
			      unsigned char *inStream,  unsigned int  length);
#endif

extern void heartbeat_reset_timer(int tick_ms);
uint32_t dsp_start_ticks = 0;
static int a7_boot_done = 0;

int a7_dsp_get_bootnum(void)
{
    return boot_num;
}
void a7_dsp_set_bootnum(int num)
{
    boot_num = num;
}

#ifdef MCU_SLAVE_FS_EN
void load_fs_bin(void)
{
    /// decompress fs
    TRACE(0, "mcu_dsp_fs_enable");

    unsigned int uncompressed_size;
    uint32_t fs_entry = PSRAMFS_BASE;
    int fs_remains = ((uint32_t)__fs_bin_end - (uint32_t)__fs_bin_start);//length of byte
    TRACE(0, "fs decompress start fs_entry:0x%x __fs_bin_start:0x%x fs_remains:%d",fs_entry,__fs_bin_start,fs_remains);
    int ret = dsp_code_decom ((unsigned char *)fs_entry, &uncompressed_size, (unsigned char *)__fs_bin_start, fs_remains);
    TRACE(0, "fs decompress end");
    if (ret != 0) {
        TRACE(2|TR_ATTR_IMM, "%s:%d fs decompress FAIL!", __FUNCTION__, __LINE__);
        return;
    }
}
#endif

static int __a7_dsp_boot(void)
{
    int ret = -1;
    uint32_t entry;
#ifndef DSP_IMAGE_COMP
    struct HAL_DMA_CH_CFG_T dma_cfg;
    enum HAL_DMA_RET_T dma_ret;
#endif
    uint32_t remains;
    unsigned int uncompressed_size;
#if (CONFIG_A7_BIN_OFFSET > 0)
    uint32_t dsp_code_start = (FLASH_BASE+CONFIG_A7_BIN_OFFSET)+4;;
    uint32_t __dsp_code_end;
    uint32_t __dsp_code_dize;
    __dsp_code_dize = be32toh(*((uint32_t *)(FLASH_BASE+CONFIG_A7_BIN_OFFSET)));
    __dsp_code_end = dsp_code_start +__dsp_code_dize;
    TRACE(0,"a7_dsp_boot dsp_code_start=0x%x,size=0x%d",dsp_code_start,__dsp_code_dize);
#ifdef DSP_USE_PSRAM
    uint32_t __dsp_code_text_end;
    uint32_t dsp_code_text_start = (FLASH_BASE+CONFIG_A7_BIN2_OFFSET)+4;
    __dsp_code_dize = be32toh(*((uint32_t *)(FLASH_BASE+CONFIG_A7_BIN2_OFFSET)));
    __dsp_code_text_end = dsp_code_text_start +__dsp_code_dize;
    TRACE(0,"a7_dsp_boot dsp_code_text_start=0x%x,size=0x%d",dsp_code_text_start,__dsp_code_dize);
#endif
#elif defined (NUTTX_BUILD)
    uint32_t dsp_code_start = (uint32_t)__dsp_code_start+4;
#ifdef DSP_USE_PSRAM
    uint32_t dsp_code_text_start = (uint32_t)__dsp_code_text_start+4;
#endif
#else
    uint32_t dsp_code_start = (uint32_t)__dsp_code_start;
#ifdef DSP_USE_PSRAM
    uint32_t dsp_code_text_start = (uint32_t)__dsp_code_text_start;
#endif
#endif
    TRACE(0, "%s", __FUNCTION__);

    /* init dsp clock */
    hal_cmu_dsp_clock_enable();
    hal_cmu_dsp_reset_clear();
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_390M);

#ifdef DSP_USE_PSRAM
#ifndef DSP_IMAGE_COMP
    //entry = *(__dsp_code_end - 1);
    entry = PSRAMDSP_BASE;
#if 1 //DMA copy
    // remains = __dsp_code_end - dsp_code_start - 1;
    remains = ((uint32_t)__dsp_code_text_end - (uint32_t)dsp_code_text_start);
    remains = remains / 4; // dma use word length

    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
    dma_cfg.dst_periph = HAL_GPDMA_MEM;
    dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
    dma_cfg.handler = NULL;
    dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
    dma_cfg.src_periph = HAL_GPDMA_MEM;
    dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
    dma_cfg.type = HAL_DMA_FLOW_M2M_DMA;
    dma_cfg.try_burst = false;
    dma_cfg.ch = hal_dma_get_chan(dma_cfg.dst_periph, HAL_DMA_LOW_PRIO);
    ASSERT(dma_cfg.ch != HAL_DMA_CHAN_NONE, "%s: Failed to get dma chan", __func__);

    dma_cfg.dst = entry;
    dma_cfg.src = (uint32_t)dsp_code_text_start;

    while (remains > 0) {
        if (remains > HAL_DMA_MAX_DESC_XFER_SIZE) {
            dma_cfg.src_tsize = HAL_DMA_MAX_DESC_XFER_SIZE;
        } else {
            dma_cfg.src_tsize = remains;
        }
        dma_ret = hal_dma_start(&dma_cfg);
        ASSERT(dma_ret == HAL_DMA_OK, "%s: Failed to start dma: %d", __func__, dma_ret);

        while (hal_dma_chan_busy(dma_cfg.ch));

        dma_cfg.dst += dma_cfg.src_tsize * 4;
        dma_cfg.src += dma_cfg.src_tsize * 4;
        remains -= dma_cfg.src_tsize;
    }

    hal_dma_free_chan(dma_cfg.ch);
#else
    remains = ((uint32_t)__dsp_code_end - (uint32_t)dsp_code_start); //byte length
    memcpy((unsigned char *)entry, (unsigned char *)dsp_code_start, remains);
#endif
#else // DSP_IMAGE_COMP
    entry = PSRAMDSP_BASE;
    remains = ((uint32_t)__dsp_code_text_end - (uint32_t)dsp_code_text_start);//length of byte
    //dsp_code_decom((uint32_t)__dsp_code_start, remains, (unsigned char *)entry);
    TRACE(0, "a7 decompress start");
    ret = dsp_code_decom ((unsigned char *)entry, &uncompressed_size, (unsigned char *)dsp_code_text_start, remains);
    TRACE(0, "a7 decompress end");
    if (ret != 0) {
        //hal_cmu_dsp_reset_set();
        //hal_cmu_dsp_clock_disable();
        TRACE(2|TR_ATTR_IMM, "%s:%d FAIL!", __FUNCTION__, __LINE__);
        return ret;
    }
#endif // DSP_IMAGE_COMP
#endif
    /* load dsp image */
#ifndef DSP_IMAGE_COMP
    //entry = *(__dsp_code_end - 1);
#if (DSP_PSRAMUHS_SIZE > 0)
    entry = DSP_PSRAMUHS_BASE;
#else
    entry = DSP_RAM_BASE;
#endif
#if 1 //DMA copy
    // remains = __dsp_code_end - dsp_code_start - 1;
    remains = ((uint32_t)__dsp_code_end - (uint32_t)dsp_code_start);
    remains = remains / 4; // dma use word length

    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
    dma_cfg.dst_periph = HAL_GPDMA_MEM;
    dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
    dma_cfg.handler = NULL;
    dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
    dma_cfg.src_periph = HAL_GPDMA_MEM;
    dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
    dma_cfg.type = HAL_DMA_FLOW_M2M_DMA;
    dma_cfg.try_burst = false;
    dma_cfg.ch = hal_dma_get_chan(dma_cfg.dst_periph, HAL_DMA_LOW_PRIO);
    ASSERT(dma_cfg.ch != HAL_DMA_CHAN_NONE, "%s: Failed to get dma chan", __func__);

    dma_cfg.dst = entry;
    dma_cfg.src = (uint32_t)dsp_code_start;

    while (remains > 0) {
        if (remains > HAL_DMA_MAX_DESC_XFER_SIZE) {
            dma_cfg.src_tsize = HAL_DMA_MAX_DESC_XFER_SIZE;
        } else {
            dma_cfg.src_tsize = remains;
        }
        dma_ret = hal_dma_start(&dma_cfg);
        ASSERT(dma_ret == HAL_DMA_OK, "%s: Failed to start dma: %d", __func__, dma_ret);

        while (hal_dma_chan_busy(dma_cfg.ch));

        dma_cfg.dst += dma_cfg.src_tsize * 4;
        dma_cfg.src += dma_cfg.src_tsize * 4;
        remains -= dma_cfg.src_tsize;
    }

    hal_dma_free_chan(dma_cfg.ch);
#else
    remains = ((uint32_t)__dsp_code_end - (uint32_t)dsp_code_start); //byte length
    memcpy((unsigned char *)entry, (unsigned char *)dsp_code_start, remains);
#endif
#else // DSP_IMAGE_COMP
#if (DSP_PSRAMUHS_SIZE > 0)
    entry = DSP_PSRAMUHS_BASE;
#else
    entry = DSP_RAM_BASE;
#endif
    remains = ((uint32_t)__dsp_code_end - (uint32_t)dsp_code_start);//length of byte
    //dsp_code_decom((uint32_t)__dsp_code_start, remains, (unsigned char *)entry);
    TRACE(0, "a7 decompress start");
    ret = dsp_code_decom ((unsigned char *)entry, &uncompressed_size, (unsigned char *)dsp_code_start, remains);
    TRACE(0, "a7 decompress end");
    if (ret != 0) {
        //hal_cmu_dsp_reset_set();
        //hal_cmu_dsp_clock_disable();
        TRACE(2|TR_ATTR_IMM, "%s:%d FAIL!", __FUNCTION__, __LINE__);
        return ret;
    }
#endif // DSP_IMAGE_COMP

    hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
    TRACE(0, "boot entry %p", (void *)entry);
    if (*(uint16_t *)entry == 0xb673)
        hal_cmu_dsp_init_boot_reg(entry | 1);
    else
        hal_cmu_dsp_init_boot_reg(entry);
    a7_dsp_set_bootnum(a7_dsp_get_bootnum()+1);

#if defined(CONFIG_RPTUN)
#if !defined(CONFIG_BES_RPTUN_DELAY_BOOT)
    bes_rptun_heap_init();
#endif
#else
    transq_msg_init();
    heartbeat_init();
    hal_trace_print_a7_flush(0);
    a7_cmd_recv_init();

#ifdef __A7_DSP_AUDIO_TASK__
    mcu_audio_task_init();
#endif

#ifdef __A7_DSP_CODEC__
    mcu_codec_open();
#endif
#endif

    //switch to a7 jtag
    //*(volatile uint32_t*)0x40000140 |= (0xff<<11);
    //*(volatile uint32_t*)0x4000005c |= (1<<26);

    uint32_t lock = int_lock();
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_52M);
    hal_sys_timer_delay(TICKS_TO_MS(10));
    /* start to run dsp */
    hal_cmu_dsp_start_cpu();
    hal_sys_timer_delay(TICKS_TO_MS(100));
    app_sysfreq_req(APP_SYSFREQ_USER_APP_INIT, APP_SYSFREQ_390M);
    int_unlock(lock);

    ret = 0;
    return ret;
}

void mcu_dsp_disable(void)
{
    A7_RECOVER_EVENT status = a7_dsp_status_get();
    if (status != A7_RECOVER_DONE) {
        TRACE(1, "%s %d dsp is not running status:%d,exit!", __FUNCTION__, __LINE__, status);
        return;
    }
    TRACE(1, "%s %d dsp will be disable, status:%d", __FUNCTION__, __LINE__, status);
    a7_dsp_recover_report_event(A7_USER_DISABLE);
    a7_dsp_set_crash_status(1);
    heartbeat_stop_timer();
    transq_msg_onoff(0);
    transq_msg_flush();

    TRACE(1, "%s %d stop mic", __FUNCTION__, __LINE__);
#ifdef __A7_DSP_AUDIO_TASK__
    app_mic_stop(MIC_APP_MICRECORD);
    osDelay(10);
#endif
    TRACE(1, "%s %d stop cpu", __FUNCTION__, __LINE__);
    hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
    hal_cmu_dsp_stop_cpu();
    osDelay(1);
    hal_cmu_dsp_reset_set();
    osDelay(1);
    TRACE(1, "%s %d clock disable", __FUNCTION__, __LINE__);
    hal_cmu_dsp_clock_disable();
    osDelay(1);
    TRACE(1, "%s %d psramuhs sleep", __FUNCTION__, __LINE__);
    hal_psramuhs_sleep();
    TRACE(1, "%s %d done", __FUNCTION__, __LINE__);
}

void mcu_dsp_enable(void)
{
    uint8_t retry = 0;
    A7_RECOVER_EVENT status = a7_dsp_status_get();
    if ((status == A7_RECOVER_DONE) || (status == A7_RECOVERING)) {
        TRACE(1, "%s %d dsp is aready or recovering! status:%u", __FUNCTION__, __LINE__, status);
        return;
    }
    TRACE(1, "%s %d dsp will be boot, status:%d", __FUNCTION__, __LINE__, status);
    a7_dsp_recover_report_event(A7_RECOVERING);
    dsp_start_ticks = hal_sys_timer_get();
    hal_psramuhs_wakeup();
    osDelay(1);
    transq_msg_reinit();
    while (0 != __a7_dsp_boot()) {
        retry++;
        TRACE(1|TR_ATTR_IMM, "__a7_dsp_boot fail, retry %d", retry);
        if (retry >= A7_DSP_BOOT_RETRY_NUM) {
            TRACE(0|TR_ATTR_IMM, "__a7_dsp_boot fail deadly, now reboot m33");
            hal_sw_bootmode_set(HAL_SW_BOOTMODE_A7_REBOOT);
            hal_cmu_sys_reboot();
            break;
        }
    }

#if defined(CONFIG_RPTUN)
    a7_dsp_recover_report_event(A7_RECOVER_DONE);
#endif

/*  if (retry < A7_DSP_BOOT_RETRY_NUM) {
        a7_dsp_recover_report_event(A7_USER_ENABLE);
    } */
    heartbeat_reset_timer(5*1000);
    a7_boot_done = 1;
    TRACE(1, "%s %d done", __FUNCTION__, __LINE__);
}

static void __a7_dsp_reboot(void)
{
    int retry = 0;
    A7_RECOVER_EVENT status = a7_dsp_status_get();
    TRACE(1, "%s %d, boot_num=%d a7 status:%d", __FUNCTION__, __LINE__, a7_dsp_get_bootnum(), status);
    if ((a7_dsp_get_bootnum() > 0) && (status != A7_USER_DISABLE)) { // it's reboot
        //sendTrace("crashReport","dspreboot",NULL,NULL,NULL);
        dsp_start_ticks = hal_sys_timer_get();
        transq_msg_onoff(0);
#ifdef __A7_DSP_AUDIO_TASK__
        app_mic_stop(MIC_APP_MICRECORD);
        osDelay(1500);
#endif
        TRACE(1, "%s %d", __FUNCTION__, __LINE__);

        hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
        hal_cmu_dsp_stop_cpu();
        TRACE(1, "%s %d", __FUNCTION__, __LINE__);
        hal_cmu_dsp_reset_set();
        hal_cmu_dsp_clock_disable();
        TRACE(1, "%s %d", __FUNCTION__, __LINE__);

        transq_msg_flush();
        transq_msg_reinit();
        TRACE(1, "%s %d", __FUNCTION__, __LINE__);
    }

    if (status == A7_USER_DISABLE) {
        hal_psramuhs_wakeup();
        osDelay(1);
    }
    while (0 != __a7_dsp_boot()) {
        retry++;
        TRACE(1|TR_ATTR_IMM, "__a7_dsp_boot fail, retry %d", retry);
        if (retry >= A7_DSP_BOOT_RETRY_NUM) {
            TRACE(0|TR_ATTR_IMM, "__a7_dsp_boot fail deadly, now reboot m33,set a7 reboot");
            hal_sw_bootmode_set(HAL_SW_BOOTMODE_A7_REBOOT);
            hal_cmu_sys_reboot();
            break;
        }
    }

    heartbeat_reset_timer(5*1000);

    TRACE(1, "%s %d", __FUNCTION__, __LINE__);

}

osThreadId a7_reboot_thread_id = NULL;
static void __a7_reboot_thread(void const* arg)
{
    TRACE(1, "\n\n%-32s start......\n", __FUNCTION__);
    __a7_dsp_reboot();
    TRACE(1, "%s exit......", __FUNCTION__);
    a7_reboot_thread_id = NULL;
    osThreadExit();
}
osThreadDef(__a7_reboot_thread, osPriorityAboveNormal, 1, 4*1024, "a7_reboot_thread");

void a7_dsp_reboot(void)
{
    /* check if previous reboot thread is running */
    if (a7_reboot_thread_id)
        return;

    a7_reboot_thread_id = osThreadCreate(osThread(__a7_reboot_thread), NULL);
    if (a7_reboot_thread_id == NULL){
        printf("%s %d fail to create a7_reboot_thread_id task\n",__func__,__LINE__);
    }
    printf("%s create a7_reboot_thread_id task = 0x%X", __func__, (uint32_t)a7_reboot_thread_id);
}

int is_a7_boot_done(void)
{
    return a7_boot_done;
}

void a7_dsp_boot(void)
{
    a7_dsp_set_bootnum(0);
    __a7_dsp_reboot();
    a7_boot_done = 1;
}

void a7_dsp_stop(void)
{
    a7_dsp_set_bootnum(0);
    hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
    hal_cmu_dsp_stop_cpu();
    printf("%s %d", __FUNCTION__, __LINE__);
    hal_cmu_dsp_reset_set();
    hal_cmu_dsp_clock_disable();
    printf("%s %d", __FUNCTION__, __LINE__);
    a7_boot_done = 0;
}
#endif

