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

#include "frameworks/bridge/declarative_frontend/jsview/js_hyperlink.h"

#include "base/log/ace_trace.h"
#include "core/components/hyperlink/hyperlink_component.h"
#include "core/components/text/text_component.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSHyperlink::Create(const JSCallbackInfo& args)
{
    std::list<RefPtr<Component>> children;
    RefPtr<OHOS::Ace::HyperlinkComponent> component = AceType::MakeRefPtr<HyperlinkComponent>(children);
    if (args.Length() == 1) {
        std::string address;
        if (ParseJsString(args[0], address)) {
            component->SetAddress(address);
        }
    } else if (args.Length() == 2) {
        std::string address;
        if (ParseJsString(args[0], address)) {
            component->SetAddress(address);
        }
        std::string summary;
        if (ParseJsString(args[1], summary)) {
            component->SetSummary(summary);
        }
    }

    ViewStackProcessor::GetInstance()->Push(component);
    JSInteractableView::SetFocusNode(true);
}

void JSHyperlink::Pop()
{
    auto hyperlink =
        AceType::DynamicCast<OHOS::Ace::HyperlinkComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    JSContainerBase::Pop();
    if (hyperlink) {
        std::string summary = hyperlink->GetSummary();
        std::list<RefPtr<Component>> children = hyperlink->GetChildren();
        std::list<RefPtr<Component>> flexChild;
        if (!summary.empty()) {
            RefPtr<OHOS::Ace::TextComponent> text = AceType::MakeRefPtr<TextComponent>(summary);
            flexChild.emplace_back(text);
        }
        for (const auto& child : children) {
            flexChild.emplace_back(child);
        }
        RefPtr<OHOS::Ace::ColumnComponent> columnComponent =
            AceType::MakeRefPtr<OHOS::Ace::ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, flexChild);
        columnComponent->SetMainAxisSize(MainAxisSize::MIN);
        columnComponent->SetCrossAxisSize(CrossAxisSize::MIN);
        hyperlink->ClearChildren();
        hyperlink->AppendChild(columnComponent);
    }
}

void JSHyperlink::SetColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(info[0], color)) {
        return;
    }
    auto component = GetComponent();
    if (!component) {
        LOGE("component is not valid");
        return;
    }
    component->SetColor(color);
}

void JSHyperlink::JSBind(BindingTarget globalObj)
{
    JSClass<JSHyperlink>::Declare("Hyperlink");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSHyperlink>::StaticMethod("create", &JSHyperlink::Create, opt);
    JSClass<JSHyperlink>::StaticMethod("color", &JSHyperlink::SetColor, opt);
    JSClass<JSHyperlink>::StaticMethod("pop", &JSHyperlink::Pop, opt);
    JSClass<JSHyperlink>::Inherit<JSContainerBase>();
    JSClass<JSHyperlink>::Inherit<JSViewAbstract>();
    JSClass<JSHyperlink>::Bind<>(globalObj);
}

RefPtr<HyperlinkComponent> JSHyperlink::GetComponent()
{
    auto stack = ViewStackProcessor::GetInstance();
    if (!stack) {
        return nullptr;
    }
    return AceType::DynamicCast<HyperlinkComponent>(stack->GetMainComponent());
}

} // namespace OHOS::Ace::Framework