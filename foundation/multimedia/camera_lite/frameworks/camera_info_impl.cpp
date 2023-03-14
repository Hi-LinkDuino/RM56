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
#include "camera_info_impl.h"
#include "media_log.h"

using namespace std;
namespace OHOS {
namespace Media {
CameraInfoImpl::CameraInfoImpl(int32_t cameraType, int32_t cameraFacingType)
{
    this->cameraType = cameraType;
    this->cameraFacingType = cameraFacingType;
}
int32_t CameraInfoImpl::GetCameraType() const
{
    return cameraType;
}

void CameraInfoImpl::SetCameraType(int32_t cameraType)
{
    this->cameraType = cameraType;
    return;
}

int32_t CameraInfoImpl::GetCameraFacingType() const
{
    return cameraFacingType;
}

void CameraInfoImpl::SetCameraFacingType(int32_t cameraFacingType)
{
    this->cameraFacingType = cameraFacingType;
    return;
}
} // namespace Media
} // namespace OHOS