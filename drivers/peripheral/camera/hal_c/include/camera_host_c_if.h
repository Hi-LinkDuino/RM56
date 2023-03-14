/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HOS_CAMERA_HOST_C_IF_H
#define HOS_CAMERA_HOST_C_IF_H

#include "camera_types_c_if.h"
#include "camera_device_c_if.h"
#include "camera_metadata_c_if.h"

#define CAMERA_C_HAL_LIB_PATH HDF_LIBRARY_FULL_PATH("libcamera_c_hal")

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CameraHostCallbackCIF {
    void (*OnCameraStatus)(char* cameraId, int cameraStatus);
    void (*OnFlashlightStatus)(char* cameraId, int flashlightStatus);
} CameraHostCallbackCIF;

typedef struct CameraHostCIF {
    int (*SetCallback)(CameraHostCallbackCIF callback);
    int (*GetCameraIds)(char** ids[], int* size);
    int (*GetCameraAbility)(const char* cameraId, CameraAbilityCIF* ability);
    int (*OpenCamera)(const char* cameraId, CameraDeviceCallbackCIF callback, CameraDeviceCIF* device);
    int (*SetFlashlight)(const char* cameraId, int* isEnable);
} CameraHostCIF;

typedef int (*HOST_C_FUNC_SETCALLBACK)(CameraHostCallbackCIF callback);
typedef int (*HOST_C_FUNC_GETCAMERAIDS)(char** ids[], int* size);
typedef int (*HOST_C_FUNC_GETCAMERAABILITY)(const char* cameraId, CameraAbilityCIF* ability);
typedef int (*HOST_C_FUNC_OPENCAMERA)(const char* cameraId, CameraDeviceCallbackCIF callback, CameraDeviceCIF* device);
typedef int (*HOST_C_FUNC_SETFLASHLIGHT)(const char* cameraId, int* isEnable);

#ifdef __cplusplus
}
#endif

#endif
