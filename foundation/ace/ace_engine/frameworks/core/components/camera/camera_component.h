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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_COMPONENT_H

#include <string>

#include "base/utils/utils.h"
#include "core/components/video/texture_component.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

enum class DevicePosition : int32_t {
    CAMERA_FACING_FRONT = 0,
    CAMERA_FACING_BACK,
    CAMERA_FACING_OTHERS,
};

enum class FlashType : int32_t {
    OFF = 0,
    ON,
    AUTO,
    TORCH,
};

struct TakePhotoParams {
    std::string quality;
    std::string success;
    std::string fail;
    std::string complete;

    void clear()
    {
        quality.clear();
        success.clear();
        fail.clear();
        complete.clear();
    }
};

class CameraController : public virtual AceType {
    DECLARE_ACE_TYPE(CameraController, AceType);

public:
    using TakePhotoImpl = std::function<void(const TakePhotoParams&)>;
    using StartRecorderImpl = std::function<void()>;
    using CloseRecordImpl = std::function<void(const std::string&)>;

    void TakePhoto(const TakePhotoParams& params)
    {
        if (takePhotoImpl_) {
            takePhotoImpl_(params);
        }
    }

    void SetTakePhotoImpl(TakePhotoImpl&& takePhotoImpl)
    {
        takePhotoImpl_ = std::move(takePhotoImpl);
    }

    void StartRecord()
    {
        if (startRecorderImpl_) {
            startRecorderImpl_();
        }
    }

    void SetStartRecordImpl(StartRecorderImpl&& startRecorderImpl)
    {
        startRecorderImpl_ = std::move(startRecorderImpl);
    }

    void CloseRecorder(const std::string& recorderId)
    {
        if (closeRecordImpl_) {
            closeRecordImpl_(recorderId);
        }
    }

    void SetCloseRecorderImpl(CloseRecordImpl&& closeRecordImpl)
    {
        closeRecordImpl_ = std::move(closeRecordImpl);
    }

private:
    TakePhotoImpl takePhotoImpl_;
    StartRecorderImpl startRecorderImpl_;
    CloseRecordImpl closeRecordImpl_;
};

class ACE_EXPORT CameraComponent : public TextureComponent {
    DECLARE_ACE_TYPE(CameraComponent, TextureComponent);

public:
    CameraComponent()
    {
        cameraController_ = AceType::MakeRefPtr<CameraController>();
    };
    ~CameraComponent() override = default;

    RefPtr<CameraController> GetCameraController() const
    {
        return cameraController_;
    }

    void SetFlash(FlashType flash)
    {
        flash_ = flash;
    }

    FlashType GetFlash() const
    {
        return flash_;
    }

    void SetDevicePosition(DevicePosition devicePosition)
    {
        devicePosition_ = devicePosition;
    }

    DevicePosition GetDevicePosition() const
    {
        return devicePosition_;
    }

    void SetErrorEventId(const EventMarker& eventId)
    {
        errorEventId_ = eventId;
    }

    const EventMarker& GetErrorEventId() const
    {
        return errorEventId_;
    }

    void SetCameraId(const std::string& cameraId)
    {
        cameraId_ = cameraId;
    }

    const std::string& GetCameraId() const
    {
        return cameraId_;
    }

    void SetResolutionWidth(int32_t resolutionWidth)
    {
        resolutionWidth_ = resolutionWidth;
    }

    int32_t GetResolutionWidth() const
    {
        return resolutionWidth_;
    }

    void SetResolutionHeight(int32_t resolutionHeight)
    {
        resolutionHeight_ = resolutionHeight;
    }

    int32_t GetResolutionHeight() const
    {
        return resolutionHeight_;
    }

    void SignSetResolution(bool isSetResolution)
    {
        isSetResolution_ = isSetResolution;
    }

    bool IsSetResolution() const
    {
        return isSetResolution_;
    }

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

private:
    RefPtr<CameraController> cameraController_;
    FlashType flash_ = FlashType::AUTO;
    DevicePosition devicePosition_ = DevicePosition::CAMERA_FACING_BACK;
    EventMarker errorEventId_;
    std::string cameraId_;
    int32_t resolutionWidth_ = 0;
    int32_t resolutionHeight_ = 0;
    bool isSetResolution_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_COMPONENT_H
