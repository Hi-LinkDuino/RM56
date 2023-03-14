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

#include "core/components/swiper/swiper_component.h"

#include "core/components/display/display_component.h"
#include "core/components/foreach/for_each_component.h"
#include "core/components/swiper/render_swiper.h"
#include "core/components/swiper/swiper_element.h"
#include "core/components_v2/swiper/swiper_element.h"

namespace OHOS::Ace {
constexpr double MAX_OPACITY = 255.0;
const std::string DISPLAY_COMPOSED_NAME = "SwiperDisplayChild";

SwiperComponent::SwiperComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SwiperDeclaration>();
        declaration_->Init();
    }
}

SwiperComponent::SwiperComponent(const std::list<RefPtr<Component>>& children, bool showIndicator)
    : SwiperComponent(children)
{
    SetShowIndicator(showIndicator);
}

void SwiperComponent::AppendChild(const RefPtr<Component>& child)
{
    if (AceType::InstanceOf<V2::LazyForEachComponent>(child)) {
        auto lazyForEach = AceType::DynamicCast<V2::LazyForEachComponent>(child);
        lazyForEachComponent_ = lazyForEach;
    }

    if (AceType::InstanceOf<ComposedComponent>(child)) {
        auto composed = AceType::DynamicCast<ComposedComponent>(child);
        auto display = AceType::MakeRefPtr<DisplayComponent>(child);
        display->SetOpacity(MAX_OPACITY);
        ComponentGroup::AppendChild(AceType::MakeRefPtr<ComposedComponent>("display" + composed->GetName(),
            DISPLAY_COMPOSED_NAME, display));
    } else {
        ComponentGroup::AppendChild(child);
    }
}

RefPtr<RenderNode> SwiperComponent::CreateRenderNode()
{
    return RenderSwiper::Create();
}

RefPtr<Element> SwiperComponent::CreateElement()
{
    if (lazyForEachComponent_.Upgrade()) {
        return AceType::MakeRefPtr<V2::SwiperElement>();
    } else {
        return AceType::MakeRefPtr<SwiperElement>();
    }
}

uint32_t SwiperComponent::GetIndex() const
{
    return declaration_->GetIndex();
}

void SwiperComponent::SetIndex(uint32_t index)
{
    declaration_->SetIndex(index);
}

Axis SwiperComponent::GetAxis() const
{
    return declaration_->GetAxis();
}
void SwiperComponent::SetAxis(Axis axis)
{
    declaration_->SetAxis(axis);
}

bool SwiperComponent::IsLoop() const
{
    return declaration_->IsLoop();
}

void SwiperComponent::SetLoop(bool loop)
{
    declaration_->SetLoop(loop);
}

bool SwiperComponent::IsAutoPlay() const
{
    return declaration_->IsAutoPlay();
}
void SwiperComponent::SetAutoPlay(bool autoPlay)
{
    declaration_->SetAutoPlay(autoPlay);
}

bool SwiperComponent::IsShow() const
{
    return show_;
}
void SwiperComponent::SetShow(bool show)
{
    show_ = show;
}

double SwiperComponent::GetAutoPlayInterval() const
{
    return declaration_->GetAutoPlayInterval();
}
void SwiperComponent::SetAutoPlayInterval(double autoPlayInterval)
{
    declaration_->SetAutoPlayInterval(autoPlayInterval);
}

SwiperDisplayMode SwiperComponent::GetDisplayMode() const
{
    return declaration_->GetDisplayMode();
}
void SwiperComponent::SetDisplayMode(SwiperDisplayMode displayMode)
{
    declaration_->SetDisplayMode(displayMode);
}

void SwiperComponent::SetChangeEventId(const EventMarker& changeEventId)
{
    declaration_->SetChangeEventId(changeEventId);
}
const EventMarker& SwiperComponent::GetChangeEventId() const
{
    return declaration_->GetChangeEventId();
}

const EventMarker& SwiperComponent::GetRotationEventId() const
{
    return declaration_->GetRotationEventId();
}
void SwiperComponent::SetRotationEventId(const EventMarker& rotationEventId)
{
    declaration_->SetRotationEventId(rotationEventId);
}

const EventMarker& SwiperComponent::GetClickEventId() const
{
    return declaration_->GetClickEventId();
}

