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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GESTURE_LISTENER_GESTURE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GESTURE_LISTENER_GESTURE_COMPONENT_H

#include "core/gestures/gesture_group.h"
#include "core/gestures/single_child_gesture.h"

#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class GestureComponent final : public SoleChildComponent {
    DECLARE_ACE_TYPE(GestureComponent, SoleChildComponent);

public:
    GestureComponent() = default;
    explicit GestureComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~GestureComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return nullptr;
    }

    void SetPriority(GesturePriority priority)
    {
        priority_ = priority;
    }

    GesturePriority GetPriority()
    {
        return priority_;
    }

    void SetGestureMask(GestureMask gestureMask)
    {
        gestureMask_ = gestureMask;
    }

    GestureMask GetGestureMask()
    {
        return gestureMask_;
    }

    void PushGesture(RefPtr<Gesture> gesture)
    {
        gestureStack_.push(gesture);
    }

    void PopGesture()
    {
        if (gestureStack_.size() <= 1) {
            return;
        }

        auto gesture = gestureStack_.top();
        gestureStack_.pop();

        auto gestureGroup = AceType::DynamicCast<GestureGroup>(gestureStack_.top());
        if (gestureGroup) {
            gestureGroup->AddGesture(gesture);
        }

        auto container = AceType::DynamicCast<SingleChildGesture>(gestureStack_.top());
        if (container) {
            container->SetChild(std::move(gesture));
        }
    }

    RefPtr<Gesture> TopGesture()
    {
        if (gestureStack_.empty()) {
            return nullptr;
        }
        return gestureStack_.top();
    }

    RefPtr<Gesture> FinishGesture()
    {
        if (gestureStack_.empty()) {
            return nullptr;
        }

        auto gesture = gestureStack_.top();
        gestureStack_.pop();

        return gesture;
    }

private:
    GesturePriority priority_ = GesturePriority::Low;
    GestureMask gestureMask_ = GestureMask::Normal;
    std::stack<RefPtr<Gesture>> gestureStack_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GESTURE_LISTENER_GESTURE_COMPONENT_H
