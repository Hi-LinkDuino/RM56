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

#include "core/components_v2/inspector/text_picker_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/picker/picker_base_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string, std::function<std::string(const TextPickerComposedElement&)>> CREATE_JSON_MAP {
    { "defaultPickerItemHeight", [](const TextPickerComposedElement& inspector)
        { return inspector.GetDefaultPickerItemHeight(); } },
    { "selected", [](const TextPickerComposedElement& inspector) { return inspector.GetSelected(); } },
    { "range", [](const TextPickerComposedElement& inspector) { return inspector.GetRange(); } }
};
}

void TextPickerComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("defaultPickerItemHeight: ").append(GetDefaultPickerItemHeight()));
    DumpLog::GetInstance().AddDesc(std::string("selected: ").append(GetSelected()));
    DumpLog::GetInstance().AddDesc(std::string("range: ").append(GetRange()));
}

std::unique_ptr<JsonValue> TextPickerComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string TextPickerComposedElement::GetDefaultPickerItemHeight() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        return render->GetColumnHeight().ToString();
    }
    return "";
}

std::string TextPickerComposedElement::GetSelected() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        return std::to_string(render->GetSelected());
    }
    return "0";
}

std::string TextPickerComposedElement::GetRange() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        auto range = render->GetRange();
        std::string result = "[";
        for (const auto& item : range) {
            result += "\"";
            result += item;
            result += "\"";
            result += ",";
        }
        result = result.substr(0, result.size() - 1);
        result += "]";
        return result;
    }
    return "";
}

RefPtr<RenderPickerBase> TextPickerComposedElement::GetRenderPickerBase() const
{
    auto node = GetInspectorNode(PickerBaseElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderPickerBase>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2
