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

#include "core/components_v2/inspector/shape_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/shape/shape_element.h"
#include "core/components_v2/inspector/shape_container_composed_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const ShapeComposedElement&)>> CREATE_JSON_MAP {
    { "fill", [](const ShapeComposedElement& inspector) { return inspector.GetFill(); } },
    { "fillOpacity", [](const ShapeComposedElement& inspector) { return inspector.GetFillOpacity(); } },
    { "stroke", [](const ShapeComposedElement& inspector) { return inspector.GetStroke(); } },
    { "strokeDashOffset", [](const ShapeComposedElement& inspector) { return inspector.GetStrokeDashOffset(); } },
    { "strokeLineCap", [](const ShapeComposedElement& inspector) { return inspector.GetStrokeLineCap(); } },
    { "strokeLineJoin", [](const ShapeComposedElement& inspector) { return inspector.GetStrokeLineJoin(); } },
    { "strokeMiterLimit", [](const ShapeComposedElement& inspector) { return inspector.GetStrokeMiterLimit(); } },
    { "strokeOpacity", [](const ShapeComposedElement& inspector) { return inspector.GetStrokeOpacity(); } },
    { "strokeWidth", [](const ShapeComposedElement& inspector) { return inspector.GetStrokeWidth(); } },
    { "antiAlias", [](const ShapeComposedElement& inspector) { return inspector.GetAntiAlias(); } },
    { "commands", [](const ShapeComposedElement& inspector) { return inspector.GetCommands(); } }
};

using JsonRectType = std::function<std::unique_ptr<JsonValue>(const ShapeComposedElement&)>;
const std::unordered_map<std::string, JsonRectType> CREATE_JSON_JSON_VALUE_RECT_MAP {
    { "radius", [](const ShapeComposedElement& inspector) { return inspector.GetRadiusArray(); } },
    { "radiusWidth", [](const ShapeComposedElement& inspector) { return inspector.GetRadiusWidthArray(); } },
    { "radiusHeight", [](const ShapeComposedElement& inspector) { return inspector.GetRadiusHeightArray(); } }
};

using JsonFuncType = std::function<std::unique_ptr<JsonValue>(const ShapeComposedElement&)>;
const std::unordered_map<std::string, JsonFuncType> CREATE_JSON_JSON_VALUE_MAP {
    { "strokeDashArray", [](const ShapeComposedElement& inspector) { return inspector.GetStrokeDashArray(); } }
};

using JsonPointType = std::function<std::unique_ptr<JsonValue>(const ShapeComposedElement&)>;
const std::unordered_map<std::string, JsonPointType> CREATE_JSON_JSON_VALUE_POINT_MAP {
    { "startPoint", [](const ShapeComposedElement& inspector) { return inspector.GetStartPointArray(); } },
    { "endPoint", [](const ShapeComposedElement& inspector) { return inspector.GetEndPointArray(); } },
};

using JsonPointsType = std::function<std::unique_ptr<JsonValue>(const ShapeComposedElement&)>;
const std::unordered_map<std::string, JsonPointsType> CREATE_JSON_JSON_VALUE_POINTS_MAP {
    { "points", [](const ShapeComposedElement& inspector) { return inspector.GetPointsArray(); } }
};

} // namespace

void ShapeComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    for (const auto& value : CREATE_JSON_MAP) {
        DumpLog::GetInstance().AddDesc(std::string(value.first + ":").append(value.second(*this)));
    }
}

std::string ShapeComposedElement::GetWidth() const
{
    auto renderShape = AceType::DynamicCast<RenderShape>(GetInspectorNode(ShapeElement::TypeId()));
    if (renderShape) {
        return renderShape->GetWidthDimension().ToString();
    }
    return InspectorComposedElement::GetWidth();
}

std::string ShapeComposedElement::GetHeight() const
{
    auto renderShape = AceType::DynamicCast<RenderShape>(GetInspectorNode(ShapeElement::TypeId()));
    if (renderShape) {
        return renderShape->GetHeightDimension().ToString();
    }
    return InspectorComposedElement::GetHeight();
}

