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
#ifndef OHOS_CAMERA_INFO_IMPL_H
#define OHOS_CAMERA_INFO_IMPL_H

#include "camera_info.h"

namespace OHOS {
namespace Media {
class CameraInfoImpl : public CameraInfo {
public:
    CameraInfoImpl() = delete;
    ~CameraInfoImpl() = default;
    CameraInfoImpl(int32_t cameraType, int32_t cameraFacingType);

    void SetCameraType(int32_t cameraType);
    int32_t GetCameraType() const override;
    void SetCameraFacingType(int32_t cameraFacingType);
    int32_t GetCameraFacingType() const override;

private:
    int32_t cameraType;
    int32_t cameraFacingType;
};
} // namespace Media
} // namespace OHOS


#endif // OHOS_CAMERA_INFO_IMPL_H
