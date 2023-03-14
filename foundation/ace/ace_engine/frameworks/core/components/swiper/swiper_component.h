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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_COMPONENT_H

#include "core/components/common/layout/constants.h"
#include "base/utils/macros.h"
#include "core/components/common/properties/swiper_indicator.h"
#include "core/components/declaration/swiper/swiper_declaration.h"
#include "core/components/swiper/swiper_controller.h"
#include "core/components_v2/foreach/lazy_foreach_component.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

using SwiperChangeEndListener = std::function<void(int32_t)>;
using MoveCallback = std::function<void(int32_t)>;

class ACE_EXPORT SwiperChangeEvent : public BaseEventInfo, public EventToJSONStringAdapter {
    DECLARE_RELATIONSHIP_OF_CLASSES(SwiperChangeEvent, BaseEventInfo, EventToJSONStringAdapter);

public:
    explicit SwiperChangeEvent(int32_t index) : BaseEventInfo("SwiperChangeEvent"), index_(index) {}
    ~SwiperChangeEvent() = default;

    int32_t GetIndex() const
    {
        return index_;
    }

    std::string ToJSONString() const override
    {
        return std::string(R"("change",{"index":)").append(std::to_string(index_).append("},null"));
    }

private:
    int32_t index_ = 0;
};

class ACE_EXPORT SwiperComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SwiperComponent, ComponentGroup);

public:
    explicit SwiperComponent(const std::list<RefPtr<Component>>& children);
    SwiperComponent(const std::list<RefPtr<Component>>& children, bool showIndicator);
    ~SwiperComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void AppendChild(const RefPtr<Component>& child) override;

    uint32_t GetIndex() const;
    void SetIndex(uint32_t index);
    void SetDuration(double duration);
    double GetDuration() const;
    Axis GetAxis() const;
    void SetAxis(Axis axis);
    bool IsLoop() const;
    void SetLoop(bool loop);
    bool IsAutoPlay() const;
    void SetAutoPlay(bool autoPlay);
    bool IsShow() const;
    void SetShow(bool show);
    bool IsAnimationOpacity() const;
    void SetAnimationOpacity(bool animationOpacity);
    const Dimension& GetItemSpace() const;
    void SetItemSpace(const Dimension& space);
    int32_t GetDisplayCount() const;
    void SetDisplayCount(int32_t displayCount);
    void SetDigitalIndicator(bool digitalIndicator);
    bool GetDigitalIndicator() const;
    Color GetFadeColor() const;
    void SetFadeColor(const Color& fadeColor);
    int32_t GetCachedSize() const;
    void SetCachedSize(int32_t cachedSize);
    const Dimension& GetPreviousMargin() const;
    void SetPreviousMargin(const Dimension& margin);
    const Dimension& GetNextMargin() const;
    void SetNextMargin(const Dimension& margin);
    EdgeEffect GetEdgeEffect() const;
    void SetEdgeEffect(const EdgeEffect edgeEffect);
    void SetAnimationFinishEventId(const EventMarker& animationFinishEventId);
    const EventMarker& GetAnimationFinishEventId() const;
    double GetAutoPlayInterval() const;
    void SetAutoPlayInterval(double autoPlayInterval);
    SwiperDisplayMode GetDisplayMode() const;
    void SetDisplayMode(SwiperDisplayMode displayMode);

    AnimationCurve GetAnimationCurve() const;
    void SetAnimationCurve(AnimationCurve animationCurve);

    const EventMarker& GetChangeEventId() const;
    void SetChangeEventId(const EventMarker& changeEventId);
    const EventMarker& GetRotationEventId() const;
    void SetRotationEventId(const EventMarker& rotationEventId);
    const EventMarker& GetClickEventId() const;
    void SetClickEventId(const EventMarker& clickEventId);
    const EventMarker& GetRemoteMessageEventId() const;
    void SetRemoteMessageEventId(const EventMarker& eventId);

    RefPtr<SwiperController> GetSwiperController() const;
    const RefPtr<RotationController>& GetRotationController() const;

    void SetShowIndicator(bool showIndicator);
    bool IsShowIndicator() const;
    RefPtr<SwiperIndicator> GetIndicator() const;
    void SetIndicator(const RefPtr<SwiperIndicator>& indicator);

    const SwiperChangeEndListener& GetChangeEndListener() const
    {
        return changeEndListener_;
    }

    void SetChangeEndListener(const SwiperChangeEndListener& changeEndListener)
    {
        changeEndListener_ = changeEndListener;
    }

    void SetMoveCallback(const MoveCallback& moveCallback)
    {
        moveCallback_ = moveCallback;
    }

    const MoveCallback& GetMoveCallback() const
    {
        return moveCallback_;
    }

    void SetSlideContinue(bool slideContinued)
    {
        slideContinued_ = slideContinued;
    }

    bool GetSlideContinue() const
    {
        return slideContinued_;
    }

    void DisableSwipe(bool disableSwipe)
    {
        disableSwipe_ = disableSwipe;
    }

    bool GetDisableSwipe() const
    {
        return disableSwipe_;
    }

    void SetDeclaration(const RefPtr<SwiperDeclaration>& declaration)
    {
        if (declaration) {
            declaration_ = declaration;
        }
    }

    bool GetDisableRotation() const
    {
        return disableRation_;
    }

    void SetDisableRotation(bool disableRation)
    {
        disableRation_ = disableRation;
    }

    void SetMainSwiperSize(MainSwiperSize mainSwiperSize)
    {
        mainSwiperSize_ = mainSwiperSize;
    }

    MainSwiperSize GetMainSwiperSize() const
    {
        return mainSwiperSize_;
    }

    RefPtr<V2::LazyForEachComponent> GetLazyForEachComponent() const
    {
        return lazyForEachComponent_.Upgrade();
    }

    void SetCurve(const RefPtr<Curve>& curve)
    {
        curve_ = curve;
    }

    const RefPtr<Curve>& GetCurve() const
    {
        return curve_;
    }

    bool operator==(const SwiperComponent& swiper) const
    {
        return *declaration_ == *swiper.declaration_ &&
            show_ == swiper.show_ &&
            slideContinued_ == swiper.slideContinued_ &&
            disableRation_ == swiper.disableRation_ &&
            disableSwipe_ == swiper.disableSwipe_ &&
            mainSwiperSize_ == swiper.mainSwiperSize_ &&
            curve_ == swiper.curve_;
    }

private:
    RefPtr<SwiperDeclaration> declaration_;
    bool show_ { true };
    bool slideContinued_ { false };
    bool disableRation_ { false };
    bool disableSwipe_ { false };
    SwiperChangeEndListener changeEndListener_;
    MoveCallback moveCallback_;
    MainSwiperSize mainSwiperSize_ = MainSwiperSize::AUTO;
    WeakPtr<V2::LazyForEachComponent> lazyForEachComponent_;
    RefPtr<Curve> curve_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_COMPONENT_H
