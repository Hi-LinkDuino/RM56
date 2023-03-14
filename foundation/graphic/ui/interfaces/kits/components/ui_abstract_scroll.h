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
 * @file ui_abstract_scroll.h
 *
 * @brief Declares the base class used to define the attributes of a scroll. The <b>UIList</b>, <b>UIScrollView</b>, and
 *        <b>UISwipeView</b> inherit from this class.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_ABSTRACT_SCROLL_H
#define GRAPHIC_LITE_UI_ABSTRACT_SCROLL_H

#include "animator/animator.h"
#include "animator/easing_equation.h"
#include "components/ui_view_group.h"

namespace OHOS {
class BarEaseInOutAnimator;
class UIAbstractScrollBar;
/**
 * @brief Defines the attributes of a scroll, including the scroll direction, blank size of a scroll view, velocity and
 *        effects of a scroll animation.
 *
 * @since 1.0
 * @version 1.0
 */
class UIAbstractScroll : public UIViewGroup {
public:
    /**
     * @brief A constructor used to create a <b>UIAbstractScroll</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIAbstractScroll();

    /**
     * @brief A destructor used to delete the <b>UIAbstractScroll</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIAbstractScroll();

    /**
     * @brief Obtains the view type.
     * @return Returns the view type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_ABSTRACT_SCROLL;
    }

    /**
     * @brief Sets the blank size for this scroll view.
     *
     *
     * @param value Indicates the blank size to set. The default value is <b>0</b>. Taking a vertical scroll as an
     *              example, the value <b>0</b> indicates that the head node can only scroll downwards the top of the
     *              view and the tail node scroll upwards the bottom; the value <b>10</b> indicates that the head node
     *              can continue scrolling down by 10 pixels after it reaches the top of the view.
     * @since 1.0
     * @version 1.0
     */
    void SetScrollBlankSize(uint16_t size)
    {
        scrollBlankSize_ = size;
    }

    /**
     * @brief Sets the maximum scroll distance after a finger lifts the screen.
     *
     * @param distance Indicates the maximum scroll distance to set. The default value is <b>0</b>, indicating that the
     *                 scroll distance is not limited.
     * @since 1.0
     * @version 1.0
     */
    void SetMaxScrollDistance(uint16_t distance)
    {
        maxScrollDistance_ = distance;
    }

    /**
     * @brief Sets the rebound size, which is the distance a knob moves after being released when it reaches the end of
     *        a scrollbar.
     *
     * @param size Indicates the rebound size to set.
     * @since 1.0
     * @version 1.0
     */
    void SetReboundSize(uint16_t size)
    {
        reboundSize_ = size;
    }

    /**
     * @brief Obtains the maximum scroll distance after a finger lifts the screen.
     *
     * @return Returns the maximum scroll distance. The default value is <b>0</b>, indicating that the scroll distance
     * is not limited.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetMaxScrollDistance() const
    {
        return maxScrollDistance_;
    }

    /**
     * @brief Sets the easing function that specifies a scroll animation after a finger lifts the screen.
     *
     * @param func Indicates the easing function to set. The default function is {@link EasingEquation::CubicEaseOut}.
     *             For details, see {@link EasingEquation}.
     * @since 1.0
     * @version 1.0
     */
    void SetDragFunc(EasingFunc func)
    {
        easingFunc_ = func;
    }

    /**
     * @brief Sets whether to continue scrolling after a finger lifts the screen.
     *
     * @param throwDrag Specifies whether to continue scrolling after a finger lifts the screen. <b>true</b> indicates
     *                  the scroll continues, and <b>false</b> indicates the scroll stops immediately after a finger
     *                  lifts.
     * @since 1.0
     * @version 1.0
     */
    void SetThrowDrag(bool throwDrag)
    {
        throwDrag_ = throwDrag;
    }

    /**
     * @brief Moves the position of all child views.
     *
     * @param offsetX Indicates the offset distance by which a child view is moved on the x-axis.
     * @param offsetY Indicates the offset distance by which a child view is moved on the y-axis.
     * @since 1.0
     * @version 1.0
     */
    void MoveChildByOffset(int16_t offsetX, int16_t offsetY) override;

    /**
     * @brief Sets the drag acceleration.
     *
     * @param value Indicates the drag acceleration to set. The default value is <b>10</b>. A larger drag acceleration
     *              indicates a higher inertial scroll velocity.
     * @since 1.0
     * @version 1.0
     */
    void SetDragACCLevel(uint16_t value)
    {
        if (value != 0) {
            dragAccCoefficient_ = value;
        }
    }

