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

#include "event_util.h"
#include "ace_log.h"
#include "async_task_manager.h"

namespace OHOS {
namespace ACELite {
constexpr char ATTR_TYPE[] = "type";
constexpr char ATTR_TARGET[] = "target";
constexpr char ATTR_CURRENT_TARGET[] = "currentTarget";
constexpr char ATTR_TIMESTAMP[] = "timestamp";
constexpr char ATTR_GLOBAL_X[] = "globalX";
constexpr char ATTR_GLOBAL_Y[] = "globalY";
constexpr char ATTR_DIRECTION[] = "direction";
constexpr char ATTR_DIRECTION_RIGHT[] = "right";
constexpr char ATTR_DIRECTION_LEFT[] = "left";
constexpr char ATTR_DIRECTION_UP[] = "up";
constexpr char ATTR_DIRECTION_DOWN[] = "down";
const char *EventUtil::EVENT_CLICK = "click";
const char *EventUtil::EVENT_DELETE = "delete";
const char *EventUtil::EVENT_LONGPRESS = "longpress";
const char *EventUtil::EVENT_SWIPE = "swipe";
const char *EventUtil::EVENT_TOUCH = "touch";

void CallbackExecutor(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to execute the callback function of event.");
        return;
    }

    auto *params = static_cast<CallbackParams *>(data);
    if (!JSFunction::Is(params->fn)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "failed to execute the callback function of event.");
        delete params;
        params = nullptr;
        return;
    }
    constexpr uint8_t argsLength = 1;
    JSValue args[argsLength] = {params->arg};
    JSRelease(JSFunction::Call(params->fn, params->vm, args, argsLength));
    JSRelease(params->arg);
    delete params;
    params = nullptr;
}

JSValue EventUtil::CreateEvent(const char *type, UIView &view, const Event &event)
{
    // create a JAVASCRIPT plain object that is used as the input parameter of
    // the callback function for click or longpress event.
    JSValue arg = JSObject::Create();
    // set the 'type' attribute value
    JSObject::SetString(arg, ATTR_TYPE, type);
    // set the 'target' attribute value
    UIView *target = nullptr;
    UIView *currentTarget = nullptr;
    Point point = event.GetCurrentPos();
    view.GetTargetView(point, &currentTarget, &target);
    JSObject::Set(arg, ATTR_TARGET, GetElementByUIView(target));
    // set the 'currentTarget' attribute value
    JSObject::Set(arg, ATTR_CURRENT_TARGET, GetElementByUIView(currentTarget));
    // set the 'timestamp' attribute value
    JSObject::SetNumber(arg, ATTR_TIMESTAMP, event.GetTimeStamp());
    // set the 'globalX' attribute
    JSObject::SetNumber(arg, ATTR_GLOBAL_X, point.x);
    // set the 'globalY' attribute
    JSObject::SetNumber(arg, ATTR_GLOBAL_Y, point.y);
    return arg;
}
JSValue EventUtil::CreateSwipeEvent(UIView &view, const DragEvent &event)
{
    // create a JAVASCRIPT plain object that is used as the input parameter of
    // the event callback function.
    JSValue arg = EventUtil::CreateEvent(EVENT_SWIPE, view, event);
    // set the 'direction' attribute for the input parameter of the swipe event callback function.
    switch (event.GetDragDirection()) {
        case DragEvent::DIRECTION_LEFT_TO_RIGHT: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_RIGHT);
            break;
        }
        case DragEvent::DIRECTION_RIGHT_TO_LEFT: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_LEFT);
            break;
        }
        case DragEvent::DIRECTION_TOP_TO_BOTTOM: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_DOWN);
            break;
        }
        case DragEvent::DIRECTION_BOTTOM_TO_TOP: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_UP);
            break;
        }
        default: {
            HILOG_ERROR(HILOG_MODULE_ACE, "failed to set direction attribute for event argument.");
            break;
        }
    }
    return arg;
}
JSValue EventUtil::CreateTouchEvent(UIView &view, const DragEvent &event)
{
    // create a JAVASCRIPT plain object that is used as the input parameter of
    // the event callback function.
    JSValue arg = EventUtil::CreateEvent(EVENT_TOUCH, view, event);
    switch (event.GetDragDirection()) {
        case DragEvent::DIRECTION_LEFT_TO_RIGHT: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_RIGHT);
            break;
        }
        case DragEvent::DIRECTION_RIGHT_TO_LEFT: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_LEFT);
            break;
        }
        case DragEvent::DIRECTION_TOP_TO_BOTTOM: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_DOWN);
            break;
        }
        case DragEvent::DIRECTION_BOTTOM_TO_TOP: {
            JSObject::SetString(arg, ATTR_DIRECTION, ATTR_DIRECTION_UP);
            break;
        }
        default: {
            HILOG_ERROR(HILOG_MODULE_ACE, "failed to set direction attribute for event argument.");
            break;
        }
    }
    return arg;
}
void EventUtil::InvokeCallback(JSValue vm, JSValue callback, JSValue event, const void *context)
{
    auto *params = new CallbackParams();
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to invoke event callback.");
        return;
    }
    params->vm = vm;
    params->fn = callback;
    params->arg = event;
    // The views may be destroyed or recreated in conditional or list rendering.
    // If we directly call the event callback function, the program will crash.
    if (DISPATCH_FAILURE ==
        AsyncTaskManager::GetInstance().Dispatch(CallbackExecutor, static_cast<void *>(params), context)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "EventUtil::InvokeCallback failed: Async task dispatch failure.");
        delete params;
        params = nullptr;
        JSRelease(event);
    }
}
JSValue EventUtil::GetElementByUIView(UIView *view)
{
    if (view == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to get element by UI view.");
        return JSUndefined::Create();
    }
    UIView::ViewExtraMsg *extraMsg = view->GetExtraMsg();
    if (extraMsg == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to get element by UI view.");
        return JSUndefined::Create();
    }
    JSValue *elementPtr = reinterpret_cast<JSValue *>(extraMsg->elementPtr);
    if (elementPtr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fail to get element by UI view.");
        return JSUndefined::Create();
    }
    return *elementPtr;
}
} // namespace ACELite
} // namespace OHOS
