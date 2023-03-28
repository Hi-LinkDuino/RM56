#ifndef __SPI_DEV_H__
#define __SPI_DEV_H__

#include "cmsis_os2.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_spi.h"
#include "hal_timer.h"

// #define CONFIG_SPI_SIMPLEX_TEST
#define CONFIG_SPI_SIMPLEX_DMA_TEST
// #define CONFIG_SPI_DUPLEX_TEST
// #define SPI_SLAVE

#ifdef RTOS
#define mdelay(ms) osDelay(ms)
#else
#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))
#endif

struct spi_dev {
    struct HAL_IOMUX_PIN_FUNCTION_MAP clk;
    struct HAL_IOMUX_PIN_FUNCTION_MAP cs;
    struct HAL_IOMUX_PIN_FUNCTION_MAP mosi;
    struct HAL_IOMUX_PIN_FUNCTION_MAP miso;
    struct HAL_SPI_CFG_T cfg;
#ifdef USE_DMA_CB
    osSemaphoreId_t sem;
#endif
    int (*open)(const struct HAL_SPI_CFG_T *cfg);
    int (*close)(uint32_t cs);
    int (*transfer)(const void *txbuf, void *rxbuf, uint32_t len);
    int (*dma_transfer)(const void *txbuf, void *rxbuf, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);
    void (*dma_cb)(int error);
};

#ifdef __cplusplus
extern "C" {
#endif

int spi_init(struct spi_dev *dev);
void spi_deinit(struct spi_dev *dev);
int spi_transfer(const struct spi_dev *dev, const void *txbuf, void *rxbuf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
