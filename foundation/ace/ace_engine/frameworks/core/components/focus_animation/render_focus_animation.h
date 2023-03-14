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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_RENDER_FOCUS_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_RENDER_FOCUS_ANIMATION_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/focus_animation/focus_animation_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderFocusAnimation : public RenderNode {
    DECLARE_ACE_TYPE(RenderFocusAnimation, RenderNode);

public:
    RenderFocusAnimation() : RenderNode(true) {}
    ~RenderFocusAnimation() override = default;
    static RefPtr<RenderNode> Create();
    void PerformLayout() override;
    void Update(const RefPtr<Component>& component) override;
    bool MarkNeedRenderSpecial() override;
    void SetFocusAnimationProperties(const RRect& rrect, const Color& color, const Offset& offset, bool isIndented);
    void SetFocusAnimationProperties(const RRect& rrect, const Color& color, const Offset& offset,
        const Rect& clipRect);
    void SetFocusAnimationProperties(const RRect& rrect, const Offset& offset);
    void CancelFocusAnimation();
    void StopFocusAnimation();
    void StartFocusAnimation();
    void SetIsKeyEvent(bool isKeyEvent);
    bool MouseHoverTest(const Point& parentLocalPoint) override
    {
        // this node is not require hover state.
        return false;
    }
    bool IsRepaintBoundary() const override
    {
        return true;
    }

    bool IsRoot() const
    {
        return isRoot_;
    }

protected:
    float progress_ = 0.0f;
    Dimension blurMaskRadius_ = 8.0_vp;
    bool isRoot_ = false;
    bool isDisplay_ = false;
    bool isIndented_ = false;
    bool isNeedClip_ = false;
    bool isKeyEvent_ = false;
    bool animationParamsSet_ = false;
    Rect clipRect_;
    RRect rrect_;
    Offset offset_;
    Size layoutSize_;
    Color pathColor_;
    RefPtr<FocusAnimationTheme> focusTheme_;
    RefPtr<CurveAnimation<float>> focusAnimation_;
    RefPtr<Animator> animator_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_RENDER_FOCUS_ANIMATION_H
