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

#include "core/components/common/properties/page_transition_option.h"

#include "base/utils/device_type.h"
#include "base/utils/system_properties.h"
#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/animation/property_animatable.h"

namespace OHOS::Ace {
namespace {

constexpr float TRANSITION_FACTOR = 0.8f;
constexpr float TRANSITION_FACTOR_IN = 0.5f;
constexpr float TRANSITION_FACTOR_OUT = 0.2f;
constexpr float BLACK_OPACITY = 0.3f;
constexpr float SHARED_TRANSITION_ENTER_START_OFFSET_X = 68.0f;

void CreateSharedInOption(bool isRightToLeft, double deviceViewScale, double deviceWidth, TweenOption& option)
{
    // opacity. parameters only have local meaning, no need to define const variables.
    auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);   // start with invisible
    auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.428f, 0.0f); // invisible at 42.8% of total duration
    opacityKeyframe2->SetCurve(Curves::LINEAR);
    auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f); // finally visible
    opacityKeyframe3->SetCurve(Curves::FRICTION);
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimation->AddKeyframe(opacityKeyframe1);
    opacityAnimation->AddKeyframe(opacityKeyframe2);
    opacityAnimation->AddKeyframe(opacityKeyframe3);
    option.SetOpacityAnimation(opacityAnimation);
    // translate. parameters only have local meaning, no need to define const variables.
    double startX = SHARED_TRANSITION_ENTER_START_OFFSET_X / deviceViewScale - deviceWidth;
    double endX = 0.0;
    if (isRightToLeft) {
        startX = -1.0 * startX;
        endX = 0.0;
    }
    auto translateXKeyframe1 =
        AceType::MakeRefPtr<Keyframe<DimensionOffset>>(0.0f, Offset(startX, 0)); // start with startX
    auto translateXKeyframe2 =
        AceType::MakeRefPtr<Keyframe<DimensionOffset>>(0.428f, Offset(startX, 0)); // start at 42.8% of total duration
    translateXKeyframe2->SetCurve(Curves::LINEAR);
    auto translateXKeyframe3 = AceType::MakeRefPtr<Keyframe<DimensionOffset>>(1.0f, Offset(endX, 0)); // finally at endX
    translateXKeyframe3->SetCurve(Curves::FRICTION);
    auto translateXAnimation = AceType::MakeRefPtr<KeyframeAnimation<DimensionOffset>>();
    translateXAnimation->AddKeyframe(translateXKeyframe1);
    translateXAnimation->AddKeyframe(translateXKeyframe2);
    translateXAnimation->AddKeyframe(translateXKeyframe3);
    option.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateXAnimation);
}

void CreateSharedOutOption(TweenOption& option)
{
    // opacity. parameters only have local meaning, no need to define const variables.
    auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 1.0f);   // start with invisible
    auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.714f, 0.0f); // invisible at 71.4% of total duration
    opacityKeyframe2->SetCurve(Curves::FRICTION);
    auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 0.0f); // finally visible
    opacityKeyframe3->SetCurve(Curves::LINEAR);
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimation->AddKeyframe(opacityKeyframe1);
    opacityAnimation->AddKeyframe(opacityKeyframe2);
    opacityAnimation->AddKeyframe(opacityKeyframe3);
    option.SetOpacityAnimation(opacityAnimation);
}

// Follow Watch's UI Standard.
void CreateSharedStaticOption(TweenOption& option)
{
    // opacity. parameters only have local meaning, no need to define const variables.
    auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);   // start with invisible
    auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.428f, 0.0f); // invisible at 42.8% of total duration
    opacityKeyframe2->SetCurve(Curves::LINEAR);
    auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f); // finally visible
    opacityKeyframe3->SetCurve(Curves::FRICTION);
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimation->AddKeyframe(opacityKeyframe1);
    opacityAnimation->AddKeyframe(opacityKeyframe2);
    opacityAnimation->AddKeyframe(opacityKeyframe3);
    option.SetOpacityAnimation(opacityAnimation);
}

} // namespace

