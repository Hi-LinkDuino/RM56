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

#include "core/components/camera/camera_element.h"

#include <sstream>
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
#include "base/i18n/localization.h"
#endif
#include "base/utils/system_properties.h"
#include "core/components/camera/camera_component.h"
#include "core/components/video/render_texture.h"

namespace OHOS::Ace {
namespace {

const char CALLBACK_COMPLETE[] = "complete";
const char IS_SUCESS[] = "isSucceed";
const char PHOTO_PATH[] = "uri";
const char NULL_STRING[] = "";
const char ERROR_MSG[] = "errormsg";
const char ERROR_CODE[] = "errorcode";

inline std::string GetStringFromMap(const std::map<std::string, std::string>& map, std::string key)
{
    auto iter = map.find(key);
    if (iter != map.end()) {
        return iter->second;
    }
    return NULL_STRING;
}

#ifndef OHOS_STANDARD_SYSTEM
inline int32_t GetSizeFromQuality(const std::string& quality)
{
    int32_t qualityInt;
    if (quality == "high") {
        qualityInt = 2;
    } else if (quality == "low") {
        qualityInt = 0;
    } else {
        qualityInt = 1;
    }
    return qualityInt;
}
#endif

} // namespace

CameraElement::~CameraElement()
{
    ReleasePlatformResource();
}

void CameraElement::ReleasePlatformResource()
{
#ifdef OHOS_STANDARD_SYSTEM
    StopPreview();
    if (camera_) {
        camera_->Release();
        camera_.Reset();
    }
#else
    callbackIds_.clear();
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    if (texture_) {
        auto platformTaskExecutor =
            SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

        // Release camera first.
        if (camera_) {
            RealseDeclarativePara();
            camera_->Release();

            if (platformTaskExecutor.IsRunOnCurrentThread()) {
                camera_.Reset();
            } else {
                // Make sure it's destroyed when it's release task done.
                platformTaskExecutor.PostTask([camera = camera_]() {});
            }
        }

        if (platformTaskExecutor.IsRunOnCurrentThread()) {
            texture_->Release();
            texture_.Reset();
        } else {
#if defined(ENABLE_NATIVE_VIEW)
            texture_->Release();
            // Make sure it's destroyed when it's release task done.
            platformTaskExecutor.PostTask([texture = texture_]() {});
#else
            auto gpuTaskExecutor =
                SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::GPU);
            // Release texture after paint.
            auto weak = AceType::WeakClaim(AceType::RawPtr(texture_));
            gpuTaskExecutor.PostTask([weak, platformTaskExecutor]() {
                auto texture = weak.Upgrade();
                if (texture == nullptr) {
                    LOGE("texture is nullptr");
                    return;
                }
                texture->Release();
                // Make sure it's destroyed when it's release task done.
                platformTaskExecutor.PostTask([texture]() {});
            });
#endif
        }
    }
#endif
}

void CameraElement::Prepare(const WeakPtr<Element>& parent)
{
    auto themeManager = GetThemeManager();
    if (!themeManager) {
        return;
    }

    auto cameraComponent = AceType::DynamicCast<CameraComponent>(component_);
    theme_ = themeManager->GetTheme<CameraTheme>();
    if (cameraComponent) {
#ifndef OHOS_STANDARD_SYSTEM
        InitDeclarativePara(cameraComponent);
#endif
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        textDirection_ = cameraComponent->GetTextDirection();
#endif
        InitEvent(cameraComponent);
        SetMethodCall(cameraComponent);
        CreatePlatformResource();
    }

    RenderElement::Prepare(parent);
    if (renderNode_) {
        auto renderTexture = AceType::DynamicCast<RenderTexture>(renderNode_);
        renderTexture->SetTextureSizeChange(
            [weak = WeakClaim(this)](int64_t textureId, int32_t textureWidth, int32_t textureHeight) {
                auto cameraElement = weak.Upgrade();
                if (cameraElement) {
                    cameraElement->OnTextureSize(textureId, textureWidth, textureHeight);
                }
            });
        renderTexture->SetHiddenChangeEvent(
            [weak = WeakClaim(this)](bool hidden) {
                auto cameraElement = weak.Upgrade();
                if (cameraElement) {
                    cameraElement->HiddenChange(hidden);
                }
            });
#if OHOS_STANDARD_SYSTEM
        renderTexture->SetTextureOffsetChange(
            [weak = WeakClaim(this)](int64_t textureId, int32_t x, int32_t y) {
                auto cameraElement = weak.Upgrade();
                if (cameraElement) {
                    cameraElement->OnTextureOffset(textureId, x, y);
                }
            });
#else
        renderTexture->SetIsAddGaussianFuzzy(false);
#endif
    }
}

