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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_POSITION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_POSITION_CONTROLLER_H

#include <functional>

#include "core/animation/curve.h"
#include "core/components/scroll/scroll_controller_interface.h"
#include "core/components/scroll/scrollable.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr uint32_t POSITION_MIDDLE = 0;
constexpr uint32_t POSITION_TOP = 1 << 0;
constexpr uint32_t POSITION_BOTTOM = 1 << 1;

enum class ScrollEvent : size_t {
    SCROLL_TOP = 0,
    SCROLL_BOTTOM,
    SCROLL_TOUCHUP,
    SCROLL_END,
    SCROLL_POSITION,
    SCROLL_EDGE,
    SCROLL_LEFT,
    SCROLL_RIGHT,
    UNKNOWN,
};

using OnScrollFunc = std::function<void(const std::string&)>;

class ScrollEventInfo : public BaseEventInfo, public EventToJSONStringAdapter {
    DECLARE_RELATIONSHIP_OF_CLASSES(ScrollEventInfo, BaseEventInfo, EventToJSONStringAdapter);
public:
    ScrollEventInfo(ScrollEvent type, double scrollX, double scrollY, int32_t scrollState);

    ~ScrollEventInfo() = default;

    std::string ToJSONString() const override;

    ScrollEvent GetType() const
    {
        return type_;
    }

    double GetScrollX() const
    {
        return scrollX_;
    }

    double GetScrollY() const
    {
        return scrollY_;
    }

    int32_t GetScrollState() const
    {
        return scrollState_;
    }

    void SetScrollType(ScrollEvent type)
    {
        type_ = type;
    }

private:
    ScrollEvent type_ = ScrollEvent::SCROLL_TOP;
    double scrollX_ = 0.0;
    double scrollY_ = 0.0;
    int32_t scrollState_ = -1;
};

using ScrollEventFunc = std::function<void(std::shared_ptr<ScrollEventInfo>&)>;
class ACE_EXPORT ScrollPositionController : public ScrollController {
    DECLARE_ACE_TYPE(ScrollPositionController, AceType);

public:
    ScrollPositionController() = default;
    ~ScrollPositionController() override = default;

    void SetInitialIndex(int32_t index)
    {
        if (index == 0) {
            flags_ = POSITION_TOP;
        } else {
            flags_ = POSITION_MIDDLE;
        }
        initialIndex_ = index;
    }

    int32_t GetInitialIndex() const
    {
        return initialIndex_;
    }

    void SetInitialOffset(double initialOffset)
    {
        if (NearZero(initialOffset)) {
            flags_ = POSITION_TOP;
        } else {
            flags_ = POSITION_MIDDLE;
        }
        initialOffset_ = initialOffset;
    }

    double GetInitialOffset() const
    {
        return initialOffset_;
    }

    double GetCurrentPosition() const;
    Axis GetScrollDirection() const override;

    void JumpTo(int32_t index, int32_t source = SCROLL_FROM_JUMP) override;
    void JumpTo(double position);
    bool AnimateTo(const Dimension& position, float duration, const RefPtr<Curve>& curve) override;
    bool AnimateTo(double position, float duration, const RefPtr<Curve>& curve, bool limitDuration = true,
        const std::function<void()>& onFinish = nullptr);
    bool AnimateToTarget(const ComposeId& targetId, float duration, const RefPtr<Curve>& curve,
        bool limitDuration = true, const std::function<void()>& onFinish = nullptr);
    void ScrollBy(double pixelX, double pixelY, bool smooth) override;
    void ScrollArrow(double scrollDistance, bool reverse, bool smooth);
    void ScrollToEdge(ScrollEdgeType scrollEdgeType, bool smooth) override;
    void ScrollPage(bool reverse, bool smooth) override;
    Offset GetCurrentOffset() const override;