constexpr int32_t TRANSITION_PHONE_DURATION = 300; // 0.3s
constexpr int32_t TRANSITION_WATCH_DURATION = 300; // 0.3s
constexpr int32_t TRANSITION_TV_DURATION = 1000;   // 1s
constexpr double TRANSITION_DEFAULT_WIDTH = 750.0;
constexpr double TRANSITION_DEFAULT_HEIGHT = 1900.0;
constexpr double TRANSITION_DEFAULT_CORNER_RADIUS = 0.0;

TransitionTweenOption::TransitionTweenOption(bool isRightToLeft, const WeakPtr<PipelineContext>& context)
{
    auto pipelineContext = context.Upgrade();
    if (pipelineContext) {
        auto deviceWidth = pipelineContext->GetStageRect().Width();
        deviceWidth_ = deviceWidth > 0.0 ? deviceWidth : deviceWidth_;
        auto deviceHeight = pipelineContext->GetStageRect().Height();
        deviceHeight_ = deviceHeight > 0.0 ? deviceHeight : deviceHeight_;
        auto viewScale = pipelineContext->GetViewScale();
        deviceViewScale_ = viewScale > 0.0 ? viewScale : deviceViewScale_;
        isRightToLeft_ = isRightToLeft;
        windowModal_ = pipelineContext->GetWindowModal();
    }
}

const TweenOption& TransitionTweenOption::GetTransitionContentInOption() const
{
    return contentInOption_;
}

const TweenOption& TransitionTweenOption::GetTransitionContentOutOption() const
{
    return contentOutOption_;
}

const TweenOption& TransitionTweenOption::GetTransitionBackgroundInOption() const
{
    return backgroundInOption_;
}

const TweenOption& TransitionTweenOption::GetTransitionBackgroundOutOption() const
{
    return backgroundOutOption_;
}

const TweenOption& TransitionTweenOption::GetTransitionFrontDecorationOption() const
{
    return frontDecorationOption_;
}

const TweenOption& TransitionTweenOption::GetSharedTransitionFrontDecorationOption() const
{
    return sharedFrontDecorationOption_;
}

const TweenOption& TransitionTweenOption::GetSharedInOption() const
{
    return sharedTransitionInOption_;
}

const TweenOption& TransitionTweenOption::GetSharedOutOption() const
{
    return sharedTransitionOutOption_;
}

TransitionTvTweenOption::TransitionTvTweenOption(bool isRightToLeft, const WeakPtr<PipelineContext>& context)
    : TransitionTweenOption(isRightToLeft, context)
{
    CreateTransitionInOption();
    CreateTransitionOutOption();
    // Currently, Tv Do not have UI standard, no need to create special shared transition.
}

void TransitionTvTweenOption::CreateTransitionInOption()
{
    CreateTransitionInContentOption();
    CreateTransitionInBackgroundOption();
}

void TransitionTvTweenOption::CreateTransitionInContentOption()
{
    // content opacity. parameters only have local meaning, no need to define const variables.
    auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);  // start with invisible
    auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.55f, 0.0f); // invisible at 55% of total duration
    opacityKeyframe2->SetCurve(Curves::FRICTION);
    auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f); // finally visible
    opacityKeyframe3->SetCurve(Curves::FRICTION);
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimation->AddKeyframe(opacityKeyframe1);
    opacityAnimation->AddKeyframe(opacityKeyframe2);
    opacityAnimation->AddKeyframe(opacityKeyframe3);
    contentInOption_.SetOpacityAnimation(opacityAnimation);

    // content scale. parameters only have local meaning, no need to define const variables.
    auto scaleKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);  // start with invisible
    auto scaleKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.55f, 0.9f); // 90% scale at 55% of total duration
    scaleKeyframe2->SetCurve(Curves::FRICTION);
    auto scaleKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f); // finally 100% scale
    scaleKeyframe3->SetCurve(Curves::FRICTION);
    auto scaleAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    scaleAnimation->AddKeyframe(scaleKeyframe1);
    scaleAnimation->AddKeyframe(scaleKeyframe2);
    scaleAnimation->AddKeyframe(scaleKeyframe3);
    contentInOption_.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimation);
    contentInOption_.SetDuration(TRANSITION_TV_DURATION);
}

