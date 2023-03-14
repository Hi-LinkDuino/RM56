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

#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"

#include "base/log/log_wrapper.h"
#include "core/common/container.h"
#ifdef PLUGIN_COMPONENT_SUPPORTED
#include "core/common/plugin_manager.h"
#endif
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/pipeline/base/single_child.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_hover_function.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_key_function.h"
#include "frameworks/bridge/declarative_frontend/engine/js_execution_scope_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_pan_handler.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_touch_handler.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSInteractableView::JsOnTouch(const JSCallbackInfo& args)
{
    LOGD("JSInteractableView JsOnTouch");
    if (args[0]->IsFunction()) {
        auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
        if (!inspector) {
            LOGE("fail to get inspector for on touch event");
            return;
        }
        auto impl = inspector->GetInspectorFunctionImpl();
        RefPtr<JsTouchFunction> jsOnTouchFunc = AceType::MakeRefPtr<JsTouchFunction>(JSRef<JSFunc>::Cast(args[0]));
        auto onTouchId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnTouchFunc), impl](BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                if (impl) {
                    impl->UpdateEventInfo(*info);
                }
                auto touchInfo = TypeInfoHelper::DynamicCast<TouchEventInfo>(info);
                ACE_SCORING_EVENT("onTouch");
                func->Execute(*touchInfo);
            },
            "onTouch");
        auto touchComponent = ViewStackProcessor::GetInstance()->GetTouchListenerComponent();
        touchComponent->SetOnTouchId(onTouchId);
    }
}

void JSInteractableView::JsOnKey(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        RefPtr<JsKeyFunction> jsOnKeyFunc = AceType::MakeRefPtr<JsKeyFunction>(JSRef<JSFunc>::Cast(args[0]));
        auto onKeyId = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(jsOnKeyFunc)](BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                auto keyInfo = TypeInfoHelper::DynamicCast<KeyEventInfo>(info);
                ACE_SCORING_EVENT("onKey");
                func->Execute(*keyInfo);
            },
            "onKey", 0);
        auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
        focusableComponent->SetFocusable(true);
        focusableComponent->SetOnKeyId(onKeyId);
    }
}

void JSInteractableView::JsOnHover(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        RefPtr<JsHoverFunction> jsOnHoverFunc = AceType::MakeRefPtr<JsHoverFunction>(JSRef<JSFunc>::Cast(args[0]));
        auto onHoverId = [execCtx = args.GetExecutionContext(), func = std::move(jsOnHoverFunc)](bool info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("onHover");
            func->Execute(info);
        };
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        box->SetOnHoverId(onHoverId);
    }
}

void JSInteractableView::JsOnPan(const JSCallbackInfo& args)
{
    LOGD("JSInteractableView JsOnPan");
    if (args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSPanHandler* handler = obj->Unwrap<JSPanHandler>();
        if (handler) {
            handler->CreateComponent(args);
        }
    }
}

void JSInteractableView::JsOnDelete(const JSCallbackInfo& info)
{
    LOGD("JSInteractableView JsOnDelete");
    if (info[0]->IsFunction()) {
        RefPtr<JsFunction> jsOnDeleteFunc =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto onDeleteId = EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsOnDeleteFunc)]() {
            LOGD("onDelete callback");
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("onDelete");
            func->Execute();
        });
        auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
        focusableComponent->SetOnDeleteId(onDeleteId);
    }
}

void JSInteractableView::JsTouchable(const JSCallbackInfo& info)
{
    if (info[0]->IsBoolean()) {
        auto mainComponent = ViewStackProcessor::GetInstance()->GetMainComponent();
        if (!mainComponent) {
            return;
        }
        mainComponent->SetTouchable(info[0]->ToBoolean());
    }
}