std::unique_ptr<JsonValue> ShapeComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    auto shapeType = GetShapeType();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }

    for (const auto& value : CREATE_JSON_JSON_VALUE_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    if ((shapeType == "Polyline") || (shapeType == "Polygon")) {
        for (const auto& value : CREATE_JSON_JSON_VALUE_POINTS_MAP) {
            resultJson->Put(value.first.c_str(), value.second(*this));
        }
    }
    if (shapeType == "Line") {
        for (const auto& value : CREATE_JSON_JSON_VALUE_POINT_MAP) {
            resultJson->Put(value.first.c_str(), value.second(*this));
        }
    }
    if (shapeType == "Rect") {
        for (const auto& value : CREATE_JSON_JSON_VALUE_RECT_MAP) {
            resultJson->Put(value.first.c_str(), value.second(*this));
        }
    }
    return resultJson;
}

std::string ShapeComposedElement::GetShapeType() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return SHAPE_TYPE_STRINGS[static_cast<int32_t>(render->GetShapeType())];
    }
    return "shape";
}

std::string ShapeComposedElement::GetCommands() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return render->GetPathCmd();
    }
    return "";
}

std::string ShapeComposedElement::GetAntiAlias() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return ConvertBoolToString(render->GetAntiAlias());
    }
    return "";
}

std::string ShapeComposedElement::GetFill() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return render->GetFillState().GetColor().ColorToString();
    }
    return "";
}

std::string ShapeComposedElement::GetFillOpacity() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return std::to_string(render->GetFillState().GetOpacity().GetValue());
    }
    return "";
}

std::string ShapeComposedElement::GetStroke() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return render->GetStrokeState().GetColor().ColorToString();
    }
    return "";
}

std::string ShapeComposedElement::GetStrokeDashOffset() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return render->GetStrokeState().GetStrokeDashOffset().ToString();
    }
    return "";
}

std::unique_ptr<JsonValue> ShapeComposedElement::GetStrokeDashArray() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
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

std::string ShapeComposedElement::GetStrokeLineCap() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        auto style = render->GetStrokeState().GetLineCap();
        return ShapeContainerComposedElement::LineCapStyleToString(style);
    }
    return "";
}

std::string ShapeComposedElement::GetStrokeLineJoin() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        auto style = render->GetStrokeState().GetLineJoin();
        return ShapeContainerComposedElement::LineJoinStyleToString(style);
    }
    return "";
}

std::string ShapeComposedElement::GetStrokeMiterLimit() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return std::to_string(render->GetStrokeState().GetMiterLimit());
    }
    return "";
}

std::string ShapeComposedElement::GetStrokeOpacity() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return std::to_string(render->GetStrokeState().GetOpacity().GetValue());
    }
    return "";
}

std::string ShapeComposedElement::GetStrokeWidth() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    if (render) {
        return render->GetStrokeState().GetLineWidth().ToString();
    }
    return "";
}

std::unique_ptr<JsonValue> ShapeComposedElement::GetRadiusArray() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    auto jsonRadiusArray = JsonUtil::CreateArray(true);
    if (!render) {
        jsonRadiusArray->Put("0", 0);
        return jsonRadiusArray;
    }
    std::vector<Radius> rads = {
        render->GetTopLeftRadius(), render->GetTopRightRadius(), render->GetBottomLeftRadius(),
        render->GetBottomRightRadius()
    };
    for (size_t i = 0; i < rads.size(); i++) {
        auto jsonObject = JsonUtil::CreateArray(false);
        jsonObject->Put("0", rads[i].GetX().Value());
        jsonObject->Put("1", rads[i].GetY().Value());
        auto index = std::to_string(i);
        jsonRadiusArray->Put(index.c_str(), jsonObject);
    }
    if (rads[0].GetX().Value() == -1) {
        auto radiusArray = JsonUtil::CreateArray(true);
        radiusArray->Put("0", 0);
        return radiusArray;
    }
    return jsonRadiusArray;
}

