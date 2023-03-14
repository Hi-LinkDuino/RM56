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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_RENDER_GRID_LAYOUT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_RENDER_GRID_LAYOUT_H

#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <vector>

#include "core/animation/animation.h"
#include "core/animation/animator.h"
#include "core/animation/scroll_motion.h"
#include "core/animation/spring_motion.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout_item.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/stack/stack_element.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/gesture_info.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t DEFAULT_FINGERS = 1;
constexpr int32_t DEFAULT_DURATION = 150;
constexpr int32_t DEFAULT_DISTANCE = 0;

constexpr int32_t DRAG_LEAVE = -1;
constexpr int32_t DRAG_ENTER = 1;
constexpr int32_t NONE = 0;
constexpr double ITEM_ANIMATION_DURATION = 300.0;
constexpr double ITEM_ANIMATION_DURATION_NO = 40.0;

constexpr double GRID_SPRING_MASS = 1.0;
constexpr double GRID_SPRING_STIFF = 228.0;
constexpr double GRID_SPRING_DAMP = 30.0;
constexpr double GRID_SPRING_DAMP_INC = 0;
constexpr double GRID_SPRING_SLIDE_LIMIT = 20.0;

}; // namespace

enum class GridLayoutAnimationAct {
    ANIMATION_NONE = 0,
    ANIMATION_DRAG_MOVE,
    ANIMATION_DRAG_DROP,
    ANIMATION_RESTORE_SCENE,
};

enum class GridSpringGravitationDirect {
    SPRING_NONE = 0,
    SPRING_TO_LEFT,
    SPRING_TO_RIGHT,
    SPRING_TO_UP,
    SPRING_TO_DOWN,
};

enum class GridSlideDirect {
    SLIDE_NODE = 0,
    SLIDE_HORIZON,
    SLIDE_VERTICAL,
};

enum class GridSlideStatus {
    SLIDE_NONE = 0,
    SLIDE_START,
    SLIDE_SLIDING,
    SLIDE_SPRING_START,
};

class GridItemIndexPosition {
public:
    GridItemIndexPosition(int32_t rowIndex, int32_t colIndex) : rowIndex_(rowIndex), colIndex_(colIndex) {}
    ~GridItemIndexPosition() = default;
    GridItemIndexPosition(const GridItemIndexPosition& r)
    {
        rowIndex_ = r.rowIndex_;
        colIndex_ = r.colIndex_;
    }
    GridItemIndexPosition operator=(const GridItemIndexPosition& r)
    {
        if (this != &r) {
            rowIndex_ = r.rowIndex_;
            colIndex_ = r.colIndex_;
        }
        return *this;
    }
    bool operator==(const GridItemIndexPosition& data)
    {
        return (data.rowIndex_ == rowIndex_) && (data.colIndex_ == colIndex_);
    }
    bool operator!=(const GridItemIndexPosition& data)
    {
        return !(*this == data);
    }

    int32_t rowIndex_;
    int32_t colIndex_;
};

using OnItemDragFunc = std::function<void(const Dimension&, const Dimension&)>;
using OnAnimationCallJSFunc = std::function<void()>;
using OnCallJSDropFunc = std::function<void()>;

