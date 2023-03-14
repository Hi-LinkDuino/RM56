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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RENDER_SIDE_BAR_CONTAINER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RENDER_SIDE_BAR_CONTAINER_H

#include "base/geometry/dimension.h"
#include "base/memory/referenced.h"
#include "core/components/common/layout/constants.h"
#include "core/components/stack/render_stack.h"
#include "core/components/side_bar/side_bar_animation_controller.h"
#include "core/components/side_bar/side_bar_container_component.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/click_recognizer.h"

namespace OHOS::Ace {

class RenderSideBarContainer : public RenderStack {
    DECLARE_ACE_TYPE(RenderSideBarContainer, RenderStack);
public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void OnStatusChanged(RenderStatus renderStatus) override;
    void UpdateElementPosition(double offset);
    double GetSidebarWidth() const;
    double GetSlidePosition() const;
    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint,
        const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    
    bool GetShowSideBarContainer() const
    {
        return showSideBar_;
    }

    bool GetShowControlButton() const
    {
        return showControlButton_;
    }

    std::string GetStyle() const
    {
        return style_;
    }

    std::string GetShowIcon() const
    {
        return iconShow_;
    }

    std::string GetHiddenIcon() const
    {
        return iconHidden_;
    }

    std::string GetSwitchIcon() const
    {
        return iconSwitch_;
    }

    double GetButtonLeft() const
    {
        return buttonLeft_;
    }

    double GetButtonTop() const
    {
        return buttonTop_;
    }

    double GetButtonWidth() const
    {
        return buttonWidth_;
    }

    double GetButtonHeight() const
    {
        return buttonHeight_;
    }

    const Dimension& GetSideBarWidth() const
    {
        return sidebarWidth_;
    }

    const Dimension& GetSideBarMinWidth() const
    {
        return minSidebarWidth_;
    }

    const Dimension& GetSideBarMaxWidth() const
    {
        return maxSidebarWidth_;
    }

private:
    void DoSideBarAnimation();
    void SetChildrenStatus() override;
    void HandleDragStart();
    void HandleDragUpdate(double xOffset);
    void HandleDragEnd();
    void UpdateRenderImage();
    void InitializeDragAndAnimation();

    RefPtr<GestureRecognizer> dragRecognizer_;
    RefPtr<SideBarAnimationController> animationController_;
    RefPtr<SideBarContainerComponent> sideBar_;
    WeakPtr<RenderNode> renderImage_;
    SideStatus status_ = SideStatus::SHOW;
    SideStatus pendingStatus_ = status_;

    bool isFocus_ = false;
    bool isAnimation_ = false;
    bool isInitialized_ = false;
    bool showControlButton_ = true;
    bool showSideBar_ = true;
    std::string style_;
    std::string iconShow_;
    std::string iconHidden_;
    std::string iconSwitch_;
    double buttonLeft_ = 16.0;
    double buttonTop_ = 48.0;
    double buttonWidth_ = 32.0;
    double buttonHeight_ = 32.0;

    Dimension sidebarWidth_ = 200.0_vp;
    Dimension minSidebarWidth_ = 200.0_vp;
    Dimension maxSidebarWidth_ = 280.0_vp;
    Dimension curPosition_ = -sidebarWidth_;
    Dimension preSidebarWidth_;
    Rect exceptRegion_;
};

} // namespace OHOS::Ace

#endif  // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RENDER_SIDE_BAR_CONTAINER_H
