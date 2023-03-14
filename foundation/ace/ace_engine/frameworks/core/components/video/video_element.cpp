/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/video/video_element.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/log/dump_log.h"
#include "base/log/log.h"
#include "base/resource/internal_resource.h"
#include "base/utils/utils.h"
#include "core/common/container_scope.h"
#include "core/components/align/align_component.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/components/image/image_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/slider/slider_component.h"
#include "core/components/stage/stage_element.h"
#include "core/components/text/text_component.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/video/render_texture.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/pipeline_context.h"

#ifdef OHOS_STANDARD_SYSTEM
#include <securec.h>

#include "display_type.h"
#include "surface.h"
#include "window_manager.h"

#ifdef ENABLE_ROSEN_BACKEND
#include "core/components/video/rosen_render_texture.h"
#endif

#endif

namespace OHOS::Ace {
namespace {

const char* PLAY_LABEL = "play";
const char* PAUSE_LABEL = "pause";
const char* FULLSCREEN_LABEL = "fullscreen";
const char* EXIT_FULLSCREEN_LABEL = "exitFullscreen";

#ifdef OHOS_STANDARD_SYSTEM
const char* SURFACE_STRIDE_ALIGNMENT = "8";
constexpr int32_t SURFACE_QUEUE_SIZE = 5;
constexpr int32_t WINDOW_HEIGHT_DEFAULT = 1;
constexpr int32_t WINDOW_WIDTH_DEFAULT = 1;
constexpr int32_t FILE_PREFIX_LENGTH = 7;
#endif
constexpr float ILLEGAL_SPEED = 0.0f;
constexpr int32_t COMPATIBLE_VERSION = 5;

#ifdef OHOS_STANDARD_SYSTEM
constexpr float SPEED_0_75_X = 0.75;
constexpr float SPEED_1_00_X = 1.00;
constexpr float SPEED_1_25_X = 1.25;
constexpr float SPEED_1_75_X = 1.75;
constexpr float SPEED_2_00_X = 2.00;

OHOS::Media::PlayerSeekMode ConvertToMediaSeekMode(SeekMode seekMode)
{
    OHOS::Media::PlayerSeekMode mode = OHOS::Media::SEEK_PREVIOUS_SYNC;
    if (seekMode == SeekMode::SEEK_NEXT_SYNC) {
        mode = OHOS::Media::SEEK_NEXT_SYNC;
    } else if (seekMode == SeekMode::SEEK_CLOSEST_SYNC) {
        mode = OHOS::Media::SEEK_CLOSEST_SYNC;
    } else if (seekMode == SeekMode::SEEK_CLOSEST) {
        mode = OHOS::Media::SEEK_CLOSEST;
    }
    return mode;
}

OHOS::Media::PlaybackRateMode ConvertToMediaPlaybackSpeed(float speed)
{
    OHOS::Media::PlaybackRateMode mode = OHOS::Media::SPEED_FORWARD_1_00_X;
    if (NearEqual(speed, SPEED_0_75_X)) {
        mode = OHOS::Media::PlaybackRateMode::SPEED_FORWARD_0_75_X;
    } else if (NearEqual(speed, SPEED_1_00_X)) {
        mode = OHOS::Media::PlaybackRateMode::SPEED_FORWARD_1_00_X;
    } else if (NearEqual(speed, SPEED_1_25_X)) {
        mode = OHOS::Media::PlaybackRateMode::SPEED_FORWARD_1_25_X;
    } else if (NearEqual(speed, SPEED_1_75_X)) {
        mode = OHOS::Media::PlaybackRateMode::SPEED_FORWARD_1_75_X;
    } else if (NearEqual(speed, SPEED_2_00_X)) {
        mode = OHOS::Media::PlaybackRateMode::SPEED_FORWARD_2_00_X;
    } else {
        LOGW("speed is not supported yet.");
    }
    return mode;
}
#endif

} // namespace

VideoElement::~VideoElement()
{
    if (!startBtnClickId_.IsEmpty()) {
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(startBtnClickId_);
    }

    if (!sliderMovedCallbackId_.IsEmpty()) {
        BackEndEventManager<void(const std::string&)>::GetInstance().RemoveBackEndEvent(sliderMovedCallbackId_);
    }

    if (!sliderMovingCallbackId_.IsEmpty()) {
        BackEndEventManager<void(const std::string&)>::GetInstance().RemoveBackEndEvent(sliderMovingCallbackId_);
    }

    if (!fullscreenBtnClickId_.IsEmpty()) {
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(fullscreenBtnClickId_);
    }

    if (!shieldId_.IsEmpty()) {
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(startBtnClickId_);
    }

    if (!isExternalResource_) {
        if (isFullScreen_) {
            ExitFullScreen();
        }
        UnSubscribeMultiModal();
    } else {
        if (player_) {
            player_->PopListener();
        }
    }
    ReleasePlatformResource();
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_ != nullptr) {
        mediaPlayer_->Release();
    }
#endif
}

void VideoElement::PerformBuild()
{
    RefPtr<VideoComponent> videoComponent = AceType::DynamicCast<VideoComponent>(component_);

    if (videoComponent == nullptr) {
        return;
    }
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, videoComponent->GetChild());
}

void VideoElement::InitStatus(const RefPtr<VideoComponent>& videoComponent)
{
    imageFit_ = videoComponent->GetFit();
    imagePosition_ = videoComponent->GetImagePosition();
    needControls_ = videoComponent->NeedControls();
    isAutoPlay_ = videoComponent->IsAutoPlay();
    isMute_ = videoComponent->IsMute();
    src_ = videoComponent->GetSrc();
    poster_ = videoComponent->GetPoster();
    posterImage_ = videoComponent->GetPosterImage();
    isFullScreen_ = videoComponent->IsFullscreen();
    direction_ = videoComponent->GetDirection();
    startTime_ = videoComponent->GetStartTime();
    if (isLoop_ != videoComponent->IsLoop()) {
        isLoop_ = videoComponent->IsLoop();
        EnableLooping(isLoop_);
    }

    if (speed_ != videoComponent->GetSpeed()) {
        speed_ = videoComponent->GetSpeed();
        SetSpeed(speed_);
    }

#ifdef OHOS_STANDARD_SYSTEM
    PreparePlayer();
#endif

    if (!videoComponent->GetPlayer().Invalid() && !videoComponent->GetTexture().Invalid()) {
        player_ = videoComponent->GetPlayer().Upgrade();
        texture_ = videoComponent->GetTexture().Upgrade();

        if (player_ && texture_) {
            isExternalResource_ = true;
            videoComponent->SetPlayer(nullptr);
            videoComponent->SetTexture(nullptr);
            InitListener();
        }
    }
}

