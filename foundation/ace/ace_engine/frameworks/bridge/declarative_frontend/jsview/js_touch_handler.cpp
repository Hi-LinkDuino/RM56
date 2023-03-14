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

#include "frameworks/bridge/declarative_frontend/jsview/js_touch_handler.h"

#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

RefPtr<OHOS::Ace::SingleChild> JSTouchHandler::CreateComponent(const JSCallbackInfo& args)
{
    LOGD("JSTouchHandler wrapComponent");
        auto touchComponent = ViewStackProcessor::GetInstance()->GetTouchListenerComponent();

    if (jsOnDownFunc_) {
        auto touchDownId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnDownFunc_)](BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                TouchEventInfo* touchInfo = static_cast<TouchEventInfo*>(info);
                if (!touchInfo) {
                    LOGE("Error processing event. Not an instance of TouchEventInfo");
                    return;
                }
                func->Execute(*touchInfo);
            },
            "touchDown", 0);
        touchComponent->SetOnTouchDownId(touchDownId);
    }

    if (jsOnUpFunc_) {
        auto touchUpId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnUpFunc_)](BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                TouchEventInfo* touchInfo = static_cast<TouchEventInfo*>(info);
                if (!touchInfo) {
                    LOGE("Error processing event. Not an instance of TouchEventInfo");
                    return;
                }
                func->Execute(*touchInfo);
            },
            "touchUp", 0);
        touchComponent->SetOnTouchUpId(touchUpId);
    }

    if (jsOnMoveFunc_) {
        auto touchMoveId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnMoveFunc_)](BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                TouchEventInfo* touchInfo = static_cast<TouchEventInfo*>(info);
                if (!touchInfo) {
                    LOGE("Error processing event. Not an instance of TouchEventInfo");
                    return;
                }
                func->Execute(*touchInfo);
            },
            "touchMove", 0);
        touchComponent->SetOnTouchMoveId(touchMoveId);
    }

    if (jsOnCancelFunc_) {
        auto touchCancelId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnCancelFunc_)](BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                TouchEventInfo* touchInfo = static_cast<TouchEventInfo*>(info);
                if (!touchInfo) {
                    LOGE("Error processing event. Not an instance of TouchEventInfo");
                    return;
                }
                func->Execute(*touchInfo);
            },
            "touchCancel", 0);
        touchComponent->SetOnTouchCancel(touchCancelId);
    }

    return touchComponent;
}

void JSTouchHandler::JsHandlerOnTouch(TouchEvent action, const JSCallbackInfo& args)
{
    LOGD("JSTouchHandler JsHandlerOnTouch");
    if (args[0]->IsFunction()) {
        JSRef<JSFunc> jsFunction = JSRef<JSFunc>::Cast(args[0]);
        RefPtr<JsTouchFunction> handlerFunc = AceType::MakeRefPtr<JsTouchFunction>(jsFunction);
        switch (action) {
            case TouchEvent::DOWN:
                jsOnDownFunc_ = handlerFunc;
                break;
            case TouchEvent::UP:
                jsOnUpFunc_ = handlerFunc;
                break;
            case TouchEvent::MOVE:
                jsOnMoveFunc_ = handlerFunc;
                break;
            case TouchEvent::CANCEL:
                jsOnCancelFunc_ = handlerFunc;
                break;
            default:
                break;
        }
    }
    args.ReturnSelf();
}

void JSTouchHandler::JsHandlerOnDown(const JSCallbackInfo& args)
{
    JSTouchHandler::JsHandlerOnTouch(TouchEvent::DOWN, args);
}

void JSTouchHandler::JsHandlerOnUp(const JSCallbackInfo& args)
{
    JSTouchHandler::JsHandlerOnTouch(TouchEvent::UP, args);
}

void JSTouchHandler::JsHandlerOnMove(const JSCallbackInfo& args)
{
    JSTouchHandler::JsHandlerOnTouch(TouchEvent::MOVE, args);
}

void JSTouchHandler::JsHandlerOnCancel(const JSCallbackInfo& args)
{
    JSTouchHandler::JsHandlerOnTouch(TouchEvent::CANCEL, args);
}

void JSTouchHandler::JSBind(BindingTarget globalObj)
{
    JSClass<JSTouchHandler>::Declare("TouchHandler");
    JSClass<JSTouchHandler>::CustomMethod("onDown", &JSTouchHandler::JsHandlerOnDown);
    JSClass<JSTouchHandler>::CustomMethod("onUp", &JSTouchHandler::JsHandlerOnUp);
    JSClass<JSTouchHandler>::CustomMethod("onMove", &JSTouchHandler::JsHandlerOnMove);
    JSClass<JSTouchHandler>::CustomMethod("onCancel", &JSTouchHandler::JsHandlerOnCancel);
    JSClass<JSTouchHandler>::Bind<>(globalObj);
}
}; // namespace OHOS::Ace::Framework
