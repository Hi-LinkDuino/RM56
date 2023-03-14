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

#include "core/components_v2/inspector/slider_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/slider/flutter_render_slider.h"
#include "core/components/slider/slider_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string,
    std::function<std::unique_ptr<JsonValue>(const SliderComposedElement&)>> CREATE_JSON_MAP {
    { "constructor", [](const SliderComposedElement& inspector) { return inspector.GetConstructor(); } }
};

const std::unordered_map<std::string, std::function<std::string(const SliderComposedElement&)>> CREATE_JSON_STRING_MAP {
    { "blockColor", [](const SliderComposedElement& inspector) { return inspector.GetBlockColor(); } },
    { "trackColor", [](const SliderComposedElement& inspector) { return inspector.GetTrackColor(); } },
    { "selectedColor", [](const SliderComposedElement& inspector) { return inspector.GetSelectedColor(); } },
    { "showSteps", [](const SliderComposedElement& inspector) { return inspector.GetShowSteps(); } },
    { "showTips", [](const SliderComposedElement& inspector) { return inspector.GetShowTips(); } }
};

} // namespace

void SliderComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("value: ").append(GetValue()));
    DumpLog::GetInstance().AddDesc(std::string("max: ").append(GetMax()));
    DumpLog::GetInstance().AddDesc(std::string("min: ").append(GetMin()));
    DumpLog::GetInstance().AddDesc(std::string("step: ").append(GetStep()));
    DumpLog::GetInstance().AddDesc(std::string("reverse: ").append(GetReverse()));
    DumpLog::GetInstance().AddDesc(std::string("trackThickness: ").append(GetThickness()));
    DumpLog::GetInstance().AddDesc(std::string("blockColor: ").append(GetBlockColor()));
    DumpLog::GetInstance().AddDesc(std::string("trackColor: ").append(GetTrackColor()));
    DumpLog::GetInstance().AddDesc(std::string("selectedColor: ").append(GetSelectedColor()));
    DumpLog::GetInstance().AddDesc(std::string("showSteps: ").append(GetShowSteps()));
    DumpLog::GetInstance().AddDesc(std::string("showTips: ").append(GetShowTips()));
}

std::unique_ptr<JsonValue> SliderComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    for (const auto& value : CREATE_JSON_STRING_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::unique_ptr<JsonValue> SliderComposedElement::GetConstructor() const
{
    auto jsonValue = JsonUtil::Create(true);
    jsonValue->Put("direction", GetSliderDirection().c_str());
    jsonValue->Put("value", GetValue().c_str());
    jsonValue->Put("max", GetMax().c_str());
    jsonValue->Put("min", GetMin().c_str());
    jsonValue->Put("step", GetStep().c_str());
    jsonValue->Put("style", GetStyle().c_str());
    jsonValue->Put("reverse", GetReverse().c_str());
    jsonValue->Put("trackThickness", GetThickness().c_str());
    return jsonValue;
}

std::string SliderComposedElement::GetSliderDirection() const
{
    auto renderSlider = GetRenderSlider();
    if (!renderSlider) {
        return "Axis.Axis.Horizontal";
    }
    auto direction = renderSlider->GetDirection();
    if (direction == Axis::VERTICAL) {
        return "Axis.Vertical";
    } else {
        return "Axis.Horizontal";
    }
}

std::string SliderComposedElement::GetValue() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return std::to_string(renderSlider->GetValue());
    }
    return "0";
}

std::string SliderComposedElement::GetMax() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return std::to_string(renderSlider->GetMax());
    }
    return "100";
}

std::string SliderComposedElement::GetMin() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return std::to_string(renderSlider->GetMin());
    }
    return "0";
}

std::string SliderComposedElement::GetStep() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return std::to_string(renderSlider->GetStep());
    }
    return "1";
}

std::string SliderComposedElement::GetStyle() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        SliderMode mode = renderSlider->GetMode();
        if (mode == SliderMode::INSET) {
            return "SliderStyle.InSet";
        } else {
            return "SliderStyle.OutSet";
        }
    }
    return "SliderStyle.OutSet";
}

std::string SliderComposedElement::GetReverse() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return ConvertBoolToString(renderSlider->GetIsReverse());
    }
    return "false";
}

std::string SliderComposedElement::GetThickness() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return StringUtils::DoubleToString(renderSlider->GetThickness());
    }
    return "";
}

std::string SliderComposedElement::GetBlockColor() const
{
    auto renderSlider = GetRenderSlider();
    auto flutterRenderSlider = AceType::DynamicCast<FlutterRenderSlider>(renderSlider);
    if (flutterRenderSlider) {
        auto block = flutterRenderSlider->GetRenderBlock();
        if (block) {
            return block->GetBlockColor().ColorToString();
        }
        return "";
    }
    return "";
}

std::string SliderComposedElement::GetTrackColor() const
{
    auto renderSlider = GetRenderSlider();
    auto flutterRenderSlider = AceType::DynamicCast<FlutterRenderSlider>(renderSlider);
    if (flutterRenderSlider) {
        auto track = flutterRenderSlider->GetRenderTrack();
        if (track) {
            return track->GetBackgroundColor().ColorToString();
        }
        return "";
    }
    return "";
}

std::string SliderComposedElement::GetSelectedColor() const
{
    auto renderSlider = GetRenderSlider();
    auto flutterRenderSlider = AceType::DynamicCast<FlutterRenderSlider>(renderSlider);
    if (flutterRenderSlider) {
        auto track = flutterRenderSlider->GetRenderTrack();
        if (track) {
            return track->GetSelectColor().ColorToString();
        }
        return "";
    }
    return "";
}

std::string SliderComposedElement::GetShowSteps() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return ConvertBoolToString(renderSlider->GetShowSteps());
    }
    return "false";
}

std::string SliderComposedElement::GetShowTips() const
{
    auto renderSlider = GetRenderSlider();
    if (renderSlider) {
        return ConvertBoolToString(renderSlider->GetShowTips());
    }
    return "false";
}

RefPtr<RenderSlider> SliderComposedElement::GetRenderSlider() const
{
    auto node = GetInspectorNode(SliderElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderSlider>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2
