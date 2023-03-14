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
 * @file ui_view.h
 *
 * @brief Declares the base class of a view, providing basic view attributes and operations. All views are derived
 *        from this class.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_VIEW_H
#define GRAPHIC_LITE_UI_VIEW_H

#include "events/cancel_event.h"
#include "events/click_event.h"
#include "events/drag_event.h"
#include "events/event.h"
#include "events/long_press_event.h"
#include "events/press_event.h"
#include "events/release_event.h"
#if ENABLE_ROTATE_INPUT
#include "events/rotate_event.h"
#endif
#include "gfx_utils/color.h"
#include "gfx_utils/geometry2d.h"
#include "gfx_utils/graphic_buffer.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/style.h"
#include "gfx_utils/transform.h"

namespace OHOS {
/* Enumerates view types. */
enum UIViewType : uint8_t {
    UI_ROOT_VIEW = 0,
    UI_VIEW_GROUP,
    UI_LABEL,
    UI_ARC_LABEL,
    UI_LABEL_BUTTON,
    UI_CHECK_BOX,
    UI_TOGGLE_BUTTON,
    UI_RADIO_BUTTON,
    UI_IMAGE_VIEW,
    UI_BOX_PROGRESS,
    UI_SLIDER,
    UI_CIRCLE_PROGRESS,
    UI_SCROLL_VIEW,
    UI_LIST,
    UI_DIGITAL_CLOCK,
    UI_ANALOG_CLOCK,
    UI_PICKER,
    UI_SWIPE_VIEW,
    UI_TIME_PICKER,
    UI_ABSTRACT_CLOCK,
    UI_ABSTRACT_PROGRESS,
    UI_ABSTRACT_SCROLL,
    UI_AXIS,
    UI_BUTTON,
    UI_CANVAS,
    UI_CHART,
    UI_IMAGE_ANIMATOR_VIEW,
    UI_REPEAT_BUTTON,
    UI_TEXTURE_MAPPER,
    UI_DIALOG,
    UI_QRCODE,
    UI_NUMBER_MAX
};

#if ENABLE_DEBUG
const char* const VIEW_TYPE_STRING[UI_NUMBER_MAX] = {
    "RootView",         "UIViewGroup",     "UILabel",
    "UIArcLabel",       "UILabelButton",   "UICheckBox",
    "UIToggleButton",   "UIRadioButton",   "UIImageView",
    "UIBoxProgress",    "UISlider",        "UICircleProgress",
    "UIScrollView",     "UIList",          "UIDigitalClock",
    "UIAnalogClock",    "UIPicker",        "UISwipeView",
    "UITimePicker",     "UIAbstractClock", "UIAbstractProgress",
    "UIAbstractScroll", "UIAxis",          "UIButton",
    "UICanvas",         "UIChart",         "UIImageAnimatorView",
    "UIRepeatButton",   "UITextureMapper", "UIDialog",
    "UIQrcode",
};
#endif // ENABLE_DEBUG

/**
 * @brief Defines the base class of a view, providing basic view attributes and operations. All views are derived
 *        from this class.
 *
 * @since 1.0
 * @version 1.0
 */
class UIView : public HeapBase {
public:
    /**
     * @brief Defines a click event listener. You need to register this listener with the view to listen to
     *        click events.
     *
     * @since 1.0
     * @version 1.0
     */
    class OnClickListener : public HeapBase {
    public:
        /**
         * @brief Called when a view is clicked.
         * @param view Indicates the view clicked.
         * @param event Indicates the click event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnClick(UIView& view, const ClickEvent& event)
        {
            return false;
        }

        /**
         * @brief A destructor used to delete the <b>OnClickListener</b> instance.
         * @since 1.0
         * @version 1.0
         */
        virtual ~OnClickListener() {}
    };

    /**
     * @brief Defines a long-press event listener. You need to register this listener with the view to listen to
     *        long-press events.
     *
     * @since 1.0
     * @version 1.0
     */
    class OnLongPressListener : public HeapBase {
    public:
        /**
         * @brief Called when a view is long pressed.
         * @param view Indicates the view long pressed.
         * @param event Indicates the long-press event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnLongPress(UIView& view, const LongPressEvent& event)
        {
            return false;
        }

        /**
         * @brief A destructor used to delete the <b>OnLongPressListener</b> instance.
         * @since 1.0
         * @version 1.0
         */
        virtual ~OnLongPressListener() {}
    };

    /**
     * @brief Defines a drag event listener. You need to register this listener with the view to listen to drag events.
     *
     * @since 1.0
     * @version 1.0
     */
    class OnDragListener : public HeapBase {
    public:
        /**
         * @brief Called when a view starts to drag.
         * @param view Indicates the view dragged.
         * @param event Indicates the drag event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnDragStart(UIView& view, const DragEvent& event)
        {
            return false;
        }

        /**
         * @brief Called when a view is being dragged.
         * @param view Indicates the view dragged.
         * @param event Indicates the drag event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnDrag(UIView& view, const DragEvent& event)
        {
            return false;
        }

        /**
         * @brief Called when a view stops dragging.
         * @param view Indicates the view dragged.
         * @param event Indicates the drag event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnDragEnd(UIView& view, const DragEvent& event)
        {
            return false;
        }

        /**
         * @brief A destructor used to delete the <b>OnDragListener</b> instance.
         * @since 1.0
         * @version 1.0
         */
        virtual ~OnDragListener() {}
    };