void JSInteractableView::JsOnClick(const JSCallbackInfo& info)
{
    if (info[0]->IsFunction()) {
        auto click = ViewStackProcessor::GetInstance()->GetBoxComponent();
        auto tapGesture = GetTapGesture(info);
        if (tapGesture) {
            click->SetOnClick(tapGesture);
        }

        auto onClickId = GetClickEventMarker(info);
        auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
        focusableComponent->SetOnClickId(onClickId);
    }
}

EventMarker JSInteractableView::GetClickEventMarker(const JSCallbackInfo& info)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("fail to get inspector for on get click event marker");
        return EventMarker();
    }
    auto impl = inspector->GetInspectorFunctionImpl();
    RefPtr<JsClickFunction> jsOnClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onClickId = EventMarker(
        [execCtx = info.GetExecutionContext(), func = std::move(jsOnClickFunc), impl](const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            auto clickInfo = TypeInfoHelper::DynamicCast<ClickInfo>(info);
            auto newInfo = *clickInfo;
            if (impl) {
                impl->UpdateEventInfo(newInfo);
            }
            ACE_SCORING_EVENT("onClick");
            func->Execute(newInfo);
        });
    return onClickId;
}

RefPtr<Gesture> JSInteractableView::GetTapGesture(
    const JSCallbackInfo& info, int32_t countNum, int32_t fingerNum)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("fail to get inspector for on touch event");
        return nullptr;
    }
    auto impl = inspector->GetInspectorFunctionImpl();
    RefPtr<Gesture> tapGesture = AceType::MakeRefPtr<TapGesture>(countNum, fingerNum);
    RefPtr<JsClickFunction> jsOnClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(info[0]));
    tapGesture->SetOnActionId(
        [execCtx = info.GetExecutionContext(), func = std::move(jsOnClickFunc), impl](GestureEvent& info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            if (impl) {
                impl->UpdateEventInfo(info);
            }
            ACE_SCORING_EVENT("onClick");
            func->Execute(info);
        });
    return tapGesture;
}

RefPtr<Gesture> JSInteractableView::GetRemoteMessageTapGesture(const JSCallbackInfo& info)
{
    RefPtr<Gesture> tapGesture = AceType::MakeRefPtr<TapGesture>();
    RefPtr<JsClickFunction> jsOnClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto eventCallback = GetRemoteMessageEventCallback(info);
    tapGesture->SetOnActionId([func = std::move(eventCallback)]
        (const GestureEvent& info) {
        func();
    });
    return tapGesture;
}

void JSInteractableView::SetFocusable(bool focusable)
{
    auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
    if (focusableComponent) {
        focusableComponent->SetFocusable(focusable);
    }
}
void JSInteractableView::SetFocusNode(bool isFocusNode)
{
    auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent(false);
    if (focusableComponent) {
        focusableComponent->SetFocusNode(!isFocusNode);
    }
}

void JSInteractableView::JsOnAppear(const JSCallbackInfo& info)
{
    if (info[0]->IsFunction()) {
        RefPtr<JsFunction> jsOnAppearFunc =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));

        auto onAppearId = EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsOnAppearFunc)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            LOGI("About to call JsOnAppear method on js");
            ACE_SCORING_EVENT("onAppear");
            func->Execute();
        });
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        component->SetOnAppearEventId(onAppearId);
    }
}

void JSInteractableView::JsOnDisAppear(const JSCallbackInfo& info)
{
    if (info[0]->IsFunction()) {
        RefPtr<JsFunction> jsOnDisAppearFunc =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto onDisAppearId = EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsOnDisAppearFunc)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            LOGD("Start to call JsOnDisAppear method on js");
            ACE_SCORING_EVENT("onDisAppear");
            func->Execute();
        });
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        component->SetOnDisappearEventId(onDisAppearId);
    }
}

void JSInteractableView::JsOnAccessibility(const JSCallbackInfo& info)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("this component does not have inspector");
        return;
    }
    inspector->SetAccessibilityEvent(GetEventMarker(info, { "eventType" }));
}