#ifdef OHOS_STANDARD_SYSTEM
::OHOS::sptr<::OHOS::Subwindow> VideoElement::CreateSubwindow()
{
    const auto& wmi = ::OHOS::WindowManager::GetInstance();
    if (wmi == nullptr) {
        LOGE("Window manager get instance failed");
        return nullptr;
    }

    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGE("context is nullptr");
        return nullptr;
    }

    auto option = SubwindowOption::Get();
    option->SetWidth(WINDOW_WIDTH_DEFAULT);
    option->SetHeight(WINDOW_HEIGHT_DEFAULT);
    option->SetX(0);
    option->SetY(0);
    option->SetWindowType(SUBWINDOW_TYPE_NORMAL);
    auto window = wmi->GetWindowByID(context->GetWindowId());
    if (window == nullptr) {
        LOGE("window is nullptr");
        return nullptr;
    }

    ::OHOS::sptr<::OHOS::Subwindow> ret = nullptr;
    auto wret = wmi->CreateSubwindow(ret, window, option);
    if (wret != WM_OK) {
        LOGE("create subwindow failed, because %{public}s", WMErrorStr(wret).c_str());
        return nullptr;
    }
    return ret;
}

void VideoElement::RegistMediaPlayerEvent()
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGE("context is nullptr");
        return;
    }

    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    auto videoElement = WeakClaim(this);

    auto&& positionUpdatedEvent = [videoElement, uiTaskExecutor](uint32_t currentPos) {
        uiTaskExecutor.PostSyncTask([&videoElement, currentPos] {
            auto video = videoElement.Upgrade();
            if (video != nullptr) {
                video->OnCurrentTimeChange(currentPos);
            }
        });
    };

    auto&& stateChangedEvent = [videoElement, uiTaskExecutor](PlaybackStatus status) {
        uiTaskExecutor.PostSyncTask([&videoElement, status] {
            auto video = videoElement.Upgrade();
            if (video) {
                LOGD("OnPlayerStatus");
                video->OnPlayerStatus(status);
            }
        });
    };

    mediaPlayerCallback_ = std::make_shared<MediaPlayerCallback>(ContainerScope::CurrentId());
    mediaPlayerCallback_->SetPositionUpdatedEvent(positionUpdatedEvent);
    mediaPlayerCallback_->SetStateChangedEvent(stateChangedEvent);
    mediaPlayer_->SetPlayerCallback(mediaPlayerCallback_);
}

void VideoElement::CreateMediaPlayer()
{
    if (mediaPlayer_ != nullptr) {
        return;
    }
#ifndef ENABLE_ROSEN_BACKEND
    subWindow_ = CreateSubwindow();
    if (subWindow_ == nullptr) {
        LOGE("Create subwindow failed");
        return;
    }
#endif

    mediaPlayer_ = OHOS::Media::PlayerFactory::CreatePlayer();
    if (mediaPlayer_ == nullptr) {
        LOGE("Create player failed");
        return;
    }

    PreparePlayer();
}

void VideoElement::PreparePlayer()
{
    if (!hasSrcChanged_) {
        return;
    }
    if (mediaPlayer_ == nullptr) {
        LOGE("mediaPlayer_ is nullptr");
        return;
    }

    if (mediaPlayer_->Reset() != 0) {
        LOGE("Player Reset failed");
        return;
    }
    std::string filePath = src_;
    LOGI("filePath : %{private}s", filePath.c_str());

    // Remove file:// prefix for get fd.
    if (StringUtils::StartWith(filePath, "file://")) {
        filePath = filePath.substr(FILE_PREFIX_LENGTH);
    }

    int32_t fd = -1;
    // SetSource by fd.
    if (StringUtils::StartWith(filePath, "dataability://")) {
        auto context = context_.Upgrade();
        if (!context) {
            LOGE("get context fail");
            return;
        }
        auto dataProvider = AceType::DynamicCast<DataProviderManagerStandard>(context->GetDataProviderManager());
        if (!dataProvider) {
            LOGE("get data provider fail");
            return;
        }
        auto dataAbilityHelper = dataProvider->GetDataAbilityHelper();
        if (dataAbilityHelper) {
            fd = dataAbilityHelper->OpenFile(filePath, "r");
        }
    } else if (!StringUtils::StartWith(filePath, "http")) {
        filePath = GetAssetAbsolutePath(filePath);
        fd = open(filePath.c_str(), O_RDONLY);
    }

    if (fd >= 0) {
        // get size of file.
        struct stat statBuf;
        auto statRes = fstat(fd, &statBuf);
        if (statRes != 0) {
            LOGE("get stat fail");
            close(fd);
            return;
        }
        auto size = statBuf.st_size;
        if (mediaPlayer_->SetSource(fd, 0, size) != 0) {
            LOGE("Player SetSource failed");
            close(fd);
            return;
        }
        close(fd);
    } else {
        if (mediaPlayer_->SetSource(filePath) != 0) {
            LOGE("Player SetSource failed");
            return;
        }
    }
    
    RegistMediaPlayerEvent();

    sptr<Surface> producerSurface;
#ifdef ENABLE_ROSEN_BACKEND
    if (renderNode_) {
        auto rosenTexture = AceType::DynamicCast<RosenRenderTexture>(renderNode_);
        if (rosenTexture) {
            producerSurface = rosenTexture->GetSurface();
        }
    }
#else
    producerSurface = subWindow_->GetSurface();
#endif

    if (producerSurface == nullptr) {
        LOGE("producerSurface is nullptr");
        return;
    }
    producerSurface->SetQueueSize(SURFACE_QUEUE_SIZE);
    producerSurface->SetUserData("SURFACE_STRIDE_ALIGNMENT", SURFACE_STRIDE_ALIGNMENT);
    producerSurface->SetUserData("SURFACE_FORMAT", std::to_string(PIXEL_FMT_RGBA_8888));
    if (mediaPlayer_->SetVideoSurface(producerSurface) != 0) {
        LOGE("Player SetVideoSurface failed");
        return;
    }
    if (mediaPlayer_->PrepareAsync() != 0) {
        LOGE("Player prepare failed");
        return;
    }
    hasSrcChanged_ = false;
}

std::string VideoElement::GetAssetAbsolutePath(const std::string& fileName)
{
    const auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        LOGW("the pipeline context is null");
        return fileName;
    }
    auto assetManager = pipelineContext->GetAssetManager();
    if (!assetManager) {
        LOGW("the assetManager is null");
        return fileName;
    }
    std::string filePath = assetManager->GetAssetPath(fileName);
    std::string absolutePath = filePath + fileName;
    return absolutePath;
}
#endif

void VideoElement::ResetStatus()
{
    needControls_ = true;
    isAutoPlay_ = false;
    isMute_ = false;
    duration_ = 0;
    currentPos_ = 0;
    isPlaying_ = false;
    isReady_ = false;
    isInitialState_ = true;
    isError_ = false;
    videoWidth_ = 0.0;
    videoHeight_ = 0.0;
    isLoop_ = false;
    startTime_ = 0;
    durationText_ = Localization::GetInstance()->FormatDuration(0);
    currentPosText_ = Localization::GetInstance()->FormatDuration(0);
}

