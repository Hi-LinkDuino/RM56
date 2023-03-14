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

#include "frameworks/bridge/declarative_frontend/jsview/js_span.h"

#include <sstream>
#include <string>
#include <vector>

#include "base/geometry/dimension.h"
#include "base/log/ace_trace.h"
#include "core/components/declaration/span/span_declaration.h"
#include "core/event/ace_event_handler.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<TextCase> TEXT_CASES = { TextCase::NORMAL, TextCase::LOWERCASE, TextCase::UPPERCASE };

} // namespace

void JSSpan::SetFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetFontSize(fontSize);
    component->SetTextStyle(std::move(textStyle));

    auto declaration = component->GetDeclaration();
    if (declaration) {
        declaration->SetHasSetFontSize(true);
    }
}

void JSSpan::SetFontWeight(const std::string& value)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetFontWeight(ConvertStrToFontWeight(value));
    component->SetTextStyle(std::move(textStyle));

    auto declaration = component->GetDeclaration();
    if (declaration) {
        declaration->SetHasSetFontWeight(true);
    }
}

void JSSpan::SetTextColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color textColor;
    if (!ParseJsColor(info[0], textColor)) {
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetTextColor(textColor);
    component->SetTextStyle(std::move(textStyle));

    auto declaration = component->GetDeclaration();
    if (declaration) {
        declaration->SetHasSetFontColor(true);
    }
}

void JSSpan::SetFontStyle(int32_t value)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    if (value >= 0 && value < static_cast<int32_t>(FONT_STYLES.size())) {
        auto textStyle = component->GetTextStyle();
        textStyle.SetFontStyle(FONT_STYLES[value]);
        component->SetTextStyle(std::move(textStyle));

        auto declaration = component->GetDeclaration();
        if (declaration) {
            declaration->SetHasSetFontStyle(true);
        }
    } else {
        LOGE("Text fontStyle(%d) illegal value", value);
    }
}

void JSSpan::SetFontFamily(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::vector<std::string> fontFamilies;
    if (!ParseJsFontFamilies(info[0], fontFamilies)) {
        LOGE("Parse FontFamilies failed");
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetFontFamilies(fontFamilies);
    component->SetTextStyle(std::move(textStyle));

    auto declaration = component->GetDeclaration();
    if (declaration) {
        declaration->SetHasSetFontFamily(true);
    }
}

void JSSpan::SetLetterSpacing(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionFp(info[0], value)) {
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    auto textStyle = component->GetTextStyle();
    textStyle.SetLetterSpacing(value);
    component->SetTextStyle(std::move(textStyle));

    auto declaration = component->GetDeclaration();
    if (declaration) {
        declaration->SetHasSetLetterSpacing(true);
    }
}

void JSSpan::SetTextCase(int32_t value)
{
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }

    if (value >= 0 && value < static_cast<int32_t>(TEXT_CASES.size())) {
        auto textStyle = component->GetTextStyle();
        textStyle.SetTextCase(TEXT_CASES[value]);
        component->SetTextStyle(std::move(textStyle));

        auto declaration = component->GetDeclaration();
        if (declaration) {
            declaration->SetHasSetTextCase(true);
        }
    } else {
        LOGE("Text textCase(%d) illegal value", value);
    }
}

void JSSpan::SetDecoration(const JSCallbackInfo& info)
{
    if (info[0]->IsObject()) {
        auto component = GetComponent();
        if (component) {
            auto textStyle = component->GetTextStyle();

            JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
            JSRef<JSVal> typeValue = obj->GetProperty("type");
            JSRef<JSVal> colorValue = obj->GetProperty("color");

            if (typeValue->IsNumber()) {
                textStyle.SetTextDecoration(TextDecoration(typeValue->ToNumber<int32_t>()));
            }
            Color colorVal;
            if (ParseJsColor(colorValue, colorVal)) {
                textStyle.SetTextDecorationColor(colorVal);
            }
            component->SetTextStyle(std::move(textStyle));

            auto declaration = component->GetDeclaration();
            if (declaration) {
                declaration->SetHasSetTextDecoration(true);
            }
        }
    }
    info.SetReturnValue(info.This());
}

