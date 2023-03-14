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

#include "core/components_v2/inspector/progress_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/progress/progress_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const ProgressComposedElement&)>> CREATE_JSON_MAP {
    { "value", [](const ProgressComposedElement& inspector) { return inspector.GetValue(); } },
    { "constructor", [](const ProgressComposedElement& inspector) { return inspector.ProgressOptions(); } },
    { "color", [](const ProgressComposedElement& inspector) { return inspector.GetColor(); } }
};

using JsonFuncType = std::function<std::unique_ptr<JsonValue>(const ProgressComposedElement&)>;
const std::unordered_map<std::string, JsonFuncType> CREATE_JSON_JSON_VALUE_MAP {
    { "style", [](const ProgressComposedElement& inspector) { return inspector.GetCircularStyle(); } }
};

} // namespace

void ProgressComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("value: ").append(GetValue()));
    DumpLog::GetInstance().AddDesc(std::string("constructor: ").append(ProgressOptions()));
    DumpLog::GetInstance().AddDesc(std::string("color: ").append(GetColor()));
}

std::unique_ptr<JsonValue> ProgressComposedElement::ToJsonObject() const
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

std::string ProgressComposedElement::ProgressOptions() const
{
    auto JsonValue = JsonUtil::Create(false);
    JsonValue->Put("value", GetValue().c_str());
    JsonValue->Put("total", GetTotal().c_str());
    JsonValue->Put("type", GetTypeProgress().c_str());
    return JsonValue->ToString();
}

std::string ProgressComposedElement::GetValue() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        return std::to_string(renderProgress->GetValue());
    }
    return "";
}

std::string ProgressComposedElement::GetTotal() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        return std::to_string(renderProgress->GetMaxValue());
    }
    return "100";
}

std::string ProgressComposedElement::GetTypeProgress() const
{
    auto renderProgress = GetRenderProgress();
    if (!renderProgress) {
        return "ProgressStyle.Linear";
    }
    ProgressType type = renderProgress->GetProgressType();

    std::string result = "";
    switch (type) {
        case ProgressType::LINEAR:
            result = "ProgressStyle.Linear";
            break;
        case ProgressType::MOON:
            result = "ProgressStyle.Eclipse";
            break;
        case ProgressType::SCALE:
            result = "ProgressStyle.ScaleRing";
            break;
        case ProgressType::RING:
            result = "ProgressStyle.Ring";
            break;
        case ProgressType::CAPSULE:
            result = "ProgressStyle.Capsule";
            break;
        default:
            return "ProgressStyle.Linear";
    }
    return result;
}

std::string ProgressComposedElement::GetColor() const
{
    auto renderProgress = GetRenderProgress();
    if (renderProgress) {
        Color color = renderProgress->GetProgressComponent()->GetSelectColor();
        return color.ColorToString();
    }
    return "";
}

std::unique_ptr<JsonValue> ProgressComposedElement::GetCircularStyle() const
{
    auto renderProgress = GetRenderProgress();
    auto jsonCircularStyle = JsonUtil::Create(true);
    if (!renderProgress) {
        return jsonCircularStyle;
    }
    Dimension strokeWidth = renderProgress->GetProgressComponent()->GetTrackThickness();
    jsonCircularStyle->Put("strokeWidth", strokeWidth.ToString().c_str());
    auto scaleCount = renderProgress->GetProgressComponent()->GetScaleNumber();
    jsonCircularStyle->Put("scaleCount", std::to_string(scaleCount).c_str());
    Dimension scaleWidth = renderProgress->GetProgressComponent()->GetScaleWidth();
    jsonCircularStyle->Put("scaleWidth", scaleWidth.ToString().c_str());
    return jsonCircularStyle;
}

RefPtr<RenderProgress> ProgressComposedElement::GetRenderProgress() const
{
    auto node = GetInspectorNode(ProgressElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderProgress>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2
