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

#include "core/components/list/interactive_effect.h"

#include "base/log/event_report.h"
#include "core/animation/keyframe.h"
#include "core/components/list/render_list_item.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t TOUCH_START_DELAY = 100; // Unit: milliseconds.

} // namespace

InteractiveEffect::InteractiveEffect(const WeakPtr<PipelineContext>& context)
{
    auto pipelineContext = context.Upgrade();
    if (pipelineContext) {
        context_ = pipelineContext;
        controller_ = AceType::MakeRefPtr<Animator>(pipelineContext);
    } else {
        LOGE("context is null.");
    }
}

void InteractiveEffect::UpdateContext(const WeakPtr<PipelineContext>& context)
{
    auto pipelineContext = context.Upgrade();
    if (pipelineContext) {
        context_ = pipelineContext;
        controller_ = AceType::MakeRefPtr<Animator>(pipelineContext);
    }
}

void InteractiveEffect::Initialize(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return;
    }
    if (!theme_) {
        theme_ = themeManager->GetTheme<ListItemTheme>();
    }
}

void InteractiveEffect::FinishPreviousAnimation()
{
    if (controller_ && !controller_->IsStopped()) {
        controller_->Finish();
    }
}

void InteractiveEffect::ShowAnimation(ItemState state)
{
    FinishPreviousAnimation();
    switch (state) {
        case ItemState::FOCUS:
            HandleOnFocus();
            BuildStateAnimation();
            break;
        case ItemState::BLUR:
            HandleOnBlur();
            BuildStateAnimation();
            break;
        case ItemState::CLICK:
            HandleOnClick();
            BuildClickAnimation();
            break;
        default:
            LOGW("invalid state.");
            break;
    }
}

void InteractiveEffect::TouchDownAnimation()
{
    FinishPreviousAnimation();
    if (!theme_) {
        LOGE("theme is invalid, stop build animation");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }
    RefPtr<KeyframeAnimation<double>> alphaAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    CreateDoubleAnimation(alphaAnimation, alphaBegin_, alphaEnd_);
    StartTouchAnimation(controller_, alphaAnimation, TOUCH_START_DELAY);
}

void InteractiveEffect::TouchUpAnimation()
{
    FinishPreviousAnimation();
    if (!theme_) {
        LOGE("theme is invalid, stop build animation");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }
    RefPtr<KeyframeAnimation<double>> alphaAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    CreateDoubleAnimation(alphaAnimation, GetAlpha(), alphaBegin_);
    StartTouchAnimation(controller_, alphaAnimation);
}

void InteractiveEffect::CancelTouchAnimation()
{
    if (!theme_) {
        LOGE("theme is invalid, stop build animation");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }
    if (controller_ && !controller_->IsStopped()) {
        controller_->Stop();
    }
    double currentAlpha = GetAlpha();
    if (NearEqual(currentAlpha, alphaBegin_)) {
        return;
    }
    RefPtr<KeyframeAnimation<double>> alphaAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    CreateDoubleAnimation(alphaAnimation, currentAlpha, alphaBegin_);
    StartTouchAnimation(controller_, alphaAnimation);
}

void InteractiveEffect::StartTouchAnimation(RefPtr<Animator> controller,
    RefPtr<KeyframeAnimation<double>>& doubleAnimation, int32_t startDelay)
{
    if (!controller || !doubleAnimation) {
        return;
    }
    controller->ClearInterpolators();
    controller->SetStartDelay(startDelay);
    controller->AddInterpolator(doubleAnimation);
    controller->SetDuration(PRESS_ANIMATION_DURATION);
    controller->SetFillMode(FillMode::FORWARDS);
    controller->Play();
}

void InteractiveEffect::CreateDoubleAnimation(RefPtr<KeyframeAnimation<double>>& doubleAnimation, double beginValue,
    double endValue)
{
    if (!doubleAnimation) {
        return;
    }
    auto alphaFrameStart = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_ZERO_TIME, beginValue);
    auto alphaFrameEnd = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_END_TIME, endValue);
    doubleAnimation->SetCurve(Curves::SHARP);
    doubleAnimation->AddKeyframe(alphaFrameStart);
    doubleAnimation->AddKeyframe(alphaFrameEnd);
    doubleAnimation->AddListener([weakEffect = AceType::WeakClaim(this)](double value) {
        auto effect = weakEffect.Upgrade();
        if (effect) {
            effect->SetAlpha(value);
            effect->MarkItemRender();
        }
    });
}

bool InteractiveEffect::NeedClickAnimation()
{
    auto node = item_.Upgrade();
    RefPtr<RenderListItem> listItem;
    if (!node || !AceType::DynamicCast<RenderListItem>(node)) {
        return false;
    } else {
        listItem = AceType::DynamicCast<RenderListItem>(node);
        if (!listItem->GetSupportClick()) {
            return false;
        }
    }
    return true;
}

void InteractiveEffect::BuildClickAnimation()
{
    if (!theme_) {
        LOGE("theme is invalid, stop build animation");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }

    if (!NeedClickAnimation()) {
        return;
    }

    RefPtr<KeyframeAnimation<double>> alphaAnimation = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    BuildClickAlphaAnimation(alphaAnimation);
    if (controller_) {
        controller_->ClearInterpolators();
        controller_->AddInterpolator(alphaAnimation);
        controller_->SetDuration(clickDuration_);
        controller_->Play();
    }
}

void InteractiveEffect::MarkItemRender()
{
    auto item = item_.Upgrade();
    if (item) {
        item->MarkNeedRender();
        auto listItem = AceType::DynamicCast<RenderListItem>(item);
        if (listItem) {
            if (listItem->GetSticky()) {
                auto list = list_.Upgrade();
                if (list) {
                    list->MarkNeedRender();
                }
            }
        }
    }
}

void InteractiveEffect::BuildClickAlphaAnimation(const RefPtr<KeyframeAnimation<double>>& alphaAnimation)
{
    auto alphaFrameStart = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_ZERO_TIME, alphaBegin_);
    auto alphaFrameMid = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_HALF_TIME, alphaEnd_);
    auto alphaFrameEnd = AceType::MakeRefPtr<Keyframe<double>>(ANIMATION_END_TIME, alphaBegin_);
    alphaFrameMid->SetCurve(Curves::FRICTION);
    alphaFrameEnd->SetCurve(Curves::FRICTION);

    alphaAnimation->AddKeyframe(alphaFrameStart);
    alphaAnimation->AddKeyframe(alphaFrameMid);
    alphaAnimation->AddKeyframe(alphaFrameEnd);
    alphaAnimation->AddListener([weakEffect = AceType::WeakClaim(this)](double value) {
        auto effect = weakEffect.Upgrade();
        if (effect) {
            effect->SetAlpha(value);
            effect->MarkItemRender();
        }
    });
}

} // namespace OHOS::Ace