void VideoElement::Prepare(const WeakPtr<Element>& parent)
{
    auto themeManager = GetThemeManager();
    if (!themeManager) {
        return;
    }
    auto videoComponent = AceType::DynamicCast<VideoComponent>(component_);
    theme_ = themeManager->GetTheme<VideoTheme>();
    sliderTheme_ = themeManager->GetTheme<SliderTheme>();
    if (videoComponent) {
        textDirection_ = videoComponent->GetTextDirection();

        ResetStatus();
        InitStatus(videoComponent);
        InitEvent(videoComponent);
        SetRespondChildEvent();
        if (!isExternalResource_) {
            SetMethodCall(videoComponent);
            CreatePlatformResource();
            PrepareMultiModalEvent();
            SubscribeMultiModal();
        }
        videoComponent->SetChild(CreateChild());
        fullscreenEvent_ = videoComponent->GetFullscreenEvent();
    }

    RenderElement::Prepare(parent);
    if (renderNode_) {
#ifdef OHOS_STANDARD_SYSTEM
        CreateMediaPlayer();
#endif
        auto renderTexture = AceType::DynamicCast<RenderTexture>(renderNode_);
        if (renderTexture) {
            renderTexture->SetHiddenChangeEvent([weak = WeakClaim(this)](bool hidden) {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->HiddenChange(hidden);
                }
            });
            renderTexture->SetTextureSizeChange(
                [weak = WeakClaim(this)](int64_t textureId, int32_t textureWidth, int32_t textureHeight) {
                    auto videoElement = weak.Upgrade();
                    if (videoElement) {
                        videoElement->OnTextureSize(textureId, textureWidth, textureHeight);
                    }
                });
        }
    }
    isElementPrepared_ = true;
}

void VideoElement::OnTextureSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight)
{
#ifdef OHOS_STANDARD_SYSTEM

#ifndef ENABLE_ROSEN_BACKEND
    if (subWindow_ != nullptr) {
        auto context = context_.Upgrade();
        if (!context) {
            LOGE("context is nullptr");
            return;
        }
        int32_t height = textureHeight;
        if (needControls_) {
            height -= theme_->GetBtnSize().Height();
            height -= theme_->GetBtnEdge().Top().Value();
            height -= theme_->GetBtnEdge().Bottom().Value();
        }
        if (height <= 0) {
            height = textureHeight;
        }
        float viewScale = context->GetViewScale();
        subWindow_->Resize(textureWidth * viewScale, height * viewScale);
        LOGI("SetSubWindowSize width: %{public}f, height: %{public}f", textureWidth * viewScale, height * viewScale);

        if (renderNode_ != nullptr) {
            Offset offset = renderNode_->GetGlobalOffset();
            subWindow_->Move(offset.GetX() * viewScale, offset.GetY() * viewScale);
            LOGI("SubWindow move X: %{public}f, Y: %{public}f", offset.GetX() * viewScale, offset.GetY() * viewScale);
        }
    }
#endif

#else
    if (texture_) {
        texture_->OnSize(textureId, textureWidth, textureHeight);
    }
#endif
}

bool VideoElement::HasPlayer() const
{
#ifdef OHOS_STANDARD_SYSTEM
    return mediaPlayer_ != nullptr;
#else
    return player_ != nullptr;
#endif
}

void VideoElement::HiddenChange(bool hidden)
{
    if (isPlaying_ && hidden && HasPlayer()) {
        pastPlayingStatus_ = isPlaying_;
        Pause();
        return;
    }

    if (!hidden && pastPlayingStatus_) {
        isPlaying_ = !pastPlayingStatus_;
        pastPlayingStatus_ = false;
        Start();
    }
}

void VideoElement::PrepareMultiModalEvent()
{
    if (!multimodalEventFullscreen_) {
        multimodalEventFullscreen_ = [weak = WeakClaim(this)](const AceMultimodalEvent&) {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->FullScreen();
            }
        };
    }

    if (!multimodalEventPause_) {
        multimodalEventPause_ = [weak = WeakClaim(this)](const AceMultimodalEvent&) {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->Pause();
            }
        };
    }

    if (!multimodalEventPlay_) {
        multimodalEventPlay_ = [weak = WeakClaim(this)](const AceMultimodalEvent&) {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->Start();
            }
        };
    }

    if (!multimodalEventFullscreenExit_) {
        multimodalEventFullscreenExit_ = [weak = WeakClaim(this)](const AceMultimodalEvent&) {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->ExitFullScreen();
            }
        };
    };
}

bool VideoElement::SubscribeMultiModal()
{
    if (isSubscribeMultimodal_) {
        return true;
    }
    if (multiModalScene_.Invalid()) {
        const auto pipelineContext = GetContext().Upgrade();
        if (!pipelineContext) {
            LOGW("the pipeline context is null");
            return false;
        }
        const auto multimodalManager = pipelineContext->GetMultiModalManager();
        if (!multimodalManager) {
            LOGW("the multimodal manager is null");
            return false;
        }
        const auto scene = multimodalManager->GetCurrentMultiModalScene();
        if (!scene) {
            return false;
        }

        playVoiceEvent_ = VoiceEvent(PLAY_LABEL, SceneLabel::VIDEO);
        scene->SubscribeVoiceEvent(playVoiceEvent_, multimodalEventPlay_);

        pauseVoiceEvent_ = VoiceEvent(PAUSE_LABEL, SceneLabel::VIDEO);
        scene->SubscribeVoiceEvent(pauseVoiceEvent_, multimodalEventPause_);

        fullscreenVoiceEvent_ = VoiceEvent(FULLSCREEN_LABEL, SceneLabel::VIDEO);
        scene->SubscribeVoiceEvent(fullscreenVoiceEvent_, multimodalEventFullscreen_);

        exitFullscreenVoiceEvent_ = VoiceEvent(EXIT_FULLSCREEN_LABEL, SceneLabel::VIDEO);
        scene->SubscribeVoiceEvent(exitFullscreenVoiceEvent_, multimodalEventFullscreenExit_);
        multiModalScene_ = scene;
        isSubscribeMultimodal_ = true;
    }
    return true;
}

bool VideoElement::UnSubscribeMultiModal()
{
    if (!isSubscribeMultimodal_) {
        return true;
    }
    auto multiModalScene = multiModalScene_.Upgrade();
    if (!multiModalScene) {
        LOGE("fail to destroy multimodal event due to multiModalScene is null");
        return false;
    }
    if (!playVoiceEvent_.GetVoiceContent().empty()) {
        multiModalScene->UnSubscribeVoiceEvent(playVoiceEvent_);
    }
    if (!pauseVoiceEvent_.GetVoiceContent().empty()) {
        multiModalScene->UnSubscribeVoiceEvent(pauseVoiceEvent_);
    }
    if (!exitFullscreenVoiceEvent_.GetVoiceContent().empty()) {
        multiModalScene->UnSubscribeVoiceEvent(exitFullscreenVoiceEvent_);
    }
    if (!fullscreenVoiceEvent_.GetVoiceContent().empty()) {
        multiModalScene->UnSubscribeVoiceEvent(fullscreenVoiceEvent_);
    }
    isSubscribeMultimodal_ = false;
    return true;
}

