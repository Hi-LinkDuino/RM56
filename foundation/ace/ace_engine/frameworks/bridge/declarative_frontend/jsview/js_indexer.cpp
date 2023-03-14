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

#include "bridge/declarative_frontend/jsview/js_indexer.h"

#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_scroller.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components_v2/indexer/indexer_component.h"
#include "core/components_v2/indexer/render_indexer.h"

namespace OHOS::Ace::Framework {
namespace {
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<V2::AlignStyle> ALIGN_STYLE = { V2::AlignStyle::LEFT, V2::AlignStyle::RIGHT };
}; // namespace

void JSIndexer::Create(const JSCallbackInfo& args)
{
    if (args.Length() >= 1 && args[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        std::vector<std::string> indexerArray;

        JSRef<JSVal> arrayVal = obj->GetProperty("arrayValue");
        if (!arrayVal->IsArray()) {
            LOGW("info is invalid");
            return;
        }

        JSRef<JSArray> array = JSRef<JSArray>::Cast(arrayVal);
        int32_t length = static_cast<int32_t>(array->Length());
        if (length <= 0) {
            LOGE("info is invalid");
            return;
        }
        for (int32_t i = 0; i < length; i++) {
            JSRef<JSVal> value = array->GetValueAt(i);
            std::string tmp;
            if (ParseJsString(value, tmp)) {
                indexerArray.emplace_back(tmp);
            }
        }

        JSRef<JSVal> selectedVal = obj->GetProperty("selected");
        if (!selectedVal->IsNumber()) {
            LOGE("info is invalid");
            return;
        }
        auto indexerComponent =
            AceType::MakeRefPtr<V2::IndexerComponent>(indexerArray, selectedVal->ToNumber<int32_t>());
        ViewStackProcessor::GetInstance()->Push(indexerComponent);
        JSInteractableView::SetFocusNode(true);
        args.ReturnSelf();
    }
}

void JSIndexer::SetSelectedColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }
    auto indexerComponent =
        AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (indexerComponent) {
        auto textStyle = indexerComponent->GetActiveTextStyle();
        textStyle.SetTextColor(color);
        indexerComponent->SetActiveTextStyle(std::move(textStyle));
    }
}

void JSIndexer::SetColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }
    auto indexerComponent =
        AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (indexerComponent) {
        auto textStyle = indexerComponent->GetNormalTextStyle();
        textStyle.SetTextColor(color);
        indexerComponent->SetNormalTextStyle(std::move(textStyle));
    }
}

void JSIndexer::SetPopupColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }
    auto indexerComponent =
        AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (indexerComponent) {
        auto textStyle = indexerComponent->GetBubbleTextStyle();
        textStyle.SetTextColor(color);
        indexerComponent->SetBubbleTextStyle(std::move(textStyle));
    }
}

void JSIndexer::SetSelectedBackgroundColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }
    auto indexerComponent =
        AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (indexerComponent) {
        indexerComponent->SetSelectedBackgroundColor(color);
    }
}

void JSIndexer::SetPopupBackground(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }
    auto indexerComponent =
        AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (indexerComponent) {
        indexerComponent->SetBubbleBackgroundColor(color);
    }
}

void JSIndexer::SetUsingPopup(bool state)
{
    auto indexerComponent =
        AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (indexerComponent) {
        indexerComponent->SetBubbleEnabled(state);
    }
}

void JSIndexer::SetSelectedFont(const JSCallbackInfo& args)
{
    if (args.Length() >= 1 && args[0]->IsObject()) {
        auto indexerComponent =
            AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (indexerComponent) {
            auto textStyle = indexerComponent->GetActiveTextStyle();
            GetFontContent(args, textStyle);
            indexerComponent->SetActiveTextStyle(std::move(textStyle));
        }
    }
}

void JSIndexer::SetPopupFont(const JSCallbackInfo& args)
{
    if (args.Length() >= 1 && args[0]->IsObject()) {
        auto indexerComponent =
            AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (indexerComponent) {
            auto textStyle = indexerComponent->GetBubbleTextStyle();
            GetFontContent(args, textStyle);
            indexerComponent->SetBubbleTextStyle(std::move(textStyle));
        }
    }
}

void JSIndexer::SetFont(const JSCallbackInfo& args)
{
    if (args.Length() >= 1 && args[0]->IsObject()) {
        auto indexerComponent =
            AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (indexerComponent) {
            auto textStyle = indexerComponent->GetNormalTextStyle();
            GetFontContent(args, textStyle);
            indexerComponent->SetNormalTextStyle(std::move(textStyle));
        }
    }
}

void JSIndexer::JsOnSelected(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onSelected = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE(execCtx);
                auto eventInfo = TypeInfoHelper::DynamicCast<V2::IndexerEventInfo>(info);
                if (!eventInfo) {
                    return;
                }
                auto params = ConvertToJSValues(eventInfo->GetSelectedIndex());
                func->Call(JSRef<JSObject>(), params.size(), params.data());
            });

        auto indexerComponent =
            AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (indexerComponent) {
            indexerComponent->SetSelectedEvent(onSelected);
        }
    }
}

