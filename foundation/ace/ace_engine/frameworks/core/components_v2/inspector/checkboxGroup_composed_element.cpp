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

#include "core/components_v2/inspector/checkboxGroup_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string,
    std::function<std::string(const CheckboxGroupComposedElement&)>> CREATE_JSON_MAP {
    { "group", [](const CheckboxGroupComposedElement& inspector) { return inspector.GetGroupName(); } },
    { "selectAll", [](const CheckboxGroupComposedElement& inspector) { return inspector.GetSelectAll(); } },
    { "selectedColor", [](const CheckboxGroupComposedElement& inspector) { return inspector.GetSelectedColor(); } }
};

} // namespace

void CheckboxGroupComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("group: ").append(GetGroupName()));
    DumpLog::GetInstance().AddDesc(std::string("selectAll: ").append(GetSelectAll()));
    DumpLog::GetInstance().AddDesc(std::string("selectedColor: ").append(GetSelectedColor()));
}

std::unique_ptr<JsonValue> CheckboxGroupComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string CheckboxGroupComposedElement::GetGroupName() const
{
    auto renderCheckbox = GetRenderCheckbox();
    if (renderCheckbox) {
        return renderCheckbox->GetGroupName().c_str();
    }
    return "";
}

std::string CheckboxGroupComposedElement::GetSelectAll() const
{
    auto renderCheckbox = GetRenderCheckbox();
    auto selectAll = renderCheckbox ? renderCheckbox->GetCheckBoxValue() : false;
    return ConvertBoolToString(selectAll);
}

std::string CheckboxGroupComposedElement::GetSelectedColor() const
{
    auto renderCheckbox = GetRenderCheckbox();
    if (renderCheckbox) {
        return renderCheckbox->GetActiveColor().ColorToString();
    }
    return "";
}

RefPtr<RenderCheckbox> CheckboxGroupComposedElement::GetRenderCheckbox() const
{
    auto node = GetInspectorNode(CheckableElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderCheckbox>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2