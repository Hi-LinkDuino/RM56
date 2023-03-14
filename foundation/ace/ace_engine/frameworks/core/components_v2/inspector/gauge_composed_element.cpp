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

#include "core/components_v2/inspector/gauge_composed_element.h"

#include "base/log/dump_log.h"
#include "base/utils/string_utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/progress/progress_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string, std::function<std::string(const GaugeComposedElement&)>> CREATE_JSON_MAP {
    { "value", [](const GaugeComposedElement& inspector) { return inspector.GetValue(); } },
    { "max", [](const GaugeComposedElement& inspector) { return inspector.GetMaxValue(); } },
    { "min", [](const GaugeComposedElement& inspector) { return inspector.GetMinValue(); } },
    { "startAngle", [](const GaugeComposedElement& inspector) { return inspector.GetStartAngle(); } },
    { "endAngle", [](const GaugeComposedElement& inspector) { return inspector.GetEndAngle(); } },
    { "strokeWidth", [](const GaugeComposedElement& inspector) { return inspector.GetStrokeWidth(); } },
    { "colors", [](const GaugeComposedElement& inspector) { return inspector.GetColors(); } }
};
} // namespace

void GaugeComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("value: ").append(GetValue()));
    DumpLog::GetInstance().AddDesc(std::string("max: ").append(GetMaxValue()));
    DumpLog::GetInstance().AddDesc(std::string("min: ").append(GetMinValue()));
    DumpLog::GetInstance().AddDesc(std::string("startAngle: ").append(GetStartAngle()));
    DumpLog::GetInstance().AddDesc(std::string("endAngle: ").append(GetEndAngle()));
    DumpLog::GetInstance().AddDesc(std::string("strokeWidth: ").append(GetStrokeWidth()));
    DumpLog::GetInstance().AddDesc(std::string("colors: ").append(GetColors()));
}

std::unique_ptr<JsonValue> GaugeComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string GaugeComposedElement::GetValue() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        return StringUtils::DoubleToString(renderProgress->GetValue());
    }
    return "0";
}

std::string GaugeComposedElement::GetMaxValue() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        return StringUtils::DoubleToString(renderProgress->GetMaxValue());
    }
    return "100";
}

std::string GaugeComposedElement::GetMinValue() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        return StringUtils::DoubleToString(renderProgress->GetMinValue());
    }
    return "0";
}

std::string GaugeComposedElement::GetStartAngle() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        auto startAngle = renderProgress->GetProgressComponent()->GetTrack()->GetTrackInfo()->GetStartDegree();
        return StringUtils::DoubleToString(startAngle);
    }
    return "-150";
}

std::string GaugeComposedElement::GetEndAngle() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        auto endAngle = renderProgress->GetProgressComponent()->GetTrack()->GetTrackInfo()->GetSweepDegree();
        return StringUtils::DoubleToString(endAngle);
    }
    return "150";
}

std::string GaugeComposedElement::GetStrokeWidth() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        return renderProgress->GetProgressComponent()->GetTrackThickness().ToString();
    }
    return "";
}

std::string GaugeComposedElement::GetColors() const
{
    auto renderProgress = GetRenderProgress();
    auto jsonColors = JsonUtil::CreateArray(true);
    if (!renderProgress) {
        return jsonColors->ToString();
    }
    auto colors = renderProgress->GetProgressComponent()->GetTrack()->GetSectionsColors();
    auto size = colors.size();
    if (size == 1) {
        return colors[0].ColorToString();
    }
    auto index = 0;
    for (auto value : colors) {
        jsonColors->Put(std::to_string(index++).c_str(), value.ColorToString().c_str());
    }
    return jsonColors->ToString();
}

RefPtr<RenderProgress> GaugeComposedElement::GetRenderProgress() const
{
    auto node = GetInspectorNode(ProgressElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderProgress>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2