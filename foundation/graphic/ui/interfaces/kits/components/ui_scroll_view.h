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
 * @file ui_scroll_view.h
 *
 * @brief Declares a view group that allows its child views to be displayed as scroll events occur.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_SCROLL_VIEW_H
#define GRAPHIC_LITE_UI_SCROLL_VIEW_H

#include "animator/animator.h"
#include "components/ui_abstract_scroll.h"

namespace OHOS {
/**
 * @brief Supports horizontal or vertical scroll of child views. This class is inherited from {@link UIAbstractScroll}.
 *
 * Horizontal or vertical scroll occurs only when the width or height of the child view is greater than that of the
 * <b>UIScrollView</b>.
 *
 * @since 1.0
 * @version 1.0
 */
class UIScrollView : public UIAbstractScroll {
public:
    /**
     * @brief Represents a listener that contains a callback to be invoked upon scroll state changes. The state can
     *        either be {@link SCROLL_STATE_STOP} or {@link SCROLL_STATE_MOVE}.
     * @since 1.0
     * @version 1.0
     */
    class OnScrollListener : public HeapBase {
    public:
        /**
         * @brief A constructor used to create an <b>OnScrollListener</b> instance with the default scroll state
         *        {@link SCROLL_STATE_STOP}.
         * @since 1.0
         * @version 1.0
         */
        OnScrollListener() : state_(SCROLL_STATE_STOP) {}

        /**
         * @brief A destructor used to delete the <b>OnScrollListener</b> instance.
         * @since 1.0
         * @version 1.0
         */
        virtual ~OnScrollListener() {}

        /**
         * @brief Called when a scroll starts.
         *
         * @since 1.0
         * @version 1.0
         */
        virtual void OnScrollStart() {}

        /**
         * @brief Called when a scroll ends.
         *
         * @since 1.0
         * @version 1.0
         */
        virtual void OnScrollEnd() {}

        /**
         * @brief Obtains the scroll state of this view.
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
        uint8_t state_;
    };

    /**
     * @brief A constructor used to create a <b>UIScrollView</b> instance, with both horizontal and vertical scrolls
     *        supported.
     *
     * @since 1.0
     * @version 1.0
     */
    UIScrollView();

    /**
     * @brief A destructor used to delete the <b>UIScrollView</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIScrollView() {}

    /**
     * @brief Obtains the view type.
     * @return Returns the view type, as defined in {@link UIViewType}.
     *
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_SCROLL_VIEW;
    }

#if ENABLE_ROTATE_INPUT
    bool OnRotateEvent(const RotateEvent& event) override;

    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif

    bool OnDragEvent(const DragEvent& event) override;

    bool OnDragEndEvent(const DragEvent& event) override;

    bool OnPressEvent(const PressEvent& event) override;

    /**
     * @brief Scrolls the content of this view.
     *
     * @param xDistance Indicates the offset distance by which the content is scrolled on the x-axis.
     * @param yDistance Indicates the offset distance by which the content is scrolled on the y-axis.
     * @since 1.0
     * @version 1.0
     */
    void ScrollBy(int16_t xDistance, int16_t yDistance);

    /**
     * @brief Sets whether a horizontal scroll is enabled.
     *
     * @param state Specifies whether a horizontal scroll is enabled. <b>true</b> indicates a horizontal scroll is
     *              enabled, and <b>false</b> indicates the opposite case.
     * @since 1.0
     * @version 1.0
     */
    void SetHorizontalScrollState(bool state)
    {
        if (direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL) {
            direction_ = state ? HORIZONTAL_AND_VERTICAL : VERTICAL;
        } else {
            direction_ = state ? HORIZONTAL : HORIZONTAL_NOR_VERTICAL;
        }
    }

    /**
     * @brief Checks whether a horizontal scroll is enabled.
     *
     * @return Returns <b>true</b> if a horizontal scroll is enabled; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetHorizontalScrollState() const
    {
        return (direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL);
    }

    /**
     * @brief Sets whether a vertical scroll is enabled.
     *
     * @param state Specifies whether a vertical scroll is enabled. <b>true</b> indicates a vertical scroll is enabled,
     *              and <b>false</b> indicates the opposite case.
     * @since 1.0
     * @version 1.0
     */
    void SetVerticalScrollState(bool state)
    {
        if (direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL) {
            direction_ = state ? HORIZONTAL_AND_VERTICAL : HORIZONTAL;
        } else {
            direction_ = state ? VERTICAL : HORIZONTAL_NOR_VERTICAL;
        }
    }

    /**
     * @brief Checks whether a vertical scroll is enabled.
     *
     * @return Returns <b>true</b> if a vertical scroll is enabled, returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool GetVerticalScrollState() const
    {
        return (direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL);
    }

    /**
     * @brief Registers a listener that contains a callback to be invoked upon scroll state changes.
     *
     * @param scrollListener Indicates the listener to register. For details, see {@link OnScrollListener}.
     * @since 1.0
     * @version 1.0
     */
    void RegisterScrollListener(OnScrollListener* scrollListener)
    {
        scrollListener_ = scrollListener;
    }

protected:
    void StopAnimator() override;
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;
    bool MoveOffset(int16_t offsetX, int16_t offsetY);

private:
    void Drag(const DragEvent& event);
    void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY) override;
    void RefreshScrollBar();
    OnScrollListener* scrollListener_;
#if ENABLE_VIBRATOR
    int16_t totalRotateLen_;
    int16_t lastVibratorRotateLen_;
#endif
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_SCROLL_VIEW_H