void VideoElement::SetNewComponent(const RefPtr<Component>& newComponent)
{
    if (newComponent == nullptr || !isElementPrepared_) {
        Element::SetNewComponent(newComponent);
        return;
    }
    auto videoComponent = AceType::DynamicCast<VideoComponent>(newComponent);
    if (videoComponent) {
        if (src_ == videoComponent->GetSrc()) {
            if (isError_) {
                return;
            }
            hasSrcChanged_ = false;
            InitStatus(videoComponent);

            // When the video is in the initial state and the attribute is auto play, start playing.
            if (isInitialState_ && isAutoPlay_) {
                Start();
            }
            if (isMute_) {
                SetVolume(0.0f);
            } else {
                SetVolume(1.0f);
            }
        } else {
            hasSrcChanged_ = true;
            ResetStatus();
            InitStatus(videoComponent);
            CreatePlatformResource();
        }
        if (texture_) {
#ifndef OHOS_STANDARD_SYSTEM
            videoComponent->SetTextureId(texture_->GetId());
#endif
            videoComponent->SetSrcWidth(videoWidth_);
            videoComponent->SetSrcHeight(videoHeight_);
            videoComponent->SetFit(imageFit_);
            videoComponent->SetImagePosition(imagePosition_);
        }
        videoComponent->SetChild(CreateChild());

        Element::SetNewComponent(videoComponent);
    }
}

void VideoElement::InitEvent(const RefPtr<VideoComponent>& videoComponent)
{
    if (!videoComponent->GetPreparedEventId().IsEmpty()) {
        onPrepared_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetPreparedEventId(), context_);
    }

    if (!videoComponent->GetFinishEventId().IsEmpty()) {
        onFinish_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetFinishEventId(), context_);
    }

    if (!videoComponent->GetErrorEventId().IsEmpty()) {
        onError_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetErrorEventId(), context_);
    }

    if (!videoComponent->GetTimeUpdateEventId().IsEmpty()) {
        onTimeUpdate_ =
            AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetTimeUpdateEventId(), context_);
    }

    if (!videoComponent->GetStartEventId().IsEmpty()) {
        onStart_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetStartEventId(), context_);
    }

    if (!videoComponent->GetPauseEventId().IsEmpty()) {
        onPause_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetPauseEventId(), context_);
    }

    if (!videoComponent->GetStopEventId().IsEmpty()) {
        onStop_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetStopEventId(), context_);
    }

    if (!videoComponent->GetSeekingEventId().IsEmpty()) {
        onSeeking_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetSeekingEventId(), context_);
    }

    if (!videoComponent->GetSeekedEventId().IsEmpty()) {
        onSeeked_ = AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetSeekedEventId(), context_);
    }

    if (!videoComponent->GetFullscreenChangeEventId().IsEmpty()) {
        onFullScreenChange_ =
            AceAsyncEvent<void(const std::string&)>::Create(videoComponent->GetFullscreenChangeEventId(), context_);
    }
}

void VideoElement::SetMethodCall(const RefPtr<VideoComponent>& videoComponent)
{
    auto videoController = videoComponent->GetVideoController();
    if (videoController) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        videoController->SetStartImpl([weak = WeakClaim(this), uiTaskExecutor]() {
            uiTaskExecutor.PostTask([weak]() {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->Start();
                }
            });
        });
        videoController->SetPausetImpl([weak = WeakClaim(this), uiTaskExecutor]() {
            uiTaskExecutor.PostTask([weak]() {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->Pause();
                }
            });
        });
        videoController->SetStopImpl([weak = WeakClaim(this), uiTaskExecutor]() {
            uiTaskExecutor.PostTask([weak]() {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->Stop();
                }
            });
        });
        videoController->SetSeekToImpl([weak = WeakClaim(this), uiTaskExecutor](float pos, SeekMode seekMode) {
            uiTaskExecutor.PostTask([weak, pos, seekMode]() {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->SetCurrentTime(pos, seekMode);
                }
            });
        });
        videoController->SetRequestFullscreenImpl([weak = WeakClaim(this), uiTaskExecutor](bool isPortrait) {
            uiTaskExecutor.PostTask([weak, isPortrait]() {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->OnPreFullScreen(isPortrait);
                    videoElement->FullScreen();
                }
            });
        });
        videoController->SetExitFullscreenImpl([weak = WeakClaim(this), uiTaskExecutor](bool isSync) {
            if (isSync) {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->ExitFullScreen();
                }
                return;
            }
            uiTaskExecutor.PostTask([weak]() {
                auto videoElement = weak.Upgrade();
                if (videoElement) {
                    videoElement->ExitFullScreen();
                }
            });
        });
    }
}

void VideoElement::SetRespondChildEvent()
{
    shieldId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    startBtnClickId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void()>::GetInstance().BindBackendEvent(startBtnClickId_, [weak = WeakClaim(this)]() {
        auto videoElement = weak.Upgrade();
        if (videoElement) {
            videoElement->OnStartBtnClick();
        }
    });
    sliderMovedCallbackId_ = BackEndEventManager<void(const std::string&)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(const std::string&)>::GetInstance().BindBackendEvent(
        sliderMovedCallbackId_, [weak = WeakClaim(this)](const std::string& param) {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->OnSliderChange(param);
            }
        });
    sliderMovingCallbackId_ = BackEndEventManager<void(const std::string&)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(const std::string&)>::GetInstance().BindBackendEvent(
        sliderMovingCallbackId_, [weak = WeakClaim(this)](const std::string& param) {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->OnSliderMoving(param);
            }
        });
    fullscreenBtnClickId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void()>::GetInstance().BindBackendEvent(fullscreenBtnClickId_, [weak = WeakClaim(this)]() {
        auto videoElement = weak.Upgrade();
        if (videoElement) {
            videoElement->OnFullScreenBtnClick();
        }
    });
}

void VideoElement::CreatePlatformResource()
{
    ReleasePlatformResource();

    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);

    auto errorCallback = [weak = WeakClaim(this), uiTaskExecutor](
                             const std::string& errorId, const std::string& param) {
        uiTaskExecutor.PostTask([weak, errorId, param] {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->OnError(errorId, param);
            }
        });
    };
    texture_ = AceType::MakeRefPtr<Texture>(context_, errorCallback);

    texture_->Create([weak = WeakClaim(this), errorCallback](int64_t id) mutable {
        auto videoElement = weak.Upgrade();
        if (videoElement) {
            videoElement->CreatePlayer(id, std::move(errorCallback));
        }
    });
}

void VideoElement::CreatePlayer(int64_t id, ErrorCallback&& errorCallback)
{
    player_ = AceType::MakeRefPtr<Player>(id, src_, context_, std::move(errorCallback));
    player_->SetMute(isMute_);
    player_->SetAutoPlay(isAutoPlay_);
    InitListener();
    player_->Create(nullptr);
}

