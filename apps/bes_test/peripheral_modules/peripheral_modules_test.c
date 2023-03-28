#include "peripheral_modules_test.h"
#include "hal_trace.h"

extern void i2s_loop_dac_i2s_test(uint8_t channel);

int bes_peripheral_modules_test(void)
{
    printf("%s enter\r\n", __func__);

#ifdef GPIO_PERIPH_TEST
    gpio_test();
#endif

#ifdef SPI_PERIPH_TEST
    spi_test();
#endif

#ifdef UART_PERIPH_TEST
    uart_test();
#endif

#ifdef TIMER_PERIPH_TEST
    hwtimer_test();
#endif

#ifdef DMA_PERIPH_TEST
    dma_test();
#endif

#ifdef PWM_PERIPH_TEST
    pwm_test();
#endif

#ifdef I2C_PERIPH_TEST
    i2c_test();
    // aht10_test();
#endif

#ifdef WATCHDOG_PERIPH_TEST
    watchdog_test();
#endif

#ifdef I2S_PERIPH_TEST
    i2s_loop_dac_i2s_test(2);
#endif
    printf("%s leave\r\n", __func__);
    return 0;
}