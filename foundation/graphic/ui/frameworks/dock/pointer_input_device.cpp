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

#include "dock/pointer_input_device.h"

#include "components/root_view.h"
#include "core/render_manager.h"
#if ENABLE_AOD
#include "events/aod_callback.h"
#endif
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/graphic_math.h"
#if ENABLE_WINDOW
#include "window/window.h"
#endif

namespace OHOS {
void PointerInputDevice::DispatchEvent(const DeviceData& data)
{
    curPos_ = data.point;
#if ENABLE_WINDOW
    Window* window = RenderManager::GetInstance().GetWindowById(data.winId);
    if (window == nullptr) {
        return;
    }
    curPos_.x = curPos_.x - window->GetRect().GetLeft();
    curPos_.y = curPos_.y - window->GetRect().GetTop();
    UIViewGroup* rootView = window->GetRootView();
#else
    UIViewGroup* rootView = RootView::GetInstance();
#endif
    if (rootView == nullptr) {
        GRAPHIC_LOGE("No valid rootview to dispatch input event!\n");
        return;
    }
    // invalid touchable and draggable view will be reset to nullptr
    if ((touchableView_ != nullptr) && !RootView::FindSubView(*rootView, touchableView_)) {
        touchableView_ = nullptr;
        lastPos_ = curPos_;
    }
    if ((draggableView_ != nullptr) && !RootView::FindSubView(*rootView, draggableView_)) {
        draggableView_ = nullptr;
        lastPos_ = curPos_;
        dragLastPos_ = curPos_;
        dragLen_ = { 0, 0 };
        dragStep_ = { 0, 0 };
        isDragging_ = false;
    }

    if (data.state == STATE_PRESS) {
        DispatchPressEvent(rootView);
    } else {
        DispatchReleaseEvent(rootView);
    }
    dragLastPos_ = lastPos_;
    lastPos_ = curPos_;
}

void PointerInputDevice::DispatchPressEvent(UIViewGroup* rootView)
{
    // first time to press
    if (!pressState_) {
        rootView->GetTargetView(curPos_, &touchableView_, &targetView_);
        if (touchableView_ == nullptr) {
            GRAPHIC_LOGD("PointerInputDevice::DispatchPressEvent cannot find target view!\n");
            return;
        }
        draggableView_ = GetDraggableView(touchableView_);
        pressState_ = true;
        pressTimeStamp_ = HALTick::GetInstance().GetTime();
        lastPos_ = curPos_;
        dragLastPos_ = lastPos_;
        return;
    }
    uint32_t elapse = HALTick::GetInstance().GetElapseTime(pressTimeStamp_);
    DispatchDragStartEvent();
    DispatchDragEvent();
    if (!isDragging_ && (touchableView_ != nullptr) && !cancelSent_) {
        UIView* tempView = nullptr;
        rootView->GetTargetView(curPos_, &tempView, &targetView_);
        if (tempView != touchableView_) {
            DispatchCancelEvent();
        } else {
            if (!pressSent_ && (elapse > INDEV_PRESS_TIME_IN_DRAG)) {
                PressEvent evt(curPos_);
                UIView* parent = touchableView_->GetParent();
                if (!touchableView_->OnPressEvent(evt)) {
                    while (parent != nullptr) {
                        PressEvent evtParent(curPos_);
                        if (parent->OnPressEvent(evtParent)) {
                            break;
                        }
                        parent = parent->GetParent();
                    }
                }
                pressSent_ = true;
            }
            DispatchLongPressEvent(elapse);
        }
    }
}

bool PointerInputDevice::ProcessReleaseEvent()
{
    UIView* parent = touchableView_->GetParent();
    // reissue press event.
    if (!pressSent_) {
        PressEvent evtPress(curPos_);
        if (!touchableView_->OnPressEvent(evtPress)) {
            while (parent != nullptr) {
                PressEvent evtPressParent(curPos_);
                if (parent->OnPressEvent(evtPressParent)) {
                    break;
                }
                parent = parent->GetParent();
            }
        }
        pressSent_ = true;
        return false;
    } else {
        ReleaseEvent evtRelease(curPos_);
        if (!touchableView_->OnReleaseEvent(evtRelease)) {
            while (parent != nullptr) {
                ReleaseEvent evtReleaseParent(curPos_);
                if (parent->OnReleaseEvent(evtReleaseParent)) {
                    break;
                }
                parent = parent->GetParent();
            }
        }

        if (pressSent_ && needClick_) {
            ClickEvent evt(curPos_);
            parent = touchableView_->GetParent();
            if (!touchableView_->OnClickEvent(evt)) {
                while (parent != nullptr) {
#if ENABLE_AOD
                    OnClickEventHappen(parent);
#endif
                    ClickEvent evtParent(curPos_);
                    if (parent->OnClickEvent(evtParent)) {
                        break;
                    }
                    parent = parent->GetParent();
                }
            }
#if ENABLE_AOD
            OnClickEventHappen(*touchableView_);
#endif
        }
    }
    return true;
}

void PointerInputDevice::DispatchReleaseEvent(UIViewGroup* rootView)
{
    if (!pressState_) {
        return;
    }

    DispatchDragStartEvent();
    DispatchDragEndEvent();
    if (!isDragging_ && (touchableView_ != nullptr) && !cancelSent_) {
        UIView* tempView = nullptr;
        rootView->GetTargetView(curPos_, &tempView, &targetView_);
        if (tempView != touchableView_) {
            DispatchCancelEvent();
        } else {
            if (!ProcessReleaseEvent()) {
                return;
            }
        }
    }
    isDragging_ = false;
    pressState_ = false;
    pressSent_ = false;
    cancelSent_ = false;
    longPressSent_ = false;
    needClick_ = true;
    touchableView_ = nullptr;
}

void PointerInputDevice::DispatchDragStartEvent()
{
    if (draggableView_ == nullptr) {
        return;
    }
    dragStep_.x = curPos_.x - lastPos_.x;
    dragStep_.y = curPos_.y - lastPos_.y;
    dragLen_.x += dragStep_.x;
    dragLen_.y += dragStep_.y;
    if (!isDragging_) {
        if ((MATH_ABS(dragLen_.x) >= INDEV_DRAG_LIMIT) || (MATH_ABS(dragLen_.y) >= INDEV_DRAG_LIMIT)) {
            if ((touchableView_ != nullptr) && !cancelSent_) {
                DispatchCancelEvent();
            }
            // Send Drag Begin Event.
            DragEvent evt(curPos_, lastPos_, dragLen_);
            UIView* parent = draggableView_->GetParent();
            if (!draggableView_->OnDragStartEvent(evt)) {
                while (parent != nullptr) {
                    DragEvent evtParent(curPos_, lastPos_, dragLen_);
                    if (parent->OnDragStartEvent(evtParent)) {
                        break;
                    }
                    parent = parent->GetParent();
                }
            }
            dragLastPos_ = lastPos_;
            isDragging_ = true;
        }
    }
}

void PointerInputDevice::DispatchDragEvent()
{
    if ((draggableView_ == nullptr) || !isDragging_) {
        return;
    }
    if ((dragStep_.x != 0) || (dragStep_.y != 0)) {
        DragEvent evt(curPos_, lastPos_, dragLen_);
        UIView* parent = draggableView_->GetParent();
        if (!draggableView_->OnDragEvent(evt)) {
            while (parent != nullptr) {
                DragEvent evtParent(curPos_, lastPos_, dragLen_);
                if (parent->OnDragEvent(evtParent)) {
                    break;
                }
                parent = parent->GetParent();
            }
        }
    }
}

void PointerInputDevice::DispatchDragEndEvent()
{
    if (draggableView_ == nullptr) {
        return;
    }

    if (isDragging_) {
        DragEvent evt(curPos_, lastPos_, dragLen_);
        UIView* parent = draggableView_->GetParent();
        evt.SetPreLastPoint(dragLastPos_);
        if (!draggableView_->OnDragEndEvent(evt)) {
            while (parent != nullptr) {
                DragEvent evtParent(curPos_, lastPos_, dragLen_);
                if (parent->OnDragEndEvent(evtParent)) {
                    break;
                }
                parent = parent->GetParent();
            }
        }
#if ENABLE_AOD
        OnDragEndEventHappen(*draggableView_);
#endif
    }
    dragLen_ = { 0, 0 };
    dragStep_ = { 0, 0 };
    draggableView_ = nullptr;
}

void PointerInputDevice::DispatchLongPressEvent(uint32_t elapse)
{
    if (!longPressSent_ && (elapse > INDEV_LONG_PRESS_TIME)) {
        longPressSent_ = true;
        LongPressEvent evt(curPos_, pressTimeStamp_);
        bool isConsumed = touchableView_->OnLongPressEvent(evt);
        if (touchableView_->GetOnLongPressListener() != nullptr) {
            needClick_ = false;
        }
        UIView* parent = touchableView_->GetParent();
#if ENABLE_AOD
        OnLongPressEventHappen(*touchableView_);
#endif
        if (!isConsumed) {
            while (parent != nullptr) {
                LongPressEvent evtParent(curPos_, pressTimeStamp_);
                isConsumed = parent->OnLongPressEvent(evtParent);
                if (needClick_ && (parent->GetOnLongPressListener() != nullptr)) {
                    needClick_ = false;
                }
#if ENABLE_AOD
                OnLongPressEventHappen(*parent);
#endif
                if (isConsumed) {
                    break;
                }
                parent = parent->GetParent();
            }
        }
    }
}

void PointerInputDevice::DispatchCancelEvent()
{
    CancelEvent evt(lastPos_);
    UIView* parent = touchableView_->GetParent();
    if (!touchableView_->OnCancelEvent(evt)) {
        while (parent != nullptr) {
            CancelEvent evtParent(lastPos_);
            if (parent->OnCancelEvent(evtParent)) {
                break;
            }
            parent = parent->GetParent();
        }
    }
    cancelSent_ = true;
}
}  // namespace OHOS