    /**
     * @brief Obtains the drag acceleration.
     *
     * @return Returns the drag acceleration.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetDragACCLevel() const
    {
        return dragAccCoefficient_;
    }

    /**
     * @brief Sets the compensation distance after a finger lifts the screen.
     *
     * @param value Indicates the compensation distance to set. The default value is <b>0</b>.
     * @since 1.0
     * @version 1.0
     */
    void SetSwipeACCLevel(uint16_t value)
    {
        swipeAccCoefficient_ = value;
    }

    /**
     * @brief Obtains the compensation distance after a finger lifts the screen.
     *
     * @return Returns the compensation distance.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetSwipeACCLevel() const
    {
        return swipeAccCoefficient_;
    }

#if ENABLE_ROTATE_INPUT
    /**
     * @brief Sets coefficient for rotation dragthrow animation. The view will roll farther with larger coeffcient.
     *
     * @param value Indicates the coefficient to set. The default value is <b>0</b>.
     * @since 1.0
     * @version 1.0
     */
    void SetRotateACCLevel(uint8_t value)
    {
        rotateAccCoefficient_ = value;
    }

    /**
     * @brief Obtains the coefficient for rotation drag throw animation.
     *
     * @return Returns the coefficient for rotation drag throw animation.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetRotateACCLevel() const
    {
        return rotateAccCoefficient_;
    }

    /**
     * @brief Obtains the rotation factor.
     *
     * @return Returns the rotation factor.
     * @since 5.0
     * @version 3.0
     */
    float GetRotateFactor() const
    {
        return rotateFactor_;
    }

    /**
     * @brief Sets the rotation factor.
     *
     * @param factor Indicates the rotation factor to set.
     * @since 5.0
     * @version 3.0
     */
    void SetRotateFactor(float factor)
    {
        if (MATH_ABS(factor) > MAX_ROTATE_FACTOR) {
            rotateFactor_ = (factor > 0) ? MAX_ROTATE_FACTOR : -MAX_ROTATE_FACTOR;
            return;
        }
        rotateFactor_ = factor;
    }

    /**
     * @brief Sets threshold for rotation drag throw animation. The view will roll easier with larger threshold.
     *
     * @param threshold Indicates the rotation factor to set.
     *
     * @since 6
     */
    void SetRotateThrowThreshold(uint8_t threshold)
    {
        if (threshold == 0) {
            return;
        }
        rotateThrowthreshold_ = threshold;
    }

    bool OnRotateStartEvent(const RotateEvent& event) override;

    bool OnRotateEvent(const RotateEvent& event) override;

    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif

    void SetXScrollBarVisible(bool visible);

    void SetYScrollBarVisible(bool visible);

    void SetScrollBarSide(uint8_t side)
    {
        scrollBarSide_ = side;
    }

    void SetScrollBarCenter(const Point& center)
    {
        scrollBarCenter_ = center;
        scrollBarCenterSetFlag_ = true;
    }

    /**
     * @brief Sets the list direction.
     *
     * @param direction Indicates the list direction, either {@link HORIZONTAL} or {@link VERTICAL}.
     * @since 1.0
     * @version 1.0
     */
    void SetDirection(uint8_t direction)
    {
        direction_ = direction;
    }

    /**
     * @brief Obtains the list direction.
     * @return Returns the list direction, either {@link HORIZONTAL} or {@link VERTICAL}.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetDirection() const
    {
        return direction_;
    }

    void OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    static constexpr uint8_t HORIZONTAL = 0;
    static constexpr uint8_t VERTICAL = 1;

protected:
    static constexpr uint8_t HORIZONTAL_AND_VERTICAL = 2;
    static constexpr uint8_t HORIZONTAL_NOR_VERTICAL = 3;
    /* calculate drag throw distance, last drag distance in one tick * DRAG_DISTANCE_COEFFICIENT */
    static constexpr uint8_t DRAG_DISTANCE_COEFFICIENT = 5;
    /* calculate drag throw times, drag distance / DRAG_TIMES_COEFFICIENT */
    static constexpr uint8_t DRAG_TIMES_COEFFICIENT = 18;
    /* the minimum duration of the swipe animator */
    static constexpr uint8_t MIN_DRAG_TIMES = 5;
    /* acceleration calculation coefficient */
    static constexpr uint8_t DRAG_ACC_FACTOR = 10;
    /* the maximum number of historical drag data */
    static constexpr uint8_t MAX_DELTA_SIZE = 3;

