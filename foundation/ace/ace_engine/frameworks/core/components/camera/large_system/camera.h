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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_LARGE_SYSTEM_CAMERA_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_LARGE_SYSTEM_CAMERA_H

#include <list>

#include "core/components/camera/camera_component.h"
#include "core/components/common/layout/constants.h"
#include "core/components/video/resource/resource.h"
#include "core/components/video/resource/texture.h"

namespace OHOS::Ace {

class Camera : public Resource {
    DECLARE_ACE_TYPE(Camera, Resource);

public:
    using TakePhotoListener = std::function<void(const std::map<std::string, std::string>&)>;
    using ErrorListener = std::function<void(const std::string&, const std::string&)>;
    using SizeChangeListener = std::function<void(int32_t, int32_t)>;
    using PrepareEventListener = std::function<void()>;

    Camera(int64_t textureId, const WeakPtr<PipelineContext>& context, ErrorCallback&& onError,
        DevicePosition devicePosition) : Resource("camera", context, std::move(onError)),
        textureId_(textureId), devicePosition_(devicePosition) {}
    ~Camera() override = default;

    void Release(const std::function<void(bool)>& onRelease = nullptr) override;
    void Create(const std::function<void(int64_t)>& onCreate);
    void CreateCamera(const std::function<void(int64_t)>& onCreate);
    void OpenCamera();

    void TakePhoto(int32_t quality);
    void SetFlashMode(FlashType flash);
    void SetPreViewSize(int32_t width, int32_t height);
    void AddTakePhotoListener(TakePhotoListener&& listener);
    void AddErrorListener(ErrorListener&& listener);
    void AddPreViewSizeChang(SizeChangeListener&& listener);
    void AddPrepareEventListener(PrepareEventListener&& listener);

    void SetBufferSize(Size bufferSize)
    {
        bufferSize_ = bufferSize;
    }

    void SetDeviceOrientation(bool isPortrait)
    {
        isPortrait_ = isPortrait;
    }

    void ChangeCameraCompenentId(bool changType, std::string id);

private:
    void OnAddTakePhotoListener(TakePhotoListener&& listener);
    void OnAddErrorListener(ErrorListener&& listener);
    void UnRegisterEvent();

    void OnTakePhoto(const std::string& param);
    void OnError(const std::string& param);
    void OnPreViewSizeChang(const std::string& param);
    void OnPrepare(const std::string& param);

    int64_t textureId_ = INVALID_ID;
    bool isPortrait_ = true;

    TakePhotoListener takePhotoListener_;
    ErrorListener onErrorListener_;
    SizeChangeListener onSizeChangeListener_;
    PrepareEventListener prepareEventListener_;

    DevicePosition devicePosition_ = DevicePosition::CAMERA_FACING_FRONT;
    Size bufferSize_; // default buffer size
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_LARGE_SYSTEM_CAMERA_H
