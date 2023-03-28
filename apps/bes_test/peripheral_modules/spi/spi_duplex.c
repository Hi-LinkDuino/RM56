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
#include "spi_msg.h"
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_SPI_DUPLEX_TEST

#define BUFSIZE 1024
#define MSGSIZE 32

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
        .rate = 1000000,
        .clk_delay_half = true, // mode 3
        .clk_polarity = true,
#ifdef SPI_SLAVE
        .slave = true, // slave
#else
        .slave = false, // master
#endif
        .dma_rx = false,
        .dma_tx = false, // dma_rx == dma_tx always
        .rx_sep_line = false,
        .cs = 0,
        .tx_bits = 8, // data size = 8
        .rx_bits = 8,
        .rx_frame_bits = 0,
    },
    .open = hal_spilcd_open,
    .close = hal_spilcd_close,
    .transfer = hal_spilcd_recv,
};

static char txbuf[BUFSIZE];
static char rxbuf[BUFSIZE];

// spi msg transfer: hdr + payload at twice
int spi_msg_transfer(struct spi_dev *dev, struct spi_msg *txmsg, struct spi_msg *rxmsg)
{
    int len = (txmsg->len > 0) ? (txmsg->len) : (SPI_MSG_HDR_SIZE + MSGSIZE);
    if (spi_transfer(dev, txmsg, rxmsg, SPI_MSG_HDR_SIZE) != 0)
        return -1;
    mdelay(1); // !!delay is important, otherwise slave rx error
    memset(rxmsg, 0, len);
    if (spi_transfer(dev, txmsg->payload, rxmsg, len) != 0)
        return -1;
    return rxmsg->len;
}

void spi_master(void)
{
    char buf[MSGSIZE];
    int id = 0;
    struct spi_msg *msg = NULL;
    printf("spi master test...\r\n");
    while (1) {
        /* spi master write */
        snprintf(buf, sizeof(buf), "spi master msg %d", id++);
        msg = (struct spi_msg *)txbuf;
        spi_msg_encode(msg, SPI_WRITE, buf, sizeof(buf));
        spi_msg_transfer(&dev, msg, (struct spi_msg *)rxbuf);
        printf("spi master tx: %s\r\n", buf);
        mdelay(500);

        /* spi master read */
        spi_msg_encode(msg, SPI_READ, NULL, 0);
        spi_msg_transfer(&dev, msg, (struct spi_msg *)rxbuf);
        msg = (struct spi_msg *)rxbuf;
        if (spi_msg_decode(msg) > 0) {
            printf("spi master rx: %s\r\n", msg->payload);
        } else {
            spi_print_hdr("spi master rx error", msg);
        }
        mdelay(500);
    }
}

static int spi_slave_msg_handler(struct spi_msg *rxmsg, struct spi_msg *txmsg)
{
    if (rxmsg->type == SPI_WRITE) {
        printf("spi slave rx: %s\r\n", rxmsg->payload);
        return 0;
    } else if (rxmsg->type == SPI_READ) {
        static int id = 0;
        char buf[MSGSIZE];
        snprintf(buf, MSGSIZE, "spi slave msg %d", id++);
        return spi_msg_encode(txmsg, SPI_READ, buf, MSGSIZE);
    }
    return 0;
}

void spi_slave(void)
{
    struct spi_msg *msg = NULL;
    int cstate = SPI_RX_HDR;
    int event, ret;
    printf("spi slave test...\r\n");
    while (1) {
        switch (cstate) {
        case SPI_RX_HDR: {
            memset(rxbuf, 0, sizeof(rxbuf));
            msg = (struct spi_msg *)rxbuf;
            if (spi_transfer(&dev, txbuf, rxbuf, SPI_MSG_HDR_SIZE) == 0) {
                ret = spi_msg_check_hdr(msg);
                if (ret < 0) {
                    spi_print_hdr("slave hdr error", msg);
                    event = SPI_ERR;
                } else if (ret == 0) {
                    event = SPI_RX_HDR_WITHOUT_DATA_DONE;
                } else {
                    event = SPI_RX_HDR_WITH_DATA_DONE;
                }
            } else {
                event = SPI_ERR;
            }

            if (event == SPI_RX_HDR_WITHOUT_DATA_DONE) {
                int tx = spi_slave_msg_handler(msg, (struct spi_msg *)txbuf);
                if (tx)
                    cstate = SPI_TX_DATA;
                else
                    cstate = SPI_RX_HDR;
            } else if (event == SPI_RX_HDR_WITH_DATA_DONE)
                cstate = SPI_RX_DATA;
            else
                cstate = SPI_RX_HDR;

            break;
        }
        case SPI_RX_DATA: {
            msg = (struct spi_msg *)rxbuf;
            if (spi_transfer(&dev, txbuf, msg->payload, msg->len) == 0) {
                ret = spi_msg_decode(msg);
                if (ret <= 0) {
                    spi_print_payload("slave rx error", msg);
                    event = SPI_ERR;
                } else {
                    event = SPI_RX_DATA_DONE;
                }
            } else {
                event = SPI_ERR;
            }

            if (event == SPI_RX_DATA_DONE) {
                int tx = spi_slave_msg_handler(msg, (struct spi_msg *)txbuf);
                if (tx)
                    cstate = SPI_TX_DATA;
                else
                    cstate = SPI_RX_HDR;
            } else
                cstate = SPI_RX_HDR;
            break;
        }
        case SPI_TX_DATA: {
            msg = (struct spi_msg *)txbuf;
            spi_transfer(&dev, txbuf, msg, msg->len + SPI_MSG_HDR_SIZE);
            cstate = SPI_RX_HDR;
            break;
        }
        default:
            cstate = SPI_RX_HDR;
            break;
        }
    }
}

void spi_test(void)
{
    printf("%s enter\r\n", __func__);
    if (spi_init(&dev) != 0) {
        printf("spi_init failed\r\n");
        return;
    }
    printf("spi_init succeed\r\n");

    if (dev.cfg.slave) {
        spi_slave();
    } else {
        spi_master();
    }

    printf("%s leave\r\n", __func__);
}
#endif
