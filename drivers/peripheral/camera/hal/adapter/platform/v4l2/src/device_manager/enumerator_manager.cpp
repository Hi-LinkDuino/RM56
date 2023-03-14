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

#include "enumerator_manager.h"

namespace OHOS::Camera {
EnumeratorManager::EnumeratorManager() {}

EnumeratorManager::~EnumeratorManager()
{
    uvcVideo_->V4L2UvcDetectUnInit();
    uvcVideo_ = nullptr;
}

RetCode EnumeratorManager::Init()
{
    uvcVideo_ = std::make_shared<HosV4L2UVC>();
    if (uvcVideo_ == nullptr) {
        CAMERA_LOGE("%s Create HosV4L2UVC fail", __FUNCTION__);
        return RC_ERROR;
    }
    uvcVideo_->V4L2UvcDetectInit([&](const std::string hardwareName,
        std::vector<DeviceControl>& deviceControl,
        std::vector<DeviceFormat>& deviceFormat, bool uvcState) {
        UvcCallBack(hardwareName, deviceControl, deviceFormat, uvcState);
    });
    return RC_OK;
}

void EnumeratorManager::SetCallBack(UvcDataCb cb)
{
    uvcCb_ = cb;
}

void EnumeratorManager::UvcCallBack(const std::string hardwareName,
    std::vector<DeviceControl>& deviceControl,
    std::vector<DeviceFormat>& deviceFormat, bool uvcState)
{
    if (uvcCb_ == nullptr) {
        CAMERA_LOGE("%s uvcCb_ is nullptr", __FUNCTION__);
        return;
    }
    uvcCb_(hardwareName, deviceControl, deviceFormat, uvcState);
}
} // namespace OHOS::Camera