void CameraElement::HiddenChange(bool hidden)
{
    if (cameraState_ == CameraState::STATE_DISABLE) {
        LOGE("CameraElement: CameraState is not enable when hidden change.");
        return;
    }

    if (hidden) {
        LOGI("CameraElement::HiddenChange: hidden.");
        ReleasePlatformResource();
    } else {
        LOGI("CameraElement::HiddenChange: show.");
        CreatePlatformResource();
    }
}

void CameraElement::InitEvent(const RefPtr<CameraComponent>& cameraComponent)
{
    if (!cameraComponent->GetErrorEventId().IsEmpty()) {
        onError_ = AceAsyncEvent<void(const std::string&)>::Create(cameraComponent->GetErrorEventId(), context_);
    }
}

void CameraElement::OnTextureSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight)
{
    LOGI("CameraElement::OnTextureSize");
    auto renderTexture = AceType::DynamicCast<RenderTexture>(renderNode_);
    if (!renderTexture || !camera_) {
        LOGI("CameraElement: renderNode is not renderTexture type or camera is null..");
        return;
    }

#ifdef OHOS_STANDARD_SYSTEM
    if (renderTexture->GetFit() == ImageFit::FILL) {
        LOGI("CameraElement:OnTextureSize  %{public}d %{public}d ", textureWidth, textureHeight);
        camera_->OnCameraSizeChange(textureWidth, textureHeight);
    }
#else
    if (!texture_) {
        LOGI("CameraElement: texture is null.");
        return;
    }

    if (renderTexture->IsComponentSize() && renderTexture->GetFit() == ImageFit::FILL) {
        if (isSetResolution_) {
            camera_->SetPreViewSize(resolutionWidth_, resolutionHeight_);
        } else {
            camera_->SetPreViewSize(textureWidth, textureHeight);
        }
        renderTexture->SetIsComponentSize(false);
    } else {
        texture_->OnSize(textureId, textureWidth, textureHeight);
    }
#endif
}

void CameraElement::SetNewComponent(const RefPtr<Component>& newComponent)
{
    if (newComponent == nullptr) {
        Element::SetNewComponent(newComponent);
        return;
    }

    auto cameraComponent = AceType::DynamicCast<CameraComponent>(newComponent);
    if (cameraComponent) {
#ifdef OHOS_STANDARD_SYSTEM
        cameraComponent->SetFit(ImageFit::FILL);
#else
        if (devicePosition_ == cameraComponent->GetDevicePosition()) {
            UpdateCamera(cameraComponent);
        } else {
            UpdateCamera(cameraComponent);
            if (cameraState_ != CameraState::STATE_DISABLE) {
                if (renderNode_) {
                    auto renderTexture = AceType::DynamicCast<RenderTexture>(renderNode_);
                    if (renderTexture) {
                        renderTexture->SetIsAddGaussianFuzzy(true);
                    }
                }
                CreatePlatformResource();
            }
        }
#endif
    }
    Element::SetNewComponent(newComponent);
}