void SwiperComponent::SetClickEventId(const EventMarker& clickEventId)
{
    declaration_->SetClickEventId(clickEventId);
}

const EventMarker& SwiperComponent::GetRemoteMessageEventId() const
{
    return declaration_->GetRemoteMessageEventId();
}

void SwiperComponent::SetRemoteMessageEventId(const EventMarker& eventId)
{
    declaration_->SetRemoteMessageEventId(eventId);
}

void SwiperComponent::SetDuration(double duration)
{
    declaration_->SetDuration(duration);
}
double SwiperComponent::GetDuration() const
{
    return declaration_->GetDuration();
}

void SwiperComponent::SetShowIndicator(bool showIndicator)
{
    declaration_->SetShowIndicator(showIndicator);
}
bool SwiperComponent::IsShowIndicator() const
{
    return declaration_->IsShowIndicator();
}

RefPtr<SwiperIndicator> SwiperComponent::GetIndicator() const
{
    if (declaration_->IsShowIndicator()) {
        return declaration_->GetIndicator();
    }
    return nullptr;
}

void SwiperComponent::SetIndicator(const RefPtr<SwiperIndicator>& indicator)
{
    declaration_->SetIndicator(indicator);
}

RefPtr<SwiperController> SwiperComponent::GetSwiperController() const
{
    return declaration_->GetSwiperController();
}

const RefPtr<RotationController>& SwiperComponent::GetRotationController() const
{
    return declaration_->GetRotationController();
}

void SwiperComponent::SetDigitalIndicator(bool digitalIndicator)
{
    return declaration_->SetDigitalIndicator(digitalIndicator);
}

bool SwiperComponent::GetDigitalIndicator() const
{
    return declaration_->GetDigitalIndicator();
}

AnimationCurve SwiperComponent::GetAnimationCurve() const
{
    return declaration_->GetAnimationCurve();
}

void SwiperComponent::SetAnimationCurve(AnimationCurve animationCurve)
{
    declaration_->SetAnimationCurve(animationCurve);
}

bool SwiperComponent::IsAnimationOpacity() const
{
    return declaration_->IsAnimationOpacity();
}

void SwiperComponent::SetAnimationOpacity(bool animationOpacity)
{
    declaration_->SetAnimationOpacity(animationOpacity);
}

const Dimension& SwiperComponent::GetItemSpace() const
{
    return declaration_->GetItemSpace();
}

void SwiperComponent::SetItemSpace(const Dimension& space)
{
    declaration_->SetItemSpace(space);
}

Color SwiperComponent::GetFadeColor() const
{
    return  declaration_->GetFadeColor();
}

void SwiperComponent::SetFadeColor(const Color& fadeColor)
{
    declaration_->SetFadeColor(fadeColor);
}

EdgeEffect SwiperComponent::GetEdgeEffect() const
{
    return declaration_->GetEdgeEffect();
}

void SwiperComponent::SetEdgeEffect(const EdgeEffect edgeEffect)
{
    declaration_->SetEdgeEffect(edgeEffect);
}

int32_t SwiperComponent::GetDisplayCount() const
{
    return declaration_->GetDisplayCount();
}

void SwiperComponent::SetDisplayCount(int32_t displayCount)
{
    declaration_->SetDisplayCount(displayCount);
}

void SwiperComponent::SetAnimationFinishEventId(const EventMarker& animationFinishEventId)
{
    declaration_->SetAnimationFinishEventId(animationFinishEventId);
}

const EventMarker& SwiperComponent::GetAnimationFinishEventId() const
{
    return declaration_->GetAnimationFinishEventId();
}

int32_t SwiperComponent::GetCachedSize() const
{
    return declaration_->GetCachedSize();
}

void SwiperComponent::SetCachedSize(int32_t cachedSize)
{
    declaration_->SetCachedSize(cachedSize);
}

void SwiperComponent::SetPreviousMargin(const Dimension& margin)
{
    declaration_->SetPreviousMargin(margin);
}

const Dimension& SwiperComponent::GetPreviousMargin() const
{
    return declaration_->GetPreviousMargin();
}

void SwiperComponent::SetNextMargin(const Dimension& margin)
{
    declaration_->SetNextMargin(margin);
}

const Dimension& SwiperComponent::GetNextMargin() const
{
    return declaration_->GetNextMargin();
}

} // namespace OHOS::Ace
