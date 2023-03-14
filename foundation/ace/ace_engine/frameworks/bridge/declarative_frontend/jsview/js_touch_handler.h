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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TOUCH_HANDLER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TOUCH_HANDLER_H

#include "core/components/touch_listener/touch_listener_component.h"
#include "core/event/ace_event_handler.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_touch_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSTouchHandler {
private:
    enum TouchEvent { DOWN, UP, MOVE, CANCEL };

private:
    RefPtr<OHOS::Ace::Component> component_;
    RefPtr<JsTouchFunction> jsOnDownFunc_;
    RefPtr<JsTouchFunction> jsOnUpFunc_;
    RefPtr<JsTouchFunction> jsOnMoveFunc_;
    RefPtr<JsTouchFunction> jsOnCancelFunc_;

public:
    JSTouchHandler() = default;
    virtual ~JSTouchHandler()
    {
        LOGD("Destroy: JSTouchHandler");
    }

    virtual RefPtr<OHOS::Ace::SingleChild> CreateComponent(const JSCallbackInfo& args);

    static void JSBind(BindingTarget globalObj);

    void JsHandlerOnTouch(TouchEvent action, const JSCallbackInfo& args);
    void JsHandlerOnDown(const JSCallbackInfo& args);
    void JsHandlerOnUp(const JSCallbackInfo& args);
    void JsHandlerOnMove(const JSCallbackInfo& args);
    void JsHandlerOnCancel(const JSCallbackInfo& args);
}; // JSTouchHandler
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_TOUCH_HANDLER_H
