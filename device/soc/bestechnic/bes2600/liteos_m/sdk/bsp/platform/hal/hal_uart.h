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
#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CHIP_HAS_UART

#include "plat_types.h"
#include "stdbool.h"
#include "stdint.h"
#include "hal_dma.h"

//#define BT_UART

#define HAL_UART_DMA_TRANSFER_STEP                  0xFFF
#ifdef __KNOWLES
#define HAL_UART_DMA_TRANSFER_STEP_PINGPANG         640
#else
#define HAL_UART_DMA_TRANSFER_STEP_PINGPANG         4
#endif

enum HAL_UART_ID_T {
    HAL_UART_ID_0 = 0,
#if (CHIP_HAS_UART >= 2)
    HAL_UART_ID_1,
#endif
#if (CHIP_HAS_UART >= 3)
    HAL_UART_ID_2,
#endif
#if (CHIP_HAS_UART >= 4)
    HAL_UART_ID_3,
#endif
#ifdef BT_UART
    HAL_UART_ID_BT,
#endif

    HAL_UART_ID_QTY
};

enum HAL_UART_PARITY_T {
    HAL_UART_PARITY_NONE,
    HAL_UART_PARITY_ODD,
    HAL_UART_PARITY_EVEN,
    HAL_UART_PARITY_FORCE1,
    HAL_UART_PARITY_FORCE0,
};

enum HAL_UART_STOP_BITS_T {
    HAL_UART_STOP_BITS_1,
    HAL_UART_STOP_BITS_2,
};

enum HAL_UART_DATA_BITS_T {
    HAL_UART_DATA_BITS_5,
    HAL_UART_DATA_BITS_6,
    HAL_UART_DATA_BITS_7,
    HAL_UART_DATA_BITS_8,
};

enum HAL_UART_FLOW_CONTROL_T {
    HAL_UART_FLOW_CONTROL_NONE,
    HAL_UART_FLOW_CONTROL_RTS,
    HAL_UART_FLOW_CONTROL_CTS,
    HAL_UART_FLOW_CONTROL_RTSCTS,
};

enum HAL_UART_FIFO_LEVEL_T {
    HAL_UART_FIFO_LEVEL_1_8,
    HAL_UART_FIFO_LEVEL_1_4,
    HAL_UART_FIFO_LEVEL_1_2,
    HAL_UART_FIFO_LEVEL_3_4,
    HAL_UART_FIFO_LEVEL_7_8,
};

enum HAL_UART_XFER_TYPE_T {
    HAL_UART_XFER_TYPE_TX       = (1 << 0),
    HAL_UART_XFER_TYPE_RX       = (1 << 1),
};

struct HAL_UART_CFG_T {
    enum HAL_UART_PARITY_T parity;
    enum HAL_UART_STOP_BITS_T stop;
    enum HAL_UART_DATA_BITS_T data;
    enum HAL_UART_FLOW_CONTROL_T flow;
    enum HAL_UART_FIFO_LEVEL_T rx_level;
    enum HAL_UART_FIFO_LEVEL_T tx_level;
    uint32_t baud;
    bool dma_rx : 1;
    bool dma_tx : 1;
    bool dma_rx_stop_on_err : 1;
};

struct HAL_UART_BUF_T {
    uint8_t *buf;
    uint32_t len;
    bool irq;
    bool loop_hdr;
};

union HAL_UART_STATUS_T {
    struct {
        uint32_t FE   :1; // frame error
        uint32_t PE   :1; // parity error
        uint32_t BE   :1; // break error
        uint32_t OE   :1; // overrun error
    };
    uint32_t reg;
};

union HAL_UART_FLAG_T {
    struct {
        uint32_t CTS  :1;
        uint32_t DSR  :1;
        uint32_t DCD  :1;
        uint32_t BUSY :1;
        uint32_t RXFE :1; // rx fifo empty
        uint32_t TXFF :1; // tx fifo full
        uint32_t RXFF :1; // rx fifo full
        uint32_t TXFE :1; // tx fifo empty
        uint32_t RI   :1; // ring indicator
    };
    uint32_t reg;
};

union HAL_UART_IRQ_T {
    struct {
        uint32_t RIM  :1; // ri
        uint32_t CTSM :1; // cts
        uint32_t DCDM :1; // dcd
        uint32_t DSRM :1; // dsr
        uint32_t RX   :1; // rx
        uint32_t TX   :1; // tx
        uint32_t RT   :1; // receive timeout
        uint32_t FE   :1; // framing error
        uint32_t PE   :1; // parity error
        uint32_t BE   :1; // break error
        uint32_t OE   :1; // overrun
    };
    uint32_t reg;
};

typedef void (*HAL_UART_IRQ_HANDLER_T)(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status);

typedef void (*HAL_UART_IRQ_RXDMA_HANDLER_T)(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status);

typedef void (*HAL_UART_IRQ_TXDMA_HANDLER_T)(uint32_t xfer_size, int dma_error);

