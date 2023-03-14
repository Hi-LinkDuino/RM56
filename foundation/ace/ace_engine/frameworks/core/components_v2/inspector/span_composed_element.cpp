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

#include "core/components_v2/inspector/span_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text_span/text_span_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const char NULL_STRING[] = "";

const std::unordered_map<std::string, std::function<std::string(const SpanComposedElement&)>> CREATE_JSON_MAP {
    { "content", [](const SpanComposedElement& inspector) { return inspector.GetSpanData(); } },
    { "fontsize", [](const SpanComposedElement& inspector) { return inspector.GetSpanTextFontSize(); } },
    { "decoration", [](const SpanComposedElement& inspector) { return inspector.GetDeclaration(); } },
    { "textCase", [](const SpanComposedElement& inspector) { return inspector.GetTextCase(); } }
};

} // namespace

void SpanComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("content: ").append(GetSpanData()));
    DumpLog::GetInstance().AddDesc(std::string("decoration: ").append(GetDeclaration()));
    DumpLog::GetInstance().AddDesc(std::string("textCase: ").append(GetTextCase()));
}

std::unique_ptr<JsonValue> SpanComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string SpanComposedElement::GetSpanData() const
{
    auto renderTextSpan = GetRenderTextSpan();
    return renderTextSpan ? renderTextSpan->GetSpanData() : NULL_STRING;
}

std::string SpanComposedElement::GetSpanTextFontSize() const
{
    auto renderTextSpan = GetRenderTextSpan();
    auto fontSize = renderTextSpan ? renderTextSpan->GetSpanStyle().GetFontSize() : Dimension();
    return fontSize.ToString();
}

std::string SpanComposedElement::GetTextCase() const
{
    auto renderTextSpan = GetRenderTextSpan();
    auto textCase =
        renderTextSpan ? renderTextSpan->GetSpanStyle().GetTextCase() : TextCase::NORMAL;
    return ConvertWrapTextCaseToStirng(textCase);
}

std::string SpanComposedElement::GetDeclaration() const
{
    auto jsonSpanDeclaration = JsonUtil::Create(true);
    auto renderTextSpan = GetRenderTextSpan();
    auto textDecoration = renderTextSpan ? renderTextSpan->GetSpanStyle().GetTextDecoration() : TextDecoration::NONE;
    auto textDecorationColor = renderTextSpan ? renderTextSpan->GetSpanStyle().GetTextDecorationColor() : Color::BLACK;
    jsonSpanDeclaration->Put("type", ConvertWrapTextDecorationToStirng(textDecoration).c_str());
    jsonSpanDeclaration->Put("color", ConvertColorToString(textDecorationColor).c_str());
    return jsonSpanDeclaration->ToString();
}

RefPtr<RenderTextSpan> SpanComposedElement::GetRenderTextSpan() const
{
    auto node = GetInspectorNode(TextSpanElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderTextSpan>(node);
    }
    return nullptr;
}

// span render with parent(text) together
std::string SpanComposedElement::GetRect()
{
    std::string strRec;
    Rect rect = GetParentRect();
    strRec = std::to_string(rect.Left()).append(",").
        append(std::to_string(rect.Top())).
        append(",").append(std::to_string(rect.Width())).
        append(",").append(std::to_string(rect.Height()));
    return strRec;
}
} // namespace OHOS::Ace::V2
