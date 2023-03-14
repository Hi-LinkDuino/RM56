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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_RENDER_GRID_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_RENDER_GRID_H

#include <functional>
#include <map>
#include <vector>

#include "core/components/common/layout/constants.h"
#include "core/components/grid/grid_controller.h"
#include "core/components/scroll/scrollable.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

typedef std::map<KeyDirection, int32_t> RelationMap;
typedef std::map<int32_t, RelationMap> ChildRelationsMap;

class RenderGrid : public RenderNode {
    DECLARE_ACE_TYPE(RenderGrid, RenderNode);

public:
    using BuildGridDataFunc = std::function<void(int32_t index, int32_t count)>;
    using OnScrollFunc = std::function<void(const std::string&)>;

    RenderGrid();
    ~RenderGrid() override = default;

    static RefPtr<RenderNode> Create();

    void OnChildAdded(const RefPtr<RenderNode>& renderNode) override;

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    void RegisterCallback(const BuildGridDataFunc& callback)
    {
        callback_ = callback;
    }

    Offset LimitOffset(const Offset& offset);

    // update offset with delta value.
    bool UpdateOffset(const Offset& delta);

    // Support to grid element response focus event.
    int32_t RequestNextFocus(bool vertical, bool reverse, Offset position);
    void HandleOnFocus(int32_t focusIndex);
    void HandleOnBlur();

    bool GetFocusState() const
    {
        return focused_;
    }

    int32_t GetFocusIndex() const
    {
        return focusIndex_;
    }

    int32_t GetCurrentIndex() const;

    double GetCurrentPosition() const;

    void JumpTo(int32_t index);

    void AnimateTo(int32_t index, float duration, const RefPtr<Curve>& curve);

    void JumpTo(double position);

    void AnimateTo(double position, float duration, const RefPtr<Curve>& curve);

    void ScrollBy(int32_t pixelX, int32_t pixelY, bool smooth);

    void HandleTouchDown();

    void HandleTouchUp();

    void HandleTouchCancel();

    void HandleScrollEnd();

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    LayoutParam MakeInnerLayoutParam(int32_t columnSpan) const;

    // If the cache is not enough during the traversal, the request of the child node is performed.
    void RequestChildIfNeeded();

    // Traverses all children to update render flag and position.
    void Traverse();

    template<typename T>
    T MakeValue(double mainValue, double crossValue) const
    {
        return isVertical_ ? T(crossValue, mainValue) : T(mainValue, crossValue);
    }

    void CalculateAxisSize();

    double GetMainSize(const Size& size) const
    {
        return isVertical_ ? size.Height() : size.Width();
    }

    double GetCrossSize(const Size& size) const
    {
        return isVertical_ ? size.Width() : size.Height();
    }

    Offset GetChildOffset(const RefPtr<RenderNode>& child) const;

    // Sets child position, the mainAxis does not contain the offset.
    void SetChildPosition(const RefPtr<RenderNode>& child, double mainSize, int32_t gridPos, int32_t columnSpan);

    // If child is not full visible, move the layer and make it full visible.
    void MoveChildToViewPort(int32_t index);

    // Get the target child by index.
    RefPtr<RenderNode> GetChildByIndex(int32_t index) const;

    // Get the center position for the target child.
    Offset GetCenterPosition(const Offset& pos, const Size& size) const;

    // If the child is visible in the view port.
    bool IsChildVisible(const RefPtr<RenderNode>& node) const;

    // Build Position relation Map
    void BuildPositionRelations(int32_t gridPos, int32_t columnSpan, int32_t index);

    int32_t FocusMove(const Offset& position, const std::function<bool(int32_t)>& condition, KeyDirection keyDirection,
        bool checkChildVisible);
    // Handle direction key up
    int32_t focusMoveUp(const Offset& position);

    // Handle direction key down
    int32_t focusMoveDown(const Offset& position);

    // Handle direction key left
    int32_t focusMoveLeft(const Offset& position);

    // Handle direction key right
    int32_t focusMoveRight(const Offset& position);

    void ResetScrollable();

    void InitialGridProp();

    int32_t GetChildSpan(const RefPtr<RenderNode>& child) const;

    void OnScroll(double scrollX, double scrollY, int32_t scrollState) const;
    void OnScrollBottom() const;
    void OnScrollTop() const;
    void OnScrollEnd() const;
    void OnScrollTouchUp() const;

    // This saves the relative main axis value of all child nodes without grid drag offset.
    std::vector<double> posOfChildren_;
    std::vector<int32_t> spanOfChildren_;

    bool isVertical_ = false;
    bool updateFlag_ = false;
    bool childrenChanged_ = true;
    FlexDirection direction_ = FlexDirection::ROW;
    FlexAlign crossAxisAlign_ = FlexAlign::STRETCH;
    BuildGridDataFunc callback_;
    OnScrollFunc onScroll_;
    OnScrollFunc onScrollBottom_;
    OnScrollFunc onScrollTop_;
    OnScrollFunc onScrollEnd_;
    OnScrollFunc onScrollTouchUp_;

    Offset offset_;
    RefPtr<Scrollable> scrollable_;
    RefPtr<GridController> gridController_;
    RefPtr<Animator> animator_;
    RefPtr<RawRecognizer> touchRecognizer_; // For receive touch event
    bool onTouch_ = false;
    bool reachTop_ = false;
    bool reachBottom_ = false;
    bool reachBetween_ = false;
    bool triggerTouchUp_ = false;
    int32_t focusIndex_ = -1;
    int32_t focused_ = false;
    ChildRelationsMap childRelations_; // Key:index Value:Relations
    std::map<int32_t, int32_t> indexPositions_; // Key:Child index Value:Grid
    std::map<int32_t, int32_t> gridPositions_; // Key:Grid Value:Child index

    double totalMainSize_ = 0.0; // Width When vertical, Height when horizontal
    double mainSize_ = 0.0;
    double crossSize_ = 0.0;
    double gridLen_ = 0.0;
    double gridWidth_ = -1.0;
    double gridHeight_ = -1.0;
    int32_t index_ = 0;
    int32_t count_ = 0;
    int32_t maxCount_ = 0;
    int32_t columnCount_ = 0;
    int32_t columnExtent_ = 0;
    int32_t cachedCount_ = 10; // the default value of cached child number.
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_RENDER_GRID_H
