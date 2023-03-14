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

#include "core/components/progress/render_bubble_progress.h"

#include "base/log/event_report.h"
#include "core/components/progress/progress_theme.h"

namespace OHOS::Ace {
namespace {

constexpr double SIN_45 = 0.707;
constexpr double COS_45 = 0.707;
constexpr Color DARK_COLOR = Color(0xFF333333);
constexpr Color LIGHT_COLOR = Color(0xFF8A8A8A);

} // namespace

RenderBubbleProgress::RenderBubbleProgress() : RenderNode(true) {}

void RenderBubbleProgress::OnPostFlush()
{
    static const int32_t totalTime = 8;
    static const int32_t totalDuration = 4800;
    RefPtr<CurveAnimation<double>> animation =
        AceType::MakeRefPtr<CurveAnimation<double>>(0, totalTime, Curves::LINEAR);
    animation->AddListener([weak = AceType::WeakClaim(this)](float progress) {
        auto bubble = weak.Upgrade();
        if (!bubble) {
            return;
        }
        bubble->step_ = static_cast<int32_t>(progress);
        float fraction = progress - bubble->step_;
        auto evaluatorDarkToLight = AceType::MakeRefPtr<LinearEvaluator<Color>>();
        bubble->lightToDark_ = evaluatorDarkToLight->Evaluate(Color(LIGHT_COLOR), Color(DARK_COLOR), fraction);
        bubble->darkToLight_ = evaluatorDarkToLight->Evaluate(Color(DARK_COLOR), Color(LIGHT_COLOR), fraction);
        bubble->MarkNeedRender();
    });
    animatorController_->AddInterpolator(animation);
    animatorController_->SetDuration(totalDuration);
    animatorController_->SetIteration(ANIMATION_REPEAT_INFINITE);
    animatorController_->Play();
    simulationPrepared_ = true;
}

void RenderBubbleProgress::Update(const RefPtr<Component>& component)
{
    if (!animatorController_) {
        auto context = GetContext();
        animatorController_ = AceType::MakeRefPtr<Animator>(context);
        auto pipelineContext = context.Upgrade();
        if (!pipelineContext) {
            LOGE("context is nullptr");
            return;
        }
        pipelineContext->AddPostFlushListener(AceType::Claim(this));
    }

    auto bubbleProgress = AceType::DynamicCast<BubbleProgressComponent>(component);
    if (!bubbleProgress) {
        LOGE("Update with nullptr");
        return;
    }
    diameter_ = NormalizeToPx(bubbleProgress->GetDiameter());
    maxCircleRadius_ = NormalizeToPx(bubbleProgress->GetBubbleRadius());
    MarkNeedLayout();
}

void RenderBubbleProgress::PerformLayout()
{
    // the diameter will be constrain by layout size.
    Size layoutSize;
    auto theme = GetTheme<ProgressTheme>();
    if (!NearEqual(diameter_, 0.0)) {
        layoutSize = GetLayoutParam().Constrain(Size(diameter_, diameter_));
    } else {
        if (GetLayoutParam().GetMaxSize().IsInfinite()) {
            double defaultDiameter = NormalizeToPx(theme->GetBubbleDiameter());
            layoutSize = Size(defaultDiameter, defaultDiameter);
        } else {
            layoutSize = GetLayoutParam().GetMaxSize();
        }
    }

    // radius is half of the height or width
    radius_ = std::min(layoutSize.Height(), layoutSize.Width()) / 2.0;
    center_ = Vertex(layoutSize.Width() / 2.0, layoutSize.Height() / 2.0);

    // based on law of sines, when the r1 : r2 > 4.83 : 1, the circle will never be overlapped.
    maxCircleRadius_ = NearEqual(maxCircleRadius_, 0.0) ? NormalizeToPx(theme->GetBubbleRadius()) : maxCircleRadius_;
    SetLayoutSize(layoutSize);
    CalculateCirclePosition();
}

void RenderBubbleProgress::CalculateCirclePosition()
{
    double radius = radius_ - maxCircleRadius_;
    subCircleCenter_.clear();
    // Add bottom right points
    subCircleCenter_.push_back(center_ + Vertex(0.0, radius));
    subCircleCenter_.push_back(center_ + Vertex(SIN_45 * radius, COS_45 * radius));
    subCircleCenter_.push_back(center_ + Vertex(radius, 0.0));

    // Add top right points
    subCircleCenter_.push_back(center_ + Vertex(COS_45 * radius, -1 * SIN_45 * radius));
    subCircleCenter_.push_back(center_ + Vertex(0.0, -1 * radius));

    // Add top left points
    subCircleCenter_.push_back(center_ + Vertex(-1 * SIN_45 * radius, -1 * COS_45 * radius));
    subCircleCenter_.push_back(center_ + Vertex(-1 * radius, 0.0));

    // Add bottom left points
    subCircleCenter_.push_back(center_ + Vertex(-1 * SIN_45 * radius, COS_45 * radius));
}

void RenderBubbleProgress::OnVisibleChanged()
{
    AnimationChanged();
}

void RenderBubbleProgress::OnHiddenChanged(bool hidden)
{
    AnimationChanged();
}

void RenderBubbleProgress::AnimationChanged()
{
    if (!simulationPrepared_) {
        return;
    }
    if (GetVisible() && !GetHidden()) {
        if (animatorController_) {
            animatorController_->Play();
        } else {
            LOGI("fail to start progress animation");
        }
    } else {
        if (animatorController_) {
            animatorController_->Pause();
        } else {
            LOGI("fail to stop progress animation");
        }
    }
}

void RenderBubbleProgress::OnAppShow()
{
    RenderNode::OnAppShow();
    AnimationChanged();
}

void RenderBubbleProgress::OnAppHide()
{
    RenderNode::OnAppHide();
    AnimationChanged();
}

}