void JSIndexer::JsOnRequestPopupData(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto requestPopupData =
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])]
            (std::shared_ptr<V2::IndexerEventInfo> info) {
                    std::vector<std::string> popupData;
                    JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx, popupData);
                    auto params = ConvertToJSValues(info->GetSelectedIndex());
                    JSRef<JSArray> result = func->Call(JSRef<JSObject>(), params.size(), params.data());
                    if (result.IsEmpty()) {
                        LOGE("Error calling onRequestPopupData result is empty.");
                        return popupData;
                    }

                    if (!result->IsArray()) {
                        LOGE("Error calling onRequestPopupData result is not array.");
                        return popupData;
                    }

                    for (size_t i = 0; i < result->Length(); i++) {
                        if (result->GetValueAt(i)->IsString()) {
                            auto item = result->GetValueAt(i);
                            popupData.emplace_back(item->ToString());
                        } else {
                            LOGE("Error calling onRequestPopupData index %{public}zu is not string.", i);
                        }
                    }
                    return popupData;
            };

        auto indexerComponent =
            AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (indexerComponent) {
            indexerComponent->SetRequestPopupDataFunc(requestPopupData);
        }
    }
}

void JSIndexer::JsOnPopupSelected(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onPopupSelected = EventMarker(
            [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](const BaseEventInfo* info) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                auto eventInfo = TypeInfoHelper::DynamicCast<V2::IndexerEventInfo>(info);
                if (!eventInfo) {
                    return;
                }
                auto params = ConvertToJSValues(eventInfo->GetSelectedIndex());
                func->Call(JSRef<JSObject>(), params.size(), params.data());
            });

        auto indexerComponent =
            AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (indexerComponent) {
            indexerComponent->SetPopupSelectedEvent(onPopupSelected);
        }
    }
}

void JSIndexer::GetFontContent(const JSCallbackInfo& args, TextStyle& textStyle)
{
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    JSRef<JSVal> size = obj->GetProperty("size");
    if (size->IsNumber()) {
        textStyle.SetFontSize(Dimension(size->ToNumber<double>(), DimensionUnit::FP));
    }

    JSRef<JSVal> weight = obj->GetProperty("weight");
    if (weight->IsString()) {
        textStyle.SetFontWeight(ConvertStrToFontWeight(weight->ToString()));
    }

    JSRef<JSVal> family = obj->GetProperty("family");
    std::vector<std::string> fontFamilies;
    if (ParseJsFontFamilies(family, fontFamilies)) {
        textStyle.SetFontFamilies(fontFamilies);
    }

    JSRef<JSVal> style = obj->GetProperty("style");
    if (style->IsNumber()) {
        int32_t value = style->ToNumber<int32_t>();
        if (value >= 0 && value < static_cast<int32_t>(FONT_STYLES.size())) {
            textStyle.SetFontStyle(FONT_STYLES[value]);
        }
    }
}

void JSIndexer::SetItemSize(const JSCallbackInfo& args)
{
    if (args.Length() >= 1) {
        Dimension value;
        if (ParseJsDimensionVp(args[0], value)) {
            auto indexerComponent =
                AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
            if (indexerComponent) {
                indexerComponent->SetItemSize(value);
            }
        }
    }
}

void JSIndexer::SetAlignStyle(int32_t value)
{
    if (value >= 0 && value < static_cast<int32_t>(ALIGN_STYLE.size())) {
        auto indexerComponent =
                AceType::DynamicCast<V2::IndexerComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (indexerComponent) {
            indexerComponent->SetAlignStyle(ALIGN_STYLE[value]);
        }
    }
}

void JSIndexer::JSBind(BindingTarget globalObj)
{
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSIndexer>::Declare("AlphabetIndexer");
    JSClass<JSIndexer>::StaticMethod("create", &JSIndexer::Create);
    // API7 onSelected deprecated
    JSClass<JSIndexer>::StaticMethod("onSelected", &JSIndexer::JsOnSelected);
    JSClass<JSIndexer>::StaticMethod("onSelect", &JSIndexer::JsOnSelected);
    JSClass<JSIndexer>::StaticMethod("color", &JSIndexer::SetColor, opt);
    JSClass<JSIndexer>::StaticMethod("selectedColor", &JSIndexer::SetSelectedColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupColor", &JSIndexer::SetPopupColor, opt);
    JSClass<JSIndexer>::StaticMethod("selectedBackgroundColor", &JSIndexer::SetSelectedBackgroundColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupBackground", &JSIndexer::SetPopupBackground, opt);
    JSClass<JSIndexer>::StaticMethod("usingPopup", &JSIndexer::SetUsingPopup, opt);
    JSClass<JSIndexer>::StaticMethod("selectedFont", &JSIndexer::SetSelectedFont);
    JSClass<JSIndexer>::StaticMethod("font", &JSIndexer::SetFont);
    JSClass<JSIndexer>::StaticMethod("popupFont", &JSIndexer::SetPopupFont);
    JSClass<JSIndexer>::StaticMethod("itemSize", &JSIndexer::SetItemSize, opt);
    JSClass<JSIndexer>::StaticMethod("alignStyle", &JSIndexer::SetAlignStyle, opt);
    JSClass<JSIndexer>::StaticMethod("onRequestPopupData", &JSIndexer::JsOnRequestPopupData, opt);
    // keep compatible, need remove after
    JSClass<JSIndexer>::StaticMethod("onPopupSelected", &JSIndexer::JsOnPopupSelected, opt);
    JSClass<JSIndexer>::StaticMethod("onPopupSelect", &JSIndexer::JsOnPopupSelected, opt);
    JSClass<JSIndexer>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSIndexer>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSIndexer>::Inherit<JSContainerBase>();
    JSClass<JSIndexer>::Inherit<JSViewAbstract>();
    JSClass<JSIndexer>::Bind(globalObj);
}
} // namespace OHOS::Ace::Framework
