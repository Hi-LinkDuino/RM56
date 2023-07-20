#ifndef __CSI_DSI_CONF_H__
#define __CSI_DSI_CONF_H__

/* CSI DSI DATA BUFFER CONFIG */

#if (CAMERA_IMAGE_WIDTH == 1080) && (CAMERA_IMAGE_HEIGHT == 1280)
#define CSI_DATA_BUF_SIZE (CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT * 2 * 2)
#else
#define CSI_DATA_BUF_SIZE (0x200000)
#endif

#define DSI_DATA_BUF_SIZE (0x200000)
#ifdef PSRAMUHS_ENABLE
#define CSI_DSI_BUF_BASE   (PSRAMUHS_NC_BASE)
#define CSI_DSI_BUF_END    (PSRAMUHS_NC_BASE + DSP_PSRAMUHS_SIZE)
#define DSI_DATA_BUF_BASE  (CSI_DSI_BUF_END - DSI_DATA_BUF_SIZE)
#define CSI_DATA_BUF_BASE  (DSI_DATA_BUF_BASE - CSI_DATA_BUF_SIZE)
#define CSI_DATA_BUF_ADDR  (CSI_DATA_BUF_BASE)
#define DSI_DATA_BUF_ADDR  (DSI_DATA_BUF_BASE)

#else /* PSRAMUHS_ENABLE */
#define CSI_DATA_BUF_ADDR  (DSP_RAM_BASE)
#define DSI_DATA_BUF_ADDR  (DSP_RAM_BASE)
#endif

/* MEDIA HEAP BUFFER */
#if !defined(CHIP_ROLE_DSP) && !defined(CHIP_ROLE_AP)
#define MEDIA_HEAP_RAW_SIZE    (CSI_DATA_BUF_SIZE + DSI_DATA_BUF_SIZE)
#elif defined(CHIP_ROLE_DSP) && !defined(CHIP_ROLE_AP)
#define MEDIA_HEAP_RAW_SIZE (0x240000)
#elif defined(CHIP_ROLE_AP) && !defined(CHIP_ROLE_DSP)
#define MEDIA_HEAP_RAW_SIZE (0x100000)
#else
#error "both CHIP_ROLE_AP and CHIP_ROLE_DSP are defined !"
#endif

#ifndef MEDIA_HEAP_SIZE
#define MEDIA_HEAP_SIZE    MEDIA_HEAP_RAW_SIZE
#endif

#ifndef MEDIA_HEAP_ADDR
#define MEDIA_HEAP_ADDR    (CSI_DATA_BUF_BASE)
#endif

/* CSI SENSOR PIN CONFIG */
#define CSI_SENS_PDN_PIN   (HAL_GPIO_PIN_P2_4)
#define CSI_SENS_RSTN_PIN  (HAL_GPIO_PIN_P2_5)
#define CSI_SENS_MCLK_PIN  (HAL_GPIO_PIN_P3_4)

/* CSI SYNC PIN CONFIG */
#define CSI_SYNC_OUT_PIN HAL_IOMUX_PIN_P3_0
#define CSI_SYNC_IN_PIN  HAL_IOMUX_PIN_P3_1

/* CSI SENSOR I2C CONFIG */
#define GC2145_I2C_ID       HAL_I2C_ID_0
#define GC2145_I2C_ADD      0x3c
#define GC2145_I2C_SPEED_HZ 100000

/* CSI SENSOR I2C CONFIG */
#define SC132GS_I2C_ID       HAL_I2C_ID_0
#define SC132GS_I2C_ADD      0x30 //31,32,33 depend on SID0, SID1
#define SC132GS_I2C_SPEED_HZ 100000

#ifdef CAMERA_IMAGE_WIDTH
#define CSI_IMG_PIXEL_WIDTH CAMERA_IMAGE_WIDTH
#else
#define CSI_IMG_PIXEL_WIDTH 800
#endif
#ifdef CAMERA_IMAGE_HEIGHT
#define CSI_IMG_PIXEL_HEIGHT CAMERA_IMAGE_HEIGHT
#else
#define CSI_IMG_PIXEL_HEIGHT 600
#endif

#endif
