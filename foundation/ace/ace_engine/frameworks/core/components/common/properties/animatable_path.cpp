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

#include "core/components/common/properties/animatable_path.h"

#include "include/core/SkPath.h"
#include "include/core/SkString.h"
#include "include/utils/SkParsePath.h"

#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

AnimatablePath& AnimatablePath::operator=(const AnimatablePath& newValue)
{
    SetAnimationOption(newValue.GetAnimationOption());
    auto context = context_.Upgrade();
    if (!context || !animationCallback_) {
        SetValue(newValue.GetValue());
        return *this;
    }
    AnimationOption explicitAnim;
    explicitAnim = context->GetExplicitAnimationOption();
    if (explicitAnim.IsValid()) {
        SetAnimationOption(explicitAnim);
        AnimateTo(newValue.GetValue());
    } else if (animationOption_.IsValid()) {
        AnimateTo(newValue.GetValue());
    } else {
        SetValue(newValue.GetValue());
    }
    isFirstAssign_ = false;
    return *this;
}

void AnimatablePath::AnimateTo(std::string endValue)
{
    if (isFirstAssign_) {
        isFirstAssign_ = false;
        SetValue(endValue);
        return;
    }
    if (path_ == endValue) {
        return;
    }
    pathFrom_ = FormatPathString(path_);
    pathTo_ = FormatPathString(endValue);
    if (pathFrom_ == pathTo_) {
        return;
    }

    ResetController();

    SkPath skPathFrom;
    SkPath skPathTo;
    SkParsePath::FromSVGString(pathFrom_.c_str(), &skPathFrom);
    SkParsePath::FromSVGString(pathTo_.c_str(), &skPathTo);
    if (!skPathTo.isInterpolatable(skPathFrom)) {
        isFirstAssign_ = false;
        SetValue(endValue);
        return;
    }

    if (!animationController_) {
        animationController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    RefPtr<CurveAnimation<double>> animation =
        AceType::MakeRefPtr<CurveAnimation<double>>(0.0, 1.0, animationOption_.GetCurve());
    animation->AddListener(std::bind(&AnimatablePath::OnAnimationCallback, this, std::placeholders::_1));

    animationController_->AddInterpolator(animation);
    auto onFinishEvent = animationOption_.GetOnFinishEvent();
    if (onFinishEvent) {
        animationController_->AddStopListener([onFinishEvent, weakContext = context_] {
            auto context = weakContext.Upgrade();
            if (context) {
                context->PostAsyncEvent(onFinishEvent);
            } else {
                LOGE("the context is null");
            }
        });
    }
    animationController_->SetDuration(animationOption_.GetDuration());
    animationController_->SetStartDelay(animationOption_.GetDelay());
    animationController_->SetIteration(animationOption_.GetIteration());
    animationController_->SetTempo(animationOption_.GetTempo());
    animationController_->SetAnimationDirection(animationOption_.GetAnimationDirection());
    animationController_->Play();
}

void AnimatablePath::ResetController()
{
    if (animationController_) {
        if (!animationController_->IsStopped()) {
            animationController_->Stop();
        }
        animationController_->ClearInterpolators();
        animationController_->ClearAllListeners();
        animationController_.Reset();
    }
}

void AnimatablePath::OnAnimationCallback(double value)
{
    SkPath skPathFrom;
    SkPath skPathTo;
    SkParsePath::FromSVGString(pathFrom_.c_str(), &skPathFrom);
    SkParsePath::FromSVGString(pathTo_.c_str(), &skPathTo);

    SkPath out;
    SkString outString;
    if (skPathTo.interpolate(skPathFrom, value, &out)) {
        SkParsePath::ToSVGString(out, &outString);
        SetValue(outString.c_str());
    } else {
        SetValue(pathTo_);
    }

    if (animationCallback_) {
        animationCallback_();
    }
}

// Format path string before judging whether interpolation can be done
// e.g. 'M0 20 L50 50 L50 100 Z' --> 'M0 20L50 50L50 100L0 20Z'
std::string AnimatablePath::FormatPathString(const std::string& path)
{
    SkPath skPath;
    SkString outString;
    SkParsePath::FromSVGString(path.c_str(), &skPath);
    SkParsePath::ToSVGString(skPath, &outString);
    return outString.c_str();
}

}