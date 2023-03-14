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

#include "core/components_v2/inspector/text_clock_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text/text_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const TextClockComposedElement&)>> CREATE_JSON_MAP {
    { "timeZoneOffset", [](const TextClockComposedElement& inspector) { return inspector.GetTimeZoneOffset(); } },
    { "format", [](const TextClockComposedElement& inspector) { return inspector.GeFormat(); } },
    { "fontColor", [](const TextClockComposedElement& inspector) { return inspector.GetTextFontColor(); } },
    { "fontSize", [](const TextClockComposedElement& inspector) { return inspector.GetTextFontSize(); } },
    { "fontStyle", [](const TextClockComposedElement& inspector) { return inspector.GetTextFontStyle(); } },
    { "fontWeight", [](const TextClockComposedElement& inspector) { return inspector.GetTextFontWeight(); } },
    { "fontFamily", [](const TextClockComposedElement& inspector) { return inspector.GetTextFontFamily(); } },
};

} // namespace

void TextClockComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("timeZoneOffset: ").append(GetTimeZoneOffset()));
    DumpLog::GetInstance().AddDesc(std::string("format: ").append(GeFormat()));
    DumpLog::GetInstance().AddDesc(std::string("fontColor: ").append(GetTextFontColor()));
    DumpLog::GetInstance().AddDesc(std::string("fontSize: ").append(GetTextFontSize()));
    DumpLog::GetInstance().AddDesc(std::string("fontStyle: ").append(GetTextFontStyle()));
    DumpLog::GetInstance().AddDesc(std::string("fontWeight: ").append(GetTextFontWeight()));
    DumpLog::GetInstance().AddDesc(std::string("fontFamily: ").append(GetTextFontFamily()));
}

std::unique_ptr<JsonValue> TextClockComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string TextClockComposedElement::GetTimeZoneOffset() const
{
    auto render = GetRenderTextClock();
    if (render) {
        double timeZoneOffset = render->GetHoursWest();
        return std::to_string(timeZoneOffset);
    }
    return "";
}

std::string TextClockComposedElement::GeFormat() const
{
    auto render = GetRenderTextClock();
    if (render) {
        return render->GeFormat();
    }
    return "";
}

RefPtr<RenderTextClock> TextClockComposedElement::GetRenderTextClock() const
{
    auto node = GetInspectorNode(TextClockElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderTextClock>(node);
    }
    return nullptr;
}

std::string TextClockComposedElement::GetTextFontColor() const
{
    auto renderText = GetRenderText();
    auto fontColor =
        renderText ? renderText->GetTextStyle().GetTextColor() : Color::BLACK;
    return fontColor.ColorToString();
}

std::string TextClockComposedElement::GetTextFontSize() const
{
    auto renderText = GetRenderText();
    auto fontSize =
        renderText ? renderText->GetTextStyle().GetFontSize() : Dimension();
    return fontSize.ToString();
}

std::string TextClockComposedElement::GetTextFontStyle() const
{
    auto renderText = GetRenderText();
    auto fontStyle =
        renderText ? renderText->GetTextStyle().GetFontStyle() : FontStyle::NORMAL;
    return ConvertWrapFontStyleToStirng(fontStyle);
}

std::string TextClockComposedElement::GetTextFontWeight() const
{
    auto renderText = GetRenderText();
    auto fontWeight =
        renderText ? renderText->GetTextStyle().GetFontWeight() : FontWeight::NORMAL;
    return ConvertWrapFontWeightToStirng(fontWeight);
}

std::string TextClockComposedElement::GetTextFontFamily() const
{
    auto renderText = GetRenderText();
    auto fontFamily = renderText ? renderText->GetTextStyle().GetFontFamilies() : std::vector<std::string>();
    return ConvertFontFamily(fontFamily);
}

std::string TextClockComposedElement::ConvertFontFamily(const std::vector<std::string>& fontFamily) const
{
    std::string result = "";
    for (const auto& item : fontFamily) {
        result += item;
        result += ",";
    }
    result = result.substr(0, result.size() - 1);
    return result;
}

RefPtr<RenderText> TextClockComposedElement::GetRenderText() const
{
    auto renderTextClock = GetRenderTextClock();
    if (renderTextClock) {
        return renderTextClock->GetRenderText();
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2