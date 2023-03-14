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

#include "core/components_v2/inspector/picker_text_dialog_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/picker/picker_base_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string,
    std::function<std::string(const PickerTextDialogComposedElement&)>> CREATE_JSON_MAP {
    { "show", [](const PickerTextDialogComposedElement& inspector) { return inspector.GetShow(); } },
};
}

void PickerTextDialogComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("show: ").append(GetShow()));
}

std::unique_ptr<JsonValue> PickerTextDialogComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string PickerTextDialogComposedElement::GetShow() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto jsonShow = JsonUtil::Create(false);
    jsonValue->Put("range", GetRange());
    jsonValue->Put("selected", GetSelected().c_str());
    jsonValue->Put("defaultPickerItemHeight", GetDefaultPickerItemHeight().c_str());
    jsonShow->Put("show", jsonValue);
    return jsonShow->ToString();
}

std::string PickerTextDialogComposedElement::GetDefaultPickerItemHeight() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        return render->GetColumnHeight().ToString();
    }
    return "";
}

std::string PickerTextDialogComposedElement::GetSelected() const
{
    auto render = GetRenderPickerBase();
    if (render) {
        return std::to_string(render->GetSelected());
    }
    return "0";
}

std::unique_ptr<JsonValue> PickerTextDialogComposedElement::GetRange() const
{
    auto render = GetRenderPickerBase();
    auto jsonRange = JsonUtil::CreateArray(false);
    if (render) {
        auto range = render->GetRange();
        for (size_t i = 0; i < range.size(); i++) {
            auto index = std::to_string(i);
            jsonRange->Put(index.c_str(), range[i].c_str());
        }
        return jsonRange;
    }
    return jsonRange;
}

RefPtr<RenderPickerBase> PickerTextDialogComposedElement::GetRenderPickerBase() const
{
    auto node = GetInspectorNode(PickerBaseElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderPickerBase>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2
