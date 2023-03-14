/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_DRIVER_LOG_H
#define AUDIO_DRIVER_LOG_H
#include "hdf_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// 1 is on; 0 is off
#define AUDIO_ADM_DEBUG_ON 0
#define AUDIO_DRIVER_DEBUG_ON 0
#define AUDIO_DEVICE_DEBUG_ON 0

// audio ADM log define
#ifdef __LITEOS__
#define ADM_LOG_DEBUG(fmt, arg...) do { \
    } while (0)
#elif AUDIO_ADM_DEBUG_ON
#define ADM_LOG_DEBUG(fmt, arg...) do { \
    HDF_LOGD("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)
#else
#define ADM_LOG_DEBUG(fmt, arg...) do { \
    } while (0)
#endif

#define ADM_LOG_ERR(fmt, arg...) do { \
    HDF_LOGE("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

#define ADM_LOG_INFO(fmt, arg...) do { \
    HDF_LOGI("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

#define ADM_LOG_WARNING(fmt, arg...) do { \
    HDF_LOGW("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

// audio driver log define
#ifdef __LITEOS__
#define AUDIO_DRIVER_LOG_DEBUG(fmt, arg...) do { \
    } while (0)
#elif AUDIO_DRIVER_DEBUG_ON
#define AUDIO_DRIVER_LOG_DEBUG(fmt, arg...) do { \
    HDF_LOGD("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)
#else
#define AUDIO_DRIVER_LOG_DEBUG(fmt, arg...) do { \
    } while (0)
#endif

#define AUDIO_DRIVER_LOG_ERR(fmt, arg...) do { \
    HDF_LOGE("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

#define AUDIO_DRIVER_LOG_INFO(fmt, arg...) do { \
    HDF_LOGI("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

#define AUDIO_DRIVER_LOG_WARNING(fmt, arg...) do { \
    HDF_LOGW("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

// audio device log define
#ifdef __LITEOS__
#define AUDIO_DEVICE_LOG_DEBUG(fmt, arg...) do { \
    } while (0)
#elif AUDIO_DEVICE_DEBUG_ON
#define AUDIO_DEVICE_LOG_DEBUG(fmt, arg...) do { \
    HDF_LOGD("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)
#else
#define AUDIO_DEVICE_LOG_DEBUG(fmt, arg...) do { \
    } while (0)
#endif

#define AUDIO_DEVICE_LOG_ERR(fmt, arg...) do { \
    HDF_LOGE("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

#define AUDIO_DEVICE_LOG_INFO(fmt, arg...) do { \
    HDF_LOGI("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

#define AUDIO_DEVICE_LOG_WARNING(fmt, arg...) do { \
    HDF_LOGW("[%s][line:%d]: " fmt, __func__, __LINE__, ##arg); \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_DRIVER_LOG_H */
