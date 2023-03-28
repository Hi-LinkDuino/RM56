#ifndef __PERIPHERAL_MODULES_TEST_H__
#define __PERIPHERAL_MODULES_TEST_H__

#include "hal_timer.h"
#include "uart/uart.h"

#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))

#ifdef __cplusplus
extern "C" {
#endif

int bes_peripheral_modules_test(void);

int aht10_test(void);
void gpio_test(void);
void uart_test(void);
void hwtimer_test(void);
void spi_test(void);
void dma_test(void);
void pwm_test(void);
void watchdog_test(void);
void i2c_test(void);
void i2c_sim_test(void);

#ifdef __cplusplus
}
#endif

#endif
