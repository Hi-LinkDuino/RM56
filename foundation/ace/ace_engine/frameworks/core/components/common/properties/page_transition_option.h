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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PAGE_TRANSITION_OPTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PAGE_TRANSITION_OPTION_H

#include "base/utils/device_type.h"
#include "core/animation/curves.h"
#include "core/animation/interpolator.h"
#include "core/animation/keyframe.h"
#include "core/animation/keyframe_animation.h"
#include "core/animation/page_transition_listener.h"
#include "core/animation/shared_transition_effect.h"
#include "core/components/common/properties/tween_option.h"
#include "core/components/page_transition/page_transition_info.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

extern const int32_t TRANSITION_PHONE_DURATION;
extern const int32_t TRANSITION_WATCH_DURATION;
extern const int32_t TRANSITION_TV_DURATION;
extern const double TRANSITION_DEFAULT_WIDTH;
extern const double TRANSITION_DEFAULT_HEIGHT;

enum class TransitionDirection {
    TRANSITION_IN,
    TRANSITION_OUT,
};

class TransitionTweenOption : public AceType {
public:
    TransitionTweenOption(bool isRightToLeft, const WeakPtr<PipelineContext>& context);
    ~TransitionTweenOption() override = default;
    const TweenOption& GetTransitionContentInOption() const;
    const TweenOption& GetTransitionContentOutOption() const;
    const TweenOption& GetTransitionBackgroundInOption() const;
    const TweenOption& GetTransitionBackgroundOutOption() const;
    const TweenOption& GetTransitionFrontDecorationOption() const;
    const TweenOption& GetSharedTransitionFrontDecorationOption() const;
    const TweenOption& GetSharedInOption() const;
    const TweenOption& GetSharedOutOption() const;

protected:
    double deviceWidth_ = TRANSITION_DEFAULT_WIDTH;   // default device width
    double deviceHeight_ = TRANSITION_DEFAULT_HEIGHT;   // default device height
    double deviceViewScale_ = 1.0; // default device view scale
    bool isRightToLeft_ = false; // default RTL config
    WindowModal windowModal_ = WindowModal::NORMAL;
    TweenOption contentInOption_;
    TweenOption contentOutOption_;
    TweenOption backgroundInOption_;
    TweenOption backgroundOutOption_;
    TweenOption frontDecorationOption_;
    TweenOption sharedFrontDecorationOption_;
    TweenOption sharedTransitionInOption_;
    TweenOption sharedTransitionOutOption_;
};

class TransitionTvTweenOption : public TransitionTweenOption {
public:
    TransitionTvTweenOption(bool isRightToLeft, const WeakPtr<PipelineContext>& context);
    ~TransitionTvTweenOption() override = default;

private:
    void CreateTransitionInOption();
    void CreateTransitionInContentOption();
    void CreateTransitionInBackgroundOption();
    void CreateTransitionOutOption();
    void CreateTransitionOutContentOption();
    void CreateTransitionOutBackgroundOption();
    static void CreatTransitionOutOption(TweenOption& option);
};

class TransitionPhoneTweenOption : public TransitionTweenOption {
public:
    TransitionPhoneTweenOption(TransitionEvent event, bool isRightToLeft, const WeakPtr<PipelineContext>& context);
    TransitionPhoneTweenOption(TransitionEvent event, bool isRightToLeft, const RRect& rrect,
        const WeakPtr<PipelineContext>& context);
    ~TransitionPhoneTweenOption() override = default;

private:
    void CreateTransitionInOption(TransitionEvent event);
    void CreateTransitionOutOption(TransitionEvent event);
    void CreateDialogModalTransitionInOption(TransitionEvent event);
    void CreateDialogModalTransitionOutOption(TransitionEvent event);
    void CreateCardTransitionOutOption(TransitionEvent event);
    void CreateCardTransitionInOption(TransitionEvent event, const RRect& rrect);
    void CreateCornerAnimationInOption(TransitionEvent event, const RRect& rrect);
    void CreateCardOpacityAnimationInOption(TransitionEvent event);
};

class TransitionWatchTweenOption : public TransitionTweenOption {
public:
    TransitionWatchTweenOption(TransitionEvent event, bool isRightToLeft, const WeakPtr<PipelineContext>& context);
    ~TransitionWatchTweenOption() override = default;

private:
    void CreateTransitionInOption(TransitionEvent event);
    void CreateTransitionOutOption(TransitionEvent event);
};

class TransitionDeclarativeTweenOption final : public TransitionTweenOption {
public:
    TransitionDeclarativeTweenOption(bool isRightToLeft, const WeakPtr<PipelineContext>& context)
        : TransitionTweenOption(isRightToLeft, context)
    {}
    virtual ~TransitionDeclarativeTweenOption() = default;

    void CreateSlideEffectAnimation(
        TweenOption& tweenOption, SlideEffect effect, PageTransitionType type, TransitionDirection direction);
};

class TransitionTweenOptionFactory {
public:
    static void CreateSharedTweenOption(SharedTransitionEffectType type, TweenOption& option);
    static RefPtr<TransitionTweenOption> CreateTransitionTweenOption(DeviceType deviceType, TransitionEvent event,
        bool isRightToLeft, const RRect& rect, const WeakPtr<PipelineContext>& context);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_PAGE_TRANSITION_OPTION_H