#ifdef OHOS_STANDARD_SYSTEM
void CameraElement::OnTextureOffset(int64_t textureId, int32_t x, int32_t y)
{
    LOGI("CameraElement::OnTextureOffset");
    auto renderTexture = AceType::DynamicCast<RenderTexture>(renderNode_);
    if (!renderTexture) {
        LOGI("CameraElement::OnTextureSize: renderNode is not renderTexture type.");
        return;
    }

    if (camera_ && renderTexture->GetFit() == ImageFit::FILL) {
        LOGI("Camera:OnTextureOffset %{public}d %{public}d", x, y);
        camera_->OnCameraOffsetChange(x, y);
    }
}

void CameraElement::CreateCamera()
{
    LOGI("CameraElement::CreateCamera");
    camera_ = AceType::MakeRefPtr<Camera>(context_);
    InitListener();
    camera_->SetRenderNode(GetRenderNode());
    camera_->Create(nullptr);
    StartPreview();
}

void CameraElement::OnRecorderCallBack(const std::map<std::string, std::string>& result)
{
    if (recorderBackId_.empty()) {
        return;
    }

    std::string param = std::string("{\"arguments\":[");
    if (GetStringFromMap(result, IS_SUCESS) == "1") {
        param.append("{\"")
            .append(PHOTO_PATH)
            .append("\":\"file://")
            .append(GetStringFromMap(result, PHOTO_PATH))
            .append("\"}],\"method\":\"success\"}");
        ExecuteJsCallback(recorderBackId_, param);
    } else if (GetStringFromMap(result, IS_SUCESS) != "1") {
        param.append("{\"")
            .append(ERROR_MSG)
            .append("\":\"")
            .append(GetStringFromMap(result, ERROR_MSG))
            .append("\", \"")
            .append(ERROR_CODE)
            .append("\":\"")
            .append(GetStringFromMap(result, ERROR_CODE))
            .append("\"}],\"method\":\"fail\"}");
        ExecuteJsCallback(recorderBackId_, param);
    }

    std::string complete = std::string("{\"arguments\":[{\"")
        .append(CALLBACK_COMPLETE)
        .append("\":\"\"}],\"method\":\"complete\"}");
    ExecuteJsCallback(recorderBackId_, complete);
}

void CameraElement::StartPreview()
{
    if (camera_ && !isPreViewing_) {
        camera_->StartPreview();
        isPreViewing_ = true;
    }
}

void CameraElement::StopPreview()
{
    if (camera_ && isPreViewing_) {
        camera_->Stop(true);
        isPreViewing_ = false;
    }
}

void CameraElement::TakePhoto(const Size& size)
{
    if (camera_) {
        camera_->TakePhoto(size);
    }
}

void CameraElement::StartRecord()
{
    LOGI("CameraElement::StartRecord.");
    if (camera_ && !isRecording_) {
        camera_->StartRecord();
        isRecording_ = true;
    }
}

void CameraElement::CloseRecorder(const std::string& params)
{
    LOGI("CameraElement::CloseRecorder.");
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    if (camera_ && isRecording_) {
        recorderBackId_ = params;

        context->GetTaskExecutor()->PostTask(
            [weak = WeakClaim(this)] {
                auto cameraElement = weak.Upgrade();
                if (cameraElement) {
                    cameraElement->CloseRecorder();
                }
            },
            TaskExecutor::TaskType::PLATFORM);
        isRecording_ = false;
    }
}

void CameraElement::CloseRecorder()
{
    if (camera_) {
        LOGI("CameraElement:CloseRecorder Task");
        camera_->Stop(false);
    }
}

#else
void CameraElement::UpdateCamera(const RefPtr<CameraComponent> &cameraComponent)
{
    if (flash_ != cameraComponent->GetFlash()) {
        flash_ = cameraComponent->GetFlash();
        if (camera_) {
            camera_->SetFlashMode(flash_);
        }
    }
    devicePosition_ = cameraComponent->GetDevicePosition();
    if (texture_) {
        cameraComponent->SetTextureId(texture_->GetId());
        cameraComponent->SetFit(ImageFit::CONTAIN);
        cameraComponent->SetSrcWidth(preViewWidth_);
        cameraComponent->SetSrcHeight(preViewHeight_);
    }
}

