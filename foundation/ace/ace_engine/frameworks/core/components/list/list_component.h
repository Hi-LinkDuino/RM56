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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_COMPONENT_H

#include "base/utils/macros.h"
#include "core/animation/simple_spring_chain.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/common/rotation/rotation_controller.h"
#include "core/components/list/list_element.h"
#include "core/components/list/list_item_component.h"
#include "core/components/list/list_theme.h"
#include "core/components/list/render_list.h"
#include "core/components/scroll/scroll_edge_effect.h"
#include "core/components/scroll/scroll_position_controller.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

inline constexpr int32_t INDEX_EXPAND_ALL = -999;

class ACE_EXPORT ListComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(ListComponent, ComponentGroup);

public:
    ListComponent() = default;
    explicit ListComponent(const std::list<RefPtr<Component>>& children);
    ~ListComponent() override = default;

    void InsertChild(int32_t position, const RefPtr<Component>& child) override;
    void AppendChild(const RefPtr<Component>& child) override;
    void RemoveChild(const RefPtr<Component>& child) override;
    void SetGroupState(int32_t expandIndex, bool expand);

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void SetScrollPage(bool scrollPage)
    {
        scrollPage_ = scrollPage;
    }

    void SetChainAnimation(bool chainAnimation)
    {
        chainAnimation_ = chainAnimation;
    }

    void SetOnScroll(const EventMarker& onScroll)
    {
        onScroll_ = onScroll;
    }

    void SetOnScrollBottom(const EventMarker& onScrollBottom)
    {
        onScrollBottom_ = onScrollBottom;
    }

    void SetOnScrollTop(const EventMarker& onScrollTop)
    {
        onScrollTop_ = onScrollTop;
    }

    void SetOnScrollEnd(const EventMarker& onScrollEnd)
    {
        onScrollEnd_ = onScrollEnd;
    }

    void SetOnScrollTouchUp(const EventMarker& onScrollTouchUp)
    {
        onScrollTouchUp_ = onScrollTouchUp;
    }

    bool GetScrollPage() const
    {
        return scrollPage_;
    }

    bool GetChainAnimation() const
    {
        return chainAnimation_;
    }

    FlexDirection GetDirection() const
    {
        return direction_;
    }

    void SetDirection(FlexDirection direction)
    {
        if (direction == FlexDirection::COLUMN_REVERSE || direction == FlexDirection::ROW_REVERSE) {
            LOGE("Not support. direction: %{public}d", direction);
            return;
        }
        direction_ = direction;
    }

    int32_t GetTotalCount() const
    {
        return totalCount_;
    }

    int32_t GetItemsCount() const
    {
        return GetChildren().size();
    }

    void SetTotalCount(int32_t totalCount)
    {
        if (totalCount < 0) {
            LOGE("Invalid TotalCount %{public}d", totalCount);
            return;
        }
        LOGD("SetTotalCount to %{public}d.", totalCount);
        totalCount_ = totalCount;
    }

    const EventMarker& GetOnRequestItem() const
    {
        return onRequestItem_;
    }

    void SetOnRequestItem(const EventMarker& onRequestItem)
    {
        onRequestItem_ = onRequestItem;
    }

    int32_t GetCachedCount() const
    {
        return cachedCount_;
    }

    int32_t GetBeginIndex() const
    {
        return beginIndex_;
    }

    int32_t GetEndIndex() const
    {
        return endIndex_;
    }

    int32_t GetRepeatedLength() const
    {
        return repeatedLength_;
    }

    int32_t GetIndexOffset() const
    {
        return indexOffset_;
    }

    void SetCachedCount(int32_t cachedCount)
    {
        if (cachedCount <= 0) {
            LOGE("Invalid CachedCount %{public}d", cachedCount);
            return;
        }
        LOGD("SetCachedCount to %{public}d.", cachedCount);
        cachedCount_ = cachedCount;
    }

    void SetBeginIndex(int32_t beginIndex)
    {
        beginIndex_ = beginIndex;
    }

    void SetEndIndex(int32_t endIndex)
    {
        endIndex_ = endIndex;
    }

    void SetRepeatedLength(int32_t repeatedLength)
    {
        repeatedLength_ = repeatedLength;
    }

    void SetIndexOffset(int32_t indexOffset)
    {
        indexOffset_ = indexOffset;
    }

    RefPtr<ScrollPositionController> GetPositionController() const
    {
        return positionController_;
    }

    void SetPositionController(const RefPtr<ScrollPositionController>& controller)
    {
        positionController_ = controller;
    }

    const RefPtr<ScrollEdgeEffect>& GetScrollEffect() const
    {
        return scrollEffect_;
    }

    void SetScrollEffect(const RefPtr<ScrollEdgeEffect>& scrollEffect)
    {
        scrollEffect_ = scrollEffect;
    }

    const EventMarker& GetOnScroll() const
    {
        return onScroll_;
    }

    const EventMarker& GetOnScrollBottom() const
    {
        return onScrollBottom_;
    }

    const EventMarker& GetOnScrollTop() const
    {
        return onScrollTop_;
    }

    const EventMarker& GetOnScrollEnd() const
    {
        return onScrollEnd_;
    }

    const EventMarker& GetOnScrollTouchUp() const
    {
        return onScrollTouchUp_;
    }

    void InitStyle(const RefPtr<ListTheme>& theme);

    const Dimension& GetGradientWidth() const
    {
        return gradientWidth_;
    }

    void SetGradientWidth(const Dimension& gradientWidth)
    {
        if (gradientWidth.Value() < 0.0) {
            LOGE("Invalid GradientWidth %{public}lf", gradientWidth_.Value());
            return;
        }
        LOGD("SetGradientWidth to %{public}lf.", gradientWidth_.Value());
        gradientWidth_ = gradientWidth;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    void SetBackgroundColor(const Color& backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    void SetFlexAlign(FlexAlign flexAlign);

    void SetColumnCount(int32_t count);

    void SetColumnExtent(int32_t extent);

    void SetItemExtent(const Dimension& itemExtent);

    void SetWidth(double width);

    void SetHeight(double height);

    void SetInRefresh(bool inRefresh)
    {
        inRefresh_ = inRefresh;
    }

    void SetSupportItemCenter(bool center)
    {
        supportItemCenter_ = center;
    }

    bool GetSupportItemCenter() const
    {
        return supportItemCenter_;
    }

    FlexAlign GetFlexAlign() const
    {
        return flexAlign_;
    }

    int32_t GetColumnCount() const
    {
        return columnCount_;
    }

    int32_t GetColumnExtent() const
    {
        return columnExtent_;
    }

    const Dimension& GetItemExtent() const
    {
        return itemExtent_;
    }

    double GetWidth() const
    {
        return width_;
    }

    double GetHeight() const
    {
        return height_;
    }

    bool NeedPreBuild() const
    {
        return needPreBuild_;
    }

    bool NeedUpdateElement() const
    {
        return needUpdateElement_;
    }

    bool IsInRefresh() const
    {
        return inRefresh_;
    }

    void MarkNeedUpdateElement(bool needUpdateElement)
    {
        needUpdateElement_ = needUpdateElement;
    }

    void SetScrollBar(const RefPtr<ScrollBar>& scrollBar)
    {
        scrollBar_ = scrollBar;
    }

    RefPtr<ScrollBar> GetScrollBar() const
    {
        return scrollBar_;
    }

    void SetRightToLeft(bool rightToLeft)
    {
        rightToLeft_ = rightToLeft;
    }

    bool GetRightToLeft() const
    {
        return rightToLeft_;
    }

    void SetUpdateEffect(bool updateEffect)
    {
        updateEffect_ = updateEffect;
    }

    bool GetUpdateEffect() const
    {
        return updateEffect_;
    }

    void UpdateListItemIndex()
    {
        int32_t index = 0; // Update the item index by the order in list.
        for (const auto& iter : GetChildren()) {
            auto listItemComponent = ListItemComponent::GetListItem(iter);
            if (listItemComponent) {
                listItemComponent->SetIndex(index++);
            }
        }
    }

    void SetPageReady(bool pageReady)
    {
        pageReady_ = pageReady;
    }

    bool GetPageReady() const
    {
        return pageReady_;
    }

    bool IsCenterLayout() const
    {
        return isCenterLayout_;
    }

    void MarkCenterLayout(bool isCenter)
    {
        isCenterLayout_ = isCenter;
    }

    bool IsItemScale() const
    {
        return itemScale_;
    }

    void SetItemScale(bool itemScale)
    {
        itemScale_ = itemScale;
    }

    const RefPtr<RotationController>& GetRotationController() const
    {
        return rotationController_;
    }

    void SetChainProperty(const SpringChainProperty& property)
    {
        chainProperty_ = property;
    }

    const SpringChainProperty& GetChainProperty() const
    {
        return chainProperty_;
    }

    void SetOverSpringProperty(const RefPtr<SpringProperty>& property)
    {
        if (property && property->IsValid()) {
            overSpringProperty_ = property;
        }
    }

    void SetOverSpringProperty(double mass, double stiffness, double damping)
    {
        overSpringProperty_ = AceType::MakeRefPtr<SpringProperty>(mass, stiffness, damping);
    }

    const RefPtr<SpringProperty>& OverSpringProperty()
    {
        return overSpringProperty_;
    }

    bool NeedDivider() const
    {
        return needDivider_;
    }

    void MarkNeedDivider(bool needDivider)
    {
        needDivider_ = needDivider;
    }

    const Dimension& GetDividerOrigin() const
    {
        return dividerOrigin_;
    }

    void SetDividerOrigin(const Dimension& origin)
    {
        dividerOrigin_ = origin;
    }

    const Dimension& GetDividerLength() const
    {
        return dividerLength_;
    }

    void SetDividerLength(const Dimension& length)
    {
        dividerLength_ = length;
    }

    const Color& GetDividerColor() const
    {
        return dividerColor_;
    }

    void SetDividerColor(const Color& color)
    {
        dividerColor_ = color;
    }

    const Dimension& GetDividerHeight() const
    {
        return dividerHeight_;
    }

    void SetDividerHeight(const Dimension& dividerHeight)
    {
        dividerHeight_ = dividerHeight;
    }

    const EventMarker& GetOnRotateId() const
    {
        return onRotateId_;
    }

    void SetOnRotateId(const EventMarker& onRotateId)
    {
        onRotateId_ = onRotateId;
    }

    void SetScrollVibrate(bool scrollVibrate)
    {
        scrollVibrate_ = scrollVibrate;
    }

    bool GetScrollVibrate()
    {
        return scrollVibrate_;
    }

    bool IsRotationVibrate() const
    {
        return rotationVibrate_;
    }

    void MarkNeedRotationVibrate(bool needVibrate)
    {
        rotationVibrate_ = needVibrate;
    }

    void SetAccessibilityDisabled(bool disable)
    {
        accessibilityDisabled_ = disable;
    }

    bool IsAccessibilityDisabled() const
    {
        return accessibilityDisabled_;
    }

private:
    EventMarker onRequestItem_;
    EventMarker onScroll_;
    EventMarker onScrollBottom_;
    EventMarker onScrollTop_;
    EventMarker onScrollEnd_;
    EventMarker onScrollTouchUp_;
    EventMarker onRotateId_;
    Dimension gradientWidth_;
    Color backgroundColor_ = Color::WHITE;
    FlexDirection direction_ = FlexDirection::COLUMN;
    bool scrollPage_ { false };
    bool chainAnimation_ { false };
    int32_t totalCount_ { 0 };
    int32_t cachedCount_ { 1 }; // the default value of cached child number.
    int32_t beginIndex_ { LIST_PARAM_INVAID };
    int32_t endIndex_ { LIST_PARAM_INVAID };
    int32_t repeatedLength_ { 0 };
    int32_t indexOffset_ { 0 };
    RefPtr<ScrollPositionController> positionController_;
    RefPtr<RotationController> rotationController_ = AceType::MakeRefPtr<RotationController>();
    RefPtr<ScrollEdgeEffect> scrollEffect_;
    FlexAlign flexAlign_ { FlexAlign::STRETCH };
    double width_ { DEFAULT_GRID_WIDTH };
    double height_ { DEFAULT_GRID_HEIGHT };
    int32_t columnCount_ { DEFAULT_COLUMN_COUNT };
    int32_t columnExtent_ { DEFAULT_COLUMN_EXTENT };
    Dimension itemExtent_;
    bool needPreBuild_ = false;
    bool needUpdateElement_ = false;
    bool rightToLeft_ = false;
    RefPtr<ScrollBar> scrollBar_;
    bool inRefresh_ = false;
    bool supportItemCenter_ = false;
    bool updateEffect_ = false;
    bool pageReady_ = false;
    bool itemScale_ = false;
    bool isCenterLayout_ = false;
    SpringChainProperty chainProperty_;
    RefPtr<SpringProperty> overSpringProperty_;
    WeakPtr<ListElement> listElement_;

    // divider style
    Dimension dividerOrigin_;
    Dimension dividerLength_;
    Dimension dividerHeight_ = DIVIDER_DEFAULT_HEIGHT;
    Color dividerColor_;
    bool needDivider_ = false;

    bool scrollVibrate_ = true;
    bool rotationVibrate_ = false;
    bool accessibilityDisabled_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_LIST_COMPONENT_H
