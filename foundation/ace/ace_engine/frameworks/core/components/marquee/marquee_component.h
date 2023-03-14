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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MARQUEE_MARQUEE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MARQUEE_MARQUEE_COMPONENT_H

#include "core/components/marquee/marquee_element.h"
#include "core/components/marquee/render_marquee.h"
#include "core/components/text/text_component.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

inline constexpr double DEFAULT_MARQUEE_SCROLL_AMOUNT = 6.0;
inline constexpr int32_t DEFAULT_MARQUEE_LOOP = -1;

using MarqueeStartFunc = std::function<void()>;
using MarqueeStopFunc = std::function<void()>;

class ACE_EXPORT MarqueeController : public virtual AceType {
    DECLARE_ACE_TYPE(MarqueeController, AceType);

public:
    void Start() const
    {
        if (startFunc_) {
            startFunc_();
        }
    }

    void Stop() const
    {
        if (stopFunc_) {
            stopFunc_();
        }
    }

    void SetFunction(const MarqueeStartFunc& startFunc, const MarqueeStopFunc& stopFunc)
    {
        startFunc_ = startFunc;
        stopFunc_ = stopFunc;
    }

private:
    MarqueeStartFunc startFunc_;
    MarqueeStopFunc stopFunc_;
};

class ACE_EXPORT MarqueeComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(MarqueeComponent, SoleChildComponent);

public:
    MarqueeComponent() : SoleChildComponent()
    {
        textChild_ = AceType::MakeRefPtr<TextComponent>("");
        SetChild(textChild_);
        controller_ = AceType::MakeRefPtr<MarqueeController>();
    }

    ~MarqueeComponent() override = default;
    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<MarqueeElement>();
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderMarquee::Create();
    }

    const std::string& GetValue() const
    {
        return textChild_->GetData();
    }

    void SetValue(const std::string& value)
    {
        textChild_->SetData(value);
    }

    const TextStyle& GetTextStyle() const
    {
        return textChild_->GetTextStyle();
    }

    void SetTextStyle(const TextStyle& textStyle)
    {
        textChild_->SetTextStyle(textStyle);
        textChild_->SetFocusColor(textStyle.GetTextColor());
    }

    double GetScrollAmount() const
    {
        return scrollAmount_;
    }

    void SetScrollAmount(double scrollAmount)
    {
        scrollAmount_ = scrollAmount;
    }

    int32_t GetLoop() const
    {
        return loop_;
    }

    void SetLoop(int32_t loop)
    {
        loop_ = loop;
    }

    MarqueeDirection GetDirection() const
    {
        return direction_;
    }

    void SetDirection(MarqueeDirection direction)
    {
        direction_ = direction;
    }

    void SetBounceEventId(const EventMarker& eventId)
    {
        bounceEventId_ = eventId;
    }

    const EventMarker& GetBounceEventId() const
    {
        return bounceEventId_;
    }

    void SetFinishEventId(const EventMarker& eventId)
    {
        finishEventId_ = eventId;
    }

    const EventMarker& GetFinishEventId() const
    {
        return finishEventId_;
    }

    void SetStartEventId(const EventMarker& eventId)
    {
        startEventId_ = eventId;
    }

    const EventMarker& GetStartEventId() const
    {
        return startEventId_;
    }

    RefPtr<MarqueeController> GetController() const
    {
        return controller_;
    }

    void SetPlayerStatus(bool playerStatus)
    {
        playerStatus_ = playerStatus;
    }

    bool GetPlayerStatus() const
    {
        return playerStatus_;
    }

    ACE_DEFINE_COMPONENT_EVENT(OnStart, void());
    ACE_DEFINE_COMPONENT_EVENT(OnBounce, void());
    ACE_DEFINE_COMPONENT_EVENT(OnFinish, void());

private:
    RefPtr<TextComponent> textChild_;
    double scrollAmount_ = DEFAULT_MARQUEE_SCROLL_AMOUNT;
    int32_t loop_ = DEFAULT_MARQUEE_LOOP;
    bool playerStatus_ = false;
    MarqueeDirection direction_ { MarqueeDirection::LEFT };
    EventMarker bounceEventId_;
    EventMarker finishEventId_;
    EventMarker startEventId_;
    RefPtr<MarqueeController> controller_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MARQUEE_MARQUEE_COMPONENT_H
