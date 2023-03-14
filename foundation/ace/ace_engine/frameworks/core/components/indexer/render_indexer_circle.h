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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_RENDER_INDEXER_CIRCLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_RENDER_INDEXER_CIRCLE_H

#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/arc/render_arc.h"
#include "core/components/indexer/render_indexer.h"

namespace OHOS::Ace {

constexpr double INDEXER_HOT_RGN_RADIUS = 23.0;
constexpr int32_t CHANGE_FROM_CLICK = -1;
constexpr int32_t CHANGE_FROM_LIST = 0;
constexpr int32_t INDEXER_ANIMATION_DURATION = 300;

enum class IndexerItemStatus { EXPAND, COLLAPSE, ANIMATION };

class RenderIndexerCircle : public RenderIndexer {
    DECLARE_ACE_TYPE(RenderIndexerCircle, RenderIndexer);

public:
    RenderIndexerCircle() = default;
    ~RenderIndexerCircle() override = default;
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool TouchTest(const Point& globalPoint, const Point& parentLocalPoint, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;
    void HandleTouchDown(const TouchEventInfo& info) override;
    void HandleTouchUp(const TouchEventInfo& info) override;
    void HandleTouchMove(const TouchEventInfo& info) override;
    int32_t GetTouchedItemIndex(const Offset& touchPosition) override;
    void InitIndexTable() override;
    void HandleListMoveItems(int32_t curHeadIndex) override;
    void UpdateCurrentSectionItem(int32_t curSection) override;
    void BeginFocusAnimation(int32_t originIndex, int32_t targetIndex) override;
    bool RotationTest(const RotationEvent& event) override;
    bool NeedProcess(const RefPtr<RenderNode>& item) const override;
    bool NeedRotation() const override;

    bool IsValidArc();
    bool IsValidIndicatorBox();
    bool IsValidBubbleBox() override;
    bool IsItemColorEnabled() const
    {
        return itemColorEnabled_;
    }

    IndexerItemStatus GetIndexerStatus()
    {
        return curStatus_;
    }

protected:
    bool NeedChangeIndexer(int32_t index);
    bool ChangeLanguage(int32_t index);
    void HandleRotation(double value);
    void CollapseItems();
    void ExpandItems();

    // marker item focused
    void SetItemsFocused(int32_t index, bool force = false, bool smooth = true);

    // calculate index in items by position index in circle
    int32_t GetActualItemIndex(int32_t positionIndex);

    // calculate position index in circle by index in items
    int32_t GetPositionItemIndex(int32_t index);

    // identify indexer status
    IndexerItemStatus nextStatus_ = IndexerItemStatus::EXPAND;
    IndexerItemStatus curStatus_ = IndexerItemStatus::EXPAND;

    RefPtr<Animator> collapseController_;       // control expand or collapse animation
    RefPtr<Animator> focusController_;          // control click to change focus animation
    RefPtr<Animator> collapseScrollController_; // control focus change when scroll

    int32_t curItemType_ = 0;
    int32_t itemMaxCount_ = INDEXER_ITEM_MAX_COUNT; // the max count of the items while in expand mode
    int32_t currentCount_ = INDEXER_ITEM_MAX_COUNT; // current shown item nums exclude collapse item
    double arcHeadOffset_ = 0.0;    // the offset of the arc, item # to the original position, in radians
    double arcMaxLen_ = M_PI * 2.0; // the max length of the arc, in radians

private:
    void InitBubbleBox(const Size& size);
    void InitIndicatorBox();
    void ResetItemsPosition();
    void BuildArcAnimation();
    void BeginArcAnimation(bool collapse);
    void BeginCollapseScrollAnimation(double originOffset, double targetOffset);
    void HandleCollapseScrollAnimationStop();
    void HandleArcAnimationStart();
    void HandleArcAnimationStop();
    void InitFocusInterpolator(double origin, double target, int32_t originIndex, int32_t targetIndex);
    void InitCollapseScrollInterpolator(double origin, double target);
    void SetIndexerItemStatus(bool inAnimation, int32_t originIndex, int32_t targetIndex);
    void UpdateItemBackground(double value, int32_t originIndex, int32_t targetIndex);
    void UpdateItemBackground(double value, int32_t index);
    void UpdateArcTail(double value);
    int32_t GetNearestItem(const Offset& position, bool move);
    int32_t GetNearestItem(double position);
    double GetPositionAngle(const Offset& position);
    bool CollapseItemHitTest(const Offset& position);
    void FireIndexerChangeEvent() const;

    // update all item visible status with range from head to tail
    void SetItemsVisible(double arcHead, double arcTail);

    // update specific item's position
    void SetItemPosition(const RefPtr<RenderNode>& item, int32_t positionIndex);
    void SetItemPosition(const RefPtr<RenderNode>& item, double angle);

    // update all items's position by arc offset
    void UpdateItemsPosition(double arcOffset);

    // update collapse item position
    void SetCollapseItemPosition(double position);

    // handle touch down event when arc is collapsed
    void TouchOnCollapseArc(const Offset& position);

    // rotate items for animation
    void RotateItems(double arcOffset);

    std::function<void(const std::string&)> indexerChangeEvent_;

    double outerRadius_ = 0.0;                   // the outer radius of the arc, the same as half of the layout width
    RefPtr<RenderArc> arc_;                      // reference of the arc render node
    RefPtr<RenderBox> indicatorBox_;             // reference of the indicator box render node
    double arcHeadPosition_ = INDEXER_ARC_BEGIN; // the begin position of the arc, in radians
    double arcTailPosition_ = 2 * M_PI;          // the tail position of the arc, in radians
    int32_t collapseItemCount_ = INDEXER_COLLAPSE_ITEM_COUNT; // the max count of the items while in collapse mode
    double collapseItemPosition_ = INDEXER_ARC_BEGIN;         // position of the collapse item, in radians
    double hotRgnSize_ = INDEXER_HOT_RGN_RADIUS;              // radius for item clicking
    double rotationStepValue_ = 0.0;
    double perItemExtent_ = 2.0 * M_PI / itemMaxCount_;

    bool touching_ = false; // whether the item is in touching
    bool itemColorEnabled_ = true;
    bool hasCollapseItem_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_RENDER_INDEXER_CIRCLE_H
