/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/jsview/js_richtext.h"

#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/web/web_component.h"

namespace OHOS::Ace::Framework {
void JSRichText::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGI("richtext create error, info is non-valid");
        return;
    }

    std::string data = "";
    RefPtr<WebComponent> webComponent;
    if (ParseJsString(info[0], data)) {
        webComponent = AceType::MakeRefPtr<OHOS::Ace::WebComponent>("");
        webComponent->SetData(data);
    } else {
        LOGE("richtext component failed to parse data");
        return;
    }
    ViewStackProcessor::GetInstance()->Push(webComponent);
}

void JSRichText::JSBind(BindingTarget globalObj)
{
    JSClass<JSRichText>::Declare("RichText");
    JSClass<JSRichText>::StaticMethod("create", &JSRichText::Create);
    JSClass<JSRichText>::StaticMethod("onStart", &JSRichText::OnStart);
    JSClass<JSRichText>::StaticMethod("onComplete", &JSRichText::OnComplete);
    JSClass<JSRichText>::Inherit<JSViewAbstract>();
    JSClass<JSRichText>::Bind<>(globalObj);
}

void JSRichText::OnStart(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto eventMarker = EventMarker([execCtx = info.GetExecutionContext(),
            func = std::move(jsFunc)](const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute();
        });
        auto webComponent = AceType::DynamicCast<WebComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        webComponent->SetPageStartedEventId(eventMarker);
    }
}

void JSRichText::OnComplete(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto eventMarker = EventMarker([execCtx = info.GetExecutionContext(),
            func = std::move(jsFunc)](const BaseEventInfo* info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            func->Execute();
        });
        auto webComponent = AceType::DynamicCast<WebComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        webComponent->SetPageFinishedEventId(eventMarker);
    }
}
} // namespace OHOS::Ace::Framework
