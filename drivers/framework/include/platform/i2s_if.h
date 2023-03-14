/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup I2S
 * @{
 *
 * @brief Defines standard APIs of the  (I2S) capabilities.
 *
 * The I2S module abstracts the I2S capabilities of different system platforms to provide stable APIs
 * for driver development.
 * This module can create and destroy I2S device handles, read and write I2S data,
 * and obtain and set configuration parameters.
 *
 * @since 1.0
 */

/**
 * @file i2s_if.h
 *
 * @brief Defines standard I2S-specific interfaces for driver development.
 *
 * A driver needs to use the I2S-specific interfaces for data writing and reading
 * before performing any operations on an I2S-compliant device.
 *
 * @since 1.0
 */

#ifndef I2S_IF_H
#define I2S_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines the configuration of an I2S device.
 *
 * @attention The specific I2S controller determines which variables in this structure are supported.
 *
 * @since 1.0
 */

#define I2S_DATA_TRANSFER_PERIOD 20

enum I2sSampleRate {
    I2S_SAMPLE_RATE_8K = 8,
    I2S_SAMPLE_RATE_16K = 16,
    I2S_SAMPLE_RATE_32K = 32,
    I2S_SAMPLE_RATE_48K = 48,
    I2S_SAMPLE_RATE_44_1K = 441, // 44.1k
    I2S_SAMPLE_RATE_96K = 96,
    I2S_SAMPLE_RATE_192K = 192,
};

enum I2sWordWidth {
    I2S_WORDWIDTH_8BIT = 8,
    I2S_WORDWIDTH_16BIT = 16,
    I2S_WORDWIDTH_18BIT = 18,
    I2S_WORDWIDTH_20BIT = 20,
    I2S_WORDWIDTH_24BIT = 24,
    I2S_WORDWIDTH_32BIT = 32,
};

enum I2sMode {
    I2S_MODE_MASTER_TX_MODE,
    I2S_MODE_MASTER_RX_MODE,
    I2S_MODE_STANDBY_TX_MODE,
    I2S_MODE_STANDBY_RX_MODE,
};

enum I2sLoopMode {
    I2S_NORMAL_MODE,
    I2S_LOOPBACK_MODE,
};

enum I2sChannel {
    I2S_CHANNEL_LOW_LEFT,
    I2S_CHANNEL_LOW_RIGHT,
};

enum I2sFrameLen {
    I2S_FRAME_LEN_32BIT = 32,
    I2S_FRAME_LEN_64BIT = 64,
};

enum I2sProtocolType {
    I2S_PROTOCOL_I2S_STD,
    I2S_PROTOCOL_I2S_MSB,
    I2S_PROTOCOL_I2S_LSB,
    I2S_PROTOCOL_PCM_STD,
    I2S_PROTOCOL_PCM_SHORT,
    I2S_PROTOCOL_PCM_LONG,
};

enum I2sPcmFscDelay {
    I2S_PCM_START_1_CYCLE_BEFORE_MSB,   /**< PCM FSC starts one cycle before MSB bit */
    I2S_PCM_START_SYNC_MSB,             /**< PCM FSC starts at the same time as MSB bit */
};

enum I2sDataSel {
    I2S_RISING_EDGE_TX,
    I2S_RISING_EDGE_RX,
    I2S_FALLING_EDGE_TX,
    I2S_FALLING_EDGE_RX,
};

enum I2sChannelMode {
    I2S_CHANNEL_MODE_STEREO,
    I2S_CHANNEL_MODE_MONO,
};

enum I2sChannelIfMode {
    I2S_CHANNEL_IF_MODE_I2S,
    I2S_CHANNEL_IF_MODE_PCM,
};

enum I2sWriteChannel {
    I2S_WRITE_CHANNEL_AUDIO,
    I2S_WRITE_CHANNEL_OUTPUT,
};

enum I2slFsSel {
    I2SL_FS_SEL_1024_FS,
    I2SL_FS_SEL_512_FS,
    I2SL_FS_SEL_320_FS,
    I2SL_FS_SEL_256_FS,
    I2SL_FS_SEL_128_FS,
    I2SL_FS_SEL_64_FS,
};


struct I2sCfg {
    enum I2sSampleRate sampleRate;     /**< I2S sample rate, 8k,16k,32k... */
    enum I2sWordWidth width;            /**< I2S word width, 8bit,16bit,20bit,24bit... */
    enum I2sMode  mode;                /**< I2S mode, master/standby, tx/rx */
    enum I2sPcmFscDelay syncMode;      /**< pcm mode, FSC starts location */
    enum I2sLoopMode  loopMode;        /**< is loopback */
    enum I2sProtocolType type;         /**< I2S protocol type */
    enum I2sChannel  channel;
    enum I2sFrameLen  frameLen;
    enum I2sDataSel  dataSel;
    enum I2sChannelMode channelMode;   /**< I2S mode,data channel */
    uint8_t samplePrecision;          /**< bit */
    enum I2sChannelIfMode channelIfMode;
    uint32_t mclk;                     /**< KHZ */
    uint32_t bclk;                     /**< KHZ */
    enum I2sWriteChannel writeChannel;
    enum I2slFsSel i2slFsSel;
    uint8_t Reserved[2];
};

/**
 * @brief Defines the custom I2S transfer message.
 *
 * @attention The specific I2S controller determines whether <b>speed</b>, <b>delayUs</b>,
 * and <b>csChange</b> are supported.
 *
 * @since 1.0
 */
struct I2sMsg {
    uint8_t *wbuf;       /**< Address of the write buffer */
    uint8_t *rbuf;       /**< Address of the read buffer */
    uint32_t len;        /**< Length of the read and write buffers. The read buffer and the write
                          * buffer have the same length. */
    uint32_t *pRlen;
};


/**
 * @brief Obtains the handle of an I2S controller.
 *
 * You must call this function before accessing the I2S bus.
 *
 * @param number Indicates the I2S controller ID.
 *
 * @return Returns the pointer to the {@link DevHandle} of the I2S controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 * @since 1.0
 */
DevHandle I2sOpen(int16_t number);

 /**
 * @brief Releases the handle of an I2S controller.
 *
 * If you no longer need to access the I2S controller, you should call this function to close its handle so as
 * to release unused memory resources.
 *
 * @param handle Indicates the pointer to the device handle of the I2S controller.
 *
 * @since 1.0
 */
void I2sClose(DevHandle handle);
void I2sEnable(DevHandle handle);
void I2sDisable(DevHandle handle);
int32_t I2sWrite(DevHandle handle, uint8_t *buf, uint32_t len, uint32_t *pWlen);
int32_t I2sRead(DevHandle handle, uint8_t *buf, uint32_t len, uint32_t *pRlen);
void I2sStartWrite(DevHandle handle);
void I2sStopWrite(DevHandle handle);
void I2sStartRead(DevHandle handle);
void I2sStopRead(DevHandle handle);
void I2sSetCfg(DevHandle handle, struct I2sCfg *cfg);
void I2sGetCfg(DevHandle handle, struct I2sCfg *cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* I2S_IF_H */
/** @} */
