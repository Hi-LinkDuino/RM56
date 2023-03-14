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

#include "frameworks/bridge/declarative_frontend/jsview/js_marquee.h"

#include "core/components/marquee/marquee_component.h"
#include "core/components/marquee/marquee_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSMarquee::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("marquee create error, info is non-valid");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto marqueeComponent = AceType::MakeRefPtr<OHOS::Ace::MarqueeComponent>();

    auto src = paramObject->GetProperty("src");
    if (!src->IsString()) {
        LOGE("marquee create error, src is non-valid");
        return;
    }
    marqueeComponent->SetValue(src->ToString());

    auto getStart = paramObject->GetProperty("start");
    bool start = getStart->IsBoolean() ? getStart->ToBoolean() : false;
    marqueeComponent->SetPlayerStatus(start);

    auto getStep = paramObject->GetProperty("step");
    if (getStep->IsNumber()) {
        marqueeComponent->SetScrollAmount(getStep->ToNumber<double>());
    }

    auto getLoop = paramObject->GetProperty("loop");
    if (getLoop->IsNumber()) {
        marqueeComponent->SetLoop(getLoop->ToNumber<int32_t>());
    }

    auto getFromStart = paramObject->GetProperty("fromStart");
    bool fromStart = getFromStart->IsBoolean() ? getFromStart->ToBoolean() : true;
    if (fromStart) {
        marqueeComponent->SetDirection(MarqueeDirection::LEFT);
    } else {
        marqueeComponent->SetDirection(MarqueeDirection::RIGHT);
    }
    ViewStackProcessor::GetInstance()->Push(marqueeComponent);
}

void JSMarquee::JSBind(BindingTarget globalObj)
{
    JSClass<JSMarquee>::Declare("Marquee");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSMarquee>::StaticMethod("create", &JSMarquee::Create, opt);
    JSClass<JSMarquee>::StaticMethod("fontColor", &JSMarquee::SetTextColor);
    JSClass<JSMarquee>::StaticMethod("fontSize", &JSMarquee::SetFontSize);
    JSClass<JSMarquee>::StaticMethod("allowScale", &JSMarquee::SetAllowScale);
    JSClass<JSMarquee>::StaticMethod("fontWeight", &JSMarquee::SetFontWeight);
    JSClass<JSMarquee>::StaticMethod("fontFamily", &JSMarquee::SetFontFamily);
    JSClass<JSMarquee>::StaticMethod("onStart", &JSMarquee::OnStart);
    JSClass<JSMarquee>::StaticMethod("onBounce", &JSMarquee::OnBounce);
    JSClass<JSMarquee>::StaticMethod("onFinish", &JSMarquee::OnFinish);
    JSClass<JSMarquee>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSMarquee>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSMarquee>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSMarquee>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSMarquee>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSMarquee>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSMarquee>::Inherit<JSViewAbstract>();
    JSClass<JSMarquee>::Bind(globalObj);
}

void JSMarquee::SetTextColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("SetTextColor create error, info is non-valid");
        return;
    }

    Color color;
    if (!ParseJsColor(info[0], color)) {
        LOGE("the info[0] is null");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::MarqueeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetTextColor) is null");
        return;
    }
    auto textStyle = component->GetTextStyle();
    textStyle.SetTextColor(color);
    component->SetTextStyle(std::move(textStyle));
}

void JSMarquee::SetFontSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("SetFrontSize create error, info is non-valid");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::MarqueeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetFrontSize) is null");
        return;
    }

    Dimension fontSize;
    if (!ParseJsDimensionFp(info[0], fontSize)) {
        return;
    }
    auto textStyle = component->GetTextStyle();
    textStyle.SetFontSize(fontSize);
    component->SetTextStyle(std::move(textStyle));
}

void JSMarquee::SetAllowScale(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsBoolean()) {
        LOGE("SetAllowScale create error, info is non-valid");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::MarqueeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetAllowScale) is null");
        return;
    }
    auto textStyle = component->GetTextStyle();
    textStyle.SetAllowScale(info[0]->ToBoolean());
    component->SetTextStyle(std::move(textStyle));
}

void JSMarquee::SetFontWeight(const std::string& value)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::MarqueeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetFrontWeight) is null");
        return;
    }
    auto textStyle = component->GetTextStyle();
    textStyle.SetFontWeight(ConvertStrToFontWeight(value));
    component->SetTextStyle(std::move(textStyle));
}

void JSMarquee::SetFontFamily(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("SetFrontFamily create error, info is non-valid");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::MarqueeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("The component(SetFrontFamily) is null");
        return;
    }

    std::vector<std::string> fontFamilies;
    if (!ParseJsFontFamilies(info[0], fontFamilies)) {
        LOGE("Parse FontFamilies failed");
        return;
    }
    auto textStyle = component->GetTextStyle();
    textStyle.SetFontFamilies(fontFamilies);
    component->SetTextStyle(std::move(textStyle));
}

void JSMarquee::OnStart(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&MarqueeComponent::SetOnStart, info)) {
        LOGW("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSMarquee::OnBounce(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&MarqueeComponent::SetOnBounce, info)) {
        LOGW("Failed to bind event");
    }
    info.ReturnSelf();
}

void JSMarquee::OnFinish(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&MarqueeComponent::SetOnFinish, info)) {
        LOGW("Failed to bind event");
    }
    info.ReturnSelf();
}

} // namespace OHOS::Ace::Framework
