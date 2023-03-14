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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOUCH_LISTENER_TOUCH_LISTENER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOUCH_LISTENER_TOUCH_LISTENER_COMPONENT_H

#include <string>

#include "core/components/touch_listener/render_touch_listener.h"
#include "core/components/touch_listener/touch_listener_element.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

using OnTouchEventCallback = std::function<void(const TouchEventInfo&)>;

class TouchListenerComponent final : public SoleChildComponent {
    DECLARE_ACE_TYPE(TouchListenerComponent, SoleChildComponent);

public:
    TouchListenerComponent() = default;
    explicit TouchListenerComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~TouchListenerComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        return MakeRefPtr<TouchListenerElement>();
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderTouchListener::Create();
    }

    void SetEvent(const EventMarker& eventMarker, uint32_t action, uint32_t stage,
        uint32_t eventType)
    {
        eventIds_[action][stage][eventType] = eventMarker;
    }

    const EventMarker& GetEvent(
        uint32_t action, uint32_t stage, uint32_t eventType) const
    {
        return eventIds_[action][stage][eventType];
    }

    void SetOnTouchDownId(const EventMarker& onTouchDownId)
    {
        eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_DOWN] = onTouchDownId;
    }
    void SetOnTouchMoveId(const EventMarker& onTouchMoveId)
    {
        eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_MOVE] = onTouchMoveId;
    }
    void SetOnTouchUpId(const EventMarker& onTouchUpId)
    {
        eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_UP] = onTouchUpId;
    }
    void SetOnTouchCancel(const EventMarker& onTouchCancel)
    {
        eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_CANCEL] = onTouchCancel;
    }
    void SetOnSwipeId(const EventMarker& onSwipeId)
    {
        onSwipeId_ = onSwipeId;
    }

    void SetOnTouchId(const EventMarker& onTouchId)
    {
        onTouchId_ = onTouchId;
    }

    const EventMarker& GetOnTouchDownId() const
    {
        return eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_DOWN];
    }
    const EventMarker& GetOnTouchMoveId() const
    {
        return eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_MOVE];
    }
    const EventMarker& GetOnTouchUpId() const
    {
        return eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_UP];
    }
    const EventMarker& GetOnTouchCancelId() const
    {
        return eventIds_[EventAction::ON][EventStage::BUBBLE][EventType::TOUCH_CANCEL];
    }
    const EventMarker& GetOnSwipeId() const
    {
        return onSwipeId_;
    }
    const EventMarker& GetOnTouchId() const
    {
        return onTouchId_;
    }

    bool IsVisible() const
    {
        return isVisible_;
    }

    void SetIsVisible(bool isVisible)
    {
        isVisible_ = isVisible;
    }

private:
    EventMarker eventIds_[EventAction::SIZE][EventStage::SIZE][EventType::SIZE];
    EventMarker onSwipeId_;
    EventMarker onTouchId_;

    bool isVisible_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOUCH_LISTENER_TOUCH_LISTENER_COMPONENT_H