void VideoElement::InitListener()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    auto videoElement = WeakClaim(this);
    if (!isExternalResource_) {
        auto onTextureRefresh = [videoElement, uiTaskExecutor]() {
            uiTaskExecutor.PostSyncTask([&videoElement] {
                auto video = videoElement.Upgrade();
                if (video) {
                    video->OnTextureRefresh();
                }
            });
        };
        texture_->SetRefreshListener(onTextureRefresh);
    }

    auto onPrepared = [videoElement, uiTaskExecutor](uint32_t width, uint32_t height, bool isPlaying, uint32_t duration,
                          uint32_t currentPos, bool needFireEvent) {
        uiTaskExecutor.PostSyncTask([&videoElement, width, height, isPlaying, duration, currentPos, needFireEvent] {
            auto video = videoElement.Upgrade();
            if (video) {
                video->OnPrepared(width, height, isPlaying, duration, currentPos, needFireEvent);
            }
        });
    };

    auto onPlayerStatus = [videoElement, uiTaskExecutor](bool isPlaying) {
        uiTaskExecutor.PostSyncTask([&videoElement, isPlaying] {
            auto video = videoElement.Upgrade();
            if (video) {
                video->OnPlayerStatus(isPlaying ? PlaybackStatus::STARTED : PlaybackStatus::NONE);
            }
        });
    };

    auto onCurrentTimeChange = [videoElement, uiTaskExecutor](uint32_t currentPos) {
        uiTaskExecutor.PostSyncTask([&videoElement, currentPos] {
            auto video = videoElement.Upgrade();
            if (video) {
                video->OnCurrentTimeChange(currentPos);
            }
        });
    };

    auto onCompletion = [videoElement, uiTaskExecutor] {
        uiTaskExecutor.PostSyncTask([&videoElement] {
            auto video = videoElement.Upgrade();
            if (video) {
                video->OnCompletion();
            }
        });
    };

    player_->AddPreparedListener(onPrepared);
    player_->AddPlayStatusListener(onPlayerStatus);
    player_->AddCurrentPosListener(onCurrentTimeChange);
    player_->AddCompletionListener(onCompletion);

    player_->AddRefreshRenderListener([videoElement]() {
        auto video = videoElement.Upgrade();
        if (video) {
            video->OnTextureRefresh();
        }
    });
}

void VideoElement::ReleasePlatformResource()
{
#ifndef OHOS_STANDARD_SYSTEM
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    // Reusing texture will cause a problem that last frame of last video will be display.
    if (texture_) {
        auto platformTaskExecutor =
            SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

        // Release player first.
        if (player_) {
            if (!isExternalResource_) {
                player_->Stop();
                player_->Release();
            }

            if (platformTaskExecutor.IsRunOnCurrentThread()) {
                player_.Reset();
            } else {
                // Make sure it's destroyed when it's release task done.
                platformTaskExecutor.PostTask([player = player_]() {});
            }
        }

        if (platformTaskExecutor.IsRunOnCurrentThread()) {
            if (!isExternalResource_) {
                texture_->Release();
            }
            texture_.Reset();
        } else {
            if (!isExternalResource_) {
#if defined(ENABLE_NATIVE_VIEW)
                texture_->Release();
            }
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
            } else {
                // Make sure it's destroyed when it's release task done.
                platformTaskExecutor.PostTask([texture = texture_]() {});
            }
#endif
        }
    }
#endif
}

void VideoElement::UpdataChild(const RefPtr<Component>& childComponent)
{
    const auto& child = children_.empty() ? nullptr : children_.front();
    UpdateChild(child, childComponent);
}

void VideoElement::OnError(const std::string& errorId, const std::string& param)
{
    isError_ = true;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string errorcode = "This component is not supported on PC Preview.";
#else
    std::string errorcode = Localization::GetInstance()->GetErrorDescription(errorId);
#endif
    UpdataChild(CreateErrorText(errorcode));

    if (onError_) {
        std::string param;
        if (IsDeclarativePara()) {
            auto json = JsonUtil::Create(true);
            json->Put("error", "");
            param = json->ToString();
        } else {
            param = std::string("\"error\",{").append("}");
        }
        onError_(param);
    }
}

void VideoElement::OnPrepared(
    uint32_t width, uint32_t height, bool isPlaying, uint32_t duration, uint32_t currentPos, bool needFireEvent)
{
    isPlaying_ = isPlaying;
    isReady_ = true;
    videoWidth_ = width;
    videoHeight_ = height;
    duration_ = duration;
    currentPos_ = currentPos;

    IntTimeToText(duration_, durationText_);
    IntTimeToText(currentPos_, currentPosText_);

    auto video = AceType::MakeRefPtr<VideoComponent>();
#ifndef OHOS_STANDARD_SYSTEM
    video->SetTextureId(texture_->GetId());
#endif
    video->SetSrcWidth(videoWidth_);
    video->SetSrcHeight(videoHeight_);
    video->SetFit(imageFit_);
    video->SetImagePosition(imagePosition_);

    if (isPlaying || currentPos != 0) {
        isInitialState_ = false;
    }

    if (renderNode_ != nullptr) {
        renderNode_->Update(video);
    }
    UpdataChild(CreateChild());

    if (needFireEvent && onPrepared_) {
        std::string param;
        if (IsDeclarativePara()) {
            auto json = JsonUtil::Create(true);
            json->Put("duration", static_cast<double>(duration_));
            param = json->ToString();
        } else {
            param = std::string("\"prepared\",{\"duration\":").append(std::to_string(duration_)).append("}");
        }
        LOGI("video onPrepared event: %s ", param.c_str());
        onPrepared_(param);
    }

    if (!isExternalResource_) {
        SetCurrentTime(startTime_);
        EnableLooping(isLoop_);
        SetSpeed(speed_);
    }

    if (isStop_) {
        isStop_ = false;
        Start();
    }

#ifdef OHOS_STANDARD_SYSTEM
    if (isAutoPlay_) {
        Start();
    }
#endif
}

void VideoElement::OnPlayerStatus(PlaybackStatus status)
{
    bool isPlaying = (status == PlaybackStatus::STARTED);
    if (isInitialState_) {
        isInitialState_ = !isPlaying;
    }

    isPlaying_ = isPlaying;
    if (!isFullScreen_ || isExternalResource_) {
        UpdataChild(CreateChild());
    }

    if (isPlaying) {
        if (onStart_) {
            std::string param;
            if (IsDeclarativePara()) {
                auto json = JsonUtil::Create(true);
                json->Put("start", "");
                param = json->ToString();
            } else {
                param = std::string("\"start\",{").append("}");
            }
            LOGE("video onStart event: %s ", param.c_str());
            onStart_(param);
        }
    } else {
        if (onPause_) {
            std::string param;
            if (IsDeclarativePara()) {
                auto json = JsonUtil::Create(true);
                json->Put("pause", "");
                param = json->ToString();
            } else {
                param = std::string("\"pause\",{").append("}");
            }
            LOGE("video onPause event: %s ", param.c_str());
            onPause_(param);
        }
    }

#ifdef OHOS_STANDARD_SYSTEM
    if (status == PlaybackStatus::PREPARED) {
        auto context = context_.Upgrade();
        if (context == nullptr) {
            LOGE("context is nullptr");
            return;
        }
        if (!mediaPlayer_) {
            return;
        }
        auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        auto videoElement = WeakClaim(this);
        Size videoSize = Size(mediaPlayer_->GetVideoWidth(), mediaPlayer_->GetVideoHeight());
        int32_t milliSecondDuration = 0;
        mediaPlayer_->GetDuration(milliSecondDuration);
        uiTaskExecutor.PostSyncTask(
            [&videoElement, videoSize, duration = milliSecondDuration / MILLISECONDS_TO_SECONDS] {
                auto video = videoElement.Upgrade();
                if (video) {
                    LOGI("Video OnPrepared video size: %{public}s", videoSize.ToString().c_str());
                    video->OnPrepared(videoSize.Width(), videoSize.Height(), false, duration, 0, true);
                }
            });
    } else if (status == PlaybackStatus::PLAYBACK_COMPLETE) {
        OnCompletion();
    }
#endif
}

