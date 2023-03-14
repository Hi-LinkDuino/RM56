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

#include "core/components_v2/inspector/texttimer_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text_field/text_field_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {

namespace {

const std::unordered_map<std::string, std::function<std::string(const TextTimerComposedElement&)>> CREATE_JSON_MAP {
    { "isCountDown", [](const TextTimerComposedElement& inspector) { return inspector.GetIsCountDown(); } },
    { "count", [](const TextTimerComposedElement& inspector) { return inspector.GetCount(); } },
    { "format", [](const TextTimerComposedElement& inspector) { return inspector.GetFormat(); } },
    { "fontColor", [](const TextTimerComposedElement& inspector) { return inspector.GetFontColor(); } },
    { "fontSize", [](const TextTimerComposedElement& inspector) { return inspector.GetFontSize(); } },
    { "fontWeight", [](const TextTimerComposedElement& inspector) { return inspector.GetFontWeight(); } },
    { "fontStyle", [](const TextTimerComposedElement& inspector) { return inspector.GetFontStyle(); } },
    { "fontfamily", [](const TextTimerComposedElement& inspector) { return inspector.GetFontFamily(); } },
};

} // namespace

void TextTimerComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("isCountDown: ").append(GetIsCountDown()));
    DumpLog::GetInstance().AddDesc(std::string("count: ").append(GetCount()));
    DumpLog::GetInstance().AddDesc(std::string("format: ").append(GetFormat()));
    DumpLog::GetInstance().AddDesc(std::string("fontColor : ").append(GetFontColor()));
    DumpLog::GetInstance().AddDesc(std::string("fontSize : ").append(GetFontSize()));
    DumpLog::GetInstance().AddDesc(std::string("fontWeight : ").append(GetFontWeight()));
    DumpLog::GetInstance().AddDesc(std::string("fontStyle : ").append(GetFontSize()));
    DumpLog::GetInstance().AddDesc(std::string("fontfamily : ").append(GetFontSize()));
}

std::unique_ptr<JsonValue> TextTimerComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string TextTimerComposedElement::GetIsCountDown() const
{
    auto render = GetRenderTextTimer();
    if (render) {
        return ConvertBoolToString(render->GetIsCountDown());
    }
    return "false";
}

std::string TextTimerComposedElement::GetCount() const
{
    auto render = GetRenderTextTimer();
    if (render) {
        return std::to_string(render->GetCount());
    }
    return "60000";
}

std::string TextTimerComposedElement::GetFormat() const
{
    auto render = GetRenderTextTimer();
    if (render) {
        return render->GetFormat();
    }
    return "hh:mm:ss.ms";
}

std::string TextTimerComposedElement::GetFontColor() const
{
    auto render = GetRenderTextTimer();
    auto fontColor =
        render ? render->GetTextStyle().GetTextColor() : Color::BLACK;
    return fontColor.ColorToString();
}
std::string TextTimerComposedElement::GetFontSize() const
{
    auto render = GetRenderTextTimer();
    auto fontSize =
        render ? render->GetTextStyle().GetFontSize() : Dimension();
    return fontSize.ToString();
}

std::string TextTimerComposedElement::GetFontWeight() const
{
    auto render = GetRenderTextTimer();
    auto fontWeight =
        render ? render->GetTextStyle().GetFontWeight() : FontWeight::NORMAL;
    return ConvertWrapFontWeightToStirng(fontWeight);
}

std::string TextTimerComposedElement::GetFontStyle() const
{
    auto render = GetRenderTextTimer();
    auto fontStyle =
        render ? render->GetTextStyle().GetFontStyle() : FontStyle::NORMAL;
    return ConvertWrapFontStyleToStirng(fontStyle);
}

std::string TextTimerComposedElement::GetFontFamily() const
{
    auto render = GetRenderTextTimer();
    auto fontFamily = render ? render->GetTextStyle().GetFontFamilies() : std::vector<std::string>();
    return ConvertFontFamily(fontFamily);
}

std::string TextTimerComposedElement::ConvertFontFamily(const std::vector<std::string>& fontFamily) const
{
    std::string result = "";
    for (const auto& item : fontFamily) {
        result += item;
        result += ",";
    }
    result = result.substr(0, result.size() - 1);
    return result;
}

RefPtr<RenderTextTimer> TextTimerComposedElement::GetRenderTextTimer() const
{
    auto node = GetInspectorNode(TextTimerElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderTextTimer>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2