/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SIDE_BAR_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SIDE_BAR_DECLARATION_H

#include "core/components/declaration/common/declaration.h"

namespace OHOS::Ace {
struct SideBarContainerAttribute : Attribute {
    bool isShow = true;
    SideBarContainerType style = SideBarContainerType::EMBED;
    bool isShowButton = true;
    double left = 16.0;
    double top = 48.0;
    double sidebarWidth = 200.0;
    double width = 32.0;
    double height = 32.0;
    std::string exicon;
    std::string changing;
    std::string inicon;
};

struct RegionEvent : Event {
    EventMarker clickEventId = EventMarker("-1");
};

class SideBarDeclaration : public Declaration {
    DECLARE_ACE_TYPE(SideBarDeclaration, Declaration);

public:
    SideBarDeclaration() = default;
    ~SideBarDeclaration() override = default;

    bool IsShowSideBar() const
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isShow;
    }

    void SetShowSideBar(bool isShow)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isShow = isShow;
    }

    SideBarContainerType GetSideBarContainerType() const
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.style;
    }

    void SetSideBarContainerType(SideBarContainerType style)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.style = style;
    }

    bool IsShowControlButton() const
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isShowButton;
    }

    void SetShowControlButton(bool isShow)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isShowButton = isShow;
    }

    void SetImageWidth(double width)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.width = width;
    }

    double GetImageWidth()
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.width;
    }

    void SetImageHeight(double height)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.height = height;
    }

    double GetImageHeight()
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.height;
    }

    void SetShowIcon(const std::string& path)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.exicon = std::move(path);
    }

    std::string& GetShowIcon()
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.exicon;
    }

    void SetHiddenIcon(const std::string& path)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.inicon = std::move(path);
    }

    std::string& GetHiddenIcon()
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.inicon;
    }

    void SetSwitchIcon(const std::string& path)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.changing = std::move(path);
    }

    std::string& GetSwitchIcon()
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.changing;
    }

    double GetLeft() const
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.left;
    }

    double GetTop() const
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.top;
    }

    double GetSidebarWidth() const
    {
        auto& attribute = static_cast<SideBarContainerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.sidebarWidth;
    }

    void SetLeft(double left)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.left = left;
    }

    void SetTop(double top)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.top = top;
    }

    void SetSidebarWidth(double width)
    {
        auto& attribute = MaybeResetAttribute<SideBarContainerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.sidebarWidth = width;
    }

    const EventMarker& GetClickedEventId() const
    {
        auto& event = static_cast<RegionEvent&>(GetEvent(EventTag::SPECIALIZED_EVENT));
        return event.clickEventId;
    }

    void SetClickedFunction(std::function<void()>&& callback)
    {
        auto& event = MaybeResetEvent<RegionEvent>(EventTag::SPECIALIZED_EVENT);
        event.clickEventId.SetPreFunction(std::move(callback));
    }

protected:
    void InitSpecialized() override;
};
} // namespace OHOS::Ace

#endif  // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SIDE_BAR_DECLARATION_H