void VideoElement::OnCurrentTimeChange(uint32_t currentPos)
{
#ifdef OHOS_STANDARD_SYSTEM
    if (currentPos == currentPos_) {
        return;
    }
    if (duration_ == 0) {
        int32_t duration = 0;
        if (mediaPlayer_->GetDuration(duration) == 0) {
            duration_ = duration / MILLISECONDS_TO_SECONDS;
            IntTimeToText(duration_, durationText_);
        }
    }
#endif

    isInitialState_ = isInitialState_ ? currentPos == 0 : false;
    IntTimeToText(currentPos, currentPosText_);
    currentPos_ = currentPos;

    UpdataChild(CreateChild());

    if (onTimeUpdate_) {
        std::string param;
        if (IsDeclarativePara()) {
            auto json = JsonUtil::Create(true);
            json->Put("time", static_cast<double>(currentPos));
            param = json->ToString();
        } else {
            param = std::string("\"timeupdate\",{\"currenttime\":").append(std::to_string(currentPos)).append("}");
        }
        LOGI("video onTimeUpdate event: %s ", param.c_str());
        onTimeUpdate_(param);
    }
}

void VideoElement::OnCompletion()
{
    LOGI("VideoElement::OnCompletion");
    currentPos_ = duration_;
    IntTimeToText(currentPos_, currentPosText_);

    isPlaying_ = false;
    UpdataChild(CreateChild());

    if (onFinish_) {
        std::string param;
        if (IsDeclarativePara()) {
            auto json = JsonUtil::Create(true);
            json->Put("finish", "");
            param = json->ToString();
        } else {
            param = std::string("\"finish\",{").append("}");
        }
        LOGI("video onFinish event: %s ", param.c_str());
        onFinish_(param);
    }
}

const RefPtr<Component> VideoElement::CreateErrorText(const std::string& errorMsg)
{
    auto text = AceType::MakeRefPtr<TextComponent>(errorMsg);
    text->SetTextStyle(theme_->GetErrorTextStyle());
    text->SetTextDirection(textDirection_);

    std::list<RefPtr<Component>> childrenAlign;
    childrenAlign.emplace_back(text);

    return AceType::MakeRefPtr<AlignComponent>(childrenAlign, Alignment::TOP_CENTER);
}

const RefPtr<Component> VideoElement::CreateCurrentText()
{
    auto textPos = AceType::MakeRefPtr<TextComponent>(currentPosText_);
    textPos->SetTextStyle(theme_->GetTimeTextStyle());
    return textPos;
}

const RefPtr<Component> VideoElement::CreateDurationText()
{
    auto textDuration = AceType::MakeRefPtr<TextComponent>(durationText_);
    textDuration->SetTextStyle(theme_->GetTimeTextStyle());
    return textDuration;
}

const RefPtr<Component> VideoElement::CreateSlider()
{
    auto slider = AceType::MakeRefPtr<SliderComponent>(currentPos_, 1.0, 0.0, duration_);
    slider->InitStyle(sliderTheme_);
    slider->SetOnMoveEndEventId(sliderMovedCallbackId_);
    slider->SetOnMovingEventId(sliderMovingCallbackId_);
    slider->SetTextDirection(textDirection_);
    return slider;
}

const RefPtr<Component> VideoElement::CreatePlayBtn()
{
    auto imageIcon = InternalResource::ResourceId::PLAY_SVG;

    if (pastPlayingStatus_ || isPlaying_) {
        imageIcon = InternalResource::ResourceId::PAUSE_SVG;
    }

    auto image = AceType::MakeRefPtr<ImageComponent>(imageIcon);
    const Size& btnSize = theme_->GetBtnSize();
    image->SetWidth(Dimension(btnSize.Width(), DimensionUnit::VP));
    image->SetHeight(Dimension(btnSize.Height(), DimensionUnit::VP));
    image->SetTextDirection(textDirection_);
    image->SetMatchTextDirection(true);
    std::list<RefPtr<Component>> btnChildren;
    btnChildren.emplace_back(image);

    auto button = AceType::MakeRefPtr<ButtonComponent>(btnChildren);
    button->SetWidth(Dimension(btnSize.Width(), DimensionUnit::VP));
    button->SetHeight(Dimension(btnSize.Height(), DimensionUnit::VP));
    button->SetType(ButtonType::ICON);

    if (IsDeclarativePara()) {
        button->SetClickFunction([weak = WeakClaim(this)]() {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->OnStartBtnClick();
            }
        });
    } else {
        button->SetClickedEventId(startBtnClickId_);
    }
    return button;
}

const RefPtr<Component> VideoElement::CreateFullScreenBtn()
{
    auto imageIcon = InternalResource::ResourceId::FULLSCREEN_SVG;

    if (isFullScreen_) {
        imageIcon = InternalResource::ResourceId::QUIT_FULLSCREEN_SVG;
    }

    auto image = AceType::MakeRefPtr<ImageComponent>(imageIcon);
    const Size& btnSize = theme_->GetBtnSize();
    image->SetWidth(Dimension(btnSize.Width(), DimensionUnit::VP));
    image->SetHeight(Dimension(btnSize.Height(), DimensionUnit::VP));
    image->SetTextDirection(textDirection_);
    image->SetMatchTextDirection(true);

    std::list<RefPtr<Component>> btnChildren;
    btnChildren.emplace_back(image);

    auto button = AceType::MakeRefPtr<ButtonComponent>(btnChildren);
    button->SetWidth(Dimension(btnSize.Width(), DimensionUnit::VP));
    button->SetHeight(Dimension(btnSize.Height(), DimensionUnit::VP));
    button->SetType(ButtonType::ICON);

    if (IsDeclarativePara()) {
        button->SetClickFunction([weak = WeakClaim(this), isFullScreen = isFullScreen_]() {
            auto videoElement = weak.Upgrade();
            if (videoElement) {
                videoElement->OnFullScreenBtnClick();
            }
        });
    } else {
        button->SetClickedEventId(fullscreenBtnClickId_);
    }
    return button;
}

const RefPtr<Component> VideoElement::SetPadding(const RefPtr<Component>& component, Edge&& edge)
{
    auto paddingComponent = AceType::MakeRefPtr<PaddingComponent>();
    paddingComponent->SetPadding(std::move(edge));
    paddingComponent->SetChild(component);

    return paddingComponent;
}

const RefPtr<Component> VideoElement::CreateControl()
{
    std::list<RefPtr<Component>> rowChildren;

    rowChildren.emplace_back(SetPadding(CreatePlayBtn(), Edge(theme_->GetBtnEdge())));

    rowChildren.emplace_back(SetPadding(CreateCurrentText(), Edge(theme_->GetTextEdge())));

    rowChildren.emplace_back(
        AceType::MakeRefPtr<FlexItemComponent>(VIDEO_CHILD_COMMON_FLEX_GROW, VIDEO_CHILD_COMMON_FLEX_SHRINK,
            VIDEO_CHILD_COMMON_FLEX_BASIS, SetPadding(CreateSlider(), Edge(theme_->GetSliderEdge()))));

    rowChildren.emplace_back(SetPadding(CreateDurationText(), Edge(theme_->GetTextEdge())));

#ifndef OHOS_STANDARD_SYSTEM
    rowChildren.emplace_back(SetPadding(CreateFullScreenBtn(), Edge(theme_->GetBtnEdge())));
#endif

    auto decoration = AceType::MakeRefPtr<Decoration>();
    decoration->SetBackgroundColor(theme_->GetBkgColor());
    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetBackDecoration(decoration);
    auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, rowChildren);
    row->SetTextDirection(textDirection_);
    box->SetChild(row);

    auto gestureListener = AceType::MakeRefPtr<GestureListenerComponent>(box);
    gestureListener->SetOnClickId(shieldId_);
    gestureListener->SetOnLongPressId(shieldId_);

    return gestureListener;
}

