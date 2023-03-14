/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_PLATFORM_IF_H
#define AUDIO_PLATFORM_IF_H

#include "audio_host.h"
#include "audio_platform_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines paltform private data.
 *
 * @since 1.0
 * @version 1.0
 */
struct PlatformData {
    const char *drvPlatformName;         /**< Platform module name */

    /**
     * @brief Defines platform device init.
     *
     * @param audioCard Indicates an audio card device.
     * @param platform Indicates a platform device.
     *
     * @return Returns <b>0</b> if Platform device init success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*PlatformInit)(const struct AudioCard *audioCard, const struct PlatformDevice *platform);

    struct AudioDmaOps *ops;             /**< Platform module private data */
    struct CircleBufInfo renderBufInfo;  /**< Render pcm stream transfer */
    struct CircleBufInfo captureBufInfo; /**< Capture pcm stream transfer */
    struct PcmInfo renderPcmInfo;        /**< Render pcm stream info */
    struct PcmInfo capturePcmInfo;       /**< Capture pcm stream info */
    bool platformInitFlag;               /**< Platform init flag */
    struct AudioMmapData mmapData;       /**< Mmap mode transfer data */
    uint32_t mmapLoopCount;              /**< Loop count for mmap mode pcm stream */
    void *dmaPrv;                        /**< Platform dma private data */
};

/**
 * @brief Defines Dma operation function set.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioDmaOps {
    /**
     * @brief Defines Dma buff alloc.
     *
     * @param platformData Indicates dma device.
     * @param streamType Indicates capture or render.
     *
     * @return Returns <b>0</b> if dma buffer alloc success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaBufAlloc)(struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines dma buffer free.
     *
     * @param platformData Indicates dma device.
     * @param streamType Indicates capture or render.
     *
     * @return Returns <b>0</b> if dma buffer free success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaBufFree)(struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines dma request channel.
     *
     * @param platformData Indicates dma device.
     *
     * @return Returns <b>0</b> if dma request channel success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaRequestChannel)(const struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines dma channel config.
     *
     * @param platformData Indicates dma device.
     *
     * @return Returns <b>0</b> if dma channel config set success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaConfigChannel)(const struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines dma prepare function.
     *
     * @param platformData Indicates dma device.
     *
     * @return Returns <b>0</b> if dma device prep set success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaPrep)(const struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines dma submit function.
     *
     * @param platformData Indicates dma device.
     *
     * @return Returns <b>0</b> if dma device submit success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaSubmit)(const struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines dma pending function.
     *
     * @param platformData Indicates dma device.
     *
     * @return Returns <b>0</b> if dma pending success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaPending)(struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines pcm stream transfer pause.
     *
     * @param platformData Indicates dma device.
     *
     * @return Returns <b>0</b> if pcm stream transfer pause success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaPause)(struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines pcm stream transfer resume.
     *
     * @param platformData Indicates dma device.
     *
     * @return Returns <b>0</b> if pcm stream transfer resume success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaResume)(const struct PlatformData *platformData, const enum AudioStreamType streamType);

    /**
     * @brief Defines Get the function of the current playback or recording position.
     *
     * @param platformData Indicates dma device.
     * @param pointer Indicates dma pointer.
     *
     * @return Returns <b>0</b> if dma device pointer position get success; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DmaPointer)(struct PlatformData *platformData, const enum AudioStreamType streamType, uint32_t *pointer);
};

/**
 * @brief Defines Dai device name and data.
 *
 * @since 1.0
 * @version 1.0
 */
struct PlatformDevice {
    const char *devPlatformName;    /**< Platform device name */
    struct PlatformData *devData;   /**< Platform module private data */
    struct HdfDeviceObject *device; /**< HDF device */
    struct DListHead list;          /**< Platform list */
};

/**
 * @brief Defines Platform host in audio driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct PlatformHost {
    struct IDeviceIoService service; /**< Services provided by patform */
    struct HdfDeviceObject *device;  /**< HDF device */
    void *priv;                      /**< Platform private data interface */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