    /**
     * @brief Defines a touch event listener. You need to register this listener with the view to listen to touch
     *        events.
     *
     * @since 1.0
     * @version 1.0
     */
    class OnTouchListener : public HeapBase {
    public:
        /**
         * @brief Called when a view is pressed.
         * @param view Indicates the view pressed.
         * @param event Indicates the press event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnPress(UIView& view, const PressEvent& event)
        {
            return false;
        }

        /**
         * @brief Called when a view is released.
         * @param view Indicates the view released.
         * @param event Indicates the release event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnRelease(UIView& view, const ReleaseEvent& event)
        {
            return false;
        }

        /**
         * @brief Called when a click event on a view is canceled.
         * @param view Indicates the view on which a click event is canceled.
         * @param event Indicates the cancel event.
         * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
         *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
         *         it is transferred to the parent view after local processing is complete.)
         * @since 1.0
         * @version 1.0
         */
        virtual bool OnCancel(UIView& view, const CancelEvent& event)
        {
            return false;
        }

        /**
         * @brief A destructor used to delete the <b>OnTouchListener</b> instance.
         * @since 1.0
         * @version 1.0
         */
        virtual ~OnTouchListener() {}
    };

#if ENABLE_ROTATE_INPUT
    /**
     * @brief Defines a rotation event listener.
     *        You need to register this listener with the view to listen for rotation events.
     * @since 5.0
     * @version 3.0
     */
    class OnRotateListener : public HeapBase {
    public:
        /**
         * @brief Called when the view starts to rotate.
         * @param view Indicates the view that responds to the rotation event.
         * @param event Indicates the rotation event.
         * @return Returns <b>true</b> if the rotation event is consumed; returns <b>false</b> otherwise.
         * @since 6
         */
        virtual bool OnRotateStart(UIView& view, const RotateEvent& event)
        {
            return false;
        }

        /**
         * @brief Called when a rotation event occurs on a view.
         * @param view Indicates the view that responds to the rotation event.
         * @param event Indicates the rotation event.
         * @return Returns <b>true</b> if the rotation event is consumed; returns <b>false</b> otherwise.
         * @since 5.0
         * @version 3.0
         */
        virtual bool OnRotate(UIView& view, const RotateEvent& event)
        {
            return false;
        }

        /**
         * @brief Called when the view stops rotating.
         * @param view Indicates the view that responds to the rotation event.
         * @param event Indicates the rotation event.
         * @return Returns <b>true</b> if the rotation event is consumed; returns <b>false</b> otherwise.
         * @since 6
         */
        virtual bool OnRotateEnd(UIView& view, const RotateEvent& event)
        {
            return false;
        }

        /**
         * @brief A destructor used to delete an <b>OnRotateListener</b> instance.
         * @since 5.0
         * @version 3.0
         */
        virtual ~OnRotateListener() {}
    };

    /**
     * @brief Called when the view starts to rotate.
     * @param event Indicates the rotation event.
     * @since 6
     */
    virtual bool OnRotateStartEvent(const RotateEvent& event);

    /**
     * @brief Called when a rotation event occurs on the view.
     * @param event Indicates the rotation event.
     * @since 5.0
     * @version 3.0
     */
    virtual bool OnRotateEvent(const RotateEvent& event);

    /**
     * @brief Called when the view stops rotating.
     * @param event Indicates the rotation event.
     * @since 6
     */
    virtual bool OnRotateEndEvent(const RotateEvent& event);

    /**
     * @brief Sets a rotation event listener for the view.
     * @param onRotateListener Indicates the pointer to the rotation event listener to set.
     * @since 5.0
     * @version 3.0
     */
    void SetOnRotateListener(OnRotateListener* onRotateListener)
    {
        onRotateListener_ = onRotateListener;
    }

    /**
     * @brief Obtains the rotation event listener for the view.
     * @return Returns the rotation event listener.
     * @since 5.0
     * @version 3.0
     */
    OnRotateListener*& GetOnRotateListener()
    {
        return onRotateListener_;
    }
#endif

    /**
     * @brief Stores extra information about a <b>UIView</b> instance.
     * @param elementPtr Indicates the void pointer to the extra information about the <b>UIView</b> instance.
     * @since 5.0
     * @version 3.0
     */
    struct ViewExtraMsg {
        void* elementPtr;
    };

    /**
     * @brief A default constructor used to create an <b>UIView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    UIView();

    /**
     * @brief A constructor used to create an <b>UIView</b> instance.
     * @param id Indicates the pointer to the view ID.
     * @since 1.0
     * @version 1.0
     */
    explicit UIView(const char* id) : UIView()
    {
        id_ = id;
    }

    /**
     * @brief A destructor used to delete the <b>UIView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIView();

    /**
     * @brief Called before a view is drawn. This function is used to check whether the invalidated area
     *        can be fully cover by this view so as to optimize the drawing process.
     * @param invalidatedArea Indicates the area to judge and returns the covered area when partly coverd.
     * @return Returns <b>true</b> if the invalidated area is fully covered; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnPreDraw(Rect& invalidatedArea) const;

    /**
     * @brief Called when a view is drawn.
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);

    /**
     * @brief Called after a view is drawn.
     * @param invalidatedArea Indicates the area in which the view is drawn.
     * @since 1.0
     * @version 1.0
     */
    virtual void OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);

    /**
     * @brief Remeasures the view size.
     * @since 1.0
     * @version 1.0
     */
    virtual void ReMeasure() {}

