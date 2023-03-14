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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_ELEMENT_H

#ifdef OHOS_STANDARD_SYSTEM
#include "core/components/camera/standard_system/camera.h"
#else
#include "core/components/camera/large_system/camera.h"
#endif

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
#include "core/components/align/align_component.h"
#include "core/components/text/text_component.h"
#include "core/components/theme/theme_manager.h"
#endif
#include "core/components/camera/camera_component.h"
#include "core/components/camera/camera_theme.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/composed_element.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

enum CameraState : int32_t {
    STATE_UMDEFINED,
    STATE_ENABLE,
    STATE_DISABLE
};

class CameraElement : public RenderElement, public FocusNode {
    DECLARE_ACE_TYPE(CameraElement, RenderElement, FocusNode);

public:
    using ErrorCallback = std::function<void(const std::string&, const std::string&)>;
    using EventCallback = std::function<void(const std::string&)>;

    CameraElement() = default;
    ~CameraElement() override;

    void Prepare(const WeakPtr<Element>& parent) override;
    void SetNewComponent(const RefPtr<Component>& newComponent) override;
    void TakePhoto(const TakePhotoParams& params);

#ifdef OHOS_STANDARD_SYSTEM
    void StartRecord();
    void CloseRecorder(const std::string& params);
    void StartPreview();
    void StopPreview();
#endif

private:
    void SetMethodCall(const RefPtr<CameraComponent>& cameraComponent);
    void OnTextureSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight);
    void CreatePlatformResource();
    void ReleasePlatformResource();
    void InitListener();
    void OnTakePhotoCallBack(const std::map<std::string, std::string>& result);
    void ExecuteJsCallback(const std::string& callbackId, const std::string& result);
    void InitEvent(const RefPtr<CameraComponent>& cameraComponent);
    void OnError(const std::string& errorcode, const std::string& errormsg);
    void HiddenChange(bool hidden);
    void OnPrepared();

#ifdef OHOS_STANDARD_SYSTEM
    void CreateCamera();
    void OnRecorderCallBack(const std::map<std::string, std::string>& result);
    void OnTextureOffset(int64_t textureId, int32_t x, int32_t y);
    void TakePhoto(const Size& size);
    void CloseRecorder();

    std::string recorderBackId_;
    bool isRecording_ = false;
    bool isPreViewing_ = false;
#else
    void CreateTexture();
    void CreateCamera(int64_t id, ErrorCallback &&errorCallback);
    void UpdateCamera(const RefPtr<CameraComponent>& cameraComponent);
    void OnPreViewSizeChange(int32_t preViewWidth, int32_t preViewHeight);
    void InitDeclarativePara(const RefPtr<CameraComponent>& cameraComponent);
    void HandleDeclarativePara();
    void RealseDeclarativePara();
    void OnTextureRefresh();

    RefPtr<Texture> texture_;
    FlashType flash_ = FlashType::AUTO;
    DevicePosition devicePosition_ = DevicePosition::CAMERA_FACING_BACK;
    int32_t preViewWidth_ = 0;
    int32_t preViewHeight_ = 0;
    int32_t resolutionWidth_ = 0;
    int32_t resolutionHeight_ = 0;
    bool isSetResolution_ = false;
    std::string cameraId_;
#endif
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void UpdataChild(const RefPtr<Component>& childComponent);
    const RefPtr<Component> CreateErrorText(const std::string& errorMsg);
    TextDirection textDirection_ = TextDirection::LTR;
#endif

    EventCallback onTakePhotoEvent_;
    EventCallback onError_;
    TakePhotoParams callbackIds_;
    RefPtr<CameraTheme> theme_;
    RefPtr<Camera> camera_;

    CameraState cameraState_ = CameraState::STATE_UMDEFINED;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_ELEMENT_H
