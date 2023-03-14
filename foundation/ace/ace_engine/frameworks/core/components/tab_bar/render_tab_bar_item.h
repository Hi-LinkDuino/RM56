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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_BAR_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_BAR_ITEM_H

#include "base/memory/ace_type.h"
#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/box/render_box.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/gestures/raw_recognizer.h"

namespace OHOS::Ace {

class RenderTabBarItem : public RenderBox {
DECLARE_ACE_TYPE(RenderTabBarItem, RenderBox)

public:
    ~RenderTabBarItem() override = default;
    static RefPtr<RenderNode> Create();

    void SetTouching(bool isTouching)
    {
        touching_ = isTouching;
    }

    bool IsTouching() const
    {
        return touching_;
    }

protected:
    RenderTabBarItem();
    void OnTouchTestHit(const Offset& coordinateOffset, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    void PerformLayout() override;

    bool onHover_ = false;
    double hoverOpacity_ = 0.0;

private:
    void HandleTouchDown();
    void HandleTouchUp();
    void StartHoverAnimation(RefPtr<Animator> controller,
        RefPtr<KeyframeAnimation<double>>& doubleAnimation);
    void ResetController(RefPtr<Animator>& controller);
    void CreateDoubleAnimation(RefPtr<KeyframeAnimation<double>>& doubleAnimation, double beginValue,
        double endValue, bool hover);
    void PlayPressAnimation(double endOpacityRatio);

    bool touching_ = false; // whether the item is in touching
    RefPtr<RawRecognizer> touchRecognizer_;
    RefPtr<Animator> controllerEnter_;
    RefPtr<Animator> controllerExit_;
    RefPtr<Animator> controllerPress_;
    RefPtr<KeyframeAnimation<double>> doubleAnimationEnter_;
    RefPtr<KeyframeAnimation<double>> doubleAnimationExit_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_RENDER_TAB_BAR_ITEM_H
