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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_ITEM_COMPONENT_H

#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ToolBarItemComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ToolBarItemComponent, SoleChildComponent);

public:
    using OptionChildrenCallBack = std::function<std::list<RefPtr<Component>>()>;
    using EventCallback = std::function<void()>;

    ToolBarItemComponent() = default;
    ~ToolBarItemComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetIsEndItem(bool isEndItem)
    {
        isEndItem_ = isEndItem;
    }

    bool GetIsEndItem()
    {
        return isEndItem_;
    }

    void SetOptionChildrenCallBack(OptionChildrenCallBack&& optionChildrenCallBack)
    {
        optionChildrenCallBack_ = std::move(optionChildrenCallBack);
    }

    const OptionChildrenCallBack& GetOptionChildrenCallBack() const
    {
        return optionChildrenCallBack_;
    }

    void SetMenuMinWidth(const Dimension& menuMinWidth)
    {
        menuMinWidth_ = menuMinWidth;
    }

    const Dimension& GetMenuMinWidth() const
    {
        return menuMinWidth_;
    }

    void SetPressColor(const Color& pressColor)
    {
        pressColor_ = pressColor;
    }

    const Color& GetPressColor()
    {
        return pressColor_;
    }

    void SetClickedEventId(const EventMarker& eventId)
    {
        clickEventId_ = eventId;
    }

    const EventMarker& GetClickedEventId() const
    {
        return clickEventId_;
    }

    void SetRadius(const Dimension& rrectRadius)
    {
        rrectRadius_ = rrectRadius;
    }

    const Dimension& GetRadius()
    {
        return rrectRadius_;
    }

    void SetFocusColor(const Color& focusColor)
    {
        focusColor_ = focusColor;
    }

    const Color& GetFocusColor()
    {
        return focusColor_;
    }

    void SetHoverColor(const Color& hoverColor)
    {
        hoverColor_ = hoverColor;
    }

    const Color& GetHoverColor()
    {
        return hoverColor_;
    }

private:
    OptionChildrenCallBack optionChildrenCallBack_;
    EventMarker clickEventId_;

    Dimension menuMinWidth_;
    bool isEndItem_ = false;
    Dimension rrectRadius_;

    Color pressColor_;
    Color hoverColor_;
    Color focusColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_ITEM_COMPONENT_H
