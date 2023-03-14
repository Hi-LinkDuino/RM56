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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTENT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTENT_COMPONENT_H

#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_controller.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

// The default interval for scrolling content in ms.
inline constexpr float DEFAULT_TABCONTENT_ANIMATION_DURATION = 200.0f;

class ACE_EXPORT TabContentChangeEvent : public BaseEventInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(TabContentChangeEvent, BaseEventInfo);

public:
    explicit TabContentChangeEvent(int32_t index) : BaseEventInfo("TabContentChangeEvent"), index_(index) {}

    ~TabContentChangeEvent() = default;

    int32_t GetIndex() const
    {
        return index_;
    }

private:
    int32_t index_ = 0;
};

class TabContentComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(TabContentComponent, ComponentGroup);

public:
    TabContentComponent(const std::list<RefPtr<Component>>& contents, const RefPtr<TabController>& controller);
    ~TabContentComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void SetController(const RefPtr<TabController>& controller)
    {
        controller_ = controller;
    }

    RefPtr<TabController> GetController() const
    {
        return controller_;
    }

    void SetScrollable(bool scrollable)
    {
        scrollable_ = scrollable;
    }

    bool IsScrollable() const
    {
        return scrollable_;
    }

    void SetVertical(bool isVertical)
    {
        vertical_ = isVertical;
    }

    bool IsVertical() const
    {
        return vertical_;
    }

    void SetScrollDuration(float scrollDuration)
    {
        scrollDuration_ = scrollDuration;
    }

    float GetScrollDuration() const
    {
        return scrollDuration_;
    }

    void SetChangeEventId(const EventMarker& changeEventId)
    {
        changeEventId_ = changeEventId;
    }

    const EventMarker& GetChangeEventId() const
    {
        return changeEventId_;
    }

    void SetDomChangeEventId(const EventMarker& domChangeEventId)
    {
        domChangeEventId_ = domChangeEventId;
    }

    const EventMarker& GetDomChangeEventId() const
    {
        return domChangeEventId_;
    }

    void SetUseProxy(bool useProxy)
    {
        useProxy_ = useProxy;
    }

private:
    RefPtr<TabController> controller_;

    bool scrollable_ = true; // the default value is true
    bool vertical_ = false;  // the tab is vertical or not, default value is false
    bool useProxy_ = false;
    float scrollDuration_ = DEFAULT_TABCONTENT_ANIMATION_DURATION;

    EventMarker changeEventId_;
    EventMarker domChangeEventId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTENT_COMPONENT_H