void JSSpan::JsOnClick(const JSCallbackInfo& info)
{
    if (info[0]->IsFunction()) {
        auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
        if (!inspector) {
            LOGE("fail to get inspector for on click event");
            return;
        }
        auto impl = inspector->GetInspectorFunctionImpl();
        RefPtr<JsClickFunction> jsOnClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(info[0]));
        auto onClickId = EventMarker(
            [execCtx = info.GetExecutionContext(), func = std::move(jsOnClickFunc), impl](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                LOGD("About to call onclick method on js");
                auto clickInfo = TypeInfoHelper::DynamicCast<ClickInfo>(info);
                auto newInfo = *clickInfo;
                if (impl) {
                    impl->UpdateEventInfo(newInfo);
                }
                ACE_SCORING_EVENT("Span.onClick");
                func->Execute(newInfo);
            });
        auto component = GetComponent();
        if (component) {
            component->SetOnClick(onClickId);
        }
    }
}

void JSSpan::JsRemoteMessage(const JSCallbackInfo& info)
{
    EventMarker remoteMessageEventId;
    JSInteractableView::JsRemoteMessage(info, remoteMessageEventId);
    auto stack = ViewStackProcessor::GetInstance();
    auto textSpanComponent = AceType::DynamicCast<TextSpanComponent>(stack->GetMainComponent());
    textSpanComponent->SetRemoteMessageEventId(remoteMessageEventId);
}

void JSSpan::JSBind(BindingTarget globalObj)
{
    JSClass<JSSpan>::Declare("Span");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSpan>::StaticMethod("create", &JSSpan::Create, opt);
    JSClass<JSSpan>::StaticMethod("fontColor", &JSSpan::SetTextColor, opt);
    JSClass<JSSpan>::StaticMethod("fontSize", &JSSpan::SetFontSize, opt);
    JSClass<JSSpan>::StaticMethod("fontWeight", &JSSpan::SetFontWeight, opt);
    JSClass<JSSpan>::StaticMethod("fontStyle", &JSSpan::SetFontStyle, opt);
    JSClass<JSSpan>::StaticMethod("fontFamily", &JSSpan::SetFontFamily, opt);
    JSClass<JSSpan>::StaticMethod("letterSpacing", &JSSpan::SetLetterSpacing, opt);
    JSClass<JSSpan>::StaticMethod("textCase", &JSSpan::SetTextCase, opt);
    JSClass<JSSpan>::StaticMethod("decoration", &JSSpan::SetDecoration);
    JSClass<JSSpan>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSSpan>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSSpan>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSSpan>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSSpan>::StaticMethod("remoteMessage", &JSSpan::JsRemoteMessage);
    JSClass<JSSpan>::StaticMethod("onClick", &JSSpan::JsOnClick);
    JSClass<JSSpan>::Inherit<JSContainerBase>();
    JSClass<JSSpan>::Inherit<JSViewAbstract>();
    JSClass<JSSpan>::Bind<>(globalObj);
}

void JSSpan::Create(const JSCallbackInfo& info)
{
    std::string label;
    if (info.Length() > 0) {
        ParseJsString(info[0], label);
    }

    auto spanComponent = AceType::MakeRefPtr<OHOS::Ace::TextSpanComponent>(label);
    ViewStackProcessor::GetInstance()->Push(spanComponent);

    // Init text style, allowScale is not supported in declarative.
    auto textStyle = spanComponent->GetTextStyle();
    textStyle.SetAllowScale(false);
    spanComponent->SetTextStyle(std::move(textStyle));
}

RefPtr<TextSpanComponent> JSSpan::GetComponent()
{
    auto stack = ViewStackProcessor::GetInstance();
    if (!stack) {
        return nullptr;
    }
    auto component = AceType::DynamicCast<TextSpanComponent>(stack->GetMainComponent());
    return component;
}

} // namespace OHOS::Ace::Framework
