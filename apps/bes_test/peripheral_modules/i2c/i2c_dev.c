#include "i2c_dev.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include <string.h>

#ifdef CONFIG_I2C_HW
int i2c_init(struct i2c_dev *dev)
{
    if (dev->inited)
        return 0;

    if (dev->bus == HAL_I2C_ID_0)
        hal_iomux_set_i2c0();
    else
        hal_iomux_set_i2c1();

    if (hal_i2c_open(dev->bus, &dev->cfg) == 0) {
        dev->inited = 1;
        return 0;
    }
    return -1;
}

void i2c_deinit(struct i2c_dev *dev)
{
    hal_i2c_close(dev->bus);
}

uint32_t i2c_write(struct i2c_dev *dev, const uint8_t *txbuf, uint16_t txlen)
{
    uint32_t ret;
    if (dev->cfg.mode == HAL_I2C_API_MODE_SIMPLE)
        ret = hal_i2c_simple_send(dev->bus, dev->addr, txbuf, txlen);
    else
        ret = hal_i2c_task_send(dev->bus, dev->addr, txbuf, txlen, 0, NULL);

    if (ret) {
        printf("[%s] failed, dev_addr 0x%x, ret %u\r\n", __func__, dev->addr, ret);
    }
    return ret;
}

uint32_t i2c_read(struct i2c_dev *dev, uint8_t *rxbuf, uint16_t rxlen)
{
    uint32_t ret;
    if (dev->cfg.mode == HAL_I2C_API_MODE_SIMPLE)
        ret = hal_i2c_simple_recv(dev->bus, dev->addr, NULL, 0, rxbuf, rxlen);
    else
        ret = hal_i2c_task_recv(dev->bus, dev->addr, NULL, 0, rxbuf, rxlen, 0, NULL);

    if (ret) {
        printf("[%s] failed, dev_addr 0x%x, ret %d\r\n", __func__, dev->addr, ret);
    }
    return ret;
}
#endif