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

#include "core/components/progress/render_loading_progress.h"

#include "base/log/event_report.h"
#include "base/log/log.h"
#include "core/animation/curve_animation.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/progress/progress_theme.h"

namespace OHOS::Ace {
namespace {

constexpr double LOOP_DEGREES = 360.0;
constexpr double TAIL_ALPHA_RATIO = 0.82;
constexpr int32_t LOOP_DURATION = 1200;
constexpr int32_t MOVE_DURATION = LOOP_DURATION / 12; // Comet move without tail.
constexpr int32_t TAIL_DURATION = LOOP_DURATION / 4;  // Comet move with tail getting longer.
constexpr double DRAG_ANGLE_BEGIN = -15.0;
constexpr double DRAG_ANGLE_RANGE = 30.0;
constexpr double RING_SCALE_BEGIN = 0.5;
constexpr double RING_SCALE_RANGE = 0.5;
constexpr double MOVE_START = DRAG_ANGLE_BEGIN + DRAG_ANGLE_RANGE;
constexpr double MOVE_END = MOVE_START + LOOP_DEGREES / LOOP_DURATION * MOVE_DURATION;
constexpr double TAIL_END = MOVE_END + LOOP_DEGREES / LOOP_DURATION * TAIL_DURATION;
constexpr double LOOP_END = TAIL_END + LOOP_DEGREES;
constexpr int32_t START_POINT = 0;
constexpr int32_t MIDDLE_POINT = 1;
constexpr int32_t END_POINT = 2;
constexpr double CENTER_POINT = 2.0;
const Dimension MODE_SMALL = 16.0_vp;
const Dimension MODE_MIDDLE = 40.0_vp;
const Dimension MODE_LARGE = 76.0_vp;
const Dimension MODE_COMET_RADIUS[] = { 3.0_vp, 3.0_vp, 2.2_vp };
const Dimension MODE_RING_WIDTH[] = { 2.8_vp, 1.9_vp, 1.2_vp };
const Dimension MODE_RING_BLUR_RADIUS[] = { 0.5_vp, 0.2_vp, 0.1_vp };
const Dimension MODE_RING_BG_WIDTH[] = { 3.0_vp, 3.0_vp, 2.0_vp };
const Dimension MODE_RING_BG_BLUR_RADIUS[] = { 2.0_vp, 2.0_vp, 2.0_vp };

} // namespace

RenderLoadingProgress::RenderLoadingProgress() : RenderNode(true) {}

void RenderLoadingProgress::Update(const RefPtr<Component>& component)
{
    auto loadingProgress = AceType::DynamicCast<LoadingProgressComponent>(component);
    if (!loadingProgress) {
        LOGE("Update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    progressColor_ = loadingProgress->GetProgressColor();
    moveRatio_ = loadingProgress->GetMoveRatio();
    cometTailLen_ = loadingProgress->GetCometTailLen();
    diameterDimension_ = loadingProgress->GetDiameter();
    ringRadiusDimension_ = loadingProgress->GetRingRadius();
    orbitRadiusDimension_ = loadingProgress->GetOrbitRadius();
    MarkNeedLayout();
}

void RenderLoadingProgress::UpdateRingAnimation()
{
    auto ringMove = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    ringMove->AddListener([weak = WeakClaim(this)](double value) {
        auto loading = weak.Upgrade();
        if (loading) {
            loading->ringOffset_.SetY(value * loading->scale_);
            loading->MarkNeedRender();
        }
    });
    double moveRange = ringRadius_ * moveRatio_ * 2.0;
    auto keyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);
    auto keyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.25f, -moveRange);
    auto keyframe3 = AceType::MakeRefPtr<Keyframe<float>>(0.75f, moveRange);
    auto keyframe4 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 0.0f);
    keyframe2->SetCurve(AceType::MakeRefPtr<CubicCurve>(0.0f, 0.0f, 0.67f, 1.0f));
    keyframe3->SetCurve(AceType::MakeRefPtr<CubicCurve>(0.33f, 0.0f, 0.67f, 1.0f));
    keyframe4->SetCurve(AceType::MakeRefPtr<CubicCurve>(0.33f, 0.0f, 1.0f, 1.0f));
    ringMove->AddKeyframe(keyframe1);
    ringMove->AddKeyframe(keyframe2);
    ringMove->AddKeyframe(keyframe3);
    ringMove->AddKeyframe(keyframe4);
    ringController_->ClearInterpolators();
    ringController_->AddInterpolator(ringMove);
    ringController_->SetIteration(ANIMATION_REPEAT_INFINITE);
    ringController_->SetDuration(LOOP_DURATION);
    ringController_->Play();
}

