/***************************************************************************
 * Copyright 2015-2021 BES.
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
 ***************************************************************************/

#include "hal_spi.h"
#include "string.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_location.h"
#include "cmsis.h"
#include "alsa.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "app_utils.h"
#include "kfifo.h"
#include "hal_dma.h"
#include "cqueue.h"

#ifdef SPI_AUDIO_TEST

unsigned int spi_stereo_pcm[] = {
#include "super_marie_stereo_pcm"
};
alsa_handle_t * spi_master_handle;

/* master send to slave test data */
#define BufferSize 256
uint8_t spi_master_buffer_tx[BufferSize] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
    };

/* slave recieve spi master data */
#define SPI_AUDIO_RX_SIZE 1024
SRAM_BSS_LOC uint8_t spi_slave_buffer_rx[SPI_AUDIO_RX_SIZE*2] = {0};

static void spi_send_test(void)
{
    uint32_t remain_len = BufferSize;
    uint32_t master_pos = 0;
    uint32_t count = 0;
    uint32_t i = 0;

    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 1);
    TRACE(3, "spi master SEND test");
    osDelay(5000);
test_start:
    while (remain_len > 0) {
        hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 0);
        hal_spilcd_dma_send((void*)(spi_master_buffer_tx + master_pos), 16, NULL);
        hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 1);
        DUMP8("0x%x ", spi_master_buffer_tx+master_pos, 16);
        printf("\n");
        master_pos += 16;
        remain_len -= 16;
        count++;
        osDelay(1);
    }
    TRACE(3, "spi master SEND done count=%d", count);
    if (count == 160) {
        return;
    }
    remain_len = BufferSize;
    master_pos = 0;
    for(i = 0; i < BufferSize; i++) {
        spi_master_buffer_tx[i]++;
    }
    goto test_start;
}

/*transfer a piece of music*/
static void send_audio_via_spi(void)
{
    int pcm_pos = (int)&spi_stereo_pcm[0];
    int remain_len = sizeof(spi_stereo_pcm);

    int master_pos = (int)&spi_stereo_pcm[0];
    uint32_t master_count = 0;
    TRACE(0, "sizeof super_marie_stereo_pcm = %d", sizeof(spi_stereo_pcm));

    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 1);
    TRACE(3, "spi master send audio stream test");
    osDelay(5000);

    while (remain_len > 0) {
        if (remain_len < 1024) {
            /*master play*/
            alsa_write(spi_master_handle, (uint8_t *)master_pos, remain_len);
            hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 0);
            hal_spilcd_dma_send((void*)pcm_pos, remain_len, NULL);
            hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 1);
            remain_len = 0;
        } else {
            /*master play*/
            alsa_write(spi_master_handle, (uint8_t *)master_pos, 1024);
            hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 0);
            /*send to slave*/
            hal_spilcd_dma_send((void*)pcm_pos, 1024, NULL);
            master_count++;
            TRACE(2,"master count = %d", master_count);
            hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 1);
            pcm_pos += 1024;
            master_pos += 1024;
            remain_len -= 1024;
        }
        hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 1);
        osDelay(5);
    }
}

int spi_audio_master_init(void)
{
    int ret;
    struct HAL_SPI_CFG_T spi_cfg;

    memset(&spi_cfg, 0, sizeof(struct HAL_SPI_CFG_T));
    spi_cfg.slave = false;
    spi_cfg.rate = 20000000;
    spi_cfg.rx_bits = 8;
    spi_cfg.tx_bits = 16;
    spi_cfg.cs = 0;
    spi_cfg.dma_rx = true;
    spi_cfg.dma_tx = true;
    spi_cfg.clk_delay_half = true;
    spi_cfg.clk_polarity = true;
    spi_cfg.rx_frame_bits = 0;

    //SPI master mode
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd2[] = {
        //{HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SPILCD_DI0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        //{HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };
    hal_iomux_init(pinmux_spilcd2, ARRAY_SIZE(pinmux_spilcd2));

    spi_master_handle = alsa_open(ALSA_MODE_OUT, 16000, 2, ALSA_PCM_FORMAT_S16_LE);
    if (spi_master_handle) {
        alsa_start(spi_master_handle);
    }
    ret = hal_spilcd_open(&spi_cfg);
    if (ret != 0) {
        TRACE(3,"spi master open error");
        return ret;
    } else {
        TRACE(3,"spi master open succ");
    }
    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P2_2, HAL_GPIO_DIR_OUT, 1);
    send_audio_via_spi();
    //spi_send_test();
    TRACE(3,"spi master send done");
    return 0;
}

#ifdef SPI_SLAVE
static CQueue spi_audio_queue = {0};
#define PCM_STREAM_SPI_FIFO_SIZE (1024*16)
static uint8_t pcm_spi_audio_buff[PCM_STREAM_SPI_FIFO_SIZE];
static uint8_t spi_playback_buf[1024*2];
alsa_handle_t * spi_alsa_handle;
osThreadId spi_audio_thread_tid;
#define SPI_AUDIO_SIGNAL_ID 0xdd
static void spi_audio_play_thread(void const *argument);
osThreadDef(spi_audio_play_thread, osPriorityAboveNormal, 1, 1024*4, "spi_audio_play_thread");
static uint8_t g_pp_index = 0;