    /**
     * @brief Refreshes the invalidated area of the view.
     * @since 1.0
     * @version 1.0
     */
    void Invalidate();

    /**
     * @brief Refreshes a view in a specified invalidated area.
     * @param invalidatedArea Indicates the area to refresh.
     * @since 1.0
     * @version 1.0
     */
    void InvalidateRect(const Rect& invalidatedArea);

    /**
     * @brief Called when the view is long pressed.
     * @param event Indicates the long-press event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnLongPressEvent(const LongPressEvent& event);

    /**
     * @brief Called when the view starts to drag.
     * @param event Indicates the drag event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnDragStartEvent(const DragEvent& event);

    /**
     * @brief Called when the view is being dragged.
     * @param event Indicates the drag event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnDragEvent(const DragEvent& event);

    /**
     * @brief Called when the view stops dragging.
     * @param event Indicates the drag event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnDragEndEvent(const DragEvent& event);

    /**
     * @brief Called when the view is clicked.
     * @param event Indicates the click event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnClickEvent(const ClickEvent& event);

    /**
     * @brief Called when the view is pressed.
     * @param event Indicates the press event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnPressEvent(const PressEvent& event);

    /**
     * @brief Called when the view is released.
     * @param event Indicates the release event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnReleaseEvent(const ReleaseEvent& event);

    /**
     * @brief Called when a click event on the view is canceled.
     * @param event Indicates the cancel event.
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     * @since 1.0
     * @version 1.0
     */
    virtual bool OnCancelEvent(const CancelEvent& event);

    /**
     * @brief Sets a drag event listener for the view.
     * @param onDragListener Indicates the pointer to the drag event listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnDragListener(OnDragListener* onDragListener)
    {
        onDragListener_ = onDragListener;
    }

    /**
     * @brief Obtains the drag event listener for the view.
     * @return Returns the drag event listener.
     * @since 1.0
     * @version 1.0
     */
    OnDragListener*& GetOnDragListener()
    {
        return onDragListener_;
    }

    /**
     * @brief Sets a click event listener for the view.
     * @param onClickListener Indicates the pointer to the click event listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnClickListener(OnClickListener* onClickListener)
    {
        onClickListener_ = onClickListener;
    }

    /**
     * @brief Obtains the click event listener for the view.
     * @return Returns the click event listener.
     * @since 1.0
     * @version 1.0
     */
    OnClickListener*& GetOnClickListener()
    {
        return onClickListener_;
    }

    /**
     * @brief Sets a long-press event listener for the view.
     * @param onLongPressListener Indicates the pointer to the long-press event listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnLongPressListener(OnLongPressListener* onLongPressListener)
    {
        onLongPressListener_ = onLongPressListener;
    }

    /**
     * @brief Obtains the long-press event listener for the view.
     * @return Returns the long-press event listener.
     * @since 1.0
     * @version 1.0
     */
    OnLongPressListener*& GetOnLongPressListener()
    {
        return onLongPressListener_;
    }

    /**
     * @brief Sets a touch event listener for the view.
     * @param onTouchListener Indicates the pointer to the touch event listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnTouchListener(OnTouchListener* onTouchListener)
    {
        onTouchListener_ = onTouchListener;
    }

    /**
     * @brief Obtains the touch event listener for the view.
     * @return Returns the touch event listener.
     * @since 1.0
     * @version 1.0
     */
    OnTouchListener*& GetTouchListener()
    {
        return onTouchListener_;
    }

    /**
     * @brief Obtains the top-level view based on specified coordinates.
     * @param point Indicates the coordinates to specify.
     * @param last Indicates the double pointer to the view that contains the specified coordinates.
     * @since 1.0
     * @version 1.0
     */
    virtual void GetTargetView(const Point& point, UIView** last);

    /**
     * @brief Obtains the current view and target view based on specified coordinates. The obtained current view must
     *        include the specified coordinates and is a visible top view that can respond to touch events, and the
     *        obtained target view must be the top view that includes the specified coordinates.
     * @param point Indicates the specified coordinates.
     * @param current Indicates the double pointer to the current view to obtain.
     *        <b>nullptr</b> indicates that the target view fails to be obtained.
     * @param target Indicates the double pointer to the target view to obtain.
     *        <b>nullptr</b> indicates that the target view fails to be obtained.
     * @since 5.0
     * @version 3.0
     */
    virtual void GetTargetView(const Point& point, UIView** current, UIView** target);

    /**
     * @brief Sets the parent view for the view.
     * @param parent Indicates the pointer to the parent view to set.
     * @since 1.0
     * @version 1.0
     */
    void SetParent(UIView* parent)
    {
        parent_ = parent;
    }

    /**
     * @brief Obtains the parent view of the view.
     * @return Returns the pointer to the parent view.
     * @since 1.0
     * @version 1.0
     */
    UIView* GetParent() const
    {
        return parent_;
    }

    /**
     * @brief Sets the next sibling view for the view.
     * @param sibling Indicates the pointer to the next sibling view to set.
     * @since 1.0
     * @version 1.0
     */
    void SetNextSibling(UIView* sibling)
    {
        nextSibling_ = sibling;
    }

    /**
     * @brief Obtains the next sibling view of the view.
     * @return Returns the pointer to the next sibling view.
     * @since 1.0
     * @version 1.0
     */
    UIView* GetNextSibling() const
    {
        return nextSibling_;
    }

