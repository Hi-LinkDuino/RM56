/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_DSP_IF_H
#define AUDIO_DSP_IF_H
#include "audio_host.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines Dsp device name and data.
 *
 * @since 1.0
 * @version 1.0
 */
struct DspDevice {
    const char *devDspName;         /**< Dsp device name */
    struct DspData *devData;        /**< Dsp module private data */
    struct HdfDeviceObject *device; /**< HDF device */
    struct DListHead list;          /**< Dsp list */
};

/**
 * @brief Defines Dsp operation function set.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioDspOps {
    /**
     * @brief Defines Dsp device start up function.
     *
     * @param audioCard Indicates an audio card device.
     * @param dsp Indicates a dsp device.
     *
     * @return Returns <b>0</b> if Dsp device start up success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Startup)(const struct AudioCard *audioCard, const struct DspDevice *dsp);

    /**
     * @brief Defines Dsp device hardware param function.
     *
     * @param audioCard Indicates an audio card device.
     * @param param Indicates pcm params set.
     *
     * @return Returns <b>0</b> if dsp param set success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*HwParams)(const struct AudioCard *audioCard, const struct AudioPcmHwParams *param);

    /**
     * @brief Defines Dsp device trigger function.
     *
     * @param audioCard Indicates an audio card device.
     * @param dsp Indicates a dsp device.
     *
     * @return Returns <b>0</b> if dsp device trigger success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Trigger)(struct AudioCard *audioCard, int, struct DspDevice *dsp);
};

/**
 * @brief Defines Dsp host in audio driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct DspHost {
    struct IDeviceIoService service; /**< Services provided by dsp */
    struct HdfDeviceObject *device;  /**< HDF device */
    void *priv;                      /**< Dsp private data interface */
};

/**
 * @brief Defines dsp private data.
 *
 * @since 1.0
 * @version 1.0
 */
struct DspData {
    const char *drvDspName; /**< Dsp driver name */

    /**
     * @brief Defines Dsp device init.
     *
     * @param dsp Indicates a Dsp device.
     *
     * @return Returns <b>0</b> if dsp device init success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DspInit)(const struct DspDevice *dsp);

    /**
     * @brief Defines Dsp device msgs read.
     *
     * @param dsp Indicates a dsp device.
     * @param msgs Indicates transfer msgs data.
     * @param len Indicates msgs length.
     *
     * @return Returns <b>0</b> if dsp device read msgs success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Read)(const struct DspDevice *dsp, const void *msgs, const uint32_t len);

    /**
     * @brief Defines Dsp device msgs write.
     *
     * @param dsp Indicates a dsp device.
     * @param msgs Indicates transfer msgs data.
     * @param len Indicates msgs length.
     *
     * @return Returns <b>0</b> if dsp device write msgs success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Write)(const struct DspDevice *dsp, const void *msgs, const uint32_t len);

    /**
     * @brief Defines Dsp device decode.
     *
     * @param audioCard Indicates an audio card.
     * @param buf Indicates decode pcm buffer data.
     * @param dsp Indicates a dsp device.
     *
     * @return Returns <b>0</b> if Dsp device decode success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Decode)(const struct AudioCard *audioCard, const uint8_t *buf, const struct DspDevice *dsp);

    /**
     * @brief Defines Dsp device encode.
     *
     * @param audioCard audioCard Indicates an audio card.
     * @param buf Indicates encode pcm buffer data.
     * @param dsp Indicates a dsp device.
     *
     * @return Returns <b>0</b> if Dsp device encode success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Encode)(const struct AudioCard *audioCard, const uint8_t *buf, const struct DspDevice *dsp);

    /**
     * @brief Defines Dsp device equalizer.
     *
     * @param audioCard audioCard Indicates an audio card.
     * @param buf Indicates equalizer pcm buffer data.
     * @param dsp Indicates a dsp device.
     *
     * @return Returns <b>0</b> if Dsp device equalizer success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Equalizer)(const struct AudioCard *audioCard, const uint8_t *buf, const struct DspDevice *dsp);
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
