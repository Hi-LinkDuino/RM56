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

#include "frameworks/bridge/common/dom/dom_video.h"

#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t METHOD_VIDEO_TO_ARGS_SIZE = 1;

}

DOMVideo::DOMVideo(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    videoChild_ = AceType::MakeRefPtr<VideoComponent>();
    videoChild_->SetFullscreenEvent([weak = AceType::WeakClaim(this)](bool fullscreen, const WeakPtr<Player>& player,
                                        const WeakPtr<Texture>& texture) {
        auto client = weak.Upgrade();
        RefPtr<OHOS::Ace::Component> result;
        if (client) {
            result = client->FireFullscreen(fullscreen, player, texture);
        }
        return result;
    });
    if (IsRightToLeft()) {
        videoChild_->SetTextDirection(TextDirection::RTL);
    }
}

bool DOMVideo::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // Operator map for attr
    static const std::unordered_map<std::string, void (*)(const RefPtr<VideoComponent>&, const std::string&)>
        attrOperators = {
            { DOM_VIDEO_MUTED,
                [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetMute(StringToBool(val));
                } },
            { DOM_VIDEO_SRC,
                [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetSrc(val);
                } },
            { DOM_VIDEO_AUTOPLAY,
                [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetAutoPlay(StringToBool(val));
                } },
            { DOM_VIDEO_POSTER,
                [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetPoster(val);
                } },
            { DOM_VIDEO_CONTROLS,
                [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetNeedControls(StringToBool(val));
                } },
            { DOM_VIDEO_LOOP,
                [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetLoop(StringToBool(val));
                } },
            { DOM_VIDEO_START_TIME,
                [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetStartTime(StringToInt(val));
                } },
            { DOM_VIDEO_SPEED, [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetSpeed(StringUtils::StringToDouble(val));
                } },
            { DOM_VIDEO_DIRECTION, [](const RefPtr<VideoComponent>& video, const std::string& val) {
                    video->SetDirection(val);
                } },
        };
    auto operatorIter = attrOperators.find(attr.first);
    if (operatorIter != attrOperators.end()) {
        operatorIter->second(videoChild_, attr.second);
        return true;
    }
    return false;
}

bool DOMVideo::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // Operator map for styles
    static const std::unordered_map<std::string, void (*)(const RefPtr<VideoComponent>&, const std::string&)>
        styleOperators = {
            // Set video-fit
            { DOM_VIDEO_FIT, [](const RefPtr<VideoComponent>& video,
                             const std::string& val) { video->SetFit(ConvertStrToFit(val)); } },
            // Set video-position
            { DOM_VIDEO_POSITION, [](const RefPtr<VideoComponent>& video,
                                const std::string& val) { video->SetImagePosition(ConvertStrToPosition(val)); } },
        };
    auto operatorIter = styleOperators.find(style.first);
    if (operatorIter != styleOperators.end()) {
        operatorIter->second(videoChild_, style.second);
        return true;
    }
    return false;
}

bool DOMVideo::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    // Operator map for event
    static const std::unordered_map<std::string, void (*)(const RefPtr<VideoComponent>&, const EventMarker&)>
        eventOperators = {
            { DOM_VIDEO_EVENT_PREPARED, [](const RefPtr<VideoComponent>& video,
                                        const EventMarker& event) { video->SetPreparedEventId(event); } },
            { DOM_VIDEO_EVENT_START,
                [](const RefPtr<VideoComponent>& video, const EventMarker& event) { video->SetStartEventId(event); } },
            { DOM_VIDEO_EVENT_PAUSE,
                [](const RefPtr<VideoComponent>& video, const EventMarker& event) { video->SetPauseEventId(event); } },
            { DOM_VIDEO_EVENT_STOP,
                [](const RefPtr<VideoComponent>& video, const EventMarker& event) { video->SetStopEventId(event); } },
            { DOM_VIDEO_EVENT_FINISH,
                [](const RefPtr<VideoComponent>& video, const EventMarker& event) { video->SetFinishEventId(event); } },
            { DOM_VIDEO_EVENT_ERROR,
                [](const RefPtr<VideoComponent>& video, const EventMarker& event) { video->SetErrorEventId(event); } },
            { DOM_VIDEO_EVENT_SEEKING, [](const RefPtr<VideoComponent>& video,
                                       const EventMarker& event) { video->SetSeekingEventId(event); } },
            { DOM_VIDEO_EVENT_SEEKED,
                [](const RefPtr<VideoComponent>& video, const EventMarker& event) { video->SetSeekedEventId(event); } },
            { DOM_VIDEO_EVENT_TIMEUPDATE, [](const RefPtr<VideoComponent>& video,
                                          const EventMarker& event) { video->SetTimeUpdateEventId(event); } },
            { DOM_VIDEO_EVENT_FULLSCREENCHANGE,
                [](const RefPtr<VideoComponent>& video, const EventMarker& event) {
                    video->SetFullscreenChangeEventId(event);
                } },
        };
    auto operatorIter = eventOperators.find(event);
    if (operatorIter != eventOperators.end()) {
        operatorIter->second(videoChild_, EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

void DOMVideo::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    // Operator map for method
    static const std::unordered_map<std::string, void (*)(const RefPtr<VideoComponent>&, const std::string&)>
        methedOperators = {
            { DOM_VIDEO_METHOD_START,
                [](const RefPtr<VideoComponent>& video, const std::string& args) {
                    auto controller = video->GetVideoController();
                    ACE_DCHECK(controller);
                    controller->Start();
                } },
            { DOM_VIDEO_METHOD_PAUSE,
                [](const RefPtr<VideoComponent>& video, const std::string& args) {
                    auto controller = video->GetVideoController();
                    ACE_DCHECK(controller);
                    controller->Pause();
                } },
            { DOM_VIDEO_METHOD_STOP,
                [](const RefPtr<VideoComponent>& video, const std::string& args) {
                  auto controller = video->GetVideoController();
                  ACE_DCHECK(controller);
                  controller->Stop();
                } },
            { DOM_VIDEO_METHOD_SEEK_TO,
                [](const RefPtr<VideoComponent>& video, const std::string& args) {
                    auto controller = video->GetVideoController();
                    ACE_DCHECK(controller);
                    auto value = GetParamFromJson(args, "currenttime");
                    if (value) {
                        int32_t pos = value->GetInt();
                        controller->SeekTo(pos);
                    }
                } },
            { DOM_VIDEO_METHOD_REQUEST_FULLSCREEN,
                [](const RefPtr<VideoComponent>& video, const std::string& args) {
                    auto controller = video->GetVideoController();
                    ACE_DCHECK(controller);
                    auto value = GetParamFromJson(args, "screenOrientation ");
                    if (value) {
                        controller->RequestFullscreen(!(value->GetString() == "landscape"));
                    }
                } },
            { DOM_VIDEO_METHOD_EXIT_FULLSCREEN,
                [](const RefPtr<VideoComponent>& video, const std::string& args) {
                    auto controller = video->GetVideoController();
                    ACE_DCHECK(controller);
                    controller->ExitFullscreen(false);
                } },
        };
    auto operatorIter = methedOperators.find(method);
    if (operatorIter != methedOperators.end()) {
        operatorIter->second(videoChild_, args);
    }
}

std::unique_ptr<JsonValue> DOMVideo::GetParamFromJson(const std::string& json, const std::string& key)
{
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(json);
    if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != METHOD_VIDEO_TO_ARGS_SIZE) {
        LOGE("parse args error");
        return nullptr;
    }

    std::unique_ptr<JsonValue> Value = argsValue->GetArrayItem(0)->GetValue(key);
    if (!Value) {
        LOGE("get index failed");
        return nullptr;
    }
    return Value;
}

