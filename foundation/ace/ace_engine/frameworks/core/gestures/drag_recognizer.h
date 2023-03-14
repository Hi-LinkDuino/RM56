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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_DRAG_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_DRAG_RECOGNIZER_H

#include <functional>
#include <unordered_map>

#include "base/geometry/axis.h"
#include "base/geometry/offset.h"
#include "core/event/touch_event.h"
#include "core/gestures/gesture_recognizer.h"
#include "core/gestures/velocity.h"
#include "core/gestures/velocity_tracker.h"
#include "core/pipeline/pipeline_context.h"


namespace OHOS::Ace {

class DragStartInfo : public TouchLocationInfo, public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(DragStartInfo, TouchLocationInfo, BaseEventInfo);

public:
    explicit DragStartInfo(int32_t fingerId) : TouchLocationInfo(fingerId), BaseEventInfo("onDragStart") {}
    ~DragStartInfo() override = default;
};

class DragUpdateInfo : public TouchLocationInfo, public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(DragUpdateInfo, TouchLocationInfo, BaseEventInfo);

public:
    explicit DragUpdateInfo(int32_t fingerId) : TouchLocationInfo(fingerId), BaseEventInfo("onDragUpdate") {}
    ~DragUpdateInfo() override = default;

    DragUpdateInfo& SetDelta(const Offset& delta)
    {
        delta_ = delta;
        return *this;
    }
    DragUpdateInfo& SetMainDelta(double mainDelta)
    {
        mainDelta_ = mainDelta;
        return *this;
    }

    const Offset& GetDelta() const
    {
        return delta_;
    }
    double GetMainDelta() const
    {
        return mainDelta_;
    }

private:
    // The delta offset between current point and the previous update.
    Offset delta_;

    // The delta offset in the main axis between current point and the previous update.
    double mainDelta_ = 0.0;
};

class DragEndInfo : public TouchLocationInfo, public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(DragEndInfo, TouchLocationInfo, BaseEventInfo);

public:
    explicit DragEndInfo(int32_t fingerId) : TouchLocationInfo(fingerId), BaseEventInfo("onDragEnd") {}
    ~DragEndInfo() override = default;

    const Velocity& GetVelocity() const
    {
        return velocity_;
    }
    double GetMainVelocity() const
    {
        return mainVelocity_;
    }

    const Offset& GetOffset() const
    {
        return offset_;
    }

    DragEndInfo& SetVelocity(const Velocity& velocity)
    {
        velocity_ = velocity;
        return *this;
    }
    DragEndInfo& SetMainVelocity(double mainVelocity)
    {
        mainVelocity_ = mainVelocity;
        return *this;
    }

    DragEndInfo& SetOffset(const Offset& offset)
    {
        offset_ = offset;
        return *this;
    }

private:
    // The velocity of the moving touch point when it leaves screen.
    Velocity velocity_;

    // The offset from the original touch point
    Offset offset_;

    // The velocity of the moving touch point in main axis when it leaves screen.
    double mainVelocity_ = 0.0;
};

using DragStartCallback = std::function<void(const DragStartInfo&)>;
using DragUpdateCallback = std::function<void(const DragUpdateInfo&)>;
using DragEndCallback = std::function<void(const DragEndInfo&)>;
using DragCancelCallback = std::function<void()>;
using DragUpdateNotifyCallback = std::function<void(double x, double y, const DragUpdateInfo& updateInfo)>;
using DragEndNotifyCallback = std::function<void(double x, double y, const DragEndInfo& endInfo)>;

class DragRecognizer : public GestureRecognizer {
    DECLARE_ACE_TYPE(DragRecognizer, GestureRecognizer);

public:
    explicit DragRecognizer(Axis axis) : axis_(axis) {}
    ~DragRecognizer() override = default;

    void OnAccepted(size_t touchId) override;
    void OnRejected(size_t touchId) override;

    void SetOnDragStart(const DragStartCallback& onDragStart)
    {
        onDragStart_ = onDragStart;
    }