void CameraElement::CreateTexture()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    auto uiTaskExecutor =
        SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    auto errorCallback = [weak = WeakClaim(this), uiTaskExecutor](
                             const std::string& errorcode, const std::string& errormsg) {
        uiTaskExecutor.PostTask([weak, errorcode, errormsg] {
            auto cameraElement = weak.Upgrade();
            if (cameraElement) {
                cameraElement->OnError(errorcode, errormsg);
            }
        });
    };
    texture_ = AceType::MakeRefPtr<Texture>(context_, errorCallback);

    texture_->Create([weak = WeakClaim(this), errorCallback](int64_t id) mutable {
        auto cameraElement = weak.Upgrade();
        if (cameraElement) {
            cameraElement->CreateCamera(id, std::move(errorCallback));
        }
    });
}

void CameraElement::CreateCamera(int64_t id, ErrorCallback&& errorCallback)
{
    camera_ = AceType::MakeRefPtr<Camera>(id, context_, std::move(errorCallback), devicePosition_);
    InitListener();
    if (theme_) {
        camera_->SetBufferSize(theme_->GetBufferSize());
    }
    camera_->SetDeviceOrientation(
        SystemProperties::GetDevcieOrientation() == DeviceOrientation::PORTRAIT);
    camera_->Create(nullptr);
    HandleDeclarativePara();
}

void CameraElement::OnPreViewSizeChange(int32_t preViewWidth, int32_t preViewHeight)
{
    auto camera = AceType::MakeRefPtr<CameraComponent>();
    camera->SetTextureId(texture_->GetId());
    camera->SetFit(ImageFit::CONTAIN);
    camera->SetSrcWidth(preViewWidth);
    camera->SetSrcHeight(preViewHeight);
    preViewWidth_ = preViewWidth;
    preViewHeight_ = preViewHeight;

    if (renderNode_ != nullptr) {
        renderNode_->Update(camera);
    }
}

void CameraElement::InitDeclarativePara(const RefPtr<CameraComponent>& cameraComponent)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    if (context->GetIsDeclarative()) {
        cameraId_ = cameraComponent->GetCameraId();
        isSetResolution_ = cameraComponent->IsSetResolution();
        if (isSetResolution_) {
            resolutionWidth_ = cameraComponent->GetResolutionWidth();
            resolutionHeight_ = cameraComponent->GetResolutionHeight();
        }
    }
}

void CameraElement::HandleDeclarativePara()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    if (context->GetIsDeclarative() && camera_) {
        camera_->ChangeCameraCompenentId(true, cameraId_);
    }
}

void CameraElement::RealseDeclarativePara()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    if (context->GetIsDeclarative() && camera_) {
        camera_->ChangeCameraCompenentId(false, cameraId_);
    }
}

void CameraElement::OnTextureRefresh()
{
    if (renderNode_) {
        renderNode_->MarkNeedRender();
    }
}
#endif

void CameraElement::OnPrepared()
{
    LOGI("CameraElement::OnPrepared");
    auto renderTexture = AceType::DynamicCast<RenderTexture>(renderNode_);
    if (!renderTexture) {
        LOGE("CameraElement: the renderNode is not renderTexture type.");
        return;
    }

    auto camera = AceType::MakeRefPtr<CameraComponent>();
    camera->SetFit(ImageFit::FILL);
#ifndef OHOS_STANDARD_SYSTEM
    camera->SetTextureId(texture_->GetId());
    renderTexture->SetIsComponentSize(true);
#endif
    renderTexture->Update(camera);
    cameraState_ = CameraState::STATE_ENABLE;

#ifndef OHOS_STANDARD_SYSTEM
    renderTexture->OnLayout();
    camera_->SetFlashMode(flash_);
#endif
}

