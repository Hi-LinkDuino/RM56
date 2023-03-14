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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_RENDER_TOOL_BAR_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_RENDER_TOOL_BAR_ITEM_H

#include <vector>

#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/focus_animation/render_focus_animation.h"
#include "core/components/tool_bar/tool_bar_item_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderToolBarItem : public RenderNode, public FocusNode {
    DECLARE_ACE_TYPE(RenderToolBarItem, RenderNode)

public:
    using TargetMenuCallBack = std::function<void(const Offset&, const Offset&)>;
    using EventCallback = std::function<void()>;

    RenderToolBarItem();
    ~RenderToolBarItem() override = default;
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void Initialize();

    void SetTargetMenuCallBack(TargetMenuCallBack&& targetMenuCallBack)
    {
        targetMenuCallBack_ = std::move(targetMenuCallBack);
    }

    void SetFocusEventFlag(bool onFocus)
    {
        onFocus_ = onFocus;
        if (position_ == PositionType::FIXED) {
            auto context = context_.Upgrade();
            if (!context) {
                LOGE("Pipeline context upgrade fail!");
                return;
            }
            if (onFocus_) {
                context->SetUseRootAnimation(true);
            } else {
                context->SetUseRootAnimation(false);
            }
        }
    }

    void SetFocusAnimation(const RefPtr<RenderFocusAnimation>& focusAnimation)
    {
        focusAnimation_ = focusAnimation;
    }

    const RefPtr<RenderFocusAnimation>& GetFocusAnimation() const
    {
        return focusAnimation_;
    }

    void RequestFocusAnimation(const Offset& animationOffset, const Rect& animationRect, const Radius& radius);
    void OnStatusChanged(RenderStatus renderStatus) override;
    void HandleClickEvent();

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    void HandleTouchEvent(bool isTouch);
    void HandleMoveEvent(const TouchEventInfo& info);
    void ShowMenu();
    void PlayEventEffectAnimation(const Color& endColor);

    Color pressColor_;
    Color focusColor_;
    Color hoverColor_;
    Color mouseHoverColor_;
    Dimension rrectRadius_;
    bool onFocus_ = false;
    bool onHover_ = false;
    bool onTouch_ = false;
    EventCallback onClick_;
    RefPtr<RenderFocusAnimation> focusAnimation_;

private:
    void StartHoverAnimation(RefPtr<Animator> controller,
        RefPtr<KeyframeAnimation<Color>>& colorAnimation);
    void ResetController(RefPtr<Animator>& controller);
    void CreateColorAnimation(RefPtr<KeyframeAnimation<Color>>& colorAnimation, const Color& beginValue,
        const Color& endValue, bool hover);
    void InitAccessibilityEventListener();
    RefPtr<RawRecognizer> touchRecognizer_;
    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<Animator> controllerEnter_;
    RefPtr<Animator> controllerExit_;
    RefPtr<KeyframeAnimation<Color>> colorAnimationEnter_;
    RefPtr<KeyframeAnimation<Color>> colorAnimationExit_;
    TargetMenuCallBack targetMenuCallBack_;
    bool isEndItem_ = false;
    Dimension menuMinWidth_;
    PositionType position_ { PositionType::RELATIVE };
    RefPtr<Animator> eventEffectController_;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_RENDER_TOOL_BAR_ITEM_H
