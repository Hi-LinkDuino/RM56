/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_ACCESSORY_IF_H
#define AUDIO_ACCESSORY_IF_H

#include "audio_host.h"
#include "audio_control.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines Accessory Codec device name and data.
 *
 * @since 1.0
 * @version 1.0
 */
struct AccessoryDevice {
    const char *devAccessoryName;   /**< Accesssory device name */
    struct AccessoryData *devData;  /**< Accesssory private data */
    struct HdfDeviceObject *device; /**< HDF device */
    struct DListHead list;          /**< Accesssory list */
    struct OsalMutex mutex;         /**< Accesssory mutex */
};

/**
 * @brief Defines Accessory host in audio driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct AccessoryHost {
    struct IDeviceIoService service; /**< Services provided by Accesssory */
    struct HdfDeviceObject *device;  /**< HDF device */
    void *priv;                      /**< Accesssory private data interface */
};

/**
 * @brief Defines Accesssory private data.
 *
 * @since 1.0
 * @version 1.0
 */
struct AccessoryData {
    const char *drvAccessoryName;         /**< Accesssory driver name */

    /**
     * @brief Defines Accesssory device init.
     *
     * @param audioCard Indicates an audio card device.
     * @param accesssory Indicates an Accesssory device.
     *
     * @return Returns <b>0</b> if Accesssory device init success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Init)(struct AudioCard *audioCard, const struct AccessoryDevice *accesssory);

    /**
     * @brief Defines Accesssory device reg read.
     *
     * @param accesssory Indicates a accesssory device.
     * @param reg Indicates read reg address.
     * @param value Indicates read reg value.
     *
     * @return Returns <b>0</b> if accesssory device read reg success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Read)(const struct AccessoryDevice *accesssory, uint32_t reg, uint32_t *value);

    /**
     * @brief Defines accesssory device reg write.
     *
     * @param accesssory Indicates a accesssory device.
     * @param reg Indicates write reg address.
     * @param value Indicates write reg value.
     *
     * @return Returns <b>0</b> if accesssory device write reg success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Write)(const struct AccessoryDevice *accesssory, uint32_t reg, uint32_t value);

    const struct AudioKcontrol *controls; /**< Accesssory control structure array pointer */
    int numControls;                      /**< Accesssory controls the number of structure array elements */
    struct AudioRegCfgData* regConfig;    /**< Accesssory registers configured in HCS */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
