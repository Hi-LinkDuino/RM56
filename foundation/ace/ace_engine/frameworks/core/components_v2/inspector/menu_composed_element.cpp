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

#include "core/components_v2/inspector/menu_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/select_popup/render_select_popup.h"
#include "core/components/select_popup/select_popup_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string,
    std::function<std::unique_ptr<JsonValue>(const MenuComposedElement&)>> CREATE_JSON_MAP {
    { "bindMenu", [](const MenuComposedElement& inspector) { return inspector.GetValues(); } }
};

} // namespace

void MenuComposedElement::Dump()
{
    InspectorComposedElement::Dump();
}

std::unique_ptr<JsonValue> MenuComposedElement::ToJsonObject() const
{
    auto resultJson = JsonUtil::Create(true);
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

std::unique_ptr<JsonValue> MenuComposedElement::GetValues() const
{
    auto jsonDashArray = JsonUtil::CreateArray(false);
    auto menuElement = GetContentElement<MenuElement>(MenuElement::TypeId());
    if (menuElement) {
        auto options = menuElement->GetSelectOptions();
        for (size_t i = 0; i < options.size(); ++i) {
            auto index = std::to_string(i);
            std::string value = options[i]->GetValue();
            auto jsonValue = JsonUtil::Create(true);
            jsonValue->Put("value", value.c_str());
            jsonDashArray->Put(index.c_str(), jsonValue);
        }
        return jsonDashArray;
    }
    return jsonDashArray;
}

} // namespace OHOS::Ace::V2