    /**
     * @brief Sets whether the view is visible.
     * @param visible Specifies whether to set the view visible. Value <b>true</b> means to set the view visible,
     *                and <b>false</b> means the opposite.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetVisible(bool visible)
    {
        if (visible_ != visible) {
            visible_ = visible;
            needRedraw_ = true;
            Invalidate();
        }
    }

    /**
     * @brief Checks whether the view is visible.
     * @return Returns <b>true</b> if the view is visible; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsVisible() const
    {
        return visible_;
    }

    /**
     * @brief Sets whether the view is touchable.
     * @param touchable Specifies whether to set the view touchable. Value <b>true</b> means to set the view touchable,
     *                  and <b>false</b> means the opposite.
     * @since 1.0
     * @version 1.0
     */
    void SetTouchable(bool touch)
    {
        touchable_ = touch;
    }

    /**
     * @brief Checks whether the view is touchable.
     * @return Returns <b>true</b> if the view is touchable; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsTouchable() const
    {
        return touchable_;
    }

    /**
     * @brief Sets whether the view is draggable.
     * @param draggable Specifies whether to set the view draggable. Value <b>true</b> means to set the view draggable,
     *                  and <b>false</b> means the opposite.
     * @since 1.0
     * @version 1.0
     */
    void SetDraggable(bool draggable)
    {
        draggable_ = draggable;
        dragParentInstead_ = !draggable;
    }

    /**
     * @brief Checks whether the view is draggable.
     * @return Returns <b>true</b> if the view is draggable; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsDraggable() const
    {
        return draggable_;
    }

    /**
     * @brief Sets whether to transfer the drag event to the parent view for processing when the view is being dragged.
     * @param dragParentInstead Specifies whether to transfer the event to the parent view for processing.
     *                          Value <b>true</b> means to transfer the event to the parent view for processing,
     *                          and <b>false</b> means the opposite.
     * @since 1.0
     * @version 1.0
     */
    void SetDragParentInstead(bool dragParentInstead)
    {
        dragParentInstead_ = dragParentInstead;
    }

    /**
     * @brief Obtains whether the view transfers a drag event to the parent view for processing.
     * @return Returns <b>true</b> if the view transfers the event to the parent view for processing;
     *         returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsDragParentInstead() const
    {
        return dragParentInstead_;
    }

    /**
     * @brief Obtains the absolute rectangle area of the view. When the view has deformation such as rotation,
     *        the rectangle area is the intersection set of the absolute rectangle area and deformation matrix.
     * @return Returns the absolute rectangle area.
     * @since 1.0
     * @version 1.0
     */
    Rect GetRect() const;

    /**
     * @brief Obtains the visible absolute rectangle area of the view.
     * @return Returns the visible absolute rectangle area.
     * @since 1.0
     * @version 1.0
     */
    Rect GetVisibleRect() const;

    /**
     * @brief Obtains the valid absolute rectangle area of the view. The valid area refers to the area where the view
     *        can be displayed. Generally, the valid area is the same as the visible view area, but they may be
     *        different in the grid layout.
     * @return Returns the valid absolute rectangle area.
     * @since 1.0
     * @version 1.0
     */
    Rect GetMaskedRect() const;

    /**
     * @brief Obtains the absolute rectangle area of the view.
     * @return Returns the absolute rectangle area.
     * @since 1.0
     * @version 1.0
     */
    Rect GetOrigRect() const;

    /**
     * @brief Obtains the content of the absolute rectangle area of the view. This area excludes padding.
     * @return Returns the content of the absolute rectangle area.
     * @since 1.0
     * @version 1.0
     */
    virtual Rect GetContentRect();

    /**
     * @brief Obtains the rectangular area of the view relative to the parent view, that is, the rectangular area
     *        relative to the coordinates of the parent view.
     * @return Returns the rectangle area relative to the parent view.
     * @since 1.0
     * @version 1.0
     */
    Rect GetRelativeRect() const
    {
        return rect_;
    }

    /**
     * @brief Adjusts the size of the visible area. This operation may affect the final display size.
     * @param x Indicates the new x-coordinate.
     * @param y Indicates the new y-coordinate.
     * @param width Indicates the new width.
     * @param height Indicates the new height.
     * @since 1.0
     * @version 1.0
     */
    void ResizeVisibleArea(int16_t x, int16_t y, int16_t width, int16_t height)
    {
        if (visibleRect_ == nullptr) {
            visibleRect_ = new Rect();
            if (visibleRect_ == nullptr) {
                GRAPHIC_LOGE("new Rect fail");
                return;
            }
        }
        visibleRect_->SetWidth(width);
        visibleRect_->SetHeight(height);
        visibleRect_->SetPosition(x, y);
    }

