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

#include "core/animation/animator.h"
#include "core/animation/friction_motion.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/scroll_bar/scroll_bar_component.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_RENDER_SCROLL_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_RENDER_SCROLL_BAR_H

namespace OHOS::Ace {

class RenderScrollBar : public RenderNode {
    DECLARE_ACE_TYPE(RenderScrollBar, RenderNode);

public:
    RenderScrollBar() = default;
    ~RenderScrollBar() override;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnPaintFinish() override;

    const std::vector<Rect>& GetTouchRectList() override
    {
        touchRectList_.clear();
        touchRectList_.emplace_back(childRect_);
        return touchRectList_;
    }

    bool SupportOpacity() override
    {
        return true;
    }

    Axis GetAxis() const
    {
        return axis_;
    }

    DisplayMode GetDisplayMode() const
    {
        return displayMode_;
    }

    void StartAnimator();
    void StopAnimator();

protected:
    int32_t opacity_ = 0;
    DisplayMode displayMode_ = DisplayMode::AUTO;

private:
    void InitRecognizer();
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void HandleDragStart(const DragStartInfo& info);
    void HandleDragUpdate(const DragUpdateInfo& info);
    void HandleDragFinish();

    void InitOpacity();
    void InitAnimator();
    void InitChildPosition();
    void UpdateDisplayOpacity(int32_t opacity);

    bool isAxisChanged_ = true;
    bool touchInBarRegion_ = true;
    Rect childRect_;
    Axis axis_ = Axis::VERTICAL;
    RefPtr<DragRecognizer> dragRecognizer_;
    RefPtr<ScrollBarProxy> proxy_;

    // When display mode is auto and there is no operation, start opacity animation from show to hide.
    RefPtr<Animator> disappearAnimator_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_RENDER_SCROLL_BAR_H
