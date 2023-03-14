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

#include "core/components_v2/inspector/toggle_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const ToggleComposedElement&)>> CREATE_JSON_MAP {
    { "isOn", [](const ToggleComposedElement& inspector) { return inspector.GetChecked(); } },
    { "selectedColor", [](const ToggleComposedElement& inspector) { return inspector.GetSelectedColor(); } },
    { "type", [](const ToggleComposedElement& inspector) { return inspector.GetToggleType(); } }
};

} // namespace

void ToggleComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("isOn: ").append(GetChecked()));
    DumpLog::GetInstance().AddDesc(std::string("selectedColor: ").append(GetSelectedColor()));
    DumpLog::GetInstance().AddDesc(std::string("type: ").append(GetToggleType()));
}

std::unique_ptr<JsonValue> ToggleComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string ToggleComposedElement::GetChecked() const
{
    auto renderToggle = GetRenderToggle();
    auto checked = renderToggle ? renderToggle->GetToggleComponent()->GetCheckedState() : false;
    return ConvertBoolToString(checked);
}

std::string ToggleComposedElement::GetToggleType() const
{
    return std::string("ToggleType.Button");
}

std::string ToggleComposedElement::GetSelectedColor() const
{
    auto renderToggle = GetRenderToggle();
    if (renderToggle) {
        return renderToggle->GetToggleComponent()->GetCheckedColor().ColorToString();
    }
    return "";
}

RefPtr<RenderToggle> ToggleComposedElement::GetRenderToggle() const
{
    auto node = GetInspectorNode(ToggleElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderToggle>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2