void TransitionTvTweenOption::CreateTransitionInBackgroundOption()
{
    // background opacity. parameters only have local meaning, no need to define const variables.
    auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f); // start with invisible
    auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.5f, 0.0f); // invisible at 50% of total duration
    opacityKeyframe2->SetCurve(Curves::FRICTION);
    auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(0.95f, 1.0f); // visible at 95% of total duration
    opacityKeyframe3->SetCurve(Curves::FRICTION);
    auto opacityKeyframe4 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f); // finally visible
    opacityKeyframe4->SetCurve(Curves::FRICTION);
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimation->AddKeyframe(opacityKeyframe1);
    opacityAnimation->AddKeyframe(opacityKeyframe2);
    opacityAnimation->AddKeyframe(opacityKeyframe3);
    opacityAnimation->AddKeyframe(opacityKeyframe4);
    backgroundInOption_.SetOpacityAnimation(opacityAnimation);

    // background scale. parameters only have local meaning, no need to define const variables.
    auto scaleKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f); // start with invisible
    auto scaleKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.5f, 1.1f); // 110% scale at 50% of total duration
    scaleKeyframe2->SetCurve(Curves::FRICTION);
    auto scaleKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(0.95f, 1.0f); // 100% scale at 95% of total duration
    scaleKeyframe3->SetCurve(Curves::FRICTION);
    auto scaleKeyframe4 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f); // finally 100% scale
    scaleKeyframe4->SetCurve(Curves::FRICTION);
    auto scaleAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    scaleAnimation->AddKeyframe(scaleKeyframe1);
    scaleAnimation->AddKeyframe(scaleKeyframe2);
    scaleAnimation->AddKeyframe(scaleKeyframe3);
    scaleAnimation->AddKeyframe(scaleKeyframe4);
    backgroundInOption_.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimation);
}

void TransitionTvTweenOption::CreateTransitionOutOption()
{
    CreateTransitionOutContentOption();
    CreateTransitionOutBackgroundOption();
}

void TransitionTvTweenOption::CreateTransitionOutContentOption()
{
    CreatTransitionOutOption(contentOutOption_);
}

void TransitionTvTweenOption::CreateTransitionOutBackgroundOption()
{
    CreatTransitionOutOption(backgroundOutOption_);
}

void TransitionTvTweenOption::CreatTransitionOutOption(TweenOption& option)
{
    // out opacity. parameters only have local meaning, no need to define const variables.
    auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 1.0f); // start with visible
    auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.1f, 1.0f); // visible at 10% of total duration
    opacityKeyframe2->SetCurve(Curves::FRICTION);
    auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(0.55f, 0.0f); // invisible at 55% of total duration
    opacityKeyframe3->SetCurve(Curves::FRICTION);
    auto opacityKeyframe4 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 0.0f); // finally invisible
    opacityKeyframe4->SetCurve(Curves::FRICTION);
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    opacityAnimation->AddKeyframe(opacityKeyframe1);
    opacityAnimation->AddKeyframe(opacityKeyframe2);
    opacityAnimation->AddKeyframe(opacityKeyframe3);
    opacityAnimation->AddKeyframe(opacityKeyframe4);
    option.SetOpacityAnimation(opacityAnimation);

    // out scale. parameters only have local meaning, no need to define const variables.
    auto scaleKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 1.0f); // 100% scale at start
    auto scaleKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.1f, 1.0f); // 100% scale at 10% of total duration
    scaleKeyframe2->SetCurve(Curves::FRICTION);
    auto scaleKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(0.55f, 1.1f); // 110% scale at 55% of total duration
    scaleKeyframe3->SetCurve(Curves::FRICTION);
    auto scaleKeyframe4 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f); // 100% scale at 100% of total duration
    scaleKeyframe4->SetCurve(Curves::FRICTION);
    auto scaleAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    scaleAnimation->AddKeyframe(scaleKeyframe1);
    scaleAnimation->AddKeyframe(scaleKeyframe2);
    scaleAnimation->AddKeyframe(scaleKeyframe3);
    scaleAnimation->AddKeyframe(scaleKeyframe4);
    option.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimation);
    option.SetDuration(TRANSITION_TV_DURATION);
}