void RenderLoadingProgress::UpdateCometAnimation()
{
    auto cometMoveStart = AceType::MakeRefPtr<CurveAnimation<float>>(MOVE_START, MOVE_END,
        AceType::MakeRefPtr<CubicCurve>(0.6f, 0.2f, 1.0f, 1.0f));
    cometMoveStart->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto loading = weak.Upgrade();
        if (loading) {
            CometParam para;
            para.angular = value;
            loading->cometParams_.clear();
            loading->cometParams_.emplace_back(para);
            loading->UpdateCometParams();
        }
    });
    cometController_->ClearInterpolators();
    cometController_->AddInterpolator(cometMoveStart);
    cometController_->SetIteration(1);
    cometController_->SetDuration(MOVE_DURATION);
    cometController_->SetFillMode(FillMode::FORWARDS);
    cometController_->ClearStopListeners();
    moveStopId_ = cometController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto loading = weak.Upgrade();
        if (loading) {
            loading->DoCometTailAnimation();
        }
    });
    cometController_->Play();
}

void RenderLoadingProgress::DoCometTailAnimation()
{
    auto cometMoveTail = AceType::MakeRefPtr<CurveAnimation<float>>(0.0, cometTailLen_, Curves::LINEAR);
    cometMoveTail->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto loading = weak.Upgrade();
        if (loading) {
            loading->cometCurTail_ = value;
            if (loading->moveStopId_ != 0 && loading->cometController_) {
                loading->cometController_->RemoveStopListener(loading->moveStopId_);
                loading->moveStopId_ = 0;
            }
        }
    });
    auto cometMoveDegree = AceType::MakeRefPtr<CurveAnimation<float>>(MOVE_END, TAIL_END, Curves::LINEAR);
    cometMoveDegree->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto loading = weak.Upgrade();
        if (loading) {
            double step = loading->cometTailLen_ / loading->cometCount_;
            int32_t count = 0;
            while (count < loading->cometCount_) {
                double curStep = std::min(count * step, value);
                if (count < (int32_t)loading->cometParams_.size()) {
                    loading->cometParams_[count].angular = value - curStep;
                } else {
                    CometParam para;
                    para.angular = value - curStep;
                    loading->cometParams_.emplace_back(para);
                }
                if (count * step >= loading->cometCurTail_) {
                    break;
                }
                count++;
            }
            loading->UpdateCometParams();
        }
    });
    cometController_->ClearInterpolators();
    cometController_->AddInterpolator(cometMoveTail);
    cometController_->AddInterpolator(cometMoveDegree);
    cometController_->SetIteration(1);
    cometController_->SetDuration(TAIL_DURATION);
    cometController_->SetFillMode(FillMode::FORWARDS);
    tailStopId_ = cometController_->AddStopListener([weak = AceType::WeakClaim(this)]() {
        auto loading = weak.Upgrade();
        if (loading) {
            loading->DoCometLoopAnimation();
        }
    });
    cometController_->Play();
}

void RenderLoadingProgress::DoCometLoopAnimation()
{
    auto cometLoopDegree = AceType::MakeRefPtr<CurveAnimation<float>>(TAIL_END, LOOP_END, Curves::LINEAR);
    cometLoopDegree->AddListener([weak = AceType::WeakClaim(this)](double value) {
        auto loading = weak.Upgrade();
        if (loading) {
            int32_t count = 0;
            double step = loading->cometTailLen_ / loading->cometCount_;
            for (auto& para : loading->cometParams_) {
                para.angular = value - count * step;
                count++;
            }
            loading->UpdateCometParams();
            if (loading->tailStopId_ != 0 && loading->cometController_) {
                loading->cometController_->RemoveStopListener(loading->tailStopId_);
                loading->tailStopId_ = 0;
            }
        }
    });
    cometController_->ClearInterpolators();
    cometController_->AddInterpolator(cometLoopDegree);
    cometController_->SetIteration(ANIMATION_REPEAT_INFINITE);
    cometController_->SetDuration(LOOP_DURATION);
    cometController_->Play();
}

void RenderLoadingProgress::UpdateCometParams()
{
    if (cometParams_.empty()) {
        return;
    }
    int32_t count = 0;
    float alpha = 0.0f;
    for (auto& para : cometParams_) {
        if (count == 0) { // Update Head Comet Parameter.
            para.scale = GetCometScaleByDegree(para.angular);
            para.alpha = floor(UINT8_MAX * GetCometAlphaByDegree(cometParams_[0].angular));
        } else { // Update Tail Comets Parameter.
            para.scale = GetCometScaleByDegree(para.angular);
            para.alpha = floor(alpha);
        }
        alpha = para.alpha * TAIL_ALPHA_RATIO;
        count++;
    }
}

