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

#ifdef CONFIG_UART_INT

#define UART_FRAME_BUFSIZE 512
#define UART_FIFO_BUFSIZE 512

static struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_4, // max 16 bytes received once, it does not work to change it to other values.
    .baud = 9600,
    .dma_rx = false,
    .dma_tx = false,
    .dma_rx_stop_on_err = false,
};

static enum HAL_UART_ID_T uart_id = HAL_UART_ID_2; ///< @see enum HAL_UART_ID_T
static struct kfifo uart_fifo;
static char uart_fifo_buf[UART_FIFO_BUFSIZE];

static void uart_tx(const void *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        hal_uart_blocked_putc(uart_id, *((char *)buf + i));
    }
}

static void uart_rx(void)
{
    static char buf[UART_FRAME_BUFSIZE] = {0};
    static int i = 0;
    while (kfifo_get(&uart_fifo, (unsigned char *)&buf[i], 1)) {
        if (buf[i] == '\n') {
            // just send back
            uart_tx(buf, i + 1);
            i = 0;
        } else {
            i = (i + 1) % UART_FRAME_BUFSIZE;
        }
    }
}

static void uart_rx_isr(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status)
{
    uint8_t buf[UART_FRAME_BUFSIZE] = {0};
    uint32_t i = 0;
    if (id != uart_id) {
        printf("unexpected interrupt from uart %d\r\n", id);
        return;
    }
    while (hal_uart_readable(id)) {
        ASSERT(i < sizeof(buf), "buf full");
        buf[i++] = hal_uart_getc(id);
    }
    if (i > 0) {
        // buf[i] = '\0';
        // printf("rx(%d): %s", i, buf);
        kfifo_put(&uart_fifo, buf, i);
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
    union HAL_UART_IRQ_T mask;
    printf("[%s:%d] %s\r\n", __FILE__, __LINE__, __func__);
    hal_set_uart_iomux(uart_id);
    hal_uart_open(uart_id, &uart_cfg);
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    mask.RT = 1;
    hal_uart_irq_set_mask(uart_id, mask);
    hal_uart_irq_set_handler(uart_id, uart_rx_isr);

    kfifo_init(&uart_fifo, (unsigned char *)uart_fifo_buf, UART_FIFO_BUFSIZE);

    while (1) {
        uart_rx();
        osDelay(10);
    }
}
#endif