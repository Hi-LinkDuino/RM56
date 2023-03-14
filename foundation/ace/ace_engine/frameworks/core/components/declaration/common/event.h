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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_EVENT_H

#include <string>

#include "core/event/ace_event_handler.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

enum class EventTag {
    COMMON_RAW_EVENT = 0,
    COMMON_GESTURE_EVENT,
    COMMON_REMOTE_MESSAGE_GRESURE_EVENT,
    COMMON_FOCUS_EVENT,
    COMMON_KEY_EVENT,
    COMMON_MOUSE_EVENT,
    COMMON_SWIPE_EVENT,
    COMMON_ATTACH_EVENT,
    COMMON_CROWN_EVENT,
    SPECIALIZED_EVENT,
    SPECIALIZED_REMOTE_MESSAGE_EVENT,
    UNKNOWN,
    DEFAULT,
};

struct EventParam {
    virtual void FireEvent(const WeakPtr<PipelineContext>& context);
    bool isRefreshed = false;
    EventMarker eventMarker;
};

struct TouchEventParam : EventParam {
    TouchEventInfo touchEvent = TouchEventInfo("unknown");
    void FireEvent(const WeakPtr<PipelineContext>& context) override;
};

struct SwipeEvent : EventParam {};

struct Event {
    bool IsValid() const
    {
        return tag != EventTag::UNKNOWN;
    }

    bool IsShared() const
    {
        return isShared;
    }

    bool isShared = true;
    EventTag tag = EventTag::DEFAULT;
};

struct CommonRawEvent : Event {
    TouchEventParam touchStart;
    TouchEventParam touchMove;
    TouchEventParam touchCancel;
    TouchEventParam touchEnd;
    TouchEventParam captureTouchStart;
    TouchEventParam captureTouchMove;
    TouchEventParam captureTouchCancel;
    TouchEventParam captureTouchEnd;
    TouchEventParam catchBubbleTouchStart;
    TouchEventParam catchBubbleTouchMove;
    TouchEventParam catchBubbleTouchCancel;
    TouchEventParam catchBubbleTouchEnd;
    TouchEventParam catchCaptureTouchStart;
    TouchEventParam catchCaptureTouchMove;
    TouchEventParam catchCaptureTouchCancel;
    TouchEventParam catchCaptureTouchEnd;
};

struct CommonGestureEvent : Event {
    EventParam click;
    EventParam doubleClick;
    EventParam longPress;
    EventParam pinchStart;
    EventParam pinchUpdate;
    EventParam pinchEnd;
    EventParam pinchCancel;
    EventParam dragStart;
    EventParam drag;
    EventParam dragEnd;
    EventParam dragEnter;
    EventParam dragOver;
    EventParam dragLeave;
    EventParam dragDrop;
};

struct CommonFocusEvent : Event {
    EventParam focus;
    EventParam blur;
};

struct CommonKeyEvent : Event {
    EventParam key;
};

struct CommonMouseEvent : Event {
    EventParam mouse;
    EventParam mouseHover;
    EventParam mouseHoverExit;
};

struct CommonSwipeEvent : Event {
    EventParam swipe;
};

struct CommonAttachEvent : Event {
    EventParam attached;
    EventParam detached;
};

struct CommonCrownEvent : Event {
    EventParam rotate;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_EVENT_H
