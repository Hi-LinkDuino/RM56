/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_META_DATA_H
#define OHOS_META_DATA_H

#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} CameraRect;

typedef struct {
    uint32_t width;
    uint32_t height;
} CameraPicSize;

typedef enum {
    /* Frame rate */
    CAM_FRAME_FPS = 0,
    /* Auto exposure mode */
    CAM_AE_MODE,
    /* Auto exposure time */
    CAM_AE_EXPO_TIME,
    /* Auto exposure compensation value */
    CAM_AE_COMPENSATION,
    /* Auto exposure lock */
    CAM_AE_LOCK,
    /* Auto exposure region */
    CAM_AE_REGIONS,
    /* White balance mode */
    CAM_AWB_MODE,
    /* White balance lock */
    CAM_AWB_LOCK,
    /* White balance region */
    CAM_AWB_REGION,
    /* Video stabilization feature */
    CAM_VIDEO_STABILIZATION_NABLE,
    /* Image luminance */
    CAM_IMAGE_LUMA,
    /* Image contrast */
    CAM_IMAGE_CONTRAST,
    /* Image hue */
    CAM_IMAGE_HUE,
    /* Image saturation */
    CAM_IMAGE_SATURATION,
    /* Hot pixel correction */
    CAM_HOT_PIXEL_ENABLE,
    /* Level of hot pixel correction */
    CAM_HOT_PIXEL_LEVEL,
    /* Image denoising mode */
    CAM_IMAGE_DENOISE_MODE,
    /* Image denoising level */
    CAM_IMAGE_DENOISE_LEVEL,
    /* Image mirror flip */
    CAM_IMAGE_INVERT_MODE,
    /* Image CROP_RECT */
    CAM_IMAGE_CROP_RECT,
    /* Only vaild when frameConfigType is callback*/
    CAM_IMAGE_FORMAT,
    /* Auto foucue mode */
    CAM_AF_MODE,
    /* Image denoising level */
    PARAM_KEY_SIZE,
    PARAM_KEY_IMAGE_ENCODE_QFACTOR
} CameraFuncKey;

/**
 @brief Enumerates parameters for image rotation and mirroring.
 */
typedef enum {
    CAM_CENTER_MIRROR = 0,     /**< Central mirroring */
    CAM_HORIZONTAL_MIRROR = 1, /**< Horizontal mirroring */
    CAM_VETICALLY_MIRROR = 2,  /**< Vertical mirroring */
    CAM_ROTATION_90 = 3,       /**< 90-degree rotation */
    CAM_ROTATION_180 = 4,      /**< 180-degree rotation  */
} CameraInvertMode;

#define ENCODER_TYPE 0
typedef enum {
    CAM_FORMAT_YVU420 = 0,
    CAM_FORMAT_JPEG,
    CAM_FORMAT_H264,
    CAM_FORMAT_H265,
    CAM_FORMAT_BUTT
} CameraEncodeFormat;

typedef enum {
    CAM_IMAGE_YUV420,
    CAM_IMAGE_RAW12
} CameraImageFormat;

typedef enum {
    AF_MODE_CONTINUOUS,
    AF_MODE_CLOSE
} AfMode;

typedef enum {
    AE_MODE_OPEN,
    AE_MODE_CLOSE
} AeMode;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif // OHOS_META_DATA_H