int hal_uart_open(enum HAL_UART_ID_T id, const struct HAL_UART_CFG_T *cfg);

int hal_uart_reopen(enum HAL_UART_ID_T id, const struct HAL_UART_CFG_T *cfg);

int hal_uart_close(enum HAL_UART_ID_T id);

int hal_uart_opened(enum HAL_UART_ID_T id);

void hal_uart_sleep(void);

void hal_uart_wakeup(void);

int hal_uart_change_baud_rate(enum HAL_UART_ID_T id, uint32_t rate);

int hal_uart_pause(enum HAL_UART_ID_T id, enum HAL_UART_XFER_TYPE_T type);

int hal_uart_continue(enum HAL_UART_ID_T id, enum HAL_UART_XFER_TYPE_T type);

int hal_uart_readable(enum HAL_UART_ID_T id);

int hal_uart_writable(enum HAL_UART_ID_T id);

uint8_t hal_uart_getc(enum HAL_UART_ID_T id);

int hal_uart_putc(enum HAL_UART_ID_T id, uint8_t c);

uint8_t hal_uart_blocked_getc(enum HAL_UART_ID_T id);

int hal_uart_blocked_putc(enum HAL_UART_ID_T id, uint8_t c);

union HAL_UART_FLAG_T hal_uart_get_flag(enum HAL_UART_ID_T id);

union HAL_UART_STATUS_T hal_uart_get_status(enum HAL_UART_ID_T id);

void hal_uart_clear_status(enum HAL_UART_ID_T id);

void hal_uart_break_set(enum HAL_UART_ID_T id);

void hal_uart_break_clear(enum HAL_UART_ID_T id);

void hal_uart_flush(enum HAL_UART_ID_T id, uint32_t ticks);

union HAL_UART_IRQ_T hal_uart_get_raw_irq(enum HAL_UART_ID_T id);

void hal_uart_clear_irq(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T irq);

union HAL_UART_IRQ_T hal_uart_irq_get_mask(enum HAL_UART_ID_T id);

union HAL_UART_IRQ_T hal_uart_irq_set_mask(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T mask);

HAL_UART_IRQ_HANDLER_T hal_uart_irq_set_handler(enum HAL_UART_ID_T id, HAL_UART_IRQ_HANDLER_T handler);

void hal_uart_irq_set_dma_handler(enum HAL_UART_ID_T id, HAL_UART_IRQ_RXDMA_HANDLER_T rxdma, HAL_UART_IRQ_TXDMA_HANDLER_T txdma);

int hal_uart_dma_recv(enum HAL_UART_ID_T id, uint8_t *buf, uint32_t len,
                      struct HAL_DMA_DESC_T *desc, uint32_t *desc_cnt);

int hal_uart_dma_recv_pingpang(enum HAL_UART_ID_T id, uint8_t *buf, uint32_t len,
                      struct HAL_DMA_DESC_T *desc, uint32_t *desc_cnt);


int hal_uart_dma_recv_mask(enum HAL_UART_ID_T id, uint8_t *buf, uint32_t len,
                           struct HAL_DMA_DESC_T *desc, uint32_t *desc_cnt,
                           const union HAL_UART_IRQ_T *mask);

int hal_uart_dma_recv_mask_pingpang(enum HAL_UART_ID_T id, uint8_t *buf, uint32_t len,
                                    struct HAL_DMA_DESC_T *desc, uint32_t *desc_cnt,
                                    const union HAL_UART_IRQ_T *mask, uint32_t step);

int hal_uart_dma_recv_mask_stream(enum HAL_UART_ID_T id, uint8_t *buf, uint32_t len,
                                  struct HAL_DMA_DESC_T *desc, uint32_t *desc_cnt,
                                  const union HAL_UART_IRQ_T *mask, uint32_t step);

int hal_uart_dma_recv_mask_buf_list(enum HAL_UART_ID_T id, const struct HAL_UART_BUF_T *ubuf, uint32_t ucnt,
                                    struct HAL_DMA_DESC_T *desc, uint32_t *desc_cnt, const union HAL_UART_IRQ_T *mask);

uint32_t hal_uart_get_dma_recv_addr(enum HAL_UART_ID_T id);

uint32_t hal_uart_stop_dma_recv(enum HAL_UART_ID_T id);

int hal_uart_dma_send(enum HAL_UART_ID_T id, const uint8_t *buf, uint32_t len,
                      struct HAL_DMA_DESC_T *desc, uint32_t *desc_cnt);

uint32_t hal_uart_stop_dma_send(enum HAL_UART_ID_T id);


// ========================================================================
// Test function

int hal_uart_printf_init(void);

void hal_uart_printf_output(const uint8_t *buf, uint32_t len);

static inline void hal_uart_output(const uint8_t *buf, uint32_t len)
{
    return hal_uart_printf_output(buf, len);
}

void hal_uart_printf(const char *fmt, ...);

#endif // CHIP_HAS_UART

#ifdef __cplusplus
}
#endif

#endif

