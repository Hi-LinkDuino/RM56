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
#include "hal_trace.h"
#include "spi_dev.h"
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_SPI_SIMPLEX_DMA_TEST

#define BUFSIZE 32

static struct spi_dev dev = {
#ifdef SPI_SLAVE
    .clk = {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    .cs = {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    .mosi = {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    .miso = {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SPILCD_DI0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
#else
    .clk = {HAL_IOMUX_PIN_P2_3, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    .cs = {HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    .mosi = {HAL_IOMUX_PIN_P2_1, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    .miso = {HAL_IOMUX_PIN_P2_0, HAL_IOMUX_FUNC_SPILCD_DI0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
#endif
    .cfg = {
        .rate = 4000000,
        .clk_delay_half = true, // mode 3
        .clk_polarity = true,
#ifdef SPI_SLAVE
        .slave = true, // slave
#else
        .slave = false, // master
#endif
        .dma_rx = true,
        .dma_tx = true, // dma_rx == dma_tx always
        .rx_sep_line = false,
        .cs = 0,
        .tx_bits = 8, // data size = 8
        .rx_bits = 8,
        .rx_frame_bits = 0,
    },
    .open = hal_spilcd_open,
    .close = hal_spilcd_close,
    .transfer = hal_spilcd_recv,
    .dma_transfer = hal_spilcd_dma_recv,
    .dma_cb = NULL,
};

void spi_test(void)
{
    char txbuf[BUFSIZE], rxbuf[BUFSIZE];
    printf("%s enter\r\n", __func__);
    if (spi_init(&dev) != 0) {
        printf("spi_init failed\r\n");
        return;
    }
    printf("spi_init succeed\r\n");

    if (!dev.cfg.slave) {
        int id = 0;
        printf("spi master send test...\r\n");
        while (1) {
            snprintf(txbuf, BUFSIZE, "spi master msg %d", id++);
            if (spi_transfer(&dev, txbuf, rxbuf, BUFSIZE) == 0) {
                printf("tx: %s\r\n", txbuf);
            } else {
                printf("spi master tx msg %d failed\r\n", id - 1);
            }
            mdelay(100);
        }
    } else {
        printf("spi slave recv test...\r\n");
        // uint32_t now = GET_CURRENT_MS();
        // uint32_t bytes = 0;
        while (1) {
            memset(rxbuf, 0, BUFSIZE);
            if (spi_transfer(&dev, txbuf, rxbuf, BUFSIZE) == 0) {
                rxbuf[BUFSIZE - 1] = '\0';
                printf("rx: %s\r\n", rxbuf);
                // if (strncmp(rxbuf, "spi master", 10) != 0)
                //     goto _err;

                // bytes += BUFSIZE;
                // if (GET_CURRENT_MS() - now >= 1000) {
                //     printf("spi rate %u bytes/sec\r\n", bytes);
                //     now = GET_CURRENT_MS();
                //     bytes = 0;
                // }
            } else {
                // _err :
                printf("spi slave rx err: %s\r\n", rxbuf);
                // now = GET_CURRENT_MS();
                // bytes = 0;
            }
            // mdelay(1);
        }
    }

    printf("%s leave\r\n", __func__);
}
#endif
