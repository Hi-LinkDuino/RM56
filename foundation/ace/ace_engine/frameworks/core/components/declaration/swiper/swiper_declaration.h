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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SWIPER_SWIPER_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SWIPER_SWIPER_DECLARATION_H

#include "core/components/common/properties/swiper_indicator.h"
#include "core/components/declaration/common/declaration.h"
#include "core/components/swiper/swiper_controller.h"
#include "frameworks/core/components/common/rotation/rotation_controller.h"

namespace OHOS::Ace {

inline constexpr uint32_t DEFAULT_SWIPER_CURRENT_INDEX = 0;
inline constexpr double DEFAULT_SWIPER_ANIMATION_DURATION = 500.0;
inline constexpr double DEFAULT_SWIPER_AUTOPLAY_INTERVAL = 3000.0;
inline constexpr int32_t DEFAULT_SWIPER_CACHED_SIZE = -1;
inline constexpr int32_t DEFAULT_SWIPER_DISPLAY_COUNT = 1;

enum class SwiperDisplayMode {
    STRETCH = 0,
    AUTO_LINEAR,
};

struct SwiperAttribute : Attribute {
    uint32_t index = DEFAULT_SWIPER_CURRENT_INDEX;
    double duration = DEFAULT_SWIPER_ANIMATION_DURATION;
    Axis axis = Axis::HORIZONTAL;
    bool loop = true;
    bool autoPlay = false;
    bool animationOpacity = true;
    bool digitalIndicator = false;
    double autoPlayInterval = DEFAULT_SWIPER_AUTOPLAY_INTERVAL;
    int32_t cachedSize = DEFAULT_SWIPER_CACHED_SIZE;
    EdgeEffect edgeEffect = EdgeEffect::SPRING;
    SwiperDisplayMode displayMode = SwiperDisplayMode::STRETCH;
    int32_t displayCount = DEFAULT_SWIPER_DISPLAY_COUNT;
};

struct SwiperStyle : Style {
    AnimationCurve animationCurve { AnimationCurve::FRICTION };
    Dimension itemSpace;
    Color fadeColor = Color::GRAY;
    Dimension previousMargin;
    Dimension nextMargin;
};

struct SwiperEvent : Event {
    EventMarker changeEventId;
    EventMarker rotationEventId;
    EventMarker clickEventId;
    EventMarker animationFinishEventId;
};

struct SwiperMethod : Method {};

class SwiperDeclaration : public Declaration {
    DECLARE_ACE_TYPE(SwiperDeclaration, Declaration);

public:
    SwiperDeclaration();
    ~SwiperDeclaration() override = default;
    void InitializeStyle() override;
    uint32_t GetIndex() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.index;
    }
    void SetIndex(uint32_t index)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.index = index;
    }

    double GetDuration() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.duration;
    }
    void SetDuration(double duration)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.duration = duration;
    }

    Axis GetAxis() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.axis;
    }
    void SetAxis(Axis axis)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.axis = axis;
    }

    bool IsLoop() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.loop;
    }
    void SetLoop(bool loop)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.loop = loop;
    }

    int32_t GetDisplayCount() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.displayCount;
    }

    void SetDisplayCount(int32_t displayCount)
    {
        if (displayCount > 0) {
            auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
            attribute.displayCount = displayCount;
        }
    }

    bool IsAutoPlay() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.autoPlay;
    }
    void SetAutoPlay(bool autoPlay)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.autoPlay = autoPlay;
    }

    double GetAutoPlayInterval() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.autoPlayInterval;
    }
    void SetAutoPlayInterval(double autoPlayInterval)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.autoPlayInterval = autoPlayInterval;
    }

    int32_t GetCachedSize() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.cachedSize;
    }

    void SetCachedSize(int32_t cachedSize)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.cachedSize = cachedSize;
    }

    EdgeEffect GetEdgeEffect() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.edgeEffect;
    }

    void SetEdgeEffect(const EdgeEffect edgeEffect)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.edgeEffect = edgeEffect;
    }

    bool IsAnimationOpacity() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.animationOpacity;
    }
    void SetAnimationOpacity(bool animationOpacity)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.animationOpacity = animationOpacity;
    }

    bool GetDigitalIndicator() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.digitalIndicator;
    }
    void SetDigitalIndicator(bool digitalIndicator)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.digitalIndicator = digitalIndicator;
    }

    SwiperDisplayMode GetDisplayMode() const
    {
        auto& attribute = static_cast<SwiperAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.displayMode;
    }

    void SetDisplayMode(SwiperDisplayMode displayMode)
    {
        auto& attribute = MaybeResetAttribute<SwiperAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.displayMode = displayMode;
    }

    AnimationCurve GetAnimationCurve() const
    {
        auto& style = static_cast<SwiperStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.animationCurve;
    }
    void SetAnimationCurve(AnimationCurve animationCurve)
    {
        auto& style = MaybeResetStyle<SwiperStyle>(StyleTag::SPECIALIZED_STYLE);
        style.animationCurve = animationCurve;
    }

    const Dimension& GetItemSpace() const
    {
        auto& style = static_cast<SwiperStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.itemSpace;
    }

    void SetItemSpace(const Dimension& space)
    {
        auto& style = MaybeResetStyle<SwiperStyle>(StyleTag::SPECIALIZED_STYLE);
        style.itemSpace = space;
    }

    Color GetFadeColor() const
    {
        auto& style = static_cast<SwiperStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.fadeColor;
    }

    void SetFadeColor(Color fadeColor)
    {
        auto& style = MaybeResetStyle<SwiperStyle>(StyleTag::SPECIALIZED_STYLE);
        style.fadeColor = fadeColor;
    }

    void SetPreviousMargin(const Dimension& margin)
    {
        auto& style = MaybeResetStyle<SwiperStyle>(StyleTag::SPECIALIZED_STYLE);
        style.previousMargin = margin;
    }

    const Dimension& GetPreviousMargin() const
    {
        auto& style = static_cast<SwiperStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.previousMargin;
    }

    void SetNextMargin(const Dimension& margin)
    {
        auto& style = MaybeResetStyle<SwiperStyle>(StyleTag::SPECIALIZED_STYLE);
        style.nextMargin = margin;
    }

    const Dimension& GetNextMargin() const
    {
        auto& style = static_cast<SwiperStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.nextMargin;
    }

    const EventMarker& GetChangeEventId() const
    {
        auto& event = static_cast<SwiperEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.changeEventId;
    }
    void SetChangeEventId(const EventMarker& changeEventId)
    {
        auto& event = MaybeResetEvent<SwiperEvent>(EventTag::SPECIALIZED_EVENT);
        event.changeEventId = changeEventId;
    }

    const EventMarker& GetRotationEventId() const
    {
        auto& event = static_cast<SwiperEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.rotationEventId;
    }
    void SetRotationEventId(const EventMarker& rotationEventId)
    {
        auto& event = MaybeResetEvent<SwiperEvent>(EventTag::SPECIALIZED_EVENT);
        event.rotationEventId = rotationEventId;
    }

    const EventMarker& GetClickEventId() const
    {
        auto& event = static_cast<SwiperEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.clickEventId;
    }

    void SetClickEventId(const EventMarker& clickEventId)
    {
        auto& event = MaybeResetEvent<SwiperEvent>(EventTag::SPECIALIZED_EVENT);
        event.clickEventId = clickEventId;
    }

    const EventMarker& GetRemoteMessageEventId() const
    {
        auto& event = static_cast<SwiperEvent&>(GetEvent(EventTag::SPECIALIZED_REMOTE_MESSAGE_EVENT));
        return event.clickEventId;
    }

    void SetRemoteMessageEventId(const EventMarker& eventId)
    {
        auto& event = MaybeResetEvent<SwiperEvent>(EventTag::SPECIALIZED_REMOTE_MESSAGE_EVENT);
        event.clickEventId = eventId;
    }

    void SetAnimationFinishEventId(const EventMarker& animationFinishEventId)
    {
        auto& event = MaybeResetEvent<SwiperEvent>(EventTag::SPECIALIZED_EVENT);
        event.animationFinishEventId = animationFinishEventId;
    }

    const EventMarker& GetAnimationFinishEventId() const
    {
        auto& event = static_cast<SwiperEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.animationFinishEventId;
    }

    RefPtr<SwiperController> GetSwiperController() const
    {
        return swiperController_;
    }

    const RefPtr<RotationController>& GetRotationController() const
    {
        return rotationController_;
    }

    bool IsShowIndicator() const
    {
        return showIndicator_;
    }
    void SetShowIndicator(bool showIndicator)
    {
        showIndicator_ = showIndicator;
    }
    const RefPtr<SwiperIndicator>& GetIndicator() const
    {
        return indicator_;
    }
    void SetIndicator(const RefPtr<SwiperIndicator>& indicator)
    {
        indicator_ = indicator;
    }

    bool operator==(const SwiperDeclaration& swiper) const
    {
        return GetDuration() == swiper.GetDuration() &&
            GetAxis() == swiper.GetAxis() &&
            IsLoop() == swiper.IsLoop() &&
            IsAutoPlay() == swiper.IsAutoPlay() &&
            GetAutoPlayInterval() == swiper.GetAutoPlayInterval() &&
            GetCachedSize() == swiper.GetCachedSize() &&
            IsAnimationOpacity() == swiper.IsAnimationOpacity() &&
            GetEdgeEffect() == swiper.GetEdgeEffect() &&
            GetDigitalIndicator() == swiper.GetDigitalIndicator() &&
            GetDisplayMode() == swiper.GetDisplayMode() &&
            GetAnimationCurve() == swiper.GetAnimationCurve() &&
            GetItemSpace() == swiper.GetItemSpace() &&
            GetFadeColor() == swiper.GetFadeColor() &&
            GetPreviousMargin() == swiper.GetPreviousMargin() &&
            GetNextMargin() == swiper.GetNextMargin() &&
            IsShowIndicator() == swiper.IsShowIndicator() &&
            GetDisplayCount() == swiper.GetDisplayCount();
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetSpecializedEvent(int32_t pageId, const std::string& eventId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

private:
    bool showIndicator_ = true;
    RefPtr<SwiperIndicator> indicator_;
    RefPtr<SwiperController> swiperController_;
    RefPtr<RotationController> rotationController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SWIPER_SWIPER_DECLARATION_H
