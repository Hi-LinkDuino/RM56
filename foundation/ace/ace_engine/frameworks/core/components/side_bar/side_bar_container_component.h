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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SIDE_BAR_CONTAINER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SIDE_BAR_CONTAINER_COMPONENT_H

#include "base/geometry/dimension.h"
#include "core/components/stack/stack_component.h"
#include "core/components/declaration/side_bar/side_bar_declaration.h"
#include "core/components_v2/common/common_def.h"

namespace OHOS::Ace {
enum class SideStatus {
    SHOW,
    HIDDEN,
    CHANGING,
    AUTO,
};

class ACE_EXPORT SideBarContainerComponent : public StackComponent {
    DECLARE_ACE_TYPE(SideBarContainerComponent, StackComponent);
public:
    ACE_DEFINE_COMPONENT_EVENT(OnChange, void(bool));

    explicit SideBarContainerComponent(const std::list<RefPtr<Component>>& children);
    ~SideBarContainerComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void Build();

    void SetShowSideBar(bool isShow)
    {
        if (isShow) {
            sideStatus_ = SideStatus::SHOW;
        } else {
            sideStatus_ = SideStatus::HIDDEN;
        }
        isShow_ = isShow;
    }

    SideStatus GetSideBarStatus() const
    {
        return sideStatus_;
    }

    void SetSideBarContainerType(SideBarContainerType style)
    {
        declaration_->SetSideBarContainerType(style);
    }

    SideBarContainerType GetSideBarContainerType()
    {
        return declaration_->GetSideBarContainerType();
    }

    void SetShowControlButton(bool isShow)
    {
        declaration_->SetShowControlButton(isShow);
    }

    bool GetShowControlButton()
    {
        return declaration_->IsShowControlButton();
    }

    void SetButtonWidth(double width);

    void SetButtonHeight(double height);

    double GetButtonWidth() const;

    double GetButtonHeight() const;

    double GetButtonTop() const;

    double GetButtonLeft() const;

    void SetShowIcon(const std::string& path);

    std::string& GetShowIcon() const;

    void SetHiddenIcon(const std::string& path);

    std::string& GetHiddenIcon() const;

    void SetSwitchIcon(const std::string& path);

    std::string& GetSwitchIcon() const;

    void SetButtonLeft(double left);

    void SetButtonTop(double top);

    void SetClickedFunction(std::function<void()>&& clickCallback);

    void SetSideBarMaxWidth(const Dimension& max)
    {
        sidebarMaxWidth_ = max;
    }

    void SetSideBarMinWidth(const Dimension& min)
    {
        sidebarMinWidth_ = min;
    }

    const Dimension& GetSideBarMaxWidth()
    {
        return sidebarMaxWidth_;
    }

    const Dimension& GetSideBarMinWidth()
    {
        return sidebarMinWidth_;
    }

    void SetSideBarWidth(const Dimension& width)
    {
        isCustomDefineWidth_ = true;
        sidebarWidth_ = width;
    }

    const Dimension& GetSideBarWidth()
    {
        return sidebarWidth_;
    }

    bool IsSideBarwidthDefined()
    {
        return isCustomDefineWidth_;
    }

    bool GetIsShow() const
    {
        return isShow_;
    }

private:
    RefPtr<Component> BuildButton();

    std::function<void()> buttonClick_;
    RefPtr<SideBarDeclaration> declaration_;
    Dimension sidebarWidth_ = 200.0_vp;
    Dimension sidebarMinWidth_ = 200.0_vp;
    Dimension sidebarMaxWidth_ = 280.0_vp;
    bool isCustomDefineWidth_ = false;
    bool isShow_ = true;
    SideStatus sideStatus_ = SideStatus::AUTO;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SIDE_BAR_CONTAINER_COMPONENT_H