const RefPtr<Component> VideoElement::CreatePoster()
{
    RefPtr<ImageComponent> image;
    if (posterImage_) {
        image = posterImage_;
    } else {
        image = AceType::MakeRefPtr<ImageComponent>(poster_);
    }
    if (!image) {
        LOGE("create poster image fail");
        return nullptr;
    }
    image->SetImageFit(imageFit_);
    image->SetImageObjectPosition(imagePosition_);
    image->SetFitMaxSize(true);

    std::list<RefPtr<Component>> childrenAlign;
    childrenAlign.emplace_back(image);

    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(AceType::MakeRefPtr<AlignComponent>(childrenAlign, Alignment::CENTER));
    return box;
}

const RefPtr<Component> VideoElement::CreateChild()
{
    RefPtr<Component> child;
    if (isInitialState_ && (!poster_.empty() || posterImage_)) {
        std::list<RefPtr<Component>> columnChildren;
#ifndef OHOS_STANDARD_SYSTEM
        columnChildren.emplace_back(AceType::MakeRefPtr<FlexItemComponent>(VIDEO_CHILD_COMMON_FLEX_GROW,
            VIDEO_CHILD_COMMON_FLEX_SHRINK, VIDEO_CHILD_COMMON_FLEX_BASIS, CreatePoster()));
#else
#ifdef ENABLE_ROSEN_BACKEND
        columnChildren.emplace_back(AceType::MakeRefPtr<FlexItemComponent>(VIDEO_CHILD_COMMON_FLEX_GROW,
            VIDEO_CHILD_COMMON_FLEX_SHRINK, VIDEO_CHILD_COMMON_FLEX_BASIS, CreatePoster()));
#endif
#endif
        if (needControls_) {
            columnChildren.emplace_back(CreateControl());
        } else if (IsDeclarativePara()) {
            columnChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
        }
        child = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_END, FlexAlign::SPACE_AROUND, columnChildren);
    } else if (needControls_) {
        std::list<RefPtr<Component>> childrenAlign;
        childrenAlign.emplace_back(CreateControl());
        child = AceType::MakeRefPtr<AlignComponent>(childrenAlign, Alignment::BOTTOM_RIGHT);
    }

    if (child) {
        auto display = AceType::MakeRefPtr<DisplayComponent>(child);
        if (!display) {
            LOGE("Create display component failed. display is null.");
            return display;
        }
        auto textureRender = GetRenderNode();
        if (!textureRender) {
            return display;
        }
        auto displayRender = AceType::DynamicCast<RenderDisplay>(textureRender->GetFirstChild());
        if (!displayRender) {
            return display;
        }
        uint8_t opacity = displayRender->GetOpacity();
        display->SetOpacity(opacity * 1.0 / UINT8_MAX);
        return display;
    } else {
        return child;
    }
}

bool VideoElement::IsDeclarativePara()
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }

    return context->GetIsDeclarative();
}

void VideoElement::OnStartBtnClick()
{
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_->IsPlaying()) {
#else
    if (isPlaying_) {
#endif
        Pause();
    } else {
        Start();
    }
}

void VideoElement::OnFullScreenBtnClick()
{
    if (!isFullScreen_) {
        FullScreen();
    } else {
        ExitFullScreen();
    }
}

void VideoElement::OnSliderChange(const std::string& param)
{
    size_t pos = param.find("\"value\":");
    if (pos != std::string::npos) {
        if (pastPlayingStatus_) {
            isPlaying_ = false;
            Start();
            pastPlayingStatus_ = false;
        }
        std::stringstream ss;
        uint32_t value = 0;

        ss << param.substr(pos + 8); // Need to add the length of "\"value\":".
        ss >> value;

        SetCurrentTime(value);
        if (onSeeked_) {
            std::string param;
            if (IsDeclarativePara()) {
                auto json = JsonUtil::Create(true);
                json->Put("time", static_cast<double>(value));
                param = json->ToString();
            } else {
                param = std::string("\"seeked\",{\"currenttime\":").append(std::to_string(value)).append("}");
            }
            onSeeked_(param);
        }
    }
}

void VideoElement::OnSliderMoving(const std::string& param)
{
    size_t pos = param.find("\"value\":");
    if (pos != std::string::npos) {
        if (isPlaying_ && !pastPlayingStatus_) {
            Pause();
            pastPlayingStatus_ = true;
        }
        std::stringstream ss;
        uint32_t value = 0;

        // Need to add the length of "\"value\":".
        if (param.size() > (pos + 8)) {
            ss << param.substr(pos + 8);
            ss >> value;
        }

        SetCurrentTime(value);
        if (onSeeking_) {
            std::string param;
            if (IsDeclarativePara()) {
                auto json = JsonUtil::Create(true);
                json->Put("time", static_cast<double>(value));
                param = json->ToString();
            } else {
                param = std::string("\"seeking\",{\"currenttime\":").append(std::to_string(value)).append("}");
            }
            onSeeking_(param);
        }
    }
}

void VideoElement::IntTimeToText(uint32_t time, std::string& timeText)
{
    // Whether the duration is longer than 1 hour.
    bool needShowHour = duration_ > 3600;
    timeText = Localization::GetInstance()->FormatDuration(time, needShowHour);
}

void VideoElement::Start()
{
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_ == nullptr) {
        LOGE("player is null");
        return;
    }
    if (isStop_) {
        if (mediaPlayer_->Prepare() != 0) {
            LOGE("Player prepare failed");
            return;
        }
    }
    if (!mediaPlayer_->IsPlaying()) {
        LOGI("Video Start");
        auto context = context_.Upgrade();
        if (context == nullptr) {
            LOGE("context is nullptr");
            return;
        }
        auto platformTask = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::BACKGROUND);
        platformTask.PostTask([mediaPlayer = mediaPlayer_] { mediaPlayer->Play(); });
    }
#else
    if (isStop_) {
        CreatePlatformResource();
        return;
    }
    if (!isPlaying_ && player_) {
        player_->Start();
    }
#endif
}

void VideoElement::Pause()
{
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_ != nullptr && mediaPlayer_->IsPlaying()) {
        LOGI("Video Pause");
        mediaPlayer_->Pause();
    }
#else
    if (isPlaying_ && player_) {
        player_->Pause();
    }
#endif
}

void VideoElement::Stop()
{
    OnCurrentTimeChange(0);
    OnPlayerStatus(PlaybackStatus::STOPPED);
#ifndef OHOS_STANDARD_SYSTEM
    ReleasePlatformResource();
#else
    if (mediaPlayer_ != nullptr) {
        mediaPlayer_->Stop();
    }
#endif
    isStop_ = true;
}

