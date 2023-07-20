/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_list.h
 *
 * @brief Declares a scrollable list in the vertical or horizontal direction. * This scrollable list can be used with
 *        the adapter {@link AbstractAdapter} to implement scrolling, inertial scrolling, automatic alignment, and
 *        invoking of a callback when a child view at the preset position is selected as this list scrolls. It is used
 *        when there is a large number of child views with a fixed format.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_LIST_H
#define GRAPHIC_LITE_UI_LIST_H

#include "components/abstract_adapter.h"
#include "components/ui_abstract_scroll.h"
#include "dock/focus_manager.h"
#include "dock/vibrator_manager.h"
#include "gfx_utils/list.h"

namespace OHOS {
/**
 * @brief Represents a listener that contains a callback to be invoked when the scroll state changes or when a new child
 *        view is selected at the preset position as this list scrolls. The scroll state can be {@link
 *        SCROLL_STATE_STOP} or {@link SCROLL_STATE_MOVE}.
 * @since 1.0
 * @version 1.0
 */
class ListScrollListener : public HeapBase {
public:
    /**
     * @brief A constructor used to create a <b>ListScrollListener</b> instance with the default state
     *        {@link SCROLL_STATE_STOP}.
     * @since 1.0
     * @version 1.0
     */
    ListScrollListener() : state_(SCROLL_STATE_STOP) {}

    /**
     * @brief A destructor used to delete the <b>ListScrollListener</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~ListScrollListener() {}

    /**
     * @brief Called when a scroll starts.
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnScrollStart(int16_t index, UIView* view) {}

    /**
     * @brief Called when a scroll ends.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 1.0
     * @version 1.0
     *
     */
    virtual void OnScrollEnd(int16_t index, UIView* view) {}

    /**
     * @brief Called when move to the top of scroll.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 8
     * @version 8
     *
     */
    virtual void OnScrollTop(int16_t index, UIView* view) {}

    /**
     * @brief Called when move to the bottom of scroll.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 8
     * @version 8
     *
     */
    virtual void OnScrollBottom(int16_t index, UIView* view) {}

    /**
     * @brief Called when a new child view is selected at the preset position as this list scrolls. For details about
     * how to set the position, see {@link SetSelectPosition}.
     *
     * @param index Indicates the index of the child view being selected at the preset position. This parameter should
     *              be set to {@link NULL_SELECT_INDEX} if no child view is selected or no position is preset.
     * @param view Indicates the child view being selected at the preset position. This parameter should be set to
     *             <b>NULL</b> if no child view is selected or no position is preset.
     * @since 1.0
     * @version 1.0
     *
     */
    virtual void OnItemSelected(int16_t index, UIView* view) {}

    /**
     * @brief Obtains the scroll state of this list.
     *
     * @return Returns the scroll state, either {@link SCROLL_STATE_STOP} or {@link SCROLL_STATE_MOVE}.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetScrollState() const
    {
        return state_;
    }

    void SetScrollState(uint8_t state)
    {
        state_ = state;
    }

    static constexpr uint8_t SCROLL_STATE_STOP = 0;
    static constexpr uint8_t SCROLL_STATE_MOVE = 1;

private:
    friend class UIList;
    uint8_t state_;
};

/**
 * @brief Represents a scrollable list which is used with the adapter {@link AbstractAdapter} to implement scrolling,
 *        inertial scrolling, automatic alignment, and invoking of a callback when a child view is selected at the
 *        preset position as this list scrolls.
 *
 * You need to override {@link AbstractAdapter} to implement functions for setting and saving data, obtaining data
 * quantity, and creating child views. <b>UIList</b> is used when there is a large number of child views with a fixed
 * format. This list automatically reclaims the child views removed out of the current view as it scrolls so that as
 * many as child views can be displayed with a lower memory consumption.
 *
 * @since 1.0
 * @version 1.0
 */
class UIList : public UIAbstractScroll {
public:
    /**
     * @brief A constructor used to create a <b>UIList</b> instance in the vertical direction.
     * @since 1.0
     * @version 1.0
     */
    UIList();

    /**
     * @brief A constructor used to create a <b>UIList</b> instance in the specified direction.
     *
     * @param direction Indicates the <b>UIList</b> direction, either {@link HORIZONTAL} or {@link VERTICAL}.
     * @since 1.0
     * @version 1.0
     */
    explicit UIList(uint8_t direction);

