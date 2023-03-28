#include "cmsis_os.h"
#include "hal_i2c.h"
#include "hal_trace.h"
#include "peripheral_modules_test.h"
#include <stdbool.h>

#define I2C_DEVICE_NAME "i2c"
#define AHT10_ADDR 0x38            /* slave address */
#define AHT10_CALIBRATION_CMD 0xE1 /* cmd */
#define AHT10_NORMAL_CMD 0xA8      /* cmd */
#define AHT10_GET_DATA 0xAC        /* get data cmd */
#define TEST_CONT 20

static bool initialized = false; /* init sensor */

struct BES_I2C_OBJ_T {
    uint16_t dev_addr;      /**< slave device addr */
    uint32_t address_width; /**< Addressing mode: 7 bit or 10 bit */
    osMutexId mutex_id;
    int port;
    struct HAL_I2C_CONFIG_T i2c_cfg;
};

osMutexDef(i2c_mutex);
static struct BES_I2C_OBJ_T bes_i2c_obj;

/* write reg */
static int32_t write_reg(struct BES_I2C_OBJ_T *i2c_obj, uint8_t reg, uint8_t *data)
{
    int32_t ret;
    //ASSERT(i2c_obj==NULL);
    enum HAL_I2C_ID_T i2c_port = i2c_obj->port;
    uint8_t buf[3];

    buf[0] = reg; //cmd
    buf[1] = data[0];
    buf[2] = data[1];

    /* I2C send data */
    if (i2c_obj->i2c_cfg.mode == HAL_I2C_API_MODE_SIMPLE) {
        ret = hal_i2c_simple_send(i2c_port, AHT10_ADDR, buf, 3);
    } else {
        ret = hal_i2c_task_send(i2c_port, AHT10_ADDR, buf, 3, 0, NULL);
    }

    if (ret) {
        printf("%s:%d,i2c send fail, dev_addr = 0x%x, ret = %d\r\n", __func__, __LINE__, AHT10_ADDR, ret);
    }
    return ret;
}

/* read reg data */
static int32_t read_regs(struct BES_I2C_OBJ_T *i2c_obj, uint8_t len, uint8_t *buf)
{
    int32_t ret;
    //ASSERT(i2c_obj==NULL);
    enum HAL_I2C_ID_T i2c_port = i2c_obj->port;
    /* I2C recev data */
    if (i2c_obj->i2c_cfg.mode == HAL_I2C_API_MODE_SIMPLE) {
        ret = hal_i2c_simple_recv(i2c_port, AHT10_ADDR, buf, 0, buf, len);
    } else {
        ret = hal_i2c_task_recv(i2c_port, AHT10_ADDR, buf, 0, buf, len, 0, NULL);
    }

    if (ret) {
        printf("%s:%d,i2c recev fail, dev_addr = 0x%x, ret = %d\r\n", __func__, __LINE__, AHT10_ADDR, ret);
    }
    return ret;
}

static int i2c_drv_init(enum HAL_I2C_ID_T port, struct BES_I2C_OBJ_T *i2c_obj)
{
    int32_t lock_ret, ret = -1;
    //ASSERT(i2c_obj==NULL);
    i2c_obj->port = port;
    i2c_obj->address_width = 8;
    i2c_obj->i2c_cfg.speed = 100000;
    i2c_obj->i2c_cfg.mode = HAL_I2C_API_MODE_SIMPLE;
    i2c_obj->i2c_cfg.use_dma = 0;
    i2c_obj->i2c_cfg.use_sync = 1;
    i2c_obj->i2c_cfg.as_master = 1;
    /* initialize mutex */
    i2c_obj->mutex_id = osMutexCreate(osMutex(i2c_mutex));
    if (!i2c_obj->mutex_id) {
        //ASSERT(0, "cannot create mutex");
    }
    lock_ret = osMutexWait(i2c_obj->mutex_id, osWaitForever);
    if (lock_ret != 0) {
        printf("create i2c_mutex fail\r\n");
        return lock_ret;
    }
    if (port == HAL_I2C_ID_0) {
        hal_iomux_set_i2c0();
    } else {
        hal_iomux_set_i2c1();
    }
    ret = hal_i2c_open(port, &i2c_obj->i2c_cfg);
    if (ret) {
        printf("open %d i2c fail,ret %d\r\n", port, ret);
        return -1;
    } else {
        printf("open %d i2c succ.\r\n", port);
    }
    osMutexRelease(i2c_obj->mutex_id);
    return ret;
}

static void aht10_init(struct BES_I2C_OBJ_T *i2c_obj)
{
    //ASSERT(i2c_obj==NULL);
    uint8_t temp[2] = {0, 0};
    temp[0] = 0x08;
    temp[1] = 0x00;
    write_reg(i2c_obj, AHT10_CALIBRATION_CMD, temp);
    osDelay(400);
}

static void read_temp_humi(struct BES_I2C_OBJ_T *i2c_obj, float *cur_temp, float *cur_humi)
{
    uint8_t temp[6];
    //ASSERT(i2c_obj==NULL);
    //enum HAL_I2C_ID_T i2c_port = i2c_obj->port;
    uint8_t rt_temp[2] = {0, 0};
    rt_temp[0] = 0x33;
    rt_temp[1] = 0x00;
    write_reg(i2c_obj, AHT10_GET_DATA, rt_temp); /* send cmd */
    read_regs(i2c_obj, 6, temp);                 /* read sensor data */

    /* humidity data */
    *cur_humi = (temp[1] << 12 | temp[2] << 4 | (temp[3] & 0xf0) >> 4) * 100.0 / (1 << 20);
    /* temperature data */
    *cur_temp = ((temp[3] & 0xf) << 16 | temp[4] << 8 | temp[5]) * 200.0 / (1 << 20) - 50;
}

int aht10_test(void)
{
    printf("enter %s %d ", __func__, __LINE__);
    float humidity, temperature;
    enum HAL_I2C_ID_T port = HAL_I2C_ID_0;
    if (!initialized) {
        i2c_drv_init(port, &bes_i2c_obj);
        osDelay(1000);
        aht10_init(&bes_i2c_obj);
        initialized = true;
    }
    if (initialized) {
        for (size_t i = 0; i < TEST_CONT; i++) {
            /* read temperature and humidity data */
            read_temp_humi(&bes_i2c_obj, &temperature, &humidity);

            printf("read aht10 sensor humidity   : %d.%d %%\r\n", (int)humidity, (int)(humidity * 10) % 10);
            if (temperature >= 0) {
                printf("read aht10 sensor temperature: %d.%d°C\r\n", (int)temperature, (int)(temperature * 10) % 10);
            } else {
                printf("read aht10 sensor temperature: %d.%d°C\r\n", (int)temperature, (int)(-temperature * 10) % 10);
            }
            osDelay(2000); //must
        }
    }
    return 0;
}