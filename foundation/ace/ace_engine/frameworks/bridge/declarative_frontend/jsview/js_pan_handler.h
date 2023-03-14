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

#ifndef FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JSVIEW_JS_PAN_HANDLER_H
#define FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JSVIEW_JS_PAN_HANDLER_H

#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/event/ace_event_handler.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_pan_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSPanHandler {
private:
    enum class PanEvent { START, UPDATE, END, CANCEL };

private:
    RefPtr<OHOS::Ace::Component> component_;
    RefPtr<JsPanFunction> jsOnStartFunc_;
    RefPtr<JsPanFunction> jsOnUpdateFunc_;
    RefPtr<JsPanFunction> jsOnEndFunc_;
    RefPtr<JsPanFunction> jsOnCancelFunc_;

public:
    JSPanHandler() = default;
    virtual ~JSPanHandler()
    {
        LOGD("Destroy: JSPanHandler");
    }

    virtual RefPtr<OHOS::Ace::SingleChild> CreateComponent(const JSCallbackInfo& args);

    static void JSBind(BindingTarget globalObj);

    void JsHandlerOnPan(PanEvent action, const JSCallbackInfo& args);
    void JsHandlerOnStart(const JSCallbackInfo& args);
    void JsHandlerOnUpdate(const JSCallbackInfo& args);
    void JsHandlerOnEnd(const JSCallbackInfo& args);
    void JsHandlerOnCancel(const JSCallbackInfo& args);
}; // JSPanHandler

} // namespace OHOS::Ace::Framework
#endif // FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JSVIEW_JS_PAN_HANDLER_H