RefPtr<TransitionTweenOption> TransitionTweenOptionFactory::CreateTransitionTweenOption(
    DeviceType deviceType, TransitionEvent event, bool isRightToLeft, const RRect& rrect,
    const WeakPtr<PipelineContext>& context)
{
    switch (deviceType) {
        case DeviceType::PHONE:
            if (rrect.GetRect().IsValid()) {
                return AceType::MakeRefPtr<TransitionPhoneTweenOption>(event, isRightToLeft, rrect, context);
            }
            return AceType::MakeRefPtr<TransitionPhoneTweenOption>(event, isRightToLeft, context);
        case DeviceType::TV:
            return AceType::MakeRefPtr<TransitionTvTweenOption>(isRightToLeft, context);
        case DeviceType::WATCH:
            return AceType::MakeRefPtr<TransitionWatchTweenOption>(event, isRightToLeft, context);
        default:
            LOGE("unknown transition type: %{public}d, use default type", deviceType);
            return AceType::MakeRefPtr<TransitionPhoneTweenOption>(event, isRightToLeft, context);
    }
}

void TransitionTweenOptionFactory::CreateSharedTweenOption(SharedTransitionEffectType type, TweenOption& option)
{
    if (type == SharedTransitionEffectType::SHARED_EFFECT_STATIC) {
        CreateSharedStaticOption(option);
    } else {
        LOGW("Unsupported Shared transition effect type: %{public}d", type);
    }
}

TransitionPhoneTweenOption::TransitionPhoneTweenOption(
    TransitionEvent event, bool isRightToLeft, const WeakPtr<PipelineContext>& context)
    : TransitionTweenOption(isRightToLeft, context)
{
    if (windowModal_ == WindowModal::DIALOG_MODAL) {
        CreateDialogModalTransitionInOption(event);
        CreateDialogModalTransitionOutOption(event);
    } else {
        CreateTransitionInOption(event);
        CreateTransitionOutOption(event);
    }
    CreateSharedInOption(isRightToLeft_, deviceViewScale_, deviceWidth_, sharedTransitionInOption_);
    CreateSharedOutOption(sharedTransitionOutOption_);
    // extra options for phone
    auto widthAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(deviceWidth_, deviceWidth_, Curves::FRICTION);
    sharedTransitionOutOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, widthAnimation);

    auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
        DimensionOffset(Offset()), DimensionOffset(Offset()), Curves::LINEAR);
    sharedTransitionOutOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);

    auto colorAnimation = AceType::MakeRefPtr<CurveAnimation<Color>>(
        Color::FromRGBO(0, 0, 0, 0.0), Color::FromRGBO(0, 0, 0, 0.0), Curves::FRICTION);
    sharedFrontDecorationOption_.SetColorAnimation(colorAnimation);
    sharedFrontDecorationOption_.SetIsBackground(false);
}

