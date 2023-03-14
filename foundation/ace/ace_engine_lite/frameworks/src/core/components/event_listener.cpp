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
#include "event_listener.h"
#include "ace_log.h"
#include "jerryscript.h"
#include "js_fwk_common.h"
#include "root_view.h"

namespace OHOS {
namespace ACELite {
#ifdef JS_EXTRA_EVENT_SUPPORT
KeyBoardEventListener::KeyBoardEventListener(jerry_value_t fn, const uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

KeyBoardEventListener::~KeyBoardEventListener()
{
    jerry_release_value(fn_);
}

bool KeyBoardEventListener::OnKeyAct(UIView &view, const KeyEvent &event)
{
    if (jerry_value_is_undefined(fn_)) {
        return false;
    }

    jerry_value_t *args = ConvertKeyEventInfo(event);
    jerry_release_value(CallJSFunctionOnRoot(fn_, args, 1));
    ClearEventListener(args, 1);
    return true;
}

ViewOnTouchCancelListener::ViewOnTouchCancelListener(jerry_value_t fn, uint16_t id)
{
    fn_ = jerry_acquire_value(fn);
    id_ = id;
}

ViewOnTouchCancelListener::~ViewOnTouchCancelListener()
{
    jerry_release_value(fn_);
}

bool ViewOnTouchCancelListener::OnCancel(UIView &view, const CancelEvent &event)
{
    return CallBaseEvent(fn_, event, id_);
}
#endif // JS_EXTRA_EVENT_SUPPORT

void ViewOnTouchListener::SetBindTouchStartFuncName(jerry_value_t bindTouchStartFunc)
{
    if (!jerry_value_is_undefined(bindTouchStartFunc)) {
        bindTouchStartFunc_ = jerry_acquire_value(bindTouchStartFunc);
    }
}

void ViewOnTouchListener::SetBindTouchMoveFuncName(jerry_value_t bindTouchMoveFunc)
{
    if (!jerry_value_is_undefined(bindTouchMoveFunc)) {
        bindTouchMoveFunc_ = jerry_acquire_value(bindTouchMoveFunc);
    }
}


void ViewOnTouchListener::SetBindTouchEndFuncName(jerry_value_t bindTouchEndFunc)
{
    if (!jerry_value_is_undefined(bindTouchEndFunc)) {
        bindTouchEndFunc_ = jerry_acquire_value(bindTouchEndFunc);
    }
}

void ViewOnTouchListener::SetBindSwipeFuncName(jerry_value_t bindSwipeFunc)
{
    if (!jerry_value_is_undefined(bindSwipeFunc)) {
        bindSwipeFunc_ = jerry_acquire_value(bindSwipeFunc);
    }
}

void ViewOnTouchListener::SetStopPropagation(bool isStopPropogation)
{
    isStopPropagation_ = isStopPropogation;
}

bool ViewOnTouchListener::OnDragStart(UIView& view, const DragEvent &event)
{
    if (JSUndefined::Is(bindTouchStartFunc_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnDragStart received, but no JS function to call");
        return isStopPropagation_;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDragStart received");

    JSValue arg = EventUtil::CreateTouchEvent(view, event);
    EventUtil::InvokeCallback(vm_, bindTouchStartFunc_, arg, this);
    return isStopPropagation_;
}

bool ViewOnTouchListener::OnDrag(UIView& view, const DragEvent& event)
{
    if (JSUndefined::Is(bindTouchMoveFunc_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "OnDrag received, but no JS function to call");
        return isStopPropagation_;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDrag received");

    JSValue arg = EventUtil::CreateTouchEvent(view, event);
    EventUtil::InvokeCallback(vm_, bindTouchMoveFunc_, arg, this);
    return isStopPropagation_;
}

bool ViewOnTouchListener::OnDragEnd(UIView& view, const DragEvent &event)
{
    if (!JSUndefined::Is(bindSwipeFunc_)) {
        JSValue argSwipe = EventUtil::CreateSwipeEvent(view, event);
        EventUtil::InvokeCallback(vm_, bindSwipeFunc_, argSwipe, this);
    }

    if (!JSUndefined::Is(bindTouchEndFunc_)) {
        JSValue argDragEnd = EventUtil::CreateTouchEvent(view, event);
        EventUtil::InvokeCallback(vm_, bindTouchEndFunc_, argDragEnd, this);
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "OnDragEnd received");
    HILOG_DEBUG(HILOG_MODULE_ACE, "Swipe received");
    return isStopPropagation_;
}
} // namespace ACELite
} // namespace OHOS
