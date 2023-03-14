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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_RENDER_TOGGLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_RENDER_TOGGLE_H

#include "base/utils/system_properties.h"
#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/common/properties/color.h"
#include "core/components/toggle/toggle_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderToggle : public RenderNode {
    DECLARE_ACE_TYPE(RenderToggle, RenderNode);

public:
    ~RenderToggle() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void UpdateFocusAnimation();

    RefPtr<ToggleComponent> GetToggleComponent()
    {
        return toggleComponent_;
    }

protected:
    RenderToggle();
    virtual Size Measure() = 0;
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void HandleClickEvent();
    void HandleTouchEvent(bool touched);
    void HandleMoveEvent(const TouchEventInfo& info);
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;
    void OnMouseClickDownAnimation() override;
    void OnMouseClickUpAnimation() override;
    void SetOnChange(const std::function<void(bool)>& value)
    {
        onChangeToggle_ = value;
    }

    RefPtr<ToggleComponent> toggleComponent_;

    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<RawRecognizer> touchRecognizer_;
    RefPtr<Animator> hoverControllerEnter_;
    RefPtr<Animator> hoverControllerExit_;
    RefPtr<Animator> clickControllerDown_;
    RefPtr<Animator> clickControllerUp_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationEnter_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationExit_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationUp_;
    RefPtr<KeyframeAnimation<float>> scaleAnimationDown_;

    bool widthDefined_ = false;
    bool heightDefined_ = false;
    bool isPressed_ = false;
    bool isMoveEventValid_ = false;
    bool isFocus_ = false;
    float scale_ = 1.0f;

    Size toggleSize_;
    std::function<void()> onClick_;
    std::function<void(const std::string&)> onChange_;
    std::function<void(bool)> onChangeToggle_;

private:
    void ResetController(RefPtr<Animator>& controller);
    void CreateFloatAnimation(RefPtr<KeyframeAnimation<float>>& floatAnimation, float beginValue, float endValue);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOGGLE_RENDER_TOGGLE_H
