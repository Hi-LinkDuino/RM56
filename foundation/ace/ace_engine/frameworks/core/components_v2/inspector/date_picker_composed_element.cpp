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

#include "core/components_v2/inspector/date_picker_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/picker/picker_base_element.h"
#include "core/components/picker/picker_date_component.h"
#include "core/components/picker/picker_time_component.h"
#include "core/components/picker/render_picker_base.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string, std::function<std::string(const DatePickerComposedElement&)>> CREATE_JSON_MAP {
    { "lunar", [](const DatePickerComposedElement& inspector) { return inspector.GetLunar(); } },
    { "start", [](const DatePickerComposedElement& inspector) { return inspector.GetStart(); } },
    { "end", [](const DatePickerComposedElement& inspector) { return inspector.GetEnd(); } },
    { "selected", [](const DatePickerComposedElement& inspector) { return inspector.GetSelected(); } }
};
}

void DatePickerComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("lunar: ").append(GetLunar()));
    DumpLog::GetInstance().AddDesc(std::string("start: ").append(GetStart()));
    DumpLog::GetInstance().AddDesc(std::string("end: ").append(GetEnd()));
    DumpLog::GetInstance().AddDesc(std::string("selected: ").append(GetSelected()));
}

std::unique_ptr<JsonValue> DatePickerComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string DatePickerComposedElement::GetLunar() const
{
    auto render = GetRenderPickerBase();
    auto showLunar = render ? render->GetPickerBaseComponent()->IsShowLunar() : false;
    return ConvertBoolToString(showLunar);
}

std::string DatePickerComposedElement::GetStart() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        auto start = render->GetStartDate();
        std::string startDate;
        startDate += std::to_string(start.GetYear());
        startDate += "-";
        startDate += std::to_string(start.GetMonth());
        startDate += "-";
        startDate += std::to_string(start.GetDay());
        return startDate;
    }
    return "1970-1-1";
}

std::string DatePickerComposedElement::GetEnd() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        auto start = render->GetEndDate();
        std::string endDate;
        endDate += std::to_string(start.GetYear());
        endDate += "-";
        endDate += std::to_string(start.GetMonth());
        endDate += "-";
        endDate += std::to_string(start.GetDay());
        return endDate;
    }
    return "2100-12-31";
}

std::string DatePickerComposedElement::GetSelected() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        auto start = render->GetSelectedDate();
        std::string selectedDate;
        selectedDate += std::to_string(start.GetYear());
        selectedDate += "-";
        selectedDate += std::to_string(start.GetMonth());
        selectedDate += "-";
        selectedDate += std::to_string(start.GetDay());
        return selectedDate;
    }
    return "";
}

RefPtr<RenderPickerBase> DatePickerComposedElement::GetRenderPickerBase() const
{
    auto node = GetInspectorNode(PickerBaseElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderPickerBase>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2
