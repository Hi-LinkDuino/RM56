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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_INTERACTIVE_EFFECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_INTERACTIVE_EFFECT_H

#include "base/memory/ace_type.h"
#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/list/list_item_theme.h"
#include "core/pipeline/base/render_node.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

const double DEFAULT_FOCUS_DURATION = 100.0;
const double DEFAULT_CLICK_DURATION = 200.0;
const double PRESS_ANIMATION_DURATION = 100.0;
const float ANIMATION_ZERO_TIME = 0.0f;
const float ANIMATION_HALF_TIME = 0.5f;
const float ANIMATION_END_TIME = 1.0f;

enum class ItemState {
    FOCUS = 0, // center position of list on watch
    BLUR, // not FOCUS, and not neighbor to FOCUS on watch
    NEARBY, // neighbor to FOCUS on watch
    CLICK, // instant state when item clicked
    NONE,
};

class InteractiveEffect : public AceType {
    DECLARE_ACE_TYPE(InteractiveEffect, AceType);

public:
    explicit InteractiveEffect(const WeakPtr<PipelineContext>& context);
    ~InteractiveEffect() = default;
    void ShowAnimation(ItemState state);
    void TouchDownAnimation();
    void TouchUpAnimation();
    void CancelTouchAnimation();
    void Initialize(const RefPtr<ThemeManager>& themeManager);
    void UpdateContext(const WeakPtr<PipelineContext>& context);
    void FinishPreviousAnimation();

    double GetScale() const
    {
        return scale_;
    }
    void SetScale(double scale)
    {
        scale_ = scale;
    }
    double GetAlpha() const
    {
        return alpha_;
    }
    void SetAlpha(double alpha)
    {
        alpha_ = alpha;
    }
    double GetOpacity() const
    {
        return opacity_;
    }
    void SetOpacity(double opacity)
    {
        opacity_ = opacity;
    }
    void SetItemNode(const WeakPtr<RenderNode>& item)
    {
        item_ = item;
    }
    void SetListNode(const WeakPtr<RenderNode>& list)
    {
        list_ = list;
    }

    const RefPtr<Animator>& GetAnimator() const
    {
        return controller_;
    }

    virtual void HandleOnFocus() {}
    virtual void HandleOnBlur() {}

    // default click effect.
    virtual void HandleOnClick()
    {
        if (theme_) {
            alphaBegin_ = theme_->GetClickAlphaBegin();
            alphaEnd_ = theme_->GetClickAlphaEnd();
            clickDuration_ = theme_->GetClickAnimationDuration(); // 200.0
        }
    }

    virtual void BuildStateAnimation() {}
    virtual void BuildClickAnimation();

protected:
    void MarkItemRender();
    bool NeedClickAnimation();
    void BuildClickAlphaAnimation(const RefPtr<KeyframeAnimation<double>>& alphaAnimation);
    void CreateDoubleAnimation(RefPtr<KeyframeAnimation<double>>& doubleAnimation, double beginValue, double endValue);
    void StartTouchAnimation(RefPtr<Animator> controller,
        RefPtr<KeyframeAnimation<double>>& doubleAnimation, int32_t startDelay = 0);

    double focusDuration_ = DEFAULT_FOCUS_DURATION; // focus animation duration, ms
    double clickDuration_ = DEFAULT_CLICK_DURATION; // click animation duration, ms
    double scaleBegin_ = 1.0;
    double scaleEnd_ = 1.0;
    double opacityBegin_ = 0;
    double opacityEnd_ = 1.0;
    double alphaBegin_ = 0.0;
    double alphaEnd_ = 0.1;
    double alpha_ = 0.0;   // alpha factor in argb
    double scale_ = 1.0;   // for canvas scale
    double opacity_ = 1.0; // for opacity layer
    WeakPtr<RenderNode> item_;
    WeakPtr<RenderNode> list_;
    RefPtr<PipelineContext> context_;
    RefPtr<Animator> controller_;
    RefPtr<ListItemTheme> theme_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_INTERACTIVE_EFFECT_H
