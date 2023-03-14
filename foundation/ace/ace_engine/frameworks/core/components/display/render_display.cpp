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

#include "core/components/display/render_display.h"

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "core/animation/curve_animation.h"

namespace OHOS::Ace {

void RenderDisplay::Update(const RefPtr<Component>& component)
{
    const RefPtr<DisplayComponent> display = AceType::DynamicCast<DisplayComponent>(component);
    if (!display) {
        LOGE("RenderDisplay update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    displayComponent_ = display;
    bool needAppearingCard = false;
    if ((visible_ == VisibleType::GONE) && (display->GetVisible() != VisibleType::GONE)) {
        needAppearingCard = true;
    }
    UpdateVisibleType(display->GetVisible());
    duration_ = display->GetAppearingDuration();
    transitionOpacity_ = display->GetOpacity();
    disappearingOpacity_ = display->GetDisappearingOpacity();
    appearingOpacity_ = display->GetAppearingOpacity();
    hasDisappearTransition_ = display->HasDisappearTransition();
    hasAppearTransition_ = display->HasAppearTransition();
    if (!hasDisappearTransition_) {
        pendingAppearing_ = false;
    }
    if (!pendingAppearing_) {
        animatableOpacity_ = AnimatableDouble(display->GetOpacity(), display->GetOpacityAnimationOption());
    }
    double opacity = std::min(std::max(animatableOpacity_.GetValue(), 0.0), 1.0);
    if (disableLayer_ != display->IsDisableLayer()) {
        // recover opacity in child
        UpdateOpacity(UINT8_MAX);
        disableLayer_ = display->IsDisableLayer();
    }
    opacity_ = static_cast<uint8_t>(round(opacity * UINT8_MAX));
    auto limitDuration = GetCardAppearingDuration();
    if (duration_ > 0 && limitDuration > 0 && needAppearingCard) {
        CreateAppearingAnimation(opacity_, limitDuration);
        opacity_ = 0;
        animator_->Play();
    }

    if (display->HasStateAttributes()) {
        stateAttributeList_ = display->GetStateAttributes();
    }
    OnStatusStyleChanged(VisualState::NORMAL);
    SetShadow(display->GetShadow());
    MarkNeedLayout();
}

void RenderDisplay::PerformLayout()
{
    if (pendingAppearing_) {
        if (animatableOpacity_.GetAnimationStatus() != Animator::Status::RUNNING) {
            animatableOpacity_.MoveTo(appearingOpacity_);
        }
        // appearing transition do not need stop callback anymore.
        animatableOpacity_.SetAnimationStopCallback(nullptr);
        animatableOpacity_ = AnimatableDouble(opacity_ / (1.0 * UINT8_MAX));
        pendingAppearing_ = false;
    }
    LayoutParam layoutParam = GetLayoutParam();
    if (visible_ == VisibleType::GONE) {
        layoutParam.SetMinSize(Size());
        layoutParam.SetMaxSize(Size());
        SetVisible(false);
    }
    Size childSize;
    if (!GetChildren().empty()) {
        const auto& child = GetChildren().front();
        child->Layout(layoutParam);
        childSize = child->GetLayoutSize();
        child->SetPosition(Offset());
    }
    SetLayoutSize(childSize);
}

void RenderDisplay::GetOpacityCallbacks()
{
    opacityCallbacks_.clear();
    if (!disableLayer_) {
        return;
    }
    GetDomOpacityCallbacks(GetNodeId(), opacityCallbacks_);
}

void RenderDisplay::Dump()
{
    DumpLog::GetInstance().AddDesc(
        std::string("Display: ")
            .append(visible_ == VisibleType::VISIBLE ? "visible"
                                                        : visible_ == VisibleType::INVISIBLE ? "invisible" : "gone"));
    DumpLog::GetInstance().AddDesc(std::string("Opacity: ").append(std::to_string(animatableOpacity_.GetValue())));
}

bool RenderDisplay::GetVisible() const
{
    return RenderNode::GetVisible() && (visible_ == VisibleType::VISIBLE);
}

void RenderDisplay::OnOpacityDisappearingCallback()
{
    LOGD("OnOpacityDisappearingCallback");
    RefPtr<RenderNode> child = AceType::Claim(this);
    while (child && !child->IsDisappearing()) {
        child = child->GetParent().Upgrade();
    }
    if (!child) {
        return;
    }
    auto parent = child->GetParent().Upgrade();
    if (parent) {
        parent->ClearDisappearingNode(child);
    }
}

void RenderDisplay::OnOpacityAnimationCallback()
{
    double value = animatableOpacity_.GetValue();
    opacity_ = static_cast<uint8_t>(round(value * UINT8_MAX));
    MarkNeedRender();
    if (disableLayer_) {
        UpdateOpacity(opacity_);
    }
}

bool RenderDisplay::HasDisappearingTransition(int32_t nodeId)
{
    return hasDisappearTransition_ || RenderNode::HasDisappearingTransition(nodeId);
}

void RenderDisplay::OnTransition(TransitionType type, int32_t id)
{
    LOGD("OnTransition. type: %{public}d, id: %{public}d", type, id);
    if (type == TransitionType::APPEARING) {
        pendingAppearing_ = true;
        MarkNeedLayout();
    } else if (type == TransitionType::DISAPPEARING) {
        animatableOpacity_.SetAnimationStopCallback(std::bind(&RenderDisplay::OnOpacityDisappearingCallback, this));
        animatableOpacity_ = AnimatableDouble(disappearingOpacity_);
    }
}

void RenderDisplay::ClearRenderObject()
{
    RenderNode::ClearRenderObject();
    animatableOpacity_ = 1.0;
    appearingOpacity_ = 0.0;
    disappearingOpacity_ = 0.0;
}

int32_t RenderDisplay::GetCardAppearingDuration()
{
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        LOGE("pipeline context is nullptr");
        return 0;
    }
    auto limitDuration = pipeline->GetAppearingDuration();
    return limitDuration;
}

void RenderDisplay::ResetAppearingAnimation()
{
    if (animator_->GetStatus() != Animator::Status::STOPPED) {
        animator_->Stop();
    }
    animator_->ClearInterpolators();
}

void RenderDisplay::CreateAppearingAnimation(uint8_t opacity, int32_t limit)
{
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>(context_);
    }
    ResetAppearingAnimation();
    if (!appearingAnimation_) {
        appearingAnimation_ = AceType::MakeRefPtr<CurveAnimation<uint8_t>>(0, opacity, Curves::FRICTION);
    }
    appearingAnimation_->AddListener([weak = WeakClaim(this)](const uint8_t value) {
        auto display = weak.Upgrade();
        if (display) {
            display->UpdateOpacity(value);
        }
    });
    animator_->AddInterpolator(appearingAnimation_);
    animator_->SetDuration(std::min(duration_, limit));
}

void RenderDisplay::OnStatusStyleChanged(VisualState componentState)
{
    RenderNode::OnStatusStyleChanged(componentState);
    if (stateAttributeList_ == nullptr) {
        return;
    }

    bool updated = false;
    for (auto& attribute : stateAttributeList_->GetAttributesForState(componentState)) {
        updated = true;
        switch (attribute->id_) {
            case DisplayStateAttribute::OPACITY: {
                auto opacityState =
                    AceType::DynamicCast<StateAttributeValue<DisplayStateAttribute, AnimatableDouble>>(attribute);
                if (!pendingAppearing_) {
                    animatableOpacity_ = opacityState->value_;
                }
                opacity_ = static_cast<uint8_t>(round(animatableOpacity_.GetValue() * UINT8_MAX));
            } break;
            default:
                break;
        }
    }
    if (updated) {
        MarkNeedLayout();
    }
}

} // namespace OHOS::Ace
