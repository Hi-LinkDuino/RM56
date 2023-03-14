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

#include "core/components_v2/inspector/search_composed_element.h"

#include <unordered_map>

#include "core/components/search/search_element.h"
#include "core/components_v2/inspector/utils.h"

#include "base/log/dump_log.h"

namespace OHOS::Ace::V2 {

const std::unordered_map<std::string, std::function<std::string(const SearchComposedElement&)>> CREATE_JSON_MAP {
    { "icon", [](const SearchComposedElement& inspector) { return inspector.GetIcon(); } },
    { "searchButton", [](const SearchComposedElement& inspector) { return inspector.GetSearchButton(); } }
};

void SearchComposedElement::Dump(void)
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("search_composed_element"));
    DumpLog::GetInstance().AddDesc(
        std::string("icon: ").append(GetIcon()));
    DumpLog::GetInstance().AddDesc(
        std::string("searchButton: ").append(GetSearchButton()));
}

std::unique_ptr<OHOS::Ace::JsonValue> SearchComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string SearchComposedElement::GetIcon(void) const
{
    auto renderSearch = GetRenderSearch();
    std::string icon = renderSearch ? renderSearch->GetSearchComponent()->GetCloseIconSrc() : "";
    return icon;
}

std::string SearchComposedElement::GetSearchButton(void) const
{
    auto renderSearch = GetRenderSearch();
    std::string searchButton = renderSearch ? renderSearch->GetSearchComponent()->GetSearchText() : "";
    return searchButton;
}

OHOS::Ace::RefPtr<OHOS::Ace::RenderSearch> SearchComposedElement::GetRenderSearch() const
{
    auto node = GetInspectorNode(SearchElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderSearch>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2
