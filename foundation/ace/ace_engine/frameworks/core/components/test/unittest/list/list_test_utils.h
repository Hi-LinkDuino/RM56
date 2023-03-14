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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_LIST_LIST_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_LIST_LIST_TEST_UTILS_H

#include <core/components/list/tv_interactive_effect.h>
#include <core/components/list/watch_interactive_effect.h>
#include <core/components/scroll/render_multi_child_scroll.h>

#include "core/components/box/render_box.h"
#include "core/components/list/list_item_group_component.h"
#include "core/components/list/render_list.h"
#include "core/components/list/render_list_item.h"
#include "core/components/list/render_list_item_group.h"

namespace OHOS::Ace {
namespace {

using ListEventCallback = std::function<void(const std::string&, const std::string&)>;

class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    MockRenderBox() = default;
    ~MockRenderBox() override = default;
};

class MockRenderList final : public RenderList {
    DECLARE_ACE_TYPE(MockRenderList, RenderList);

public:
    MockRenderList() = default;
    ~MockRenderList() override = default;

    bool IsWatch() const
    {
        return SystemProperties::GetDeviceType() == DeviceType::WATCH;
    }

    bool IsTV() const
    {
        return SystemProperties::GetDeviceType() == DeviceType::TV;
    }

    double GetViewPortSize()
    {
        return GetMainSize(viewPort_);
    }
};

class MockRenderListItem final : public RenderListItem {
    DECLARE_ACE_TYPE(MockRenderListItem, RenderListItem);

public:
    MockRenderListItem() = default;
    ~MockRenderListItem() override = default;
    const RefPtr<InteractiveEffect>& GetEffectController() const
    {
        return focusController_;
    }

    void SetPrimary(bool primary)
    {
        primary_ = primary;
    }

    bool NeedDivider() const
    {
        return needDivider_;
    }

    Dimension GetDividerHeight() const
    {
        return dividerHeight_;
    }

    Dimension GetDividerOrigin() const
    {
        return dividerOrigin_;
    }

    Dimension GetDividerLength() const
    {
        return dividerLength_;
    }

    Color GetDividerColor() const
    {
        return dividerColor_;
    }

    void MarkNeedDivider()
    {
        needDivider_ = true;
    }

    void SetDividerHeight(const Dimension& height)
    {
        dividerHeight_ = height;
    }

    void PerformLayout() override;
    void HandleClicked();
    void PlayPressDownAnimation();
    void ResetFocusEffect(const WeakPtr<PipelineContext>& context);
};

void MockRenderListItem::PerformLayout()
{
    RenderListItem::PerformLayout();
}

void MockRenderListItem::HandleClicked()
{
    RenderListItem::HandleClicked();
}

void MockRenderListItem::PlayPressDownAnimation()
{
    RenderListItem::PlayPressDownAnimation();
}

void MockRenderListItem::ResetFocusEffect(const WeakPtr<PipelineContext>& context)
{
#ifdef WEARABLE_PRODUCT
    if (SystemProperties::GetDeviceType() == DeviceType::WATCH) {
        focusController_ = AceType::MakeRefPtr<WatchInteractiveEffect>(context);
#else
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        focusController_ = AceType::MakeRefPtr<TVInteractiveEffect>(context);
#endif
    } else {
        focusController_ = AceType::MakeRefPtr<InteractiveEffect>(GetContext());
    }

    auto pipelineContext = context.Upgrade();
    if (pipelineContext) {
        focusController_->Initialize(pipelineContext->GetThemeManager());
    }
    focusController_->UpdateContext(context);
    focusController_->SetItemNode(AceType::WeakClaim(this));
}

class MockRenderScroll final : public RenderMultiChildScroll {
    DECLARE_ACE_TYPE(MockRenderScroll, RenderMultiChildScroll);

public:
    MockRenderScroll() = default;
    ~MockRenderScroll() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    double GetOverScroll();
    RefPtr<ScrollEdgeEffect> GetScrollEffect();
};

double MockRenderScroll::GetOverScroll()
{
    return overScroll_;
}

void MockRenderScroll::Update(const RefPtr<Component>& component)
{
    return RenderMultiChildScroll::Update(component);
}

void MockRenderScroll::PerformLayout()
{
    return RenderMultiChildScroll::PerformLayout();
}

RefPtr<ScrollEdgeEffect> MockRenderScroll::GetScrollEffect()
{
    return scrollEffect_;
}

class MockRenderListItemGroup final : public RenderListItemGroup {
    DECLARE_ACE_TYPE(MockRenderListItemGroup, RenderListItemGroup);

public:
    MockRenderListItemGroup() = default;
    ~MockRenderListItemGroup() override = default;

    void SetExpand(bool expand)
    {
        expand_ = expand;
    }

    bool GetExpand() const
    {
        return expand_;
    }

    void SetOnClick(GroupEventFunc onClicked)
    {
        onClicked_ = onClicked;
    }

    void SetOnCollapse(GroupEventFunc onCollapse)
    {
        onCollapse_ = onCollapse;
    }

    void SetOnExpand(GroupEventFunc onExpand)
    {
        onExpand_ = onExpand;
    }

    void SetGroupId(const std::string groupId)
    {
        groupId_ = groupId;
    }

    void HandleClicked();
};

void MockRenderListItemGroup::HandleClicked()
{
    RenderListItemGroup::HandleClicked();
}

class MockEventHandler : public AceEventHandler {
    void HandleAsyncEvent(const EventMarker& eventMarker) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent, bool& result) override {};
    void HandleSyncEvent(
        const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) override {};
};

class TestListEventHander : public MockEventHandler {
public:
    explicit TestListEventHander(ListEventCallback eventCallback) : eventCallback_(eventCallback) {}
    ~TestListEventHander() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, param);
        }
    };

private:
    ListEventCallback eventCallback_;
};

} // namespace

class ListTestUtils {
public:
    static RefPtr<RenderBox> CreateRenderBox(double width, double height);
    static RefPtr<RenderNode> CreateRenderItem(
        double width, double height, const RefPtr<PipelineContext>& context, bool primary_ = false);
    static RefPtr<RenderNode> CreateRenderItemGroup(double width, double height, const RefPtr<PipelineContext>& context,
        bool needItem = false, bool specifyPrimary = true);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_LIST_LIST_TEST_UTILS_H
