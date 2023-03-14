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

#include "camera_host_callback_wrapper.h"
#ifdef __cplusplus
extern "C" {
#endif

static OHOS::sptr<OHOS::Camera::ICameraHostCallback> g_callback;
void BindCameraHostCallback(const OHOS::sptr<OHOS::Camera::ICameraHostCallback>& callback)
{
    g_callback = callback;
}

void HostCBOnCameraStatus(char* cameraId, int cameraStatus)
{
    std::string id(cameraId);
    g_callback->OnCameraStatus(id, static_cast<OHOS::Camera::CameraStatus>(cameraStatus));
    return;
}

void HostCBOnFlashlightStatus(char* cameraId, int flashlightStatus)
{
    std::string id(cameraId);
    g_callback->OnFlashlightStatus(id, static_cast<OHOS::Camera::FlashlightStatus>(flashlightStatus));
}

#ifdef __cplusplus
}
#endif
