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

#include "core/components_v2/inspector/textarea_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text_field/text_field_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {

namespace {

const std::unordered_map<std::string, std::function<std::string(const TextareaComposedElement&)>> CREATE_JSON_MAP {
    { "placeholder", [](const TextareaComposedElement& inspector) { return inspector.GetPlaceholder(); } },
    { "text", [](const TextareaComposedElement& inspector) { return inspector.GetText(); } },
    { "placeholderColor", [](const TextareaComposedElement& inspector) { return inspector.GetPlaceholderColor(); } },
    { "placeholderFont", [](const TextareaComposedElement& inspector) { return inspector.GetPlaceholderFont(); } },
    { "textAlign", [](const TextareaComposedElement& inspector) { return inspector.GetTextAlign(); } },
    { "caretColor", [](const TextareaComposedElement& inspector) { return inspector.GetCaretColor(); } },
    { "fontColor", [](const TextareaComposedElement& inspector) { return inspector.GetTextFontColor(); } },
    { "fontSize", [](const TextareaComposedElement& inspector) { return inspector.GetTextFontSize(); } },
    { "fontStyle", [](const TextareaComposedElement& inspector) { return inspector.GetTextFontStyle(); } },
    { "fontWeight", [](const TextareaComposedElement& inspector) { return inspector.GetTextFontWeight(); } },
    { "fontFamily", [](const TextareaComposedElement& inspector) { return inspector.GetTextFontFamily(); } },
    { "inputFilter", [](const TextareaComposedElement& inspector) { return inspector.GetTextInputFilter(); } },
};

} // namespace

void TextareaComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("placeholder: ").append(GetPlaceholder()));
    DumpLog::GetInstance().AddDesc(std::string("text: ").append(GetText()));
    DumpLog::GetInstance().AddDesc(std::string("placeholderColor: ").append(GetPlaceholderColor()));
    DumpLog::GetInstance().AddDesc(std::string("placeholderFont: ").append(GetPlaceholderFont()));
    DumpLog::GetInstance().AddDesc(std::string("textAlign: ").append(GetTextAlign()));
    DumpLog::GetInstance().AddDesc(std::string("caretColor: ").append(GetCaretColor()));
    DumpLog::GetInstance().AddDesc(std::string("fontColor: ").append(GetTextFontColor()));
    DumpLog::GetInstance().AddDesc(std::string("fontSize: ").append(GetTextFontSize()));
    DumpLog::GetInstance().AddDesc(std::string("fontStyle: ").append(GetTextFontStyle()));
    DumpLog::GetInstance().AddDesc(std::string("fontWeight: ").append(GetTextFontWeight()));
    DumpLog::GetInstance().AddDesc(std::string("fontFamily: ").append(GetTextFontFamily()));
    DumpLog::GetInstance().AddDesc(std::string("inputFilter: ").append(GetTextInputFilter()));
}

std::unique_ptr<JsonValue> TextareaComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string TextareaComposedElement::GetPlaceholder() const
{
    auto render = GetRenderTextField();
    if (render) {
        return render->GetPlaceholder();
    }
    return "";
}

std::string TextareaComposedElement::GetText() const
{
    auto render = GetRenderTextField();
    if (render) {
        return render->GetValue();
    }
    return "";
}

std::string TextareaComposedElement::GetPlaceholderColor() const
{
    auto render = GetRenderTextField();
    if (render) {
        return ConvertColorToString(render->GetInactivePlaceholderColor());
    }
    return "";
}