float RenderLoadingProgress::GetCometScaleByDegree(double degree)
{
    // Scale Curve::LINEAR Degrees(  0 - 180) --> Scale(100% -  65%)
    // Scale Curve::LINEAR Degrees(180 - 360) --> Scale( 65% - 100%)
    if (degree > 360.0) {
        degree = degree - 360.0;
    }
    if (degree >= 0.0 && degree <= 180.0) {
        return 1.0 - 0.35 * degree / 180.0;
    }
    if (degree >= 180.0 && degree <= 360.0) {
        return 0.65 + 0.35 * (degree - 180.0) / 180.0;
    }
    return 1.0f;
}

float RenderLoadingProgress::GetCometAlphaByDegree(double degree)
{
    // Alpha Curve::LINEAR Degrees(  0 -  15) --> Alpha(100% - 100%)
    // Alpha Curve::LINEAR Degrees( 15 - 180) --> Scale(100% -  20%)
    // Alpha Curve::LINEAR Degrees(180 - 345) --> Scale( 20% - 100%)
    // Alpha Curve::LINEAR Degrees(345 - 360) --> Scale(100% - 100%)
    if (degree > 360.0) {
        degree = degree - 360.0;
    }
    if (degree >= 15.0 && degree <= 180.0) {
        return 1.0 - 0.8 * (degree - 15.0) / (180.0 - 15.0);
    } else if (degree >= 180.0 && degree <= 345.0) {
        return 0.2 + 0.8 * (degree - 180.0) / (345.0 - 180.0);
    } else {
        return 1.0f;
    }
}

void RenderLoadingProgress::SetLoadingMode(int32_t mode)
{
    if (loadingMode_ == mode) {
        return;
    }

    LOGI("SetLoadingMode to %{public}d", mode);
    loadingMode_ = mode;
    MarkNeedLayout();
    if (loadingMode_ != MODE_DRAG) {
        return;
    }
    if (ringController_ && cometController_) {
        ringController_->ClearStopListeners();
        cometController_->ClearStopListeners();
        ringController_->Finish();
        cometController_->Finish();
        ringController_ = nullptr;
        cometController_ = nullptr;
        moveStopId_ = 0;
        tailStopId_ = 0;
    }
}

void RenderLoadingProgress::SetDragRange(double minDistance, double maxDistance)
{
    minDistance_ = minDistance;
    maxDistance_ = maxDistance;
}

void RenderLoadingProgress::SetDragDistance(double distance)
{
    distance = std::clamp(distance, minDistance_, maxDistance_);
    if (NearEqual(curDistance_, distance)) {
        return;
    }
    curDistance_ = distance;
    double percent = (curDistance_ - minDistance_) / (maxDistance_ - minDistance_);
    double scale = RING_SCALE_BEGIN + RING_SCALE_RANGE * percent;
    switch (loadingMode_) {
        case MODE_LOOP: {
            return;
        }
        case MODE_DRAG: {
            exitScale_ = 1.0;
            exitAlpha_ = 1.0;
            dragScale_ = scale;
            dragAlpha_ = percent;
            // Update Comet Para when drag distance changed.
            CometParam para;
            para.alpha = floor(UINT8_MAX * dragAlpha_);
            para.angular = DRAG_ANGLE_BEGIN + DRAG_ANGLE_RANGE * percent;
            if (para.angular < 0.0) {
                para.angular = para.angular + 360.0;
            }
            cometParams_.clear();
            cometParams_.emplace_back(para);
            break;
        }
        case MODE_EXIT: {
            dragScale_ = 1.0;
            dragAlpha_ = 1.0;
            exitScale_ = scale;
            exitAlpha_ = percent;
            break;
        }
        default: {
            LOGW("Unsupported loading mode:%{public}d.", loadingMode_);
            break;
        }
    }
    MarkNeedRender();
}