void CameraElement::CreatePlatformResource()
{
    cameraState_ = CameraState::STATE_DISABLE;
    ReleasePlatformResource();

    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

#ifdef OHOS_STANDARD_SYSTEM
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)] {
            auto cameraElement = weak.Upgrade();
            if (cameraElement) {
                cameraElement->CreateCamera();
            }
        },
        TaskExecutor::TaskType::PLATFORM);
#else
    CreateTexture();
#endif
}

void CameraElement::SetMethodCall(const RefPtr<CameraComponent>& cameraComponent)
{
    auto cameraController = cameraComponent->GetCameraController();
    if (cameraController) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        auto uiTaskExecutor =
            SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        cameraController->SetTakePhotoImpl(
            [weak = WeakClaim(this), uiTaskExecutor](
                const TakePhotoParams& params) {
                    uiTaskExecutor.PostTask([weak, params] {
                        auto cameraElement = weak.Upgrade();
                        if (cameraElement) {
                            cameraElement->TakePhoto(params);
                        }
                    });
        });
#ifdef OHOS_STANDARD_SYSTEM
        cameraController->SetStartRecordImpl(
            [weak = WeakClaim(this), uiTaskExecutor]() {
                    uiTaskExecutor.PostTask([weak] {
                        auto cameraElement = weak.Upgrade();
                        if (cameraElement) {
                            cameraElement->StartRecord();
                        }
                    });
        });

        cameraController->SetCloseRecorderImpl(
            [weak = WeakClaim(this), uiTaskExecutor](
                const std::string& params) {
                    uiTaskExecutor.PostTask([weak, params] {
                        auto cameraElement = weak.Upgrade();
                        if (cameraElement) {
                            cameraElement->CloseRecorder(params);
                        }
                    });
        });
#endif
    }
}

void CameraElement::InitListener()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    if (!camera_) {
        LOGE("CameraElement::InitListener: camera is null.");
        return;
    }

    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    auto takePhotoListener = [weak = WeakClaim(this), uiTaskExecutor](
        const std::map<std::string, std::string> result) {
            uiTaskExecutor.PostTask([weak, result] {
                auto camera = weak.Upgrade();
                if (camera) {
                    camera->OnTakePhotoCallBack(result);
                }
            });
    };
    camera_->AddTakePhotoListener(takePhotoListener);

    auto onError = [weak = WeakClaim(this), uiTaskExecutor](const std::string& errorcode, const std::string& errormsg) {
        uiTaskExecutor.PostTask([weak, errorcode, errormsg] {
            auto camera = weak.Upgrade();
            if (camera) {
                camera->OnError(errorcode, errormsg);
            }
        });
    };
    camera_->AddErrorListener(onError);

    auto preparedCallBack = [weak = WeakClaim(this), uiTaskExecutor]() {
            uiTaskExecutor.PostTask([weak] {
                auto camera = weak.Upgrade();
                if (camera) {
                    camera->OnPrepared();
                }
            });
    };
    camera_->AddPrepareEventListener(preparedCallBack);

#ifdef OHOS_STANDARD_SYSTEM
    auto recorderCallBack = [weak = WeakClaim(this), uiTaskExecutor](
        const std::map<std::string, std::string> result) {
            uiTaskExecutor.PostTask([weak, result] {
                auto camera = weak.Upgrade();
                if (camera) {
                    camera->OnRecorderCallBack(result);
                }
            });
    };
    camera_->AddRecordListener(recorderCallBack);
#else
    auto onSizeChangeListener = [weak = WeakClaim(this), uiTaskExecutor](
        int32_t preViewWidth, int32_t preViewHeight) {
            uiTaskExecutor.PostTask([weak, preViewWidth, preViewHeight] {
                auto camera = weak.Upgrade();
                if (camera) {
                    camera->OnPreViewSizeChange(preViewWidth, preViewHeight);
                }
            });
    };
    camera_->AddPreViewSizeChang(onSizeChangeListener);

    if (texture_) {
        auto onTextureRefresh = [weak = WeakClaim(this), uiTaskExecutor]() {
            uiTaskExecutor.PostSyncTask([weak] {
                auto camera = weak.Upgrade();
                if (camera) {
                    camera->OnTextureRefresh();
                }
            });
        };
        texture_->SetRefreshListener(onTextureRefresh);
    }
