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

#include "core/components/video/video_element_v2.h"

#include "base/json/json_util.h"
#include "core/components/box/box_component.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {

RefPtr<Component> VideoElementV2::GetRootComponent()
{
    std::string componentNames[] = {
        "flexItem", "display", "transform", "touch", "pan_gesture", "click_gesture",
        "focusable", "box", "shared_transition"
    };
    for (auto& name : componentNames) {
        auto iter = map_.find(name);
        if (iter != map_.end()) {
            iter->second->OnWrap();
            return iter->second;
        }
    }
    return nullptr;
}

RefPtr<Component> VideoElementV2::FireFullscreen(
    bool isFullScreen, const WeakPtr<Player>& player, const WeakPtr<Texture>& texture)
{
    const auto& rootComponent = GetRootComponent();
    if (!rootComponent) {
        LOGE("VideoElementV2:rootComponent is null.");
        return nullptr;
    }

    auto singleChild = AceType::DynamicCast<SingleChild>(rootComponent);
    if (!singleChild) {
        LOGE("VideoElementV2:singleChild is null.");
        return nullptr;
    }
    if (isFullScreen) {
        if (isFullscreen_) {
            return nullptr;
        }
        originComponent_ = singleChild->GetChild();
        videoComponent_->SetFullscreen(true);
        videoComponent_->SetPlayer(player);
        videoComponent_->SetTexture(texture);
        singleChild->SetChild(GetEventComponents(videoComponent_));
        isFullscreen_ = true;
        return rootComponent;
    }

    if (!isFullscreen_) {
        return nullptr;
    }

    if (!originComponent_) {
        LOGE("the origin component is null");
        return nullptr;
    }
    videoComponent_->SetFullscreen(false);
    videoComponent_->SetPlayer(nullptr);
    videoComponent_->SetTexture(nullptr);
    singleChild->SetChild(originComponent_);
    isFullscreen_ = false;
    auto controller = videoComponent_->GetVideoController();
    if (controller) {
        controller->ExitFullscreen(true);
    }
    return nullptr;
}

RefPtr<Component> VideoElementV2::GetEventComponents(const RefPtr<Component>& videoChild)
{
    std::vector<RefPtr<Component>> components;
    std::string componentNames[] = {
        "display", "transform", "touch", "pan_gesture", "click_gesture", "focusable"
    };
    for (auto& name : componentNames) {
        auto iter = map_.find(name);
        if (iter != map_.end()) {
            iter->second->OnWrap();
            components.emplace_back(iter->second);
        }
    }

    for (int32_t idx = static_cast<int32_t>(components.size()) - 1; idx - 1 >= 0; --idx) {
        auto singleChild = AceType::DynamicCast<SingleChild>(components[idx - 1]);
        if (singleChild) {
            singleChild->SetChild(components[idx]);
            continue;
        }
    }

    auto box = AceType::MakeRefPtr<BoxComponent>();
    Component::MergeRSNode(box);
    auto oldBoxIter = map_.find("box");
    if (oldBoxIter != map_.end()) {
        auto oldBox = AceType::DynamicCast<BoxComponent>(oldBoxIter->second);
        if (oldBox) {
            box->SetOnClick(oldBox->GetOnClick());
        }
    }
    box->SetChild(videoChild);

    if (!components.empty()) {
        const auto& lastEventComponent = AceType::DynamicCast<SingleChild>(components.back());
        if (lastEventComponent) {
            lastEventComponent->SetChild(box);
            return components.front();
        }
    }
    return box;
}

void VideoElementV2::SetNewComponent(const RefPtr<Component>& newComponent)
{
    auto videoComponent = AceType::DynamicCast<VideoComponentV2>(newComponent);
    if (!videoComponent_ && videoComponent) {
        videoComponent_ = videoComponent;
        if (!videoComponent->GetPlayer().Upgrade() || !videoComponent->GetTexture().Upgrade()) {
            videoComponent_->SetFullscreenEvent(
                [weak = AceType::WeakClaim(this)](bool fullscreen, const WeakPtr<Player>& player,
                                            const WeakPtr<Texture>& texture) {
                    auto client = weak.Upgrade();
                    RefPtr<OHOS::Ace::Component> result;
                    if (client) {
                        result = client->FireFullscreen(fullscreen, player, texture);
                    }
                    return result;
                });
        }
    }

    if (videoComponent) {
        UpdateVideoComponent(videoComponent);
        auto singleChild = AceType::DynamicCast<SingleChild>(videoComponent->GetParent().Upgrade());
        if (singleChild) {
            singleChild->SetChild(videoComponent_);
        }
        VideoElement::SetNewComponent(videoComponent_);
    } else {
        VideoElement::SetNewComponent(newComponent);
    }
}

void VideoElementV2::UpdateVideoComponent(const RefPtr<VideoComponentV2>& videoComponent)
{
    if (videoComponent) {
        videoComponent_->SetMute(videoComponent->IsMute());
        videoComponent_->SetSrc(videoComponent->GetSrc());
        videoComponent_->SetAutoPlay(videoComponent->IsAutoPlay());
        videoComponent_->SetPoster(videoComponent->GetPoster());
        videoComponent_->SetPosterImage(videoComponent->GetPosterImage());
        videoComponent_->SetNeedControls(videoComponent->NeedControls());
        videoComponent_->SetLoop(videoComponent->IsLoop());
        videoComponent_->SetStartTime(videoComponent->GetStartTime());
        videoComponent_->SetSpeed(videoComponent->GetSpeed());
        videoComponent_->SetDirection(videoComponent->GetDirection());
        videoComponent_->SetFit(videoComponent->GetFit());
        videoComponent_->SetGestureComponentMap(videoComponent->GetGestureComponentMap());

        videoComponent_->SetPreparedEventId(videoComponent->GetPreparedEventId());
        videoComponent_->SetStartEventId(videoComponent->GetStartEventId());
        videoComponent_->SetPauseEventId(videoComponent->GetPauseEventId());
        videoComponent_->SetStopEventId(videoComponent->GetStopEventId());
        videoComponent_->SetFinishEventId(videoComponent->GetFinishEventId());
        videoComponent_->SetErrorEventId(videoComponent->GetErrorEventId());
        videoComponent_->SetSeekingEventId(videoComponent->GetSeekingEventId());
        videoComponent_->SetSeekedEventId(videoComponent->GetSeekedEventId());
        videoComponent_->SetTimeUpdateEventId(videoComponent->GetTimeUpdateEventId());
        videoComponent_->SetFullscreenChangeEventId(videoComponent->GetFullscreenChangeEventId());

        videoComponent->SetVideoController(videoComponent_->GetVideoController());
    }
}

void VideoElementV2::InitStatus(const RefPtr<VideoComponent>& videoComponent)
{
    VideoElement::InitStatus(videoComponent);
    auto videoComponentV2 = AceType::DynamicCast<VideoComponentV2>(videoComponent);
    if (videoComponentV2) {
        map_ = videoComponentV2->GetGestureComponentMap();
    }
}

void VideoElementV2::OnPreFullScreen(bool isPortrait)
{
    direction_ = isPortrait ? "horizontal" : "vertical";
}

} // namespace OHOS::Ace