TransitionPhoneTweenOption::TransitionPhoneTweenOption(
    TransitionEvent event, bool isRightToLeft, const RRect& rrect, const WeakPtr<PipelineContext>& context)
    : TransitionTweenOption(isRightToLeft, context)
{
    CreateCardTransitionInOption(event, rrect);
    CreateCardTransitionOutOption(event);

    CreateSharedInOption(isRightToLeft_, deviceViewScale_, deviceWidth_, sharedTransitionInOption_);
    CreateSharedOutOption(sharedTransitionOutOption_);
    // extra options for phone
    auto widthAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(deviceWidth_, deviceWidth_, Curves::FRICTION);
    sharedTransitionOutOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, widthAnimation);

    auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
        DimensionOffset(Offset()), DimensionOffset(Offset()), Curves::LINEAR);
    sharedTransitionOutOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);

    auto colorAnimation = AceType::MakeRefPtr<CurveAnimation<Color>>(
        Color::FromRGBO(0, 0, 0, 0.0), Color::FromRGBO(0, 0, 0, 0.0), Curves::FRICTION);
    sharedFrontDecorationOption_.SetColorAnimation(colorAnimation);
    sharedFrontDecorationOption_.SetIsBackground(false);
}

void TransitionPhoneTweenOption::CreateDialogModalTransitionInOption(TransitionEvent event)
{
    RefPtr<CurveAnimation<DimensionOffset>> translateAnimation;
    if (isRightToLeft_) {
        translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
            DimensionOffset(Dimension(-deviceWidth_, DimensionUnit::PX), Dimension()),
            DimensionOffset(Dimension(), Dimension()), Curves::FRICTION);
    } else {
        translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
            DimensionOffset(Dimension(deviceWidth_, DimensionUnit::PX), Dimension()),
            DimensionOffset(Dimension(), Dimension()), Curves::FRICTION);
    }
    contentInOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    contentInOption_.SetDuration(TRANSITION_PHONE_DURATION);
}

void TransitionPhoneTweenOption::CreateTransitionInOption(TransitionEvent event)
{
    if (isRightToLeft_) {
        auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
            DimensionOffset(Dimension(-deviceWidth_ * TRANSITION_FACTOR_IN, DimensionUnit::PX), Dimension()),
            DimensionOffset(Dimension(), Dimension()), Curves::FRICTION);
        auto widthAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(
            deviceWidth_ * TRANSITION_FACTOR_IN, deviceWidth_, Curves::FRICTION);
        contentInOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, widthAnimation);
        contentInOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    } else {
        auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
            DimensionOffset(Dimension(deviceWidth_ * TRANSITION_FACTOR_IN, DimensionUnit::PX), Dimension()),
            DimensionOffset(Dimension(), Dimension()), Curves::FRICTION);
        auto offsetXAnimation =
            AceType::MakeRefPtr<CurveAnimation<float>>(deviceWidth_ * TRANSITION_FACTOR_IN, 0.0f, Curves::FRICTION);
        contentInOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_OFFSET_X, offsetXAnimation);
        contentInOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    }
    if (event == TransitionEvent::POP_START) {
        contentInOption_.SetCurve(Curves::FAST_OUT_SLOW_IN);
    }
    contentInOption_.SetDuration(TRANSITION_PHONE_DURATION);
}

void TransitionPhoneTweenOption::CreateDialogModalTransitionOutOption(TransitionEvent event)
{
    RefPtr<CurveAnimation<DimensionOffset>> translateAnimation;
    if (isRightToLeft_) {
        translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Offset()),
            DimensionOffset(Offset(deviceWidth_, 0)), Curves::FRICTION);
    } else {
        translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Offset()),
            DimensionOffset(Offset(-deviceWidth_, 0)), Curves::FRICTION);
    }
    auto opacityAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 1.0f, Curves::LINEAR);
    contentOutOption_.SetOpacityAnimation(opacityAnimation);
    contentOutOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
}

