#include "cmsis_os.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_spi.h"
#include "hal_trace.h"
#include <stdint.h>
#include <string.h>

/* W25QXX SPI Flash */
#define SPI_FLASH_PageSize 256
#define SPI_FLASH_PerWritePageSize 256
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactureDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
#define WIP_Flag 0x01 /* Write In Progress (WIP) flag */
#define Dummy_Byte 0xFF

#define FLASH_WriteAddress 0x00000
#define FLASH_ReadAddress FLASH_WriteAddress
#define FLASH_SectorToErase FLASH_WriteAddress

/**
 * @attention   If you want to set spi clk > 24MHz, you must enable pll mode.
 *              If you want to use spi dma, you must ensure the hal_dmacfg_best2003.h, 
 *                  such as bes2003_audma_fifo_addr and bes2003_audma_fifo_periph
 */
#define USE_DMA_CB

#ifdef USE_DMA_CB
static void spi_dma_cb(int error);
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

static struct spi_dev spi_devs[] = {
    /* spi0 as spi_dma demo */
    {
        .clk = {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_SPI_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        .cs = {HAL_IOMUX_PIN_P0_6, HAL_IOMUX_FUNC_SPI_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        .mosi = {HAL_IOMUX_PIN_P0_7, HAL_IOMUX_FUNC_SPI_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        .miso = {HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_SPI_DI0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        .cfg = {
            .rate = 12000000,
            .clk_delay_half = true, // mode 3
            .clk_polarity = true,
            .slave = false, // master
            .dma_rx = true,
            .dma_tx = true, // dma_rx == dma_tx always
            .rx_sep_line = false,
            .cs = 0,
            .tx_bits = 8, // data size = 8
            .rx_bits = 8,
            .rx_frame_bits = 0,
        },
        .open = hal_spi_open,
        .close = hal_spi_close,
        .transfer = hal_spi_recv,
        .dma_transfer = hal_spi_dma_recv,
#ifdef USE_DMA_CB
        .dma_cb = spi_dma_cb,
#else
        .dma_cb = NULL,
#endif
        /* if dma_cb = NULL, spi transfer block until dma done,
		otherwise the user must control the flow, such as Semaphore */
    },
    /* spi1 as spi_no_dma demo */
    {
        .clk = {HAL_IOMUX_PIN_P3_7, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},

        .cs = {HAL_IOMUX_PIN_P3_6, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        .mosi = {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        .miso = {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_SPILCD_DI0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        .cfg = {
            .rate = 12000000,
            .clk_delay_half = true,
            .clk_polarity = true,
            .slave = false,
            .dma_rx = false,
            .dma_tx = false,
            .rx_sep_line = false,
            .cs = 0,
            .tx_bits = 8,
            .rx_bits = 8,
            .rx_frame_bits = 0,
        },
        .open = hal_spilcd_open,
        .close = hal_spilcd_close,
        .transfer = hal_spilcd_recv,
        // .dma_transfer = hal_spilcd_dma_recv,
        // .dma_cb = spi_dma_cb,
    }};

static struct spi_dev *spi_find_device(uint8_t index)
{
    if (index >= ARRAY_SIZE(spi_devs))
        return NULL;
    else
        return &spi_devs[index];
}

static int spi_init(struct spi_dev *dev)
{
    ASSERT(dev, "dev mustn't be null\r\n");
    ASSERT(dev->open, "dev->open mustn't be null\r\n");
    ASSERT(!dev->cfg.slave, "it's only for spi master\r\n");

    hal_iomux_init(&dev->clk, 1);
    hal_iomux_init(&dev->cs, 1);
    hal_iomux_init(&dev->miso, 1);
    hal_iomux_init(&dev->mosi, 1);
    // if (dev->cfg.rate >= 24000000) {
    //     hal_iomux_set_io_drv(dev->mosi.pin, 3);
    //     hal_iomux_set_io_drv(dev->clk.pin, 3);
    // }
    int ret = dev->open(&dev->cfg);
    if (ret) {
        printf("spi open error %d\r\n", ret);
        return ret;
    }
    /*if cs use as gpio ,pull up at first*/
    if (dev->cs.function == HAL_IOMUX_FUNC_AS_GPIO) {
        hal_gpio_pin_set_dir(dev->cs.pin, HAL_GPIO_DIR_OUT, 1);
    }
#ifdef USE_DMA_CB
    if (dev->cfg.dma_rx && dev->cfg.dma_tx && dev->dma_cb) {
        osSemaphoreDef(_sem);
        dev->sem = osSemaphoreCreate(osSemaphore(_sem), 0);
        ASSERT(dev->sem, "dev->sem cerate failed\r\n");
    }
#endif
    printf("spi clk=%u\r\n", dev->cfg.rate);
    return 0;
}

static void spi_deinit(struct spi_dev *dev)
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

static int spi_transfer(const struct spi_dev *dev, const void *txbuf, void *rxbuf, uint32_t len)
{
    int ret = 0;
    if (!dev->cfg.slave && dev->cs.function == HAL_IOMUX_FUNC_AS_GPIO) { // cs is controlled by software
        hal_gpio_pin_clr(dev->cs.pin);
    }
    if (dev->cfg.dma_rx && dev->cfg.dma_tx) {
        ret = dev->dma_transfer(txbuf, rxbuf, len, dev->dma_cb);
#ifdef USE_DMA_CB
        if (dev->dma_cb)
            osSemaphoreWait(dev->sem, 1000);
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

#ifdef USE_DMA_CB
static void spi_dma_cb(int error)
{
    if (error) {
        printf("spi rx dma cb, error %d\r\n", error);
        return;
    }
    const struct spi_dev *dev = spi_find_device(0);
    if (dev->sem)
        osSemaphoreRelease(dev->sem);
}
#endif

void SPI_FLASH_CS_LOW(const struct spi_dev *dev)
{
    if (dev->cs.function == HAL_IOMUX_FUNC_AS_GPIO) {
        hal_gpio_pin_clr(dev->cs.pin);
    }
}

void SPI_FLASH_CS_HIGH(const struct spi_dev *dev)
{
    if (dev->cs.function == HAL_IOMUX_FUNC_AS_GPIO) {
        hal_gpio_pin_set(dev->cs.pin);
    }
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(const struct spi_dev *dev)
{
    uint8_t cmd = W25X_WriteEnable;
    uint8_t data;
    spi_transfer(dev, &cmd, &data, 1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(const struct spi_dev *dev)
{
    uint8_t cmd[2] = {W25X_ReadStatusReg, Dummy_Byte};
    uint8_t data[2] = {0};
    do {
        spi_transfer(dev, &cmd, &data, 2);
    } while ((data[1] & WIP_Flag) == 1); /* Write in progress */
}

void SPI_Flash_PowerDown(const struct spi_dev *dev)
{
    uint8_t cmd = W25X_PowerDown;
    uint8_t data = 0;
    spi_transfer(dev, &cmd, &data, 1);
}

void SPI_Flash_WAKEUP(const struct spi_dev *dev)
{
    uint8_t cmd = W25X_ReleasePowerDown;
    uint8_t data = 0;
    spi_transfer(dev, &cmd, &data, 1);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(const struct spi_dev *dev, uint32_t SectorAddr)
{
    /* Send write enable instruction */
    SPI_FLASH_WriteEnable(dev);
    SPI_FLASH_WaitForWriteEnd(dev);
    /* Sector Erase */
    uint8_t cmd[4] = {W25X_SectorErase, (SectorAddr & 0xFF0000) >> 16,
                      (SectorAddr & 0xFF00) >> 8, SectorAddr & 0xFF};
    uint8_t data[4];
    spi_transfer(dev, &cmd, &data, sizeof(cmd));
    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd(dev);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(const struct spi_dev *dev)
{
    /* Send write enable instruction */
    SPI_FLASH_WriteEnable(dev);
    /* Bulk Erase */
    uint8_t cmd = W25X_ChipErase;
    uint8_t data = 0;
    spi_transfer(dev, &cmd, &data, 1);
    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd(dev);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(const struct spi_dev *dev, const uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    /* Enable the write access to the FLASH */
    SPI_FLASH_WriteEnable(dev);
    if (NumByteToWrite > SPI_FLASH_PerWritePageSize) {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
    }
    uint8_t cmd[NumByteToWrite + 4];
    uint8_t data[NumByteToWrite + 4];
    cmd[0] = W25X_PageProgram;
    cmd[1] = (WriteAddr & 0xFF0000) >> 16;
    cmd[2] = (WriteAddr & 0xFF00) >> 8;
    cmd[3] = WriteAddr & 0xFF;
    memcpy(&cmd[4], pBuffer, NumByteToWrite);
    spi_transfer(dev, &cmd, &data, NumByteToWrite + 4);
    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd(dev);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferWrite(const struct spi_dev *dev, const uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % SPI_FLASH_PageSize;
    count = SPI_FLASH_PageSize - Addr;
    NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, NumByteToWrite);
        } else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            while (NumOfPage--) {
                SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr += SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, NumOfSingle);
        }
    } else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
            {
                temp = NumOfSingle - count;

                SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, count);
                WriteAddr += count;
                pBuffer += count;

                SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, temp);
            } else {
                SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, NumByteToWrite);
            }
        } else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            NumByteToWrite -= count;
            NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, count);
            WriteAddr += count;
            pBuffer += count;

            while (NumOfPage--) {
                SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr += SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            if (NumOfSingle != 0) {
                SPI_FLASH_PageWrite(dev, pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferRead(const struct spi_dev *dev, uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    uint8_t cmd[NumByteToRead + 4];
    uint8_t data[NumByteToRead + 4];
    cmd[0] = W25X_ReadData;
    cmd[1] = (ReadAddr & 0xFF0000) >> 16;
    cmd[2] = (ReadAddr & 0xFF00) >> 8;
    cmd[3] = ReadAddr & 0xFF;
    spi_transfer(dev, &cmd, &data, NumByteToRead + 4);
    memcpy(pBuffer, &data[4], NumByteToRead);
}

uint32_t SPI_FLASH_ReadJedecID(const struct spi_dev *dev)
{
    uint8_t cmd[4] = {W25X_JedecDeviceID, Dummy_Byte, Dummy_Byte, Dummy_Byte};
    uint8_t data[4];
    spi_transfer(dev, &cmd, &data, 4);
    return (data[1] << 16) | (data[2] << 8) | data[3];
}

uint32_t SPI_FLASH_ReadDeviceID(const struct spi_dev *dev)
{
    uint8_t cmd[5] = {W25X_DeviceID, Dummy_Byte, Dummy_Byte, Dummy_Byte, Dummy_Byte};
    uint8_t data[5];
    spi_transfer(dev, &cmd, &data, 5);
    return data[4];
}

uint32_t SPI_FLASH_ReadManufactureID(const struct spi_dev *dev)
{
    uint8_t cmd[6] = {W25X_ManufactureDeviceID, 0, 0, 0, 0, 0};
    uint8_t data[6];
    spi_transfer(dev, &cmd, &data, 6);
    return (uint32_t)(data[4] << 8) | data[5];
}

void w25q_test(void)
{
    struct spi_dev *dev = spi_find_device(1);
    if (spi_init(dev) != 0) {
        printf("spi_init failed\r\n");
        return;
    }
    SPI_Flash_WAKEUP(dev);
    uint32_t id = SPI_FLASH_ReadManufactureID(dev);
    printf("ManufactureID=0x%X\r\n", id);
    id = SPI_FLASH_ReadDeviceID(dev);
    printf("DeviceID=0x%X\r\n", id);
    id = SPI_FLASH_ReadJedecID(dev);
    printf("JedecID=0x%X\r\n", id);

#define BUFSIZE 300 // smaller or bigger than SPI_FLASH_PageSize
    uint8_t txbuf[BUFSIZE] = "This is w25qxx flash test.";
    uint8_t rxbuf[BUFSIZE];
    /* Erase SPI FLASH Sector to write on */
    SPI_FLASH_SectorErase(dev, FLASH_SectorToErase);
    printf("SPI_FLASH_SectorErase\r\n");

    SPI_FLASH_BufferWrite(dev, txbuf, FLASH_WriteAddress, BUFSIZE);
    printf("tx: %s\r\n", txbuf);

    SPI_FLASH_BufferRead(dev, rxbuf, FLASH_ReadAddress, BUFSIZE);
    printf("rx: %s\r\n", rxbuf);

    if (memcmp(txbuf, rxbuf, BUFSIZE) == 0) {
        printf("W25Qxx Test Passed!\r\n");
    } else {
        printf("W25Qxx Test Failed!\r\n");
    }
    SPI_Flash_PowerDown(dev);
    spi_deinit(dev);
    printf("%s finished\r\n", __func__);
}