void VideoElement::SetCurrentTime(float currentPos, SeekMode seekMode)
{
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_ != nullptr && GreatOrEqual(currentPos, 0.0) && LessNotEqual(currentPos, duration_)) {
        LOGI("Video Seek");
        mediaPlayer_->Seek(currentPos * MILLISECONDS_TO_SECONDS, ConvertToMediaSeekMode(seekMode));
    }
#else
    if (currentPos >= 0 && currentPos < duration_ && player_) {
        player_->SeekTo(currentPos);
    }
#endif
}

void VideoElement::FullScreen()
{
    if (!isFullScreen_ && !isError_) {
        if (fullscreenEvent_) {
            RefPtr<Component> component = fullscreenEvent_(true, player_, texture_);
            if (component) {
                auto context = context_.Upgrade();
                if (!context) {
                    return;
                }

                auto stackElement = context->GetLastStack();
                if (!stackElement) {
                    return;
                }

                // add fullscreen component cover component
                if (IsDeclarativePara()) {
                    stackElement->PushComponent(AceType::MakeRefPtr<ComposedComponent>("0", "fullscreen", component));
                } else {
                    auto composedComponent = AceType::DynamicCast<ComposedComponent>(component);
                    if (!composedComponent) {
                        LOGE("VideoElement::FullScreen: is not ComposedComponent");
                        return;
                    }
                    if (composedComponent->IsInspector()) {
                        LOGE("VideoElement::FullScreen: is InspectorComposedComponent");
                        return;
                    }
                    stackElement->PushComponent(AceType::MakeRefPtr<ComposedComponent>(composedComponent->GetId(),
                        composedComponent->GetName() + "fullscreen", composedComponent->GetChild()));
                }
                isFullScreen_ = true;
                currentPlatformVersion_ = context->GetMinPlatformVersion();
                if (player_ && currentPlatformVersion_ > COMPATIBLE_VERSION) {
#ifndef OHOS_STANDARD_SYSTEM
                    player_->SetDirection(direction_);
#endif
                }
                if (onFullScreenChange_) {
                    std::string param;
                    if (IsDeclarativePara()) {
                        auto json = JsonUtil::Create(true);
                        json->Put("fullscreen", isFullScreen_);
                        param = json->ToString();
                    } else {
                        param = std::string("\"fullscreenchange\",{\"fullscreen\":")
                                    .append(std::to_string(isFullScreen_))
                                    .append("}");
                    }
                    onFullScreenChange_(param);
                }
            }
        }
    }
}

void VideoElement::ExitFullScreen()
{
    if (fullscreenEvent_) {
        fullscreenEvent_(false, nullptr, nullptr);
    }

    if (!isExternalResource_ && isFullScreen_) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }

        auto stackElement = context->GetLastStack();
        if (!stackElement) {
            return;
        }
        stackElement->PopComponent();
        currentPlatformVersion_ = context->GetMinPlatformVersion();
        if (player_ && currentPlatformVersion_ > COMPATIBLE_VERSION) {
#ifndef OHOS_STANDARD_SYSTEM
            player_->SetLandscape();
#endif
        }
        isFullScreen_ = false;
        if (onFullScreenChange_) {
            std::string param;
            if (IsDeclarativePara()) {
                auto json = JsonUtil::Create(true);
                json->Put("fullscreen", isFullScreen_);
                param = json->ToString();
            } else {
                param = std::string("\"fullscreenchange\",{\"fullscreen\":")
                            .append(std::to_string(isFullScreen_))
                            .append("}");
            }
            onFullScreenChange_(param);
        }
        if (renderNode_) {
            renderNode_->MarkNeedLayout();
        }
    }
}

void VideoElement::SetVolume(float volume)
{
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_ != nullptr) {
        mediaPlayer_->SetVolume(volume, volume);
    }
#else
    if (player_) {
        player_->SetVolume(volume);
    }
#endif
}

void VideoElement::EnableLooping(bool loop)
{
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_) {
        mediaPlayer_->SetLooping(loop);
    }
#else
    if (player_) {
        player_->EnableLooping(loop);
    }
#endif
}

void VideoElement::SetSpeed(float speed)
{
    if (speed <= ILLEGAL_SPEED) {
        LOGE("speed is not valid: %{public}lf", speed);
        return;
    }
#ifdef OHOS_STANDARD_SYSTEM
    if (mediaPlayer_ != nullptr) {
        mediaPlayer_->SetPlaybackSpeed(ConvertToMediaPlaybackSpeed(speed));
    }
#else
    if (player_) {
        player_->SetSpeed(speed);
    }
#endif
}

void VideoElement::Dump()
{
    if (texture_) {
        DumpLog::GetInstance().AddDesc("texture:", texture_->GetHashCode());
    }
    if (player_) {
        DumpLog::GetInstance().AddDesc("player:", player_->GetHashCode());
    }
    DumpLog::GetInstance().AddDesc("isError:", isError_);
    DumpLog::GetInstance().AddDesc("poster:", poster_);
    DumpLog::GetInstance().AddDesc("isInitialState_:", isInitialState_);
    DumpLog::GetInstance().AddDesc("videoWidth:", videoWidth_);
    DumpLog::GetInstance().AddDesc("videoHeight:", videoHeight_);
    DumpLog::GetInstance().AddDesc("isReady:", isReady_);
    DumpLog::GetInstance().AddDesc("src:", src_);
    DumpLog::GetInstance().AddDesc("isAutoPlay:", isAutoPlay_);
    DumpLog::GetInstance().AddDesc("needControls:", needControls_);
    DumpLog::GetInstance().AddDesc("isMute:", isMute_);
}

bool VideoElement::OnKeyEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.action != KeyAction::UP) {
        return false;
    }
    switch (keyEvent.code) {
        case KeyCode::KEY_BACK:
        case KeyCode::KEY_ESCAPE: {
            if (isFullScreen_) {
                ExitFullScreen();
                return true;
            }
            break;
        }
        case KeyCode::KEY_ENTER: {
            if (!isFullScreen_) {
                FullScreen();
            } else {
                OnStartBtnClick();
            }
            return true;
        }
        case KeyCode::TV_CONTROL_MEDIA_PLAY: {
            OnStartBtnClick();
            return true;
        }
        case KeyCode::TV_CONTROL_LEFT: {
            if (isFullScreen_) {
                OnKeyLeft();
                if (!isPlaying_) {
                    Start();
                }
                return true;
            }
            break;
        }
        case KeyCode::TV_CONTROL_RIGHT: {
            if (isFullScreen_) {
                OnKeyRight();
                if (!isPlaying_) {
                    Start();
                }
                return true;
            }
            break;
        }
        default:
            break;
    }
    return false;
}

void VideoElement::OnKeyLeft()
{
    SetCurrentTime(currentPos_ > VIDEO_SEEK_STEP ? currentPos_ - VIDEO_SEEK_STEP : 0);
}

void VideoElement::OnKeyRight()
{
    if (currentPos_ + VIDEO_SEEK_STEP < duration_) {
        SetCurrentTime(currentPos_ + VIDEO_SEEK_STEP);
    }
}

void VideoElement::OnTextureRefresh()
{
    auto context = context_.Upgrade();
    if (context) {
        context->MarkForcedRefresh();
    }
}

} // namespace OHOS::Ace