std::string TextareaComposedElement::GetPlaceholderFont() const
{
    auto render = GetRenderTextField();
    auto jsonValue = JsonUtil::Create(true);
    if (render) {
        auto  placeHoldStyle = render->GetPlaceHoldStyle();
        jsonValue->Put("size", placeHoldStyle.GetFontSize().ToString().c_str());
        auto weight = placeHoldStyle.GetFontWeight();
        if (weight == FontWeight::W100) {
            jsonValue->Put("weight", "100");
        } else if (weight == FontWeight::W200) {
            jsonValue->Put("weight", "200");
        } else if (weight == FontWeight::W300) {
            jsonValue->Put("weight", "300");
        } else if (weight == FontWeight::W400) {
            jsonValue->Put("weight", "400");
        } else if (weight == FontWeight::W500) {
            jsonValue->Put("weight", "500");
        } else if (weight == FontWeight::W600) {
            jsonValue->Put("weight", "600");
        } else if (weight == FontWeight::W700) {
            jsonValue->Put("weight", "700");
        } else if (weight == FontWeight::W800) {
            jsonValue->Put("weight", "800");
        } else if (weight == FontWeight::W900) {
            jsonValue->Put("weight", "900");
        } else {
            jsonValue->Put("weight", ConvertWrapFontWeightToStirng(weight).c_str());
        }
        auto family = placeHoldStyle.GetFontFamilies();
        std::string jsonFamily = ConvertFontFamily(family);
        jsonValue->Put("family", jsonFamily.c_str());
        auto fontStyle = placeHoldStyle.GetFontStyle();
        jsonValue->Put("style", ConvertWrapFontStyleToStirng(fontStyle).c_str());
        return jsonValue->ToString();
    }
    return "";
}

std::string TextareaComposedElement::GetTextAlign() const
{
    auto render = GetRenderTextField();
    auto textAlign =
        render ? render->GetTextAlign() : TextAlign::START;
    return ConvertWrapTextAlignToString(textAlign);
}

std::string TextareaComposedElement::GetCaretColor() const
{
    auto render = GetRenderTextField();
    if (render) {
        return ConvertColorToString(render->GetCursorColor());
    }
    return "";
}

std::string TextareaComposedElement::GetTextFontColor() const
{
    auto render = GetRenderTextField();
    if (render) {
        return ConvertColorToString(render->GetEditingStyle().GetTextColor());
    }
    return "";
}

std::string TextareaComposedElement::GetTextFontSize() const
{
    auto render = GetRenderTextField();
    if (render) {
        return render->GetEditingStyle().GetFontSize().ToString().c_str();
    }
    return "";
}

std::string TextareaComposedElement::GetTextFontStyle() const
{
    auto render = GetRenderTextField();
    auto fontStyle =
        render ? render->GetEditingStyle().GetFontStyle() : FontStyle::NORMAL;
    return ConvertWrapFontStyleToStirng(fontStyle);
}

std::string TextareaComposedElement::GetTextFontWeight() const
{
    auto render = GetRenderTextField();
    auto weight =
        render ? render->GetEditingStyle().GetFontWeight() : FontWeight::NORMAL;
    if (weight == FontWeight::W100) {
            return "100";
        } else if (weight == FontWeight::W200) {
            return "200";
        } else if (weight == FontWeight::W300) {
            return "300";
        } else if (weight == FontWeight::W400) {
            return "400";
        } else if (weight == FontWeight::W500) {
            return "500";
        } else if (weight == FontWeight::W600) {
            return "600";
        } else if (weight == FontWeight::W700) {
            return "700";
        } else if (weight == FontWeight::W800) {
            return "800";
        } else if (weight == FontWeight::W900) {
            return "900";
        } else {
            return ConvertWrapFontWeightToStirng(weight);
        }
}

std::string TextareaComposedElement::GetTextFontFamily() const
{
    auto render = GetRenderTextField();
    auto fontFamily = render ? render->GetEditingStyle().GetFontFamilies() : std::vector<std::string>();
    return ConvertFontFamily(fontFamily);
}

RefPtr<RenderTextField> TextareaComposedElement::GetRenderTextField() const
{
    auto node = GetInspectorNode(TextFieldElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderTextField>(node);
    }
    return nullptr;
}

std::string TextareaComposedElement::ConvertFontFamily(const std::vector<std::string>& fontFamily) const
{
    std::string result = "";
    for (const auto& item : fontFamily) {
        result += item;
        result += ",";
    }
    result = result.substr(0, result.size() - 1);
    return result;
}

std::string TextareaComposedElement::GetTextInputFilter() const
{
    auto render = GetRenderTextField();
    if (render) {
        return render->GetTextInputFilter();
    }
    return "";
}

} // namespace OHOS::Ace::V2
