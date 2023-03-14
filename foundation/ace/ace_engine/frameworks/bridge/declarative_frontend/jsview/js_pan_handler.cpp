/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/jsview/js_pan_handler.h"

#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

RefPtr<OHOS::Ace::SingleChild> JSPanHandler::CreateComponent(const JSCallbackInfo& args)
{
    LOGD("JSPanHandler wrapComponent");
    auto gestureComponent = ViewStackProcessor::GetInstance()->GetPanGestureListenerComponent();

    if (jsOnStartFunc_) {
        auto dragStartId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnStartFunc_)](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                const DragStartInfo* dragStartInfo = static_cast<const DragStartInfo*>(info);

                if (!dragStartInfo) {
                    LOGE("Error processing event. Not an instance of DragStartInfo");
                    return;
                }
                ACE_SCORING_EVENT("PanHandler.onDragStart");
                func->Execute(*dragStartInfo);
            },
            "dragStart", 0);
        gestureComponent->SetOnVerticalDragStartId(dragStartId);
    }

    if (jsOnUpdateFunc_) {
        auto dragUpdateId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnUpdateFunc_)](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                const DragUpdateInfo* dragUpdateInfo = static_cast<const DragUpdateInfo*>(info);

                if (!dragUpdateInfo) {
                    LOGE("Error processing event. Not an instance of DragUpdateInfo");
                    return;
                }
                ACE_SCORING_EVENT("PanHandler.onDragUpdate");
                func->Execute(*dragUpdateInfo);
            },
            "dragUpdate", 0);
        gestureComponent->SetOnVerticalDragUpdateId(dragUpdateId);
    }

    if (jsOnEndFunc_) {
        auto dragEndId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnEndFunc_)](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                const DragEndInfo* dragEndInfo = static_cast<const DragEndInfo*>(info);

                if (!dragEndInfo) {
                    LOGE("Error processing event. Not an instance of DragEndInfo");
                    return;
                }
                ACE_SCORING_EVENT("PanHandler.onDragEnd");
                func->Execute(*dragEndInfo);
            },
            "dragEnd", 0);
        gestureComponent->SetOnVerticalDragEndId(dragEndId);
    }

    if (jsOnCancelFunc_) {
        auto dragCancelId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnCancelFunc_)](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("PanHandler.onDragCancel");
                func->Execute();
            }, "dragCancel", 0);
        gestureComponent->SetOnVerticalDragCancelId(dragCancelId);
    }

    return gestureComponent;
}

void JSPanHandler::JsHandlerOnPan(PanEvent action, const JSCallbackInfo& args)
{
    LOGD("JSPanHandler JsHandlerOnPan");
    if (args[0]->IsFunction()) {
        JSRef<JSFunc> jsFunction = JSRef<JSFunc>::Cast(args[0]);
        RefPtr<JsPanFunction> handlerFunc = AceType::MakeRefPtr<JsPanFunction>(jsFunction);
        switch (action) {
            case PanEvent::START:
                jsOnStartFunc_ = handlerFunc;
                break;
            case PanEvent::UPDATE:
                jsOnUpdateFunc_ = handlerFunc;
                break;
            case PanEvent::END:
                jsOnEndFunc_ = handlerFunc;
                break;
            case PanEvent::CANCEL:
                jsOnCancelFunc_ = handlerFunc;
                break;
            default:
                break;
        }
    }
    args.ReturnSelf();
}

void JSPanHandler::JsHandlerOnStart(const JSCallbackInfo& args)
{
    JSPanHandler::JsHandlerOnPan(PanEvent::START, args);
}

void JSPanHandler::JsHandlerOnUpdate(const JSCallbackInfo& args)
{
    JSPanHandler::JsHandlerOnPan(PanEvent::UPDATE, args);
}

void JSPanHandler::JsHandlerOnEnd(const JSCallbackInfo& args)
{
    JSPanHandler::JsHandlerOnPan(PanEvent::END, args);
}

void JSPanHandler::JsHandlerOnCancel(const JSCallbackInfo& args)
{
    JSPanHandler::JsHandlerOnPan(PanEvent::CANCEL, args);
}

void JSPanHandler::JSBind(BindingTarget globalObj)
{
    JSClass<JSPanHandler>::Declare("PanHandler");
    JSClass<JSPanHandler>::CustomMethod("onStart", &JSPanHandler::JsHandlerOnStart);
    JSClass<JSPanHandler>::CustomMethod("onUpdate", &JSPanHandler::JsHandlerOnUpdate);
    JSClass<JSPanHandler>::CustomMethod("onEnd", &JSPanHandler::JsHandlerOnEnd);
    JSClass<JSPanHandler>::CustomMethod("onCancel", &JSPanHandler::JsHandlerOnCancel);
    JSClass<JSPanHandler>::Bind<>(globalObj);
}
}; // namespace OHOS::Ace::Framework