    static constexpr uint16_t SCROLL_BAR_WIDTH = 4;
    static constexpr uint8_t MAX_ROTATE_FACTOR = 128;

    class ListAnimatorCallback : public AnimatorCallback {
    public:
        ListAnimatorCallback()
            : curtTime_(0),
              dragTimes_(0),
              startValueX_(0),
              endValueX_(0),
              previousValueX_(0),
              startValueY_(0),
              endValueY_(0),
              previousValueY_(0)
        {
        }

        virtual ~ListAnimatorCallback() {}

        void SetDragTimes(uint16_t times)
        {
            dragTimes_ = times;
        }

        void SetDragStartValue(int16_t startValueX, int16_t startValueY)
        {
            startValueX_ = startValueX;
            previousValueX_ = startValueX;
            startValueY_ = startValueY;
            previousValueY_ = startValueY;
        }

        void SetDragEndValue(int16_t endValueX, int16_t endValueY)
        {
            endValueX_ = endValueX;
            endValueY_ = endValueY;
        }

        void ResetCallback()
        {
            curtTime_ = 0;
            dragTimes_ = 0;
            startValueX_ = 0;
            endValueX_ = 0;
            startValueY_ = 0;
            endValueY_ = 0;
        }

        void Callback(UIView* view) override;

        uint16_t curtTime_;
        uint16_t dragTimes_;
        int16_t startValueX_;
        int16_t endValueX_;
        int16_t previousValueX_;
        int16_t startValueY_;
        int16_t endValueY_;
        int16_t previousValueY_;
    };

    bool DragThrowAnimator(Point currentPos, Point lastPos, uint8_t dragDirection, bool dragBack = true);

    virtual void StopAnimator();

    virtual bool DragXInner(int16_t distance) = 0;

    virtual bool DragYInner(int16_t distance) = 0;

    void RefreshDelta(int16_t distance)
    {
        lastDelta_[deltaIndex_ % MAX_DELTA_SIZE] = distance;
        deltaIndex_++;
    }

    void InitDelta();

    void RefreshRotate(int16_t distance)
    {
        lastRotate_[rotateIndex_ % MAX_DELTA_SIZE] = distance;
        rotateIndex_++;
    }

    void InitRotate();

    virtual void CalculateDragDistance(Point currentPos,
                                       Point lastPos,
                                       uint8_t dragDirection,
                                       int16_t& dragDistanceX,
                                       int16_t& dragDistanceY);

    void StartAnimator(int16_t dragDistanceX, int16_t dragDistanceY);

    virtual void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY){}

    int16_t GetMaxDelta() const;

    int16_t GetMaxRotate() const;

    void RefreshAnimator();

    virtual void FixDistance(int16_t& distanceX, int16_t& distanceY) {}

    uint16_t scrollBlankSize_ = 0;
    uint16_t reboundSize_ = 0;
    uint16_t maxScrollDistance_ = 0;
    int16_t lastDelta_[MAX_DELTA_SIZE] = {0};
    int16_t lastRotate_[MAX_DELTA_SIZE] = {0};
    uint8_t dragAccCoefficient_ = DRAG_ACC_FACTOR;
    uint8_t swipeAccCoefficient_ = 0;
    uint8_t direction_ : 2;
    uint8_t deltaIndex_ : 2;
    uint8_t rotateIndex_ : 2;
    uint8_t reserve_ : 2;
    bool throwDrag_ = false;
    EasingFunc easingFunc_;
    ListAnimatorCallback animatorCallback_;
    Animator scrollAnimator_;
#if ENABLE_ROTATE_INPUT
    uint8_t rotateAccCoefficient_ = 0;
    float rotateFactor_;
    uint8_t rotateThrowthreshold_;
    bool isRotating_;
#endif
    bool yScrollBarVisible_ = false;
    UIAbstractScrollBar* yScrollBar_ = nullptr;
    bool xScrollBarVisible_ = false;
    UIAbstractScrollBar* xScrollBar_ = nullptr;
    uint8_t scrollBarSide_;
    Point scrollBarCenter_;
    bool scrollBarCenterSetFlag_;
    bool dragBack_ = true;
#if DEFAULT_ANIMATION
    friend class BarEaseInOutAnimator;
    BarEaseInOutAnimator* barEaseInOutAnimator_ = nullptr;
#endif
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_ABSTRACT_SCROLL_H