    void SetScrollEvent(
        ScrollEvent event, const ScrollEventFunc& scrollEvent)
    {
        switch (event) {
            case ScrollEvent::SCROLL_TOP:
                scrollTop_ = scrollEvent;
                break;
            case ScrollEvent::SCROLL_BOTTOM:
                scrollBottom_ = scrollEvent;
                break;
            case ScrollEvent::SCROLL_END:
                scrollEnd_ = scrollEvent;
                break;
            case ScrollEvent::SCROLL_TOUCHUP:
                scrollTouchUp_ = scrollEvent;
                break;
            case ScrollEvent::SCROLL_POSITION:
                scrollPosition_ = scrollEvent;
                break;
            case ScrollEvent::SCROLL_EDGE:
                scrollEdge_ = scrollEvent;
                break;
            default:
                LOGW("unknown scroll event");
                break;
        }
    }

    void HandleScrollEvent(std::shared_ptr<ScrollEventInfo> info) const
    {
        if (!info) {
            LOGE("scroll event info is null");
            return;
        }
        auto eventType = info->GetType();
        switch (eventType) {
            case ScrollEvent::SCROLL_TOP:
                if (scrollTop_) {
                    scrollTop_(info);
                }
                if (scrollEdge_) {
                    scrollEdge_(info);
                }
                break;
            case ScrollEvent::SCROLL_BOTTOM:
                if (scrollBottom_) {
                    scrollBottom_(info);
                }
                if (scrollEdge_) {
                    scrollEdge_(info);
                }
                break;
            case ScrollEvent::SCROLL_END:
                if (scrollEnd_) {
                    scrollEnd_(info);
                }
                break;
            case ScrollEvent::SCROLL_TOUCHUP:
                if (scrollTouchUp_) {
                    scrollTouchUp_(info);
                }
                break;
            case ScrollEvent::SCROLL_POSITION:
                if (scrollPosition_) {
                    scrollPosition_(info);
                }
                break;
            default:
                LOGW("handle unknown scroll event");
                break;
        }
    }

    void SetTop()
    {
        flags_ |= POSITION_TOP;
    }

    void SetBottom()
    {
        flags_ |= POSITION_BOTTOM;
    }

    void SetMiddle()
    {
        flags_ = POSITION_MIDDLE;
    }

    bool IsTop() const
    {
        return (flags_ & POSITION_TOP);
    }

    bool IsBottom() const
    {
        return (flags_ & POSITION_BOTTOM);
    }

    void SetNonScrollable()
    {
        flags_ = (POSITION_TOP | POSITION_BOTTOM);
    }

    using OnFirstChanged = std::function<void(int32_t)>;
    void AddFirstChangedCallback(const OnFirstChanged& callback)
    {
        onChanged_ = callback;
    }

    using IndexerRotation = std::function<bool()>;
    void AddIndexerStateQueryCallback(const IndexerRotation& callback)
    {
        indexerRotationCallback_ = callback;
    }

    bool IsScrollNeedRotation()
    {
        return  !(indexerRotationCallback_ && indexerRotationCallback_());
    }

    void SetFirstItemIndex(int32_t firstIndex)
    {
        firstIndex_ = firstIndex;
        if (onChanged_) {
            onChanged_(firstIndex_);
        }
    }

    int32_t GetFirstItemIndex() const
    {
        return firstIndex_;
    }

private:
    int32_t initialIndex_ = 0;
    double initialOffset_ = 0.0;
    OnFirstChanged onChanged_;
    IndexerRotation indexerRotationCallback_;
    int32_t firstIndex_ = 0;

    ScrollEventFunc scrollTop_;
    ScrollEventFunc scrollBottom_;
    ScrollEventFunc scrollTouchUp_;
    ScrollEventFunc scrollEnd_;
    ScrollEventFunc scrollPosition_;
    ScrollEventFunc scrollEdge_;
    uint32_t flags_ = POSITION_TOP;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_POSITION_CONTROLLER_H
