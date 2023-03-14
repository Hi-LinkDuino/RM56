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

#include "frameworks/bridge/declarative_frontend/jsview/js_divider.h"

#include "core/components/box/box_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
void JSDivider::Create()
{
    RefPtr<Component> dividerComponent = AceType::MakeRefPtr<OHOS::Ace::DividerComponent>();
    ViewStackProcessor::GetInstance()->Push(dividerComponent);
}

void JSDivider::SetVertical(bool isVertical)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::DividerComponent>(stack->GetMainComponent());
    if (component) {
        component->SetVertical(isVertical);
    }

    auto box = stack->GetBoxComponent();
    if (isVertical) {
        box->SetFlex(BoxFlex::FLEX_Y);
    } else {
        box->SetFlex(BoxFlex::FLEX_X);
    }
}

void JSDivider::SetLineCap(int lineCap)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::DividerComponent>(stack->GetMainComponent());
    auto lineCapStyle = LineCap::BUTT;

    if (static_cast<int>(LineCap::SQUARE) == lineCap) {
        lineCapStyle = LineCap::SQUARE;
    } else if (static_cast<int>(LineCap::ROUND) == lineCap) {
        lineCapStyle = LineCap::ROUND;
    } else {
        // default linecap of divider
        lineCapStyle = LineCap::BUTT;
    }
    if (component) {
        component->SetLineCap(lineCapStyle);
    }
}

void JSDivider::SetDividerColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color dividerColor;
    if (!ParseJsColor(info[0], dividerColor)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::DividerComponent>(stack->GetMainComponent());
    if (component) {
        component->SetDividerColor(dividerColor);
    }
}

void JSDivider::SetStrokeWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension strokeWidth;
    if (!ParseJsDimensionVp(info[0], strokeWidth)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::DividerComponent>(stack->GetMainComponent());
    if (component) {
        component->SetStrokeWidth(strokeWidth);
    }
}

void JSDivider::JSBind(BindingTarget globalObj)
{
    JSClass<JSDivider>::Declare("Divider");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSDivider>::StaticMethod("create", &JSDivider::Create, opt);
    JSClass<JSDivider>::StaticMethod("color", &JSDivider::SetDividerColor, opt);
    JSClass<JSDivider>::StaticMethod("vertical", &JSDivider::SetVertical, opt);
    JSClass<JSDivider>::StaticMethod("strokeWidth", &JSDivider::SetStrokeWidth, opt);
    JSClass<JSDivider>::StaticMethod("lineCap", &JSDivider::SetLineCap, opt);
    JSClass<JSDivider>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSDivider>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSDivider>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSDivider>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSDivider>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSDivider>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSDivider>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSDivider>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);

    JSClass<JSDivider>::Inherit<JSViewAbstract>();
    JSClass<JSDivider>::Bind<>(globalObj);
}
} // namespace OHOS::Ace::Framework
