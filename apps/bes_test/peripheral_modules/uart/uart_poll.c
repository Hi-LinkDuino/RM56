/**
 * @brief   A uart echo demo with hal library.
 */
#include "cmsis_os2.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include "hal_uart.h"
#include "kfifo.h"
#include "uart.h"
#include <stdint.h>

#ifdef CONFIG_UART_POLL

#define UART_FRAME_BUFSIZE 512

static struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_4, // max 16 bytes received once, it does not work to change it to other values.
    .baud = 1200,                        // change from 115200 to 1200, it can receive more than 16 bytes once.
    .dma_rx = false,
    .dma_tx = false,
    .dma_rx_stop_on_err = false,
};

static enum HAL_UART_ID_T uart_id = HAL_UART_ID_2; ///< @see enum HAL_UART_ID_T
static uint8_t uart_buf[UART_FRAME_BUFSIZE];

static void uart_tx(const void *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        hal_uart_blocked_putc(uart_id, *((char *)buf + i));
    }
}

static void uart_rx(void)
{
    static uint32_t i = 0;
    while (hal_uart_readable(uart_id)) {
        ASSERT(i < sizeof(uart_buf), "buf full");
        uart_buf[i++] = hal_uart_getc(uart_id);
    }
    if (i > 0) {
        // uart_buf[i] = '\0';
        // printf("rx(%d): %s", i, uart_buf);
        uart_tx(uart_buf, i);
        i = 0;
    }
}

/**
 * uart iomux default
 * uart0 rx/tx - gpio 16/17
 * uart1 rx/tx - gpio 32/03
 * uart2 rx/tx - gpio 22/23
 */
static void hal_set_uart_iomux(enum HAL_UART_ID_T uart_id)
{
    if (uart_id == HAL_UART_ID_0) {
        hal_iomux_set_uart0();
    } else if (uart_id == HAL_UART_ID_1) {
        hal_iomux_set_uart1();
    } else {
        hal_iomux_set_uart2();
    }
}

void uart_test(void)
{
    printf("[%s:%d] %s\r\n", __FILE__, __LINE__, __func__);
    hal_set_uart_iomux(uart_id);
    hal_uart_open(uart_id, &uart_cfg);

    while (1) {
        uart_rx();
        osDelay(10);
    }
}

#endif