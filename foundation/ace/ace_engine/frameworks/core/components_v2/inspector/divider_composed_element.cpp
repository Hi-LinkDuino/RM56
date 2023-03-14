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

#include "core/components_v2/inspector/divider_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {

namespace {

const std::unordered_map<std::string, std::function<std::string(const DividerComposedElement&)>> CREATE_JSON_MAP {
    { "vertical", [](const DividerComposedElement& inspector) { return inspector.GetDividerVertical(); } },
    { "color", [](const DividerComposedElement& inspector) { return inspector.GetDividerColor(); } },
    { "strokeWidth", [](const DividerComposedElement& inspector) { return inspector.GetStrokeWidth(); } },
    { "lineCap", [](const DividerComposedElement& inspector) { return inspector.GetDividerLineCap(); } },
};

}

void DividerComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("vertical: ").append(GetDividerVertical().c_str()));
    DumpLog::GetInstance().AddDesc(
        std::string("color: ").append(GetDividerColor().c_str()));
    DumpLog::GetInstance().AddDesc(
        std::string("strokeWidth: ").append(GetStrokeWidth().c_str()));
    DumpLog::GetInstance().AddDesc(
        std::string("lineCap: ").append(GetDividerLineCap().c_str()));
}

std::unique_ptr<JsonValue> DividerComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string DividerComposedElement::GetDividerVertical() const
{
    auto renderDivider = GetRenderDivider();
    auto vertical = renderDivider ? renderDivider->GetIsVertical() : false;
    return ConvertBoolToString(vertical);
}

std::string DividerComposedElement::GetDividerColor() const
{
    auto renderDivider = GetRenderDivider();
    auto color = renderDivider ? renderDivider->GetDividerColor() : Color::BLACK;
    return ConvertColorToString(color);
}

std::string DividerComposedElement::GetStrokeWidth() const
{
    auto renderDivider = GetRenderDivider();
    auto strokeWidth = renderDivider ? renderDivider->GetStrokeWidth() : Dimension(1.0);
    return std::to_string(strokeWidth.Value());
}

std::string DividerComposedElement::GetDividerLineCap() const
{
    auto renderDivider = GetRenderDivider();
    auto lineCap = renderDivider ? renderDivider->GetLineCap() : LineCap::BUTT;
    return ConvertLineCapToString(lineCap);
}

RefPtr<RenderDivider> DividerComposedElement::GetRenderDivider() const
{
    auto node = GetInspectorNode(DividerElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderDivider>(node);
    }
    return nullptr;
}

std::string DividerComposedElement::ConvertLineCapToString(LineCap cap) const
{
    std::string result = "";
    switch (cap) {
        case LineCap::BUTT:
            result = "LineCapStyle.Butt";
            break;
        case LineCap::ROUND:
            result = "LineCapStyle.Round";
            break;
        case LineCap::SQUARE:
            result = "LineCapStyle.Square";
            break;
        default:
            LOGD("input do not match any ButtonType");
    }
    return result;
}

} // namespace OHOS::Ace::V2