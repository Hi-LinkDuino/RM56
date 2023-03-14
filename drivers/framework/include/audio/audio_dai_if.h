/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_DAI_IF_H
#define AUDIO_DAI_IF_H

#include "audio_host.h"
#include "audio_parse.h"
#include "audio_control.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines Dai device name and data.
 *
 * @since 1.0
 * @version 1.0
 */
struct DaiDevice {
    const char *devDaiName;         /**< Dai device name */
    struct DaiData *devData;        /**< Dai module private data */
    struct HdfDeviceObject *device; /**< HDF device */
    struct DListHead list;          /**< Dai list */
};

/**
 * @brief Defines Dai operation function set.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioDaiOps {
    /**
     * @brief Defines Dai device start up function.
     *
     * @param audioCard Indicates an audio card device.
     * @param dai Indicates a dai device.
     *
     * @return Returns <b>0</b> if dai device start up success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Startup)(const struct AudioCard *audioCard, const struct DaiDevice *dai);

    /**
     * @brief Defines Dai device hardware param set function.
     *
     * @param audioCard Indicates an audio card device.
     * @param param Indicates pcm param set.
     *
     * @return Returns <b>0</b> if dai pcm param set success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*HwParams)(const struct AudioCard *audioCard, const struct AudioPcmHwParams *param);

    /**
     * @brief Defines Dai device trigger function.
     *
     * @param audioCard Indicates an audio card device.
     * @param cmd Indicates a Command id.
     * @param dai Indicates a dai device.
     *
     * @return Returns <b>0</b> if dai device trigger success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Trigger)(const struct AudioCard *audioCard, int cmd, const struct DaiDevice *dai);
};

/**
 * @brief Defines dai private data.
 *
 * @since 1.0
 * @version 1.0
 */
struct DaiData {
    const char *drvDaiName;                    /**< dai driver name */

    /**
     * @brief Defines Dai device init.
     *
     * @param audioCard Indicates an audio card device.
     * @param dai Indicates a dai device.
     *
     * @return Returns <b>0</b> if dai device init success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DaiInit)(struct AudioCard *audioCard, const struct DaiDevice *dai);

    /**
     * @brief Defines Dai device reg read.
     *
     * @param virtualAddress Indicates base reg IoRemap address.
     * @param reg Indicates reg offset.
     * @param value Indicates read reg value.
     *
     * @return Returns <b>0</b> if dai device read reg success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Read)(unsigned long virtualAddress, uint32_t reg, uint32_t *value);

    /**
     * @brief Defines Dai device reg write.
     *
     * @param virtualAddress Indicates base reg IoRemap address.
     * @param reg Indicates reg offset.
     * @param value Indicates write reg value.
     *
     * @return Returns <b>0</b> if dai device write reg success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Write)(unsigned long virtualAddress, uint32_t reg, uint32_t value);

    const struct AudioDaiOps *ops;             /**< dai set of operation functions */
    struct PcmInfo pcmInfo;                    /**< dai pcm info */
    struct AudioKcontrol *controls;            /**< dai control structure array pointer */
    int numControls;                           /**< dai controls the number of structure array elements */
    bool daiInitFlag;                          /**< dai init flag */
    unsigned long regVirtualAddr;              /**< dai base reg IoRemap address */
    struct AudioRegCfgData *regConfig;         /**< dai registers configured in HCS */
    struct AudioRegCfgGroupNode **regCfgGroup; /**< dai register group configured in HCS */
    struct OsalMutex mutex;                    /**< dai mutex */
};

/**
 * @brief Defines Dai host in audio driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct DaiHost {
    struct IDeviceIoService service; /**< Services provided by dai */
    struct HdfDeviceObject *device;  /**< HDF device */
    void *priv;                      /**< Dai private data interface */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
