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

#include "core/components/data_panel/render_data_panel.h"

#include "core/animation/spring_animation.h"

namespace OHOS::Ace {
namespace {

constexpr double MILLISECOND_PER_PERCENT = 20.0;
constexpr double MAX_TRANSITION_TIME = 5000.0;
constexpr double MIN_TRANSITION_TIME = 200.0;
constexpr double SECOND_TO_MILLISECOND = 1000.0;

} // namespace

RenderDataPanel::RenderDataPanel() : RenderNode(true) {}

void RenderDataPanel::Update(const RefPtr<Component>& component)
{
    const RefPtr<DataPanelComponent> dataPanelComponent = AceType::DynamicCast<DataPanelComponent>(component);
    if (!dataPanelComponent) {
        return;
    }
    height_ = dataPanelComponent->GetDefaultHeight();
    width_ = dataPanelComponent->GetDefaultWidth();
    measureType_ = dataPanelComponent->GetMeasureType();
    type_ = dataPanelComponent->GetPanelType();
    thickness_ = dataPanelComponent->GetThickness();
    useEffect_ = dataPanelComponent->GetEffects();
    backgroundTrack_ = dataPanelComponent->GetTrackColor();
    autoScale_ = dataPanelComponent->GetAutoScale();
    userAnimationDuration_ = dataPanelComponent->GetAnimationDuration();

    if (animationInitialized_) {
        return;
    }

    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        return;
    }
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>(pipelineContext);
    }
    if (!progressTransitionController_) {
        progressTransitionController_ = AceType::MakeRefPtr<Animator>(pipelineContext);
    }
}

const Size RenderDataPanel::Measure()
{
    if (measureType_ == MeasureType::CONTENT) {
        return Size(NormalizeToPx(width_), NormalizeToPx(height_));
    } else {
        Size maxSize = GetLayoutParam().GetMaxSize();
        return Size(maxSize.Width(), maxSize.Height());
    }
}

void RenderDataPanel::PerformLayout()
{
    Size panelSize = Measure();
    SetLayoutSize(panelSize);
    if (type_ == ChartType::LOADING || (type_ == ChartType::RAINBOW && needReplayAnimation_)) {
        PlayAnimation();
        AnimationChanged();
    }
    needReplayAnimation_ = false;
}

void RenderDataPanel::OnVisibleChanged()
{
    AnimationChanged();
}

void RenderDataPanel::OnHiddenChanged(bool hidden)
{
    AnimationChanged();
}

void RenderDataPanel::AnimationChanged()
{
    if (type_ != ChartType::LOADING) {
        return;
    }
    if (GetVisible() && !GetHidden()) {
        if (animator_) {
            // when the dataPanel is visible, reset to user defined state.
            isUserSetPlay_ ? animator_->Play() : animator_->Pause();
        } else {
            LOGI("fail to start progress animation");
        }
    } else {
        if (animator_) {
            animator_->Pause();
        } else {
            LOGI("fail to stop progress animation");
        }
    }
}