    void SetOnDragUpdate(const DragUpdateCallback& onDragUpdate)
    {
        onDragUpdate_ = onDragUpdate;
    }

    void SetOnDragEnd(const DragEndCallback& onDragEnd)
    {
        onDragEnd_ = onDragEnd;
    }

    void SetOnDragCancel(const DragCancelCallback& onDragCancel)
    {
        onDragCancel_ = onDragCancel;
    }

    void SetDragUpdateNotify(const DragUpdateNotifyCallback& onDragNotifyCall)
    {
        onDragUpdateNotifyCall_ = onDragNotifyCall;
    }

    void SetDragEndNotify(const DragEndNotifyCallback& onDragNotifyCall)
    {
        onDragEndNotifyCall_ = onDragNotifyCall;
    }

    const TouchRestrict& GetTouchRestrict() const
    {
        return touchRestrict_;
    }

    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override
    {
        auto drag = AceType::DynamicCast<DragRecognizer>(recognizer);

        if (!drag) {
            return false;
        }

        if (axis_ != drag->axis_) {
            return false;
        }

        onDragStart_ = drag->onDragStart_;
        onDragUpdate_ = drag->onDragUpdate_;
        onDragEnd_ = drag->onDragEnd_;
        onDragCancel_ = drag->onDragCancel_;

        return true;
    }

    void SetContext(WeakPtr<PipelineContext> context)
    {
        context_ = std::move(context);
    }

private:
    void HandleTouchDownEvent(const TouchEvent& event) override;
    void HandleTouchUpEvent(const TouchEvent& event) override;
    void HandleTouchMoveEvent(const TouchEvent& event) override;
    void HandleTouchCancelEvent(const TouchEvent& event) override;
    bool IsDragGestureAccept(double offset) const;

    void Accept(size_t touchId);
    void Reject(size_t touchId);

    template<typename TFunc, typename... Ts>
    void AsyncCallback(TFunc&& func, Ts&&... args)
    {
        auto ctx = context_.Upgrade();
        if (!ctx) {
            std::forward<TFunc>(func)(std::forward<Ts>(args)...);
            return;
        }

        auto marker = EventMarker(
            [this, f = std::forward<TFunc>(func), args = std::make_tuple(std::forward<Ts>(args)...)]() mutable {
                std::apply(f, std::move(args));
            });

        ctx->FireAsyncEvent(std::move(marker));
    }

    WeakPtr<PipelineContext> context_;

    class DragFingersInfo {
    public:
        DragFingersInfo() = default;
        explicit DragFingersInfo(Axis axis) : velocityTracker_(axis) {}
        ~DragFingersInfo() = default;

        VelocityTracker velocityTracker_;
        Offset dragOffset_;
        DetectState states_ { DetectState::READY };
    };
    std::unordered_map<size_t, DragFingersInfo> dragFingers_;

    Axis axis_;
    DragStartCallback onDragStart_;
    DragUpdateCallback onDragUpdate_;
    DragEndCallback onDragEnd_;
    DragCancelCallback onDragCancel_;
    DragUpdateNotifyCallback onDragUpdateNotifyCall_;
    DragEndNotifyCallback onDragEndNotifyCall_;
};

class VerticalDragRecognizer : public DragRecognizer {
    DECLARE_ACE_TYPE(VerticalDragRecognizer, DragRecognizer);

public:
    VerticalDragRecognizer() : DragRecognizer(Axis::VERTICAL) {}
    ~VerticalDragRecognizer() override = default;
};

class HorizontalDragRecognizer : public DragRecognizer {
    DECLARE_ACE_TYPE(HorizontalDragRecognizer, DragRecognizer);

public:
    HorizontalDragRecognizer() : DragRecognizer(Axis::HORIZONTAL) {}
    ~HorizontalDragRecognizer() override = default;
};

class FreeDragRecognizer : public DragRecognizer {
    DECLARE_ACE_TYPE(FreeDragRecognizer, DragRecognizer);

public:
    FreeDragRecognizer() : DragRecognizer(Axis::FREE) {}
    ~FreeDragRecognizer() override = default;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_DRAG_RECOGNIZER_H
