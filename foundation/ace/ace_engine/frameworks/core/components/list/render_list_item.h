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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_ITEM_H

#include "core/common/vibrator/vibrator.h"
#include "core/components/common/properties/border.h"
#include "core/components/display/render_display.h"
#include "core/components/list/interactive_effect.h"
#include "core/components/list/layout_manager.h"
#include "core/components/list/list_item_component.h"
#include "core/components/scroll/scroll_position_controller.h"
#include "core/event/ace_event_helper.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

constexpr double RADIUS_START = 10000.0;
constexpr double RADIUS_END = 1000.0;

class RenderList;
class RenderListItem : public RenderNode {
    DECLARE_ACE_TYPE(RenderListItem, RenderNode);

public:
    ~RenderListItem() override = default;
    void Dump() override;

    static RefPtr<RenderNode> Create();
    void HandleFocusEvent(bool focus, bool isInGroup = false);
    void ShowFocusAnimation(bool focus, const Rect& listRect, double scale = TV_ITEM_SCALE);
    void UpdateItemFocusRect(double scale);
    void NotifyGroupPrimaryChange();

    std::string GetType() const
    {
        return type_;
    }

    void SetIndex(int32_t index)
    {
        index_ = index;
    }

    int32_t GetIndex() const
    {
        return index_;
    }

    uint32_t GetFlags() const
    {
        return flags_;
    }

    bool TestFlag(uint32_t flag) const
    {
        return (flags_ & flag);
    }

    void RemoveFlag(uint32_t flag)
    {
        flags_ &= ~flag;
    }

    double GetPositionInList() const;

    int32_t GetOperation() const
    {
        return op_;
    }

    void SetOperation(int32_t op)
    {
        op_ = op;
    }

    bool IsFocused() const
    {
        return focused_;
    }

    void SetFocused(bool focused)
    {
        focused_ = focused;
    }

    void SetGrid(int32_t grid)
    {
        grid_ = grid;
    }

    int32_t GetGrid() const
    {
        return grid_;
    }

    int32_t GetColumnSpan() const
    {
        return columnSpan_;
    }

    bool GetSticky() const
    {
        return sticky_;
    }

    void SetSticky(bool sticky)
    {
        sticky_ = sticky;
    }

    StickyMode GetStickyMode() const
    {
        return stickyMode_;
    }

    bool GetNearByOpacitySticky() const
    {
        return nearByOpacitySticky_;
    }

    void SetNearByOpacitySticky(bool nearBy)
    {
        nearByOpacitySticky_ = nearBy;
    }

    bool GetClonedBySticky() const
    {
        return clonedBySticky_;
    }

    void SetClonedBySticky(bool clonedBySticky)
    {
        clonedBySticky_ = clonedBySticky;
    }

    bool GetSupportScale() const
    {
        return supportScale_;
    }

    bool GetSupportOpacity() const
    {
        return supportOpacity_;
    }

    bool GetSupportClick() const
    {
        return supportClick_;
    }

    bool GetPrimary() const
    {
        return primary_;
    }

    void SetPrimaryChange(bool change)
    {
        primaryChange_ = change;
    }

    bool GetPrimaryChange() const
    {
        return primaryChange_;
    }

    void SetPrimary(bool primary)
    {
        if (primary_ != primary) {
            primaryChange_ = true;
            auto item = AceType::DynamicCast<ListItemComponent>(itemComponent_);
            if (item) {
                item->SetPrimary(primary);
            }
        }
        primary_ = primary;
    }

    bool GetCurPrimary() const
    {
        return curPrimary_;
    }

    void SetCurPrimary(bool primary)
    {
        curPrimary_ = primary;
    }

    bool GetRedraw() const
    {
        return redraw_;
    }

    void SetRedraw(bool redraw)
    {
        redraw_ = redraw;
    }

    void HandleItemEffect(bool isFromRotate = false);

    // judge is item at center of viewport or not, used on watch
    bool IsItemCenter(bool isVertical, Size viewport);

    ItemState GetCurrentState() const
    {
        return currentState_;
    }

    void SetCurrentState(ItemState state)
    {
        currentState_ = state;
    }

    using ListItemClickedFunc = std::function<void()>;
    void RegisterClickedCallback(const ListItemClickedFunc& clicked)
    {
        clicked_ = clicked;
    }

    static RefPtr<RenderListItem> GetRenderListItem(const RefPtr<RenderNode>& renderNode)
    {
        RefPtr<RenderNode> listItem = renderNode;
        while (listItem) {
            if (AceType::InstanceOf<RenderListItem>(listItem)) {
                return AceType::DynamicCast<RenderListItem>(listItem);
            }

            auto& children = listItem->GetChildren();
            if (children.empty()) {
                return nullptr;
            }
            listItem = children.front();
        }
        return nullptr;
    }

