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

#include "core/components/focus_animation/render_focus_animation.h"

#include "base/log/event_report.h"
#include "core/components/scroll/render_multi_child_scroll.h"
#include "core/components/scroll/render_single_child_scroll.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t TOTAL_DURATION = 3000;
constexpr float SEMICIRCLE = 180.0f;

} // namespace

void RenderFocusAnimation::Update(const RefPtr<Component>& component)
{
    if (animationParamsSet_) {
        return;
    }
    auto focusAnimation = AceType::DynamicCast<FocusAnimationComponent>(component);
    if (!focusAnimation) {
        LOGE("RenderFocusAnimation update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }

    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext update with nullptr");
        return;
    }
    auto theme = pipelineContext->GetThemeManager();
    if (!theme) {
        LOGE("calendar theme update with nullptr");
        return;
    }
    focusTheme_ = theme->GetTheme<FocusAnimationTheme>();
    layoutSize_.SetWidth(pipelineContext->GetRootWidth());
    layoutSize_.SetHeight(pipelineContext->GetRootHeight());
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        animator_ = AceType::MakeRefPtr<Animator>(pipelineContext);
        focusAnimation_ = AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, SEMICIRCLE, Curves::LINEAR);
        focusAnimation_->AddListener([weak = AceType::WeakClaim(this)](const float& value) {
            auto client = weak.Upgrade();
            if (client) {
                client->progress_ = value;
                client->MarkNeedRender(true);
            }
        });
        animator_->AddInterpolator(focusAnimation_);
        animator_->SetDuration(TOTAL_DURATION);
        animator_->SetIteration(ANIMATION_REPEAT_INFINITE);
    }

    isDisplay_ = false;
    animationParamsSet_ = true;
    isRoot_ = focusAnimation->IsRoot();
}

bool RenderFocusAnimation::MarkNeedRenderSpecial()
{
    auto parent = GetParent().Upgrade();
    auto renderScroll = AceType::DynamicCast<RenderScroll>(parent);
    if (renderScroll) {
        return true;
    }
    return false;
}

void RenderFocusAnimation::SetFocusAnimationProperties(const RRect& rrect, const Offset& offset)
{
    rrect_ = rrect;
    offset_ = offset - GetGlobalOffset();
    isNeedClip_ = false;
    if (focusTheme_) {
        pathColor_ = focusTheme_->GetColor();
    }
    animationParamsSet_ = true;
    if ((offset_.GetX() + rrect.Width()) > layoutSize_.Width() && (offset_.GetX() + rrect.Width()) > 0) {
        layoutSize_.SetWidth(offset_.GetX() + rrect.Width() + NormalizeToPx(blurMaskRadius_) * 2);
        MarkNeedLayout();
    }
    if ((offset_.GetY() + rrect.Height()) > layoutSize_.Height() && (offset_.GetY() + rrect.Height()) > 0) {
        layoutSize_.SetHeight((offset_.GetY() + rrect.Height()) + NormalizeToPx(blurMaskRadius_) * 2);
        MarkNeedLayout();
    }
}

void RenderFocusAnimation::SetFocusAnimationProperties(
    const RRect& rrect, const Color& color, const Offset& offset, bool isIndented)
{
    SetFocusAnimationProperties(rrect, offset);
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        pathColor_ = color;
        if (!isDisplay_) {
            isDisplay_ = true;
            animator_->Play();
        }
    } else if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
        if (isKeyEvent_) {
            // When the device is phone, only the key event will require paint.
            isDisplay_ = true;
            isIndented_ = isIndented;
            MarkNeedLayout();
        }
    }
}

void RenderFocusAnimation::SetFocusAnimationProperties(
    const RRect& rrect, const Color& color, const Offset& offset, const Rect& clipRect)
{
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        isNeedClip_ = true;
        clipRect_ = clipRect;
    }
    SetFocusAnimationProperties(rrect, color, offset, false);
}

void RenderFocusAnimation::CancelFocusAnimation()
{
    isDisplay_ = false;
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        animator_->Pause();
    }
    MarkNeedRender(true);
}

void RenderFocusAnimation::PerformLayout()
{
    SetLayoutSize(layoutSize_);
}

void RenderFocusAnimation::StopFocusAnimation()
{
    if (SystemProperties::GetDeviceType() != DeviceType::TV) {
        return;
    }
    animator_->Stop();
    MarkNeedRender(true);
}

void RenderFocusAnimation::StartFocusAnimation()
{
    isDisplay_ = true;
    if (SystemProperties::GetDeviceType() == DeviceType::PHONE) {
        MarkNeedRender();
    } else if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        animator_->Play();
    }
}

void RenderFocusAnimation::SetIsKeyEvent(bool isKeyEvent)
{
    isKeyEvent_ = isKeyEvent;
    MarkNeedRender();
}

} // namespace OHOS::Ace