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

#include "render_touch_listener.h"

#include "core/components/touch_listener/touch_listener_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

RenderTouchListener::RenderTouchListener() : rawRecognizer_(AceType::MakeRefPtr<RawRecognizer>()) {}

RefPtr<RenderNode> RenderTouchListener::Create()
{
    return AceType::MakeRefPtr<RenderTouchListener>();
}

void RenderTouchListener::Update(const RefPtr<Component>& component)
{
    auto touchComponent = AceType::DynamicCast<TouchListenerComponent>(component);
    ACE_DCHECK(touchComponent);
    responseRegion_ = touchComponent->GetResponseRegion();
    isResponseRegion_ = touchComponent->IsResponseRegion();
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        onTouchEventCallback_ = AceSyncEvent<void(const std::shared_ptr<TouchEventInfo>&)>::Create(
            touchComponent->GetOnTouchId(), context_);
        return;
    }

    for (uint32_t eventStage = 0; eventStage < EventStage::SIZE; eventStage++) {
        for (uint32_t touchEventType = 0; touchEventType < EventType::SIZE; touchEventType++) {
            auto& onEventId = touchComponent->GetEvent(EventAction::ON, eventStage, touchEventType);
            if (!onEventId.IsEmpty()) {
                rawRecognizer_->SetOnEventCallback(
                    AceAsyncEvent<void(const TouchEventInfo&)>::Create(onEventId, context_), eventStage,
                    touchEventType);
            }
            auto& catchEventId = touchComponent->GetEvent(EventAction::CATCH, eventStage, touchEventType);
            if (!catchEventId.IsEmpty()) {
                rawRecognizer_->SetCatchEventCallback(
                    AceAsyncEvent<void()>::Create(catchEventId, context_), eventStage, touchEventType);
            }
        }
    }
    touchable_ = touchComponent->IsTouchable();
    isVisible_ = touchComponent->IsVisible();
    interceptTouchEvent_ = !touchable_;

    if (!touchComponent->GetEvent(EventAction::CATCH, EventStage::CAPTURE, EventType::TOUCH_DOWN).IsEmpty()) {
        EventMarker eventMarker("catchEvent");
        auto event = AceAsyncEvent<void()>::Create(eventMarker, context_);
        rawRecognizer_->SetCatchEventCallback(event, EventStage::CAPTURE, EventType::TOUCH_UP);
        rawRecognizer_->SetCatchEventCallback(event, EventStage::CAPTURE, EventType::TOUCH_MOVE);
    }
    SetOnSwipe(AceAsyncEvent<void(const SwipeEventInfo&)>::Create(touchComponent->GetOnSwipeId(), context_));
}

void RenderTouchListener::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    LOGD("render touch listener: on touch test hit!");
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        coordinateOffset_ = coordinateOffset;
        result.emplace_back(Claim(this));
        return;
    }
    rawRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(rawRecognizer_);
    if (swipeRecognizer_) {
        result.emplace_back(swipeRecognizer_);
    }
}

bool RenderTouchListener::GetVisible() const
{
    return RenderNode::GetVisible() && isVisible_;
}

bool RenderTouchListener::DispatchEvent(const TouchEvent& point)
{
    return true;
}

bool RenderTouchListener::HandleEvent(const TouchEvent& point)
{
    bool isPropagation = true;
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        if (point.type == TouchType::DOWN) {
            touchPointMap_[point.id] = point;
            isPropagation = TriggerTouchCallBack(point);
        } else if (point.type == TouchType::UP) {
            isPropagation = TriggerTouchCallBack(point);
            touchPointMap_.erase(point.id);
        } else {
            for (const auto& pointPair : touchPointMap_) {
                if (pointPair.first == point.id && (pointPair.second.x != point.x || pointPair.second.y != point.y)) {
                    touchPointMap_[point.id] = point;
                    isPropagation = TriggerTouchCallBack(point);
                    break;
                }
            }
        }
    }
    return isPropagation;
}

bool RenderTouchListener::TriggerTouchCallBack(const TouchEvent& changedPoint)
{
    if (!onTouchEventCallback_) {
        return true;
    }
    auto event = std::make_shared<TouchEventInfo>("touchEvent");
    event->SetTimeStamp(changedPoint.time);
    TouchLocationInfo changedInfo(changedPoint.id);
    float localX = changedPoint.x - coordinateOffset_.GetX();
    float localY = changedPoint.y - coordinateOffset_.GetY();
    changedInfo.SetLocalLocation(Offset(localX, localY));
    changedInfo.SetGlobalLocation(Offset(changedPoint.x, changedPoint.y));
    changedInfo.SetScreenLocation(Offset(changedPoint.screenX, changedPoint.screenY));
    changedInfo.SetTouchType(changedPoint.type);
    event->AddChangedTouchLocationInfo(std::move(changedInfo));

    // all fingers collection
    for (const auto& pointPair : touchPointMap_) {
        float globalX = pointPair.second.x;
        float globalY = pointPair.second.y;
        float screenX = pointPair.second.screenX;
        float screenY = pointPair.second.screenY;
        float localX = pointPair.second.x - coordinateOffset_.GetX();
        float localY = pointPair.second.y - coordinateOffset_.GetY();
        TouchLocationInfo info(pointPair.second.id);
        info.SetGlobalLocation(Offset(globalX, globalY));
        info.SetLocalLocation(Offset(localX, localY));
        info.SetScreenLocation(Offset(screenX, screenY));
        info.SetTouchType(pointPair.second.type);
        event->AddTouchLocationInfo(std::move(info));
    }
    onTouchEventCallback_(event);
    LOGD("IsStopPropagation = %{public}d", event->IsStopPropagation());
    return event->IsStopPropagation() ? false : true;
}

} // namespace OHOS::Ace