#endif
}

void CameraElement::OnTakePhotoCallBack(const std::map<std::string, std::string>& result)
{
    if (GetStringFromMap(result, IS_SUCESS) == "1" && !callbackIds_.success.empty()) {
        std::string param = std::string("{\"")
                                .append(PHOTO_PATH)
                                .append("\":\"file://")
                                .append(GetStringFromMap(result, PHOTO_PATH))
                                .append("\"}");
        ExecuteJsCallback(callbackIds_.success, param);
    } else if (GetStringFromMap(result, IS_SUCESS) != "1" && !callbackIds_.fail.empty()) {
        std::string param = std::string("{\"")
                                .append(ERROR_MSG)
                                .append("\":\"")
                                .append(GetStringFromMap(result, ERROR_MSG))
                                .append("\", \"")
                                .append(ERROR_CODE)
                                .append("\":\"")
                                .append(GetStringFromMap(result, ERROR_CODE))
                                .append("\"}");
        ExecuteJsCallback(callbackIds_.fail, param);
    }
    if (!callbackIds_.complete.empty()) {
        std::string param = std::string("{\"").append(CALLBACK_COMPLETE).append("\":\"\"}");
        ExecuteJsCallback(callbackIds_.complete, param);
    }
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
const RefPtr<Component> CameraElement::CreateErrorText(const std::string& errorMsg)
{
    auto text = AceType::MakeRefPtr<TextComponent>(errorMsg);
    text->SetTextStyle(theme_->GetErrorTextStyle());
    text->SetTextDirection(textDirection_);

    std::list<RefPtr<Component>> childrenAlign;
    childrenAlign.emplace_back(text);

    return AceType::MakeRefPtr<AlignComponent>(childrenAlign, Alignment::TOP_CENTER);
}

void CameraElement::UpdataChild(const RefPtr<Component>& childComponent)
{
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, childComponent);
}
#endif

void CameraElement::OnError(const std::string& errorcode, const std::string& errormsg)
{
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string errorDescription = "This component is not supported on PC Preview.";
    UpdataChild(CreateErrorText(errorDescription));
#endif
    if (onError_) {
        std::string param = std::string("\"error\",{\"")
                                .append(ERROR_MSG)
                                .append("\":\"")
                                .append(errormsg)
                                .append("\", \"")
                                .append(ERROR_CODE)
                                .append("\":\"")
                                .append(errorcode)
                                .append("\"}");
        onError_(param);
    }
    cameraState_ = CameraState::STATE_UMDEFINED;
}

void CameraElement::TakePhoto(const TakePhotoParams& params)
{
    if (cameraState_ != CameraState::STATE_ENABLE) {
        LOGE("CameraState is not enable.");
        return;
    }

    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    if (!camera_) {
        LOGE("Camera is null.");
        return;
    }

    callbackIds_.clear();
    callbackIds_ = params;
#ifdef OHOS_STANDARD_SYSTEM
    context->GetTaskExecutor()->PostTask(
        [weak = WeakClaim(this)] {
            auto cameraElement = weak.Upgrade();
            if (cameraElement) {
                cameraElement->TakePhoto(Size());
            }
        },
        TaskExecutor::TaskType::PLATFORM);
#else
        camera_->TakePhoto(GetSizeFromQuality(params.quality));
#endif
}

void CameraElement::ExecuteJsCallback(const std::string& callbackId, const std::string& result)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->SendCallbackMessageToFrontend(callbackId, result);
    LOGI("CameraElement: ExecuteJsCallback %{public}s %{public}s", callbackId.c_str(), result.c_str());
}

} // namespace OHOS::Ace