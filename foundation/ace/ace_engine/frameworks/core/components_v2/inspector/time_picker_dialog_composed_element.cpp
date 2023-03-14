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

#include "core/components_v2/inspector/time_picker_dialog_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/picker/picker_base_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string, 
    std::function<std::string(const TimePickerDialogComposedElement&)>> CREATE_JSON_MAP {
    { "show", [](const TimePickerDialogComposedElement& inspector) { return inspector.GetShow(); } }
};
}

void TimePickerDialogComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("show: ").append(GetShow()));
}

std::unique_ptr<JsonValue> TimePickerDialogComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string TimePickerDialogComposedElement::GetShow() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto jsonShow = JsonUtil::Create(false);
    jsonValue->Put("selected", GetSelected().c_str());
    jsonValue->Put("useMilitaryTime", GetUseMilitaryTime().c_str());
    jsonShow->Put("show", jsonValue);
    return jsonShow->ToString();
}

std::string TimePickerDialogComposedElement::GetSelected() const
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

std::string TimePickerDialogComposedElement::GetUseMilitaryTime() const
{
    auto render = GetRenderPickerBase();
    auto useMilitaryTime = true;
    if (render) {
        auto columns = render->GetRenderPickerColumn();
        for (const auto& column : columns) {
            if (!column) {
                continue;
            }
            if (column->GetColumnTag() == PickerBaseComponent::PICKER_AMPM_COLUMN) {
                useMilitaryTime = false;
                break;
            }
        }
    }
    return ConvertBoolToString(useMilitaryTime);
}

RefPtr<RenderPickerBase> TimePickerDialogComposedElement::GetRenderPickerBase() const
{
    auto node = GetInspectorNode(PickerBaseElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderPickerBase>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2
