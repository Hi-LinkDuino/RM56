/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_CODEC_IF_H
#define AUDIO_CODEC_IF_H

#include "audio_host.h"
#include "audio_control.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines Codec device name and data.
 *
 * @since 1.0
 * @version 1.0
 */
struct CodecDevice {
    const char *devCodecName;       /**< Codec device name */
    struct CodecData *devData;      /**< Codec module private data */
    struct HdfDeviceObject *device; /**< HDF device */
    struct DListHead list;          /**< Codec list */
    struct OsalMutex mutex;         /**< Codec mutex */
};

/**
 * @brief Defines Codec private data.
 *
 * @since 1.0
 * @version 1.0
 */
struct CodecData {
    const char *drvCodecName;                  /**< Codec driver name */

    /**
     * @brief Defines Codec device init.
     *
     * @param audioCard Indicates an audio card device.
     * @param codec Indicates a codec device.
     *
     * @return Returns <b>0</b> if codec device init success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Init)(struct AudioCard *audioCard, const struct CodecDevice *codec);

    /**
     * @brief Defines Codec device reg read.
     *
     * @param virtualAddress Indicates base reg IoRemap address.
     * @param reg Indicates reg offset.
     * @param value Indicates read reg value.
     *
     * @return Returns <b>0</b> if codec device read reg success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Read)(unsigned long virtualAddress, uint32_t reg, uint32_t *value);

    /**
     * @brief Defines Codec device reg write.
     *
     * @param virtualAddress Indicates base reg IoRemap address.
     * @param reg Indicates reg offset.
     * @param value Indicates write reg value.
     *
     * @return Returns <b>0</b> if codec device write reg success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Write)(unsigned long virtualAddress, uint32_t reg, uint32_t value);

    struct AudioKcontrol *controls;            /**< Codec control structure array pointer */
    uint32_t numControls;                           /**< Number of array elements of Codec controls */
    struct AudioSapmComponent *sapmComponents; /**< Codec power management component array pointer */
    int numSapmComponent;                      /**< Number of array elements of codec power management component */
    const struct AudioSapmRoute *sapmRoutes;   /**< Codec power management route array pointer */
    int numSapmRoutes;                         /**< Number of power management route array elements */
    unsigned long virtualAddress;              /**< Codec base reg IoRemap address */
    struct AudioRegCfgData *regConfig;         /**< Codec registers configured in HCS */
    struct AudioRegCfgGroupNode **regCfgGroup; /**< Codec register group configured in HCS */
};

/**
 * @brief Defines Codec host in audio driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct CodecHost {
    struct IDeviceIoService service; /**< Services provided by codec */
    struct HdfDeviceObject *device;  /**< HDF device */
    void *priv;                      /**< Codec private data interface */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* CODEC_CORE_H */
