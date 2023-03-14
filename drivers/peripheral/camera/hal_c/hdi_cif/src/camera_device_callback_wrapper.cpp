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

#include "camera_device_callback_wrapper.h"
#ifdef __cplusplus
extern "C" {
#endif

static OHOS::sptr<OHOS::Camera::ICameraDeviceCallback> g_deviceCallback;

void BindCameraDeviceCallback(const OHOS::sptr<OHOS::Camera::ICameraDeviceCallback>& callback)
{
    g_deviceCallback = callback;
}

void DeviceCBOnError(int /*ErrorTypeCIF*/ type, int errorMsg)
{
    if (g_deviceCallback == nullptr) {
        return;
    }

    g_deviceCallback->OnError(static_cast<OHOS::Camera::ErrorType>(type), errorMsg);

    return;
}

void DeviceCBOnResult(long timestamp, CameraResultCIF* result)
{
    if (g_deviceCallback == nullptr) {
        return;
    }

    std::shared_ptr<OHOS::CameraStandard::CameraMetadata> meta = std::make_shared<OHOS::CameraStandard::CameraMetadata>(0,0);
    g_deviceCallback->OnResult(timestamp, meta);

    return;
}

#ifdef __cplusplus
}
#endif