void pcm_spi_stream_fifo_init()
{
    if (spi_audio_thread_tid) {
        osSignalClear(spi_audio_thread_tid, SPI_AUDIO_SIGNAL_ID);
    }

    memset(spi_slave_buffer_rx,0,sizeof(spi_slave_buffer_rx));
    InitCQueue(&spi_audio_queue, PCM_STREAM_SPI_FIFO_SIZE,
                (CQItemType *)pcm_spi_audio_buff);
}

uint32_t pcm_spi_get_fifo_data(uint8_t *buf)
{
    uint32_t avail = 0;

    avail = LengthOfCQueue(&spi_audio_queue);
    if (avail > 0) {
        if (avail < SPI_AUDIO_RX_SIZE) {
            TRACE(2,"%s: Invalid  SPI STREAM SIZE: %d", __FUNCTION__, avail);
            return 0;
        } else {
            if(!DeCQueue(&spi_audio_queue, buf, SPI_AUDIO_RX_SIZE))
                return SPI_AUDIO_RX_SIZE;
            TRACE(2,"%s: queue get data error len %d", __func__, SPI_AUDIO_RX_SIZE);
            return 0;
        }
    } else {
        TRACE(0,"uart no stream to get");
        return 0;
    }
}

uint32_t avil_len_of_the_fifo()
{
	return LengthOfCQueue(&spi_audio_queue);
}

static void spi1_recv_dma_irq(uint32_t error)
{
    int32_t ret;
    if(!EnCQueue(&spi_audio_queue, (uint8_t *)&spi_slave_buffer_rx[g_pp_index*SPI_AUDIO_RX_SIZE], SPI_AUDIO_RX_SIZE)) {
        ret = SPI_AUDIO_RX_SIZE;
    }
    if (ret < SPI_AUDIO_RX_SIZE) {
        TRACE(2,"%s:WARNING !!! SPI STREAM OVERFLOW Dropping %d Bytes", __FUNCTION__, (SPI_AUDIO_RX_SIZE - ret));
    }
    osSignalSet(spi_audio_thread_tid, SPI_AUDIO_SIGNAL_ID);
    g_pp_index = !g_pp_index;
}

static void spi_audio_play_thread(void const *argument)
{
    uint32_t len = 0;
    osEvent evt;
    TRACE(3,"spi_audio_play_thread func start");

    while(1) {
        evt = osSignalWait(SPI_AUDIO_SIGNAL_ID, osWaitForever);
        if (evt.status == osEventSignal) {
            len = pcm_spi_get_fifo_data(spi_playback_buf);
            alsa_write(spi_alsa_handle, spi_playback_buf, len);
        } else {
            TRACE(0,"[%s] ERROR: evt.status = %d", __func__, evt.status);
            continue;
        }
    }
}

static struct HAL_DMA_DESC_T spi_dma_desc[17] = {0};
#define ARRAY_SIZE(a)        (sizeof(a)/sizeof(a[0]))
int spi_audio_slave_init(void)
{
    int ret;
    struct HAL_SPI_CFG_T spi_cfg;

    memset(&spi_cfg, 0, sizeof(struct HAL_SPI_CFG_T));
    spi_cfg.slave = true;
    spi_cfg.rate = 160000000;
    spi_cfg.rx_bits = 16;
    spi_cfg.tx_bits = 8;
    spi_cfg.cs = 0;
    spi_cfg.dma_rx = true;
    spi_cfg.dma_tx = true;
    spi_cfg.clk_delay_half = true;
    spi_cfg.clk_polarity = true;
    spi_cfg.rx_frame_bits = 0;

    uint32_t desc_cnt = ARRAY_SIZE(spi_dma_desc);
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd[] = {
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SPILCD_DI0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        //{HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };
    hal_iomux_init(pinmux_spilcd, ARRAY_SIZE(pinmux_spilcd));

    hal_iomux_set_spilcd_slave();

    ret = hal_spilcd_open(&spi_cfg);
    if (ret != 0) {
        TRACE(3,"spi slave open error");
        return ret;
    } else {
        TRACE(3,"spi slave open succ");
    }
    spi_alsa_handle = alsa_open(ALSA_MODE_OUT, 16000, 2, ALSA_PCM_FORMAT_S16_LE);
   if (spi_alsa_handle) {
        alsa_start(spi_alsa_handle);
    }
    pcm_spi_stream_fifo_init();
    TRACE(2, "%s %s create spi_audio_play_thread", __FILE__, __func__);
    spi_audio_thread_tid = osThreadCreate(osThread(spi_audio_play_thread), NULL);

    hal_spi_slave_dma_recv(&spi_slave_buffer_rx[0], SPI_AUDIO_RX_SIZE*2, (HAL_SPI_DMA_HANDLER_T)spi1_recv_dma_irq,
                           &spi_dma_desc[0], &desc_cnt, SPI_RX_DMA_MODE_PINGPANG, SPI_AUDIO_RX_SIZE);
}
#endif //endif SPI_SLAVE

void bes_spi_audio_test(void)
{
    #ifndef SPI_SLAVE
        TRACE(2, "spi audio master init");
        spi_audio_master_init();
    #else
        TRACE(2, "spi audio slave init");
        spi_audio_slave_init();
    #endif
}

#endif //endif SPI_AUDIO_TEST