void TransitionPhoneTweenOption::CreateTransitionOutOption(TransitionEvent event)
{
    if (isRightToLeft_) {
        auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Offset()),
            DimensionOffset(Offset(deviceWidth_ * TRANSITION_FACTOR_OUT, 0)), Curves::FRICTION);
        auto offsetXAnimation =
            AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, deviceWidth_ * TRANSITION_FACTOR, Curves::FRICTION);
        contentOutOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_OFFSET_X, offsetXAnimation);
        contentOutOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    } else {
        auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Offset()),
            DimensionOffset(Offset(-deviceWidth_ * TRANSITION_FACTOR_OUT, 0)), Curves::FRICTION);
        auto widthAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(
            deviceWidth_, deviceWidth_ * TRANSITION_FACTOR_OUT, Curves::FRICTION);
        contentOutOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, widthAnimation);
        contentOutOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    }
    auto opacityAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 1.0f, Curves::LINEAR);
    contentOutOption_.SetOpacityAnimation(opacityAnimation);

    auto colorAnimation = AceType::MakeRefPtr<CurveAnimation<Color>>(
        Color::FromRGBO(0, 0, 0, 0.0), Color::FromRGBO(0, 0, 0, BLACK_OPACITY), Curves::FRICTION);
    frontDecorationOption_.SetColorAnimation(colorAnimation);
    frontDecorationOption_.SetIsBackground(false);

    if (event == TransitionEvent::POP_START) {
        contentOutOption_.SetCurve(Curves::FAST_OUT_SLOW_IN);
        frontDecorationOption_.SetCurve(Curves::FAST_OUT_SLOW_IN);
    }
}

TransitionWatchTweenOption::TransitionWatchTweenOption(
    TransitionEvent event, bool isRightToLeft, const WeakPtr<PipelineContext>& context)
    : TransitionTweenOption(isRightToLeft, context)
{
    CreateTransitionInOption(event);
    CreateTransitionOutOption(event);
    CreateSharedInOption(isRightToLeft_, deviceViewScale_, deviceWidth_, sharedTransitionInOption_);
    CreateSharedOutOption(sharedTransitionOutOption_);
}

void TransitionWatchTweenOption::CreateTransitionInOption(TransitionEvent event)
{
    RefPtr<CurveAnimation<DimensionOffset>> translateAnimation;
    if (isRightToLeft_) {
        translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
            DimensionOffset(Dimension(-deviceWidth_, DimensionUnit::PX), Dimension()),
            DimensionOffset(Dimension(), Dimension()), Curves::FRICTION);
    } else {
        translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(
            DimensionOffset(Dimension(deviceWidth_, DimensionUnit::PX), Dimension()),
            DimensionOffset(Dimension(), Dimension()), Curves::FRICTION);
    }
    contentInOption_.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    contentInOption_.SetDuration(TRANSITION_WATCH_DURATION);

    if (event == TransitionEvent::POP_START) {
        contentInOption_.SetCurve(Curves::SMOOTH);
    }
}

void TransitionWatchTweenOption::CreateTransitionOutOption(TransitionEvent event)
{
    if (isRightToLeft_) {
        auto offsetXAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(0.0f, deviceWidth_, Curves::FRICTION);
        contentOutOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_OFFSET_X, offsetXAnimation);
    } else {
        auto widthAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(deviceWidth_, 0.0f, Curves::FRICTION);
        contentOutOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_WIDTH, widthAnimation);
    }
    auto opacityAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, 1.0f, Curves::FRICTION);
    contentOutOption_.SetOpacityAnimation(opacityAnimation);

    if (event == TransitionEvent::POP_START) {
        contentOutOption_.SetCurve(Curves::SMOOTH);
    }
}