    /**
     * @brief Sets the width for the view.
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetWidth(int16_t width)
    {
        if (GetWidth() != width) {
            int16_t newWidth = width + style_->paddingLeft_ + style_->paddingRight_ +
                               (style_->borderWidth_ * 2); /* 2: left and right border */
            rect_.SetWidth(newWidth);
        }
    }

    /**
     * @brief Sets a percentage that represents the proportion of the view's width to the parent view's width.
     * @param widthPercent Indicates the percentage to set, the decimal form of which ranges from 0 to 1.
     * @since 5.0
     * @version 3.0
     */
    virtual void SetWidthPercent(float widthPercent);

    /**
     * @brief Obtains the width for the view.
     * @return Returns the view width.
     * @since 1.0
     * @version 1.0
     */
    virtual int16_t GetWidth()
    {
        return rect_.GetWidth() - (style_->paddingLeft_ + style_->paddingRight_) -
               (style_->borderWidth_ * 2); /* 2: left and right border */
    }

    /**
     * @brief Sets the height for the view.
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetHeight(int16_t height)
    {
        if (GetHeight() != height) {
            int16_t newHeight = height + style_->paddingTop_ + style_->paddingBottom_ +
                                (style_->borderWidth_ * 2); /* 2: top and bottom border */
            rect_.SetHeight(newHeight);
        }
    }

    /**
     * @brief Sets a percentage that represents the proportion of the view's hieght to the parent view's hieght.
     * @param widthPercent Indicates the percentage to set, the decimal form of which ranges from 0 to 1.
     * @since 5.0
     * @version 3.0
     */
    virtual void SetHeightPercent(float heightPercent);

    /**
     * @brief Obtains the height for the view.
     * @return Returns the view height.
     * @since 1.0
     * @version 1.0
     */
    virtual int16_t GetHeight()
    {
        return rect_.GetHeight() - (style_->paddingTop_ + style_->paddingBottom_) -
               (style_->borderWidth_ * 2); /* 2: top and bottom border */
    }

    /**
     * @brief Adjusts the size of the view.
     * @param width Indicates the new width.
     * @param height Indicates the new height.
     * @since 1.0
     * @version 1.0
     */
    virtual void Resize(int16_t width, int16_t height)
    {
        SetWidth(width);
        SetHeight(height);
    }

    /**
     * @brief Adjusts the size of the view based on specified percentages.
     * @param widthPercent Indicates the percentage that represents the proportion of the view's width
     *        to the parent view's width to set, the decimal form of which ranges from 0 to 1.
     * @param heightPercent Indicates the percentage that represents the proportion of the view's height
     *        to the parent view's height to set, the decimal form of which ranges from 0 to 1.
     * @since 5.0
     * @version 3.0
     */
    virtual void ResizePercent(float widthPercent, float heightPercent);

    /**
     * @brief Sets the x-coordinate for the view.
     * @param x Indicates the x-coordinate to set.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetX(int16_t x)
    {
        if (GetX() != x) {
            rect_.SetX(x + GetStyle(STYLE_MARGIN_LEFT));
        }
    }

    /**
     * @brief Sets a percentage that represents the proportion of the view's x-coordinate
     *        to the parent view's x-coordinate.
     * @param xPercent Indicates the percentage to set, the decimal form of which ranges from 0 to 1.
     * @since 5.0
     * @version 3.0
     */
    virtual void SetXPercent(float xPercent);

    /**
     * @brief Obtains the x-coordinate for the view.
     * @return Returns the x-coordinate.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetX() const
    {
        return rect_.GetX() - GetStyle(STYLE_MARGIN_LEFT);
    }

    /**
     * @brief Sets the y-coordinate for the view.
     * @param y Indicates the y-coordinate to set.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetY(int16_t y)
    {
        if (GetY() != y) {
            rect_.SetY(y + GetStyle(STYLE_MARGIN_TOP));
        }
    }

    /**
     * @brief Sets a percentage that represents the proportion of the view's y-coordinate
     *        to the parent view's y-coordinate.
     * @param yPercent Indicates the percentage to set, the decimal form of which ranges from 0 to 1.
     * @since 5.0
     * @version 3.0
     */
    virtual void SetYPercent(float yPercent);

    /**
     * @brief Obtains the y-coordinate for the view.
     * @return Returns the y-coordinate.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetY() const
    {
        return rect_.GetY() - GetStyle(STYLE_MARGIN_TOP);
    }

    /**
     * @brief 获取组件设置margin属性后margin的宽度，包括组件宽度加marginLeft 加 marginRight.
     * @return margin的宽度
     * @since 6
     */
    int16_t GetWidthWithMargin()
    {
        return GetRelativeRect().GetWidth() + GetStyle(STYLE_MARGIN_LEFT) + GetStyle(STYLE_MARGIN_RIGHT);
    }

    /**
     * @brief 获取组件设置margin属性后margin的高度度，包括组件宽度加marginTop 加 marginBottom.
     * @return margin的高度
     * @since 6
     */
    int16_t GetHeightWithMargin()
    {
        return GetRelativeRect().GetHeight() + GetStyle(STYLE_MARGIN_TOP) + GetStyle(STYLE_MARGIN_BOTTOM);
    }

    /**
     * @brief Sets the position for the view.
     * @param x Indicates the x-coordinate to set.
     * @param y Indicates the y-coordinate to set.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetPosition(int16_t x, int16_t y)
    {
        SetX(x);
        SetY(y);
    }

    /**
     * @brief Sets the position percentages for the view.
     * @param xPercent Indicates the percentage that represents the proportion of the view's x-coordinate
     *        to the parent view's x-coordinate to set, the decimal form of which ranges from 0 to 1.
     * @param yPercent Indicates the percentage that represents the proportion of the view's y-coordinate
     *        to the parent view's y-coordinate to set, the decimal form of which ranges from 0 to 1.
     * @since 5.0
     * @version 3.0
     */
    virtual void SetPositionPercent(float xPercent, float yPercent);

    /**
     * @brief Adjusts the position and size of the view.
     * @param x Indicates the new x-coordinate.
     * @param y Indicates the new y-coordinate.
     * @param width Indicates the new width.
     * @param height Indicates the new height.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetPosition(int16_t x, int16_t y, int16_t width, int16_t height)
    {
        SetPosition(x, y);
        SetWidth(width);
        SetHeight(height);
    }

    /**
     * @brief Sets the position and size percentages for the view.
     * @param xPercent Indicates the percentage that represents the proportion of the view's x-coordinate
     *        to the parent view's x-coordinate to set, the decimal form of which ranges from 0 to 1.
     * @param yPercent Indicates the percentage that represents the proportion of the view's y-coordinate
     *        to the parent view's y-coordinate, the decimal form of which ranges from 0 to 1.
     * @param widthPercent Indicates the percentage that represents the proportion of the view's width
     *        to the parent view's width, the decimal form of which ranges from 0 to 1.
     * @param heightPercent Indicates the percentage that represents the proportion of the view's height
     *        to the parent view's height, the decimal form of which ranges from 0 to 1.
     * @since 5.0
     * @version 3.0
     */
    virtual void SetPositionPercent(float xPercent, float yPercent, float widthPercent, float heightPercent);

    /**
     * @brief Checks whether the view is a container view.
     * @return Returns <b>true</b> if the view is a container view; returns <b>false</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    bool IsViewGroup() const
    {
        return isViewGroup_;
    }

    /**
     * @brief Sets whether to intercept the input event. If intercepted, the view does not transfer the input event to
     *        the parent view after local processing.
     * @param isIntercept Specifies whether to intercept the input event. Value <b>true</b> means to intercept the input
     *                    event, and <b>false</b> means the opposite.
     * @since 1.0
     * @version 1.0
     */
    void SetIntercept(bool isIntercept)
    {
        isIntercept_ = isIntercept;
    }

    /**
     * @brief Gets whether to intercept the input event. If intercepted, the view does not transfer the input event to
     *        the parent view after local processing.
     * @return Returns <b>true</b> if intercept the input event, and <b>false</b> means the opposite.
     * @since 1.0
     * @version 1.0
     */
    bool IsIntercept()
    {
        return isIntercept_;
    }

    /**
     * @brief Sets the affine transformation matrix.
     * @param transMap Indicates the transformation matrix.
     * @since 1.0
     * @version 1.0
     */
    void SetTransformMap(const TransformMap& transMap);

    /**
     * @brief Obtains an affine transformation matrix.
     * @return Returns the transform matrix.
     * @since 1.0
     * @version 1.0
     */
    TransformMap& GetTransformMap()
    {
        if (transMap_ == nullptr) {
            transMap_ = new TransformMap();
        }
        return *transMap_;
    }

    /**
     * @brief Obtains the child view of a specified ID.
     * @return Returns the pointer to the child view.
     * @since 1.0
     * @version 1.0
     */
    virtual UIView* GetChildById(const char* id) const
    {
        return nullptr;
    }

    /**
     * @brief Sets the view ID.
     * @param id Indicates the pointer to the view ID.
     * @since 1.0
     * @version 1.0
     */
    void SetViewId(const char* id)
    {
        id_ = id;
    }

    /**
     * @brief Obtains the view ID.
     * @return Returns the pointer to the view ID.
     * @since 1.0
     * @version 1.0
     */
    const char* GetViewId() const
    {
        return id_;
    }

    /**
     * @brief Sets the view index.
     * @param index Indicates the view index to set.
     * @since 1.0
     * @version 1.0
     */
    void SetViewIndex(int16_t index)
    {
        index_ = index;
    }

    /**
     * @brief Obtains the view index.
     * @return Returns the view index.
     * @since 1.0
     * @version 1.0
     */
    int16_t GetViewIndex() const
    {
        return index_;
    }

    /**
     * @brief Obtains the view type.
     * @return Returns the view type.
     * @since 1.0
     * @version 1.0
     */
    virtual UIViewType GetViewType() const
    {
        return UI_NUMBER_MAX;
    }

    /**
     * @brief Lays out all child views according to the preset arrangement mode.
     * @param needInvalidate Specifies whether to refresh the invalidated area after the layout is complete.
     *                       Value <b>true</b> means to refresh the invalidated area after the layout is complete,
     *                       and <b>false</b> means the opposite.
     * @since 1.0
     * @version 1.0
     */
    virtual void LayoutChildren(bool neeInvalidate = false) {}

    /**
     * @brief Lays out the view in the center of the parent view.
     * @param xOffset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *                the offset to the right, and a negative number indicates the offset to the left.
     * @param yOffset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *                the offset to the bottom, and a negative number indicates the offset to the top.
     * @since 1.0
     * @version 1.0
     */
    void LayoutCenterOfParent(int16_t xOffSet = 0, int16_t yOffset = 0);

    /**
     * @brief Lays out the view on the left of the parent view.
     * @param offset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *               the offset to the right, and a negative number indicates the offset to the left.
     * @since 1.0
     * @version 1.0
     */
    void LayoutLeftOfParent(int16_t offset = 0);

    /**
     * @brief Lays out the view on the right of the parent view.
     * @param offset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *               the offset to the left, and a negative number indicates the offset to the right.
     * @since 1.0
     * @version 1.0
     */
    void LayoutRightOfParent(int16_t offset = 0);

    /**
     * @brief Lays out the view on the top of the parent view.
     * @param offset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *               the offset to the bottom, and a negative number indicates the offset to the top.
     * @since 1.0
     * @version 1.0
     */
    void LayoutTopOfParent(int16_t offset = 0);

    /**
     * @brief Lays out the view on the bottom of the parent view.
     * @param offset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *               the offset to the top, and a negative number indicates the offset to the bottom.
     * @since 1.0
     * @version 1.0
     */
    void LayoutBottomOfParent(int16_t offset = 0);

    /**
     * @brief Aligns the view with the left of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *               the offset to the right, and a negative number indicates the offset to the left.
     * @since 1.0
     * @version 1.0
     */
    void AlignLeftToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Aligns the view with the right of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *               the offset to the left, and a negative number indicates the offset to the right.
     * @since 1.0
     * @version 1.0
     */
    void AlignRightToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Aligns the view with the top of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *               the offset to the bottom, and a negative number indicates the offset to the top.
     * @since 1.0
     * @version 1.0
     */
    void AlignTopToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Aligns the view with the bottom of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *               the offset to the top, and a negative number indicates the offset to the bottom.
     * @since 1.0
     * @version 1.0
     */
    void AlignBottomToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Aligns the view with the center of a sibling view in the x-axis.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *               the offset to the right, and a negative number indicates the offset to the left.
     * @since 1.0
     * @version 1.0
     */
    void AlignHorCenterToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Aligns the view with the center of a sibling view in the y-axis.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *               the offset to the bottom, and a negative number indicates the offset to the top.
     * @since 1.0
     * @version 1.0
     */
    void AlignVerCenterToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Lays out the view on the left of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *               the offset to the left, and a negative number indicates the offset to the right.
     * @since 1.0
     * @version 1.0
     */
    void LayoutLeftToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Lays out the view on the right of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the x-axis after the view is placed. A positive number indicates
     *               the offset to the right, and a negative number indicates the offset to the left.
     * @since 1.0
     * @version 1.0
     */
    void LayoutRightToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Lays out the view on the above of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *               the offset to the top, and a negative number indicates the offset to the bottom.
     * @since 1.0
     * @version 1.0
     */
    void LayoutTopToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Lays out the view on the below of a sibling view.
     * @param id Indicates the pointer to the ID of the sibling view.
     * @param offset Indicates the offset added to the y-axis after the view is placed. A positive number indicates
     *               the offset to the bottom, and a negative number indicates the offset to the top.
     * @since 1.0
     * @version 1.0
     */
    void LayoutBottomToSibling(const char* id, int16_t offset = 0);

    /**
     * @brief Sets the view style.
     * @param style Indicates the view style.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetStyle(Style& style);

    /**
     * @brief Sets a style.
     *
     * @param key Indicates the key of the style to set.
     * @param value Indicates the value matching the key.
     * @since 1.0
     * @version 1.0
     */
    virtual void SetStyle(uint8_t key, int64_t value);

    /**
     * @brief Obtains the value of a style.
     *
     * @param key Indicates the key of the style.
     * @return Returns the value of the style.
     * @since 1.0
     * @version 1.0
     */
    virtual int64_t GetStyle(uint8_t key) const
    {
        return style_->GetStyle(key);
    }

    /**
     * @brief Obtains the view style. This function applies to scenarios where the style does not need to be modified,
     *        which saves memory.
     * @return Returns the view style.
     * @since 1.0
     * @version 1.0
     */
    const Style& GetStyleConst() const
    {
        return *style_;
    }

    /**
     * @brief Sets the opacity for the view.
     *
     * @param opaScale Indicates the opacity to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOpaScale(uint8_t opaScale)
    {
        opaScale_ = opaScale;
    }

    /**
     * @brief Obtains the view opacity.
     *
     * @return Returns the view opacity.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetOpaScale() const
    {
        return opaScale_;
    }

    /**
     * @brief Obtains the extra message about a <b>UIView</b> instance. This field is ignored by the graphics
     *        framework and can be anything you set.
     *
     * @return Returns the pointer to the extra message about the <b>UIView</b> instance.
     * @since 5.0
     * @version 3.0
     */
    ViewExtraMsg* GetExtraMsg()
    {
        return viewExtraMsg_;
    }

    /**
     * @brief Sets the extra message about a <b>UIView</b> instance. This field is ignored by the graphics
     *        framework and can be anything you set.
     *
     * @param extraMsg Indicates the extra message to set.
     * @since 5.0
     * @version 3.0
     */
    void SetExtraMsg(ViewExtraMsg* extraMsg)
    {
        viewExtraMsg_ = extraMsg;
    }

    /**
     * @brief Rotates the view in 2d.
     * @param angle Indicates the rotation angle.
     * @param pivot Indicates the coordinates of the rotation pivot.
     * @since 5.0
     * @version 3.0
     */
    void Rotate(int16_t angle, const Vector2<float>& pivot);

    /**
     * @brief Rotates the view in 3d.
     * @param angle Indicates the rotation angle.
     * @param pivotStart Indicates the coordinates of the rotation start pivot.
     * @param pivotEnd Indicates the coordinates of the rotation end pivot.
     * @since 5.0
     * @version 3.0
     */
    void Rotate(int16_t angle, const Vector3<float>& pivotStart, const Vector3<float>& pivotEnd);

    /**
     * @brief Scales the view in 2d.
     *
     * @param scale Indicates the scale factor on x- and y- axes.
     * @param pivot Indicates the scaling pivot.
     * @since 5.0
     * @version 3.0
     */
    void Scale(const Vector2<float>& scale, const Vector2<float>& pivot);

    /**
     * @brief Scales the view in 3d.
     *
     * @param scale Indicates the scale factor on x- and y- axes.
     * @param pivot Indicates the scaling pivot.
     * @since 5.0
     * @version 3.0
     */
    void Scale(const Vector3<float>& scale, const Vector3<float>& pivot);

    /**
     * @brief Shears the view in 3d.
     *
     * @param shearX Indicates the shear parameters around x- axes,
     *               which means many it shears in y and z direction(current invalid).
     * @param shearY Indicates the shear parameters around y- axes,
     *               which means many it shears in x and z direction(current invalid).
     * @param shaerZ Indicates the shear parameters around z- axes,
     *               which means many it shears in x and y.
     * @since 5.0
     * @version 3.0
     */
    void Shear(const Vector2<float>& shearX, const Vector2<float>& shearY, const Vector2<float>& shearZ);

    void Translate(const Vector2<int16_t>& trans);

    void Translate(const Vector3<int16_t>& trans);

    bool IsTransInvalid();

    void SetCameraDistance(int16_t distance);

    void SetCameraPosition(const Vector2<float>& position);

    void ResetTransParameter();

