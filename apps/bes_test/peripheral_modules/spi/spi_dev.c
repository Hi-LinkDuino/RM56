#include "spi_dev.h"
#include "hal_trace.h"

int spi_init(struct spi_dev *dev)
{
    ASSERT(dev, "dev mustn't be null");
    ASSERT(dev->open, "dev->open mustn't be null");

    hal_iomux_init(&dev->clk, 1);
    hal_iomux_init(&dev->cs, 1);
    hal_iomux_init(&dev->miso, 1);
    hal_iomux_init(&dev->mosi, 1);
    if (dev->cfg.slave) {
        hal_iomux_set_spilcd_slave();
    }
    // if master cs use as gpio, pull up at first
    if (!dev->cfg.slave && dev->cs.function == HAL_IOMUX_FUNC_AS_GPIO) {
        hal_gpio_pin_set_dir(dev->cs.pin, HAL_GPIO_DIR_OUT, 1);
    }
    if (dev->open(&dev->cfg)) {
        printf("spi open error\r\n");
        return -1;
    }
#ifdef USE_DMA_CB
    if (dev->cfg.dma_rx && dev->cfg.dma_tx && dev->dma_cb) {
        dev->sem = osSemaphoreNew(1, 0, NULL);
        ASSERT(dev->sem, "dev->sem cerate failed\r\n");
    }
#endif
    printf("spi clk=%u\r\n", dev->cfg.rate);
    return 0;
}

void spi_deinit(struct spi_dev *dev)
{
    if (!dev)
        return;

    if (dev->close)
        dev->close(0);

#ifdef USE_DMA_CB
    if (dev->sem) {
        osSemaphoreDelete(dev->sem);
        dev->sem = NULL;
    }
#endif
}

int spi_transfer(const struct spi_dev *dev, const void *txbuf, void *rxbuf, uint32_t len)
{
    int ret = 0;
    ASSERT(dev && txbuf && rxbuf && (len > 0), "arg error");
    if (!dev->cfg.slave && dev->cs.function == HAL_IOMUX_FUNC_AS_GPIO) { // cs is controlled by software
        hal_gpio_pin_clr(dev->cs.pin);
    }
    if (dev->cfg.dma_rx && dev->cfg.dma_tx) {
        ret = dev->dma_transfer(txbuf, rxbuf, len, dev->dma_cb);
#ifdef USE_DMA_CB
        if (dev->dma_cb)
            osSemaphoreAcquire(dev->sem, 1000);
#endif
    } else {
        ret = dev->transfer(txbuf, rxbuf, len);
    }
    if (!dev->cfg.slave && dev->cs.function == HAL_IOMUX_FUNC_AS_GPIO) {
        hal_gpio_pin_set(dev->cs.pin);
    }
    if (ret) {
        printf("spi_transfer error %d\r\n", ret);
    }
    return ret;
}