void TransitionPhoneTweenOption::CreateCardTransitionOutOption(TransitionEvent event)
{
    contentOutOption_.SetDuration(TRANSITION_PHONE_DURATION);
    // blackening treatment
    if (event == TransitionEvent::POP_START) {
        auto colorAnimation = AceType::MakeRefPtr<CurveAnimation<Color>>(Color::FromRGBO(0, 0, 0, 0.2),
            Color::FromRGBO(0, 0, 0, 0.0), Curves::FRICTION);
        frontDecorationOption_.SetColorAnimation(colorAnimation);
    } else {
        auto colorAnimation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
        auto colorKeyframe1 = AceType::MakeRefPtr<Keyframe<Color>>(0.0f, Color::FromRGBO(0, 0, 0, 0.0));
        auto colorKeyframe2 = AceType::MakeRefPtr<Keyframe<Color>>(0.571f, Color::FromRGBO(0, 0, 0, 0.2));
        auto colorKeyframe3 = AceType::MakeRefPtr<Keyframe<Color>>(1.0f, Color::FromRGBO(0, 0, 0, 0.2));
        colorKeyframe2->SetCurve(Curves::SHARP);
        colorAnimation->AddKeyframe(colorKeyframe1);
        colorAnimation->AddKeyframe(colorKeyframe2);
        colorAnimation->AddKeyframe(colorKeyframe3);
        frontDecorationOption_.SetColorAnimation(colorAnimation);
    }
    frontDecorationOption_.SetIsBackground(false);
}

void TransitionPhoneTweenOption::CreateCardTransitionInOption(TransitionEvent event, const RRect& rrect)
{
    const Offset& position = rrect.GetRect().GetOffset();
    const Size& size = rrect.GetRect().GetSize();
    contentInOption_.SetTransformOrigin(Dimension(), Dimension());
    if (event == TransitionEvent::POP_START) {
        auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(Offset()),
            DimensionOffset(position), Curves::FRICTION);
        auto heightAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(deviceHeight_,
            size.Height() * deviceWidth_ / size.Width(), Curves::FRICTION);
        auto scaleAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(1.0f, size.Width() / deviceWidth_,
            Curves::FRICTION);
        contentInOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_HEIGHT, heightAnimation);
        contentInOption_.SetTranslateAnimations(AnimationType::TRANSLATE, translateAnimation);
        contentInOption_.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimation);
    } else {
        auto translateAnimation = AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(DimensionOffset(position),
            DimensionOffset(Offset()), Curves::FRICTION);
        auto heightAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(size.Height() * deviceWidth_ / size.Width(),
            deviceHeight_, Curves::FRICTION);
        auto scaleAnimation = AceType::MakeRefPtr<CurveAnimation<float>>(size.Width() / deviceWidth_, 1.0f,
            Curves::FRICTION);
        contentInOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_HEIGHT, heightAnimation);
        contentInOption_.SetTranslateAnimations(AnimationType::TRANSLATE, translateAnimation);
        contentInOption_.SetTransformFloatAnimation(AnimationType::SCALE, scaleAnimation);
    }
    CreateCornerAnimationInOption(event, rrect);
    CreateCardOpacityAnimationInOption(event);
    contentInOption_.SetDuration(TRANSITION_PHONE_DURATION);
}

void TransitionPhoneTweenOption::CreateCornerAnimationInOption(TransitionEvent event, const RRect& rrect)
{
    auto cornerRadiusAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    auto cornerRadius = rrect.GetCorner().topLeftRadius.GetX().Value() * deviceWidth_ / rrect.Width();
    if (event == TransitionEvent::POP_START) {
        auto cornerKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, TRANSITION_DEFAULT_CORNER_RADIUS);
        auto cornerKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.428f, cornerRadius);
        auto cornerKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, cornerRadius);
        cornerKeyframe2->SetCurve(Curves::FRICTION);
        cornerRadiusAnimation->AddKeyframe(cornerKeyframe1);
        cornerRadiusAnimation->AddKeyframe(cornerKeyframe2);
        cornerRadiusAnimation->AddKeyframe(cornerKeyframe3);
    } else {
        auto cornerKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, cornerRadius);
        auto cornerKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.571f, cornerRadius);
        auto cornerKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, TRANSITION_DEFAULT_CORNER_RADIUS);
        cornerKeyframe3->SetCurve(Curves::FRICTION);
        cornerRadiusAnimation->AddKeyframe(cornerKeyframe1);
        cornerRadiusAnimation->AddKeyframe(cornerKeyframe2);
        cornerRadiusAnimation->AddKeyframe(cornerKeyframe3);
    }
    contentInOption_.SetPropertyAnimationFloat(PropertyAnimatableType::PROPERTY_BORDER_RADIUS, cornerRadiusAnimation);
}

