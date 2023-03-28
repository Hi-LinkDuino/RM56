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
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_UART_DMA

#define UART_DMA_BUFSIZE 512
#define UART_FRAME_BUFSIZE 512
#define UART_FIFO_BUFSIZE 512

static struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_4, ///< other fifo_level may works
    .baud = 921600,
    .dma_rx = true,
    .dma_tx = true,
    .dma_rx_stop_on_err = false,
};

static enum HAL_UART_ID_T uart_id = HAL_UART_ID_2; ///< @see enum HAL_UART_ID_T
static struct kfifo uart_fifo;
static char uart_fifo_buf[UART_FIFO_BUFSIZE];
static char uart_dma_buf[UART_DMA_BUFSIZE];
#ifdef RTOS
static osSemaphoreId_t uart_sem;
#else
static volatile int uart_rx_flag = 0;
#endif

#ifdef UART_LOOPBACK_TEST
#ifdef RTOS
static osTimerId_t uart_timer;
#else
static HWTIMER_ID uart_timer = NULL;
#endif
#endif

static void uart_tx(const void *buf, int len)
{
    if (!uart_cfg.dma_tx) {
        int i;
        for (i = 0; i < len; i++) {
            hal_uart_blocked_putc(uart_id, *((char *)buf + i));
        }
    } else {
        hal_uart_dma_send(uart_id, (const unsigned char *)buf, len, NULL, NULL);
    }
}

static void uart_rx(void)
{
    unsigned int ret;
    int len = 0;
    char buf[UART_FRAME_BUFSIZE];

    ret = kfifo_get(&uart_fifo, (unsigned char *)&len, sizeof(int));
    if (ret != sizeof(int) || len <= 0) {
        printf("kfifo_get len error\r\n");
        return;
    }
    len = MIN(len, sizeof(buf) - 1);
    ret = kfifo_get(&uart_fifo, (unsigned char *)buf, len);
    if (ret <= 0 || ret > len) {
        printf("kfifo_get data error\r\n");
        return;
    }
    // buf[ret] = '\0'; ///< ret<=sizeof(buf)-1
    // printf("rx: %s", buf);
    // just send back
    uart_tx(buf, ret);
}

static void uart_rx_start(void)
{
    struct HAL_DMA_DESC_T dma_desc_rx;
    unsigned int desc_cnt = 1;
    union HAL_UART_IRQ_T mask;

    mask.reg = 0;
    mask.BE = 0;
    mask.FE = 0;
    mask.OE = 0;
    mask.PE = 0;
    mask.RT = 1;

    hal_uart_dma_recv_mask(uart_id, (uint8_t *)uart_dma_buf, UART_DMA_BUFSIZE, &dma_desc_rx, &desc_cnt, &mask);
}

static void uart_dma_rx_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    if (dma_error != 0 || xfer_size == 0) {
        printf("dma error: %d, xfer_size: %d\r\n", dma_error, xfer_size);
        return;
    }
    // printf("dma rxlen: %d\r\n", xfer_size);
    kfifo_put(&uart_fifo, (unsigned char *)&xfer_size, sizeof(xfer_size));
    kfifo_put(&uart_fifo, (unsigned char *)uart_dma_buf, xfer_size);
#ifdef RTOS
    osSemaphoreRelease(uart_sem);
#else
    uart_rx_flag = 1;
#endif
    uart_rx_start();
}

#ifdef UART_LOOPBACK_TEST
static void uart_timer_handler(void *arg)
{
    char msg[64];
    static int i = 0;
    snprintf(msg, sizeof(msg), "msg %d\r\n", ++i);
    printf("tx: %s", msg);
    uart_tx(msg, strlen(msg));
#ifndef RTOS
    hwtimer_start(uart_timer, MS_TO_TICKS(1000));
#endif
}
#endif

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
    hal_uart_irq_set_dma_handler(uart_id, uart_dma_rx_handler, NULL);

    kfifo_init(&uart_fifo, (unsigned char *)uart_fifo_buf, UART_FIFO_BUFSIZE);
#ifdef RTOS
    uart_sem = osSemaphoreNew(1, 0, NULL);
#endif

#ifdef UART_LOOPBACK_TEST
#ifdef RTOS
    uart_timer = osTimerNew(uart_timer_handler, osTimerPeriodic, NULL, NULL);
    osTimerStart(uart_timer, 1000);
#else
    uart_timer = hwtimer_alloc(uart_timer_handler, 0);
    hwtimer_start(uart_timer, MS_TO_TICKS(1000));
#endif
#endif

    uart_rx_start();
    printf("start uart loop...\r\n");
    while (1) {
#ifdef RTOS
        osSemaphoreAcquire(uart_sem, osWaitForever);
        uart_rx();
#else
        while (uart_rx_flag == 0)
            ;
        uart_rx_flag = 0;
        uart_rx();
#endif
    }
}
#endif