RefPtr<Component> DOMVideo::FireFullscreen(
    bool isFullScreen, const WeakPtr<Player>& player, const WeakPtr<Texture>& texture)
{
    const auto& composed = GetRootComponent();
    ACE_DCHECK(composed);
    if (isFullScreen) {
        if (isFullscreen_) {
            return nullptr;
        }
        originComponent_ = composed->GetChild();
        videoChild_->SetFullscreen(true);
        videoChild_->SetPlayer(player);
        videoChild_->SetTexture(texture);
        composed->SetChild(GetEventComponents(videoChild_));
        isFullscreen_ = true;
    } else {
        if (!isFullscreen_) {
            return nullptr;
        }
        if (!originComponent_) {
            LOGE("the origin component is null");
            return nullptr;
        }
        videoChild_->SetFullscreen(false);
        videoChild_->SetPlayer(nullptr);
        videoChild_->SetTexture(nullptr);
        composed->SetChild(originComponent_);
        isFullscreen_ = false;

        auto controller = videoChild_->GetVideoController();
        ACE_DCHECK(controller);
        controller->ExitFullscreen(true);
        return nullptr;
    }

    return GetRootComponent();
}

RefPtr<Component> DOMVideo::GetEventComponents(const RefPtr<Component>& videoChild)
{
    std::vector<RefPtr<SoleChildComponent>> components;
    RefPtr<FocusableComponent> focusableEventComponent = GetFocusableComponent();
    if (focusableEventComponent) {
        components.emplace_back(focusableEventComponent);
    }
    RefPtr<TouchListenerComponent> touchEventComponent = GetTouchListenerComponent();
    if (touchEventComponent) {
        components.emplace_back(touchEventComponent);
    }
    RefPtr<GestureListenerComponent> gestureEventComponent = GetGestureListenerComponent();
    if (gestureEventComponent) {
        components.emplace_back(gestureEventComponent);
    }
    RefPtr<MouseListenerComponent> mouseEventComponent = GetMouseListenerComponent();
    if (mouseEventComponent) {
        components.emplace_back(mouseEventComponent);
    }
    for (int32_t idx = static_cast<int32_t>(components.size()) - 1; idx >= 1; --idx) {
        components[idx - 1]->SetChild(components[idx]);
    }

    auto box = AceType::MakeRefPtr<BoxComponent>();
    Component::MergeRSNode(box);
    RefPtr<BoxComponent> oldBox = GetBoxComponent();
    if (oldBox) {
        box->SetOnClick(oldBox->GetOnClick());
    }
    box->SetChild(videoChild);

    if (!components.empty()) {
        const auto& lastEventComponent = components.back();
        if (lastEventComponent) {
            lastEventComponent->SetChild(box);
            return components.front();
        }
    }
    return box;
}

ImageFit DOMVideo::ConvertStrToFit(const std::string& fit)
{
    static const std::unordered_map<std::string, ImageFit> IMAGE_FIT_TABLE = {
        { "fill", ImageFit::FILL },
        { "contain", ImageFit::CONTAIN },
        { "cover", ImageFit::COVER },
        { "scaledown", ImageFit::SCALEDOWN },
        { "none", ImageFit::NONE },
    };
    auto imageFitIter = IMAGE_FIT_TABLE.find(fit);
    return imageFitIter != IMAGE_FIT_TABLE.end() ? imageFitIter->second : ImageFit::CONTAIN;
}

ImageObjectPosition DOMVideo::ConvertStrToPosition(const std::string& position)
{
    ImageObjectPosition objectPosition;
    if (!ParseBackgroundImagePosition(position, objectPosition)) {
        LOGE("ConvertStrToPosition failed");
    }
    return objectPosition;
}

} // namespace OHOS::Ace::Framework
