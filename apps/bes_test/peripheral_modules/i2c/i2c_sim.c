#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "i2c_dev.h"

#ifdef CONFIG_I2C_SW

#define SCL HAL_GPIO_PIN_P0_4
#define SDA HAL_GPIO_PIN_P0_5

// #define delay() hal_sys_timer_delay(US_TO_TICKS(4)) // half an i2c clk period
#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))

static void delay()
{
    uint32_t ticks = US_TO_FAST_TICKS(3);
    uint32_t now = hal_fast_sys_timer_get();
    while (now + ticks > hal_fast_sys_timer_get())
        ;
}

int i2c_init()
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
        {SCL, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {SDA, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };
    hal_iomux_init(pinmux, 2);
    hal_gpio_pin_set_dir(SCL, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_OUT, 1);
    return 0;
}

void i2c_deinit()
{
    hal_gpio_pin_set_dir(SCL, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_OUT, 0);
}

static void i2c_start()
{
    hal_gpio_pin_set(SCL);
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_OUT, 1);
    delay();
    hal_gpio_pin_clr(SDA); // SCL=1, SDA 1->0
    delay();
}

static void i2c_stop()
{
    hal_gpio_pin_clr(SCL);
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_OUT, 0);
    delay();
    hal_gpio_pin_set(SCL);
    hal_gpio_pin_set(SDA); // SCL=1, SDA 0->1
    delay();
}

/**
 * @brief I2C read ack, used when i2c write 
 * @return 0-ACK, others-NACK
 */
static unsigned char i2c_read_ack()
{
    hal_gpio_pin_clr(SCL);
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_IN, 0);
    delay();
    hal_gpio_pin_set(SCL);
    delay();
    return hal_gpio_pin_get_val(SDA);
}

/**
 * @brief I2C send ack, used when i2c read
 */
void i2c_send_ack()
{
    hal_gpio_pin_clr(SCL);
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_OUT, 0);
    delay();
    hal_gpio_pin_set(SCL);
    delay();
}

void i2c_send_nack()
{
    hal_gpio_pin_clr(SCL);
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_OUT, 1);
    delay();
    hal_gpio_pin_set(SCL);
    delay();
}

int i2c_write_byte(unsigned char b)
{
    int i;
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_OUT, hal_gpio_pin_get_val(SDA));
    for (i = 7; i >= 0; i--) {
        hal_gpio_pin_clr(SCL);
        delay();
        if (b & (1 << i)) // MSB
            hal_gpio_pin_set(SDA);
        else
            hal_gpio_pin_clr(SDA);
        hal_gpio_pin_set(SCL);
        delay();
    }
    return i2c_read_ack();
}

unsigned char i2c_read_byte()
{
    int i;
    unsigned char r = 0;
    hal_gpio_pin_set_dir(SDA, HAL_GPIO_DIR_IN, 0);
    for (i = 7; i >= 0; i--) {
        hal_gpio_pin_clr(SCL);
        delay();
        r = (r << 1) | hal_gpio_pin_get_val(SDA); // MSB
        hal_gpio_pin_set(SCL);
        delay();
    }
    i2c_send_ack();
    return r;
}

void i2c_read(unsigned char addr, unsigned char *rxbuf, int rxlen)
{
    int i;
    unsigned char t;
    i2c_start();
    t = (addr << 1) | 1; // bit0 = 1, R
    i2c_write_byte(t);
    for (i = 0; i < rxlen; i++)
        rxbuf[i] = i2c_read_byte();
    i2c_stop();
}

void i2c_write(unsigned char addr, unsigned char *txbuf, int txlen)
{
    int i;
    unsigned char t;
    i2c_start();
    t = (addr << 1) | 0; // bit0 = 0, W
    i2c_write_byte(t);
    for (i = 0; i < txlen; i++)
        i2c_write_byte(txbuf[i]);
    i2c_stop();
}

/************************************************************************/
#define AHT10_ADDR 0x38
#define AHT10_CALIBRATED (1 << 3)
#define AHT10_BUSY (1 << 7)

static uint8_t ath10_calibrate_cmd[] = {0xE1, 0x08, 0x00};
static uint8_t ath10_measure_cmd[] = {0xAC, 0x33, 0x00};
static uint8_t ath10_reset_cmd[] = {0xBA};

static void aht10_init()
{
    uint8_t status;
    i2c_read(AHT10_ADDR, &status, 1);
    if (!(status & AHT10_CALIBRATED)) {
        printf("ath10_calibrate\r\n");
        i2c_write(AHT10_ADDR, ath10_calibrate_cmd, sizeof(ath10_calibrate_cmd));
    }
    mdelay(40);
}

static void ath10_reset()
{
    i2c_write(AHT10_ADDR, ath10_reset_cmd, sizeof(ath10_reset_cmd));
    mdelay(20);
}

static void aht10_read_temperature_humidity(double *temperature, double *humidity)
{
    uint8_t status;
    uint8_t data[6] = {0};
    i2c_write(AHT10_ADDR, ath10_measure_cmd, sizeof(ath10_measure_cmd));
    mdelay(80); // wait for aht10 measurement completed
    i2c_read(AHT10_ADDR, &status, 1);
    if (status & AHT10_BUSY) {
        printf("aht10 busy\r\n");
        return;
    }
    i2c_read(AHT10_ADDR, data, 6);
    printf("rx: %02X %02X %02X %02X %02X %02X\r\n", data[0], data[1], data[2], data[3], data[4], data[5]);
    *humidity = (data[1] << 12 | data[2] << 4 | (data[3] & 0xf0) >> 4) * 100.0 / (1 << 20);
    *temperature = ((data[3] & 0xf) << 16 | data[4] << 8 | data[5]) * 200.0 / (1 << 20) - 50;
}

void i2c_test(void)
{
    int i;
    i2c_init();
    ath10_reset();
    aht10_init();
    for (i = 0; i < 30; i++) {
        double temperature = 0.0, humidity = 0.0;
        aht10_read_temperature_humidity(&temperature, &humidity);
        printf("aht10 temperature %.2f¡æ, humidity %.0f%%\r\n", temperature, humidity);
        mdelay(1000);
    }
    i2c_deinit();
}
#endif