    const RefPtr<ScrollPositionController>& GetScrollController() const
    {
        return scrollController_;
    }

    void SetScrollController(const RefPtr<ScrollPositionController>& controller)
    {
        scrollController_ = controller;
    }

    double GetStickyRadius() const
    {
        return stickyRadius_;
    }

    void SetExpandOpacity(int32_t opacity)
    {
        expandOpacity_ = opacity;
    }

    void SetStretch(bool isStretch)
    {
        isStretch_ = isStretch;
    }

    void SetPreLayoutSize(const Size& size)
    {
        preLayoutSize_ = size;
    }

    const Size& GetPreLayoutSize() const
    {
        return preLayoutSize_;
    }

    double GetScaleFactor() const
    {
        return scaleFactor_;
    }

    double GetOpacityFactor() const
    {
        return opacityFactor_;
    }

    TransitionEffect GetTransitionEffect() const
    {
        return transitionEffect_;
    }

    FlexAlign GetSelfAlign() const
    {
        return selfAlign_;
    }

    void SetSelfAlign(FlexAlign selfAlign)
    {
        selfAlign_ = selfAlign;
    }

    void OnGroupClicked();
    void HandleStickyEvent(bool sticky);
    void HandleClicked();
    void CalculateScaleFactorOnWatch();
    void RunCardTransitionAnimation(double shiftHeight);
    void StopCardTransitionAnimation();
    RRect GetRRect() const;

protected:
    RenderListItem();

    void Update(const RefPtr<Component>& component) override;
    void ResetFocusEffect();
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    void UpdateAccessibilityAttr();
    void MoveToViewPort();
    void PlayPressDownAnimation();
    void PlayPressUpAnimation();
    void OnCancelPressAnimation() override;
    RefPtr<RenderList> GetRenderList() const;
    bool NeedDivider();
    void PerformLayout() override;
    bool IsListVertical();
    bool IsLastItem();
    Size GetPaintSize() const;
    EdgePx GetMarginInPx() const;
    Border GetFocusBorder() const;

    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<RawRecognizer> touchRecognizer_;
    RefPtr<Vibrator> vibrator_;
    RefPtr<InteractiveEffect> focusController_;
    RefPtr<RenderDisplay> renderDisplay_;
    bool needVibrate_ = true;
    bool rotationVibrate_ = false;
    bool supportScale_ = true;
    bool supportOpacity_ = false;
    bool supportClick_ = true;
    bool isStretch_ = false;
    bool isTitle_ = false;
    int32_t expandOpacity_ = UINT8_MAX;
    bool primary_ = false;
    bool curPrimary_ = false;
    bool primaryChange_ = false;
    bool needDivider_ = false;
    bool isActive_ = false;
    Color clickColor_ = Color::TRANSPARENT;
    FlexAlign selfAlign_ = FlexAlign::AUTO;

    Dimension dividerOrigin_;
    Dimension dividerLength_;
    Dimension dividerHeight_ = DIVIDER_DEFAULT_HEIGHT;
    Color dividerColor_;
    StickyMode stickyMode_ = StickyMode::NONE;
    double scaleFactor_ = 1.0;
    double opacityFactor_ = 1.0;
    RRect focusAnimationRRect_;
    bool makeCardTransition_ = false;

private:
    void Initialize();
    Offset GetPaintOffset() const;
    std::function<void()> clickEvent_;
    std::function<void(const std::string&)> stickyEvent_;
    Size preLayoutSize_;
    int32_t index_ = -1;
    uint32_t flags_ = 0;
    int32_t op_ = LIST_ITEM_OP_NONE;
    std::string type_;
    Color focusAnimationColor_ = Color::WHITE;
    bool focused_ = false;
    int32_t grid_ = -1;
    int32_t columnSpan_ = 1;
    ItemState lastState_ = ItemState::NONE;
    ItemState currentState_ = ItemState::NONE;
    RefPtr<ScrollPositionController> scrollController_;
    bool pressAnimation_ = false;
    bool redraw_ = false;
    bool sticky_ = false;
    bool nearByOpacitySticky_ = false;
    bool clonedBySticky_ = false;
    double stickyRadius_ = RADIUS_END;
    RefPtr<Component> itemComponent_;

    ListItemClickedFunc clicked_;

    TransitionEffect transitionEffect_ = TransitionEffect::NONE;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_RENDER_LIST_ITEM_H
