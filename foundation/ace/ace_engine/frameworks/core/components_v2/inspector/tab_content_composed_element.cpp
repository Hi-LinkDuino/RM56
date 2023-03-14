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

#include "core/components_v2/inspector/tab_content_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const TabContentComposedElement&)>> CREATE_JSON_MAP {
    { "tabBar", [](const TabContentComposedElement& inspector) { return inspector.GetTabBar(); } }
};

} // namespace

void TabContentComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("icon: ").append(GetIcon()));
    DumpLog::GetInstance().AddDesc(std::string("text: ").append(GetText()));
}

std::unique_ptr<JsonValue> TabContentComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string TabContentComposedElement::GetIcon() const
{
    auto element = GetTabContentItemElement();
    return element ? element->GetIcon() : "";
}

std::string TabContentComposedElement::GetText() const
{
    auto element = GetTabContentItemElement();
    return element ? element->GetText() : "";
}

std::string TabContentComposedElement::GetTabBar() const
{
    auto element = GetTabContentItemElement();
    if (!element) {
        LOGD("GetTabContentItemElement is null");
        return "";
    }
    if (!element->GetIcon().empty()) {
        auto jsonValue = JsonUtil::Create(true);
        jsonValue->Put("icon", element->GetIcon().c_str());
        jsonValue->Put("text", element->GetText().c_str());
        return jsonValue->ToString();
    } else {
        return element->GetText();
    }
}

RefPtr<TabContentItemElement> TabContentComposedElement::GetTabContentItemElement() const
{
    auto child = children_.empty() ? nullptr : children_.front();
    while (child) {
        if (AceType::TypeId(child) == TabContentItemElement::TypeId()) {
            return AceType::DynamicCast<TabContentItemElement>(child);
        }
        child = child->GetChildren().empty() ? nullptr : child->GetChildren().front();
    }
    return nullptr;
}

void TabContentComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto tabContentElement = GetContentElement<TabContentItemElement>(TabContentItemElement::TypeId());
    if (!tabContentElement) {
        LOGE("get GetTabContentElement failed");
        return;
    }
    tabContentElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    tabContentElement->MarkDirty();
}

void TabContentComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto tabContentElement = GetContentElement<TabContentItemElement>(TabContentItemElement::TypeId());
    if (!tabContentElement) {
        LOGE("get GetTabContentElement failed");
        return;
    }
    auto child = tabContentElement->GetChildBySlot(slot);
    tabContentElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    tabContentElement->MarkDirty();
}

void TabContentComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto tabContentElement = GetContentElement<TabContentItemElement>(TabContentItemElement::TypeId());
    if (!tabContentElement) {
        LOGE("get GetTabContentElement failed");
        return;
    }
    auto child = tabContentElement->GetChildBySlot(slot);
    tabContentElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    tabContentElement->MarkDirty();
}

} // namespace OHOS::Ace::V2