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

#include "core/components_v2/inspector/shape_container_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/shape/render_shape_container.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

using StrFuncType = std::function<std::string(const ShapeContainerComposedElement&)>;
const std::unordered_map<std::string, StrFuncType> CREATE_JSON_MAP {
    { "fill", [](const ShapeContainerComposedElement& inspector) { return inspector.GetFill(); } },
    { "fillOpacity", [](const ShapeContainerComposedElement& inspector) { return inspector.GetFillOpacity(); } },
    { "stroke", [](const ShapeContainerComposedElement& inspector) { return inspector.GetStroke(); } },
    { "strokeDashOffset",
        [](const ShapeContainerComposedElement& inspector) { return inspector.GetStrokeDashOffset(); } },
    { "strokeLineCap", [](const ShapeContainerComposedElement& inspector) { return inspector.GetStrokeLineCap(); } },
    { "strokeLineJoin", [](const ShapeContainerComposedElement& inspector) { return inspector.GetStrokeLineJoin(); } },
    { "strokeMiterLimit",
        [](const ShapeContainerComposedElement& inspector) { return inspector.GetStrokeMiterLimit(); } },
    { "strokeOpacity", [](const ShapeContainerComposedElement& inspector) { return inspector.GetStrokeOpacity(); } },
    { "strokeWidth", [](const ShapeContainerComposedElement& inspector) { return inspector.GetStrokeWidth(); } },
    { "antiAlias", [](const ShapeContainerComposedElement& inspector) { return inspector.GetAntiAlias(); } },
};

using JsonFuncType = std::function<std::unique_ptr<JsonValue>(const ShapeContainerComposedElement&)>;
const std::unordered_map<std::string, JsonFuncType> CREATE_JSON_JSON_VALUE_MAP {
    { "strokeDashArray",
        [](const ShapeContainerComposedElement& inspector) { return inspector.GetStrokeDashArray(); } },
    { "viewPort", [](const ShapeContainerComposedElement& inspector) { return inspector.GetViewBox(); } }
};

}

std::string ShapeContainerComposedElement::LineCapStyleToString(LineCapStyle lineCapStyle)
{
    static const std::unordered_map<LineCapStyle, std::string> STYLE_MAP {
        { LineCapStyle::BUTT, "LineCapStyle.Butt" },
        { LineCapStyle::ROUND, "LineCapStyle.Round" },
        { LineCapStyle::SQUARE, "LineCapStyle.Square" },
    };

    auto pos = STYLE_MAP.find(lineCapStyle);
    if (pos != STYLE_MAP.end()) {
        return pos->second;
    }
    return "";
}

std::string ShapeContainerComposedElement::LineJoinStyleToString(LineJoinStyle lineJoinStyle)
{
    static const std::unordered_map<LineJoinStyle, std::string> STYLE_MAP {
        { LineJoinStyle::MITER, "LineJoinStyle.Miter" },
        { LineJoinStyle::BEVEL, "LineJoinStyle.Bevel" },
        { LineJoinStyle::ROUND, "LineJoinStyle.Round" },
    };

    auto pos = STYLE_MAP.find(lineJoinStyle);
    if (pos != STYLE_MAP.end()) {
        return pos->second;
    }
    return "";
}

void ShapeContainerComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    for (const auto& value : CREATE_JSON_MAP) {
        DumpLog::GetInstance().AddDesc(std::string(value.first + ": ").append(value.second(*this)));
    }
}

std::unique_ptr<JsonValue> ShapeContainerComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    for (const auto& value : CREATE_JSON_JSON_VALUE_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

std::string ShapeContainerComposedElement::GetAntiAlias() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return ConvertBoolToString(render->GetAntiAlias());
    }
    return "";
}

std::string ShapeContainerComposedElement::GetFill() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return render->GetFillState().GetColor().ColorToString();
    }
    return "";
}

std::string ShapeContainerComposedElement::GetFillOpacity() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return std::to_string(render->GetFillState().GetOpacity().GetValue());
    }
    return "";
}

std::string ShapeContainerComposedElement::GetStroke() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return render->GetStrokeState().GetColor().ColorToString();
    }
    return "";
}

std::string ShapeContainerComposedElement::GetStrokeDashOffset() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return render->GetStrokeState().GetStrokeDashOffset().ToString();
    }
    return "";
}

std::unique_ptr<JsonValue> ShapeContainerComposedElement::GetStrokeDashArray() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    auto jsonDashArray = JsonUtil::CreateArray(false);
    if (render) {
        std::vector<Dimension> array = render->GetStrokeState().GetStrokeDashArray();
        for (size_t i = 0; i < array.size(); i++) {
            auto index = std::to_string(i);
            auto value = array[i].ToString();
            jsonDashArray->Put(index.c_str(), value.c_str());
        }
    }
    return jsonDashArray;
}

std::string ShapeContainerComposedElement::GetStrokeLineCap() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        auto style = render->GetStrokeState().GetLineCap();
        return LineCapStyleToString(style);
    }
    return "";
}

std::string ShapeContainerComposedElement::GetStrokeLineJoin() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        auto style = render->GetStrokeState().GetLineJoin();
        return LineJoinStyleToString(style);
    }
    return "";
}

std::string ShapeContainerComposedElement::GetStrokeMiterLimit() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return std::to_string(render->GetStrokeState().GetMiterLimit());
    }
    return "";
}

std::string ShapeContainerComposedElement::GetStrokeOpacity() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return std::to_string(render->GetStrokeState().GetOpacity().GetValue());
    }
    return "";
}

std::string ShapeContainerComposedElement::GetStrokeWidth() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    if (render) {
        return render->GetStrokeState().GetLineWidth().ToString();
    }
    return "";
}

std::unique_ptr<JsonValue> ShapeContainerComposedElement::GetViewBox() const
{
    auto render = GetContentRender<RenderShapeContainer>(ShapeContainerElement::TypeId());
    auto jsonViewBox = JsonUtil::Create(false);
    if (render) {
        auto viewBox = render->GetShapeViewBox();
        jsonViewBox->Put("x", viewBox.Left().ToString().c_str());
        jsonViewBox->Put("y", viewBox.Top().ToString().c_str());
        jsonViewBox->Put("width", viewBox.Width().ToString().c_str());
        jsonViewBox->Put("height", viewBox.Height().ToString().c_str());
    }
    return jsonViewBox;
}

} // namespace OHOS::Ace::V2