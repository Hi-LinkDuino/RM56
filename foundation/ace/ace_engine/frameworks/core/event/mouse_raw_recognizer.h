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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MOUSE_RAW_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MOUSE_RAW_RECOGNIZER_H

#include <functional>

#include "core/event/mouse_event.h"

namespace OHOS::Ace {

class MouseEventInfo : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(MouseEventInfo, BaseEventInfo);

public:
    explicit MouseEventInfo(const std::string& type) : BaseEventInfo(type) {}
    ~MouseEventInfo() override = default;

    void SetMouseLocationInfo(const MouseEvent& event)
    {
        localMouse_ = event;
    }
    void SetMouseGlobalInfo(const MouseEvent& event)
    {
        globalMouse_ = event;
    }

    const MouseEvent& GetGlobalMouse() const
    {
        return globalMouse_;
    }
    const MouseEvent& GetLocalMouse() const
    {
        return localMouse_;
    }

private:
    MouseEvent localMouse_;
    MouseEvent globalMouse_;
};

using MouseCallback = std::function<void(const MouseEventInfo&)>;
using MouseHoverCallback = std::function<void()>;
using OnHoverCallback = std::function<void(bool)>;

class MouseRawRecognizer : public MouseEventTarget {
    DECLARE_ACE_TYPE(MouseRawRecognizer, MouseEventTarget);

public:
    void HandleEvent(const MouseEvent& event) override;
    void HandleHoverEvent(MouseState mouseState);

    void SetOnMouse(const MouseCallback& onMouse)
    {
        onMouse_ = onMouse;
    }

    void SetOnMouseHover(const MouseHoverCallback& onMouseHover)
    {
        onMouseHover_ = onMouseHover;
    }

    void SetOnHover(const OnHoverCallback& onHover)
    {
        onHover_ = onHover;
    }

    void SetOnMouseHoverExit(const MouseHoverCallback& onMouseHoverExit)
    {
        onMouseHoverExit_ = onMouseHoverExit;
    }

    // Coordinate offset is used to calculate the local location of the mouse point in the render node.
    void SetCoordinateOffset(const Offset& coordinateOffset)
    {
        coordinateOffset_ = coordinateOffset;
    }

private:
    MouseEventInfo CreateMouseEventInfo(const MouseEvent& event) const;

    MouseCallback onMouse_;
    Offset coordinateOffset_;
    MouseEvent lastEvent_;
    MouseHoverCallback onMouseHover_;
    MouseHoverCallback onMouseHoverExit_;
    OnHoverCallback onHover_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MOUSE_RAW_RECOGNIZER_H