#if ENABLE_ROTATE_INPUT
    /**
     * @brief Requests the focus on the view.
     *
     * @since 5.0
     * @version 3.0
     */
    virtual void RequestFocus();

    /**
     * @brief Clears the focus on the view.
     *
     * @since 5.0
     * @version 3.0
     */
    virtual void ClearFocus();
#endif
#if ENABLE_FOCUS_MANAGER
    /**
     * @brief 设置视图是否可获焦.
     *
     * @param focusable 是否可获焦.
     * @since 5.0
     * @version 3.0
     */
    void SetFocusable(bool focusable)
    {
        focusable_ = focusable;
    }

    /**
     * @brief 获取视图是否可获焦.
     *
     * @return 是否可获焦.
     * @since 5.0
     * @version 3.0
     */
    bool IsFocusable() const
    {
        return focusable_;
    }

    /**
     * @brief 组件获焦响应
     *
     * @since 5.0
     * @version 3.0
     */
    void Focus();

    /**
     * @brief 组件失焦响应
     *
     * @since 5.0
     * @version 3.0
     */
    void Blur();

    /**
     * @brief 焦点改变事件监听类，开发者需要向视图组件注册该类实现事件的监听.
     *
     * @since 5.0
     * @version 3.0
     */
    class OnFocusListener : public HeapBase {
    public:
        /**
         * @brief 回调函数，视图获焦时触发.
         * @param view 获焦的视图
         * @since 5.0
         * @version 3.0
         */
        virtual bool OnFocus(UIView& view)
        {
            return false;
        }

        /**
         * @brief 回调函数，视图失焦时触发.
         * @param view 失焦的视图
         * @since 5.0
         * @version 3.0
         */
        virtual bool OnBlur(UIView& view)
        {
            return false;
        }

        /**
         * @brief 析构函数.
         * @since 5.0
         * @version 3.0
         */
        virtual ~OnFocusListener() {}
    };

    /**
     * @brief 设置当前视图焦点改变事件监听者.
     * @param onFocusListener 焦点改变事件监听者.
     * @since 5.0
     * @version 3.0
     */
    void SetOnFocusListener(OnFocusListener* onFocusListener)
    {
        onFocusListener_ = onFocusListener;
    }

    /**
     * @brief 获取当前视图焦点改变事件监听者.
     * @return 焦点改变事件监听者.
     * @since 5.0
     * @version 3.0
     */
    OnFocusListener* GetOnFocusListener() const
    {
        return onFocusListener_;
    }