std::unique_ptr<JsonValue> ShapeComposedElement::GetRadiusHeightArray() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    auto jsonRadiusArray = JsonUtil::CreateArray(true);
    if (!render) {
        jsonRadiusArray->Put("0", 0);
        return jsonRadiusArray;
    }
    Radius topLeftRadius = render->GetTopLeftRadius();
    Radius topRightRadius = render->GetTopRightRadius();
    Radius bottomRightRadius = render->GetBottomRightRadius();
    Radius bottomLeftRadius = render->GetBottomLeftRadius();
    if (topLeftRadius.GetY().Value() == -1) {
        jsonRadiusArray->Put("0", 0);
        return jsonRadiusArray;
    }
    if ((topLeftRadius.GetY() == topRightRadius.GetY()) && (topLeftRadius.GetY() == bottomRightRadius.GetY()) &&
        (topLeftRadius.GetY() == bottomLeftRadius.GetY())) {
        jsonRadiusArray->Put("0", topLeftRadius.GetY().Value());
        return jsonRadiusArray;
    } else {
        jsonRadiusArray->Put("0", topLeftRadius.GetY().Value());
        jsonRadiusArray->Put("1", topRightRadius.GetY().Value());
        jsonRadiusArray->Put("2", bottomLeftRadius.GetY().Value());
        jsonRadiusArray->Put("3", bottomRightRadius.GetY().Value());
        return jsonRadiusArray;
    }
}

std::unique_ptr<JsonValue> ShapeComposedElement::GetRadiusWidthArray() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    auto jsonRadiusArray = JsonUtil::CreateArray(true);
    if (!render) {
        jsonRadiusArray->Put("0", 0);
        return jsonRadiusArray;
    }
    Radius topLeftRadius = render->GetTopLeftRadius();
    Radius topRightRadius = render->GetTopRightRadius();
    Radius bottomRightRadius = render->GetBottomRightRadius();
    Radius bottomLeftRadius = render->GetBottomLeftRadius();
    if (topLeftRadius.GetX().Value() == -1) {
        jsonRadiusArray->Put("0", 0);
        return jsonRadiusArray;
    }
    if ((topLeftRadius.GetX() == topRightRadius.GetX()) && (topLeftRadius.GetX() == bottomRightRadius.GetX()) &&
        (topLeftRadius.GetX() == bottomLeftRadius.GetX())) {
        jsonRadiusArray->Put("0", topLeftRadius.GetX().Value());
        return jsonRadiusArray;
    } else {
        jsonRadiusArray->Put("0", topLeftRadius.GetX().Value());
        jsonRadiusArray->Put("1", topRightRadius.GetX().Value());
        jsonRadiusArray->Put("2", bottomLeftRadius.GetX().Value());
        jsonRadiusArray->Put("3", bottomRightRadius.GetX().Value());
        return jsonRadiusArray;
    }
}

std::unique_ptr<JsonValue> ShapeComposedElement::GetStartPointArray() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    auto startPointArray = JsonUtil::CreateArray(false);
    if (!render) {
        startPointArray->Put("0", 0);
        startPointArray->Put("1", 0);
        return startPointArray;
    }
    ShapePoint startPoint = render->GetStartPoint();
    startPointArray->Put("0", startPoint.first.Value());
    startPointArray->Put("1", startPoint.second.Value());
    return startPointArray;
}

std::unique_ptr<JsonValue> ShapeComposedElement::GetEndPointArray() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    auto endPointArray = JsonUtil::CreateArray(false);
    if (!render) {
        endPointArray->Put("0", 0);
        endPointArray->Put("1", 0);
        return endPointArray;
    }
    ShapePoint endPoint = render->GetEndPoint();
    endPointArray->Put("0", endPoint.first.Value());
    endPointArray->Put("1", endPoint.second.Value());
    return endPointArray;
}

std::unique_ptr<JsonValue> ShapeComposedElement::GetPointsArray() const
{
    auto render = GetContentRender<RenderShape>(ShapeElement::TypeId());
    auto pointsArray = JsonUtil::CreateArray(false);
    if (!render) {
        return pointsArray;
    }
    std::vector<ShapePoint> points = render->GetPoints();
    for (size_t i = 0; i < points.size(); i++) {
        auto pointsObject = JsonUtil::CreateArray(false);
        pointsObject->Put("0", points[i].first.Value());
        pointsObject->Put("1", points[i].second.Value());
        auto index = std::to_string(i);
        pointsArray->Put(index.c_str(), pointsObject);
    }
    return pointsArray;
}

} // namespace OHOS::Ace::V2