void RenderProgressDataPanel::Update(const RefPtr<Component>& component)
{
    RenderDataPanel::Update(component);
    const RefPtr<ProgressDataPanelComponent> dataPanelComponent =
        AceType::DynamicCast<ProgressDataPanelComponent>(component);
    if (!dataPanelComponent) {
        return;
    }
    startColor_ = dataPanelComponent->GetStartColor();
    endColor_ = dataPanelComponent->GetEndColor();
    // if the type is changed, need to initialize the animator
    if (isLoading_ != (dataPanelComponent->GetPanelType() == ChartType::LOADING)) {
        animationInitialized_ = false;
    }
    isLoading_ = dataPanelComponent->GetPanelType() == ChartType::LOADING ? true : false;
    auto animationStartProgress = dataPanelComponent->GetProgressValue();
    auto needUpdateAnimation = false;
    if (!NearEqual(animationStartProgress, previousPercentValue_)) {
        animationDuring_ = std::chrono::steady_clock::now() - previousUpdateTime_;
        percentChange_ = animationStartProgress - previousPercentValue_;
        previousPercentValue_ = animationStartProgress;
        previousUpdateTime_ = std::chrono::steady_clock::now();
        needUpdateAnimation = true;
    }
    if (GreatNotEqual(progress_, 100.0)) {
        LOGI("chart value %{public}lf is larger than 100.0", progress_);
        progress_ = 100.0;
    }
    if (LessNotEqual(progress_, 0.0)) {
        LOGI("chart value %{public}lf is smaller than 0.0", progress_);
        progress_ = 0.0;
    }
    if (isLoading_ && !animationInitialized_) {
        animation_ = AceType::MakeRefPtr<CurveAnimation<double>>(0.0, 2.0, Curves::LINEAR);
        animation_->AddListener([this](const double& value) {
            rotateAngle_ = 360 * (1 - std::cos(M_PI * value / 2.0) + value / 2.0) / 3;
            if (value > 1.0) {
                sweepDegree_ = 90.0 * (2 - value);
            } else {
                sweepDegree_ = 90.0 * value;
            }
            MarkNeedRender();
        });
        PrepareAnimation();
        PlayAnimation();
        AnimationChanged();
        animationInitialized_ = true;
    } else if (!isLoading_ && needUpdateAnimation) {
        animation_ = AceType::MakeRefPtr<CurveAnimation<double>>(0.0, 1.5, Curves::LINEAR);
        animation_->AddListener([this](const double& value) {
            percent_ = value;
            if (percent_ > 1.0) {
                percent_ = 1.0;
            }
            MarkNeedRender();
        });

        transitionAnimation_ =
            AceType::MakeRefPtr<CurveAnimation<double>>(progress_, previousPercentValue_, Curves::EASE_OUT);
        transitionAnimation_->AddListener([this](const double& value) {
            progress_ = value;
            MarkNeedRender();
        });
        PrepareAnimation();
        StopAnimation();
        PlayAnimation();
        AnimationChanged();
        animationInitialized_ = true;
    }
}

void RenderProgressDataPanel::PrepareAnimation()
{
    if (animator_) {
        animator_->AddInterpolator(animation_);
        animator_->SetDuration(2000);
        animator_->SetIteration(ANIMATION_REPEAT_INFINITE);
    }

    if (progressTransitionController_) {
        double animationTime = 0.0;
        if (std::abs(percentChange_ * MILLISECOND_PER_PERCENT) > animationDuring_.count() * SECOND_TO_MILLISECOND) {
            animationTime = std::abs(percentChange_ * MILLISECOND_PER_PERCENT);
        } else {
            animationTime = animationDuring_.count() * SECOND_TO_MILLISECOND;
        }

        if (animationTime < MIN_TRANSITION_TIME || NearEqual(previousPercentValue_, 100.0)) {
            animationTime = MIN_TRANSITION_TIME;
        } else if (animationTime > MAX_TRANSITION_TIME) {
            animationTime = MAX_TRANSITION_TIME;
        }
        progressTransitionController_->AddInterpolator(transitionAnimation_);
        progressTransitionController_->SetDuration(animationTime);
        progressTransitionController_->SetIteration(1);
    }
}

void RenderPercentageDataPanel::PrepareAnimation()
{
    if (animator_) {
        if (GreatOrEqual(userAnimationDuration_, 0.0)) {
            animator_->SetDuration(userAnimationDuration_);
        } else {
            animator_->SetDuration(3000);
        }
        if (!useEffect_) {
            animator_->SetDuration(0.0);
        }
        animator_->SetIteration(1);
    }
}

void RenderPercentageDataPanel::Update(const RefPtr<Component>& component)
{
    RenderDataPanel::Update(component);
    const RefPtr<PercentageDataPanelComponent> dataPanelComponent =
        AceType::DynamicCast<PercentageDataPanelComponent>(component);
    if (!dataPanelComponent) {
        return;
    }
    startDegree_ = dataPanelComponent->GetStartDegree();
    sweepDegree_ = dataPanelComponent->GetSweepDegree();
    segments_ = dataPanelComponent->GetSegments();
    totalValue_ = dataPanelComponent->GetTotalValue();
    maxValue_ = dataPanelComponent->GetMaxValue();
    panelType_ = dataPanelComponent->GetPanelType();
    if (!animationInitialized_) {
        auto springProperty = AceType::MakeRefPtr<SpringProperty>(1.0f, 100.0f, 15.0f);
        auto springAnimation = AceType::MakeRefPtr<SpringAnimation>(springProperty);
        springAnimation->AddListener([this](const double& value) {
            animationPercent_ = value;
            MarkNeedRender();
        });
        animator_->AddInterpolator(springAnimation);
        animationInitialized_ = true;
    }
    PrepareAnimation();
    needReplayAnimation_ = true;
    MarkNeedLayout();
}

} // namespace OHOS::Ace