void TransitionPhoneTweenOption::CreateCardOpacityAnimationInOption(TransitionEvent event)
{
    auto opacityAnimation = AceType::MakeRefPtr<KeyframeAnimation<float>>();
    if (event == TransitionEvent::POP_START) {
        auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 1.0f);
        auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.571f, 1.0f);
        auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 0.0f);
        opacityKeyframe3->SetCurve(Curves::SHARP);
        opacityAnimation->AddKeyframe(opacityKeyframe1);
        opacityAnimation->AddKeyframe(opacityKeyframe2);
        opacityAnimation->AddKeyframe(opacityKeyframe3);
    } else {
        auto opacityKeyframe1 = AceType::MakeRefPtr<Keyframe<float>>(0.0f, 0.0f);
        auto opacityKeyframe2 = AceType::MakeRefPtr<Keyframe<float>>(0.286f, 1.0f);
        auto opacityKeyframe3 = AceType::MakeRefPtr<Keyframe<float>>(1.0f, 1.0f);
        opacityKeyframe2->SetCurve(Curves::SHARP);
        opacityAnimation->AddKeyframe(opacityKeyframe1);
        opacityAnimation->AddKeyframe(opacityKeyframe2);
        opacityAnimation->AddKeyframe(opacityKeyframe3);
    }
    contentInOption_.SetOpacityAnimation(opacityAnimation);
}

void TransitionDeclarativeTweenOption::CreateSlideEffectAnimation(
    TweenOption& tweenOption, SlideEffect effect, PageTransitionType type, TransitionDirection direction)
{
    bool exitFlag = false;
    if (type == PageTransitionType::EXIT || type == PageTransitionType::EXIT_POP ||
        type == PageTransitionType::EXIT_PUSH) {
        exitFlag = true;
    }

    if (effect == SlideEffect::LEFT) {
        auto init = DimensionOffset(Dimension(-deviceWidth_, DimensionUnit::PX), Dimension());
        auto target = DimensionOffset(Dimension(), Dimension());
        if (exitFlag) {
            std::swap(init, target);
        }
        auto translateAnimation =
            AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(init, target, tweenOption.GetCurve());
        tweenOption.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    } else if (effect == SlideEffect::RIGHT) {
        auto init = DimensionOffset(Dimension(deviceWidth_, DimensionUnit::PX), Dimension());
        auto target = DimensionOffset(Dimension(), Dimension());
        if (exitFlag) {
            std::swap(init, target);
        }
        auto translateAnimation =
            AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(init, target, tweenOption.GetCurve());
        tweenOption.SetTranslateAnimations(AnimationType::TRANSLATE_X, translateAnimation);
    } else if (effect == SlideEffect::TOP) {
        auto init = DimensionOffset(Dimension(), Dimension(-deviceHeight_, DimensionUnit::PX));
        auto target = DimensionOffset(Dimension(), Dimension());
        if (exitFlag) {
            std::swap(init, target);
        }
        auto translateAnimation =
            AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(init, target, tweenOption.GetCurve());
        tweenOption.SetTranslateAnimations(AnimationType::TRANSLATE_Y, translateAnimation);
    } else if (effect == SlideEffect::BOTTOM) {
        auto init = DimensionOffset(Dimension(), Dimension(deviceHeight_, DimensionUnit::PX));
        auto target = DimensionOffset(Dimension(), Dimension());
        if (exitFlag) {
            std::swap(init, target);
        }
        auto translateAnimation =
            AceType::MakeRefPtr<CurveAnimation<DimensionOffset>>(init, target, tweenOption.GetCurve());
        tweenOption.SetTranslateAnimations(AnimationType::TRANSLATE_Y, translateAnimation);
    }
}

} // namespace OHOS::Ace