#endif

    /**
     * @brief 获取当前视图的bitmap截图.请注意该接口会申请内存，请在需要释放时使用{@link ImageCacheFree()}接口.
     * @param info bitmap存储对象，获取的截图将被存到该引用中.
     * @return bitmap是否获取成功.
     * @since 5.0
     * @version 3.0
     */
    bool GetBitmap(ImageInfo& bitmap);

    bool IsOnViewTree();

protected:
    bool touchable_ : 1;
    bool visible_ : 1;
    bool draggable_ : 1;
    bool dragParentInstead_ : 1;
    bool isViewGroup_ : 1;
    bool needRedraw_ : 1;
    bool styleAllocFlag_ : 1;
    bool isIntercept_ : 1;
#if ENABLE_FOCUS_MANAGER
    bool focusable_ : 1;
#endif
    uint8_t opaScale_;
    int16_t index_;
    const char* id_;
    UIView* parent_;
    UIView* nextSibling_;
    Style* style_;
    TransformMap* transMap_;
    OnClickListener* onClickListener_;
    OnLongPressListener* onLongPressListener_;
    OnDragListener* onDragListener_;
    OnTouchListener* onTouchListener_;
#if ENABLE_FOCUS_MANAGER
    OnFocusListener* onFocusListener_;
#endif
#if ENABLE_ROTATE_INPUT
    OnRotateListener* onRotateListener_;
#endif
    ViewExtraMsg* viewExtraMsg_;

    uint8_t GetMixOpaScale() const;
    bool IsInvalid(float percent);
    void DrawViewBounds(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);
    void UpdateRectInfo(uint8_t key, const Rect& rect);

private:
    Rect rect_;
    Rect* visibleRect_;
    void SetupThemeStyles();
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_VIEW_H
