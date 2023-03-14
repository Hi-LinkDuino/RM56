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

#ifndef HOS_CAMERA_HOST_CALLBACK_WRAPPER_H
#define HOS_CAMERA_HOST_CALLBACK_WRAPPER_H

#include <memory>
#include "types.h"
#include "icamera_host_callback.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void BindCameraHostCallback(const OHOS::sptr<OHOS::Camera::ICameraHostCallback>& callback);
extern void HostCBOnCameraStatus(char* cameraId, int cameraStatus);
extern void HostCBOnFlashlightStatus(char* cameraId, int flashlightStatus);

#ifdef __cplusplus
}
#endif

#endif