void JSInteractableView::UpdateEventTarget(NodeId id, BaseEventInfo& info)
{
    auto container = Container::Current();
    if (!container) {
        LOGE("fail to get container");
        return;
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        LOGE("fail to get context");
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGE("fail to get accessibility manager");
        return;
    }
    accessibilityManager->UpdateEventTarget(id, info);
}

EventMarker JSInteractableView::GetEventMarker(const JSCallbackInfo& info, const std::vector<std::string>& keys)
{
    if (!info[0]->IsFunction()) {
        LOGE("info[0] is not a function.");
        return EventMarker();
    }

    RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
    auto eventMarker =
        EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc), keys](const std::string& param) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("onClick");
            func->Execute(keys, param);
        });
    return eventMarker;
}

void JSInteractableView::JsCommonRemoteMessage(const JSCallbackInfo& info)
{
    if (info.Length() != 0 && info[0]->IsObject()) {
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        EventMarker remoteMessageEventId;
        JsRemoteMessage(info, remoteMessageEventId);
        box->SetRemoteMessageEvent(remoteMessageEventId);
    }
}

void JSInteractableView::JsRemoteMessage(const JSCallbackInfo& info, EventMarker& eventMarker)
{
    if (info.Length() == 0 || !info[0]->IsObject()) {
        LOGE("RemoteMessage JSCallbackInfo param is empty or type is not Object.");
        return;
    }

    auto eventCallback = GetRemoteMessageEventCallback(info);
    eventMarker = EventMarker(
        [func = std::move(eventCallback)](BaseEventInfo* info) {
            auto touchInfo = TypeInfoHelper::DynamicCast<ClickInfo>(info);
            if (touchInfo && touchInfo->GetType().compare("onClick") == 0) {
                func();
            }
        });
}

std::function<void()> JSInteractableView::GetRemoteMessageEventCallback(const JSCallbackInfo& info)
{
    auto obj = JSRef<JSObject>::Cast(info[0]);
    auto actionValue = obj->GetProperty("action");
    std::string action;
    if (actionValue->IsString()) {
        action = actionValue->ToString();
    }
    auto abilityValue = obj->GetProperty("ability");
    std::string ability;
    if (abilityValue->IsString()) {
        ability = abilityValue->ToString();
    }
    auto paramsObj = obj->GetProperty("params");
    std::string params;
    if (paramsObj->IsObject()) {
        params = paramsObj->ToString();
    }
    auto eventCallback = [action, ability, params]() {
        LOGE("JSInteractableView::JsRemoteMessage. eventMarker");
        if (action.compare("message") == 0) {
            // onCall
        } else if (action.compare("route") == 0) {
            // startAbility
#ifdef PLUGIN_COMPONENT_SUPPORTED
            std::vector<std::string> strList;
            SplitString(ability, '/', strList);
            if (strList.size() <= 1) {
                LOGE("App bundleName or abilityName is empty.");
                return;
            }
            int32_t result = PluginManager::GetInstance().StartAbility(strList[0], strList[1], params);
            if (result != 0) {
                LOGE("JSInteractableView::JsRemoteMessage: Failed to start the APP %{public}s.", ability.c_str());
            }
#else
            LOGE("JSInteractableView::JsRemoteMessage: Unsupported Windows and Mac platforms to start APP.");
#endif
        } else {
            LOGE("action's name is not message or route.");
        }
    };

    return eventCallback;
}

void JSInteractableView::SplitString(const std::string& str, char tag, std::vector<std::string>& strList)
{
    std::string subStr;
    for (size_t i = 0; i < str.length(); i++) {
        if (tag == str[i]) {
            if (!subStr.empty()) {
                strList.push_back(subStr);
                subStr.clear();
            }
        } else {
            subStr.push_back(str[i]);
        }
    }
    if (!subStr.empty()) {
        strList.push_back(subStr);
    }
}
} // namespace OHOS::Ace::Framework