    /**
     * @brief A destructor used to delete the <b>UIList</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIList();

    /**
     * @brief Obtains the view type.
     * @return Returns the view type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_LIST;
    }

    bool OnDragEvent(const DragEvent& event) override;

    bool OnDragEndEvent(const DragEvent& event) override;

    bool OnPressEvent(const PressEvent& event) override;

#if ENABLE_ROTATE_INPUT
    bool OnRotateStartEvent(const RotateEvent& event) override;

    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif

    /**
     * @brief Sets the adapter for this list. The content of this list is initialized when the adapter is set.
     *
     * @param adapter Indicates the adapter to set. For details, see {@link AbstractAdapter}.
     * @since 1.0
     * @version 1.0
     */
    void SetAdapter(AbstractAdapter* adapter);

    void Reload();
    /**
     * @brief Moves the position of all child views.
     *
     * @param offsetX Indicates the offset distance by which a child view is moved on the x-axis.
     * @param offsetY Indicates the offset distance by which a child view is moved on the y-axis.
     * @since 1.0
     * @version 1.0
     */
    void MoveChildByOffset(int16_t x, int16_t y) override;

    /**
     * @brief Scrolls to change the index of the first row or column of the current view.
     *
     * @param index Indicates the new index of the first row or column of the current view.
     * @since 1.0
     * @version 1.0
     */
    void ScrollTo(uint16_t index);

    /**
     * @brief Scrolls the content in this list.
     *
     * @param distance Indicates the distance by which the content is scrolled.
     * @since 1.0
     * @version 1.0
     */
    void ScrollBy(int16_t distance);

    /**
     * @brief Sets the start index for this list.
     *
     * @param index Indicates the start index to set. The default value is <b>0</b>.
     * @since 1.0
     * @version 1.0
     *
     */
    void SetStartIndex(uint16_t index)
    {
        startIndex_ = index;
    }

    /**
     * @brief Obtains the start index of this list. The default value is <b>0</b>.
     *
     * @return Returns the start index.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetStartIndex() const
    {
        return startIndex_;
    }

    /**
     * @brief Sets the loop state for this list, in which a loop scroll is possible since the top and bottom of the list
     *        are connected together.
     *
     * @param state Indicates the loop state to set.
     * @since 1.0
     * @version 1.0
     */
    void SetLoopState(bool state)
    {
        isLoopList_ = state;
    }

    /**
     * @brief Checks whether this list is in a loop state.
     *
     * @return Returns if the list is in the loop state; returns <b>false</b> if the list is in the common state.
     * @since 1.0
     * @version 1.0
     */
    bool GetLoopState() const
    {
        return isLoopList_;
    }

    /**
     * @brief Sets the position where a child view is selected as this list scrolls.
     *
     * When a child view is selected at the specified position, the callback {@link ListScrollListener} is invoked.
     * You can implement the zoom-in and color-change effects in the callback.
     *
     * @param position Indicates the position to set. The default value is <b>0</b>, indicating that no position is
     *                 set.
     * @since 1.0
     * @version 1.0
     */
    void SetSelectPosition(uint16_t position)
    {
        selectPosition_ = position;
    }

    /**
     * @brief Obtains the child view being selected at the preset position.
     *
     * @return Returns the child view if available; returns <b>NULL</b> if no child view is selected or no position is
     *         set.
     * @since 1.0
     * @version 1.0
     */
    UIView* GetSelectView();

    /**
     * @brief Sets the listener that contains a callback to be invoked when a child view is selected as this list
     *        scrolls.
     *
     * @param scrollListener Indicates the listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetScrollStateListener(ListScrollListener* scrollListener)
    {
        scrollListener_ = scrollListener;
    }

    /**
     * @brief Refreshes this list. The number of child views in the current view is fixed and the positions of those
     *        reserved child views as this list scrolls remain unchanged.
     * @since 1.0
     * @version 1.0
     */
    void RefreshList();

    /**
     * @brief Sets the automatic alignment state for this list. When a scroll stops, a child view is selected and its
     *        position is automatically aligned with the preset position.
     *
     * @param state Indicates the automatic alignment state. <b>true</b> indicates this state is enabled, and
     *              <b>false</b> indicates the opposite case.
     * @since 1.0
     * @version 1.0
     */
    void EnableAutoAlign(bool state)
    {
        autoAlign_ = state;
    }

