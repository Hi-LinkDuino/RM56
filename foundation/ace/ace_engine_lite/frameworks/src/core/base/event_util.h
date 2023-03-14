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

#ifndef OHOS_ACELITE_EVENT_UTIL_H
#define OHOS_ACELITE_EVENT_UTIL_H

#include "non_copyable.h"
#include "ui_view.h"
#include "wrapper/js.h"

namespace OHOS {
namespace ACELite {
struct CallbackParams : public MemoryHeap {
    JSValue vm;
    JSValue fn;
    JSValue arg;
};
class EventUtil final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(EventUtil);

    /**
     * @brief Create a JAVASCRIPT plain object that is used as the input parameter of
     * the callback function for click or longpress event.
     */
    static JSValue CreateEvent(const char *type, UIView &view, const Event &event);

    /**
     * @brief Create a JAVASCRIPT plain object that is used as the input parameter of
     * the callback function for swipe event.
     */
    static JSValue CreateSwipeEvent(UIView &view, const DragEvent &event);

    /**
     * @brief Create a JAVASCRIPT plain object that is used as the input parameter of
     * the callback function for touch event.
     */
    static JSValue CreateTouchEvent(UIView &view, const DragEvent &event);

    /**
     * @brief Invoke the callback function of event.
     */
    static void InvokeCallback(JSValue vm, JSValue callback, JSValue event, const void *context = nullptr);

    static const char *EVENT_CLICK;
    static const char *EVENT_DELETE;
    static const char *EVENT_LONGPRESS;
    static const char *EVENT_SWIPE;
    static const char *EVENT_TOUCH;

private:
    EventUtil() {}
    ~EventUtil() {}

    /**
     * @brief Get the DOM element that is reference to view
     */
    static JSValue GetElementByUIView(UIView *view);
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_EVENT_UTIL_H