void RenderLoadingProgress::PerformLayout()
{
    // the diameter will be constrain by layout size.
    diameter_ = NormalizeToPx(diameterDimension_);
    ringRadius_ = NormalizeToPx(ringRadiusDimension_);
    orbitRadius_ = NormalizeToPx(orbitRadiusDimension_);
    Size layoutSize;
    if (!NearEqual(diameter_, 0.0)) {
        layoutSize = GetLayoutParam().Constrain(Size(diameter_, diameter_));
    } else {
        if (GetLayoutParam().GetMaxSize().IsInfinite()) {
            double defaultDiameter = 0.0;
            auto theme = GetTheme<ProgressTheme>();
            if (theme) {
                defaultDiameter = NormalizeToPx(theme->GetLoadingDiameter());
            }
            layoutSize = Size(defaultDiameter, defaultDiameter);
        } else {
            layoutSize = GetLayoutParam().GetMaxSize();
        }
    }
    SetLayoutSize(layoutSize);
    UpdateLoadingSize(std::min(layoutSize.Width(), layoutSize.Height()));
    center_ = Offset(layoutSize.Width() / CENTER_POINT, layoutSize.Height() / CENTER_POINT);
    scale_ = std::min(layoutSize.Width() / (orbitRadius_ + cometRadius_) / CENTER_POINT,
        layoutSize.Height() / ringRadius_ / CENTER_POINT);
    auto pipelineContext = GetContext().Upgrade();
    if (pipelineContext && loadingMode_ != MODE_DRAG && !ringController_ && !cometController_) {
        ringController_ = AceType::MakeRefPtr<Animator>(pipelineContext);
        cometController_ = AceType::MakeRefPtr<Animator>(pipelineContext);
        UpdateRingAnimation();
        UpdateCometAnimation();
        AnimationChanged();
    }
}

void RenderLoadingProgress::UpdateLoadingSize(double diameter)
{
    if (diameter <= NormalizeToPx(MODE_SMALL)) {
        CalculateValue(START_POINT, START_POINT);
    } else if (diameter <= NormalizeToPx(MODE_MIDDLE)) {
        CalculateValue(START_POINT, MIDDLE_POINT,
            (diameter - NormalizeToPx(MODE_SMALL)) / (NormalizeToPx(MODE_MIDDLE) - NormalizeToPx(MODE_SMALL)));
    } else if (diameter <= NormalizeToPx(MODE_LARGE)) {
        CalculateValue(MIDDLE_POINT, END_POINT,
            (diameter - NormalizeToPx(MODE_MIDDLE)) / (NormalizeToPx(MODE_LARGE) - NormalizeToPx(MODE_MIDDLE)));
    } else {
        CalculateValue(END_POINT, END_POINT);
    }
}

void RenderLoadingProgress::CalculateValue(int32_t start, int32_t end, double percent)
{
    if (start == end) {
        ringWidth_ = NormalizeToPx(MODE_RING_WIDTH[start]);
        cometRadius_ = NormalizeToPx(MODE_COMET_RADIUS[start]);
        ringBlurRadius_ = NormalizeToPx(MODE_RING_BLUR_RADIUS[start]);
        ringBgWidth_ = NormalizeToPx(MODE_RING_BG_WIDTH[start]);
        ringBgBlurRadius_ = NormalizeToPx(MODE_RING_BG_BLUR_RADIUS[start]);
    } else {
        ringWidth_ = NormalizeToPx(MODE_RING_WIDTH[start] +
            (MODE_RING_WIDTH[end] - MODE_RING_WIDTH[start]) * percent);
        cometRadius_ = NormalizeToPx(MODE_COMET_RADIUS[start] +
            (MODE_COMET_RADIUS[end] - MODE_COMET_RADIUS[start]) * percent);
        ringBlurRadius_ = NormalizeToPx(MODE_RING_BLUR_RADIUS[start] +
            (MODE_RING_BLUR_RADIUS[end] - MODE_RING_BLUR_RADIUS[start]) * percent);
        ringBgWidth_ = NormalizeToPx(MODE_RING_BG_WIDTH[start] +
            (MODE_RING_BG_WIDTH[end] - MODE_RING_BG_WIDTH[start]) * percent);
        ringBgBlurRadius_ = NormalizeToPx(MODE_RING_BG_BLUR_RADIUS[start] +
            (MODE_RING_BG_BLUR_RADIUS[end] - MODE_RING_BG_BLUR_RADIUS[start]) * percent);
    }
}

void RenderLoadingProgress::OnVisibleChanged()
{
    AnimationChanged();
}

void RenderLoadingProgress::OnHiddenChanged(bool hidden)
{
    AnimationChanged();
}

void RenderLoadingProgress::AnimationChanged()
{
    if (GetVisible() && !GetHidden()) {
        if (ringController_) {
            ringController_->Play();
        }
        if (cometController_) {
            cometController_->Play();
        }
    } else {
        if (ringController_) {
            ringController_->Pause();
        }
        if (cometController_) {
            cometController_->Pause();
        }
    }
}

} // namespace OHOS::Ace
