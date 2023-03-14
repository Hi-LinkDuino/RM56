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

#include "core/components/scroll/scroll_fade_effect.h"

namespace OHOS::Ace {

void ScrollFadeEffect::InitialEdgeEffect()
{
    ResetFadeEffect(fadeColor_);
}

void ScrollFadeEffect::ResetFadeEffect(const Color& color)
{
    fadeColor_ = color;
    if (!fadePainter_) {
        fadePainter_ = ScrollFadePainter::CreatePainter();
    }

    if (!fadeController_) {
        auto scroll = scroll_.Upgrade();
        fadeController_ = AceType::MakeRefPtr<ScrollFadeController>(scroll ? scroll->GetContext() : nullptr);
        fadeController_->SetCallback([weakFade = AceType::WeakClaim(this)](double opacity, double scale) {
            auto fade = weakFade.Upgrade();
            if (fade) {
                fade->SetOpacityAndScale(opacity, scale);
            }
        });
    }
}

double ScrollFadeEffect::CalculateOverScroll(double oldPosition, bool isReachMax)
{
    if (!currentPositionCallback_ || !leadingCallback_ || !trailingCallback_) {
        LOGD("position callback is null");
        return 0.0;
    }

    double newPosition = -currentPositionCallback_();
    double minExtent = -trailingCallback_();
    double maxExtent = -leadingCallback_();

    // under scroll
    if (newPosition < oldPosition && oldPosition <= minExtent) {
        return newPosition - oldPosition;
    }

    // over scroll
    if (newPosition > oldPosition && oldPosition >= maxExtent) {
        return newPosition - oldPosition;
    }

    // crash the top
    if (newPosition < minExtent && oldPosition > minExtent) {
        LOGD("crash the top");
        return newPosition;
    }

    // crash the bottom
    if (newPosition > maxExtent && maxExtent > oldPosition && isReachMax) {
        LOGD("crash the bottom");
        return newPosition - maxExtent;
    }

    return 0.0;
}

void ScrollFadeEffect::SetPaintDirection(Axis axis, double overScroll)
{
    if (NearZero(overScroll) || !fadePainter_) {
        return;
    }
    auto const isVertical = axis == Axis::VERTICAL;
    if (isVertical && overScroll < 0.0) {
        fadePainter_->SetDirection(OverScrollDirection::UP);
    } else if (isVertical && overScroll > 0.0) {
        fadePainter_->SetDirection(OverScrollDirection::DOWN);
    } else if (overScroll < 0.0) {
        fadePainter_->SetDirection(OverScrollDirection::LEFT);
    } else {
        fadePainter_->SetDirection(OverScrollDirection::RIGHT);
    }
}

void ScrollFadeEffect::Paint(
    RenderContext& context, const Size& viewPort, const Offset& offset)
{
    if (fadePainter_) {
        fadePainter_->SetColor(fadeColor_);
        fadePainter_->PaintSide(context, viewPort, offset);
    }
}

void ScrollFadeEffect::HandleOverScroll(Axis axis, double overScroll, const Size& viewPort)
{
    if (NearZero(overScroll)) {
        return;
    }

    SetPaintDirection(axis, overScroll);

    if (!fadeController_) {
        fadeController_ = AceType::MakeRefPtr<ScrollFadeController>(scroll_.Upgrade()->GetContext());
        fadeController_->SetCallback([weakFade = AceType::WeakClaim(this)](double opacity, double scale) {
            auto fade = weakFade.Upgrade();
            if (fade) {
                fade->SetOpacityAndScale(opacity, scale);
            }
        });
    }

    if (fadeController_ && scrollable_) {
        double dragVelocity = scrollable_->GetCurrentVelocity();
        if (!NearZero(dragVelocity)) {
            fadeController_->ProcessAbsorb(std::abs(dragVelocity));
        } else {
            axis == Axis::VERTICAL
            ? fadeController_->ProcessPull(std::abs(overScroll), viewPort.Height(), viewPort.Width())
            : fadeController_->ProcessPull(std::abs(overScroll), viewPort.Width(), viewPort.Height());
        }
    }
}

void ScrollFadeEffect::SetOpacityAndScale(double opacity, double scale)
{
    if (fadePainter_) {
        fadePainter_->SetOpacity(opacity);
        fadePainter_->SetScaleFactor(scale);

        auto scroll = scroll_.Upgrade();
        if (scroll) {
            scroll->MarkNeedRender();
        }
    }
}

} // namespace OHOS::Ace