    /**
     * @brief
     * 设置自动对齐动画时长，单位为毫秒，默认为100毫秒。该功能依赖EnableAutoAlign()方法，自动对齐设置为true情况下才生效。
     *
     * @param value 自动对齐动画时长,0表示无动画。
     * @since 3.0
     * @version 3.0
     */
    void SetAutoAlignTime(uint16_t time)
    {
        alignTime_ = time;
    }

    /**
     * @brief Sets the automatic alignment mode for this list when auto align state is enabled.
     *
     * @param dragBack mode of alignment.
     *        true means list item will move back when its position crosses the line of select
     *        position partly (but not all of it).
     *        false means list item will move front when its position crosses the line of select
     *        position (no matter how many it crosses).
     *
     * @since 3.0
     * @version 3.0
     */
    void EnableCrossDragBack(bool dragBack)
    {
        dragBack_ = dragBack;
    }

    /**
     * @brief 获取自动对齐动画时长。
     *
     * @return 自动对齐动画时长。
     * @since 3.0
     * @version 3.0
     */
    uint16_t GetAutoAlignTime() const
    {
        return alignTime_;
    }

    void SetXScrollBarVisible(bool visible);

    void SetYScrollBarVisible(bool visible);

    void RemoveAll() override;

    static constexpr int8_t NULL_SELECT_INDEX = -1;

    UIView* onSelectedView_;
    void SetOpenSave(bool open)
    {
        isOpen_ = open;
        recycle_.SetOpenSave(open);
    }
    bool GetOpenSave()
    {
        return isOpen_;
    }

protected:
    static constexpr int16_t RECALCULATE_DRAG_DISTANCE = 10;
    static constexpr int16_t RECALCULATE_DRAG_TIMES = 10;
    static constexpr int16_t DEFAULT_ALIGN_TIMES = 100;
    void StopAnimator() override;
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;

private:
    friend class UIPicker;
    friend class Recycle;
    class Recycle : public HeapBase {
    public:
        explicit Recycle(UIList* list) : adapter_(nullptr), listView_(list), hasInitialiszed_(false) {}
        virtual ~Recycle();
        void InitRecycle();
        void Reload();
        UIView* GetView(int16_t index);
        void SetAdapter(AbstractAdapter* adapter)
        {
            hasInitialiszed_ = false;
            adapter_ = adapter;
        }

        bool HasInitialiszed()
        {
            return hasInitialiszed_;
        }

        void AddScrapView(UIView* view)
        {
            scrapView_.PushBack(view);
        }

        uint16_t GetAdapterItemCount()
        {
            return (adapter_ == nullptr) ? 0 : adapter_->GetCount();
        }

        void ClearScrapView()
        {
            scrapView_.Clear();
        }

        void SetOpenSave(bool open)
        {
            isOpen_ = open;
        }

        Rect32 GetAdapterItemsReletiveRect();
        void MoveAdapterItemsRelativeRect(int16_t x, int16_t y);
        void MeasureAdapterRelativeRect();

    private:
        friend class UIList;
        void FillActiveView();

        List<UIView*> scrapView_;
        AbstractAdapter* adapter_;
        UIList* listView_;
        Rect32 adapterRelativeRect_;
        bool hasInitialiszed_;
        bool isOpen_;
    };

    void PushBack(UIView* view);
    void PopItem(UIView* view);
    void PushFront(UIView* view);
    void SetHead(UIView* view);
    bool MoveChildStep(int16_t distance);
    bool MoveChildStepVertical(int16_t distance);
    bool MoveChildStepHorizontal(int16_t distance);
    uint16_t GetIndexInc(uint16_t index);
    uint16_t GetIndexDec(uint16_t index);

    bool MoveOffset(int16_t x, int16_t y);
    void UpdateScrollBar();
    bool IsNeedReCalculateDragEnd();
    bool ReCalculateDragEnd();
    void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY) override;
    void FixDistance(int16_t& distanceX, int16_t& distanceY) override;
    void FixHorDistance(int16_t& distanceX);
    void FixVerDistance(int16_t& distanceY);
    bool isLoopList_;
    bool isReCalculateDragEnd_;
    bool autoAlign_;
    uint16_t alignTime_;
    uint16_t startIndex_;
    uint16_t topIndex_;
    uint16_t bottomIndex_;
    uint16_t selectPosition_;
    int16_t onSelectedIndex_;
    Recycle recycle_;
    ListScrollListener* scrollListener_;
    bool isOpen_;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_LIST_H
