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

#include "core/components_v2/inspector/checkbox_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const CheckboxComposedElement&)>> CREATE_JSON_MAP {
    { "name", [](const CheckboxComposedElement& inspector) { return inspector.GetCheckBoxName(); } },
    { "group", [](const CheckboxComposedElement& inspector) { return inspector.GetCheckBoxGroup(); } },
    { "selectedColor", [](const CheckboxComposedElement& inspector) { return inspector.GetSelectedColor(); } },
    { "select", [](const CheckboxComposedElement& inspector) { return inspector.GetSelect(); } }
};

} // namespace

void CheckboxComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("name: ").append(GetCheckBoxName()));
    DumpLog::GetInstance().AddDesc(std::string("group: ").append(GetCheckBoxGroup()));
    DumpLog::GetInstance().AddDesc(std::string("selectedColor: ").append(GetSelectedColor()));
    DumpLog::GetInstance().AddDesc(std::string("select: ").append(GetSelect()));
}

std::unique_ptr<JsonValue> CheckboxComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string CheckboxComposedElement::GetCheckBoxName() const
{
    auto renderCheckbox = GetRenderCheckbox();
    if (renderCheckbox) {
        return renderCheckbox->GetCheckboxName().c_str();
    }
    return "";
}

std::string CheckboxComposedElement::GetCheckBoxGroup() const
{
    auto renderCheckbox = GetRenderCheckbox();
    if (renderCheckbox) {
        return renderCheckbox->GetBelongGroup().c_str();
    }
    return "";
}

std::string CheckboxComposedElement::GetSelect() const
{
    auto renderCheckbox = GetRenderCheckbox();
    if (renderCheckbox) {
        return ConvertBoolToString(renderCheckbox->GetCheckBoxValue());
    }
    return "false";
}

std::string CheckboxComposedElement::GetSelectedColor() const
{
    auto renderCheckbox = GetRenderCheckbox();
    if (renderCheckbox) {
        return renderCheckbox->GetActiveColor().ColorToString();
    }
    return "";
}

RefPtr<RenderCheckbox> CheckboxComposedElement::GetRenderCheckbox() const
{
    auto node = GetInspectorNode(CheckableElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderCheckbox>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2