class RenderGridLayout : public RenderNode {
    DECLARE_ACE_TYPE(RenderGridLayout, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;
    bool IsUseOnly() override;

    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    // Adjust focus index when grid_item request focus itself.
    void UpdateFocusInfo(int32_t focusIndex);

    // Support to grid element response focus event.
    int32_t RequestNextFocus(bool vertical, bool reverse);

    const std::string& GetColumnsTemplate() const
    {
        return colsArgs_;
    }

    const std::string& GetRowTemplate() const
    {
        return rowsArgs_;
    }

    double GetColumnsGap() const
    {
        return colGap_;
    }

    double GetRowGaps() const
    {
        return rowGap_;
    }

    Dimension GetColumns() const
    {
        return userColGap_;
    }

    Dimension GetRows() const
    {
        return userRowGap_;
    }

    const std::string& GetScrollBarWidth() const
    {
        return scrollBarWidth_;
    }

    const std::string& GetScrollBarColor() const
    {
        return scrollBarColor_;
    }

    DisplayMode GetScrollBar() const
    {
        return displayMode_;
    }

    const OnItemDragFunc& GetUpdatePositionId() const
    {
        return updatePosition_;
    }

    void SetUpdatePositionId(const OnItemDragFunc& updatePosition)
    {
        updatePosition_ = updatePosition;
    }

protected:
    virtual LayoutParam MakeInnerLayoutParam(int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan) const;

    void SetItemIndex(const RefPtr<RenderNode>& child, int32_t index);

    int32_t GetItemRowIndex(const RefPtr<RenderNode>& child) const;

    int32_t GetItemColumnIndex(const RefPtr<RenderNode>& child) const;

    int32_t GetItemSpan(const RefPtr<RenderNode>& child, bool isRow) const;

    virtual void GetNextGrid(int32_t& curRow, int32_t& curCol) const;

    virtual void GetPreviousGird(int32_t& curRow, int32_t& curCol) const;

    virtual bool CheckGridPlaced(int32_t index, int32_t row, int32_t col, int32_t& rowSpan, int32_t& colSpan);

    int32_t GetIndexByGrid(int32_t row, int32_t column) const;

    // Sets child position, the mainAxis does not contain the offset.
    virtual void SetChildPosition(
        const RefPtr<RenderNode>& child, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan);

    void DisableChild(const RefPtr<RenderNode>& child, int32_t index);

    void ConvertRepeatArgs(std::string& args);

    // Handle direction key move
    int32_t focusMove(KeyDirection direction);

    Size GetTargetLayoutSize(int32_t row, int32_t col);

    std::string PreParseRows();

    std::string PreParseCols();

    virtual void InitialGridProp();

    void UpdateAccessibilityAttr();

    std::vector<double> ParseArgs(const std::string& args, double size, double gap);

    std::vector<double> ParseAutoFill(const std::vector<std::string>& strs, double size, double gap);

    void SetPreTargetRenderGrid(const RefPtr<RenderGridLayout>& preTargetRenderGrid)
    {
        preTargetRenderGrid_ = preTargetRenderGrid;
    }

    const RefPtr<RenderGridLayout> GetPreTargetRenderGrid() const
    {
        return preTargetRenderGrid_.Upgrade();
    }

    void SetMainTargetRenderGrid(const RefPtr<RenderGridLayout>& mainTargetRenderGrid)
    {
        mainTargetRenderGrid_ = mainTargetRenderGrid;
    }

    const RefPtr<RenderGridLayout> GetMainTargetRenderGrid() const
    {
        return mainTargetRenderGrid_.Upgrade();
    }

    void SetLongPressPoint(const Point& lastLongPressPoint)
    {
        lastLongPressPoint_ = lastLongPressPoint;
    }

    const Point GetLongPressPoint() const
    {
        return lastLongPressPoint_;
    }

    void ClearPartDragInfo();
    void ClearAllDragInfo();
    void CalIsVertical();
    void RegisterLongPressedForItems();
    void CreateDragDropRecognizer();
    void ActionStart(const ItemDragInfo& info, RefPtr<Component> customComponent);
    void PanOnActionUpdate(const GestureEvent& info);
    void PanOnActionEnd(const GestureEvent& info);
    void OnDragEnter(const ItemDragInfo& info);
    void OnDragLeave(const ItemDragInfo& info);
    void OnDragMove(const ItemDragInfo& info);
    bool OnDrop(const ItemDragInfo& info);
    void ImpDragStart(const ItemDragInfo& info);
    bool ImpDropInGrid(const ItemDragInfo& info);

    void ImpDragMove(const ItemDragInfo& info);
    void ImpDragLeaveMainGrid(const ItemDragInfo& info);
    void ImpDragLeaveSubGrid(const ItemDragInfo& info);
    void ImpDragEnterMainGrid(const ItemDragInfo& info);
    void ImpDragEnterSubGrid(const ItemDragInfo& info);
    void ImpDragEnterMainGridUpdate();
    void OnCallSubDragEnter(const ItemDragInfo& info);
    void OnCallSubDragLeave(const ItemDragInfo& info);

    // Check whether the item is currently allowed to be inserted
    bool CouldBeInserted();
    bool NeedBeLarger();
    bool NeedBeSmaller();
    void BackGridMatrix();
    void RestoreScene(const ItemDragInfo& info);

    int32_t CountItemInGrid();
    int32_t CountItemInRow(const std::map<int32_t, std::map<int32_t, int32_t>>::iterator& rowGrid);
    void ResetItemPosition();
    void InitialDynamicGridProp(int32_t dragLeaveOrEnter = NONE);
    void PerformLayoutForEditGrid();
    void PerformLayoutForStaticGrid();
    bool CalDragCell(const ItemDragInfo& info);
    bool CalDragRowIndex(double dragRelativelyY, int32_t& dragRowIndex);
    bool CalDragColumIndex(double dragRelativelyX, int32_t& dragColIndex);
    void MoveItems();

    // These functions cannot be called independently, they must be called by MoveItems()
    void MoveWhenNoInsertCell();
    void MoveWhenNoInsertCellAndNoItemInDragCell();
    void MoveWhenNoInsertCellButWithItemInDragCell();
    void MoveWhenNoInsertCellButWithItemInDragCellAndDragEnter();
    void MoveWhenNoInsertCellButWithItemInDragCellAndDragStart();
    void MoveWhenWithInsertCell();

    void MoveWhenWithInsertCellAndNoItemInDragCell();
    void MoveWhenWithInsertCellButWithItemInDragCell();
    void MoveWhenWithInsertCellButWithItemInDragCellDragBeforeInsert();
    void MoveWhenWithInsertCellButWithItemInDragCellDragAfterInsert();

    void FakeRemoveDragItem();
    void FakeRemoveDragItemUpdate();
    // it should be cells which has item in
    bool MoveItemsForward(int32_t fromRow, int32_t fromColum, int32_t toRow, int32_t toColum);

    // it should be cells which has item in
    bool MoveItemsBackward(int32_t fromRow, int32_t fromColum, int32_t toRow, int32_t toColum);
    void UpdateMatrixByIndexStrong(int32_t index, int32_t row, int32_t column);
    void UpdateCurInsertPos(int32_t curInsertRow, int32_t curInsertColum);
    int32_t CalIndexForItemByRowAndColum(int32_t row, int32_t column);

    // If the first is equal the second, return true, else return false.
    bool SortCellIndex(int32_t rowFirst, int32_t columFirst, int32_t rowSecond, int32_t columSecond, bool& firstIsPre);

    // if there is no empty in the cell return false, else return true.
    bool CalTheFirstEmptyCell(int32_t& rowIndex, int32_t& columIndex, bool ignoreInsert);

    void SetGridLayoutParam();
    void CalculateVerticalSize(std::vector<double>& cols, std::vector<double>& rows, int32_t dragLeaveOrEnter);
    void CalculateHorizontalSize(std::vector<double>& cols, std::vector<double>& rows, int32_t dragLeaveOrEnter);
    void UpdateCollectionInfo(std::vector<double> cols, std::vector<double> rows);
    void ClearSpringSlideData();
    void CreateSlideRecognizer();
    void HandleSlideStart(const TouchEventInfo& info);
    void HandleSlideUpdate(const TouchEventInfo& info);
    void HandleSlideEnd(const TouchEventInfo& info);
    bool CheckLongPress();
    bool MayStartToSlide(const TouchEventInfo& info);
    void UpdateSlideStatus(GridSlideStatus status);
    GridSlideStatus GetSlideStatus();
    GridSlideDirect GetSlideDirect();
    Point GetPointFromTouchInfo(const TouchEventInfo& info);
    void MoveRelativeDistance(double& dx, double& dy);
    void CreateSpringController();
    Point GetSpringStartPoint();
    Point GetSpringEndPoint();
    void StartSpringAnimation(const Point& startPoint, const Point& endPoint);
    void FinishedSpringAnimation();
    void UpdateSprintAnimationPosition(double offset);
    void BackupSpringItemsData();
    void GetMotionPosition(const Point& startPoint, const Point& endPoint, double& start, double& end);
    void CalcSlideDirect(const Point& curPos);
    void CalcSpringGravitationDirect();

    void TriggerMoveEventForJS(const ItemDragInfo& info);
    void TriggerDropEventForJS(const ItemDragInfo& info, int32_t insertIndex, bool success);
    void InitAnimationController(const WeakPtr<PipelineContext>& context);
    bool AddNodeAnimationToController(int32_t itemIndex, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan);
    void AddNodeAnimationToControllerForDrop(
        const RefPtr<RenderNode>& item, const Point& startPoint, const Point& endPoint);
    void PrepareAnimationController(const std::string& key);
    void StartAnimationController(GridLayoutAnimationAct animationAct, const OnAnimationCallJSFunc& func);
    void StopAnimationController();
    Point CalcChildPosition(
        const RefPtr<RenderNode>& child, int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan);
    Point CalcDragChildStartPosition(const ItemDragInfo& info);
    Point CalcDragChildEndPosition(int32_t rowIndex, int32_t colIndex);
    void FinishedAnimationController(const std::string& key);
    void RegisterAnimationFinishedFunc(const std::string& key, std::function<void()> func);
    void CalcRestoreScenePosition(const ItemDragInfo& info);
    void ParseRestoreScenePosition(
        const std::map<int32_t, std::map<int32_t, int32_t>>& data, std::map<int32_t, GridItemIndexPosition>& info);
    int32_t GetDragPosRowIndex()
    {
        return dragPosRowIndex_;
    }
    int32_t GetDragPosColumnIndex()
    {
        return dragPosColumnIndex_;
    }
    void StartFlexController(const Point& endPoint, bool includeSubGrid = false);
    void FinishedFlexController();
    void FinishedFlexControllerForSubGrid();
    void CloseFlexComponent();
    void UpdateFlexComponentPosition(const Point& pos);
    void RegisterDropJSEvent(const ItemDragInfo& info, int32_t insertIndex, bool success);
    void RegisterDropJSFunc(const OnCallJSDropFunc& func);
    void CallDropJSFunc();
    bool CheckAnimation();
    bool CheckNeedShrink() const;
    void RefreshAllocatedRowSizes(int32_t rowIndex, int32_t itemRowSpan, const RefPtr<RenderNode>& item);

    bool isVertical_ = false;
    bool updateFlag_ = false;
    bool isDeclarative_ = false;
    bool needShrink_ = false;
    std::vector<double> allocatedRowSizes_;
    FlexDirection direction_ = FlexDirection::ROW;
    FlexAlign crossAxisAlign_ = FlexAlign::CENTER;

    int32_t focusRow_ = -1;
    int32_t focusCol_ = -1;
    int32_t focusIndex_ = 0;

    double colSize_ = 0.0;
    double rowSize_ = 0.0;
    double gridWidth_ = -1.0;
    double gridHeight_ = -1.0;
    int32_t colCount_ = 0;
    int32_t rowCount_ = 0;
    Dimension userColGap_ = 0.0_px;
    Dimension userRowGap_ = 0.0_px;
    double colGap_ = 0.0;
    double rowGap_ = 0.0;
    std::string colsArgs_;
    std::string rowsArgs_;
    std::string scrollBarWidth_;
    std::string scrollBarColor_;
    DisplayMode displayMode_ = DisplayMode::OFF;
    bool rightToLeft_ = false;
    bool needResetItemPosition_ = false;
    // Map structure: [rowIndex - (columnIndex, index)]
    std::map<int32_t, std::map<int32_t, int32_t>> gridMatrix_;
    // Map structure: [rowIndex - columnIndex - (width, height)]
    std::map<int32_t, std::map<int32_t, Size>> gridCells_;

    RefPtr<GestureRecognizer> dragDropGesture_;
    WeakPtr<RenderGridLayout> preTargetRenderGrid_ = nullptr;
    WeakPtr<RenderGridLayout> mainTargetRenderGrid_ = nullptr;

    // The list of renderNodes of items in the grid
    std::vector<RefPtr<RenderNode>> itemsInGrid_;

    // back for gridMatrix_
    std::map<int32_t, std::map<int32_t, int32_t>> gridMatrixBack_;

    // The maximum number of items that the grid can hold
    int32_t itemCountMax_ = -1;

    // The grid length in the main axis direction
    int32_t cellLength_ = 0;

    // The maximum number of rows (columns) that can be accommodated in a variable direction.
    // (only for dynamic limited grid)
    int32_t mainCountMax_ = -1;

    // The minimum number of rows (columns) that must be accommodated in the variable direction.
    // (only for dynamic limited grid)
    int32_t mainCountMin_ = -1;

    // The maximum number of items that the grid can hold.
    // (Only the dynamic grid needs to be used to determine whether the main sequence needs to be increased.)
    int32_t curItemCountMax_ = -1;

    // The rowIndex of the grid currently to be inserted
    int32_t curInsertRowIndex_ = -1;

    // The columnIndex of the grid currently to be inserted
    int32_t curInsertColumnIndex_ = -1;

    // The rowIndex of the grid where the Drag coordinates are located
    int32_t dragPosRowIndex_ = -1;

    // The columnIndex of the grid where the Drag coordinates are located
    int32_t dragPosColumnIndex_ = -1;

    // The index of the item currently being dragged.
    int32_t dragingItemIndex_ = -1;

    // Whether to send changes to the grid where the drag coordinate is located
    bool dragPosChanged_ = false;

    bool isDynamicGrid_ = false;

    bool editMode_ = false;

    bool itemLongPressed_ = false;

    bool itemDragEntered_ = false;

    bool itemDragStarted_ = false;

    bool isDragChangeLayout_ = false;

    bool needRestoreScene_ = false;

    bool isInMainGrid_ = false;

    bool isMainGrid_ = false;

    bool reEnter_ = false;

    WeakPtr<RenderNode> dragingItemRenderNode_;
    WeakPtr<RenderGridLayout> subGrid_;
    WeakPtr<RenderGridLayout> mainGrid_;
    RefPtr<GridLayoutComponent> component_;

    OnGridDragEnterFunc OnGridDragEnterFunc_;
    OnGridDragMoveFunc onGridDragMoveFunc_;
    OnGridDragLeaveFunc onGridDragLeaveFunc_;
    OnGridDragStartFunc onGridDragStartFunc_;
    OnGridDropFunc onGridDropFunc_;

    OnItemDragFunc updatePosition_;
    Point lastGlobalPoint_;
    Point lastLongPressPoint_;
    Point startGlobalPoint_;
    bool isExistComponent_ = false;
    std::atomic<bool> isDragging_;

    GridLayoutAnimationAct animationAct_ = GridLayoutAnimationAct::ANIMATION_NONE;
    RefPtr<Animator> animationController_;
    RefPtr<Animator> flexController_;
    bool supportAnimation_ = false;
    bool dragAnimation_ = false;
    EdgeEffect edgeEffect_ = EdgeEffect::NONE;
    std::atomic<bool> needRunAnimation_;
    std::map<std::string, std::function<void()>> animationFinishedFuncList_;
    std::mutex animationLock_;
    OnAnimationCallJSFunc jsMoveFunc_ = nullptr;
    OnAnimationCallJSFunc jsDropFunc_ = nullptr;
    OnAnimationCallJSFunc restoreSceneFunc_ = nullptr;
    std::map<int32_t, Point> gridItemPosition_;
    std::list<RefPtr<RenderNode>> animationItemList_;
    std::atomic<bool> runFlexAnimation_;
    std::atomic<bool> triggerJSDrop_;
    std::vector<OnCallJSDropFunc> dropJSFuncList_;
    std::mutex dropJSFuncListLock_;

    RefPtr<RawRecognizer> slideRecognizer_;
    GridSpringGravitationDirect gravitationDirect_ = GridSpringGravitationDirect::SPRING_NONE;
    GridSlideDirect slideDirect_ = GridSlideDirect::SLIDE_NODE;
    std::atomic<GridSlideStatus> slideStatus_;
    Point slideStartPoint_;
    Point slidePriPoint_;
    Point slideCurPoint_;
    Point slideDistance_;
    RefPtr<Animator> springController_;
    RefPtr<SpringMotion> springMotion_;
    std::vector<Offset> springStartPosition_;

    Offset mouseStartOffset_;
    Offset mouseEndOffset_;
    bool HandleMouseEvent(const MouseEvent& event) override;
    bool isMultiSelectable_ = false;
    void ClearMultiSelect();

    void MultiSelectWithoutKeyboard(const Rect& selectedZone);
    void HandleMouseEventWithoutKeyboard(const MouseEvent& event);

    void MultiSelectWhenCtrlDown(const Rect& selectedZone);
    void HandleMouseEventWhenCtrlDown(const MouseEvent& event);
    void CollectSelectedItems();
    std::set<RefPtr<RenderGridLayoutItem>> selectedItemsWithCtrl_;

    void MultiSelectWhenShiftDown(const Rect& selectedZone);
    RefPtr<RenderGridLayoutItem> GetPressItemWhenShiftDown(const Rect& selectedZone);
    void HandleMouseEventWhenShiftDown(const MouseEvent& event);
    void MultiSelectAllInRange(const RefPtr<RenderGridLayoutItem>& firstItem,
        const RefPtr<RenderGridLayoutItem>& secondItem);
    RefPtr<RenderGridLayoutItem> firstItemWithShift_;
    RefPtr<RenderGridLayoutItem> secondItemWithShift_;

    void MultiSelectAllWhenCtrlA();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_LAYOUT_RENDER_GRID_LAYOUT_H
